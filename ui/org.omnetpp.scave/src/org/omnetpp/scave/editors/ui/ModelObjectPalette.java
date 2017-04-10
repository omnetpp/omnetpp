/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.concurrent.Callable;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.util.ECollections;
import org.eclipse.emf.ecore.EClass;
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
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;
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
    public ModelObjectPalette(Composite parent, Color buttonBgColor, boolean showText, ScaveEditor editor) {
        this.editor = editor;
        this.buttonBgColor = buttonBgColor;
        this.showText = showText;

        ILabelProvider labelProvider = new AdapterFactoryLabelProvider(editor.getAdapterFactory());
        ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;

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
        String className = elementPrototype.eClass().getName();
        String hint = "Click or drag and drop to create " + StringUtils.indefiniteArticle(className,false) + " '" + className + "' object.";
        String desc = StringUtils.breakLines(getDescription(elementPrototype.eClass()),60);
        toolButton.setToolTipText(hint + "\n" + desc);

        toolButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                ResultFileManager.callWithReadLock(editor.getResultFileManager(), new Callable<Object>() {
                    @Override
                    public Object call() throws Exception {
                        addAsChildOrSibling(elementPrototype);
                        return null;
                    }
                });
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
            @Override
            public void dragStart(DragSourceEvent event) {}
            @Override
            public void dragFinished(DragSourceEvent event) {
                LocalTransfer.getInstance().javaToNative(null, null);
            }

            @Override
            public void dragSetData(DragSourceEvent event) {
                if (LocalTransfer.getInstance().isSupportedType(event.dataType))
                    event.data = new StructuredSelection(EcoreUtil.copy(elementPrototype));
            }
        });

    }

    protected void addSeparator(Composite parent) {
        Control spacer = new Composite(parent, SWT.NONE);
        spacer.setLayoutData(new RowData(0,5));
        spacer.setBackground(parent.getBackground());
    }

    protected void addAsChildOrSibling(EObject elementPrototype) {
    	EObject target = editor.getAnalysis().getCharts();
//XXX    	
//        ISelection sel = editor.getSelection();
//
//        // choose target: use the selected object, except if element is a Dataset
//        // or ChartSheet which have fixed places in the model
//        EObject target = null;
//        if (sel instanceof IStructuredSelection && ((IStructuredSelection)sel).getFirstElement() instanceof EObject)
//            target = (EObject) ((IStructuredSelection) sel).getFirstElement();

        //XXX factor out common part
        if (target != null) {
            // add "element" to "target" as child or as sibling.
            final EObject element = EcoreUtil.copy(elementPrototype);
            Command command = AddCommand.create(editor.getEditingDomain(), target, null, element);
            NewScaveObjectWizard wizard = null;
            if (command.canExecute()) {
                // try to add as child
                wizard = new NewScaveObjectWizard(editor, target, target.eContents().size(), element);
            }
            else {
                // add as sibling
                EObject parent = target.eContainer();
                int index = ECollections.indexOf(parent.eContents(), target, 0) + 1;  //+1: *after* selected element
                command = AddCommand.create(editor.getEditingDomain(), parent, null, element, index);
                if (command.canExecute()) {
                    wizard = new NewScaveObjectWizard(editor, parent, index, element);
                }
            }

            if (wizard != null) {
                WizardDialog dialog = new WizardDialog(editor.getSite().getShell(), wizard) {
                    @Override
                    protected Point getInitialSize() {
                        return getShell().computeSize(800, SWT.DEFAULT, true);
                    }
                };
                if (dialog.open() == Window.OK)
                    editor.executeCommand(command);
            }

            // if it got inserted (has parent now), select it in the viewer.
            // Note: must be in asyncExec(), otherwise setSelection() has no effect on the TreeViewers! (JFace bug?)
            if (element.eContainer() != null) {
                Display.getDefault().asyncExec(new Runnable() {
                    @Override
                    public void run() {
                        editor.setSelection(new StructuredSelection(element));
                    }
                });
            }
        }
    }

    public static String getDescription(EClass c) {
        ScaveModelPackage e = ScaveModelPackage.eINSTANCE;
        if (c == e.getAnalysis())
            return "";
        if (c == e.getInputs())
            return "Contains the input files of the analysis.";
        if (c == e.getInputFile())
            return "Specifies a result file (output vector or scalar file), or a sets of files using wildcards, to be used as input for the analysis.";
        if (c == e.getLineChart())
            return "A LineChart object creates a line chart from (a subset of the) the vector results in the dataset.";
        if (c == e.getBarChart())
            return "A BarChart object creates a bar chart from (a subset of the) the scalar results in the dataset.";
        if (c == e.getHistogramChart())
            return "A HistogramChart object creates a histogram chart from (a subset of the) the histogram results in the dataset.";
        if (c == e.getScatterChart())
            return "A ScatterChart object interprets scalars generated by different runs " +
                    "as (x,y) coordinates, and plots them on a line chart.";
        if (c == e.getProperty())
            return "";
        return null;
    }

}
