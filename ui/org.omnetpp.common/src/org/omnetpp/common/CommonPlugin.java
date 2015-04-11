/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common;


import org.apache.commons.lang3.StringUtils;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.preferences.ConfigurationScope;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.IPerspectiveDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.eclipse.ui.preferences.ScopedPreferenceStore;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.wizard.FreemarkerEclipseLoggerFactory;
import org.osgi.framework.BundleContext;

import freemarker.log.Logger;

/**
 * The activator class controls the plug-in life cycle
 */
public class CommonPlugin extends AbstractUIPlugin {

    // The plug-in ID
    public static String PLUGIN_ID;

    // The shared instance
    private static CommonPlugin plugin;

    // global preference store (per installation)
    private ScopedPreferenceStore configPreferenceStore;

    // The perspective that was active when we switched to the 'Simulate' perspective during simulation launch
    //FIXME there should be a better place (and form) to store this...
    public IPerspectiveDescriptor originalPerspective;

    /**
     * The constructor
     */
    public CommonPlugin() {
        plugin = this;
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
     */
    public void start(BundleContext context) throws Exception {
        super.start(context);
        PLUGIN_ID = getBundle().getSymbolicName();

        ImageFactory.initialize();

        // configure FreeMarker to use the Eclipse log (and NOT print to stdout)
        try {
            Logger.setLoggerFactory(new FreemarkerEclipseLoggerFactory());
        } catch (Exception e) {
            CommonPlugin.logError("Could not install Eclipse logging into FreeMarker", e);
        }
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
    public static CommonPlugin getDefault() {
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

    public static CoreException wrapIntoCoreException(Throwable exception) {
        String msg = StringUtils.defaultIfEmpty(exception.getMessage(), exception.getClass().getSimpleName());
        return wrapIntoCoreException(msg, exception);
    }

    public static CoreException wrapIntoCoreException(String message, Throwable exception) {
        return new CoreException(new Status(IStatus.ERROR, PLUGIN_ID, 0, message, exception));
    }

}
