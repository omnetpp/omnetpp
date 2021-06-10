/*===============================================================
 * File: ned2.y
 *
 *  Grammar for OMNeT++ NED-2.
 *
 *  Author: Andras Varga
 *
 *=============================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 1992,2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

/* number of expected shift-reduce conflicts */
%expect 9

/* Reserved words. Trailing '_' in some token names are there to avoid
 * (potential) conflict with macros defined in system headers.
 */
%token IMPORT PACKAGE PROPERTY
%token MODULE SIMPLE NETWORK CHANNEL MODULEINTERFACE CHANNELINTERFACE
%token EXTENDS LIKE
%token TYPES PARAMETERS GATES SUBMODULES CONNECTIONS ALLOWUNCONNECTED
%token DOUBLE_ INT_ STRING_ BOOL_ OBJECT_ XML_ VOLATILE_
%token INPUT_ OUTPUT_ INOUT_
%token IF FOR
%token RIGHTARROW LEFTARROW DBLARROW TO
%token THIS_ PARENT DEFAULT ASK CONST_ SIZEOF INDEX_ EXISTS TYPENAME XMLDOC
%token TRUE_ FALSE_ NAN_ INF_ UNDEFINED_ NULLPTR_ NULL_

/* Other tokens: identifiers, numeric literals, operators etc */
%token NAME PROPNAME INTCONSTANT REALCONSTANT STRINGCONSTANT CHARCONSTANT
%token PLUSPLUS DOUBLEASTERISK
%token EQ NE GE LE SPACESHIP
%token AND OR XOR
%token SHIFT_LEFT SHIFT_RIGHT
%token DOUBLECOLON

%token EXPRESSION_SELECTOR   /* forces parsing text as a single expression */

%token COMMONCHAR
%token INVALID_CHAR   /* just to generate parse error */

/* Operator precedences (low to high) and associativity */
%right '?' ':'
%left OR
%left XOR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left SPACESHIP
%left MATCH
%left '|'
%left '#'
%left '&'
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-'
%left '*' '/' '%'
%right '^'
%right UMIN_ NEG_ NOT_
%left '.'

%start startsymbol

%parse-param {omnetpp::nedxml::ParseContext *np}

/* requires at least bison 1.50 (tested with bison 2.1) */
%glr-parser

/* A note about parser error recovery. We only add error recovery rules to
 * toplevel elements. Attempting to add such rules to inner nonterminals (param, gate,
 * submodule, etc) has caused erratic behaviour when the synchronizing token
 * (";" or "}") was missing from the input, because bison executed pretty
 * much random rules during recovery. (For example, if a ";" is missing then the
 * parser may eat up everything into the middle of the next compound module where
 * it finally founds a ";" to synchronize on, and starts shifting tokens on the
 * stack from there, and matches them to rules applicable in that context).
 *
 * The actions in this grammar assume that they're being executed in the order
 * prescribed by the grammar, and don't tolerate that rather random behaviour
 * during which bison recovers from various parse errors.
 */

%{

#include <cstdio>
#include <cstdlib>
#include <stack>
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "yydefs.h"
#include "errorstore.h"
#include "sourcedocument.h"
#include "exception.h"
#include "nedelements.h"

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <cstring>         /* YYVERBOSE needs it */
#endif

/* increase GLR stack -- with the default 200 some NED files have reportedly caused a "memory exhausted" error */
#define YYINITDEPTH 500

#define yylloc ned2yylloc
#define yyin ned2yyin
#define yyout ned2yyout
#define yyrestart ned2yyrestart
#define yy_scan_string ned2yy_scan_string
#define yy_delete_buffer ned2yy_delete_buffer
extern FILE *yyin;
extern FILE *yyout;
struct yy_buffer_state;
struct yy_buffer_state *yy_scan_string(const char *str);
void yy_delete_buffer(struct yy_buffer_state *);
void yyrestart(FILE *);
int yylex();
void yyerror (omnetpp::nedxml::ParseContext *np, const char *s);

#include "nedutil.h"
#include "nedyyutil.h"

using namespace omnetpp;

using namespace omnetpp::common;
using namespace omnetpp::nedxml;
using namespace omnetpp::nedxml::nedyyutil;

static struct NedParserState
{
    bool inTypes;
    bool inConnGroup;
    std::stack<ASTNode *> propertyscope; // top(): where to insert properties as we parse them
    std::stack<ASTNode *> blockscope;    // top(): where to insert parameters, gates, etc
    std::stack<ASTNode *> typescope;     // top(): as blockscope, but ignore submodules and connection channels

    /* tmp flags, used with param, gate and conn */
    int paramType;
    int gateType;
    bool isVolatile;
    bool isDefault;
    YYLTYPE exprPos;
    int subgate;
    std::vector<ASTNode *> propvals; // temporarily collects property values

    /* tmp flags, used with msg fields */
    bool isAbstract;
    bool isReadonly;

    /* NED-II: modules, channels */
    NedFileElement *nedfile;
    CommentElement *comment;
    PackageElement *package;
    ImportElement *import;
    PropertyDeclElement *propertydecl;
    ExtendsElement *extends;
    InterfaceNameElement *interfacename;
    ASTNode *component;  // compound/simple module, module interface, channel or channel interface
    ParametersElement *parameters;
    ParamElement *param;
    PropertyElement *property;
    PropertyKeyElement *propkey;
    TypesElement *types;
    GatesElement *gates;
    GateElement *gate;
    SubmodulesElement *submods;
    SubmoduleElement *submod;
    ConnectionsElement *conns;
    ConnectionGroupElement *conngroup;
    ConnectionElement *conn;
    LoopElement *loop;
    ConditionElement *condition;
} ps;


static void resetParserState()
{
    static NedParserState cleanps;
    ps = cleanps;
}

static NedParserState globalps;  // for error recovery

static void restoreGlobalParserState()  // for error recovery
{
    ps = globalps;
}

static void assertNonEmpty(std::stack<ASTNode *>& somescope)
{
    // for error recovery: STL stack::top() crashes if stack is empty
    if (somescope.empty())
    {
        INTERNAL_ERROR0(nullptr, "error during parsing: scope stack empty");
        somescope.push(nullptr);
    }
}

%}

%%

/*
 * Start rule. Support parsing a standalone expression as well
 */
startsymbol
        : EXPRESSION_SELECTOR expression
        | nedfile
        ;

/*
 * Top-level components
 */
nedfile
        : definitions
        | %empty
        ;

definitions
        : definitions definition
        | definition
        ;

definition
        : packagedeclaration
        | import
        | propertydecl
        | fileproperty
        | channeldefinition
        | channelinterfacedefinition
        | simplemoduledefinition
        | compoundmoduledefinition
        | networkdefinition
        | moduleinterfacedefinition
        | ';'

        | channelinterfaceheader error '}'
                { storePos(np,  ps.component, @$); restoreGlobalParserState(); }
        | CHANNELINTERFACE error '}'
                { restoreGlobalParserState(); }
        | simplemoduleheader error '}'
                { storePos(np,  ps.component, @$); restoreGlobalParserState(); }
        | SIMPLE error '}'
                { restoreGlobalParserState(); }
        | compoundmoduleheader error '}'
                { storePos(np,  ps.component, @$); restoreGlobalParserState(); }
        | MODULE error '}'
                { restoreGlobalParserState(); }
        | networkheader error '}'
                { storePos(np,  ps.component, @$); restoreGlobalParserState(); }
        | NETWORK error '}'
                { restoreGlobalParserState(); }
        | moduleinterfaceheader error '}'
                { storePos(np,  ps.component, @$); restoreGlobalParserState(); }
        | MODULEINTERFACE error '}'
                { restoreGlobalParserState(); }
        | channelheader error '}'
                { storePos(np,  ps.component, @$); restoreGlobalParserState(); }
        | CHANNEL error '}'
                { restoreGlobalParserState(); }
        ;

packagedeclaration
        : PACKAGE dottedname ';'
                {
                  ps.package = (PackageElement *)createNedElementWithTag(np, NED_PACKAGE, ps.nedfile);
                  ps.package->setName(removeSpaces(np, @2).c_str());
                  storePos(np,  ps.package,@$);
                  storeBannerAndRightComments(np, ps.package,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

dottedname
        : dottedname '.' NAME
        | NAME
        ;

/*
 * Import
 */
import
        : IMPORT importspec ';'
                {
                  ps.import = (ImportElement *)createNedElementWithTag(np, NED_IMPORT, ps.nedfile);
                  ps.import->setImportSpec(removeSpaces(np, @2).c_str());
                  storePos(np,  ps.import,@$);
                  storeBannerAndRightComments(np, ps.import,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

importspec
        : importspec '.' importname
        | importname
        ;

importname
        : importname NAME
        | importname '*'
        | importname DOUBLEASTERISK
        | NAME
        | '*'
        | DOUBLEASTERISK
        ;

/*
 * Property declaration
 */
propertydecl
        : propertydecl_header opt_inline_properties ';'
                {
                    storePos(np,  ps.propertydecl, @$);
                    storeBannerAndRightComments(np, ps.propertydecl,@$);
                }
        | propertydecl_header '(' opt_propertydecl_keys ')' opt_inline_properties ';'
                {
                    storePos(np,  ps.propertydecl, @$);
                    storeBannerAndRightComments(np, ps.propertydecl,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

propertydecl_header
        : PROPERTY '@' PROPNAME
                {
                  ps.propertydecl = (PropertyDeclElement *)createNedElementWithTag(np, NED_PROPERTY_DECL, ps.nedfile);
                  ps.propertydecl->setName(toString(np, @3));
                }
        | PROPERTY '@' PROPNAME '[' ']'
                {
                  ps.propertydecl = (PropertyDeclElement *)createNedElementWithTag(np, NED_PROPERTY_DECL, ps.nedfile);
                  ps.propertydecl->setName(toString(np, @3));
                  ps.propertydecl->setIsArray(true);
                }
        ;

opt_propertydecl_keys
        : propertydecl_keys
        | %empty
        ;

propertydecl_keys
        : propertydecl_keys ';' propertydecl_key
        | propertydecl_key
        ;

propertydecl_key
        : property_literal
                {
                  ps.propkey = (PropertyKeyElement *)createNedElementWithTag(np, NED_PROPERTY_KEY, ps.propertydecl);
                  ps.propkey->setName(opp_trim(toString(np, @1)).c_str());
                  storePos(np,  ps.propkey, @$);
                }
        ;

/*
 * File Property
 */
fileproperty
        : property_namevalue ';'
                {
                  storePos(np,  ps.property, @$);
                  storeBannerAndRightComments(np, ps.property,@$);
                }
        ;

/*
 * Channel
 */
channeldefinition
        : channelheader '{'
                {
                  ps.typescope.push(ps.component);
                  ps.blockscope.push(ps.component);
                  ps.parameters = (ParametersElement *)createNedElementWithTag(np, NED_PARAMETERS, ps.component);
                  ps.parameters->setIsImplicit(true);
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
          '}'
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.component = ps.typescope.top();
                  ps.typescope.pop();
                  if (np->getStoreSourceFlag())
                      storeComponentSourceCode(np, ps.component, @$);
                  storePos(np,  ps.component, @$);
                  storeTrailingComment(np, ps.component,@$);
                }
        ;

channelheader
        : CHANNEL NAME
                {
                  ps.component = (ChannelElement *)createNedElementWithTag(np, NED_CHANNEL, ps.inTypes ? (ASTNode *)ps.types : (ASTNode *)ps.nedfile);
                  ((ChannelElement *)ps.component)->setName(toString(np, @2));
                }
           opt_inheritance
                { storeBannerAndRightComments(np, ps.component,@$); }
        ;

opt_inheritance
        : %empty
        | EXTENDS extendsname
        | LIKE likenames
        | EXTENDS extendsname LIKE likenames
        ;

extendsname
        : dottedname
                {
                  ps.extends = (ExtendsElement *)createNedElementWithTag(np, NED_EXTENDS, ps.component);
                  ps.extends->setName(removeSpaces(np, @1).c_str());
                  storePos(np,  ps.extends, @$);
                }
        ;

likenames
        : likenames ',' likename
        | likename
        ;

likename
        : dottedname
                {
                  ps.interfacename = (InterfaceNameElement *)createNedElementWithTag(np, NED_INTERFACE_NAME, ps.component);
                  ps.interfacename->setName(removeSpaces(np, @1).c_str());
                  storePos(np, ps.interfacename, @$);
                }
        ;

/*
 * Channel Interface
 */
channelinterfacedefinition
        : channelinterfaceheader '{'
                {
                  ps.typescope.push(ps.component);
                  ps.blockscope.push(ps.component);
                  ps.parameters = (ParametersElement *)createNedElementWithTag(np, NED_PARAMETERS, ps.component);
                  ps.parameters->setIsImplicit(true);
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
          '}'
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.component = ps.typescope.top();
                  ps.typescope.pop();
                  if (np->getStoreSourceFlag())
                      storeComponentSourceCode(np, ps.component, @$);
                  storePos(np, ps.component, @$);
                  storeTrailingComment(np, ps.component,@$);
                }
        ;

channelinterfaceheader
        : CHANNELINTERFACE NAME
                {
                  ps.component = (ChannelInterfaceElement *)createNedElementWithTag(np, NED_CHANNEL_INTERFACE, ps.inTypes ? (ASTNode *)ps.types : (ASTNode *)ps.nedfile);
                  ((ChannelInterfaceElement *)ps.component)->setName(toString(np, @2));
                }
           opt_interfaceinheritance
                { storeBannerAndRightComments(np, ps.component,@$); }
        ;

opt_interfaceinheritance
        : EXTENDS extendsnames
        | %empty
        ;

extendsnames
        : extendsnames ',' extendsname
        | extendsname
        ;

/*
 * Simple module
 */
simplemoduledefinition
        : simplemoduleheader '{'
                {
                  ps.typescope.push(ps.component);
                  ps.blockscope.push(ps.component);
                  ps.parameters = (ParametersElement *)createNedElementWithTag(np, NED_PARAMETERS, ps.component);
                  ps.parameters->setIsImplicit(true);
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
            opt_gateblock
          '}'
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.component = ps.typescope.top();
                  ps.typescope.pop();
                  if (np->getStoreSourceFlag())
                      storeComponentSourceCode(np, ps.component, @$);
                  storePos(np, ps.component, @$);
                  storeTrailingComment(np, ps.component,@$);
                }
        ;

simplemoduleheader
        : SIMPLE NAME
                {
                  ps.component = (SimpleModuleElement *)createNedElementWithTag(np, NED_SIMPLE_MODULE, ps.inTypes ? (ASTNode *)ps.types : (ASTNode *)ps.nedfile );
                  ((SimpleModuleElement *)ps.component)->setName(toString(np, @2));
                }
          opt_inheritance
                { storeBannerAndRightComments(np, ps.component,@$); }
        ;

/*
 * Module
 */
compoundmoduledefinition
        : compoundmoduleheader '{'
                {
                  ps.typescope.push(ps.component);
                  ps.blockscope.push(ps.component);
                  ps.parameters = (ParametersElement *)createNedElementWithTag(np, NED_PARAMETERS, ps.component);
                  ps.parameters->setIsImplicit(true);
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
            opt_gateblock
            opt_typeblock
            opt_submodblock
            opt_connblock
          '}'
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.component = ps.typescope.top();
                  ps.typescope.pop();
                  if (np->getStoreSourceFlag())
                      storeComponentSourceCode(np, ps.component, @$);
                  storePos(np, ps.component, @$);
                  storeTrailingComment(np, ps.component,@$);
                }
        ;

compoundmoduleheader
        : MODULE NAME
                {
                  ps.component = (CompoundModuleElement *)createNedElementWithTag(np, NED_COMPOUND_MODULE, ps.inTypes ? (ASTNode *)ps.types : (ASTNode *)ps.nedfile );
                  ((CompoundModuleElement *)ps.component)->setName(toString(np, @2));
                }
          opt_inheritance
                { storeBannerAndRightComments(np, ps.component,@$); }
        ;

/*
 * Network
 */
networkdefinition
        : networkheader '{'
                {
                  ps.typescope.push(ps.component);
                  ps.blockscope.push(ps.component);
                  ps.parameters = (ParametersElement *)ps.component->getFirstChildWithTag(NED_PARAMETERS); // networkheader already created it for @isNetwork
                  ps.parameters->setIsImplicit(true);
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
            opt_gateblock
            opt_typeblock
            opt_submodblock
            opt_connblock
          '}'
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.component = ps.typescope.top();
                  ps.typescope.pop();
                  if (np->getStoreSourceFlag())
                      storeComponentSourceCode(np, ps.component, @$);
                  storePos(np, ps.component, @$);
                  storeTrailingComment(np, ps.component,@$);
                }
        ;

networkheader
        : NETWORK NAME
                {
                  ps.component = (CompoundModuleElement *)createNedElementWithTag(np, NED_COMPOUND_MODULE, ps.inTypes ? (ASTNode *)ps.types : (ASTNode *)ps.nedfile );
                  ((CompoundModuleElement *)ps.component)->setName(toString(np, @2));
                }
          opt_inheritance
                {
                  setIsNetworkProperty(np, ps.component);
                  storeBannerAndRightComments(np, ps.component,@$);
                }
        ;

/*
 * Module Interface
 */
moduleinterfacedefinition
        : moduleinterfaceheader '{'
                {
                  ps.typescope.push(ps.component);
                  ps.blockscope.push(ps.component);
                  ps.parameters = (ParametersElement *)createNedElementWithTag(np, NED_PARAMETERS, ps.component);
                  ps.parameters->setIsImplicit(true);
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
            opt_gateblock
          '}'
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.component = ps.typescope.top();
                  ps.typescope.pop();
                  if (np->getStoreSourceFlag())
                      storeComponentSourceCode(np, ps.component, @$);
                  storePos(np, ps.component, @$);
                  storeTrailingComment(np, ps.component,@$);
                }
        ;

moduleinterfaceheader
        : MODULEINTERFACE NAME
                {
                  ps.component = (ModuleInterfaceElement *)createNedElementWithTag(np, NED_MODULE_INTERFACE, ps.inTypes ? (ASTNode *)ps.types : (ASTNode *)ps.nedfile);
                  ((ModuleInterfaceElement *)ps.component)->setName(toString(np, @2));
                }
           opt_interfaceinheritance
                { storeBannerAndRightComments(np, ps.component,@$); }
        ;

/*
 * Parameters
 */
opt_paramblock
        : opt_params   /* "parameters" keyword is optional */
                {
                  storePos(np, ps.parameters, @$);
                  if (!ps.parameters->getFirstChild()) { // delete "parameters" element if empty
                      ps.parameters->getParent()->removeChild(ps.parameters);
                      delete ps.parameters;
                  }
                }
        | PARAMETERS ':'
                {
                  ps.parameters->setIsImplicit(false);
                  storeBannerAndRightComments(np, ps.parameters,@1,@2);
                }
          opt_params
                { storePos(np, ps.parameters, @$); }
        ;

opt_params
        : params
        | %empty
        ;

params
        : params paramsitem
        | paramsitem
        ;

paramsitem
        : param
        | property
        ;

param
        : param_typenamevalue
        | pattern_value
        ; /* no error recovery rule -- see discussion at top */

/*
 * Parameter
 */
param_typenamevalue
        : param_typename opt_inline_properties ';'
                {
                  ps.propertyscope.pop();
                  storePos(np, ps.param, @$);
                  storeBannerAndRightComments(np, ps.param,@$);
                }
        | param_typename opt_inline_properties '=' paramvalue opt_inline_properties ';'
                {
                  ps.propertyscope.pop();
                  if (!isEmpty(ps.exprPos))  // note: $4 cannot be checked, as it's always nullptr when expression parsing is off
                      addExpression(np, ps.param, "value",ps.exprPos,$4);
                  else {
                      // Note: "=default" is currently not accepted in NED files, because
                      // it would be complicated to support in the Inifile Editor.
                      if (ps.isDefault)
                          np->getErrors()->addError(ps.param,"applying the default value (\"=default\" syntax) is not supported in NED files");
                  }
                  ps.param->setIsDefault(ps.isDefault);
                  storePos(np, ps.param, @$);
                  storeBannerAndRightComments(np, ps.param,@$);
                }
        ;

param_typename
        : opt_volatile paramtype NAME
                {
                  ps.param = addParameter(np, ps.parameters, @3);
                  ps.param->setType(ps.paramType);
                  ps.param->setIsVolatile(ps.isVolatile);
                  ps.propertyscope.push(ps.param);
                }
        | NAME
                {
                  ps.param = addParameter(np, ps.parameters, @1);
                  ps.propertyscope.push(ps.param);
                }
        ;

pattern_value
        : pattern '=' paramvalue ';'
                {
                  ps.param = addParameter(np, ps.parameters, @1);
                  ps.param->setIsPattern(true);
                  const char *patt = ps.param->getName();
                  if (strchr(patt,' ') || strchr(patt,'\t') || strchr(patt,'\n'))
                      np->getErrors()->addError(ps.param,"parameter name patterns may not contain whitespace");
                  if (!isEmpty(ps.exprPos))  // note: $3 cannot be checked, as it's always nullptr when expression parsing is off
                      addExpression(np, ps.param, "value",ps.exprPos,$3);
                  else {
                      // Note: "=default" is currently not accepted in NED files, because
                      // it would be complicated to support in the Inifile Editor.
                      if (ps.isDefault)
                          np->getErrors()->addError(ps.param,"applying the default value (\"=default\" syntax) is not supported in NED files");
                  }
                  ps.param->setIsDefault(ps.isDefault);
                  storePos(np, ps.param, @$);
                  storeBannerAndRightComments(np, ps.param,@$);
                }
        ;

paramtype
        : DOUBLE_
                { ps.paramType = PARTYPE_DOUBLE; }
        | INT_
                { ps.paramType = PARTYPE_INT; }
        | STRING_
                { ps.paramType = PARTYPE_STRING; }
        | BOOL_
                { ps.paramType = PARTYPE_BOOL; }
        | OBJECT_
                { ps.paramType = PARTYPE_OBJECT; }
        | XML_
                { ps.paramType = PARTYPE_XML; }
        ;

opt_volatile
        : VOLATILE_
                { ps.isVolatile = true; }
        | %empty
                { ps.isVolatile = false; }
        ;

paramvalue
        : expression
                { $$ = $1; ps.exprPos = @1; ps.isDefault = false; }
        | DEFAULT '(' expression ')'
                { $$ = $3; ps.exprPos = @3; ps.isDefault = true; }
        | DEFAULT
                {
                  $$ = nullptr; ps.exprPos = makeEmptyYYLoc(); ps.isDefault = true;
                }
        | ASK
                {
                  np->getErrors()->addError(ps.parameters,"interactive prompting (\"=ask\" syntax) is not supported in NED files");
                  $$ = nullptr; ps.exprPos = makeEmptyYYLoc(); ps.isDefault = false;
                }
        ;

opt_inline_properties
        : inline_properties
        | %empty
        ;

inline_properties
        : inline_properties property_namevalue
        | property_namevalue
        ;

pattern
        : pattern2 '.' pattern_elem  /* to ensure that pattern contains at least one '.' */
        | pattern2 '.' TYPENAME
        ;

pattern2
        : pattern2 '.' pattern_elem
        | pattern_elem
        ;

pattern_elem
        : pattern_name
        | pattern_name '[' pattern_index ']'
        | pattern_name '[' '*' ']'
        | DOUBLEASTERISK
        ;

pattern_name
        : NAME
        | NAME '$' NAME  /* subgate */
        | CHANNEL    /* needed for setting channel parameters */
        | '{' pattern_index '}'
        | '*'
        | pattern_name NAME
        | pattern_name '{' pattern_index '}'
        | pattern_name '*'
        ;

pattern_index
        : INTCONSTANT
        | INTCONSTANT TO INTCONSTANT
        | TO INTCONSTANT
        | INTCONSTANT TO
        ;

/*
 * Property
 */
property
        : property_namevalue ';'
                {
                  storePos(np, ps.property, @$);
                  storeBannerAndRightComments(np, ps.property,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

property_namevalue
        : property_name
        | property_name '(' opt_property_keys ')'
        ;

property_name
        : '@' PROPNAME
                {
                  assertNonEmpty(ps.propertyscope);
                  ps.property = addProperty(np, ps.propertyscope.top(), toString(np, @2));
                  ps.propvals.clear(); // just to be safe
                }
        | '@' PROPNAME '[' PROPNAME ']'
                {
                  assertNonEmpty(ps.propertyscope);
                  ps.property = addProperty(np, ps.propertyscope.top(), toString(np, @2));
                  ps.property->setIndex(toString(np, @4));
                  ps.propvals.clear(); // just to be safe
                }
        ;

opt_property_keys
        : property_keys  /* can't allow epsilon rule here, because @foo() would result in "ambiguous syntax" :( */
        ;

property_keys
        : property_keys ';' property_key
        | property_key
        ;

property_key
        : property_literal '=' property_values
                {
                  ps.propkey = (PropertyKeyElement *)createNedElementWithTag(np, NED_PROPERTY_KEY, ps.property);
                  ps.propkey->setName(opp_trim(toString(np, @1)).c_str());
                  for (int i=0; i<(int)ps.propvals.size(); i++)
                      ps.propkey->appendChild(ps.propvals[i]);
                  ps.propvals.clear();
                  storePos(np, ps.propkey, @$);
                }
        | property_values
                {
                  ps.propkey = (PropertyKeyElement *)createNedElementWithTag(np, NED_PROPERTY_KEY, ps.property);
                  ps.propkey->appendChild($1);
                  for (int i=0; i<(int)ps.propvals.size(); i++)
                      ps.propkey->appendChild(ps.propvals[i]);
                  ps.propvals.clear();
                  storePos(np, ps.propkey, @$);
                }
        ;

property_values
        : property_values ',' property_value
                { ps.propvals.push_back($3); }
        | property_value
                { ps.propvals.push_back($1); }
        ;

property_value
        : property_literal
                {
                  $$ = createPropertyValue(np, @$);
                }
        | %empty
                {
                  LiteralElement *node = (LiteralElement *)createNedElementWithTag(np, NED_LITERAL);
                  node->setType(LIT_SPEC); // and leave both value and text at ""
                  $$ = node;
                }
        ;

property_literal
        : property_literal COMMONCHAR
        | property_literal STRINGCONSTANT
        | COMMONCHAR
        | STRINGCONSTANT
        ;

/*
 * Gates
 */
opt_gateblock
        : gateblock
        | %empty
        ;

gateblock
        : GATES ':'
                {
                  assertNonEmpty(ps.blockscope);
                  ps.gates = (GatesElement *)createNedElementWithTag(np, NED_GATES, ps.blockscope.top());
                  storeBannerAndRightComments(np, ps.gates,@1,@2);
                }
          opt_gates
                {
                  storePos(np, ps.gates, @$);
                }
        ;

opt_gates
        : gates
        | %empty
        ;

gates
        : gates gate
                {
                  storeBannerAndRightComments(np, ps.gate,@2);
                }
        | gate
                {
                  storeBannerAndRightComments(np, ps.gate,@1);
                }
        ;

/*
 * Gate
 */
gate
        : gate_typenamesize
                {
                  ps.propertyscope.push(ps.gate);
                }
          opt_inline_properties ';'
                {
                  ps.propertyscope.pop();
                  storePos(np, ps.gate, @$);
                }
        ; /* no error recovery rule -- see discussion at top */

gate_typenamesize
        : gatetype NAME
                {
                  ps.gate = addGate(np, ps.gates, @2);
                  ps.gate->setType(ps.gateType);
                }
        | gatetype NAME '[' ']'
                {
                  ps.gate = addGate(np, ps.gates, @2);
                  ps.gate->setType(ps.gateType);
                  ps.gate->setIsVector(true);
                }
        | gatetype NAME vector
                {
                  ps.gate = addGate(np, ps.gates, @2);
                  ps.gate->setType(ps.gateType);
                  ps.gate->setIsVector(true);
                  addExpression(np, ps.gate, "vector-size",ps.exprPos,$3);
                }
        | NAME
                {
                  ps.gate = addGate(np, ps.gates, @1);
                }
        | NAME '[' ']'
                {
                  ps.gate = addGate(np, ps.gates, @1);
                  ps.gate->setIsVector(true);
                }
        | NAME vector
                {
                  ps.gate = addGate(np, ps.gates, @1);
                  ps.gate->setIsVector(true);
                  addExpression(np, ps.gate, "vector-size",ps.exprPos,$2);
                }
        ;

gatetype
        : INPUT_
                { ps.gateType = GATETYPE_INPUT; }
        | OUTPUT_
                { ps.gateType = GATETYPE_OUTPUT; }
        | INOUT_
                { ps.gateType = GATETYPE_INOUT; }
        ;

/*
 * Local Types
 */
opt_typeblock
        : typeblock
        | %empty
        ;

typeblock
        : TYPES ':'
                {
                  assertNonEmpty(ps.blockscope);
                  ps.types = (TypesElement *)createNedElementWithTag(np, NED_TYPES, ps.blockscope.top());
                  storeBannerAndRightComments(np, ps.types,@1,@2);
                  if (ps.inTypes)
                     np->getErrors()->addError(ps.types,"more than one level of type nesting is not allowed");
                  ps.inTypes = true;
                }
           opt_localtypes
                {
                  ps.inTypes = false;
                  storePos(np, ps.types, @$);
                }
        ;

opt_localtypes
        : localtypes
        | %empty
        ;

localtypes
        : localtypes localtype
        | localtype
        ;

localtype
        : propertydecl
        | channeldefinition
        | channelinterfacedefinition
        | simplemoduledefinition
        | compoundmoduledefinition
        | networkdefinition
        | moduleinterfacedefinition
        | ';'
        ;

/*
 * Submodules
 */
opt_submodblock
        : submodblock
        | %empty
        ;

submodblock
        : SUBMODULES ':'
                {
                  assertNonEmpty(ps.blockscope);
                  ps.submods = (SubmodulesElement *)createNedElementWithTag(np, NED_SUBMODULES, ps.blockscope.top());
                  storeBannerAndRightComments(np, ps.submods,@1,@2);
                }
          opt_submodules
                {
                  storePos(np, ps.submods, @$);
                }
        ;

opt_submodules
        : submodules
        | %empty
        ;

submodules
        : submodules submodule
        | submodule
        ;

submodule
        : submoduleheader ';'
                {
                  storeBannerAndRightComments(np, ps.submod,@1,@2);
                  storePos(np, ps.submod, @$);
                }
        | submoduleheader '{'
                {
                  ps.blockscope.push(ps.submod);
                  ps.parameters = (ParametersElement *)createNedElementWithTag(np, NED_PARAMETERS, ps.submod);
                  ps.parameters->setIsImplicit(true);
                  ps.propertyscope.push(ps.parameters);
                  storeBannerAndRightComments(np, ps.submod,@1,@2);
                }
          opt_paramblock
          opt_gateblock
          '}' opt_semicolon
                {
                  ps.blockscope.pop();
                  ps.propertyscope.pop();
                  storePos(np, ps.submod, @$);
                  storeTrailingComment(np, ps.submod,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

submoduleheader
        : submodulename ':' dottedname opt_condition
                {
                  ps.submod->setType(removeSpaces(np, @3).c_str());
                  if ($4) ps.submod->appendChild($4);
                }
        | submodulename ':' likeexpr LIKE dottedname opt_condition
                {
                  addOptionalExpression(np, ps.submod, "like-expr", ps.exprPos, $3);
                  ps.submod->setLikeType(removeSpaces(np, @5).c_str());
                  ps.submod->setIsDefault(ps.isDefault);
                  if ($6) ps.submod->appendChild($6);
                }
        ;

submodulename
        : NAME
                {
                  ps.submod = (SubmoduleElement *)createNedElementWithTag(np, NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString(np, @1));
                }
        |  NAME vector
                {
                  ps.submod = (SubmoduleElement *)createNedElementWithTag(np, NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString(np, @1));
                  addExpression(np, ps.submod, "vector-size",ps.exprPos,$2);
                }
        ;

likeexpr
        : '<' '>'
                { $$ = nullptr; ps.exprPos = makeEmptyYYLoc(); ps.isDefault = false; }
        | '<' expression '>' /* XXX this expression is the source of one shift-reduce conflict because it may contain '>' */
                { $$ = $2; ps.exprPos = @2; ps.isDefault = false; }
        | '<' DEFAULT '(' expression ')' '>'
                { $$ = $4; ps.exprPos = @4; ps.isDefault = true; }
        ;

opt_condition
        : condition
                { $$ = $1; }
        | %empty
                { $$ = nullptr; }
        ;

/*
 * Connections
 */
opt_connblock
        : connblock
        | %empty
        ;

connblock
        : CONNECTIONS ALLOWUNCONNECTED ':'
                {
                  assertNonEmpty(ps.blockscope);
                  ps.conns = (ConnectionsElement *)createNedElementWithTag(np, NED_CONNECTIONS, ps.blockscope.top());
                  ps.conns->setAllowUnconnected(true);
                  storeBannerAndRightComments(np, ps.conns,@1,@3);
                }
          opt_connections
                {
                  storePos(np, ps.conns, @$);
                }
        | CONNECTIONS ':'
                {
                  assertNonEmpty(ps.blockscope);
                  ps.conns = (ConnectionsElement *)createNedElementWithTag(np, NED_CONNECTIONS, ps.blockscope.top());
                  storeBannerAndRightComments(np, ps.conns,@1,@2);
                }
          opt_connections
                {
                  storePos(np, ps.conns, @$);
                }
        ;

opt_connections
        : connections
        | %empty
        ;

connections
        : connections connectionsitem
        | connectionsitem
        ;

connectionsitem
        : connectiongroup
        | connection opt_loops_and_conditions ';'
                {
                  if ($2) {
                      transferChildren($2, ps.conn);
                      delete $2;
                  }
                  storePos(np, ps.conn, @$);
                  storeBannerAndRightComments(np, ps.conn,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

connectiongroup
        : opt_loops_and_conditions '{'
                {
                  if (ps.inConnGroup)
                      np->getErrors()->addError(ps.conngroup,"nested connection groups are not allowed");
                  ps.conngroup = (ConnectionGroupElement *)createNedElementWithTag(np, NED_CONNECTION_GROUP, ps.conns);
                  if ($1) {
                      // for's and if's were collected in a temporary UnknownElement, put them under conngroup now
                      transferChildren($1, ps.conngroup);
                      delete $1;
                  }
                  ps.inConnGroup = true;
                  storeBannerAndRightComments(np, ps.conngroup,@1);
                }
          connections '}' opt_semicolon
                {
                  ps.inConnGroup = false;
                  storePos(np, ps.conngroup,@$);
                  storeTrailingComment(np, ps.conngroup,@$);
                }
        ;

opt_loops_and_conditions
        : loops_and_conditions
                { $$ = $1; }
        | %empty
                { $$ = nullptr; }
        ;

loops_and_conditions
        : loops_and_conditions ',' loop_or_condition
                {
                  $1->appendChild($3);
                  $$ = $1;
                }
        | loop_or_condition
                {
                  $$ = new UnknownElement();
                  $$->appendChild($1);
                }
        ;

loop_or_condition
        : loop
        | condition
        ;

loop
        : FOR NAME '=' expression TO expression
                {
                  ps.loop = (LoopElement *)createNedElementWithTag(np, NED_LOOP);
                  ps.loop->setParamName( toString(np, @2) );
                  addExpression(np, ps.loop, "from-value",@4,$4);
                  addExpression(np, ps.loop, "to-value",@6,$6);
                  storePos(np, ps.loop, @$);
                  $$ = ps.loop;
                }
        ;

/*
 * Connection
 */
connection
        : leftgatespec RIGHTARROW rightgatespec
                {
                  ps.conn->setIsBidirectional(false);
                  ps.conn->setIsForwardArrow(true);
                }
        | leftgatespec RIGHTARROW channelspec RIGHTARROW rightgatespec
                {
                  ps.conn->setIsBidirectional(false);
                  ps.conn->setIsForwardArrow(true);
                }
        | leftgatespec LEFTARROW rightgatespec
                {
                  swapConnection(ps.conn);
                  ps.conn->setIsBidirectional(false);
                  ps.conn->setIsForwardArrow(false);
                }
        | leftgatespec LEFTARROW channelspec LEFTARROW rightgatespec
                {
                  swapConnection(ps.conn);
                  ps.conn->setIsBidirectional(false);
                  ps.conn->setIsForwardArrow(false);
                }
        | leftgatespec DBLARROW rightgatespec
                {
                  ps.conn->setIsBidirectional(true);
                }
        | leftgatespec DBLARROW channelspec DBLARROW rightgatespec
                {
                  ps.conn->setIsBidirectional(true);
                }
        ;

leftgatespec
        : leftmod '.' leftgate
        | parentleftgate
        ;

leftmod
        : NAME vector
                {
                  ps.conn = (ConnectionElement *)createNedElementWithTag(np, NED_CONNECTION, ps.inConnGroup ? (ASTNode*)ps.conngroup : (ASTNode*)ps.conns );
                  ps.conn->setSrcModule( toString(np, @1) );
                  addExpression(np, ps.conn, "src-module-index",ps.exprPos,$2);
                }
        | NAME
                {
                  ps.conn = (ConnectionElement *)createNedElementWithTag(np, NED_CONNECTION, ps.inConnGroup ? (ASTNode*)ps.conngroup : (ASTNode*)ps.conns );
                  ps.conn->setSrcModule( toString(np, @1) );
                }
        ;

leftgate
        : NAME opt_subgate
                {
                  ps.conn->setSrcGate( toString(np,  @1) );
                  ps.conn->setSrcGateSubg(ps.subgate);
                }
        | NAME opt_subgate vector
                {
                  ps.conn->setSrcGate( toString(np,  @1) );
                  ps.conn->setSrcGateSubg(ps.subgate);
                  addExpression(np, ps.conn, "src-gate-index",ps.exprPos,$3);
                }
        | NAME opt_subgate PLUSPLUS
                {
                  ps.conn->setSrcGate( toString(np,  @1) );
                  ps.conn->setSrcGateSubg(ps.subgate);
                  ps.conn->setSrcGatePlusplus(true);
                }
        ;

parentleftgate
        : NAME opt_subgate
                {
                  ps.conn = (ConnectionElement *)createNedElementWithTag(np, NED_CONNECTION, ps.inConnGroup ? (ASTNode*)ps.conngroup : (ASTNode*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(np, @1));
                  ps.conn->setSrcGateSubg(ps.subgate);
                }
        | NAME opt_subgate vector
                {
                  ps.conn = (ConnectionElement *)createNedElementWithTag(np, NED_CONNECTION, ps.inConnGroup ? (ASTNode*)ps.conngroup : (ASTNode*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(np, @1));
                  ps.conn->setSrcGateSubg(ps.subgate);
                  addExpression(np, ps.conn, "src-gate-index",ps.exprPos,$3);
                }
        | NAME opt_subgate PLUSPLUS
                {
                  ps.conn = (ConnectionElement *)createNedElementWithTag(np, NED_CONNECTION, ps.inConnGroup ? (ASTNode*)ps.conngroup : (ASTNode*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(np, @1));
                  ps.conn->setSrcGateSubg(ps.subgate);
                  ps.conn->setSrcGatePlusplus(true);
                }
        ;

rightgatespec
        : rightmod '.' rightgate
        | parentrightgate
        ;

rightmod
        : NAME
                {
                  ps.conn->setDestModule( toString(np, @1) );
                }
        | NAME vector
                {
                  ps.conn->setDestModule( toString(np, @1) );
                  addExpression(np, ps.conn, "dest-module-index",ps.exprPos,$2);
                }
        ;

rightgate
        : NAME opt_subgate
                {
                  ps.conn->setDestGate( toString(np,  @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                }
        | NAME opt_subgate vector
                {
                  ps.conn->setDestGate( toString(np,  @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                  addExpression(np, ps.conn, "dest-gate-index",ps.exprPos,$3);
                }
        | NAME opt_subgate PLUSPLUS
                {
                  ps.conn->setDestGate( toString(np,  @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                  ps.conn->setDestGatePlusplus(true);
                }
        ;

parentrightgate
        : NAME opt_subgate
                {
                  ps.conn->setDestGate( toString(np,  @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                }
        | NAME opt_subgate vector
                {
                  ps.conn->setDestGate( toString(np,  @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                  addExpression(np, ps.conn, "dest-gate-index",ps.exprPos,$3);
                }
        | NAME opt_subgate PLUSPLUS
                {
                  ps.conn->setDestGate( toString(np,  @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                  ps.conn->setDestGatePlusplus(true);
                }
        ;

opt_subgate
        : '$' NAME
                {
                  const char *s = toString(np, @2);
                  if (!strcmp(s,"i"))
                      ps.subgate = SUBGATE_I;
                  else if (!strcmp(s,"o"))
                      ps.subgate = SUBGATE_O;
                  else
                       np->getErrors()->addError(currentLocation(np), "invalid subgate spec '%s', must be 'i' or 'o'", toString(np, @2));
                }
        | %empty
                { ps.subgate = SUBGATE_NONE; }
        ;

channelspec
        : channelspec_header
        | channelspec_header '{'
                {
                  ps.parameters = (ParametersElement *)createNedElementWithTag(np, NED_PARAMETERS, ps.conn);
                  ps.parameters->setIsImplicit(true);
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
          '}'
                {
                  ps.propertyscope.pop();
                }
        ;


channelspec_header
        : opt_channelname
        | opt_channelname dottedname
                {
                  ps.conn->setType(removeSpaces(np, @2).c_str());
                }
        | opt_channelname likeexpr LIKE dottedname
                {
                  addOptionalExpression(np, ps.conn, "like-expr", ps.exprPos, $2);
                  ps.conn->setLikeType(removeSpaces(np, @4).c_str());
                  ps.conn->setIsDefault(ps.isDefault);
                }
        ;

opt_channelname
        : %empty
        | NAME ':'
                { ps.conn->setName(toString(np, @1)); }
        ;

/*
 * Condition
 */
condition
        : IF expression
                {
                  ps.condition = (ConditionElement *)createNedElementWithTag(np, NED_CONDITION);
                  addExpression(np, ps.condition, "condition",@2,$2);
                  storePos(np, ps.condition, @$);
                  $$ = ps.condition;
                }
        ;

/*
 * Common part
 */
vector
        : '[' expression ']'
                { $$ = $2; ps.exprPos = @2; }
        ;

expression
        : expr
        ;

/*
 * Expressions
 */
expr
        : simple_expr
        | functioncall
        | expr '.' functioncall
        | object
        | array
        | '(' expr ')'

        | expr '+' expr
        | expr '-' expr
        | expr '*' expr
        | expr '/' expr
        | expr '%' expr
        | expr '^' expr
        | '-' expr
                %prec UMIN_

        | expr EQ expr
        | expr NE expr
        | expr '>' expr
        | expr GE expr
        | expr '<' expr
        | expr LE expr
        | expr SPACESHIP expr
        | expr MATCH expr

        | expr AND expr
        | expr OR expr
        | expr XOR expr

        | '!' expr
                %prec NOT_

        | expr '&' expr
        | expr '|' expr
        | expr '#' expr

        | '~' expr
                %prec NEG_
        | expr SHIFT_LEFT expr
        | expr SHIFT_RIGHT expr
        | expr '?' expr ':' expr
        ;

functioncall
        : funcname '(' opt_exprlist ')'
        ;

array
        : '[' ']'
        | '[' exprlist ']'
        | '[' exprlist ',' ']'
        ;

object
        : '{' opt_keyvaluelist '}'
        | NAME '{' opt_keyvaluelist '}'
        | NAME DOUBLECOLON NAME '{' opt_keyvaluelist '}'
        | NAME DOUBLECOLON NAME DOUBLECOLON NAME '{' opt_keyvaluelist '}' /* note: doing it properly with a 'qname' rule introduces a spurious reduce/reduce conflict (???) */
        | NAME DOUBLECOLON NAME DOUBLECOLON NAME DOUBLECOLON NAME '{' opt_keyvaluelist '}'
        ;

opt_exprlist  /* note: do not move optional final ',' into this rule, as it is also used as function arg lists */
        : exprlist
        | %empty
        ;

exprlist
        : exprlist ',' expr
        | expr
        ;

opt_keyvaluelist
        : keyvaluelist
        | keyvaluelist ','
        | %empty
        ;

keyvaluelist
        : keyvaluelist ',' keyvalue
        | keyvalue
        ;

keyvalue
        : key ':' expr
        ;

key
        : STRINGCONSTANT
        | NAME
        | INTCONSTANT
        | REALCONSTANT
        | quantity
        | '-' INTCONSTANT
        | '-' REALCONSTANT
        | '-' quantity
        | NAN_
        | INF_
        | '-' INF_
        | TRUE_
        | FALSE_
        | NULL_
        | NULLPTR_
        ;

simple_expr
        : qname
        | operator
        | literal
        ;

funcname
        : NAME
        | BOOL_
        | INT_
        | DOUBLE_
        | STRING_
        | OBJECT_
        | XML_
        | XMLDOC
        ;

qname_elem
        : NAME
        | NAME '[' expr ']'
        | THIS_
        | PARENT
        ;

qname
        : qname '.' qname_elem
        | qname_elem
        ;

operator
        : INDEX_
        | TYPENAME
        | qname '.' INDEX_
        | qname '.' TYPENAME
        | EXISTS '(' qname ')'
        | SIZEOF '(' qname ')'
        ;

literal
        : stringliteral
        | boolliteral
        | numliteral
        | otherliteral
        ;

stringliteral
        : STRINGCONSTANT
        ;

boolliteral
        : TRUE_
        | FALSE_
        ;

numliteral
        : INTCONSTANT
        | realconstant_ext
        | quantity
        ;

otherliteral
        : UNDEFINED_
        | NULLPTR_
        | NULL_
        ;

quantity
        : quantity INTCONSTANT NAME
        | quantity realconstant_ext NAME
        | INTCONSTANT NAME
        | realconstant_ext NAME
        ;

realconstant_ext
        : REALCONSTANT
        | INF_
        | NAN_
        ;

opt_semicolon
        : ';'
        | %empty
        ;

%%

//----------------------------------------------------------------------
// general bison/flex stuff:
//
int ned2yylex_destroy();  // from lex.XXX.cc file

ASTNode *doParseNed(ParseContext *np, const char *nedtext)
{
#if YYDEBUG != 0      /* #if added --VA */
    yydebug = YYDEBUGGING_ON;
#endif

    ned2yylex_destroy();

    DETECT_PARSER_REENTRY();

    // reset the lexer
    pos.co = 0;
    pos.li = 1;
    prevpos = pos;

    yyin = nullptr;
    yyout = stderr; // not used anyway

    // alloc buffer
    struct yy_buffer_state *handle = yy_scan_string(nedtext);
    if (!handle)
        {np->getErrors()->addError("", "unable to allocate work memory"); return nullptr;}

    // create parser state and NedFileElement
    resetParserState();
    ps.nedfile = new NedFileElement();

    // store file name with slashes always, even on Windows -- neddoc relies on that
    ps.nedfile->setFilename(slashifyFilename(np->getFileName()).c_str());
    ps.nedfile->setVersion("2");

    // storing the start and end position of the whole file for the NedFileElement
    // NOTE: we cannot use storePos() because it strips off the leading spaces
    // and comments from the element.
    YYLTYPE pos = np->getSource()->getFullTextPos();
    SourceRegion region;
    region.startLine = pos.first_line;
    region.startColumn = pos.first_column;
    region.endLine = pos.last_line;
    region.endColumn = pos.last_column;
    ps.nedfile->setSourceRegion(region);

    // store file comment
    storeFileComment(np, ps.nedfile);

    ps.propertyscope.push(ps.nedfile);

    globalps = ps; // remember this for error recovery

    if (np->getStoreSourceFlag())
        storeSourceCode(np, ps.nedfile, np->getSource()->getFullTextPos());

    // parse
    try
    {
        yyparse(np);
    }
    catch (NedException& e)
    {
        yyerror(np, (std::string("error during parsing: ")+e.what()).c_str());
        yy_delete_buffer(handle);
        return nullptr;
    }

    if (np->getErrors()->empty())
    {
        // more sanity checks
        if (ps.propertyscope.size()!=1 || ps.propertyscope.top()!=ps.nedfile)
            INTERNAL_ERROR0(nullptr, "error during parsing: imbalanced propertyscope");
        if (!ps.blockscope.empty() || !ps.typescope.empty())
            INTERNAL_ERROR0(nullptr, "error during parsing: imbalanced blockscope or typescope");
    }
    yy_delete_buffer(handle);

    return ps.nedfile;
}

void yyerror(ParseContext *np, const char *s)
{
    // chop newline
    char buf[250];
    strcpy(buf, s);
    if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

    np->error(buf, pos.li);
}
