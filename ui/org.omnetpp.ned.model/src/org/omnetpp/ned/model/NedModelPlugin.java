/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.ned.model.ex.NedElementFactoryEx;
import org.osgi.framework.BundleContext;

/**
 * Activator class for the NED model plug-in.
 *
 * @author rhornig
 */
public class NedModelPlugin extends AbstractUIPlugin {
    public static String PLUGIN_ID;

    //The shared instance.
    private static NedModelPlugin plugin;

    /**
     * The constructor.
     */
    public NedModelPlugin() {
        plugin = this;

        // initialize the model factory to use this factory as default
        // this is called when the plugin is activated
        NedElementFactoryEx.setInstance(new NedElementFactoryEx());

    }

    /**
     * This method is called upon plug-in activation
     */
    @Override
    public void start(BundleContext context) throws Exception {
        super.start(context);
        PLUGIN_ID = getBundle().getSymbolicName();
    }

    /**
     * This method is called when the plug-in is stopped
     */
    @Override
    public void stop(BundleContext context) throws Exception {
        super.stop(context);
        plugin = null;
    }

    /**
     * Returns the shared instance.
     */
    public static NedModelPlugin getDefault() {
        return plugin;
    }

    public static void log(Exception e) {
        IStatus status = new Status(Status.ERROR, PLUGIN_ID, 1, e.getMessage(), e);
        getDefault().getLog().log(status);
    }

}
