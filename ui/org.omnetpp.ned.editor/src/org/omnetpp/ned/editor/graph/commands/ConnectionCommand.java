package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;

import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.pojo.ConnectionElement;

public class ConnectionCommand extends Command {
    // we store the references to the modules here because getSrcModuleRef should not be called when
    // the element is not yet added to the model
    protected IConnectableElement srcModule;
    protected IConnectableElement destModule;
    protected IConnectableElement originalSrcModule;
    protected IConnectableElement originalDestModule;
    // contains the provided original ConnectionElement
    protected ConnectionElementEx modelConn;
    // a template element used to store the requested connection
    protected ConnectionElementEx newConn;
    // the connection element used to hold the original values (used for undo)
    protected ConnectionElementEx originalConn;
    // the containing compound module
    protected CompoundModuleElementEx compoundModuleParent;


    /**
     * @param conn A connection element (either in the model or not yet inserted) used 
     * @param compoundModuleElement
     */
    public ConnectionCommand(ConnectionElementEx conn, CompoundModuleElementEx compoundModuleElement) {
        this.modelConn = conn;
        this.newConn = (ConnectionElementEx)conn.dup();
        this.originalConn = (ConnectionElementEx)conn.dup();
        srcModule = originalSrcModule = conn.getSrcModuleRef();
        destModule = originalDestModule = conn.getDestModuleRef();
        this.compoundModuleParent = compoundModuleElement;
    }

    @Override
    public void execute() {
        redo();
    }

    /**
     * Returns the connection node used as a template, for the command. If the command is executed
     * this Connection element will be inserted into the model (and the old one (passed in the constructor)
     * is removed (if it was in the model).
     */
    public ConnectionElement getConnection() {
        return newConn;
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

    public CompoundModuleElementEx getCompoundModuleParent() {
        return compoundModuleParent;
    }

}
