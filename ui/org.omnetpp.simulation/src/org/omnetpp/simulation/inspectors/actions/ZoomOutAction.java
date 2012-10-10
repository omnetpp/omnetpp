package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;


public class ZoomOutAction extends AbstractInspectorAction {
    public ZoomOutAction() {
        super("Zoom out", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_ZOOMOUT));
    }

    @Override
    public void run() {
        GraphicalModuleInspectorPart inspector = (GraphicalModuleInspectorPart)getInspectorPart();
        inspector.zoomOut();
    }

    @Override
    public void update() {
        GraphicalModuleInspectorPart inspector = (getInspectorPart() instanceof GraphicalModuleInspectorPart) ? (GraphicalModuleInspectorPart)getInspectorPart() : null;
        setEnabled(inspector != null);
        setChecked(inspector != null && inspector.canZoomOut());
    }
}
