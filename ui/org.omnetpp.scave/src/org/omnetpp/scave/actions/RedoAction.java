/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

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
 * Perform Redo action on the ScaveEditor.
 */
public class RedoAction extends AbstractScaveAction {
    public RedoAction() {
        setText("Redo Scave Operation");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_TOOL_REDO));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        editor.getActiveCommandStack().redo();
    }

    @Override
    public void update() {
        super.update();

        CommandStack activeCommandStack = ScaveEditor.getActiveScaveCommandStack();
        ICommand commandToRedo = activeCommandStack == null ? null : activeCommandStack.getCommandToRedo();
        String label = commandToRedo == null ? "Can't Redo" : ("Redo " + commandToRedo.getLabel());

        setText(label);
        setDescription(label);
        setToolTipText(label);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        CommandStack activeCommandStack = editor.getActiveCommandStack();
		return activeCommandStack != null && activeCommandStack.canRedo();
    }
}
