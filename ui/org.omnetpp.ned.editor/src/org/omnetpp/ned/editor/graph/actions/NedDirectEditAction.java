/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.gef.ui.actions.DirectEditAction;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.ned.editor.graph.parts.EditPartUtil;

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
        return EditPartUtil.isEditable(getSelectedObjects().get(0));
    }


}
