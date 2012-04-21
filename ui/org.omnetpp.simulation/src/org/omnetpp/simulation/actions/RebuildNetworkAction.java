package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.SimulationController;

/**
 * The Rebuild Network action.
 *  
 * @author Andras
 */
public class RebuildNetworkAction extends AbstractSimulationAction {
    public RebuildNetworkAction() {
        setText("Rebuild Network");
        setToolTipText("Rebuild Network");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_RESTART));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getActiveSimulationController();
            if (!ensureNotRunning(controller))
                return;
            if (!controller.isNetworkPresent()) {
                MessageDialog.openInformation(getShell(), "Error", "No network has been set up yet.");
                return;
            }
            
            controller.rebuildNetwork();
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }
}
