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
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
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
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.cdt.makefile.MakefileTools;
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
//FIXME change methods should be synchronized etc
//FIXME should be a ResourceChangeListener
public class DependencyCache {
    /**
     * Represents an #include in a C++ file
     */
    public static class Include {
        public String filename;
        public boolean isSysInclude; // true: <foo.h>, false: "foo.h"
        public IFile resolvesTo;

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
            return filename.hashCode()*31 + (isSysInclude ? 1231 : 1237);
        }

        @Override
        public boolean equals(Object obj) {
            if (obj == null || getClass() != obj.getClass())
                return false;
            Include other = (Include) obj;
            return this == obj || (filename.equals(other.filename) && isSysInclude == other.isSysInclude);
        }
    }

    //XXX TODO later: private Set<IProject> dirtyProjects = new HashSet<IProject>(); // projects that need to be processed again
    //XXX TODO later: private Map<IFile,Long> fileLastRead = new HashMap<IFile, Long>(); // modification timestamps 
    private Map<IFile,List<Include>> fileIncludes = new HashMap<IFile, List<Include>>();
    //XXX TODO later: private Set<IResource> linkedResources = new HashSet<IResource>(); // because we want to generate warnings for them


    public void collectIncludesFully(IProject project, final IProgressMonitor monitor) throws CoreException, IOException {
        // parse all C++ source files for #include; also warn for linked-in files
        // (note: warning for linked-in folders will be issued in generateMakefiles())
        monitor.subTask("Scanning source files in project " + project.getName() + "...");

        // since we're doing a "full build" on this project, remove existing entries from fileIncludes[]
        for (IFile f : fileIncludes.keySet().toArray(new IFile[]{}))
            if (f.getProject().equals(project))
                fileIncludes.remove(f);

        // parse all C++ files for #includes
        final ICSourceEntry[] sourceEntries = CDTUtils.getSourceEntriesIfExist(project);
        project.accept(new IResourceVisitor() {
            public boolean visit(IResource resource) throws CoreException {
                warnIfLinkedResource(resource);
                if (MakefileTools.isNonGeneratedCppFile(resource) || MakefileTools.isMsgFile(resource))
                    processFileIncludes((IFile)resource);
                return MakefileTools.isGoodFolder(resource) && !CDataUtil.isExcluded(resource.getProjectRelativePath(), sourceEntries);
            }
        });
    }

    public void collectIncludesIncrementally(IResourceDelta delta, IProgressMonitor monitor) throws CoreException, IOException {
        if (monitor != null)
            monitor.subTask("Scanning changed files in project " + delta.getResource().getProject().getName() + "...");
        processDelta(delta);
    }

    protected void processDelta(IResourceDelta delta) throws CoreException {
        // re-parse changed C++ source files for #include; also warn for linked-in files
        // (note: warning for linked-in folders will be issued in generateMakefiles())
        final ICSourceEntry[] sourceEntries = CDTUtils.getSourceEntriesIfExist((IProject)delta.getResource());
        delta.accept(new IResourceDeltaVisitor() {
            public boolean visit(IResourceDelta delta) throws CoreException {
                IResource resource = delta.getResource();
                boolean isSourceFile = MakefileTools.isNonGeneratedCppFile(resource) || MakefileTools.isMsgFile(resource);
                switch (delta.getKind()) {
                    case IResourceDelta.ADDED:
                        warnIfLinkedResource(resource);
                        if (isSourceFile)
                            processFileIncludes((IFile)resource);
                        break;
                    case IResourceDelta.CHANGED:
                        warnIfLinkedResource(resource);
                        if (isSourceFile)
                            processFileIncludes((IFile)resource);
                        break;
                    case IResourceDelta.REMOVED: 
                        if (isSourceFile) 
                            fileIncludes.remove(resource);
                        break;
                }
                return MakefileTools.isGoodFolder(resource) && !CDataUtil.isExcluded(resource.getProjectRelativePath(), sourceEntries);
            }
        });
    }

    /**
     * Parses the file for the list of #includes, and returns true if it changed 
     * since the previous state.
     */
    protected boolean processFileIncludes(IFile file) throws CoreException {
        //XXX check / update timestamp
        List<Include> includes = parseIncludes(file);

        if (!includes.equals(fileIncludes.get(file))) {
            fileIncludes.put(file, includes);
            return true;
        }
        return false;
    }

    protected void warnIfLinkedResource(IResource resource) {
        if (resource.isLinked() && !(resource instanceof IProject))
            ; //TODO addMarker(resource, IMarker.SEVERITY_ERROR, "Linked resources are not supported by Makefiles");
    }

    /**
     * Collect #includes from a C++ source file
     */
    protected static List<Include> parseIncludes(IFile file) throws CoreException {
        try {
            String contents = FileUtils.readTextFile(file.getContents()) + "\n";
            return parseIncludes(contents);
        } catch (IOException e) {
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
            result.add(new Include(fileName.trim().replace('\\','/'), isSysInclude));
        }
        return result;
    }

    
//    //XXX currently not used
//    public Map<IFile,List<Include>> collectIncludes(IContainer[] containers, final IProgressMonitor monitor) throws CoreException {
//        final Map<IFile,List<Include>> result = new HashMap<IFile,List<Include>>();
//
//        for (IContainer container : containers) {
//            for (IResource member : container.members()) {
//                if (MakefileTools.isCppFile(member) || MakefileTools.isMsgFile(member)) {
//                    monitor.subTask(member.getFullPath().toString());
//                    IFile file = (IFile)member;
//                    List<Include> includes = parseIncludes(file);
//                    result.put(file, includes);
//
//                    if (MakefileTools.isMsgFile(file)) {
//                        // pretend that the generated _m.h file also exists
//                        String msgHFileName = file.getName().replaceFirst("\\.[^.]*$", "_m.h");
//                        IFile msgHFile = file.getParent().getFile(new Path(msgHFileName));
//                        if (!msgHFile.exists()) // otherwise it'll be visited as well
//                            result.put(msgHFile, includes);
//                    }
//                    monitor.worked(1);
//                    if (monitor.isCanceled())
//                        return null;
//                }
//            }
//        }
//        return result;
//    }

    protected void resolveIncludes() {
        // we'll ignore the standard C/C++ headers
        final Set<String> standardHeaders = new HashSet<String>(Arrays.asList(MakefileTools.ALL_STANDARD_HEADERS.split(" ")));

        // build a hash table of all files, for easy lookup by name
        Map<String,List<IFile>> filesByName = new HashMap<String, List<IFile>>();
        for (IFile file : fileIncludes.keySet()) {
            String name = file.getName();
            if (!filesByName.containsKey(name))
                filesByName.put(name, new ArrayList<IFile>());
            filesByName.get(name).add(file);
        }

        // process each file, and gradually expand dependencies list
        Set<Include> unresolvedIncludes = new HashSet<Include>();
        Set<Include> ambiguousIncludes = new HashSet<Include>();
        Set<Include> unsupportedIncludes = new HashSet<Include>();

        for (IFile file : fileIncludes.keySet()) {
            IContainer container = file.getParent();
            for (Include include : fileIncludes.get(file)) {
                include.resolvesTo = null;
                if (include.isSysInclude && standardHeaders.contains(include.filename)) {
                    // this is a standard C/C++ header file, just ignore
                }
                if (include.filename.contains("..")) {
                    // we only recognize an include containing ".." if it's relative to the current dir
                    String filename = include.filename.replaceFirst("_m\\.h$", ".msg");
                    IPath includeFileLocation = container.getLocation().append(new Path(filename));
                    IFile[] f = ResourcesPlugin.getWorkspace().getRoot().findFilesForLocation(includeFileLocation);
                    if (f.length == 0 || !f[0].exists()) {
                        System.out.println("CANNOT HANDLE INCLUDE WITH '..' UNLESS IT'S RELATIVE TO THE CURRENT DIR: " + include.filename); //XXX
                        unsupportedIncludes.add(include);
                    }
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

                    if (count == 0) {
                        // included file not found. XXX what do we do?
                        unresolvedIncludes.add(include);
                    }
                    else if (count > 1) {
                        // ambiguous include file.  XXX what do we do?
                        ambiguousIncludes.add(include);
                    }
                    else {
                        // include resolved successfully and unambiguously
                        include.resolvesTo = includedFile;
                    }
                }
            }
        }

        System.out.println("resolveIncludes: unresolved includes: " + StringUtils.join(unresolvedIncludes, " "));
        System.out.println("resolveIncludes: ambiguous includes: " + StringUtils.join(ambiguousIncludes, " "));
        System.out.println("resolveIncludes: cannot process: " + StringUtils.join(unsupportedIncludes, " "));
    }

    /**
     * For each folder, it determines which other folders it depends on (i.e. includes files from).
     */
    public Map<IContainer,Set<IContainer>> getFolderDependencies() {
        //FIXME if not changed etc...
        return calculateFolderDependencies();
    }

    protected Map<IContainer,Set<IContainer>> calculateFolderDependencies() {
        // find out which files the #includes correspond to
        resolveIncludes();

        // process each file, and gradually expand dependencies list
        Set<Include> unsupportedIncludes = new HashSet<Include>();
        Map<IContainer,Set<IContainer>> result = new HashMap<IContainer,Set<IContainer>>();

        for (IFile file : fileIncludes.keySet()) {
            IContainer container = file.getParent();
            if (!result.containsKey(container))
                result.put(container, new HashSet<IContainer>());
            Set<IContainer> currentDeps = result.get(container);

            for (Include include : fileIncludes.get(file)) {
                if (include.resolvesTo != null) {
                    // include resolved successfully and unambiguously
                    IFile includedFile = include.resolvesTo;
                    IContainer dependency = includedFile.getParent();
                    int numSubdirs = StringUtils.countMatches(include.filename, "/");
                    for (int i=0; i<numSubdirs && !(dependency instanceof IWorkspaceRoot); i++)
                        dependency = dependency.getParent();
                    if (dependency instanceof IWorkspaceRoot) {
                        //XXX error: cannot represent included dir in the workspace: it is higher than project root
                        unsupportedIncludes.add(include);
                    }
                    Assert.isTrue(dependency.getLocation().toString().equals(StringUtils.removeEnd(includedFile.getLocation().toString(), "/"+include.filename))); //XXX why as Assert...?

                    // add folder to the dependent folders
                    if (dependency != container && !currentDeps.contains(dependency))
                        currentDeps.add(dependency);
                }
            }
        }

        System.out.println("calculateDependencies: cannot represent with -I: " + StringUtils.join(unsupportedIncludes, " "));

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

    /**
     * For each folder, it determines which other folders it depends on (i.e. includes files from).
     * Returns the results grouped by folders; that is, each folder maps to the set of files
     * it contains, and each file maps to its dependencies (everything it #includes, directly
     * or indirectly). Grouping by folders significantly speeds up makefile generation. 
     */
    public Map<IContainer,Map<IFile,Set<IFile>>> getPerFileDependencies() {
        //FIXME if not changed etc...
        return calculatePerFileDependencies();
    }

    protected Map<IContainer,Map<IFile,Set<IFile>>> calculatePerFileDependencies() {
        resolveIncludes();

        Map<IFile,Set<IFile>> includedFilesMap = new HashMap<IFile, Set<IFile>>();
        for (IFile file : fileIncludes.keySet()) {
            Set<IFile> includedFiles = new HashSet<IFile>();
            for (Include include : fileIncludes.get(file))
                if (include.resolvesTo != null)
                    includedFiles.add(include.resolvesTo);
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
}
