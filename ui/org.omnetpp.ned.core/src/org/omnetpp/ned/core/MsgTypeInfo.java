/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeInfo;
import org.omnetpp.ned.model.pojo.FieldElement;
import org.omnetpp.ned.model.pojo.NamespaceElement;
import org.omnetpp.ned.model.pojo.NedElementTags;

/**
 * Default implementation for IMsgTypeInfo.
 *
 * @author levy
 */
// TODO: implement caching if performance turns out to be bad
public class MsgTypeInfo implements IMsgTypeInfo, NedElementTags {
    protected IMsgTypeElement typeNode;

    public MsgTypeInfo(IMsgTypeElement node) {
        typeNode = node;
    }

    public IMsgTypeElement getSuperType() {
        String name = typeNode.getFirstExtends();

        if (name == null)
            return null;
        else
            return NedResourcesPlugin.getMsgResources().lookupMsgType(name);
    }

    public Set<IMsgTypeElement> getLocalUsedTypes() {
        Set<IMsgTypeElement> usedTypes = new HashSet<IMsgTypeElement>();

        for (FieldElement field = (FieldElement)typeNode.getFirstChildWithTag(NED_FIELD); field != null; field = field.getNextFieldSibling()) {
            IMsgTypeElement usedType = NedResourcesPlugin.getMsgResources().lookupMsgType(field.getDataType());

            if (usedType != null)
                usedTypes.add(usedType);
        }

        return usedTypes;
    }

    public Map<String, FieldElement> getLocalFields() {
        Map<String, FieldElement> fields = new LinkedHashMap<String, FieldElement>();

        for (FieldElement field = (FieldElement)typeNode.getFirstChildWithTag(NED_FIELD); field != null; field = field.getNextFieldSibling())
            fields.put(field.getName(), field);

        return fields;
    }

    public Map<String, FieldElement> getFields() {
        Map<String, FieldElement> fields = new LinkedHashMap<String, FieldElement>();

        IMsgTypeElement typeElement = typeNode;

        while (typeElement != null) {
            fields.putAll(typeElement.getMsgTypeInfo().getLocalFields());
            typeElement = typeElement.getMsgTypeInfo().getSuperType();
        }

        return fields;
    }

    public Map<String, Map<String, PropertyElementEx>> getProperties() {
        // TODO:
        return new LinkedHashMap<String, Map<String, PropertyElementEx>>();
    }

    public String getNamespaceName() {
        NamespaceElement namespaceElement = (NamespaceElement)typeNode.getPreviousSiblingWithTag(NED_NAMESPACE);
        return namespaceElement == null ? "" : namespaceElement.getName();
    }

    public String getFullyQualifiedCppClassName() {
        return StringUtils.joinWithSeparator(getNamespaceName(), "::", typeNode.getName());
    }
}
