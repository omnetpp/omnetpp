package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.IInspectorContainer;
import org.omnetpp.simulation.model.cObject;

/**
 *
 * @author Andras
 */
//TODO disable if there's no parent!
public class InspectParentAction extends AbstractInspectorAction {
    public InspectParentAction() {
        super("Inspect parent", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_PARENT));
    }

    @Override
    public void run() {
        cObject parent = getInspectorPart().getObject().getOwner();
        if (parent != null) {
            IInspectorContainer container = getInspectorPart().getContainer();
            container.inspect(parent);
        }
    }

    @Override
    public void update() {
        cObject parent = getInspectorPart().getObject().getOwner();
        setEnabled(parent != null);
    }
}
