package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditor;

/**
 * The Setup Network action.
 *
 * @author Andras
 */
public class SetupNetworkAction extends AbstractSimulationAction {
    public SetupNetworkAction(SimulationEditor editor) {
        super(editor);
        setText("Set Up Network...");
        setToolTipText("Set Up Network...");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_NEWNET));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getSimulationController();
            if (!ensureNotRunning(controller))
                return;

            throw new RuntimeException("NOT IMPLEMENTED"); //TODO
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }

    @Override
    public void updateState() {
        SimState state = getSimulationController().getUIState();
        setEnabled(state != SimState.DISCONNECTED && state != SimState.RUNNING);
    }
}
