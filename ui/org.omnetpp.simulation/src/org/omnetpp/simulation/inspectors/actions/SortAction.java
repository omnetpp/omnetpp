package org.omnetpp.simulation.inspectors.actions;

import org.eclipse.jface.resource.ImageDescriptor;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.inspectors.ObjectFieldsInspectorPart;
import org.omnetpp.simulation.inspectors.ObjectFieldsViewer.Ordering;


/**
 * For ObjectFieldsInspectorPart.
 * 
 * @author Andras
 */
public class SortAction extends AbstractInspectorAction {
    private static final ImageDescriptor IMG_SORT = SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_SORT);
    
    public SortAction() {
        super("Sort fields", AS_CHECK_BOX, IMG_SORT);
    }
    
    @Override
    public void run() {
        ObjectFieldsInspectorPart inspector = (ObjectFieldsInspectorPart) getInspectorPart();
        inspector.setOrdering(isChecked() ? Ordering.ALPHABETICAL : Ordering.NATURAL);
    }
}
