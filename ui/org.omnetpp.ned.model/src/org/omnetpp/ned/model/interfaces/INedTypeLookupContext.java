/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.INedElement;


/**
 * Marker interface for elements in whose context NED type names
 * need to be resolved (compound module and NED file)
 *
 * @author andras
 */
public interface INedTypeLookupContext extends INedElement, IHasResolver {
    /**
     * Returns an empty string or a string ending in a dot. For a NED file
     * this string contains the package name, and for a container type
     * (compound module) it contains the fully qualified name of the module.
     */
    public String getQNameAsPrefix();
}