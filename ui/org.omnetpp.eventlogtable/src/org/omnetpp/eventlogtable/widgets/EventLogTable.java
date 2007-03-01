package org.omnetpp.eventlogtable.widgets;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.IActionBars;
import org.omnetpp.common.virtualtable.VirtualTableViewer;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.EventLogTableFacade;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.MatchKind;
import org.omnetpp.eventlog.engine.MessageDependency;
import org.omnetpp.eventlog.engine.MessageDependencyList;
import org.omnetpp.eventlogtable.editors.EventLogInput;

public class EventLogTable extends VirtualTableViewer<EventLogEntry> {
	public EventLogTable(Composite parent, IActionBars actionBars) {
		super(parent);

		setContentProvider(new EventLogTableContentProvider());
		setLineRenderer(new EventLogTableLineRenderer());

		TableColumn tableColumn = virtualTable.createColumn();
		tableColumn.setWidth(60);
		tableColumn.setText("Event #");

		tableColumn = virtualTable.createColumn();
		tableColumn.setWidth(140);
		tableColumn.setText("Time");

		tableColumn = virtualTable.createColumn();
		tableColumn.setWidth(2000);
		tableColumn.setText("Details");

		addPopupMenu();
	}

	private void addPopupMenu() {
		Menu popupMenu = new Menu(virtualTable);

		addSearchTextMenuItem(popupMenu);
		addSeparatorMenuItem(popupMenu);
		addGotoEventMenuItem(popupMenu);
		addGotoSimulationTimeMenuItem(popupMenu);
		addSeparatorMenuItem(popupMenu);
		addGotoEventCauseMenuItem(popupMenu);
		addGotoMessageArrivalMenuItem(popupMenu);
		addGotoMessageOriginMenuItem(popupMenu);
		addGotoMessageReuseMenuItem(popupMenu);
		addSeparatorMenuItem(popupMenu);
		addBookmarkMenuItem(popupMenu);

		virtualTable.setMenu(popupMenu);
	}

	private void addSeparatorMenuItem(Menu popupMenu) {
		new MenuItem(popupMenu, SWT.SEPARATOR);
	}

	private void addSearchTextMenuItem(Menu popupMenu) {
		MenuItem subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Search text...");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				InputDialog dialog = new InputDialog(null, "Search text", "Please enter the search text", null, null);
				dialog.open();

				// TODO:
				//String searchText = dialog.getValue();
				//IEventLog eventLog = getEventLog();
			}
		});
	}

	private void addGotoEventCauseMenuItem(Menu popupMenu) {
		MenuItem subMenuItem;
		subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Goto event cause");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				BeginSendEntry beginSendEntry = virtualTable.getSelectionElement().getEvent().getCause().getCauseBeginSendEntry();
				
				if (beginSendEntry != null)
					virtualTable.gotoElement(beginSendEntry);
			}
		});
	}

	private void addGotoMessageArrivalMenuItem(Menu popupMenu) {
		MenuItem subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Goto message arrival");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IEvent event = virtualTable.getSelectionElement().getEvent();
				MessageDependencyList consequences = event.getConsequences();
				EventLogEntry eventLogEntry = virtualTable.getSelectionElement();
				
				for (int i = 0; i < consequences.size(); i++) {
					MessageDependency consequence = consequences.get(i);
					if (consequence.getCauseBeginSendEntry().equals(eventLogEntry)) {
						IEvent consequenceEvent = consequence.getConsequenceEvent();
						
						if (consequenceEvent != null)
							virtualTable.gotoElement(consequenceEvent.getEventEntry());
					}
				}
			}
		});
	}

	private void addGotoMessageOriginMenuItem(Menu popupMenu) {
		MenuItem subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Goto message origin");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IEvent event = virtualTable.getSelectionElement().getEvent();
				MessageDependencyList causes = event.getCauses();
				EventLogEntry eventLogEntry = virtualTable.getSelectionElement();
				
				for (int i = 0; i < causes.size(); i++) {
					MessageDependency cause = causes.get(i);
					if (getEventLogTableFacade().MessageDependency_isMessageReuse(cause.getCPtr()) &&
						cause.getConsequenceBeginSendEntry().equals(eventLogEntry)) {
						IEvent causeEvent = cause.getCauseEvent();
						
						if (causeEvent != null)
							virtualTable.gotoElement(causeEvent.getEventEntry());
					}
				}
			}
		});
	}

	private void addGotoMessageReuseMenuItem(Menu popupMenu) {
		MenuItem subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Goto message reuse");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IEvent event = virtualTable.getSelectionElement().getEvent();
				MessageDependencyList consequences = event.getConsequences();
				
				for (int i = 0; i < consequences.size(); i++) {
					MessageDependency consequence = consequences.get(i);
					if (getEventLogTableFacade().MessageDependency_isMessageReuse(consequence.getCPtr())) {
						BeginSendEntry beginSendEntry = consequence.getConsequenceBeginSendEntry();
						
						if (beginSendEntry != null)
							virtualTable.gotoElement(beginSendEntry);
					}
				}
			}
		});
	}

	private void addGotoEventMenuItem(Menu popupMenu) {
		MenuItem subMenuItem;
		subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Goto event...");
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
						virtualTable.gotoElement(event.getEventEntry());
					else
						MessageDialog.openError(null, "Goto event" , "No such event: " + eventNumber);
				}
				catch (Exception x) {
					// void
				}
			}
		});
	}

	private void addGotoSimulationTimeMenuItem(Menu popupMenu) {
		MenuItem subMenuItem;
		subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Goto simulation time...");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				InputDialog dialog = new InputDialog(null, "Goto simulation time", "Please enter the simulation time to go to", null, new IInputValidator() {
					public String isValid(String newText) {
						try {
							double simulationTime = Double.parseDouble(newText);
							
							if (simulationTime >= 0)
								return null;
							else
								return "Negative simulation time";
						}
						catch (Exception e) {
							return "Not a number";
						}
					}
				});

				dialog.open();

				try {
					double simulationTime = Double.parseDouble(dialog.getValue());
					IEventLog eventLog = getEventLog();
					IEvent event = eventLog.getEventForSimulationTime(simulationTime, MatchKind.FIRST_OR_NEXT);

					if (event != null)
						virtualTable.gotoElement(event.getEventEntry());
					else
						MessageDialog.openError(null, "Goto simulation time" , "No such simulation time: " + simulationTime);
				}
				catch (Exception x) {
					// void
				}
			}
		});
	}

	private void addBookmarkMenuItem(Menu popupMenu) {
		MenuItem subMenuItem;
		subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Bookmark");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent ev) {
				EventLogInput eventLogInput = (EventLogInput)getInput();
				try {
					IMarker marker = eventLogInput.getFile().createMarker(IMarker.BOOKMARK);
					IEvent event = ((EventLogEntry)virtualTable.getSelectionElement()).getEvent();
					marker.setAttribute(IMarker.LOCATION, "# " + event.getEventNumber());
					marker.setAttribute("EventNumber", event.getEventNumber());
					refresh();
				}
				catch (CoreException e) {
					throw new RuntimeException(e);
				}
			}
		});
	}

	@Override
	public void setInput(Object input) {
		super.setInput(input);

		if (input != null)
			((EventLogTableLineRenderer)getLineRenderer()).setResource(((EventLogInput)input).getFile());
	}

	public IEventLog getEventLog() {
		return ((EventLogInput)getInput()).getEventLog();
	}

	public EventLogTableFacade getEventLogTableFacade() {
		return ((EventLogInput)getInput()).getEventLogTableFacade();
	}

	public EventLogTableContentProvider getEventLogTableContentProvider() {
		return (EventLogTableContentProvider)getContentProvider();
	}

	public int getFilterMode() {
		return getEventLogTableContentProvider().getFilterMode();
	}

	public void setFilterMode(int i) {
		getEventLogTableContentProvider().setFilterMode(i);
		virtualTable.stayNear();
	}
}
