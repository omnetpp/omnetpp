/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Removes the given object from the model. Nothing happens if the object is not part of the model.
 */
public class RemoveObjectAction extends AbstractScaveAction {
    private EObject element;

    public RemoveObjectAction(EObject element, String text) {
        setText(text);
        setToolTipText("Remove the element");
        this.element = element;
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection structuredSelection) {
        EditingDomain editingDomain = scaveEditor.getEditingDomain();
        Command command = RemoveCommand.create(editingDomain, element);
        scaveEditor.executeCommand(command);
    }

    @Override
    public boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return !selection.isEmpty();
    }
}
