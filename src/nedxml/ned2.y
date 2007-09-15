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

/* Reserved words */
%token IMPORT PACKAGE PROPERTY
%token MODULE SIMPLE NETWORK CHANNEL MODULEINTERFACE CHANNELINTERFACE
%token EXTENDS LIKE
%token TYPES PARAMETERS GATES SUBMODULES CONNECTIONS ALLOWUNCONNECTED
%token DOUBLETYPE INTTYPE STRINGTYPE BOOLTYPE XMLTYPE VOLATILE TYPENAME
%token INPUT_ OUTPUT_ INOUT_
%token IF FOR
%token RIGHTARROW LEFTARROW DBLARROW TO
%token TRUE_ FALSE_ THIS_ DEFAULT CONST_ SIZEOF INDEX_ XMLDOC

/* Other tokens: identifiers, numeric literals, operators etc */
%token NAME INTCONSTANT REALCONSTANT STRINGCONSTANT CHARCONSTANT
%token PLUSPLUS DOUBLEASTERISK
%token EQ NE GE LE
%token AND OR XOR NOT
%token BIN_AND BIN_OR BIN_XOR BIN_COMPL
%token SHIFT_LEFT SHIFT_RIGHT

%token EXPRESSION_SELECTOR   /* forces parsing text as a singe expression */

%token INVALID_CHAR   /* just to generate parse error */

/* Operator precedences (low to high) and associativity */
%right '?' ':'
%left AND OR XOR
%left EQ NE '>' GE '<' LE
%left BIN_AND BIN_OR BIN_XOR
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-'
%left '*' '/' '%'
%right '^'
%left UMIN NOT BIN_COMPL

%start startsymbol

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

#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include "nedyydefs.h"
#include "nederror.h"

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <string.h>         /* YYVERBOSE needs it */
#endif

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
void yyerror (const char *s);

#include "nedparser.h"
#include "nedfilebuffer.h"
#include "nedelements.h"
#include "nedutil.h"
#include "nedyylib.h"

static struct NED2ParserState
{
    bool inTypes;
    bool inConnGroup;
    std::stack<NEDElement *> propertyscope; // top(): where to insert properties as we parse them
    std::stack<NEDElement *> blockscope;    // top(): where to insert parameters, gates, etc
    std::stack<NEDElement *> typescope;     // top(): as blockscope, but ignore submodules and connection channels

    /* tmp flags, used with param, gate and conn */
    int paramType;
    int gateType;
    bool isVolatile;
    bool isDefault;
    YYLTYPE exprPos;
    int subgate;
    std::vector<NEDElement *> propvals; // temporarily collects property values

    /* tmp flags, used with msg fields */
    bool isAbstract;
    bool isReadonly;

    /* NED-II: modules, channels */
    NedFileNode *nedfile;
    CommentNode *comment;
    PackageNode *package;
    ImportNode *import;
    PropertyDeclNode *propertydecl;
    ExtendsNode *extends;
    InterfaceNameNode *interfacename;
    NEDElement *component;  // compound/simple module, module interface, channel or channel interface
    ParametersNode *parameters;
    ParamNode *param;
    PatternNode *pattern;
    PropertyNode *property;
    PropertyKeyNode *propkey;
    TypesNode *types;
    GatesNode *gates;
    GateNode *gate;
    SubmodulesNode *submods;
    SubmoduleNode *submod;
    ConnectionsNode *conns;
    ConnectionGroupNode *conngroup;
    ConnectionNode *conn;
    ChannelSpecNode *chanspec;
    LoopNode *loop;
    ConditionNode *condition;
} ps;


static void resetParserState()
{
    static NED2ParserState cleanps;
    ps = cleanps;
}

static NED2ParserState globalps;  // for error recovery

static void restoreGlobalParserState()  // for error recovery
{
    ps = globalps;
}

static void assertNonEmpty(std::stack<NEDElement *>& somescope) {
    // for error recovery: STL stack::top() crashes if stack is empty
    if (somescope.empty())
    {
        INTERNAL_ERROR0(NULL, "error during parsing: scope stack empty");
        somescope.push(NULL);
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
        |
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
                { storePos(ps.component, @$); restoreGlobalParserState(); }
        | CHANNELINTERFACE error '}'
                { restoreGlobalParserState(); }
        | simplemoduleheader error '}'
                { storePos(ps.component, @$); restoreGlobalParserState(); }
        | SIMPLE error '}'
                { restoreGlobalParserState(); }
        | compoundmoduleheader error '}'
                { storePos(ps.component, @$); restoreGlobalParserState(); }
        | MODULE error '}'
                { restoreGlobalParserState(); }
        | networkheader error '}'
                { storePos(ps.component, @$); restoreGlobalParserState(); }
        | NETWORK error '}'
                { restoreGlobalParserState(); }
        | moduleinterfaceheader error '}'
                { storePos(ps.component, @$); restoreGlobalParserState(); }
        | MODULEINTERFACE error '}'
                { restoreGlobalParserState(); }
        | channelheader error '}'
                { storePos(ps.component, @$); restoreGlobalParserState(); }
        | CHANNEL error '}'
                { restoreGlobalParserState(); }
        ;

packagedeclaration
        : PACKAGE dottedname ';'
                {
                  ps.package = (PackageNode *)createNodeWithTag(NED_PACKAGE, ps.nedfile);
                  ps.package->setName(removeSpaces(@2).c_str());
                  storePos(ps.package,@$);
                  storeBannerAndRightComments(ps.package,@$);
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
                  ps.import = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile);
                  ps.import->setImportSpec(removeSpaces(@2).c_str());
                  storePos(ps.import,@$);
                  storeBannerAndRightComments(ps.import,@$);
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
                    storePos(ps.propertydecl, @$);
                    storeBannerAndRightComments(ps.propertydecl,@$);
                }
        | propertydecl_header '(' opt_propertydecl_keys ')' opt_inline_properties ';'
                {
                    storePos(ps.propertydecl, @$);
                    storeBannerAndRightComments(ps.propertydecl,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

propertydecl_header
        : PROPERTY '@' NAME
                {
                  ps.propertydecl = (PropertyDeclNode *)createNodeWithTag(NED_PROPERTY_DECL, ps.nedfile);
                  ps.propertydecl->setName(toString(@3));
                }
        | PROPERTY '@' NAME '[' ']'
                {
                  ps.propertydecl = (PropertyDeclNode *)createNodeWithTag(NED_PROPERTY_DECL, ps.nedfile);
                  ps.propertydecl->setName(toString(@3));
                  ps.propertydecl->setIsArray(true);
                }
        ;

opt_propertydecl_keys
        : propertydecl_keys
        |
        ;

propertydecl_keys
        : propertydecl_keys ';' propertydecl_key
        | propertydecl_key
        ;

propertydecl_key
        : NAME
                {
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.propertydecl);
                  ps.propkey->setName(toString(@1));
                  storePos(ps.propkey, @$);
                }
        ;

/*
 * File Property
 */
fileproperty
        : property_namevalue ';'
                {
                  storePos(ps.property, @$);
                  storeBannerAndRightComments(ps.property,@$);
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
                  ps.parameters = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.component);
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
                      storeComponentSourceCode(ps.component, @$);
                  storePos(ps.component, @$);
                  storeTrailingComment(ps.component,@$);
                }
        ;

channelheader
        : CHANNEL NAME
                {
                  ps.component = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile);
                  ((ChannelNode *)ps.component)->setName(toString(@2));
                }
           opt_inheritance
                { storeBannerAndRightComments(ps.component,@$); }
        ;

opt_inheritance
        :
        | EXTENDS extendsname
        | LIKE likenames
        | EXTENDS extendsname LIKE likenames
        ;

extendsname
        : dottedname
                {
                  ps.extends = (ExtendsNode *)createNodeWithTag(NED_EXTENDS, ps.component);
                  ps.extends->setName(removeSpaces(@1).c_str());
                  storePos(ps.extends, @$);
                }
        ;

likenames
        : likenames ',' likename
        | likename
        ;

likename
        : dottedname
                {
                  ps.interfacename = (InterfaceNameNode *)createNodeWithTag(NED_INTERFACE_NAME, ps.component);
                  ps.interfacename->setName(removeSpaces(@1).c_str());
                  storePos(ps.interfacename, @$);
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
                  ps.parameters = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.component);
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
                      storeComponentSourceCode(ps.component, @$);
                  storePos(ps.component, @$);
                  storeTrailingComment(ps.component,@$);
                }
        ;

channelinterfaceheader
        : CHANNELINTERFACE NAME
                {
                  ps.component = (ChannelInterfaceNode *)createNodeWithTag(NED_CHANNEL_INTERFACE, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile);
                  ((ChannelInterfaceNode *)ps.component)->setName(toString(@2));
                }
           opt_interfaceinheritance
                { storeBannerAndRightComments(ps.component,@$); }
        ;

opt_interfaceinheritance
        : EXTENDS extendsnames
        |
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
                  ps.parameters = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.component);
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
                      storeComponentSourceCode(ps.component, @$);
                  storePos(ps.component, @$);
                  storeTrailingComment(ps.component,@$);
                }
        ;

simplemoduleheader
        : SIMPLE NAME
                {
                  ps.component = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile );
                  ((SimpleModuleNode *)ps.component)->setName(toString(@2));
                }
          opt_inheritance
                { storeBannerAndRightComments(ps.component,@$); }
        ;

/*
 * Module
 */
compoundmoduledefinition
        : compoundmoduleheader '{'
                {
                  ps.typescope.push(ps.component);
                  ps.blockscope.push(ps.component);
                  ps.parameters = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.component);
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
                      storeComponentSourceCode(ps.component, @$);
                  storePos(ps.component, @$);
                  storeTrailingComment(ps.component,@$);
                }
        ;

compoundmoduleheader
        : MODULE NAME
                {
                  ps.component = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile );
                  ((CompoundModuleNode *)ps.component)->setName(toString(@2));
                }
          opt_inheritance
                { storeBannerAndRightComments(ps.component,@$); }
        ;

/*
 * Network
 */
networkdefinition
        : networkheader '{'
                {
                  ps.typescope.push(ps.component);
                  ps.blockscope.push(ps.component);
                  ps.parameters = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.component);
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
                      storeComponentSourceCode(ps.component, @$);
                  storePos(ps.component, @$);
                  storeTrailingComment(ps.component,@$);
                }
        ;

networkheader
        : NETWORK NAME
                {
                  ps.component = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile );
                  ((CompoundModuleNode *)ps.component)->setName(toString(@2));
                  ((CompoundModuleNode *)ps.component)->setIsNetwork(true);
                }
          opt_inheritance
                { storeBannerAndRightComments(ps.component,@$); }
        ;

/*
 * Module Interface
 */
moduleinterfacedefinition
        : moduleinterfaceheader '{'
                {
                  ps.typescope.push(ps.component);
                  ps.blockscope.push(ps.component);
                  ps.parameters = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.component);
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
                      storeComponentSourceCode(ps.component, @$);
                  storePos(ps.component, @$);
                  storeTrailingComment(ps.component,@$);
                }
        ;

moduleinterfaceheader
        : MODULEINTERFACE NAME
                {
                  ps.component = (ModuleInterfaceNode *)createNodeWithTag(NED_MODULE_INTERFACE, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile);
                  ((ModuleInterfaceNode *)ps.component)->setName(toString(@2));
                }
           opt_interfaceinheritance
                { storeBannerAndRightComments(ps.component,@$); }
        ;

/*
 * Parameters
 */
opt_paramblock
        : opt_params   /* "parameters" keyword is optional */
                {
                  storePos(ps.parameters, @$);
                  if (!ps.parameters->getFirstChild()) { // delete "parameters" element if empty
                      ps.parameters->getParent()->removeChild(ps.parameters);
                      delete ps.parameters;
                  }
                }
        | PARAMETERS ':'
                {
                  ps.parameters->setIsImplicit(false);
                  storeBannerAndRightComments(ps.parameters,@1,@2);
                }
          opt_params
                { storePos(ps.parameters, @$); }
        ;

opt_params
        : params
        |
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
                {
                  ps.propertyscope.push(ps.param);
                }
          opt_inline_properties ';'
                {
                  ps.propertyscope.pop();
                  storePos(ps.param, @$);
                  storeBannerAndRightComments(ps.param,@$);
                }
        | pattern_value
                {
                  ps.propertyscope.push(ps.pattern);
                }
          opt_inline_properties ';'
                {
                  ps.propertyscope.pop();
                  storePos(ps.pattern, @$);
                  storeBannerAndRightComments(ps.pattern,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

/*
 * Parameter
 */
param_typenamevalue
        : opt_volatile paramtype NAME
                {
                  ps.param = addParameter(ps.parameters, @3);
                  ps.param->setType(ps.paramType);
                  ps.param->setIsVolatile(ps.isVolatile);
                }
        | opt_volatile paramtype NAME '=' paramvalue
                {
                  ps.param = addParameter(ps.parameters, @3);
                  ps.param->setType(ps.paramType);
                  ps.param->setIsVolatile(ps.isVolatile);
                  addExpression(ps.param, "value",ps.exprPos,$5);
                  ps.param->setIsDefault(ps.isDefault);
                }
        | NAME '=' paramvalue
                {
                  ps.param = addParameter(ps.parameters, @1);
                  addExpression(ps.param, "value",ps.exprPos,$3);
                  ps.param->setIsDefault(ps.isDefault);
                }
        | NAME
                {
                  ps.param = addParameter(ps.parameters, @1);
                }
        | TYPENAME '=' paramvalue  /* this is to assign module type with the "<> like Foo" syntax */
                {
                  ps.param = addParameter(ps.parameters, @1);
                  addExpression(ps.param, "value", ps.exprPos,$3);
                  ps.param->setIsDefault(ps.isDefault);
                }
        ;

pattern_value
        : '/' pattern '/' '=' paramvalue
                {
                  ps.pattern = (PatternNode *)createNodeWithTag(NED_PATTERN, ps.parameters);
                  ps.pattern->setPattern(toString(@2));
                  addExpression(ps.pattern, "value",ps.exprPos,$5);
                  ps.pattern->setIsDefault(ps.isDefault);
                }
        ;

paramtype
        : DOUBLETYPE
                { ps.paramType = NED_PARTYPE_DOUBLE; }
        | INTTYPE
                { ps.paramType = NED_PARTYPE_INT; }
        | STRINGTYPE
                { ps.paramType = NED_PARTYPE_STRING; }
        | BOOLTYPE
                { ps.paramType = NED_PARTYPE_BOOL; }
        | XMLTYPE
                { ps.paramType = NED_PARTYPE_XML; }
        ;

opt_volatile
        : VOLATILE
                { ps.isVolatile = true; }
        |
                { ps.isVolatile = false; }
        ;

paramvalue
        : expression
                { $$ = $1; ps.isDefault = false; ps.exprPos = @1; }
        | DEFAULT '(' expression ')'
                { $$ = $3; ps.isDefault = true; ps.exprPos = @3; }
        ;

opt_inline_properties
        : inline_properties
        |
        ;

inline_properties
        : inline_properties property_namevalue
        | property_namevalue
        ;

pattern /* this attempts to capture inifile-like patterns */
        : pattern pattern_elem
        | pattern_elem
        ;

pattern_elem
        : '.'
        | '*'
        | '?'
        | DOUBLEASTERISK
        | NAME
        | INTCONSTANT
        | TO
        | '[' pattern ']'
        | '{' pattern '}'
        /* allow reserved words in patterns as well */
        | IMPORT | PACKAGE | PROPERTY
        | MODULE | SIMPLE | NETWORK | CHANNEL | MODULEINTERFACE | CHANNELINTERFACE
        | EXTENDS | LIKE
        | DOUBLETYPE | INTTYPE | STRINGTYPE | BOOLTYPE | XMLTYPE | VOLATILE | TYPENAME
        | INPUT_ | OUTPUT_ | INOUT_ | IF | FOR
        | TYPES | PARAMETERS | GATES | SUBMODULES | CONNECTIONS | ALLOWUNCONNECTED
        | TRUE_ | FALSE_ | THIS_ | DEFAULT | CONST_ | SIZEOF | INDEX_ | XMLDOC
        ;

/*
 * Property
 */
property
        : property_namevalue ';'
                {
                  storePos(ps.property, @$);
                  storeBannerAndRightComments(ps.property,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

property_namevalue
        : property_name
        | property_name '(' opt_property_keys ')'
        ;

property_name
        : '@' NAME
                {
                  assertNonEmpty(ps.propertyscope);
                  ps.property = addProperty(ps.propertyscope.top(), toString(@2));
                  ps.propvals.clear(); // just to be safe
                }
        | '@' NAME '[' NAME ']'
                {
                  assertNonEmpty(ps.propertyscope);
                  ps.property = addProperty(ps.propertyscope.top(), toString(@2));
                  ps.property->setIndex(toString(@4));
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
        : NAME '=' property_values
                {
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  ps.propkey->setName(toString(@1));
                  for (int i=0; i<(int)ps.propvals.size(); i++)
                      ps.propkey->appendChild(ps.propvals[i]);
                  ps.propvals.clear();
                  storePos(ps.propkey, @$);
                }
        | property_values
                {
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  ps.propkey->appendChild($1);
                  for (int i=0; i<(int)ps.propvals.size(); i++)
                      ps.propkey->appendChild(ps.propvals[i]);
                  ps.propvals.clear();
                  storePos(ps.propkey, @$);
                }
        ;

property_values
        : property_values ',' property_value
                { ps.propvals.push_back($3); }
        | property_value
                { ps.propvals.push_back($1); }
        ;

property_value
        : NAME
                { $$ = createLiteral(NED_CONST_STRING, @1, @1); /* not a quoted string */ }
        | '$' NAME
                { $$ = createLiteral(NED_CONST_STRING, @$, @$); /* not a quoted string */ }
        | STRINGCONSTANT
                { $$ = createStringLiteral(@1); }
        | TRUE_
                { $$ = createLiteral(NED_CONST_BOOL, @1, @1); }
        | FALSE_
                { $$ = createLiteral(NED_CONST_BOOL, @1, @1); }
        | INTCONSTANT
                { $$ = createLiteral(NED_CONST_INT, @1, @1); }
        | REALCONSTANT
                { $$ = createLiteral(NED_CONST_DOUBLE, @1, @1); }
        | quantity
                { $$ = createQuantityLiteral(@1); }
        | '-'  /* antivalue ("remove existing value from this position") */
                { $$ = createLiteral(NED_CONST_SPEC, @1, @1); }
        |  /* nothing (no value) */
                {
                  LiteralNode *node = (LiteralNode *)createNodeWithTag(NED_LITERAL);
                  node->setType(NED_CONST_SPEC); // and leave both value and text at ""
                  $$ = node;
                }
        ;

/*
 * Gates
 */
opt_gateblock
        : gateblock
        |
        ;

gateblock
        : GATES ':'
                {
                  assertNonEmpty(ps.blockscope);
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.blockscope.top());
                  storeBannerAndRightComments(ps.gates,@1,@2);
                }
          opt_gates
                {
                  storePos(ps.gates, @$);
                }
        ;

opt_gates
        : gates
        |
        ;

gates
        : gates gate
                {
                  storeBannerAndRightComments(ps.gate,@2);
                }
        | gate
                {
                  storeBannerAndRightComments(ps.gate,@1);
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
                  storePos(ps.gate, @$);
                }
        ; /* no error recovery rule -- see discussion at top */

gate_typenamesize
        : gatetype NAME
                {
                  ps.gate = addGate(ps.gates, @2);
                  ps.gate->setType(ps.gateType);
                }
        | gatetype NAME '[' ']'
                {
                  ps.gate = addGate(ps.gates, @2);
                  ps.gate->setType(ps.gateType);
                  ps.gate->setIsVector(true);
                }
        | gatetype NAME vector
                {
                  ps.gate = addGate(ps.gates, @2);
                  ps.gate->setType(ps.gateType);
                  ps.gate->setIsVector(true);
                  addVector(ps.gate, "vector-size",@3,$3);
                }
        | NAME
                {
                  ps.gate = addGate(ps.gates, @1);
                }
        | NAME '[' ']'
                {
                  ps.gate = addGate(ps.gates, @1);
                  ps.gate->setIsVector(true);
                }
        | NAME vector
                {
                  ps.gate = addGate(ps.gates, @1);
                  ps.gate->setIsVector(true);
                  addVector(ps.gate, "vector-size",@2,$2);
                }
        ;

gatetype
        : INPUT_
                { ps.gateType = NED_GATETYPE_INPUT; }
        | OUTPUT_
                { ps.gateType = NED_GATETYPE_OUTPUT; }
        | INOUT_
                { ps.gateType = NED_GATETYPE_INOUT; }
        ;

/*
 * Local Types
 */
opt_typeblock
        : typeblock
        |
        ;

typeblock
        : TYPES ':'
                {
                  assertNonEmpty(ps.blockscope);
                  ps.types = (TypesNode *)createNodeWithTag(NED_TYPES, ps.blockscope.top());
                  storeBannerAndRightComments(ps.types,@1,@2);
                  if (ps.inTypes)
                     np->getErrors()->addError(ps.types,"more than one level of type nesting is not allowed");
                  ps.inTypes = true;
                }
           opt_localtypes
                {
                  ps.inTypes = false;
                  storePos(ps.types, @$);
                }
        ;

opt_localtypes
        : localtypes
        |
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
        |
        ;

submodblock
        : SUBMODULES ':'
                {
                  assertNonEmpty(ps.blockscope);
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.blockscope.top());
                  storeBannerAndRightComments(ps.submods,@1,@2);
                }
          opt_submodules
                {
                  storePos(ps.submods, @$);
                }
        ;

opt_submodules
        : submodules
        |
        ;

submodules
        : submodules submodule
        | submodule
        ;

submodule
        : submoduleheader ';'
                {
                  storeBannerAndRightComments(ps.submod,@1,@2);
                  storePos(ps.submod, @$);
                }
        | submoduleheader '{'
                {
                  ps.blockscope.push(ps.submod);
                  ps.parameters = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.submod);
                  ps.parameters->setIsImplicit(true);
                  ps.propertyscope.push(ps.parameters);
                  storeBannerAndRightComments(ps.submod,@1,@2);
                }
          opt_paramblock
          opt_gateblock
          '}' opt_semicolon
                {
                  ps.blockscope.pop();
                  ps.propertyscope.pop();
                  storePos(ps.submod, @$);
                  storeTrailingComment(ps.submod,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

submoduleheader
        : submodulename ':' dottedname
                {
                  ps.submod->setType(removeSpaces(@3).c_str());
                }
        | submodulename ':' likeparam LIKE dottedname
                {
                  addLikeParam(ps.submod, "like-param", @3, $3);
                  ps.submod->setLikeType(removeSpaces(@5).c_str());
                }
        ;

submodulename
        : NAME
                {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString(@1));
                }
        |  NAME vector
                {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString(@1));
                  addVector(ps.submod, "vector-size",@2,$2);
                }
        ;

likeparam
        : '<' '>'
                { $$ = NULL; }
        | '<' '@' NAME '>'
                { $$ = NULL; }
        | '<' thisqualifier '.' '@' NAME '>'
                { $$ = NULL; }
        | '<' expression '>' /* XXX this expression is the source of one shift-reduce conflict because it may contain '>' */
                { $$ = $2; }
        ;

thisqualifier
        : THIS_
        | NAME
                { np->getErrors()->addError(currentLocation(), "invalid property qualifier `%s', only `this' is allowed here", toString(@1)); }
        | NAME vector
                { np->getErrors()->addError(currentLocation(), "invalid property qualifier `%s', only `this' is allowed here", toString(@1)); }
        ;


/*
 * Connections
 */
opt_connblock
        : connblock
        |
        ;

connblock
        : CONNECTIONS ALLOWUNCONNECTED ':'
                {
                  assertNonEmpty(ps.blockscope);
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.blockscope.top());
                  ps.conns->setAllowUnconnected(true);
                  storeBannerAndRightComments(ps.conns,@1,@3);
                }
          opt_connections
                {
                  storePos(ps.conns, @$);
                }
        | CONNECTIONS ':'
                {
                  assertNonEmpty(ps.blockscope);
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.blockscope.top());
                  storeBannerAndRightComments(ps.conns,@1,@2);
                }
          opt_connections
                {
                  storePos(ps.conns, @$);
                }
        ;

opt_connections
        : connections
        |
        ;

connections
        : connections connectionsitem
        | connectionsitem
        ;

connectionsitem
        : connectiongroup
        | connection opt_loops_and_conditions ';'
                {
                  ps.chanspec = (ChannelSpecNode *)ps.conn->getFirstChildWithTag(NED_CHANNEL_SPEC);
                  if (ps.chanspec)
                      ps.conn->appendChild(ps.conn->removeChild(ps.chanspec)); // move channelspec to conform DTD
                  if ($2) {
                      transferChildren($2, ps.conn);
                      delete $2;
                  }
                  storePos(ps.conn, @$);
                  storeBannerAndRightComments(ps.conn,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

connectiongroup
        : opt_loops_and_conditions '{'
                {
                  if (ps.inConnGroup)
                      np->getErrors()->addError(ps.conngroup,"nested connection groups are not allowed");
                  ps.conngroup = (ConnectionGroupNode *)createNodeWithTag(NED_CONNECTION_GROUP, ps.conns);
                  if ($1) {
                      // for's and if's were collected in a temporary UnknownNode, put them under conngroup now
                      transferChildren($1, ps.conngroup);
                      delete $1;
                  }
                  ps.inConnGroup = true;
                  storeBannerAndRightComments(ps.conngroup,@1);
                }
          connections '}' opt_semicolon
                {
                  ps.inConnGroup = false;
                  storePos(ps.conngroup,@$);
                  storeTrailingComment(ps.conngroup,@$);
                }
        ;

opt_loops_and_conditions
        : loops_and_conditions
                { $$ = $1; }
        |
                { $$ = NULL; }
        ;

loops_and_conditions
        : loops_and_conditions ',' loop_or_condition
                {
                  $1->appendChild($3);
                  $$ = $1;
                }
        | loop_or_condition
                {
                  $$ = new UnknownNode();
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
                  ps.loop = (LoopNode *)createNodeWithTag(NED_LOOP);
                  ps.loop->setParamName( toString(@2) );
                  addExpression(ps.loop, "from-value",@4,$4);
                  addExpression(ps.loop, "to-value",@6,$6);
                  storePos(ps.loop, @$);
                  $$ = ps.loop;
                }
        ;

/*
 * Connection
 */
connection
        : leftgatespec RIGHTARROW rightgatespec
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                }
        | leftgatespec RIGHTARROW channelspec RIGHTARROW rightgatespec
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                }
        | leftgatespec LEFTARROW rightgatespec
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                }
        | leftgatespec LEFTARROW channelspec LEFTARROW rightgatespec
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                }
        | leftgatespec DBLARROW rightgatespec
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_BIDIR);
                }
        | leftgatespec DBLARROW channelspec DBLARROW rightgatespec
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_BIDIR);
                }
        ;

leftgatespec
        : leftmod '.' leftgate
        | parentleftgate
        ;

leftmod
        : NAME vector
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inConnGroup ? (NEDElement*)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                  addVector(ps.conn, "src-module-index",@2,$2);
                }
        | NAME
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inConnGroup ? (NEDElement*)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                }
        ;

leftgate
        : NAME opt_subgate
                {
                  ps.conn->setSrcGate( toString( @1) );
                  ps.conn->setSrcGateSubg(ps.subgate);
                }
        | NAME opt_subgate vector
                {
                  ps.conn->setSrcGate( toString( @1) );
                  ps.conn->setSrcGateSubg(ps.subgate);
                  addVector(ps.conn, "src-gate-index",@3,$3);
                }
        | NAME opt_subgate PLUSPLUS
                {
                  ps.conn->setSrcGate( toString( @1) );
                  ps.conn->setSrcGateSubg(ps.subgate);
                  ps.conn->setSrcGatePlusplus(true);
                }
        ;

parentleftgate
        : NAME opt_subgate
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inConnGroup ? (NEDElement*)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                  ps.conn->setSrcGateSubg(ps.subgate);
                }
        | NAME opt_subgate vector
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inConnGroup ? (NEDElement*)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                  ps.conn->setSrcGateSubg(ps.subgate);
                  addVector(ps.conn, "src-gate-index",@3,$3);
                }
        | NAME opt_subgate PLUSPLUS
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inConnGroup ? (NEDElement*)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
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
                  ps.conn->setDestModule( toString(@1) );
                }
        | NAME vector
                {
                  ps.conn->setDestModule( toString(@1) );
                  addVector(ps.conn, "dest-module-index",@2,$2);
                }
        ;

rightgate
        : NAME opt_subgate
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                }
        | NAME opt_subgate vector
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                  addVector(ps.conn, "dest-gate-index",@3,$3);
                }
        | NAME opt_subgate PLUSPLUS
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                  ps.conn->setDestGatePlusplus(true);
                }
        ;

parentrightgate
        : NAME opt_subgate
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                }
        | NAME opt_subgate vector
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                  addVector(ps.conn, "dest-gate-index",@3,$3);
                }
        | NAME opt_subgate PLUSPLUS
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGateSubg(ps.subgate);
                  ps.conn->setDestGatePlusplus(true);
                }
        ;

opt_subgate
        : '$' NAME
                {
                  const char *s = toString(@2);
                  if (!strcmp(s,"i"))
                      ps.subgate = NED_SUBGATE_I;
                  else if (!strcmp(s,"o"))
                      ps.subgate = NED_SUBGATE_O;
                  else
                       np->getErrors()->addError(currentLocation(), "invalid subgate spec `%s', must be `i' or `o'", toString(@2));
                }
        |
                { ps.subgate = NED_SUBGATE_NONE; }
        ;

channelspec
        : channelspec_header
                { storePos(ps.chanspec, @$); }
        | channelspec_header '{'
                {
                  ps.parameters = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.chanspec);
                  ps.parameters->setIsImplicit(true);
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
          '}'
                {
                  ps.propertyscope.pop();
                  storePos(ps.chanspec, @$);
                }
        ;


channelspec_header
        :
                {
                  ps.chanspec = (ChannelSpecNode *)createNodeWithTag(NED_CHANNEL_SPEC, ps.conn);
                }
        | dottedname
                {
                  ps.chanspec = (ChannelSpecNode *)createNodeWithTag(NED_CHANNEL_SPEC, ps.conn);
                  ps.chanspec->setType(removeSpaces(@1).c_str());
                }
        | likeparam LIKE dottedname
                {
                  ps.chanspec = (ChannelSpecNode *)createNodeWithTag(NED_CHANNEL_SPEC, ps.conn);
                  addLikeParam(ps.chanspec, "like-param", @1, $1);
                  ps.chanspec->setLikeType(removeSpaces(@3).c_str());
                }
        ;

/*
 * Condition
 */
condition
        : IF expression
                {
                  ps.condition = (ConditionNode *)createNodeWithTag(NED_CONDITION);
                  addExpression(ps.condition, "condition",@2,$2);
                  storePos(ps.condition, @$);
                  $$ = ps.condition;
                }
        ;

/*
 * Common part
 */
vector
        : '[' expression ']'
                { $$ = $2; }
        ;

expression
        :
          expr
                {
                  if (np->getParseExpressionsFlag()) $$ = createExpression($1);
                }
        | xmldocvalue
                {
                  if (np->getParseExpressionsFlag()) $$ = createExpression($1);
                }
        ;

/*
 * Expressions
 */
/* FIXME TBD: storePos() stuff for expressions */
xmldocvalue
        : XMLDOC '(' stringliteral ',' stringliteral ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction("xmldoc", $3, $5); }
        | XMLDOC '(' stringliteral ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction("xmldoc", $3); }
        ;

expr
        : simple_expr
        | '(' expr ')'
                { $$ = $2; }
        | CONST_ '(' expr ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction("const", $3); }

        | expr '+' expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("+", $1, $3); }
        | expr '-' expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("-", $1, $3); }
        | expr '*' expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("*", $1, $3); }
        | expr '/' expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("/", $1, $3); }
        | expr '%' expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("%", $1, $3); }
        | expr '^' expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("^", $1, $3); }

        | '-' expr
                %prec UMIN
                { if (np->getParseExpressionsFlag()) $$ = unaryMinus($2); }

        | expr EQ expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("==", $1, $3); }
        | expr NE expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("!=", $1, $3); }
        | expr '>' expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator(">", $1, $3); }
        | expr GE expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator(">=", $1, $3); }
        | expr '<' expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("<", $1, $3); }
        | expr LE expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("<=", $1, $3); }

        | expr AND expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("&&", $1, $3); }
        | expr OR expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("||", $1, $3); }
        | expr XOR expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("##", $1, $3); }

        | NOT expr
                %prec UMIN
                { if (np->getParseExpressionsFlag()) $$ = createOperator("!", $2); }

        | expr BIN_AND expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("&", $1, $3); }
        | expr BIN_OR expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("|", $1, $3); }
        | expr BIN_XOR expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("#", $1, $3); }

        | BIN_COMPL expr
                %prec UMIN
                { if (np->getParseExpressionsFlag()) $$ = createOperator("~", $2); }
        | expr SHIFT_LEFT expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("<<", $1, $3); }
        | expr SHIFT_RIGHT expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator(">>", $1, $3); }
        | expr '?' expr ':' expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator("?:", $1, $3, $5); }

        | NAME '(' ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction(toString(@1)); }
        | NAME '(' expr ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction(toString(@1), $3); }
        | NAME '(' expr ',' expr ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction(toString(@1), $3, $5); }
        | NAME '(' expr ',' expr ',' expr ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction(toString(@1), $3, $5, $7); }
        | NAME '(' expr ',' expr ',' expr ',' expr ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction(toString(@1), $3, $5, $7, $9); }
         ;

simple_expr
        : identifier
        | special_expr
        | literal
        ;

identifier
        : NAME
                { if (np->getParseExpressionsFlag()) $$ = createIdent(@1); }
        | THIS_ '.' NAME
                { if (np->getParseExpressionsFlag()) $$ = createIdent(@3, @1); }
        | NAME '.' NAME
                { if (np->getParseExpressionsFlag()) $$ = createIdent(@3, @1); }
        | NAME '[' expr ']' '.' NAME
                { if (np->getParseExpressionsFlag()) $$ = createIdent(@6, @1, $3); }
        ;

special_expr
        : INDEX_
                { if (np->getParseExpressionsFlag()) $$ = createFunction("index"); }
        | INDEX_ '(' ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction("index"); }
        | SIZEOF '(' identifier ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction("sizeof", $3); }
        ;

literal
        : stringliteral
        | boolliteral
        | numliteral
        ;

stringliteral
        : STRINGCONSTANT
                { if (np->getParseExpressionsFlag()) $$ = createStringLiteral(@1); }
        ;

boolliteral
        : TRUE_
                { if (np->getParseExpressionsFlag()) $$ = createLiteral(NED_CONST_BOOL, @1, @1); }
        | FALSE_
                { if (np->getParseExpressionsFlag()) $$ = createLiteral(NED_CONST_BOOL, @1, @1); }
        ;

numliteral
        : INTCONSTANT
                { if (np->getParseExpressionsFlag()) $$ = createLiteral(NED_CONST_INT, @1, @1); }
        | REALCONSTANT
                { if (np->getParseExpressionsFlag()) $$ = createLiteral(NED_CONST_DOUBLE, @1, @1); }
        | quantity
                { if (np->getParseExpressionsFlag()) $$ = createQuantityLiteral(@1); }
        ;

quantity
        : quantity INTCONSTANT NAME
        | quantity REALCONSTANT NAME
        | INTCONSTANT NAME
        | REALCONSTANT NAME
        ;

opt_semicolon
        : ';'
        |
        ;

%%

//----------------------------------------------------------------------
// general bison/flex stuff:
//

NEDElement *doParseNED2(NEDParser *p, const char *nedtext)
{
#if YYDEBUG != 0      /* #if added --VA */
    yydebug = YYDEBUGGING_ON;
#endif

    // reset the lexer
    pos.co = 0;
    pos.li = 1;
    prevpos = pos;

    yyin = NULL;
    yyout = stderr; // not used anyway

    // alloc buffer
    struct yy_buffer_state *handle = yy_scan_string(nedtext);
    if (!handle)
        {np->getErrors()->addError("", "unable to allocate work memory"); return false;}

    // create parser state and NEDFileNode
    np = p;
    resetParserState();
    ps.nedfile = new NedFileNode();

    // store file name with slashes always, even on Windows -- neddoc relies on that
    ps.nedfile->setFilename(slashifyFilename(np->getFileName()).c_str());
    ps.nedfile->setVersion("2");

    // store file comment
    storeFileComment(ps.nedfile);

    ps.propertyscope.push(ps.nedfile);

    globalps = ps; // remember this for error recovery

    if (np->getStoreSourceFlag())
        storeSourceCode(ps.nedfile, np->getSource()->getFullTextPos());

    // parse
    int ret;
    try
    {
        ret = yyparse();
    }
    catch (NEDException& e)
    {
        yyerror((std::string("error during parsing: ")+e.what()).c_str());
        yy_delete_buffer(handle);
        return 0;
    }

    if (np->getErrors()->empty())
    {
        // more sanity checks
        if (ps.propertyscope.size()!=1 || ps.propertyscope.top()!=ps.nedfile)
            INTERNAL_ERROR0(NULL, "error during parsing: imbalanced propertyscope");
        if (!ps.blockscope.empty() || !ps.typescope.empty())
            INTERNAL_ERROR0(NULL, "error during parsing: imbalanced blockscope or typescope");
    }
    yy_delete_buffer(handle);

    //FIXME TODO: fill in @documentation properties from comments
    return ps.nedfile;
}

void yyerror(const char *s)
{
    // chop newline
    char buf[250];
    strcpy(buf, s);
    if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

    np->error(buf, pos.li);
}

