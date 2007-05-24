package org.omnetpp.scave.actions;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.util.ECollections;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.wizard.NewScaveObjectWizard;

/**
 * Adds an "Apply" or "Compute" node to the chart.
 */
public class NewChartProcessingOpAction extends AbstractScaveAction {
	private ProcessingOp elementPrototype; 

	/**
	 * Creates the action. 
	 * @param text the action text
	 * @param elementPrototype  Prototype of object to add. If the "operation" attribute is null, 
	 *                          a dialog will be shown.
	 */
	public NewChartProcessingOpAction(String text, ProcessingOp elementPrototype) {
		setText(text);
		setToolTipText("Adds a(n) '"+elementPrototype.getClass().getSimpleName()+"' node to the chart"); //XXX better label
		this.elementPrototype = elementPrototype;
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		Chart chart = getChart(editor, selection);
		if (chart == null)
			return;

		EObject parent = chart.eContainer();
		int index = ECollections.indexOf(parent.eContents(), chart, 0);
		EObject element = EcoreUtil.copy(elementPrototype);
		if (elementPrototype.getOperation() == null) {
			// show dialog
			NewScaveObjectWizard wizard = new NewScaveObjectWizard(editor, parent, index, element);
			WizardDialog dialog = new WizardDialog(editor.getSite().getShell(), wizard);
			if (dialog.open() == Window.OK) {
				Command command = AddCommand.create(editor.getEditingDomain(), parent, null, element, index);  
				editor.executeCommand(command);
			}
		}
		else {
			// just add "element" into "parent", before "chart"
			Command command = AddCommand.create(editor.getEditingDomain(), parent, null, element, index);
			editor.executeCommand(command);
		}

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
