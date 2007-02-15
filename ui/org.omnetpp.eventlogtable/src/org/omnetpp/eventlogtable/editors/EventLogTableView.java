package org.omnetpp.eventlogtable.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuCreator;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IViewSite;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.virtualtable.VirtualTableSelection;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlogtable.selection.EventLogSelection;
import org.omnetpp.eventlogtable.widgets.EventLogTable;
import org.omnetpp.eventlogtable.widgets.EventLogTableItemProvider;

/**
 * View for displaying and navigating simulation events and associated log messages.
 */
public class EventLogTableView extends ViewPart {
	protected EventLogTable eventLogTable;

	@Override
	public void createPartControl(Composite parent) {
		eventLogTable = new EventLogTable(parent);
		
		IViewSite viewSite = (IViewSite)getSite();
		IToolBarManager toolBarManager = viewSite.getActionBars().getToolBarManager();
		addSetFilterModeAction(toolBarManager);
		addSetDisplayModeAction(toolBarManager);

		// we want to provide selection for the sequence chart tool (an IEditPart)
		viewSite.setSelectionProvider(eventLogTable);

		// follow selection
		viewSite.getPage().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != eventLogTable && selection instanceof EventLogSelection) {
					EventLogSelection eventLogSelection = (EventLogSelection)selection;
					List<EventLogEntry> eventLogEntries = new ArrayList<EventLogEntry>();
					for (IEvent event : eventLogSelection.getEvents())
						eventLogEntries.add(event.getEventEntry());
					eventLogTable.setSelection(new VirtualTableSelection<EventLogEntry>(eventLogSelection.getInput(), eventLogEntries));
				}
			}
		});
		
		// bootstrap with current selection
		eventLogTable.setSelection(getSite().getSelectionProvider().getSelection());
	}

	private void addSetFilterModeAction(IToolBarManager toolBarManager) {
		Action action = new Action("Show", Action.AS_DROP_DOWN_MENU) {
			@Override
			public void run() {
				eventLogTable.setFilterMode((eventLogTable.getFilterMode() + 1) % 5);
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
									
								eventLogTable.getEventLogTableContentProvider().setCustomFilter(pattern);
								eventLogTable.setFilterMode(4);
							}
						});
					}

					private void addSubMenuItem(Menu menu, String text, final int filterMode) {
						addSubMenuItem(menu, text, new SelectionAdapter() {
							public void widgetSelected(SelectionEvent e) {
								eventLogTable.setFilterMode(filterMode);
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
				EventLogTableItemProvider eventLogTableItemProvider = (EventLogTableItemProvider)eventLogTable.getLabelProvider();
				eventLogTableItemProvider.setDisplayMode(EventLogTableItemProvider.DisplayMode.values()[(eventLogTableItemProvider.getDisplayMode().ordinal() + 1) % EventLogTableItemProvider.DisplayMode.values().length]);
				eventLogTable.refresh();
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
								EventLogTableItemProvider eventLogTableItemProvider = (EventLogTableItemProvider)eventLogTable.getLabelProvider();
								eventLogTableItemProvider.setDisplayMode(displayMode);
								eventLogTable.refresh();
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
	
	@Override
	public void setFocus() {
		eventLogTable.getControl().setFocus();
	}
}
