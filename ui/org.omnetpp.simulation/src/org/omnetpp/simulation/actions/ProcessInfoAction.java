package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.controller.SimulationController.SimState;
import org.omnetpp.simulation.editors.SimulationEditor;

/**
 * Brings up a dialog with properties of the simulation process we are attached to.
 *
 * @author Andras
 */
public class ProcessInfoAction extends AbstractSimulationAction {
    public ProcessInfoAction(SimulationEditor editor) {
        super(editor);
        setText("Process Info");
        setToolTipText("Process Info");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_PROCESSINFO));
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getSimulationController();

            if (!haveSimulation(controller))
                return;

            String[] items = new String[] {
                    "Host", controller.getHostName(),
                    "Port", ""+controller.getPortNumber(),
                    "URL", controller.getUrlBase(),
                    "Command line", "<TODO>",  //TODO
                    "Process Id", ""+controller.getProcessId(),
                    "", "",
                    "Configuration name", controller.getConfigName(),
                    "Run number", ""+controller.getRunNumber(),
                    "Network name", controller.getNetworkName(),
                    "Simulation state", controller.getState().name(),
                    "Last event's event number", ""+controller.getLastEventNumber(),
                    "Last event's simulation time", controller.getLastEventSimulationTime().toString(),
            };

            // TODO this is a quick'n'ugly solution, we need a nicer looking dialog!
            String message = "";
            for (int i=0; i<items.length; i+= 2)
                message += items[i].length()==0 ? "\n" : items[i] + ":\t\t" + items[i+1] + "\n";
            MessageDialog.openInformation(getShell(), "Process Information", message);
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }

    @Override
    public void updateState() {
        SimState state = getSimulationController().getState();
        setEnabled(state != SimState.DISCONNECTED);
    }
}
