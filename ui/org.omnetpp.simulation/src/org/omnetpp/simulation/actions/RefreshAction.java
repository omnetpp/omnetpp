package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.controller.SimulationController.SimState;

/**
 * The Setup Inifile Config action.
 *
 * @author Andras
 */
public class RefreshAction extends AbstractSimulationAction {
    public RefreshAction(SimulationController controller) {
        super(controller);
        setText("Refresh");
        setToolTipText("Refresh");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_REFRESH));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getSimulationController();
            controller.refreshStatus();
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }

    @Override
    public void updateState() {
        SimState state = getSimulationController().getState();
        setEnabled(state != SimState.DISCONNECTED && state != SimState.RUNNING);
    }
}
