/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

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
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ContributionItemFactory;
import org.eclipse.ui.handlers.HandlerUtil;
import org.eclipse.ui.keys.IBindingService;
import org.eclipse.ui.menus.CommandContributionItem;
import org.eclipse.ui.menus.CommandContributionItemParameter;
import org.eclipse.ui.part.EditorActionBarContributor;
import org.eclipse.ui.texteditor.StatusLineContributionItem;
import org.omnetpp.animation.AnimationCorePlugin;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.controller.IAnimationListener;
import org.omnetpp.animation.widgets.AnimationCanvas;
import org.omnetpp.common.eventlog.GotoEventDialog;
import org.omnetpp.common.eventlog.GotoSimulationTimeDialog;
import org.omnetpp.common.eventlog.IFollowSelectionSupport;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.eventlog.engine.IEvent;

/**
 * Contributes context menu and toolbar items to the platform.
 *
 * @author levy
 */
public class AnimationContributor extends EditorActionBarContributor implements IPartListener, ISelectionChangedListener, IAnimationListener, IMenuListener {
    public final static String TOOL_IMAGE_DIR = "icons/full/etool16/";
    public final static String IMAGE_ANIMATION_MODE = TOOL_IMAGE_DIR + "animationmode.png";
    public final static String IMAGE_INCREASE_ANIMATION_SPEED = TOOL_IMAGE_DIR + "increaseanimationspeed.gif";
    public final static String IMAGE_DECREASE_ANIMATION_SPEED = TOOL_IMAGE_DIR + "decreaseanimationspeed.gif";
    public final static String IMAGE_GOTO_BEGIN = TOOL_IMAGE_DIR + "gotobegin.gif";
    public final static String IMAGE_GOTO_FIRST_EVENT = TOOL_IMAGE_DIR + "gotofirstevent.gif";
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

    protected AnimationCanvas animationCanvas;

    protected AnimationAction increaseAnimationSpeedAction;
    protected AnimationAction decreaseAnimationSpeedAction;
    protected AnimationAction gotoBeginAction;
    protected AnimationAction gotoFirstEventAction;
    protected AnimationAction gotoEndAction;
    protected AnimationAction playForwardAction;
    protected AnimationAction stepForwardAction;
    protected AnimationAction playBackwardAction;
    protected AnimationAction stepBackwardAction;
    protected AnimationAction stopAction;
    protected AnimationAction copyToClipboardAction;
    protected AnimationAction refreshAction;
    protected AnimationAction pinAction;
    protected AnimationPositionContribution animationPositionContribution;

	/*************************************************************************************
	 * CONSTRUCTION
	 */

	public AnimationContributor() {
		this.increaseAnimationSpeedAction = createIncreaseAnimationSpeedActionAction();
        this.decreaseAnimationSpeedAction = createDecreaseAnimationSpeedActionAction();
		this.gotoBeginAction = createGotoBeginAction();
        this.gotoFirstEventAction = createGotoFirstEventAction();
        this.gotoEndAction = createGotoEndAction();
        this.playForwardAction = createPlayForwardAction();
        this.stepForwardAction = createStepForwardAction();
        this.playBackwardAction = createPlayBackwardAction();
        this.stepBackwardAction = createStepBackardAction();
        this.stopAction = createStopAction();
        this.copyToClipboardAction = createCopyToClipboardAction();
        this.refreshAction = createRefreshAction();
        this.pinAction = createPinAction();
        this.animationPositionContribution = createAnimationPositionContribution();
    }

    public AnimationContributor(AnimationCanvas animationCanvas) {
		this();
		this.animationCanvas = animationCanvas;
		animationCanvas.addSelectionChangedListener(this);
	}

	@Override
	public void dispose() {
	    if (animationCanvas != null)
	        animationCanvas.removeSelectionChangedListener(this);
	    animationCanvas = null;
		super.dispose();
	}

	/*************************************************************************************
	 * CONTRIBUTIONS
	 */

	public void contributeToPopupMenu(IMenuManager menuManager) {
		menuManager.setRemoveAllWhenShown(true);
		menuManager.addMenuListener(this);
	}

    public void menuAboutToShow(IMenuManager menuManager) {
        // Show In submenu
        MenuManager showInSubmenu = new MenuManager(getShowInMenuLabel());
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        showInSubmenu.add(ContributionItemFactory.VIEWS_SHOW_IN.create(workbenchWindow));

        menuManager.add(new Separator());

        IMenuManager subMenuManager = new MenuManager("Speed");
        menuManager.add(subMenuManager);
        subMenuManager.add(increaseAnimationSpeedAction);
        subMenuManager.add(decreaseAnimationSpeedAction);

        subMenuManager = new MenuManager("Play");
        menuManager.add(subMenuManager);

        subMenuManager.add(gotoBeginAction);
        subMenuManager.add(gotoFirstEventAction);
        subMenuManager.add(playBackwardAction);
        subMenuManager.add(stepBackwardAction);
        subMenuManager.add(stopAction);
        subMenuManager.add(stepForwardAction);
        subMenuManager.add(playForwardAction);
        subMenuManager.add(gotoEndAction);
        subMenuManager.add(new Separator());

        subMenuManager = new MenuManager("Go To");
        menuManager.add(subMenuManager);
        subMenuManager.add(createGotoEventCommandContributionItem());
        subMenuManager.add(createGotoSimulationTimeCommandContributionItem());

        menuManager.add(new Separator());
        menuManager.add(pinAction);
        menuManager.add(copyToClipboardAction);
        menuManager.add(refreshAction);
        menuManager.add(new Separator());

        menuManager.add(showInSubmenu);
    }

	@Override
	public void contributeToToolBar(IToolBarManager toolBarManager) {
	    contributeToToolBar(toolBarManager, false);
	}

	public void contributeToToolBar(IToolBarManager toolBarManager, boolean view) {
        toolBarManager.add(new Separator());
        toolBarManager.add(increaseAnimationSpeedAction);
        toolBarManager.add(decreaseAnimationSpeedAction);
        toolBarManager.add(new Separator());
        toolBarManager.add(gotoBeginAction);
        toolBarManager.add(gotoFirstEventAction);
        toolBarManager.add(playBackwardAction);
        toolBarManager.add(stepBackwardAction);
        toolBarManager.add(stopAction);
        toolBarManager.add(stepForwardAction);
        toolBarManager.add(playForwardAction);
        toolBarManager.add(gotoEndAction);
        toolBarManager.add(new Separator());
        toolBarManager.add(refreshAction);
        if (view)
            toolBarManager.add(pinAction);
        toolBarManager.add(animationPositionContribution);
	}

	@Override
	public void setActiveEditor(IEditorPart targetEditor) {
		if (targetEditor instanceof AnimationEditor) {
			if (animationCanvas != null) {
				getPage().removePartListener(this);
				animationCanvas.removeSelectionChangedListener(this);
				animationCanvas.getAnimationController().removeAnimationListener(this);
			}
			animationCanvas = ((AnimationEditor)targetEditor).getAnimationCanvas();
            if (animationCanvas.getAnimationContributor() == null)
                animationCanvas.setAnimationContributor(this);
			getPage().addPartListener(this);
			animationCanvas.addSelectionChangedListener(this);
			animationCanvas.getAnimationController().addAnimationListener(this);
            animationPositionContribution.configureSlider();
			update();
		}
		else
		    animationCanvas = null;
	}

	public void update() {
		try {
			for (Field field : AnimationContributor.class.getDeclaredFields()) {
				Class<?> fieldType = field.getType();
				if (fieldType == AnimationAction.class || fieldType == AnimationMenuAction.class) {
					AnimationAction fieldValue = (AnimationAction)field.get(this);
					if (fieldValue != null && animationCanvas != null) {
						fieldValue.setEnabled(true);
						fieldValue.update();
						if ((getPage() != null && !(getPage().getActivePart() instanceof AnimationEditor)) || animationCanvas.getInput().isLongRunningOperationInProgress())
							fieldValue.setEnabled(false);
					}
				}
				else if (fieldType == StatusLineContributionItem.class) {
					StatusLineContributionItem fieldValue = (StatusLineContributionItem)field.get(this);
					if (animationCanvas != null)
						fieldValue.update();
				}
				else if (fieldType == AnimationPositionContribution.class) {
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

    protected String getShowInMenuLabel() {
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

    public void partActivated(IWorkbenchPart part) {
        update();
    }

    public void partBroughtToTop(IWorkbenchPart part) {
    }

    public void partClosed(IWorkbenchPart part) {
    }

    public void partDeactivated(IWorkbenchPart part) {
        update();
    }

    public void partOpened(IWorkbenchPart part) {
    }

    public void animationStarted() {
        update();
    }

    public void animationStopped() {
        update();
    }

    public void animationPositionChanged(AnimationPosition animationPosition) {
        update();
    }

	/*************************************************************************************
	 * ACTIONS
	 */

    protected AnimationAction createIncreaseAnimationSpeedActionAction() {
        return new AnimationAction("Increase Animation Speed", Action.AS_PUSH_BUTTON, AnimationCorePlugin.getImageDescriptor(IMAGE_INCREASE_ANIMATION_SPEED)) {
            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().increaseAnimationSpeed();
            }
        };
    }

    protected AnimationAction createDecreaseAnimationSpeedActionAction() {
        return new AnimationAction("Decrease Animation Speed", Action.AS_PUSH_BUTTON, AnimationCorePlugin.getImageDescriptor(IMAGE_DECREASE_ANIMATION_SPEED)) {
            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().decreaseAnimationSpeed();
            }
        };
    }

    protected AnimationAction createGotoBeginAction() {
        return new AnimationAction("Go To Begin", Action.AS_PUSH_BUTTON, AnimationCorePlugin.getImageDescriptor(IMAGE_GOTO_BEGIN)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getEventLog().isEmpty() && !animationCanvas.getAnimationController().isAtBeginAnimationPosition());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().gotoBeginAnimationPosition();
            }
        };
    }

    protected AnimationAction createGotoFirstEventAction() {
        return new AnimationAction("Go To First Event", Action.AS_PUSH_BUTTON, AnimationCorePlugin.getImageDescriptor(IMAGE_GOTO_FIRST_EVENT)) {
            @Override
            public void update() {
                IEvent firstEvent = animationCanvas.getEventLog().getFirstEvent();
                setEnabled(firstEvent != null && firstEvent.getNextEvent() != null && !animationCanvas.getAnimationController().isAtFirstEventAnimationPosition());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().gotoFirstEventAnimationPosition();
            }
        };
    }

    protected AnimationAction createGotoEndAction() {
        return new AnimationAction("Go To End", Action.AS_PUSH_BUTTON, AnimationCorePlugin.getImageDescriptor(IMAGE_GOTO_END)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getEventLog().isEmpty() && !animationCanvas.getAnimationController().isAtEndAnimationPosition());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().gotoEndAnimationPosition();
            }
        };
    }

    protected AnimationAction createPlayForwardAction() {
        return new AnimationAction("Play Forward", Action.AS_PUSH_BUTTON, AnimationCorePlugin.getImageDescriptor(IMAGE_PLAY_FORWARD)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getEventLog().isEmpty() && !animationCanvas.getAnimationController().isAtEndAnimationPosition());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().playAnimationForward();
            }
        };
    }

    protected AnimationAction createStepForwardAction() {
        return new AnimationAction("Step Forward", Action.AS_PUSH_BUTTON, AnimationCorePlugin.getImageDescriptor(IMAGE_STEP_FORWARD)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getEventLog().isEmpty() && !animationCanvas.getAnimationController().isAtEndAnimationPosition());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().stepAnimationForwardToNextAnimationChange();
            }
        };
    }

    protected AnimationAction createPlayBackwardAction() {
        return new AnimationAction("Play Backward", Action.AS_PUSH_BUTTON, AnimationCorePlugin.getImageDescriptor(IMAGE_PLAY_BAKWARD)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getEventLog().isEmpty() && !animationCanvas.getAnimationController().isAtBeginAnimationPosition());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().playAnimationBackward();
            }
        };
    }

    protected AnimationAction createStepBackardAction() {
        return new AnimationAction("Step Backward", Action.AS_PUSH_BUTTON, AnimationCorePlugin.getImageDescriptor(IMAGE_STEP_BACKWARD)) {
            @Override
            public void update() {
                setEnabled(!animationCanvas.getEventLog().isEmpty() && !animationCanvas.getAnimationController().isAtBeginAnimationPosition());
            }

            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().stepAnimationBackwardToPreviousAnimationChange();
            }
        };
    }

    protected AnimationAction createStopAction() {
        return new AnimationAction("Stop", Action.AS_PUSH_BUTTON, AnimationCorePlugin.getImageDescriptor(IMAGE_STOP)) {
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

    protected CommandContributionItem createGotoEventCommandContributionItem() {
        return new CommandContributionItem(new CommandContributionItemParameter(PlatformUI.getWorkbench(), null, "org.omnetpp.animation.gotoEvent", SWT.PUSH));
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

    protected CommandContributionItem createGotoSimulationTimeCommandContributionItem() {
        return new CommandContributionItem(new CommandContributionItemParameter(PlatformUI.getWorkbench(), null, "org.omnetpp.animation.gotoSimulationTime", SWT.PUSH));
    }

    public static class GotoSimulationTimeHandler extends AbstractHandler {
        // TODO: setEnabled(!getEventLog().isEmpty());
        public Object execute(ExecutionEvent executionEvent) throws ExecutionException {
            IWorkbenchPart part = HandlerUtil.getActivePartChecked(executionEvent);
            if (part instanceof IAnimationCanvasProvider) {
                AnimationCanvas animationCanvas = ((IAnimationCanvasProvider)part).getAnimationCanvas();
                GotoSimulationTimeDialog dialog = new GotoSimulationTimeDialog(animationCanvas.getEventLog(), animationCanvas.getAnimationController().getCurrentSimulationTime());
                if (dialog.open() == Window.OK)
                    animationCanvas.getAnimationController().gotoSimulationTime(dialog.getSimulationTime());
            }
            return null;
        }
    }

    protected AnimationAction createCopyToClipboardAction() {
        return new AnimationAction("Copy to Clipboard", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                animationCanvas.copyToClipboard();
            }
        };
    }

    protected AnimationAction createRefreshAction() {
        return new AnimationAction("Refresh", Action.AS_PUSH_BUTTON, ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_REFRESH)) {
            @Override
            protected void doRun() {
                animationCanvas.getAnimationController().refreshAnimation();
            }
        };
    }

    protected AnimationAction createPinAction() {
        return new AnimationAction("Pin", Action.AS_CHECK_BOX, ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_UNPIN)) {
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

    protected AnimationPositionContribution createAnimationPositionContribution() {
        return new AnimationPositionContribution("AnimationPositionContribution");
    }

    protected class AnimationPositionContribution extends ControlContribution {
        protected Slider slider;

        protected AnimationPositionContribution(String id) {
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
                    long firstEventNumber = animationController.getAnimationCoordinateSystem().getFirstEventNumber();
                    long lastEventNumber = animationController.getAnimationCoordinateSystem().getLastEventNumber();
                    if (selection == firstEventNumber)
                        animationController.gotoBeginAnimationPosition();
                    else if (selection == lastEventNumber)
                        animationController.gotoEndAnimationPosition();
                    else
                        animationController.gotoEventNumber(selection);
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
            AnimationController animationController = animationCanvas.getAnimationController();
            if (slider != null && !slider.isDisposed() && animationController.getCurrentAnimationPosition().isCompletelySpecified())
                slider.setSelection((int)animationController.getCurrentEventNumber());
        }

        public void configureSlider() {
            if (slider != null && animationCanvas != null) {
                AnimationController animationController = animationCanvas.getAnimationController();
                if (animationController != null && animationController.getCurrentAnimationPosition().isCompletelySpecified()) {
                    long firstEventNumber = animationController.getAnimationCoordinateSystem().getFirstEventNumber();
                    long lastEventNumber = animationController.getAnimationCoordinateSystem().getLastEventNumber();
                    // TODO: handle overflow
                    slider.setValues((int)animationController.getCurrentEventNumber(), (int)firstEventNumber, (int)(lastEventNumber - firstEventNumber + 1), 1, 1, 10);
                }
                else
                    slider.setValues(0, 0, 1, 1, 1, 1);
            }
        }
    }

    /*************************************************************************************
     * CLASSES
     */

    protected abstract class AnimationAction extends Action {
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
	            AnimationCorePlugin.logError(e);
	        }
		}

        protected abstract void doRun();
	}

	protected abstract class AnimationMenuAction extends AnimationAction {
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
            protected Menu controlMenu;

            protected Menu parentMenu;

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
