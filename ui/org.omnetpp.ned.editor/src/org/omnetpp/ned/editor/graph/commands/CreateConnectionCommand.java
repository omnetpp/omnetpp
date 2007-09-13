package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;

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
public class CreateConnectionCommand extends ConnectionCommand {
    private ImportElement importElement;
    private String fullyQualifiedTypeName;

    public CreateConnectionCommand(ConnectionElementEx conn, CompoundModuleElementEx compoundModuleElement) {
        super(conn, compoundModuleElement);
        Assert.isTrue(conn.getParent() == null, "Connection is already in the model");
        Assert.isNotNull(compoundModuleElement, "Parent compound module must be specified");
        setLabel("Create Connection");
        
        this.fullyQualifiedTypeName = conn.getEffectiveType();  // redo() destructively modifies child's type
    }
    
    @Override
    public void redo() {
        compoundModuleParent.addConnection(newConn);
        importElement = NEDElementUtilEx.addImportFor(newConn); // note: overwrites "type" (or "like-type") attribute
    }
    
    @Override
    public void undo() {
        newConn.removeFromParent();
        NEDElementUtilEx.setEffectiveType(newConn, fullyQualifiedTypeName); // restore original value (redo() will need it)
        if (importElement != null)
            importElement.removeFromParent();
    }
}
