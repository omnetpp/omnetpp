package org.omnetpp.cdt.makefile;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.OmnetppMainPlugin;

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
    public static final String IGNORABLE_DIRS[] = "CVS RCS SCCS _darcs blib .git .svn .git .bzr .hg backups".split(" +");

    private MakefileTools() {
    }
    
    /**
     * Returns true if the given resource is a file with "cc", "cpp" or "h" extension,
     * but not _m.cc/cpp/h or _n.cc/cpp/h.
     */
    public static boolean isNonGeneratedCppFile(IResource resource) {
        // not an _m.cc or _n.cc file
        return isCppFile(resource) && !resource.getName().matches("_[mn]\\.[^.]+$"); 
    }

    /**
     * Returns true if the given resource is a file with "cc", "cpp" or "h" extension.
     */
    public static boolean isCppFile(IResource resource) {
        if (resource instanceof IFile) {
            String fileExtension = ((IFile)resource).getFileExtension();
            if ("cc".equalsIgnoreCase(fileExtension) || "cpp".equalsIgnoreCase(fileExtension) || "h".equalsIgnoreCase(fileExtension))
                return true;
        }
        return false;
    }

    /**
     * Returns true if the given resource is a file with "msg" extension.
     */
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

    /**
     * Converts inputPath to be relative to referenceDir. This is not possible
     * if the two paths are from different devices, so in this case the method
     * returns the original inputPath unchanged.
     */
    public static IPath makeRelativePath(IPath inputPath, IPath referenceDir) {
        Assert.isTrue(inputPath.isAbsolute());
        Assert.isTrue(referenceDir.isAbsolute());
        if (referenceDir.equals(inputPath))
            return new Path(".");
        if (!StringUtils.equals(inputPath.getDevice(), referenceDir.getDevice()))
            return inputPath;
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
            throw Activator.wrapIntoCoreException(e);
        }
    }

    /**
     * Utility function to determine whether a given container is covered by
     * a given makefile. CDT source dirs and exclusions (CSourceEntry) are ignored, 
     * instead there is excludedFolders (list of folder-relative paths to be excluded;
     * it excludes subtrees not single folders), and makeFolders (i.e. the given folder
     * may falls into the tree of another makefile).
     * 
     * Both excludedFolders and makeFolders may be null.
     */
    public static boolean makefileCovers(IContainer makefileFolder, IContainer folder, boolean deep, List<String> excludedFolders, List<IContainer> makeFolders) {
        if (!deep) {
            return folder.equals(makefileFolder);
        }
        else {
            if (!makefileFolder.getFullPath().isPrefixOf(folder.getFullPath()))
                return false; // not under that folder
            if (excludedFolders != null) {
                IPath folderRelativePath = folder.getFullPath().removeFirstSegments(makefileFolder.getFullPath().segmentCount());
                for (String exludedFolder : excludedFolders) 
                    if (new Path(exludedFolder).isPrefixOf(folderRelativePath))
                        return false; // excluded
            }
            if (makeFolders != null) {
                // we visit the ancestors of this folder; if we find another makefile first, the answer is "false"
                for (IContainer tmp = folder; !tmp.equals(makefileFolder); tmp = tmp.getParent())
                    if (makeFolders.contains(tmp))
                        return false;
            }
            return true;
        }
    }

    /**
     * Collects source directories from the project and all dependent projects 
     * containing files matching the provided pattern (regexp). Nonexistent, 
     * closed and non-CDT dependent projects will be ignored.
     */
    public static List<IContainer> collectDirs(ICProjectDescription projectDescription, String pattern) throws CoreException {
        Assert.isNotNull(projectDescription);
		List<IContainer> result = new ArrayList<IContainer>();
		collectDirs(projectDescription, result, pattern);
		return result; 
	}

    private static void collectDirs(ICProjectDescription projectDescription, final List<IContainer> result, final String pattern) throws CoreException {
        IProject project = projectDescription.getProject();
        final ICSourceEntry[] srcEntries = CDataUtil.makeRelative(project, projectDescription.getActiveConfiguration().getSourceEntries());

    	for (ICSourceEntry srcEntry : srcEntries) {
    		IResource sourceFolder = project.findMember(srcEntry.getFullPath()); 
    		if (sourceFolder != null) {
		    	sourceFolder.accept(new IResourceVisitor() {
		    		public boolean visit(IResource resource) throws CoreException {
		    			if (MakefileTools.isGoodFolder(resource)) {
		    				if (!CDTUtils.isExcluded(resource, srcEntries)
		    						&& (pattern == null || containsFileMatchingPattern((IContainer)resource, pattern)))
		    					result.add((IContainer)resource);
		
		    				return true;
		    			}
		    			return false;
		    		}
		    	});
    		}
    	}

    	// collect directories from referenced projects too (recursively)
    	for(IProject refProj : project.getReferencedProjects()) {
            ICProjectDescription refProjDesc = CoreModel.getDefault().getProjectDescription(refProj);
            if (refProjDesc != null)
                collectDirs(refProjDesc, result, pattern);
        }
    }

    /**
     * True if the given container has a file which name matches the given pattern.
     */
    private static boolean containsFileMatchingPattern(IContainer container, String pattern) throws CoreException {
    	for (IResource member : container.members())
    		if (member.getFullPath().toPortableString().matches(pattern))
    			return true;
    	return false;
    }
    
    /**
     * Returns *location* paths for the include directories: the OMNeT++ include directory,
     * plus source directories from the project and all dependent projects. 
     */
    public static List<IPath> getOmnetppIncludeLocationsForProject(ICProjectDescription projectDescription) throws CoreException {
        List<IPath> result = new ArrayList<IPath>();

        // add the omnetpp include directory
        result.add(new Path(OmnetppMainPlugin.getOmnetppInclDir()));

        // add project source directories as include dirs for the indexer
        // Note: "*.h" pattern is not good because of includes of the form "subdir/file.h" 
        // (i.e. "subdir" might need to be added to the include path too, even if it doesn't contain any header)
        for (IContainer incDir : MakefileTools.collectDirs(projectDescription, null)) 
            result.add(incDir.getLocation());
        return result;
    }

//XXX experimental 
//    
//    public static void updateProjectIncludePaths(ICProjectDescription projectDescription) throws CoreException {
//        List<IContainer> desiredIncDirs = MakefileTools.collectDirs(projectDescription, null);
//        for (ICConfigurationDescription config : projectDescription.getConfigurations()) {
//            ICFolderDescription rootFolderDesc = (ICFolderDescription)config.getResourceDescription(new Path(""), true);
//            ICLanguageSetting[] languageSettings = rootFolderDesc.getLanguageSettings();
//
//            ICLanguageSetting languageSetting = null;
//            for (ICLanguageSetting l : languageSettings) {
//                System.out.println("name:" + l.getName() + " langsettingid:" + l.getId() + " parentid:" + l.getParent().toString() + " langID:" + l.getLanguageId());
//                List<ICLanguageSettingEntry> list = l.getSettingEntriesList(ICSettingEntry.INCLUDE_PATH);
//                //list.add(new CIncludePathEntry("/hello/bubu", ICSettingEntry.VALUE_WORKSPACE_PATH | ICSettingEntry.READONLY | ICSettingEntry.BUILTIN));
//                //list.add(new CIncludePathEntry("/hello/bubu"+System.currentTimeMillis(), ~0));
//                list.add(new CIncludePathEntry("/hello/syspath"+System.currentTimeMillis(), ICSettingEntry.READONLY | ICSettingEntry.BUILTIN));
//                for (ICLanguageSettingEntry e : list)
//                    System.out.println("   " + e);
//                // possible flags: BUILTIN, READONLY, LOCAL (??), VALUE_WORKSPACE_PATH, RESOLVED
//                l.setSettingEntries(ICSettingEntry.INCLUDE_PATH, list); // ===> DOES NOT WORK, BUILTIN/READONLY FLAGS GET CLEARED BY CDT
//
//                System.out.println("UTANA:");
//                List<ICLanguageSettingEntry> list2 = l.getSettingEntriesList(ICSettingEntry.INCLUDE_PATH);
//                for (ICLanguageSettingEntry e : list2) {
//                    if (e instanceof CIncludePathEntry) {
//                        ReflectionUtils.setFieldValue(e, "fFlags", e.getFlags() | ICSettingEntry.READONLY);
//                    }
//                    System.out.println("   " + e);
//                }
//            }
//            // remove existing include paths
//            // add new include paths
//            //XXX language settings! and folder specific!! AAARGH....
//        }
//
//        // see https://bugs.eclipse.org/bugs/show_bug.cgi?id=206372
//            
//        // testing CoreModel.setRawPathEntries() -- yields editable entries like ICLangageSettingEntry...
//        IProject project = projectDescription.getProject();
//        ICProject cproject = CoreModel.getDefault().getCModel().getCProject(project.getName());
//        IPathEntry[] rawPathEntries = CoreModel.getRawPathEntries(cproject);
//        for (IPathEntry e : rawPathEntries)
//            System.out.println("** ENTRY: " + e);
//        IncludeEntry newEntry = new IncludeEntry(new Path(""), new Path(""), new Path(""), new Path("/someproj/inc-"+System.currentTimeMillis()), true, new IPath[]{}, false);
//        rawPathEntries = (IPathEntry[])ArrayUtils.add(rawPathEntries, newEntry);
//        CoreModel.setRawPathEntries(cproject, rawPathEntries, null);
//
//        // random test code...
//        IDiscoveredPathInfo discoveredInfo = MakeCorePlugin.getDefault().getDiscoveryManager().getDiscoveredInfo(project);
//        System.out.println(discoveredInfo);
//    }

}
