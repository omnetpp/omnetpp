package org.omnetpp.ned.core;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeInfo;
import org.omnetpp.ned.model.interfaces.ITypeElement;
import org.omnetpp.ned.model.pojo.FieldElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;

// TODO: implement caching if performance turns out to be bad
public class MsgTypeInfo implements IMsgTypeInfo, NEDElementTags {
    protected IMsgTypeElement componentNode;
    
    protected MsgResources resources;

    public MsgTypeInfo(IMsgTypeElement node) {
        componentNode = node;
        resources = NEDResourcesPlugin.getMSGResources();
    }

    public ITypeElement getFirstExtendsRef() {
        String name = componentNode.getFirstExtends();
        
        if (name == null)
            return null;
        else
            return resources.lookupMsgType(name);
    }

    public Set<IMsgTypeElement> getLocalUsedTypes() {
        Set<IMsgTypeElement> usedTypes = new HashSet<IMsgTypeElement>();
        
        for (FieldElement field = (FieldElement)componentNode.getFirstChildWithTag(NED_FIELD); field != null; field = field.getNextFieldSibling()) {
            IMsgTypeElement usedType = resources.lookupMsgType(field.getDataType());
            
            if (usedType != null)
                usedTypes.add(usedType);
        }

        return usedTypes;
    }

    public Map<String, FieldElement> getFields() {
        Map<String, FieldElement> fields = new HashMap<String, FieldElement>();
        for (FieldElement field = (FieldElement)componentNode.getFirstChildWithTag(NED_FIELD); field != null; field = field.getNextFieldSibling())
            fields.put(field.getName(), field);

        return fields;
    }

    public Map<String, PropertyElementEx> getProperties() {
        // TODO:
        return new HashMap<String, PropertyElementEx>();
    }
}
