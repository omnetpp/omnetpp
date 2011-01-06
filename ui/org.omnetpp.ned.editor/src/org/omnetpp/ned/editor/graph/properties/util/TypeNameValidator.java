/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import org.eclipse.core.resources.IProject;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * Validator used for checking new top level type names. NOTE: Inner types should be validated differently.
 * 
 * @author rhornig, andras (cleanup)
 */
public class TypeNameValidator extends NedNameValidator {
    private INedTypeElement typeElement;
    
    public TypeNameValidator(INedTypeElement type) {
        this.typeElement = type;
    }

    @Override
    public String isValid(Object newText) {
        IProject project = NedResourcesPlugin.getNedResources().getNedFile(typeElement.getContainingNedFileElement()).getProject();
        if (NedResourcesPlugin.getNedResources().getReservedQNames(project).contains(newText) &&
                !newText.equals(typeElement.getName()))
            return "Name is already in use. There is already a type with the same name.";  //FIXME ez szar: package-ekkel mi van???

        return super.isValid(newText);
    }
}