package org.omnetpp.scave.editors.ui;

import java.util.HashMap;

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
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.ScaveModelFactory;

/**
 * 
 * @author Andras
 */
public class ModelObjectPalette {
	// config:
	private ScaveEditor editor;
	private HashMap<ToolItem, EObject> toolItems = new HashMap<ToolItem, EObject>();
	
	// state:
	private Object objectToDrag = null;

	/**
	 * Adds palette buttons to the given toolbar, and configures them.
	 */
	public ModelObjectPalette(ToolBar toolbar, ScaveEditor editor) {
		this.editor = editor;

		// Set up drag source; code is based on the following line from AbstractEMFModelEditor:
		// viewer.addDragSupport(dndOperations, transfers, new ViewerDragAdapter(viewer));
		int dndOperations = DND.DROP_COPY | DND.DROP_MOVE | DND.DROP_LINK;
		Transfer[] transferTypes = new Transfer[] { LocalTransfer.getInstance() };
		final DragSource dragSource = new DragSource(toolbar, dndOperations);
		dragSource.setTransfer(transferTypes);

		// modeled after EMF's ViewerDragAdapter
		dragSource.addDragListener(new DragSourceListener() {
			public void dragStart(DragSourceEvent event) {}
			public void dragFinished(DragSourceEvent event) {
				System.out.println("dragFinished()");
				LocalTransfer.getInstance().javaToNative(null, null);
			}

			public void dragSetData(DragSourceEvent event) {
				System.out.println("dragSetData()");
				if (LocalTransfer.getInstance().isSupportedType(event.dataType))
					event.data = new StructuredSelection(objectToDrag);
			}
		});

		// When a tool item is clicked, we need to decide on the object we'd drop.
		// (note: the whole toolbar is one Control, tool items are not Controls themselves,
		// so we cannot add mouse listeners to individual tool items)
		toolbar.addMouseListener(new MouseListener() {
			public void mouseDoubleClick(MouseEvent e) {}
			public void mouseUp(MouseEvent e) {}
			public void mouseDown(MouseEvent e) {
				ToolItem toolItem = findToolItemUnderMouse(e);
				objectToDrag = toolItem==null ? null : toolItems.get(toolItem);
				System.out.println("object to drag: "+objectToDrag);
			}
		});
		
		ILabelProvider labelProvider = new AdapterFactoryLabelProvider(editor.getAdapterFactory());
		ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;

		addToolItem(toolbar, factory.createDataset(), labelProvider);
		addToolItem(toolbar, factory.createAdd(), labelProvider);
		addToolItem(toolbar, factory.createDiscard(), labelProvider);
		addToolItem(toolbar, factory.createSelect(), labelProvider);
		addToolItem(toolbar, factory.createDeselect(), labelProvider);
		addToolItem(toolbar, factory.createExcept(), labelProvider);
		new ToolItem(toolbar, SWT.SEPARATOR);
		addToolItem(toolbar, factory.createApply(), labelProvider);
		addToolItem(toolbar, factory.createCompute(), labelProvider);
		addToolItem(toolbar, factory.createGroup(), labelProvider);
		new ToolItem(toolbar, SWT.SEPARATOR);
		addToolItem(toolbar, factory.createChartSheet(), labelProvider);
		addToolItem(toolbar, factory.createBarChart(), labelProvider);
		addToolItem(toolbar, factory.createLineChart(), labelProvider);
		addToolItem(toolbar, factory.createHistogramChart(), labelProvider);
		addToolItem(toolbar, factory.createScatterChart(), labelProvider);
	}

	protected ToolItem findToolItemUnderMouse(MouseEvent e) {
		for (ToolItem item : toolItems.keySet())
			if (item.getBounds().contains(e.x, e.y))
				return item;
		return null;
	}

	protected void addToolItem(ToolBar tb, final EObject elementPrototype, ILabelProvider labelProvider) {
		ToolItem toolItem = new ToolItem(tb, SWT.PUSH);
		toolItem.setImage(labelProvider.getImage(elementPrototype));
		toolItem.setText(labelProvider.getText(elementPrototype));
		toolItem.setToolTipText("Click or drag to create "+labelProvider.getText(elementPrototype));

		toolItems.put(toolItem, elementPrototype);

		toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				addAsChildOrSibling(elementPrototype);
			}
		});
	}

	protected void addAsChildOrSibling(EObject elementProtoType) {
		ISelection sel = editor.getSelection();
		if (sel instanceof IStructuredSelection && ((IStructuredSelection)sel).getFirstElement() instanceof EObject) {
			// add "element" to "target" as child or as sibling.
			EObject target = (EObject) ((IStructuredSelection) sel).getFirstElement();
			EObject element = EcoreUtil.copy(elementProtoType);

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
		}
	}
}
