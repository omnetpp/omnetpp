package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.IConnectable;
import org.omnetpp.ned2.model.IConnectionContainer;
import org.omnetpp.ned2.model.INamedGraphNode;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.ConnectionNode;

/**
 * (Re)assigns a Connection to srcModule/destModule sub/compound module gates and also adds it to the
 * model (to the compound module's connectios section) (or removes it if the new source or destination is NULL)
 * @author rhornig
 */
public class ConnectionCommand extends Command {

    protected IConnectable oldSrcModule;
    protected String oldSrcGate;
    protected IConnectable oldDestModule;
    protected String oldDestGate;
    protected IConnectable srcModule;
    protected String srcGate;
    protected IConnectable destModule;
    protected String destGate;
    protected ConnectionNodeEx connNode;
    protected ConnectionNodeEx connNodeNextSibling = null;
    protected IConnectionContainer parent = null;

    @Override
    public String getLabel() {
        if (connNode != null 
                && srcModule == null && destModule == null)
            return "Delete connection";
        return "Move connection";
    }

    public ConnectionCommand (ConnectionNodeEx conn) {
        connNode = conn;
        oldSrcModule = conn.getSrcModuleRef();
        oldDestModule = conn.getDestModuleRef();
        oldSrcGate = conn.getSrcGate();
        oldDestGate = conn.getDestGate();
    }
    /**
     * Input output gate config consistency can be checked here
     */
    @Override
    public boolean canExecute() {
        return true;
    }

    @Override
    public void execute() {
        if (srcModule != null) 
            connNode.setSrcModuleRef(srcModule);
        
        if (srcGate != null)
            connNode.setSrcGate(srcGate);
        
        if (destModule != null) 
            connNode.setDestModuleRef(destModule);
        
        if (destGate != null)
            connNode.setDestGate(destGate);
        
        // if both src and dest module should be detached then remove it 
        // from the model totally (ie delete it)
        if (srcModule == null && destModule == null) {
            // just store the NEXT sibling so we can put it back during undo to the right place
            connNodeNextSibling = (ConnectionNodeEx)connNode.getNextConnectionNodeSibling();
            // store the parent too so we now where to put it back during undo
            parent = (IConnectionContainer)connNode.getParent().getParent();
            // now detach from both src and dest modules
            connNode.setSrcModuleRef(null);
            connNode.setDestModuleRef(null);
            // and remove from the parent too
           	connNode.removeFromParent();
        }
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public void undo() {
        // if it was removed from the model, put it back
        if (connNode.getParent() == null && parent != null)
            parent.insertConnection(connNodeNextSibling, connNode);
        
        // attach to the original modules and gates
        connNode.setSrcModuleRef(oldSrcModule);
        connNode.setSrcGate(oldSrcGate);
        connNode.setDestModuleRef(oldDestModule);
        connNode.setDestGate(oldDestGate);
    }

    public void setSrcModule(INamedGraphNode newSrcModule) {
        srcModule = newSrcModule;
    }

    public void setSrcGate(String newSrcGate) {
        srcGate = newSrcGate;
    }

    public void setDestModule(INamedGraphNode newDestModule) {
        destModule = newDestModule;
    }

    public void setDestGate(String newDestGate) {
        destGate = newDestGate;
    }
}
