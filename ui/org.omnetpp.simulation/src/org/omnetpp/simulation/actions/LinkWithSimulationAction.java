package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.SimulationController;

/**
 * The "Kill simulation on editor close" action.
 *
 * @author Andras
 */
public class LinkWithSimulationAction extends AbstractSimulationAction {
    public LinkWithSimulationAction(SimulationController controller) {
        super(controller, AS_CHECK_BOX);
        setText("Kill simulation on editor close");
        setToolTipText("Kill simulation on editor close");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_LINK));
        setChecked(true);  // the best guess until the first updateState() comes
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getSimulationController();
            if (!haveSimulation(controller))
                return;

            controller.setCancelJobOnDispose(isChecked());
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
        setEnabled(getSimulationController().canCancelLaunch());
        setChecked(getSimulationController().getCancelJobOnDispose());
    }
}
