/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.editor.EditorUtil;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.osgi.framework.BundleContext;

/**
 * Activator for the plug-in.
 *
 * @author rhornig, andras
 */
public class NedResourcesPlugin extends AbstractUIPlugin {

    public static String PLUGIN_ID;

    // The shared instance.
    private static NedResourcesPlugin plugin;

    private NedSourceFoldersEditingManager nedSourceFoldersEditingManager = new NedSourceFoldersEditingManager();

    /**
     * The constructor.
     */
    public NedResourcesPlugin() {
        plugin = this;
    }

    /**
     * This method is called upon plug-in activation
     */
    @Override
    public void start(BundleContext context) throws Exception {
        super.start(context);
        PLUGIN_ID = getBundle().getSymbolicName();
    }

    /**
     * This method is called when the plug-in is stopped
     */
    @Override
    public void stop(BundleContext context) throws Exception {
        NedResources.getInstance().dispose();
        MsgResources.getInstance().dispose();

        plugin = null;
        super.stop(context);
    }

    /**
     * Returns the shared instance.
     */
    public static NedResourcesPlugin getDefault() {
        return plugin;
    }

    /**
     * Returns the NED file cache of the shared instance of the plugin
     */
    public static INedResources getNedResources() {
        return NedResources.getInstance();
    }

    /**
     * Returns the MSG file cache of the shared instance of the plugin
     */
    public static MsgResources getMsgResources() {
        return MsgResources.getInstance();
    }

    /**
     * When more than one property pages edit the ".nedfolders" file, they can
     * communicate via NedSourceFoldersEditingManager, i.e. can edit the same
     * working copy.
     */
    public static NedSourceFoldersEditingManager getNedSourceFoldersEditingManager() {
        return getDefault().nedSourceFoldersEditingManager;
    }

    /**
     * Returns an image descriptor for the image file at the given
     * plug-in relative path.
     */
    public static ImageDescriptor getImageDescriptor(String path) {
        return AbstractUIPlugin.imageDescriptorFromPlugin(PLUGIN_ID, path);
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
     * Opens the given INedElement in a NED editor, and positions the cursor on it.
     * @param element must not be null, and must be part of the model (i.e. in INedResources)
     */
    public static void openNedElementInEditor(INedElement element) {
        Assert.isNotNull(element);
        openNedElementInEditor(element, IGotoNedElement.Mode.AUTOMATIC);
    }

    /**
     * Opens the given INedElement in a NED editor, and positions the cursor on it.
     *
     * @param element must not be null, and must be part of the model (i.e. in INedResources)
     * @param mode IGotoNedElement.Mode  whether the editor should be opened in text or graphical mode
     *             or in automatic mode
     */
    public static void openNedElementInEditor(INedElement element, IGotoNedElement.Mode mode) {
        INedTypeInfo typeInfo = element.getSelfOrEnclosingTypeElement().getNedTypeInfo();
        IFile file = typeInfo.getNedFile();

        // check if file is null; if so, it is a built-in type
        if (file == null) {
            MessageDialog.openInformation(Display.getDefault().getActiveShell(),
                    "Cannot Open Type", "Built-in types cannot be opened for editing.");
            return;
        }


        try {
            IEditorPart editor = EditorUtil.openEditor(file, true);

            // select the component so it will be visible in the opened editor
            if (editor instanceof IGotoNedElement) {
                ((IGotoNedElement)editor).showInEditor(element, mode);
            }
        } catch (PartInitException e) {
            // no message dialog is needed, because the platform displays an ErrorEditPart anyway
            logError("Cannot open NED editor", e);
        }
    }

}
