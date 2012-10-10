package org.omnetpp.simulation.inspectors.actions;

import java.io.IOException;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;


/**
 *
 * @author Andras
 */
public class StopAction extends AbstractInspectorAction {
    public StopAction() {
        super("Stop simulation", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_STOP));
    }

    @Override
    public void run() {
        try {
            getSimulationController().stop();
        }
        catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}
