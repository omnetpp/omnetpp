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
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DragSource;
import org.eclipse.swt.dnd.DragSourceEvent;
import org.eclipse.swt.dnd.DragSourceListener;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelFactory;

/**
 * Toolbar as a palette for creating Scave objects, emulated with buttons on a composite.
 * The user is responsible for setting the layout on the composite.
 * 
 * @author Andras
 */
public class ModelObjectPalette2 {
	private Color buttonBgColor;
	private boolean showText;
	private ScaveEditor editor;

	/**
	 * Adds palette buttons to the given toolbar, and configures them.
	 */
	public ModelObjectPalette2(Composite parent, ScaveEditor editor, Color buttonBgColor, boolean showText) {
		this.editor = editor;
		this.buttonBgColor = buttonBgColor;
		this.showText = showText;

		ILabelProvider labelProvider = new AdapterFactoryLabelProvider(editor.getAdapterFactory());
		ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;

		addToolItem(parent, factory.createDataset(), labelProvider);
		addToolItem(parent, factory.createAdd(), labelProvider);
		addToolItem(parent, factory.createDiscard(), labelProvider);
		addToolItem(parent, factory.createSelect(), labelProvider);
		addToolItem(parent, factory.createDeselect(), labelProvider);
		addToolItem(parent, factory.createExcept(), labelProvider);
		new Label(parent, SWT.NONE);
		addToolItem(parent, factory.createApply(), labelProvider);
		addToolItem(parent, factory.createCompute(), labelProvider);
		addToolItem(parent, factory.createGroup(), labelProvider);
		new Label(parent, SWT.NONE);
		addToolItem(parent, factory.createChartSheet(), labelProvider);
		addToolItem(parent, factory.createBarChart(), labelProvider);
		addToolItem(parent, factory.createLineChart(), labelProvider);
		addToolItem(parent, factory.createHistogramChart(), labelProvider);
		addToolItem(parent, factory.createScatterChart(), labelProvider);
	}
		
	protected void addToolItem(Composite parent, final EObject elementPrototype, ILabelProvider labelProvider) {
		//Button toolButton = new Button(parent, SWT.PUSH);
		ToolButton toolButton = new ToolButton(parent, SWT.NONE);
		toolButton.setBackground(buttonBgColor);
		toolButton.setImage(labelProvider.getImage(elementPrototype));
		if (showText)
			toolButton.setText(labelProvider.getText(elementPrototype));
		toolButton.setToolTipText("Click or drag&&drop to create "+labelProvider.getText(elementPrototype));

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

	protected void addAsChildOrSibling(EObject elementProtoType) {
		ISelection sel = editor.getSelection();
		EObject target = null;
		if (elementProtoType instanceof Dataset)
			target = editor.getAnalysis().getDatasets();
		else if (elementProtoType instanceof ChartSheet)
			target = editor.getAnalysis().getChartSheets();
		else if (sel instanceof IStructuredSelection && ((IStructuredSelection)sel).getFirstElement() instanceof EObject)
			target = (EObject) ((IStructuredSelection) sel).getFirstElement();
		
		if (target != null)	{
			// add "element" to "target" as child or as sibling.
			final EObject element = EcoreUtil.copy(elementProtoType);

			Command command = AddCommand.create(editor.getEditingDomain(), target, null, element);
			if (command.canExecute()) {
				// try to add as child
				command.execute();
			}
			else {
				// add as sibling
				int index = ECollections.indexOf(target.eContainer().eContents(), target, 0);
				command = AddCommand.create(editor.getEditingDomain(), target.eContainer(), null, element, index + 1);
				command.execute();
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
