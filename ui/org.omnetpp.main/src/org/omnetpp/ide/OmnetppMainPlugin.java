package org.omnetpp.ide;


import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
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
	}

    public static String getVersion() {
        return (String)getDefault().getBundle().getHeaders().get("Bundle-Version");
    }
    
    /*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
	 */
	public void stop(BundleContext context) throws Exception {
		plugin = null;
		super.stop(context);
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
	 * @return The omnetpp bin directory, or empty string "" if omnetpp root dir is not defined.
	 */
	public static String getOmnetppBinDir() {
		// FIXME in fact we have to look into the Makefile.inc or configuser.vc files in the root and get the directory from there
		String oppRoot = getOmnetppRootDir();
		if (StringUtils.isBlank(oppRoot))
			return "";
    	return new Path(oppRoot).append("bin").toOSString();
    }

	/**
	 * @return The omnetpp inc directory, or empty string "" if omnetpp root dir is not defined.
	 */
	public static String getOmnetppInclDir() {
        // FIXME in fact we have to look into the Makefile.inc or configuser.vc files in the root and get the directory from there
		String oppRoot = getOmnetppRootDir();
		if (StringUtils.isBlank(oppRoot))
			return "";
    	return new Path(oppRoot).append("include").toOSString();
    }

	/**
	 * @return The omnetpp lib directory, or empty string "" if omnetpp root dir is not defined.
	 */
	public static String getOmnetppLibDir() {
        // FIXME in fact we have to look into the Makefile.inc or configuser.vc files in the root and get the directory from there
		String oppRoot = getOmnetppRootDir();
		if (StringUtils.isBlank(oppRoot))
			return "";
    	return new Path(oppRoot).append("lib").toOSString();
    }

}
