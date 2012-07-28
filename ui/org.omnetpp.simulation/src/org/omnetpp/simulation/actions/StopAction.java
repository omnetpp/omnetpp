package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.controller.SimulationController.SimState;
import org.omnetpp.simulation.editors.SimulationEditor;

/**
 * The Stop action.
 *
 * @author Andras
 */
public class StopAction extends AbstractSimulationAction {
    public StopAction(SimulationEditor editor) {
        super(editor);
        setText("Stop");
        setToolTipText("Stop");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_STOP));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getSimulationController();
            if (!haveSimulation(controller))
                return;

            controller.stop();
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }

    @Override
    public void updateState() {
        setEnabled(getSimulationController().getState() == SimState.RUNNING);
    }
}
