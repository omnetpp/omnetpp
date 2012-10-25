package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditorContributor;

/**
 * The Call Finish action
 *
 * @author Andras
 */
public class CallFinishAction extends AbstractSimulationActionDelegate {
    @Override
    protected void registerInContributor(IAction thisAction) {
        SimulationEditorContributor.callFinishAction = thisAction;
    }

    @Override
    public void run(IAction action) {
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
        catch (CommunicationException e) {
            // nothing -- error dialog and logging is already taken care of in the lower layers
        }
        finally {
            updateState();
        }
    }

    @Override
    public void updateState() {
        SimState state = getSimulationController().getUIState();
        boolean failure = getSimulation().isInFailureMode();
        setEnabled(!failure && (state == SimState.READY || state == SimState.TERMINATED || state == SimState.ERROR)); // we also allow it in ERROR case, see SimulationController method
    }
}
