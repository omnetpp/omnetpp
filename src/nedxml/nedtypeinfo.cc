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

USING_NAMESPACE


NEDTypeInfo::NEDTypeInfo(NEDResourceCache *resolver, const char *qname, NEDElement *tree)
{
    this->resolver = resolver;
    this->qualifiedName = qname;
    this->tree = tree;
    this->flattenedTree = NULL;

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

    // this is a temporary way to get some validation...
    getFlattenedTree();
}

NEDTypeInfo::~NEDTypeInfo()
{
    // note: we don't delete "tree", as it belongs to resolver
    if (flattenedTree && flattenedTree!=tree)
        delete flattenedTree;
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

NEDElement *NEDTypeInfo::getFlattenedTree() const
{
    if (!flattenedTree)
        flattenedTree = buildFlattenedTree();
    return flattenedTree;
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
    for (int i=0; i<(int)interfacenames.size(); i++)
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

NEDElement *NEDTypeInfo::getSubmoduleElement(const char *subcomponentName) const
{
    SubmodulesElement *submodulesNode = getSubmodulesElement();
    if (submodulesNode)
    {
        NEDElement *submoduleNode = submodulesNode->getFirstChildWithAttribute(NED_SUBMODULE, "name", subcomponentName);
        if (submoduleNode)
            return submoduleNode;
    }
    return NULL;
}

NEDElement *NEDTypeInfo::getConnectionElement(long id) const
{
    ConnectionsElement *connectionsNode = getConnectionsElement();
    if (connectionsNode)
    {
        for (NEDElement *child=connectionsNode->getFirstChild(); child; child=child->getNextSibling())
        {
            if (child->getTagCode() == NED_CONNECTION) {
                if (child->getId() == id)
                    return child;
            }
            else if (child->getTagCode() == NED_CONNECTION_GROUP) {
                NEDElement *conngroup = child;
                for (NEDElement *child=conngroup->getFirstChild(); child; child=child->getNextSibling())
                    if (child->getId() == id && child->getTagCode() == NED_CONNECTION)
                        return child;
            }
        }
    }
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
        if ((param = getResolver()->lookup(extendsName(i))->findLocalParamDecl(name))!=NULL)
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
        if ((gate = getResolver()->lookup(extendsName(i))->findLocalGateDecl(name))!=NULL)
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
                                   (getType()==CHANNEL ? "channel" : "module"), iparam->getName(), idecl->fullName());

            // check parameter type
            if (param->getType()!=iparam->getType())
                throw NEDException(param, "type of parameter `%s' must be %s, as required by interface `%s'",
                                   param->getName(), iparam->getAttribute("type"), idecl->fullName());

            // check parameter volatile flag
            if (param->getIsVolatile() && !iparam->getIsVolatile())
                throw NEDException(param, "parameter `%s' must not be volatile, as required by interface `%s'",
                                   param->getName(), idecl->fullName());
            if (!param->getIsVolatile() && iparam->getIsVolatile())
                throw NEDException(param, "parameter `%s' must be volatile, as required by interface `%s'",
                                   param->getName(), idecl->fullName());

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
                                   (getType()==CHANNEL ? "channel" : "module"), igate->getName(), idecl->fullName());

            // check gate type
            if (gate->getType()!=igate->getType())
                throw NEDException(gate, "type of gate `%s' must be %s, as required by interface `%s'",
                                   gate->getName(), igate->getAttribute("type"), idecl->fullName());

            // check vector/nonvector
            if (!igate->getIsVector() && gate->getIsVector())
                throw NEDException(gate, "gate `%s' must not be a vector gate, as required by interface `%s'",
                                   gate->getName(), idecl->fullName());
            if (igate->getIsVector() && !gate->getIsVector())
                throw NEDException(gate, "gate `%s' must be a vector gate, as required by interface `%s'",
                                   gate->getName(), idecl->fullName());

            // if both are vectors, check vector size specs are compatible
            if (igate->getIsVector() && gate->getIsVector())
            {
                ExpressionElement *gatesizeExpr = (ExpressionElement *)gate->getFirstChildWithAttribute(NED_EXPRESSION, "target", "vector-size");
                ExpressionElement *igatesizeExpr = (ExpressionElement *)igate->getFirstChildWithAttribute(NED_EXPRESSION, "target", "vector-size");

                bool hasGatesize = !opp_isempty(gate->getVectorSize()) || gatesizeExpr!=NULL;
                bool ihasGatesize = !opp_isempty(igate->getVectorSize()) || igatesizeExpr!=NULL;

                if (hasGatesize && !ihasGatesize)
                    throw NEDException(gate, "size of gate vector `%s' must be left unspecified, as required by interface `%s'",
                                       gate->getName(), idecl->fullName());
                if (!hasGatesize && ihasGatesize)
                    throw NEDException(gate, "size of gate vector `%s' must be specified as in interface `%s'",
                                       gate->getName(), idecl->fullName());

                // if both gatesizes are given, check that they are actually the same
                if (hasGatesize && ihasGatesize)
                {
                    bool mismatch = (gatesizeExpr && igatesizeExpr) ?
                        NEDElementUtil::compareTree(gatesizeExpr, igatesizeExpr)!=0 : // with parsed expressions
                        opp_strcmp(gate->getVectorSize(), igate->getVectorSize())!=0;  // with unparsed expressions
                    if (mismatch)
                        throw NEDException(gate, "size of gate vector `%s' must be specified as in interface `%s'",
                                           gate->getName(), idecl->fullName());
                }

            }

            // TODO check properties
        }
    }
}

//XXX note: we currently don't use this for anything
NEDElement *NEDTypeInfo::buildFlattenedTree() const
{
    NEDElement *result = NULL;
    NEDLookupContext context = NEDResourceCache::getParentContextOf(qualifiedName.c_str(), tree);
    for (NEDElement *child=tree->getFirstChildWithTag(NED_EXTENDS); child; child=child->getNextSiblingWithTag(NED_EXTENDS))
    {
        // resolve and store base type name
        const char *extendsname = ((ExtendsElement *)child)->getName();
        std::string extendsqname = getResolver()->resolveNedType(context, extendsname);
        Assert(!extendsqname.empty());
        //XXX extendsnames probably not needed, ever:
        //extendsnames.push_back(extendsqname);

        // check the type
        NEDTypeInfo *decl = getResolver()->lookup(extendsqname.c_str());
        Assert(decl);
        if (getType() != decl->getType())  //XXX currently duplicate check
            throw NEDException(getTree(), "a %s cannot extend a %s (%s)", getTree()->getTagName(), decl->getTree()->getTagName(), extendsqname.c_str());

        NEDElement *basetree = decl->getFlattenedTree();
        if (!result)
            result = basetree->dupTree();  //FIXME: does not copy srcloc, srcregion!
        else
            mergeNEDType(result, basetree);
    }
    if (!result)
        result = tree;
    else
        mergeNEDType(result, tree);
    return result;
}

inline bool isNEDType(NEDElement *node)
{
    int tag = node->getTagCode();
    return tag==NED_SIMPLE_MODULE || tag==NED_MODULE_INTERFACE || tag==NED_COMPOUND_MODULE || tag==NED_CHANNEL_INTERFACE || tag==NED_CHANNEL;
}

void NEDTypeInfo::mergeNEDType(NEDElement *basetree, const NEDElement *tree) const
{
    // TODO remove all "extends" causes (not needed)
    for (ExtendsElement *i=(ExtendsElement *)basetree->getFirstChildWithTag(NED_EXTENDS); i; i=(ExtendsElement *)i->getNextSiblingWithTag(NED_EXTENDS))
        delete basetree->removeChild(i);

    // merge interfaces ("like" clauses)
    for (InterfaceNameElement *i=(InterfaceNameElement *)tree->getFirstChildWithTag(NED_INTERFACE_NAME); i; i=(InterfaceNameElement *)i->getNextSiblingWithTag(NED_INTERFACE_NAME))
    {
        basetree->appendChild(i->dupTree());  //TODO convert to fully qualified name
    }

    // merge parameters
    ParametersElement *params = (ParametersElement *)tree->getFirstChildWithTag(NED_PARAMETERS);
    ParametersElement *baseparams = (ParametersElement *)basetree->getFirstChildWithTag(NED_PARAMETERS);
    if (params && !baseparams)
    {
        // just copy it
        basetree->appendChild(params->dupTree());
    }
    else if (params && baseparams)
    {
        // merge parameters
        for (ParamElement *param=params->getFirstParamChild(); param; param=param->getNextParamSibling())
        {
            // find existing param in base
            ParamElement *baseparam = (ParamElement *)baseparams->getFirstChildWithAttribute(NED_PARAM, "name", param->getName());
            if (!baseparam)
            {
                // just copy it
                baseparams->appendChild(param->dupTree());
            }
            else
            {
                // already exists -- must merge
                if (param->getType()!=NED_PARTYPE_NONE)
                    throw NEDException(param, "redeclaration of parameter `%s'", param->getName()); //XXX "declared at XXX"

                // take over value if exists
                if (!opp_isempty(param->getValue()))
                {
                    baseparam->setValue(param->getValue());
                    baseparam->setIsDefault(param->getIsDefault());
                }
                ExpressionElement *valueexpr = param->getFirstExpressionChild();
                if (valueexpr)
                {
                    delete baseparam->getFirstExpressionChild();
                    baseparam->appendChild(valueexpr->dupTree());
                    baseparam->setIsDefault(param->getIsDefault());
                }

                // merge properties
                mergeProperties(baseparam, param);
            }
        }

        //TODO merge PatternElements

        // merge properties
        mergeProperties(baseparams, params);
    }

    // merge gates
    GatesElement *gates = (GatesElement *)tree->getFirstChildWithTag(NED_GATES);
    GatesElement *basegates = (GatesElement *)basetree->getFirstChildWithTag(NED_GATES);
    if (gates && !basegates)
    {
        // just copy it
        basetree->appendChild(gates->dupTree());
    }
    else if (gates && basegates)
    {
        // merge
        for (GateElement *gate=gates->getFirstGateChild(); gate; gate=gate->getNextGateSibling())
        {
            // find existing gate in base
            GateElement *basegate = (GateElement *)basegates->getFirstChildWithAttribute(NED_GATE, "name", gate->getName());
            if (!basegate)
            {
                // just copy it
                basegates->appendChild(gate->dupTree());
            }
            else
            {
                // already exists -- must merge
                if (gate->getType()!=NED_GATETYPE_NONE)
                    throw NEDException(gate, "redeclaration of gate `%s'", gate->getName()); //XXX "declared at XXX"

                // take over gatesize if exists
                if (!opp_isempty(gate->getVectorSize()))
                {
                    basegate->setVectorSize(gate->getVectorSize());
                }
                ExpressionElement *gatesizeexpr = gate->getFirstExpressionChild();
                if (gatesizeexpr)
                {
                    delete basegate->getFirstExpressionChild();
                    basegate->appendChild(gatesizeexpr->dupTree());
                }

                //  merge properties
                mergeProperties(basegate, gate);
            }
        }
    }

    // merge inner types
    TypesElement *types = (TypesElement *)tree->getFirstChildWithTag(NED_TYPES);
    TypesElement *basetypes = (TypesElement *)basetree->getFirstChildWithTag(NED_TYPES);
    if (types && !basetypes)
    {
        // just copy it
        basetree->appendChild(types->dupTree());
    }
    else if (types && basetypes)
    {
        // merge
        for (NEDElement *type=types->getFirstChild(); type; type=type->getNextSibling())
        {
            if (isNEDType(type))
            {
                // find same type in base
                const char *name = type->getAttribute("name");
                NEDElement *basetype;
                for (basetype=basetypes->getFirstChild(); basetype; basetype=basetype->getNextSibling())
                    if (isNEDType(basetype) && opp_strcmp(name, basetype->getAttribute("name"))==0)
                        break;

                if (!basetype)
                {
                    // no such name yet, just copy it
                    basetypes->appendChild(type->dupTree());
                }
                else
                {
                    // already exists ==> error (types don't get merged)
                    throw NEDException(type, "redeclaration of inner type `%s'", name); //XXX "declared at XXX"
                }
            }
        }
    }

    // merge submodules
    SubmodulesElement *submodules = (SubmodulesElement *)tree->getFirstChildWithTag(NED_SUBMODULES);
    SubmodulesElement *basesubmodules = (SubmodulesElement *)basetree->getFirstChildWithTag(NED_SUBMODULES);
    if (submodules)
    {
        if (!basesubmodules)
            basetree->appendChild(basesubmodules=submodules->dup());

        for (SubmoduleElement *submodule=submodules->getFirstSubmoduleChild(); submodule; submodule=submodule->getNextSubmoduleSibling())
        {
            // find existing submodule in base
            SubmoduleElement *basesubmodule = (SubmoduleElement *)basesubmodules->getFirstChildWithAttribute(NED_SUBMODULE, "name", submodule->getName());
            if (!basesubmodule)
            {
                // just copy it
                // TODO change typename to fully qualified name!!!
                basesubmodules->appendChild(submodule->dupTree());
            }
            else
            {
                // already exists ==> error (submodules don't get merged)
                throw NEDException(submodule, "redeclaration of submodule `%s'", submodule->getName()); //XXX "declared at XXX"
            }
        }
    }

    // merge connections
    ConnectionsElement *connections = (ConnectionsElement *)tree->getFirstChildWithTag(NED_CONNECTIONS);
    ConnectionsElement *baseconnections = (ConnectionsElement *)basetree->getFirstChildWithTag(NED_CONNECTIONS);
    if (connections && !baseconnections)
    {
        // just copy it
        basetree->appendChild(connections->dupTree());
    }
    else if (connections && baseconnections)
    {
        // merge "allowunconnected" bit
        if (connections->getAllowUnconnected())
            baseconnections->setAllowUnconnected(true);

        // just copy existing connections
        for (NEDElement *connection=connections->getFirstChild(); connection; connection=connection->getNextSibling())
        {
            baseconnections->appendChild(connection->dupTree());
        }
    }

    // TODO put "parameters:", "gates:", "types:" etc elements in the correct order in the tree
}

void NEDTypeInfo::mergeProperties(NEDElement *basetree, const NEDElement *tree) const
{
    for (NEDElement *child=tree->getFirstChildWithTag(NED_PROPERTY); child; child=child->getNextSiblingWithTag(NED_PROPERTY))
    {
        PropertyElement *prop = (PropertyElement *)child;

        // find corresponding property in basetree, by name+index
        PropertyElement *baseprop = NULL;
        for (NEDElement *basechild=basetree->getFirstChildWithTag(NED_PROPERTY); basechild; basechild=basechild->getNextSiblingWithTag(NED_PROPERTY))
            if (opp_strcmp(prop->getName(), ((PropertyElement *)basechild)->getName())==0 &&
                opp_strcmp(prop->getIndex(), ((PropertyElement *)basechild)->getIndex())==0)
                {baseprop = (PropertyElement *)basechild; break;}

        // if not there, add it, else merge.
        if (!baseprop)
            basetree->appendChild(prop->dupTree());
        else
            mergeProperty(baseprop, prop);   //FIXME add special case for @display
    }
}

void NEDTypeInfo::mergeProperty(PropertyElement *baseprop, const PropertyElement *prop) const
{
    for (PropertyKeyElement *key=prop->getFirstPropertyKeyChild(); key; key=key->getNextPropertyKeySibling())
    {
        PropertyKeyElement *basekey = (PropertyKeyElement *)baseprop->getFirstChildWithAttribute(NED_PROPERTY_KEY, "name", key->getName());
        if (!basekey)
        {
            // just copy it
            baseprop->appendChild(key->dupTree());
        }
        else
        {
            // merge property
            mergePropertyKey(basekey, key);
        }
    }
}

void NEDTypeInfo::mergePropertyKey(PropertyKeyElement *basekey, const PropertyKeyElement *key) const
{
    // merge positional ones
    LiteralElement *baseliteral = basekey->getFirstLiteralChild();
    LiteralElement *literal = key->getFirstLiteralChild();
    while (baseliteral && literal)
    {
        LiteralElement *nextbaseliteral = baseliteral->getNextLiteralSibling();
        if (literal->getType()==NED_CONST_SPEC && opp_strcmp(literal->getValue(),"-")==0)
        {
            // antivalue
            LiteralElement *blank = new LiteralElement();
            blank->setType(NED_CONST_SPEC);
            basekey->insertChildBefore(baseliteral, blank);
            delete basekey->removeChild(baseliteral);
        }
        else
        {
            // replace
            basekey->insertChildBefore(baseliteral, literal->dupTree());
            delete basekey->removeChild(baseliteral);
        }
        baseliteral = nextbaseliteral;
        literal = literal->getNextLiteralSibling();
    }

    // copy the rest
    while (literal)
    {
        basekey->appendChild(literal->dupTree());
        literal = literal->getNextLiteralSibling();
    }
}

