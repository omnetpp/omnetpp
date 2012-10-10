package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;

/**
 *
 * @author Andras
 */
public class ShowSubmoduleNamesAction extends AbstractInspectorAction {
    public ShowSubmoduleNamesAction() {
        super("Show submodule names", AS_CHECK_BOX, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_MODNAMES));
    }

    @Override
    public void run() {
        GraphicalModuleInspectorPart inspector = (GraphicalModuleInspectorPart)getInspectorPart();
        inspector.setShowNameLabels(isChecked());
    }

    @Override
    public void update() {
        GraphicalModuleInspectorPart inspector = (getInspectorPart() instanceof GraphicalModuleInspectorPart) ? (GraphicalModuleInspectorPart)getInspectorPart() : null;
        setEnabled(inspector != null);
        setChecked(inspector != null && inspector.getShowNameLabels());
    }
}
