package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;
import org.omnetpp.ned2.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.ex.ConnectionNodeEx;
import org.omnetpp.ned2.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned2.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned2.model.interfaces.IHasConnections;
import org.omnetpp.ned2.model.interfaces.IHasParent;
import org.omnetpp.ned2.model.pojo.ConnectionNode;
import org.omnetpp.ned2.model.pojo.NEDElementTags;

/**
 * (Re)assigns a Connection to srcModule/destModule sub/compound module gates and also adds it to the
 * model (to the compound module's connectios section) (or removes it if both the new source and destination is NULL)
 * @author rhornig
 */
// TODO handling of subgates $i and $o is missing
public class ConnectionCommand extends Command {

	protected IHasConnections oldSrcModule;
	protected IHasConnections oldDestModule;
	protected ConnectionNode oldConn = (ConnectionNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementTags.NED_CONNECTION);
    
    protected IHasConnections srcModule;
    protected IHasConnections destModule;
	protected ConnectionNode newConn =(ConnectionNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementTags.NED_CONNECTION);
	// connection model to be changed
    protected ConnectionNodeEx connModel;
    protected ConnectionNodeEx connNodeNextSibling = null;
    protected CompoundModuleNodeEx parent = null;
    private ModuleEditPart editPart;

    @Override
    public String getLabel() {
        if (connModel != null && srcModule == null && destModule == null)
            return "Delete connection";
        
        if (connModel != null && srcModule != null && destModule != null)
            return "Create connection";

        return "Move connection";
    }

    /**
     * Create delet a connection element and insert it into the compoundParent as container
     * @param conn Conection Model 
     * @param compoundEditPart Connection's container's (compound module) controller object
     */
    public ConnectionCommand(ConnectionNodeEx conn, ModuleEditPart compoundEditPart) {
        connModel = conn;
        editPart = compoundEditPart;
        oldSrcModule = connModel.getSrcModuleRef();
        oldDestModule = connModel.getDestModuleRef();
        oldConn = (ConnectionNode)connModel.dup(null);
        newConn = (ConnectionNode)connModel.dup(null);
    }
    
    
    /**
     * Handles which module can be connected to which
     */
    @Override
    public boolean canExecute() {
        
        // allow deletion (both module is null)
    	if(srcModule == null && destModule == null)
    		return true;
    	// we can connect two submodules module ONLY if they are siblings (ie they have the same parent)
    	IHasConnections sMod = srcModule != null ? srcModule : connModel.getSrcModuleRef();
    	IHasConnections dMod = destModule != null ? destModule : connModel.getDestModuleRef();

    	if(sMod instanceof SubmoduleNodeEx && dMod instanceof SubmoduleNodeEx &&
    			((IHasParent)sMod).getParent() == ((IHasParent)dMod).getParent()) 
    		return true;
    	
    	// if one module is bubmodule and the other is compound, the compound module MUST contain the submodule
    	if (sMod instanceof CompoundModuleNodeEx && dMod instanceof SubmoduleNodeEx &&
    			((SubmoduleNodeEx)dMod).getCompoundModule() == sMod)
    		return true;
    	
    	if (dMod instanceof CompoundModuleNodeEx && sMod instanceof SubmoduleNodeEx &&
    			((SubmoduleNodeEx)sMod).getCompoundModule() == dMod)
    		return true;
    	
    	// do not allow command in any other cases
        return false;
    }

    @Override
    public void execute() {
   		// do the changes
    	redo();
    }

    @Override
    public void redo() {
        // if both src and dest module should be detached then remove it 
        // from the model totally (ie delete it)
        if (srcModule == null && destModule == null) {
            // just store the NEXT sibling so we can put it back during undo to the right place
            connNodeNextSibling = (ConnectionNodeEx)connModel.getNextConnectionNodeSibling();
            // store the parent too so we now where to put it back during undo
            // FIXME this does not work if connections are placed in connection groups
            parent = (CompoundModuleNodeEx)connModel.getParent().getParent();
            // now detach from both src and dest modules
            connModel.setSrcModuleRef(null);
            connModel.setDestModuleRef(null);
            // and remove from the parent too
            connModel.removeFromParent();
            return;
        }

        // if the connection is not yet added to the compound module, add it, so later change notification will be handled correctly
        if(connModel.getParent() == null) 
            editPart.getCompoundModulePart().getCompoundModuleModel().addConnection(connModel);

        if (srcModule != null && oldSrcModule != srcModule) 
            connModel.setSrcModuleRef(srcModule);
        
        if (newConn.getSrcGate() != null && !newConn.getSrcGate().equals(oldConn.getSrcGate()))
            connModel.setSrcGate(newConn.getSrcGate());
        
        if (destModule != null && oldDestModule != destModule) 
            connModel.setDestModuleRef(destModule);
        
        if (newConn.getDestGate() != null && !newConn.getDestGate().equals(oldConn.getDestGate()))
            connModel.setDestGate(newConn.getDestGate());
        // copy the rest of the connection data (notification will be generated)
        copyConn(newConn, connModel);
        
    }

    @Override
    public void undo() {
        // if it was removed from the model, put it back
        if (connModel.getParent() == null && parent != null)
            parent.insertConnection(connNodeNextSibling, connModel);
        
        // attach to the original modules and gates
        connModel.setSrcModuleRef(oldSrcModule);
        connModel.setDestModuleRef(oldDestModule);

        copyConn(oldConn, connModel);
    }

	/**
	 * Utility method to copy base connection properties (except module names) from one 
	 * connectionNode to the other
	 * @param from
	 * @param to
	 */
	public static void copyConn(ConnectionNode from, ConnectionNode to) {
//        to.setSrcModule(from.getSrcModule());
		to.setSrcModuleIndex(from.getSrcModuleIndex());
        to.setSrcGate(from.getSrcGate());
        to.setSrcGateIndex(from.getSrcGateIndex());
        to.setSrcGatePlusplus(from.getSrcGatePlusplus());
        to.setSrcGateSubg(from.getSrcGateSubg());
        
//        to.setDestModule(from.getDestModule());
        to.setDestModuleIndex(from.getDestModuleIndex());
        to.setDestGate(from.getDestGate());
        to.setDestGateIndex(from.getDestGateIndex());
        to.setDestGatePlusplus(from.getDestGatePlusplus());
        to.setDestGateSubg(from.getDestGateSubg());
        
        to.setArrowDirection(from.getArrowDirection());
	}

    public void setSrcModule(IHasConnections newSrcModule) {
        srcModule = newSrcModule;
    }

    public void setDestModule(IHasConnections newDestModule) {
        destModule = newDestModule;
    }

    public String getSrcGate() {
    	return newConn.getSrcGate();
    }
    
    public void setSrcGate(String newSrcGate) {
        newConn.setSrcGate(newSrcGate);
    }

    public String getDestGate() {
    	return newConn.getDestGate();
    }

    public void setDestGate(String newDestGate) {
        newConn.setDestGate(newDestGate);
    }

	public IHasConnections getDestModule() {
		return destModule;
	}

	public IHasConnections getSrcModule() {
		return srcModule;
	}
    
    /**
     * @return The connection node used as a template, for the command. If the command is executed
     * the model will have the same content as the temaplate connection.
     */
    public ConnectionNode getConnectionTemplate() {
    	return newConn;
    }
 
	
}
