/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.INedFileElement;
import org.omnetpp.ned.model.pojo.MsgFileElement;

public class MsgFileElementEx extends MsgFileElement implements INedFileElement {
    protected MsgFileElementEx() {
    }

    protected MsgFileElementEx(INedElement parent) {
        super(parent);
    }

    public List<IMsgTypeElement> getTopLevelTypeNodes() {
        List<IMsgTypeElement> result = new ArrayList<IMsgTypeElement>();
        for (INedElement currChild : this)
            if (currChild instanceof IMsgTypeElement)
                result.add((IMsgTypeElement)currChild);

        return result;
    }

    @SuppressWarnings("unchecked")
    public Map<String, PropertyElementEx> getProperties() {
        Map<String, PropertyElementEx> map = new HashMap<String, PropertyElementEx>();

        INedElement node = getFirstChildWithTag(NED_PROPERTY);
        while (node != null) {
            if (node instanceof IHasName && node.getTagCode() == NED_PROPERTY)
                ((Map)map).put(((IHasName)node).getName(), (PropertyElementEx)node);

            node = node.getNextSibling();
        }

        return map;
    }
}
