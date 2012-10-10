package org.omnetpp.simulation.inspectors.actions;

import java.util.List;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.model.cObject;

/**
 * 
 * @author Andras
 */
public class InspectAsObjectAction extends AbstractInspectorAction {
    public InspectAsObjectAction() {
        super("Inspect as object", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_ASOBJECT));
    }

    @Override
    public void run() {
        List<cObject> objects = getObjects();
        IInspectorContainer container = getInspectorContainer();
        container.inspect(objects, true);
    }

    @Override
    public void update() {
        setEnabled(!getObjects().isEmpty());
    }

}
