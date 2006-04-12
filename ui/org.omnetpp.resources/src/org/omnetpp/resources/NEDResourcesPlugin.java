package org.omnetpp.resources;

import org.eclipse.core.runtime.Plugin;
import org.osgi.framework.BundleContext;

/**
 * Plugin that manages background NED parsing and provides access to NED files' contents.
 */
public class NEDResourcesPlugin extends Plugin {

	// The shared instance.
	private static NEDResourcesPlugin plugin;

	// The actual NED cache
	private NEDResources resources = new NEDResources();
	
	/**
	 * The constructor.
	 */
	public NEDResourcesPlugin() {
		plugin = this;
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
	 */
	public static NEDResourcesPlugin getDefault() {
		return plugin;
	}

	/**
	 * Returns the NED file cache of the shared instance of the plugin
	 */
	public static NEDResources getNEDResources() {
		return plugin.resources;
	}
}
