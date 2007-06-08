package org.omnetpp.eventlogtable.editors;

import java.util.ArrayList;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuCreator;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.part.EditorActionBarContributor;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.IMessageDependency;
import org.omnetpp.eventlog.engine.IMessageDependencyList;
import org.omnetpp.eventlog.engine.MatchKind;
import org.omnetpp.eventlogtable.widgets.EventLogTable;


public class EventLogTableContributor extends EditorActionBarContributor implements ISelectionChangedListener {
	private static EventLogTableContributor singleton;

	protected EventLogTable eventLogTable;

	protected Separator separatorAction;

	protected EventLogTableAction searchTextAction;

	protected EventLogTableAction gotoEventAction;

	protected EventLogTableAction gotoSimulationTimeAction;

	protected EventLogTableAction gotoEventCauseAction;

	protected EventLogTableAction gotoMessageArrivalAction;

	protected EventLogTableAction gotoMessageOriginAction;

	protected EventLogTableAction gotoMessageReuseAction;

	protected EventLogTableAction toggleBookmarkAction;

	protected EventLogTableMenuAction filterModeAction;

	protected EventLogTableMenuAction displayModeAction;
	
	public EventLogTableContributor() {
		createActions();

		if (singleton == null)
			singleton = this;
	}

	public EventLogTableContributor(EventLogTable eventLogTable) {
		this.eventLogTable = eventLogTable;
		createActions();
		eventLogTable.addSelectionChangedListener(this);
	}
	
	@Override
	public void dispose() {
		super.dispose();
		singleton = null;
	}

	public static EventLogTableContributor getDefault() {
		Assert.isTrue(singleton != null);

		return singleton;
	}
	
	public void contributeToPopupMenu(IMenuManager menuManager) {
		menuManager.add(searchTextAction);
		menuManager.add(separatorAction);
		menuManager.add(gotoEventAction);
		menuManager.add(gotoSimulationTimeAction);
		menuManager.add(separatorAction);
		menuManager.add(gotoEventCauseAction);
		menuManager.add(gotoMessageArrivalAction);
		menuManager.add(gotoMessageOriginAction);
		menuManager.add(gotoMessageReuseAction);
		menuManager.add(separatorAction);
		menuManager.add(toggleBookmarkAction);
		menuManager.add(separatorAction);
		menuManager.add(filterModeAction);
		menuManager.add(displayModeAction);
	}

	@Override
	public void contributeToToolBar(IToolBarManager toolBarManager) {
		toolBarManager.add(filterModeAction);
		toolBarManager.add(displayModeAction);		
	}
	
	@Override
	public void setActiveEditor(IEditorPart targetEditor) {
		if (targetEditor instanceof EventLogTableEditor) {
			if (eventLogTable != null)
				eventLogTable.removeSelectionChangedListener(this);

			eventLogTable = ((EventLogTableEditor)targetEditor).getEventLogTable();
			eventLogTable.addSelectionChangedListener(this);
	
			update();
		}
	}

	public void selectionChanged(SelectionChangedEvent event) {
		update();
	}

	private void createActions() {
		separatorAction = new Separator();
		searchTextAction = createSearchTextAction();
		gotoEventAction = createGotoEventAction();
		gotoSimulationTimeAction = createGotoSimulationTimeAction();
		gotoEventCauseAction = createGotoEventCauseAction();
		gotoMessageArrivalAction = createGotoMessageArrivalAction();
		gotoMessageOriginAction = createGotoMessageOriginAction();
		gotoMessageReuseAction = createGotoMessageReuseAction();
		toggleBookmarkAction = createToggleBookmarkAction();
		filterModeAction = createFilterModeAction();
		displayModeAction = createDisplayModeAction();
	}

	private void update() {
		searchTextAction.update();
		separatorAction.update();
		gotoEventAction.update();
		gotoSimulationTimeAction.update();
		separatorAction.update();
		gotoEventCauseAction.update();
		gotoMessageArrivalAction.update();
		gotoMessageOriginAction.update();
		gotoMessageReuseAction.update();
		separatorAction.update();
		toggleBookmarkAction.update();
		separatorAction.update();
		filterModeAction.update();
		displayModeAction.update();
	}

	private IEventLog getEventLog() {
		return eventLogTable.getEventLog();
	}
	
	private EventLogTableAction createSearchTextAction() {
		return new EventLogTableAction("Search raw text...") {
			@Override
			public void run() {
				InputDialog dialog = new InputDialog(null, "Search raw text", "Please enter the search text", null, null);

				if (dialog.open() == Window.OK) {
					
					// TODO: add case sensitivity, forward/backward search, etc.
	
					String searchText = dialog.getValue();
					EventLogEntry foundEventLogEntry = getEventLog().findEventLogEntry(eventLogTable.getSelectionElement().getEventLogEntry(eventLogTable.getEventLogInput()), searchText, true);
	
					if (foundEventLogEntry != null)
						eventLogTable.gotoClosestElement(new EventLogEntryReference(foundEventLogEntry));
					else
						MessageDialog.openInformation(null, "Search raw text", "No matches found!");
				}
			}
			
			@Override
			public void update() {
				setEnabled(getEventLog().getApproximateNumberOfEvents() != 0);
			}
		};
	}

	private EventLogTableAction createGotoEventCauseAction() {
		return new EventLogTableAction("Goto event cause") {
			@Override
			public void run() {
				eventLogTable.gotoClosestElement(new EventLogEntryReference(getCauseEventLogEntry()));
			}

			@Override
			public void update() {
				setEnabled(getCauseEventLogEntry() != null);
			}

			private EventLogEntry getCauseEventLogEntry() {
				EventLogEntryReference eventLogEntryReference = eventLogTable.getSelectionElement();
				
				if (eventLogEntryReference == null)
					return null;
				else {
					IMessageDependency cause = eventLogEntryReference.getEventLogEntry(eventLogTable.getEventLogInput()).getEvent().getCause();
					
					if (cause == null)
						return null;
					else
						return cause.getBeginSendEntry();
				}
			}
		};
	}

	private EventLogTableAction createGotoMessageArrivalAction() {
		return new EventLogTableAction("Goto message arrival") {
			@Override
			public void run() {
				eventLogTable.gotoElement(new EventLogEntryReference(getConsequenceEventLogEntry()));
			}

			@Override
			public void update() {
				setEnabled(getConsequenceEventLogEntry() != null);
			}
			
			private EventLogEntry getConsequenceEventLogEntry() {
				EventLogEntryReference eventLogEntryReference = eventLogTable.getSelectionElement();

				if (eventLogEntryReference != null) {
					EventLogEntry eventLogEntry = eventLogEntryReference.getEventLogEntry(eventLogTable.getEventLogInput());
					IEvent event = eventLogEntry.getEvent();
					IMessageDependencyList consequences = event.getConsequences();
					
					for (int i = 0; i < consequences.size(); i++) {
						IMessageDependency consequence = consequences.get(i);

						if (!eventLogTable.getEventLogTableFacade().MessageDependency_getIsReuse(consequence.getCPtr()) &&
							consequence.getBeginSendEntry().equals(eventLogEntry))
						{
							IEvent consequenceEvent = consequence.getConsequenceEvent();
							
							if (consequenceEvent != null)
								return consequenceEvent.getEventEntry();
						}
					}
				}

				return null;
			}
		};
	}

	private EventLogTableAction createGotoMessageOriginAction() {
		return new EventLogTableAction("Goto message origin") {
			@Override
			public void run() {
				eventLogTable.gotoElement(new EventLogEntryReference(getMessageOriginEventLogEntry()));
			}

			@Override
			public void update() {
				setEnabled(getMessageOriginEventLogEntry() != null);
			}

			private EventLogEntry getMessageOriginEventLogEntry() {
				EventLogEntryReference eventLogEntryReference = eventLogTable.getSelectionElement();

				if (eventLogEntryReference != null) {
					EventLogEntry eventLogEntry = eventLogEntryReference.getEventLogEntry(eventLogTable.getEventLogInput());
					IEvent event = eventLogEntry.getEvent();
					IMessageDependencyList causes = event.getCauses();
					
					for (int i = 0; i < causes.size(); i++) {
						IMessageDependency cause = causes.get(i);

						if (eventLogTable.getEventLogTableFacade().MessageDependency_getIsReuse(cause.getCPtr()) &&
							cause.getBeginSendEntry().equals(eventLogEntry)) {
							IEvent causeEvent = cause.getCauseEvent();
							
							if (causeEvent != null)
								return causeEvent.getEventEntry();
						}
					}
				}

				return null;
			}
		};
	}

	private EventLogTableAction createGotoMessageReuseAction() {
		return new EventLogTableAction("Goto message reuse") {
			@Override
			public void run() {
				eventLogTable.gotoClosestElement(new EventLogEntryReference(getMessageReuseEventLogEntry()));
			}

			@Override
			public void update() {
				setEnabled(getMessageReuseEventLogEntry() != null);
			}

			private EventLogEntry getMessageReuseEventLogEntry() {
				EventLogEntryReference eventLogEntryReference = eventLogTable.getSelectionElement();

				if (eventLogEntryReference != null) {
					EventLogEntry eventLogEntry = eventLogEntryReference.getEventLogEntry(eventLogTable.getEventLogInput());
					IEvent event = eventLogEntry.getEvent();
					IMessageDependencyList consequences = event.getConsequences();
					
					for (int i = 0; i < consequences.size(); i++) {
						IMessageDependency consequence = consequences.get(i);

						if (eventLogTable.getEventLogTableFacade().MessageDependency_getIsReuse(consequence.getCPtr())) {
							BeginSendEntry beginSendEntry = consequence.getBeginSendEntry();
							
							if (beginSendEntry != null)
								return beginSendEntry;
						}
					}
				}
				
				return null;
			}
		};
	}

	private EventLogTableAction createGotoEventAction() {
		return new EventLogTableAction("Goto event...") {
			@Override
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

				if (dialog.open() == Window.OK) {
					try {
						int eventNumber = Integer.parseInt(dialog.getValue());
						IEventLog eventLog = getEventLog();
						IEvent event = eventLog.getEventForEventNumber(eventNumber);
	
						if (event != null)
							eventLogTable.gotoElement(new EventLogEntryReference(event.getEventEntry()));
						else
							MessageDialog.openError(null, "Goto event" , "No such event: " + eventNumber);
					}
					catch (Exception x) {
						// void
					}
				}
			}
			
			@Override
			public void update() {
				setEnabled(getEventLog().getApproximateNumberOfEvents() != 0);
			}
		};
	}

	private EventLogTableAction createGotoSimulationTimeAction() {
		return new EventLogTableAction("Goto simulation time...") {
			@Override
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

				if (dialog.open() == Window.OK) {
					try {
						BigDecimal simulationTime = BigDecimal.parse(dialog.getValue());
						IEventLog eventLog = getEventLog();
						IEvent event = eventLog.getEventForSimulationTime(simulationTime, MatchKind.FIRST_OR_NEXT);
	
						if (event != null)
							eventLogTable.gotoElement(new EventLogEntryReference(event.getEventEntry()));
						else
							MessageDialog.openError(null, "Goto simulation time" , "No such simulation time: " + simulationTime);
					}
					catch (Exception x) {
						// void
					}
				}
			}
			
			@Override
			public void update() {
				setEnabled(getEventLog().getApproximateNumberOfEvents() != 0);
			}
		};
	}

	private EventLogTableAction createToggleBookmarkAction() {
		return new EventLogTableAction("Toggle bookmark") {
			@Override
			public void run() {
				try {
					EventLogEntryReference eventLogEntryReference = eventLogTable.getSelectionElement();

					if (eventLogEntryReference != null) {
						IEvent event = eventLogEntryReference.getEventLogEntry(eventLogTable.getEventLogInput()).getEvent();
						EventLogInput eventLogInput = (EventLogInput)eventLogTable.getInput();
						
						boolean found = false;
						IMarker[] markers = eventLogInput.getFile().findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);

						for (IMarker marker : markers)
							if (marker.getAttribute("EventNumber", -1) == event.getEventNumber()) {
								marker.delete();
								found = true;
							}

						if (!found) {
							IMarker marker = eventLogInput.getFile().createMarker(IMarker.BOOKMARK);
							marker.setAttribute(IMarker.LOCATION, "# " + event.getEventNumber());
							marker.setAttribute("EventNumber", event.getEventNumber());
						}

						update();
						eventLogTable.redraw();
					}
				}
				catch (CoreException e) {
					throw new RuntimeException(e);
				}
			}

			@Override
			public void update() {
				setEnabled(eventLogTable.getSelectionElement() != null);
			}
		};
	}

	private EventLogTableMenuAction createDisplayModeAction() {
		return new EventLogTableMenuAction("Display mode", Action.AS_DROP_DOWN_MENU, ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_DISPLAY_MODE)) {
			private AbstractMenuCreator menuCreator;

			@Override
			public void run() {
				eventLogTable.setDisplayMode((getMenuIndex() + 1) % 2);
				eventLogTable.redraw();
				update();
			}

			@Override
			protected int getMenuIndex() {
				return eventLogTable.getDisplayMode();
			}

			@Override
			public IMenuCreator getMenuCreator() {
				if (menuCreator == null) {
					menuCreator = new AbstractMenuCreator() {
						@Override
						protected void createMenu(Menu menu) {
							addSubMenuItem(menu, "Descriptive", 0);
							addSubMenuItem(menu, "Raw", 1);
						}
	
						private void addSubMenuItem(Menu menu, String text, final int displayMode) {
							MenuItem subMenuItem = new MenuItem(menu, SWT.RADIO);
							subMenuItem.setText(text);
							subMenuItem.addSelectionListener( new SelectionAdapter() {
								public void widgetSelected(SelectionEvent e) {
									MenuItem menuItem = (MenuItem)e.widget;
									
									if (menuItem.getSelection()) {
										eventLogTable.setDisplayMode(displayMode);
										eventLogTable.redraw();
										update();
									}
								}
							});
						}
					};
				}
				
				return menuCreator;
			}
		};
	}

	private EventLogTableMenuAction createFilterModeAction() {
		return new EventLogTableMenuAction("Filter mode", Action.AS_DROP_DOWN_MENU, ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_FILTER)) {
			private AbstractMenuCreator menuCreator;

			@Override
			public void run() {
				eventLogTable.setFilterMode((eventLogTable.getFilterMode() + 1) % 5);
				eventLogTable.configureVerticalScrollBar();
				update();
			}

			@Override
			protected int getMenuIndex() {
				return eventLogTable.getFilterMode();
			}
			
			@Override
			public IMenuCreator getMenuCreator() {
				if (menuCreator == null) {
					menuCreator = new AbstractMenuCreator() {
						@Override
						protected void createMenu(Menu menu) {
							addSubMenuItem(menu, "All", 0);
							addSubMenuItem(menu, "Events, message sends and log messages", 1);
							addSubMenuItem(menu, "Events and log messages", 2);
							addSubMenuItem(menu, "Events", 3);
							addSubMenuItem(menu, "Custom filter...", new SelectionAdapter() {
								public void widgetSelected(SelectionEvent e) {
									MenuItem menuItem = (MenuItem)e.widget;
									
									if (menuItem.getSelection()) {
										InputDialog dialog = new InputDialog(null, "Search pattern", "Please enter the search pattern such as (BS and c(MyMessage))", null, null);
										dialog.open();
		
										String pattern = dialog.getValue();
										if (pattern == null || pattern.equals(""))
											pattern = "*";
		
										eventLogTable.setCustomFilter(pattern);
										eventLogTable.setFilterMode(4);
										update();
									}
								}
							});
						}
	
						private void addSubMenuItem(final Menu menu, String text, final int filterMode) {
							addSubMenuItem(menu, text, new SelectionAdapter() {
								public void widgetSelected(SelectionEvent e) {
									MenuItem menuItem = (MenuItem)e.widget;
									
									if (menuItem.getSelection()) {
										eventLogTable.setFilterMode(filterMode);
										update();
									}
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
				
				return menuCreator;
			}
		};
	}
	
	private abstract class EventLogTableAction extends Action {
		public EventLogTableAction(String text) {
			super(text);
		}

		public EventLogTableAction(String text, int style, ImageDescriptor image) {
			super(text, style);
			setImageDescriptor(image);
		}

		public void update() {
		}
	}

	private abstract class EventLogTableMenuAction extends EventLogTableAction {
		protected ArrayList<Menu> menus = new ArrayList<Menu>();

		public EventLogTableMenuAction(String text, int style, ImageDescriptor image) {
			super(text, style, image);
		}
		
		@Override
		public void update() {
			for (Menu menu : menus)
				if (!menu.isDisposed())
					updateMenu(menu);
		}
		
		protected void addMenu(Menu menu) {
			Assert.isTrue(menu != null);

			menus.add(menu);
			updateMenu(menu);
		}
		
		protected void removeMenu(Menu menu) {
			Assert.isTrue(menu != null);

			menus.remove(menu);
		}
		
		protected abstract int getMenuIndex();

		protected void updateMenu(Menu menu) {
			for (int i = 0; i < menu.getItemCount(); i++) {
				boolean selection = i == getMenuIndex();
				MenuItem menuItem = menu.getItem(i);

				if (menuItem.getSelection() != selection)
					menuItem.setSelection(selection);
			}
		}

		protected abstract class AbstractMenuCreator implements IMenuCreator {
			private Menu controlMenu;

			private Menu parentMenu;
		
			public void dispose() {
				if (controlMenu != null) {
					controlMenu.dispose();
					removeMenu(controlMenu);
				}

				if (parentMenu != null) {
					parentMenu.dispose();
					removeMenu(parentMenu);
				}
			}

			public Menu getMenu(Control parent) {
				if (controlMenu == null) {
					controlMenu = new Menu(parent);
					createMenu(controlMenu);
					addMenu(controlMenu);
				}
				
				return controlMenu;
			}

			public Menu getMenu(Menu parent) {
				if (parentMenu == null) {
					parentMenu = new Menu(parent);
					createMenu(parentMenu);
					addMenu(parentMenu);
				}
				
				return parentMenu;
			}

			protected abstract void createMenu(Menu menu);
		}
	}
}
