/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import java.util.Map;
import java.util.Set;

import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.pojo.FieldElement;

/**
 * Wraps an MSG type, provides easy lookup for fields, properties.
 *
 * @author levy
 */
public interface IMsgTypeInfo {
    public IMsgTypeElement getSuperType();

    /** Types used locally in this type */
    public Set<IMsgTypeElement> getLocalUsedTypes();

    /** Fields declared locally in this type */
    public Map<String, FieldElement> getLocalFields();

    /** Property nodes, including inherited ones; the most recent one for each property.
     * (Given the special inheritance rules for properties, this may not be what you want;
     * see getPropertyInheritanceChain().
     */
    public Map<String, Map<String, PropertyElementEx>> getProperties();

    /** Fields including inherited ones */
    public Map<String, FieldElement> getFields();

    /**
     * Returns the fully qualified C++ class name using the @namespace property and the type's name.
     */
    public String getFullyQualifiedCppClassName();
}
