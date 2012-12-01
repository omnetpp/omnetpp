package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.Dialog;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditorContributor;

/**
 * The Run Until action.
 *
 * @author Andras
 */
public class RunUntilAction extends AbstractSimulationActionDelegate {
    @Override
    protected void registerInContributor(IAction thisAction) {
        SimulationEditorContributor.runUntilAction = thisAction;
    }

    @Override
    public void run(IAction action) {
        try {
            SimulationController controller = getSimulationController();
            if (!ensureNetworkReady())
                return;

            RunUntilDialog dialog = new RunUntilDialog(getShell());
            if (dialog.open() == Dialog.OK) {
                controller.runUntil(dialog.getRunMode(), dialog.getSimTime(), dialog.getEventNumber(), dialog.getModule(), dialog.getMessage());
            }
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

        setChecked(getSimulationController().isRunUntilActive());
    }

}
