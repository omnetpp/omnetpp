package org.omnetpp.neddoc;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.util.UIUtils;
import org.osgi.framework.BundleContext;

/**
 * This class controls the plug-in life cycle
 */
public class NeddocPlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static final String PLUGIN_ID = "org.omnetpp.neddoc";

	// The shared instance
	private static NeddocPlugin plugin;
	
	private GeneratorConfiguration generatorConfiguration;
	
	/**
	 * The constructor
	 */
	public NeddocPlugin() {
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
		plugin = this;
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
	 *
	 * @return the shared instance
	 */
	public static NeddocPlugin getDefault() {
		return plugin;
	}

	/**
	 * Returns an image descriptor for the image file at the given
	 * plug-in relative path
	 *
	 * @param path the path
	 * @return the image descriptor
	 */
	public static ImageDescriptor getImageDescriptor(String path) {
		return imageDescriptorFromPlugin(PLUGIN_ID, path);
	}

	/**
	 * Returns the GeneratorConfiguration instance which stores settings between subsequent
	 * documentation generations.
	 */
	public GeneratorConfiguration getGeneratorConfiguration() {
	    if (generatorConfiguration == null) {
	        generatorConfiguration = new GeneratorConfiguration();
	        restoreGeneratorConfiguration(generatorConfiguration);
	    }

	    return generatorConfiguration;
	}

    public void storeGeneratorConfiguration(GeneratorConfiguration generatorConfiguration) {
        generatorConfiguration.store(UIUtils.getDialogSettings(this, GeneratorConfigurationDialog.class.getName()));
    }

    public void restoreGeneratorConfiguration(GeneratorConfiguration generatorConfiguration) {
        generatorConfiguration.restore(UIUtils.getDialogSettings(this, GeneratorConfigurationDialog.class.getName()));
    }
}
