package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;


public class ShowSubmoduleNamesAction extends AbstractInspectorAction {
    public ShowSubmoduleNamesAction() {
        super("Show submodule names", AS_CHECK_BOX, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_MODNAMES));
    }
    
    @Override
    public void run() {
        GraphicalModuleInspectorPart inspector = (GraphicalModuleInspectorPart)getInspectorPart();
        inspector.setShowNameLabels(!inspector.getShowNameLabels());
        setChecked(inspector.getShowNameLabels());
    }
}
