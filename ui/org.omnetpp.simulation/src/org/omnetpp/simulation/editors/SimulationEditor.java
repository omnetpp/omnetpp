package org.omnetpp.simulation.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.animation.widgets.AnimationLever;
import org.omnetpp.animation.widgets.AnimationPositionControl;
import org.omnetpp.animation.widgets.AnimationStatus;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.simulation.SimulationEditorInput;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.actions.CallFinishAction;
import org.omnetpp.simulation.actions.ExpressRunAction;
import org.omnetpp.simulation.actions.FastRunAction;
import org.omnetpp.simulation.actions.LinkWithSimulationAction;
import org.omnetpp.simulation.actions.ProcessInfoAction;
import org.omnetpp.simulation.actions.RebuildNetworkAction;
import org.omnetpp.simulation.actions.RunAction;
import org.omnetpp.simulation.actions.RunUntilAction;
import org.omnetpp.simulation.actions.SetupIniConfigAction;
import org.omnetpp.simulation.actions.SetupNetworkAction;
import org.omnetpp.simulation.actions.StepAction;
import org.omnetpp.simulation.actions.StopAction;
import org.omnetpp.simulation.controller.ISimulationCallback;
import org.omnetpp.simulation.controller.ISimulationStateListener;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.controller.SimulationController.SimState;
import org.omnetpp.simulation.inspectors.GraphicalModulePart;
import org.omnetpp.simulation.inspectors.IInspectorPart;
import org.omnetpp.simulation.inspectors.InfoTextInspectorPart;
import org.omnetpp.simulation.inspectors.QueueInspectorPart;
import org.omnetpp.simulation.inspectors.SimulationCanvas;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cQueue;
import org.omnetpp.simulation.model.cSimulation;

/**
 *
 * @author Andras
 */
//TODO better icons for animation controls
//TODO when playing, the "Play" or "Play backward" icon should remain pushed!
//TODO initial module creations should NOT be animated at all! (they are NOT part of initialize() but take place before that, as part of setupNetwork())
//TODO StepForward: change to "StepForwardToNextEvent" from "NextAnimationChange"
//TODO why animation toolbar cannot mirror the simulation toolbar, only with green buttons?
//TODO toolbar icon: "tie editor lifetime to simulation process lifetime" ("terminate process when editor closes, and vice versa")
//TODO an "Attach To" dialog that lists simulation processes on the given host (scans ports)
public class SimulationEditor extends EditorPart implements /*TODO IAnimationCanvasProvider,*/ ISimulationCallback {
    public static final String EDITOR_ID = "org.omnetpp.simulation.editors.SimulationEditor";  // note: string is duplicated in the Launch plugin code

    protected SimulationController simulationController;

//    protected EventLogAnimationCanvas animationCanvas;
    protected SimulationCanvas simulationCanvas;

    private Label statusLabel;

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        if (!(input instanceof SimulationEditorInput))
            throw new PartInitException("Invalid input: it must be a SimulationEditorInput");

        setSite(site);
        setInput(input);
        setPartName(input.getName());

        SimulationEditorInput simInput = (SimulationEditorInput)input;
        simulationController = new SimulationController(simInput.getHostName(), simInput.getPortNumber(), simInput.getLauncherJob());
        simulationController.setSimulationCallback(this);

        site.setSelectionProvider(new SelectionProvider());
    }

    @Override
    public void createPartControl(Composite parent) {
        parent.setLayout(removeSpacing(new GridLayout(1, true)));

        // create tabfolder for simulation and animation ribbons
        CTabFolder tabFolder = new CTabFolder(parent, SWT.HORIZONTAL);
        tabFolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        // make tab colors nicer; see Scave's FormToolkit2.setColors() for the original
        Color blue = new Color(null, 183, 202, 232);
        Color lighterBlue = new Color(null, 233, 239, 245);
        tabFolder.setSelectionBackground(new Color[]{blue, lighterBlue}, new int[] {25}, true);

        // create tabs
        CTabItem simulationTab = new CTabItem(tabFolder, SWT.NONE);
        simulationTab.setText("Simulate");
        CTabItem animationTab = new CTabItem(tabFolder, SWT.NONE);
        animationTab.setText("Playback");

        tabFolder.setSelection(simulationTab);

        // create simulation ribbon
        Composite simulationRibbon = new Composite(tabFolder, SWT.NONE);
        simulationTab.setControl(simulationRibbon);
        simulationRibbon.setLayout(removeSpacing(new GridLayout(1, false)));

        Composite simulationToolbars = new Composite(simulationRibbon, SWT.NONE);
        simulationToolbars.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        simulationToolbars.setLayout(new GridLayout(5, false));

        ToolBar toolbar1 = new ToolBar(simulationToolbars, SWT.NONE);
        new ActionContributionItem(new LinkWithSimulationAction(simulationController)).fill(toolbar1, -1);
        new ActionContributionItem(new ProcessInfoAction(simulationController)).fill(toolbar1, -1);
        new ActionContributionItem(new SetupIniConfigAction(simulationController)).fill(toolbar1, -1);
        new ActionContributionItem(new SetupNetworkAction(simulationController)).fill(toolbar1, -1);
        new ActionContributionItem(new RebuildNetworkAction(simulationController)).fill(toolbar1, -1);
        ToolBar toolbar2 = new ToolBar(simulationToolbars, SWT.NONE);
        new ActionContributionItem(new StepAction(simulationController)).fill(toolbar2, -1);
        new ActionContributionItem(new RunAction(simulationController)).fill(toolbar2, -1);
        new ActionContributionItem(new FastRunAction(simulationController)).fill(toolbar2, -1);
        new ActionContributionItem(new ExpressRunAction(simulationController)).fill(toolbar2, -1);
        new ActionContributionItem(new RunUntilAction(simulationController)).fill(toolbar2, -1);
        ToolBar toolbar3 = new ToolBar(simulationToolbars, SWT.NONE);
        new ActionContributionItem(new StopAction(simulationController)).fill(toolbar3, -1);
        new ActionContributionItem(new CallFinishAction(simulationController)).fill(toolbar3, -1);

        statusLabel = new Label(simulationRibbon, SWT.BORDER);
        statusLabel.setText("n/a");
        statusLabel.setLayoutData(new GridData(SWT.FILL, SWT.END, true, false));

        Canvas futureEventsTimeline = new Canvas(simulationRibbon, SWT.BORDER);
        futureEventsTimeline.setBackground(ColorFactory.BEIGE);
        GridData l = new GridData(SWT.FILL, SWT.END, true, false);
        l.heightHint = 20;
        futureEventsTimeline.setLayoutData(l);

        // create animation ribbon
        Composite animationRibbon = new Composite(tabFolder, SWT.NONE);
        animationTab.setControl(animationRibbon);
        animationRibbon.setLayout(removeSpacing(new GridLayout(1, false)));
        animationRibbon.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        Composite animationToolbars = new Composite(animationRibbon, SWT.NONE);
        animationToolbars.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        animationToolbars.setLayout(new GridLayout(3, false));

        SimulationContributor contributor = (SimulationContributor)getEditorSite().getActionBarContributor();
        ToolBar animToolbar1 = new ToolBar(animationToolbars, SWT.NONE);
        new ActionContributionItem(contributor.gotoBeginningAction).fill(animToolbar1, -1);
        new ActionContributionItem(contributor.stepBackwardAction).fill(animToolbar1, -1);
        new ActionContributionItem(contributor.playBackwardAction).fill(animToolbar1, -1);
        new ActionContributionItem(contributor.playForwardAction).fill(animToolbar1, -1);
        new ActionContributionItem(contributor.stopAction).fill(animToolbar1, -1);
        new ActionContributionItem(contributor.stepForwardAction).fill(animToolbar1, -1);
        new ActionContributionItem(contributor.gotoEndAction).fill(animToolbar1, -1);
        contributor.separatorAction.fill(animToolbar1, -1);
        contributor.separatorAction.fill(animToolbar1, -1);
        new ActionContributionItem(contributor.animationTimeModeAction).fill(animToolbar1, -1);
        contributor.separatorAction.fill(animToolbar1, -1);
        new ActionContributionItem(contributor.refreshAction).fill(animToolbar1, -1);

        contributor.separatorAction.fill(animToolbar1, -1);

        new ActionContributionItem(contributor.decreasePlaybackSpeedAction).fill(animToolbar1, -1);
        final AnimationLever animationLever = new AnimationLever(animToolbar1, SWT.NONE);
        ToolItem ti = new ToolItem(animToolbar1, SWT.SEPARATOR, animToolbar1.getItemCount());
        ti.setControl(animationLever.getLever());
        ti.setWidth(animationLever.getLever().computeSize(SWT.DEFAULT, SWT.DEFAULT, true).x);
        new ActionContributionItem(contributor.increasePlaybackSpeedAction).fill(animToolbar1, -1);

        AnimationStatus animationStatus = new AnimationStatus(animationToolbars, SWT.NONE);
        animationStatus.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        AnimationPositionControl animationPositionControl = new AnimationPositionControl(animationRibbon, SWT.NONE);
        animationPositionControl.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        // create simulation canvas (TODO: on the same tab as the controls!)
        simulationCanvas = new SimulationCanvas(simulationController, parent, SWT.DOUBLE_BUFFERED | SWT.BORDER);
        simulationCanvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

//        // create animation canvas
//        animationCanvas = new EventLogAnimationCanvas(parent, SWT.DOUBLE_BUFFERED) {
//            private Figure messageFigure;
//            private ArrayList<SubmoduleFigure> submoduleFigures = new ArrayList<SubmoduleFigure>();
//
//            @Override
//            public void animationPositionChanged(AnimationPosition animationPosition) {
//                super.animationPositionChanged(animationPosition);
//                if (animationController.isAtEndAnimationPosition()) {
//                    highlightNextEventModule((SimulationModel)animationController.getRootModel(), simulationController.getNextEventModuleId());
//                    highlightNextEventMessage();
//                }
//                else {
//                    // KLUDGE: this might accidentally remove a border not set here
//                    for (SubmoduleFigure submoduleFigure : submoduleFigures)
//                        submoduleFigure.setBorder(null);
//                    submoduleFigures.clear();
//                    if (messageFigure != null)
//                        messageFigure.setBorder(null);
//                }
//            }
//
//            private void highlightNextEventModule(SimulationModel simulation, int moduleId) {
//                ModuleModel module = simulation.getModuleById(moduleId);
//                highlightNextModule(simulation, module, false);
//            }
//
//            private void highlightNextModule(SimulationModel simulation, ModuleModel module, boolean isAncestor) {
//                SubmoduleFigure submoduleFigure = (SubmoduleFigure)animationCanvas.getFigure(module, SubmoduleFigure.class);
//                if (submoduleFigure != null) {
//                    // KLUDGE: mark next module
//                    submoduleFigure.setBorder(new LineBorder(ColorFactory.RED, isAncestor ? 1 : 2));
//                    submoduleFigures.add(submoduleFigure);
//                }
//                ModuleModel parentModule = module.getParentModule();
//                if (parentModule != null)
//                    highlightNextModule(simulation, parentModule, true);
//            }
//
//            private void highlightNextEventMessage() {
//                MessageModel message = new MessageModel();
//                message.setId((int)simulationController.getNextEventMessageId());
//                messageFigure = (Figure)animationCanvas.getFigure(message, Figure.class);
//                if (messageFigure != null) {
//                    // KLUDGE: mark next message
//                    messageFigure.setBorder(new LineBorder(ColorFactory.RED));
//                }
//            }
//        };
//        animationCanvas.setBackground(new Color(null, 235, 235, 235));
//        animationCanvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
//        animationCanvas.setWorkbenchPart(this);
//
//        AnimationController animationController = new AnimationController(new BlankAnimationCoordinateSystem(), new EmptyAnimationPrimitiveProvider());
//        animationCanvas.setAnimationController(animationController);
//        animationController.setAnimationCanvas(animationCanvas);
//
//        final AnimationPlaybackController animationPlaybackController = new AnimationPlaybackController(animationController);
//        animationCanvas.setAnimationPlaybackController(animationPlaybackController);
//        simulationController.setAnimationPlaybackController(animationPlaybackController);
//
//        animationLever.setAnimationPlaybackController(animationPlaybackController);
//        animationStatus.setAnimationController(animationController);
//        animationPositionControl.setAnimationController(animationController);
//
//        // animationCanvas.setInput(eventLogInput);
//
//        animationCanvas.setAnimationContributor(contributor);
//
//        animationPlaybackController.addPlaybackStateListener(new IAnimationPlaybackStateListener() {
//            @Override
//            public void playbackStateChanged(AnimationPlaybackController controller) {
//                animationLever.setSecondaryAnimationSpeed(animationPlaybackController.getPlaybackSpeed());
//            }
//        });
//
//        getSite().setSelectionProvider(animationCanvas);  //TODO does not work for now
//
        // update status display when something happens to the simulation
        simulationController.addSimulationStateListener(new ISimulationStateListener() {
            @Override
            public void simulationStateChanged(final SimulationController controller) {
//                if (controller.getState() == SimState.DISCONNECTED || controller.getState() == SimState.NONETWORK)
//                    return; //FIXME this is a hack because animationController doesn't like empty eventlogs
//                if (controller.getEventlogFile() != null)
//                    setEventlogFileName(controller.getEventlogFile());
//                if (animationCanvas.getInput() != null) {
//                    AnimationController animationController = animationCanvas.getAnimationController();
//                    AnimationPosition currentAnimationPosition = animationController.getCurrentAnimationPosition();
//                    int fileChange = animationCanvas.getInput().getEventLog().getFileReader().checkFileForChanges();
//                    if (!currentAnimationPosition.isCompletelySpecified() || fileChange != FileReader.FileChangedState.UNCHANGED) {
//                        // NOTE: we need to copy the simulation time BigDecimal to avoid dangling pointers after synchronize
//                        if (currentAnimationPosition.getSimulationTime() != null)
//                            currentAnimationPosition.setSimulationTime(new BigDecimal(currentAnimationPosition.getSimulationTime()));
//                        animationController.clearInternalState();
//                        animationCanvas.getInput().synchronize(fileChange);
//                        if (currentAnimationPosition.isCompletelySpecified()) {
//                            AnimationPosition stopAnimationPosition = animationController.getAnimationCoordinateSystem().getAnimationPosition(controller.getEventNumber(), BoundKind.UPPER_BOUND);
//                            animationController.gotoAnimationPosition(currentAnimationPosition);
//                            animationController.startAnimation(true, stopAnimationPosition);
//                        }
//                        else
//                            animationController.gotoInitialAnimationPosition();
//                    }
//                }
                updateStatusDisplay();
            }
        });

        // obtain initial status query
        Display.getDefault().asyncExec(new Runnable() {
            @Override
            public void run() {
                try {
                    // KLUDGE: TODO: this is necessary, because connection timeout does not work as expected (apache HttpClient ignores the provided value)
                    Thread.sleep(1000);
                    simulationController.refreshStatus();
                } catch (Exception e) {
                    MessageDialog.openError(getSite().getShell(), "Error", "An error occurred while connecting to the simulation: " + e.getMessage());
                    SimulationPlugin.logError(e);
                }
            }
        });

    }

//    private void setEventlogFileName(String fileName) {
//        IFile file = ResourcesPlugin.getWorkspace().getRoot().getFileForLocation(new Path(fileName));
//        // TODO refresh if file exists but IFile.isAccessible() returns true!
//        if (file != null && new File(fileName).isFile() && (animationCanvas.getInput() == null || !animationCanvas.getInput().getFile().equals(file))) {
//            IEventLog eventLog = new EventLog(new FileReader(file.getLocation().toOSString(), false));
//            EventLogInput eventLogInput = new EventLogInput(file, eventLog);
//            // TODO: we need a way to include the upcoming event in the animation
//            EventLogAnimationPrimitiveProvider animationPrimitiveProvider = new EventLogAnimationPrimitiveProvider(eventLogInput, new EventLogAnimationParameters());
//            AnimationController animationController = animationCanvas.getAnimationController();
//            animationController.setProviders(new EventLogAnimationCoordinateSystem(eventLogInput), animationPrimitiveProvider);
//            animationPrimitiveProvider.setAnimationController(animationController); //TODO animationController.setProviders() should already do that
//            animationCanvas.setInput(eventLogInput);  //TODO should not be necessary (canvas should not know about input)
//        }
//    }

    private GridLayout removeSpacing(GridLayout l) {
        l.horizontalSpacing = l.verticalSpacing = l.marginHeight = l.marginWidth = 0;
        return l;
    }

    @Override
    public void dispose() {
        super.dispose();

        simulationController.dispose();
        simulationController = null;
    }

    @Override
    public void setFocus() {
        simulationCanvas.setFocus();
    }

    public SimulationController getSimulationController() {
        return simulationController;
    }

//    public AnimationCanvas getAnimationCanvas() {
//        return animationCanvas;
//    }

    public SimulationCanvas getSimulationCanvas() {
        return simulationCanvas;
    }
    
    protected void updateStatusDisplay() {
        if (!statusLabel.isDisposed()) {
            SimulationController controller = getSimulationController();
            String status = "   pid=" + controller.getProcessId();  //TODO remove (or: display elsewhere, e.g in some tooltip or status dialog; also hostname)
            status += "   " + controller.getState().name();

            cSimulation simulation = (cSimulation) controller.getRootObject(SimulationController.ROOTOBJ_SIMULATION);
            cModule module = simulation.getModuleById(controller.getNextEventModuleId());
            if (module != null && !module.isFilledIn())
                module.safeLoad();

            if (controller.getState() != SimState.DISCONNECTED && controller.getState() != SimState.NONETWORK)
                status +=
                "  -  " + controller.getConfigName() + " #" + controller.getRunNumber() +
                "   (" + controller.getNetworkName() + ")" +
                "  -  Event #" + controller.getEventNumber() +
                "   t=" + controller.getSimulationTime() + "s" +
                "   next=" + controller.getNextSimulationTime() + "s" +
                " in " + (module == null ? "unknown" : module.getFullPath() + " (" + module.getClassName() /*FIXME*/ + ")");
            statusLabel.setText(status);
        }
    }

    public void openInspector(cObject object) {
        //TODO more dynamic inspector type selection
        IInspectorPart inspectorPart = null;
        if (object instanceof cModule)
            inspectorPart = new GraphicalModulePart((cModule)object, (cSimulation) simulationController.getRootObject(SimulationController.ROOTOBJ_SIMULATION));
//        else if (object instanceof cMessage)
//            inspectorPart = new MessageInspectorPart((cMessage)object);
        else if (object instanceof cQueue)
            inspectorPart = new QueueInspectorPart((cQueue)object);
        else // fallback
            inspectorPart = new InfoTextInspectorPart(object);
        simulationCanvas.addInspectorPart(inspectorPart);
    }

    @Override
    public String askParameter(String paramName, String ownerFullPath, String paramType, String prompt, String defaultValue, String unit, String[] choices) {
        String dialogMessage = "Enter parameter " + paramName + "." + ownerFullPath; //TODO refine (use prompt, paramType, etc)
        InputDialog dialog = new InputDialog(getSite().getShell(), "Enter Parameter", dialogMessage, defaultValue, null);
        if (dialog.open() == Dialog.CANCEL)
            return null;
        return dialog.getValue();
    }

    @Override
    public void displayError(String errorMessage) {
        MessageDialog.openError(getSite().getShell(), "Error", errorMessage);
    }

	@Override
    public void doSave(IProgressMonitor monitor) {
        // Nothing
    }

    @Override
    public void doSaveAs() {
        // Nothing
    }

    @Override
    public boolean isDirty() {
        return false;
    }

    @Override
    public boolean isSaveAsAllowed() {
        return false;
    }


}
