package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.IAction;
import org.omnetpp.simulation.controller.Simulation.RunMode;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditorContributor;

/**
 * The Express Run action.
 *
 * @author Andras
 */
public class ExpressRunAction extends AbstractSimulationActionDelegate {
    @Override
    protected void registerInContributor(IAction thisAction) {
        SimulationEditorContributor.expressRunAction = thisAction;
    }

    @Override
    public void run(IAction action) {
        try {
            SimulationController controller = getSimulationController();
            if (!ensureNetworkReady())
                return;

            if (!controller.isRunning())
                controller.run(RunMode.EXPRESS);
            else
                controller.switchToRunMode(RunMode.EXPRESS);
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
        setEnabled(!failure && (state == SimState.READY || state == SimState.RUNNING));

        RunMode runMode = getSimulationController().getCurrentRunMode();
        setChecked(runMode == RunMode.EXPRESS);
    }

}
