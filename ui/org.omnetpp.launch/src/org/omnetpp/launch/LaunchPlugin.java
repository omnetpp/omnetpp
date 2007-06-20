package org.omnetpp.launch;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.variables.VariablesPlugin;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import org.omnetpp.common.util.StringUtils;

/**
 * The activator class controls the plug-in life cycle
 */
public class LaunchPlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static String PLUGIN_ID;

	// The shared instance
	private static LaunchPlugin plugin;

	/**
	 * The constructor
	 */
	public LaunchPlugin() {
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
	 */
	@Override
    public void start(BundleContext context) throws Exception {
		super.start(context);
		plugin = this;
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
	public static LaunchPlugin getDefault() {
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
     * Expands and returns the working directory attribute of the given launch
     * configuration. Returns <code>null</code> if a working directory is not
     * specified. If specified, the working is verified to point to an existing
     * directory in the local file system.
     *
     * @return an absolute path to a directory in the local file system, or
     * <code>null</code> if unspecified
     * @throws CoreException if unable to retrieve the associated launch
     * configuration attribute, if unable to resolve any variables, or if the
     * resolved location does not point to an existing directory in the local
     * file system
     */
    public static IPath getWorkingDirectoryPath(ILaunchConfiguration config){
        String location;
        try {
            // get the working directory. the default will be the project root of the currently selected exe file
            location = config.getAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY,"");
            if (StringUtils.isEmpty(location))
                    location = "${project_loc:/"+config.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME,"")+"/"
                           +config.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME,"")+"}";

            if (location != null) {
                String expandedLocation = VariablesPlugin.getDefault().getStringVariableManager().performStringSubstitution(location);
                if (expandedLocation.length() > 0) {
                    IPath newPath = new Path(expandedLocation);
                    return newPath.makeAbsolute();
                }
            }
        } catch (CoreException e) {
            LaunchPlugin.logError("Error getting working directory from configuration", e);
        }
        return new Path("${workspace_loc}");
    }
}
