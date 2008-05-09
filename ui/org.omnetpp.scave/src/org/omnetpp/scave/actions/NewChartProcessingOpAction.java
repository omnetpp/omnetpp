package org.omnetpp.scave.actions;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.util.ECollections;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model2.NonnotifyingReparentCommand;
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
		ProcessingOp element = (ProcessingOp) EcoreUtil.copy(elementPrototype);
		boolean doit = true;
		if (elementPrototype.getOperation() == null) {
			// show dialog
			int index = ECollections.indexOf(parent.eContents(), chart, 0);
			NewScaveObjectWizard wizard = new NewScaveObjectWizard(editor, parent, index, element);
			WizardDialog dialog = new WizardDialog(editor.getSite().getShell(), wizard);
			if (dialog.open() != Window.OK) 
				doit = false;
		}
		if (doit) {
			// just add "element" into "parent", before "chart"
			insertProcessingOp(chart, element, editor);
		}
	}

	private void insertProcessingOp(Chart chart, ProcessingOp element, ScaveEditor editor) {
		EObject parent = chart.eContainer();
		int index = ECollections.indexOf(parent.eContents(), chart, 0);
		EditingDomain editingDomain = editor.getEditingDomain();

		// if chart is already grouped (it is the last item in a group), just add operation before the chart 
		if (parent instanceof Group && index == parent.eContents().size()-1) {
			Command command = AddCommand.create(editingDomain, parent, null, element, index);
			editor.executeCommand(command);
		}
		else {
			// otherwise, create a group for it
			//
			// trick: RemoveCommand(chart) closes the ChartPage as a side effect, so we 
			// have to wrap moving the chart into the Group into a non-notifying CompoundCommand; 
			// at the same time Group's insertion has to be notifying, otherwise viewers won't 
			// update properly.
			Group group = ScaveModelFactory.eINSTANCE.createGroup();
			group.setName(chart.getName());

			CompoundCommand command = new CompoundCommand();
			command.append(AddCommand.create(editingDomain, parent, null, group, index));
			command.append(new NonnotifyingReparentCommand(editingDomain, chart, group, 0));
			command.append(AddCommand.create(editingDomain, group, null, element, 0));

			command.setLabel(command.getCommandList().get(2).getLabel());
			editor.executeCommand(command);
		}
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
		return chart instanceof LineChart || chart instanceof ScatterChart;
	}
}
