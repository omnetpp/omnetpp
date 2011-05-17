/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.cache;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.CProjectDescriptionEvent;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
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
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.cdt.build.MakefileBuilder;
import org.omnetpp.cdt.build.MakefileTools;
import org.omnetpp.common.Debug;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * Keeps track of which cc/h files include which other files, to be used for
 * dependency generation in Makefiles.
 * 
 * Conditionals (#ifdef) are ignored altogether; this vastly simplifies things
 * (compiler-defined and externally defined macros don't need to be known; a
 * header file's include list doesn't depend on which project/file it is
 * included from, etc), allows platform-independent dependencies to be
 * generated, and in general has much more advantages than drawbacks.
 * 
 * Strategy of handling msg files: pretend that _m.cc/h files exist, then
 * extract dependencies from them as from normal .cc/h files.
 * 
 * Implementation note: We minimize file read operations (we only re-read file
 * contents when the file has changed on the disk, as indicated by its
 * timestamp), but to keep the code simple, we aggressively discard cached
 * dependency info whenever something changes in the project. Dependency info is
 * recomputed on demand.
 * 
 * @author Andras
 */
// Note: we may need to factor out a getNonTransitiveFolderDependencies() too
// when we write a "Cross-folder Dependencies View" (using DOT to render the graph?)
public class DependencyCache {
	private static boolean debug = false;
	private static final int DELAY_MILLIS = 1000; // delay for updating markers
	
    // the standard C/C++ headers (we'll ignore those #include directives)
    protected static final Set<String> standardHeaders = new HashSet<String>(Arrays.asList(MakefileTools.ALL_STANDARD_HEADERS.trim().split(" +")));

    /**
     * Represents an #include in a C++ file
     */
    static class Include {
        public String filename; // the included file name
        public boolean isSysInclude; // true: <foo.h>, false: "foo.h"
        public IFile file; // the file in which the #include occurs (needed to make Include unique so we can use it as Map<> key)
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

    static class FileIncludes {
        List<Include> includes; // list of #includes in the file
        long modificationStamp; // date/time of file when it was parsed (we only re-parse the file if its timestamp has changed)

        @Override
        public String toString() {
            return "[" + StringUtils.join(includes, " ") + "] tstamp=" + modificationStamp;
        }
    }

    // list of #include directives per file; contains .msg files but not _m.cc/h files
    // Note: this map is NOT touched when we receive resource change notifications! 
    // (We use the timestamp to decide whether a FileIncludes entry is out of date.)
    private Map<IFile,FileIncludes> fileIncludes = new HashMap<IFile, FileIncludes>();
    
    // list of projects whose files in fileIncludes are up to date
    private Set<IProject> projectsWithUpToDateFileIncludes = new HashSet<IProject>();

    static class DependencyData {
        IProject project;
        List<IProject> projectGroup;  // this project and its referenced projects
        Map<Include,List<IFile>> includeResolutions; // ideally it should be Map<Include,IFile>, but we want to store all candidates for ambiguous #includes
        Map<IContainer,Set<IContainer>> folderDependencies;
        Map<IContainer, Map<IFile, Set<IFile>>> perFileDependencies;
    }

    // cached dependencies
    private Map<IProject,DependencyData> projectDependencyData = new HashMap<IProject, DependencyData>();

    private DelayedJob delayedJob = new DelayedJob(DELAY_MILLIS) {
        public void run() {
            refreshDependencies(ProjectUtils.getOpenProjects());
        }
    };
    
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
            // on resource change events, mark the corresponding project(s) as not up to date
            if (event.getDelta() != null) {
                event.getDelta().accept(new IResourceDeltaVisitor() {
                    public boolean visit(IResourceDelta delta) throws CoreException {
                        IResource resource = delta.getResource();
                        if (resource instanceof IFile) {
                            int kind = delta.getKind();
                            if (kind==IResourceDelta.ADDED || kind==IResourceDelta.REMOVED || kind==IResourceDelta.CHANGED) {
                                if (MakefileTools.isNonGeneratedCppFile(resource) || MakefileTools.isMsgFile(resource)) {
                                    projectChanged(resource.getProject());
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
        projectChanged(e.getProject());
    }
    
    /**
     * A file or something in the project has changed. We need to invalidate
     * relevant parts of the cache.
     */
    synchronized protected void projectChanged(IProject project) {
        projectsWithUpToDateFileIncludes.remove(project);
        for (IProject p : projectDependencyData.keySet().toArray(new IProject[]{}))
            if (projectDependencyData.get(p).projectGroup.contains(project))
                projectDependencyData.remove(p);
        delayedJob.restartTimer();
    }

    /**
     * Discards cached #include information for this project. Useful for incremental
     * builder invocations with type==CLEAN_BUILD.
     */
    synchronized public void clean(IProject project) {
        for (IFile f : fileIncludes.keySet().toArray(new IFile[]{}))
            if (f.getProject().equals(project))
                fileIncludes.remove(f);
        projectChanged(project);
    }

    /**
     * Computes dependency information for the given projects, and updates 
     * corresponding markers as a side effect.
     */
    synchronized public void refreshDependencies(IProject[] projects) {
        Debug.println("DependencyCache: running delayed refreshDependencies() job");
        for (IProject project : projects) 
            if (CoreModel.getDefault().getProjectDescription(project) != null)  // i.e. is a CDT project
                getProjectDependencyData(project); // refresh dependency data and update markers
    }
    
    /**
     * For each folder, it determines which other folders it depends on (i.e. includes files from).
     *
     * Note: may be a long-running operation, so it needs to invoked from a background job
     * where UI responsiveness is an issue.
     */
    synchronized public Map<IContainer,Set<IContainer>> getFolderDependencies(IProject project) {
        DependencyData projectData = getProjectDependencyData(project);
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
    synchronized public Map<IContainer,Map<IFile,Set<IFile>>> getPerFileDependencies(IProject project) {
        DependencyData projectData = getProjectDependencyData(project);
        return projectData.perFileDependencies;
    }

    /**
     * Return the given project and all projects referenced from it (transitively).
     * Note: may take long: needs to invoked from a background job where UI responsiveness is an issue.
     */
    synchronized public IProject[] getProjectGroup(IProject project) {
        DependencyData projectData = getProjectDependencyData(project);
        return projectData.projectGroup.toArray(new IProject[]{});
    }

    protected DependencyData getProjectDependencyData(IProject project) {
        if (!projectDependencyData.containsKey(project)) {
            // compute and store dependency data
            DependencyData data = computeDependencies(project);
            projectDependencyData.put(project, data);
        }
        return projectDependencyData.get(project);
    }

    protected DependencyData computeDependencies(IProject project) {
        Debug.println("DependencyCache: (re)computing dependency info for project " + project.getName());

        DependencyData data = new DependencyData();
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

        ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer(MakefileBuilder.MARKER_ID);

        // Register all source files (including excluded ones!) in the marker synchronizer;
        // we need to register all, because the "excluded" state of a folder may change
        // over time, and we don't want to leave stale problem markers around.
        // Also, ignore referenced projects, as they are processed on their own right
        registerAllFilesInMarkerSynchronizer(project, markerSync);

        // ensure all files in this project group have been parsed for #includes
        long begin = System.currentTimeMillis();
        for (IProject p : data.projectGroup)
            if (!projectsWithUpToDateFileIncludes.contains(p))
                scanChangedFilesForIncludes(p, markerSync);
        if (debug)
        	Debug.println("SCANNED: " + (System.currentTimeMillis() - begin) + "ms");

        // collect list of .h and .cc files in this project group (also, add _m.cc/_m.h for msg files)
        Map<IFile,List<Include>> cppSourceFiles = collectCppSourceFilesInProjectGroup(data, markerSync);

        // resolve includes
        resolveIncludes(data, cppSourceFiles, markerSync);

        // calculate per-file dependencies
        data.perFileDependencies = calculatePerFileDependencies(data, cppSourceFiles, markerSync);

        // calculate folder dependencies
        data.folderDependencies = calculateFolderDependencies(data, cppSourceFiles, markerSync);

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
            addMarker(markerSync, project, IMarker.SEVERITY_ERROR, "Error traversing directory tree: " + StringUtils.nullToEmpty(e.getMessage()), -1);
            Activator.logError(e);
        }
    }

    /**
     * Brings fileIncludes up to date: checks all files in the project, and parses those for #include
     * directives that have not yet been parsed or changed since the last check (based on modification 
     * timestamp.) Also throws out files that are now in excluded folders.
     */
    protected void scanChangedFilesForIncludes(IProject project, final ProblemMarkerSynchronizer markerSync) {
        if (debug)
        	Debug.println("scanChangedFilesForIncludes(): " + project);

        // Note: fileIncludes may contain files that are now in excluded folders; we'll throw them out
        // of fileIncludes because dependency computation methods no longer checks for exclusions.
        
        try {
            // parse all C++ source files for #include; also warn for linked-in files
            ICProjectDescription prjDesc = CoreModel.getDefault().getProjectDescription(project);
            ICConfigurationDescription cfg = prjDesc==null ? null : prjDesc.getActiveConfiguration();
            final ICSourceEntry[] sourceEntries = cfg==null ? new ICSourceEntry[0] : cfg.getSourceEntries();
            final Set<IFile> sourceFilesFound = new HashSet<IFile>();
            project.accept(new IResourceVisitor() {
                // variables for caching CDTUtils.isExcluded(), which is expensive
                IContainer lastFolder = null;
                boolean lastFolderIsExcluded = false;

                public boolean visit(IResource resource) throws CoreException {
                    // warn for linked resources
                    if (resource.isLinked())
                        addMarker(markerSync, resource, IMarker.SEVERITY_ERROR, "Linked resources are not supported by Makefiles", -1);
                    if (resource instanceof IFile) {
                        IFile file = (IFile) resource;
                        IContainer folder = file.getParent();
                        if (folder != lastFolder) {
                            lastFolder = folder;
                            lastFolderIsExcluded = CDTUtils.isExcluded(folder, sourceEntries);
                        }
                        if (!lastFolderIsExcluded) {
                            if (MakefileTools.isNonGeneratedCppFile(file) || MakefileTools.isMsgFile(file)) {
                                checkFileIncludes(file);
                                sourceFilesFound.add(file);
                            }
                        }
                    }
                    return MakefileTools.isGoodFolder(resource);
                }
            });

            // remove from fileIncludes the files that became excluded since
            for (IFile f : fileIncludes.keySet().toArray(new IFile[]{}))
                if (f.getProject().equals(project) && !sourceFilesFound.contains(f))
                    fileIncludes.remove(f);
            
            // project is OK now
            projectsWithUpToDateFileIncludes.add(project);
        }
        catch (CoreException e) {
            addMarker(markerSync, project, IMarker.SEVERITY_ERROR, "Error scanning source files for #includes: " + StringUtils.nullToEmpty(e.getMessage()), -1);
            Activator.logError(e);
        }
    }

    /**
     * Parses the file for the list of #includes, if it's not up to date already.
     */
    protected void checkFileIncludes(IFile file) throws CoreException {
        long fileTime = file.getModificationStamp(); // Note: must NOT compare this for greater/less, only for equals!
        FileIncludes fileData = fileIncludes.get(file);
        if (fileData == null || fileData.modificationStamp != fileTime) {
            if (fileData == null)
                fileData = new FileIncludes();

            // re-parse file for includes
            fileData.includes = parseIncludes(file);
            fileData.modificationStamp = fileTime;
            fileIncludes.put(file, fileData);

            if (debug)
                Debug.println("   parsed includes from " + file + ": " + fileData.toString());

            // clear cached dependencies (need to be recalculated)
            for (IProject p : projectDependencyData.keySet().toArray(new IProject[]{}))
                if (projectDependencyData.get(p).projectGroup.contains(file.getProject()))
                    projectDependencyData.remove(p);
        }
        else {
            if (debug)
                Debug.println("   up-to-date includes: " + file + ": " + fileData.toString());
        }
    }

    /**
     * Collect #includes from a C++ source file
     */
    protected static List<Include> parseIncludes(IFile file) throws CoreException {
        try {
            String contents = FileUtils.readTextFile(file.getContents(), file.getCharset()) + "\n";
            return parseIncludes(contents, file);
        }
        catch (Exception e) {
            throw Activator.wrapIntoCoreException("Error collecting #includes from " + file.getFullPath(), e);
        }
    }

    /**
     * Collect #includes from C++ source file contents
     */
    protected static List<Include> parseIncludes(String source, IFile sourceFile) {
        List<Include> result = new ArrayList<Include>();
        Matcher matcher = Pattern.compile("(?m)^[ \t]*#\\s*include[ \t]+([\"<])(.*?)[\">].*$").matcher(source);
        while (matcher.find()) {
            boolean isSysInclude = matcher.group(1).equals("<");
            String fileName = matcher.group(2);
            int line = StringUtils.countNewLines(source.substring(0, matcher.start())) + 1;
            result.add(new Include(sourceFile, line, fileName.trim().replace('\\','/'), isSysInclude));
        }
        return result;
    }

    protected Map<IFile,List<Include>> collectCppSourceFilesInProjectGroup(DependencyData data, ProblemMarkerSynchronizer markerSync) {
        // collect list of .h and .cc files in this project group
        // (meanwhile resolve msg files to _m.cc and _m.h)
        Map<IFile,List<Include>> cppSourceFiles = new HashMap<IFile,List<Include>>();
        for (IFile file : fileIncludes.keySet()) {
            if (data.projectGroup.contains(file.getProject())) {
                if (file.getFileExtension().equals("msg")) {
                    // from a msg file, the build process will generate:
                    // - an _m.h file gets generated with all the #include from the msg file
                    // - an _m.cc file which includes the _m.h file
                    IFile mhFile = file.getParent().getFile(new Path(file.getName().replaceFirst("\\.msg$", "_m.h")));
                    IFile mccFile = file.getParent().getFile(new Path(file.getName().replaceFirst("\\.msg$", "_m.cc"))); //XXX or .cpp?
                    cppSourceFiles.put(mhFile, fileIncludes.get(file).includes);
                    List<Include> mccIncludes = new ArrayList<Include>();
                    mccIncludes.add(new Include(mccFile, 1, mhFile.getName(), false));
                    cppSourceFiles.put(mccFile, mccIncludes);
                }
                else {
                    cppSourceFiles.put(file, fileIncludes.get(file).includes);
                }
            }
        }
        return cppSourceFiles;
    }

    protected void resolveIncludes(DependencyData data, Map<IFile,List<Include>> cppSourceFiles, ProblemMarkerSynchronizer markerSync) {
        // build a hash table of files in this project group, for easy lookup by name
        Map<String,List<IFile>> filesByName = new HashMap<String, List<IFile>>();
        for (IFile file : cppSourceFiles.keySet()) {
            String name = file.getName();
            if (!filesByName.containsKey(name))
                filesByName.put(name, new ArrayList<IFile>());
            filesByName.get(name).add(file);
        }

        // resolve includes in each file
        data.includeResolutions = new HashMap<Include, List<IFile>>();
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
                        addMarker(markerSync, file, IMarker.SEVERITY_WARNING, "Makefile autodeps: cannot resolve #include with '..' unless it is relative to the current dir", include.line);
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
                        data.includeResolutions.put(include, includeTargets);

                    if (count == 0) {
                        // included file not found; skip warning if it's a system include (see comment above)
                        if (!include.isSysInclude)
                            addMarker(markerSync, file, IMarker.SEVERITY_WARNING, "Makefile autodeps: cannot find included file: " + include.toString(), include.line);
                    }
                    else if (count > 1) {
                        // ambiguous include file
                        addMarker(markerSync, file, IMarker.SEVERITY_WARNING, "Makefile autodeps: ambiguous include (more than one matching file found): " + include.toString(), include.line);
                    }
                }
            }
        }
    }

    protected Map<IContainer,Map<IFile,Set<IFile>>> calculatePerFileDependencies(DependencyData data, Map<IFile, List<Include>> cppSourceFiles, ProblemMarkerSynchronizer markerSync) {
        // for each file, collect the list of files it includes
        Map<IFile,Set<IFile>> includedFilesMap = new HashMap<IFile, Set<IFile>>();
        for (IFile file : cppSourceFiles.keySet()) {
            Set<IFile> includedFiles = new HashSet<IFile>();
            for (Include include : cppSourceFiles.get(file))
                if (data.includeResolutions.containsKey(include))
                    for (IFile includedFile : data.includeResolutions.get(include))
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

    protected Map<IContainer,Set<IContainer>> calculateFolderDependencies(DependencyData data, Map<IFile, List<Include>> cppSourceFiles, ProblemMarkerSynchronizer markerSync) {
        // process each file, and gradually expand dependencies list
        Map<IContainer,Set<IContainer>> result = new HashMap<IContainer,Set<IContainer>>();

        for (IFile file : cppSourceFiles.keySet()) {
            IContainer container = file.getParent();
            if (!result.containsKey(container))
                result.put(container, new HashSet<IContainer>());
            Set<IContainer> currentDeps = result.get(container);

            for (Include include : cppSourceFiles.get(file)) {
                if (data.includeResolutions.containsKey(include)) {
                    for (IFile includedFile : data.includeResolutions.get(include)) {
                        IContainer dependency = includedFile.getParent();
                        int numSubdirs = StringUtils.countMatches(include.filename, "/");
                        for (int i=0; i<numSubdirs && !(dependency instanceof IWorkspaceRoot); i++)
                            dependency = dependency.getParent();
                        if (dependency instanceof IWorkspaceRoot)
                            addMarker(markerSync, file, IMarker.SEVERITY_WARNING, "Makefile generation: cannot represent included directory in the workspace: it is higher than project root: " + include.toString(), include.line);
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

    public void dumpPerFileDependencies(IProject project) {
    	Map<IContainer, Map<IFile, Set<IFile>>> perFileDependencies = getPerFileDependencies(project);
    	for(IContainer con : perFileDependencies.keySet()) {
    		Debug.println("folder: "+con.getFullPath());
    		for(IFile file : perFileDependencies.get(con).keySet())
    			Debug.println("  file: "+file.getName()+": "+perFileDependencies.get(con).get(file).toString());
    	}
    }

    protected void addMarker(ProblemMarkerSynchronizer markerSynchronizer, IResource file, int severity, String message, int line) {
        Map<String, Object> map = new HashMap<String, Object>();
        map.put(IMarker.SEVERITY, severity);
        if (line >= 0)
            map.put(IMarker.LINE_NUMBER, line);
        map.put(IMarker.MESSAGE, message);
        markerSynchronizer.addMarker(file, MakefileBuilder.MARKER_ID, map);
    }
}
