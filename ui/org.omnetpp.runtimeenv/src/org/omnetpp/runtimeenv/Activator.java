package org.omnetpp.runtimeenv;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle
 * @author Andras
 */
public class Activator extends AbstractUIPlugin {

	// The plug-in ID
	public static final String PLUGIN_ID = "org.omnetpp.runtimeenv";

	// The shared instance
	private static Activator plugin;
	
	private static SimulationManager simulationManager;
	
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
	    simulationManager = new SimulationManager();
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
	 */
	public void stop(BundleContext context) throws Exception {
        simulationManager.dispose();
        simulationManager = null;
        
		plugin = null;
		super.stop(context);
	}

	/**
	 * Returns the shared instance
	 *
	 * @return the shared instance
	 */
	public static Activator getDefault() {
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
	
	public static IWorkbenchPage getActiveWorkbenchPage() {
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        return window == null ? null : window.getActivePage();
    }
	
	public static IEditorPart openEditor(IEditorInput input, String editorId) {
	    IWorkbenchPage page = getActiveWorkbenchPage();
	    if (page != null) {
	        try {
                return page.openEditor(input, editorId);
            }
            catch (PartInitException e) {
                logError("Cannot open editor for " + input, e);
                e.printStackTrace();  //TODO error dialog or something!
            }
	    }
	    return null;
	}

	public static SimulationManager getSimulationManager() {
	    return simulationManager;
	}
}
