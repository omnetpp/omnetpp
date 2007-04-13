package org.omnetpp.sequencechart.editors;

import java.util.ArrayList;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuCreator;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.part.EditorActionBarContributor;
import org.eclipse.ui.texteditor.StatusLineContributionItem;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.MessageDependency;
import org.omnetpp.sequencechart.widgets.SequenceChart;


public class SequenceChartContributor extends EditorActionBarContributor {
	private static SequenceChartContributor singleton;

	protected SequenceChart sequenceChart;

	protected Separator separatorAction;

	protected SequenceChartMenuAction timelineModeAction;

	protected SequenceChartMenuAction axisOrderingModeAction;

	protected SequenceChartAction showEventNumbersAction;

	protected SequenceChartAction showMessageNamesAction;

	protected SequenceChartAction showReuseMessagesAction;

	protected SequenceChartAction showArrowHeadsAction;

	protected SequenceChartAction increaseSpacingAction;

	protected SequenceChartAction decreaseSpacingAction;

	protected SequenceChartAction zoomInAction;

	protected SequenceChartAction zoomOutAction;

	protected SequenceChartAction denseAxesAction;

	protected SequenceChartAction balancedAxesAction;

	protected StatusLineContributionItem timelineModeStatus;
	
	public SequenceChartContributor() {
		this.separatorAction = new Separator();
		this.timelineModeAction = createTimelineModeAction();
		this.axisOrderingModeAction = createAxisOrderingModeAction();
		this.showEventNumbersAction = createShowEventNumbersAction();
		this.showMessageNamesAction = createShowMessageNamesAction();
		this.showReuseMessagesAction = createShowReuseMessagesAction();
		this.showArrowHeadsAction = createShowArrowHeadsAction();
		this.increaseSpacingAction = createIncreaseSpacingAction();
		this.decreaseSpacingAction = createDecreaseSpacingAction();
		this.zoomInAction = createZoomInAction();
		this.zoomOutAction = createZoomOutAction();
		this.denseAxesAction = createDenseAxesAction();
		this.balancedAxesAction = createBalancedAxesAction();
		
		this.timelineModeStatus = createTimelineModeStatus();

		if (singleton == null)
			singleton = this;
	}
	
	public SequenceChartContributor(SequenceChart sequenceChart) {
		this();
		this.sequenceChart = sequenceChart;
	}
	
	public static SequenceChartContributor getDefault() {
		return singleton;
	}
	
	public void contributeToPopupMenu(IMenuManager menuManager) {
		menuManager.setRemoveAllWhenShown(true);
		menuManager.addMenuListener(new IMenuListener() {
			public void menuAboutToShow(IMenuManager menuManager) {
				// dynamic menu
				ArrayList<IEvent> events = new ArrayList<IEvent>();
				ArrayList<MessageDependency> msgs = new ArrayList<MessageDependency>();
				Point p = sequenceChart.toControl(sequenceChart.getDisplay().getCursorLocation());
				sequenceChart.collectStuffUnderMouse(p.x, p.y, events, msgs);

				// events submenu
				for (final IEvent event : events) {
					IMenuManager subMenuManager = new MenuManager(sequenceChart.getEventText(event));
					menuManager.add(subMenuManager);

					subMenuManager.add(createCenterEventAction(event));
					subMenuManager.add(createSelectEventAction(event));
					subMenuManager.add(createFilterEventCausesConsequencesAction(event));
				}

				if (events.size() != 0)
					menuManager.add(separatorAction);

				// messages submenu
				for (final MessageDependency msg : msgs) {
					IMenuManager subMenuManager = new MenuManager(sequenceChart.getMessageText(msg));
					menuManager.add(subMenuManager);

					subMenuManager.add(createZoomToMessageAction(msg));
					subMenuManager.add(createGotoCauseAction(msg));
					subMenuManager.add(createGotoConsequenceAction(msg));
				}
				
				if (msgs.size() != 0)
					menuManager.add(separatorAction);					

				// axis submenu
				final ModuleTreeItem axisModule = sequenceChart.findAxisAt(p.y);
				if (axisModule != null) {
					IMenuManager subMenuManager = new MenuManager(sequenceChart.getAxisText(axisModule));

					subMenuManager.add(createCenterAxisAction(axisModule));
					subMenuManager.add(createZoomToAxisValueAction(axisModule, p.x));
					menuManager.add(separatorAction);
				}

				// static menu
				menuManager.add(timelineModeAction);
				menuManager.add(axisOrderingModeAction);
				menuManager.add(separatorAction);
				menuManager.add(showEventNumbersAction);
				menuManager.add(showMessageNamesAction);
				menuManager.add(showReuseMessagesAction);
				menuManager.add(showArrowHeadsAction);
				menuManager.add(separatorAction);
				menuManager.add(increaseSpacingAction);
				menuManager.add(decreaseSpacingAction);
				menuManager.add(separatorAction);
				menuManager.add(zoomInAction);
				menuManager.add(zoomOutAction);
				menuManager.add(separatorAction);
				menuManager.add(denseAxesAction);
				menuManager.add(balancedAxesAction);
				menuManager.add(separatorAction);
			}
		});
	}

	@Override
	public void contributeToToolBar(IToolBarManager toolBarManager) {
		toolBarManager.add(timelineModeAction);
		toolBarManager.add(axisOrderingModeAction);
		toolBarManager.add(separatorAction);
		toolBarManager.add(showEventNumbersAction);
		toolBarManager.add(showMessageNamesAction);
		toolBarManager.add(separatorAction);
		toolBarManager.add(increaseSpacingAction);
		toolBarManager.add(decreaseSpacingAction);
		toolBarManager.add(separatorAction);
		toolBarManager.add(zoomInAction);
		toolBarManager.add(zoomOutAction);
	}

    public void contributeToStatusLine(IStatusLineManager statusLineManager) {
    	statusLineManager.add(timelineModeStatus);
    }
	
	@Override
	public void setActiveEditor(IEditorPart targetEditor) {
		sequenceChart = ((SequenceChartEditor)targetEditor).getSequenceChart();
		timelineModeAction.update();
		axisOrderingModeAction.update();
		showEventNumbersAction.update();
		showMessageNamesAction.update();
		showReuseMessagesAction.update();
		showArrowHeadsAction.update();
		timelineModeStatus.update();
	}

	private SequenceChartMenuAction createTimelineModeAction() {
		return new SequenceChartMenuAction("Timeline mode", Action.AS_DROP_DOWN_MENU, ImageFactory.getDescriptor(ImageFactory.SEQUENCE_CHART_IMAGE_TIMELINE_MODE)) {
			@Override
			public void run() {
				sequenceChart.setTimelineMode(SequenceChart.TimelineMode.values()[(sequenceChart.getTimelineMode().ordinal() + 1) % SequenceChart.TimelineMode.values().length]);
				timelineModeStatus.update();
				update();
			}

			@Override
			protected int getMenuIndex() {
				return sequenceChart.getTimelineMode().ordinal();
			}
			
			@Override
			public IMenuCreator getMenuCreator() {
				return new AbstractMenuCreator() {
					@Override
					protected void createMenu(Menu menu) {
						addSubMenuItem(menu, "Linear", SequenceChart.TimelineMode.LINEAR);
						addSubMenuItem(menu, "Step", SequenceChart.TimelineMode.STEP);
						addSubMenuItem(menu, "Non linear", SequenceChart.TimelineMode.NON_LINEAR);
					}

					private void addSubMenuItem(Menu menu, String text, final SequenceChart.TimelineMode timelineMode) {
						MenuItem subMenuItem = new MenuItem(menu, SWT.RADIO);
						subMenuItem.setText(text);
						subMenuItem.addSelectionListener( new SelectionAdapter() {
							public void widgetSelected(SelectionEvent e) {
								MenuItem menuItem = (MenuItem)e.widget;
								
								if (menuItem.getSelection()) {
									sequenceChart.setTimelineMode(timelineMode);
									timelineModeStatus.update();
									update();
								}
							}
						});
					}
				};
			}
		};
	}

	private SequenceChartMenuAction createAxisOrderingModeAction() {
		return new SequenceChartMenuAction("Axis ordering mode", Action.AS_DROP_DOWN_MENU, ImageFactory.getDescriptor(ImageFactory.SEQUENCE_CHART_IMAGE_AXIS_ORDERING_MODE)) {
			@Override
			public void run() {
				sequenceChart.setAxisOrderingMode(SequenceChart.AxisOrderingMode.values()[(sequenceChart.getAxisOrderingMode().ordinal() + 1) % SequenceChart.AxisOrderingMode.values().length]);
				update();
			}

			@Override
			protected int getMenuIndex() {
				return sequenceChart.getAxisOrderingMode().ordinal();
			}
			
			@Override
			public IMenuCreator getMenuCreator() {
				return new AbstractMenuCreator() {
					@Override
					protected void createMenu(Menu menu) {
						addSubMenuItem(menu, "Manual...", SequenceChart.AxisOrderingMode.MANUAL);
						addSubMenuItem(menu, "Module id", SequenceChart.AxisOrderingMode.MODULE_ID);
						addSubMenuItem(menu, "Module name", SequenceChart.AxisOrderingMode.MODULE_NAME);
						addSubMenuItem(menu, "Minimize crossings", SequenceChart.AxisOrderingMode.MINIMIZE_CROSSINGS);
						addSubMenuItem(menu, "Minimize crossings hierarchically", SequenceChart.AxisOrderingMode.MINIMIZE_CROSSINGS_HIERARCHICALLY);
					}

					private void addSubMenuItem(Menu menu, String text, final SequenceChart.AxisOrderingMode axisOrderingMode) {
						MenuItem subMenuItem = new MenuItem(menu, SWT.RADIO);
						subMenuItem.setText(text);
						subMenuItem.addSelectionListener( new SelectionAdapter() {
							public void widgetSelected(SelectionEvent e) {
								MenuItem menuItem = (MenuItem)e.widget;
								
								if (menuItem.getSelection()) {
									sequenceChart.setAxisOrderingMode(axisOrderingMode);
									update();
								}
							}
						});
					}
				};
			}
		};
	}
	
	private SequenceChartAction createShowEventNumbersAction() {
		return new SequenceChartAction(null, Action.AS_CHECK_BOX, ImageFactory.getDescriptor(ImageFactory.SEQUENCE_CHART_IMAGE_SHOW_EVENT_NUMBERS)) {
			@Override
			public void run() {
				sequenceChart.setShowEventNumbers(!sequenceChart.getShowEventNumbers());
				update();
			}
			
			@Override
			public void update() {
				boolean showEventNumbers = sequenceChart.getShowEventNumbers();
				setChecked(showEventNumbers);
				setText(showEventNumbers ? "Hide event numbers" : "Show event numbers");
			}
		};
	}

	private SequenceChartAction createShowMessageNamesAction() {
		return new SequenceChartAction(null, Action.AS_CHECK_BOX, ImageFactory.getDescriptor(ImageFactory.SEQUENCE_CHART_IMAGE_SHOW_MESSAGE_NAMES)) {
			@Override
			public void run() {
				sequenceChart.setShowMessageNames(!sequenceChart.getShowMessageNames());
				update();
			}
			
			@Override
			public void update() {
				boolean showMessageNames = sequenceChart.getShowMessageNames();
				setChecked(showMessageNames);
				setText(showMessageNames ? "Hide message names" : "Show message names");
			}
		};
	}
	
	private SequenceChartAction createShowReuseMessagesAction() {
		return new SequenceChartAction(null, Action.AS_CHECK_BOX, ImageFactory.getDescriptor(ImageFactory.SEQUENCE_CHART_IMAGE_SHOW_REUSE_MESSAGES)) {
			@Override
			public void run() {
				sequenceChart.setShowReuseMessages(!sequenceChart.getShowReuseMessages());
				update();
			}
			
			@Override
			public void update() {
				boolean showReuseMessage = sequenceChart.getShowReuseMessages();
				setChecked(showReuseMessage);
				setText(showReuseMessage ? "Hide reuse messages" : "Show reuse messages");
			}
		};
	}
	
	private SequenceChartAction createShowArrowHeadsAction() {
		return new SequenceChartAction(null, Action.AS_CHECK_BOX, ImageFactory.getDescriptor(ImageFactory.SEQUENCE_CHART_IMAGE_SHOW_ARROW_HEADS)) {
			@Override
			public void run() {
				sequenceChart.setShowArrowHeads(!sequenceChart.getShowArrowHeads());
				update();
			}
			
			@Override
			public void update() {
				boolean showArrowHeads = sequenceChart.getShowArrowHeads();
				setChecked(showArrowHeads);
				setText(showArrowHeads ? "Hide arrow heads" : "Show arrow heads");
			}
		};
	}
	
	private SequenceChartAction createIncreaseSpacingAction() {
		return new SequenceChartAction("Increase spacing", Action.AS_PUSH_BUTTON, ImageFactory.getDescriptor(ImageFactory.SEQUENCE_CHART_IMAGE_INCREASE_SPACING)) {
			@Override
			public void run() {
				sequenceChart.setAxisSpacing(sequenceChart.getAxisSpacing() + 5);
			}
		};
	}
	
	private SequenceChartAction createDecreaseSpacingAction() {
		return new SequenceChartAction("Decrease spacing", Action.AS_PUSH_BUTTON, ImageFactory.getDescriptor(ImageFactory.SEQUENCE_CHART_IMAGE_DECREASE_SPACING)) {
			@Override
			public void run() {
				sequenceChart.setAxisSpacing(sequenceChart.getAxisSpacing() - 5);
			}
		};
	}
	
	private SequenceChartAction createZoomInAction() {
		return new SequenceChartAction("Zoom in", Action.AS_PUSH_BUTTON, ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_ZOOMPLUS)) {
			@Override
			public void run() {
				sequenceChart.zoomIn();
			}
		};
	}

	private SequenceChartAction createZoomOutAction() {
		return new SequenceChartAction("Zoom out", Action.AS_PUSH_BUTTON, ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_ZOOMMINUS)) {
			@Override
			public void run() {
				sequenceChart.zoomOut();
			}
		};
	}

	private SequenceChartAction createDenseAxesAction() {
		return new SequenceChartAction("Dense axes", Action.AS_PUSH_BUTTON, ImageFactory.getDescriptor(ImageFactory.SEQUENCE_CHART_IMAGE_DENSE_AXES)) {
			@Override
			public void run() {
				sequenceChart.setAxisSpacing(16);
			}
		};
	}

	private SequenceChartAction createBalancedAxesAction() {
		return new SequenceChartAction("Balanced axes", Action.AS_PUSH_BUTTON, ImageFactory.getDescriptor(ImageFactory.SEQUENCE_CHART_IMAGE_BALANCED_AXES)) {
			@Override
			public void run() {
				sequenceChart.setAxisSpacing(-1);
			}
		};
	}

	private SequenceChartAction createCenterEventAction(final IEvent event) {
		return new SequenceChartAction("Center", SWT.PUSH) {
			@Override
			public void run() {
				sequenceChart.gotoElement(event);
			}
		};
	}

	private SequenceChartAction createSelectEventAction(final IEvent event) {
		return new SequenceChartAction("Select", SWT.PUSH) {
			@Override
			public void run() {
				sequenceChart.setSelectionEvent(event);
			}
		};
	}

	private SequenceChartAction createFilterEventCausesConsequencesAction(final IEvent event) {
		return new SequenceChartAction("Filter causes/consequences", SWT.PUSH) {
			@Override
			public void run() {
				// TODO:
			}
		};
	}

	private SequenceChartAction createZoomToMessageAction(final MessageDependency msg) {
		return new SequenceChartAction("Zoom to message", SWT.PUSH) {
			@Override
			public void run() {
				sequenceChart.zoomToSimulationTimeRange(msg.getCauseEvent().getSimulationTime().doubleValue(), msg.getConsequenceEvent().getSimulationTime().doubleValue(), (int)(sequenceChart.getViewportWidth() * 0.1));
			}
		};
	}

	private SequenceChartAction createGotoCauseAction(final MessageDependency msg) {
		return new SequenceChartAction("Goto cause event", SWT.PUSH) {
			@Override
			public void run() {
				sequenceChart.gotoElement(msg.getCauseEvent());
			}
		};
	}

	private SequenceChartAction createGotoConsequenceAction(final MessageDependency msg) {
		return new SequenceChartAction("Goto consequence event", SWT.PUSH) {
			@Override
			public void run() {
				sequenceChart.gotoElement(msg.getConsequenceEvent());
			}
		};
	}

	private SequenceChartAction createCenterAxisAction(final ModuleTreeItem axisModule) {
		return new SequenceChartAction("Center", SWT.PUSH) {
			@Override
			public void run() {
				sequenceChart.scrollToAxisModule(axisModule);
			}
		};
	}

	private SequenceChartAction createZoomToAxisValueAction(final ModuleTreeItem axisModule, final int x) {
		return new SequenceChartAction("Zoom to value", SWT.PUSH) {
			@Override
			public void run() {
				sequenceChart.zoomToAxisValue(axisModule, sequenceChart.getSimulationTimeForViewportCoordinate(x));
			}
		};
	}	

	private StatusLineContributionItem createTimelineModeStatus() {
		return new StatusLineContributionItem("Timeline mode") {
			@Override
		    public void update() {
				setText(sequenceChart.getTimelineMode().name());
		    }
		};
	}

	private abstract class SequenceChartAction extends Action {
		public SequenceChartAction(String text, int style) {			
			super(text, style);
		}

		public SequenceChartAction(String text, int style, ImageDescriptor image) {
			super(text, style);
			setImageDescriptor(image);
		}

		public void update() {
		}
	}
	
	private abstract class SequenceChartMenuAction extends SequenceChartAction {
		protected ArrayList<Menu> menus = new ArrayList<Menu>();

		public SequenceChartMenuAction(String text, int style, ImageDescriptor image) {
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
