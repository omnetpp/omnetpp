package org.omnetpp.cdt.makefile;

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

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
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

    public static void generateMakefiles(IContainer container, IProgressMonitor monitor) throws CoreException {
        Map<IFile, List<Include>> fileIncludes = processFilesIn(container, monitor);
        Map<IContainer,List<IContainer>> deps = calculateDependencies(fileIncludes);
        
        String makeMakeFile = generateMakeMakeFile(deps);
        
        for (IContainer folder : deps.keySet()) {
            System.out.print("Folder " + folder.getFullPath().toString() + " depends on: ");
            for (IContainer dep : deps.get(folder)) {
                System.out.print(" " + makeRelativePath(folder.getFullPath(), dep.getFullPath()).toString());
            }
            System.out.println();
        }
        
        System.out.println("\n\n" + makeMakeFile);
    }

    public static String generateMakeMakeFile(Map<IContainer, List<IContainer>> deps) {
        String result = "";
        for (IContainer folder : deps.keySet()) {
            List<String> includeOptions = new ArrayList<String>();
            for (IContainer dep : deps.get(folder))
                includeOptions.add("-I" + makeRelativePath(folder.getFullPath(), dep.getFullPath()).toString());
            String folderPath = folder.getProjectRelativePath().toString();  //XXX refine
            result += "\tcd " + folderPath + " && $(MAKEMAKE) $(OPTS) -n -r " + StringUtils.join(includeOptions, " ") + "\n";
        }
        return result;
    }

    /**
     * For each folder, it determines which other folders it depends on (i.e. includes files from).
     */
    public static Map<IContainer,List<IContainer>> calculateDependencies(Map<IFile,List<Include>> fileIncludes) {
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
        Map<IContainer,List<IContainer>> result = new HashMap<IContainer,List<IContainer>>();
        Set<Include> unresolvedIncludes = new HashSet<Include>();
        Set<Include> ambiguousIncludes = new HashSet<Include>();
        for (IFile file : fileIncludes.keySet()) {
            IContainer container = file.getParent();
            if (!result.containsKey(container))
                result.put(container, new ArrayList<IContainer>());
            List<IContainer> currentDeps = result.get(container);
            
            for (Include include : fileIncludes.get(file)) {
                if (include.isSysInclude && standardHeaders.contains(include.filename)) {
                    // this is a standard C/C++ header file, just ignore
                }
                else if (include.filename.contains("/")) {
                    //TODO deal with it separately. interpret as relative path to the current file?
                    System.out.println("CONTAINS SLASH: " + include.filename);
                }
                else {
                    // determine which IFile(s) the include maps to
                    List<IFile> list = filesByName.get(include.filename);
                    if (list == null || list.isEmpty()) {
                        // oops, included file not found. what do we do?
                        unresolvedIncludes.add(include);
                    }
                    else if (list.size() > 1) {
                        // oops, ambiguous include file.  what do we do?
                        ambiguousIncludes.add(include);
                    }
                    else {
                        // include resolved successfully and unambiguously
                        IFile includedFile = list.get(0);

                        // add its folder to the dependent folders
                        IContainer dependentContainer = includedFile.getParent();
                        if (dependentContainer != container && !currentDeps.contains(dependentContainer))
                            currentDeps.add(dependentContainer);
                    }
                }
            }
        }

        System.out.println("includes not found: " + StringUtils.join(unresolvedIncludes, " "));
        System.out.println("ambiguous includes: " + StringUtils.join(ambiguousIncludes, " "));

        //TODO calculate transitive closure here...
        
        return result;
    }

    public static Map<IFile,List<Include>> processFilesIn(IContainer container, final IProgressMonitor monitor) throws CoreException {
        final Map<IFile,List<Include>> result = new HashMap<IFile,List<Include>>();
        container.accept(new IResourceVisitor() {
            public boolean visit(IResource resource) throws CoreException {
                if (isCppFile(resource) || isMsgFile(resource)) {
                    monitor.subTask(resource.getFullPath().toString());
                    try {
                        IFile file = (IFile)resource;
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
                        throw new RuntimeException("Could not process file " + resource.getFullPath().toString(), e);
                    }
                    monitor.worked(1);
                }
                if (monitor.isCanceled())
                    return false;
                return true;
            }
        });
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
