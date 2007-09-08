package org.omnetpp.ned.editor.graph.properties.util;

import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;

/**
 * Validator used for checking top level type names. NOTE: Inner types should be validated differently.
 */
public class TypeNameValidator extends NedNameValidator {

    public TypeNameValidator(IHasName element) {
        super(element);
    }

    @Override
    public String isValid(Object newText) {
        if (NEDResourcesPlugin.getNEDResources().getReservedQNames(INEDTypeResolver.FIXME_INSERT_CONTEXTPROJECT_HERE).contains(newText) &&
                !newText.equals(originalName))
            return "Name is already in use. There is already a type with the same name.";

        return super.isValid(newText);
    }
}