/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.ui.SWTFactory;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;

/**
 * Dialog to choose which chart elements to reset to the chart template.
 *
 * @author andras
 */
public class ResetChartToTemplateDialog extends TitleAreaDialog {
    private String title;
    private Button resetPropertiesCheckbox;
    private Button resetDialogPagesCheckbox;
    private Button resetChartScriptCheckbox;

    public static class Options {
        public boolean resetProperties;
        public boolean resetDialogPages;
        public boolean resetChartScript;
    }

    private Options result;

    public ResetChartToTemplateDialog(Shell parentShell, String title) {
        super(parentShell);
        this.title = title;
    }

    @Override
    protected void buttonPressed(int buttonId) {
        if (buttonId == IDialogConstants.OK_ID) {
            result = new Options();
            result.resetProperties = resetPropertiesCheckbox.getSelection();
            result.resetDialogPages = resetDialogPagesCheckbox.getSelection();
            result.resetChartScript = resetChartScriptCheckbox.getSelection();
        }

        getDialogBoundsSettings().put("resetProperties", resetPropertiesCheckbox.getSelection());
        getDialogBoundsSettings().put("resetDialogPages", resetDialogPagesCheckbox.getSelection());
        getDialogBoundsSettings().put("resetChartScript", resetChartScriptCheckbox.getSelection());

        super.buttonPressed(buttonId);
    }

    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        if (title != null)
            shell.setText(title);
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        setTitle("Reset chart to the chart template from which it was created");
        Composite composite = SWTFactory.createComposite(parent, 1, 1, GridData.FILL_BOTH);
        ((GridLayout)composite.getLayout()).marginTop = 10;
        ((GridLayout)composite.getLayout()).marginLeft = 10;

        SWTFactory.createLabel(composite, "Choose which elements of the chart to reset:", 1);
        resetChartScriptCheckbox = SWTFactory.createCheckButton(composite, "Python script", null, true, 1);
        resetDialogPagesCheckbox = SWTFactory.createCheckButton(composite, "Dialog pages", null, true, 1);
        resetPropertiesCheckbox = SWTFactory.createCheckButton(composite, "Properties  (i.e. settings in the Edit Chart dialog)", null, false, 1);
        SWTFactory.setIndent(resetPropertiesCheckbox, 10);
        SWTFactory.setIndent(resetDialogPagesCheckbox, 10);
        SWTFactory.setIndent(resetChartScriptCheckbox, 10);
        applyDialogFont(composite);

        if (getDialogBoundsSettings().get("resetProperties") != null) {
            resetPropertiesCheckbox.setSelection(getDialogBoundsSettings().getBoolean("resetProperties"));
            resetDialogPagesCheckbox.setSelection(getDialogBoundsSettings().getBoolean("resetDialogPages"));
            resetChartScriptCheckbox.setSelection(getDialogBoundsSettings().getBoolean("resetChartScript"));
        }
        return composite;
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
    }

    public Options getResult() {
        return result;
    }
}
