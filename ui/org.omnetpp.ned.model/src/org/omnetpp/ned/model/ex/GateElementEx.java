/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ex;

import java.util.HashMap;
import java.util.Map;

import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.IHasIndex;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IHasProperties;
import org.omnetpp.ned.model.pojo.GateElement;

/**
 * Extended GateElement
 *
 * @author rhornig
 */
public class GateElementEx extends GateElement implements IHasIndex, IHasName, IHasProperties {

    protected GateElementEx() {
        super();
    }

    protected GateElementEx(INedElement parent) {
        super(parent);
    }

    public String getNameWithIndex() {
        String result = getName();
        if (getIsVector())
            result += "["+ ((getVectorSize()==null) ? "" : getVectorSize())+"]";
        return result;
    }

    public Map<String, Map<String, PropertyElementEx>> getProperties() {
        HashMap<String, Map<String, PropertyElementEx>> map = new HashMap<String, Map<String, PropertyElementEx>>();
        NedElementUtilEx.collectProperties(this, map);
        return map;
    }
}
