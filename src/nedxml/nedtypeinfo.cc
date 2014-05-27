//==========================================================================
// NEDTYPEINFO.CC -
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

#include <stdio.h>
#include <string.h>
#include <sstream>
#include "stringutil.h"
#include "nederror.h"
#include "nedutil.h"
#include "nedexception.h"
#include "nedtypeinfo.h"
#include "nedresourcecache.h"
#include "ned2generator.h"

NAMESPACE_BEGIN


NEDTypeInfo::NEDTypeInfo(NEDResourceCache *resolver, const char *qname, bool isInnerType, NEDElement *tree)
{
    this->resolver = resolver;
    this->qualifiedName = qname;
    this->isInner = isInnerType;
    this->tree = tree;

    switch (tree->getTagCode()) {
        case NED_SIMPLE_MODULE: type = SIMPLE_MODULE; break;
        case NED_COMPOUND_MODULE: type = COMPOUND_MODULE; break;
        case NED_MODULE_INTERFACE: type = MODULEINTERFACE; break;
        case NED_CHANNEL: type = CHANNEL; break;
        case NED_CHANNEL_INTERFACE: type = CHANNELINTERFACE; break;
        default: throw NEDException("NEDTypeInfo: element of wrong type (<%s>) passed into constructor", tree->getTagName());
    }
    bool isInterface = type==MODULEINTERFACE || type==CHANNELINTERFACE;

    // add "extends" and "like" names, after resolving them
    NEDLookupContext context = NEDResourceCache::getParentContextOf(qname, tree);
    for (NEDElement *child=tree->getFirstChild(); child; child=child->getNextSibling())
    {
        if (child->getTagCode()==NED_EXTENDS) {
            // resolve and store base type name
            const char *extendsname = ((ExtendsElement *)child)->getName();
            std::string extendsqname = getResolver()->resolveNedType(context, extendsname);
            Assert(!extendsqname.empty());
            extendsnames.push_back(extendsqname);

            // check the type
            NEDTypeInfo *decl = getResolver()->lookup(extendsqname.c_str());
            Assert(decl);
            if (getType() != decl->getType())
                throw NEDException(getTree(), "a %s cannot extend a %s (%s)", getTree()->getTagName(), decl->getTree()->getTagName(), extendsqname.c_str());

            // collect interfaces from our base types
            if (isInterface)
                for (int i=0; i<decl->numExtendsNames(); i++)
                    extendsnames.push_back(decl->extendsName(i));
            else
                for (int i=0; i<decl->numInterfaceNames(); i++)
                    interfacenames.push_back(decl->interfaceName(i));
        }
        if (child->getTagCode()==NED_INTERFACE_NAME) {
            // resolve and store base type
            const char *interfacename = ((InterfaceNameElement *)child)->getName();
            std::string interfaceqname = getResolver()->resolveNedType(context, interfacename);
            Assert(!interfaceqname.empty());
            interfacenames.push_back(interfaceqname);

            // check the type (must be an interface)
            NEDTypeInfo *decl = getResolver()->lookup(interfaceqname.c_str());
            Assert(decl);
            if (decl->getType() != (getType()==CHANNEL ? CHANNELINTERFACE : MODULEINTERFACE))
                throw NEDException(getTree(), "base type %s is expected to be a %s interface", interfaceqname.c_str(), (getType()==CHANNEL ? "channel" : "module"));

            // we support all interfaces that our base interfaces extend
            for (int i=0; i<decl->numExtendsNames(); i++)
                interfacenames.push_back(decl->extendsName(i));
        }
    }

    if (!isInterface)
    {
        // check that we have all parameters and gates required by the interfaces we support
        for (int i=0; i<(int)interfacenames.size(); i++) {
            NEDTypeInfo *interfaceDecl = getResolver()->lookup(interfacenames[i].c_str());
            Assert(interfaceDecl);
            checkComplianceToInterface(interfaceDecl);
        }
    }

    // fill in enclosingTypeName for inner types
    if (isInner)
    {
        const char *lastDot = strrchr(qname, '.');
        Assert(lastDot); // if it's an inner type, must have a parent
        enclosingTypeName = std::string(qname, lastDot-qname);
    }

    // resolve C++ class name for modules/channels; for interfaces it remains empty
    if (getType()==SIMPLE_MODULE || getType()==COMPOUND_MODULE || getType()==CHANNEL)
    {
        // Note: @class() be used to override inherited implementation class.
        // The @class property itself does NOT get inherited.
        const char *explicitClassName = NEDElementUtil::getLocalStringProperty(getTree(), "class");
        if (!opp_isempty(explicitClassName))
        {
            if (explicitClassName[0]==':' && explicitClassName[1]==':')
                implClassName = explicitClassName+2;
            else
                implClassName = opp_join("::", getCxxNamespace().c_str(), explicitClassName); //note: if the name doesn't exist in the namespace, we could try the global namespace as C++ does, but doing so would have little to no (or even negative) benefit in practice
        }
        else
        {
            if (numExtendsNames()!=0)
                implClassName = opp_nulltoempty(getSuperDecl()->getImplementationClassName());
            else if (getType()==COMPOUND_MODULE)
                implClassName = OPP_PREFIX "cModule";
            else
                implClassName = opp_join("::", getCxxNamespace().c_str(), getName());
        }
    }

    // TODO check that parameter, gate, submodule and inner type declarations don't conflict with those in super types
}

NEDTypeInfo::~NEDTypeInfo()
{
    // note: we don't delete "tree", as it belongs to resolver
}

const char *NEDTypeInfo::getName() const
{
    // take substring after the last dot
    const char *qname = getFullName();
    const char *lastdot = strrchr(qname, '.');
    return !lastdot ? qname : lastdot + 1;
}

const char *NEDTypeInfo::getFullName() const
{
    return qualifiedName.c_str();
}

NEDElement *NEDTypeInfo::getTree() const
{
    return tree;
}

std::string NEDTypeInfo::getPackage() const
{
    NEDElement *nedfile = getTree()->getParentWithTag(NED_NED_FILE);
    PackageElement *packageDecl = nedfile ? (PackageElement *) nedfile->getFirstChildWithTag(NED_PACKAGE) : NULL;
    return packageDecl ? packageDecl->getName() : "";
}

std::string NEDTypeInfo::getPackageProperty(const char *propertyName) const
{
    // find first such property in the current file, then in package.ned of this package and parent packages
    for (NedFileElement *nedfile = (NedFileElement *)getTree()->getParentWithTag(NED_NED_FILE);
         nedfile != NULL;
         nedfile = getResolver()->getParentPackageNedFile(nedfile))
    {
        const char *propertyValue = NEDElementUtil::getLocalStringProperty(nedfile, propertyName);
        if (propertyValue)
            return propertyValue;
    }
    return "";
}

std::string NEDTypeInfo::getCxxNamespace() const
{
    return getPackageProperty("namespace");
}

std::string NEDTypeInfo::info() const
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

std::string NEDTypeInfo::nedSource() const
{
    std::stringstream out;
    NEDErrorStore errors;
    generateNED2(out, getTree(), &errors);
    return out.str();
}

const char *NEDTypeInfo::interfaceName(int k) const
{
    if (k<0 || k>=(int)interfacenames.size())
        throw NEDException("NEDTypeInfo: interface index %d out of range 0..%d", k, interfacenames.size()-1);
    return interfacenames[k].c_str();
}

bool NEDTypeInfo::supportsInterface(const char *qname)
{
    // linear search is OK because #interfaces is typically just one or two
    for (int i=0; i<(int)interfacenames.size(); i++)
        if (interfacenames[i] == qname)
            return true;
    return false;
}

const char *NEDTypeInfo::extendsName(int k) const
{
    if (k<0 || k>=(int)extendsnames.size())
        throw NEDException("NEDTypeInfo: extendsName(): index %d out of range 0..%d", k, extendsnames.size()-1);
    return extendsnames[k].c_str();
}

const char *NEDTypeInfo::getEnclosingTypeName() const
{
    return isInner ? enclosingTypeName.c_str() : NULL;
}

const char *NEDTypeInfo::getImplementationClassName() const
{
    return implClassName.empty() ? NULL : implClassName.c_str();
}

NEDTypeInfo *NEDTypeInfo::getSuperDecl() const
{
    const char *superName = extendsName(0);
    return getResolver()->getDecl(superName);
}

bool NEDTypeInfo::isNetwork() const
{
    return NEDElementUtil::getLocalBoolProperty(getTree(), "isNetwork");
}

ParametersElement *NEDTypeInfo::getParametersElement() const
{
    return (ParametersElement *)getTree()->getFirstChildWithTag(NED_PARAMETERS);
}

GatesElement *NEDTypeInfo::getGatesElement() const
{
    return (GatesElement *)getTree()->getFirstChildWithTag(NED_GATES);
}

SubmodulesElement *NEDTypeInfo::getSubmodulesElement() const
{
    return (SubmodulesElement *)getTree()->getFirstChildWithTag(NED_SUBMODULES);
}

ConnectionsElement *NEDTypeInfo::getConnectionsElement() const
{
    return (ConnectionsElement *)getTree()->getFirstChildWithTag(NED_CONNECTIONS);
}

SubmoduleElement *NEDTypeInfo::getLocalSubmoduleElement(const char *subcomponentName) const
{
    SubmodulesElement *submodulesNode = getSubmodulesElement();
    if (submodulesNode)
    {
        NEDElement *submoduleNode = submodulesNode->getFirstChildWithAttribute(NED_SUBMODULE, "name", subcomponentName);
        if (submoduleNode)
            return (SubmoduleElement *)submoduleNode;
    }
    return NULL;
}

ConnectionElement *NEDTypeInfo::getLocalConnectionElement(long id) const
{
    if (id == -1)
        return NULL;  // "not a NED connection"

    ConnectionsElement *connectionsNode = getConnectionsElement();
    if (connectionsNode)
    {
        for (NEDElement *child=connectionsNode->getFirstChild(); child; child=child->getNextSibling())
        {
            if (child->getTagCode() == NED_CONNECTION) {
                if (child->getId() == id)
                    return (ConnectionElement *)child;
            }
            else if (child->getTagCode() == NED_CONNECTION_GROUP) {
                NEDElement *conngroup = child;
                for (NEDElement *child=conngroup->getFirstChild(); child; child=child->getNextSibling())
                    if (child->getId() == id && child->getTagCode() == NED_CONNECTION)
                        return (ConnectionElement *)child;
            }
        }
    }
    return NULL;
}

SubmoduleElement *NEDTypeInfo::getSubmoduleElement(const char *name) const
{
    SubmoduleElement *submodule = getLocalSubmoduleElement(name);
    if (submodule)
        return submodule;
    for (int i=0; i<numExtendsNames(); i++)
        if ((submodule = getResolver()->lookup(extendsName(i))->getSubmoduleElement(name))!=NULL)
            return submodule;
    return NULL;
}

ConnectionElement *NEDTypeInfo::getConnectionElement(long id) const
{
    ConnectionElement *conn = getLocalConnectionElement(id);
    if (conn)
        return conn;
    for (int i=0; i<numExtendsNames(); i++)
        if ((conn = getResolver()->lookup(extendsName(i))->getConnectionElement(id))!=NULL)
            return conn;
    return NULL;
}

ParamElement *NEDTypeInfo::findLocalParamDecl(const char *name) const
{
    ParametersElement *params = getParametersElement();
    if (params)
        for (ParamElement *param=params->getFirstParamChild(); param; param=param->getNextParamSibling())
            if (param->getType()!=NED_PARTYPE_NONE && opp_strcmp(param->getName(), name)==0)
                return param;
    return NULL;
}

ParamElement *NEDTypeInfo::findParamDecl(const char *name) const
{
    ParamElement *param = findLocalParamDecl(name);
    if (param)
        return param;
    for (int i=0; i<numExtendsNames(); i++)
        if ((param = getResolver()->lookup(extendsName(i))->findParamDecl(name))!=NULL)
            return param;
    return NULL;
}

GateElement *NEDTypeInfo::findLocalGateDecl(const char *name) const
{
    GatesElement *gates = getGatesElement();
    if (gates)
        for (GateElement *gate=gates->getFirstGateChild(); gate; gate=gate->getNextGateSibling())
            if (gate->getType()!=NED_PARTYPE_NONE && opp_strcmp(gate->getName(), name)==0)
                return gate;
    return NULL;
}

GateElement *NEDTypeInfo::findGateDecl(const char *name) const
{
    GateElement *gate = findLocalGateDecl(name);
    if (gate)
        return gate;
    for (int i=0; i<numExtendsNames(); i++)
        if ((gate = getResolver()->lookup(extendsName(i))->findGateDecl(name))!=NULL)
            return gate;
    return NULL;
}

void NEDTypeInfo::checkComplianceToInterface(NEDTypeInfo *idecl)
{
    // TODO check properties

    // check parameters
    ParametersElement *iparams = idecl->getParametersElement();
    if (iparams)
    {
        for (ParamElement *iparam=iparams->getFirstParamChild(); iparam; iparam=iparam->getNextParamSibling())
        {
            // find param decl
            ParamElement *param = findParamDecl(iparam->getName());
            if (!param)
                throw NEDException(getTree(), "%s type has no parameter `%s', required by interface `%s'",
                                   (getType()==CHANNEL ? "channel" : "module"), iparam->getName(), idecl->getFullName());

            // check parameter type
            if (param->getType()!=iparam->getType())
                throw NEDException(param, "type of parameter `%s' must be %s, as required by interface `%s'",
                                   param->getName(), iparam->getAttribute("type"), idecl->getFullName());

            // check parameter volatile flag
            if (param->getIsVolatile() && !iparam->getIsVolatile())
                throw NEDException(param, "parameter `%s' must not be volatile, as required by interface `%s'",
                                   param->getName(), idecl->getFullName());
            if (!param->getIsVolatile() && iparam->getIsVolatile())
                throw NEDException(param, "parameter `%s' must be volatile, as required by interface `%s'",
                                   param->getName(), idecl->getFullName());

            // TODO check properties
        }
    }

    // check gates
    GatesElement *igates = idecl->getGatesElement();
    if (igates)
    {
        for (GateElement *igate=igates->getFirstGateChild(); igate; igate=igate->getNextGateSibling())
        {
            // find gate decl
            GateElement *gate = findGateDecl(igate->getName());
            if (!gate)
                throw NEDException(getTree(), "%s type has no gate `%s', required by interface `%s'",
                                   (getType()==CHANNEL ? "channel" : "module"), igate->getName(), idecl->getFullName());

            // check gate type
            if (gate->getType()!=igate->getType())
                throw NEDException(gate, "type of gate `%s' must be %s, as required by interface `%s'",
                                   gate->getName(), igate->getAttribute("type"), idecl->getFullName());

            // check vector/nonvector
            if (!igate->getIsVector() && gate->getIsVector())
                throw NEDException(gate, "gate `%s' must not be a vector gate, as required by interface `%s'",
                                   gate->getName(), idecl->getFullName());
            if (igate->getIsVector() && !gate->getIsVector())
                throw NEDException(gate, "gate `%s' must be a vector gate, as required by interface `%s'",
                                   gate->getName(), idecl->getFullName());

            // if both are vectors, check vector size specs are compatible
            if (igate->getIsVector() && gate->getIsVector())
            {
                ExpressionElement *gatesizeExpr = (ExpressionElement *)gate->getFirstChildWithAttribute(NED_EXPRESSION, "target", "vector-size");
                ExpressionElement *igatesizeExpr = (ExpressionElement *)igate->getFirstChildWithAttribute(NED_EXPRESSION, "target", "vector-size");

                bool hasGatesize = !opp_isempty(gate->getVectorSize()) || gatesizeExpr!=NULL;
                bool ihasGatesize = !opp_isempty(igate->getVectorSize()) || igatesizeExpr!=NULL;

                if (hasGatesize && !ihasGatesize)
                    throw NEDException(gate, "size of gate vector `%s' must be left unspecified, as required by interface `%s'",
                                       gate->getName(), idecl->getFullName());
                if (!hasGatesize && ihasGatesize)
                    throw NEDException(gate, "size of gate vector `%s' must be specified as in interface `%s'",
                                       gate->getName(), idecl->getFullName());

                // if both gatesizes are given, check that they are actually the same
                if (hasGatesize && ihasGatesize)
                {
                    bool mismatch = (gatesizeExpr && igatesizeExpr) ?
                        NEDElementUtil::compareTree(gatesizeExpr, igatesizeExpr)!=0 : // with parsed expressions
                        opp_strcmp(gate->getVectorSize(), igate->getVectorSize())!=0;  // with unparsed expressions
                    if (mismatch)
                        throw NEDException(gate, "size of gate vector `%s' must be specified as in interface `%s'",
                                           gate->getName(), idecl->getFullName());
                }

            }

            // TODO check properties
        }
    }
}

NAMESPACE_END

