package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditor;

/**
 * The Call Finish action
 *
 * @author Andras
 */
public class CallFinishAction extends AbstractSimulationAction {
    public CallFinishAction(SimulationEditor editor) {
        super(editor);
        setText("Finalize");
        setToolTipText("Invokes the finish() method on all components in the simulation");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_FINISH));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getSimulationController();

            // make sure state is not RUNNING, NONETWORK, or FINISHCALLED
            if (!ensureNotRunning(controller))
                return;
            if (!controller.isNetworkPresent())
                return;
            if (controller.getUIState() == SimState.FINISHCALLED) {
                MessageDialog.openInformation(getShell(), "Finish Called", "finish() has been invoked already.");
                return;
            }

            // let the user confirm the action, especially if we are in the ERROR state
            if (controller.getUIState() == SimState.ERROR) {
                if (!MessageDialog.openQuestion(getShell(), "Warning", "Simulation was stopped with error, calling finish() might produce unexpected results. Proceed anyway?"))
                    return;
            }
            else {
                if (!MessageDialog.openQuestion(getShell(), "Question", "Do you want to conclude this simulation run and invoke finish() on all modules?"))
                    return;
            }

            //XXX busy "Invoking finish() on all modules..."
            controller.callFinish();

        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }

    @Override
    public void updateState() {
        SimState state = getSimulationController().getUIState();
        setEnabled(state == SimState.READY || state == SimState.TERMINATED || state == SimState.ERROR); // we also allow it in ERROR case, see SimulationController method
    }
}
