package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasIndex;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IHasProperties;
import org.omnetpp.ned.model.pojo.GateElement;
import org.omnetpp.ned.model.pojo.PropertyKeyElement;

/**
 * Extended GateElement
 *
 * @author rhornig
 */
public class GateElementEx extends GateElement implements IHasIndex, IHasName, IHasProperties {

    protected GateElementEx() {
        super();
    }

    protected GateElementEx(INEDElement parent) {
        super(parent);
    }

    public String getNameWithIndex() {
        String result = getName();
        if (getIsVector())
            result += "["+ ((getVectorSize()==null) ? "" : getVectorSize())+"]";
        return result;
    }

    @SuppressWarnings("unchecked")
    public Map<String, PropertyElementEx> getProperties() {
        Map<String, PropertyElementEx> map = new HashMap<String, PropertyElementEx>();

        INEDElement node = getFirstChildWithTag(NED_PROPERTY);
        while (node != null) {
            if (node instanceof IHasName && node.getTagCode() == NED_PROPERTY)
                ((Map)map).put(((IHasName)node).getName(), (PropertyElementEx)node);
            
            node = node.getNextSibling();
        }

        return map;
    }

    public ArrayList<String> getLabels() {
        PropertyElementEx propertyElement = getProperties().get("labels");
        ArrayList<String> labels = new ArrayList<String>();
        
        if (propertyElement != null)
            for (PropertyKeyElement key = propertyElement.getFirstPropertyKeyChild(); key != null; key = key.getNextPropertyKeySibling())
                labels.add(key.getFirstLiteralChild().getValue());

        return labels;
    }
}
