package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.commands.Command;

import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.pojo.ImportElement;

/**
 * A command that adds a connection to the given compound module. Optionally adds an import statement
 * and turns the type name into a simple name if the connection has a fully qualified name.
 *   
 * @author rhornig
 */
public class CreateConnectionCommand extends Command {
    private ImportElement importElement;
    private String fullyQualifiedTypeName;
    // the containing compound module
    protected CompoundModuleElementEx parentModule;
    // a template element used to store the requested connection
    protected ConnectionElementEx connection;

    public CreateConnectionCommand(ConnectionElementEx conn, CompoundModuleElementEx compoundModuleElement) {
        Assert.isTrue(conn.getParent() == null, "Connection is already in the model");
        Assert.isNotNull(compoundModuleElement, "Parent compound module must be specified");
        setLabel("Create Connection");
        connection = conn;
        parentModule = compoundModuleElement;
        this.fullyQualifiedTypeName = conn.getEffectiveType();  // redo() destructively modifies child's type
    }
    
    @Override
    public void execute() {
        redo();
    }
    
    @Override
    public void redo() {
        parentModule.addConnection(connection);
        importElement = NEDElementUtilEx.addImportFor(connection); // note: overwrites "type" (or "like-type") attribute
    }
    
    @Override
    public void undo() {
        connection.removeFromParent();
        NEDElementUtilEx.setEffectiveType(connection, fullyQualifiedTypeName); // restore original value (redo() will need it)
        if (importElement != null)
            importElement.removeFromParent();
    }

    public ConnectionElementEx getConnection() {
        return connection;
    }
}
