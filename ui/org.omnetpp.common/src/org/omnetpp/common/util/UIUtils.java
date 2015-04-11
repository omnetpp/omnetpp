/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.CommonPlugin;

public class UIUtils {
    // normal size error/warning/info icons
    // Note: we could use PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_WARN_TSK),
    // but the error icon seems to be holey on Windows ("x" consists of transparent pixels!) which
    // looks bad in the graphical editor
    public static final Image ICON_ERROR = CommonPlugin.getCachedImage("icons/obj16/error.png");
    public static final Image ICON_WARNING = CommonPlugin.getCachedImage("icons/obj16/warning.png");
    public static final Image ICON_INFO = CommonPlugin.getCachedImage("icons/obj16/info.png");

    // small icons for ControlDecoration, as overlay, etc
    public static final Image ICON_ERROR_SMALL = CommonPlugin.getCachedImage("icons/ovr16/error.gif");
    public static final Image ICON_ERROR_SMALL_ROUND = CommonPlugin.getCachedImage("icons/ovr16/error_round.gif");
    public static final Image ICON_ERROR_SMALL_SQUARE = CommonPlugin.getCachedImage("icons/ovr16/error_square.gif"); // for
    public static final Image ICON_WARNING_SMALL = CommonPlugin.getCachedImage("icons/ovr16/warning.gif");
    public static final Image ICON_INFO_SMALL = CommonPlugin.getCachedImage("icons/ovr16/info.gif");


    public static IDialogSettings getDialogSettings(AbstractUIPlugin plugin, String name) {
        IDialogSettings pluginDialogSettings = plugin.getDialogSettings();
        IDialogSettings dialogSettings = pluginDialogSettings.getSection(name);

        if (dialogSettings == null)
            return pluginDialogSettings.addNewSection(name);
        else
            return dialogSettings;
    }

    public static void updateProblemDecoration(ControlDecoration decoration, int severity, String text) {
        decoration.setImage(severity==IMarker.SEVERITY_ERROR ? ICON_ERROR_SMALL :
            severity==IMarker.SEVERITY_WARNING ? ICON_WARNING_SMALL : ICON_INFO_SMALL);
        decoration.setDescriptionText(text);

        // setDescriptionText(null) does not seem to be enough to remove the image and the hover, so apply more pressure...
        if (text != null)
            decoration.show();
        else {
            decoration.hide();
            decoration.hideHover();
        }
    }

    public static void dumpWidgetHierarchy(Control control) {
        dumpWidgetHierarchy(control, 0);
    }

    protected static void dumpWidgetHierarchy(Control control, int level) {
        System.out.println(StringUtils.repeat("  ", level) + control.toString() + (!control.isVisible() ? " (not visible)" : "") +
                " " + control.getLayoutData() + " " + (control instanceof Composite ? ((Composite)control).getLayout() : ""));

        if (control instanceof Composite)
            for (Control child : ((Composite)control).getChildren())
                dumpWidgetHierarchy(child, level+1);
    }




}
