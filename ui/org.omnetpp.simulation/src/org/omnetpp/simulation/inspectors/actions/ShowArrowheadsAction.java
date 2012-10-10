package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;

/**
 *
 * @author Andras
 */
public class ShowArrowheadsAction extends AbstractInspectorAction {
    public ShowArrowheadsAction() {
        super("Show arrowheads", AS_CHECK_BOX, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_ARROWHEADS));
    }

    @Override
    public void run() {
        GraphicalModuleInspectorPart inspector = (GraphicalModuleInspectorPart)getInspectorPart();
        inspector.setShowArrowHeads(isChecked());
    }

    @Override
    public void update() {
        GraphicalModuleInspectorPart inspector = (getInspectorPart() instanceof GraphicalModuleInspectorPart) ? (GraphicalModuleInspectorPart)getInspectorPart() : null;
        setEnabled(inspector != null);
        setChecked(inspector != null && inspector.getShowArrowHeads());
    }
}
