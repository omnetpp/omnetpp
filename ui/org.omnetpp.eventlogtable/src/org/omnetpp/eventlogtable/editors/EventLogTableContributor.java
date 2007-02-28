package org.omnetpp.eventlogtable.editors;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuCreator;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.part.EditorActionBarContributor;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.eventlogtable.widgets.EventLogTable;
import org.omnetpp.eventlogtable.widgets.EventLogTableLineRenderer;


public class EventLogTableContributor extends EditorActionBarContributor {
	protected EventLogTable eventLogTable;
	
	@Override
	public void contributeToToolBar(IToolBarManager toolBarManager) {
		addFilterModeAction(toolBarManager);
		addDisplayModeAction(toolBarManager);		
	}
	
	@Override
	public void setActiveEditor(IEditorPart targetEditor) {
		eventLogTable = ((EventLogTableEditor)targetEditor).getEventLogTable();
	}

	private void addDisplayModeAction(IToolBarManager toolBarManager) {
		Action action = new Action("Display", Action.AS_DROP_DOWN_MENU) {
			@Override
			public void run() {
				EventLogTableLineRenderer eventLogTableItemProvider = (EventLogTableLineRenderer)eventLogTable.getLineRenderer();
				eventLogTableItemProvider.setDisplayMode(EventLogTableLineRenderer.DisplayMode.values()[(eventLogTableItemProvider.getDisplayMode().ordinal() + 1) % EventLogTableLineRenderer.DisplayMode.values().length]);
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
								eventLogTable.refresh();
							}
						});
					}
				};
			}
		};

		action.setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_DISPLAY_MODE));
		toolBarManager.add(action);
	}

	private void addFilterModeAction(IToolBarManager toolBarManager) {
		Action action = new Action("Show", Action.AS_DROP_DOWN_MENU) {
			@Override
			public void run() {
				eventLogTable.setFilterMode((eventLogTable.getFilterMode() + 1) % 5);
				eventLogTable.getVirtualTable().updateVerticalBarParameters();
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

		action.setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_FILTER));
		toolBarManager.add(action);
	}
}
