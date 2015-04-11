/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.launch;


import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.IJobManager;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle
 *
 * @author rhornig
 */
public class LaunchPlugin extends AbstractUIPlugin {

    // The plug-in ID
    public static String PLUGIN_ID;

    // The shared instance
    private static LaunchPlugin plugin;

    /**
     * The constructor
     */
    public LaunchPlugin() {
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
     */
    @Override
    public void start(BundleContext context) throws Exception {
        super.start(context);
        plugin = this;
        PLUGIN_ID = getBundle().getSymbolicName();
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
     */
    @Override
    public void stop(BundleContext context) throws Exception {
        // cancel all running jobs and wait for them to finish before continuing with shutdown
        IJobManager jobMan = Job.getJobManager();
        jobMan.cancel(SimulationLauncherJob.SIMULATION_JOB_FAMILY);
        jobMan.join(SimulationLauncherJob.SIMULATION_JOB_FAMILY, null);

        plugin = null;
        super.stop(context);
    }

    /**
     * Returns the shared instance
     */
    public static LaunchPlugin getDefault() {
        return plugin;
    }

    /**
     * Returns an image descriptor for the image file at the given
     * plug-in relative path.
     */
    public static ImageDescriptor getImageDescriptor(String path) {
        return imageDescriptorFromPlugin(PLUGIN_ID, path);
    }

    /**
     * Creates an image. IMPORTANT: The image is NOT cached! Callers
     * are responsible for disposal of the image.
     */
    public static Image getImage(String path) {
        return getImageDescriptor(path).createImage();
    }

    public static void logError(Throwable exception) {
        logError(exception.toString(), exception);
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
