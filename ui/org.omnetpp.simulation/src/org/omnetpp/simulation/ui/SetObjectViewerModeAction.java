package org.omnetpp.simulation.ui;

import org.eclipse.jface.action.Action;
import org.omnetpp.common.ui.IUpdateableAction;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Mode;


/**
 *
 * @author Andras
 */
public class SetObjectViewerModeAction extends Action implements IUpdateableAction {
    private ObjectFieldsViewer viewer;
    private Mode mode;

    public SetObjectViewerModeAction(ObjectFieldsViewer viewer, Mode mode) {
        super("", AS_CHECK_BOX);
        setImageDescriptor(ObjectFieldsViewer.getImageDescriptorFor(mode));
        setText(ObjectFieldsViewer.getActionTooltipFor(mode));
        setToolTipText(ObjectFieldsViewer.getActionTooltipFor(mode));
        this.mode = mode;
        this.viewer = viewer;
    }

    @Override
    public void run() {
        viewer.setMode(mode);
    }

    @Override
    public void update() {
        setChecked(viewer.getMode() == mode);
    }
}
