package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.IAction;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditorContributor;

/**
 * The Setup Network action.
 *
 * @author Andras
 */
public class SetupNetworkAction extends AbstractSimulationActionDelegate {
    @Override
    protected void registerInContributor(IAction thisAction) {
        SimulationEditorContributor.setupNetworkAction = thisAction;
    }

    @Override
    public void run(IAction action) {
        try {
            SimulationController controller = getSimulationController();
            if (!ensureNotRunning(controller))
                return;

            throw new RuntimeException("NOT IMPLEMENTED"); //TODO
        }
        finally {
            updateState();
        }
    }

    @Override
    public void updateState() {
        SimState state = getSimulationController().getUIState();
        boolean failure = getSimulation().isInFailureMode();
        setEnabled(!failure && (state != SimState.DISCONNECTED && state != SimState.RUNNING));
    }
}
