package org.omnetpp.common.util;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.ui.plugin.AbstractUIPlugin;

public class UIUtils {
    public static IDialogSettings getDialogSettings(AbstractUIPlugin plugin, String name) {
        IDialogSettings pluginDialogSettings = plugin.getDialogSettings();
        IDialogSettings dialogSettings = pluginDialogSettings.getSection("OpenNEDTypeDialog");

        if (dialogSettings == null)
            return pluginDialogSettings.addNewSection("OpenNEDTypeDialog");
        else
            return dialogSettings; 
    }
}
