//==========================================================================
// CNEDDECLARATION.CC -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cneddeclaration.h"
#include "cexception.h"
#include "cproperties.h"
#include "cdynamicexpression.h"
#include "cgate.h"
#include "cmodule.h"
#include "cdisplaystring.h"
#include "cnedloader.h"
#include "nederror.h"
#include "stringutil.h"

USING_NAMESPACE


cNEDDeclaration::cNEDDeclaration(NEDResourceCache *resolver, const char *qname, NEDElement *tree) : NEDTypeInfo(resolver, qname, tree)
{
    props = NULL;
}

cNEDDeclaration::~cNEDDeclaration()
{
    if (props && props->removeRef()==0)
        delete props;
    clearPropsMap(paramPropsMap);
    clearPropsMap(gatePropsMap);
    clearPropsMap(submodulePropsMap);
    clearPropsMap(connectionPropsMap);

    //XXX printf("%s: %d cached expressions\n", getName(), parimplMap.size());
    clearSharedParImplMap(parimplMap);
}

void cNEDDeclaration::clearPropsMap(StringPropsMap& propsMap)
{
    // decrement refs in the props maps, and delete object if refcount reaches zero
    for (StringPropsMap::iterator it = propsMap.begin(); it!=propsMap.end(); ++it)
        if (it->second->removeRef()==0)
            delete it->second;
    propsMap.clear();
}

void cNEDDeclaration::clearSharedParImplMap(SharedParImplMap& parimplMap)
{
    for (SharedParImplMap::iterator it = parimplMap.begin(); it!=parimplMap.end(); ++it)
        delete it->second;
    parimplMap.clear();
}

cNEDDeclaration *cNEDDeclaration::getSuperDecl() const
{
    cNEDDeclaration *decl = dynamic_cast<cNEDDeclaration *>(NEDTypeInfo::getSuperDecl());
    ASSERT(decl);
    return decl;
}

void cNEDDeclaration::putIntoPropsMap(StringPropsMap& propsMap, const std::string& name, cProperties *props) const
{
    StringPropsMap::const_iterator it = propsMap.find(name);
    ASSERT(it==propsMap.end()); //XXX or?
    propsMap[name] = props;
    props->addRef();
}

cProperties *cNEDDeclaration::getFromPropsMap(const StringPropsMap& propsMap, const std::string& name) const
{
    StringPropsMap::const_iterator it = propsMap.find(name);
    return it==propsMap.end() ? NULL : it->second;
}

cProperties *cNEDDeclaration::getProperties() const
{
    cProperties *props = doProperties();
    if (!props)
        throw cRuntimeError("Internal error in NED type `%s': no properties", getFullName());
    return props;
}

cProperties *cNEDDeclaration::doProperties() const
{
    if (props)
        return props; // already computed

    // get inherited properties
    if (numExtendsNames()!=0)
        props = getSuperDecl()->doProperties();

    // update with local properties
    props = mergeProperties(props, getParametersElement());
    props->addRef();
    return props;
}

cProperties *cNEDDeclaration::getParamProperties(const char *paramName) const
{
    cProperties *props = doParamProperties(paramName);
    if (!props)
        throw cRuntimeError("Internal error in NED type `%s': no properties for parameter %s", getFullName(), paramName);
    return props;
}

cProperties *cNEDDeclaration::doParamProperties(const char *paramName) const
{
    cProperties *props = getFromPropsMap(paramPropsMap, paramName);
    if (props)
        return props; // already computed

    // get inherited properties
    if (numExtendsNames()!=0)
        props = getSuperDecl()->doParamProperties(paramName);

    // update with local properties
    ParametersElement *paramsNode = getParametersElement();
    NEDElement *paramNode = paramsNode ? paramsNode->getFirstChildWithAttribute(NED_PARAM, "name", paramName) : NULL;
    if (!paramNode && !props)
        return NULL;  // error: parameter not found anywhere
    props = mergeProperties(props, paramNode);
    putIntoPropsMap(paramPropsMap, paramName, props);
    return props;
}

cProperties *cNEDDeclaration::getGateProperties(const char *gateName) const
{
    cProperties *props = doGateProperties(gateName);
    if (!props)
        throw cRuntimeError("Internal error in NED type `%s': no properties for gate %s", getFullName(), gateName);
    return props;
}

cProperties *cNEDDeclaration::doGateProperties(const char *gateName) const
{
    cProperties *props = getFromPropsMap(gatePropsMap, gateName);
    if (props)
        return props; // already computed

    // get inherited properties
    if (numExtendsNames()!=0)
        props = getSuperDecl()->doGateProperties(gateName);

    // update with local properties
    GatesElement *gatesNode = getGatesElement();
    NEDElement *gateNode = gatesNode ? gatesNode->getFirstChildWithAttribute(NED_GATE, "name", gateName) : NULL;
    if (!gateNode && !props)
        return NULL;  // error: gate not found anywhere
    props = mergeProperties(props, gateNode);
    putIntoPropsMap(gatePropsMap, gateName, props);
    return props;
}

cProperties *cNEDDeclaration::getSubmoduleProperties(const char *submoduleName, const char *submoduleType) const
{
    cProperties *props = doSubmoduleProperties(submoduleName, submoduleType);
    if (!props)
        throw cRuntimeError("Internal error in NED type `%s': no properties for submodule %s, type %s", getFullName(), submoduleName, submoduleType);
    return props;

}
cProperties *cNEDDeclaration::doSubmoduleProperties(const char *submoduleName, const char *submoduleType) const
{
    std::string key = std::string(submoduleName) + ":" + submoduleType;
    cProperties *props = getFromPropsMap(submodulePropsMap, key.c_str());
    if (props)
        return props; // already computed

    // get inherited properties: either from base type (if this is an inherited submodule),
    // or from its type decl.
    if (numExtendsNames()!=0)
        props = getSuperDecl()->doSubmoduleProperties(submoduleName, submoduleType);
    if (!props)
        props = cNEDLoader::getInstance()->getDecl(submoduleType)->getProperties();

    // update with local properties
    NEDElement *subcomponentNode = getSubmoduleElement(submoduleName);
    if (!subcomponentNode && !props)
        return NULL; // error: no such submodule FIXME must not return NULL!
    NEDElement *paramsNode = subcomponentNode ? subcomponentNode->getFirstChildWithTag(NED_PARAMETERS) : NULL;
    props = mergeProperties(props, paramsNode);
    putIntoPropsMap(submodulePropsMap, key.c_str(), props);
    return props;
}

cProperties *cNEDDeclaration::getConnectionProperties(int connectionId, const char *channelType) const
{
    cProperties *props = doConnectionProperties(connectionId, channelType);
    if (!props)
        throw cRuntimeError("Internal error in NED type `%s': no properties for connection with id=%d type=%s", getFullName(), connectionId, channelType);
    return props;
}

cProperties *cNEDDeclaration::doConnectionProperties(int connectionId, const char *channelType) const
{
    std::string key = opp_stringf("%d:%s", connectionId, channelType);
    cProperties *props = getFromPropsMap(connectionPropsMap, key.c_str());
    if (props)
        return props; // already computed

    // get inherited properties: either from base type (if this is an inherited connection),
    // or from the channel type's type decl.
    if (numExtendsNames()!=0)
        props = getSuperDecl()->doConnectionProperties(connectionId, channelType);
    if (!props)
        props = cNEDLoader::getInstance()->getDecl(channelType)->getProperties();

    // update with local properties
    NEDElement *connectionNode = getConnectionElement(connectionId);
    if (!connectionNode && !props)
        return NULL; // error: no such connection

    NEDElement *connspecNode = connectionNode ? connectionNode->getFirstChildWithTag(NED_CHANNEL_SPEC) : NULL;
    NEDElement *paramsNode = connspecNode ? connspecNode->getFirstChildWithTag(NED_PARAMETERS) : NULL;
    props = mergeProperties(props, paramsNode);
    putIntoPropsMap(connectionPropsMap, key.c_str(), props);
    return props;
}

cProperties *cNEDDeclaration::mergeProperties(const cProperties *baseprops, NEDElement *parent)
{
    // returns parent's properties merged with props; both can be NULL.
    // retval is never NULL but can be an empty cProperties.
    // the props object doesn't get modified -- if it would have to be modified,
    // it gets dupped first.
    cProperties *props = const_cast<cProperties *>(baseprops);
    if (!parent)
        return props ? props : new cProperties();
    NEDElement *firstPropertyChild = parent->getFirstChildWithTag(NED_PROPERTY);
    if (!firstPropertyChild)
        return props ? props : new cProperties();

    props = props ? props->dup() : new cProperties();
    for (NEDElement *child=firstPropertyChild; child; child=child->getNextSiblingWithTag(NED_PROPERTY))
    {
        PropertyElement *propNode = (PropertyElement *)child;
        const char *propName = propNode->getName();
        const char *propIndex = propNode->getIndex();
        if (!propIndex[0]) propIndex = NULL;  // no index is NULL not ""
        cProperty *prop = props->get(propName, propIndex);
        if (!prop)
            props->add(prop = new cProperty(propName, propIndex));
        if (!strcmp(propName, "display"))
            updateDisplayProperty(propNode, prop);
        else
            updateProperty(propNode, prop);
    }
    return props;
}

void cNEDDeclaration::updateProperty(PropertyElement *propNode, cProperty *prop)
{
    prop->setIsImplicit(propNode->getIsImplicit());

    for (NEDElement *child=propNode->getFirstChildWithTag(NED_PROPERTY_KEY); child; child=child->getNextSiblingWithTag(NED_PROPERTY_KEY))
    {
        PropertyKeyElement *propKeyNode = (PropertyKeyElement *)child;
        const char *key = propKeyNode->getName();
        if (!prop->hasKey(key))
            prop->addKey(key);

        // collect values
        int k = 0;
        for (NEDElement *child2=propKeyNode->getFirstChildWithTag(NED_LITERAL); child2; child2=child2->getNextSiblingWithTag(NED_LITERAL), k++)
        {
            LiteralElement *literalNode = (LiteralElement *)child2;
            const char *value = literalNode->getValue(); //XXX what about unitType()?
            if (value && *value)
            {
                if (!strcmp(value, "-"))  // "anti-value"
                    prop->setValue(key, k, "");
                else
                    prop->setValue(key, k, value);
            }
        }
    }
}

void cNEDDeclaration::updateDisplayProperty(PropertyElement *propNode, cProperty *prop)
{
    // @display() has to be treated specially
    // find new display string
    PropertyKeyElement *propKeyNode = (PropertyKeyElement *)propNode->getFirstChildWithTag(NED_PROPERTY_KEY);
    if (!propKeyNode)
        return;
    LiteralElement *literalNode = (LiteralElement *)propKeyNode->getFirstChildWithTag(NED_LITERAL);
    if (!literalNode)
        return;
    const char *newdisplaystring = literalNode->getValue();

    // if old display string is empty, just set it
    if (!prop->hasKey(cProperty::DEFAULTKEY))
        prop->addKey(cProperty::DEFAULTKEY);
    if (prop->getNumValues(cProperty::DEFAULTKEY)==0)
    {
        prop->setValue(cProperty::DEFAULTKEY, 0, newdisplaystring);
        return;
    }

    // merge
    const char *olddisplaystring = prop->value(cProperty::DEFAULTKEY, 0);
    cDisplayString d(olddisplaystring);
    cDisplayString dnew(newdisplaystring);
    d.updateWith(dnew);
    prop->setValue(cProperty::DEFAULTKEY, 0, d.str());
}

cParImpl *cNEDDeclaration::getSharedParImplFor(NEDElement *node)
{
    SharedParImplMap::const_iterator it = parimplMap.find(node->getId());
    //XXX printf("      getExpr: %ld -> %p\n", node->getId(), it==parimplMap.end() ? NULL : it->second);
    return it==parimplMap.end() ? NULL : it->second;
}

void cNEDDeclaration::putSharedParImplFor(NEDElement *node, cParImpl *value)
{
    //XXX printf("      putExpr: %ld -> %p\n", node->getId(), value);
    SharedParImplMap::const_iterator it = parimplMap.find(node->getId());
    ASSERT(it==parimplMap.end()); //XXX or?
    if (it==parimplMap.end())
        parimplMap[node->getId()] = value;
}


