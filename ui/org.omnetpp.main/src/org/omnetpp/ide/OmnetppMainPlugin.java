package org.omnetpp.ide;

import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.UUID;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.IConstants;
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
	
	private String latestVersion = null;
	private boolean latestVersionChecked = false;
	
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
    
    public static String getUUID() {
        String uuid = getDefault().getPluginPreferences().getString("UUID");
        if (StringUtils.isEmpty(uuid)) {
            uuid = UUID.randomUUID().toString();
            getDefault().getPluginPreferences().setValue("UUID", uuid);
            getDefault().savePluginPreferences();
        }
        return uuid;
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
		return getDefault().getPreferenceStore().getString(OmnetppPreferencePage.OMNETPP_ROOT);
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
	
    public boolean haveNetworkConnection() {
        return getLatestVersion()!=null;
    }

	public boolean haveNewVersion() {
	    return getLatestVersion()!=null && !getVersion().equals(getLatestVersion());
	}
	
	/**
     * Returns the latest version by checking the omnetpp.org site, or null 
     * if the request has failed (no network present, etc). The result is cached,
     * that is, only the first call actually does a HTTP request, even it it fails.
     */
    public String getLatestVersion() {
        if (!latestVersionChecked) {
            try {
                byte buffer[] = new byte[1024];
                URL url = new URL(IConstants.LATESTVERSION_URL+getUrlParams());
                url.openStream().read(buffer);
                latestVersion = new String(buffer).trim();
            } catch (MalformedURLException e) {
            } catch (IOException e) {
            }
            latestVersionChecked = true;
        }
        return latestVersion;
    }

    public static String getUrlParams() {
        return "?v="+getVersion()+"&s="+getUUID();
    }
    
}
