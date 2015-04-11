/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;

/**
 * Deletes an object from the model and as a special case also removes all
 * associated connections if the object was a SubmoduleElement
 *
 * @author rhornig
 */
public class DeleteCommand extends Command {
    private static class ElementUndoItem {
        public INedElement node;
        public INedElement parent;
        public INedElement nextSibling;
    }

    private final ElementUndoItem elementUndoItem = new ElementUndoItem();
    private final List<ElementUndoItem> connectionUndoItems = new ArrayList<ElementUndoItem>();

    public DeleteCommand(INedElement toBeDeleted) {
        elementUndoItem.node = toBeDeleted;
    }

    @Override
    public void execute() {
        elementUndoItem.parent = elementUndoItem.node.getParent();
        elementUndoItem.nextSibling = elementUndoItem.node.getNextSibling();
        String label = "Delete";
        if (elementUndoItem.node instanceof IHasName)
            label += " "+((IHasName)elementUndoItem.node).getName();
        setLabel(label);

        redo();
    }

    @Override
    public void redo() {
        // check if the element is a submodule, because its connections should be deleted too
        if (elementUndoItem.node instanceof SubmoduleElementEx)
            deleteConnections((SubmoduleElementEx)elementUndoItem.node);

        elementUndoItem.node.removeFromParent();

        // check if the element is a connection, because its group should be deleted too if empty
        if (elementUndoItem.node instanceof ConnectionElementEx)
            deleteEmptyConnectionGroup(elementUndoItem.parent);
    }

    @Override
    public void undo() {
        elementUndoItem.parent.insertChildBefore(elementUndoItem.nextSibling, elementUndoItem.node);
        restoreConnections();
    }

    private void deleteConnections(SubmoduleElementEx module) {
        // use a set so self connections wont be added two times to the collection
        HashSet<ConnectionElementEx> connections = new HashSet<ConnectionElementEx>();
        connections.addAll(module.getSrcConnections());
        connections.addAll(module.getDestConnections());

        for (ConnectionElementEx node : connections) {
            ElementUndoItem uitem = new ElementUndoItem();
            uitem.node = node;
            uitem.parent = node.getParent();
            uitem.nextSibling = node.getNextConnectionSibling();
            connectionUndoItems.add(uitem);
            node.removeFromParent();
            deleteEmptyConnectionGroup(uitem.parent);
        }
    }

    private void deleteEmptyConnectionGroup(INedElement node) {
        if (node instanceof ConnectionGroupElement) {
            ConnectionGroupElement group = (ConnectionGroupElement)node;

            if (group.getFirstConnectionChild() == null) {
                ElementUndoItem uitem = new ElementUndoItem();
                uitem.node = group;
                uitem.parent = group.getParent();
                uitem.nextSibling = group.getNextSibling();
                connectionUndoItems.add(uitem);
                group.removeFromParent();
            }
        }
    }

    private void restoreConnections() {
        // we have to iterate backwards, so all element will be put
        // back into its correct place
        for (int i = connectionUndoItems.size()-1; i>=0; --i) {
            ElementUndoItem ui = connectionUndoItems.get(i);
            ui.parent.insertChildBefore(ui.nextSibling, ui.node);
        }
        connectionUndoItems.clear();
    }
}
