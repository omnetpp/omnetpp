/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts.policies;

import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editpolicies.TreeContainerEditPolicy;
import org.eclipse.gef.requests.ChangeBoundsRequest;
import org.eclipse.gef.requests.CreateRequest;
import org.omnetpp.ned.editor.graph.commands.ReorderCommand;
import org.omnetpp.ned.model.INedElement;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class NedTreeContainerEditPolicy extends TreeContainerEditPolicy {

    @SuppressWarnings("unchecked")
    @Override
    protected Command getMoveChildrenCommand(ChangeBoundsRequest request) {
        CompoundCommand command = new CompoundCommand();
        int newIndex = findIndexOfTreeItemAt(request.getLocation());
        EditPart insertBeforeEditPart = null;
        if (newIndex >= 0 && newIndex < getHost().getChildren().size() )
            insertBeforeEditPart = (EditPart)getHost().getChildren().get(newIndex);

        for (EditPart editPart2move : (List<EditPart>)request.getEditParts()) {
            if (insertBeforeEditPart == null ||
                    insertBeforeEditPart.getModel() == editPart2move.getModel()) {
              command.add(UnexecutableCommand.INSTANCE);
              return command;
            }
            command.add(new ReorderCommand((INedElement)insertBeforeEditPart.getModel(), (INedElement)editPart2move.getModel()));
        }
        return command;
    }

    @Override
    protected Command getAddCommand(ChangeBoundsRequest request) {
        return null;
    }

    @Override
    protected Command getCreateCommand(CreateRequest request) {
        return null;
    }

}