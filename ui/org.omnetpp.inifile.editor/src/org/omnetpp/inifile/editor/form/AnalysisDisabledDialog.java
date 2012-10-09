package org.omnetpp.inifile.editor.form;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Shell;

/**
 * Information dialog opened when the ini file analysis is disabled and some operation
 * can not continue without analyzing the ini file.
 * It allows the user to turn on the analysis now.
 *
 * @author tomi
 */
public class AnalysisDisabledDialog {
    /**
     * Opens the dialog and returns true if the user wants to enable the ini file analysis.
     */
    public static boolean openDialog(Shell shell) {
        MessageDialog dialog = new MessageDialog(shell, "Ini File Analysis Disabled",
                null, "Ini file analysis is currently disabled. Missing keys can not be identified " +
                      "without analyzing the ini file. Do you want to enable analysis?",
                MessageDialog.INFORMATION, new String[] {"Cancel", "Enable"}, 1);
        return dialog.open() == 1;
    }
}
