package org.omnetpp.ned.editor.graph.properties.util;

import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;

/**
 * Validator used for checking submodule names.
 */
public class SubmoduleNameValidator extends NedNameValidator {
    CompoundModuleElementEx compoundModuleModel;

    public SubmoduleNameValidator(SubmoduleElementEx element) {
        super(element);
        this.compoundModuleModel = element.getCompoundModule();
    }

    @Override
    public String isValid(Object newText) {
        if (compoundModuleModel.getNEDTypeInfo().getMembers().containsKey(newText) &&
                !newText.equals(originalName))
            return "Name is already in use. Submodule name must not be the same as an existing" +
            		" submodule, gate, parameter or inner type name.";

        return super.isValid(newText);
    }
}