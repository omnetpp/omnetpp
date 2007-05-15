package org.omnetpp.inifile.editor;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle
 */
public class InifileEditorPlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static String PLUGIN_ID;

	// The shared instance
	private static InifileEditorPlugin plugin;
	
	/**
	 * The constructor
	 */
	public InifileEditorPlugin() {
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
	public static InifileEditorPlugin getDefault() {
		return plugin;
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

}
