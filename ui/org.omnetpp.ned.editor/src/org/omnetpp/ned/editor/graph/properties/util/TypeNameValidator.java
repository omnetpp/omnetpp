/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Set;

import org.eclipse.core.resources.IProject;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;

/**
 * Validator used for checking new type names.
 *
 * @author rhornig, andras (cleanup)
 */
public class TypeNameValidator extends NedNameValidator {
    private INedTypeElement typeElement;

    public TypeNameValidator(INedTypeElement type) {
        this.typeElement = type;
    }

    @Override
    public String isValid(Object simpleName) {
        INedTypeResolver resolver = typeElement.getResolver();
        IProject project = resolver.getNedFile(typeElement.getContainingNedFileElement()).getProject();
        String namePrefix = typeElement.getNedTypeInfo().getNamePrefix();
        Set<String> existingQTypeNames = resolver.getReservedQNames(project);  // top level types
        if (typeElement.getEnclosingLookupContext() instanceof CompoundModuleElementEx) // add inner types if needed
            existingQTypeNames.addAll(resolver.getLocalTypeNames(typeElement.getEnclosingLookupContext(), INedTypeResolver.ALL_FILTER));

        if (existingQTypeNames.contains(namePrefix+simpleName) &&
                !simpleName.equals(typeElement.getName()))
            return "Name is already in use. There is already a type with the same name.";

        return super.isValid(simpleName);
    }
}