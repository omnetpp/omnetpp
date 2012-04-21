package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.SimulationController;

/**
 * The Fast Run action.
 *  
 * @author Andras
 */
public class FastRunAction extends AbstractSimulationAction {
    public FastRunAction() {
        setText("Fast Run");
        setToolTipText("Fast Run");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_FAST));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getActiveSimulationController();
            if (!ensureNetworkReady(controller))
                return;
            
            controller.fastRun();
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }
}
