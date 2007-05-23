package org.omnetpp.scave.actions;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.util.ECollections;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.LineChart;

/**
 * Adds an "Apply" or "Compute" node to the chart.
 */
public class AddProcessingToChartAction extends AbstractScaveAction {
	private EObject elementPrototype; 

 	public AddProcessingToChartAction(String text, EObject elementPrototype) {
		setText(text);
		setToolTipText("Adds a(n) '"+elementPrototype.getClass().getSimpleName()+"' node to the chart"); //XXX better label
		this.elementPrototype = elementPrototype;
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		Chart chart = getChart(editor, selection);
		if (chart == null)
			return;

		// add "element" into "parent", just before "chart"
		EObject parent = chart.eContainer();
		EObject element = EcoreUtil.copy(elementPrototype);
		int index = ECollections.indexOf(parent.eContents(), chart, 0);
		Command command = AddCommand.create(editor.getEditingDomain(), parent, null, element, index);
		command.execute();  //XXX undo stack gets messed up???? how???
		
		// recalculate chart if it's the active page.
		//XXX actually: should be done if it's open, on whichever page; it would be even better 
		// if this updating happened automatically, in response to model changes
		if (editor.getActiveEditorPage() instanceof ChartPage)
			((ChartPage)editor.getActiveEditorPage()).getChartUpdater().updateDataset(); 
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
		return chart != null && chart instanceof LineChart;
	}
}
