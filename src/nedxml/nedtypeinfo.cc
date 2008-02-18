//==========================================================================
// NEDTYPEINFO.CC -
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

USING_NAMESPACE


NEDTypeInfo::NEDTypeInfo(NEDResourceCache *resolver, const char *qname, NEDElement *tree)
{
    this->resolver = resolver;
    this->qualifiedName = qname;
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
        for (int i=0; i<interfacenames.size(); i++) {
            NEDTypeInfo *interfaceDecl = getResolver()->lookup(interfacenames[i].c_str());
            Assert(interfaceDecl);
            checkComplianceToInterface(interfaceDecl);
        }
    }

    // resolve C++ class name
    if (getType()==SIMPLE_MODULE || getType()==CHANNEL)
    {
        // Note: @class() be used to override inherited implementation class.
        // The @class property itself does NOT get inherited.
        const char *explicitClassName = NEDElementUtil::getLocalStringProperty(getTree(), "class");
        if (!opp_isempty(explicitClassName))
            implClassName = opp_join("::", getCxxNamespace().c_str(), explicitClassName);
        else if (numExtendsNames()!=0)
            implClassName = opp_nulltoempty(getSuperDecl()->implementationClassName());
        else
            implClassName = opp_join("::", getCxxNamespace().c_str(), name());
    }
}

NEDTypeInfo::~NEDTypeInfo()
{
    // nothing -- we don't manage the tree, only cache a pointer to it
}

const char *NEDTypeInfo::name() const
{
    // take substring after the last dot
    const char *qname = fullName();
    const char *lastdot = strrchr(qname, '.');
    return !lastdot ? qname : lastdot + 1;
}

const char *NEDTypeInfo::fullName() const
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

std::string NEDTypeInfo::getCxxNamespace() const
{
    // find first @namespace() property in package.ned of this package and parent packages
    std::string packageName = getPackage();
    const char *namespaceName = NULL;
    while (true) {
        // check the package.ned file for this property
        NEDElement *nedfile = getResolver()->getPackageNedFile(packageName.c_str());
        namespaceName = nedfile ? NEDElementUtil::getLocalStringProperty(nedfile, "namespace") : NULL;
        if (namespaceName)
            break;

        if (packageName.empty())
            break;

        // go one package up -- drop part after last dot
        size_t k = packageName.rfind(".", packageName.length());
        packageName.resize(k==std::string::npos ? 0 : k);
    }

    return opp_nulltoempty(namespaceName);
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
        throw NEDException("cNEDDeclaration: interface index %d out of range 0..%d", k, interfacenames.size()-1);
    return interfacenames[k].c_str();
}

bool NEDTypeInfo::supportsInterface(const char *qname)
{
    // linear search is OK because #interfaces is typically just one or two
    for (int i=0; i<interfacenames.size(); i++)
        if (interfacenames[i] == qname)
            return true;
    return false;
}

const char *NEDTypeInfo::extendsName(int k) const
{
    if (k<0 || k>=(int)extendsnames.size())
        throw NEDException("cNEDDeclaration: extendsName(): index %d out of range 0..%d", k, extendsnames.size()-1);
    return extendsnames[k].c_str();
}

const char *NEDTypeInfo::implementationClassName() const
{
    return implClassName.empty() ? NULL : implClassName.c_str();
}

NEDTypeInfo *NEDTypeInfo::getSuperDecl() const
{
    const char *superName = extendsName(0);
    return getResolver()->getDecl(superName);
}

bool NEDTypeInfo::isNetwork()
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

NEDElement *NEDTypeInfo::getSubcomponentElement(const char *subcomponentName) const
{
    // try as submodule
    SubmodulesElement *submodulesNode = getSubmodulesElement();
    if (submodulesNode)
    {
        NEDElement *submoduleNode = submodulesNode->getFirstChildWithAttribute(NED_SUBMODULE, "name", subcomponentName);
        if (submoduleNode)
            return submoduleNode;
    }

    // try as connection
    ConnectionsElement *connectionsNode = getConnectionsElement();
    if (connectionsNode)
    {
        //FIXME try to find a channel named subcomponentName here...
    }
    return NULL;
}

void NEDTypeInfo::checkComplianceToInterface(NEDTypeInfo *ifDecl)
{
/*FIXME TODO
    // check properties
    //XXX

    // check parameters
    for (int i=0; i<ifDecl->numPars(); i++)
    {
        int k = findPar(ifDecl->parName(i));
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
*/
}

/*
void NEDTypeInfo::assertParMatchesBase(cPar *par, const cPar *basepar,
                                                const char *parname, const char *basename)
{
    if (basepar->isSet() && par->isSet())
        throw cRuntimeError(this, "parameter `%s' already set in base `%s', it cannot be overridden", parname, basename);

    if (par->type()!=basepar->type() || par->isVolatile()!=ifpar->isVolatile())
        throw cRuntimeError(this, "type of parameter `%s' differs from that in base `%s'", parname, basename);
}

bool NEDTypeInfo::parMatchesInterface(cPar *par, const cPar *ifpar)
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


