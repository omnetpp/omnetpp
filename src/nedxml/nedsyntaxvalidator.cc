//==========================================================================
// nedsyntaxvalidator.cc
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NedSyntaxValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <clocale>
#include "common/opp_ctype.h"
#include "common/stringutil.h"
#include "common/unitconversion.h"
#include "nedsyntaxvalidator.h"

#include "errorstore.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

void NedSyntaxValidator::checkExpressionAttributes(ASTNode *node, const char *attrs[], bool optional[], int n)
{
    // check mandatory expressions are there
    for (int i = 0; i < n; i++) {
        if (!optional[i]) {
            // attribute must be there
            if (!node->getAttribute(attrs[i]) || !(node->getAttribute(attrs[i]))[0])
                errors->addError(node, "Missing attribute '%s'", attrs[i]);
        }
    }
}

void NedSyntaxValidator::checkEnumAttribute(ASTNode *node, const char *attr, const char *values[], int n)
{
    const char *value = node->getAttribute(attr);
    for (int i = 0; i < n; i++)
        if (strcmp(value, values[i]) == 0)
            return;
    errors->addError(node, "Illegal attribute value '%s'", value);
}

void NedSyntaxValidator::checkDottedNameAttribute(ASTNode *node, const char *attr, bool wildcardsAllowed)
{
    const char *s = node->getAttribute(attr);
    assert(s);
    if (!*s)
        return;
    for ( ; *s; s++)
        if (!opp_isalnumext(*s) && *s != '.' && (wildcardsAllowed ? *s != '*' : true)) {
            errors->addError(node, "Illegal character in %s '%s'", attr, node->getAttribute(attr));
            return;
        }
}

void NedSyntaxValidator::checkPropertyNameAttribute(ASTNode *node, const char *attr)
{
    const char *s = node->getAttribute(attr);
    assert(s);
    if (!*s)
        return;
    for ( ; *s; s++)
        if (!opp_isalnumext(*s) && strchr("-:.", *s) == nullptr) {  // note: same rule as for XML attribute names
            errors->addError(node, "Illegal character in %s '%s'", attr, node->getAttribute(attr));
            return;
        }
}

void NedSyntaxValidator::checkPropertyIndexAttribute(ASTNode *node, const char *attr)
{
    const char *s = node->getAttribute(attr);
    assert(s);
    if (!*s)
        return;
    for ( ; *s; s++)
        if (!opp_isalnumext(*s) && strchr("*?{}-:.", *s) == nullptr) {  // note: same rule as for XML attribute names, plus wildcards
            errors->addError(node, "Illegal character in %s '%s'", attr, node->getAttribute(attr));
            return;
        }
}

bool NedSyntaxValidator::isWithinSubcomponent(ASTNode *node)
{
    // only returns true if node is within the BODY of a submodule or a connection
    // (i.e. returns *false* for a submodule vector size)
    for ( ; node != nullptr; node = node->getParent()) {
        if (node->getTagCode() == NED_PARAMETERS || node->getTagCode() == NED_GATES) {
            int sectionOwnerType = node->getParent()->getTagCode();
            return sectionOwnerType == NED_SUBMODULE || sectionOwnerType == NED_CONNECTION;
        }
    }
    return false;
}

bool NedSyntaxValidator::isWithinInnerType(ASTNode *node)
{
    for ( ; node != nullptr; node = node->getParent())
        if (node->getTagCode() == NED_TYPES)
            return true;

    return false;
}

void NedSyntaxValidator::validateElement(FilesElement *node)
{
}

void NedSyntaxValidator::validateElement(NedFileElement *node)
{
}

void NedSyntaxValidator::validateElement(CommentElement *node)
{
    const char *values[] = { "banner", "right", "trailing", "inner" };
    checkEnumAttribute(node, "locid", values, 4);
}

void NedSyntaxValidator::validateElement(PackageElement *node)
{
    checkDottedNameAttribute(node, "name", false);
}

void NedSyntaxValidator::validateElement(ImportElement *node)
{
    checkDottedNameAttribute(node, "import-spec", true);
}

void NedSyntaxValidator::validateElement(PropertyDeclElement *node)
{
    checkPropertyNameAttribute(node, "name");
}

void NedSyntaxValidator::validateElement(ExtendsElement *node)
{
    checkDottedNameAttribute(node, "name", false);
}

void NedSyntaxValidator::validateElement(InterfaceNameElement *node)
{
    checkDottedNameAttribute(node, "name", false);
}

void NedSyntaxValidator::validateElement(SimpleModuleElement *node)
{
}

void NedSyntaxValidator::validateElement(ModuleInterfaceElement *node)
{
}

void NedSyntaxValidator::validateElement(CompoundModuleElement *node)
{
}

void NedSyntaxValidator::validateElement(ParametersElement *node)
{
}

void NedSyntaxValidator::validateElement(ParamElement *node)
{
    ASTNode *parent = node->getParent();
    if (parent)
        parent = parent->getParent();

    // param declarations cannot occur in submodules
    if (parent->getTagCode() == NED_SUBMODULE) {
        if (node->getType() != PARTYPE_NONE)
            errors->addError(node, "Cannot define new parameters within a submodule");
    }

    // check isPattern flag is consistent with 'name' attribute
    bool containsDot = strchr(node->getName(), '.') != nullptr;
    if (!node->getIsPattern() && containsDot)
        errors->addError(node, "Parameter names must not contain a dot");
    if (node->getIsPattern() && !containsDot)
        errors->addError(node, "Parameter name patterns must contain a dot");

    // type+pattern are not compatible
    if (node->getIsPattern() && node->getType() != PARTYPE_NONE)
        errors->addError(node, "Name must be an identifier when defining a new parameter");

    // in module or channel interfaces, one cannot specify parameter values
    if (parent->getTagCode() == NED_MODULE_INTERFACE || parent->getTagCode() == NED_CHANNEL_INTERFACE) {
        if (!opp_isempty(node->getValue()))
            errors->addError(node, "Cannot specify parameter values within a module interface or channel interface");
    }
}

void NedSyntaxValidator::validateElement(PropertyElement *node)
{
    checkPropertyNameAttribute(node, "name");
    checkPropertyIndexAttribute(node, "index");

    if (opp_streq(node->getName(), "suppressWarnings"))
        return;

    // properties cannot occur on submodule or connection parameters/gates.
    // structure: submodule>parameters>parameter>property
    ASTNode *parent = node->getParent();
    if (parent && (parent->getTagCode() == NED_PARAM || parent->getTagCode() == NED_GATE)) {
        ASTNode *container = parent->getParent()->getParent();
        if (container && (container->getTagCode() == NED_SUBMODULE || container->getTagCode() == NED_CONNECTION))
            errors->addError(node, "Cannot modify parameter/gate properties in a submodule or connection");
    }
}

void NedSyntaxValidator::validateElement(PropertyKeyElement *node)
{
    checkPropertyNameAttribute(node, "name");
}

void NedSyntaxValidator::validateElement(GatesElement *node)
{
}

void NedSyntaxValidator::validateElement(TypesElement *node)
{
}

void NedSyntaxValidator::validateElement(GateElement *node)
{
    // param declarations cannot occur in submodules
    if (node->getType() != GATETYPE_NONE) {
        ASTNode *parent = node->getParent();
        if (parent)
            parent = parent->getParent();
        if (parent->getTagCode() == NED_SUBMODULE)
            errors->addError(node, "Cannot define new gates within a submodule");
    }
}

void NedSyntaxValidator::validateElement(SubmodulesElement *node)
{
}

void NedSyntaxValidator::validateElement(SubmoduleElement *node)
{
    const char *expr[] = { "like-expr", "vector-size" };
    bool opt[] = { true, true };
    checkExpressionAttributes(node, expr, opt, 2);

    checkDottedNameAttribute(node, "type", false);
    checkDottedNameAttribute(node, "like-type", false);
}

void NedSyntaxValidator::validateElement(ConnectionsElement *node)
{
}

void NedSyntaxValidator::validateElement(ConnectionElement *node)
{
    const char *expr[] = { "condition", "src-module-index", "src-gate-index", "dest-module-index", "dest-gate-index", "like-expr" };
    bool opt[] = { true, true, true, true, true, true };
    checkExpressionAttributes(node, expr, opt, 6);

    // plusplus and gate index expression cannot be both there
    bool srcGateIx = !opp_isempty(node->getSrcGateIndex());
    bool destGateIx = !opp_isempty(node->getDestGateIndex());
    if (srcGateIx && node->getSrcGatePlusplus())
        errors->addError(node, "Wrong source gate: cannot have both gate index and '++' operator specified");
    if (destGateIx && node->getDestGatePlusplus())
        errors->addError(node, "Wrong destination gate: cannot have both gate index and '++' operator specified");

    checkDottedNameAttribute(node, "type", false);
    checkDottedNameAttribute(node, "like-type", false);
}

void NedSyntaxValidator::validateElement(ChannelInterfaceElement *node)
{
}

void NedSyntaxValidator::validateElement(ChannelElement *node)
{
}

void NedSyntaxValidator::validateElement(ConnectionGroupElement *node)
{
    // TODO check loop vars are unique
}

void NedSyntaxValidator::validateElement(LoopElement *node)
{
    const char *expr[] = {"from-value", "to-value"};
    bool opt[] = {false, false};
    checkExpressionAttributes(node, expr, opt, 2);
}

void NedSyntaxValidator::validateElement(ConditionElement *node)
{
    const char *expr[] = {"condition"};
    bool opt[] = {true};
    checkExpressionAttributes(node, expr, opt, 1);
}

void NedSyntaxValidator::validateElement(LiteralElement *node)
{
    // verify syntax of constant
    int type = node->getType();
    const char *value = node->getValue();
    // const char *text = node->getText();

    // Note: null value is valid as well, because that represents the "" string literal!
    if (opp_isempty(value))
        value = "";

    if (type == LIT_BOOL) {
        // check bool
        if (strcmp(value, "true") && strcmp(value, "false"))
            errors->addError(node, "Bool constant should be 'true' or 'false'");
        // TBD check that if text is present, it's the same as value
    }
    else if (type == LIT_INT) {
        // check int
        char *s;
        (void)strtol(value, &s, 0);
        if (s && *s)
            errors->addError(node, "Invalid integer constant '%s'", value);
        // TBD check that if text is present, it's the same as value
    }
    else if (type == LIT_DOUBLE) {
        // check real
        char *s;
        setlocale(LC_NUMERIC, "C");
        (void)strtod(value, &s);
        if (s && *s)
            errors->addError(node, "Invalid real constant '%s'", value);
        // TBD check that if text is present, it's the same as value
    }
    else if (type == LIT_QUANTITY) {
        // check quantity
        try {
            // validate syntax and unit compatibility ("5s 2kg")
            std::string unit;
            UnitConversion::parseQuantity(value, unit);
        }
        catch (std::exception& e) {
            errors->addError(node, "%s", e.what());
        }
        // TBD check that if text is present, it's the same as value (modulo whitespace)
    }
    else if (type == LIT_STRING) {
        // string: no restriction on value
        // TBD check that if text is present, it's a quoted version of the value
    }
}

void NedSyntaxValidator::validateElement(UnknownElement *node)
{
}

}  // namespace nedxml
}  // namespace omnetpp

