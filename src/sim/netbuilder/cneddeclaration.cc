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
#include "ned2generator.h"
#include "nederror.h"


cNEDDeclaration::cNEDDeclaration(const char *name, NEDElement *tree) :
cNEDDeclarationBase(name), NEDComponent(tree)
{
    // add "extends" and "like" names
    for (NEDElement *child=tree->getFirstChild(); child; child=child->getNextSibling())
    {
        if (child->getTagCode()==NED_EXTENDS)
            extendsnames.push_back(((ExtendsNode *)child)->getName());
        if (child->getTagCode()==NED_INTERFACE_NAME)
            interfacenames.push_back(((InterfaceNameNode *)child)->getName());
    }

    //FIXME copy from parent?
    if (tree->getTagCode()==NED_SIMPLE_MODULE || tree->getTagCode()==NED_CHANNEL)
        implClassName = name;
}

cNEDDeclaration::~cNEDDeclaration()
{
//FIXME TODO
}

std::string cNEDDeclaration::info() const
{
    std::stringstream out;
    if (numExtendsNames()>0)
    {
        out << "extends ";
        for (int i=0; i<numExtendsNames(); i++)
            out << (i?", ":"") << extendsName(i);
        out << "  ";
    }

    if (numInterfaceNames()>0)
    {
        out << "like ";
        for (int i=0; i<numInterfaceNames(); i++)
            out << (i?", ":"") << interfaceName(i);
        out << "  ";
    }

    if (!implClassName.empty())
        out << "C++ class: " << implClassName;

    return out.str();
}

std::string cNEDDeclaration::detailedInfo() const
{
    std::stringstream out;
    out << "Name: " << name();
    if (numExtendsNames()>0)
    {
        out << "extends ";
        for (int i=0; i<numExtendsNames(); i++)
            out << (i?", ":"") << extendsName(i);
        out << "  ";
    }

    if (numInterfaceNames()>0)
    {
        out << "like ";
        for (int i=0; i<numInterfaceNames(); i++)
            out << (i?", ":"") << interfaceName(i);
        out << "  ";
    }
    if (!implClassName.empty())
        out << "\nC++ class: " << implClassName << "\n";
    return out.str();
}

std::string cNEDDeclaration::nedSource() const
{
    std::stringstream out;
    NEDErrorStore errors;
    generateNED2(out, getTree(), &errors);
    return out.str();
}

const char *cNEDDeclaration::interfaceName(int k) const
{
    if (k<0 || k>=interfacenames.size())
        throw new cRuntimeError(this, "interface index %d out of range 0..%d", k, interfacenames.size()-1);
    return interfacenames[k].c_str();
}

const char *cNEDDeclaration::extendsName(int k) const
{
    if (k<0 || k>=extendsnames.size())
        throw new cRuntimeError(this, "extendsName(): index %d out of range 0..%d", k, extendsnames.size()-1);
    return extendsnames[k].c_str();
}

const char *cNEDDeclaration::implementationClassName() const
{
    return implClassName.empty() ? NULL : implClassName.c_str();
}

void cNEDDeclaration::putIntoPropsMap(PropertiesMap& propsMap, const std::string& name, cProperties *props)
{
    PropertiesMap::const_iterator it = propsMap.find(name);
    ASSERT(it==propsMap.end()); //XXX or?
    if (it==propsMap.end())
        propsMap[name] = props;
}

cProperties *cNEDDeclaration::getFromPropsMap(const PropertiesMap& propsMap, const std::string& name) const
{
    PropertiesMap::const_iterator it = propsMap.find(name);
    return it==propsMap.end() ? NULL : it->second;
}

cProperties *cNEDDeclaration::properties() const
{
    return props;
}

cProperties *cNEDDeclaration::paramProperties(const char *paramName) const
{
    return getFromPropsMap(paramPropsMap, paramName);
}

cProperties *cNEDDeclaration::gateProperties(const char *gateName) const
{
    return getFromPropsMap(gatePropsMap, gateName);
}

cProperties *cNEDDeclaration::subcomponentProperties(const char *subcomponentName) const
{
    return getFromPropsMap(subcomponentPropsMap, subcomponentName);
}

cProperties *cNEDDeclaration::subcomponentParamProperties(const char *subcomponentName, const char *paramName) const
{
    return getFromPropsMap(subcomponentParamPropsMap, std::string(subcomponentName)+"."+paramName);
}

cProperties *cNEDDeclaration::subcomponentGateProperties(const char *subcomponentName, const char *gateName) const
{
    return getFromPropsMap(subcomponentGatePropsMap, std::string(subcomponentName)+"."+gateName);
}

ParametersNode *cNEDDeclaration::getParametersNode()
{
    return (ParametersNode *)getTree()->getFirstChildWithTag(NED_PARAMETERS);
}

GatesNode *cNEDDeclaration::getGatesNode()
{
    return (GatesNode *)getTree()->getFirstChildWithTag(NED_GATES);
}

SubmodulesNode *cNEDDeclaration::getSubmodulesNode()
{
    return (SubmodulesNode *)getTree()->getFirstChildWithTag(NED_SUBMODULES);
}

ConnectionsNode *cNEDDeclaration::getConnectionsNode()
{
    return (ConnectionsNode *)getTree()->getFirstChildWithTag(NED_CONNECTIONS);
}

cParValue *cNEDDeclaration::getCachedExpression(ExpressionNode *expr)
{
    ExpressionMap::const_iterator it = expressionMap.find(expr->getId());
printf("      getExpr: %ld -> %p\n", expr->getId(), it==expressionMap.end() ? NULL : it->second);
    return it==expressionMap.end() ? NULL : it->second;
}

void cNEDDeclaration::putCachedExpression(ExpressionNode *expr, cParValue *value)
{
printf("      putExpr: %ld -> %p\n", expr->getId(), value);
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
        throw new cRuntimeError(this, "parameter `%s' already set in base `%s', it cannot be overridden", parname, basename);

    if (par->type()!=basepar->type() || par->isVolatile()!=ifpar->isVolatile())
        throw new cRuntimeError(this, "type of parameter `%s' differs from that in base `%s'", parname, basename);
}

bool cNEDDeclaration::parMatchesInterface(cPar *par, const cPar *ifpar)
{
    if (par->type()!=basepar->type() || par->isVolatile()!=ifpar->isVolatile())
        throw new cRuntimeError(this, "type of parameter `%s' differs from that in interface `%s'", parname, interfacename);

    if (ifpar->isSet()) // values must match as well
    {
        if (!par->isSet())
            throw new cRuntimeError(this, "parameter `%s' must have the same value as in interface `%s'", parname, interfacename);

        if (ifpar->isConstant())
        {
            if (!par->isConstant())
                throw new cRuntimeError(this, "parameter `%s' must have the same value as in interface `%s'", parname, interfacename);
            if (!const_cast<cPar&>(ifpar).equals(*par))   //FIXME const_cast -- eliminate
                throw new cRuntimeError(this, "parameter `%s' must have the same value as in interface `%s'", parname, interfacename);
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
            throw new cRuntimeError(this, "declaration for interface `%s' not found", interfacename);
        if (!ifdecl->isResolved())
            ifdecl->resolve();

        // check parameters
        for (int i=0; i<ifdecl->numPars(); i++)
        {
            int k = findPar(ifdecl->parName(i));
            if (k<0)
                throw new cRuntimeError(this, "has no parameter `%s' required by interface `%s'", ifdecl->parName(i), interfacename);
            verifyParameterMatch(params[k].value, ifdecl->par(i), ifdecl->parName(i), interfacename);
        }

        // check gates
        for (int i=0; i<ifdecl->numGates(); i++)
        {
            int k = findGate(ifdecl->gateName(i));
            if (k<0)
                throw new cRuntimeError(this, "has no gate `%s' required by interface `%s'", ifdecl->gateName(i), interfacename);
            GateDescription& g = gates[k];
            if (g.type != ifdecl->gateType(i))
                throw new cRuntimeError(this, "type of gate `%s' differs from that in interface `%s'", ifdecl->gateName(i), interfacename);
            if (g.isvector != ifdecl->gateIsVector(i))
                throw new cRuntimeError(this, "vectorness of gate `%s' differs from that in interface `%s'", ifdecl->gateName(i), interfacename);
            if (ifdecl->gateSize(i)!=-1 && g.gatesize!=ifdecl->gateSize(i))
                throw new cRuntimeError(this, "size of gate vector `%s[]' differs from that in interface `%s'", ifdecl->gateName(i), interfacename);
        }
    }
}
*/

