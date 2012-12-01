package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.IAction;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.controller.SimulationController.ConnState;
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
            if (controller.getConnectionState() == ConnState.OFFLINE)
                controller.goOnline(); // give it another chance
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
        ConnState connState = getSimulationController().getConnectionState();
        SimState simState = getSimulationController().getUIState();
        setEnabled(connState == ConnState.OFFLINE || (connState == ConnState.ONLINE && simState != SimState.RUNNING));
    }
}
