package org.omnetpp.ned2.model;

import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

/**
 * The main plugin class to be used in the desktop.
 */
public class NEDModelPlugin extends AbstractUIPlugin {

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
	public void start(BundleContext context) throws Exception {
		super.start(context);
	}

	/**
	 * This method is called when the plug-in is stopped
	 */
	public void stop(BundleContext context) throws Exception {
		super.stop(context);
		plugin = null;
	}

	/**
	 * Returns the shared instance.
	 *
	 * @return the shared instance.
	 */
	public static NEDModelPlugin getDefault() {
		return plugin;
	}
}
