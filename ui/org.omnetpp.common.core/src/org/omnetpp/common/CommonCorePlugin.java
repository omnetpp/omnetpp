package org.omnetpp.common;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.preferences.ConfigurationScope;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.eclipse.ui.preferences.ScopedPreferenceStore;
import org.omnetpp.common.image.ImageFactory;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle
 */
public class CommonCorePlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static final String PLUGIN_ID = "org.omnetpp.common.core";

	// The shared instance
	private static CommonCorePlugin plugin;
	
	// global preference store (per installation)
	private ScopedPreferenceStore configPreferenceStore;
	
	/**
	 * The constructor
	 */
	public CommonCorePlugin() {
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.core.runtime.Plugins#start(org.osgi.framework.BundleContext)
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
		plugin = this;
		
        ImageFactory.initialize(getConfigurationPreferenceStore().getString(IConstants.PREF_OMNETPP_IMAGE_PATH).split(";"));
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.core.runtime.Plugin#stop(org.osgi.framework.BundleContext)
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
	public static CommonCorePlugin getDefault() {
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
	 * Creates an image. IMPORTANT: The image is NOT cached! Callers 
	 * are responsible for disposal of the image. 
	 */
	public static Image getImage(String path) {
		return getImageDescriptor(path).createImage();
	}

	/**
	 * Like getImage(), but the image gets cached in an internal image registry,
	 * so clients do not need to (moreover, must not) dispose of the image.
	 */
	public static Image getCachedImage(String path) {
		ImageRegistry imageRegistry = getDefault().getImageRegistry();
		Image image = imageRegistry.get(path);
		if (image==null) {
			image = getImage(path);
			imageRegistry.put(path, image);
		}
		return image;
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
			if (exception!=null)
				exception.printStackTrace();
		}
	}
	
    public static ScopedPreferenceStore getConfigurationPreferenceStore() {
        // Create the preference store lazily.
        if (getDefault().configPreferenceStore == null) {
        	getDefault().configPreferenceStore = new ScopedPreferenceStore(new ConfigurationScope(), PLUGIN_ID);
        }
        return getDefault().configPreferenceStore;
    }
}
