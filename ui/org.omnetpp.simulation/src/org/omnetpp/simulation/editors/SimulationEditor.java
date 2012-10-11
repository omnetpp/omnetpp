package org.omnetpp.simulation.editors;

import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.contexts.IContextService;
import org.eclipse.ui.part.EditorPart;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.SimulationEditorInput;
import org.omnetpp.common.ui.DelegatingSelectionProvider;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.canvas.SelectionUtils;
import org.omnetpp.simulation.canvas.SimulationCanvas;
import org.omnetpp.simulation.controller.ISimulationStateListener;
import org.omnetpp.simulation.controller.ISimulationUICallback;
import org.omnetpp.simulation.controller.Simulation;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;
import org.omnetpp.simulation.inspectors.IInspectorPart;
import org.omnetpp.simulation.inspectors.actions.IInspectorAction;
import org.omnetpp.simulation.inspectors.actions.InspectAsObjectAction;
import org.omnetpp.simulation.inspectors.actions.RunLocalAction;
import org.omnetpp.simulation.inspectors.actions.RunLocalFastAction;
import org.omnetpp.simulation.inspectors.actions.RunUntilMessageAction;
import org.omnetpp.simulation.inspectors.actions.RunUntilMessageFastAction;
import org.omnetpp.simulation.liveanimation.AnimationDirector;
import org.omnetpp.simulation.liveanimation.LiveAnimationController;
import org.omnetpp.simulation.model.cMessage;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cSimulation;
import org.omnetpp.simulation.ui.ModulePathsMessageFilter;
import org.omnetpp.simulation.ui.SpeedControl;
import org.omnetpp.simulation.ui.TimelineContentProvider;
import org.omnetpp.simulation.ui.TimelineControl;
import org.omnetpp.simulation.views.SimulationObjectPropertySheetPage;

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
public class SimulationEditor extends EditorPart implements /*TODO IAnimationCanvasProvider,*/ ISimulationUICallback {
    public static final String CONTEXT_SIMULATION = "org.omnetpp.context.simulation";
    public static final String EDITOR_ID = "org.omnetpp.simulation.editors.SimulationEditor";  // note: string is duplicated in the Launch plugin code

    protected SimulationController simulationController;

    protected Composite editorRootControl;
//    protected EventLogAnimationCanvas animationCanvas;
    protected SimulationCanvas simulationCanvas;

    protected Label statusLabel;
    protected TimelineControl timeline;

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        if (!(input instanceof SimulationEditorInput))
            throw new PartInitException("Invalid input: it must be a SimulationEditorInput");

        setSite(site);
        setInput(input);
        setPartName(input.getName());

        ((IContextService)getSite().getService(IContextService.class)).activateContext(CONTEXT_SIMULATION);

        SimulationEditorInput simInput = (SimulationEditorInput)input;
        Simulation simulation = new Simulation(simInput.getHostName(), simInput.getPortNumber(), simInput.getLauncherJob());
        simulationController = new SimulationController(simulation);
        simulationController.setSimulationUICallback(this);

        getSite().setSelectionProvider(new DelegatingSelectionProvider());  // must do it now, because 'editor opened' notification goes right after init() returns
}

    @Override
    public void createPartControl(Composite parent) {
        editorRootControl = parent;
        parent.setLayout(removeSpacing(new GridLayout(1, true)));

//        // create tabfolder for simulation and animation ribbons
//        CTabFolder tabFolder = new CTabFolder(parent, SWT.HORIZONTAL);
//        tabFolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
//
//        // make tab colors nicer; see Scave's FormToolkit2.setColors() for the original
//        Color blue = new Color(null, 183, 202, 232);
//        Color lighterBlue = new Color(null, 233, 239, 245);
//        tabFolder.setSelectionBackground(new Color[]{blue, lighterBlue}, new int[] {25}, true);
//
//        // create tabs
//        CTabItem simulationTab = new CTabItem(tabFolder, SWT.NONE);
//        simulationTab.setText("Simulate");
//        CTabItem animationTab = new CTabItem(tabFolder, SWT.NONE);
//        animationTab.setText("Playback");
//
//        tabFolder.setSelection(simulationTab);

        // create simulation ribbon
        Composite simulationRibbon = new Composite(parent, SWT.NONE);
//        simulationTab.setControl(simulationRibbon);
        simulationRibbon.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        simulationRibbon.setLayout(removeSpacing(new GridLayout(1, false)));

        Composite simulationToolbars = new Composite(simulationRibbon, SWT.NONE);
        simulationToolbars.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        simulationToolbars.setLayout(new GridLayout(5, false));

        ToolBar toolbar1 = new ToolBar(simulationToolbars, SWT.NONE);
        new ActionContributionItem(SimulationEditorContributor.linkWithSimulationAction).fill(toolbar1, -1);
        new ActionContributionItem(SimulationEditorContributor.processInfoAction).fill(toolbar1, -1);
        new ActionContributionItem(SimulationEditorContributor.refreshAction).fill(toolbar1, -1);
        new ActionContributionItem(SimulationEditorContributor.setupIniConfigAction).fill(toolbar1, -1);
        new ActionContributionItem(SimulationEditorContributor.setupNetworkAction).fill(toolbar1, -1);
        new ActionContributionItem(SimulationEditorContributor.rebuildNetworkAction).fill(toolbar1, -1);
        ToolBar toolbar2 = new ToolBar(simulationToolbars, SWT.NONE);
        new ActionContributionItem(SimulationEditorContributor.stepAction).fill(toolbar2, -1);
        new ActionContributionItem(SimulationEditorContributor.runAction).fill(toolbar2, -1);
        new ActionContributionItem(SimulationEditorContributor.fastRunAction).fill(toolbar2, -1);
        new ActionContributionItem(SimulationEditorContributor.expressRunAction).fill(toolbar2, -1);
        new ActionContributionItem(SimulationEditorContributor.runUntilAction).fill(toolbar2, -1);
        ToolBar toolbar3 = new ToolBar(simulationToolbars, SWT.NONE);
        new ActionContributionItem(SimulationEditorContributor.stopAction).fill(toolbar3, -1);
        new ActionContributionItem(SimulationEditorContributor.callFinishAction).fill(toolbar3, -1);
        SpeedControl speedControl = new SpeedControl("");
        speedControl.fill(toolbar3, toolbar3.getItemCount());

        statusLabel = new Label(simulationRibbon, SWT.BORDER);
        statusLabel.setText("n/a");
        statusLabel.setLayoutData(new GridData(SWT.FILL, SWT.END, true, false));

        // create and configure timeline
        timeline = new TimelineControl(simulationRibbon, SWT.BORDER);
        GridData l = new GridData(SWT.FILL, SWT.END, true, false);
        timeline.setLayoutData(l);
        TimelineContentProvider timelineProvider = new TimelineContentProvider(simulationController.getSimulation());
        timeline.setContentProvider(timelineProvider);
        timeline.setLabelProvider(timelineProvider);

        configureTimeline();


//        // create animation ribbon
//        Composite animationRibbon = new Composite(tabFolder, SWT.NONE);
//        animationTab.setControl(animationRibbon);
//        animationRibbon.setLayout(removeSpacing(new GridLayout(1, false)));
//        animationRibbon.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
//
//        Composite animationToolbars = new Composite(animationRibbon, SWT.NONE);
//        animationToolbars.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
//        animationToolbars.setLayout(new GridLayout(3, false));
//
//        SimulationContributor contributor = (SimulationContributor)getEditorSite().getActionBarContributor();
//        ToolBar animToolbar1 = new ToolBar(animationToolbars, SWT.NONE);
//        new ActionContributionItem(contributor.gotoBeginningAction).fill(animToolbar1, -1);
//        new ActionContributionItem(contributor.stepBackwardAction).fill(animToolbar1, -1);
//        new ActionContributionItem(contributor.playBackwardAction).fill(animToolbar1, -1);
//        new ActionContributionItem(contributor.playForwardAction).fill(animToolbar1, -1);
//        new ActionContributionItem(contributor.stopAction).fill(animToolbar1, -1);
//        new ActionContributionItem(contributor.stepForwardAction).fill(animToolbar1, -1);
//        new ActionContributionItem(contributor.gotoEndAction).fill(animToolbar1, -1);
//        contributor.separatorAction.fill(animToolbar1, -1);
//        contributor.separatorAction.fill(animToolbar1, -1);
//        new ActionContributionItem(contributor.animationTimeModeAction).fill(animToolbar1, -1);
//        contributor.separatorAction.fill(animToolbar1, -1);
//        new ActionContributionItem(contributor.refreshAction).fill(animToolbar1, -1);
//
//        contributor.separatorAction.fill(animToolbar1, -1);
//
//        new ActionContributionItem(contributor.decreasePlaybackSpeedAction).fill(animToolbar1, -1);
//        final AnimationLever animationLever = new AnimationLever(animToolbar1, SWT.NONE);
//        ToolItem ti = new ToolItem(animToolbar1, SWT.SEPARATOR, animToolbar1.getItemCount());
//        ti.setControl(animationLever.getLever());
//        ti.setWidth(animationLever.getLever().computeSize(SWT.DEFAULT, SWT.DEFAULT, true).x);
//        new ActionContributionItem(contributor.increasePlaybackSpeedAction).fill(animToolbar1, -1);
//
//        AnimationStatus animationStatus = new AnimationStatus(animationToolbars, SWT.NONE);
//        animationStatus.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
//
//        AnimationPositionControl animationPositionControl = new AnimationPositionControl(animationRibbon, SWT.NONE);
//        animationPositionControl.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        // create simulation canvas (TODO: on the same tab as the controls!)
        simulationCanvas = new SimulationCanvas(parent, SWT.DOUBLE_BUFFERED | SWT.BORDER, this);
        simulationCanvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        // update inspectors when something happens in the simulation
        simulationController.addSimulationStateListener(new ISimulationStateListener() {
            @Override
            public void simulationStateChanged(SimulationController controller) {
                simulationCanvas.refreshInspectors();
                timeline.redraw();
                if (controller.isLastEventAnimationDone())
                    showNextEventMarker();
            }
        });

        // create animation controller for the simulation canvas
        AnimationDirector animationDirector = new AnimationDirector(simulationCanvas, simulationController.getSimulation());
        LiveAnimationController liveAnimationController = new LiveAnimationController(animationDirector);
        simulationController.setLiveAnimationController(liveAnimationController);
        speedControl.setController(liveAnimationController);

        DelegatingSelectionProvider delegatingSelectionProvider = (DelegatingSelectionProvider) getSite().getSelectionProvider();
        delegatingSelectionProvider.setSelectionProvider(simulationCanvas);  //TODO switch to "animationCanvas" when use flips to "Playback" tab

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
                }
                catch (Exception e) {
                    MessageDialog.openError(getSite().getShell(), "Error", "An error occurred while connecting to the simulation: " + e.getMessage());
                    SimulationPlugin.logError(e);
                    return;
                }

                // immediately offer setting up a network
                if (simulationController.getUIState() == SimState.NONETWORK)
                    SimulationEditorContributor.setupIniConfigAction.run();
            }
        });

    }

    protected void configureTimeline() {
        // add hover support
        new HoverSupport().adapt(timeline, new IHoverInfoProvider() {
            @Override
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                Object[] messages = timeline.findMessages(new Point(x,y), 3);
                if (messages.length == 0)
                    return null;
                String html = "<ul>\n";
                for (Object o : messages) {
                    cMessage msg = (cMessage)o;
                    html += "<li>(" + msg.getClassName() + ")&nbsp;<b>" + msg.getName() + "</b> -- "+ msg.getInfo() + "<br>\n";
                }
                html += "</ul>";
                return new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(html));
            }
        });

        // double-clicking opens messages
        timeline.addSelectionListener(new SelectionListener() {
            @Override
            public void widgetSelected(SelectionEvent e) { }

            @Override
            public void widgetDefaultSelected(SelectionEvent e) {
                ISelection selection = timeline.getSelection();
                List<cObject> objects = SelectionUtils.getObjects(selection, cObject.class);
                simulationCanvas.inspect(objects, true);
            }
        });

        // timeline context menu
        final MenuManager timelineMenuManager = new MenuManager("#PopupMenu");
        timeline.setMenu(timelineMenuManager.createContextMenu(timeline));
        timelineMenuManager.setRemoveAllWhenShown(true);
        timelineMenuManager.addMenuListener(new IMenuListener() {
            @Override
            public void menuAboutToShow(IMenuManager manager) {
                SimulationEditor.this.populateContextMenu(timelineMenuManager, timeline.getSelection());
            }
        });

        // timeline follows selection
        ISelectionChangedListener selectionChangeListener = new ISelectionChangedListener() {
            @Override
            public void selectionChanged(SelectionChangedEvent e) {
                System.out.println("timeline: got selection change, new selection: " + e.getSelection());  //TODO
                if (e.getSelection() instanceof IStructuredSelection) {
                    List<cModule> selectedModules = SelectionUtils.getObjects(e.getSelection(), cModule.class);
                    TimelineContentProvider provider = (TimelineContentProvider) timeline.getContentProvider();
                    if (selectedModules.isEmpty())
                        provider.setFilter(null); // no filtering
                    else
                        provider.setFilter(new ModulePathsMessageFilter(selectedModules.toArray(new cModule[]{})));
                    timeline.redraw();
                }
            }
        };
        getSite().getSelectionProvider().addSelectionChangedListener(selectionChangeListener);
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

    @Override
    @SuppressWarnings("rawtypes")
    public Object getAdapter(Class adapter) {
        // ensure this editor has a custom page in Properties View; see PropertySheet documentation
        if (adapter.equals(IPropertySheetPage.class))
            return new SimulationObjectPropertySheetPage();
        return super.getAdapter(adapter);
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
            //TODO this should not be a single label because text jumps due to the use of proportional font
            SimulationController controller = getSimulationController();
            String status = "   pid=" + controller.getSimulation().getProcessId();  //TODO remove (or: display elsewhere, e.g in some tooltip or status dialog; also hostname)
            status += "   " + controller.getUIState().name();

            if (controller.getUIState() != SimState.DISCONNECTED && controller.getUIState() != SimState.NONETWORK) {
                Simulation simulation = controller.getSimulation();
                status += "  -  " + simulation.getConfigName() + " #" + simulation.getRunNumber() + "   (" + simulation.getNetworkName() + ")";

                cSimulation csimulation = (cSimulation) controller.getSimulation().getRootObject(Simulation.ROOTOBJ_SIMULATION);
                if (!csimulation.isFilledIn())
                    csimulation.safeLoad();

                long eventNumber = controller.getEventNumber();
                BigDecimal simTime = controller.getSimulationTime();
                cModule module = csimulation.getModuleById(controller.getEventModuleId());
                if (module != null && !module.isFilledIn())
                    module.safeLoad();
                String moduleText = module == null ? "n/a" : module.getFullPath() + " (" + module.getShortTypeName() + ")";
                status += "  -  Event #" + eventNumber + "   t=" + simTime + "s" + " in " + moduleText;
            }

            statusLabel.setText(status);
        }
    }

    protected void showNextEventMarker() {
        // remove existing
        for (IInspectorPart inspector : simulationCanvas.getInspectors())
            if (inspector instanceof GraphicalModuleInspectorPart)
                ((GraphicalModuleInspectorPart)inspector).removeNextEventMarker();

        // get next event's module ID
        SimulationController controller = getSimulationController();
        int nextEventModuleId = controller.getEventModuleId();
        if (nextEventModuleId <= 1)
            return;

        // add marker to it and all ancestors
        cSimulation simulation = (cSimulation) controller.getSimulation().getRootObject(Simulation.ROOTOBJ_SIMULATION);
        if (!simulation.isFilledIn())
            simulation.safeLoad();
        cModule module = simulation.getModuleById(nextEventModuleId);
        if (!module.isFilledIn())
            module.safeLoad();

        addNextEventMarkerTo(module, true);
        while ((module = module.getParentModule()) != null) {
            if (!module.isFilledIn())
                module.safeLoad();
            addNextEventMarkerTo(module, false);
        }
    }

    protected void addNextEventMarkerTo(cModule module, boolean primary) {
        if (module.getParentModule() != null) {
            // find out if this module appears as a submodule in a graphical module inspector, and if so, highlight it
            IInspectorPart inspector = simulationCanvas.findInspectorFor(module.getParentModule());
            if (inspector instanceof GraphicalModuleInspectorPart)
                ((GraphicalModuleInspectorPart)inspector).setNextEventMarker(module, primary);
        }
    }

    public void openInspector(cObject object) {
        simulationCanvas.inspect(object);  //FIXME probably inspector creation should be brought out of SimulationCanvas!!!
    }

//    @Override
//    public void displayError(String errorMessage) {
//        MessageDialog.openError(getSite().getShell(), "Error", errorMessage);
//    }

    @Override
    public String askParameter(String paramName, String ownerFullPath, String paramType, String prompt, String defaultValue, String unit, String[] choices) {
        String dialogMessage = "Enter parameter " + paramName + "." + ownerFullPath; //TODO refine (use prompt, paramType, etc)
        InputDialog dialog = new InputDialog(getSite().getShell(), "Enter Parameter", dialogMessage, defaultValue, null);
        if (dialog.open() == Dialog.CANCEL)
            return null;
        return dialog.getValue();
    }

    @Override
    public String gets(String prompt, String defaultValue) {
        InputDialog dialog = new InputDialog(getSite().getShell(), "Simulation", prompt, defaultValue, null);
        if (dialog.open() != Dialog.OK)
            return null;
        else
            return dialog.getValue();
    }

    @Override
    public boolean askYesNo(String message) {
        return MessageDialog.openQuestion(getSite().getShell(), "Simulation", message);
    }

    @Override
    public void messageDialog(String message) {
        MessageDialog.openError(getSite().getShell(), "Simulation", message);
    }

    public void populateContextMenu(MenuManager menu, ISelection selection) {
        List<cObject> objects = SelectionUtils.getObjects(selection, cObject.class);
        if (objects.size() == 0)
            return;

        // assemble the context menu
        //XXX for now, let's deal with the first object only
        if (objects.size() == 1 && objects.get(0) instanceof cModule) {
            menu.add(new RunLocalAction());
            menu.add(new RunLocalFastAction());
            menu.add(new Separator());
        }
        if (objects.size() == 1 && objects.get(0) instanceof cMessage) {
            menu.add(new RunUntilMessageAction());
            menu.add(new RunUntilMessageFastAction());
            menu.add(new Separator());
        }

        menu.add(new InspectAsObjectAction());

        // call setContext() on the added actions
        for (IContributionItem item : menu.getItems()) {
            if (item instanceof ActionContributionItem) {
                IAction action = ((ActionContributionItem)item).getAction();
                if (action instanceof IInspectorAction)
                    ((IInspectorAction) action).setContext(simulationCanvas, selection);
            }
        }
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
