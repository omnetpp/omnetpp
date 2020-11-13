//==========================================================================
// nedcrossvalidator.cc - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NedCrossValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/stringutil.h"
#include "exception.h"
#include "nedresourcecache.h"
#include "nedcrossvalidator.h"

#include "errorstore.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

NedCrossValidator::NedCrossValidator(NedResourceCache *res, ErrorStore *e)
    : NedValidatorBase(e), resolver(res)
{
}

ASTNode *NedCrossValidator::getXXXDeclaration(const char *name, int tagcode1, int tagcode2)
{
    NedTypeInfo *component = resolver->lookup(name);
    if (!component)
        return nullptr;

    int type = component->getTree()->getTagCode();
    if (type != tagcode1 && type != tagcode2)
        return nullptr;
    return component->getTree();
}

ASTNode *NedCrossValidator::getModuleDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_SIMPLE_MODULE, NED_COMPOUND_MODULE);
}

ASTNode *NedCrossValidator::getChannelDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_CHANNEL);
}

ASTNode *NedCrossValidator::getModuleInterfaceDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_MODULE_INTERFACE);
}

ASTNode *NedCrossValidator::getChannelInterfaceDeclaration(const char *name)
{
    return getXXXDeclaration(name, NED_CHANNEL_INTERFACE);
}

void NedCrossValidator::validateElement(FilesElement *node)
{
}

void NedCrossValidator::validateElement(NedFileElement *node)
{
}

void NedCrossValidator::validateElement(CommentElement *node)
{
}

void NedCrossValidator::validateElement(ImportElement *node)
{
}

void NedCrossValidator::validateElement(PropertyDeclElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(ExtendsElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(InterfaceNameElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(SimpleModuleElement *node)
{
    // FIXME revise
    // make sure module type name does not exist yet
    if (getModuleDeclaration(node->getName()))
        errors->addError(node, "redefinition of module with name '%s'", node->getName());
}

void NedCrossValidator::validateElement(ModuleInterfaceElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(CompoundModuleElement *node)
{
    // FIXME revise
    // make sure module type name does not exist yet
    if (getModuleDeclaration(node->getName()))
        errors->addError(node, "redefinition of module with name '%s'", node->getName());
}

void NedCrossValidator::validateElement(ParametersElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(ParamElement *node)
{
    // FIXME code comes from substparamnode -- REVISE
    if (!moduleTypeDecl)
        return;

    // make sure parameter exists in module type
    const char *paramName = node->getName();
    ASTNode *params = moduleTypeDecl->getFirstChildWithTag(NED_PARAMETERS);
    if (!params || params->getFirstChildWithAttribute(NED_PARAM, "name", paramName) == nullptr) {
        errors->addError(node, "module type '%s' has no parameter named '%s'", moduleTypeDecl->getAttribute("name"), paramName);
        return;
    }

    // TBD compile-time check for type mismatch
}

void NedCrossValidator::validateElement(PropertyElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(PropertyKeyElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(GatesElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(GateElement *node)
{
    // FIXME the following lines come from gatesizenode -- REVISE!
    if (!moduleTypeDecl)
        return;

    // make sure gate exists in module type
    const char *gatename = node->getName();
    GatesElement *gatesdecl = (GatesElement *)moduleTypeDecl->getFirstChildWithTag(NED_GATES);
    if (!gatesdecl) {
        errors->addError(node, "module type does not have gates");
        return;
    }
    GateElement *gatedecl = (GateElement *)gatesdecl->getFirstChildWithAttribute(NED_GATE, "name", gatename);
    if (!gatedecl) {
        errors->addError(node, "module type does not have a gate named '%s'", gatename);
        return;
    }

    // check it is vector
//    if (!gatedecl->getIsVector())
//        errors->addError(node, "gate '%s' is not a vector gate",gatename);
}

void NedCrossValidator::validateElement(TypesElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(SubmodulesElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(SubmoduleElement *node)
{
    // FIXME revise
    // make sure module type exists
    const char *type_name = node->getType();
    moduleTypeDecl = getModuleDeclaration(type_name);
    if (!moduleTypeDecl)
        errors->addError(node, "unknown module type '%s'", type_name);
}

void NedCrossValidator::validateElement(ConnectionsElement *node)
{
    // FIXME revise
}

void NedCrossValidator::checkGate(GateElement *gate, bool hasGateIndex, bool isInput, ASTNode *conn, bool isSrc)
{
    // FIXME revise
    // check gate direction, check if vector
    const char *q = isSrc ? "wrong source gate for connection" : "wrong destination gate for connection";
    if (hasGateIndex && !gate->getIsVector())
        errors->addError(conn, "%s: extra gate index or '++' ('%s' is not a vector gate)", q, gate->getName());
    else if (!hasGateIndex && gate->getIsVector())
        errors->addError(conn, "%s: missing gate index ('%s' is a vector gate)", q, gate->getName());

    // check gate direction, check if vector
    if (isInput && gate->getType() == GATETYPE_OUTPUT)
        errors->addError(conn, "%s: input gate expected but '%s' is an output gate", q, gate->getName());
    else if (!isInput && gate->getType() == GATETYPE_INPUT)
        errors->addError(conn, "%s: output gate expected but '%s' is an input gate", q, gate->getName());
}

void NedCrossValidator::validateConnGate(const char *submodName, bool hasSubmodIndex,
        const char *gateName, bool hasGateIndex,
        ASTNode *parent, ASTNode *conn, bool isSrc)
{
    // FIXME revise
    const char *q = isSrc ? "wrong source gate for connection" : "wrong destination gate for connection";
    if (opp_isempty(submodName)) {
        // connected to parent module: check such gate is declared
        ASTNode *gates = parent->getFirstChildWithTag(NED_GATES);
        GateElement *gate;
        if (!gates || (gate = (GateElement *)gates->getFirstChildWithAttribute(NED_GATE, "name", gateName)) == nullptr)
            errors->addError(conn, "%s: compound module has no gate named '%s'", q, gateName);
        else
            checkGate(gate, hasGateIndex, isSrc, conn, isSrc);
    }
    else {
        // check such submodule is declared
        ASTNode *submods = parent->getFirstChildWithTag(NED_SUBMODULES);
        SubmoduleElement *submod = nullptr;
        if (!submods || (submod = (SubmoduleElement *)submods->getFirstChildWithAttribute(NED_SUBMODULE, "name", submodName)) == nullptr) {
            errors->addError(conn, "%s: compound module has no submodule named '%s'", q, submodName);
        }
        else {
            bool isSubmodVector = !opp_isempty(submod->getVectorSize());
            if (hasSubmodIndex && !isSubmodVector)
                errors->addError(conn, "%s: extra submodule index ('%s' is not a vector submodule)", q, submodName);
            else if (!hasSubmodIndex && isSubmodVector)
                errors->addError(conn, "%s: missing submodule index ('%s' is a vector submodule)", q, submodName);

            // check gate
            ASTNode *submodType = getModuleDeclaration(submod->getType());
            if (!submodType)
                return;  // we gave error earlier if submod type is not present
            ASTNode *gates = submodType->getFirstChildWithTag(NED_GATES);
            GateElement *gate;
            if (!gates || (gate = (GateElement *)gates->getFirstChildWithAttribute(NED_GATE, "name", gateName)) == nullptr)
                errors->addError(conn, "%s: submodule '%s' has no gate named '%s'", q, submodName, gateName);
            else
                checkGate(gate, hasGateIndex, !isSrc, conn, isSrc);
        }
    }
}

void NedCrossValidator::validateElement(ConnectionElement *node)
{
    // FIXME revise
    // make sure submodule and gate names are valid, gate direction is OK
    // and that gates & modules are really vector (or really not)
    ASTNode *compound = node->getParentWithTag(NED_COMPOUND_MODULE);
    if (!compound)
        INTERNAL_ERROR0(node, "occurs outside a compound-module");

    bool srcModIx = !opp_isempty(node->getSrcModuleIndex());
    bool srcGateIx = !opp_isempty(node->getSrcGateIndex()) || node->getSrcGatePlusplus();
    bool destModIx = !opp_isempty(node->getDestModuleIndex());
    bool destGateIx = !opp_isempty(node->getDestGateIndex()) || node->getDestGatePlusplus();
    validateConnGate(node->getSrcModule(), srcModIx, node->getSrcGate(), srcGateIx, compound, node, true);
    validateConnGate(node->getDestModule(), destModIx, node->getDestGate(), destGateIx, compound, node, false);
}

void NedCrossValidator::validateElement(ChannelInterfaceElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(ChannelElement *node)
{
    // FIXME revise
    // make sure channel type name does not exist yet
    if (getChannelDeclaration(node->getName()))
        errors->addError(node, "redefinition of channel with name '%s'", node->getName());
}

void NedCrossValidator::validateElement(ConnectionGroupElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(LoopElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(ConditionElement *node)
{
    // FIXME revise
}

void NedCrossValidator::validateElement(LiteralElement *node)
{
}

void NedCrossValidator::validateElement(UnknownElement *node)
{
}

}  // namespace nedxml
}  // namespace omnetpp

