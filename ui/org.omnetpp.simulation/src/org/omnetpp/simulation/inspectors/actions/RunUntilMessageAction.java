package org.omnetpp.simulation.inspectors.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.Simulation.RunMode;
import org.omnetpp.simulation.model.cMessage;

/**
 *
 * @author Andras
 */
public class RunUntilMessageAction extends AbstractInspectorAction {
    public RunUntilMessageAction() {
        super("Run until next arrival of this message", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_MRUN));
    }

    @Override
    public void run() {
        try {
            cMessage message = getMessage();
            getSimulationController().runUntilMessage(RunMode.NORMAL, message);
        }
        catch (CommunicationException e) {
            // nothing -- error dialog and logging is already taken care of in the lower layers
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }

    @Override
    public void update() {
        boolean online = getSimulationController().isOnline();
        setEnabled(online && getMessage() != null && getSimulationController().isSimulationOK());
    }

}
