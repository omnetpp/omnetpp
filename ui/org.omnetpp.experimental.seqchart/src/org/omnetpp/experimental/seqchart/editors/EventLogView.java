package org.omnetpp.experimental.seqchart.editors;

import java.util.ArrayList;

import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.eventlog.engine.EventEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.experimental.seqchart.widgets.EventLogVirtualTableContentProvider;
import org.omnetpp.experimental.seqchart.widgets.EventLogVirtualTableItemProvider;
import org.omnetpp.experimental.seqchart.widgets.LongVirtualTableContentProvider;
import org.omnetpp.experimental.seqchart.widgets.LongVirtualTableItemProvider;
import org.omnetpp.experimental.seqchart.widgets.VirtualTableSelection;
import org.omnetpp.experimental.seqchart.widgets.VirtualTableViewer;

/**
 * View for displaying and navigating simulation events and associated log messages.
 */
//TODO add double-click support (factor out filtering to an Action?)
//TODO add context menu etc
//FIXME the <category name="OMNeT++/OMNEST" id="org.omnetpp"> stuff in plugin.xml doesn't seem to take effect
public class EventLogView extends ViewPart {

	public static final String PART_ID = "org.omnetpp.experimental.seqchart.editors.EventLogView"; 

	protected VirtualTableViewer virtualTableViewer;
	//protected Action doubleClickAction;


	/**
	 * The constructor.
	 */
	public EventLogView() {
	}

	/**
	 * Create the viewer and initialize it.
	 */
	@Override
	public void createPartControl(Composite parent) {
		Table table = new Table(parent, SWT.FULL_SELECTION | SWT.VIRTUAL | SWT.MULTI);
		table.setHeaderVisible(true);
		table.setLinesVisible(false);
		TableColumn tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(60);
		tableColumn.setText("Event#");
		TableColumn tableColumn1 = new TableColumn(table, SWT.NONE);
		tableColumn1.setWidth(140);
		tableColumn1.setText("Time");
		TableColumn tableColumn4 = new TableColumn(table, SWT.NONE);
		tableColumn4.setWidth(800);
		tableColumn4.setText("Details / log message");

		//virtualTableViewer = createLongTableViewer(table);
		virtualTableViewer = createEventLogTableViewer(table);
	
		addPopupMenu();

		// we want to provide selection for the sequence chart tool (an IEditPart)
		getSite().setSelectionProvider(virtualTableViewer);

		// follow selection
		getSite().getPage().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != virtualTableViewer && selection instanceof EventLogSelection) {
					EventLogSelection eventLogSelection = (EventLogSelection)selection;
					ArrayList<EventEntry> eventEntries = new ArrayList<EventEntry>();
					for (IEvent event : eventLogSelection.getEvents())
						eventEntries.add(event.getEventEntry());
					virtualTableViewer.setSelection(new VirtualTableSelection(eventLogSelection.getInput(), eventEntries));
				}
			}
		});
		
		// bootstrap with current selection
		virtualTableViewer.setSelection(getSite().getSelectionProvider().getSelection());
	}

	private void addPopupMenu() {
		Menu popupMenu = new Menu(virtualTableViewer.getControl());

		// find menu item
		MenuItem subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Search text");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				InputDialog dialog = new InputDialog(null, "Search text", "Please enter the search text", null, null);
				dialog.open();

				String searchText = dialog.getValue();
				IEventLog eventLog = (IEventLog)virtualTableViewer.getInput();
				// TODO:
			}
		});

		// goto event menu item
		subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Goto event");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				InputDialog dialog = new InputDialog(null, "Goto event", "Please enter the event number to go to", null, new IInputValidator() {
					public String isValid(String newText) {
						try {
							int eventNumber = Integer.parseInt(newText);
							
							if (eventNumber >= 0)
								return null;
							else
								return "Negative event number";
						}
						catch (Exception e) {
							return "Not a number";
						}
					}
				});

				dialog.open();

				try {
					int eventNumber = Integer.parseInt(dialog.getValue());
					IEventLog eventLog = (IEventLog)virtualTableViewer.getInput();
					IEvent event = eventLog.getEventForEventNumber(eventNumber);

					if (event != null)
						virtualTableViewer.gotoElement(event.getEventEntry());
					else
						MessageDialog.openError(null, "Goto event" , "No such event: " + eventNumber);
				}
				catch (Exception x) {
					// void
				}
			}
		});

		virtualTableViewer.getControl().setMenu(popupMenu);
	}
	
	/**
	 * For debug purposes.
	 */
	protected VirtualTableViewer createLongTableViewer(Table table) {
		VirtualTableViewer virtualTableViewer = new VirtualTableViewer(table);
		virtualTableViewer.setContentProvider(new LongVirtualTableContentProvider());
		//virtualTableViewer.setInput(100L);
		virtualTableViewer.setInput(1000000000L);
		virtualTableViewer.setLabelProvider(new LongVirtualTableItemProvider());
		
		return virtualTableViewer;
	}
	
	protected VirtualTableViewer createEventLogTableViewer(Table table) {
		VirtualTableViewer virtualTableViewer = new VirtualTableViewer(table);
		virtualTableViewer.setContentProvider(new EventLogVirtualTableContentProvider());
		virtualTableViewer.setInput(null);
		virtualTableViewer.setLabelProvider(new EventLogVirtualTableItemProvider());
		
		return virtualTableViewer;
	}
	
	/**
	 * Passing the focus request to the viewer's control.
	 */
	@Override
	public void setFocus() {
		virtualTableViewer.getControl().setFocus();
	}

//	private void hookDoubleClickAction() {
//		viewer.addDoubleClickListener(new IDoubleClickListener() {
//			public void doubleClick(DoubleClickEvent event) {
//				doubleClickAction.run();
//			}
//		});
//	}
}
