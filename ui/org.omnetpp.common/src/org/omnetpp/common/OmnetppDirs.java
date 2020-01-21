package org.omnetpp.common;

import java.io.File;

import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.omnetpp.common.util.StringUtils;

/**
 * Supply locations of various directories in this OMNeT++ installation.
 *
 * Note: stuff in this class used to be part of OmnetppMainPlugin.
 *
 * @author rhornig
 */
public class OmnetppDirs {
    /**
     * Return the OMNeT++ version as a string.
     */
    public static String getVersion() {
        return CommonPlugin.getDefault().getBundle().getHeaders().get("Bundle-Version");
    }

    /**
     * Return the OMNeT++ major version as a string.
     */
    public static String getMajorVersion() {
        String[] parts = getVersion().split("\\.");
        return parts[0];
    }

    /**
     * Return the OMNeT++ minor version as a string.
     */
    public static String getMinorVersion() {
        String[] parts = getVersion().split("\\.");
        return parts[1];
    }

    /**
     * The main directory (where Makefile.inc resides). Returns empty string if
     * omnetpp location cannot be determined.
     */
    public static String getOmnetppRootDir() {
        return CommonPlugin.getConfigurationPreferenceStore().getString(IConstants.PREF_OMNETPP_ROOT);
    }

    /**
     * Returns the omnetpp bin directory, or empty string "" if the omnetpp root dir is not defined.
     */
    public static String getOmnetppBinDir() {
        return getOmnetppSubdir("bin");
    }

    /**
     * Returns the omnetpp include directory, or empty string "" if the omnetpp root dir is not defined.
     */
    public static String getOmnetppInclDir() {
        return getOmnetppSubdir("include");
    }

    /**
     * Returns the omnetpp lib directory, or empty string "" if omnetpp root dir is not defined.
     */
    public static String getOmnetppLibDir() {
        return getOmnetppSubdir("lib");
    }

    /**
     * Returns the omnetpp python directory, or empty string "" if the omnetpp root dir is not defined.
     */
    public static String getOmnetppPythonDir() {
        return  getOmnetppSubdir("python");
    }

    /**
     * Returns the omnetpp samples directory, or empty string "" if omnetpp root dir is not defined.
     */
    public static String getOmnetppSamplesDir() {
        return getOmnetppSubdir("samples");
    }

    private static String getOmnetppSubdir(String name) {
        String omnetppRoot = getOmnetppRootDir();
        return StringUtils.isBlank(omnetppRoot) ? "" : new Path(omnetppRoot).append(name).toOSString();
    }

    /**
     * Returns the MSYS bin directory which contains utilities like perl, wish etc., or the empty string ""
     * on non-Windows OS or if it cannot be determined.
     */
    public static String getToolsMsysBinDir() {
        if (!Platform.getOS().equals(Platform.OS_WIN32))
            return "";
        String oppRoot = getOmnetppRootDir();
        if (StringUtils.isBlank(oppRoot))
            return "";
        return new Path(oppRoot).append("tools").append("win64").append("usr").append("bin").toOSString();
    }

    /**
     * Returns the MinGW bin directory, or the empty string "" on non-Windows OS or if it cannot be determined.
     */
    public static String getToolsMingwBinDir() {
        if (!Platform.getOS().equals(Platform.OS_WIN32))
            return "";
        String oppRoot = getOmnetppRootDir();
        if (StringUtils.isBlank(oppRoot))
            return "";
        return new Path(oppRoot).append("tools").append("win64").append("mingw64").append("bin").toOSString();
    }

    /**
     * Returns the MinGW bin directory, or the empty string "" on non-Windows OS or if it cannot be determined.
     */
    public static String getToolsVisualCBinDir() {
        if (!Platform.getOS().equals(Platform.OS_WIN32))
            return "";
        String oppRoot = getOmnetppRootDir();
        if (StringUtils.isBlank(oppRoot))
            return "";
        return new Path(oppRoot).append("tools").append("win64").append("visualc").append("bin").toOSString();
    }

    /**
     * checks if there exist a unix styled compiled lib under the lib
     */
    public static boolean isOppsimUnixStyleLibraryPresent(boolean debug) {
        String libFileName = getOmnetppLibDir() + "/liboppsim" + (debug ? "_dbg" : "");
        String extension1, extension2;
        if (Platform.getOS().equals(Platform.OS_WIN32)) {
            // on windows we do not test for dynamic libs (as they are in the bin directory)
            extension1 = ".a";
            extension2 = ".lib";
        } else if (Platform.getOS().equals(Platform.OS_MACOSX)) {
            extension1 = ".dylib";
            extension2 = ".a";
        } else {
            extension1 = ".so";
            extension2 = ".a";
        }

        File file1 = new File(libFileName+extension1);
        File file2 = new File(libFileName+extension2);
        return file1.exists() || file2.exists();
    }

    /**
     * checks if there exist a compiled lib usable by ClangC2 under the lib folder
     */
    public static boolean isOppsimClangC2LibraryPresent(boolean debug) {
        String fileBaseName = getOmnetppLibDir() +"/oppsim" + (debug ? "d" : "");
        File libFile = new File(fileBaseName+".lib");
        return libFile.exists();
    }
}
