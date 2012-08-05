package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;

/**
 * 
 * @author Andras
 */
//FIXME comes up in the wrong state (setChecked() is not called after creation)
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
