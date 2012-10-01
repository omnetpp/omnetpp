package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditor;

/**
 * The Run Until action.
 *
 * @author Andras
 */
public class RunUntilAction extends AbstractSimulationAction {
    public RunUntilAction(SimulationEditor editor) {
        super(editor, AS_CHECK_BOX);
        setText("Run Until...");
        setToolTipText("Run Until...");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_UNTIL));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getSimulationController();
            if (!ensureNetworkReady())
                return;

            RunUntilDialog dialog = new RunUntilDialog(getShell());
            if (dialog.open() == Dialog.OK) {
                controller.runUntil(dialog.getRunMode(), dialog.getSimTime(), dialog.getEventNumber(), dialog.getModule(), dialog.getMessage());
            }
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
        SimState state = getSimulationController().getUIState();
        setEnabled(state == SimState.READY || state == SimState.RUNNING);

        setChecked(getSimulationController().isRunUntilActive());
    }

}
