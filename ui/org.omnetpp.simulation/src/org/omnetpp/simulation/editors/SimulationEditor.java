package org.omnetpp.simulation.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.common.simulation.SimulationEditorInput;
import org.omnetpp.common.ui.FigureCanvas;
import org.omnetpp.common.ui.SelectionProvider;
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

    protected ScrolledComposite sc;
    protected FigureCanvas canvas;

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
        GridLayout layout = new GridLayout(1, true);
        layout.horizontalSpacing = layout.verticalSpacing = layout.marginHeight = layout.marginWidth = 0;
        parent.setLayout(layout);

        Composite controlArea = new Composite(parent, SWT.BORDER);
        controlArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        controlArea.setLayout(new GridLayout(4, false));
        
        ToolBar toolbar1 = new ToolBar(controlArea, SWT.NONE);
        new ActionContributionItem(new SetupIniConfigAction(simulationController)).fill(toolbar1, -1);
        new ActionContributionItem(new SetupNetworkAction(simulationController)).fill(toolbar1, -1);
        new ActionContributionItem(new RebuildNetworkAction(simulationController)).fill(toolbar1, -1);
        ToolBar toolbar2 = new ToolBar(controlArea, SWT.NONE);
        new ActionContributionItem(new StepAction(simulationController)).fill(toolbar2, -1);
        new ActionContributionItem(new RunAction(simulationController)).fill(toolbar2, -1);
        new ActionContributionItem(new FastRunAction(simulationController)).fill(toolbar2, -1);
        new ActionContributionItem(new ExpressRunAction(simulationController)).fill(toolbar2, -1);
        new ActionContributionItem(new RunUntilAction(simulationController)).fill(toolbar2, -1);
        ToolBar toolbar3 = new ToolBar(controlArea, SWT.NONE);
        new ActionContributionItem(new StopAction(simulationController)).fill(toolbar3, -1);
        new ActionContributionItem(new CallFinishAction(simulationController)).fill(toolbar3, -1);

        statusLabel = new Label(controlArea, SWT.NONE);
        statusLabel.setText("n/a");
        statusLabel.setLayoutData(new GridData(SWT.FILL, SWT.END, true, false));
        
        sc = new ScrolledComposite(parent, SWT.V_SCROLL | SWT.H_SCROLL | SWT.BORDER);
        sc.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        // create canvas
        canvas = new FigureCanvas(sc, SWT.DOUBLE_BUFFERED);
        sc.setContent(canvas);
        canvas.setBackground(new Color(null, 235, 235, 235));

        // update status display when something happens to the simulation
        simulationController.addSimulationStateListener(new ISimulationStateListener() {
            @Override
            public void simulationStateChanged(SimulationController controller) {
                Display.getDefault().asyncExec(new Runnable() {
                    @Override
                    public void run() {
                        updateStatusDisplay();
                    }
                });
            }
        });
        
        // obtain initial status query
        Display.getDefault().asyncExec(new Runnable() {
            @Override
            public void run() {
                try {
                    simulationController.refreshStatus();
                } catch (Exception e) {
                    MessageDialog.openError(getSite().getShell(), "Error", "An error occurred while connecting to the simulation: " + e.getMessage());
                    SimulationPlugin.logError(e);
                }
            }
        });
        
    }

    @Override
    public void dispose() {
    	super.dispose();
    	
    	if (simulationController.canCancelLaunch()) {
    	    boolean ans = MessageDialog.openQuestion(getSite().getShell(), "Terminate?", "Do you want to terminate the simulation process?");
    	    if (ans)
    	        simulationController.cancelLaunch();
    	}
    }

    @Override
    public void setFocus() {
        canvas.setFocus();
    }

    public SimulationController getSimulationController() {
        return simulationController;
    }

    protected void updateStatusDisplay() {
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
