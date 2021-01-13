/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.analysismodel;

import java.util.Map;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.actions.ResetChartToTemplateDialog;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.FormEditorPage;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model.commands.AdjustChartPropertiesCommand;
import org.omnetpp.scave.model.commands.CompoundCommand;
import org.omnetpp.scave.model.commands.SetChartDialogPagesCommand;
import org.omnetpp.scave.model.commands.SetChartPropertiesCommand;
import org.omnetpp.scave.model.commands.SetChartScriptCommand;
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
            ResetChartToTemplateDialog dialog = new ResetChartToTemplateDialog(Display.getCurrent().getActiveShell(), "Reset Chart To Template");
            if (dialog.open() == Dialog.OK) {
                ResetChartToTemplateDialog.Options result = dialog.getResult();
                CompoundCommand command = new CompoundCommand("Reset Chart To Template");

                if (result.resetChartScript)
                    command.append(new SetChartScriptCommand(chart, template.getPythonScript()));
                if (result.resetDialogPages) {
                    command.append(new SetChartDialogPagesCommand(chart, template.getDialogPages()));
                    if (result.resetProperties)
                        command.append(new SetChartPropertiesCommand(chart, template.getProperties()));
                    else
                        command.append(new AdjustChartPropertiesCommand(chart, template.getProperties()));
                }
                else if (result.resetProperties) {
                    // IMPORTANT: We must not modify the set of property names already in the chart (as they must stay
                    // consistent with the dialog pages), so we only overwrite the values of those properties which are
                    // also present in the chart template.
                    Map<String,String> properties = chart.getPropertiesAsMap();
                    Map<String,String> templateProperties = template.getProperties();
                    for (String name : templateProperties.keySet())
                        if (properties.containsKey(name))
                            properties.put(name, templateProperties.get(name)); // overwrite value from the template
                    command.append(new SetChartPropertiesCommand(chart, properties));
                }

                scaveEditor.getActiveCommandStack().execute(command);
            }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return ScaveModelUtil.getChartFromSingleSelection(selection) != null;
    }
}
