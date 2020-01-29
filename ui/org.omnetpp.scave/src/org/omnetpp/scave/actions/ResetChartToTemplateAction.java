/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charttemplates.ChartTemplate;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.commands.ResetChartToTemplateCommand;

/**
 * Resets either the dialog pages, or the script of a chart to its template.
 */
public class ResetChartToTemplateAction extends AbstractScaveAction {

    public ResetChartToTemplateAction() {
        setText("Reset Chart to Template");
        setDescription("Resets the script and the dialog of a chart to those of its template");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_ETOOL_CLEAR));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) {
        Chart chart = scaveEditor.getActiveChartScriptEditor().getChart();

        //InputDialog dialog = new InputDialog(Display.getCurrent().getActiveShell(), "Create Chart", "Enter chart name", suggestedName, null);

        //if (dialog.open() == InputDialog.OK) {

        ChartTemplate template = scaveEditor.getChartTemplateRegistry().findTemplateByID(chart.getTemplateID());
        ResetChartToTemplateCommand command = new ResetChartToTemplateCommand(chart, template.getPythonScript(), template.getDialogPages());

        scaveEditor.getActiveCommandStack().execute(command);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return editor.getActiveChartScriptEditor() != null && editor.getActiveChartScriptEditor().getChart() != null;
    }
}
