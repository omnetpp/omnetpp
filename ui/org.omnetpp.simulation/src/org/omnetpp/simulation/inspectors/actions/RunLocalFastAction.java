package org.omnetpp.simulation.inspectors.actions;

import java.io.IOException;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
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
        catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @Override
    public void update() {
        setEnabled(getModule() != null && getSimulationController().isSimulationOK());
    }
}
