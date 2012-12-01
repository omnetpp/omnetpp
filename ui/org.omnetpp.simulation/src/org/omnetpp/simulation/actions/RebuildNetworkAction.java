package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.Simulation;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditorContributor;
import org.omnetpp.simulation.model.cObject;

/**
 * The Rebuild Network action.
 *
 * @author Andras
 */
public class RebuildNetworkAction extends AbstractSimulationActionDelegate {
    @Override
    protected void registerInContributor(IAction thisAction) {
        SimulationEditorContributor.rebuildNetworkAction = thisAction;
    }

    @Override
    public void run(IAction action) {
        try {
            SimulationController controller = getSimulationController();
            if (!ensureNotRunning(controller))
                return;
            if (!controller.isNetworkPresent()) {
                MessageDialog.openInformation(getShell(), "Error", "No network has been set up yet.");
                return;
            }

            controller.rebuildNetwork();

            cObject network = controller.getSimulation().getRootObject(Simulation.ROOTOBJ_SYSTEMMODULE);
            getSimulationCanvas().inspect(network);
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
        setEnabled(online && (state != SimState.NONETWORK && state != SimState.RUNNING));
    }
}
