/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.editors;

import java.lang.reflect.Field;
import java.util.ArrayList;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ControlContribution;
import org.eclipse.jface.action.IMenuCreator;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.window.Window;
import org.eclipse.osgi.util.NLS;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ContributionItemFactory;
import org.eclipse.ui.handlers.HandlerUtil;
import org.eclipse.ui.internal.Workbench;
import org.eclipse.ui.keys.IBindingService;
import org.eclipse.ui.menus.CommandContributionItem;
import org.eclipse.ui.menus.CommandContributionItemParameter;
import org.eclipse.ui.part.EditorActionBarContributor;
import org.eclipse.ui.texteditor.StatusLineContributionItem;
import org.omnetpp.animation.AnimationPlugin;
import org.omnetpp.animation.widgets.AnimationCanvas;
import org.omnetpp.animation.widgets.AnimationConfigurationDialog;
import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.animation.widgets.AnimationPosition;
import org.omnetpp.animation.widgets.IAnimationListener;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.GotoEventDialog;
import org.omnetpp.common.eventlog.GotoSimulationTimeDialog;
import org.omnetpp.common.eventlog.IEventLogChangeListener;
import org.omnetpp.common.eventlog.IFollowSelectionSupport;
import org.omnetpp.common.image.ImageFactory;

public class AnimationContributor extends EditorActionBarContributor implements ISelectionChangedListener, IEventLogChangeListener {
    public final static String TOOL_IMAGE_DIR = "icons/full/etool16/";
    public final static String IMAGE_ANIMATION_MODE = TOOL_IMAGE_DIR + "animationmode.png";
    public final static String IMAGE_SPEED_UP = TOOL_IMAGE_DIR + "speedup.gif";
    public final static String IMAGE_SPEED_DOWN = TOOL_IMAGE_DIR + "speeddown.gif";
    public final static String IMAGE_GOTO_BEGIN = TOOL_IMAGE_DIR + "gotobegin.gif";
    public final static String IMAGE_GOTO_END = TOOL_IMAGE_DIR + "gotoend.gif";
    public final static String IMAGE_PLAY_FORWARD = TOOL_IMAGE_DIR + "playforward.gif";
    public final static String IMAGE_STEP_FORWARD = TOOL_IMAGE_DIR + "stepforward.gif";
    public final static String IMAGE_PLAY_BAKWARD = TOOL_IMAGE_DIR + "playbackward.gif";
    public final static String IMAGE_STEP_BACKWARD = TOOL_IMAGE_DIR + "stepbackward.gif";
    public final static String IMAGE_STOP = TOOL_IMAGE_DIR + "stop.gif";

    public final static String IMAGE_SCHEDULE_MESSAGE = TOOL_IMAGE_DIR + "ScheduleMessage.png";
    public final static String IMAGE_SCHEDULE_SELF_MESSAGE = TOOL_IMAGE_DIR + "ScheduleSelfMessage.png";
    public final static String IMAGE_SEND_MESSAGE = TOOL_IMAGE_DIR + "SendMessage.png";
    public final static String IMAGE_SEND_SELF_MESSAGE = TOOL_IMAGE_DIR + "SendSelfMessage.png";
    public final static String IMAGE_HANDLE_MESSAGE = TOOL_IMAGE_DIR + "HandleMessage.png";

    private static AnimationContributor singleton;
	private AnimationCanvas animationCanvas;

	private Separator separatorAction;
    private AnimationAction animationModeAction;
    private AnimationAction speedUpAction;
    private AnimationAction speedDownAction;
    private AnimationAction gotoBeginAction;
    private AnimationAction gotoEndAction;
    private AnimationAction playForwardAction;
    private AnimationAction stepForwardAction;
    private AnimationAction playBackwardAction;
    private AnimationAction stepBackwardAction;
    private AnimationAction stopAction;
    private AnimationAction copyToClipboardAction;
    private AnimationAction refreshAction;
    private AnimationAction layoutAction;
    private AnimationAction configureAction;
    private AnimationAction pinAction;
    private StatusLineContributionItem animationStatus;
    private AnimationPositionContribution animationPositionContribution;

	/*************************************************************************************
	 * CONSTRUCTION
	 */

	public AnimationContributor() {
		this.separatorAction = new Separator();
		this.animationModeAction = createAnimationModeAction();
		this.speedUpAction = createSpeedUpActionAction();
        this.speedDownAction = createSpeedDownActionAction();
		this.gotoBeginAction = createGotoBeginAction();
        this.gotoEndAction = createGotoEndAction();
        this.playForwardAction = createPlayForwardAction();
        this.stepForwardAction = createStepForwardAction();
        this.playBackwardAction = createPlayBackwardAction();
        this.stepBackwardAction = createStepBackardAction();
        this.stopAction = createStopAction();
        this.copyToClipboardAction = createCopyToClipboardAction();
        this.refreshAction = createRefreshAction();
        this.layoutAction = createLayoutAction();
        this.configureAction = createConfigureAction();
        this.pinAction = createPinAction();
		this.animationStatus = createAnimationStatus();
		this.animationPositionContribution = createAnimationPositionContribution();

		if (singleton == null)
			singleton = this;
	}

    public AnimationContributor(AnimationCanvas animationCanvas) {
		this();
		this.animationCanvas = animationCanvas;
		animationCanvas.addSelectionChangedListener(this);
	}

    public static AnimationContributor getDefault() {
        if (singleton == null)
            singleton = new AnimationContributor();

        return singleton;
    }

	@Override
	public void dispose() {
	    if (animationCanvas != null)
	        animationCanvas.removeSelectionChangedListener(this);

	    animationCanvas = null;
		singleton = null;

		super.dispose();
	}

	/*************************************************************************************
	 * CONTRIBUTIONS
	 */

	public void contributeToPopupMenu(IMenuManager menuManager) {
		menuManager.setRemoveAllWhenShown(true);
		menuManager.addMenuListener(new IMenuListener() {
			public void menuAboutToShow(IMenuManager menuManager) {
                // Show In submenu
                MenuManager showInSubmenu = new MenuManager(getShowInMenuLabel());
                IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                showInSubmenu.add(ContributionItemFactory.VIEWS_SHOW_IN.create(workbenchWindow));

                // context menu static part
                // TODO: menuManager.add(animationModeAction);
                menuManager.add(separatorAction);

                IMenuManager subMenuManager = new MenuManager("Speed");
                menuManager.add(subMenuManager);
                subMenuManager.add(speedUpAction);
                subMenuManager.add(speedDownAction);

                subMenuManager = new MenuManager("Play");
                menuManager.add(subMenuManager);

                subMenuManager.add(gotoBeginAction);
                subMenuManager.add(playBackwardAction);
                subMenuManager.add(stepBackwardAction);
                subMenuManager.add(stopAction);
                subMenuManager.add(stepForwardAction);
                subMenuManager.add(playForwardAction);
                subMenuManager.add(gotoEndAction);
                subMenuManager.add(separatorAction);

                subMenuManager = new MenuManager("Go To");
                menuManager.add(subMenuManager);
                subMenuManager.add(createGotoEventCommandContributionItem());
                subMenuManager.add(createGotoSimulationTimeCommandContributionItem());

                menuManager.add(separatorAction);
                menuManager.add(configureAction);
                menuManager.add(separatorAction);
                menuManager.add(pinAction);
                menuManager.add(copyToClipboardAction);
                menuManager.add(refreshAction);
                //menuManager.add(layoutAction);
                menuManager.add(separatorAction);

                menuManager.add(showInSubmenu);
			}
		});
	}

	@Override
	public void contributeToToolBar(IToolBarManager toolBarManager) {
	    contributeToToolBar(toolBarManager, false);
	}

	public void contributeToToolBar(IToolBarManager toolBarManager, boolean view) {
        // TODO: doesn't work at the moment
	    // toolBarManager.add(animationModeAction);
        toolBarManager.add(separatorAction);
        toolBarManager.add(speedUpAction);
        toolBarManager.add(speedDownAction);
        toolBarManager.add(separatorAction);
        toolBarManager.add(gotoBeginAction);
        toolBarManager.add(playBackwardAction);
        toolBarManager.add(stepBackwardAction);
        toolBarManager.add(stopAction);
        toolBarManager.add(stepForwardAction);
        toolBarManager.add(playForwardAction);
        toolBarManager.add(gotoEndAction);
        toolBarManager.add(separatorAction);
        toolBarManager.add(refreshAction);
        if (view)
            toolBarManager.add(pinAction);
        toolBarManager.add(animationPositionContribution);
	}

    @Override
    public void contributeToStatusLine(IStatusLineManager statusLineManager) {
        // TODO: remove? AnimationTimeline shows the same information already...
    	// statusLineManager.add(animationStatus);
    }

	@Override
	public void setActiveEditor(IEditorPart targetEditor) {
		if (targetEditor instanceof AnimationEditor) {
			EventLogInput eventLogInput;
			if (animationCanvas != null) {
				eventLogInput = animationCanvas.getInput();
				if (eventLogInput != null)
					eventLogInput.removeEventLogChangedListener(this);

				animationCanvas.removeSelectionChangedListener(this);
				// TODO: animationCanvas.getAnimationController().removeAnimationListener(listener);
			}

			animationCanvas = ((AnimationEditor)targetEditor).getAnimationCanvas();

			eventLogInput = animationCanvas.getInput();
			if (eventLogInput != null)
				eventLogInput.addEventLogChangedListener(this);

			animationCanvas.addSelectionChangedListener(this);
			animationCanvas.getAnimationController().addAnimationListener(new IAnimationListener() {
	            public void runningStateChanged(boolean isRunning) {
	                update();
	            }

	            public void animationPositionChanged(AnimationPosition animationPosition) {
	                update();
	            }
	        });

			update();
            animationPositionContribution.configureSlider();
		}
		else
		    animationCanvas = null;
	}

	public void update() {
		try {
			for (Field field : getClass().getDeclaredFields()) {
				Class<?> fieldType = field.getType();

				if (fieldType == AnimationAction.class ||
					fieldType == AnimationMenuAction.class)
				{
					AnimationAction fieldValue = (AnimationAction)field.get(this);

					if (fieldValue != null && animationCanvas != null) {
						fieldValue.setEnabled(true);
						fieldValue.update();
						if (animationCanvas.getInput().isLongRunningOperationInProgress())
							fieldValue.setEnabled(false);
					}
				}
				if (fieldType == StatusLineContributionItem.class) {
					StatusLineContributionItem fieldValue = (StatusLineContributionItem)field.get(this);
					if (animationCanvas != null)
						fieldValue.update();
				}
                if (fieldType == AnimationPositionContribution.class) {
                    AnimationPositionContribution fieldValue = (AnimationPositionContribution)field.get(this);
                    if (animationCanvas != null)
                        fieldValue.update();
                }
			}
		}
		catch (Exception e) {
			throw new RuntimeException(e);
		}
	}

    private String getShowInMenuLabel() {
        String keyBinding = null;

        IBindingService bindingService = (IBindingService)PlatformUI.getWorkbench().getAdapter(IBindingService.class);
        if (bindingService != null)
            keyBinding = bindingService.getBestActiveBindingFormattedFor("org.eclipse.ui.navigate.showInQuickMenu");

        if (keyBinding == null)
            keyBinding = "";

        return NLS.bind("Show In \t{0}", keyBinding);
    }

    /*************************************************************************************
	 * NOTIFICATIONS
	 */

    public void selectionChanged(SelectionChangedEvent event) {
        update();
    }

    public void eventLogAppended() {
		// void
	}

    public void eventLogOverwritten() {
        // void
    }

    public void eventLogFilterRemoved() {
		update();
	}

	public void eventLogFiltered() {
		update();
	}

	public void eventLogLongOperationEnded() {
		update();
	}

	public void eventLogLongOperationStarted() {
		update();
	}

	public void eventLogProgress() {
		// void
	}

	/*************************************************************************************
	 * ACTIONS
	 */

    private AnimationAction createSpeedUpActionAction() {
        return new AnimationAction("Speed Up", Action.AS_PUSH_BUTTON, AnimationPlugin.getImageDescriptor(IMAGE_SPEED_UP)) {
            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().speedUp();
            }
        };
    }

    private AnimationAction createSpeedDownActionAction() {
        return new AnimationAction("Speed Down", Action.AS_PUSH_BUTTON, AnimationPlugin.getImageDescriptor(IMAGE_SPEED_DOWN)) {
            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().speedDown();
            }
        };
    }

    private AnimationAction createGotoBeginAction() {
        return new AnimationAction("Goto Begin", Action.AS_PUSH_BUTTON, AnimationPlugin.getImageDescriptor(IMAGE_GOTO_BEGIN)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getAnimationController().isAtAnimationBegin());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().gotoAnimationBegin();
            }
        };
    }

    private AnimationAction createGotoEndAction() {
        return new AnimationAction("Goto End", Action.AS_PUSH_BUTTON, AnimationPlugin.getImageDescriptor(IMAGE_GOTO_END)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getAnimationController().isAtAnimationEnd());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().gotoAnimationEnd();
            }
        };
    }

    private AnimationAction createPlayForwardAction() {
        return new AnimationAction("Play Forward", Action.AS_PUSH_BUTTON, AnimationPlugin.getImageDescriptor(IMAGE_PLAY_FORWARD)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getAnimationController().isAtAnimationEnd());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().playAnimationForward();
            }
        };
    }

    private AnimationAction createStepForwardAction() {
        return new AnimationAction("Step Forward", Action.AS_PUSH_BUTTON, AnimationPlugin.getImageDescriptor(IMAGE_STEP_FORWARD)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getAnimationController().isAtAnimationEnd());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().stepAnimationForwardToNextAnimationChange();
            }
        };
    }

    private AnimationAction createPlayBackwardAction() {
        return new AnimationAction("Play Backward", Action.AS_PUSH_BUTTON, AnimationPlugin.getImageDescriptor(IMAGE_PLAY_BAKWARD)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getAnimationController().isAtAnimationBegin());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().playAnimationBackward();
            }
        };
    }

    private AnimationAction createStepBackardAction() {
        return new AnimationAction("Step Backward", Action.AS_PUSH_BUTTON, AnimationPlugin.getImageDescriptor(IMAGE_STEP_BACKWARD)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getAnimationController().isAtAnimationBegin());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().stepAnimationBackwardToPreviousAnimationChange();
            }
        };
    }

    private AnimationAction createStopAction() {
        return new AnimationAction("Stop", Action.AS_PUSH_BUTTON, AnimationPlugin.getImageDescriptor(IMAGE_STOP)) {
            @Override
            public void update() {
                setEnabled(animationCanvas.getAnimationController().isRunning());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().stopAnimation();
            }
        };
    }

    private AnimationMenuAction createAnimationModeAction() {
        return new AnimationMenuAction("Animation Mode", Action.AS_DROP_DOWN_MENU, AnimationPlugin.getImageDescriptor(IMAGE_ANIMATION_MODE)) {
            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().setAnimationMode(EventLogInput.TimelineMode.values()[(animationCanvas.getAnimationController().getAnimationMode().ordinal() + 1) % EventLogInput.TimelineMode.values().length]);
                update();
            }

            @Override
            protected int getMenuIndex() {
                return animationCanvas.getAnimationController().getAnimationMode().ordinal();
            }

            @Override
            public IMenuCreator getMenuCreator() {
                return new AbstractMenuCreator() {
                    @Override
                    protected void createMenu(Menu menu) {
                        addSubMenuItem(menu, "Linear", EventLogInput.TimelineMode.SIMULATION_TIME);
                        addSubMenuItem(menu, "Event number", EventLogInput.TimelineMode.EVENT_NUMBER);
                        addSubMenuItem(menu, "Step", EventLogInput.TimelineMode.STEP);
                        addSubMenuItem(menu, "Nonlinear", EventLogInput.TimelineMode.NONLINEAR);
                    }

                    private void addSubMenuItem(Menu menu, String text, final EventLogInput.TimelineMode timelineMode) {
                        MenuItem subMenuItem = new MenuItem(menu, SWT.RADIO);
                        subMenuItem.setText(text);
                        subMenuItem.addSelectionListener( new SelectionAdapter() {
                            @Override
                            public void widgetSelected(SelectionEvent e) {
                                MenuItem menuItem = (MenuItem)e.widget;

                                if (menuItem.getSelection()) {
                                    animationCanvas.getAnimationController().setAnimationMode(timelineMode);
                                    update();
                                }
                            }
                        });
                    }
                };
            }
        };
    }

    private CommandContributionItem createGotoEventCommandContributionItem() {
        return new CommandContributionItem(new CommandContributionItemParameter(Workbench.getInstance(), null, "org.omnetpp.animation.gotoEvent", SWT.PUSH));
    }

    public static class GotoEventHandler extends AbstractHandler {
        // TODO: setEnabled(!getEventLog().isEmpty());
        public Object execute(ExecutionEvent executionEvent) throws ExecutionException {
            IWorkbenchPart part = HandlerUtil.getActivePartChecked(executionEvent);
            if (part instanceof IAnimationCanvasProvider) {
                AnimationCanvas animationCanvas = ((IAnimationCanvasProvider)part).getAnimationCanvas();
                GotoEventDialog dialog = new GotoEventDialog(animationCanvas.getEventLog());
                if (dialog.open() == Window.OK)
                    animationCanvas.getAnimationController().gotoEventNumber(dialog.getEvent().getEventNumber());
            }
            return null;
        }
    }

    private CommandContributionItem createGotoSimulationTimeCommandContributionItem() {
        return new CommandContributionItem(new CommandContributionItemParameter(Workbench.getInstance(), null, "org.omnetpp.animation.gotoSimulationTime", SWT.PUSH));
    }

    public static class GotoSimulationTimeHandler extends AbstractHandler {
        // TODO: setEnabled(!getEventLog().isEmpty());
        public Object execute(ExecutionEvent executionEvent) throws ExecutionException {
            IWorkbenchPart part = HandlerUtil.getActivePartChecked(executionEvent);
            if (part instanceof IAnimationCanvasProvider) {
                AnimationCanvas animationCanvas = ((IAnimationCanvasProvider)part).getAnimationCanvas();
                GotoSimulationTimeDialog dialog = new GotoSimulationTimeDialog(animationCanvas.getEventLog(), animationCanvas.getAnimationController().getSimulationTime());
                if (dialog.open() == Window.OK)
                    animationCanvas.getAnimationController().gotoSimulationTime(dialog.getSimulationTime());
            }
            return null;
        }
    }

    private AnimationAction createCopyToClipboardAction() {
        return new AnimationAction("Copy to Clipboard", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                animationCanvas.copyToClipboard();
            }
        };
    }

    private AnimationAction createRefreshAction() {
        return new AnimationAction("Refresh", Action.AS_PUSH_BUTTON, ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_REFRESH)) {
            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().refreshAnimation();
            }
        };
    }

    private AnimationAction createLayoutAction() {
        return new AnimationAction("Layout", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                // TODO:
            }
        };
    }

    private AnimationAction createConfigureAction() {
        return new AnimationAction("Configure...", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                AnimationController animationController = animationCanvas.getAnimationController();
                AnimationConfigurationDialog configurationDialog = new AnimationConfigurationDialog(Display.getCurrent().getActiveShell(), animationController.getAnimationParameters());
                org.omnetpp.common.engine.BigDecimal simulationTime = animationController.getSimulationTime();
                if (configurationDialog.open() == Window.OK) {
                    animationController.reloadAnimationPrimitives();
                    animationController.gotoSimulationTime(simulationTime);
                    animationPositionContribution.configureSlider();
                }
            }
        };
    }

    private AnimationAction createPinAction() {
        return new AnimationAction("Pin", Action.AS_CHECK_BOX, ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_UNPIN)) {
            @Override
            protected void doRun() {
                IWorkbenchPart workbenchPart = animationCanvas.getWorkbenchPart();
                if (workbenchPart instanceof IFollowSelectionSupport) {
                    IFollowSelectionSupport followSelectionSupport = (IFollowSelectionSupport)workbenchPart;
                    followSelectionSupport.setFollowSelection(!followSelectionSupport.getFollowSelection());
                }
            }

            @Override
            public void update() {
                IWorkbenchPart workbenchPart = animationCanvas.getWorkbenchPart();
                if (workbenchPart instanceof IFollowSelectionSupport)
                    setChecked(!((IFollowSelectionSupport)workbenchPart).getFollowSelection());
            }
        };
    }

    private StatusLineContributionItem createAnimationStatus() {
        return new StatusLineContributionItem("Animation", true, 64) {
			@Override
		    public void update() {
			    AnimationController animationController = animationCanvas.getAnimationController();
				setText("#" + animationController.getEventNumber() + " " + animationController.getSimulationTime() + "s " + animationController.getAnimationTime());
		    }
		};
	}

    private AnimationPositionContribution createAnimationPositionContribution() {
        return new AnimationPositionContribution("AnimationPositionContribution");
    }

    private class AnimationPositionContribution extends ControlContribution {
        private Slider slider;

        private AnimationPositionContribution(String id) {
            super(id);
        }

        @Override
        protected Control createControl(Composite parent) {
            slider = new Slider(parent, SWT.NONE);
            slider.addSelectionListener(new SelectionListener() {
                public void widgetSelected(SelectionEvent e) {
                    // NOTE: this must be the first line to avoid circular selection change
                    int selection = slider.getSelection();
                    AnimationController animationController = animationCanvas.getAnimationController();
                    animationController.stopAnimation();
                    animationController.gotoAnimationTime(Math.min(selection, animationController.getAnimationEnd().getAnimationTime()));
                    AnimationContributor.this.update();
                }

                public void widgetDefaultSelected(SelectionEvent e) {
                }
            });
            configureSlider();
            return slider;
        }

        @Override
        public void update() {
            super.update();
            if (slider != null && !slider.isDisposed())
                slider.setSelection((int)animationCanvas.getAnimationController().getAnimationTime());
        }

        public void configureSlider() {
            if (slider != null && animationCanvas != null) {
                AnimationController animationController = animationCanvas.getAnimationController();
                if (animationController != null)
                    slider.setValues((int)animationController.getAnimationTime(), 0, (int)Math.ceil(animationController.getAnimationEnd().getAnimationTime()) + 1, 1, 1, 10);
            }
        }
    }

    /*************************************************************************************
     * CLASSES
     */

    private abstract class AnimationAction extends Action {
        public AnimationAction(String text, int style) {
            super(text, style);
        }

		public AnimationAction(String text, int style, ImageDescriptor image) {
			super(text, style);
			setImageDescriptor(image);
		}

		public void update() {
		}

		@Override
		public void run() {
	        try {
	            doRun();
	        }
	        catch (Exception e) {
	            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
	            AnimationPlugin.logError(e);
	        }
		}

        protected abstract void doRun();
	}

	private abstract class AnimationMenuAction extends AnimationAction {
		protected ArrayList<Menu> menus = new ArrayList<Menu>();

		public AnimationMenuAction(String text, int style, ImageDescriptor image) {
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
