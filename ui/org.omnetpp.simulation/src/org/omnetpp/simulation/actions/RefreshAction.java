package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.IAction;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditorContributor;

/**
 * The Setup Inifile Config action.
 *
 * @author Andras
 */
public class RefreshAction extends AbstractSimulationActionDelegate {
    @Override
    protected void registerInContributor(IAction thisAction) {
        SimulationEditorContributor.refreshAction = thisAction;
    }

    @Override
    public void run(IAction action) {
        try {
            SimulationController controller = getSimulationController();
            if (controller.getSimulation().isInFailureMode())
                controller.getSimulation().clearFailureMode(); // give it another chance
            controller.refreshStatus();
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
        setEnabled(failure || (state != SimState.DISCONNECTED && state != SimState.RUNNING));
    }
}
