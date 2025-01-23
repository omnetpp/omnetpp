/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import java.util.Arrays;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.Debug;

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
            dialogSettings = pluginDialogSettings.addNewSection(name);
        return dialogSettings;
    }

    public static void saveSashWeights(SashForm sash, IDialogSettings settings, String sashName) {
        String value = "";
        for (int weight : sash.getWeights())
            value += Integer.toString(weight) + " ";
        settings.put(sashName + ".weights", value.trim());
    }

    public static void restoreSashWeights(SashForm sash, IDialogSettings settings, String sashName) {
        String[] tokens = StringUtils.nullToEmpty(settings.get(sashName + ".weights")).split(" ");
        if (tokens.length > 0 && tokens.length == sash.getChildren().length) {
            try {
                int[] weights = new int[tokens.length];
                for (int i = 0; i < tokens.length; i++)
                    weights[i] = Math.max(1, Integer.parseInt(tokens[i]));
                sash.setWeights(weights);
            }
            catch (NumberFormatException e) {
                // ignore
            }
        }
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

    public static void createContextMenuFor(Control control, boolean removeAllWhenShown, IMenuListener menuListener) {
        MenuManager contextMenu = new MenuManager("#PopUp");
        contextMenu.setRemoveAllWhenShown(removeAllWhenShown);
        contextMenu.addMenuListener(menuListener);
        Menu menu = contextMenu.createContextMenu(control);
        control.setMenu(menu);
        // Note: don't register the context menu with the editor site, or "Run As", "Debug As", "Team", and other irrelevant menu items appear...
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

    public static void dumpLayout(Control control) {
        dumpLayout(control, 0);
    }

    protected static void dumpLayout(Control control, int level) {
        String info = "bounds=" + control.getBounds();
        if (control instanceof Composite)
            info += " clientArea=" + ((Composite)control).getClientArea();
        System.out.println(StringUtils.repeat("  ", level) + control.toString() + (!control.isVisible() ? " (not visible)" : "") + " " + info);

        if (control instanceof Composite)
            for (Control child : ((Composite)control).getChildren())
                dumpLayout(child, level+1);
    }

    /**
     * Recursively finds the first descendent of the given control (including itself)
     * that is an instance of the given class.
     */
    public static <T extends Control> T findWidgetByClass(Control control, Class<T> clazz) {
        if (clazz.isInstance(control))
            return (T)control;
        if (control instanceof Composite)
            for (Control child : ((Composite)control).getChildren()) {
                T found = findWidgetByClass(child, clazz);
                if (found != null)
                    return found;
            }
        return null;
    }

    /**
     * Wraps existing listeners of the widget (control) with code that measures
     * and prints their execution time.
     */
    public static void timeExistingListeners(Widget widget) {
        Object eventTable = ReflectionUtils.getFieldValue(widget, "eventTable");
        Listener[] listeners = (Listener[]) ReflectionUtils.getFieldValue(eventTable, "listeners");
        for (int i=0; i<listeners.length; i++) {
            final Listener origListener = listeners[i];
            listeners[i] = new Listener() {
                @Override
                public void handleEvent(Event event) {
                    Debug.time(event.toString(), 0, () -> origListener.handleEvent(event));
                }
            };
        }
    }

    public static void removeAllListeners(Widget widget) {
        Object eventTable = ReflectionUtils.getFieldValue(widget, "eventTable");
        Listener[] listeners = (Listener[]) ReflectionUtils.getFieldValue(eventTable, "listeners");
        listeners = Arrays.copyOf(listeners, listeners.length);
        int[] types = (int[]) ReflectionUtils.getFieldValue(eventTable, "types");
        types = Arrays.copyOf(types, types.length);
        for (int i=0; i<listeners.length; i++)
            if (listeners[i] != null)
                widget.removeListener(types[i], listeners[i]);
    }

    public static void reopenEditor(IWorkbenchPage page, IEditorInput oldInput, IFileEditorInput newInput, boolean save) {
        IEditorPart oldEditor = page.findEditor(oldInput);
        if (oldEditor != null)
            page.closeEditor(oldEditor, save);

        try {
            IDE.openEditor(page, newInput.getFile());
        }
        catch (PartInitException e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Could not reopen editor for " + newInput.toString());
            CommonPlugin.logError("Could not reopen editor", e);
        }

    }
}
