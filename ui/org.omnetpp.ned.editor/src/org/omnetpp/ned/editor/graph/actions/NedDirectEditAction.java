package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.gef.ui.actions.DirectEditAction;
import org.eclipse.ui.IWorkbenchPart;

import org.omnetpp.ned.editor.graph.edit.policies.PolicyUtil;

/**
 * Specialized DirectEditAction that disables the action if the element is read only
 * @author rhornig
 */
public class NedDirectEditAction extends DirectEditAction {

    public NedDirectEditAction(IWorkbenchPart part) {
        super(part);
    }

    @Override
    protected boolean calculateEnabled() {
        // check if direct edit is possible at all
        if (!super.calculateEnabled())
            return false;
        // allows only the editing of non-readonly parts
        return PolicyUtil.isEditable(getSelectedObjects().get(0));
    }


}
