//==========================================================================
// nedsemanticvalidator.cc - part of the OMNeT++ Discrete System Simulation System
//
// Contents:
//   class NEDSemanticValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include "nederror.h"
#include "nedsemanticvalidator.h"
#include "nedcompiler.h" // for NEDSymbolTable


NEDSemanticValidator::NEDSemanticValidator(bool parsedExpr, NEDSymbolTable *symbtab)
{
    parsedExpressions = parsedExpr;
    symboltable = symbtab;
}

NEDSemanticValidator::~NEDSemanticValidator()
{
}

NEDElement *NEDSemanticValidator::findChildWithTagAndAttribute(NEDElement *node, int tag, const char *attr, const char *value)
{
    for (NEDElement *child=node->getFirstChildWithTag(tag); child; child = child->getNextSiblingWithTag(tag))
        if (!strcmp(child->getAttribute(attr), value))
            return child;
    return NULL;
}

void NEDSemanticValidator::validateElement(NedFilesNode *node)
{
}

void NEDSemanticValidator::validateElement(NedFileNode *node)
{
}

void NEDSemanticValidator::validateElement(ImportNode *node)
{
}

void NEDSemanticValidator::validateElement(ImportedFileNode *node)
{
}

void NEDSemanticValidator::validateElement(ChannelNode *node)
{
    // make sure channel type name does not exist yet
    if (symboltable->getChannelDeclaration(node->getName()))
        NEDError(node, "redefinition of channel with name '%s'",node->getName());
}

void NEDSemanticValidator::validateElement(ChannelAttrNode *node)
{
}

void NEDSemanticValidator::validateElement(NetworkNode *node)
{
    // make sure network name does not exist yet
    if (symboltable->getNetworkDeclaration(node->getName()))
        NEDError(node, "redefinition of network with name '%s'",node->getName());

    // make sure module type exists
    const char *type_name = node->getLikeName() ? node->getLikeName() : node->getTypeName();
    moduletypedecl = symboltable->getModuleDeclaration(type_name);
    if (!moduletypedecl)
        NEDError(node, "unknown module type '%s'",type_name);
}

void NEDSemanticValidator::validateElement(SimpleModuleNode *node)
{
    // make sure module type name does not exist yet
    if (symboltable->getModuleDeclaration(node->getName()))
        NEDError(node, "redefinition of module with name '%s'",node->getName());
}

void NEDSemanticValidator::validateElement(CompoundModuleNode *node)
{
    // make sure module type name does not exist yet
    if (symboltable->getModuleDeclaration(node->getName()))
        NEDError(node, "redefinition of module with name '%s'",node->getName());
}

void NEDSemanticValidator::validateElement(ParamsNode *node)
{
}

void NEDSemanticValidator::validateElement(ParamNode *node)
{
}

void NEDSemanticValidator::validateElement(GatesNode *node)
{
}

void NEDSemanticValidator::validateElement(GateNode *node)
{
}

void NEDSemanticValidator::validateElement(MachinesNode *node)
{
}

void NEDSemanticValidator::validateElement(MachineNode *node)
{
}

void NEDSemanticValidator::validateElement(SubmodulesNode *node)
{
}

void NEDSemanticValidator::validateElement(SubmoduleNode *node)
{
    // make sure module type exists
    const char *type_name = node->getLikeName() ? node->getLikeName() : node->getTypeName();
    moduletypedecl = symboltable->getModuleDeclaration(type_name);
    if (!moduletypedecl)
        NEDError(node, "unknown module type '%s'",type_name);
}

void NEDSemanticValidator::validateElement(SubstparamsNode *node)
{
}

void NEDSemanticValidator::validateElement(SubstparamNode *node)
{
    if (!moduletypedecl)
        return;

    // make sure parameter exists in module type
    const char *paramname = node->getName();
    ParamNode *paramdecl = (ParamNode *)findChildWithTagAndAttribute(moduletypedecl->getFirstChildWithTag(NED_PARAMS), NED_PARAM, "name", paramname);
    if (!paramdecl)
        NEDError(node, "module type does not have a parameter named '%s'",paramname);

    // check type matches
    //FIXME
}

void NEDSemanticValidator::validateElement(GatesizesNode *node)
{
}

void NEDSemanticValidator::validateElement(GatesizeNode *node)
{
    if (!moduletypedecl)
        return;

    // make sure gate exists in module type
    const char *gatename = node->getName();
    GateNode *gatedecl = (GateNode *)findChildWithTagAndAttribute(moduletypedecl->getFirstChildWithTag(NED_GATES), NED_GATE, "name", gatename);
    if (!gatedecl)
        NEDError(node, "module type does not have a gate name '%s'",gatename);

    // check it is vector
    //FIXME
}

void NEDSemanticValidator::validateElement(SubstmachinesNode *node)
{
    // make sure machine counts match in module type and here
    //FIXME
}

void NEDSemanticValidator::validateElement(SubstmachineNode *node)
{
}

void NEDSemanticValidator::validateElement(ConnectionsNode *node)
{
}

void NEDSemanticValidator::validateElement(ConnectionNode *node)
{
}

void NEDSemanticValidator::validateElement(ConnAttrNode *node)
{
}

void NEDSemanticValidator::validateElement(ForLoopNode *node)
{
}

void NEDSemanticValidator::validateElement(LoopVarNode *node)
{
}

void NEDSemanticValidator::validateElement(DisplayStringNode *node)
{
}

void NEDSemanticValidator::validateElement(ExpressionNode *node)
{
}

void NEDSemanticValidator::validateElement(OperatorNode *node)
{
}

void NEDSemanticValidator::validateElement(FunctionNode *node)
{
}

void NEDSemanticValidator::validateElement(ParamRefNode *node)
{
}

void NEDSemanticValidator::validateElement(IdentNode *node)
{
}

void NEDSemanticValidator::validateElement(ConstNode *node)
{
}

void NEDSemanticValidator::validateElement(UnknownNode *node)
{
}

