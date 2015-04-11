/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import java.util.List;
import java.util.Map;

import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.ParamElementEx;

/**
 * Interface for acquiring parameter lists for the object
 *
 * @author rhornig
 */
public interface IHasParameters extends INedElement {

    /**
     * Returns the name - parameter node association. The returned map must not be modified.
     *
     * "Best-Effort": This method never returns null, but the returned list
     * may be incomplete if some NED type is incorrect, missing, or duplicate.
     */
    public Map<String, ParamElementEx> getParamDeclarations();

    /**
     * Returns the name - parameter node association where the last parameter - value
     * assignment was done. The returned map must not be modified.
     *
     * "Best-Effort": This method never returns null, but the returned list
     * may be incomplete if some NED type is incorrect, missing, or duplicate.
     */
    public Map<String, ParamElementEx> getParamAssignments();

    /**
     * TODO:
     */
    public List<ParamElementEx> getParameterInheritanceChain(String parameterName);
}
