package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.interfaces.IConnectableNode;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;

/**
 * (Re)assigns a Connection to srcModule/destModule sub/compound module gates and also adds it to the
 * model (to the compound module's connections section) (or removes it if both the new source and destination is NULL)
 *
 * @author rhornig
 */
// TODO handling of subgates $i and $o is missing
public class ConnectionCommand extends Command {

	protected IConnectableNode oldSrcModule;
	protected IConnectableNode oldDestModule;
	protected ConnectionElement oldConn = (ConnectionElement)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_CONNECTION);

    protected IConnectableNode srcModule;
    protected IConnectableNode destModule;
	protected ConnectionElement newConn =(ConnectionElement)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_CONNECTION);
	// connection model to be changed
    protected ConnectionElementEx connModel;
    protected ConnectionElementEx connNodeNextSibling;
    protected CompoundModuleElementEx parent;
    private CompoundModuleEditPart parentEditPart;
    private ModuleEditPart srcEditPart;
    private ModuleEditPart destEditPart;

    /**
     * Create, delete, or modify  a connection element
     * @param conn Connection Model
     * @param compoundEditPart Connection's container's (compound module) controller object
     * @param sourceEditPart
     * @param targetEditPart
     */
    public ConnectionCommand(ConnectionElementEx conn, CompoundModuleEditPart compoundEditPart,
                                ModuleEditPart sourceEditPart, ModuleEditPart targetEditPart) {
        this.connModel = conn;
        this.parentEditPart = compoundEditPart;
        this.srcEditPart = sourceEditPart;
        this.destEditPart = targetEditPart;
        this.oldSrcModule = connModel.getSrcModuleRef();
        this.oldDestModule = connModel.getDestModuleRef();
        this.oldConn = (ConnectionElement)connModel.dup();
        this.newConn = (ConnectionElement)connModel.dup();
    }

    /**
     * Returns true if we are currently creating a new connection
     */
    public boolean isCreating() {
        // new connections are not inserted into the model
        return connModel.getParent() == null;
    }

    /**
     * Returns true if we are deleting the given connection
     */
    public boolean isDeleting() {
        // deleting if both src and dest module is null
        return destModule == null && srcModule == null;
    }

    /**
     * Returns true if we are reconnecting the source end
     */
    public boolean isSrcMoving() {
        return !isCreating() && !isDeleting() && srcModule != oldSrcModule;
    }

    /**
     * Returns true if we are reconnecting the source end
     */
    public boolean isDestMoving() {
        return !isCreating() && !isDeleting() && destModule != oldDestModule;
    }

    @Override
    public String getLabel() {
        if (connModel != null && isDeleting())
            return "Delete connection";

        return "Connect";
    }

    /**
     * Handles which module can be connected to which
     */
    @Override
    public boolean canExecute() {
        return srcEditPart!=null && destEditPart!=null &&
           (srcEditPart.getCompoundModulePart() == destEditPart.getCompoundModulePart());
    }

    @Override
    public void execute() {
    	redo();
    }

    @Override
    public void redo() {
        // if both src and dest module should be detached then remove it
        // from the model totally (ie delete it)
        if (srcModule == null && destModule == null) {
            // just store the NEXT sibling so we can put it back during undo to the right place
            connNodeNextSibling = (ConnectionElementEx)connModel.getNextConnectionElementSibling();
            // store the parent too so we now where to put it back during undo
            // FIXME this does not work if connections are placed in connection groups
            parent = (CompoundModuleElementEx)connModel.getParent().getParent();
            // and remove from the parent too
            parentEditPart.getCompoundModuleModel().removeConnection(connModel);
            return;
        }

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

        // if the connection is not yet added to the compound module, add it, so later change notification will be handled correctly
        if (connModel.getParent() == null)
            parentEditPart.getCompoundModulePart().getCompoundModuleModel().addConnection(connModel);
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
	public static void copyConn(ConnectionElement from, ConnectionElement to) {
		to.setSrcModuleIndex(from.getSrcModuleIndex());
        to.setSrcGate(from.getSrcGate());
        to.setSrcGateIndex(from.getSrcGateIndex());
        to.setSrcGatePlusplus(from.getSrcGatePlusplus());
        to.setSrcGateSubg(from.getSrcGateSubg());

        to.setDestModuleIndex(from.getDestModuleIndex());
        to.setDestGate(from.getDestGate());
        to.setDestGateIndex(from.getDestGateIndex());
        to.setDestGatePlusplus(from.getDestGatePlusplus());
        to.setDestGateSubg(from.getDestGateSubg());

        to.setArrowDirection(from.getArrowDirection());
	}

    public void setSrcModule(IConnectableNode newSrcModule) {
        srcModule = newSrcModule;
    }

    public void setDestModule(IConnectableNode newDestModule) {
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

	public IConnectableNode getDestModule() {
		return destModule;
	}

	public IConnectableNode getSrcModule() {
		return srcModule;
	}

    /**
     * Returns the connection node used as a template, for the command. If the command is executed
     * the model will have the same content as the template connection.
     */
    public ConnectionElement getConnectionTemplate() {
    	return newConn;
    }

    /**
     * Returns the parent editPart of this connection
     */
    public CompoundModuleEditPart getParentEditPart() {
        return parentEditPart;
    }

    public ModuleEditPart getDestEditPart() {
        return destEditPart;
    }

    public void setDestEditPart(ModuleEditPart destEditPart) {
        this.destEditPart = destEditPart;
    }

    public ModuleEditPart getSrcEditPart() {
        return srcEditPart;
    }

    public void setSrcEditPart(ModuleEditPart srcEditPart) {
        this.srcEditPart = srcEditPart;
    }


}
