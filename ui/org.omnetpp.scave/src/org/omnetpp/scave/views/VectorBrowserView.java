package org.omnetpp.scave.views;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.datatable.VectorResultContentProvider;
import org.omnetpp.scave.editors.datatable.VectorResultRowRenderer;
import org.omnetpp.scave.engine.IndexFile;
import org.omnetpp.scave.engine.OutputVectorEntry;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model2.ChartDataPoint;
import org.omnetpp.scave.model2.ComputedResultFileUpdater;
import org.omnetpp.scave.model2.ChartLine;
import org.omnetpp.scave.model2.ComputedResultFileUpdater.CompletionEvent;

/**
 * View for vector data.
 *
 * @author tomi
 */
//XXX if last editor is closed, view still displays the last vector
public class VectorBrowserView extends ViewWithMessagePart {
	public static final String ID = "org.omnetpp.scave.VectorBrowserView";
	
	protected TableColumn eventNumberColumn;
	protected VirtualTable<OutputVectorEntry> viewer;
	protected ISelectionListener selectionChangedListener;
	protected VectorResultContentProvider contentProvider;
	
	protected GotoAction gotoLineAction;
	protected GotoAction gotoEventAction;
	protected GotoAction gotoTimeAction;

	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);
		createPulldownMenu();
		hookSelectionChangedListener();
		setViewerInput(getSite().getPage().getSelection());
	}
	
	@Override
	protected Control createViewControl(Composite parent) {
		contentProvider = new VectorResultContentProvider();
		viewer = new VirtualTable<OutputVectorEntry>(parent, SWT.NONE);
		viewer.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		viewer.setContentProvider(contentProvider);
		viewer.setRowRenderer(new VectorResultRowRenderer());

		TableColumn tableColumn = viewer.createColumn();
		tableColumn.setWidth(60);
		tableColumn.setText("Item#");
		tableColumn = viewer.createColumn();
		tableColumn.setWidth(140);
		tableColumn.setText("Time");
		tableColumn = viewer.createColumn();
		tableColumn.setWidth(140);
		tableColumn.setText("Value");
		return viewer;
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
			viewer.scrollToElement(entry);
	}
	
	public void gotoEvent(int eventNumber) {
		OutputVectorEntry entry = contentProvider.getElementByEventNumber(eventNumber, true);
		if (entry != null)
			viewer.scrollToElement(entry);
	}
	
	public void gotoTime(BigDecimal time) {
		OutputVectorEntry entry = contentProvider.getElementBySimulationTime(time, true);
		if (entry != null)
			viewer.scrollToElement(entry);
	}
	
	public void selectLine(int lineNumber) {
		OutputVectorEntry entry = contentProvider.getElementBySerial(lineNumber);
		if (entry != null) {
			viewer.scrollToElement(entry);
			viewer.setSelectionElement(entry);
		}
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
		viewer.setFocus();
	}

	public void setViewerInput(ISelection selection) {
		VectorResult selectedVector = null;
		ResultFileManager manager = null;
		int dataPointIndex = -1;
		
		if (selection instanceof IDListSelection) {
			// show the first selected vector (XXX merge vectors?)
			IDListSelection idlistSelection = (IDListSelection)selection;
			selectedVector = idlistSelection.getFirstAsVector();
			manager = idlistSelection.getResultFileManager();
		}
		else if (selection instanceof IStructuredSelection) {
			Object selectedObject = ((IStructuredSelection)selection).getFirstElement();
			if (selectedObject instanceof ChartLine) {
				ChartLine selectedLine = (ChartLine)selectedObject;
				if (selectedLine.getResultItem() instanceof VectorResult) {
					selectedVector = (VectorResult)selectedLine.getResultItem();
					manager = selectedLine.getResultFileManager();
					if (selectedObject instanceof ChartDataPoint)
						dataPointIndex = ((ChartDataPoint)selectedObject).getIndex();
				}
			}
		}
		
		if (ObjectUtils.equals(selectedVector, viewer.getInput())) {
			if (dataPointIndex >= 0)
				selectLine(dataPointIndex);
			return;
		}
		
		// recompute computed files if needed
		if (selectedVector != null && manager != null && selectedVector.isComputed()) {
			Object computation = selectedVector.getComputation(); 
			if (computation instanceof ProcessingOp) {
				ProcessingOp operation =  (ProcessingOp)computation;
				VectorResult origInput = (VectorResult)viewer.getInput();
				if (origInput == null || operation != origInput.getComputation() ||
						operation.getComputedFile() == null || !operation.isComputedFileUpToDate()) {
					setViewerInput((VectorResult)null);
					showMessage("Computing vectors...");
					final VectorResult finalVector = selectedVector;
					final int serialToShow = dataPointIndex;
					ComputedResultFileUpdater.instance().ensureComputedFile(operation, manager,
						new ComputedResultFileUpdater.CompletionCallback() {
							public void completed(final CompletionEvent event) {
								if (Display.getCurrent() == null) {
									Display.getDefault().asyncExec(new Runnable() {
										public void run() {
											setViewerInputOrMessage(finalVector, serialToShow, event.getStatus());
										}
									});
								}
								else
									setViewerInputOrMessage(finalVector, serialToShow, event.getStatus());
							}
						});
					return;
				}
			}
			else
				selectedVector = null;
		}
		
		setViewerInput(selectedVector);
		if (dataPointIndex >= 0)
			selectLine(dataPointIndex);
	}
	
	public void setViewerInputOrMessage(VectorResult input, int serial, IStatus status) {
		final int severity = status.getSeverity();
		if (severity == IStatus.OK) {
			setViewerInput(input);
			if (serial >= 0)
				selectLine(serial);
		}
		else if (severity == IStatus.CANCEL)
			showMessage("Canceled");
		else if (severity == IStatus.ERROR)
			showMessage("Failed: " + status.getMessage());
	}
	
	protected void setViewerInput(VectorResult input) {
		if (!ObjectUtils.equals(input, viewer.getInput())) {
			viewer.setInput(input);
			// show message instead of empty table, when no index file
			checkInput(input);
			if (input != null) {
				boolean hasEventNumbers = input.getColumns().indexOf('E') >= 0;
				gotoEventAction.setEnabled(hasEventNumbers);
				setEventNumberColumnVisible(hasEventNumbers);
				setContentDescription(String.format("'%s.%s' in run %s from file %s",
						input.getModuleName(), input.getName(),
						input.getFileRun().getRun().getRunName(),
						input.getFileRun().getFile().getFilePath()));
			}
			else
				setContentDescription("");
			viewer.redraw();
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
				hideMessage();
				if (eventNumberColumn != null && input.getColumns().indexOf('E') < 0) {
					eventNumberColumn.dispose();
					eventNumberColumn = null;
				}
				else if (eventNumberColumn == null && input.getColumns().indexOf('E') >= 0) {
					eventNumberColumn = viewer.createColumn();
					eventNumberColumn.setWidth(60);
					eventNumberColumn.setText("Event#");
					viewer.setColumnOrder(new int[] {0,3,1,2});
				}
			}
			else {
				String fileInWorkspace = input.getFileRun().getFile().getFilePath();
				showMessage("Vector content cannot be browsed, because the index file (.vci) " +
							   "for \""+fileInWorkspace+"\" is missing or out of date.");
			}
		}
	}
	
	// Order: Item#, Event#, Time, Value
	private static final int[] ColumnOrder = new int[] {0,3,1,2};
	
	private void setEventNumberColumnVisible(boolean visible) {
		if (eventNumberColumn != null && !visible) {
			eventNumberColumn.dispose();
			eventNumberColumn = null;
		}
		else if (eventNumberColumn == null && visible) {
			eventNumberColumn = viewer.createColumn();
			eventNumberColumn.setWidth(60);
			eventNumberColumn.setText("Event#");
			viewer.setColumnOrder(ColumnOrder);
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
				Object targetAddr = parseTarget(dialog.getValue());
				if (targetAddr != null) {
					switch (target) {
					case Line: view.gotoLine((Integer)targetAddr); break;
					case Event: view.gotoEvent((Integer)targetAddr); break;
					case Time: view.gotoTime((BigDecimal)targetAddr); break;
					}
				}
			}
		}
		
		public Object parseTarget(String str) {
			try
			{
				if (target == GotoTarget.Time)
					return BigDecimal.parse(str);
				else
					return Integer.parseInt(str);
			} catch (Exception e) {
				return null;
			}
		}
	}
}
