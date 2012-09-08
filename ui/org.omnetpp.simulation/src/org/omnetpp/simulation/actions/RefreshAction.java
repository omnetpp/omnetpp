package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditor;

/**
 * The Setup Inifile Config action.
 *
 * @author Andras
 */
public class RefreshAction extends AbstractSimulationAction {
    public RefreshAction(SimulationEditor editor) {
        super(editor);
        setText("Refresh");
        setToolTipText("Refresh");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_REFRESH));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getSimulationController();
            controller.getSimulation().refreshStatus();
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }

    @Override
    public void updateState() {
        SimState state = getSimulationController().getUIState();
        setEnabled(state != SimState.DISCONNECTED && state != SimState.RUNNING);
    }
}
