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
%token IMPORT, PACKAGE, PROPERTY
%token MODULE, SIMPLE, NETWORK, CHANNEL, INTERFACE, CHANNELINTERFACE
%token EXTENDS, LIKE, WITHCPPCLASS
%token TYPES, PARAMETERS, GATES, SUBMODULES, CONNECTIONS, ALLOWUNCONNECTED
%token DOUBLETYPE, INTTYPE, STRINGTYPE, BOOLTYPE, XMLTYPE, FUNCTION
%token INPUT, OUTPUT, INOUT
%token IF
%token RIGHTARROW, LEFTARROW, DBLARROW, WHILE, TO
%token TRUE_, FALSE_, DEFAULT, CONST_, SIZEOF, INDEX_, XMLDOC

/* Other tokens: identifiers, numeric literals, operators etc */
%token NAME INTCONSTANT REALCONSTANT STRINGCONSTANT CHARCONSTANT
%token SUBMODINDEX PLUSPLUS
%token EQ NE GT GE LS LE
%token AND OR XOR NOT
%token BIN_AND BIN_OR BIN_XOR BIN_COMPL
%token SHIFT_LEFT SHIFT_RIGHT

%token INVALID_CHAR   /* just to generate parse error --VA */

/* Operator precedences (low to high) and associativity */
%left '?' ':'
%left AND OR XOR
%left EQ NE GT GE LS LE
%left BIN_AND BIN_OR BIN_XOR
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-'
%left '*' '/' '%'
%right '^'
%left UMIN NOT BIN_COMPL

%start nedfile


%{

#include <stdio.h>
#include <stdlib.h>
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
void yyerror (char *s);


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
    bool inNetwork;

    /* tmp flags, used with msg fields */
    bool isAbstract;
    bool isReadonly;

    /* NED-II: modules, channels */
    NedFileNode *nedfile;
    WhitespaceNode *whitespace;
    ImportNode *imports;
    PropertydefNode *propertydef;
    ExtendsNode *extends;
    InterfaceNameNode *interfacename;
    NEDElement *module;  // in fact, CompoundModuleNode* or SimpleModule*
    ModuleInterfaceNode *moduleinterface;
    ParametersNode *parameters;
    ParamGroupNode *paramgroup;
    ParamNode *param;
    PropertyNode *property;
    KeyValueNode *keyvalue;
    GatesNode *gates;
    GateGroupNode *gategroup;
    GateNode *gate;
    SubmodulesNode *submods;
    SubmoduleNode *submod;
    ConnectionsNode *conns;
    ConnectionNode *conn;
    ChannelInterfaceNode *channelinterface;
    ChannelNode *channel;
    ConnectionGroupNode *connectiongroup;
    LoopNode *loop;
    ConditionNode *condition;
} ps;

NEDElement *createNodeWithTag(int tagcode, NEDElement *parent=NULL);

void setFileComment(NEDElement *node);
void setBannerComment(NEDElement *node, YYLTYPE tokenpos);
void setRightComment(NEDElement *node, YYLTYPE tokenpos);
void setTrailingComment(NEDElement *node, YYLTYPE tokenpos);
void setComments(NEDElement *node, YYLTYPE pos);
void setComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos);

ChannelAttrNode *addChanAttr(NEDElement *channel, const char *attrname);
ParamNode *addParameter(NEDElement *params, YYLTYPE namepos, int type);
GateNode *addGate(NEDElement *gates, YYLTYPE namepos, int is_in, int is_vector );
SubmoduleNode *addSubmodule(NEDElement *submods, YYLTYPE namepos, YYLTYPE typepos,YYLTYPE likeparampos);
GatesizeNode *addGateSize(NEDElement *gatesizes, YYLTYPE namepos);
SubstparamNode *addSubstparam(NEDElement *substparams, YYLTYPE namepos);
SubstmachineNode *addSubstmachine(NEDElement *substmachines, YYLTYPE namepos);
ConnAttrNode *addConnAttr(NEDElement *conn, const char *attrname);
LoopVarNode *addLoopVar(NEDElement *forloop, YYLTYPE varnamepos);
NetworkNode *addNetwork(NEDElement *nedfile, YYLTYPE namepos, YYLTYPE typepos);
DisplayStringNode *addDisplayString(NEDElement *parent, YYLTYPE dispstrpos);

YYLTYPE trimBrackets(YYLTYPE vectorpos);
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
ParamRefNode *createParamRef(const char *param, const char *paramindex=NULL, const char *module=NULL, const char *moduleindex=NULL);
IdentNode *createIdent(const char *name);
ConstNode *createConst(int type, const char *value, const char *text=NULL);
ConstNode *createTimeConst(const char *text);
NEDElement *createParamRefOrIdent(const char *name);
NEDElement *unaryMinus(NEDElement *node);

void addVector(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);
void addExpression(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);

%}

%%

/*
 * Top-level components
 */
nedfile
        : somedefinitions
        ;

somedefinitions
        : somedefinitions definition
        |
        ;

definition
        : import

        | propertydecl
                { /*TBD*/ }
        | property
                { /*TBD*/ }
        | channeldefinition
                { if (ps.storeSourceCode) ps.channel->setSourceCode(toString(@1)); }
        | channelinterfacedefinition
                { if (ps.storeSourceCode) ps.channelinterf->setSourceCode(toString(@1)); }
        | simplemoduledefinition
                { if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(@1)); }
        | compoundmoduledefinition
                { if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(@1)); }
        | networkdefinition
                { if (ps.storeSourceCode) ps.network->setSourceCode(toString(@1)); }
        | moduleinterfacedefinition
                { if (ps.storeSourceCode) ps.interf->setSourceCode(toString(@1)); }
        ;

/*
 * Import
 */
import
        : IMPORT STRINGCONSTANT ';'
                {
                  /* FIXME */
                  ps.import = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile);
                  ps.import->setFilename(toString(trimQuotes(@1)));
                  setComments(ps.import,@1);
                }
        ;

/*
 * Property declaration
 */
propertydecl
        : PROPERTY '@' NAME ';'
                { /*TBD*/ }
        | PROPERTY '@' NAME '(' opt_propertydecl_keys ')' ';'
                { /*TBD*/ }
        ;

opt_propertydecl_keys
        : propertydecl_keys
        |
        ;

propertydecl_keys
        : propertydecl_keys ',' propertydecl_key
                { /*TBD*/ }
        | propertydecl_key
                { /*TBD*/ }
        ;

propertydecl_key
        : NAME
        ;

/*
 * Property
 */
property
        : '@' NAME
                { /*TBD*/ }
        | '@' NAME '(' opt_property_keys ')' ';'
                { /*TBD*/ }
        ;

opt_property_keys
        : property_keys
        |
        ;

property_keys
        : property_keys ',' property_key
                { /*TBD*/ }
        | property_key
                { /*TBD*/ }
        ;

property_key
        : NAME '=' property_value
                { /*TBD*/ }
        | property_value
                { /*TBD*/ }
        ;

property_value
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
 * Channel
 */
channeldefinition
        : channelheader '{'
            opt_paramblock
          '}' opt_semicolon
                {
                  setTrailingComment(ps.channel,@4);
                }
        ;

channelheader
        : CHANNEL NAME
                {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(@2));
                  setComments(ps.channel,@1,@2);
                }
           opt_inheritance
        ;

opt_inheritance
        :
        | EXTENDS NAME
        | LIKE likenames
        | EXTENDS NAME LIKE likenames
        ;

likenames
        : likenames ',' NAME
        | NAME
        ;

/*
 * Channel Interface
 */
channelinterfacedefinition
        : channelinterfaceheader '{'
            opt_paramblock
          '}' opt_semicolon
                {
                  setTrailingComment(ps.module,@6);
                }
        ;

channelinterfaceheader
        : CHANNELINTERFACE NAME
                {
                  //FIXME
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(@2));
                  setComments(ps.channel,@1,@2);
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
            opt_paramblock
            opt_gateblock
          '}' opt_semicolon
                {
                  setTrailingComment(ps.module,@6);
                }
        ;

simplemoduleheader
        : SIMPLE NAME
                {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(@2));
                  setComments(ps.module,@1,@2);
                }
          opt_inheritance
        ;

/*
 * Module
 */
compoundmoduledefinition
        : compoundmoduleheader '{'
            opt_paramblock
            opt_gateblock
            opt_submodblock
            opt_connblock
          '}' opt_semicolon
                {
                  setTrailingComment(ps.module,@9);
                }
        ;

compoundmoduleheader
        : MODULE NAME
                {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(@2));
                  setComments(ps.module,@1,@2);
                }
          opt_inheritance
        ;

/*
 * Network
 */
networkdefinition
        : networkheader '{'
            opt_paramblock
            opt_gateblock
            opt_submodblock
            opt_connblock
          '}' opt_semicolon
                {
                  setTrailingComment(ps.network,@5);
                  ps.inNetwork=0;
                }
        ;

networkheader
        : NETWORK NAME
                {
                  ps.network = addNetwork(ps.nedfile,@2,@4);
                  setComments(ps.network,@1,@5);
                  ps.inNetwork=1;
                }
          opt_inheritance
        ;

/*
 * Module Interface
 */
moduleinterfacedefinition
        : moduleinterfaceheader '{'
            opt_paramblock
            opt_gateblock
          '}' opt_semicolon
                {
                  setTrailingComment(ps.module,@6);
                }
        ;

moduleinterfaceheader
        : INTERFACE NAME
                {
                  //FIXME
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(@2));
                  setComments(ps.channel,@1,@2);
                }
           opt_interfaceinheritance
        ;

/*
 * Parameters
 */
opt_paramblock
        : paramblock
        |
        ;

paramblock
        : PARAMETERS ':'
                {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,@1,@2);
                }
          opt_params
                {
                }
        ;

opt_params
        : params
        |
        ;

params
        : params paramsitem
                {
                  setComments(ps.param,@2);
                }
        | paramsitem
                {
                  setComments(ps.param,@1);
                }
        ;

paramsitem
        : param
        | paramgroup
        | property
        ;

paramgroup
        : condition '{' params_nogroup '}'
        | '{' params_nogroup '}'
        ;

params_nogroup   /* same as params, but without the paramgroup rule */
        : params_nogroup paramsitem_nogroup
                {
                  setComments(ps.param,@2);
                }
        | paramsitem_nogroup
                {
                  setComments(ps.param,@1);
                }
        ;

paramsitem_nogroup
        : param
        | property
        ;

/*
 * Parameter
 */
param
        : paramtype opt_function NAME opt_properties ';'
                {
                  //FIXME
                  ps.param = addParameter(ps.params,@1,TYPE_NUMERIC);
                }
        | paramtype opt_function NAME '=' expression opt_properties ';'
        | NAME '=' expression opt_properties ';'
        | qualifier '.' NAME '=' expression opt_properties ';'
        ;

paramtype
        : DOUBLETYPE
        | INTTYPE
        | STRINGTYPE
        | BOOLTYPE
        | XMLTYPE
        ;

opt_properties
        : properties
        |
        ;

properties
        : properties property
        | property
        ;

opt_function
        : FUNCTION
        |
        ;

qualifier
        : FIXME_TODO
        ;

/*
 * Condition
 */
condition
        : IF expression
                {
                  //FIXME
                }

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
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,@1,@2);
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
                  setComments(ps.gate,@2);
                }
        | gatesitem
                {
                  setComments(ps.gate,@1);
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
                  setComments(ps.gate,@2);
                }
        | gate
                {
                  setComments(ps.gate,@1);
                }
        ;

/*
 * Gate
 */
gate
        : gatetype NAME opt_properties ';'
        | gatetype NAME '[' ']' opt_properties ';'
        | gatetype NAME '[' expression ']' opt_properties ';'
        | NAME opt_properties ';'
        | NAME '[' expression ']' opt_properties ';'
        ;

gatetype
        : INPUT
        | OUTPUT
        | INOUT
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
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,@1,@2);
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
        : submoduleheader '{'
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, NULLPOS);
                  setComments(ps.submod,@1,@4);
                }
          opt_paramblock
          opt_gateblock
          '}' opt_semicolon
        ;

submoduleheader
        : NAME ':' NAME
        | NAME vector ':' NAME
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, NULLPOS);
                  addVector(ps.submod, "vector-size",@4,$4);
                  setComments(ps.submod,@1,@5);
                }
        | NAME ':' likeparam LIKE NAME
        | NAME vector ':' likeparam  LIKE NAME
        ;

likeparam
        : '<' '>'
        | '<' '@' NAME '>'
        | '<' qualifier '.' '@' NAME '>'  /* note: qualifier here must be "this" */
        | '<' expression '>'
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
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,@1,@3);
                }
          opt_connections
                {
                }
        | CONNECTIONS ':'
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,@1,@2);
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
        | connection whereclause
        | connection
        ;

connectiongroup
        : whereclause '{' connections_nogroup '}'
        | '{' connections_nogroup '}' whereclause
        | '{' connections_nogroup '}'
        ;

connections_nogroup   /* same as connections, but without the connectiongroup rule */
        : connections_nogroup connectionsitem_nogroup
                {
                  setComments(ps.gate,@2);
                }
        | connectionsitem_nogroup
                {
                  setComments(ps.gate,@1);
                }
        ;

connectionsitem_nogroup
        : connection whereclause
        | connection
        ;

whereclause
        : WHERE whereitems
        ;

whereitems
        : whereitems ',' whereitem
        | whereitem
        ;

whereitem
        : condition
        | loopvar
        ;

loopvar
        : NAME '=' expression TO expression
                {
                  ps.loopvar = addLoopVar(ps.forloop,@1);
                  addExpression(ps.loopvar, "from-value",@3,$3);
                  addExpression(ps.loopvar, "to-value",@5,$5);
                  setComments(ps.loopvar,@1,@5);
                }
        ;

/*
 * Connection
 */
connection
        : leftgatespec RIGHT_ARROW rightgatespec ';'
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,@1,@5);
                }
        | leftgatespec RIGHT_ARROW channeldescr RIGHT_ARROW rightgatespec ';'
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,@1,@7);
                }
        | leftgatespec LEFT_ARROW rightgatespec ';'
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,@1,@5);
                }
        | leftgatespec LEFT_ARROW channeldescr LEFT_ARROW rightgatespec ';'
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,@1,@7);
                }
        | leftgatespec DBLARROW rightgatespec ';'
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,@1,@5);
                }
        | leftgatespec DBLARROW channeldescr DBLARROW rightgatespec ';'
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,@1,@7);
                }
        ;

leftgatespec
        : leftmod '.' leftgate
        | parentleftgate
        ;

leftmod
        : NAME vector
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                  addVector(ps.conn, "src-module-index",@2,$2);
                }
        | NAME
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                }
        ;

leftgate
        : NAME vector
                {
                  ps.conn->setSrcGate( toString( @1) );
                  addVector(ps.conn, "src-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setSrcGate( toString( @1) );
                }
        | NAME PLUSPLUS
                {
                  ps.conn->setSrcGate( toString( @1) );
                  ps.conn->setSrcGatePlusplus(true);
                }
        ;

parentleftgate
        : NAME vector
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                  addVector(ps.conn, "src-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                }
        | NAME PLUSPLUS
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
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
        : NAME vector
                {
                  ps.conn->setDestModule( toString(@1) );
                  addVector(ps.conn, "dest-module-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setDestModule( toString(@1) );
                }
        ;

rightgate
        : NAME vector
                {
                  ps.conn->setDestGate( toString( @1) );
                  addVector(ps.conn, "dest-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setDestGate( toString( @1) );
                }
        | NAME PLUSPLUS
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGatePlusplus(true);
                }
        ;

parentrightgate
        : NAME vector
                {
                  ps.conn->setDestGate( toString( @1) );
                  addVector(ps.conn, "dest-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setDestGate( toString( @1) );
                }
        | NAME PLUSPLUS
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGatePlusplus(true);
                }
        ;


channeldescr
        : NAME
                {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",@1,createExpression(createConst(NED_CONST_STRING, toString(@1))));
                }
        | NAME expression
                {
                  ps.connattr = addConnAttr(ps.conn,toString(@1));
                  addExpression(ps.connattr, "value",@2,$2);
                }
        | channeldescr NAME expression
                {
                  ps.connattr = addConnAttr(ps.conn,toString(@2));
                  addExpression(ps.connattr, "value",@3,$3);
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
        | inputvalue
                {
                  if (ps.parseExpressions) $$ = createExpression($1);
                }
        | xmldocvalue
                {
                  if (ps.parseExpressions) $$ = createExpression($1);
                }
        ;

/*
 * Expressions (3 shift-reduce conflicts here)
 */

inputvalue
        : INPUT_ '(' expr ',' expr ')'
                { if (ps.parseExpressions) $$ = createFunction("input", $3, $5); }
        | INPUT_ '(' expr ')'
                { if (ps.parseExpressions) $$ = createFunction("input", $3); }
        | INPUT_ '(' ')'
                { if (ps.parseExpressions) $$ = createFunction("input"); }
        | INPUT_
                { if (ps.parseExpressions) $$ = createFunction("input"); }
        ;

xmldocvalue
        : XMLDOC '(' stringconstant ',' stringconstant ')'
                { if (ps.parseExpressions) $$ = createFunction("xmldoc", $3, $5); }
        | XMLDOC '(' stringconstant ')'
                { if (ps.parseExpressions) $$ = createFunction("xmldoc", $3); }
        ;

expr
        : simple_expr
        | '(' expr ')'
                { $$ = $2; }

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
        | expr GT expr
                { if (ps.parseExpressions) $$ = createOperator(">", $1, $3); }
        | expr GE expr
                { if (ps.parseExpressions) $$ = createOperator(">=", $1, $3); }
        | expr LS expr
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
        | string_expr
        | boolconst_expr
        | numconst_expr
        | special_expr
        ;

parameter_expr
        : NAME
                {
                  // if there's no modifier, might be a loop variable too
                  $$ = createParamRefOrIdent(toString(@1));
                }
        | REF NAME
                {
                  $$ = createParamRef(toString(@2));
                  ((ParamRefNode *)$$)->setIsRef(true);
                }
        | REF ANCESTOR NAME
                {
                  $$ = createParamRef(toString(@3));
                  ((ParamRefNode *)$$)->setIsRef(true);
                  ((ParamRefNode *)$$)->setIsAncestor(true);
                }
        | ANCESTOR REF NAME
                {
                  $$ = createParamRef(toString(@3));
                  ((ParamRefNode *)$$)->setIsRef(true);
                  ((ParamRefNode *)$$)->setIsAncestor(true);
                }
        | ANCESTOR NAME
                {
                  $$ = createParamRef(toString(@2));
                  ((ParamRefNode *)$$)->setIsAncestor(true);
                }
        ;

string_expr
        : stringconstant
        ;

boolconst_expr
        : TRUE_
                { $$ = createConst(NED_CONST_BOOL, "true"); }
        | FALSE_
                { $$ = createConst(NED_CONST_BOOL, "false"); }
        ;

numconst_expr
        : numconst
        ;

special_expr
        : SUBMODINDEX
                { if (ps.parseExpressions) $$ = createFunction("index"); }
        | SUBMODINDEX '(' ')'
                { if (ps.parseExpressions) $$ = createFunction("index"); }
        | SIZEOF '(' NAME ')'
                { if (ps.parseExpressions) $$ = createFunction("sizeof", createIdent(toString(@3))); }
        ;

stringconstant
        : STRINGCONSTANT
                { $$ = createConst(NED_CONST_STRING, toString(trimQuotes(@1))); }
        ;

numconst
        : INTCONSTANT
                { $$ = createConst(NED_CONST_INT, toString(@1)); }
        | REALCONSTANT
                { $$ = createConst(NED_CONST_REAL, toString(@1)); }
        | quantity
                { $$ = createTimeConst(toString(@1)); }

        ;

quantity
        : quantity INTCONSTANT NAME
        | quantity REALCONSTANT NAME
        | INTCONSTANT NAME
        | REALCONSTANT NAME
        ;

opt_semicolon : ';' | ;

comma_or_semicolon : ',' | ';' ;

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

    np = p;
    ps.nedfile = nf;
    ps.parseExpressions = parseexpr;
    ps.storeSourceCode = storesrc;
    sourcefilename = sourcefname;

    if (storesrc)
        ps.nedfile->setSourceCode(np->nedsource->getFullText());

    try {
        return yyparse();
    } catch (NEDException *e) {
        NEDError(NULL, "internal error while parsing: %s", e->errorMessage());
        delete e;
        return 0;
    }
}


void yyerror (char *s)
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

void setFileComment(NEDElement *node)
{
    node->setAttribute("file-comment", np->nedsource->getFileComment() );
}

void setBannerComment(NEDElement *node, YYLTYPE tokenpos)
{
    node->setAttribute("banner-comment", np->nedsource->getBannerComment(tokenpos) );
}

void setRightComment(NEDElement *node, YYLTYPE tokenpos)
{
    node->setAttribute("right-comment", np->nedsource->getTrailingComment(tokenpos) );
}

void setTrailingComment(NEDElement *node, YYLTYPE tokenpos)
{
    node->setAttribute("trailing-comment", np->nedsource->getTrailingComment(tokenpos) );
}

void setComments(NEDElement *node, YYLTYPE pos)
{
    setBannerComment(node, pos);
    setRightComment(node, pos);
}

void setComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos)
{
    YYLTYPE pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;

    setBannerComment(node, pos);
    setRightComment(node, pos);
}

ChannelAttrNode *addChanAttr(NEDElement *channel, const char *attrname)
{
    ChannelAttrNode *chanattr = (ChannelAttrNode *)createNodeWithTag(NED_CHANNEL_ATTR, channel );
    chanattr->setName( attrname );
    return chanattr;
}

ParamNode *addParameter(NEDElement *params, YYLTYPE namepos, int type)
{
   const char *s;
   switch (type)
   {
       case TYPE_NUMERIC:   s = "numeric"; break;
       case TYPE_CONST_NUM: s = "numeric const"; break;
       case TYPE_STRING:    s = "string"; break;
       case TYPE_BOOL:      s = "bool"; break;
       case TYPE_XML:       s = "xml"; break;
       case TYPE_ANYTYPE:   s = "anytype"; break;
       default: s="?";
   }

   ParamNode *param = (ParamNode *)createNodeWithTag(NED_PARAM,params);
   param->setName( toString( namepos) );
   param->setDataType( s );
   return param;
}

GateNode *addGate(NEDElement *gates, YYLTYPE namepos, int is_in, int is_vector )
{
   GateNode *gate = (GateNode *)createNodeWithTag(NED_GATE,gates);
   gate->setName( toString( namepos) );
   gate->setDirection(is_in ? NED_GATEDIR_INPUT : NED_GATEDIR_OUTPUT);
   gate->setIsVector(is_vector);
   return gate;
}

SubmoduleNode *addSubmodule(NEDElement *submods, YYLTYPE namepos, YYLTYPE typepos,YYLTYPE likeparampos)
{
   SubmoduleNode *submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE,submods);
   submod->setName( toString( namepos) );
   submod->setTypeName( toString( typepos) );
   submod->setLikeParam( toString( likeparampos) );

   return submod;
}

GatesizeNode *addGateSize(NEDElement *gatesizes, YYLTYPE namepos)
{
   GatesizeNode *gatesize = (GatesizeNode *)createNodeWithTag(NED_GATESIZE,gatesizes);
   gatesize->setName( toString( namepos) );
   return gatesize;
}

SubstparamNode *addSubstparam(NEDElement *substparams, YYLTYPE namepos)
{
   SubstparamNode *substparam = (SubstparamNode *)createNodeWithTag(NED_SUBSTPARAM,substparams);
   substparam->setName( toString( namepos) );
   return substparam;
}

SubstmachineNode *addSubstmachine(NEDElement *substmachines, YYLTYPE namepos)
{
   SubstmachineNode *substmachine = (SubstmachineNode *)createNodeWithTag(NED_SUBSTMACHINE,substmachines);
   substmachine->setName( toString( namepos) );
   return substmachine;
}

LoopVarNode *addLoopVar(NEDElement *forloop, YYLTYPE varnamepos)
{
   LoopVarNode *loopvar = (LoopVarNode *)createNodeWithTag(NED_LOOP_VAR,forloop);
   loopvar->setParamName( toString( varnamepos) );
   return loopvar;
}

ConnAttrNode *addConnAttr(NEDElement *conn, const char *attrname)
{
    ConnAttrNode *connattr = (ConnAttrNode *)createNodeWithTag(NED_CONN_ATTR,conn);
    connattr->setName( attrname );
    return connattr;
}


NetworkNode *addNetwork(NEDElement *nedfile, YYLTYPE namepos, YYLTYPE typepos)
{
   NetworkNode *network = (NetworkNode *)createNodeWithTag(NED_NETWORK,nedfile);
   network->setName( toString( namepos) );
   network->setTypeName( toString( typepos) );
   return network;
}

DisplayStringNode *addDisplayString(NEDElement *parent, YYLTYPE dispstrpos)
{
   DisplayStringNode *dispstr = (DisplayStringNode *)createNodeWithTag(NED_DISPLAY_STRING,parent);
   dispstr->setValue( toString( trimQuotes(dispstrpos)) );
   return dispstr;
}

YYLTYPE trimBrackets(YYLTYPE vectorpos)
{
   // should check it's really brackets that get chopped off
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

ParamRefNode *createParamRef(const char *param, const char *paramindex, const char *module, const char *moduleindex)
{
   ParamRefNode *par = (ParamRefNode *)createNodeWithTag(NED_PARAM_REF);
   par->setParamName(param);
   if (paramindex) par->setParamIndex(paramindex);
   if (module) par->setModule(module);
   if (moduleindex) par->setModuleIndex(moduleindex);
   return par;
}

IdentNode *createIdent(const char *name)
{
   IdentNode *ident = (IdentNode *)createNodeWithTag(NED_IDENT);
   ident->setName(name);
   return ident;
}

ConstNode *createConst(int type, const char *value, const char *text)
{
   ConstNode *c = (ConstNode *)createNodeWithTag(NED_CONST);
   c->setType(type);
   if (value) c->setValue(value);
   if (text) c->setText(text);
   return c;
}

ConstNode *createTimeConst(const char *text)
{
   ConstNode *c = (ConstNode *)createNodeWithTag(NED_CONST);
   c->setType(NED_CONST_TIME);
   if (text) c->setText(text);

   double t = NEDStrToSimtime(text);
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

NEDElement *createParamRefOrIdent(const char *name)
{
    // determine if 'name' can be a loop variable. if so, use createIdent()
    bool isvar = false;
    if (ps.inLoop)
    {
        for (NEDElement *child=ps.forloop->getFirstChildWithTag(NED_LOOP_VAR); child; child=child->getNextSiblingWithTag(NED_LOOP_VAR))
        {
            LoopVarNode *loopvar = (LoopVarNode *)child;
            if (!strcmp(loopvar->getParamName(),name))
                isvar = true;
        }
    }
    return isvar ? (NEDElement *)createIdent(name) : (NEDElement *)createParamRef(name);
}

NEDElement *unaryMinus(NEDElement *node)
{
    // if not a constant, must appy unary minus operator
    if (node->getTagCode()!=NED_CONST)
        return createOperator("-", node);

    ConstNode *constNode = (ConstNode *)node;

    // only int and real constants can be negative, string, bool, etc cannot
    if (constNode->getType()!=NED_CONST_INT && constNode->getType()!=NED_CONST_REAL)
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
