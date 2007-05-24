package org.omnetpp.scave.editors.ui;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.util.ECollections;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.ui.dnd.LocalTransfer;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DragSource;
import org.eclipse.swt.dnd.DragSourceEvent;
import org.eclipse.swt.dnd.DragSourceListener;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.wizard.NewScaveObjectWizard;

/**
 * Toolbar as a palette for creating Scave objects, emulated with buttons on a composite.
 * The user is responsible for setting the layout on the composite (e.g. RowLayout with
 * fill=true).
 * 
 * @author Andras
 */
public class ModelObjectPalette {
	private Color buttonBgColor;
	private boolean showText;
	private ScaveEditor editor;

	/**
	 * Adds palette buttons to the given toolbar, and configures them.
	 */
	public ModelObjectPalette(Composite parent, Color buttonBgColor, boolean showText, ScaveEditor editor, boolean wantDatasetAndChartsheet) {
		this.editor = editor;
		this.buttonBgColor = buttonBgColor;
		this.showText = showText;

		ILabelProvider labelProvider = new AdapterFactoryLabelProvider(editor.getAdapterFactory());
		ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;

		if (wantDatasetAndChartsheet) {
			addToolItem(parent, factory.createDataset(), labelProvider);
			addSeparator(parent);
		}
		addToolItem(parent, factory.createAdd(), labelProvider);
		addToolItem(parent, factory.createDiscard(), labelProvider);
		addToolItem(parent, factory.createApply(), labelProvider);
		addToolItem(parent, factory.createCompute(), labelProvider);
		addToolItem(parent, factory.createGroup(), labelProvider);
		addSeparator(parent);
		addToolItem(parent, factory.createSelect(), labelProvider);
		addToolItem(parent, factory.createDeselect(), labelProvider);
		addToolItem(parent, factory.createExcept(), labelProvider);
		addSeparator(parent);
		addToolItem(parent, factory.createBarChart(), labelProvider);
		addToolItem(parent, factory.createLineChart(), labelProvider);
		addToolItem(parent, factory.createHistogramChart(), labelProvider);
		addToolItem(parent, factory.createScatterChart(), labelProvider);
		if (wantDatasetAndChartsheet) {
			addSeparator(parent);
			addToolItem(parent, factory.createChartSheet(), labelProvider);
		}
	}

		
	protected void addToolItem(Composite parent, final EObject elementPrototype, ILabelProvider labelProvider) {
		//Button toolButton = new Button(parent, SWT.PUSH);
		ToolButton toolButton = new ToolButton(parent, SWT.NONE);
		toolButton.setBackground(buttonBgColor);
		toolButton.setImage(labelProvider.getImage(elementPrototype));
		if (showText)
			toolButton.setText(labelProvider.getText(elementPrototype));
		toolButton.setToolTipText("Click or drag &&& drop to create an object");

		toolButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				addAsChildOrSibling(elementPrototype);
			}
		});

		// Set up drag source; code is based on the following line from AbstractEMFModelEditor:
		// viewer.addDragSupport(dndOperations, transfers, new ViewerDragAdapter(viewer));
		int dndOperations = DND.DROP_COPY | DND.DROP_MOVE | DND.DROP_LINK;
		Transfer[] transferTypes = new Transfer[] { LocalTransfer.getInstance() };
		final DragSource dragSource = new DragSource(toolButton, dndOperations);
		dragSource.setTransfer(transferTypes);

		// modeled after EMF's ViewerDragAdapter
		dragSource.addDragListener(new DragSourceListener() {
			public void dragStart(DragSourceEvent event) {}
			public void dragFinished(DragSourceEvent event) {
				LocalTransfer.getInstance().javaToNative(null, null);
			}

			public void dragSetData(DragSourceEvent event) {
				if (LocalTransfer.getInstance().isSupportedType(event.dataType))
					event.data = new StructuredSelection(elementPrototype);
			}
		});
	
	}

	protected void addSeparator(Composite parent) {
		Control spacer = new Composite(parent, SWT.NONE);
		spacer.setLayoutData(new RowData(0,5));
		spacer.setBackground(parent.getBackground());
	}

	protected void addAsChildOrSibling(EObject elementPrototype) {
		ISelection sel = editor.getSelection();
		
		// choose target: use the selected object, except if element is a Dataset 
		// or ChartSheet which have fixed places in the model
		EObject target = null;
		if (elementPrototype instanceof Dataset)
			target = editor.getAnalysis().getDatasets();
		else if (elementPrototype instanceof ChartSheet)
			target = editor.getAnalysis().getChartSheets();
		else if (sel instanceof IStructuredSelection && ((IStructuredSelection)sel).getFirstElement() instanceof EObject)
			target = (EObject) ((IStructuredSelection) sel).getFirstElement();
		
		//XXX factor out common part
		if (target != null)	{
			// add "element" to "target" as child or as sibling.
			final EObject element = EcoreUtil.copy(elementPrototype);
			Command command = AddCommand.create(editor.getEditingDomain(), target, null, element);
			if (command.canExecute()) {
				// try to add as child
				NewScaveObjectWizard wizard = new NewScaveObjectWizard(editor, target, target.eContents().size(), element);
				WizardDialog dialog = new WizardDialog(editor.getSite().getShell(), wizard);
				if (dialog.open() == Window.OK)
					editor.executeCommand(command);
			}
			else {
				// add as sibling
				EObject parent = target.eContainer();
				int index = ECollections.indexOf(parent.eContents(), target, 0) + 1;  //+1: *after* selected element
				command = AddCommand.create(editor.getEditingDomain(), parent, null, element, index);  

				if (command.canExecute()) {
					NewScaveObjectWizard wizard = new NewScaveObjectWizard(editor, parent, index, element);
					WizardDialog dialog = new WizardDialog(editor.getSite().getShell(), wizard);
					if (dialog.open() == Window.OK)
						editor.executeCommand(command);
				}
			}

			// if it got inserted (has parent now), select it in the viewer.
			// Note: must be in asyncExec(), otherwise setSelection() has no effect on the TreeViewers! (JFace bug?)
			if (element.eContainer() != null) {
				Display.getDefault().asyncExec(new Runnable() {
					public void run() {
						editor.setSelection(new StructuredSelection(element));
					}
				});
			}
		}
	}
}
