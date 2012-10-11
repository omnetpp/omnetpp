package org.omnetpp.simulation.inspectors.actions;

import org.omnetpp.simulation.inspectors.ObjectFieldsInspectorPart;
import org.omnetpp.simulation.ui.ObjectFieldsViewer;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Mode;


/**
 *
 * @author Andras
 */
public class SetModeAction extends AbstractInspectorAction {
    private Mode mode;

    public SetModeAction(Mode mode) {
        super("", AS_CHECK_BOX, null);
        setImageDescriptor(ObjectFieldsViewer.getImageDescriptorFor(mode));
        setText(ObjectFieldsViewer.getActionTooltipFor(mode));
        setToolTipText(ObjectFieldsViewer.getActionTooltipFor(mode));
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
