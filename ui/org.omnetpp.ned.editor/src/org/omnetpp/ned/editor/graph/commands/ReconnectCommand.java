package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.commands.Command;

import org.omnetpp.ned.model.ex.ConnectionElementEx;

/**
 * (Re)assigns a Connection to a different module/gate
 *
 * @author rhornig
 */
// TODO handling of subgates $i and $o is missing
public class ReconnectCommand extends Command {
    
    // contains the provided original ConnectionElement
    protected ConnectionElementEx modelConn;
    protected ConnectionElementEx oldConn;
    protected ConnectionElementEx templateConn;
    protected boolean isDestReconnect;

    /**
     * @param conn Connection to be reconnected
     * @param destIsConnecting Whether the destination side should be reconnected (or the source if false)
     * @param newModuleName The name of the newly assigned module
     */
    public ReconnectCommand(ConnectionElementEx conn, boolean destIsConnecting) {
        Assert.isNotNull(conn, "Connection must be specified");
        
        this.modelConn = conn;
        this.isDestReconnect = destIsConnecting;
        templateConn = (ConnectionElementEx)conn.dup();
        setLabel("Reconnect");
    }
    

    @Override
    public void execute() {
        oldConn = (ConnectionElementEx)modelConn.dup();
        redo();
    }
    
    @Override
    public void redo() {
        copyConn(templateConn, modelConn);
    }

    @Override
    public void undo() {
        copyConn(oldConn, modelConn);
    }

	/**
	 * Utility method to copy base connection properties from one
	 * connectionNode to the other (except the module name)
	 * @param from
	 * @param to
	 */
    // TODO move it somewhere else
	public static void copyConn(ConnectionElementEx from, ConnectionElementEx to) {
	    to.setSrcModule(from.getSrcModule());
		to.setSrcModuleIndex(from.getSrcModuleIndex());
        to.setSrcGate(from.getSrcGate());
        to.setSrcGateIndex(from.getSrcGateIndex());
        to.setSrcGatePlusplus(from.getSrcGatePlusplus());
        to.setSrcGateSubg(from.getSrcGateSubg());

        to.setDestModule(from.getDestModule());
        to.setDestModuleIndex(from.getDestModuleIndex());
        to.setDestGate(from.getDestGate());
        to.setDestGateIndex(from.getDestGateIndex());
        to.setDestGatePlusplus(from.getDestGatePlusplus());
        to.setDestGateSubg(from.getDestGateSubg());

        to.setArrowDirection(from.getArrowDirection());
	}

    /**
     * Returns the connection node used as a template, for the command. If the command is executed
     * this Connection elements attribute values will be set in the model connection.
     */
    public ConnectionElementEx getTemplateConnection() {
        return templateConn;
    }

    /**
     * Tells whether the command reconnects the destination side of the connection 
     */
    public boolean isDestReconnect() {
        return isDestReconnect;
    }

}
