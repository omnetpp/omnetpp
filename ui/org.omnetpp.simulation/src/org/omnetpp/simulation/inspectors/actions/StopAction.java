package org.omnetpp.simulation.inspectors.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.CommunicationException;


/**
 *
 * @author Andras
 */
public class StopAction extends AbstractInspectorAction {
    public StopAction() {
        super("Stop simulation", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_STOP));
    }

    @Override
    public void run() {
        try {
            getSimulationController().stop();
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
        boolean failure = getSimulationController().getSimulation().isInFailureMode();
        setEnabled(!failure);
    }
}
