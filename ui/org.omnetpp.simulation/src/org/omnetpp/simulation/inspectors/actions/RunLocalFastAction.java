package org.omnetpp.simulation.inspectors.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.Simulation.RunMode;
import org.omnetpp.simulation.model.cModule;

/**
 *
 * @author Andras
 */
public class RunLocalFastAction extends AbstractInspectorAction {
    public RunLocalFastAction() {
        super("Fast run until next event in this module", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_MFAST));
    }

    @Override
    public void run() {
        try {
            cModule module = getModule();
            getSimulationController().runLocal(RunMode.FAST, module);
        }
        catch (CommunicationException e) {
            // nothing -- error dialog and logging is already taken care of in the lower layers
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }

    @Override
    public void update() {
        boolean online = getSimulationController().isOnline();
        setEnabled(online && getModule() != null && getSimulationController().isSimulationOK());
    }
}
