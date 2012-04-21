package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.SimulationController;

/**
 * The Express Run action.
 *  
 * @author Andras
 */
public class ExpressRunAction extends AbstractSimulationAction {
    public ExpressRunAction() {
        setText("Express Run");
        setToolTipText("Express Run");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_EXPRESS));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getActiveSimulationController();
            if (!ensureNetworkReady(controller))
                return;
            
            controller.expressRun();
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }
}
