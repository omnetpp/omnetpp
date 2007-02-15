package org.omnetpp.eventlogtable.widgets;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuCreator;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.IActionBars;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

public class EventLogTable extends VirtualTable<EventLogEntry> {
	public EventLogTable(Composite parent, IActionBars actionBars) {
		super(new Table(parent, SWT.FULL_SELECTION | SWT.VIRTUAL | SWT.MULTI));
		
		table.setHeaderVisible(true);
		table.setLinesVisible(false);

		TableColumn tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(60);
		tableColumn.setText("Event#");

		tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(140);
		tableColumn.setText("Time");

		tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(800);
		tableColumn.setText("Details / log message");

		setContentProvider(new EventLogTableContentProvider());
		setInput(null);
		setLabelProvider(new EventLogTableItemProvider());

		IToolBarManager toolBarManager = actionBars.getToolBarManager();
		addSetFilterModeAction(toolBarManager);
		addSetDisplayModeAction(toolBarManager);
		addPopupMenu();
	}

	public IEventLog getEventLog() {
		return (IEventLog)getInput();
	}

	private void addSetFilterModeAction(IToolBarManager toolBarManager) {
		Action action = new Action("Show", Action.AS_DROP_DOWN_MENU) {
			@Override
			public void run() {
				setFilterMode((getFilterMode() + 1) % 5);
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
						addSubMenuItem(menu, "Custom filter", new SelectionAdapter() {
							public void widgetSelected(SelectionEvent e) {
								InputDialog dialog = new InputDialog(null, "Search patter", "Please enter the search pattern such as (BS and c(MyMessage))", null, null);
								dialog.open();

								String pattern = dialog.getValue();
								if (pattern == null || pattern.equals(""))
									pattern = "*";
									
								getEventLogTableContentProvider().setCustomFilter(pattern);
								setFilterMode(4);
							}
						});
					}

					private void addSubMenuItem(Menu menu, String text, final int filterMode) {
						addSubMenuItem(menu, text, new SelectionAdapter() {
							public void widgetSelected(SelectionEvent e) {
								setFilterMode(filterMode);
							}
						});
					}

					private void addSubMenuItem(Menu menu, String text, SelectionListener adapter) {
						MenuItem subMenuItem = new MenuItem(menu, SWT.PUSH);
						subMenuItem.setText(text);
						subMenuItem.addSelectionListener(adapter);
					}
				};
			}
		};
		// TODO: find correct icon
		action.setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_TEMPLATE));
		toolBarManager.add(action);
	}

	private void addSetDisplayModeAction(IToolBarManager toolBarManager) {
		Action action = new Action("Display", Action.AS_DROP_DOWN_MENU) {
			@Override
			public void run() {
				EventLogTableItemProvider eventLogTableItemProvider = (EventLogTableItemProvider)getLabelProvider();
				eventLogTableItemProvider.setDisplayMode(EventLogTableItemProvider.DisplayMode.values()[(eventLogTableItemProvider.getDisplayMode().ordinal() + 1) % EventLogTableItemProvider.DisplayMode.values().length]);
				refresh();
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
						addSubMenuItem(menu, "Descriptive", EventLogTableItemProvider.DisplayMode.DESCRIPTIVE);
						addSubMenuItem(menu, "Raw", EventLogTableItemProvider.DisplayMode.RAW);
					}

					private void addSubMenuItem(Menu menu, String text, final EventLogTableItemProvider.DisplayMode displayMode) {
						MenuItem subMenuItem = new MenuItem(menu, SWT.PUSH);
						subMenuItem.setText(text);
						subMenuItem.addSelectionListener( new SelectionAdapter() {
							public void widgetSelected(SelectionEvent e) {
								EventLogTableItemProvider eventLogTableItemProvider = (EventLogTableItemProvider)getLabelProvider();
								eventLogTableItemProvider.setDisplayMode(displayMode);
								refresh();
							}
						});
					}
				};
			}
		};
		// TODO: find correct icon
		action.setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_TEMPLATE));
		toolBarManager.add(action);		
	}

	private void addPopupMenu() {
		Menu popupMenu = new Menu(getControl());

		// find menu item
		MenuItem subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Search text");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				activateSearchText();
			}
		});

		// goto event menu item
		subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Goto event");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				activateGotoEvent();
			}
		});

		getControl().setMenu(popupMenu);
	}

	private void activateSearchText() {
		InputDialog dialog = new InputDialog(null, "Search text", "Please enter the search text", null, null);
		dialog.open();

		// TODO:
		//String searchText = dialog.getValue();
		//IEventLog eventLog = getEventLog();
	}

	private void activateGotoEvent() {
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

	public EventLogTableContentProvider getEventLogTableContentProvider() {
		return (EventLogTableContentProvider)getContentProvider();
	}

	public int getFilterMode() {
		return getEventLogTableContentProvider().getFilterMode();
	}

	public void setFilterMode(int i) {
		getEventLogTableContentProvider().setFilterMode(i);
		fixPointElement = getEventLogTableContentProvider().getClosestElement(fixPointElement);
		refresh();
	}

	public Table getTable() {
		return table;
	}
}
