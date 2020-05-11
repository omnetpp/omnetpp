/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.analysismodel;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Saves a temporary chart.
 *
 * @author tomi, andras
 */
//TODO: when creating a "filename(...)" filter in the "Add" node, use relative path if
// possible (if this file and the result file are in the same project)
public class SaveTempChartAction extends AbstractScaveAction {

    public SaveTempChartAction() {
        setText("Save Chart");
        setDescription("Save temporary chart to Charts page");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_ETOOL_SAVE_EDIT)); //TODO
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        Chart chart = getActiveTemporaryChart(scaveEditor);
        if (chart != null) {
            ChartScriptEditor activeChartScriptEditor = scaveEditor.getActiveChartScriptEditor();
            String suggestedName = StringUtils.nullToEmpty(activeChartScriptEditor.getSuggestedChartName());
            InputDialog dialog = new InputDialog(Display.getCurrent().getActiveShell(), "Create Chart", "Enter chart name", suggestedName, null);

            if (dialog.open() == InputDialog.OK) {
                ScaveModelUtil.saveChart(scaveEditor.getChartsPage().getCommandStack(), chart, dialog.getValue(), scaveEditor.getAnalysis());
                activeChartScriptEditor.updateActions();
                scaveEditor.showAnalysisItem(chart);
            }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return getActiveTemporaryChart(editor) != null;
    }

    private Chart getActiveTemporaryChart(ScaveEditor scaveEditor) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        if (editor != null) {
            Chart chart = editor.getChart();
            if (chart != null && chart.isTemporary())
                return chart;
        }
        return null;
    }
}
