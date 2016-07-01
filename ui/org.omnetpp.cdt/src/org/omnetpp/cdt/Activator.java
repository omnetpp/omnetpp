/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt;

import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.formatter.DefaultCodeFormatterConstants;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.preferences.InstanceScope;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.jface.viewers.DecorationOverlayIcon;
import org.eclipse.jface.viewers.IDecoration;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.eclipse.ui.preferences.ScopedPreferenceStore;
import org.omnetpp.cdt.build.BuildSpecFolderRenameDetector;
import org.omnetpp.cdt.build.MsgFileRemoveDetector;
import org.omnetpp.cdt.cache.NewConfigConfigurer;
import org.omnetpp.cdt.ui.ProjectFeaturesListener;
import org.omnetpp.common.util.StringUtils;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle.
 * @author Andras
 */
public class Activator extends AbstractUIPlugin {

    // The plug-in ID
    public static final String PLUGIN_ID = "org.omnetpp.cdt";

    // The shared instance
    private static Activator plugin;

    private ProjectFeaturesListener projectFeaturesListener = new ProjectFeaturesListener();
    private NewConfigConfigurer newConfigConfigurer = new NewConfigConfigurer();
    private CProjectChecker cprojectChecker = new CProjectChecker();
    private BuildSpecFolderRenameDetector folderRenameDetector = new BuildSpecFolderRenameDetector();
    private MsgFileRemoveDetector msgFileRemoveDetector = new MsgFileRemoveDetector();

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

        projectFeaturesListener.hookListeners();
        newConfigConfigurer.hookListeners();
        cprojectChecker.hookListeners();
        folderRenameDetector.hookListeners();
        msgFileRemoveDetector.hookListeners();

        cprojectChecker.checkAllOpenProjects();

        // Set the default settings ifor CDT editors.
        // We will use spaces instead of tabs. This hack is needed because the
        // OMNeT++ code style profile is not applied by default when a new
        // workspace is created.
        // TODO we should implement the OMMeT++ style via an extension point.
        // See ToolFactory for hints on the extension point to use.
        IPreferenceStore ps = new ScopedPreferenceStore(InstanceScope.INSTANCE, CCorePlugin.PLUGIN_ID);
        ps.setDefault(DefaultCodeFormatterConstants.FORMATTER_TAB_CHAR, CCorePlugin.SPACE);
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
     */
    public void stop(BundleContext context) throws Exception {
        projectFeaturesListener.unhookListeners();
        newConfigConfigurer.unhookListeners();
        cprojectChecker.unhookListeners();
        folderRenameDetector.unhookListeners();
        msgFileRemoveDetector.unhookListeners();

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
        String msg = StringUtils.defaultIfEmpty(exception.getMessage(), exception.getClass().getSimpleName());
        return wrapIntoCoreException(msg, exception);
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
        ImageDescriptor id = getImageDescriptor(path);
        if (id == null) {
            IllegalArgumentException e = new IllegalArgumentException("Cannot load image from: "+path);
            logError(e);
            throw e;
        }
        return id.createImage();
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
     * Decorates the given image with the overlay images (any element may be null),
     * and returns the result as a new image. The index of images in the overlayImagePath
     * array is determined by the {@link IDecoration#TOP_LEFT}, {@link IDecoration#TOP_RIGHT},
     * {@link IDecoration#BOTTOM_LEFT} and {@link IDecoration#BOTTOM_RIGHT} and
     * {@link IDecoration#UNDERLAY} constant values.
     * The result image gets cached in an internal image registry,
     * so clients do not need to (moreover, must not) dispose of the image.
     */
    public static Image getCachedDecoratedImage(String baseImagePath, String overlayImagePath[]) {
        Image baseImage = getCachedImage(baseImagePath);
        ImageDescriptor[] overlayDescs = new ImageDescriptor[overlayImagePath.length];
        String key = baseImagePath;
        // Calculate a unique key for the decorated image. If all decoration is null
        // the base image key is used and the base image will be returned from the registry.
        for (int i=0; i<overlayImagePath.length; i++)
            if (overlayImagePath[i] != null) {
                overlayDescs[i] = getImageDescriptor(overlayImagePath[i]);
                key += ":"+i+"="+overlayImagePath[i];
            }
        Image result = getDefault().getImageRegistry().get(key);
        if (result == null) {
            result = new DecorationOverlayIcon(baseImage, overlayDescs).createImage();
            getDefault().getImageRegistry().put(key, result);
        }
        return result;
    }

    public static NewConfigConfigurer getNewConfigConfigurer() {
        return getDefault().newConfigConfigurer;
    }
}
