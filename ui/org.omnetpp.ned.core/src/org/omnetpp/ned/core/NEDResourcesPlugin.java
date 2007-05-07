package org.omnetpp.ned.core;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.editor.EditorUtil;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.osgi.framework.BundleContext;

/**
 * The main plugin class to be used in the desktop.
 */
public class NEDResourcesPlugin extends AbstractUIPlugin {

    public static String PLUGIN_ID;

	//The shared instance.
	private static NEDResourcesPlugin plugin;

	// The actual NED cache
	private NEDResources resources = new NEDResources();

	/**
	 * The constructor.
	 */
	public NEDResourcesPlugin() {
		plugin = this;
	}

	/**
	 * This method is called upon plug-in activation
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);

        PLUGIN_ID = getBundle().getSymbolicName();

//        System.out.println("NEDResourcesPlugin started");

        ResourcesPlugin.getWorkspace().addResourceChangeListener(NEDResourcesPlugin.getNEDResources());


        // XXX this is most probably NOT the way to archieve that
		// all NED files get parsed on startup. At minimum, this should
		// be done in the background, as a long-running operation with an
		// IProgressMonitor...
		// Cf. quote from org.eclipse.core.runtime.Plugin:
		//   "Note 2: This method is intended to perform simple initialization
		//   of the plug-in environment. The platform may terminate initializers
		//   that do not complete in a timely fashion."
		// So we should find a better way.
		readAllNedFilesInWorkspace();

	}

	/**
	 * This method is called when the plug-in is stopped
	 */
	public void stop(BundleContext context) throws Exception {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(NEDResourcesPlugin.getNEDResources());
		plugin = null;
        super.stop(context);
//        System.out.println("NEDResourcesPlugin stopped");
	}

	/**
	 * Returns the shared instance.
	 *
	 * @return the shared instance.
	 */
	public static NEDResourcesPlugin getDefault() {
		return plugin;
	}

	/**
	 * Returns the NED file cache of the shared instance of the plugin
	 */
	public static NEDResources getNEDResources() {
		return plugin.resources;
	}

	/**
	 * Returns an image descriptor for the image file at the given
	 * plug-in relative path.
	 *
	 * @param path the path
	 * @return the image descriptor
	 */
	public static ImageDescriptor getImageDescriptor(String path) {
		return AbstractUIPlugin.imageDescriptorFromPlugin(PLUGIN_ID, path);
	}

	//XXX do we need this at all ???
    // if we could activate the resouce listener before opening a project we would not need this
    // function called in start()
	private void readAllNedFilesInWorkspace() {
		try {
			IResource wsroot = ResourcesPlugin.getWorkspace().getRoot();
			wsroot.accept(new IResourceVisitor() {
				public boolean visit(IResource resource) {
					if (NEDResources.isNEDFile(resource))
						getNEDResources().readNEDFile((IFile) resource);
					return true;
				}
			});
		} catch (CoreException e) {
			logError("Error during workspace refresh: ",e);
		}
		getNEDResources().rehashIfNeeded();

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
	 * Opens the given INEDElement in a NED editor, and positions the cursor on it.
	 * @param element must NOT be null, and MUST be part of the model (i.e. in NEDResourcesPlugin)
	 */
	public static void openNEDElementInEditor(INEDElement element) {
	    Assert.isNotNull(element);
        openNEDElementInEditor(element, IGotoNedElement.Mode.AUTOMATIC);
	}

    /**
     * Opens the given INEDElement in a NED editor, and positions the cursor on it.
     * @param element must NOT be null, and MUST be part of the model (i.e. in NEDResourcesPlugin)
     * @param mode IGotoNedElement.Mode whether the editor should be opened in text or grahical mode
     *             or in automatic mode
     */
    public static void openNEDElementInEditor(INEDElement element, IGotoNedElement.Mode mode) {
        INEDTypeInfo typeInfo = element.getContainerNEDTypeInfo();
        IFile file = typeInfo.getNEDFile();

        // check if file is null. it is a built in type in this case
        if (file == null) {
            MessageDialog.openWarning(Display.getDefault().getActiveShell(), 
                    "Warning", "Built in types cannot be opened for editing!");
            return;
        }


        try {
            IEditorPart editor = EditorUtil.openEditor(file, true);

            // select the component so it will be visible in the opened editor
            if (editor instanceof IGotoNedElement) {
                ((IGotoNedElement)editor).showInEditor(element, mode);
            }
        } catch (PartInitException e) {
            // no message dialog is needed, because the platform displays an erroreditpart anyway
            logError("Cannot open NED editor", e);
        }
    }

}
