//==========================================================================
// nedsemanticvalidator.cc - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NEDSemanticValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "nederror.h"
#include "nedsemanticvalidator.h"
#include "nedresourcecache.h"
#include "stringutil.h"

USING_NAMESPACE


NEDSemanticValidator::NEDSemanticValidator(bool parsedExpr, NEDResourceCache *nedcache, NEDErrorStore *e)
   : NEDValidatorBase(e)
{
    parsedExpressions = parsedExpr;
    nedresourcecache = nedcache;
}

NEDSemanticValidator::~NEDSemanticValidator()
{
}

NEDElement *NEDSemanticValidator::getXXXDeclaration(const char *name, int tagcode1, int tagcode2)
{
    NEDComponent *component = nedresourcecache->lookup(name);
    if (!component)
        return NULL;

    int type = component->getTree()->getTagCode();
    if (type!=tagcode1 && type!=tagcode2)
        return NULL;
    return component->getTree();
}

NEDElement *NEDSemanticValidator::getModuleDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_SIMPLE_MODULE, NED_COMPOUND_MODULE);
}

NEDElement *NEDSemanticValidator::getChannelDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_CHANNEL);
}

NEDElement *NEDSemanticValidator::getModuleInterfaceDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_MODULE_INTERFACE);
}

NEDElement *NEDSemanticValidator::getChannelInterfaceDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_CHANNEL_INTERFACE);
}

NEDElement *NEDSemanticValidator::getEnumDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_ENUM);
}

NEDElement *NEDSemanticValidator::getClassDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_CLASS);
}

void NEDSemanticValidator::validateElement(FilesNode *node)
{
}

void NEDSemanticValidator::validateElement(NedFileNode *node)
{
}

void NEDSemanticValidator::validateElement(CommentNode *node)
{
}

void NEDSemanticValidator::validateElement(ImportNode *node)
{
}

void NEDSemanticValidator::validateElement(PropertyDeclNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(ExtendsNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(InterfaceNameNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(SimpleModuleNode *node)
{
    // FIXME revise
    // make sure module type name does not exist yet
    if (getModuleDeclaration(node->getName()))
        errors->addError(node, "redefinition of module with name '%s'",node->getName());
}

void NEDSemanticValidator::validateElement(ModuleInterfaceNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(CompoundModuleNode *node)
{
    // FIXME revise
    // make sure module type name does not exist yet
    if (getModuleDeclaration(node->getName()))
        errors->addError(node, "redefinition of module with name '%s'",node->getName());
}

void NEDSemanticValidator::validateElement(ParametersNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(ParamNode *node)
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

void NEDSemanticValidator::validateElement(PatternNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(PropertyNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(PropertyKeyNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(GatesNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(GateNode *node)
{
    // FIXME the following lines come from gatesizenode -- REVISE!
    if (!moduletypedecl)
        return;

    // make sure gate exists in module type
    const char *gatename = node->getName();
    GatesNode *gatesdecl = (GatesNode *)moduletypedecl->getFirstChildWithTag(NED_GATES);
    if (!gatesdecl)
    {
        errors->addError(node, "module type does not have gates");
        return;
    }
    GateNode *gatedecl = (GateNode *)gatesdecl->getFirstChildWithAttribute(NED_GATE, "name", gatename);
    if (!gatedecl)
    {
        errors->addError(node, "module type does not have a gate named '%s'",gatename);
        return;
    }

    // check it is vector
//    if (!gatedecl->getIsVector())
//        errors->addError(node, "gate '%s' is not a vector gate",gatename);
}

void NEDSemanticValidator::validateElement(TypesNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(SubmodulesNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(SubmoduleNode *node)
{
    // FIXME revise
    // make sure module type exists
    const char *type_name = node->getType();
    moduletypedecl = getModuleDeclaration(type_name);
    if (!moduletypedecl)
        errors->addError(node, "unknown module type '%s'",type_name);
}

void NEDSemanticValidator::validateElement(ConnectionsNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::checkGate(GateNode *gate, bool hasGateIndex, bool isInput, NEDElement *conn, bool isSrc)
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

void NEDSemanticValidator::validateConnGate(const char *submodName, bool hasSubmodIndex,
                                            const char *gateName, bool hasGateIndex,
                                            NEDElement *parent, NEDElement *conn, bool isSrc)
{
    // FIXME revise
    const char *q = isSrc ? "wrong source gate for connection" : "wrong destination gate for connection";
    if (opp_isempty(submodName))
    {
        // connected to parent module: check such gate is declared
        NEDElement *gates = parent->getFirstChildWithTag(NED_GATES);
        GateNode *gate;
        if (!gates || (gate=(GateNode*)gates->getFirstChildWithAttribute(NED_GATE, "name", gateName))==NULL)
            errors->addError(conn, "%s: compound module has no gate named '%s'", q, gateName);
        else
            checkGate(gate, hasGateIndex, isSrc, conn, isSrc);
    }
    else
    {
        // check such submodule is declared
        NEDElement *submods = parent->getFirstChildWithTag(NED_SUBMODULES);
        SubmoduleNode *submod = NULL;
        if (!submods || (submod=(SubmoduleNode*)submods->getFirstChildWithAttribute(NED_SUBMODULE, "name", submodName))==NULL)
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
            GateNode *gate;
            if (!gates || (gate=(GateNode*)gates->getFirstChildWithAttribute(NED_GATE, "name", gateName))==NULL)
                errors->addError(conn, "%s: submodule '%s' has no gate named '%s'", q, submodName, gateName);
            else
                checkGate(gate, hasGateIndex, !isSrc, conn, isSrc);
        }
    }
}

void NEDSemanticValidator::validateElement(ConnectionNode *node)
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

void NEDSemanticValidator::validateElement(ChannelSpecNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(ChannelInterfaceNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(ChannelNode *node)
{
    // FIXME revise
    // make sure channel type name does not exist yet
    if (getChannelDeclaration(node->getName()))
        errors->addError(node, "redefinition of channel with name '%s'",node->getName());
}

void NEDSemanticValidator::validateElement(ConnectionGroupNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(LoopNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(ConditionNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(ExpressionNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(OperatorNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(FunctionNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(IdentNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(LiteralNode *node)
{
}

void NEDSemanticValidator::validateElement(MsgFileNode *node)
{
}

void NEDSemanticValidator::validateElement(CplusplusNode *node)
{
}

void NEDSemanticValidator::validateElement(StructDeclNode *node)
{
}

void NEDSemanticValidator::validateElement(ClassDeclNode *node)
{
}

void NEDSemanticValidator::validateElement(MessageDeclNode *node)
{
}

void NEDSemanticValidator::validateElement(EnumDeclNode *node)
{
}

void NEDSemanticValidator::validateElement(EnumNode *node)
{
    // check extends-name
    const char *baseName = node->getExtendsName();
    NEDElement *base = getEnumDeclaration(baseName);
    if (!base)
        errors->addError(node, "unknown base enum type '%s'",baseName);
}

void NEDSemanticValidator::validateElement(EnumFieldsNode *node)
{
}

void NEDSemanticValidator::validateElement(EnumFieldNode *node)
{
}

void NEDSemanticValidator::validateElement(MessageNode *node)
{
    // check extends-name
    const char *baseClassName = node->getExtendsName();
    NEDElement *baseClass = getClassDeclaration(baseClassName);
    if (!baseClass)
        errors->addError(node, "unknown base class '%s'",baseClassName);
}

void NEDSemanticValidator::validateElement(ClassNode *node)
{
    // check extends-name
    const char *baseClassName = node->getExtendsName();
    NEDElement *baseClass = getClassDeclaration(baseClassName);
    if (!baseClass)
        errors->addError(node, "unknown base class '%s'",baseClassName);
}

void NEDSemanticValidator::validateElement(StructNode *node)
{
    // check extends-name
    const char *baseClassName = node->getExtendsName();
    NEDElement *baseClass = getClassDeclaration(baseClassName);
    if (!baseClass)
        errors->addError(node, "unknown base class '%s'",baseClassName);
}

void NEDSemanticValidator::validateElement(FieldNode *node)
{
}

void NEDSemanticValidator::validateElement(UnknownNode *node)
{
}

