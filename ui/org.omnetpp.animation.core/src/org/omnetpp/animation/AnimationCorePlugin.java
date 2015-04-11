/*--------------------------------------------------------------*

  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle.
 *
 * @author levy
 */
public class AnimationCorePlugin extends AbstractUIPlugin {
	// The plug-in ID
	public static String PLUGIN_ID;

	// The shared instance
	private static AnimationCorePlugin plugin;

	/**
	 * The constructor
	 */
	public AnimationCorePlugin() {
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
	public static AnimationCorePlugin getDefault() {
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
     * Logs an exception into the platform log file.
     */
    public void logException(Throwable ex) {
        getLog().log(new Status(IStatus.ERROR, PLUGIN_ID, IStatus.OK, "An exception occurred", ex));
    }

    public static void logError(Throwable exception) {
        logError(exception.toString(), exception);
    }

    public static void logError(String message, Throwable exception) {
        log(getErrorStatus(0, message, exception));
    }

    public static void log(IStatus status) {
        if (status != null) {
            if (plugin != null) {
                plugin.getLog().log(status);
            }
            else {
                if (status.getMessage() != null)
                    System.err.println(status.getMessage());
                if (status.getException() != null)
                    status.getException().printStackTrace();
            }
        }
    }

    public static IStatus getWarningStatus(String message) {
        return getWarningStatus(0, message);
    }

    public static IStatus getWarningStatus(int code, String message) {
        return getStatus(IStatus.WARNING, code, message, null);
    }

    public static IStatus getErrorStatus(Throwable exception) {
        return getErrorStatus(0, exception.getLocalizedMessage(), exception);
    }

    public static IStatus getErrorStatus(int errorCode, String message, Throwable exception) {
        return getStatus(IStatus.ERROR, errorCode, message, exception);
    }

    public static IStatus getStatus(int severity, int code, String message, Throwable exception) {
        return new Status(severity, PLUGIN_ID, code, message, exception);
    }
}
