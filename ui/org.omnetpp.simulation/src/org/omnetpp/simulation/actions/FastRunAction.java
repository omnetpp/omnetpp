package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.controller.SimulationController.RunMode;
import org.omnetpp.simulation.controller.SimulationController.SimState;

/**
 * The Fast Run action.
 *
 * @author Andras
 */
public class FastRunAction extends AbstractSimulationAction {
    public FastRunAction(SimulationController controller) {
        super(controller, AS_CHECK_BOX);
        setText("Fast Run");
        setToolTipText("Fast Run");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_FAST));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getSimulationController();
            if (!ensureNetworkReady(controller))
                return;

            controller.fastRun();
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            SimulationPlugin.logError(e);
        }
        finally {
            updateState();
        }
    }

    @Override
    public void updateState() {
        SimState state = getSimulationController().getState();
        setEnabled(state == SimState.READY || state == SimState.RUNNING);

        RunMode runMode = getSimulationController().getCurrentRunMode();
        setChecked(runMode == RunMode.FAST);
    }
}
