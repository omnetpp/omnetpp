/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide;


import java.io.File;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.util.StringUtils;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle
 */
public class OmnetppMainPlugin extends AbstractUIPlugin {

    // The plug-in ID
    public static String PLUGIN_ID;

    // The shared instance
    private static OmnetppMainPlugin plugin;

    protected OmnetppDynamicPluginLoader omnetppDynamicPluginLoader;
    protected OmnetppProjectOpenListener omnetppProjectOpenListener;

    /**
     * The constructor
     */
    public OmnetppMainPlugin() {
        plugin = this;
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
     */
    public void start(BundleContext context) throws Exception {
        super.start(context);
        PLUGIN_ID = getBundle().getSymbolicName();
        getLog().log(new Status(IStatus.INFO, PLUGIN_ID, IStatus.OK, "OMNeT++ IDE "+getVersion()+" started.", null));

        if (omnetppDynamicPluginLoader == null) {
            omnetppDynamicPluginLoader = new OmnetppDynamicPluginLoader(context);
        }
        ResourcesPlugin.getWorkspace().addResourceChangeListener(omnetppDynamicPluginLoader);

        Assert.isTrue(omnetppProjectOpenListener == null);
        omnetppProjectOpenListener = new OmnetppProjectOpenListener();
        ResourcesPlugin.getWorkspace().addResourceChangeListener(omnetppProjectOpenListener);
    }

    public static String getVersion() {
        return getDefault().getBundle().getHeaders().get("Bundle-Version");
    }

    public static String getMajorVersion() {
        String[] parts = getVersion().split("\\.");
        return parts[0];
    }

    public static String getMinorVersion() {
        String[] parts = getVersion().split("\\.");
        return parts[1];
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
     */
    public void stop(BundleContext context) throws Exception {
        plugin = null;
        super.stop(context);
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(omnetppDynamicPluginLoader);
    }

    /**
     * Returns the shared instance
     */
    public static OmnetppMainPlugin getDefault() {
        return plugin;
    }

    public static void logError(Throwable exception) {
        if (plugin != null) {
            plugin.getLog().log(new Status(IStatus.ERROR, PLUGIN_ID, 0, exception.toString(), exception));
        }
        else {
            exception.printStackTrace();
        }
    }

    public static void logError(String message, Throwable exception) {
        if (plugin != null) {
            plugin.getLog().log(new Status(IStatus.ERROR, PLUGIN_ID, 0, message, exception));
        }
        else {
            System.err.println(message);
            exception.printStackTrace();
        }
    }

    /**
     * The main directory (where Makefile.inc or configuser.vc resides). Returns empty string if
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
    public static String getMsysBinDir() {
        if (!Platform.getOS().equals(Platform.OS_WIN32))
            return "";
        String oppRoot = getOmnetppRootDir();
        if (StringUtils.isBlank(oppRoot))
            return "";
        return new Path(oppRoot).append("tools").append("win32").append("usr").append("bin").toOSString();
    }

    /**
     * Returns the MinGW bin directory, or the empty string "" on non-Windows OS or if it cannot be determined.
     */
    public static String getMingwBinDir() {
        if (!Platform.getOS().equals(Platform.OS_WIN32))
            return "";
        String oppRoot = getOmnetppRootDir();
        if (StringUtils.isBlank(oppRoot))
            return "";
        return new Path(oppRoot).append("tools").append("win32").append("mingw32").append("bin").toOSString();
    }

    /**
     * Detects whether oppsim library (built by GCC or CLANG) is present in the
     * lib directory.
     */
    public static boolean isOppsimGccOrClangLibraryPresent(boolean debug) {
        // FIXME hardcoded names! we should rather get the library path from the Makefile.inc
        // and then use that directory to check for the presence of library files.
        return isOppsimUnixStyleLibraryPresent("gcc", debug) ||
                isOppsimUnixStyleLibraryPresent("clang", debug);
    }

    /**
     * checks if there exist a unix styled compiled lib under the lib/<libSubDir>
     */
    private static boolean isOppsimUnixStyleLibraryPresent(String libSubDir, boolean debug) {
        String libFileName = getOmnetppLibDir() + "/" + libSubDir + "/liboppsim" + (debug ? "d" : "");
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
     * Detects whether oppsim library (dll/lib) (built by Visual C++) is present in the
     * lib directory.
     */
    // FIXME should be reviewed once we put all library files under /lib (instead
    // of the current /lib/<toolchain> name.
    public static boolean isOppsimVcLibraryPresent(boolean debug) {
        return isOppsimVcXXXLibraryPresent("vc90", debug) ||
               isOppsimVcXXXLibraryPresent("vc100", debug) ||
               isOppsimVcXXXLibraryPresent("vc110", debug) ||
               isOppsimVcXXXLibraryPresent("vc120", debug);
    }

    /**
     * checks if there exist a compiled lib under the lib/<libSubDir>
     */
    private static boolean isOppsimVcXXXLibraryPresent(String libSubDir, boolean debug) {
        String fileBaseName = getOmnetppLibDir() +"/" + libSubDir + "/oppsim" + (debug ? "d" : "");
        File libFile = new File(fileBaseName+".lib");
        return libFile.exists();
    }
}
