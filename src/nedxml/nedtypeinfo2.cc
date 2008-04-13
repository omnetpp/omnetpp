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
#include "nedtypeinfo2.h"
#include "nedresourcecache.h"
#include "ned2generator.h"


USING_NAMESPACE

#define FIXME_CAST NEDTypeInfo2*

NEDTypeInfo2::NEDTypeInfo2(NEDResourceCache *resolver, const char *qname, NEDElement *tree)
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
        default: throw NEDException("NEDTypeInfo2: element of wrong type (<%s>) passed into constructor", tree->getTagName());
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
            NEDTypeInfo2 *decl = (FIXME_CAST) getResolver()->lookup(extendsqname.c_str());
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
            NEDTypeInfo2 *decl = (FIXME_CAST) getResolver()->lookup(interfaceqname.c_str());
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
            NEDTypeInfo2 *interfaceDecl = (FIXME_CAST) getResolver()->lookup(interfacenames[i].c_str());
            Assert(interfaceDecl);
            checkComplianceToInterface(interfaceDecl);
        }
    }

}

NEDTypeInfo2::~NEDTypeInfo2()
{
    // note: we don't delete "tree", as it belongs to resolver
}

const char *NEDTypeInfo2::name() const
{
    // take substring after the last dot
    const char *qname = fullName();
    const char *lastdot = strrchr(qname, '.');
    return !lastdot ? qname : lastdot + 1;
}

const char *NEDTypeInfo2::fullName() const
{
    return qualifiedName.c_str();
}

std::string NEDTypeInfo2::info() const
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

NEDElement *NEDTypeInfo2::getTree() const
{
    return tree;
}

std::string NEDTypeInfo2::nedSource() const
{
    std::stringstream out;
    NEDErrorStore errors;
    generateNED2(out, getTree(), &errors);
    return out.str();
}

bool NEDTypeInfo2::isNetwork()
{
    return NEDElementUtil::getLocalBoolProperty(getTree(), "isNetwork");
}

std::string NEDTypeInfo2::getPackage() const
{
    NEDElement *nedfile = getTree()->getParentWithTag(NED_NED_FILE);
    PackageElement *packageDecl = nedfile ? (PackageElement *) nedfile->getFirstChildWithTag(NED_PACKAGE) : NULL;
    return packageDecl ? packageDecl->getName() : "";
}

std::string NEDTypeInfo2::getCxxNamespace() const
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

const char *NEDTypeInfo2::implementationClassName() const
{
    return implClassName.empty() ? NULL : implClassName.c_str();
}

NEDTypeInfo2 *NEDTypeInfo2::getSuperDecl() const
{
    const char *superName = extendsName(0);
    return (FIXME_CAST) getResolver()->getDecl(superName);
}

/*
NEDElement *NEDTypeInfo2::getConnectionElement(long id) const
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

void NEDTypeInfo2::checkComplianceToInterface(NEDTypeInfo2 *idecl)
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

void NEDTypeInfo2::mergeProperties(NEDElement *basetree, const NEDElement *tree) const
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

void NEDTypeInfo2::mergeProperty(PropertyElement *baseprop, const PropertyElement *prop) const
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

void NEDTypeInfo2::mergePropertyKey(PropertyKeyElement *basekey, const PropertyKeyElement *key) const
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
*/

void NEDTypeInfo2::collect(NEDElementMap& specificMap, int sectionTagCode, bool (*predicate)(NEDElement*))
{
    NEDElement *section = tree->getFirstChildWithTag(sectionTagCode);
    if (section)
    {
        for (NEDElement *node=section->getFirstChild(); node; node=node->getNextSibling())
        {
            if ((*predicate)(node))
            {
                const char *name = node->getAttribute("name");
                if (localMembersMap.contains(name))
                    throw NEDException(node, "name `%s' must be unique within this type", name);
                Assert(!specificMap.contains(name));
                localMembersMap.put(name, node);
                specificMap.put(name, node);
            }
        }
    }
}

inline containsExpression(NEDElement *node) {
    return node->getFirstChildWithTag(NED_EXPRESSION)!=NULL;
}

static bool isParamDecl(NEDElement *node) {
    return node->getTagCode()==NED_PARAM && ((ParamElement*)node)->getType()!=NED_PARTYPE_NONE;
}
static bool isParamValue(NEDElement *node) {
    return node->getTagCode()==NED_PARAM && (containsExpression(node) || !opp_isempty(((ParamElement*)node)->getValue()));
}
static bool isGateDecl(NEDElement *node) {
    return node->getTagCode()==NED_GATE && ((GateElement*)node)->getType()!=NED_GATETYPE_NONE;
}
static bool isGateSize(NEDElement *node) {
    return node->getTagCode()==NED_GATE && (containsExpression(node) || !opp_isempty(((GateElement*)node)->getVectorSize()));
}
static bool isInnerType(NEDElement *node) {
    return NEDElementUtil::isNEDType(node);
}
static bool isSubmodule(NEDElement *node) {
    return node->getTagCode()==NED_SUBMODULE;
}

void NEDTypeInfo2::collectLocalMembers()
{
    //TODO collect inheritance too
    collect(localParamDeclsMap, NED_PARAMETERS, isParamDecl);
    collect(localParamValuesMap, NED_PARAMETERS, isParamValue);
    collect(localGateDeclsMap, NED_GATES, isGateDecl);
    collect(localGateSizesMap, NED_GATES, isGateSize);
    collect(localInnerTypesMap, NED_TYPES, isInnerType);
    collect(localSubmodulesMap, NED_SUBMODULES, isSubmodule);
}

void NEDTypeInfo2::addDeclarations(NEDElementMap& allSomethingMap, const NEDElementMap& localSomethingMap, const char *what)
{
    for (int i=0; i<localSomethingMap.size(); i++)
    {
        const std::string& name = localSomethingMap.key(i);
        NEDElement *node = localSomethingMap.value(i);
        if (allMembersMap.contains(localSomethingMap.key(i))) {
            if (allSomethingMap.contains(name))
                throw NEDException(node, "redeclaration of %s `%s'", what, name); // FIXME revise msg: this also comes if two interfaces collide
            else
                throw NEDException(node, "name `%s' already used as a %s at %s", name, "foo", "FIXME:0"); //FIXME
        }
        allSomethingMap.put(name, node);
        allMembersMap.put(name, node);
    }
}

void NEDTypeInfo2::collectAllMembers()
{
    // collect stuff from base classes
    const NEDElementMap& extendsNames = getLocalExtendsNames();
    for (int i=0; i<extendsNamesMap.size(); i++)
    {
        const char *extendsQName = extendsNames.key(i);
        NEDTypeInfo2 *superDecl = (FIXME_CAST) getResolver()->lookup(extendsQName.c_str());

        addDeclarations(allParamDeclsMap, superDecl->getParamDeclarations(), "parameter");
        //TODO paramValues
        addDeclarations(allGateDeclsMap, superDecl->getGateDeclarations(), "gate");
        //TODO gateSizes
        addDeclarations(allInnerTypesMap, superDecl->getInnerTypes(), "inner type");
        addDeclarations(allSubmodulesMap, superDecl->getSubmodules(), "submodule");

    }

    // add local members
    collectLocalMembers();

    addDeclarations(allParamDeclsMap, localParamDeclsMap, "parameter");
    //TODO paramValues
    addDeclarations(allGateDeclsMap, localGateDeclsMap, "gate");
    //TODO gateSizes
    addDeclarations(allInnerTypesMap, localInnerTypesMap, "inner type");
    addDeclarations(allSubmodulesMap, localSubmodulesMap, "submodule");
}



