/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import java.util.List;

import org.omnetpp.ned.model.pojo.ExtendsElement;


/**
 * An interface for elements that have type info associated and can be extended
 * by other types. They are toplevel elements
 * in a NED file, except property definitions and includes.
 *
 * @author rhornig
 */
public interface INedTypeElement extends IHasResolver, ITypeElement, IHasDisplayString, IHasParameters {

    /**
     * Returns the typeinfo belonging to this NED type. This can be trusted
     * to be not null for all NED types, including duplicate and invalid ones.
     *
     * Only null for detached trees that haven't been seen at all by NEDResources
     * (i.e. not part of any NED file).
     */
    public INedTypeInfo getNedTypeInfo();

    /**
     * Returns the base object's name (Only the first extends node name returned).
     * Returns null if no base object exist or the object (or its inheritance chain)
     * is invalid (i.e. contains a cycle)
     */
    public String getFirstExtends();

    /**
     * Sets the first "extends" node to the given string. If name is null or the
     * empty string, the "extends" node will be removed.
     */
    public void setFirstExtends(String name);

    /**
     * Equivalent to <code>getNedTypeInfo().getSuperType()</code>.
     */
    public INedTypeElement getSuperType();

    /**
     * Returns the list of child elements that hold the "extends" names (usually only
     * a single element, but for interfaces it can be more)
     */
    public List<ExtendsElement> getAllExtends();

    /**
     * For an inner type it returns the containing compound module type, and
     * for a toplevel type it returns the containing NED file.
     */
    public INedTypeLookupContext getParentLookupContext();

}
