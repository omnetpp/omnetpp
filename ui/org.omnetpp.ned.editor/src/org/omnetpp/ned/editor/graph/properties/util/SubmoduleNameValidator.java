/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import org.omnetpp.ned.model.ex.SubmoduleElementEx;

/**
 * Validator used for checking submodule names.
 *
 * @author rhornig, andras (cleanup)
 */
public class SubmoduleNameValidator extends NedNameValidator {
    private SubmoduleElementEx submodule;

    public SubmoduleNameValidator(SubmoduleElementEx submodule) {
        this.submodule = submodule;
    }

    @Override
    public String isValid(Object newValue) {
        if (submodule.getCompoundModule().getNedTypeInfo().getMembers().containsKey(newValue) && !newValue.equals(submodule.getName()))
            return "Name is already in use. Submodule name must not be the same as an existing" +
                    " submodule, connection, gate, parameter or inner type name.";

        return super.isValid(newValue);
    }
}