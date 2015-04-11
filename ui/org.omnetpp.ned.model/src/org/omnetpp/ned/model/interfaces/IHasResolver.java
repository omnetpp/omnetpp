/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.INedElement;

/**
 * NED element that knows the INedTypeResolver it belongs to.
 *
 * @author andras
 */
public interface IHasResolver extends INedElement {

    /**
     * The NED type resolver this element belongs to. Never null.
     */
    public INedTypeResolver getResolver();

}
