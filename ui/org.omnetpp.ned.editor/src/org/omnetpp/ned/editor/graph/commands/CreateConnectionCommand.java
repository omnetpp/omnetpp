/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.pojo.ImportElement;

/**
 * A command that adds a connection to the given compound module. Optionally adds an import statement
 * and turns the type name into a simple name if the connection has a fully qualified name.
 *
 * @author rhornig, andras
 */
public class CreateConnectionCommand extends Command {
    private ImportElement importElement;
    private String fullyQualifiedTypeName; // needed because redo() destructively modifies the child's type
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
        fullyQualifiedTypeName = NedElementUtilEx.getTypeOrLikeType(conn);
    }

    @Override
    public void execute() {
        redo();
    }

    @Override
    public void redo() {
        parentModule.addConnection(connection);
        importElement = NedElementUtilEx.addImportFor(connection); // note: overwrites "type" (or "like-type") attribute
    }

    @Override
    public void undo() {
        connection.removeFromParent();
        NedElementUtilEx.setTypeOrLikeType(connection, fullyQualifiedTypeName); // restore original value (redo() will need it)
        if (importElement != null)
            importElement.removeFromParent();
    }

    public ConnectionElementEx getConnection() {
        return connection;
    }
}
