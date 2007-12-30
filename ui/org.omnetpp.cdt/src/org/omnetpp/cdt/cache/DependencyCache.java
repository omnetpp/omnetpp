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
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
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
 * @author Andras
 */
//TODO test with the Base/Ext test projects
//XXX markers: if I mark INET/Obsolete as "excluded", existing markers will not be removed!!!
//XXX handle _m.h files! (pretend that _m.h files exist, create IFiles for them)
//XXX how to obey "make clean" ?
public class DependencyCache {
    // the standard C/C++ headers (we'll ignore those #include directives)
    protected static final Set<String> standardHeaders = new HashSet<String>(Arrays.asList(MakefileTools.ALL_STANDARD_HEADERS.split(" ")));

    /**
     * Represents an #include in a C++ file
     */
    static class Include {
        public String filename;
        public boolean isSysInclude; // true: <foo.h>, false: "foo.h"
        public int line = -1; //XXX todo fill in while parsing

        public Include(String filename, boolean isSysInclude) {
            Assert.isTrue(filename != null);
            this.isSysInclude = isSysInclude;
            this.filename = filename;
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
        List<IFile> sourceFiles;  // only used during calculations
        Map<Include,IFile> resolvedIncludes;
        Map<IContainer,Set<IContainer>> folderDependencies;
        Map<IContainer, Map<IFile, Set<IFile>>> perFileDependencies;
    }

    // per-file includes
    private Map<IFile,FileIncludes> fileIncludes = new HashMap<IFile, FileIncludes>();

    // list of projects whose include lists are up-to-date in the cache
    private Set<IProject> fileIncludesUpToDate = new HashSet<IProject>();

    // because we want to generate warnings for them
    private Set<IResource> linkedResources = new HashSet<IResource>(); 

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
                    if (event.getDelta() != null)
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
    protected void projectChanged(IProject project) {
        fileIncludesUpToDate.remove(project);
        for (IProject p : projectData.keySet().toArray(new IProject[]{}))
            if (projectData.get(p).projectGroup.contains(project))
                projectData.remove(p);
    }

    /**
     * Returns true if all file includes are up to date, i.e. no file scanning
     * is needed to produce dependencies etc. Wherever UI responsiveness is an
     * issue, if this function returns false then other functions are to be
     * invoked in the background (i.e. in a workspace job).
     */
    // Note: should NOT be synchronized (otherwise the whole point is lost)
    public boolean isUpToDate() {
        return true;  //FIXME todo; make API per-project...?
    }

    protected void collectIncludes(IProject project, ProblemMarkerSynchronizer markers) {
        System.out.println("collectIncludesFully(): " + project);

        try {
            // parse all C++ source files for #include; also warn for linked-in files
            //XXX obsolete comment==> (note: warning for linked-in folders will be issued in generateMakefiles())
            
            // parse all C++ files for #includes
            final ICSourceEntry[] sourceEntries = CDTUtils.getSourceEntriesIfExist(project);
            project.accept(new IResourceVisitor() {
                public boolean visit(IResource resource) throws CoreException {
                    warnIfLinkedResource(resource);
                    if (MakefileTools.isNonGeneratedCppFile(resource) || MakefileTools.isMsgFile(resource))
                        checkFileIncludes((IFile)resource);
                    return MakefileTools.isGoodFolder(resource) && !CDataUtil.isExcluded(resource.getProjectRelativePath(), sourceEntries);
                }
            });
            
            // project is OK now
            fileIncludesUpToDate.add(project);
        }
        catch (CoreException e) {
            addMarker(markers, project, IMarker.SEVERITY_ERROR, "Error scanning source files for #includes: " + StringUtils.nullToEmpty(e.getMessage()), -1);
            Activator.logError(e);
        }
    }

    /** 
     * Forces re-parsing of all files in the project for #includes. 
     * Currently unused.
     */
    public void clean(IProject project) {
        // discard all parsed #includes within this project
        for (IFile f : fileIncludes.keySet().toArray(new IFile[]{}))
            if (f.getProject().equals(project))
                fileIncludes.remove(f);
        projectChanged(project);
    }

    /**
     * Parses the file for the list of #includes, if it's not up to date already.
     */
    protected void checkFileIncludes(IFile file) throws CoreException {
        //System.out.println("   checkFileIncludes(): " + file);
        long fileTime = file.getModificationStamp();
        FileIncludes fileData = fileIncludes.get(file);
        if (fileData == null || fileData.modificationStamp < fileTime) {
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

    protected void warnIfLinkedResource(IResource resource) {
        if (resource.isLinked())
            linkedResources.add(resource);
        else
            linkedResources.remove(resource);
    }

    public IResource[] getLinkedResources() {
        return linkedResources.toArray(new IResource[]{});
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
            throw Activator.wrap("Error collecting #includes from " + file.getFullPath(), e); 
        }
    }

    /**
     * Collect #includes from C++ source file contents
     */
    protected static List<Include> parseIncludes(String source) {
        List<Include> result = new ArrayList<Include>();
        Matcher matcher = Pattern.compile("(?m)^\\s*#\\s*include\\s+([\"<])(.*?)[\">].*$").matcher(source);
        while (matcher.find()) {
            boolean isSysInclude = matcher.group(1).equals("<");
            String fileName = matcher.group(2);
            result.add(new Include(fileName.trim().replace('\\','/'), isSysInclude)); //XXX fill in line number
        }
        return result;
    }

    /**
     * For each folder, it determines which other folders it depends on (i.e. includes files from).
     */
    synchronized public Map<IContainer,Set<IContainer>> getFolderDependencies(IProject project) {
        ProjectData projectData = getOrCreateProjectData(project);
        return projectData.folderDependencies; 
    }

    /**
     * For each folder, it determines which other folders it depends on (i.e. includes files from).
     * Returns the results grouped by folders; that is, each folder maps to the set of files
     * it contains, and each file maps to its dependencies (everything it #includes, directly
     * or indirectly). Grouping by folders significantly speeds up makefile generation. 
     */
    synchronized public Map<IContainer,Map<IFile,Set<IFile>>> getPerFileDependencies(IProject project) {
        ProjectData projectData = getOrCreateProjectData(project);
        return projectData.perFileDependencies; 
    }

    /**
     * Return the given project and all projects referenced from it (transitively)
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
            markerSync.register(project); // ignore referenced projects, as they are processed on their own right

            // ensure all files in this project group have been parsed for #includes
            long begin = System.currentTimeMillis();
            for (IProject p : data.projectGroup)
                if (!fileIncludesUpToDate.contains(p))
                    collectIncludes(p, markerSync);
            System.out.println("SCANNED: " + (System.currentTimeMillis() - begin) + "ms");

            // collect list of .h and .cc files in this project group
            data.sourceFiles = new ArrayList<IFile>();
            for (IFile file : fileIncludes.keySet())
                if (data.projectGroup.contains(file.getProject())) {
                    data.sourceFiles.add(file);
                    markerSync.register(file); //XXX this will remove markers added by other projects too...
                }

            // resolve includes
            resolveIncludes(data, markerSync);

            // calculate per-file dependencies
            data.perFileDependencies = calculatePerFileDependencies(data, markerSync);

            // calculate folder dependencies
            data.folderDependencies = calculateFolderDependencies(data, markerSync);

            data.sourceFiles = null; // no longer needed

            // store
            projectData.put(project, data);
            
            markerSync.runAsWorkspaceJob();
        }
        return projectData.get(project);
    }

    protected void resolveIncludes(ProjectData data, ProblemMarkerSynchronizer markerSync) {
        // build a hash table of files in this project group, for easy lookup by name
        Map<String,List<IFile>> filesByName = new HashMap<String, List<IFile>>();
        for (IFile file : data.sourceFiles) {
            String name = file.getName();
            if (!filesByName.containsKey(name))
                filesByName.put(name, new ArrayList<IFile>());
            filesByName.get(name).add(file);
        }

        // resolve includes in each file
        data.resolvedIncludes = new HashMap<Include, IFile>();
        for (IFile file : data.sourceFiles) {
            IContainer container = file.getParent();
            for (Include include : fileIncludes.get(file).includes) {
                if (include.isSysInclude && standardHeaders.contains(include.filename)) {
                    //FIXME also ignore omnetpp.h!
                    // this is a standard C/C++ header file, just ignore
                }
                else if (include.filename.contains("..")) {
                    // we only recognize an include containing ".." if it's relative to the current dir
                    String filename = include.filename.replaceFirst("_m\\.h$", ".msg");
                    IPath includeFileLocation = container.getLocation().append(new Path(filename));
                    IFile[] f = ResourcesPlugin.getWorkspace().getRoot().findFilesForLocation(includeFileLocation);
                    if (f.length == 0 || !f[0].exists())
                        addMarker(markerSync, file, IMarker.SEVERITY_WARNING, "Makefile autodependencies: cannot resolve #include with '..' unless it is relative to the current dir", include.line); //XXX implement instead of warning!!!
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
                        // included file not found
                        addMarker(markerSync, file, IMarker.SEVERITY_WARNING, "Makefile autodependencies: cannot resolve #include: " + include.toString(), include.line);
                    }
                    else {
                        // count > 1: ambiguous include file
                        addMarker(markerSync, file, IMarker.SEVERITY_WARNING, "Makefile autodependencies: ambiguous #include: " + include.toString(), include.line);
                    }
                }
            }
        }
    }

    protected Map<IContainer,Map<IFile,Set<IFile>>> calculatePerFileDependencies(ProjectData data, ProblemMarkerSynchronizer markerSync) {
        // for each file, collect the list of files it includes
        Map<IFile,Set<IFile>> includedFilesMap = new HashMap<IFile, Set<IFile>>();
        for (IFile file : data.sourceFiles) {
            Set<IFile> includedFiles = new HashSet<IFile>();
            for (Include include : fileIncludes.get(file).includes)
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

        for (IFile file : data.sourceFiles) {
            IContainer container = file.getParent();
            if (!result.containsKey(container))
                result.put(container, new HashSet<IContainer>());
            Set<IContainer> currentDeps = result.get(container);

            for (Include include : fileIncludes.get(file).includes) {
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

    protected void addMarker(ProblemMarkerSynchronizer markerSynchronizer, IResource file, int severity, String message, int line) {
        Map<String, Object> map = new HashMap<String, Object>();
        map.put(IMarker.SEVERITY, severity);
        if (line >= 0)
            map.put(IMarker.LINE_NUMBER, line);
        map.put(IMarker.MESSAGE, message);
        markerSynchronizer.addMarker(file, MakefileBuilder.MARKER_ID, map);
    }
}
