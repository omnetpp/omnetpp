package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.model.cComponent;


/**
 * 
 * @author Andras
 */
public class InspectComponentTypeAction extends AbstractInspectorAction {
    public InspectComponentTypeAction() {
        super("Inspect component type", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_MODTYPE));
    }

    @Override
    public void run() {
        cComponent component = (cComponent) getInspectorPart().getObject();
        IInspectorContainer container = getInspectorPart().getContainer();
        container.inspect(component.getComponentType());
    }
}
