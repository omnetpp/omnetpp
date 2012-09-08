package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.Simulation;
import org.omnetpp.simulation.controller.Simulation.SimState;
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
            Simulation simulation = getSimulationController().getSimulation();

            if (!haveSimulationProcess())
                return;

            String[] items = new String[] {
                    "Host", simulation.getHostName(),
                    "Port", ""+simulation.getPortNumber(),
                    "URL", simulation.getUrlBase(),
                    "Command line", "<TODO>",  //TODO
                    "Process Id", ""+simulation.getProcessId(),
                    "", "",
                    "Configuration name", simulation.getConfigName(),
                    "Run number", ""+simulation.getRunNumber(),
                    "Network name", simulation.getNetworkName(),
                    "Simulation state", simulation.getState().name(),
                    "Last event's event number", ""+simulation.getLastEventNumber(),
                    "Last event's simulation time", simulation.getLastEventSimulationTime().toString(),
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
        SimState state = getSimulationController().getUIState();
        setEnabled(state != SimState.DISCONNECTED);
    }
}
