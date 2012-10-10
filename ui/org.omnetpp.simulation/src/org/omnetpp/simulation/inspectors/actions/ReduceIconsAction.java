package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;

/**
 * 
 * @author Andras
 */
public class ReduceIconsAction extends AbstractInspectorAction {
    public ReduceIconsAction() {
        super("Reduce icons", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_REDUCEICONS));
    }

    @Override
    public void run() {
        GraphicalModuleInspectorPart inspector = (GraphicalModuleInspectorPart)getInspectorPart();
        inspector.reduceIcons();
    }

    @Override
    public void update() {
        GraphicalModuleInspectorPart inspector = (getInspectorPart() instanceof GraphicalModuleInspectorPart) ? (GraphicalModuleInspectorPart)getInspectorPart() : null;
        setEnabled(inspector != null && inspector.canReduceIcons());
    }
}
