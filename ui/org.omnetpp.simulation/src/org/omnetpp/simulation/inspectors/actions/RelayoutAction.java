package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;


/**
 * 
 * @author Andras
 */
public class RelayoutAction extends AbstractInspectorAction {
    public RelayoutAction() {
        super("Re-layout", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_RELAYOUT));
    }

    @Override
    public void run() {
        GraphicalModuleInspectorPart inspector = (GraphicalModuleInspectorPart)getInspectorPart();
        inspector.relayout();
    }
}
