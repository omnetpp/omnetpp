package org.omnetpp.cdt.makefile;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * Utility functions for Makefile generation.
 *  
 * @author Andras
 */
public class MakefileTools {
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

    // all standard C/C++ headers -- we'll ignore these #includes when looking for cross-folder dependencies
    public static final String ALL_STANDARD_HEADERS = 
        C_HEADERS + " " + C99_HEADERS + " " + CPLUSPLUS_HEADERS + " " + POSIX_HEADERS;

    // directories we'll not search for source files
    public static final String IGNORABLE_DIRS[] = "CVS RCS SCCS _darcs blib .git .svn .git .bzr .hg backups".split(" ");

    private MakefileTools() {
    }
    
    public static boolean isNonGeneratedCppFile(IResource resource) {
        // not an _m.cc or _n.cc file
        return isCppFile(resource) && !resource.getName().matches("_[mn]\\.[^.]+$"); 
    }

    public static boolean isCppFile(IResource resource) {
        if (resource instanceof IFile) {
            //TODO: ask CDT about registered file extensions?
            String fileExtension = ((IFile)resource).getFileExtension();
            if ("cc".equalsIgnoreCase(fileExtension) || "cpp".equalsIgnoreCase(fileExtension) || "h".equalsIgnoreCase(fileExtension))
                return true;
        }
        return false;
    }

    public static boolean isMsgFile(IResource resource) {
        return resource instanceof IFile && "msg".equals(((IFile)resource).getFileExtension());
    }

    /**
     * Returns true if the resource is a potential source folder 
     * (not team private or backups folder). Does NOT check whether
     * folder is marked as excluded in CDT. 
     */
    public static boolean isGoodFolder(IResource resource) {
        // note: we explicitly check for "CVS", "_darcs" etc, because they are only recognized by 
        // isTeamPrivateMember() if the corresponding plugin is installed
        return (resource instanceof IContainer && 
                !resource.getName().startsWith(".") &&
                !ArrayUtils.contains(MakefileTools.IGNORABLE_DIRS, resource.getName()) &&
                !((IContainer)resource).isTeamPrivateMember()); 
    }

    public static IPath makeRelativePath(IPath inputPath, IPath referenceDir) {
        if (referenceDir.equals(inputPath))
            return new Path(".");
        int commonPrefixLen = inputPath.matchingFirstSegments(referenceDir);
        int upLevels = referenceDir.segmentCount() - commonPrefixLen;
        return new Path(StringUtils.removeEnd(StringUtils.repeat("../", upLevels), "/")).append(inputPath.removeFirstSegments(commonPrefixLen));
    }

    public static void ensureFileContent(IFile file, byte[] bytes, IProgressMonitor monitor) throws CoreException {
        // only overwrites file if its content is not already what's desired
        try {
            if (!file.exists())
                file.create(new ByteArrayInputStream(bytes), true, monitor);
            else if (!Arrays.equals(FileUtils.readBinaryFile(file.getContents()), bytes)) // NOTE: byte[].equals does NOT compare content, only references!!!
                file.setContents(new ByteArrayInputStream(bytes), true, false, monitor);
        }
        catch (IOException e) {
            throw Activator.wrap(e);
        }
    }
    
}
