package org.omnetpp.scave.editors.ui;

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
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
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
		String className = elementPrototype.eClass().getName();
		String hint = "Click or drag &&& drop to create " + StringUtils.indefiniteArticle(className) + " '" + className + "' object.";
		String desc = StringUtils.breakLines(getDescription(elementPrototype.eClass()),60);
		toolButton.setToolTipText(hint + "\n" + desc);

		toolButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				addAsChildOrSibling(elementPrototype);
			}
		});

		// Set up drag source; code is based on the following line from AbstractEMFModelEditor:
		// viewer.addDragSupport(dndOperations, transfers, new ViewerDragAdapter(viewer));
//		int dndOperations = DND.DROP_COPY | DND.DROP_MOVE | DND.DROP_LINK;
//		Transfer[] transferTypes = new Transfer[] { LocalTransfer.getInstance() };
//		final DragSource dragSource = new DragSource(toolButton, dndOperations);
//		dragSource.setTransfer(transferTypes);
//
//		// modeled after EMF's ViewerDragAdapter
//		dragSource.addDragListener(new DragSourceListener() {
//			public void dragStart(DragSourceEvent event) {}
//			public void dragFinished(DragSourceEvent event) {
//				LocalTransfer.getInstance().javaToNative(null, null);
//			}
//
//			public void dragSetData(DragSourceEvent event) {
//				if (LocalTransfer.getInstance().isSupportedType(event.dataType))
//					event.data = new StructuredSelection(EcoreUtil.copy(elementPrototype));
//			}
//		});

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

	public static String getDescription(EClass c) {
		ScaveModelPackage e = ScaveModelPackage.eINSTANCE;
		if (c == e.getAnalysis()) 
			return "";
		
		if (c == e.getInputs()) 
			return "Contains the input files of the analysis.";
		if (c == e.getInputFile()) 
			return "Specifies a result file (output vector or scalar file), or a sets of files using wildcards, to be used as input for the analysis.";

		if (c == e.getDatasets()) 
			return "The Datasets object is a container for all datasets.";
		if (c == e.getDataset()) 
			return "In a Dataset you can choose a subset of input data, apply various processing steps, and create charts from them. " +
					"Operations within the dataset are evaluated one after another, like a program.";
		if (c == e.getAdd()) 
			return "Add operations add more scalars, vectors or histograms to the dataset.";
		if (c == e.getDiscard()) 
			return "Discard operations remove (a subset of) previously added scalars, vectors or histograms from the dataset.";
		if (c == e.getApply()) 
			return "An Apply operation replaces (a subset of the) data in the dataset with the result of some transformation. " +
					"Currently all operations operate on vector data only."; //XXX implement scalar operations
		if (c == e.getCompute()) 
			return "A Compute operation performs a calculation on (a subset of the) data in the dataset, and adds the result to the dataset. " +
					"Currently all operations operate on vector data only."; //XXX implement scalar operations
		if (c == e.getGroup()) 
			return "A Group object creates a local copy of the dataset (conceptually), and lets you apply various processing steps without affecting the dataset's main flow.";

		if (c == e.getLineChart()) 
			return "A LineChart object creates a line chart from (a subset of the) the vector results in the dataset.";
		if (c == e.getBarChart()) 
			return "A BarChart object creates a bar chart from (a subset of the) the scalar results in the dataset.";
		if (c == e.getHistogramChart()) 
			return "A HistogramChart object creates a histogram chart from (a subset of the) the histogram results in the dataset.";
		if (c == e.getScatterChart()) 
			return "A ScatterChart object interprets scalars generated by different runs " +
					"as (x,y) coordinates, and plots them on a line chart.";

		if (c == e.getSelect()) 
			return "A Select object refines the scope of a processing operation (Apply, Compute) or a chart. " + 
					"It can only occur within those object types.";
		if (c == e.getDeselect()) 
			return "A Deselect object refines the scope of a processing operation (Apply, Compute) or a chart. " + 
					"It may only occur within those object types.";
		if (c == e.getExcept()) 
			return "An Except object further refines a Select or Deselect object, and may only occur within those object types.";

		if (c == e.getParam()) 
			return "";
		if (c == e.getProperty()) 
			return "";

		if (c == e.getChartSheets()) 
			return "";
		if (c == e.getChartSheet()) 
			return "ChartSheet objects can be used to organize charts (themselves defined within Datasets) into groups.";
		return null;
	}

}
