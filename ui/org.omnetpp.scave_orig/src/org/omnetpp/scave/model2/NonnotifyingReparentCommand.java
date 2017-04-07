/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

/**
 *
 */
package org.omnetpp.scave.model2;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.domain.EditingDomain;

/**
 * An undoable command which moves an element under a new parent, without
 * firing REMOVE notifications. By using this command, it can be avoided that
 * editor pages displaying the moved object close because they mistakenly think
 * that the object got removed from the model. (Unfortunately, EMF doesn't seem
 * to have a REPARENTED or MOVED notification, so the only option is to swallow
 * the REMOVE.)
 *
 * NOTE: this command should be used within a CompoundCommmand together with other
 * (notifying!) commands, so that enough notifications get sent to keep the UI
 * up to date.
 *
 * @author Andras
 */
public class NonnotifyingReparentCommand extends CompoundCommand {
    private EObject oldParent;
    private EObject newParent;

    public NonnotifyingReparentCommand(EditingDomain editingDomain, EObject element, EObject newParent, int index) {
        Assert.isTrue(element != null && newParent != null);
        this.oldParent = element.eContainer();
        this.newParent = newParent;
        append(RemoveCommand.create(editingDomain, element));
        append(AddCommand.create(editingDomain, newParent, null, element));
    }

    @Override
    public void execute() {
        boolean oldEDeliver = oldParent.eDeliver();
        oldParent.eSetDeliver(false);
        super.execute();
        oldParent.eSetDeliver(oldEDeliver);
    }

    @Override
    public void redo() {
        boolean oldEDeliver = oldParent.eDeliver();
        oldParent.eSetDeliver(false);
        super.redo();
        oldParent.eSetDeliver(oldEDeliver);
    }

    @Override
    public void undo() {
        boolean oldEDeliver = newParent.eDeliver();
        newParent.eSetDeliver(false);
        super.undo();
        newParent.eSetDeliver(oldEDeliver);
    }

}