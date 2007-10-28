package org.omnetpp.cdt.makefile;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * Scans all C++ files, etc....
 * TODO
 *  
 * @author Andras
 */
//XXX requirements: 
// - ability to ignore some folders (i.e. pass in explicit folder list? .cppfolders file?)
// - ability to re-use hand-written makefiles in certain dirs
// - dependencies on other projects
// - compile-time flags (ie. -I for external libs)
// - ... ?
public class CppTools {
    // standard C headers, see e.g. http://www-ccs.ucsd.edu/c/lib_over.html
    public static final String C_HEADERS = 
        "assert.h ctype.h errno.h float.h iso646.h limits.h locale.h " +
        "math.h setjmp.h signal.h stdarg.h stddef.h stdio.h stdlib.h " +
        "string.h time.h wchar.h wctype.h";
    // C headers added by C99, see http://en.wikipedia.org/wiki/C_standard_library
    public static final String C99_HEADERS = 
        "complex.h fenv.h inttypes.h stdbool.h stdint.h tgmath.h";
    // standard C++ headers, see http://en.wikipedia.org/wiki/C++_standard_library#Standard_headers
    public static final String CPLUSPLUS_HEADERS = 
        "bitset deque list map queue set stack vector algorithm functional iterator " + 
        "locale memory stdexcept utility string fstream ios iostream iosfwd iomanip " + 
        "istream ostream sstream streambuf complex numeric valarray exception limits " + 
        "new typeinfo cassert cctype cerrno cfloat climits cmath csetjmp csignal " + 
        "cstdlib cstddef cstdarg ctime cstdio cstring cwchar cwctype";
    // POSIX headers, see http://en.wikipedia.org/wiki/C_POSIX_library
    public static final String POSIX_HEADERS = 
        "cpio.h dirent.h fcntl.h grp.h pwd.h sys/ipc.h sys/msg.h sys/sem.h " + 
        "sys/stat.h sys/time.h sys/types.h sys/utsname.h sys/wait.h tar.h termios.h " + 
        "unistd.h utime.h";
    public static final String ALL_STANDARD_HEADERS = 
        C_HEADERS + " " + C99_HEADERS + " " + CPLUSPLUS_HEADERS + " " + POSIX_HEADERS;

    public static final String BOILERPLATE = 
        "#\n" + 
        "# Makefile to create all other makefiles for the project.\n" + 
        "# This same file is used on all platforms including Linux (gnu make) and Windows (nmake).\n" + 
        "#\n" + 
        "# GENERATED FILE -- DO NOT MODIFY.\n" + 
        "#\n" + 
        "# The vars ROOT, MAKEMAKE and EXT have to be specified externally, on the 'make' command line.\n" + 
        "#ROOT=/home/user/projects/foo\n" + 
        "#MAKEMAKE=opp_nmakemake\n" + 
        "#EXT=.vc\n" + 
        "\n" + 
        "# To build Windows DLLs, add this to OPTS below: -PINET_API\n" + 
        "OPTS=-f -b $(ROOT) -c $(ROOT)/inetconfig$(EXT)\n" + 
        "\n" + 
        "all:\n";
    
    
    /**
     * Represents an #include in a C++ file
     */
    public static class Include {
        public String filename;
        public boolean isSysInclude; // true: <foo.h>, false: "foo.h" 

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

    public static void generateMakefiles(IContainer rootContainer, IProgressMonitor monitor) throws CoreException {
        IContainer[] containers = collectFolders(rootContainer);
        Map<IFile, List<Include>> fileIncludes = processFilesIn(containers, monitor);
        Map<IContainer,Set<IContainer>> deps = calculateDependencies(fileIncludes);
        // dumpDeps(deps);
        
        String makeMakeFile = generateMakeMakeFile(containers, deps);
        System.out.println("\n\n" + makeMakeFile);
    }

    public static void dumpDeps(Map<IContainer, Set<IContainer>> deps) {
        for (IContainer folder : deps.keySet()) {
            System.out.print("Folder " + folder.getFullPath().toString() + " depends on: ");
            for (IContainer dep : deps.get(folder)) {
                System.out.print(" " + makeRelativePath(folder.getFullPath(), dep.getFullPath()).toString());
            }
            System.out.println();
        }
    }

    public static String generateMakeMakeFile(IContainer[] containers, Map<IContainer, Set<IContainer>> deps) {
        String result = BOILERPLATE;
        for (IContainer folder : containers) {
            List<String> includeOptions = new ArrayList<String>();
            if (deps.containsKey(folder))
                for (IContainer dep : deps.get(folder))
                    includeOptions.add("-I" + makeRelativePath(folder.getFullPath(), dep.getFullPath()).toString());
            String folderPath = folder.getProjectRelativePath().toString();  //XXX refine: only relative if it fits best
            result += "\tcd " + folderPath + " && $(MAKEMAKE) $(OPTS) -n -r " + StringUtils.join(includeOptions, " ") + "\n";
        }
        return result;
    }

    /**
     * For each folder, it determines which other folders it depends on (i.e. includes files from).
     */
    public static Map<IContainer,Set<IContainer>> calculateDependencies(Map<IFile,List<Include>> fileIncludes) {
        // we'll ignore the standard C/C++ headers
        final Set<String> standardHeaders = new HashSet<String>(Arrays.asList(ALL_STANDARD_HEADERS.split(" ")));

        // build a hash table of all files, for easy lookup by name
        Map<String,List<IFile>> filesByName = new HashMap<String, List<IFile>>();
        for (IFile file : fileIncludes.keySet()) {
            String name = file.getName();
            if (!filesByName.containsKey(name))
                filesByName.put(name, new ArrayList<IFile>());
            filesByName.get(name).add(file);
        }

        // process each file, and gradually expand dependencies list
        Map<IContainer,Set<IContainer>> result = new HashMap<IContainer,Set<IContainer>>();
        Set<Include> unresolvedIncludes = new HashSet<Include>();
        Set<Include> ambiguousIncludes = new HashSet<Include>();
        Set<Include> unsupportedIncludes = new HashSet<Include>();
        
        for (IFile file : fileIncludes.keySet()) {
            IContainer container = file.getParent();
            if (!result.containsKey(container))
                result.put(container, new HashSet<IContainer>());
            Set<IContainer> currentDeps = result.get(container);
            
            for (Include include : fileIncludes.get(file)) {
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
        }

        System.out.println("includes not found: " + StringUtils.join(unresolvedIncludes, " "));
        System.out.println("ambiguous includes: " + StringUtils.join(ambiguousIncludes, " "));
        System.out.println("unsupported cases: " + StringUtils.join(unsupportedIncludes, " "));

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

public static IContainer[] collectFolders(IContainer container) throws CoreException {
    final List<IContainer> result = new ArrayList<IContainer>();
        container.accept(new IResourceVisitor() {
            public boolean visit(IResource resource) throws CoreException {
                if (resource instanceof IContainer && !resource.getName().startsWith(".") && !((IContainer)resource).isTeamPrivateMember()) { 
                    result.add((IContainer)resource);
                    return true;
                }
                return false;
            }
        });
        Collections.sort(result, new Comparator<IContainer>() {
            public int compare(IContainer o1, IContainer o2) {
                return o1.getFullPath().toString().compareTo(o2.getFullPath().toString());
            }});
        return result.toArray(new IContainer[]{});
    }

    public static Map<IFile,List<Include>> processFilesIn(IContainer[] containers, final IProgressMonitor monitor) throws CoreException {
        final Map<IFile,List<Include>> result = new HashMap<IFile,List<Include>>();

        for (IContainer container : containers) {
            for (IResource member : container.members()) {
                if (isCppFile(member) || isMsgFile(member)) {
                    monitor.subTask(member.getFullPath().toString());
                    try {
                        IFile file = (IFile)member;
                        List<Include> includes = CppTools.parseIncludes(file);
                        result.put(file, includes);
                        
                        if (isMsgFile(file)) {
                            // pretend that the generated _m.h file also exists
                            String msgHFileName = file.getName().replaceFirst("\\.[^.]*$", "_m.h");
                            IFile msgHFile = file.getParent().getFile(new Path(msgHFileName));
                            if (!msgHFile.exists()) // otherwise it'll be visited as well
                                result.put(msgHFile, includes);
                        }
                    }
                    catch (IOException e) {
                        throw new RuntimeException("Could not process file " + member.getFullPath().toString(), e);
                    }
                    monitor.worked(1);
                    if (monitor.isCanceled())
                        return null;
                }
            }
        }
        return result;
    }

    public static boolean isCppFile(IResource resource) {
        if (resource instanceof IFile) {
            //TODO: ask CDT about registered file extensions?
            String fileExtension = ((IFile)resource).getFileExtension();
            if ("cc".equalsIgnoreCase(fileExtension) || "cpp".equals(fileExtension) || "h".equals(fileExtension))
                return true;
        }
        return false;
    }

    public static boolean isMsgFile(IResource resource) {
        return resource instanceof IFile && "msg".equals(((IFile)resource).getFileExtension());
    }

    public static IPath makeRelativePath(IPath base, IPath target) {
        if (base.equals(target))
            return new Path(".");
        int commonPrefixLen = target.matchingFirstSegments(base);
        int upLevels = base.segmentCount() - commonPrefixLen;
        return new Path(StringUtils.removeEnd(StringUtils.repeat("../", upLevels), "/")).append(target.removeFirstSegments(commonPrefixLen));
    }

    /**
     * Collect #includes from a C++ source file
     */
    public static List<Include> parseIncludes(IFile file) throws CoreException, IOException {
        String contents = FileUtils.readTextFile(file.getContents()) + "\n";
        return parseIncludes(contents);
    }

    /**
     * Collect #includes from C++ source file contents
     */
    public static List<Include> parseIncludes(String source) {
        List<Include> result = new ArrayList<Include>();
        Matcher matcher = Pattern.compile("(?m)^\\s*#\\s*include\\s+([\"<])(.*?)[\">].*$").matcher(source);
        while (matcher.find()) {
            boolean isSysInclude = matcher.group(1).equals("<");
            String fileName = matcher.group(2);
            result.add(new Include(fileName.trim().replace('\\','/'), isSysInclude));
        }
        return result;
    }


}
