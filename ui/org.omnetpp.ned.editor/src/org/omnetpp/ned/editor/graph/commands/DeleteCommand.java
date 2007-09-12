package org.omnetpp.ned.editor.graph.commands;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.ConnectionsElement;

/**
 * Deletes an object from the model and as a special case also removes all
 * associated connections if the object was a SubmoduleElement
 *
 * @author rhornig
 */
public class DeleteCommand extends Command {

    // an inner class to store data to be able to undo a connection deletion
    private static class ConnectionUndoItem {
        public ConnectionElementEx node;            // the NODE that was deleted
        public ConnectionsElement parent;           // the parent of the deleted node
        public ConnectionElementEx nextSibling;     // the next sibling to be able to insert it back into the correct position
        public IConnectableElement srcModule;             // the src module the connection was originally attached to
        public IConnectableElement destModule;            // the dest module the connection was originally attached to
    }

    private static class ElementUndoItem {
        public INEDElement node;
        public INEDElement parent;
        public INEDElement nextSibling;
    }

    private final ElementUndoItem elementUndoItem = new ElementUndoItem();
    private final List<ConnectionUndoItem> connectionUndoItems = new ArrayList<ConnectionUndoItem>();

    public DeleteCommand(INEDElement toBeDeleted) {
    	super();
    	elementUndoItem.node = toBeDeleted;
    	elementUndoItem.parent = toBeDeleted.getParent();
    	elementUndoItem.nextSibling = toBeDeleted.getNextSibling();
    }

    @Override
    public void execute() {
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
    }

    @Override
    public void undo() {
        elementUndoItem.parent.insertChildBefore(elementUndoItem.nextSibling, elementUndoItem.node);
        restoreConnections();
    }

    private void deleteConnections(SubmoduleElementEx module) {
        // TODO maybe it would be enough to iterate through ALL connections one time
        // no need to separate src and dest connections
        for (ConnectionElementEx wire : module.getSrcConnections()) {
            // store all data required to undo the operation
            ConnectionUndoItem uitem = new ConnectionUndoItem();
            uitem.node = wire;
            uitem.parent = (ConnectionsElement)wire.getParent();
            uitem.nextSibling = (ConnectionElementEx)wire.getNextConnectionSibling();
            uitem.srcModule = wire.getSrcModuleRef();
            uitem.destModule = wire.getDestModuleRef();
            connectionUndoItems.add(uitem);
            // now detach the connection from the other module on the destination side
            wire.setDestModuleRef(null);    // detach the destination end of the connections
           	wire.removeFromParent();
        }

        for (ConnectionElementEx wire : module.getDestConnections()) {
            // store all data required to undo the operation
            ConnectionUndoItem uitem = new ConnectionUndoItem();
            uitem.node = wire;
            uitem.parent = (ConnectionsElement)wire.getParent();
            uitem.nextSibling = (ConnectionElementEx)wire.getNextConnectionSibling();
            uitem.srcModule = wire.getSrcModuleRef();
            uitem.destModule = wire.getDestModuleRef();
            connectionUndoItems.add(uitem);
            // now detach the connection from the other module on the destination side
            wire.setSrcModuleRef(null);    // detach the destination end of the connections
            // remove it from the model too
           	wire.removeFromParent();
        }
    }

    private void restoreConnections() {
        // we have to iterate backwards, so all element will be put
        // back into its correct place
        for (int i = connectionUndoItems.size()-1; i>=0; --i) {
            ConnectionUndoItem ui = connectionUndoItems.get(i);
            // attach to the nodes it was originally attached to
            ui.node.setSrcModuleRef(ui.srcModule);
            ui.node.setDestModuleRef(ui.destModule);
            // put it back to the model in the correct position
            ui.parent.insertChildBefore(ui.nextSibling, ui.node);
        }
        connectionUndoItems.clear();
    }
}
