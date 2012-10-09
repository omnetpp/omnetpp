package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.IInspectorContainer;
import org.omnetpp.simulation.model.cObject;


public class InspectAsObject extends AbstractInspectorAction {
    public InspectAsObject() {
        super("Inspect as object", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_ASOBJECT));
    }

    @Override
    public void run() {
        cObject object = getInspectorPart().getObject();
        IInspectorContainer container = getInspectorPart().getContainer();
        //TODO container.inspect(parent);
    }
}
