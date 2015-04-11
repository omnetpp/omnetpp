/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import org.omnetpp.ned.model.ex.ConnectionElementEx;

/**
 * Validator used for checking connection names.
 *
 * @author andras
 */
public class ConnectionNameValidator extends NedNameValidator {
    private ConnectionElementEx connection;

    public ConnectionNameValidator(ConnectionElementEx connection) {
        this.connection = connection;
    }

    @Override
    public String isValid(Object newValue) {
        if (newValue.equals(""))
            return null; // connection name is optional

        if (connection.getCompoundModule().getNedTypeInfo().getMembers().containsKey(newValue) && !newValue.equals(connection.getName()))
            return "Name is already in use. Connection name must not be the same as an existing" +
                    " submodule, connection, gate, parameter or inner type name.";

        return super.isValid(newValue);
    }
}