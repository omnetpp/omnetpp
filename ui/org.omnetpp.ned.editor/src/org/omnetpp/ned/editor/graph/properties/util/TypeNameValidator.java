/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import org.eclipse.core.resources.IProject;

import org.omnetpp.ned.core.NedResourcesPlugin;
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
        IProject project = NedResourcesPlugin.getNedResources().getNedFile(element.getContainingNedFileElement()).getProject();
        if (NedResourcesPlugin.getNedResources().getReservedQNames(project).contains(newText) &&
                !newText.equals(originalName))
            return "Name is already in use. There is already a type with the same name.";

        return super.isValid(newText);
    }
}