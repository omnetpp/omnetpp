/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide;


import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.OmnetppDirs;
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
        getLog().log(new Status(IStatus.INFO, PLUGIN_ID, IStatus.OK, "OMNeT++ IDE " + OmnetppDirs.getVersion() + " started.", null));

        if (omnetppDynamicPluginLoader == null)
            omnetppDynamicPluginLoader = new OmnetppDynamicPluginLoader(context);
        ResourcesPlugin.getWorkspace().addResourceChangeListener(omnetppDynamicPluginLoader);

        Assert.isTrue(omnetppProjectOpenListener == null);
        omnetppProjectOpenListener = new OmnetppProjectOpenListener();
        ResourcesPlugin.getWorkspace().addResourceChangeListener(omnetppProjectOpenListener);
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
}
