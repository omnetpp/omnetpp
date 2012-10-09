package com.swtworkbench.community.xswt;

import java.util.MissingResourceException;
import java.util.ResourceBundle;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import com.swtworkbench.community.xswt.metalogger.EclipseLogger;
import com.swtworkbench.community.xswt.metalogger.Logger;

public class XswtPlugin extends AbstractUIPlugin {
    private static final String PLUGIN_ID = "org.swtworkbench.xswt";
    private static XswtPlugin plugin;
    private ResourceBundle resourceBundle;

    public XswtPlugin() {
        plugin = this;
        try {
            this.resourceBundle = ResourceBundle.getBundle("com.swtworkbench.community.xswt.XswtPluginResources");
        }
        catch (MissingResourceException localMissingResourceException) {
            this.resourceBundle = null;
        }
        Logger.setLogger(new EclipseLogger(this));
        Logger.log().setDebug(false);
    }

    public void start(BundleContext context) throws Exception {
        super.start(context);
    }

    public void stop(BundleContext context) throws Exception {
        super.stop(context);
    }

    public static XswtPlugin getDefault() {
        return plugin;
    }

    public static String getResourceString(String key) {
        ResourceBundle bundle = getDefault().getResourceBundle();
        try {
            return ((bundle != null) ? bundle.getString(key) : key);
        }
        catch (MissingResourceException localMissingResourceException) {
        }
        return key;
    }

    public ResourceBundle getResourceBundle() {
        return this.resourceBundle;
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
}