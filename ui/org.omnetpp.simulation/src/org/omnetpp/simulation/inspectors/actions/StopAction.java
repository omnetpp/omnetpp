package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.IInspectorContainer;
import org.omnetpp.simulation.model.cComponent;


public class StopAction extends AbstractInspectorAction {
    public StopAction() {
        super("Stop simulation", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_STOP));
    }
    
    @Override
    public void run() {
        cComponent component = (cComponent) getInspectorPart().getObject();
        IInspectorContainer container = getInspectorPart().getContainer();
        //TODO
    }
}
