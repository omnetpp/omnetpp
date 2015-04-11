/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ex;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.LiteralElement;
import org.omnetpp.ned.model.pojo.PropertyElement;
import org.omnetpp.ned.model.pojo.PropertyKeyElement;

/**
 * Extended property element.
 * @author rhornig, andras
 */
public class PropertyElementEx extends PropertyElement implements IHasName {
    public final static String DEFAULT_PROPERTY_INDEX = "";

    public PropertyElementEx() {
        super();
    }

    public PropertyElementEx(INedElement parent) {
        super(parent);
    }

    /**
     * Returns the first value from the default key (named "") if exists;
     * otherwise returns null.
     */
    public String getSimpleValue() {
        for (INedElement child : this)
            if (child instanceof PropertyKeyElement && StringUtils.isEmpty(((PropertyKeyElement)child).getName()))
                for (INedElement grandChild : child)
                    if (grandChild instanceof LiteralElement)
                        return ((LiteralElement)grandChild).getValue();
        return null;
    }

    public String getValue(String key) {
        for (INedElement child : this)
            if (child instanceof PropertyKeyElement && key.equals(((PropertyKeyElement)child).getName()))
                for (INedElement grandChild : child)
                    if (grandChild instanceof LiteralElement)
                        return ((LiteralElement)grandChild).getValue();
        return null;
    }
}
