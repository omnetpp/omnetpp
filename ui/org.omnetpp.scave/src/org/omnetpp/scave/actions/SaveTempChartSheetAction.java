/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartSheetPage;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * Saves a temporary chart sheet.
 *
 * @author andras
 */
public class SaveTempChartSheetAction extends AbstractScaveAction {

    public SaveTempChartSheetAction() {
        setText("Save Chart Sheet");
        setDescription("Save temporary chart sheet to Charts page");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_ETOOL_SAVE_EDIT));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        ChartSheet chartsheet = getActiveTemporaryChartSheet(scaveEditor);
        if (chartsheet != null) {
            chartsheet.setTemporary(false);
            Command command = AddCommand.create(scaveEditor.getEditingDomain(), scaveEditor.getAnalysis().getCharts(), ScaveModelPackage.eINSTANCE.getCharts_Items(), chartsheet, -1);
            scaveEditor.executeCommand(command);
            scaveEditor.showAnalysisItem(chartsheet);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return getActiveTemporaryChartSheet(editor) != null;
    }

    private ChartSheet getActiveTemporaryChartSheet(ScaveEditor editor) {
        ScaveEditorPage page = editor.getActiveEditorPage();
        if (page != null && page instanceof ChartSheetPage) {
            ChartSheet chartsheet = ((ChartSheetPage)page).getChartSheet();
            if (chartsheet != null && chartsheet.isTemporary())
                return chartsheet;
        }
        return null;
    }
}
