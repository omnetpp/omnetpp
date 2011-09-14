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
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.index.IIndex;
import org.eclipse.cdt.core.index.IIndexFile;
import org.eclipse.cdt.core.index.IIndexFileLocation;
import org.eclipse.cdt.core.index.IIndexInclude;
import org.eclipse.cdt.core.index.IIndexManager;
import org.eclipse.cdt.core.index.IndexLocationFactory;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.model.ICElement;
import org.eclipse.cdt.core.model.ICProject;
import org.eclipse.cdt.core.settings.model.CProjectDescriptionEvent;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICDescriptionDelta;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescriptionListener;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.cdt.build.MakefileBuilder;
import org.omnetpp.cdt.build.MakefileTools;
import org.omnetpp.common.Debug;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * Keeps track of which cc/h files include which other files, to be used for
 * dependencies in Makefiles.
 *
 * For cc/h files, we get the list of active #includes from CDT's index. Before accessing the
 * index, we ensure that the project is indexed and the index is up to date by calling the
 * index.update() method and sleeping until the indexer becomes idle again. One inconvenience
 * of using the index is that it does not store line numbers of #includes; when we need line
 * numbers (for markers), we read the file ourselves to figure out the line number of the
 * #include in question.
 *
 * Strategy of handling msg files (they are not indexed by the CDT, so we cannot use the
 * "normal" way): parse them manually for #include lines. Also, for each .msg file we report
 * dependencies for the corresponding _m.cc file even if it doesn't currently exist.
 *
 * Implementation note: cached dependency info is stored in a single CachedData object that
 * we never modify after initial creation. Invalidation just discards the object (nulls the pointer).
 * When needed, we simply compute a new CachedData object from scratch, then assign it to the
 * cachedData pointer. Due to the use of a (conceptually) immutable CachedData object, we need
 * no locking mechanism at all (no "synchronized", etc.) Java guarantees that assigning an
 * object reference is atomic, and that's enough for us.
 *
 * @author Andras
 */
// Note: we may need to factor out a getNonTransitiveFolderDependencies() too
// when we write a "Cross-folder Dependencies View" (using DOT to render the graph?)
public class DependencyCache {
    private static Pattern INCLUDE_DIRECTIVE_PATTERN = Pattern.compile("(?m)^[ \t]*#\\s*include[ \t]+([\"<])(.*?)[\">].*$");

    // the standard C/C++ headers (we'll ignore those #include directives)
    protected static final Set<String> standardHeaders = new HashSet<String>(Arrays.asList(MakefileTools.ALL_STANDARD_HEADERS.trim().split(" +")));

    /**
     * Represents an #include in a C++ file
     */
    static class Include {
        public String filename; // the included file name
        public boolean isSysInclude; // true: <foo.h>, false: "foo.h"
        public IFile file; // the file containing the #include (needed to make Include unique so we can use it as Map<> key)
        public int line = -1; // line number of the #include line

        public Include(IFile file, int line, String filename, boolean isSysInclude) {
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
            Include other = (Include) obj;
            return this == obj || (file.equals(other.file) && line==other.line && filename.equals(other.filename) && isSysInclude == other.isSysInclude);
        }
    }

    static class ProjectDependencyData {
        IProject project;
        List<IProject> projectGroup;  // this project and its referenced projects
        Map<Include,List<IFile>> includeResolutions; // ideally it would be Map<Include,IFile>, but we want to store all candidates for ambiguous #includes
        Map<IContainer,Set<IContainer>> folderDependencies;
        Map<IContainer, Map<IFile, Set<IFile>>> perFileDependencies;
    }

    static class CachedData {
        // per-project dependency data computed from the above
        Map<IProject,ProjectDependencyData> projectDependencyDataMap = new HashMap<IProject, ProjectDependencyData>();
    }

    // all cached data are stored in CachedData which is immutable (we don't change its content once it's created),
    // so we don't have any concurrency issues and need no locks
    private CachedData cachedData = null;

    // listeners
    private IResourceChangeListener resourceChangeListener = new IResourceChangeListener() {
        public void resourceChanged(IResourceChangeEvent event) {
            DependencyCache.this.resourceChanged(event);
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
        ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceChangeListener);
        CoreModel.getDefault().addCProjectDescriptionListener(projectDescriptionListener, CProjectDescriptionEvent.APPLIED);
    }

    public void unhookListeners() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceChangeListener);
        CoreModel.getDefault().removeCProjectDescriptionListener(projectDescriptionListener);
    }

    protected void resourceChanged(IResourceChangeEvent event) {
        try {
            // invalidate the cache on resource change events
            if (cachedData != null && event.getDelta() != null) {
                event.getDelta().accept(new IResourceDeltaVisitor() {
                    public boolean visit(IResourceDelta delta) throws CoreException {
                        IResource resource = delta.getResource();
                        if (cachedData != null && resource instanceof IFile) {
                            int kind = delta.getKind();
                            int flags = delta.getFlags();
                            if (kind==IResourceDelta.ADDED || kind==IResourceDelta.REMOVED || (kind==IResourceDelta.CHANGED && (flags&IResourceDelta.CONTENT)!=0)) {
                                if (MakefileTools.isNonGeneratedCppFile(resource) || MakefileTools.isMsgFile(resource)) {
                                    cachedData = null;
                                    return false;
                                }
                            }
                        }
                        return true;
                    }
                });
            }
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
    }

    protected void projectDescriptionChanged(CProjectDescriptionEvent e) {
        if (e.getProjectDelta() == null) // apparently, if delta==null, then nothing changed
            return;

        Debug.println("DependencyCache: project description changed, discarding dependency info");
        //dumpProjectDelta(e.getProjectDelta(), "");

        // clear cache
        cachedData = null;

        // invalidate index (because maybe some feature was turned on/off, and consequently #ifdefs have changed)
        try {
            IProject project = e.getProject();
            IProject[] affectedProjects = ProjectUtils.getAllReferencingProjects(project, true);
            for (IProject p : affectedProjects) {
                ICProject cp = CoreModel.getDefault().getCModel().getCProject(p.getName());
                CCorePlugin.getIndexManager().reindex(cp);
            }
        }
        catch (Exception ex) {
            Activator.logError("Could not initiate reindexing of affected projects on CDT project configuration change", ex);
        }
    }

    /**
     * Discards cached #include information for this project. Useful for incremental
     * builder invocations with type==CLEAN_BUILD.
     */
    public void clean(IProject project) {
        cachedData = null;
    }

    /**
     * For each folder, it determines which other folders it depends on (i.e. includes files from).
     *
     * Note: may be a long-running operation, so it needs to invoked from a background job
     * where UI responsiveness is an issue.
     */
    public Map<IContainer,Set<IContainer>> getFolderDependencies(IProject project, IProgressMonitor monitor) throws CoreException {
        if (cachedData == null)
            cachedData = computeCachedData(monitor);
        ProjectDependencyData projectData = cachedData.projectDependencyDataMap.get(project);
        return projectData.folderDependencies;
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
    public Map<IContainer,Map<IFile,Set<IFile>>> getPerFileDependencies(IProject project, IProgressMonitor monitor) throws CoreException {
        if (cachedData == null)
            cachedData = computeCachedData(monitor);
        ProjectDependencyData projectData = cachedData.projectDependencyDataMap.get(project);
        return projectData.perFileDependencies;
    }

    /**
     * Return the given project and all projects referenced from it (transitively).
     * Note: may take long: needs to invoked from a background job where UI responsiveness is an issue.
     */
    public IProject[] getProjectGroup(IProject project, IProgressMonitor monitor) throws CoreException {
        if (cachedData == null)
            cachedData = computeCachedData(monitor);
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
        ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer(MakefileBuilder.MARKER_ID);

        IProject[] projects = ProjectUtils.getOmnetppProjects();

        // Register all source files (including excluded ones!) in the marker synchronizer;
        // we need to register all, because the "excluded" state of a folder may change
        // over time, and we don't want to leave stale problem markers around.
        // Also, ignore referenced projects, as they are processed on their own right
        for (IProject project : projects)
            registerAllFilesInMarkerSynchronizer(project, markerSync);

        // collect list of #include directives per file in all open projects; cover .msg files but NOT _m.cc/h files
        Map<IFile,List<Include>> fileIncludes = new HashMap<IFile, List<Include>>();
        for (IProject project : projects)
            fileIncludes.putAll(collectIncludes(project, markerSync, monitor));

        // resolve includes and compute dependencies
        CachedData data = new CachedData();
        for (IProject project : projects)
            data.projectDependencyDataMap.put(project, computeDependencies(project, fileIncludes, markerSync));

        markerSync.runAsWorkspaceJob();

        return data;
    }

    protected void registerAllFilesInMarkerSynchronizer(IProject project, final ProblemMarkerSynchronizer markerSync) {
        try {
            project.accept(new IResourceVisitor() {
                public boolean visit(IResource resource) throws CoreException {
                    if (MakefileTools.isCppFile(resource) || MakefileTools.isMsgFile(resource))
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

    protected Map<IFile, List<Include>> collectIncludes(IProject project, final ProblemMarkerSynchronizer markerSync, IProgressMonitor monitor) throws CoreException {

        // ensure index is up to date
        Debug.println("DependencyCache: updating index for project " + project.getName());
        ICProject cproject = CoreModel.getDefault().getCModel().getCProject(project.getName());
        IIndexManager indexManager = CCorePlugin.getIndexManager();
        // Note: IIndexManager.FORCE_INDEX_INCLUSION is needed, otherwise if there is a Foo.h that
        // only includes a Foo_m.h which doesn't exist yet (it will be generated during the build),
        // Foo.h will be left out from the index for some reason and we won't know about its includes.
        // See bug #410.
        indexManager.update(new ICElement[] {cproject}, IIndexManager.UPDATE_CHECK_TIMESTAMPS | IIndexManager.FORCE_INDEX_INCLUSION);
        while (!indexManager.isIndexerIdle()) {
            Debug.println("DependencyCache: waiting for indexer to finish");
            try { Thread.sleep(300); } catch (InterruptedException e) { }
            if (monitor != null && monitor.isCanceled())
                throw new CoreException(Status.CANCEL_STATUS);
        }
        Debug.println("DependencyCache: indexer done");

        // collect #includes using the index, or (for msg files) by directly parsing the file
        Debug.println("DependencyCache: collecting #includes in project " + project.getName());
        final IIndex index = indexManager.getIndex(cproject);
        try {
            index.acquireReadLock();

            // collect active #includes from all C++ source files; also warn for linked-in files
            ICProjectDescription prjDesc = CoreModel.getDefault().getProjectDescription(project);
            ICConfigurationDescription cfg = prjDesc==null ? null : prjDesc.getActiveConfiguration();
            final ICSourceEntry[] sourceEntries = cfg==null ? new ICSourceEntry[0] : cfg.getSourceEntries();
            final Map<IFile, List<Include>> result = new HashMap<IFile, List<Include>>();
            project.accept(new IResourceVisitor() {
                // variables for caching CDTUtils.isExcluded(), which is expensive
                IContainer lastFolder = null;
                boolean lastFolderIsExcluded = false;

                public boolean visit(IResource resource) throws CoreException {
                    // warn for linked resources
                    if (resource.isLinked())
                        addMarker(markerSync, resource, IMarker.SEVERITY_ERROR, "Linked resources are not supported by Makefiles");
                    if (resource instanceof IFile) {
                        IFile file = (IFile) resource;
                        IContainer folder = file.getParent();
                        if (folder != lastFolder) {
                            lastFolder = folder;
                            lastFolderIsExcluded = CDTUtils.isExcluded(folder, sourceEntries);
                        }
                        if (!lastFolderIsExcluded) {
                            if (MakefileTools.isNonGeneratedCppFile(file) || MakefileTools.isMsgFile(file)) {
                                result.put(file, collectFileIncludes(file, index));
                            }
                        }
                    }
                    return MakefileTools.isGoodFolder(resource);
                }
            });
            return result;
        }
        catch (InterruptedException e) {
            throw Activator.wrapIntoCoreException(e);
        }
        finally {
            index.releaseReadLock();
        }
    }

    /**
     * Collect #includes from a C++ source file or a msg file
     */
    protected static List<Include> collectFileIncludes(IFile file, IIndex lockedIndex) throws CoreException {
        List<Include> result = new ArrayList<Include>();

        if (file.getFileExtension().equals("h") || file.getFileExtension().equals("cc")) {
            // get list of includes from the CDT index
            IIndexFileLocation fileLocation = IndexLocationFactory.getWorkspaceIFL(file);
            IIndexFile[] indexFiles = lockedIndex.getFiles(fileLocation);
            if (indexFiles.length == 0)
                Activator.log(IMarker.SEVERITY_ERROR, "OMNeT++ DependencyCache: " + file.getFullPath() + " is not found in index! This should not happen, please report at http://bugs.omnetpp.org!");
            else {
                IIndexFile indexFile = indexFiles[0];
                IIndexInclude[] includes = lockedIndex.findIncludes(indexFile);

                //System.out.println("* " + file + ": " + includes.length + " includes");
                for (IIndexInclude include : includes) {
                    if (include.isActive()) {
                        //System.out.println("   - " + include.getName() + (include.isSystemInclude() ? ">" : "\""));
                        result.add(new Include(file, -1, include.getFullName(), include.isSystemInclude()));
                    }
                }
            }
        }
        else if (file.getFileExtension().equals("msg")) {
            try {
                // CDT doesn't know about .msg files -- parse the file manually
                String source = FileUtils.readTextFile(file.getContents(), file.getCharset()) + "\n";
                Matcher matcher = INCLUDE_DIRECTIVE_PATTERN.matcher(source);
                while (matcher.find()) {
                    boolean isSysInclude = matcher.group(1).equals("<");
                    String fileName = matcher.group(2);
                    int line = StringUtils.countNewLines(source.substring(0, matcher.start())) + 1;
                    result.add(new Include(file, line, fileName.trim().replace('\\','/'), isSysInclude));
                }
            }
            catch (Exception e) {
                throw Activator.wrapIntoCoreException("Error collecting #includes from " + file.getFullPath(), e);
            }
        }
        return result;
    }

    protected ProjectDependencyData computeDependencies(IProject project, Map<IFile, List<Include>> fileIncludes, ProblemMarkerSynchronizer markerSync) {
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
        Map<IFile,List<Include>> cppSourceFiles = collectCppSourceFilesInProjectGroup(data.projectGroup, fileIncludes, markerSync);

        // resolve includes
        data.includeResolutions = resolveIncludes(cppSourceFiles, markerSync);

        // calculate per-file dependencies
        data.perFileDependencies = calculatePerFileDependencies(data.includeResolutions, cppSourceFiles, markerSync);

        // calculate folder dependencies
        data.folderDependencies = calculateFolderDependencies(data.includeResolutions, cppSourceFiles, markerSync);

        return data;
    }

    protected Map<IFile,List<Include>> collectCppSourceFilesInProjectGroup(List<IProject> projectGroup, Map<IFile, List<Include>> fileIncludes, ProblemMarkerSynchronizer markerSync) {
        // collect list of .h and .cc files in this project group
        // (meanwhile resolve msg files to _m.cc and _m.h)
        Map<IFile,List<Include>> cppSourceFiles = new HashMap<IFile,List<Include>>();
        for (IFile file : fileIncludes.keySet()) {
            if (projectGroup.contains(file.getProject())) {
                if (file.getFileExtension().equals("msg")) {
                    // from a msg file, the build process will generate:
                    // - an _m.h file gets generated with all the #include from the msg file
                    // - an _m.cc file which includes the _m.h file
                    IFile mhFile = file.getParent().getFile(new Path(file.getName().replaceFirst("\\.msg$", "_m.h")));
                    IFile mccFile = file.getParent().getFile(new Path(file.getName().replaceFirst("\\.msg$", "_m.cc")));
                    cppSourceFiles.put(mhFile, fileIncludes.get(file));
                    List<Include> mccIncludes = new ArrayList<Include>();
                    mccIncludes.add(new Include(mccFile, -1, mhFile.getName(), false));
                    cppSourceFiles.put(mccFile, mccIncludes);
                }
                else {
                    cppSourceFiles.put(file, fileIncludes.get(file));
                }
            }
        }
        return cppSourceFiles;
    }

    protected Map<Include,List<IFile>> resolveIncludes(Map<IFile,List<Include>> cppSourceFiles, ProblemMarkerSynchronizer markerSync) {
        Map<Include,List<IFile>> includeResolutions = new HashMap<DependencyCache.Include, List<IFile>>();

        // build a hash table of files in this project group, for easy lookup by name
        Map<String,List<IFile>> filesByName = new HashMap<String, List<IFile>>();
        for (IFile file : cppSourceFiles.keySet()) {
            String name = file.getName();
            if (!filesByName.containsKey(name))
                filesByName.put(name, new ArrayList<IFile>());
            filesByName.get(name).add(file);
        }

        // resolve includes in each file
        includeResolutions = new HashMap<Include, List<IFile>>();
        for (IFile file : cppSourceFiles.keySet()) {
            IContainer container = file.getParent();
            for (Include include : cppSourceFiles.get(file)) {
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
                        // included file not found; skip warning if it's a system include (see comment above)
                        if (!include.isSysInclude)
                            addMarker(markerSync, include, IMarker.SEVERITY_WARNING, "Makefile autodeps: cannot find included file: " + include.toString());
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

    protected Map<IContainer,Map<IFile,Set<IFile>>> calculatePerFileDependencies(Map<Include, List<IFile>> includeResolutions, Map<IFile, List<Include>> cppSourceFiles, ProblemMarkerSynchronizer markerSync) {
        // for each file, collect the list of files it includes
        Map<IFile,Set<IFile>> includedFilesMap = new HashMap<IFile, Set<IFile>>();
        for (IFile file : cppSourceFiles.keySet()) {
            Set<IFile> includedFiles = new HashSet<IFile>();
            for (Include include : cppSourceFiles.get(file))
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

    protected Map<IContainer,Set<IContainer>> calculateFolderDependencies(Map<Include, List<IFile>> includeResolutions, Map<IFile, List<Include>> cppSourceFiles, ProblemMarkerSynchronizer markerSync) {
        // process each file, and gradually expand dependencies list
        Map<IContainer,Set<IContainer>> result = new HashMap<IContainer,Set<IContainer>>();

        for (IFile file : cppSourceFiles.keySet()) {
            IContainer container = file.getParent();
            if (!result.containsKey(container))
                result.put(container, new HashSet<IContainer>());
            Set<IContainer> currentDeps = result.get(container);

            for (Include include : cppSourceFiles.get(file)) {
                if (includeResolutions.containsKey(include)) {
                    for (IFile includedFile : includeResolutions.get(include)) {
                        IContainer dependency = includedFile.getParent();
                        int numSubdirs = StringUtils.countMatches(include.filename, "/");
                        for (int i=0; i<numSubdirs && !(dependency instanceof IWorkspaceRoot); i++)
                            dependency = dependency.getParent();
                        if (dependency instanceof IWorkspaceRoot)
                            addMarker(markerSync, include, IMarker.SEVERITY_WARNING, "Makefile generation: cannot represent included directory in the workspace: it is higher than project root: " + include.toString());
                        Assert.isTrue(dependency.getLocation().toString().equals(StringUtils.removeEnd(includedFile.getLocation().toString(), "/"+include.filename)));

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

    protected void addMarker(ProblemMarkerSynchronizer markerSynchronizer, Include include, int severity, String message) {
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

    public static void dumpFolderDependencies(Map<IContainer, Set<IContainer>> deps) {
        Debug.println("Folder dependencies:");
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
