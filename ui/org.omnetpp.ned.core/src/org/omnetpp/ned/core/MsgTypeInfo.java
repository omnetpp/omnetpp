package org.omnetpp.ned.core;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.omnetpp.ned.model.ex.MsgFileElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeInfo;
import org.omnetpp.ned.model.pojo.FieldElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;

/**
 * Default implementation for IMsgTypeInfo.
 *
 * @author levy
 */
// TODO: implement caching if performance turns out to be bad
public class MsgTypeInfo implements IMsgTypeInfo, NEDElementTags {
    protected IMsgTypeElement typeNode;
    
    public MsgTypeInfo(IMsgTypeElement node) {
        typeNode = node;
    }

    public IMsgTypeElement getFirstExtendsRef() {
        String name = typeNode.getFirstExtends();
        
        if (name == null)
            return null;
        else
            return NEDResourcesPlugin.getMSGResources().lookupMsgType(name);
    }

    public Set<IMsgTypeElement> getLocalUsedTypes() {
        Set<IMsgTypeElement> usedTypes = new HashSet<IMsgTypeElement>();
        
        for (FieldElement field = (FieldElement)typeNode.getFirstChildWithTag(NED_FIELD); field != null; field = field.getNextFieldSibling()) {
            IMsgTypeElement usedType = NEDResourcesPlugin.getMSGResources().lookupMsgType(field.getDataType());
            
            if (usedType != null)
                usedTypes.add(usedType);
        }

        return usedTypes;
    }

    public Map<String, FieldElement> getLocalFields() {
        Map<String, FieldElement> fields = new HashMap<String, FieldElement>();

        for (FieldElement field = (FieldElement)typeNode.getFirstChildWithTag(NED_FIELD); field != null; field = field.getNextFieldSibling())
            fields.put(field.getName(), field);
        
        return fields;
    }

    public Map<String, FieldElement> getFields() {
        Map<String, FieldElement> fields = new HashMap<String, FieldElement>();
        
        IMsgTypeElement typeElement = typeNode;
        
        while (typeElement != null) {
            fields.putAll(typeElement.getMsgTypeInfo().getLocalFields());
            typeElement = (IMsgTypeElement)typeElement.getMsgTypeInfo().getFirstExtendsRef();
        }

        return fields;
    }

    public Map<String, PropertyElementEx> getProperties() {
        // TODO:
        return new HashMap<String, PropertyElementEx>();
    }

    public String getFullyQualifiedCppClassName() {
        String className = typeNode.getName();

        MsgFileElementEx fileElement = typeNode.getContainingMsgFileElement();
        String namespace = NEDResourcesPlugin.getMSGResources().getCppNamespaceForFile(fileElement);
        
        if (namespace == null)
            return className;
        else
            return namespace + "::" + className;

    }
}
