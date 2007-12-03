package org.omnetpp.usagetracker;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Plugin;
import org.eclipse.core.runtime.Status;
import org.eclipse.swt.widgets.Display;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle
 */
public class Activator extends Plugin {

	// The plug-in ID
	public static final String PLUGIN_ID = "org.omnetpp.usagetracker";

	// The shared instance
	private static Activator plugin;
	
	/**
	 * The constructor
	 */
	public Activator() {
	}

	@Override
	public void start(BundleContext context) throws Exception {
		super.start(context);
		plugin = this;
		
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                // activate individual trackers
                new PartActivationTracker().hookListeners();
                new IdleTimeTracker().hookListeners();
                new ResourceChangeTracker().hookListeners();
                new GUITracker().hookListeners();
            }
        });
	}

    @Override
	public void stop(BundleContext context) throws Exception {
		plugin = null;
		super.stop(context);
	}

	/**
	 * Returns the shared instance
	 */
	public static Activator getDefault() {
		return plugin;
	}

	/**
	 * Record a message into the log. Timestamp will be automatically recorded.
	 */
	public static void logMessage(String message) {
	    IStatus status = new Status(Status.OK, PLUGIN_ID, message);
	    getDefault().getLog().log(status);
	}
}
