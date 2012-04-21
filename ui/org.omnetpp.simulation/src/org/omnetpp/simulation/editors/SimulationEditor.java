package org.omnetpp.simulation.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.action.ActionContributionItem;
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
import org.omnetpp.simulation.controller.ISimulationStateListener;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.controller.SimulationController.SimState;

/**
 *
 * @author Andras
 */
public class SimulationEditor extends EditorPart {
    public static final String EDITOR_ID = "org.omnetpp.simulation.SimulationEditor"; //FIXME use this in LaunchDelegate too
    
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
        new ActionContributionItem(new SetupIniConfigAction()).fill(toolbar1, -1);
        new ActionContributionItem(new SetupNetworkAction()).fill(toolbar1, -1);
        new ActionContributionItem(new RebuildNetworkAction()).fill(toolbar1, -1);
        ToolBar toolbar2 = new ToolBar(controlArea, SWT.NONE);
        new ActionContributionItem(new StepAction()).fill(toolbar2, -1);
        new ActionContributionItem(new RunAction()).fill(toolbar2, -1);
        new ActionContributionItem(new FastRunAction()).fill(toolbar2, -1);
        new ActionContributionItem(new ExpressRunAction()).fill(toolbar2, -1);
        new ActionContributionItem(new RunUntilAction()).fill(toolbar2, -1);
        ToolBar toolbar3 = new ToolBar(controlArea, SWT.NONE);
        new ActionContributionItem(new StopAction()).fill(toolbar3, -1);
        new ActionContributionItem(new CallFinishAction()).fill(toolbar3, -1);

        statusLabel = new Label(controlArea, SWT.NONE);
        statusLabel.setText("n/a");
        statusLabel.setLayoutData(new GridData(SWT.FILL, SWT.END, true, false));
        
        sc = new ScrolledComposite(parent, SWT.V_SCROLL | SWT.H_SCROLL | SWT.BORDER);
        sc.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        // create canvas
        canvas = new FigureCanvas(sc, SWT.DOUBLE_BUFFERED);
        sc.setContent(canvas);
        canvas.setBackground(new Color(null, 235, 235, 235));
        
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
        
        simulationController.refreshStatus();
    }

    @Override
    public void dispose() {
        //TODO
    	super.dispose();
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
        String status = "   " + controller.getState().name(); 

        if (controller.getState() != SimState.DISCONNECTED && controller.getState() != SimState.NONETWORK)
            status +=  
            "  -  " + controller.getConfigName() + " #" + controller.getRunNumber() + 
            "   (" + controller.getNetworkName() + ")" + 
            "  -  t=" + controller.getSimulationTime() + "s" + 
            "   Event #" + controller.getEventNumber();
        statusLabel.setText(status);
    }

    public void openInspector(Object element) {
        MessageDialog.openConfirm(getSite().getShell(), "Confirm", "openInspector('" + element + "') invoked");
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
