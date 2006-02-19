/*===============================================================
 * File: ned.y
 *
 *  Grammar for OMNeT++ NED.
 *
 *  Author: Andras Varga
 *
 *  Based on code from nedc.
 *
 *  nedc credits:
 *     original code:
 *       Jan Heijmans, Alex Paalvast, Robert van der Leij, 1996
 *       (nedc was originally named jar, for Jan+Alex+Robert)
 *     modifications
 *       Gabor Lencse 1998
 *     restructuring, maintenance, new features, etc:
 *       Andras Varga 1996-2001
 *
 *=============================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 1992,2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


%token INCLUDE SIMPLE
%token CHANNEL /*DELAY ERROR DATARATE are no longer tokens*/
%token MODULE PARAMETERS GATES GATESIZES SUBMODULES CONNECTIONS DISPLAY
%token IN OUT
%token NOCHECK LEFT_ARROW RIGHT_ARROW
%token FOR TO DO IF LIKE
%token NETWORK
%token ENDSIMPLE ENDMODULE ENDCHANNEL
%token ENDNETWORK ENDFOR
%token MACHINES ON
%token CHANATTRNAME   /* only needed to parse old-syntax NED */

%token INTCONSTANT REALCONSTANT NAME STRINGCONSTANT CHARCONSTANT
%token TRUE_ FALSE_
%token INPUT_ XMLDOC
%token REF ANCESTOR
%token CONSTDECL NUMERICTYPE STRINGTYPE BOOLTYPE XMLTYPE ANYTYPE

%token CPLUSPLUS CPLUSPLUSBODY
%token MESSAGE CLASS STRUCT ENUM NONCOBJECT
%token EXTENDS FIELDS PROPERTIES ABSTRACT READONLY
%token CHARTYPE SHORTTYPE INTTYPE LONGTYPE DOUBLETYPE UNSIGNED_

%token SIZEOF SUBMODINDEX PLUSPLUS
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

%start networkdescription


%{

/*
 * Note:
 * This file contains about 3 shift-reduce conflicts around 'expression'.
 * The rest (7-8 shift-reduce conflicts) are because for some reason
 * (without reason, actually) the grammar has difficulty recognizing
 * submodule boundaries. You can verify this by temporarily allowing only
 * one submodule (in rule for 'opt_submodules', replace 'submodules' with
 * 'submodule'). I couldn't figure out how to solve this yet.
 *
 * Plus one (real) ambiguity exists between submodule display string
 * and compound module display string if no connections are present.
 *
 * bison's "%expect nn" option cannot be used to suppress the
 * warning message because %expect is not recognized by yacc
 */


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
void yyerror (const char *s);


#include "nedparser.h"
#include "nedfilebuffer.h"
#include "nedelements.h"
#include "nedutil.h"

static YYLTYPE NULLPOS={0,0,0,0,0,0};

static const char *sourcefilename;

NEDParser *np; // FIXME will clash with new one

struct ParserState
{
    bool parseExpressions;
    bool storeSourceCode;
    bool inLoop;
    bool inNetwork;

    /* tmp flags, used with msg fields */
    bool isAbstract;
    bool isReadonly;

    /* NED-I: modules, channels, networks */
    NedFileNode *nedfile;
    WhitespaceNode *whitespace;
    ImportNode *import;
    //PropertyDeclNode *propertydecl;
    //ExtendsNode *extends;
    //InterfaceNameNode *interfacename;
    ChannelNode *channel;
    NEDElement *module;  // in fact, CompoundModuleNode* or SimpleModule*
    //ModuleInterfaceNode *moduleinterface;
    ParametersNode *params;
    //ParamGroupNode *paramgroup;
    ParamNode *param;
    ParametersNode *substparams;
    ParamGroupNode *substparamgroup;
    ParamNode *substparam;
    PropertyNode *property;
    PropertyKeyNode *propkey;
    GatesNode *gates;
    GateNode *gate;
    GatesNode *gatesizes;
    GateGroupNode *gatesizesgroup;
    GateNode *gatesize;
    SubmodulesNode *submods;
    SubmoduleNode *submod;
    ConnectionsNode *conns;
    ConnectionGroupNode *conngroup;
    ConnectionNode *conn;
    ChannelSpecNode *chanspec;
    WhereNode *where;
    LoopNode *loop;
    ConditionNode *condition;

    /* NED-II: message subclassing */
    CplusplusNode *cplusplus;
    StructDeclNode *structdecl;
    ClassDeclNode *classdecl;
    MessageDeclNode *messagedecl;
    EnumDeclNode *enumdecl;
    EnumNode *enump;
    MessageNode *messagep;
    ClassNode *classp;
    StructNode *structp;
    NEDElement *msgclassorstruct;
    EnumFieldsNode *enumfields;
    EnumFieldNode *enumfield;
    PropertiesNode *properties;
    MsgpropertyNode *msgproperty;
    FieldsNode *fields;
    FieldNode *field;
} ps;

NEDElement *createNodeWithTag(int tagcode, NEDElement *parent=NULL);

PropertyNode *addProperty(NEDElement *node, const char *name);  // directly under the node
PropertyNode *addComponentProperty(NEDElement *node, const char *name); // into ParametersNode child of node

PropertyNode *storeSourceCode(NEDElement *node, YYLTYPE tokenpos);  // directly under the node
PropertyNode *storeComponentSourceCode(NEDElement *node, YYLTYPE tokenpos); // into ParametersNode child

void setFileComment(NEDElement *node);
void setBannerComment(NEDElement *node, YYLTYPE tokenpos);
void setRightComment(NEDElement *node, YYLTYPE tokenpos);
void setTrailingComment(NEDElement *node, YYLTYPE tokenpos);
void setComments(NEDElement *node, YYLTYPE pos);
void setComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos);

ParamNode *addParameter(NEDElement *params, YYLTYPE namepos);
GateNode *addGate(NEDElement *gates, YYLTYPE namepos);
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
LiteralNode *createLiteral(int type, YYLTYPE valuepos, YYLTYPE textpos);
LiteralNode *createQuantity(const char *text);
NEDElement *unaryMinus(NEDElement *node);

void addVector(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);
void addLikeParam(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);
void addExpression(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);

%}

%%

/*
 * Top-level components (no shift-reduce conflict here)
 */
networkdescription
        : somedefinitions
        ;

somedefinitions
        : somedefinitions definition
        |
        ;

definition
        : import

        | channeldefinition_old
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.channel, @1); }
        | simpledefinition_old
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.module, @1); }
        | moduledefinition_old
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.module, @1); }
        | networkdefinition_old
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.module, @1); }

        | cplusplus
        | struct_decl
        | class_decl
        | message_decl
        | enum_decl

        | enum
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.enump, @1); }
        | message
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.messagep, @1); }
        | class
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.classp, @1); }
        | struct
                { if (ps.storeSourceCode) storeComponentSourceCode(ps.structp, @1); }
        ;

/*
 * Imports (no shift-reduce conflict here)
 */
import
        : INCLUDE
          filenames ';'
        ;

filenames
        : filenames ',' filename
        | filename
        ;

filename
        : STRINGCONSTANT
                {
                  ps.import = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  ps.import->setFilename(toString(trimQuotes(@1)));
                  setComments(ps.import,@1);
                }
        ;

/*
 * Channel - old syntax
 */
channeldefinition_old
        : channelheader_old opt_channelattrblock_old endchannel_old
        ;

channelheader_old
        : CHANNEL NAME
                {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(@2));
                  setComments(ps.channel,@1,@2);
                }
        ;

opt_channelattrblock_old
        :
        | channelattrblock_old
        ;

channelattrblock_old
        : channelattrblock_old CHANATTRNAME expression opt_semicolon
                {
                  ps.chanattr = addChanAttr(ps.channel,toString(@2));
                  addExpression(ps.chanattr, "value",@3,$3);
                  setComments(ps.channel,@2,@3);
                }
        | CHANATTRNAME expression opt_semicolon
                {
                  ps.chanattr = addChanAttr(ps.channel,toString(@1));
                  addExpression(ps.chanattr, "value",@2,$2);
                  setComments(ps.channel,@1,@2);
                }
        ;

endchannel_old
        : ENDCHANNEL NAME opt_semicolon
                {
                  setTrailingComment(ps.channel,@2);
                }
        | ENDCHANNEL opt_semicolon
                {
                  setTrailingComment(ps.channel,@1);
                }
        ;

/*
 * Simple module - old syntax
 */
simpledefinition_old
        : simpleheader_old
            opt_paramblock_old
            opt_gateblock_old
          endsimple_old
        ;

simpleheader_old
        : SIMPLE NAME
                {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(@2));
                  setComments(ps.module,@1,@2);
                }
        ;

endsimple_old
        : ENDSIMPLE NAME opt_semicolon
                {
                  setTrailingComment(ps.module,@2);
                }
        | ENDSIMPLE opt_semicolon
                {
                  setTrailingComment(ps.module,@1);
                }
        ;

/*
 * Module - old syntax
 */
moduledefinition_old
        : moduleheader_old
            opt_paramblock_old
            opt_gateblock_old
            opt_submodblock_old
            opt_connblock_old
            opt_displayblock_old
          endmodule_old
        ;

moduleheader_old
        : MODULE NAME
                {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(@2));
                  setComments(ps.module,@1,@2);
                }
        ;

endmodule_old
        : ENDMODULE NAME opt_semicolon
                {
                  setTrailingComment(ps.module,@2);
                }
        | ENDMODULE opt_semicolon
                {
                  setTrailingComment(ps.module,@1);
                }
        ;

/*
 * Display block - old syntax
 */
opt_displayblock_old
        : displayblock_old
        |
        ;

displayblock_old
        : DISPLAY ':' STRINGCONSTANT ';'
                {
                  ps.property = addComponentProperty(ps.module, "display");
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes(@3), @3);
                  ps.propkey->appendChild(literal);
                }
        ;

/*
 * Parameters - old syntax
 */
opt_paramblock_old
        : paramblock_old
        |
        ;

paramblock_old
        : PARAMETERS ':'
                {
                  ps.params = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.module );
                  setComments(ps.params,@1,@2);
                }
          opt_parameters_old
                {
                }
        ;

opt_parameters_old
        : parameters_old ';'
        |
        ;

parameters_old
        : parameters_old ',' parameter_old  /* comma as separator */
                {
                  setComments(ps.param,@3);
                }
        | parameter_old
                {
                  setComments(ps.param,@1);
                }
        ;

/*
 * Parameter
 */
parameter_old
        : NAME
                {
                  ps.param = addParameter(ps.params, @1);
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  ps.param->setIsFunction(true); // because CONST is missing
                }
        | NAME ':' NUMERICTYPE
                {
                  ps.param = addParameter(ps.params, @1);
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  ps.param->setIsFunction(true); // because CONST is missing
                }
        | CONSTDECL NAME /* for compatibility */
                {
                  ps.param = addParameter(ps.params, @1);
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                }
        | NAME ':' CONSTDECL
                {
                  ps.param = addParameter(ps.params, @1);
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                }
        | NAME ':' CONSTDECL NUMERICTYPE
                {
                  ps.param = addParameter(ps.params, @1);
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                }
        | NAME ':' NUMERICTYPE CONSTDECL
                {
                  ps.param = addParameter(ps.params, @1);
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                }
        | NAME ':' STRINGTYPE
                {
                  ps.param = addParameter(ps.params, @1);
                  ps.param->setType(NED_PARTYPE_STRING);
                }
        | NAME ':' BOOLTYPE
                {
                  ps.param = addParameter(ps.params, @1);
                  ps.param->setType(NED_PARTYPE_BOOL);
                }
        | NAME ':' XMLTYPE
                {
                  ps.param = addParameter(ps.params, @1);
                  ps.param->setType(NED_PARTYPE_XML);
                }
        | NAME ':' ANYTYPE
                {
                  NEDError(ps.params,"type 'anytype' no longer supported");
                }
        ;

/*
 * Gates - old syntax
 */
opt_gateblock_old
        : gateblock_old
        |
        ;

gateblock_old
        : GATES ':'
                {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,@1,@2);
                }
          opt_gates_old
                {
                }
        ;

opt_gates_old
        : gates_old
        |
        ;

gates_old
        : gates_old IN gatesI_old ';'
        | IN  gatesI_old ';'
        | gates_old OUT gatesO_old ';'
        | OUT gatesO_old ';'
        ;

gatesI_old
        : gatesI_old ',' gateI_old
        | gateI_old
        ;

gateI_old
        : NAME '[' ']'
                {
                  ps.gate = addGate(ps.gates, @1);
                  ps.gate->setType(NED_GATETYPE_INPUT);
                  ps.gate->setIsVector(true);
                  //setComments(ps.gate,@1,@3);
                }
        | NAME
                {
                  ps.gate = addGate(ps.gates, @1);
                  ps.gate->setType(NED_GATETYPE_INPUT);
                  //setComments(ps.gate,@1);
                }
        ;

gatesO_old
        : gatesO_old ',' gateO_old
        | gateO_old
        ;

gateO_old
        : NAME '[' ']'
                {
                  ps.gate = addGate(ps.gates, @1);
                  ps.gate->setType(NED_GATETYPE_OUTPUT);
                  ps.gate->setIsVector(true);
                  //setComments(ps.gate,@1,@3);
                }
        | NAME
                {
                  ps.gate = addGate(ps.gates, @1);
                  ps.gate->setType(NED_GATETYPE_OUTPUT);
                  //setComments(ps.gate,@1,@3);
                }
        ;

/*
 * Submodules - old syntax
 */
opt_submodblock_old
        : submodblock_old
        |
        ;

submodblock_old
        : SUBMODULES ':'
                {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,@1,@2);
                }
          opt_submodules_old
                {
                }
        ;

opt_submodules_old
        : submodules_old
        |
        ;

submodules_old
        : submodules_old submodule_old
        | submodule_old
        ;

submodule_old
        : NAME ':' NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, NULLPOS);
                  setComments(ps.submod,@1,@4);
                }
          submodule_body_old
                {
                }
        | NAME ':' NAME vector opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, NULLPOS);
                  addVector(ps.submod, "vector-size",@4,$4);
                  setComments(ps.submod,@1,@5);
                }
          submodule_body_old
                {
                }
        | NAME ':' NAME LIKE NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @5, @3);
                  setComments(ps.submod,@1,@6);
                }
          submodule_body_old
                {
                }
        | NAME ':' NAME vector LIKE NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @6, @3);
                  addVector(ps.submod, "vector-size",@4,$4);
                  setComments(ps.submod,@1,@7);
                }
          submodule_body_old
                {
                }
        ;

submodule_body_old
        : opt_substparamblocks_old
          opt_gatesizeblocks_old
          opt_submod_displayblock_old
        ;

/*
 * Substparameters - old syntax
 */
opt_substparamblocks_old
        : substparamblocks_old
        |
        ;

substparamblocks_old
        : substparamblocks_old substparamblock_old
        | substparamblock_old
        ;

substparamblock_old
        : PARAMETERS ':'
                {
                  ps.substparams = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,@1,@2);
                }
          opt_substparameters_old
                {
                }
        | PARAMETERS IF expression ':'
                {
                  ps.substparams = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",@3,$3);
                  setComments(ps.substparams,@1,@4);
                }
          opt_substparameters_old
                {
                }

        ;

opt_substparameters_old
        : substparameters_old ';'
        |
        ;

substparameters_old
        : substparameters_old ',' substparameter_old   /* comma as separator */
        | substparameter_old
        ;

substparameter_old
        : NAME '=' expression
                {
                  ps.substparam = addSubstparam(ps.substparams,@1);
                  addExpression(ps.substparam, "value",@3,$3);
                  setComments(ps.substparam,@1,@3);
                }
        ;

/*
 * Gatesizes - old syntax
 */
opt_gatesizeblocks_old
        : opt_gatesizeblocks_old gatesizeblock_old
        |
        ;

gatesizeblock_old
        : GATESIZES ':'
                {
                  ps.gatesizes = (GatesNode *)createNodeWithTag(NED_GATES, ps.submod );
                  setComments(ps.gatesizes,@1,@2);
                }
          opt_gatesizes_old
                {
                }
        | GATESIZES IF expression ':'
                {
                  ps.gatesizes = (GatesNode *)createNodeWithTag(NED_GATES, ps.submod );
                  addExpression(ps.gatesizes, "condition",@3,$3);
                  setComments(ps.gatesizes,@1,@4);
                }
          opt_gatesizes_old
                {
                }
        ;

opt_gatesizes_old
        : gatesizes_old ';'
        |
        ;

gatesizes_old
        : gatesizes_old ',' gatesize_old
        | gatesize_old
        ;

gatesize_old
        : NAME vector
                {
                  ps.gatesize = addGateSize(ps.gatesizes,@1);
                  addVector(ps.gatesize, "vector-size",@2,$2);

                  setComments(ps.gatesize,@1,@2);
                }
        | NAME
                {
                  ps.gatesize = addGateSize(ps.gatesizes,@1);
                  setComments(ps.gatesize,@1);
                }
        ;

/*
 * Submodule-displayblock - old syntax
 */
opt_submod_displayblock_old
        : DISPLAY ':' STRINGCONSTANT ';'
                {
                  ps.property = addComponentProperty(ps.submod, "display");
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes(@3), @3);
                  ps.propkey->appendChild(literal);
                }
        |
        ;

/*
 * Connections - old syntax  (about 7 shift/reduce)
 */
opt_connblock_old
        : connblock_old
        |
        ;

connblock_old
        : CONNECTIONS NOCHECK ':'
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setAllowUnconnected(true);
                  setComments(ps.conns,@1,@3);
                }
          opt_connections_old
                {
                }
        | CONNECTIONS ':'
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setAllowUnconnected(false);
                  setComments(ps.conns,@1,@2);
                }
          opt_connections_old
                {
                }
        ;

opt_connections_old
        : connections_old
        |
        ;

connections_old
        : connections_old connection_old
        | connection_old
        ;

connection_old
        : loopconnection_old
        | notloopconnection_old
        ;

loopconnection_old
        : FOR
                {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                }
          loopvarlist_old DO notloopconnections_old ENDFOR opt_semicolon
                {
                  ps.inLoop=0;
                  setComments(ps.forloop,@1,@4);
                  setTrailingComment(ps.forloop,@6);
                }
        ;

loopvarlist_old
        : loopvar_old ',' loopvarlist_old
        | loopvar_old
        ;

loopvar_old
        : NAME '=' expression TO expression
                {
                  ps.loopvar = addLoopVar(ps.forloop,@1);
                  addExpression(ps.loopvar, "from-value",@3,$3);
                  addExpression(ps.loopvar, "to-value",@5,$5);
                  setComments(ps.loopvar,@1,@5);
                }
        ;

opt_conncondition_old
        : IF expression
                {
                  addExpression(ps.conn, "condition",@2,$2);
                }
        |
        ;

opt_conn_displaystr_old
        : DISPLAY STRINGCONSTANT
                {
                  //FIXME add chanspec here if doesn't exist!
                  ps.property = addComponentProperty(ps.chanspec, "display");
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes(@2), @2);
                  ps.propkey->appendChild(literal);
                }
        |
        ;

notloopconnections_old
        : notloopconnections_old notloopconnection_old
        | notloopconnection_old
        ;

notloopconnection_old
        : leftgatespec_old RIGHT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                  setComments(ps.conn,@1,@5);
                }
        | leftgatespec_old RIGHT_ARROW channeldescr_old RIGHT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                  setComments(ps.conn,@1,@7);
                }
        | leftgatespec_old LEFT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                  setComments(ps.conn,@1,@5);
                }
        | leftgatespec_old LEFT_ARROW channeldescr_old LEFT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                  setComments(ps.conn,@1,@7);
                }
        ;

leftgatespec_old
        : leftmod_old '.' leftgate_old
        | parentleftgate_old
        ;

leftmod_old
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

leftgate_old
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

parentleftgate_old
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

rightgatespec_old
        : rightmod_old '.' rightgate_old
        | parentrightgate_old
        ;

rightmod_old
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

rightgate_old
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

parentrightgate_old
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


channeldescr_old
        : NAME
                {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",@1,createExpression(createLiteral(NED_CONST_STRING, toString(@1))));
                }
        | CHANATTRNAME expression
                {
                  ps.connattr = addConnAttr(ps.conn,toString(@1));
                  addExpression(ps.connattr, "value",@2,$2);
                }
        | channeldescr_old CHANATTRNAME expression
                {
                  ps.connattr = addConnAttr(ps.conn,toString(@2));
                  addExpression(ps.connattr, "value",@3,$3);
                }
        ;

/*
 * Network - old syntax
 */
networkdefinition_old
        : networkheader_old
            opt_substparamblocks_old
          endnetwork_old
        ;

networkheader_old
        : NETWORK NAME ':' NAME opt_semicolon
                {
                  ps.network = addNetwork(ps.nedfile,@2,@4);
                  setComments(ps.network,@1,@5);
                  ps.inNetwork=1;
                }
        ;

endnetwork_old
        : ENDNETWORK opt_semicolon
                {
                  setTrailingComment(ps.network,@1);
                  ps.inNetwork=0;
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
        : XMLDOC '(' stringliteral ',' stringliteral ')'
                { if (ps.parseExpressions) $$ = createFunction("xmldoc", $3, $5); }
        | XMLDOC '(' stringliteral ')'
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
        | special_expr
        | literal
        ;

parameter_expr
        : NAME
                {
                  // if there's no modifier, might be a loop variable too
                  if (ps.parseExpressions) $$ = createIdent(toString(@1));
                }
        | REF NAME
                {
                  if (ps.parseExpressions) $$ = createIdent(toString(@2));
                  NEDError(ps.params,"`ref' modifier no longer supported (add `function' "
                                     "modifier to destination parameter instead)");
                }
        | REF ANCESTOR NAME
                {
                  if (ps.parseExpressions) $$ = createIdent(toString(@3));
                  NEDError(ps.params,"`ancestor' and `ref' modifiers no longer supported");
                }
        | ANCESTOR REF NAME
                {
                  if (ps.parseExpressions) $$ = createIdent(toString(@3));
                  NEDError(ps.params,"`ancestor' and `ref' modifiers no longer supported");
                }
        | ANCESTOR NAME
                {
                  if (ps.parseExpressions) $$ = createIdent(toString(@2));
                  NEDError(ps.params,"`ancestor' modifier no longer supported");
                }
        ;

special_expr
        : SUBMODINDEX
                { if (ps.parseExpressions) $$ = createFunction("index"); }
        | SUBMODINDEX '(' ')'
                { if (ps.parseExpressions) $$ = createFunction("index"); }
        | SIZEOF '(' NAME ')'
                { if (ps.parseExpressions) $$ = createFunction("sizeof", createIdent(toString(@3))); }
        ;

literal
        : stringliteral
        | boolliteral
        | numliteral
        ;

stringliteral
        : STRINGCONSTANT
                { if (ps.parseExpressions) $$ = createLiteral(NED_CONST_STRING, trimQuotes(@1), @1); }
        ;

boolliteral
        : TRUE_
                { if (ps.parseExpressions) $$ = createLiteral(NED_CONST_BOOL, @1, @1); }
        | FALSE_
                { if (ps.parseExpressions) $$ = createLiteral(NED_CONST_BOOL, @1, @1); }
        ;

numliteral
        : INTCONSTANT
                { if (ps.parseExpressions) $$ = createLiteral(NED_CONST_INT, @1, @1); }
        | REALCONSTANT
                { if (ps.parseExpressions) $$ = createLiteral(NED_CONST_DOUBLE, @1, @1); }
        | quantity
                { if (ps.parseExpressions) $$ = createQuantity(toString(@1)); }
        ;

quantity
        : quantity INTCONSTANT NAME
        | quantity REALCONSTANT NAME
        | INTCONSTANT NAME
        | REALCONSTANT NAME
        ;

/*
 * NED-2: Message subclassing (no shift-reduce conflict here)
 */

cplusplus
        : CPLUSPLUS CPLUSPLUSBODY opt_semicolon
                {
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(@2)));
                  setComments(ps.cplusplus,@1,@2);
                }
        ;

struct_decl
        : STRUCT NAME ';'
                {
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(@2));
                  setComments(ps.structdecl,@1,@2);
                }
        ;

class_decl
        : CLASS NAME ';'
                {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(@2));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,@1,@2);
                }
        | CLASS NONCOBJECT NAME ';'
                {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(@3));
                  setComments(ps.classdecl,@1,@2);
                }
        ;

message_decl
        : MESSAGE NAME ';'
                {
                  ps.messagedecl = (MessageDeclNode *)createNodeWithTag(NED_MESSAGE_DECL, ps.nedfile );
                  ps.messagedecl->setName(toString(@2));
                  setComments(ps.messagedecl,@1,@2);
                }
        ;

enum_decl
        : ENUM NAME ';'
                {
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString(@2));
                  setComments(ps.enumdecl,@1,@2);
                }
        ;

enum
        : ENUM NAME '{'
                {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(@2));
                  setComments(ps.enump,@1,@2);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
          opt_enumfields '}' opt_semicolon
                {
                  setTrailingComment(ps.enump,@6);
                }
        | ENUM NAME EXTENDS NAME '{'
                {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(@2));
                  ps.enump->setExtendsName(toString(@4));
                  setComments(ps.enump,@1,@4);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
          opt_enumfields '}' opt_semicolon
                {
                  setTrailingComment(ps.enump,@8);
                }
        ;

opt_enumfields
        : enumfields
        |
        ;

enumfields
        : enumfields enumfield
        | enumfield
        ;

enumfield
        : NAME ';'
                {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(@1));
                  setComments(ps.enumfield,@1,@1);
                }
        | NAME '=' enumvalue ';'
                {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(@1));
                  ps.enumfield->setValue(toString(@3));
                  setComments(ps.enumfield,@1,@3);
                }
        ;

message
        : MESSAGE NAME '{'
                {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(@2));
                  setComments(ps.messagep,@1,@2);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.messagep,@7);
                }
        | MESSAGE NAME EXTENDS NAME '{'
                {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(@2));
                  ps.messagep->setExtendsName(toString(@4));
                  setComments(ps.messagep,@1,@4);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.messagep,@9);
                }
        ;

class
        : CLASS NAME '{'
                {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(@2));
                  setComments(ps.classp,@1,@2);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.classp,@7);
                }
        | CLASS NAME EXTENDS NAME '{'
                {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(@2));
                  ps.classp->setExtendsName(toString(@4));
                  setComments(ps.classp,@1,@4);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.classp,@9);
                }
        ;

struct
        : STRUCT NAME '{'
                {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(@2));
                  setComments(ps.structp,@1,@2);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.structp,@7);
                }
        | STRUCT NAME EXTENDS NAME '{'
                {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(@2));
                  ps.structp->setExtendsName(toString(@4));
                  setComments(ps.structp,@1,@4);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.structp,@9);
                }
        ;

opt_propertiesblock
        : PROPERTIES ':'
                {
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,@1);
                }
          opt_properties
        |
        ;

opt_properties
        : properties
        |
        ;

properties
        : properties property
        | property
        ;

property
        : NAME '=' propertyvalue ';'
                {
                  ps.msgproperty = (MsgpropertyNode *)createNodeWithTag(NED_MSGPROPERTY, ps.properties);
                  ps.msgproperty->setName(toString(@1));
                  ps.msgproperty->setValue(toString(@3));
                  setComments(ps.msgproperty,@1,@3);
                }
        ;

propertyvalue
        : STRINGCONSTANT
        | INTCONSTANT
        | REALCONSTANT
        | quantity
        | TRUE_
        | FALSE_
        ;

opt_fieldsblock
        : FIELDS ':'
                {
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,@1);
                }
          opt_fields
        |
        ;

opt_fields
        : fields
        |
        ;

fields
        : fields field
        | field
        ;

field
        : fieldmodifiers fielddatatype NAME
                {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(@3));
                  ps.field->setDataType(toString(@2));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                }
           opt_fieldvector opt_fieldenum opt_fieldvalue ';'
                {
                  setComments(ps.field,@1,@7);
                }
        | fieldmodifiers NAME
                {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(@2));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                }
           opt_fieldvector opt_fieldenum opt_fieldvalue ';'
                {
                  setComments(ps.field,@1,@6);
                }
        ;

fieldmodifiers
        : ABSTRACT
                { ps.isAbstract = true; ps.isReadonly = false; }
        | READONLY
                { ps.isAbstract = false; ps.isReadonly = true; }
        | ABSTRACT READONLY
                { ps.isAbstract = true; ps.isReadonly = true; }
        | READONLY ABSTRACT
                { ps.isAbstract = true; ps.isReadonly = true; }
        |
                { ps.isAbstract = false; ps.isReadonly = false; }
        ;

fielddatatype
        : NAME
        | NAME '*'

        | CHARTYPE
        | SHORTTYPE
        | INTTYPE
        | LONGTYPE

        | UNSIGNED_ CHARTYPE
        | UNSIGNED_ SHORTTYPE
        | UNSIGNED_ INTTYPE
        | UNSIGNED_ LONGTYPE

        | DOUBLETYPE
        | STRINGTYPE
        | BOOLTYPE
        ;


opt_fieldvector
        : '[' INTCONSTANT ']'
                {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(@2));
                }
        | '[' NAME ']'
                {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(@2));
                }
        | '[' ']'
                {
                  ps.field->setIsVector(true);
                }
        |
        ;

opt_fieldenum
        : ENUM '(' NAME ')'
                {
                  ps.field->setEnumName(toString(@3));
                }
        |
        ;

opt_fieldvalue
        : '=' fieldvalue
                {
                  ps.field->setDefaultValue(toString(@2));
                }
        |
        ;

fieldvalue
        : STRINGCONSTANT
        | CHARCONSTANT
        | INTCONSTANT
        | '-' INTCONSTANT
        | REALCONSTANT
        | '-' REALCONSTANT
        | quantity
        | TRUE_
        | FALSE_
        | NAME
        ;

enumvalue
        : INTCONSTANT
        | '-' INTCONSTANT
        | NAME
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

    // create parser state and NEDFileNode
    np = p;
    ps.nedfile = nf;
    ps.parseExpressions = parseexpr;
    ps.storeSourceCode = storesrc;
    sourcefilename = sourcefname;

    if (storesrc)
        storeSourceCode(ps.nedfile, np->nedsource->getFullTextPos());

    // parse
    int ret;
    try
    {
        ret = yyparse();
    }
    catch (NEDException *e)
    {
        INTERNAL_ERROR1(NULL, "error during parsing: %s", e->errorMessage());
        delete e;
        return 0;
    }

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

//
// Spec Properties: source code, display string
//

PropertyNode *storeSourceCode(NEDElement *node, YYLTYPE tokenpos)
{
     PropertyNode *prop = addProperty(node, "sourcecode");
     prop->setIsImplicit(true);
     PropertyKeyNode *propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, prop);
     propkey->appendChild(createLiteral(NED_CONST_STRING, tokenpos, tokenpos));  // FIXME don't store it twice
     return prop;
}

PropertyNode *storeComponentSourceCode(NEDElement *node, YYLTYPE tokenpos)
{
     PropertyNode *prop = addComponentProperty(node, "sourcecode");
     prop->setIsImplicit(true);
     PropertyKeyNode *propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, prop);
     propkey->appendChild(createLiteral(NED_CONST_STRING, tokenpos, tokenpos));  // FIXME don't store it twice
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

ParamNode *addParameter(NEDElement *params, YYLTYPE namepos)
{
   ParamNode *param = (ParamNode *)createNodeWithTag(NED_PARAM,params);
   param->setName( toString( namepos) );
   return param;
}

GateNode *addGate(NEDElement *gates, YYLTYPE namepos)
{
   GateNode *gate = (GateNode *)createNodeWithTag(NED_GATE,gates);
   gate->setName( toString( namepos) );
   return gate;
}

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
   swapAttributes(conn, "src-gate-subg", "dest-gate-subg");

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
   // if (paramindex) par->setParamIndex(paramindex);
   if (module) par->setModule(module);
   if (moduleindex) par->setModuleIndex(moduleindex);
   return par;
}

LiteralNode *createLiteral(int type, YYLTYPE valuepos, YYLTYPE textpos)
{
   LiteralNode *c = (LiteralNode *)createNodeWithTag(NED_LITERAL);
   c->setType(type);
   c->setValue(toString(valuepos));
   c->setText(toString(textpos));
   return c;
}

LiteralNode *createQuantity(const char *text)
{
   LiteralNode *c = (LiteralNode *)createNodeWithTag(NED_LITERAL);
   c->setType(NED_CONST_UNIT);
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
