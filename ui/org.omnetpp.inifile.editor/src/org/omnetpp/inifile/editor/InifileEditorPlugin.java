package org.omnetpp.inifile.editor;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
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
	@Override
	public void start(BundleContext context) throws Exception {
		super.start(context);
        PLUGIN_ID = getBundle().getSymbolicName();
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
	 */
	@Override
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
	
	/**
	 * Decorates the given image with the overlay image (either can be null),
	 * and caches and returns the result. Key is a key which will be used to 
	 * retrieve the cached image from the plugin image registry, must be unique.
	 * Flags affects the alignment of the overlay image, use SWT constants like
	 * BEGINING, END, TOP, BOTTOM. 
	 */
	//FIXME transparency gets lost
	public static Image getDecoratedImage(Image image, Image overlayImage, int flags, String key) {
		key = "decorated-image:" + key;
		ImageRegistry imageRegistry = getDefault().getImageRegistry();
		Image result = imageRegistry.get(key);
		if (result == null) {
			if (overlayImage == null)
				result = image;
			else {
				int width = image == null ? 16 : image.getBounds().width;
				int height = image == null ? 16 : image.getBounds().height;
				int x = (flags&SWT.BEGINNING)!=0 ? 0 : (flags&SWT.END)!=0 ? width - overlayImage.getBounds().width : 0;
				int y = (flags&SWT.TOP)!=0 ? 0 : (flags&SWT.BOTTOM)!=0 ? height - overlayImage.getBounds().height : 0;
				result = new Image(null, width, height); // SWT.IMAGE_COPY does not seem to work (exception)
				GC gc = new GC(result);
				if (image != null) gc.drawImage(image, 0, 0);
				gc.drawImage(overlayImage, x, y);
				gc.dispose();
			}
			imageRegistry.put(key, result);
		}
		return result;
	}
	
}
