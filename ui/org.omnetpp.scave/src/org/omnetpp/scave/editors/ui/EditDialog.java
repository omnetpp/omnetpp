/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.util.StatusUtil;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.forms.ChartEditForm;
import org.omnetpp.scave.editors.forms.BaseScaveObjectEditForm;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.commands.AddChartPropertyCommand;
import org.omnetpp.scave.model.commands.CompoundCommand;
import org.omnetpp.scave.model.commands.RemoveChartPropertyCommand;
import org.omnetpp.scave.model.commands.SetChartNameCommand;
import org.omnetpp.scave.model.commands.SetChartPropertyCommand;

/**
 * This is the edit dialog for scave model objects.
 *
 * It receives an object and optionally a set of features to be edited
 * (defaults to all editable features).
 * It replies with the changed values.
 *
 * @author tomi
 */
public class EditDialog extends TitleAreaDialog {

    private ScaveEditor editor;
    private ModelObject object;
    private ChartEditForm form;
    private Object[] values;


    /**
     * Creates the dialog. The form in the dialog will be chosen based on the object type.
     * The form can be customized via the formParameters, like which page of the
     * chart dialog should be displayed by default.
     *
     * @param parentShell    the parent shell
     * @param object         object to be edited
     * @param editor         the editor
     * @param formParameters key-value pairs understood by the form; may be null
     */
    public EditDialog(Shell parentShell, ModelObject object, ScaveEditor editor, Map<String,Object> formParameters) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.RESIZE);
        this.editor = editor;
        this.object = object;
        this.form = new ChartEditForm((Chart)object, formParameters, editor.getResultFileManager());

        this.form.addChangeListener(new BaseScaveObjectEditForm.Listener() {
            public void editFormChanged(BaseScaveObjectEditForm form) {
                updateButtonsAndErrorMessage();
            }});
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
    }

    public Object getValue(int index) {
        return values[index];
    }

    @Override
    protected void configureShell(Shell newShell) {
        super.configureShell(newShell);
        newShell.setText("Edit " + object.toString());
    }

    @Override
    protected void buttonPressed(int buttonId) {
        if (IDialogConstants.APPLY_ID == buttonId)
            applyPressed();
        else
            super.buttonPressed(buttonId);
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.APPLY_ID, IDialogConstants.APPLY_LABEL, false);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite composite = (Composite)super.createDialogArea(parent);

        Composite panel = new Composite(composite, SWT.NONE);
        panel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        setTitle(form.getTitle());
        setMessage(form.getDescription());
        form.populatePanel(panel);
        // TODO
//        features = form.getFeatures();
//        for (int i = 0; i < features.length; ++i)
//            form.setValue(features[i], object.eGet(features[i]));
        return composite;
    }

    @Override
    protected void okPressed() {
        applyChanges();
        super.okPressed();
    }

    protected void applyPressed() {
        applyChanges();
    }

    protected void updateButtonsAndErrorMessage() {
        IStatus status = form.validate();
        boolean enabled = !status.matches(IStatus.ERROR);
        Button okButton = getButton(IDialogConstants.OK_ID);
        if (okButton != null)
            okButton.setEnabled(enabled);
        Button applyButton = getButton(IDialogConstants.APPLY_ID);
        if (applyButton != null)
            applyButton.setEnabled(enabled);
        setErrorMessage(status);
    }

    private void setErrorMessage(IStatus status) {
        String message = null;
        if (status.matches(IStatus.ERROR)) {
            IStatus error = StatusUtil.getFirstDescendantWithSeverity(status, IStatus.ERROR);
            Assert.isNotNull(error);
            message = status.isMultiStatus() ? status.getMessage() + ": " : "";
            message += error.getMessage();
        }
        else if (status.matches(IStatus.WARNING)) {
            IStatus warning = StatusUtil.getFirstDescendantWithSeverity(status, IStatus.WARNING);
            Assert.isNotNull(warning);
            message = status.isMultiStatus() ? status.getMessage() + ": " : "";
            message += warning.getMessage();
        }
        setErrorMessage(message);
    }

    private void applyChanges() {

        CompoundCommand command = new CompoundCommand("Edit Chart Properties");
        if (form instanceof ChartEditForm) {
            Chart chart = (Chart)object;

            ChartEditForm chartForm = form;
            Map<String, String> props = chartForm.collectProperties();

            for (String k : props.keySet()) {
                if (k.equals(ChartEditForm.CHART_NAME_PROPERTY_KEY))
                    command.append(new SetChartNameCommand(chart, props.get(ChartEditForm.CHART_NAME_PROPERTY_KEY)));
                else {
                    Property prop = chart.lookupProperty(k);
                    String newValue = props.get(k);

                    if (prop == null) {
                        if (newValue != null)
                            command.append(new AddChartPropertyCommand(chart, new Property(k, newValue)));
                    }
                    else {
                        if (newValue != null) {
                            if (!prop.getValue().equals(newValue))
                                command.append(new SetChartPropertyCommand(prop, newValue));
                        }
                        else
                            command.append(new RemoveChartPropertyCommand(chart, prop));
                    }
                }
            }
        }

        editor.executeCommand(command);

        FormEditorPage editorPage = editor.getEditorPage((Chart)object);
        if (editorPage instanceof ChartPage)
            ((ChartPage)editorPage).chartScriptEditor.refreshChart();

    }

}
