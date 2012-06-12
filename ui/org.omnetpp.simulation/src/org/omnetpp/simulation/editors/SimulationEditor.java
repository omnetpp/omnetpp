package org.omnetpp.simulation.editors;

import java.io.File;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.LineBorder;
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
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.controller.BlankAnimationCoordinateSystem;
import org.omnetpp.animation.editors.AnimationContributorBase;
import org.omnetpp.animation.eventlog.controller.EventLogAnimationCoordinateSystem;
import org.omnetpp.animation.eventlog.providers.EventLogAnimationPrimitiveProvider;
import org.omnetpp.animation.eventlog.widgets.EventLogAnimationCanvas;
import org.omnetpp.animation.eventlog.widgets.EventLogAnimationParameters;
import org.omnetpp.animation.providers.EmptyAnimationPrimitiveProvider;
import org.omnetpp.animation.widgets.AnimationCanvas;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.simulation.MessageModel;
import org.omnetpp.common.simulation.ModuleModel;
import org.omnetpp.common.simulation.SimulationEditorInput;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.common.util.BinarySearchUtils.BoundKind;
import org.omnetpp.eventlog.engine.EventLog;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.actions.CallFinishAction;
import org.omnetpp.simulation.actions.ExpressRunAction;
import org.omnetpp.simulation.actions.FastRunAction;
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

/**
 *
 * @author Andras
 */
public class SimulationEditor extends EditorPart implements ISimulationCallback {
    public static final String EDITOR_ID = "org.omnetpp.simulation.editors.SimulationEditor";  // note: string is duplicated in the Launch plugin code

    protected SimulationController simulationController;

    protected EventLogAnimationCanvas animationCanvas;

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

        // create toolbars
        CTabFolder tabFolder = new CTabFolder(parent, SWT.HORIZONTAL);
        tabFolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        CTabItem simulationTab = new CTabItem(tabFolder, SWT.NONE);
        simulationTab.setText("Simulate");
        CTabItem animationTab = new CTabItem(tabFolder, SWT.NONE);
        animationTab.setText("Playback");

        Composite simulationControls = new Composite(tabFolder, SWT.NONE);
        simulationTab.setControl(simulationControls);
        simulationControls.setLayout(removeSpacing(new GridLayout(1, false)));

        Composite simulationToolbars = new Composite(simulationControls, SWT.NONE);
        simulationToolbars.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        simulationToolbars.setLayout(new GridLayout(4, false));

        ToolBar toolbar1 = new ToolBar(simulationToolbars, SWT.NONE);
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

        statusLabel = new Label(simulationControls, SWT.BORDER);
        statusLabel.setText("n/a");
        statusLabel.setLayoutData(new GridData(SWT.FILL, SWT.END, true, false));

        Canvas futureEventsTimeline = new Canvas(simulationControls, SWT.BORDER);
        futureEventsTimeline.setBackground(ColorFactory.BEIGE);
        GridData l = new GridData(SWT.FILL, SWT.END, true, false);
        l.heightHint = 20;
        futureEventsTimeline.setLayoutData(l);

        tabFolder.setSelection(simulationTab);

        // create canvas
        animationCanvas = new EventLogAnimationCanvas(parent, SWT.DOUBLE_BUFFERED);
        animationCanvas.setBackground(new Color(null, 235, 235, 235));
        animationCanvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        animationCanvas.setWorkbenchPart(this);

        AnimationController animationController = new AnimationController(new BlankAnimationCoordinateSystem(), new EmptyAnimationPrimitiveProvider());
        animationCanvas.setAnimationController(animationController);
        animationController.setAnimationCanvas(animationCanvas);

        // animationCanvas.setInput(eventLogInput);

        AnimationContributorBase contributor = (AnimationContributorBase) getEditorSite().getActionBarContributor();
        animationCanvas.setAnimationContributor(contributor);


        // update status display when something happens to the simulation
        simulationController.addSimulationStateListener(new ISimulationStateListener() {
            @Override
            public void simulationStateChanged(final SimulationController controller) {
                Display.getDefault().asyncExec(new Runnable() {
                    @Override
                    public void run() {
                        updateStatusDisplay();
                        if (controller.getEventlogFile() != null)
                            setEventlogFileName(controller.getEventlogFile());
                        if (animationCanvas.getInput() != null) {
                            AnimationController animationController = animationCanvas.getAnimationController();
                            int fileChange = animationCanvas.getInput().getEventLog().getFileReader().checkFileForChanges();
                            AnimationPosition currentAnimationPosition = animationController.getCurrentAnimationPosition();
                            // NOTE: we need to copy the simulation time BigDecimal to avoid dangling pointers after synchronize
                            if (currentAnimationPosition.getSimulationTime() != null)
                                currentAnimationPosition.setSimulationTime(new BigDecimal(currentAnimationPosition.getSimulationTime()));
                            animationController.clearInternalState();
                            animationCanvas.getInput().synchronize(fileChange);
                            if (currentAnimationPosition.isCompletelySpecified()) {
                                animationController.gotoAnimationPosition(currentAnimationPosition);
                                AnimationPosition boundAnimationPosition = new AnimationPosition(controller.getEventNumber() - 1, controller.getSimulationTime(), Double.MAX_VALUE, Double.MAX_VALUE);
                                AnimationPosition stopAnimationPosition = animationController.getAnimationCoordinateSystem().getAnimationPosition(controller.getEventNumber(), BoundKind.UPPER_BOUND);
                                System.out.println("GOTO FROM: " + currentAnimationPosition + " BOUND: " + boundAnimationPosition + " STOP: " + stopAnimationPosition);
                                animationController.startAnimation(true, stopAnimationPosition);
                            }
                            else {
                                System.out.println("GOTO INITIAL");
                                animationController.gotoInitialAnimationPosition();
                            }
                            // KLUDGE: mark next module
                            SubmoduleFigure submoduleFigure = (SubmoduleFigure)animationCanvas.getFigure(new ModuleModel(null, controller.getNextEventModuleId()), SubmoduleFigure.class);
                            if (submoduleFigure != null)
                                submoduleFigure.setBorder(new LineBorder(ColorFactory.RED));
                            // KLUDGE: mark next message
                            MessageModel message = new MessageModel();
                            message.setId((int)controller.getNextEventMessageId());
                            Figure figure = (Figure)animationCanvas.getFigure(message, Figure.class);
                            if (figure != null)
                                figure.setBorder(new LineBorder(ColorFactory.RED));
                        }
                    }
                });
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

    private void setEventlogFileName(String fileName) {
        IFile file = ResourcesPlugin.getWorkspace().getRoot().getFileForLocation(new Path(fileName));
        // TODO refresh if file exists but IFile.isAccessible() returns true!
        if (file != null && new File(fileName).isFile() && (animationCanvas.getInput() == null || !animationCanvas.getInput().getFile().equals(file))) {
            IEventLog eventLog = new EventLog(new FileReader(file.getLocation().toOSString(), false));
            EventLogInput eventLogInput = new EventLogInput(file, eventLog);
            // TODO: we need a way to include the upcoming event in the animation
            EventLogAnimationPrimitiveProvider animationPrimitiveProvider = new EventLogAnimationPrimitiveProvider(eventLogInput, new EventLogAnimationParameters());
            AnimationController animationController = animationCanvas.getAnimationController();
            animationController.setProviders(new EventLogAnimationCoordinateSystem(eventLogInput), animationPrimitiveProvider);
            animationPrimitiveProvider.setAnimationController(animationController); //TODO animationController.setProviders() should already do that
            animationCanvas.setInput(eventLogInput);  //TODO should not be necessary (canvas should not know about input)
        }
    }

    private GridLayout removeSpacing(GridLayout l) {
        l.horizontalSpacing = l.verticalSpacing = l.marginHeight = l.marginWidth = 0;
        return l;
    }

    @Override
    public void dispose() {
    	super.dispose();

    	//TODO when platform is shutting down, just cancel the launcherJob without asking!
    	if (simulationController.canCancelLaunch()) {
    	    boolean ans = MessageDialog.openQuestion(getSite().getShell(), "Terminate?", "Do you want to terminate the simulation process?");
    	    if (ans)
    	        simulationController.cancelLaunch();
    	}
    }

    @Override
    public void setFocus() {
        animationCanvas.setFocus();
    }

    public SimulationController getSimulationController() {
        return simulationController;
    }

    public AnimationCanvas getAnimationCanvas() {
        return animationCanvas;
    }

    protected void updateStatusDisplay() {
        if (!statusLabel.isDisposed()) {
            SimulationController controller = getSimulationController();
            String status = "   pid=" + controller.getProcessId();  //TODO remove (or: display elsewhere, e.g in some tooltip or status dialog; also hostname)
            status += "   " + controller.getState().name();

            if (controller.getState() != SimState.DISCONNECTED && controller.getState() != SimState.NONETWORK)
                status +=
                "  -  " + controller.getConfigName() + " #" + controller.getRunNumber() +
                "   (" + controller.getNetworkName() + ")" +
                "  -  Event #" + controller.getEventNumber() +
                "   t=" + controller.getSimulationTime() + "s";
            statusLabel.setText(status);
        }
    }

    public void openInspector(Object element) {
        MessageDialog.openConfirm(getSite().getShell(), "Confirm", "openInspector('" + element + "') invoked");
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
