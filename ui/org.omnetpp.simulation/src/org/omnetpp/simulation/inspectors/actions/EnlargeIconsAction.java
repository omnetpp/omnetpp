package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;


public class EnlargeIconsAction extends AbstractInspectorAction {
    public EnlargeIconsAction() {
        super("Enlarge icons", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_ENLARGEICONS));
    }
    
    @Override
    public void run() {
        GraphicalModuleInspectorPart inspector = (GraphicalModuleInspectorPart)getInspectorPart();
        inspector.enlargeIcons();
    }
}
