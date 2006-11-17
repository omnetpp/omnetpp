package org.omnetpp.eventlogtable.widgets;

import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

public class EventLogTable extends VirtualTable<EventLogEntry> {
	public EventLogTable(Composite parent) {
		super(new Table(parent, SWT.FULL_SELECTION | SWT.VIRTUAL | SWT.MULTI));

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

		setContentProvider(new EventLogVirtualTableContentProvider());
		setInput(null);
		setLabelProvider(new EventLogVirtualTableItemProvider());

		addPopupMenu();
	}

	public IEventLog getEventLog() {
		return (IEventLog)getInput();
	}

	private void addPopupMenu() {
		Menu popupMenu = new Menu(getControl());

		// find menu item
		MenuItem subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Search text");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				InputDialog dialog = new InputDialog(null, "Search text", "Please enter the search text", null, null);
				dialog.open();

				// TODO:
				//String searchText = dialog.getValue();
				//IEventLog eventLog = getEventLog();
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
					IEventLog eventLog = getEventLog();
					IEvent event = eventLog.getEventForEventNumber(eventNumber);

					if (event != null)
						gotoElement(event.getEventEntry());
					else
						MessageDialog.openError(null, "Goto event" , "No such event: " + eventNumber);
				}
				catch (Exception x) {
					// void
				}
			}
		});

		getControl().setMenu(popupMenu);
	}
}
