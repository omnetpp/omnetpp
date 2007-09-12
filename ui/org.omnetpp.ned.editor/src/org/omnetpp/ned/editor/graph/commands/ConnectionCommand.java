package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.commands.Command;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;

/**
 * (Re)assigns a Connection to srcModule/destModule sub/compound module gates and also adds it to the
 * model (to the compound module's connections section) (or removes it if both the new source and destination is NULL)
 *
 * @author rhornig
 */
// TODO handling of subgates $i and $o is missing
//FIXME import the channel type used: see CreateSubmoduleCommand and NEDElementUtilEx.addImportFor()
public class ConnectionCommand extends Command {

	protected IConnectableElement oldSrcModule;
	protected IConnectableElement oldDestModule;
	protected ConnectionElement originalConnection = (ConnectionElement)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_CONNECTION);

    protected IConnectableElement srcModule;
    protected IConnectableElement destModule;
	protected ConnectionElement connectionTemplate =(ConnectionElement)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_CONNECTION);
	// connection model to be changed
    protected ConnectionElementEx connModel;
    protected ConnectionElementEx connNodeNextSibling;
    protected CompoundModuleElementEx compoundModuleParent;
    protected boolean creating;

    /**
     * Create, delete, or modify  a connection element
     * @param conn Connection Model
     * @param compoundModuleElement Connection's container (compound module) 
     */
    public ConnectionCommand(ConnectionElementEx conn, CompoundModuleElementEx compoundModuleElement) {
        super("Connection");
        this.connModel = conn;
        this.compoundModuleParent = compoundModuleElement;
        this.oldSrcModule = connModel.getSrcModuleRef();
        this.oldDestModule = connModel.getDestModuleRef();
        this.originalConnection = (ConnectionElement)connModel.dup();
        this.connectionTemplate = (ConnectionElement)connModel.dup();
        // we are creating a new connection 
        creating = (connModel.getParent() == null);
    }

    /**
     * Returns true if we are currently creating a new connection
     */
    public boolean isCreating() {
        return creating;
    }

    /**
     * Returns true if we are reconnecting the source end
     */
    public boolean isSrcMoving() {
        return !isCreating() && srcModule != oldSrcModule;
    }

    /**
     * Returns true if we are reconnecting the source end
     */
    public boolean isDestMoving() {
        return !isCreating() && destModule != oldDestModule;
    }

    @Override
    public void execute() {
    	redo();
    }

    @Override
    public void redo() {
        
        Assert.isTrue(srcModule != null && destModule != null, "Modules cannot be empty");
        Assert.isTrue(StringUtils.isNotBlank(getSrcGate()) && StringUtils.isNotBlank(getDestGate()));

        if (oldSrcModule != srcModule)
            connModel.setSrcModuleRef(srcModule);

        if (connectionTemplate.getSrcGate() != null && !connectionTemplate.getSrcGate().equals(originalConnection.getSrcGate()))
            connModel.setSrcGate(connectionTemplate.getSrcGate());

        if (oldDestModule != destModule)
            connModel.setDestModuleRef(destModule);

        if (connectionTemplate.getDestGate() != null && !connectionTemplate.getDestGate().equals(originalConnection.getDestGate()))
            connModel.setDestGate(connectionTemplate.getDestGate());
        // copy the rest of the connection data (notification will be generated)
        copyConn(connectionTemplate, connModel);

        // if the connection is not yet added to the compound module, add it, so later change notification will be handled correctly
        if (isCreating())
            compoundModuleParent.addConnection(connModel);
    }

    @Override
    public void undo() {

        // attach to the original modules and gates
        connModel.setSrcModuleRef(oldSrcModule);
        connModel.setDestModuleRef(oldDestModule);

        copyConn(originalConnection, connModel);
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

    /**
     * Returns the connection node used as a template, for the command. If the command is executed
     * the model will have the same content as the template connection.
     */
    public ConnectionElement getConnectionTemplate() {
        return connectionTemplate;
    }

    public IConnectableElement getSrcModule() {
        return srcModule;
    }
    
    public void setSrcModule(IConnectableElement newSrcModule) {
        srcModule = newSrcModule;
    }

    public IConnectableElement getDestModule() {
        return destModule;
    }
    
    public void setDestModule(IConnectableElement newDestModule) {
        destModule = newDestModule;
    }

    public String getSrcGate() {
    	return connectionTemplate.getSrcGate();
    }

    public void setSrcGate(String newSrcGate) {
        connectionTemplate.setSrcGate(newSrcGate);
    }

    public String getDestGate() {
    	return connectionTemplate.getDestGate();
    }

    public void setDestGate(String newDestGate) {
        connectionTemplate.setDestGate(newDestGate);
    }

    public CompoundModuleElementEx getCompoundModuleModel() {
        return compoundModuleParent;
    }
}
