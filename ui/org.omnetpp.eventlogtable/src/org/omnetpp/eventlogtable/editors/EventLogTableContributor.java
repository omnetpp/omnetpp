package org.omnetpp.eventlogtable.editors;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuCreator;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.part.EditorActionBarContributor;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.MatchKind;
import org.omnetpp.eventlog.engine.MessageDependency;
import org.omnetpp.eventlog.engine.MessageDependencyList;
import org.omnetpp.eventlogtable.widgets.EventLogTable;
import org.omnetpp.eventlogtable.widgets.EventLogTableLineRenderer;


public class EventLogTableContributor extends EditorActionBarContributor {
	protected EventLogTable eventLogTable;
	
	public EventLogTableContributor() {
	}
	
	public EventLogTableContributor(EventLogTable eventLogTable) {
		this.eventLogTable = eventLogTable;
	}

	public void contributeToPopupMenu(IMenuManager menuManager) {
		menuManager.add(getSearchTextAction());
		menuManager.add(getSeparatorAction());
		menuManager.add(getGotoEventAction());
		menuManager.add(getGotoSimulationTimeAction());
		menuManager.add(getSeparatorAction());
		menuManager.add(getGotoEventCauseAction());
		menuManager.add(getGotoMessageArrivalAction());
		menuManager.add(getGotoMessageOriginAction());
		menuManager.add(getGotoMessageReuseAction());
		menuManager.add(getSeparatorAction());
		menuManager.add(getBookmarkAction());
	}
	@Override
	public void contributeToToolBar(IToolBarManager toolBarManager) {
		toolBarManager.add(getFilterModeAction());
		toolBarManager.add(getDisplayModeAction());		
	}
	
	@Override
	public void setActiveEditor(IEditorPart targetEditor) {
		eventLogTable = ((EventLogTableEditor)targetEditor).getEventLogTable();
	}

	private Separator getSeparatorAction() {
		return new Separator();
	}

	private Action getSearchTextAction() {
		return new Action("Search raw text...") {
			public void run() {
				InputDialog dialog = new InputDialog(null, "Search raw text", "Please enter the search text", null, null);
				dialog.open();
				
				// TODO: add case sensitivity, forward/backward search, etc.

				String searchText = dialog.getValue();
				EventLogEntry foundEventLogEntry = getEventLog().findEventLogEntry(eventLogTable.getSelectionElement(), searchText, true);
				
				if (foundEventLogEntry != null)
					eventLogTable.gotoElement(foundEventLogEntry);
				else
					MessageDialog.openInformation(null, "Search raw text", "No matches found!");
			}
		};
	}

	private Action getGotoEventCauseAction() {
		return new Action("Goto event cause") {
			public void run() {
				BeginSendEntry beginSendEntry = eventLogTable.getSelectionElement().getEvent().getCause().getCauseBeginSendEntry();
				
				if (beginSendEntry != null)
					eventLogTable.gotoElement(beginSendEntry);
			}
		};
	}

	private Action getGotoMessageArrivalAction() {
		return new Action("Goto message arrival") {
			public void run() {
				IEvent event = eventLogTable.getSelectionElement().getEvent();
				MessageDependencyList consequences = event.getConsequences();
				EventLogEntry eventLogEntry = eventLogTable.getSelectionElement();
				
				for (int i = 0; i < consequences.size(); i++) {
					MessageDependency consequence = consequences.get(i);
					if (consequence.getCauseBeginSendEntry().equals(eventLogEntry)) {
						IEvent consequenceEvent = consequence.getConsequenceEvent();
						
						if (consequenceEvent != null)
							eventLogTable.gotoElement(consequenceEvent.getEventEntry());
					}
				}
			}
		};
	}

	private Action getGotoMessageOriginAction() {
		return new Action("Goto message origin") {
			public void run() {
				IEvent event = eventLogTable.getSelectionElement().getEvent();
				MessageDependencyList causes = event.getCauses();
				EventLogEntry eventLogEntry = eventLogTable.getSelectionElement();
				
				for (int i = 0; i < causes.size(); i++) {
					MessageDependency cause = causes.get(i);
					if (eventLogTable.getEventLogTableFacade().MessageDependency_isMessageReuse(cause.getCPtr()) &&
						cause.getConsequenceBeginSendEntry().equals(eventLogEntry)) {
						IEvent causeEvent = cause.getCauseEvent();
						
						if (causeEvent != null)
							eventLogTable.gotoElement(causeEvent.getEventEntry());
					}
				}
			}
		};
	}

	private Action getGotoMessageReuseAction() {
		return new Action("Goto message reuse") {
			public void run() {
				IEvent event = eventLogTable.getSelectionElement().getEvent();
				MessageDependencyList consequences = event.getConsequences();
				
				for (int i = 0; i < consequences.size(); i++) {
					MessageDependency consequence = consequences.get(i);
					if (eventLogTable.getEventLogTableFacade().MessageDependency_isMessageReuse(consequence.getCPtr())) {
						BeginSendEntry beginSendEntry = consequence.getConsequenceBeginSendEntry();
						
						if (beginSendEntry != null)
							eventLogTable.gotoElement(beginSendEntry);
					}
				}
			}
		};
	}

	private Action getGotoEventAction() {
		return new Action("Goto event...") {
			public void run() {
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
						eventLogTable.gotoElement(event.getEventEntry());
					else
						MessageDialog.openError(null, "Goto event" , "No such event: " + eventNumber);
				}
				catch (Exception x) {
					// void
				}
			}
		};
	}

	private Action getGotoSimulationTimeAction() {
		return new Action("Goto simulation time...") {
			public void run() {
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
						eventLogTable.gotoElement(event.getEventEntry());
					else
						MessageDialog.openError(null, "Goto simulation time" , "No such simulation time: " + simulationTime);
				}
				catch (Exception x) {
					// void
				}
			}
		};
	}

	private Action getBookmarkAction() {
		return new Action("Bookmark") {
			public void run() {
				try {
					EventLogInput eventLogInput = (EventLogInput)eventLogTable.getInput();
					IMarker marker = eventLogInput.getFile().createMarker(IMarker.BOOKMARK);
					IEvent event = ((EventLogEntry)eventLogTable.getSelectionElement()).getEvent();
					marker.setAttribute(IMarker.LOCATION, "# " + event.getEventNumber());
					marker.setAttribute("EventNumber", event.getEventNumber());
					eventLogTable.redraw();
				}
				catch (CoreException e) {
					throw new RuntimeException(e);
				}
			}
		};
	}

	private Action getDisplayModeAction() {
		Action action = new Action("Display", Action.AS_DROP_DOWN_MENU) {
			@Override
			public void run() {
				EventLogTableLineRenderer eventLogTableItemProvider = (EventLogTableLineRenderer)eventLogTable.getLineRenderer();
				eventLogTableItemProvider.setDisplayMode(EventLogTableLineRenderer.DisplayMode.values()[(eventLogTableItemProvider.getDisplayMode().ordinal() + 1) % EventLogTableLineRenderer.DisplayMode.values().length]);
				eventLogTable.redraw();
			}
			
			@Override
			public IMenuCreator getMenuCreator() {
				return new IMenuCreator() {
					private Menu controlMenu;

					private Menu parentMenu;
					
					
					public void dispose() {
						if (controlMenu != null)
							controlMenu.dispose();

						if (parentMenu != null)
							parentMenu.dispose();
					}

					public Menu getMenu(Control parent) {
						if (controlMenu == null) {
							controlMenu = new Menu(parent);
							createMenu(controlMenu);
						}
						
						return controlMenu;
					}

					public Menu getMenu(Menu parent) {
						if (parentMenu == null) {
							parentMenu = new Menu(parent);
							createMenu(parentMenu);
						}
						
						return parentMenu;
					}

					private void createMenu(Menu menu) {
						addSubMenuItem(menu, "Descriptive", EventLogTableLineRenderer.DisplayMode.DESCRIPTIVE);
						addSubMenuItem(menu, "Raw", EventLogTableLineRenderer.DisplayMode.RAW);
					}

					private void addSubMenuItem(Menu menu, String text, final EventLogTableLineRenderer.DisplayMode displayMode) {
						MenuItem subMenuItem = new MenuItem(menu, SWT.PUSH);
						subMenuItem.setText(text);
						subMenuItem.addSelectionListener( new SelectionAdapter() {
							public void widgetSelected(SelectionEvent e) {
								EventLogTableLineRenderer eventLogTableItemProvider = (EventLogTableLineRenderer)eventLogTable.getLineRenderer();
								eventLogTableItemProvider.setDisplayMode(displayMode);
								eventLogTable.redraw();
							}
						});
					}
				};
			}
		};

		action.setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_DISPLAY_MODE));
		return action;
	}

	private Action getFilterModeAction() {
		Action action = new Action("Show", Action.AS_DROP_DOWN_MENU) {
			@Override
			public void run() {
				eventLogTable.setFilterMode((eventLogTable.getFilterMode() + 1) % 5);
				eventLogTable.updateVerticalBarParameters();
			}
			
			@Override
			public IMenuCreator getMenuCreator() {
				return new IMenuCreator() {
					private Menu controlMenu;

					private Menu parentMenu;
					
					public void dispose() {
						if (controlMenu != null)
							controlMenu.dispose();

						if (parentMenu != null)
							parentMenu.dispose();
					}

					public Menu getMenu(Control parent) {
						if (controlMenu == null) {
							controlMenu = new Menu(parent);
							createMenu(controlMenu);
						}
						
						return controlMenu;
					}

					public Menu getMenu(Menu parent) {
						if (parentMenu == null) {
							parentMenu = new Menu(parent);
							createMenu(parentMenu);
						}
						
						return parentMenu;
					}

					private void createMenu(Menu menu) {
						addSubMenuItem(menu, "All", 0);
						addSubMenuItem(menu, "Events, message sends and log messages", 1);
						addSubMenuItem(menu, "Events and log messages", 2);
						addSubMenuItem(menu, "Events", 3);
						addSubMenuItem(menu, "Custom filter...", new SelectionAdapter() {
							public void widgetSelected(SelectionEvent e) {
								InputDialog dialog = new InputDialog(null, "Search patter", "Please enter the search pattern such as (BS and c(MyMessage))", null, null);
								dialog.open();

								String pattern = dialog.getValue();
								if (pattern == null || pattern.equals(""))
									pattern = "*";

								eventLogTable.getEventLogTableContentProvider().setCustomFilter(pattern);
								eventLogTable.setFilterMode(4);
							}
						});
					}

					private void addSubMenuItem(final Menu menu, String text, final int filterMode) {
						addSubMenuItem(menu, text, new SelectionAdapter() {
							public void widgetSelected(SelectionEvent e) {
								eventLogTable.setFilterMode(filterMode);
							}
						});
					}

					private void addSubMenuItem(Menu menu, String text, SelectionListener adapter) {
						MenuItem subMenuItem = new MenuItem(menu, SWT.RADIO);
						subMenuItem.setText(text);
						subMenuItem.addSelectionListener(adapter);
					}
				};
			}
		};

		action.setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_FILTER));
		return action;
	}
	
	private IEventLog getEventLog() {
		return eventLogTable.getEventLog();
	}
}
