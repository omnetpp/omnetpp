/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelFactory;

/**
 * Toolbar as a palette for creating Scave objects.
 * Currently not used.
 *
 * @author Andras
 */
public class ModelObjectPalette1 {
    // config:
    private ScaveEditor editor;
    private HashMap<ToolItem, EObject> toolItems = new HashMap<ToolItem, EObject>();

    // state:
    private Object objectToDrag = null;

    /**
     * Adds palette buttons to the given toolbar, and configures them.
     */
    public ModelObjectPalette1(ToolBar toolbar, ScaveEditor editor, boolean showText) {
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
                LocalTransfer.getInstance().javaToNative(null, null);
            }

            public void dragSetData(DragSourceEvent event) {
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
            }
        });

        ILabelProvider labelProvider = new AdapterFactoryLabelProvider(editor.getAdapterFactory());
        ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;

        addToolItem(toolbar, factory.createDataset(), labelProvider, showText);
        addToolItem(toolbar, factory.createAdd(), labelProvider, showText);
        addToolItem(toolbar, factory.createDiscard(), labelProvider, showText);
        addToolItem(toolbar, factory.createSelect(), labelProvider, showText);
        addToolItem(toolbar, factory.createDeselect(), labelProvider, showText);
        addToolItem(toolbar, factory.createExcept(), labelProvider, showText);
        new ToolItem(toolbar, SWT.SEPARATOR);
        addToolItem(toolbar, factory.createApply(), labelProvider, showText);
        addToolItem(toolbar, factory.createCompute(), labelProvider, showText);
        addToolItem(toolbar, factory.createGroup(), labelProvider, showText);
        new ToolItem(toolbar, SWT.SEPARATOR);
        addToolItem(toolbar, factory.createChartSheet(), labelProvider, showText);
        addToolItem(toolbar, factory.createBarChart(), labelProvider, showText);
        addToolItem(toolbar, factory.createLineChart(), labelProvider, showText);
        addToolItem(toolbar, factory.createHistogramChart(), labelProvider, showText);
        addToolItem(toolbar, factory.createScatterChart(), labelProvider, showText);
    }

    protected ToolItem findToolItemUnderMouse(MouseEvent e) {
        for (ToolItem item : toolItems.keySet())
            if (item.getBounds().contains(e.x, e.y))
                return item;
        return null;
    }

    protected void addToolItem(ToolBar tb, final EObject elementPrototype, ILabelProvider labelProvider, boolean showText) {
        ToolItem toolItem = new ToolItem(tb, SWT.PUSH);
        toolItem.setImage(labelProvider.getImage(elementPrototype));
        if (showText)
            toolItem.setText(labelProvider.getText(elementPrototype));
        toolItem.setToolTipText("Click or drag&&drop to create "+labelProvider.getText(elementPrototype));

        toolItems.put(toolItem, elementPrototype);

        toolItem.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                addAsChildOrSibling(elementPrototype);
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

        if (target != null) {
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
