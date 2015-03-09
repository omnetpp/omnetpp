/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.cache;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.cdt.core.index.IIndexInclude;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.CProjectDescriptionEvent;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICDescriptionDelta;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescriptionListener;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContent;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContent.InclusionKind;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.cdt.build.MakefileBuilder;
import org.omnetpp.cdt.build.MakefileTools;
import org.omnetpp.cdt.cache.CompressedSourceFileCache.ISourceFileChangeListener;
import org.omnetpp.cdt.cache.Index.IndexFile;
import org.omnetpp.common.Debug;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * Keeps track of which cc/h files include which other files, to be used for
 * dependencies in Makefiles.
 *
 * First preprocessor instructions are extracted from cc/h and msg files, and cached in memory.
 * Then these "compressed" source files are parsed by a CDT preprocessor. The preprocessor
 * evaluates #defines and include files recursively. The includes can be queried from
 * the preprocessor after the parsing has been finished.
 *
 * One inconvenience of using this method is that the line numbers of #includes are misaligned.
 * When we need line numbers (for markers), we read the file ourselves to figure out the line
 * number of the #include in question.
 *
 * For each .msg file we report dependencies for the corresponding _m.cc file even if
 * it doesn't currently exist.
 *
 * Implementation note: cached dependency info is stored in a single CachedData object that
 * we never modify after initial creation. Invalidation just discards the object (nulls the pointer).
 * When needed, we simply compute a new CachedData object from scratch, then assign it to the
 * cachedData pointer.
 *
 * @author Andras
 */
// Note: we may need to factor out a getNonTransitiveFolderDependencies() too
// when we write a "Cross-folder Dependencies View" (using DOT to render the graph?)
@SuppressWarnings("restriction")
public class DependencyCache {
    private static Pattern INCLUDE_DIRECTIVE_PATTERN = Pattern.compile("(?m)^[ \t]*#\\s*include[ \t]+([\"<])(.*?)[\">].*$");

    // the standard C/C++ headers (we'll ignore those #include directives)
    protected static final Set<String> standardHeaders = new HashSet<String>(Arrays.asList(MakefileTools.ALL_STANDARD_HEADERS.trim().split(" +")));

    /**
     * Represents an #include in a C++ file
     */
    static class IncludeStatement {
        public String filename; // the included file name
        public boolean isSysInclude; // true: <foo.h>, false: "foo.h"
        public IFile file; // the file containing the #include (needed to make Include unique so we can use it as Map<> key)
        public int line = -1; // line number of the #include line

        public IncludeStatement(IFile file, int line, String filename, boolean isSysInclude) {
            Assert.isTrue(filename != null);
            this.isSysInclude = isSysInclude;
            this.filename = filename;
            this.file = file;
            this.line = line;
        }

        @Override
        public String toString() {
            return isSysInclude ? ("<" + filename + ">") : ("\"" + filename + "\"");
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + file.hashCode();
            result = prime * result + filename.hashCode();
            result = prime * result + (isSysInclude ? 1231 : 1237);
            result = prime * result + line;
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (obj == null || getClass() != obj.getClass())
                return false;
            IncludeStatement other = (IncludeStatement) obj;
            return this == obj || (file.equals(other.file) && line==other.line && filename.equals(other.filename) && isSysInclude == other.isSysInclude);
        }
    }

    static class ProjectDependencyData {
        IProject project;
        List<IProject> projectGroup;  // this project and its referenced projects
        Map<IncludeStatement,List<IFile>> includeResolutions; // ideally it would be Map<Include,IFile>, but we want to store all candidates for ambiguous #includes
        Map<IContainer,Set<IContainer>> crossFolderDependencies;
        Map<IContainer, Map<IFile, Set<IFile>>> perFileDependencies;
    }

    static class CachedData {
        Map<IFile, List<IncludeStatement>> fileIncludeStatements;
        // per-project dependency data computed from the above
        Map<IProject,ProjectDependencyData> projectDependencyDataMap = new HashMap<IProject, ProjectDependencyData>();
    }

    // all cached data are stored in CachedData which is immutable (we don't change its content once it's created),
    // so we don't have any concurrency issues and need no locks
    private CachedData cachedData = null;
    private boolean cachedDataInvalid = true;

    // listeners
    private ISourceFileChangeListener sourceFileChangeListener = new ISourceFileChangeListener() {
        public void sourceFileChanged(IResourceDelta delta) {
            DependencyCache.this.sourceFileChanged(delta);
        }
    };

    private ICProjectDescriptionListener projectDescriptionListener = new ICProjectDescriptionListener() {
        public void handleEvent(CProjectDescriptionEvent e) {
            projectDescriptionChanged(e);
        }
    };

    /**
     * Constructor,
     */
    public DependencyCache() {
    }

    public void hookListeners() {
        //ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceChangeListener);
        Activator.getCompressedSourceFileCache().addFileChangeListener(sourceFileChangeListener);
        CoreModel.getDefault().addCProjectDescriptionListener(projectDescriptionListener, CProjectDescriptionEvent.APPLIED);
    }

    public void unhookListeners() {
        //ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceChangeListener);
        Activator.getCompressedSourceFileCache().removeFileChangeListener(sourceFileChangeListener);
        CoreModel.getDefault().removeCProjectDescriptionListener(projectDescriptionListener);
    }

    protected void invalidateFileIncludes(IFile file) {
        cachedData.fileIncludeStatements.remove(file);
        // go through all resolved includes in all projects, and invalidate those files which include the given file
        for (ProjectDependencyData p : cachedData.projectDependencyDataMap.values())
            for (Entry<IncludeStatement, List<IFile>> e : p.includeResolutions.entrySet())
                if (e.getValue().contains(file))
                    invalidateFileIncludes(e.getKey().file);
    }

    protected void sourceFileChanged(IResourceDelta delta) {
        IResource changedFile = delta.getResource();
        Debug.format("DependencyCache: source file %s changed, discarding dependency info\n", changedFile.getLocation());
        if (cachedData != null)
            invalidateFileIncludes((IFile)changedFile);
        cachedDataInvalid = true;
    }

    protected void projectDescriptionChanged(CProjectDescriptionEvent e) {
        if (e.getProjectDelta() == null) // apparently, if delta==null, then nothing changed
            return;

        Debug.println("DependencyCache: project description changed, discarding dependency info");
        //dumpProjectDelta(e.getProjectDelta(), "");

        // clear cache
        cachedData = null;
        cachedDataInvalid = true;
    }

    /**
     * Discards cached #include information for this project. Useful for incremental
     * builder invocations with type==CLEAN_BUILD.
     */
    public synchronized void clean(IProject project) {
        cachedData = null;
        cachedDataInvalid = true;
    }

    /**
     * For each folder, it determines which other folders it depends on (i.e. includes files from).
     *
     * Note: may be a long-running operation, so it needs to invoked from a background job
     * where UI responsiveness is an issue.
     */
    public synchronized Map<IContainer,Set<IContainer>> getCrossFolderDependencies(IProject project, IProgressMonitor monitor) throws CoreException {
        if (cachedDataInvalid) {
            cachedData = computeCachedData(monitor);
            cachedDataInvalid = false;
        }
        ProjectDependencyData projectData = cachedData.projectDependencyDataMap.get(project);
        return projectData.crossFolderDependencies;
    }

    /**
     * For each file, it determines which other files it includes (directly or indirectly).
     * Message files (.msg) are represented as _m.cc and _m.h files.
     *
     * The result is grouped by folders; that is, each folder maps to the set of files
     * it contains. Grouping by folders significantly speeds up makefile generation.
     *
     * Note: may be a long-running operation, so it needs to invoked from a background job
     * where UI responsiveness is an issue.
     */
    public synchronized Map<IContainer,Map<IFile,Set<IFile>>> getPerFileDependencies(IProject project, IProgressMonitor monitor) throws CoreException {
        if (cachedDataInvalid) {
            cachedData = computeCachedData(monitor);
            cachedDataInvalid = false;
        }
        ProjectDependencyData projectData = cachedData.projectDependencyDataMap.get(project);
        return projectData.perFileDependencies;
    }

    /**
     * Return the given project and all projects referenced from it (transitively).
     * Note: may take long: needs to invoked from a background job where UI responsiveness is an issue.
     */
    public synchronized IProject[] getProjectGroup(IProject project, IProgressMonitor monitor) throws CoreException {
        if (cachedDataInvalid) {
            cachedData = computeCachedData(monitor);
            cachedDataInvalid = false;
        }
        ProjectDependencyData projectData = cachedData.projectDependencyDataMap.get(project);
        return projectData.projectGroup.toArray(new IProject[]{});
    }

    /**
     * The central function of DependencyCache, all getters delegate here to obtain all
     * dependency data related to the project. May be a long-running operation, so it is
     * supposed to be called from a background thread.
     *
     * Note: this method is reentrant: only uses local variables, and no data member of the class
     */
    protected CachedData computeCachedData(IProgressMonitor monitor) throws CoreException {
        IProject[] projects = ProjectUtils.getOmnetppProjects();
        int countOfFiles = 0;
        for (IProject project : projects)
            countOfFiles += countSourceFilesInProject(project);

        try {
            if (monitor != null)
                monitor.beginTask("Computing Dependency Info", countOfFiles + projects.length);

            // Register all source files (including excluded ones!) in the marker synchronizer;
            // we need to register all, because the "excluded" state of a folder may change
            // over time, and we don't want to leave stale problem markers around.
            // Also, ignore referenced projects, as they are processed on their own right
            ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer(MakefileBuilder.MARKER_ID);
            for (IProject project : projects)
                registerAllFilesInMarkerSynchronizer(project, markerSync);

            // collect list of #include directives per file in all open projects; cover .msg files but NOT _m.cc/h files
            if (monitor != null)
                monitor.subTask("Collecting Includes");
            Map<IFile,List<IncludeStatement>> fileIncludeStatements = new HashMap<IFile, List<IncludeStatement>>();
            for (IProject project : projects)
                fileIncludeStatements.putAll(collectIncludeStatementsInProject(project, markerSync, monitor));

            // resolve includes and compute dependencies
            if (monitor != null)
                monitor.subTask("Computing Dependencies");
            CachedData data = new CachedData();
            data.fileIncludeStatements = fileIncludeStatements;
            for (IProject project : projects) {
                data.projectDependencyDataMap.put(project, computeDependencies(project, fileIncludeStatements, markerSync));
                if (monitor != null)
                    monitor.worked(1);
            }

            markerSync.runAsWorkspaceJob();
            return data;
        }
        catch (OperationCanceledException e) {
            throw new CoreException(Status.CANCEL_STATUS);
        }
        finally {
            if (monitor != null)
                monitor.done();
        }
    }

    protected void registerAllFilesInMarkerSynchronizer(IProject project, final ProblemMarkerSynchronizer markerSync) {
        try {
            project.accept(new IResourceVisitor() {
                public boolean visit(IResource resource) throws CoreException {
                    if (MakefileTools.isCppFile(resource) || MakefileTools.isMsgFile(resource) || MakefileTools.isSmFile(resource))
                        markerSync.register(resource);
                    return true;
                }
            });
        }
        catch (CoreException e) {
            addMarker(markerSync, project, IMarker.SEVERITY_ERROR, "Error traversing directory tree: " + StringUtils.nullToEmpty(e.getMessage()));
            Activator.logError(e);
        }
    }

    protected void visitNonGeneratedSourceFiles(IProject project, final IResourceVisitor visitor) throws CoreException {
        ICProjectDescription prjDesc = CoreModel.getDefault().getProjectDescription(project);
        ICConfigurationDescription cfg = prjDesc==null ? null : prjDesc.getActiveConfiguration();
        final ICSourceEntry[] sourceEntries = cfg==null ? new ICSourceEntry[0] : cfg.getSourceEntries();
        project.accept(new IResourceVisitor() {
            // variables for caching CDTUtils.isExcluded(), which is expensive
            IContainer lastFolder = null;
            boolean lastFolderIsExcluded = false;

            public boolean visit(IResource resource) throws CoreException {
                if (resource instanceof IFile) {
                    IFile file = (IFile) resource;
                    IContainer folder = file.getParent();
                    if (folder != lastFolder) {
                        lastFolder = folder;
                        lastFolderIsExcluded = CDTUtils.isExcluded(folder, sourceEntries);
                    }
                    if (!lastFolderIsExcluded) {
                        if (MakefileTools.isNonGeneratedCppFile(file) || MakefileTools.isMsgFile(file) || MakefileTools.isSmFile(file)) {
                            visitor.visit(file);
                        }
                    }
                }
                return MakefileTools.isGoodFolder(resource);
            }
        });
    }

    protected int countSourceFilesInProject(IProject project) throws CoreException {
        final int[] count = new int[1];
        visitNonGeneratedSourceFiles(project, new IResourceVisitor() {
            public boolean visit(IResource resource) throws CoreException {
                count[0]++;
                return false;
            }
        });
        return count[0];
    }

    protected Map<IFile, List<IncludeStatement>> collectIncludeStatementsInProject(IProject project, final ProblemMarkerSynchronizer markerSync, final IProgressMonitor monitor) throws CoreException {

        // collect #includes using the index, or (for msg files) by directly parsing the file
        Debug.println("DependencyCache: collecting #includes in project " + project.getName());
        long startTime = System.currentTimeMillis();
        int startCachedFiles = Activator.getCompressedSourceFileCache().getNumberOfFiles();
        int startIndexedFiles = Activator.getIndex().getNumberOfFiles();
        final int[] count = new int[1];

        // collect active #includes from all C++ source files; also warn for linked-in files
        final Map<IFile, List<IncludeStatement>> result = new HashMap<IFile, List<IncludeStatement>>();
        visitNonGeneratedSourceFiles(project, new IResourceVisitor() {
            public boolean visit(IResource resource) throws CoreException {
                if (monitor != null && monitor.isCanceled())
                    throw new OperationCanceledException();
                // warn for linked resources
                if (resource.isLinked())
                    addMarker(markerSync, resource, IMarker.SEVERITY_ERROR, "Linked resources are not supported by Makefiles");
                if (resource instanceof IFile) {
                    IFile file = (IFile) resource;
                    List<IncludeStatement> includes = null;
                    if (cachedData != null)
                        includes = cachedData.fileIncludeStatements.get(file);
                    if (includes == null)
                        includes = collectFileIncludeStatements(file);
                    result.put(file, includes);
                    count[0]++;
                    if (monitor != null)
                        monitor.worked(1);
                }
                return true;
            }
        });

        long endTime = System.currentTimeMillis();
        int endCachedFiles = Activator.getCompressedSourceFileCache().getNumberOfFiles();
        int endIndexedFiles = Activator.getIndex().getNumberOfFiles();
        Debug.format("  processed %d files in %dms (cached: %d/%d, indexed: %d/%d)\n",
                            count[0], endTime - startTime,
                            endCachedFiles - startCachedFiles, endCachedFiles,
                            endIndexedFiles - startIndexedFiles, endIndexedFiles);
        //Activator.getIndex().dumpToDebug();
        return result;
    }

    /**
     * Collect #includes from a C++ source file or a msg file
     */
    protected static List<IncludeStatement> collectFileIncludeStatements(IFile file) throws CoreException {
        //Debug.println("Collect includes from " + file.getLocation());

        List<IncludeStatement> result = new ArrayList<IncludeStatement>();
        if (MakefileTools.isCppFile(file) || MakefileTools.isMsgFile(file) || MakefileTools.isSmFile(file)) {
            try {
                Index index = Activator.getIndex();
                FileContentProvider fileContentProvider = new FileContentProvider(file);
                InternalFileContent translationUnit = fileContentProvider.getContentForTranslationUnit();
                // scan if not indexed
                if (translationUnit.getKind() == InclusionKind.USE_SOURCE) {
                    CppScanner scanner = new CppScanner(fileContentProvider, index.getIndexer());
                    fileContentProvider.setMacroValueProvider(scanner);
                    scanner.scanFully();
                }
                // find includes in the indexed file
                IndexFile indexFile = index.resolve(file.getLocation());
                if (indexFile != null) {
                    for (IIndexInclude include : indexFile.getIncludes()) {
                        if (include.isActive()) {
                            //Debug.println("    includes " + (include.isResolved() ? "+" : "-") +include.getName());

                            // XXX because the scanner receives file contents that contains only the preprocessor directives,
                            //     the line numbers in the AST will be wrong. They cannot be cured by adding #line directives
                            //     to the generated file contents, because CPreprocessor ignores them. So we did not fill the line
                            //     numbers here. The line number can be computed by getLineForInclude() when needed.
                            result.add(new IncludeStatement(file, -1, include.getFullName(), include.isSystemInclude()));
                        }
                    }
                }
            }
            catch (Exception e) {
                throw Activator.wrapIntoCoreException("Error collecting #includes from " + file.getFullPath(), e);
            }
        }
        return result;
    }

    protected ProjectDependencyData computeDependencies(IProject project, Map<IFile, List<IncludeStatement>> fileIncludes, ProblemMarkerSynchronizer markerSync) {
        Debug.println("DependencyCache: computing dependency info for project " + project.getName());

        ProjectDependencyData data = new ProjectDependencyData();
        data.project = project;

        // determine project group (this project plus all referenced projects)
        data.projectGroup = new ArrayList<IProject>();
        data.projectGroup.add(project);
        try {
            data.projectGroup.addAll(Arrays.asList(ProjectUtils.getAllReferencedProjects(project)));
        }
        catch (CoreException e) {
            Activator.logError("Error getting the list of referenced projects", e);
        }

        // collect list of .h and .cc files in this project group (also, add _m.cc/_m.h for msg files)
        Map<IFile,List<IncludeStatement>> cppSourceFiles = collectCppSourceFilesInProjectGroup(data.projectGroup, fileIncludes, markerSync);

        // resolve includes
        data.includeResolutions = resolveIncludeStatements(cppSourceFiles, markerSync);

        // calculate per-file dependencies
        data.perFileDependencies = calculatePerFileDependencies(data.includeResolutions, cppSourceFiles, markerSync);

        // calculate folder dependencies
        data.crossFolderDependencies = calculateCrossFolderDependencies(data.includeResolutions, cppSourceFiles, markerSync);

        return data;
    }

    protected Map<IFile,List<IncludeStatement>> collectCppSourceFilesInProjectGroup(List<IProject> projectGroup, Map<IFile, List<IncludeStatement>> fileIncludes, ProblemMarkerSynchronizer markerSync) {
        // collect list of .h and .cc files in this project group
        // (meanwhile resolve msg files to _m.cc and _m.h)
        Map<IFile,List<IncludeStatement>> cppSourceFiles = new HashMap<IFile,List<IncludeStatement>>();
        for (IFile file : fileIncludes.keySet()) {
            if (projectGroup.contains(file.getProject())) {
                if (file.getFileExtension().equals("msg")) {
                    // from a msg file, the build process will generate:
                    // - an _m.h file gets generated with all the #include from the msg file
                    // - an _m.cc file which includes the _m.h file
                    IFile mhFile = file.getParent().getFile(new Path(file.getName().replaceFirst("\\.msg$", "_m.h")));
                    IFile mccFile = file.getParent().getFile(new Path(file.getName().replaceFirst("\\.msg$", "_m.cc")));
                    cppSourceFiles.put(mhFile, fileIncludes.get(file));
                    List<IncludeStatement> mccIncludes = new ArrayList<IncludeStatement>();
                    mccIncludes.add(new IncludeStatement(mccFile, -1, mhFile.getName(), false));
                    cppSourceFiles.put(mccFile, mccIncludes);
                }
                else if (file.getFileExtension().equals("sm")) {
                    // treat sm files similarly to msg files
                    IFile smhFile = file.getParent().getFile(new Path(file.getName().replaceFirst("\\.sm$", "_sm.h")));
                    IFile smccFile = file.getParent().getFile(new Path(file.getName().replaceFirst("\\.sm$", "_sm.cc")));
                    cppSourceFiles.put(smhFile, fileIncludes.get(file));
                    List<IncludeStatement> smccIncludes = new ArrayList<IncludeStatement>();
                    smccIncludes.add(new IncludeStatement(smccFile, -1, smhFile.getName(), false));
                    cppSourceFiles.put(smccFile, smccIncludes);
                }
                else {
                    cppSourceFiles.put(file, fileIncludes.get(file));
                }
            }
        }
        return cppSourceFiles;
    }

    protected Map<IncludeStatement,List<IFile>> resolveIncludeStatements(Map<IFile,List<IncludeStatement>> cppSourceFiles, ProblemMarkerSynchronizer markerSync) {
        Map<IncludeStatement,List<IFile>> includeResolutions = new HashMap<DependencyCache.IncludeStatement, List<IFile>>();

        // build a hash table of files in this project group, for easy lookup by name
        Map<String,List<IFile>> filesByName = new HashMap<String, List<IFile>>();
        for (IFile file : cppSourceFiles.keySet()) {
            String name = file.getName();
            if (!filesByName.containsKey(name))
                filesByName.put(name, new ArrayList<IFile>());
            filesByName.get(name).add(file);
        }

        // resolve includes in each file
        includeResolutions = new HashMap<IncludeStatement, List<IFile>>();
        for (IFile file : cppSourceFiles.keySet()) {
            IContainer container = file.getParent();
            for (IncludeStatement include : cppSourceFiles.get(file)) {
                if (include.isSysInclude && standardHeaders.contains(include.filename)) {
                    // this is a standard C/C++ header file, just ignore.
                    // Note: non-standards angle-bracket #includes will be resolved and
                    // used as dependency if found, but there's no warning if they're
                    // not found.
                }
                else if (include.filename.contains("..")) {
                    // we only recognize an include containing ".." if it's relative to the current dir
                    IPath includeFileLocation = container.getLocation().append(new Path(include.filename));
                    @SuppressWarnings("deprecation")
                    IFile[] f = ResourcesPlugin.getWorkspace().getRoot().findFilesForLocation(includeFileLocation);
                    if (f.length == 0 || !f[0].exists())
                        addMarker(markerSync, include, IMarker.SEVERITY_WARNING, "Makefile autodeps: cannot resolve #include with '..' unless it is relative to the current dir");
                    else {
                        List<IFile> includeTargets = new ArrayList<IFile>(Arrays.asList(f));
                        includeResolutions.put(include, includeTargets);
                    }
                }
                else {
                    List<IFile> includeTargets = new ArrayList<IFile>(); // will contain ideally 1 element, but 0 (unresolved #include) or >1 (ambiguous #include) are also possible

                    // determine which IFile(s) the include maps to
                    String justTheName = include.filename.replaceFirst("^.*/", ""); // strip prefix
                    List<IFile> candidates = filesByName.get(justTheName);
                    if (candidates == null) candidates = new ArrayList<IFile>();

                    int count = 0;
                    for (IFile candidate : candidates)
                        if (candidate.getLocation().toString().endsWith("/"+include.filename)) // note: we check "real" path (ie. location) not the workspace path!
                            {count++; includeTargets.add(candidate);}
                    if (count > 0)
                        includeResolutions.put(include, includeTargets);

                    if (count == 0) {
                        // included file not found: do not report, because CDT will report it anyway
                        // (and our warning could be incorrect, because we only search workspace paths but not the file system)
                    }
                    else if (count > 1) {
                        // ambiguous include file
                        addMarker(markerSync, include, IMarker.SEVERITY_WARNING, "Makefile autodeps: ambiguous include (more than one matching file found): " + include.toString());
                    }
                }
            }
        }
        return includeResolutions;
    }

    protected Map<IContainer,Map<IFile,Set<IFile>>> calculatePerFileDependencies(Map<IncludeStatement, List<IFile>> includeResolutions, Map<IFile, List<IncludeStatement>> cppSourceFiles, ProblemMarkerSynchronizer markerSync) {
        // for each file, collect the list of files it includes
        Map<IFile,Set<IFile>> includedFilesMap = new HashMap<IFile, Set<IFile>>();
        for (IFile file : cppSourceFiles.keySet()) {
            Set<IFile> includedFiles = new HashSet<IFile>();
            for (IncludeStatement include : cppSourceFiles.get(file))
                if (includeResolutions.containsKey(include))
                    for (IFile includedFile : includeResolutions.get(include))
                        includedFiles.add(includedFile);
            includedFilesMap.put(file, includedFiles);
        }

        // calculate transitive closure
        boolean again = true;
        while (again) {
            again = false;
            // if x includes y, add y's includes to x as well.
            // and if anything changed, repeat the whole thing
            for (IFile x : includedFilesMap.keySet())
                for (IFile y : includedFilesMap.get(x).toArray(new IFile[]{}))
                    if (includedFilesMap.get(x).addAll(includedFilesMap.get(y)))
                        again = true;
        }

        // group by folders
        Map<IContainer,Map<IFile,Set<IFile>>> result = new HashMap<IContainer, Map<IFile,Set<IFile>>>();
        for (IFile file : includedFilesMap.keySet()) {
            IContainer folder = file.getParent();
            if (!result.containsKey(folder))
                result.put(folder, new HashMap<IFile, Set<IFile>>());
            result.get(folder).put(file, includedFilesMap.get(file));
        }

        return result;
    }

    protected Map<IContainer,Set<IContainer>> calculateCrossFolderDependencies(Map<IncludeStatement, List<IFile>> includeResolutions, Map<IFile, List<IncludeStatement>> cppSourceFiles, ProblemMarkerSynchronizer markerSync) {
        // process each file, and gradually expand dependencies list
        Map<IContainer,Set<IContainer>> result = new HashMap<IContainer,Set<IContainer>>();

        for (IFile file : cppSourceFiles.keySet()) {
            IContainer container = file.getParent();
            if (!result.containsKey(container))
                result.put(container, new HashSet<IContainer>());
            Set<IContainer> currentDeps = result.get(container);

            for (IncludeStatement include : cppSourceFiles.get(file)) {
                if (includeResolutions.containsKey(include)) {
                    for (IFile includedFile : includeResolutions.get(include)) {
                        IContainer dependency;
                        if (include.filename.contains("..")) {
                            // includes containing ".." are always resolved relative to the parent of the file
                            dependency = container.getParent();
                        }
                        else {
                            dependency = includedFile.getParent();
                            int numSubdirs = StringUtils.countMatches(include.filename, "/");
                            for (int i=0; i<numSubdirs && !(dependency instanceof IWorkspaceRoot); i++)
                                dependency = dependency.getParent();
                            Assert.isTrue(dependency.getLocation().toString().equals(StringUtils.removeEnd(includedFile.getLocation().toString(), "/"+include.filename)));
                        }
                        if (dependency instanceof IWorkspaceRoot)
                            addMarker(markerSync, include, IMarker.SEVERITY_WARNING, "Makefile generation: cannot represent included directory in the workspace: it is higher than project root: " + include.toString());
                        // add folder to the dependent folders
                        if (dependency != container && !currentDeps.contains(dependency))
                            currentDeps.add(dependency);
                    }
                }
            }
        }

        // calculate transitive closure
        boolean again = true;
        while (again) {
            again = false;
            for (IContainer x : result.keySet())
                for (IContainer y : result.keySet())
                    if (x != y && result.get(y).contains(x))  // if y depends on x
                        if (result.get(y).addAll(result.get(x)))  // then add x's dependencies to y
                            again = true; // and if anything changed, repeat the whole thing
        }

        return result;
    }

    protected void addMarker(ProblemMarkerSynchronizer markerSynchronizer, IncludeStatement include, int severity, String message) {
        int line = include.line;
        if (line == -1) {
            // note: the CDT index doesn't store the line numbers of #includes, so we need to figure it out now
            try { line = getLineForInclude(include.file, include.filename, include.isSysInclude); } catch (Exception e) { }
        }
        Map<String, Object> map = new HashMap<String, Object>();
        map.put(IMarker.SEVERITY, severity);
        if (line != -1)
            map.put(IMarker.LINE_NUMBER, line);
        map.put(IMarker.MESSAGE, message);
        markerSynchronizer.addMarker(include.file, MakefileBuilder.MARKER_ID, map);
    }

    protected void addMarker(ProblemMarkerSynchronizer markerSynchronizer, IResource resource, int severity, String message) {
        Map<String, Object> map = new HashMap<String, Object>();
        map.put(IMarker.SEVERITY, severity);
        map.put(IMarker.MESSAGE, message);
        markerSynchronizer.addMarker(resource, MakefileBuilder.MARKER_ID, map);
    }

    protected int getLineForInclude(IFile file, String includeName, boolean isSysInclude) throws CoreException, IOException {
        // parse the file for the appropriate #include, and return its line number
        String source = FileUtils.readTextFile(file.getContents(), file.getCharset()) + "\n";
        Matcher matcher = INCLUDE_DIRECTIVE_PATTERN.matcher(source);
        while (matcher.find()) {
            boolean isMatchSysInclude = matcher.group(1).equals("<");
            String matchIncludeName = matcher.group(2);
            if (isMatchSysInclude == isSysInclude && matchIncludeName.equals(includeName)) {
                int line = StringUtils.countNewLines(source.substring(0, matcher.start())) + 1;
                return line;
            }
        }
        return -1;
    }

    public static void dumpCrossFolderDependencies(Map<IContainer, Set<IContainer>> deps) {
        Debug.println("Cross-folder dependencies:");
        for (IContainer folder : deps.keySet()) {
            Debug.print("  " + folder.getFullPath().toString() + " depends on: ");
            for (IContainer dep : deps.get(folder)) {
                Debug.print(" " + MakefileTools.makeRelativePath(dep.getFullPath(), folder.getFullPath()).toString());
            }
            Debug.println();
        }
    }

    public void dumpPerFileDependencies(IProject project, IProgressMonitor monitor) throws CoreException {
        Map<IContainer, Map<IFile, Set<IFile>>> perFileDependencies = getPerFileDependencies(project, monitor);
        for(IContainer con : perFileDependencies.keySet()) {
            Debug.println("folder: "+con.getFullPath());
            for(IFile file : perFileDependencies.get(con).keySet())
                Debug.println("  file: "+file.getName()+": "+perFileDependencies.get(con).get(file).toString());
        }
    }

    protected void dumpProjectDelta(ICDescriptionDelta d, String indent) {
        if (d == null)
            Debug.println(indent + "delta is null");
        else {
            Debug.println(indent + "delta kind=" + d.getDeltaKind() + " flags=" + d.getChangeFlags() + " setting:" + d.getSetting().getClass().getSimpleName() + " name:" + d.getSetting().getName());
            for (ICDescriptionDelta i : d.getChildren())
                dumpProjectDelta(i, "    " + indent);
        }
    }
}
