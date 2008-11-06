package org.omnetpp.ide;

import java.text.SimpleDateFormat;
import java.util.Date;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.preferences.ConfigurationScope;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.eclipse.ui.preferences.ScopedPreferenceStore;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.preferences.OmnetppPreferencePage;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle
 */
public class OmnetppMainPlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static String PLUGIN_ID;

	// The shared instance
	private static OmnetppMainPlugin plugin;

	private ScopedPreferenceStore configPreferenceStore;
	
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
	}

    public static String getVersion() {
        return (String)getDefault().getBundle().getHeaders().get("Bundle-Version");
    }
    
    /**
     * For use in the version check URL.
     */
    public static String getInstallDate() {
        String installDate = getDefault().getConfigurationPreferenceStore().getString("installDate");
        if (StringUtils.isEmpty(installDate)) {
            installDate = new SimpleDateFormat("yyyyMMddHHmmss").format(new Date());
            getDefault().getConfigurationPreferenceStore().setValue("installDate", installDate);
            
            // getDefault().savePluginPreferences();
        }
        return installDate;
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
	 * The main directory (where Makefile.inc or configuser.vc resides)
	 */
	public static String getOmnetppRootDir() {
		// FIXME do some fallback if not specified in the store (try the platformdir/.. at least)
		return getDefault().getConfigurationPreferenceStore().getString(OmnetppPreferencePage.OMNETPP_ROOT);
	}
	
	public static String getOmnetppBinDir() {
        // FIXME in fact we have to look into the Makefile.inc or configuser.vc files in the root and get the directory from there
    	return new Path(getOmnetppRootDir()).append("bin").toOSString();
    }

	public static String getOmnetppInclDir() {
        // FIXME in fact we have to look into the Makefile.inc or configuser.vc files in the root and get the directory from there
    	return new Path(getOmnetppRootDir()).append("include").toOSString();
    }

	public static String getOmnetppLibDir() {
        // FIXME in fact we have to look into the Makefile.inc or configuser.vc files in the root and get the directory from there
    	return new Path(getOmnetppRootDir()).append("lib").toOSString();
    }

    public IPreferenceStore getConfigurationPreferenceStore() {
        // Create the preference store lazily.
        if (configPreferenceStore == null) {
            configPreferenceStore = new ScopedPreferenceStore(new ConfigurationScope(),getBundle().getSymbolicName());

        }
        return configPreferenceStore;
    }
	
}
