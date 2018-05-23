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

static struct { const char *fname; int args; } known_funcs[] =
{
   /* <math.h> */
   {"fabs", 1},    {"fmod", 2},
   {"acos", 1},    {"asin", 1},    {"atan", 1},   {"atan2", 1},
   {"sin", 1},     {"cos", 1},     {"tan", 1},    {"hypot", 2},
   {"ceil", 1},    {"floor", 1},
   {"exp", 1},     {"pow", 2},     {"sqrt", 1},
   {"log",  1},    {"log10", 1},

   /* OMNeT++ general */
   {"min", 2},
   {"max", 2},

   /* OMNeT++: distributions without rng-id arg */
   {"uniform",2},
   {"exponential",1},
   {"normal",2},
   {"truncnormal",2},
   {"gamma_d",2},
   {"beta",2},
   {"erlang_k",2},
   {"chi_square",1},
   {"student_t",1},
   {"cauchy",2},
   {"triang",3},
   {"lognormal",2},
   {"weibull",2},
   {"pareto_shifted",3},
   {"intuniform",2},
   {"bernoulli",1},
   {"binomial",2},
   {"geometric",1},
   {"negbinomial",2},
   {"hypergeometric",3},
   {"poisson",1},

   /* OMNeT++: distributions with rng-id arg */
   {"uniform",3},
   {"exponential",2},
   {"normal",3},
   {"truncnormal",3},
   {"gamma_d",3},
   {"beta",3},
   {"erlang_k",3},
   {"chi_square",2},
   {"student_t",2},
   {"cauchy",3},
   {"triang",4},
   {"lognormal",3},
   {"weibull",3},
   {"pareto_shifted",4},
   {"intuniform",3},
   {"bernoulli",2},
   {"binomial",3},
   {"geometric",2},
   {"negbinomial",3},
   {"hypergeometric",4},
   {"poisson",2},

   /* END */
   {nullptr,0}
};

void NedSyntaxValidator::checkExpressionAttributes(ASTNode *node, const char *attrs[], bool optional[], int n)
{
    if (parsedExpressions) {
        // allow attribute values to be present, but check there are no
        // Expression children that are not in the list
        for (ASTNode *child = node->getFirstChildWithTag(NED_EXPRESSION); child; child = child->getNextSiblingWithTag(NED_EXPRESSION)) {
            ExpressionElement *expr = (ExpressionElement *)child;
            const char *target = expr->getTarget();
            int i;
            for (i = 0; i < n; i++)
                if (!strcmp(target, attrs[i]))
                    break;

            if (i == n)
                errors->addError(child, "'expression' element with invalid target attribute '%s'", target);
        }
    }
    else {
        // check: should be no Expression children at all
        if (node->getFirstChildWithTag(NED_EXPRESSION))
            errors->addError(node, "'expression' element found while using non-parsed expressions\n");
    }

    // check mandatory expressions are there
    for (int i = 0; i < n; i++) {
        if (!optional[i]) {
            if (parsedExpressions) {
                // check: Expression element must be there
                ExpressionElement *expr;
                for (expr = (ExpressionElement *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr = expr->getNextExpressionSibling())
                    if (!opp_isempty(expr->getTarget()) && !strcmp(expr->getTarget(), attrs[i]))
                        break;

                if (!expr)
                    errors->addError(node, "Expression-valued attribute '%s' not present in parsed form (missing 'expression' element)", attrs[i]);
            }
            else {
                // attribute must be there
                if (!node->getAttribute(attrs[i]) || !(node->getAttribute(attrs[i]))[0])
                    errors->addError(node, "Missing attribute '%s'", attrs[i]);
            }
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
        if (!opp_isalnumext(*s) && *s != '_' && *s != '.' && (wildcardsAllowed ? *s != '*' : true)) {
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
        if (!opp_isalnumext(*s) && strchr("_-:.", *s) == nullptr) {  // note: same rule as for XML attribute names
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
        if (!opp_isalnumext(*s) && strchr("*?{}_-:.", *s) == nullptr) {  // note: same rule as for XML attribute names, plus wildcards
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
        if (parsedExpressions) {
            if (node->getFirstChildWithTag(NED_EXPRESSION))  // && expr->getTarget()=="value"
                errors->addError(node, "Cannot specify parameter values within a module interface or channel interface");
        }
        else {
            if (!opp_isempty(node->getValue()))
                errors->addError(node, "Cannot specify parameter values within a module interface or channel interface");
        }
    }
}

void NedSyntaxValidator::validateElement(PropertyElement *node)
{
    checkPropertyNameAttribute(node, "name");
    checkPropertyIndexAttribute(node, "index");

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
    bool srcGateIx = node->getFirstChildWithAttribute(NED_EXPRESSION, "target", "src-gate-index") != nullptr;
    bool destGateIx = node->getFirstChildWithAttribute(NED_EXPRESSION, "target", "dest-gate-index") != nullptr;
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

void NedSyntaxValidator::validateElement(ExpressionElement *node)
{
    // TODO
}

void NedSyntaxValidator::validateElement(OperatorElement *node)
{
    // check operator name is valid and argument count matches
    const char *op = node->getName();

    // next list uses space as separator, so make sure op does not contain space
    if (strchr(op, ' ')) {
        errors->addError(node, "Invalid operator '%s' (contains space)", op);
        return;
    }

    // count arguments
    int args = 0;
    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling())
        args++;

    // unary?
    if (strstr("! ~", op)) {
        if (args != 1)
            errors->addError(node, "Operator '%s' should have 1 operand, not %d", op, args);
    }
    // unary or binary?
    else if (strstr("-", op)) {
        if (args != 1 && args != 2)
            errors->addError(node, "Operator '%s' should have 1 or 2 operands, not %d", op, args);
    }
    // binary?
    else if (strstr("+ * / % ^ == != > >= < <= && || ## & | # << >>", op)) {
        if (args != 2)
            errors->addError(node, "Operator '%s' should have 2 operands, not %d", op, args);
    }
    // tertiary?
    else if (strstr("?:", op)) {
        if (args != 3)
            errors->addError(node, "Operator '%s' should have 3 operands, not %d", op, args);
    }
    else {
        errors->addError(node, "Invalid operator '%s'", op);
    }
}

void NedSyntaxValidator::validateElement(FunctionElement *node)
{
    // if we know the function, check argument count
    const char *func = node->getName();
    int args = node->getNumChildren();

    // if it's an operator, treat specially
    if (!strcmp(func, "index")) {
        if (args != 0)
            errors->addError(node, "Operator 'index' does not take arguments");

        // find expression and submodule node we're under
        ASTNode *parent = node->getParent();
        while (parent && parent->getTagCode() != NED_EXPRESSION)
            parent = parent->getParent();
        ASTNode *expr = parent;

        while (parent && parent->getTagCode() != NED_SUBMODULE)
            parent = parent->getParent();
        ASTNode *submod = parent;

        if (!submod || submod->getFirstChildWithAttribute(NED_EXPRESSION, "target", "vector-size") == nullptr)
            errors->addError(node, "'index' may only occur in a submodule vector's definition");
        if (expr->getParent() == submod)
            errors->addError(node, "'index' is not allowed here");
        return;
    }
    else if (!strcmp(func, "exists")) {
        if (args != 1)
            errors->addError(node, "Operator 'exists' takes one argument");
        // else if (node->getFirstChild()->getTagCode()!=NED_IDENT)
        //    errors->addError(node, "Argument of operator 'exists' should be an identifier");
        else {
            // TBD further check it's a submodule name
        }
        return;
    }
    else if (!strcmp(func, "typename")) {
        if (args != 0)
            errors->addError(node, "Operator 'typename' does not take arguments");

        // containing expression
        ASTNode *parent = node->getParent();
        while (parent && parent->getTagCode() != NED_EXPRESSION)
            parent = parent->getParent();
        ASTNode *expr = parent;

        // typename is not allowed in the like-expression which defines the type
        if (!strcmp(expr->getAttribute("target"), "like-expr"))
            errors->addError(node, "'typename' may not occur in a type name expression");
        return;
    }
    else if (!strcmp(func, "sizeof")) {
        if (args != 1)
            errors->addError(node, "Operator 'sizeof' takes one argument");
        // else if (node->getFirstChild()->getTagCode()!=NED_IDENT)
        //    errors->addError(node, "Argument of operator 'sizeof' should be an identifier");
        else {
            // TBD further check it's an existing parent module gate or submodule name
        }
        return;
    }
    else if (!strcmp(func, "input")) {
        if (args > 2)
            errors->addError(node, "Operator 'input' takes 0, 1 or 2 arguments");
        ASTNode *op1 = node->getFirstChild();
        ASTNode *op2 = op1 ? op1->getNextSibling() : nullptr;
        if (args == 2)
            if (op2->getTagCode() != NED_LITERAL || ((LiteralElement *)op2)->getType() != LIT_STRING)
                errors->addError(node, "Second argument to 'input()' must be a string literal (prompt text)");

        ASTNode *parent = node->getParent();
        if (parent->getTagCode() != NED_EXPRESSION)
            errors->addError(node, "'input()' occurs in wrong place");
        return;
    }
    else if (!strcmp(func, "xmldoc")) {
        if (args != 1 && args != 2) {
            errors->addError(node, "'xmldoc()' takes 1 or 2 arguments");
            return;
        }
        ASTNode *op1 = node->getFirstChild();
        ASTNode *op2 = op1 ? op1->getNextSibling() : nullptr;
        if (op1->getTagCode() != NED_LITERAL || ((LiteralElement *)op1)->getType() != LIT_STRING ||
            (op2 && (op2->getTagCode() != NED_LITERAL || ((LiteralElement *)op2)->getType() != LIT_STRING)))
            errors->addError(node, "'xmldoc()' arguments must be string literals");
        return;
    }

    // check if we know about it
    bool nameFound = false;
    bool argCountMatches = false;
    for (int i = 0; known_funcs[i].fname != nullptr; i++) {
        if (!strcmp(func, known_funcs[i].fname)) {
            nameFound = true;
            if (known_funcs[i].args == args) {
                argCountMatches = true;
                break;
            }
        }
    }
    if (nameFound && !argCountMatches) {
        errors->addError(node, "Function '%s()' does not take %d arguments", func, args);
    }
}

void NedSyntaxValidator::validateElement(IdentElement *node)
{
    const char *expr[] = { "module-index", "param-index" };
    bool opt[] = { true, true };
    checkExpressionAttributes(node, expr, opt, 2);
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
            errors->addError(node, e.what());
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

