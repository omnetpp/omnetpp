/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.FormEditorPage;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model.commands.ResetChartToTemplateCommand;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Resets either the dialog pages, or the script of a chart to its template.
 */
public class ResetChartToTemplateAction extends AbstractScaveAction {

    public ResetChartToTemplateAction() {
        setText("Reset to Template");
        setDescription("Resets the script and the dialog of a chart to those of its template");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_ETOOL_CLEAR));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        Chart chart = ScaveModelUtil.getChartFromSingleSelection(selection);

        FormEditorPage editorPage = scaveEditor.getEditorPage(chart);

        if (editorPage != null) {
            boolean result = MessageDialog.openQuestion(scaveEditor.getSite().getShell(), "Chart is open",
                    "The chart you are trying to reset is currently open, but it needs to be closed to reset. Close it now?");

            if (result)
                scaveEditor.closePage(chart);
            else
                return;
        }

        ChartTemplate template = scaveEditor.getChartTemplateRegistry().findTemplateByID(chart.getTemplateID());
        if (template == null) {
            MessageDialog.openInformation(scaveEditor.getSite().getShell(), "Chart template not found",
                    "The template of this chart was not found by its id: \"" + chart.getTemplateID() + "\"");
        }
        else {
            // TODO: dialog to ask for parameters
            ResetChartToTemplateCommand command = new ResetChartToTemplateCommand(chart, template, false);
            scaveEditor.getActiveCommandStack().execute(command);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return ScaveModelUtil.getChartFromSingleSelection(selection) != null;
    }
}
