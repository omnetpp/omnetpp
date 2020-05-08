/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.model.commands.ICommand;

/**
 * Perform Undo action on the ScaveEditor.
 */
public class UndoAction extends AbstractScaveAction {
    public UndoAction() {
        setText("Undo Scave Operation");
        setDescription("Undo Scave Operation");
        setToolTipText("Undo Scave Operation");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_TOOL_UNDO));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        editor.getActiveCommandStack().undo();
    }

    @Override
    public void update() {
        super.update();

        CommandStack activeCommandStack = ScaveEditor.getActiveScaveCommandStack();
        ICommand commandToUndo = activeCommandStack == null ? null : activeCommandStack.getCommandToUndo();
        String label = commandToUndo == null ? "Can't Undo" : ("Undo " + commandToUndo.getLabel());

        setText(label);
        setDescription(label);
        setToolTipText(label);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        CommandStack activeCommandStack = editor.getActiveCommandStack();
        return activeCommandStack != null && activeCommandStack.canUndo();
    }
}
