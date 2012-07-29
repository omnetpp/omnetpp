package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;


public class ShowArrowheadsAction extends AbstractInspectorAction {
    public ShowArrowheadsAction() {
        super("Show arrowheads", AS_CHECK_BOX, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_ARROWHEADS));
    }
    
    @Override
    public void run() {
        GraphicalModuleInspectorPart inspector = (GraphicalModuleInspectorPart)getInspectorPart();
        inspector.setShowArrowHeads(!inspector.getShowArrowHeads());
        setChecked(inspector.getShowArrowHeads());
    }
}
