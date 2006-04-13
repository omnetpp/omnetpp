package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.INedModule;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.ConnectionNode;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;

/**
 * (Re)assigns a wire object to srcModule/destModule submodule gates
 * @author rhornig
 *
 */
public class ConnectionCommand extends Command {

    protected INedModule oldSrcModule;
    protected String oldSrcGate;
    protected INedModule oldDestModule;
    protected String oldDestGate;
    protected INedModule srcModule;
    protected String srcGate;
    protected INedModule destModule;
    protected String destGate;
    protected ConnectionNodeEx connNode;
    protected NEDElement parentNode = null;
    protected ConnectionNode connNodeNextSibling = null;

    public String getLabel() {
        if (connNode != null 
                && srcModule == null && destModule == null)
            return "Delete connection";
        return "Move connection";
    }

    
    /**
     * Input output gate config consistency can be checked here
     */
    public boolean canExecute() {
        return true;
    }

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
            connNodeNextSibling = connNode.getNextConnectionNodeSibling();
            // store the parent too so we now where to put it back during undo
            parentNode = connNode.getParent();
            // now detach from both src and dest modules
            connNode.setSrcModuleRef(null);
            connNode.setDestModuleRef(null);
            // and remove from the parent too
            connNode.removeFromParent();
        }
    }

    public void redo() {
        execute();
    }

    public void undo() {
        // if it was removed from the model, put it back
        if (connNode.getParent() == null && parentNode != null)
            parentNode.insertChildBefore(connNodeNextSibling, connNode);
        
        // attach to the original modules and gates
        connNode.setSrcModuleRef(oldSrcModule);
        connNode.setSrcGate(oldSrcGate);
        connNode.setDestModuleRef(oldDestModule);
        connNode.setDestGate(oldDestGate);
    }

    public INedModule getSrcModule() {
        return srcModule;
    }

    public void setSrcModule(INedModule newSrcModule) {
        srcModule = newSrcModule;
    }

    public String getSrcGate() {
        return srcGate;
    }

    public void setSrcGate(String newSrcGate) {
        srcGate = newSrcGate;
    }

    public INedModule getDestModule() {
        return destModule;
    }

    public void setDestModule(INedModule newDestModule) {
        destModule = newDestModule;
    }

    public String getDestGate() {
        return destGate;
    }

    public void setDestGate(String newDestGate) {
        destGate = newDestGate;
    }

    public ConnectionNodeEx getConnectionNode() {
        return connNode;
    }

    public void setConnectionNode(ConnectionNodeEx conn) {
        connNode = conn;
        oldSrcModule = conn.getSrcModuleRef();
        oldDestModule = conn.getDestModuleRef();
        oldSrcGate = conn.getSrcGate();
        oldDestGate = conn.getDestGate();
    }

}
