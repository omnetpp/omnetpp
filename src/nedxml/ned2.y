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
%token MODULE SIMPLE NETWORK CHANNEL INTERFACE CHANNELINTERFACE
%token EXTENDS LIKE WITHCPPCLASS
%token TYPES PARAMETERS GATES SUBMODULES CONNECTIONS ALLOWUNCONNECTED
%token DOUBLETYPE INTTYPE STRINGTYPE BOOLTYPE XMLTYPE FUNCTION TYPENAME
%token INPUT_ OUTPUT_ INOUT_
%token IF WHERE
%token RIGHTARROW LEFTARROW DBLARROW TO
%token TRUE_ FALSE_ THIS_ DEFAULT CONST_ SIZEOF INDEX_ XMLDOC

/* Other tokens: identifiers, numeric literals, operators etc */
%token NAME INTCONSTANT REALCONSTANT STRINGCONSTANT CHARCONSTANT
%token PLUSPLUS DOUBLEASTERISK
%token EQ NE GE LE
%token AND OR XOR NOT
%token BIN_AND BIN_OR BIN_XOR BIN_COMPL
%token SHIFT_LEFT SHIFT_RIGHT

%token INVALID_CHAR   /* just to generate parse error --VA */

/* Operator precedences (low to high) and associativity */
%left '?' ':'
%left AND OR XOR
%left EQ NE '>' GE '<' LE
%left BIN_AND BIN_OR BIN_XOR
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-'
%left '*' '/' '%'
%right '^'
%left UMIN NOT BIN_COMPL

%start nedfile

/* requires at least bison 1.50 (tested with bison 2.1) */
%glr-parser

%{

#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include "nedgrammar.h"
#include "nederror.h"

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <string.h>         /* YYVERBOSE needs it */
#endif


int yylex (void);
void yyrestart(FILE *);
void yyerror (const char *s);


#include "nedparser.h"
#include "nedfilebuffer.h"
#include "nedelements.h"
#include "nedutil.h"

static YYLTYPE NULLPOS={0,0,0,0,0,0};

static const char *sourcefilename;

NEDParser *np;

struct ParserState
{
    bool parseExpressions;
    bool storeSourceCode;
    bool inLoop;
    bool inTypes;
    bool inGroup;
    std::stack<NEDElement *> propertyscope; // top(): where to insert properties as we parse them
    std::stack<NEDElement *> blockscope;    // top(): where to insert parameters, gates, etc
    std::stack<NEDElement *> typescope;     // top(): as blockscope, but ignore submodules and connection channels

    /* tmp flags, used with param */
    int paramType;
    bool isFunction;
    bool isDefault;

    /* tmp flags, used with msg fields */
    bool isAbstract;
    bool isReadonly;

    /* NED-II: modules, channels */
    NedFileNode *nedfile;
    WhitespaceNode *whitespace;
    ImportNode *import;
    PropertyDeclNode *propertydecl;
    ExtendsNode *extends;
    InterfaceNameNode *interfacename;
    NEDElement *component;  // compound/simple module, module interface, channel or channel interface
    ParametersNode *parameters;
    ParamGroupNode *paramgroup;
    ParamNode *param;
    PatternNode *pattern;
    PropertyNode *property;
    KeyValueNode *keyvalue;
    TypesNode *types;
    GatesNode *gates;
    GateGroupNode *gategroup;
    GateNode *gate;
    SubmodulesNode *submods;
    SubmoduleNode *submod;
    ConnectionsNode *conns;
    ConnectionNode *conn;
    ChannelSpecNode *chanspec;
    ConnectionGroupNode *conngroup;
    LoopNode *loop;
    ConditionNode *condition;
} ps;

NEDElement *createNodeWithTag(int tagcode, NEDElement *parent=NULL);

PropertyNode *addProperty(NEDElement *node, const char *name);  // directly under the node
PropertyNode *addComponentProperty(NEDElement *node, const char *name); // into ParametersNode child of node
KeyValueNode *addPropertyValue(PropertyNode *prop, const char *key, YYLTYPE valuepos);

PropertyNode *storeSourceCode(NEDElement *node, YYLTYPE tokenpos);  // directly under the node
PropertyNode *storeComponentSourceCode(NEDElement *node, YYLTYPE tokenpos); // into ParametersNode child

PropertyNode *storeDisplayString(NEDElement *node, YYLTYPE tokenpos);  // directly under the node
PropertyNode *storeComponentDisplayString(NEDElement *node, YYLTYPE tokenpos); // into ParametersNode child

void setFileComment(NEDElement *node);
void setBannerComment(NEDElement *node, YYLTYPE tokenpos);
void setRightComment(NEDElement *node, YYLTYPE tokenpos);
void setTrailingComment(NEDElement *node, YYLTYPE tokenpos);
void setComments(NEDElement *node, YYLTYPE pos);
void setComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos);

ParamNode *addParameter(NEDElement *params, YYLTYPE namepos);
GateNode *addGate(NEDElement *gates, YYLTYPE namepos, int is_in, int is_vector );
//SubmoduleNode *addSubmodule(NEDElement *submods, YYLTYPE namepos, YYLTYPE typepos,YYLTYPE likeparampos);
LoopNode *addLoop(NEDElement *conngroup, YYLTYPE varnamepos);

YYLTYPE trimBrackets(YYLTYPE vectorpos);
YYLTYPE trimAngleBrackets(YYLTYPE vectorpos);
YYLTYPE trimQuotes(YYLTYPE vectorpos);
YYLTYPE trimDoubleBraces(YYLTYPE vectorpos);
void swapAttributes(NEDElement *node, const char *attr1, const char *attr2);
void swapExpressionChildren(NEDElement *node, const char *attr1, const char *attr2);
void swapConnection(NEDElement *conn);

const char *toString(YYLTYPE);
const char *toString(long);

ExpressionNode *createExpression(NEDElement *expr);
OperatorNode *createOperator(const char *op, NEDElement *operand1, NEDElement *operand2=NULL, NEDElement *operand3=NULL);
FunctionNode *createFunction(const char *funcname, NEDElement *arg1=NULL, NEDElement *arg2=NULL, NEDElement *arg3=NULL, NEDElement *arg4=NULL);
IdentNode *createIdent(const char *param, const char *paramindex=NULL, const char *module=NULL, const char *moduleindex=NULL);
LiteralNode *createLiteral(int type, const char *value, const char *text=NULL);
LiteralNode *createQuantity(const char *text);
NEDElement *unaryMinus(NEDElement *node);

void addVector(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);
void addLikeParam(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);
void addExpression(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);

%}

%%

/*
 * Top-level components
 */
nedfile
        : packagedeclaration somedefinitions
        | somedefinitions
        ;

somedefinitions
        : somedefinitions definition
        |
        ;

definition
        : import

        | propertydecl
                { }
        | fileproperty
                { }
        | channeldefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); } /*FIXME won't work!!! if there's an embedded type in it, that'll hijack ps.component */
        | channelinterfacedefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); }
        | simplemoduledefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); }
        | compoundmoduledefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); }
        | networkdefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); }
        | moduleinterfacedefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); }
        ;

packagedeclaration         /* TBD package is currently not supported */
        : PACKAGE packagename ';'
        ;

packagename                /* TBD package is currently not supported */
        : packagename '.' NAME
        | NAME
        ;


/*
 * Import
 */
import
        : IMPORT STRINGCONSTANT ';'
                {
                  ps.import = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile);
                  ps.import->setFilename(toString(trimQuotes(@2)));
                  //setComments(ps.import,@1);
                }
        ;

/*
 * Property declaration
 */
propertydecl
        : propertydecl_header opt_inline_properties ';'
        | propertydecl_header '(' opt_propertydecl_keys ')' opt_inline_properties ';'
        ;

propertydecl_header
        : PROPERTY '@' NAME
                {
                  ps.propertydecl = (PropertyDeclNode *)createNodeWithTag(NED_PROPERTY_DECL, ps.nedfile);
                  ps.propertydecl->setName(toString(@3));
                  //setComments(ps.propertydecl,@1);
                }
        ;

opt_propertydecl_keys
        : propertydecl_keys
        |
        ;

propertydecl_keys
        : propertydecl_keys ',' propertydecl_key
        | propertydecl_key
        ;

propertydecl_key
        : NAME
                {
                  ps.keyvalue = (KeyValueNode *)createNodeWithTag(NED_KEY_VALUE, ps.propertydecl);
                }
        ;

/*
 * File Property
 */
fileproperty
        : property_namevalue ';'
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
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
          '}' opt_semicolon
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.typescope.pop();
                  setTrailingComment(ps.component,@4);
                }
        ;

channelheader
        : CHANNEL NAME
                {
                  ps.component = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile);
                  ((ChannelNode *)ps.component)->setName(toString(@2));
                  //setComments(ps.component,@1,@2);
                }
           opt_inheritance
        | CHANNEL WITHCPPCLASS NAME
                {
                  ps.component = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile);
                  ((ChannelNode *)ps.component)->setName(toString(@2));
                  //setComments(ps.component,@1,@2);
                }
           opt_inheritance
        ;

opt_inheritance
        :
        | EXTENDS extendsname
        | LIKE likenames
        | EXTENDS extendsname LIKE likenames
        ;

extendsname
        : NAME
                {
                  ps.extends = (ExtendsNode *)createNodeWithTag(NED_EXTENDS, ps.component);
                  ps.extends->setName(toString(@1));
                }
        ;

likenames
        : likenames ',' likename
        | likename
        ;

likename
        : NAME
                {
                  ps.interfacename = (InterfaceNameNode *)createNodeWithTag(NED_INTERFACE_NAME, ps.component);
                  ps.interfacename->setName(toString(@1));
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
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
          '}' opt_semicolon
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.typescope.pop();
                  setTrailingComment(ps.component,@4);
                }
        ;

channelinterfaceheader
        : CHANNELINTERFACE NAME
                {
                  ps.component = (ChannelInterfaceNode *)createNodeWithTag(NED_CHANNEL_INTERFACE, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile);
                  ((ChannelInterfaceNode *)ps.component)->setName(toString(@2));
                  //setComments(ps.component,@1,@2);
                }
           opt_interfaceinheritance
        ;

opt_interfaceinheritance
        : EXTENDS interfacenames
        |
        ;

interfacenames
        : interfacenames ',' NAME
        | NAME
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
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
            opt_gateblock
          '}' opt_semicolon
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.typescope.pop();
                  setTrailingComment(ps.component,@6);
                }
        ;

simplemoduleheader
        : SIMPLE NAME
                {
                  ps.component = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile );
                  ((SimpleModuleNode *)ps.component)->setName(toString(@2));
                  //setComments(ps.component,@1,@2);
                }
          opt_inheritance
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
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
            opt_gateblock
            opt_typeblock
            opt_submodblock
            opt_connblock
          '}' opt_semicolon
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.typescope.pop();
                  setTrailingComment(ps.component,@9);
                }
        ;

compoundmoduleheader
        : MODULE NAME
                {
                  ps.component = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile );
                  ((CompoundModuleNode *)ps.component)->setName(toString(@2));
                  //setComments(ps.component,@1,@2);
                }
          opt_inheritance
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
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
            opt_gateblock
            opt_typeblock
            opt_submodblock
            opt_connblock
          '}' opt_semicolon
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.typescope.pop();
                  setTrailingComment(ps.component,@5);
                }
        ;

networkheader
        : NETWORK NAME
                {
                  ps.component = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile );
                  ((CompoundModuleNode *)ps.component)->setName(toString(@2));
                  ((CompoundModuleNode *)ps.component)->setIsNetwork(true);
                  //setComments(ps.component,@1,@2);
                }
          opt_inheritance
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
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
            opt_gateblock
          '}' opt_semicolon
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                  ps.typescope.pop();
                  setTrailingComment(ps.component,@6);
                }
        ;

moduleinterfaceheader
        : INTERFACE NAME
                {
                  ps.component = (ModuleInterfaceNode *)createNodeWithTag(NED_MODULE_INTERFACE, ps.inTypes ? (NEDElement *)ps.types : (NEDElement *)ps.nedfile);
                  ((ModuleInterfaceNode *)ps.component)->setName(toString(@2));
                  //setComments(ps.component,@1,@2);
                }
           opt_interfaceinheritance
        ;

/*
 * Parameters
 */
opt_paramblock
        : opt_params   /* "parameters" keyword is optional */
        | PARAMETERS ':'
                {
                  ps.parameters->setIsImplicit(false);  /* FIXME flip the default in the dtd? is-EXplicit=false lenne a default? */
                }
          opt_params
        ;

opt_params
        : params
        |
        ;

params
        : params paramsitem
                {
                  //setComments(ps.param,@2);
                }
        | paramsitem
                {
                  //setComments(ps.param,@1);
                }
        ;

paramsitem
        : param
        | paramgroup
        | property
        ;

paramgroup
        : opt_condition '{'
                {
                    if (ps.inGroup)
                       ; // FIXME issue error message: nested groups not allowed
                    ps.inGroup = true;
                    ps.paramgroup = (ParamGroupNode *)createNodeWithTag(NED_PARAM_GROUP, ps.parameters);
                }
          params_nogroup '}'
                {
                    ps.inGroup = false;
                    if ($1)
                        ps.paramgroup->appendChild($1);
                }
        ;

params_nogroup   /* FIXME ELIMINATE NOGROUP STUFF!!!!!! same as params, but without the paramgroup rule */
        : params_nogroup paramsitem_nogroup
                {
                  //setComments(ps.param,@2);
                }
        | paramsitem_nogroup
                {
                  //setComments(ps.param,@1);
                }
        ;

paramsitem_nogroup
        : param
        | property
        ;

param
        : param_typenamevalue
                {
                  ps.propertyscope.push(ps.param);
                }
          opt_inline_properties opt_condition ';'
                {
                  ps.propertyscope.pop();
                  if ($2)
                      ps.paramgroup->appendChild($2);
                }
        ;

/*
 * Parameter
 */
param_typenamevalue
        : paramtype opt_function NAME
                {
                  ps.param = addParameter(ps.inGroup ? (NEDElement *)ps.paramgroup : (NEDElement *)ps.parameters, @3);
                  ps.param->setType(ps.paramType);
                  ps.param->setIsFunction(ps.isFunction);
                }
        | paramtype opt_function NAME '=' paramvalue
                {
                  ps.param = addParameter(ps.inGroup ? (NEDElement *)ps.paramgroup : (NEDElement *)ps.parameters, @3);
                  ps.param->setType(ps.paramType);
                  ps.param->setIsFunction(ps.isFunction);
                  addExpression(ps.param, "value",@5,$5);
                  ps.param->setIsDefault(ps.isDefault);
                }
        | NAME '=' paramvalue
                {
                  ps.param = addParameter(ps.inGroup ? (NEDElement *)ps.paramgroup : (NEDElement *)ps.parameters, @1);
                  addExpression(ps.param, "value",@3,$3);
                  ps.param->setIsDefault(ps.isDefault);
                }
        | NAME
                {
                  ps.param = addParameter(ps.inGroup ? (NEDElement *)ps.paramgroup : (NEDElement *)ps.parameters, @1);
                }
        | TYPENAME '=' paramvalue  /* this is to assign module type with the "<> like Foo" syntax */
                {
                  ps.param = addParameter(ps.inGroup ? (NEDElement *)ps.paramgroup : (NEDElement *)ps.parameters, @1);
                  addExpression(ps.param, "value",@3,$3);
                  ps.param->setIsDefault(ps.isDefault);
                }
        | '/' pattern '/' '=' paramvalue
                {
                  ps.pattern = (PatternNode *)createNodeWithTag(NED_PATTERN, ps.inGroup ? (NEDElement *)ps.paramgroup : (NEDElement *)ps.parameters);
                  ps.pattern->setPattern(toString(@2));
                  addExpression(ps.pattern, "value",@5,$5);
                  ps.pattern->setIsDefault(ps.isDefault);
                }

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

opt_function
        : FUNCTION
                { ps.isFunction = true; }
        |
                { ps.isFunction = false; }
        ;

paramvalue
        : expression
                { $$ = $1; ps.isDefault = false; }
        | DEFAULT '(' expression ')'
                { $$ = $3; ps.isDefault = true; }
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
        | MODULE | SIMPLE | NETWORK | CHANNEL | INTERFACE | CHANNELINTERFACE
        | EXTENDS | LIKE | WITHCPPCLASS
        | DOUBLETYPE | INTTYPE | STRINGTYPE | BOOLTYPE | XMLTYPE | FUNCTION | TYPENAME
        | INPUT_ | OUTPUT_ | INOUT_ | IF | WHERE
        | TYPES | PARAMETERS | GATES | SUBMODULES | CONNECTIONS | ALLOWUNCONNECTED
        | TRUE_ | FALSE_ | THIS_ | DEFAULT | CONST_ | SIZEOF | INDEX_ | XMLDOC
        ;

/*
 * Property
 */
property
        : property_namevalue opt_condition ';'
                {
                  if ($2)
                      ps.property->appendChild($2);
                }
        ;

property_namevalue
        : property_name
        | property_name '(' opt_property_keys ')'
        ;

property_name
        : '@' NAME
                {
                  ps.property = addProperty(ps.propertyscope.top(), toString(@2));
                }
        ;

opt_property_keys
        : property_keys
        |
        ;

property_keys
        : property_keys ',' property_key
        | property_key
        ;

property_key
        : NAME '=' property_value
                {
                  //FIXME use addPropertyValue() function?
                  ps.keyvalue = (KeyValueNode *)createNodeWithTag(NED_KEY_VALUE, ps.property);
                  ps.keyvalue->setKey(toString(@1));
                  ps.keyvalue->setValue(toString(@3));
                }
        | property_value
                {
                  ps.keyvalue = (KeyValueNode *)createNodeWithTag(NED_KEY_VALUE, ps.property);
                  ps.keyvalue->setValue(toString(@1));
                }
        ;

property_value  /* FIXME use LiteralNode and its rules here? */
        : TRUE_
        | FALSE_
        | NAME
        | INTCONSTANT
        | REALCONSTANT
        | STRINGCONSTANT
        | CHARCONSTANT
        | quantity
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
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.blockscope.top());
                  //setComments(ps.gates,@1,@2);
                }
          opt_gates
                {
                }
        ;

opt_gates
        : gates
        |
        ;

gates
        : gates gatesitem
                {
                  //setComments(ps.gate,@2);
                }
        | gatesitem
                {
                  //setComments(ps.gate,@1);
                }
        ;

gatesitem
        : gategroup
        | gate
        ;

gategroup
        : condition '{' gates_nogroup '}'
        | '{' gates_nogroup '}'
        ;

gates_nogroup   /* same as gates, but without the gategroup rule */
        : gates_nogroup gate
                {
                  //setComments(ps.gate,@2);
                }
        | gate
                {
                  //setComments(ps.gate,@1);
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
          opt_inline_properties opt_condition ';'
                {
                  ps.propertyscope.pop();
                  //XXXX add back:
                  /*
                  if ($4)
                      ps.gate->appendChild($4);
                  */
                }
        ;

gate_typenamesize
        : gatetype NAME
        | gatetype NAME '[' ']'
        | gatetype NAME '[' expression ']'
        | NAME
        | NAME '[' ']'
        | NAME '[' expression ']'
        ;

gatetype
        : INPUT_
        | OUTPUT_
        | INOUT_
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
                  // FIXME check for further nested types
                  ps.types = (TypesNode *)createNodeWithTag(NED_TYPES, ps.blockscope.top());
                  //setComments(ps.types,@1,@2);
                  ps.inTypes = true;
                }
           opt_localtypes
                {
                  ps.inTypes = false;
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
                { /*TBD*/ }
        | channeldefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); } /*FIXME won't work*/
        | channelinterfacedefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); }
        | simplemoduledefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); }
        | compoundmoduledefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); }
        | networkdefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); }
        | moduleinterfacedefinition
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.component, @1); }
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
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.blockscope.top());
                  //setComments(ps.submods,@1,@2);
                }
          opt_submodules
                {
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
                  //setComments(ps.submod,@1,@2);
                }
        | submoduleheader '{'
                {
                  ps.blockscope.push(ps.submod);
                  //setComments(ps.submod,@1,@2);
                }
          opt_paramblock
          opt_gateblock
          '}' opt_semicolon
                {
                  ps.blockscope.pop();
                }
        ;

submoduleheader
        : submodulename ':' NAME
                {
                  ps.submod->setType(toString(@3));
                }
        | submodulename ':' likeparam LIKE NAME
                {
                  addLikeParam(ps.submod, "like-param", @3, $3);
                  ps.submod->setLikeType(toString(@5));
                }
        | submodulename ':' likeparam LIKE '*'
                {
                  addLikeParam(ps.submod, "like-param", @3, $3);
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
        | '<' qualifier '.' '@' NAME '>' /* note: qualifier here must be "this" */
                { $$ = NULL; }
        | '<' expression '>' /* XXX this expression is the source of one shift-reduce conflict because it may contain '>' */
                { $$ = $2; }
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
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.blockscope.top());
                  ps.conns->setAllowUnconnected(true);
                  //setComments(ps.conns,@1,@3);
                }
          opt_connections
                {
                }
        | CONNECTIONS ':'
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.blockscope.top());
                  //setComments(ps.conns,@1,@2);
                }
          opt_connections
                {
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
        | connection whereclause ';'
        | connection ';'
        ;

connectiongroup  /* note: semicolon at end is mandatory (cannot be opt_semicolon because it'd be ambiguous where "where" clause in "{a-->b;} where i>0 {c-->d;}" belongs) */
        : whereclause '{' connections_nogroup '}' ';'
        | '{' connections_nogroup '}' whereclause ';'
        | '{' connections_nogroup '}' ';'
        ;

connections_nogroup   /* same as connections, but without the connectiongroup rule */
        : connections_nogroup connectionsitem_nogroup
                {
                  //setComments(ps.gate,@2);
                }
        | connectionsitem_nogroup
                {
                  //setComments(ps.gate,@1);
                }
        ;

connectionsitem_nogroup
        : connection whereclause ';' /* nested "where" is in fact illegal, but let validation find that out */
        | connection ';'
        ;

whereclause
        : WHERE whereitems
        ;

whereitems
        : whereitems ',' whereitem
        | whereitem
        ;

whereitem
        : expression   /* that is, a condition */
        | loop
        ;

loop
        : NAME '=' expression TO expression
                {
                  ps.loop = addLoop(ps.conngroup,@1);
                  addExpression(ps.loop, "from-value",@3,$3);
                  addExpression(ps.loop, "to-value",@5,$5);
                  //setComments(ps.loop,@1,@5);
                }
        ;

/*
 * Connection
 */
connection
        : leftgatespec RIGHTARROW rightgatespec
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                  //setComments(ps.conn,@1,@3);
                }
        | leftgatespec RIGHTARROW channelspec RIGHTARROW rightgatespec
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                  //setComments(ps.conn,@1,@5);
                }
        | leftgatespec LEFTARROW rightgatespec
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                  //setComments(ps.conn,@1,@3);
                }
        | leftgatespec LEFTARROW channelspec LEFTARROW rightgatespec
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                  //setComments(ps.conn,@1,@5);
                }
        | leftgatespec DBLARROW rightgatespec
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_BIDIR);
                  //setComments(ps.conn,@1,@3);
                }
        | leftgatespec DBLARROW channelspec DBLARROW rightgatespec
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_BIDIR);
                  //setComments(ps.conn,@1,@5);
                }
        ;

leftgatespec
        : leftmod '.' leftgate
        | parentleftgate
        ;

leftmod
        : NAME vector
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                  addVector(ps.conn, "src-module-index",@2,$2);
                }
        | NAME
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                }
        ;

leftgate
        : NAME opt_subgate
                {
                  ps.conn->setSrcGate( toString( @1) );
                }
        | NAME vector opt_subgate
                {
                  ps.conn->setSrcGate( toString( @1) );
                  addVector(ps.conn, "src-gate-index",@2,$2);
                }
        | NAME PLUSPLUS opt_subgate
                {
                  ps.conn->setSrcGate( toString( @1) );
                  ps.conn->setSrcGatePlusplus(true);
                }
        ;

parentleftgate
        : NAME opt_subgate
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                }
        | NAME vector opt_subgate
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                  addVector(ps.conn, "src-gate-index",@2,$2);
                }
        | NAME PLUSPLUS opt_subgate
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
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
                }
        | NAME vector opt_subgate
                {
                  ps.conn->setDestGate( toString( @1) );
                  addVector(ps.conn, "dest-gate-index",@2,$2);
                }
        | NAME PLUSPLUS opt_subgate
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGatePlusplus(true);
                }
        ;

parentrightgate
        : NAME opt_subgate
                {
                  ps.conn->setDestGate( toString( @1) );
                }
        | NAME vector opt_subgate
                {
                  ps.conn->setDestGate( toString( @1) );
                  addVector(ps.conn, "dest-gate-index",@2,$2);
                }
        | NAME PLUSPLUS opt_subgate
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGatePlusplus(true);
                }
        ;

opt_subgate
        : '$' NAME
        |
        ;

channelspec
        : channelspec_header
        | channelspec_header '{'
                {
                  ps.parameters = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.chanspec);
                  ps.propertyscope.push(ps.parameters);
                }
            opt_paramblock
          '}'
                {
                  ps.propertyscope.pop();
                  ps.blockscope.pop();
                }
        ;


channelspec_header
        :
                {
                  ps.chanspec = (ChannelSpecNode *)createNodeWithTag(NED_CHANNEL_SPEC, ps.conn);
                }
        | NAME
                {
                  ps.chanspec = (ChannelSpecNode *)createNodeWithTag(NED_CHANNEL_SPEC, ps.conn);
                  ps.chanspec->setType(toString(@1));
                }
        | likeparam LIKE NAME
                {
                  ps.chanspec = (ChannelSpecNode *)createNodeWithTag(NED_CHANNEL_SPEC, ps.conn);
                  addLikeParam(ps.chanspec, "like-param", @1, $1);
                  ps.chanspec->setLikeType(toString(@3));
                }
        | likeparam LIKE '*'
                {
                  ps.chanspec = (ChannelSpecNode *)createNodeWithTag(NED_CHANNEL_SPEC, ps.conn);
                  addLikeParam(ps.chanspec, "like-param", @1, $1);
                }
        ;

/*
 * Condition
 */
opt_condition
        : condition
           { $$ = $1; }
        |
           { $$ = NULL; }
        ;

condition
        : IF expression
                {
                  ps.condition = (ConditionNode *)createNodeWithTag(NED_CONDITION);
                  addExpression(ps.condition, "condition",@2,$2);
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
                  if (ps.parseExpressions) $$ = createExpression($1);
                }
        | xmldocvalue
                {
                  if (ps.parseExpressions) $$ = createExpression($1);
                }
        ;

/*
 * Expressions
 */
xmldocvalue
        : XMLDOC '(' stringliteral ',' stringliteral ')'
                { if (ps.parseExpressions) $$ = createFunction("xmldoc", $3, $5); }
        | XMLDOC '(' stringliteral ')'
                { if (ps.parseExpressions) $$ = createFunction("xmldoc", $3); }
        ;

expr
        : simple_expr
        | '(' expr ')'
                { $$ = $2; }
        | CONST_ '(' expr ')'
                { $$ = $3; /*FIXME*/ }

        | expr '+' expr
                { if (ps.parseExpressions) $$ = createOperator("+", $1, $3); }
        | expr '-' expr
                { if (ps.parseExpressions) $$ = createOperator("-", $1, $3); }
        | expr '*' expr
                { if (ps.parseExpressions) $$ = createOperator("*", $1, $3); }
        | expr '/' expr
                { if (ps.parseExpressions) $$ = createOperator("/", $1, $3); }
        | expr '%' expr
                { if (ps.parseExpressions) $$ = createOperator("%", $1, $3); }
        | expr '^' expr
                { if (ps.parseExpressions) $$ = createOperator("^", $1, $3); }

        | '-' expr
                %prec UMIN
                { if (ps.parseExpressions) $$ = unaryMinus($2); }

        | expr EQ expr
                { if (ps.parseExpressions) $$ = createOperator("==", $1, $3); }
        | expr NE expr
                { if (ps.parseExpressions) $$ = createOperator("!=", $1, $3); }
        | expr '>' expr
                { if (ps.parseExpressions) $$ = createOperator(">", $1, $3); }
        | expr GE expr
                { if (ps.parseExpressions) $$ = createOperator(">=", $1, $3); }
        | expr '<' expr
                { if (ps.parseExpressions) $$ = createOperator("<", $1, $3); }
        | expr LE expr
                { if (ps.parseExpressions) $$ = createOperator("<=", $1, $3); }

        | expr AND expr
                { if (ps.parseExpressions) $$ = createOperator("&&", $1, $3); }
        | expr OR expr
                { if (ps.parseExpressions) $$ = createOperator("||", $1, $3); }
        | expr XOR expr
                { if (ps.parseExpressions) $$ = createOperator("##", $1, $3); }

        | NOT expr
                %prec UMIN
                { if (ps.parseExpressions) $$ = createOperator("!", $2); }

        | expr BIN_AND expr
                { if (ps.parseExpressions) $$ = createOperator("&", $1, $3); }
        | expr BIN_OR expr
                { if (ps.parseExpressions) $$ = createOperator("|", $1, $3); }
        | expr BIN_XOR expr
                { if (ps.parseExpressions) $$ = createOperator("#", $1, $3); }

        | BIN_COMPL expr
                %prec UMIN
                { if (ps.parseExpressions) $$ = createOperator("~", $2); }
        | expr SHIFT_LEFT expr
                { if (ps.parseExpressions) $$ = createOperator("<<", $1, $3); }
        | expr SHIFT_RIGHT expr
                { if (ps.parseExpressions) $$ = createOperator(">>", $1, $3); }
        | expr '?' expr ':' expr
                { if (ps.parseExpressions) $$ = createOperator("?:", $1, $3, $5); }

        | NAME '(' ')'
                { if (ps.parseExpressions) $$ = createFunction(toString(@1)); }
        | NAME '(' expr ')'
                { if (ps.parseExpressions) $$ = createFunction(toString(@1), $3); }
        | NAME '(' expr ',' expr ')'
                { if (ps.parseExpressions) $$ = createFunction(toString(@1), $3, $5); }
        | NAME '(' expr ',' expr ',' expr ')'
                { if (ps.parseExpressions) $$ = createFunction(toString(@1), $3, $5, $7); }
        | NAME '(' expr ',' expr ',' expr ',' expr ')'
                { if (ps.parseExpressions) $$ = createFunction(toString(@1), $3, $5, $7, $9); }
         ;

simple_expr
        : parameter_expr
        | stringliteral
        | boolliteral
        | numliteral
        | special_expr
        ;

parameter_expr
        : NAME
                {
                  // if there's no modifier, might be a loop variable too
                  if (ps.parseExpressions) $$ = createIdent(toString(@1));
                }
        | qualifier '.' NAME
                {
                  // if there's no modifier, might be a loop variable too
                  if (ps.parseExpressions) $$ = createIdent(toString(@1));
                }
        ;

qualifier
        : THIS_
        | NAME  /* submodule name */
        | NAME vector
        ;

boolliteral
        : TRUE_
                { if (ps.parseExpressions) $$ = createLiteral(NED_CONST_BOOL, "true"); }
        | FALSE_
                { if (ps.parseExpressions) $$ = createLiteral(NED_CONST_BOOL, "false"); }
        ;

special_expr
        : INDEX_
                { if (ps.parseExpressions) $$ = createFunction("index"); }
        | INDEX_ '(' ')'
                { if (ps.parseExpressions) $$ = createFunction("index"); }
        | SIZEOF '(' NAME ')'
                { if (ps.parseExpressions) $$ = createFunction("sizeof", createIdent(toString(@3))); }
        | SIZEOF '(' NAME '.' NAME ')'
                { if (ps.parseExpressions) $$ = createFunction("sizeof", createIdent(toString(@5), NULL, toString(@3))); }
        ;

stringliteral
        : STRINGCONSTANT
                { if (ps.parseExpressions) $$ = createLiteral(NED_CONST_STRING, toString(trimQuotes(@1))); /*FIXME store both text&value*/ }
        ;

numliteral
        : INTCONSTANT
                { if (ps.parseExpressions) $$ = createLiteral(NED_CONST_INT, toString(@1)); /*FIXME store both text&value*/ }
        | REALCONSTANT
                { if (ps.parseExpressions) $$ = createLiteral(NED_CONST_DOUBLE, toString(@1)); /*FIXME store both text&value*/ }
        | quantity
                { if (ps.parseExpressions) $$ = createQuantity(toString(@1)); }

        ;

quantity
        : quantity INTCONSTANT NAME
        | quantity REALCONSTANT NAME
        | INTCONSTANT NAME
        | REALCONSTANT NAME
        ;

opt_semicolon : ';' | ;

%%

//----------------------------------------------------------------------
// general bison/flex stuff:
//

char yyfailure[250] = "";
extern int yydebug; /* needed if compiled with yacc --VA */

extern char textbuf[];

int runparse (NEDParser *p,NedFileNode *nf,bool parseexpr, bool storesrc, const char *sourcefname)
{
#if YYDEBUG != 0      /* #if added --VA */
    yydebug = YYDEBUGGING_ON;
#endif
    pos.co = 0;
    pos.li = 1;
    prevpos = pos;

    strcpy (yyfailure, "");

    if (yyin)
        yyrestart( yyin );

    // create parser state and NEDFileNode
    np = p;
    ps.nedfile = nf;
    ps.parseExpressions = parseexpr;
    ps.storeSourceCode = storesrc;
    ps.propertyscope.push(ps.nedfile);
    sourcefilename = sourcefname;

    if (storesrc)
        storeSourceCode(ps.nedfile, np->nedsource->getFullTextPos());

    // parse
    int ret;
    try {
        ret = yyparse();
    } catch (NEDException *e) {
        NEDError(NULL, "internal error while parsing: %s", e->errorMessage());
        delete e;
        return 0;
    }

    // more sanity checks
    if (ps.propertyscope.size()!=1 || ps.propertyscope.top()!=ps.nedfile)
        NEDError(NULL, "internal error while parsing: imbalanced propertyscope");
    if (!ps.blockscope.empty() || !ps.typescope.empty())
        NEDError(NULL, "internal error while parsing: imbalanced blockscope or typescope");

    return ret;
}


void yyerror (const char *s)
{
    if (strlen(s))
        strcpy(yyfailure, s);

    // chop newline
    if (yyfailure[strlen(yyfailure)-1] == '\n')
        yyfailure[strlen(yyfailure)-1] = '\0';

    np->error(yyfailure, pos.li);
}

const char *toString(YYLTYPE pos)
{
    return np->nedsource->get(pos);
}

const char *toString(long l)
{
    static char buf[32];
    sprintf(buf,"%ld", l);
    return buf;
}

NEDElement *createNodeWithTag(int tagcode, NEDElement *parent)
{
    // create via a factory
    NEDElement *e = NEDElementFactory::getInstance()->createNodeWithTag(tagcode);

    // "debug info"
    char buf[200];
    sprintf(buf,"%s:%d",sourcefilename, pos.li);
    e->setSourceLocation(buf);

    // add to parent
    if (parent)
       parent->appendChild(e);

    return e;
}

//
// Properties
//

PropertyNode *addProperty(NEDElement *node, const char *name)
{
    PropertyNode *prop = (PropertyNode *)createNodeWithTag(NED_PROPERTY, node);
    prop->setName(name);
    return prop;
}

PropertyNode *addComponentProperty(NEDElement *node, const char *name)
{
    // add propery under the ParametersNode; create that if not yet exists
    NEDElement *params = node->getFirstChildWithTag(NED_PARAMETERS);
    if (!params)
        params = createNodeWithTag(NED_PARAMETERS, node);
    PropertyNode *prop = (PropertyNode *)createNodeWithTag(NED_PROPERTY, params);
    prop->setName(name);
    return prop;
}

KeyValueNode *addPropertyValue(PropertyNode *prop, const char *key, YYLTYPE valuepos)
{
    KeyValueNode *propval = (KeyValueNode *)createNodeWithTag(NED_KEY_VALUE, prop);
    propval->setKey(key);
    propval->setValue(toString(valuepos));
    return propval;
}

//
// Spec Properties: source code, display string
//

PropertyNode *storeSourceCode(NEDElement *node, YYLTYPE tokenpos)
{
     PropertyNode *prop = addProperty(node, "sourcecode");
     prop->setIsImplicit(true);
     addPropertyValue(prop, NULL, tokenpos);
     return prop;
}

PropertyNode *storeComponentSourceCode(NEDElement *node, YYLTYPE tokenpos)
{
     PropertyNode *prop = addComponentProperty(node, "sourcecode");
     prop->setIsImplicit(true);
     addPropertyValue(prop, NULL, tokenpos);
     return prop;
}

PropertyNode *storeDisplayString(NEDElement *node, YYLTYPE tokenpos)
{
     PropertyNode *prop = addComponentProperty(node, "display");
     prop->setIsImplicit(true);
     addPropertyValue(prop, NULL, tokenpos);
     return prop;
}

PropertyNode *storeComponentDisplayString(NEDElement *node, YYLTYPE tokenpos)
{
     PropertyNode *prop = addComponentProperty(node, "display");
     prop->setIsImplicit(true);
     addPropertyValue(prop, NULL, tokenpos);
     return prop;
}

//
// Comments
//

void setFileComment(NEDElement *node)
{
//XXX    node->setAttribute("file-comment", np->nedsource->getFileComment() );
}

void setBannerComment(NEDElement *node, YYLTYPE tokenpos)
{
//XXX    node->setAttribute("banner-comment", np->nedsource->getBannerComment(tokenpos) );
}

void setRightComment(NEDElement *node, YYLTYPE tokenpos)
{
//XXX    node->setAttribute("right-comment", np->nedsource->getTrailingComment(tokenpos) );
}

void setTrailingComment(NEDElement *node, YYLTYPE tokenpos)
{
//XXX    node->setAttribute("trailing-comment", np->nedsource->getTrailingComment(tokenpos) );
}

void setComments(NEDElement *node, YYLTYPE pos)
{
//XXX    setBannerComment(node, pos);
//XXX    setRightComment(node, pos);
}

void setComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos)
{
    YYLTYPE pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;

//XXX    setBannerComment(node, pos);
//XXX    setRightComment(node, pos);
}

//ChannelAttrNode *addChanAttr(NEDElement *channel, const char *attrname)
//{
//    ChannelAttrNode *chanattr = (ChannelAttrNode *)createNodeWithTag(NED_CHANNEL_ATTR, channel );
//    chanattr->setName( attrname );
//    return chanattr;
//}

ParamNode *addParameter(NEDElement *params, YYLTYPE namepos)
{
   ParamNode *param = (ParamNode *)createNodeWithTag(NED_PARAM,params);
   param->setName( toString( namepos) );
   return param;
}

GateNode *addGate(NEDElement *gates, YYLTYPE namepos, int is_in, int is_vector )
{
   GateNode *gate = (GateNode *)createNodeWithTag(NED_GATE,gates);
   gate->setName( toString( namepos) );
//FIXME   gate->setDirection(is_in ? NED_GATEDIR_INPUT : NED_GATEDIR_OUTPUT);
//FIXME   gate->setIsVector(is_vector);
   return gate;
}

//SubmoduleNode *addSubmodule(NEDElement *submods, YYLTYPE namepos, YYLTYPE typepos,YYLTYPE likeparampos)
//{
//   SubmoduleNode *submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE,submods);
//   submod->setName( toString( namepos) );
////FIXME   submod->setTypeName( toString( typepos) );
////FIXME   submod->setLikeParam( toString( likeparampos) );
//
//   return submod;
//}

LoopNode *addLoop(NEDElement *conngroup, YYLTYPE varnamepos)
{
   LoopNode *loop = (LoopNode *)createNodeWithTag(NED_LOOP,conngroup);
   loop->setParamName( toString( varnamepos) );
   return loop;
}

YYLTYPE trimBrackets(YYLTYPE vectorpos)
{
   // should check it's really brackets that get chopped off
   vectorpos.first_column++;
   vectorpos.last_column--;
   return vectorpos;
}

YYLTYPE trimAngleBrackets(YYLTYPE vectorpos)
{
   // should check it's really angle brackets that get chopped off
   vectorpos.first_column++;
   vectorpos.last_column--;
   return vectorpos;
}

YYLTYPE trimQuotes(YYLTYPE vectorpos)
{
   // should check it's really quotes that get chopped off
   vectorpos.first_column++;
   vectorpos.last_column--;
   return vectorpos;
}

YYLTYPE trimDoubleBraces(YYLTYPE vectorpos)
{
   // should check it's really '{{' and '}}' that get chopped off
   vectorpos.first_column+=2;
   vectorpos.last_column-=2;
   return vectorpos;
}

void addVector(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr)
{
   addExpression(elem, attrname, trimBrackets(exprpos), expr);
}

void addLikeParam(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr)
{
   if (ps.parseExpressions && !expr)
       elem->setAttribute(attrname, toString(trimAngleBrackets(exprpos)));
   else
       addExpression(elem, attrname, trimAngleBrackets(exprpos), expr);
}

void addExpression(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr)
{
   if (ps.parseExpressions) {
       elem->appendChild(expr);
       ((ExpressionNode *)expr)->setTarget(attrname);
   } else {
       elem->setAttribute(attrname, toString(exprpos));
   }
}

void swapConnection(NEDElement *conn)
{
   swapAttributes(conn, "src-module", "dest-module");
   swapAttributes(conn, "src-module-index", "dest-module-index");
   swapAttributes(conn, "src-gate", "dest-gate");
   swapAttributes(conn, "src-gate-index", "dest-gate-index");
   swapAttributes(conn, "src-gate-plusplus", "dest-gate-plusplus");

   swapExpressionChildren(conn, "src-module-index", "dest-module-index");
   swapExpressionChildren(conn, "src-gate-index", "dest-gate-index");
}

void swapAttributes(NEDElement *node, const char *attr1, const char *attr2)
{
   std::string oldv1(node->getAttribute(attr1));
   node->setAttribute(attr1,node->getAttribute(attr2));
   node->setAttribute(attr2,oldv1.c_str());
}

void swapExpressionChildren(NEDElement *node, const char *attr1, const char *attr2)
{
   ExpressionNode *expr1, *expr2;
   for (expr1=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr1; expr1=expr1->getNextExpressionNodeSibling())
      if (!strcmp(expr1->getTarget(),attr1))
          break;
   for (expr2=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr2; expr2=expr2->getNextExpressionNodeSibling())
      if (!strcmp(expr2->getTarget(),attr2))
          break;

   if (expr1) expr1->setTarget(attr2);
   if (expr2) expr2->setTarget(attr1);
}

OperatorNode *createOperator(const char *op, NEDElement *operand1, NEDElement *operand2, NEDElement *operand3)
{
   OperatorNode *opnode = (OperatorNode *)createNodeWithTag(NED_OPERATOR);
   opnode->setName(op);
   opnode->appendChild(operand1);
   if (operand2) opnode->appendChild(operand2);
   if (operand3) opnode->appendChild(operand3);
   return opnode;
}

FunctionNode *createFunction(const char *funcname, NEDElement *arg1, NEDElement *arg2, NEDElement *arg3, NEDElement *arg4)
{
   FunctionNode *funcnode = (FunctionNode *)createNodeWithTag(NED_FUNCTION);
   funcnode->setName(funcname);
   if (arg1) funcnode->appendChild(arg1);
   if (arg2) funcnode->appendChild(arg2);
   if (arg3) funcnode->appendChild(arg3);
   if (arg4) funcnode->appendChild(arg4);
   return funcnode;
}

ExpressionNode *createExpression(NEDElement *expr)
{
   ExpressionNode *expression = (ExpressionNode *)createNodeWithTag(NED_EXPRESSION);
   expression->appendChild(expr);
   return expression;
}

IdentNode *createIdent(const char *param, const char *paramindex, const char *module, const char *moduleindex)
{
   IdentNode *par = (IdentNode *)createNodeWithTag(NED_IDENT);
   par->setName(param);
//FIXME   if (paramindex) par->setParamIndex(paramindex);
   if (module) par->setModule(module);
   if (moduleindex) par->setModuleIndex(moduleindex);
   return par;
}

LiteralNode *createLiteral(int type, const char *value, const char *text)
{
   LiteralNode *c = (LiteralNode *)createNodeWithTag(NED_LITERAL);
   c->setType(type);
   if (value) c->setValue(value);
   if (text) c->setText(text);
   return c;
}

LiteralNode *createQuantity(const char *text)
{
   LiteralNode *c = (LiteralNode *)createNodeWithTag(NED_LITERAL);
   c->setType(NED_CONST_UNIT);
   if (text) c->setText(text);

   double t = 0; //NEDStrToSimtime(text);  // FIXME...
   if (t<0)
   {
       char msg[130];
       sprintf(msg,"invalid time constant '%.100s'",text);
       np->error(msg, pos.li);
   }
   char buf[32];
   sprintf(buf,"%g",t);
   c->setValue(buf);

   return c;
}

NEDElement *unaryMinus(NEDElement *node)
{
    // if not a constant, must appy unary minus operator
    if (node->getTagCode()!=NED_LITERAL)
        return createOperator("-", node);

    LiteralNode *constNode = (LiteralNode *)node;

    // only int and real constants can be negative, string, bool, etc cannot
    if (constNode->getType()!=NED_CONST_INT && constNode->getType()!=NED_CONST_DOUBLE)
    {
       char msg[140];
       sprintf(msg,"unary minus not accepted before '%.100s'",constNode->getValue());
       np->error(msg, pos.li);
       return node;
    }

    // prepend the constant with a '-'
    char *buf = new char[strlen(constNode->getValue())+2];
    buf[0] = '-';
    strcpy(buf+1, constNode->getValue());
    constNode->setValue(buf);
    delete [] buf;

    return node;
}
