package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.IAction;
import org.omnetpp.simulation.controller.Simulation.RunMode;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditorContributor;

/**
 * The Fast Run action.
 *
 * @author Andras
 */
public class FastRunAction extends AbstractSimulationActionDelegate {
    @Override
    protected void registerInContributor(IAction thisAction) {
        SimulationEditorContributor.fastRunAction = thisAction;
    }

    @Override
    public void run(IAction action) {
        try {
            SimulationController controller = getSimulationController();
            if (!ensureNetworkReady())
                return;

            if (!controller.isRunning())
                controller.run(RunMode.FAST);
            else
                controller.switchToRunMode(RunMode.FAST);
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
        boolean online = getSimulationController().isOnline();
        SimState state = getSimulationController().getUIState();
        setEnabled(online && (state == SimState.READY || state == SimState.RUNNING));

        RunMode runMode = getSimulationController().getCurrentRunMode();
        setChecked(runMode == RunMode.FAST);
    }
}
