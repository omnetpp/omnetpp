package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.SimulationController;

/**
 * The Step action.
 *  
 * @author Andras
 */
public class StepAction extends AbstractSimulationAction {
    public StepAction() {
        setText("Step");
        setToolTipText("Step");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_STEP));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getActiveSimulationController();
            if (!ensureNetworkReady(controller))
                return;
            
            controller.step();
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }
}
