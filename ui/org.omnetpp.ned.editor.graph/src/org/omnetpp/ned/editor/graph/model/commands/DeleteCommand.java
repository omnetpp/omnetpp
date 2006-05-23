package org.omnetpp.ned.editor.graph.model.commands;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.ISubmoduleContainer;
import org.omnetpp.ned2.model.INamedGraphNode;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;

/**
 * Deletes an object from the model and also removes all associated connections
 * @author rhornig
 *
 */
public class DeleteCommand extends Command {

    // an inner class to store data to be able to undo a connection deletion
    private static class ConnectionUndoItem {
        public ConnectionNodeEx node;            // the NODE that was deleted
        public ConnectionsNode parent;           // the parent of the deletednode
        public ConnectionNodeEx nextSibling;     // the next sibling to be able to insert it back into the correct position
        public INamedGraphNode srcModule;             // the src module the connection was originally attached to
        public INamedGraphNode destModule;            // the dest module the connection was originally attached to
    }
    
    private static class ModuleUndoItem {
        public INamedGraphNode node;
        public ISubmoduleContainer parent;
        public INamedGraphNode nextSibling;
    }

    private int index = -1;
    private ModuleUndoItem moduleUndoItem = new ModuleUndoItem(); 
    private List<ConnectionUndoItem> connectionUndoItems = new ArrayList<ConnectionUndoItem>();

    private void deleteConnections(INamedGraphNode module) {
        // TODO maybe it would be enough to iterate through ALL connections one time
        // no need to separate src and dest connections
        for (ConnectionNodeEx wire : module.getSrcConnections()) {
            // store all data required to undo the operation
            ConnectionUndoItem uitem = new ConnectionUndoItem();
            uitem.node = wire;
            uitem.parent = (ConnectionsNode)wire.getParent();
            uitem.nextSibling = (ConnectionNodeEx)wire.getNextConnectionNodeSibling();
            uitem.srcModule = wire.getSrcModuleRef();
            uitem.destModule = wire.getDestModuleRef();
            connectionUndoItems.add(uitem);
            // now detach the connection from the other module on the destination side
            wire.setDestModuleRef(null);    // detach the destination end of the connections
            // remove it from the model too
            if (wire.getParent() != null)
            	wire.removeFromParent();
        }
        
        for (ConnectionNodeEx wire : module.getDestConnections()) {
            // store all data required to undo the operation
            ConnectionUndoItem uitem = new ConnectionUndoItem();
            uitem.node = wire;
            uitem.parent = (ConnectionsNode)wire.getParent();
            uitem.nextSibling = (ConnectionNodeEx)wire.getNextConnectionNodeSibling();
            uitem.srcModule = wire.getSrcModuleRef();
            uitem.destModule = wire.getDestModuleRef();
            connectionUndoItems.add(uitem);
            // now detach the connection from the other module on the destination side
            wire.setSrcModuleRef(null);    // detach the destination end of the connections
            // remove it from the model too
            if (wire.getParent() != null)
            	wire.removeFromParent();
        }
    }

    @Override
    public void execute() {
        // FIXME label is not used by this comand. Maybe a BUG??? 
        setLabel("Delete " + moduleUndoItem.node.getName());
        primExecute();
    }

    protected void primExecute() {
        deleteConnections(moduleUndoItem.node);
        index = moduleUndoItem.parent.getSubmodules().indexOf(moduleUndoItem.node);
        moduleUndoItem.parent.removeSubmodule(moduleUndoItem.node);
    }

    @Override
    public void redo() {
        primExecute();
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

    public void setChild(INamedGraphNode c) {
        moduleUndoItem.node = c;
    }

    public void setParent(ISubmoduleContainer p) {
        moduleUndoItem.parent = p;
    }

    @Override
    public void undo() {
        moduleUndoItem.parent.insertSubmodule(index, moduleUndoItem.node);
        restoreConnections();
    }

}
