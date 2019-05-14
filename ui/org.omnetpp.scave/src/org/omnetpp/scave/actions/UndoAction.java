/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.commands.CommandStack;

/**
 * Perform Undo action on the ScaveEditor.
 */
public class UndoAction extends AbstractScaveAction {
    public UndoAction() {
        setText("Undo");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_TOOL_UNDO));
    }

    @Override
    protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
        editor.getActiveCommandStack().undo();
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        CommandStack activeCommandStack = editor.getActiveCommandStack();
		return activeCommandStack != null && activeCommandStack.canUndo();
    }
}
