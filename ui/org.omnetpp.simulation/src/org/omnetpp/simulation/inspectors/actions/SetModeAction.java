package org.omnetpp.simulation.inspectors.actions;

import org.eclipse.jface.resource.ImageDescriptor;
import org.omnetpp.simulation.inspectors.ObjectFieldsInspectorPart;
import org.omnetpp.simulation.inspectors.ObjectFieldsViewer.Mode;


/**
 * 
 * @author Andras
 */
public class SetModeAction extends AbstractInspectorAction {
    private Mode mode;
    
    public SetModeAction(Mode mode, String tooltip, ImageDescriptor desc) {
        super(tooltip, AS_CHECK_BOX, desc);
        this.mode = mode;
    }
    
    @Override
    public void run() {
        ObjectFieldsInspectorPart inspector = (ObjectFieldsInspectorPart) getInspectorPart();
        inspector.setMode(mode);
    }
}
