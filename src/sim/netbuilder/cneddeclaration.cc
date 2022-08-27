//==========================================================================
// CNEDDECLARATION.CC -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include "common/stringutil.h"
#include "common/patternmatcher.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cdisplaystring.h"
#include "cneddeclaration.h"
#include "nedxml/errorstore.h"
#include "cnedloader.h"

namespace omnetpp {

using namespace omnetpp::common;

cNedDeclaration::cNedDeclaration(NedResourceCache *resolver, const char *qname, bool isInnerType, NedElement *tree) :
    NedTypeInfo(resolver, qname, isInnerType, tree)
{
}

cNedDeclaration::~cNedDeclaration()
{
    if (props && props->removeRef() == 0)
        delete props;
    clearPropsMap(paramPropsMap);
    clearPropsMap(gatePropsMap);
    clearPropsMap(submodulePropsMap);
    clearPropsMap(connectionPropsMap);

    clearSharedParImpls();

    for (auto & pattern : patterns)
        delete pattern.matcher;

    for (auto & submodulePattern : submodulePatterns) {
        std::vector<PatternData>& patternDataItems = submodulePattern.second;
        for (auto & item : patternDataItems)
            delete item.matcher;
    }
}

void cNedDeclaration::clearPropsMap(StringPropsMap& propsMap)
{
    // decrement refs in the props maps, and delete object if refcount reaches zero
    for (auto & it : propsMap)
        if (it.second->removeRef() == 0)
            delete it.second;

    propsMap.clear();
}

void cNedDeclaration::clearSharedParImpls()
{
    for (auto & it : parimplMap)
        delete it.second;
    parimplMap.clear();
}

cNedDeclaration *cNedDeclaration::getSuperDecl() const
{
    cNedDeclaration *decl = dynamic_cast<cNedDeclaration *>(NedTypeInfo::getSuperDecl());
    ASSERT(decl);
    return decl;
}

const std::vector<cNedDeclaration*>& cNedDeclaration::getInheritanceChain()
{
    if (inheritanceChain.empty()) {
        for (cNedDeclaration *d = this; d; d = d->numExtendsNames() == 0 ? nullptr : d->getSuperDecl())
            inheritanceChain.push_back(d);
        std::reverse(inheritanceChain.begin(), inheritanceChain.end());
    }
    return inheritanceChain;
}

void cNedDeclaration::putIntoPropsMap(StringPropsMap& propsMap, const std::string& name, cProperties *props) const
{
    StringPropsMap::const_iterator it = propsMap.find(name);
    ASSERT(it == propsMap.end());  // XXX or?
    propsMap[name] = props;
    props->addRef();
}

cProperties *cNedDeclaration::getFromPropsMap(const StringPropsMap& propsMap, const std::string& name) const
{
    StringPropsMap::const_iterator it = propsMap.find(name);
    return it == propsMap.end() ? nullptr : it->second;
}

cProperties *cNedDeclaration::getProperties() const
{
    cProperties *props = doProperties();
    if (!props)
        throw cRuntimeError("Internal error in NED type '%s': No properties", getFullName());
    return props;
}

cProperties *cNedDeclaration::doProperties() const
{
    if (props)
        return props;  // already computed

    // get inherited properties
    if (numExtendsNames() != 0)
        props = getSuperDecl()->doProperties();

    // update with local properties
    props = updateProperties(props, getParametersElement());
    props->addRef();
    return props;
}

cProperties *cNedDeclaration::getParamProperties(const char *paramName) const
{
    cProperties *props = doParamProperties(paramName);
    if (!props)
        throw cRuntimeError("Internal error in NED type '%s': No properties for parameter %s", getFullName(), paramName);
    return props;
}

cProperties *cNedDeclaration::doParamProperties(const char *paramName) const
{
    cProperties *props = getFromPropsMap(paramPropsMap, paramName);
    if (props)
        return props;  // already computed

    // get inherited properties
    if (numExtendsNames() != 0)
        props = getSuperDecl()->doParamProperties(paramName);

    // update with local properties
    ParametersElement *paramsNode = getParametersElement();
    NedElement *paramNode = paramsNode ? paramsNode->getFirstChildWithAttribute(NED_PARAM, "name", paramName) : nullptr;
    if (!paramNode && !props)
        return nullptr;  // error: parameter not found anywhere
    props = updateProperties(props, paramNode);
    putIntoPropsMap(paramPropsMap, paramName, props);
    return props;
}

cProperties *cNedDeclaration::getGateProperties(const char *gateName) const
{
    cProperties *props = doGateProperties(gateName);
    if (!props)
        throw cRuntimeError("Internal error in NED type '%s': No properties for gate %s", getFullName(), gateName);
    return props;
}

cProperties *cNedDeclaration::doGateProperties(const char *gateName) const
{
    cProperties *props = getFromPropsMap(gatePropsMap, gateName);
    if (props)
        return props;  // already computed

    // get inherited properties
    if (numExtendsNames() != 0)
        props = getSuperDecl()->doGateProperties(gateName);

    // update with local properties
    GatesElement *gatesNode = getGatesElement();
    NedElement *gateNode = gatesNode ? gatesNode->getFirstChildWithAttribute(NED_GATE, "name", gateName) : nullptr;
    if (!gateNode && !props)
        return nullptr;  // error: gate not found anywhere
    props = updateProperties(props, gateNode);
    putIntoPropsMap(gatePropsMap, gateName, props);
    return props;
}

cProperties *cNedDeclaration::getSubmoduleProperties(const char *submoduleName, const char *submoduleType) const
{
    cProperties *props = doSubmoduleProperties(submoduleName, submoduleType);
    if (!props)
        throw cRuntimeError("Internal error in NED type '%s': No properties for submodule %s, type %s", getFullName(), submoduleName, submoduleType);
    return props;
}

cProperties *cNedDeclaration::doSubmoduleProperties(const char *submoduleName, const char *submoduleType) const
{
    std::string key = std::string(submoduleName) + ":" + submoduleType;
    cProperties *props = getFromPropsMap(submodulePropsMap, key.c_str());
    if (props)
        return props;  // already computed

    // get inherited properties: either from base type (if this is an inherited submodule),
    // or from its type decl.
    if (numExtendsNames() != 0)
        props = getSuperDecl()->doSubmoduleProperties(submoduleName, submoduleType);
    if (!props)
        props = cNedLoader::getInstance()->getDecl(submoduleType)->getProperties();

    // update with local properties
    NedElement *subcomponentNode = getLocalSubmoduleElement(submoduleName);
    if (!subcomponentNode && !props)
        return nullptr;  // error: no such submodule FIXME must not return nullptr!
    NedElement *paramsNode = subcomponentNode ? subcomponentNode->getFirstChildWithTag(NED_PARAMETERS) : nullptr;
    props = updateProperties(props, paramsNode);
    putIntoPropsMap(submodulePropsMap, key.c_str(), props);
    return props;
}

cProperties *cNedDeclaration::getConnectionProperties(int connectionId, const char *channelType) const
{
    cProperties *props = doConnectionProperties(connectionId, channelType);
    if (!props)
        throw cRuntimeError("Internal error in NED type '%s': No properties for connection with id=%d type=%s", getFullName(), connectionId, channelType);
    return props;
}

cProperties *cNedDeclaration::doConnectionProperties(int connectionId, const char *channelType) const
{
    std::string key = opp_stringf("%d:%s", connectionId, channelType);
    cProperties *props = getFromPropsMap(connectionPropsMap, key.c_str());
    if (props)
        return props;  // already computed

    // get inherited properties: either from base type (if this is an inherited connection),
    // or from the channel type's type decl.
    if (numExtendsNames() != 0)
        props = getSuperDecl()->doConnectionProperties(connectionId, channelType);
    if (!props)
        props = cNedLoader::getInstance()->getDecl(channelType)->getProperties();

    // update with local properties
    ConnectionElement *connectionNode = getLocalConnectionElement(connectionId);
    if (!connectionNode && !props)
        return nullptr;  // error: no such connection

    NedElement *paramsNode = connectionNode ? connectionNode->getFirstChildWithTag(NED_PARAMETERS) : nullptr;
    props = updateProperties(props, paramsNode);
    putIntoPropsMap(connectionPropsMap, key.c_str(), props);
    return props;
}

cProperties *cNedDeclaration::updateProperties(cProperties *props, NedElement *withPropsParent) const
{
    // returns parent's properties merged with props; both can be nullptr.
    // retval is never nullptr but can be an empty cProperties.
    // the props object doesn't get modified -- if it would have to be modified,
    // it gets dupped first.
    if (!withPropsParent)
        return props ? props : new cProperties();
    NedElement *firstPropertyChild = withPropsParent->getFirstChildWithTag(NED_PROPERTY);
    if (!firstPropertyChild)
        return props ? props : new cProperties();

    props = props ? props->dup() : new cProperties();
    for (NedElement *child = firstPropertyChild; child; child = child->getNextSiblingWithTag(NED_PROPERTY)) {
        PropertyElement *propNode = (PropertyElement *)child;
        const char *propName = propNode->getName();
        const char *propIndex = propNode->getIndex();
        if (!propIndex[0])
            propIndex = nullptr;  // no index is nullptr not ""
        cProperty *prop = props->get(propName, propIndex);
        if (!prop)
            props->add(prop = new cProperty(propName, propIndex));
        if (!strcmp(propName, "display"))
            updateDisplayProperty(prop, propNode);
        else
            updateProperty(prop, propNode);
    }
    return props;
}

void cNedDeclaration::updateProperty(cProperty *prop, PropertyElement *withPropNode) const
{
    prop->setIsImplicit(withPropNode->getIsImplicit());

    for (NedElement *child = withPropNode->getFirstChildWithTag(NED_PROPERTY_KEY); child; child = child->getNextSiblingWithTag(NED_PROPERTY_KEY)) {
        PropertyKeyElement *propKeyNode = (PropertyKeyElement *)child;
        const char *key = propKeyNode->getName();
        if (!prop->containsKey(key))
            prop->addKey(key);

        // collect values
        int k = 0;
        for (NedElement *child2 = propKeyNode->getFirstChildWithTag(NED_LITERAL); child2; child2 = child2->getNextSiblingWithTag(NED_LITERAL), k++) {
            LiteralElement *literalNode = (LiteralElement *)child2;
            const char *value = literalNode->getValue();  // XXX what about unitType()?
            if (value && *value) {
                if (!strcmp(value, "-"))  // "anti-value"
                    prop->setValue(key, k, "", nullptr, nullptr);
                else
                    prop->setValue(key, k, value, literalNode->getSourceFileName(), getFullName());
            }
        }
    }
}

void cNedDeclaration::updateDisplayProperty(cProperty *prop, PropertyElement *withPropNode) const
{
    // @display() has to be treated specially
    // find new display string
    PropertyKeyElement *propKeyNode = (PropertyKeyElement *)withPropNode->getFirstChildWithTag(NED_PROPERTY_KEY);
    if (!propKeyNode)
        return;
    LiteralElement *literalNode = (LiteralElement *)propKeyNode->getFirstChildWithTag(NED_LITERAL);
    if (!literalNode)
        return;
    const char *newDisplayString = literalNode->getValue();

    // if old display string is empty, just set it
    if (!prop->containsKey(cProperty::DEFAULTKEY))
        prop->addKey(cProperty::DEFAULTKEY);
    if (prop->getNumValues(cProperty::DEFAULTKEY) == 0) {
        prop->setValue(cProperty::DEFAULTKEY, 0, newDisplayString, literalNode->getSourceFileName(), getFullName());
        return;
    }

    // merge
    const char *oldDisplayString = prop->getValue(cProperty::DEFAULTKEY, 0);
    cDisplayString d(oldDisplayString);
    cDisplayString dnew(newDisplayString);
    d.updateWith(dnew);
    prop->setValue(cProperty::DEFAULTKEY, 0, d.str(), literalNode->getSourceFileName(), getFullName()); // in fact, different parts of the value may come from different source file dirs, but we cannot represent that
}

internal::cParImpl *cNedDeclaration::getSharedParImplFor(NedElement *node)
{
    auto it = parimplMap.find(node->getId());
    return it == parimplMap.end() ? nullptr : it->second;
}

void cNedDeclaration::putSharedParImplFor(NedElement *node, cParImpl *value)
{
    auto it = parimplMap.find(node->getId());
    ASSERT(it == parimplMap.end());
    parimplMap[node->getId()] = value;
}

const std::vector<cNedDeclaration::PatternData>& cNedDeclaration::getParamPatterns()
{
    if (!patternsValid) {
        // collect param assignment patterns from all super classes (in base-to-derived order)
        for (cNedDeclaration *d : getInheritanceChain()) {
            ParametersElement *paramsNode = d->getParametersElement();
            if (paramsNode)
                collectPatternsFrom(paramsNode, patterns);
        }
        patternsValid = true;
    }
    return patterns;
}

const std::vector<cNedDeclaration::PatternData>& cNedDeclaration::getSubmoduleParamPatterns(const char *submoduleName)
{
    StringPatternDataMap::iterator it = submodulePatterns.find(submoduleName);
    if (it == submodulePatterns.end()) {
        // do super classes as well (due to inherited submodules!)
        std::vector<PatternData>& v = submodulePatterns[submoduleName];  // create

        // collect param assignment patterns from all super classes (in base-to-derived order)
        for (cNedDeclaration *d : getInheritanceChain()) {
            SubmodulesElement *submodsNode = d->getSubmodulesElement();
            if (!submodsNode)
                continue;
            SubmoduleElement *submodNode = (SubmoduleElement *)submodsNode->getFirstChildWithAttribute(NED_SUBMODULE, "name", submoduleName);
            if (!submodNode)
                continue;
            ParametersElement *paramsNode = submodNode->getFirstParametersChild();
            if (!paramsNode)
                continue;
            collectPatternsFrom(paramsNode, v);
        }
        return v;
    }
    return it->second;
}

void cNedDeclaration::collectPatternsFrom(ParametersElement *paramsNode, std::vector<PatternData>& v)
{
    // append pattern elements to v[]
    for (ParamElement *paramNode = paramsNode->getFirstParamChild(); paramNode; paramNode = paramNode->getNextParamSibling()) {
        if (paramNode->getIsPattern()) {
            v.push_back(PatternData());
            v.back().matcher = new PatternMatcher(paramNode->getName(), true, true, true);
            v.back().patternNode = paramNode;
        }
    }
}

}  // namespace omnetpp

