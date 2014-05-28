//==========================================================================
// nedsyntaxvalidator.cc
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NEDSyntaxValidator
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
#include <stdlib.h>
#include <assert.h>
#include <locale.h>
#include "opp_ctype.h"
#include "nederror.h"
#include "nedsyntaxvalidator.h"
#include "stringutil.h"
#include "unitconversion.h"

NAMESPACE_BEGIN


// FIXME TODO: assert types begin with capital letters, and submods/gates/params with lowercase! warning if not!


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
   {NULL,0}
};

void NEDSyntaxValidator::checkExpressionAttributes(NEDElement *node, const char *attrs[], bool optional[], int n)
{
    if (parsedExpressions)
    {
        // allow attribute values to be present, but check there are no
        // Expression children that are not in the list
        for (NEDElement *child=node->getFirstChildWithTag(NED_EXPRESSION); child; child=child->getNextSiblingWithTag(NED_EXPRESSION))
        {
            ExpressionElement *expr = (ExpressionElement *) child;
            const char *target = expr->getTarget();
            int i;
            for (i=0; i<n; i++)
                if (!strcmp(target, attrs[i]))
                    break;
            if (i==n)
                errors->addError(child, "'expression' element with invalid target attribute '%s'",target);
        }
    }
    else
    {
        // check: should be no Expression children at all
        if (node->getFirstChildWithTag(NED_EXPRESSION))
            errors->addError(node, "'expression' element found while using non-parsed expressions\n");
    }

    // check mandatory expressions are there
    for (int i=0; i<n; i++)
    {
       if (!optional[i])
       {
           if (parsedExpressions)
           {
               // check: Expression element must be there
               ExpressionElement *expr;
               for (expr=(ExpressionElement *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr=expr->getNextExpressionSibling())
                   if (!opp_isempty(expr->getTarget()) && !strcmp(expr->getTarget(),attrs[i]))
                       break;
               if (!expr)
                   errors->addError(node, "expression-valued attribute '%s' not present in parsed form (missing 'expression' element)", attrs[i]);
           }
           else
           {
               // attribute must be there
               if (!node->getAttribute(attrs[i]) || !(node->getAttribute(attrs[i]))[0])
                   errors->addError(node, "missing attribute '%s'", attrs[i]);
           }
       }
    }
}

void NEDSyntaxValidator::checkDottedNameAttribute(NEDElement *node, const char *attr, bool wildcardsAllowed)
{
    const char *s = node->getAttribute(attr);
    assert(s);
    if (!*s)
        return;
    for (; *s; s++)
        if (!opp_isalnumext(*s) && *s!='_' && *s!='.' && (wildcardsAllowed ? *s!='*' : true))
            {errors->addError(node,"illegal character in %s '%s'", attr, node->getAttribute(attr)); return;}
}

void NEDSyntaxValidator::checkPropertyNameAttribute(NEDElement *node, const char *attr)
{
    const char *s = node->getAttribute(attr);
    assert(s);
    if (!*s)
        return;
    for (; *s; s++)
        if (!opp_isalnumext(*s) && strchr("_-:.", *s)==NULL) // note: same rule as for XML attribute names
            {errors->addError(node,"illegal character in %s '%s'", attr, node->getAttribute(attr)); return;}
}

void NEDSyntaxValidator::checkPropertyIndexAttribute(NEDElement *node, const char *attr)
{
    const char *s = node->getAttribute(attr);
    assert(s);
    if (!*s)
        return;
    for (; *s; s++)
        if (!opp_isalnumext(*s) && strchr("*?{}_-:.", *s)==NULL) // note: same rule as for XML attribute names, plus wildcards
            {errors->addError(node,"illegal character in %s '%s'", attr, node->getAttribute(attr)); return;}
}

bool NEDSyntaxValidator::isWithinSubcomponent(NEDElement *node)
{
    // only returns true if node is within the BODY of a submodule or a connection
    // (i.e. returns *false* for a submodule vector size)
    for (; node!=NULL; node = node->getParent())
    {
        if (node->getTagCode() == NED_PARAMETERS || node->getTagCode() == NED_GATES) {
            int sectionOwnerType = node->getParent()->getTagCode();
            return sectionOwnerType==NED_SUBMODULE || sectionOwnerType==NED_CONNECTION;
        }
    }
    return false;
}

bool NEDSyntaxValidator::isWithinInnerType(NEDElement *node)
{
    for (; node!=NULL; node = node->getParent())
        if (node->getTagCode() == NED_TYPES)
            return true;
    return false;
}

void NEDSyntaxValidator::validateElement(FilesElement *node)
{
}

void NEDSyntaxValidator::validateElement(NedFileElement *node)
{
}

void NEDSyntaxValidator::validateElement(CommentElement *node)
{
    //FIXME revise
}

void NEDSyntaxValidator::validateElement(PackageElement *node)
{
    checkDottedNameAttribute(node, "name", false);
}

void NEDSyntaxValidator::validateElement(ImportElement *node)
{
    checkDottedNameAttribute(node, "import-spec", true);
}

void NEDSyntaxValidator::validateElement(PropertyDeclElement *node)
{
    checkPropertyNameAttribute(node, "name");
}

void NEDSyntaxValidator::validateElement(ExtendsElement *node)
{
    checkDottedNameAttribute(node, "name", false);
}

void NEDSyntaxValidator::validateElement(InterfaceNameElement *node)
{
    checkDottedNameAttribute(node, "name", false);
}

void NEDSyntaxValidator::validateElement(SimpleModuleElement *node)
{
    //FIXME revise
}

void NEDSyntaxValidator::validateElement(ModuleInterfaceElement *node)
{
    //FIXME revise
}

void NEDSyntaxValidator::validateElement(CompoundModuleElement *node)
{
    //FIXME revise
}

void NEDSyntaxValidator::validateElement(ParametersElement *node)
{
    //FIXME revise
}

void NEDSyntaxValidator::validateElement(ParamElement *node)
{
    NEDElement *parent = node->getParent();
    if (parent)
        parent = parent->getParent();

    // param declarations cannot occur in submodules
    if (parent->getTagCode() == NED_SUBMODULE)
    {
        if (node->getType() != NED_PARTYPE_NONE)
            errors->addError(node, "cannot define new parameters within a submodule");
    }

    // check isPattern flag is consistent with 'name' attribute
    bool containsDot = strchr(node->getName(), '.')!=NULL;
    if (!node->getIsPattern() && containsDot)
        errors->addError(node, "parameter names must not contain a dot");
    if (node->getIsPattern() && !containsDot)
        errors->addError(node, "parameter name patterns must contain a dot");

    // type+pattern are not compatible
    if (node->getIsPattern() && node->getType() != NED_PARTYPE_NONE)
        errors->addError(node, "name must be an identifier when defining a new parameter");

    // in module or channel interfaces, one cannot specify parameter values
    if (parent->getTagCode() == NED_MODULE_INTERFACE || parent->getTagCode() == NED_CHANNEL_INTERFACE)
    {
        if (parsedExpressions)
        {
            if (node->getFirstChildWithTag(NED_EXPRESSION)) // && expr->getTarget()=="value"
                errors->addError(node, "cannot specify parameter values within a module interface or or channel interface");
        }
        else
        {
            if (!opp_isempty(node->getValue()))
                errors->addError(node, "cannot specify parameter values within a module interface or or channel interface");
        }
    }
}

void NEDSyntaxValidator::validateElement(PropertyElement *node)
{
    checkPropertyNameAttribute(node, "name");
    checkPropertyIndexAttribute(node, "index");

    // properties cannot occur on submodule or connection parameters/gates.
    // structure: submodule>parameters>parameter>property
    NEDElement *parent = node->getParent();
    if (parent && (parent->getTagCode()==NED_PARAM || parent->getTagCode()==NED_GATE))
    {
        NEDElement *container = parent->getParent()->getParent();
        if (container && (container->getTagCode()==NED_SUBMODULE || container->getTagCode()==NED_CONNECTION))
            errors->addError(node, "cannot modify parameter/gate properties in a submodule or connection");
    }
}

void NEDSyntaxValidator::validateElement(PropertyKeyElement *node)
{
    checkPropertyNameAttribute(node, "name");
}

void NEDSyntaxValidator::validateElement(GatesElement *node)
{
    //FIXME revise
}

void NEDSyntaxValidator::validateElement(TypesElement *node)
{
    // make sure type names are unique
}

void NEDSyntaxValidator::validateElement(GateElement *node)
{
    // param declarations cannot occur in submodules
    if (node->getType() != NED_GATETYPE_NONE)
    {
        NEDElement *parent = node->getParent();
        if (parent)
            parent = parent->getParent();
        if (parent->getTagCode() == NED_SUBMODULE)
            errors->addError(node, "cannot define new gates within a submodule");
    }
}

void NEDSyntaxValidator::validateElement(SubmodulesElement *node)
{
    //FIXME revise
}

void NEDSyntaxValidator::validateElement(SubmoduleElement *node)
{
    //FIXME revise
    const char *expr[] = {"like-expr", "vector-size"};
    bool opt[] = {true, true};
    checkExpressionAttributes(node, expr, opt, 2);

    checkDottedNameAttribute(node, "type", false);
    checkDottedNameAttribute(node, "like-type", false);


//    // if there's a "like", name should be an existing module parameter name
//    if (!opp_isempty(node->getLikeExpr()))
//    {
//        const char *paramName = node->getLikeExpr();
//        NEDElement *compound = node->getParentWithTag(NED_COMPOUND_MODULE);
//        if (!compound)
//            INTERNAL_ERROR0(node,"occurs outside a compound-module");
//        NEDElement *params = compound->getFirstChildWithTag(NED_PARAMETERS);
//        if (!params || params->getFirstChildWithAttribute(NED_PARAM, "name", paramName)==NULL)
//            {errors->addError(node, "compound module has no parameter named '%s'", paramName);return;}
//    }
}

//void NEDSyntaxValidator::validateElement(SubstparamsElement *node)
//{
//    const char *expr[] = {"condition"};
//    bool opt[] = {true};
//    checkExpressionAttributes(node, expr, opt, 1);
//
//    // make sure parameter names are unique
//    checkUniqueness(node, NED_SUBSTPARAM, "name");
//}

// TODO merge into 'parameters'
//void NEDSyntaxValidator::validateElement(SubstparamElement *node)
//{
//    const char *expr[] = {"value"};
//    bool opt[] = {false};
//    checkExpressionAttributes(node, expr, opt, 1);
//}

// TODO merge into 'gates'
//void NEDSyntaxValidator::validateElement(GatesizesElement *node)
//{
//    const char *expr[] = {"condition"};
//    bool opt[] = {true};
//    checkExpressionAttributes(node, expr, opt, 1);
//
//    // make sure gate names are unique
//    checkUniqueness(node, NED_GATESIZE, "name");
//}

// TODO merge into 'gates'
//void NEDSyntaxValidator::validateElement(GatesizeElement *node)
//{
//    const char *expr[] = {"vector-size"};
//    bool opt[] = {true};
//    checkExpressionAttributes(node, expr, opt, 1);
//}

void NEDSyntaxValidator::validateElement(ConnectionsElement *node)
{
    //FIXME revise
    // TBD if check=true, make sure all gates are connected
}

void NEDSyntaxValidator::validateElement(ConnectionElement *node)
{
    //FIXME revise
    const char *expr[] = {"condition", "src-module-index", "src-gate-index", "dest-module-index", "dest-gate-index", "like-expr"};
    bool opt[] = {true, true, true, true, true, true};
    checkExpressionAttributes(node, expr, opt, 6);

    // plusplus and gate index expression cannot be both there
    bool srcGateIx =  node->getFirstChildWithAttribute(NED_EXPRESSION, "target", "src-gate-index")!=NULL;
    bool destGateIx = node->getFirstChildWithAttribute(NED_EXPRESSION, "target", "dest-gate-index")!=NULL;
    if (srcGateIx && node->getSrcGatePlusplus())
        errors->addError(node, "wrong source gate: cannot have both gate index and '++' operator specified");
    if (destGateIx && node->getDestGatePlusplus())
        errors->addError(node, "wrong destination gate: cannot have both gate index and '++' operator specified");

    checkDottedNameAttribute(node, "type", false);
    checkDottedNameAttribute(node, "like-type", false);
}

void NEDSyntaxValidator::validateElement(ChannelInterfaceElement *node)
{
    //FIXME revise
}

void NEDSyntaxValidator::validateElement(ChannelElement *node)
{
    //FIXME revise
}

void NEDSyntaxValidator::validateElement(ConnectionGroupElement *node)
{
    //FIXME revise
    // TODO check loop vars are unique etc
}

void NEDSyntaxValidator::validateElement(LoopElement *node)
{
    //TODO adapt
    //const char *expr[] = {"from-value", "to-value"};
    //bool opt[] = {false, false};
    //checkExpressionAttributes(node, expr, opt, 2);
}

void NEDSyntaxValidator::validateElement(ConditionElement *node)
{
    //FIXME revise
}

void NEDSyntaxValidator::validateElement(ExpressionElement *node)
{
    //FIXME revise
}

void NEDSyntaxValidator::validateElement(OperatorElement *node)
{
    //FIXME revise
    // check operator name is valid and argument count matches
    const char *op = node->getName();

    // next list uses space as separator, so make sure op does not contain space
    if (strchr(op, ' '))
    {
        errors->addError(node, "invalid operator '%s' (contains space)",op);
        return;
    }

    // count arguments
    int args = 0;
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
       args++;

    // unary?
    if (strstr("! ~",op))
    {
         if (args!=1)
            errors->addError(node, "operator '%s' should have 1 operand, not %d", op, args);
    }
    // unary or binary?
    else if (strstr("-",op))
    {
         if (args!=1 && args!=2)
            errors->addError(node, "operator '%s' should have 1 or 2 operands, not %d", op, args);
    }
    // binary?
    else if (strstr("+ * / % ^ == != > >= < <= && || ## & | # << >>",op))
    {
         if (args!=2)
            errors->addError(node, "operator '%s' should have 2 operands, not %d", op, args);
    }
    // tertiary?
    else if (strstr("?:",op))
    {
         if (args!=3)
            errors->addError(node, "operator '%s' should have 3 operands, not %d", op, args);
    }
    else
    {
        errors->addError(node, "invalid operator '%s'",op);
    }
}

void NEDSyntaxValidator::validateElement(FunctionElement *node)
{
    //FIXME revise
    // if we know the function, check argument count
    const char *func = node->getName();
    int args = node->getNumChildren();

    // if it's an operator, treat specially
    if (!strcmp(func,"index"))
    {
         if (args!=0)
             errors->addError(node, "operator 'index' does not take arguments");

         // find expression and submodule node we're under
         NEDElement *parent = node->getParent();
         while (parent && parent->getTagCode()!=NED_EXPRESSION)
             parent = parent->getParent();
         NEDElement *expr = parent;

         while (parent && parent->getTagCode()!=NED_SUBMODULE)
             parent = parent->getParent();
         NEDElement *submod = parent;

         if (!submod || submod->getFirstChildWithAttribute(NED_EXPRESSION, "target", "vector-size")==NULL)
             errors->addError(node, "'index' may only occur in a submodule vector's definition");
         if (expr->getParent()==submod)
             errors->addError(node, "'index' is not allowed here");
         return;
    }
    else if (!strcmp(func,"sizeof"))
    {
         if (args!=1)
             errors->addError(node, "operator 'sizeof' takes one argument");
         //else if (node->getFirstChild()->getTagCode()!=NED_IDENT)
         //    errors->addError(node, "argument of operator 'sizeof' should be an identifier");
         else
         {
             // TBD further check it's an existing parent module gate or submodule name
         }
         return;
    }
    else if (!strcmp(func,"input"))
    {
         if (args>2)
             errors->addError(node, "operator 'input' takes 0, 1 or 2 arguments");
         NEDElement *op1 = node->getFirstChild();
         NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
         if (args==2)
             if (op2->getTagCode()!=NED_LITERAL || ((LiteralElement *)op2)->getType()!=NED_CONST_STRING)
                 errors->addError(node, "second argument to 'input()' must be a string literal (prompt text)");
         NEDElement *parent = node->getParent();
         if (parent->getTagCode()!=NED_EXPRESSION)
             errors->addError(node, "'input()' occurs in wrong place");
         return;
    }
    else if (!strcmp(func,"xmldoc"))
    {
         if (args!=1 && args!=2)
             {errors->addError(node, "'xmldoc()' takes 1 or 2 arguments");return;}
         NEDElement *op1 = node->getFirstChild();
         NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
         if (op1->getTagCode()!=NED_LITERAL || ((LiteralElement *)op1)->getType()!=NED_CONST_STRING ||
             (op2 && (op2->getTagCode()!=NED_LITERAL || ((LiteralElement *)op2)->getType()!=NED_CONST_STRING)))
             errors->addError(node, "'xmldoc()' arguments must be string literals");
         return;
    }

    // check if we know about it
    bool name_found = false;
    bool argc_matches = false;
    for (int i=0; known_funcs[i].fname!=NULL;i++)
    {
        if (!strcmp(func,known_funcs[i].fname))
        {
            name_found = true;
            if (known_funcs[i].args == args)
            {
                argc_matches = true;
                break;
            }
        }
    }
    if (name_found && !argc_matches)
    {
        errors->addError(node, "function '%s' cannot take %d operands", func, args);
    }
}

void NEDSyntaxValidator::validateElement(IdentElement *node)
{
    const char *expr[] = {"module-index", "param-index"};
    bool opt[] = {true, true};
    checkExpressionAttributes(node, expr, opt, 2);

/*TODO:
    bool withinSubcomponent = isWithinSubcomponent(node);
    bool withinInnerType = isWithinInnerType(node);

    const char *modulename = node->getModule();
    if (opp_isempty(modulename) || strcmp(modulename, "this")==0)
    {
        // OK -- "identifier" and "this.identifier" are allowed in expressions anywhere
    }
    else
    {
        // TODO module.ident is not legal at certain places
    }
*/
}

void NEDSyntaxValidator::validateElement(LiteralElement *node)
{
    // verify syntax of constant
    int type = node->getType();
    const char *value = node->getValue();
    //const char *text = node->getText();

    // Note: null value is valid as well, because that represents the "" string literal!
    if (opp_isempty(value)) value="";

    if (type==NED_CONST_BOOL)
    {
        // check bool
        if (strcmp(value,"true") && strcmp(value,"false"))
            errors->addError(node, "bool constant should be 'true' or 'false'");
        // TBD check that if text is present, it's the same as value
    }
    else if (type==NED_CONST_INT)
    {
        // check int
        char *s;
        (void) strtol(value, &s, 0);
        if (s && *s)
            errors->addError(node, "invalid integer constant '%s'", value);
        // TBD check that if text is present, it's the same as value
    }
    else if (type==NED_CONST_DOUBLE)
    {
        // check real
        char *s;
        setlocale(LC_NUMERIC, "C");
        (void) strtod(value, &s);
        if (s && *s)
            errors->addError(node, "invalid real constant '%s'", value);
        // TBD check that if text is present, it's the same as value
    }
    else if (type==NED_CONST_QUANTITY)
    {
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
    else if (type==NED_CONST_STRING)
    {
        // string: no restriction on value
        // TBD check that if text is present, it's a quoted version of the value
    }
}

void NEDSyntaxValidator::validateElement(MsgFileElement *node)
{
}

void NEDSyntaxValidator::validateElement(NamespaceElement *node)
{
}

void NEDSyntaxValidator::validateElement(CplusplusElement *node)
{
}

void NEDSyntaxValidator::validateElement(StructDeclElement *node)
{
}

void NEDSyntaxValidator::validateElement(ClassDeclElement *node)
{
}

void NEDSyntaxValidator::validateElement(MessageDeclElement *node)
{
}

void NEDSyntaxValidator::validateElement(PacketDeclElement *node)
{
}

void NEDSyntaxValidator::validateElement(EnumDeclElement *node)
{
}

void NEDSyntaxValidator::validateElement(EnumElement *node)
{
}

void NEDSyntaxValidator::validateElement(EnumFieldsElement *node)
{
}

void NEDSyntaxValidator::validateElement(EnumFieldElement *node)
{
}

void NEDSyntaxValidator::validateElement(MessageElement *node)
{
}

void NEDSyntaxValidator::validateElement(PacketElement *node)
{
}

void NEDSyntaxValidator::validateElement(ClassElement *node)
{
}

void NEDSyntaxValidator::validateElement(StructElement *node)
{
}

void NEDSyntaxValidator::validateElement(FieldElement *node)
{
    NEDElement *classNode = node->getParent()->getParent();
    bool isStruct = !strcmp(classNode->getTagName(), "struct");

    if (node->getIsAbstract() && isStruct)
          errors->addError(node, "a struct cannot have abstract fields");

    if (node->getIsAbstract() && !opp_isempty(node->getDefaultValue()))
         errors->addError(node, "an abstract field cannot be assigned a default value");

    if (node->getIsVector() && opp_isempty(node->getVectorSize()) && isStruct)
         errors->addError(node, "a struct cannot have dynamic array fields");

    // if (!opp_isempty(node->getDataType())) // type is there
    // {
    //      if (defined in base class too)
    //      {
    //          if (!node->getIsReadonly())
    //              errors->addError(node, "field is already declared in a base class (only readonly fields can be overridden)");
    //          if (node->getIsReadonly() && type is not the same)
    //              errors->addError(node, "field is already declared in a base class with a different type");
    //      }
    // }

    if (opp_isempty(node->getDataType())) // type is missing
    {
         if (node->getIsAbstract())
             errors->addError(node, "an abstract field needs a type");
         if (node->getIsVector())
             errors->addError(node, "cannot set array field of the base class");
         if (opp_isempty(node->getDefaultValue()))
             errors->addError(node, "missing field type");
    }

    // TBD check syntax of default value, and that its type agrees with field type

}

void NEDSyntaxValidator::validateElement(UnknownElement *node)
{
}

NAMESPACE_END

