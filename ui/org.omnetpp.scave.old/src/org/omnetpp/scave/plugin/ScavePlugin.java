/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.plugin;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import org.omnetpp.scave.model.ResultFileManagerModel;

/**
 * The main plugin class to be used in the desktop.
 *
 * TODO Global notes:
 * - some starting points for dividing the editor area into "datasets" (upper)
 *    and "charts" (lower) area:
 *     - SystemMenuMovePane action class;
 *     - IStackPresentationSite.dragStart() method
 *     - PartStack, EditorStack classes (looks like "upper" and "lower" parts are two EditorStacks)
 *     - EditorSashContainer.addDropSupport()  (maybe this is the counterpart of dragStart())
 *     - looks like a "workbook" in EditorSashContainer is the EditorStack
 *   Probably not possible to do it programmatically (if not by injecting mouse events?)
 */
public class ScavePlugin extends AbstractUIPlugin {

    //The shared instance.
    private static ScavePlugin plugin;

    // holds all scalar files loaded.
    public ResultFileManagerModel resultFileManager = new ResultFileManagerModel();

    /**
     * The constructor.
     */
    public ScavePlugin() {
        plugin = this;
    }

    /**
     * This method is called upon plug-in activation
     */
    public void start(BundleContext context) throws Exception {
        super.start(context);
    }

    /**
     * This method is called when the plug-in is stopped
     */
    public void stop(BundleContext context) throws Exception {
        super.stop(context);
        plugin = null;
    }

    /**
     * Returns the shared instance.
     */
    public static ScavePlugin getDefault() {
        return plugin;
    }

    /**
     * Returns an image descriptor for the image file at the given
     * plug-in relative path.
     *
     * @param path the path
     * @return the image descriptor
     */
    public static ImageDescriptor getImageDescriptor(String path) {
        return AbstractUIPlugin.imageDescriptorFromPlugin("Scave", path);
    }
}
