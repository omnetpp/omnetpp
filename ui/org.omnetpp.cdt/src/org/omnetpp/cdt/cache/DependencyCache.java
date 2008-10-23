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
import org.omnetpp.cdt.makefile.MakefileBuilder;
import org.omnetpp.cdt.makefile.MakefileTools;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * Keeps track of which cc/h files include which other files, to be used
 * for dependency generation in Makefiles.
 *
 * Conditionals (#ifdef) are ignored altogether; this vastly simplifies
 * things (compiler-defined and externally defined macros don't need to
 * be known; a header file's include list doesn't depend on which
 * project/file it is included from, etc), allows platform-independent
 * dependencies to be generated, and in general has much more advantages
 * than drawbacks.
 *
 * Strategy of handling msg files: pretend that _m.cc/h files exist, then
 * extract dependencies from them as from normal .cc/h files.
 *
 * @author Andras
 */
// Note: we may need to factor out a getNonTransitiveFolderDependencies() too
// when we write a "Cross-folder Dependencies View" (using DOT to render the graph?)
//XXX mindenki a SAJAT projektre tegye csak r� a markereket, a referenced projekteket hagyja ki! -- igy minden marker csak 1x lesz!
public class DependencyCache {
	private static boolean debug = false;
    // the standard C/C++ headers (we'll ignore those #include directives)
    protected static final Set<String> standardHeaders = new HashSet<String>(Arrays.asList(MakefileTools.ALL_STANDARD_HEADERS.split(" ")));

    /**
     * Represents an #include in a C++ file
     */
    static class Include {
        public String filename; // the included file name
        public boolean isSysInclude; // true: <foo.h>, false: "foo.h"
        //public IFile file; // the file in which the #include occurs -- not needed
        public int line = -1; // line number of the #include line

        public Include(int line, String filename, boolean isSysInclude) {
            Assert.isTrue(filename != null);
            this.isSysInclude = isSysInclude;
            this.filename = filename;
            this.line = line;
        }

        @Override
        public String toString() {
            return isSysInclude ? ("<" + filename + ">") : ("\"" + filename + "\"");
        }

        @Override
        public int hashCode() {
            return line*1231 + filename.hashCode()*31 + (isSysInclude ? 1231 : 1237);
        }

        @Override
        public boolean equals(Object obj) {
            if (obj == null || getClass() != obj.getClass())
                return false;
            Include other = (Include) obj;
            return this == obj || (line==other.line && filename.equals(other.filename) && isSysInclude == other.isSysInclude);
        }
    }

    static class FileIncludes {
        List<Include> includes; // list of #includes in the file
        long modificationStamp; // date/time of file when it was parsed
    }

    static class ProjectData {
        IProject project;
        List<IProject> projectGroup;  // this project and its referenced projects
        Map<IFile,List<Include>> cppSourceFiles;  // only used during calculations; contains _m.cc/h instead of msg files; filtered for projectGroup
        Map<Include,IFile> resolvedIncludes;
        Map<IContainer,Set<IContainer>> folderDependencies;
        Map<IContainer, Map<IFile, Set<IFile>>> perFileDependencies;
    }

    // per-file includes; contains .msg files but not _m.cc/h files
    private Map<IFile,FileIncludes> fileIncludes = new HashMap<IFile, FileIncludes>();

    // list of projects whose include lists are up-to-date in the cache
    private Set<IProject> fileIncludesUpToDate = new HashSet<IProject>();

    // cached dependencies
    private Map<IProject,ProjectData> projectData = new HashMap<IProject, ProjectData>();

    public DependencyCache() {
        hookListeners();
    }

    protected void hookListeners() {
        // on resource change events, mark the corresponding project(s) as not up to date
        //XXX can be optimized so that we only invalidate when really needed (better look on the change event)
        ResourcesPlugin.getWorkspace().addResourceChangeListener(new IResourceChangeListener() {
            public void resourceChanged(IResourceChangeEvent event) {
                try {
                    if (event.getDelta() != null) {
                        event.getDelta().accept(new IResourceDeltaVisitor() {
                            public boolean visit(IResourceDelta delta) throws CoreException {
                                if (delta.getResource() instanceof IProject) {
                                    projectChanged((IProject)delta.getResource());
                                    return false;
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
        });
    }

    /**
     * A file or something in the project has changed. We need to invalidate
     * relevant parts of the cache.
     */
    synchronized protected void projectChanged(IProject project) {
        fileIncludesUpToDate.remove(project);
        for (IProject p : projectData.keySet().toArray(new IProject[]{}))
            if (projectData.get(p).projectGroup.contains(project))
                projectData.remove(p);
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
     * For each folder, it determines which other folders it depends on (i.e. includes files from).
     *
     * Note: may be a long-running operation, so it needs to invoked from a background job
     * where UI responsiveness is an issue.
     */
    synchronized public Map<IContainer,Set<IContainer>> getFolderDependencies(IProject project) {
        ProjectData projectData = getOrCreateProjectData(project);
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
        ProjectData projectData = getOrCreateProjectData(project);
        return projectData.perFileDependencies;
    }

    /**
     * Return the given project and all projects referenced from it (transitively).
     * Note: may take long: needs to invoked from a background job where UI responsiveness is an issue.
     */
    synchronized public IProject[] getProjectGroup(IProject project) {
        ProjectData projectData = getOrCreateProjectData(project);
        return projectData.projectGroup.toArray(new IProject[]{});
    }

    protected ProjectData getOrCreateProjectData(IProject project) {
        if (!projectData.containsKey(project)) {
            ProjectData data = new ProjectData();
            data.project = project;

            // determine project group (this project plus all referenced projects)
            data.projectGroup = new ArrayList<IProject>();
            data.projectGroup.add(project);
            data.projectGroup.addAll(Arrays.asList(ProjectUtils.getAllReferencedProjects(project)));

            ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer(MakefileBuilder.MARKER_ID);

            // Register all source files (including excluded ones!) in the marker synchronizer;
            // we need to register all, because the "excluded" state of a folder may change
            // over time, and we don't want to leave stale problem markers around.
            // Also, ignore referenced projects, as they are processed on their own right
            //XXX does not work somehow (try on INET/Obsolete!)
            registerAllFilesInMarkerSynchronizer(project, markerSync);

            // ensure all files in this project group have been parsed for #includes
            long begin = System.currentTimeMillis();
            for (IProject p : data.projectGroup)
                if (!fileIncludesUpToDate.contains(p))
                    collectIncludes(p, markerSync);
            if (debug)
            	System.out.println("SCANNED: " + (System.currentTimeMillis() - begin) + "ms");

            // collect list of .h and .cc files in this project group (also, add _m.cc/_m.h for msg files)
            collectCppSourceFilesInProjectGroup(data, markerSync);

            // resolve includes
            resolveIncludes(data, markerSync);

            // calculate per-file dependencies
            data.perFileDependencies = calculatePerFileDependencies(data, markerSync);

            // calculate folder dependencies
            data.folderDependencies = calculateFolderDependencies(data, markerSync);

            data.cppSourceFiles = null; // no longer needed

            // store
            projectData.put(project, data);

            markerSync.runAsWorkspaceJob();
        }
        return projectData.get(project);
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

    protected void collectIncludes(IProject project, final ProblemMarkerSynchronizer markerSync) {
        if (debug)
        	System.out.println("collectIncludes(): " + project);

        try {
            // parse all C++ source files for #include; also warn for linked-in files
            final ICSourceEntry[] sourceEntries = CDTUtils.getSourceEntries(project);
            project.accept(new IResourceVisitor() {
                public boolean visit(IResource resource) throws CoreException {
                    // warn for linked resources
                    if (resource.isLinked())
                        addMarker(markerSync, resource, IMarker.SEVERITY_ERROR, "Linked resources are not supported by Makefiles", -1);
                    if (MakefileTools.isNonGeneratedCppFile(resource) || MakefileTools.isMsgFile(resource))
                        checkFileIncludes((IFile)resource);
                    return MakefileTools.isGoodFolder(resource) && !CDTUtils.isExcluded(resource, sourceEntries);
                }
            });

            // project is OK now
            fileIncludesUpToDate.add(project);
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
        if (debug)
        	System.out.println("   checkFileIncludes(): " + file);
        long fileTime = file.getModificationStamp();
        FileIncludes fileData = fileIncludes.get(file);
        if (fileData == null || fileData.modificationStamp < fileTime) {
            if (debug)
            	System.out.println("   parsing includes from: " + file);
            if (fileData == null)
                fileIncludes.put(file, (fileData = new FileIncludes()));

            // re-parse file for includes
            fileData.includes = parseIncludes(file);
            fileData.modificationStamp = fileTime;

            // clear cached dependencies (need to be recalculated)
            for (IProject p : projectData.keySet().toArray(new IProject[]{}))
                if (projectData.get(p).projectGroup.contains(file.getProject()))
                    projectData.remove(p);
        }
    }

    /**
     * Collect #includes from a C++ source file
     */
    protected static List<Include> parseIncludes(IFile file) throws CoreException {
        try {
            String contents = FileUtils.readTextFile(file.getContents()) + "\n";
            return parseIncludes(contents);
        }
        catch (IOException e) {
            throw Activator.wrapIntoCoreException("Error collecting #includes from " + file.getFullPath(), e);
        }
    }

    /**
     * Collect #includes from C++ source file contents
     */
    protected static List<Include> parseIncludes(String source) {
        List<Include> result = new ArrayList<Include>();
        Matcher matcher = Pattern.compile("(?m)^[ \t]*#\\s*include[ \t]+([\"<])(.*?)[\">].*$").matcher(source);
        while (matcher.find()) {
            boolean isSysInclude = matcher.group(1).equals("<");
            String fileName = matcher.group(2);
            int line = StringUtils.countNewLines(source.substring(0, matcher.start())) + 1;
            result.add(new Include(line, fileName.trim().replace('\\','/'), isSysInclude));
        }
        return result;
    }

    protected void collectCppSourceFilesInProjectGroup(ProjectData data, ProblemMarkerSynchronizer markerSync) {
        // collect list of .h and .cc files in this project group
        // (meanwhile resolve msg files to _m.cc and _m.h)
        data.cppSourceFiles = new HashMap<IFile,List<Include>>();
        for (IFile file : fileIncludes.keySet()) {
            if (data.projectGroup.contains(file.getProject())) {
                if (file.getFileExtension().equals("msg")) {
                    // from a msg file, the build process will generate:
                    // - an _m.h file gets generated with all the #include from the msg file
                    // - an _m.cc file which includes the _m.h file
                    IFile mhFile = file.getParent().getFile(new Path(file.getName().replaceFirst("\\.msg$", "_m.h")));
                    IFile mccFile = file.getParent().getFile(new Path(file.getName().replaceFirst("\\.msg$", "_m.cc"))); //XXX or .cpp?
                    data.cppSourceFiles.put(mhFile, fileIncludes.get(file).includes);
                    List<Include> mccIncludes = new ArrayList<Include>();
                    mccIncludes.add(new Include(1, mhFile.getName(), false));
                    data.cppSourceFiles.put(mccFile, mccIncludes);
                }
                else {
                    data.cppSourceFiles.put(file, fileIncludes.get(file).includes);
                }
            }
        }
    }

    protected void resolveIncludes(ProjectData data, ProblemMarkerSynchronizer markerSync) {
        // build a hash table of files in this project group, for easy lookup by name
        Map<String,List<IFile>> filesByName = new HashMap<String, List<IFile>>();
        for (IFile file : data.cppSourceFiles.keySet()) {
            String name = file.getName();
            if (!filesByName.containsKey(name))
                filesByName.put(name, new ArrayList<IFile>());
            filesByName.get(name).add(file);
        }

        // resolve includes in each file
        data.resolvedIncludes = new HashMap<Include, IFile>();
        for (IFile file : data.cppSourceFiles.keySet()) {
            IContainer container = file.getParent();
            for (Include include : data.cppSourceFiles.get(file)) {
                if (include.isSysInclude && standardHeaders.contains(include.filename)) {
                    // this is a standard C/C++ header file, just ignore.
                    // Note: non-standards angle-bracket #includes will be resolved and
                    // used as dependency if found, but there's no warning if they're
                    // not found.
                }
                else if (include.filename.contains("..")) {
                    // we only recognize an include containing ".." if it's relative to the current dir
                    IPath includeFileLocation = container.getLocation().append(new Path(include.filename));
                    IFile[] f = ResourcesPlugin.getWorkspace().getRoot().findFilesForLocation(includeFileLocation);
                    if (f.length == 0 || !f[0].exists())
                        addMarker(markerSync, file, IMarker.SEVERITY_WARNING, "Makefile autodeps: cannot resolve #include with '..' unless it is relative to the current dir", include.line); //XXX implement instead of warning!!!
                }
                else {
                    // determine which IFile(s) the include maps to
                    List<IFile> list = filesByName.get(include.filename.replaceFirst("^.*/", ""));
                    if (list == null) list = new ArrayList<IFile>();

                    int count = 0;
                    IFile includedFile = null;
                    for (IFile i : list)
                        if (i.getLocation().toString().endsWith("/"+include.filename)) // note: we check "real" path (ie. location) not the workspace path!
                        {count++; includedFile = i;}

                    if (count == 1) {
                        // include resolved successfully and unambiguously
                        data.resolvedIncludes.put(include, includedFile);
                    }
                    else if (count == 0) {
                        // included file not found; skip warning if it's a system include (see comment above)
                        if (!include.isSysInclude)
                            addMarker(markerSync, file, IMarker.SEVERITY_WARNING, "Makefile autodeps: cannot find included file: " + include.toString(), include.line);
                    }
                    else {
                        // count > 1: ambiguous include file
                        addMarker(markerSync, file, IMarker.SEVERITY_WARNING, "Makefile autodeps: ambiguous include (more than one matching file found): " + include.toString(), include.line);
                    }
                }
            }
        }
    }

    protected Map<IContainer,Map<IFile,Set<IFile>>> calculatePerFileDependencies(ProjectData data, ProblemMarkerSynchronizer markerSync) {
        // for each file, collect the list of files it includes
        Map<IFile,Set<IFile>> includedFilesMap = new HashMap<IFile, Set<IFile>>();
        for (IFile file : data.cppSourceFiles.keySet()) {
            Set<IFile> includedFiles = new HashSet<IFile>();
            for (Include include : data.cppSourceFiles.get(file))
                if (data.resolvedIncludes.containsKey(include))
                    includedFiles.add(data.resolvedIncludes.get(include));
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

    protected Map<IContainer,Set<IContainer>> calculateFolderDependencies(ProjectData data, ProblemMarkerSynchronizer markerSync) {
        // process each file, and gradually expand dependencies list
        Map<IContainer,Set<IContainer>> result = new HashMap<IContainer,Set<IContainer>>();

        for (IFile file : data.cppSourceFiles.keySet()) {
            IContainer container = file.getParent();
            if (!result.containsKey(container))
                result.put(container, new HashSet<IContainer>());
            Set<IContainer> currentDeps = result.get(container);

            for (Include include : data.cppSourceFiles.get(file)) {
                if (data.resolvedIncludes.containsKey(include)) {
                    // include resolved successfully and unambiguously
                    IFile includedFile = data.resolvedIncludes.get(include);
                    IContainer dependency = includedFile.getParent();
                    int numSubdirs = StringUtils.countMatches(include.filename, "/");
                    for (int i=0; i<numSubdirs && !(dependency instanceof IWorkspaceRoot); i++)
                        dependency = dependency.getParent();
                    if (dependency instanceof IWorkspaceRoot)
                        addMarker(markerSync, file, IMarker.SEVERITY_WARNING, "Makefile generation: cannot represent included directory in the workspace: it is higher than project root: " + include.toString(), include.line);
                    Assert.isTrue(dependency.getLocation().toString().equals(StringUtils.removeEnd(includedFile.getLocation().toString(), "/"+include.filename))); //XXX why as Assert...?

                    // add folder to the dependent folders
                    if (dependency != container && !currentDeps.contains(dependency))
                        currentDeps.add(dependency);
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
        System.out.println("Folder dependencies:");
        for (IContainer folder : deps.keySet()) {
            System.out.print("Folder " + folder.getFullPath().toString() + " depends on: ");
            for (IContainer dep : deps.get(folder)) {
                System.out.print(" " + MakefileTools.makeRelativePath(dep.getFullPath(), folder.getFullPath()).toString());
            }
            System.out.println();
        }
    }

    public void dumpPerFileDependencies(IProject project) {
    	Map<IContainer, Map<IFile, Set<IFile>>> perFileDependencies = getPerFileDependencies(project);
    	for(IContainer con : perFileDependencies.keySet()) {
    		System.out.println("folder: "+con.getFullPath());
    		for(IFile file : perFileDependencies.get(con).keySet())
    			System.out.println("  file: "+file.getName()+": "+perFileDependencies.get(con).get(file).toString());
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
