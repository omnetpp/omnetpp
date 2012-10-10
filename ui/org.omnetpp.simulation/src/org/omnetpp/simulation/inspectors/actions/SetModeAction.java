package org.omnetpp.simulation.inspectors.actions;

import org.eclipse.jface.resource.ImageDescriptor;
import org.omnetpp.simulation.inspectors.ObjectFieldsInspectorPart;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Mode;


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

    @Override
    public void update() {
        ObjectFieldsInspectorPart inspector = (getInspectorPart() instanceof ObjectFieldsInspectorPart) ? (ObjectFieldsInspectorPart)getInspectorPart() : null;
        setEnabled(inspector != null);
        setChecked(inspector != null && inspector.getMode() == mode);
    }
}
