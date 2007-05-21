package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Opens the Chart object on the Datasets page for a chart.
 */
public class GotoChartDefinitionAction extends AbstractScaveAction {
	public GotoChartDefinitionAction() {
		setText("Go to Definition");
		setToolTipText("Go to Definition");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		Chart chart = getChart(editor, selection);
		if (chart == null || isTemporaryChart(chart, editor))
			return;

		
		editor.showDatasetsPage();
		editor.setSelection(new StructuredSelection(chart));
	}

	protected Chart getChart(ScaveEditor editor, IStructuredSelection selection) {
		if (editor.getActiveEditorPage() instanceof ChartPage)
			return ((ChartPage)editor.getActiveEditorPage()).getChart();
		else if (selection.getFirstElement() instanceof Chart)
			return (Chart) selection.getFirstElement();
		else
			return null;
	}

	//XXX duplicate of method in CreateChartTemplateAction
	private static boolean isTemporaryChart(Chart chart, ScaveEditor editor) {
		return ScaveModelUtil.findEnclosingOrSelf(chart, Analysis.class) == editor.getTempAnalysis();
	}
	
	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		Chart chart = getChart(editor, selection);
		return chart != null && !isTemporaryChart(chart, editor);
	}
}
