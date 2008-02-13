//==========================================================================
// CNEDDECLARATION.CC -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

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
    clearPropsMap(subcomponentPropsMap);
    clearPropsMap(subcomponentParamPropsMap);
    clearPropsMap(subcomponentGatePropsMap);
    //XXX printf("%s: %d cached expressions\n", name(), expressionMap.size());
    clearExpressionMap(expressionMap);
}

void cNEDDeclaration::clearPropsMap(PropertiesMap& propsMap)
{
    // decrement refs in the props maps, and delete object if refcount reaches zero
    for (PropertiesMap::iterator it = propsMap.begin(); it!=propsMap.end(); ++it)
        if (it->second->removeRef()==0)
            delete it->second;
    propsMap.clear();
}

void cNEDDeclaration::clearExpressionMap(ExpressionMap& exprMap)
{
    for (ExpressionMap::iterator it = exprMap.begin(); it!=exprMap.end(); ++it)
        delete it->second;
    exprMap.clear();
}

cNEDDeclaration *cNEDDeclaration::getSuperDecl() const
{
    cNEDDeclaration *decl = dynamic_cast<cNEDDeclaration *>(NEDTypeInfo::getSuperDecl());
    ASSERT(decl);
    return decl;
}

void cNEDDeclaration::putIntoPropsMap(PropertiesMap& propsMap, const std::string& name, cProperties *props) const
{
    PropertiesMap::const_iterator it = propsMap.find(name);
    ASSERT(it==propsMap.end()); //XXX or?
    propsMap[name] = props;
    props->addRef();
}

cProperties *cNEDDeclaration::getFromPropsMap(const PropertiesMap& propsMap, const std::string& name) const
{
    PropertiesMap::const_iterator it = propsMap.find(name);
    return it==propsMap.end() ? NULL : it->second;
}

cProperties *cNEDDeclaration::properties() const
{
    if (props)
        return props; // already computed

    // get inherited properties
    if (numExtendsNames()!=0)
        props = getSuperDecl()->properties();

    // update with local properties
    props = mergeProperties(props, getParametersElement());
    props->addRef();
    return props;
}

cProperties *cNEDDeclaration::paramProperties(const char *paramName) const
{
    cProperties *props = getFromPropsMap(paramPropsMap, paramName);
    if (props)
        return props; // already computed

    // get inherited properties
    if (numExtendsNames()!=0)
        props = getSuperDecl()->paramProperties(paramName);

    // update with local properties
    ParametersElement *paramsNode = getParametersElement();
    NEDElement *paramNode = paramsNode ? paramsNode->getFirstChildWithAttribute(NED_PARAM, "name", paramName) : NULL;
    if (!paramNode && !props)
        return NULL;  // error: parameter not found anywhere
    props = mergeProperties(props, paramNode);
    putIntoPropsMap(paramPropsMap, paramName, props);
    return props;
}

cProperties *cNEDDeclaration::gateProperties(const char *gateName) const
{
    cProperties *props = getFromPropsMap(gatePropsMap, gateName);
    if (props)
        return props; // already computed

    // get inherited properties
    if (numExtendsNames()!=0)
        props = getSuperDecl()->gateProperties(gateName);

    // update with local properties
    GatesElement *gatesNode = getGatesElement();
    NEDElement *gateNode = gatesNode ? gatesNode->getFirstChildWithAttribute(NED_GATE, "name", gateName) : NULL;
    if (!gateNode && !props)
        return NULL;  // error: gate not found anywhere
    props = mergeProperties(props, gateNode);
    putIntoPropsMap(gatePropsMap, gateName, props);
    return props;
}

cProperties *cNEDDeclaration::subcomponentProperties(const char *subcomponentName, const char *subcomponentType) const
{
    std::string key = std::string(subcomponentName) + ":" + subcomponentType;
    cProperties *props = getFromPropsMap(subcomponentPropsMap, key.c_str());
    if (props)
        return props; // already computed

    // get inherited properties: either from base type (if this is an inherited submodule),
    // or from its type decl.
    if (numExtendsNames()!=0)
        props = getSuperDecl()->subcomponentProperties(subcomponentName, subcomponentType);
    if (!props)
        props = cNEDLoader::instance()->getDecl(subcomponentType)->properties();

    // update with local properties
    NEDElement *subcomponentNode = getSubcomponentElement(subcomponentName);
    if (!subcomponentNode && !props)
        return NULL; // error: no such submodule or channel
    NEDElement *paramsNode = subcomponentNode ? subcomponentNode->getFirstChildWithTag(NED_PARAMETERS) : NULL;
    props = mergeProperties(props, paramsNode);
    putIntoPropsMap(subcomponentPropsMap, key.c_str(), props);
    return props;
}

cProperties *cNEDDeclaration::subcomponentParamProperties(const char *subcomponentName, const char *subcomponentType, const char *paramName) const
{
    std::string key = std::string(subcomponentName)+":"+subcomponentType+"."+paramName;
    cProperties *props = getFromPropsMap(subcomponentParamPropsMap, key.c_str());
    if (props)
        return props; // already computed

    // get inherited properties: either from base type (if this is an inherited submodule),
    // or from its type decl.
    if (numExtendsNames()!=0)
        props = getSuperDecl()->subcomponentParamProperties(subcomponentName, subcomponentType, paramName);
    if (!props)
        props = cNEDLoader::instance()->getDecl(subcomponentType)->paramProperties(paramName);

    // update with local properties
    NEDElement *subcomponentNode = getSubcomponentElement(subcomponentName);
    if (!subcomponentNode && !props)
        return NULL; // error: no such submodule or channel
    NEDElement *paramsNode = subcomponentNode ? subcomponentNode->getFirstChildWithTag(NED_PARAMETERS) : NULL;
    NEDElement *paramNode = paramsNode ? paramsNode->getFirstChildWithAttribute(NED_PARAM, "name", paramName) : NULL;
    if (!paramNode && !props)
        return NULL;  // error: parameter not found anywhere
    props = mergeProperties(props, paramNode);
    putIntoPropsMap(subcomponentParamPropsMap, key.c_str(), props);
    return props;
}

cProperties *cNEDDeclaration::subcomponentGateProperties(const char *subcomponentName, const char *subcomponentType, const char *gateName) const
{
    std::string key = std::string(subcomponentName)+":"+subcomponentType+"."+gateName;
    cProperties *props = getFromPropsMap(subcomponentGatePropsMap, key.c_str());
    if (props)
        return props; // already computed

    // get inherited properties: either from base type (if this is an inherited submodule),
    // or from its type decl.
    if (numExtendsNames()!=0)
        props = getSuperDecl()->subcomponentGateProperties(subcomponentName, subcomponentType, gateName);
    if (!props)
        props = cNEDLoader::instance()->getDecl(subcomponentType)->gateProperties(gateName);

    // update with local properties
    NEDElement *subcomponentNode = getSubcomponentElement(subcomponentName);
    if (!subcomponentNode && !props)
        return NULL; // error: no such submodule or channel
    NEDElement *gatesNode = subcomponentNode ? subcomponentNode->getFirstChildWithTag(NED_GATES) : NULL;
    NEDElement *gateNode = gatesNode ? gatesNode->getFirstChildWithAttribute(NED_GATE, "name", gateName) : NULL;
    if (!gateNode && !props)
        return NULL;  // error: parameter not found anywhere
    props = mergeProperties(props, gateNode);
    putIntoPropsMap(subcomponentGatePropsMap, key.c_str(), props);
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
    if (prop->numValues(cProperty::DEFAULTKEY)==0)
    {
        prop->setValue(cProperty::DEFAULTKEY, 0, newdisplaystring);
        return;
    }

    // merge
    const char *olddisplaystring = prop->value(cProperty::DEFAULTKEY, 0);
    cDisplayString d(olddisplaystring);
    cDisplayString dnew(newdisplaystring);
    d.updateWith(dnew);
    prop->setValue(cProperty::DEFAULTKEY, 0, d.toString());
}

cParValue *cNEDDeclaration::getCachedExpression(ExpressionElement *expr)
{
    ExpressionMap::const_iterator it = expressionMap.find(expr->getId());
    //XXX printf("      getExpr: %ld -> %p\n", expr->getId(), it==expressionMap.end() ? NULL : it->second);
    return it==expressionMap.end() ? NULL : it->second;
}

void cNEDDeclaration::putCachedExpression(ExpressionElement *expr, cParValue *value)
{
    //XXX printf("      putExpr: %ld -> %p\n", expr->getId(), value);
    ExpressionMap::const_iterator it = expressionMap.find(expr->getId());
    ASSERT(it==expressionMap.end()); //XXX or?
    if (it==expressionMap.end())
        expressionMap[expr->getId()] = value;
}

//-------------------

/*
void cNEDDeclaration::assertParMatchesBase(cPar *par, const cPar *basepar,
                                                const char *parname, const char *basename)
{
    if (basepar->isSet() && par->isSet())
        throw cRuntimeError(this, "parameter `%s' already set in base `%s', it cannot be overridden", parname, basename);

    if (par->type()!=basepar->type() || par->isVolatile()!=ifpar->isVolatile())
        throw cRuntimeError(this, "type of parameter `%s' differs from that in base `%s'", parname, basename);
}

bool cNEDDeclaration::parMatchesInterface(cPar *par, const cPar *ifpar)
{
    if (par->type()!=basepar->type() || par->isVolatile()!=ifpar->isVolatile())
        throw cRuntimeError(this, "type of parameter `%s' differs from that in interface `%s'", parname, interfacename);

    if (ifpar->isSet()) // values must match as well
    {
        if (!par->isSet())
            throw cRuntimeError(this, "parameter `%s' must have the same value as in interface `%s'", parname, interfacename);

        if (ifpar->isConstant())
        {
            if (!par->isConstant())
                throw cRuntimeError(this, "parameter `%s' must have the same value as in interface `%s'", parname, interfacename);
            if (!const_cast<cPar&>(ifpar).equals(*par))   //FIXME const_cast -- eliminate
                throw cRuntimeError(this, "parameter `%s' must have the same value as in interface `%s'", parname, interfacename);
        }
        else
        {
            // we accept expressions without any check -- comparison cannot be performed 100% accurately
        }
    }
}
*/

/*
void cInterfaceNEDDeclaration::resolve()
{
...
    // verify compliance to interfaces
    for (int i=0; i<interfacenames.size(); i++)
    {
        const char *interfacename = interfacenames[i].c_str();

        // find and resolve interface declaration
        cInterfaceNEDDeclaration *ifdecl = findInterfaceDeclaration(interfacename);
        if (!ifdecl)
            throw cRuntimeError(this, "declaration for interface `%s' not found", interfacename);
        if (!ifdecl->isResolved())
            ifdecl->resolve();

        // check parameters
        for (int i=0; i<ifdecl->numPars(); i++)
        {
            int k = findPar(ifdecl->parName(i));
            if (k<0)
                throw cRuntimeError(this, "has no parameter `%s' required by interface `%s'", ifdecl->parName(i), interfacename);
            verifyParameterMatch(params[k].value, ifdecl->par(i), ifdecl->parName(i), interfacename);
        }

        // check gates
        for (int i=0; i<ifdecl->numGates(); i++)
        {
            int k = findGate(ifdecl->gateName(i));
            if (k<0)
                throw cRuntimeError(this, "has no gate `%s' required by interface `%s'", ifdecl->gateName(i), interfacename);
            GateDescription& g = gates[k];
            if (g.type != ifdecl->gateType(i))
                throw cRuntimeError(this, "type of gate `%s' differs from that in interface `%s'", ifdecl->gateName(i), interfacename);
            if (g.isvector != ifdecl->gateIsVector(i))
                throw cRuntimeError(this, "vectorness of gate `%s' differs from that in interface `%s'", ifdecl->gateName(i), interfacename);
            if (ifdecl->gateSize(i)!=-1 && g.gatesize!=ifdecl->gateSize(i))
                throw cRuntimeError(this, "size of gate vector `%s[]' differs from that in interface `%s'", ifdecl->gateName(i), interfacename);
        }
    }
}
*/

