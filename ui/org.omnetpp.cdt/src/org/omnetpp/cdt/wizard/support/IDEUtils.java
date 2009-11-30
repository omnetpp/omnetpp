package org.omnetpp.cdt.wizard.support;

import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.ui.DetailMessageDialog;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.ned.core.MsgResources;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;


/**
 * Static utility methods for the FreeMarker templates, exposed to the user
 * via BeansWrapper.
 *
 * IMPORTANT: This class must be backward compatible across OMNeT++ versions
 * at all times. DO NOT REMOVE FUNCTIONS OR CHANGE THEIR SIGNATURES; add new methods
 * instead, if needed.
 *
 * @author Andras
 */
public class IDEUtils {

    public static NEDResources getNEDResources() {
        return NEDResourcesPlugin.getNEDResources();
    }

    public static MsgResources getMsgResources() {
        return NEDResourcesPlugin.getMSGResources();
    }

    protected static Shell getShell() {
        return Display.getCurrent().getActiveShell();
    }

    /** 
     * Opens a standard message dialog, with an closable details message.
     */
    public static boolean openConfirm(final String title, final String message, final String detailsMessage) {
        final boolean[] result = new boolean[1];
        DisplayUtils.runNowOrSyncInUIThread(new Runnable() {
            public void run() {
                result[0] = DetailMessageDialog.openConfirm(getShell(), title, detailsMessage);
            }});
        return result[0];
    }

    /** 
     * Opens a standard message dialog, with an closable details message.
     */
    public static boolean openQuestion(final String title, final String message, final String detailsMessage) {
        final boolean[] result = new boolean[1];
        DisplayUtils.runNowOrSyncInUIThread(new Runnable() {
            public void run() {
                result[0] = DetailMessageDialog.openQuestion(getShell(), title, detailsMessage);
            }});
        return result[0];
    }

    /** 
     * Opens a standard message dialog, with an closable details message.
     */
    public static void openError(final String title, final String message, final String detailsMessage) {
        DisplayUtils.runNowOrSyncInUIThread(new Runnable() {
            public void run() {
                DetailMessageDialog.openError(getShell(), title, detailsMessage);
            }});
    }

    /** 
     * Opens a standard message dialog, with an closable details message.
     */
    public static void openWarning(final String title, final String message, final String detailsMessage) {
        DisplayUtils.runNowOrSyncInUIThread(new Runnable() {
            public void run() {
                DetailMessageDialog.openWarning(getShell(), title, detailsMessage);
            }});
    }

    /** 
     * Opens a standard message dialog, with an closable details message.
     */
    public static void openInformation(final String title, final String message, final String detailsMessage) {
        DisplayUtils.runNowOrSyncInUIThread(new Runnable() {
            public void run() {
                DetailMessageDialog.openInformation(getShell(), title, detailsMessage);
            }});
    }

}
