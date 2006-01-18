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
#include "nedcompiler.h" // for NEDSymbolTable


inline bool strnull(const char *s)
{
    return !s || !s[0];
}

inline bool strnotnull(const char *s)
{
    return s && s[0];
}

NEDSemanticValidator::NEDSemanticValidator(bool parsedExpr, NEDSymbolTable *symbtab)
{
    parsedExpressions = parsedExpr;
    symboltable = symbtab;
}

NEDSemanticValidator::~NEDSemanticValidator()
{
}

void NEDSemanticValidator::validateElement(FilesNode *node)
{
}

void NEDSemanticValidator::validateElement(NedFileNode *node)
{
}

void NEDSemanticValidator::validateElement(WhitespaceNode *node)
{
}

void NEDSemanticValidator::validateElement(ImportNode *node)
{
}

void NEDSemanticValidator::validateElement(PropertydefNode *node)
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
    if (symboltable->getModuleDeclaration(node->getName()))
        NEDError(node, "redefinition of module with name '%s'",node->getName());
}

void NEDSemanticValidator::validateElement(ModuleInterfaceNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(CompoundModuleNode *node)
{
    // FIXME revise
    // make sure module type name does not exist yet
    if (symboltable->getModuleDeclaration(node->getName()))
        NEDError(node, "redefinition of module with name '%s'",node->getName());
}

void NEDSemanticValidator::validateElement(ParametersNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(ParamGroupNode *node)
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
        {NEDError(node, "module type '%s' has no parameter named '%s'", moduletypedecl->getAttribute("name"), paramName);return;}

    // TBD compile-time check for type mismatch
}

void NEDSemanticValidator::validateElement(PropertyNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(KeyValueNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(GatesNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(GateGroupNode *node)
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
        NEDError(node, "module type does not have gates");
        return;
    }
    GateNode *gatedecl = (GateNode *)gatesdecl->getFirstChildWithAttribute(NED_GATE, "name", gatename);
    if (!gatedecl)
    {
        NEDError(node, "module type does not have a gate named '%s'",gatename);
        return;
    }

    // check it is vector
//    if (!gatedecl->getIsVector())
//        NEDError(node, "gate '%s' is not a vector gate",gatename);
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
    moduletypedecl = symboltable->getModuleDeclaration(type_name);
    if (!moduletypedecl)
        NEDError(node, "unknown module type '%s'",type_name);
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
        NEDError(conn, "%s: extra gate index or '++' ('%s' is not a vector gate)", q, gate->getName());
    else if (!hasGateIndex && gate->getIsVector())
        NEDError(conn, "%s: missing gate index ('%s' is a vector gate)", q, gate->getName());

    // check gate direction, check if vector
    if (isInput && gate->getType()==NED_GATEDIR_OUTPUT)
        NEDError(conn, "%s: input gate expected but '%s' is an output gate", q, gate->getName());
    else if (!isInput && gate->getType()==NED_GATEDIR_INPUT)
        NEDError(conn, "%s: output gate expected but '%s' is an input gate", q, gate->getName());
}

void NEDSemanticValidator::validateConnGate(const char *submodName, bool hasSubmodIndex,
                                            const char *gateName, bool hasGateIndex,
                                            NEDElement *parent, NEDElement *conn, bool isSrc)
{
    // FIXME revise
    const char *q = isSrc ? "wrong source gate for connection" : "wrong destination gate for connection";
    if (strnull(submodName))
    {
        // connected to parent module: check such gate is declared
        NEDElement *gates = parent->getFirstChildWithTag(NED_GATES);
        GateNode *gate;
        if (!gates || (gate=(GateNode*)gates->getFirstChildWithAttribute(NED_GATE, "name", gateName))==NULL)
            NEDError(conn, "%s: compound module has no gate named '%s'", q, gateName);
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
            NEDError(conn, "%s: compound module has no submodule named '%s'", q, submodName);
        }
        else
        {
            bool isSubmodVector = submod->getFirstChildWithAttribute(NED_EXPRESSION, "target", "vector-size")!=NULL;
            if (hasSubmodIndex && !isSubmodVector)
                NEDError(conn, "%s: extra submodule index ('%s' is not a vector submodule)", q, submodName);
            else if (!hasSubmodIndex && isSubmodVector)
                NEDError(conn, "%s: missing submodule index ('%s' is a vector submodule)", q, submodName);

            // check gate
            NEDElement *submodType = symboltable->getModuleDeclaration(submod->getType());
            if (!submodType)
                return; // we gave error earlier if submod type is not present
            NEDElement *gates = submodType->getFirstChildWithTag(NED_GATES);
            GateNode *gate;
            if (!gates || (gate=(GateNode*)gates->getFirstChildWithAttribute(NED_GATE, "name", gateName))==NULL)
                NEDError(conn, "%s: submodule '%s' has no gate named '%s'", q, submodName, gateName);
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

void NEDSemanticValidator::validateElement(ChannelInterfaceNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(ChannelNode *node)
{
    // FIXME revise
    // make sure channel type name does not exist yet
    if (symboltable->getChannelDeclaration(node->getName()))
        NEDError(node, "redefinition of channel with name '%s'",node->getName());
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

void NEDSemanticValidator::validateElement(RefNode *node)
{
    // FIXME revise
}

void NEDSemanticValidator::validateElement(ConstNode *node)
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
    NEDElement *base = symboltable->getEnumDeclaration(baseName);
    if (!base)
        NEDError(node, "unknown base enum type '%s'",baseName);
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
    NEDElement *baseClass = symboltable->getClassDeclaration(baseClassName);
    if (!baseClass)
        NEDError(node, "unknown base class '%s'",baseClassName);
}

void NEDSemanticValidator::validateElement(ClassNode *node)
{
    // check extends-name
    const char *baseClassName = node->getExtendsName();
    NEDElement *baseClass = symboltable->getClassDeclaration(baseClassName);
    if (!baseClass)
        NEDError(node, "unknown base class '%s'",baseClassName);
}

void NEDSemanticValidator::validateElement(StructNode *node)
{
    // check extends-name
    const char *baseClassName = node->getExtendsName();
    NEDElement *baseClass = symboltable->getClassDeclaration(baseClassName);
    if (!baseClass)
        NEDError(node, "unknown base class '%s'",baseClassName);
}

void NEDSemanticValidator::validateElement(FieldsNode *node)
{
}

void NEDSemanticValidator::validateElement(FieldNode *node)
{
}

void NEDSemanticValidator::validateElement(PropertiesNode *node)
{
}

void NEDSemanticValidator::validateElement(MsgpropertyNode *node)
{
}

void NEDSemanticValidator::validateElement(UnknownNode *node)
{
}

