package org.omnetpp.cdt;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.cdt.cache.DependencyCache;
import org.omnetpp.cdt.wizard.ProjectTemplateStore;
import org.omnetpp.common.image.ImageFactory;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle.
 * @author Andras
 */
public class Activator extends AbstractUIPlugin {
    private DependencyCache dependencyCache = new DependencyCache();

    // The plug-in ID
    public static final String PLUGIN_ID = "org.omnetpp.cdt";

    // The shared instance
    private static Activator plugin;

    private ProjectTemplateStore projectTemplateStore;
    
    /**
     * The constructor
     */
    public Activator() {
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
     */
    public static Activator getDefault() {
        return plugin;
    }

    public static void log(int severity, String message) {
        getDefault().getLog().log(new Status(severity, PLUGIN_ID, message));
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

    public static CoreException wrapIntoCoreException(Throwable exception) {
        return wrapIntoCoreException(exception.getMessage(), exception);
    }

    public static CoreException wrapIntoCoreException(String message, Throwable exception) {
        return new CoreException(new Status(IStatus.ERROR, PLUGIN_ID, 0, message, exception));
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
     * Decorates the given image with the overlay image (may be null), and returns 
     * the result as a new image. For positioning, use SWT.BEGIN, SWT.CENTER, SWT.END.
     * The result image gets cached in an internal image registry,
     * so clients do not need to (moreover, must not) dispose of the image.
     */
    public static Image getCachedDecoratedImage(String imagePath, String overlayImagePath, int hpos, int vpos) {
        if (overlayImagePath==null)
            return getCachedImage(imagePath);
        String key = imagePath+":"+overlayImagePath+":"+hpos+":"+vpos;
        ImageRegistry imageRegistry = getDefault().getImageRegistry();
        Image result = imageRegistry.get(key);
        if (result == null) {
            Image image = getCachedImage(imagePath);        
            Image overlayImage = getCachedImage(overlayImagePath);
            result = ImageFactory.decorateImage(image, overlayImage, hpos, vpos);
            imageRegistry.put(key, result);
        }
        return result;
    }

    /**
     * Decorates the given image with the overlay images (any element may be null), 
     * and returns the result as a new image. For positioning, use SWT.BEGIN, 
     * SWT.CENTER, SWT.END. The result image gets cached in an internal image registry,
     * so clients do not need to (moreover, must not) dispose of the image.
     */
    public static Image getCachedDecoratedImage(String imagePath, String overlayImagePath[], int hpos[], int vpos[]) {
        String key = imagePath;
        for (int i=0; i<overlayImagePath.length; i++)
            key += ":"+overlayImagePath[i]+":"+hpos[i]+":"+vpos[i];
        ImageRegistry imageRegistry = getDefault().getImageRegistry();
        Image result = imageRegistry.get(key);
        if (result == null) {
            Image baseImage = getCachedImage(imagePath);        
            Image image = baseImage;        
            for (int i=0; i<overlayImagePath.length; i++) {
                if (overlayImagePath[i] != null) {
                    Image overlayImage = getCachedImage(overlayImagePath[i]);
                    Image newImage = ImageFactory.decorateImage(image, overlayImage, hpos[i], vpos[i]);
                    if (image != baseImage)
                        image.dispose();
                    image = newImage;
                }
            }
            result = image;
            imageRegistry.put(key, result);
        }
        return result;
    }

    public static DependencyCache getDependencyCache() {
        return getDefault().dependencyCache;
    }
    
    public static ProjectTemplateStore getProjectTemplateStore() {
        if (getDefault().projectTemplateStore == null)
            getDefault().projectTemplateStore = new ProjectTemplateStore();
        return getDefault().projectTemplateStore;
    }
    
}
