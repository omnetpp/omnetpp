/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.analysismodel;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialogWithToggle;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.commands.SetChartPropertyCommand;

public class AddVectorOperationAction extends AbstractScaveAction {
    private String label;
    private String code;
    private boolean offerEditing;

    public AddVectorOperationAction(String label, String code, boolean offerEditing) {
        this.label = label;
        this.code = code;
        this.offerEditing = offerEditing;

        setText(label);

        setDescription("Add '" + label + "' operation to vector");
        setImageDescriptor(ScavePlugin.getImageDescriptor(
                code.startsWith("compute:") ? ScaveImages.IMG_ETOOL16_COMPUTE : ScaveImages.IMG_ETOOL16_APPLY));
    }


    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        Assert.isTrue(isEnabled());

        ChartScriptEditor scriptEditor = scaveEditor.getActiveChartScriptEditor();

        Chart chart = scriptEditor.getChart();

        String operation = this.code;
        if (offerEditing) {
            var dialog = new InputDialog(Display.getCurrent().getActiveShell(), "Add Vector Operation", "Edit parameters:", operation, null);
            if (dialog.open() != Dialog.OK)
                return; // cancelled
            operation = dialog.getValue();
        }

        String operations = StringUtils.nullToEmpty(chart.getPropertyValue("vector_operations"));

        String append = operation;

        if (operations.isEmpty())
            operations = append;
        else
            operations += "\n" + append;

        scriptEditor.getCommandStack().execute(new SetChartPropertyCommand(chart, "vector_operations", operations));

        scriptEditor.refreshChart();

        openHintDialog();
    }

    protected void openHintDialog() {
        IPreferenceStore preferences = ScavePlugin.getDefault().getPreferenceStore();
        String preferenceKey = getClass().getSimpleName()+".info";
        String preferenceSetting = preferences.getString(preferenceKey);
        if (!MessageDialogWithToggle.ALWAYS.equals(preferenceSetting)) {
            MessageDialogWithToggle.openInformation(
                    Display.getCurrent().getActiveShell(),
                    "Vector Operation Added",
                    "Operation \"" + label + "\" added.\n\n" +
                    "Note: Vector operations can be reviewed and edited on Input tab of the Configure Chart dialog.",
                    "Do not show this message again", false,
                    preferences, preferenceKey);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        if (editor.getActiveChartScriptEditor() == null)
            return false;

        Chart chart = editor.getActiveChartScriptEditor().getChart();
        Property operationsProperty = chart.getProperty("vector_operations");

        if (operationsProperty != null)
            return true;

        ChartTemplate template = editor.getChartTemplateRegistry().findTemplateByID(chart.getTemplateID());
        if (template == null)
            return false;

        return template.getPropertyNames().contains("vector_operations");
    }
}
