package org.omnetpp.inifile.editor.form;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;

/**
 * Information dialog opened when the ini file analysis exceeded its timeout value.
 * It allows the user to:
 * <ul>
 *   <li>wait more for the operation (possibly blocking the UI)
 *   <li>cancel the current operation
 *   <li>cancel the current operation and disable the ini file analysis
 * <ul>
 *
 * @author tomi
 */
public class AnalysisTimeoutDialog extends MessageDialog {

    public static final int CANCEL = 0;
    public static final int WAIT = 1;
    public static final int DISABLE_AND_CANCEL = 2;

    Button analysisEnabledButton;

    public AnalysisTimeoutDialog(Shell shell) {
        super(shell, "Ini File Analysis Timed Out", null,
                "Analyzing the ini file is taking too long. Do you want to wait until it completes?",
                MessageDialog.INFORMATION,
                new String[] {"Cancel","Wait"}, 1);
    }

    @Override
    protected Control createCustomArea(Composite parent) {
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new RowLayout(SWT.VERTICAL));
        Label label = new Label(panel, SWT.WRAP);
        label.setText("Turn off the analysis if you do not want to see this dialog.\n" +
                      "You can turn it back on from the context menu.");
        analysisEnabledButton = new Button(panel, SWT.CHECK);
        analysisEnabledButton.setText("Analysis");
        analysisEnabledButton.setSelection(true);
        return panel;
    }

    @Override
    protected void setReturnCode(int code) {
        if (code == CANCEL && !analysisEnabledButton.getSelection())
            code = DISABLE_AND_CANCEL;
        super.setReturnCode(code);
    }
}
