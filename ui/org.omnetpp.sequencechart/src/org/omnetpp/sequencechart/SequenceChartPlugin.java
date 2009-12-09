/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart;

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
public class SequenceChartPlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static String PLUGIN_ID;

	// The shared instance
	private static SequenceChartPlugin plugin;

	/**
	 * The constructor
	 */
	public SequenceChartPlugin() {
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
	 * Logs an exception into the platform log file.
	 */
	public void logException(Throwable ex) {
		getLog().log(new Status(IStatus.ERROR, PLUGIN_ID, IStatus.OK, "An exception occurred", ex));
	}

	/**
	 * Returns the shared instance
	 *
	 * @return the shared instance
	 */
	public static SequenceChartPlugin getDefault() {
		return plugin;
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
}
