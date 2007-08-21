package org.omnetpp.ned.model;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.osgi.framework.BundleContext;

/**
 * Activator class for the NED model plug-in.
 *
 * @author rhornig
 */
public class NEDModelPlugin extends AbstractUIPlugin {
    public static String PLUGIN_ID;

	//The shared instance.
	private static NEDModelPlugin plugin;

	/**
	 * The constructor.
	 */
	public NEDModelPlugin() {
	    plugin = this;

        // initialize the model factory to use this factory as default
        // this is called when the plugin is activated
	    NEDElementFactoryEx.setInstance(new NEDElementFactoryEx());

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
	public static NEDModelPlugin getDefault() {
		return plugin;
	}

    public static void log(Exception e) {
        IStatus status = new Status(Status.ERROR, PLUGIN_ID, 1, e.getMessage(), e);
        getDefault().getLog().log(status);
    }

}
