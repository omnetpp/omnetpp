package org.omnetpp.scave.views;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.scave.editors.datatable.VectorResultContentProvider;
import org.omnetpp.scave.editors.datatable.VectorResultLabelProvider;
import org.omnetpp.scave.engine.IndexFile;
import org.omnetpp.scave.engine.OutputVectorEntry;
import org.omnetpp.scave.engine.VectorResult;

/**
 * View for vector data.
 *
 * @author tomi
 */
public class VectorBrowserView extends ViewPart {

	Composite panel;
	Table table;
	TableColumn eventNumberColumn;
	Composite messagePanel;
	Label message;
	VirtualTable<OutputVectorEntry> viewer;
	ISelectionListener selectionChangedListener;
	VectorResultContentProvider contentProvider;
	
	GotoAction gotoLineAction;
	GotoAction gotoEventAction;
	GotoAction gotoTimeAction;

	@Override
	public void createPartControl(Composite parent) {
		panel = new Composite(parent, SWT.NONE);
		GridLayout layout = new GridLayout(1, false);
		layout.marginWidth = layout.marginHeight = 0;
		panel.setLayout(layout);
		createMessage(panel);
		createTable(panel);
		createTableViewer(table);
		createPulldownMenu();
		hookSelectionChangedListener();
	}
	
	private void createMessage(Composite parent) {
		messagePanel = new Composite(parent, SWT.NONE);
		GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
		gridData.exclude = true;
		messagePanel.setLayoutData(gridData);
		messagePanel.setLayout(new FillLayout());
		messagePanel.setBackground(ColorFactory.asColor("white"));
		message = new Label(messagePanel, SWT.WRAP);
		message.setBackground(ColorFactory.asColor("white"));
	}

	private void createTable(Composite parent) {
		// create pages
		table = new Table(parent, SWT.VIRTUAL | SWT.MULTI | SWT.FULL_SELECTION);
		table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		table.setHeaderVisible(true);
		table.setLinesVisible(true);
		TableColumn tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(60);
		tableColumn.setText("Item#");
		tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(140);
		tableColumn.setText("Time");
		tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(140);
		tableColumn.setText("Value");
	}

	private void createTableViewer(Table table) {
		contentProvider = new VectorResultContentProvider();
		viewer = new VirtualTable<OutputVectorEntry>(table);
		viewer.setContentProvider(contentProvider);
		viewer.setLabelProvider(new VectorResultLabelProvider());
		setViewerInput(getSite().getPage().getSelection());
	}
	
	private void createPulldownMenu() {
		gotoLineAction = new GotoAction(this, GotoTarget.Line);
		gotoEventAction = new GotoAction(this, GotoTarget.Event);
		gotoTimeAction = new GotoAction(this, GotoTarget.Time);

		IMenuManager manager = getViewSite().getActionBars().getMenuManager();
		manager.add(gotoLineAction);
		manager.add(gotoEventAction);
		manager.add(gotoTimeAction);
	}
	
	public void gotoLine(int lineNumber) {
		OutputVectorEntry entry = contentProvider.getElementBySerial(lineNumber);
		if (entry != null)
			viewer.gotoElement(entry);
	}
	
	public void gotoEvent(int eventNumber) {
		OutputVectorEntry entry = contentProvider.getElementByEventNumber(eventNumber, true);
		if (entry != null)
			viewer.gotoElement(entry);
	}
	
	public void gotoTime(double time) {
		OutputVectorEntry entry = contentProvider.getElementBySimulationTime(time, true);
		if (entry != null)
			viewer.gotoElement(entry);
	}
	
	
	@Override
	public void dispose() {
		unhookSelectionChangedListener();
		super.dispose();
	}
	
	private void hookSelectionChangedListener() {
		selectionChangedListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				setViewerInput(selection);
			}
		};
		getSite().getPage().addPostSelectionListener(selectionChangedListener);
	}
	
	private void unhookSelectionChangedListener() {
		if (selectionChangedListener != null)
			getSite().getPage().removePostSelectionListener(selectionChangedListener);
	}
	
	@Override
	public void setFocus() {
		table.setFocus();
	}

	public void setViewerInput(ISelection selection) {
		VectorResult selectedVector = null;
		
		if (selection instanceof IStructuredSelection) {
			IStructuredSelection structuredSelection = (IStructuredSelection)selection;
			if (structuredSelection.size() == 1) {
				Object selectedObject = structuredSelection.getFirstElement();
				if (selectedObject instanceof VectorResult) {
					selectedVector = (VectorResult)selectedObject;
				}
				else if (selectedObject instanceof IAdaptable) {
					selectedVector = (VectorResult)((IAdaptable)selectedObject).getAdapter(VectorResult.class);
				}
			}
		}
		
		if (selectedVector != viewer.getInput()) {
			viewer.setInput(selectedVector);
			// show message instead of empty table, when no index file
			checkInput(selectedVector);
			if (selectedVector != null) {
				boolean hasEventNumbers = selectedVector.getColumns().indexOf('E') >= 0;
				gotoEventAction.setEnabled(hasEventNumbers);
				setEventNumberColumnVisible(hasEventNumbers);
			}
		}
	}
	
	/**
	 * Checks if the input is a vector whose file has an index.
	 * If not then the content of the vector can not be displayed
	 * and a message is shown.
	 */
	private void checkInput(VectorResult input) {
		if (input != null) {
			String file = input.getFileRun().getFile().getFileSystemFilePath();
			if (IndexFile.isIndexFileUpToDate(file)) {
				setVisible(messagePanel, false);
				setVisible(table, true);
				if (eventNumberColumn != null && input.getColumns().indexOf('E') < 0) {
					eventNumberColumn.dispose();
					eventNumberColumn = null;
				}
				else if (eventNumberColumn == null && input.getColumns().indexOf('E') >= 0) {
					eventNumberColumn = new TableColumn(table, SWT.NONE);
					eventNumberColumn.setWidth(60);
					eventNumberColumn.setText("Event#");
					table.setColumnOrder(new int[] {0,3,1,2});
				}
			} else {
				message.setText("The vector content can not be browsed," +
						" because the index for \""+file+"\" is not up to date.");
				setVisible(messagePanel, true);
				setVisible(table, false);
				
			}
		}
	}
	
	private void setVisible(Control control, boolean visible) {
		GridData gridData = (GridData)control.getLayoutData();
		gridData.exclude = !visible;
		control.setVisible(visible);
		panel.layout(true, true);
	}
	
	// Order: Item#, Event#, Time, Value
	private static final int[] ColumnOrder = new int[] {0,3,1,2};
	
	private void setEventNumberColumnVisible(boolean visible) {
		if (eventNumberColumn != null && !visible) {
			eventNumberColumn.dispose();
			eventNumberColumn = null;
		}
		else if (eventNumberColumn == null && visible) {
			eventNumberColumn = new TableColumn(table, SWT.NONE);
			eventNumberColumn.setWidth(60);
			eventNumberColumn.setText("Event#");
			table.setColumnOrder(ColumnOrder);
		}
	}
	
	
	enum GotoTarget {
		Line,
		Event,
		Time,
	}

	static class GotoAction extends Action
	{
		VectorBrowserView view;
		GotoTarget target;
		String prompt;
		
		public GotoAction(VectorBrowserView view, GotoTarget target) {
			this.view = view;
			this.target = target;
			switch (target) {
			case Line: setText("Go to line..."); prompt = "Line number:"; break;
			case Event: setText("Go to event..."); prompt = "Event number:"; break;
			case Time: setText("Go to time..."); prompt = "Time:"; break;
			}
		}
		
		public void run() {
			IInputValidator validator = new IInputValidator() {
				public String isValid(String text) {
					if (text == null || text.length() == 0)
						return " ";
					else if (parseTarget(text) == null)
						return "Enter a" + (target == GotoTarget.Time ? "" : "n integer") + " number";
					else
						return null; // ok
				}
			};
			InputDialog dialog = new InputDialog(view.getSite().getShell(), "Go to", prompt, "", validator);
			if (dialog.open() == Window.OK) {
				Number targetAddr = parseTarget(dialog.getValue());
				switch (target) {
				case Line: view.gotoLine((Integer)targetAddr); break;
				case Event: view.gotoEvent((Integer)targetAddr); break;
				case Time: view.gotoTime((Double)targetAddr); break;
				}
			}
		}
		
		public Number parseTarget(String str) {
			try
			{
				if (target == GotoTarget.Time)
					return Double.parseDouble(str);
				else
					return Integer.parseInt(str);
			} catch (NumberFormatException e) {
				return null;
			}
		}
	}
}
