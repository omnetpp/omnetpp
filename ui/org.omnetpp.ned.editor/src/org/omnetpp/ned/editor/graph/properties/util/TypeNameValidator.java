package org.omnetpp.ned.editor.graph.properties.util;

import org.eclipse.core.resources.IProject;

import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.interfaces.IHasName;

/**
 * Validator used for checking top level type names. NOTE: Inner types should be validated differently.
 */
public class TypeNameValidator extends NedNameValidator {

    IHasName element;
    public TypeNameValidator(IHasName element) {
        super(element);
        this.element = element;
    }

    @Override
    public String isValid(Object newText) {
        IProject project = NEDResourcesPlugin.getNEDResources().getNedFile(element.getContainingNedFileElement()).getProject();
        if (NEDResourcesPlugin.getNEDResources().getReservedQNames(project).contains(newText) &&
                !newText.equals(originalName))
            return "Name is already in use. There is already a type with the same name.";

        return super.isValid(newText);
    }
}