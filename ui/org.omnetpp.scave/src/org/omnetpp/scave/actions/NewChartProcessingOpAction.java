package org.omnetpp.scave.actions;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.util.ECollections;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.editors.ui.EditDialog;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;
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
		EObject element = EcoreUtil.copy(elementPrototype);
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
			insertOperation(chart, element, editor);

			// recalculate chart if it's the active page.
			//XXX actually: should be done if it's open, on whichever page; it would be even better 
			// if this updating happened automatically, in response to model changes
			if (editor.getActiveEditorPage() instanceof ChartPage)
				((ChartPage)editor.getActiveEditorPage()).getChartUpdater().updateDataset(); 
		}
	}

	private void insertOperation(EObject chart, EObject element, ScaveEditor editor) {
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
			Group group = ScaveModelFactory.eINSTANCE.createGroup();

			CompoundCommand command = new CompoundCommand();
			command.append(RemoveCommand.create(editingDomain, chart));
			command.append(AddCommand.create(editingDomain, parent, null, group, index));
			command.append(AddCommand.create(editingDomain, group, null, element));
			command.append(AddCommand.create(editingDomain, group, null, chart));
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
		return chart != null && chart instanceof LineChart;
	}
}
