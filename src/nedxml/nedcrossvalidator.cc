//==========================================================================
// nedcrossvalidator.cc - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NEDCrossValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "stringutil.h"
#include "nederror.h"
#include "nedexception.h"
#include "nedresourcecache.h"
#include "nedcrossvalidator.h"

USING_NAMESPACE


NEDCrossValidator::NEDCrossValidator(bool parsedExpr, NEDResourceCache *res, NEDErrorStore *e)
   : NEDValidatorBase(e)
{
    parsedExpressions = parsedExpr;
    resolver = res;
}

NEDCrossValidator::~NEDCrossValidator()
{
}

NEDElement *NEDCrossValidator::getXXXDeclaration(const char *name, int tagcode1, int tagcode2)
{
    NEDTypeInfo *component = resolver->lookup(name);
    if (!component)
        return NULL;

    int type = component->getTree()->getTagCode();
    if (type!=tagcode1 && type!=tagcode2)
        return NULL;
    return component->getTree();
}

NEDElement *NEDCrossValidator::getModuleDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_SIMPLE_MODULE, NED_COMPOUND_MODULE);
}

NEDElement *NEDCrossValidator::getChannelDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_CHANNEL);
}

NEDElement *NEDCrossValidator::getModuleInterfaceDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_MODULE_INTERFACE);
}

NEDElement *NEDCrossValidator::getChannelInterfaceDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_CHANNEL_INTERFACE);
}

NEDElement *NEDCrossValidator::getEnumDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_ENUM);
}

NEDElement *NEDCrossValidator::getClassDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_CLASS);
}

void NEDCrossValidator::validateElement(FilesElement *node)
{
}

void NEDCrossValidator::validateElement(NedFileElement *node)
{
}

void NEDCrossValidator::validateElement(CommentElement *node)
{
}

void NEDCrossValidator::validateElement(ImportElement *node)
{
}

void NEDCrossValidator::validateElement(PropertyDeclElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(ExtendsElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(InterfaceNameElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(SimpleModuleElement *node)
{
    // FIXME revise
    // make sure module type name does not exist yet
    if (getModuleDeclaration(node->getName()))
        errors->addError(node, "redefinition of module with name '%s'",node->getName());
}

void NEDCrossValidator::validateElement(ModuleInterfaceElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(CompoundModuleElement *node)
{
    // FIXME revise
    // make sure module type name does not exist yet
    if (getModuleDeclaration(node->getName()))
        errors->addError(node, "redefinition of module with name '%s'",node->getName());
}

void NEDCrossValidator::validateElement(ParametersElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(ParamElement *node)
{
    // FIXME code comes from substparamnode -- REVISE
    if (!moduletypedecl)
        return;

    // make sure parameter exists in module type
    const char *paramName = node->getName();
    NEDElement *params = moduletypedecl->getFirstChildWithTag(NED_PARAMETERS);
    if (!params || params->getFirstChildWithAttribute(NED_PARAM, "name", paramName)==NULL)
        {errors->addError(node, "module type '%s' has no parameter named '%s'", moduletypedecl->getAttribute("name"), paramName);return;}

    // TBD compile-time check for type mismatch
}

void NEDCrossValidator::validateElement(PatternElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(PropertyElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(PropertyKeyElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(GatesElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(GateElement *node)
{
    // FIXME the following lines come from gatesizenode -- REVISE!
    if (!moduletypedecl)
        return;

    // make sure gate exists in module type
    const char *gatename = node->getName();
    GatesElement *gatesdecl = (GatesElement *)moduletypedecl->getFirstChildWithTag(NED_GATES);
    if (!gatesdecl)
    {
        errors->addError(node, "module type does not have gates");
        return;
    }
    GateElement *gatedecl = (GateElement *)gatesdecl->getFirstChildWithAttribute(NED_GATE, "name", gatename);
    if (!gatedecl)
    {
        errors->addError(node, "module type does not have a gate named '%s'",gatename);
        return;
    }

    // check it is vector
//    if (!gatedecl->getIsVector())
//        errors->addError(node, "gate '%s' is not a vector gate",gatename);
}

void NEDCrossValidator::validateElement(TypesElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(SubmodulesElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(SubmoduleElement *node)
{
    // FIXME revise
    // make sure module type exists
    const char *type_name = node->getType();
    moduletypedecl = getModuleDeclaration(type_name);
    if (!moduletypedecl)
        errors->addError(node, "unknown module type '%s'",type_name);
}

void NEDCrossValidator::validateElement(ConnectionsElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::checkGate(GateElement *gate, bool hasGateIndex, bool isInput, NEDElement *conn, bool isSrc)
{
    // FIXME revise
    // check gate direction, check if vector
    const char *q = isSrc ? "wrong source gate for connection" : "wrong destination gate for connection";
    if (hasGateIndex && !gate->getIsVector())
        errors->addError(conn, "%s: extra gate index or '++' ('%s' is not a vector gate)", q, gate->getName());
    else if (!hasGateIndex && gate->getIsVector())
        errors->addError(conn, "%s: missing gate index ('%s' is a vector gate)", q, gate->getName());

    // check gate direction, check if vector
    if (isInput && gate->getType()==NED_GATETYPE_OUTPUT)
        errors->addError(conn, "%s: input gate expected but '%s' is an output gate", q, gate->getName());
    else if (!isInput && gate->getType()==NED_GATETYPE_INPUT)
        errors->addError(conn, "%s: output gate expected but '%s' is an input gate", q, gate->getName());
}

void NEDCrossValidator::validateConnGate(const char *submodName, bool hasSubmodIndex,
                                            const char *gateName, bool hasGateIndex,
                                            NEDElement *parent, NEDElement *conn, bool isSrc)
{
    // FIXME revise
    const char *q = isSrc ? "wrong source gate for connection" : "wrong destination gate for connection";
    if (opp_isempty(submodName))
    {
        // connected to parent module: check such gate is declared
        NEDElement *gates = parent->getFirstChildWithTag(NED_GATES);
        GateElement *gate;
        if (!gates || (gate=(GateElement*)gates->getFirstChildWithAttribute(NED_GATE, "name", gateName))==NULL)
            errors->addError(conn, "%s: compound module has no gate named '%s'", q, gateName);
        else
            checkGate(gate, hasGateIndex, isSrc, conn, isSrc);
    }
    else
    {
        // check such submodule is declared
        NEDElement *submods = parent->getFirstChildWithTag(NED_SUBMODULES);
        SubmoduleElement *submod = NULL;
        if (!submods || (submod=(SubmoduleElement*)submods->getFirstChildWithAttribute(NED_SUBMODULE, "name", submodName))==NULL)
        {
            errors->addError(conn, "%s: compound module has no submodule named '%s'", q, submodName);
        }
        else
        {
            bool isSubmodVector = submod->getFirstChildWithAttribute(NED_EXPRESSION, "target", "vector-size")!=NULL;
            if (hasSubmodIndex && !isSubmodVector)
                errors->addError(conn, "%s: extra submodule index ('%s' is not a vector submodule)", q, submodName);
            else if (!hasSubmodIndex && isSubmodVector)
                errors->addError(conn, "%s: missing submodule index ('%s' is a vector submodule)", q, submodName);

            // check gate
            NEDElement *submodType = getModuleDeclaration(submod->getType());
            if (!submodType)
                return; // we gave error earlier if submod type is not present
            NEDElement *gates = submodType->getFirstChildWithTag(NED_GATES);
            GateElement *gate;
            if (!gates || (gate=(GateElement*)gates->getFirstChildWithAttribute(NED_GATE, "name", gateName))==NULL)
                errors->addError(conn, "%s: submodule '%s' has no gate named '%s'", q, submodName, gateName);
            else
                checkGate(gate, hasGateIndex, !isSrc, conn, isSrc);
        }
    }
}

void NEDCrossValidator::validateElement(ConnectionElement *node)
{
    // FIXME revise
    // make sure submodule and gate names are valid, gate direction is OK
    // and that gates & modules are really vector (or really not)
    NEDElement *compound = node->getParentWithTag(NED_COMPOUND_MODULE);
    if (!compound)
        INTERNAL_ERROR0(node,"occurs outside a compound-module");

    bool srcModIx =   node->getFirstChildWithAttribute(NED_EXPRESSION, "target", "src-module-index")!=NULL;
    bool srcGateIx =  node->getFirstChildWithAttribute(NED_EXPRESSION, "target", "src-gate-index")!=NULL || node->getSrcGatePlusplus();
    bool destModIx =  node->getFirstChildWithAttribute(NED_EXPRESSION, "target", "dest-module-index")!=NULL;
    bool destGateIx = node->getFirstChildWithAttribute(NED_EXPRESSION, "target", "dest-gate-index")!=NULL || node->getDestGatePlusplus();
    validateConnGate(node->getSrcModule(), srcModIx, node->getSrcGate(), srcGateIx, compound, node, true);
    validateConnGate(node->getDestModule(), destModIx, node->getDestGate(), destGateIx, compound, node, false);
}

void NEDCrossValidator::validateElement(ChannelSpecElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(ChannelInterfaceElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(ChannelElement *node)
{
    // FIXME revise
    // make sure channel type name does not exist yet
    if (getChannelDeclaration(node->getName()))
        errors->addError(node, "redefinition of channel with name '%s'",node->getName());
}

void NEDCrossValidator::validateElement(ConnectionGroupElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(LoopElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(ConditionElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(ExpressionElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(OperatorElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(FunctionElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(IdentElement *node)
{
    // FIXME revise
}

void NEDCrossValidator::validateElement(LiteralElement *node)
{
}

void NEDCrossValidator::validateElement(MsgFileElement *node)
{
}

void NEDCrossValidator::validateElement(NamespaceElement *node)
{
}

void NEDCrossValidator::validateElement(CplusplusElement *node)
{
}

void NEDCrossValidator::validateElement(StructDeclElement *node)
{
}

void NEDCrossValidator::validateElement(ClassDeclElement *node)
{
}

void NEDCrossValidator::validateElement(MessageDeclElement *node)
{
}

void NEDCrossValidator::validateElement(PacketDeclElement *node)
{
}

void NEDCrossValidator::validateElement(EnumDeclElement *node)
{
}

void NEDCrossValidator::validateElement(EnumElement *node)
{
    // check extends-name
    const char *baseName = node->getExtendsName();
    NEDElement *base = getEnumDeclaration(baseName);
    if (!base)
        errors->addError(node, "unknown base enum type '%s'",baseName);
}

void NEDCrossValidator::validateElement(EnumFieldsElement *node)
{
}

void NEDCrossValidator::validateElement(EnumFieldElement *node)
{
}

void NEDCrossValidator::validateElement(MessageElement *node)
{
    // check extends-name
    const char *baseClassName = node->getExtendsName();
    NEDElement *baseClass = getClassDeclaration(baseClassName);
    if (!baseClass)
        errors->addError(node, "unknown base class '%s'",baseClassName);
}

void NEDCrossValidator::validateElement(PacketElement *node)
{
    // check extends-name
    const char *baseClassName = node->getExtendsName();
    NEDElement *baseClass = getClassDeclaration(baseClassName);
    if (!baseClass)
        errors->addError(node, "unknown base class '%s'",baseClassName);
}

void NEDCrossValidator::validateElement(ClassElement *node)
{
    // check extends-name
    const char *baseClassName = node->getExtendsName();
    NEDElement *baseClass = getClassDeclaration(baseClassName);
    if (!baseClass)
        errors->addError(node, "unknown base class '%s'",baseClassName);
}

void NEDCrossValidator::validateElement(StructElement *node)
{
    // check extends-name
    const char *baseClassName = node->getExtendsName();
    NEDElement *baseClass = getClassDeclaration(baseClassName);
    if (!baseClass)
        errors->addError(node, "unknown base class '%s'",baseClassName);
}

void NEDCrossValidator::validateElement(FieldElement *node)
{
}

void NEDCrossValidator::validateElement(UnknownElement *node)
{
}

