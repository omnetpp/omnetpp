/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import java.util.Map;

import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.GateElementEx;

/**
 * Elements that may contain gates
 *
 * @author rhornig
 */
public interface IHasGates extends INedElement {

    /**
     * Returns the list of all gate elements.
     *
     * "Best-Effort": This method never returns null, but the returned list
     * may be incomplete if some NED type is incorrect, missing, or duplicate.
     */
    Map<String, GateElementEx> getGateDeclarations();

    /**
     * Returns the list of all gate elements where size is specified.
     *
     * "Best-Effort": This method never returns null, but the returned list
     * may be incomplete if some NED type is incorrect, missing, or duplicate.
     */
    Map<String, GateElementEx> getGateSizes();

}
