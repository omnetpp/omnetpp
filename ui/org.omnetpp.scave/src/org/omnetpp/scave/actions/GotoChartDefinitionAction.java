/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Opens the Chart object on the Datasets page for a chart.
 */
public class GotoChartDefinitionAction extends AbstractScaveAction {
    public GotoChartDefinitionAction() {
        setText("Go to Chart Definition");
        setImageDescriptor(ScavePlugin.getImageDescriptor("icons/full/etool16/gotochartdefinition.png"));
    }

    @Override
    protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
        Chart chart = getChart(editor, selection);
        if (chart == null || ScaveModelUtil.isTemporaryChart(chart, editor))
            return;

        editor.showChartsPage();
        editor.setSelection(new StructuredSelection(chart));  //TOOD chartsPage.setSelection() instead!
    }

    protected Chart getChart(ScaveEditor editor, IStructuredSelection selection) {
        if (editor.getActiveEditorPage() instanceof ChartPage)
            return ((ChartPage)editor.getActiveEditorPage()).getChart();
        else if (selection.getFirstElement() instanceof Chart)
            return (Chart) selection.getFirstElement();
        else
            return null;
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        Chart chart = getChart(editor, selection);
        return chart != null && !ScaveModelUtil.isTemporaryChart(chart, editor);
    }
}
