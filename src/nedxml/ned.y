/*===============================================================
 * File: ebnf.y
 *
 *  Grammar for OMNeT++ NED
 *
 *  By Andras Varga
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
  Copyright (C) 1992,2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


%token INCLUDE SIMPLE
%token CHANNEL DELAY ERROR DATARATE
%token MODULE PARAMETERS GATES GATESIZES SUBMODULES CONNECTIONS DISPLAY
%token IN OUT
%token NOCHECK LEFT_ARROW RIGHT_ARROW
%token FOR TO DO IF LIKE
%token NETWORK
%token ENDSIMPLE ENDMODULE ENDCHANNEL
%token ENDNETWORK ENDFOR
%token MACHINES ON         /* --LG */
%token IO_INTERFACES            /* --LG */
%token IFPAIR                   /* --LG */

%token INTCONSTANT REALCONSTANT NAME STRING
%token _TRUE _FALSE
%token INPUT
%token REF ANCESTOR
%token CONSTDECL NUMERICTYPE STRINGTYPE BOOLTYPE ANYTYPE

%token PLUS MIN MUL DIV MOD EXP
%token SIZEOF SUBMODINDEX
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
%left PLUS MIN
%left MUL DIV MOD
%right EXP
%left UMIN NOT BIN_COMPL

%start networkdescription


%{

/*
 * Note:
 * This file contains 2 shift-reduce conflicts around 'timeconstant'.
 * 4 more at opt_semicolon's after module/submodule types.
 * Plus 3 more to track down.
 *
 * bison's "%expect nn" option cannot be used to suppress the
 * warning message because %expect is not recognized by yacc
 */


#include <stdio.h>
#include <malloc.h>         /* for alloca() */
#include "nedgrammar.h"

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
    bool inLoop;
    bool inNetwork;

    NedFileNode *nedfile;
    ImportNode *imports;
    ImportedFileNode *import;
    ChannelNode *channel;
    ChannelAttrNode *chanattr;
    NetworkNode *network;
    NEDElement *module;  // in fact, CompoundModuleNode* or SimpleModule*
    ParamsNode *params;
    ParamNode *param;
    GatesNode *gates;
    GateNode *gate;
    MachinesNode *machines;
    MachineNode *machine;
    SubmodulesNode *submods;
    SubmoduleNode *submod;
    SubstparamsNode *substparams;
    SubstparamNode *substparam;
    GatesizesNode *gatesizes;
    GatesizeNode *gatesize;
    SubstmachinesNode *substmachines;
    SubstmachineNode *substmachine;
    ConnectionsNode *conns;
    ConnectionNode *conn;
    ConnAttrNode *connattr;
    ForLoopNode *forloop;
    LoopVarNode *loopvar;
} ps;

NEDElement *createNodeWithTag(int tagcode, NEDElement *parent=NULL);

void setFileComment(NEDElement *node);
void setBannerComment(NEDElement *node, YYLTYPE tokenpos);
void setRightComment(NEDElement *node, YYLTYPE tokenpos);
void setTrailingComment(NEDElement *node, YYLTYPE tokenpos);
void setComments(NEDElement *node, YYLTYPE pos);
void setComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos);

ChannelAttrNode *addChanAttr(NEDElement *channel, char *attrname);
ParamNode *addParameter(NEDElement *params, YYLTYPE namepos, int type);
GateNode *addGate(NEDElement *gates, YYLTYPE namepos, int is_in, int is_vector );
SubmoduleNode *addSubmodule(NEDElement *submods, YYLTYPE namepos, YYLTYPE typepos,YYLTYPE likepos);
GatesizeNode *addGateSize(NEDElement *gatesizes, YYLTYPE namepos);
SubstparamNode *addSubstparam(NEDElement *substparams, YYLTYPE namepos);
SubstmachineNode *addSubstmachine(NEDElement *substmachines, YYLTYPE namepos);
ConnAttrNode *addConnAttr(NEDElement *conn, char *attrname);
LoopVarNode *addLoopVar(NEDElement *forloop, YYLTYPE varnamepos);
NetworkNode *addNetwork(NEDElement *nedfile, YYLTYPE namepos, YYLTYPE typepos, YYLTYPE likepos);
DisplayStringNode *addDisplayString(NEDElement *parent, YYLTYPE dispstrpos);

YYLTYPE trimBrackets(YYLTYPE vectorpos);
YYLTYPE trimQuotes(YYLTYPE vectorpos);
void swapAttributes(NEDElement *node, const char *attr1, const char *attr2);
void swapExpressionChildren(NEDElement *node, const char *attr1, const char *attr2);
void swapConnection(NEDElement *conn);

const char *toString(YYLTYPE);
const char *toString(long);

ExpressionNode *createExpression(NEDElement *expr);
OperatorNode *createOperator(const char *op, NEDElement *operand1, NEDElement *operand2=NULL, NEDElement *operand3=NULL);
FunctionNode *createFunction(const char *funcname, NEDElement *arg1=NULL, NEDElement *arg2=NULL, NEDElement *arg3=NULL);
ParamRefNode *createParamRef(const char *param, const char *paramindex=NULL, const char *module=NULL, const char *moduleindex=NULL);
IdentNode *createIdent(const char *name);
ConstNode *createConst(int type, const char *value, const char *text=NULL);
ConstNode *createTimeConst(const char *text);
NEDElement *createParamRefOrIdent(const char *name);

void addVector(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);
void addExpression(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);

%}

%%

networkdescription
        : somedefinitions
        |
        ;

somedefinitions
        : definition somedefinitions
        | definition
        ;

definition
        : import
        | channeldefinition
        | simpledefinition
        | moduledefinition
        | network
        ;

import
        : INCLUDE
                {
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,@1);
                }
          filenames
                {
                  /* no setTrailingComment(ps.imports,@3); comment already stored with last filename */
                }
        ;

filenames
        : filename ',' filenames
        | filename ';'
        ;

filename
        : STRING
                {
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(@1)));
                  setComments(ps.import,@1);
                }
        ;

channeldefinition
        : channelheader endchannel
        | channelheader chdelay endchannel
        | channelheader cherror endchannel
        | channelheader chdatarate endchannel
        | channelheader chdelay cherror endchannel
        | channelheader chdelay chdatarate endchannel
        | channelheader cherror chdatarate endchannel
        | channelheader cherror chdelay endchannel
        | channelheader chdatarate chdelay endchannel
        | channelheader chdatarate cherror endchannel
        | channelheader chdelay cherror chdatarate endchannel
        | channelheader chdelay chdatarate cherror endchannel
        | channelheader cherror chdelay chdatarate endchannel
        | channelheader cherror chdatarate chdelay endchannel
        | channelheader chdatarate chdelay cherror endchannel
        | channelheader chdatarate cherror chdelay endchannel

        | channelheader '{' braceendchannel
        | channelheader '{' chdelay braceendchannel
        | channelheader '{' cherror braceendchannel
        | channelheader '{' chdatarate braceendchannel
        | channelheader '{' chdelay cherror braceendchannel
        | channelheader '{' chdelay chdatarate braceendchannel
        | channelheader '{' cherror chdatarate braceendchannel
        | channelheader '{' cherror chdelay braceendchannel
        | channelheader '{' chdatarate chdelay braceendchannel
        | channelheader '{' chdatarate cherror braceendchannel
        | channelheader '{' chdelay cherror chdatarate braceendchannel
        | channelheader '{' chdelay chdatarate cherror braceendchannel
        | channelheader '{' cherror chdelay chdatarate braceendchannel
        | channelheader '{' cherror chdatarate chdelay braceendchannel
        | channelheader '{' chdatarate chdelay cherror braceendchannel
        | channelheader '{' chdatarate cherror chdelay braceendchannel
        ;

channelheader
        : CHANNEL NAME
                {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(@2));
                  setComments(ps.channel,@1,@2);
                }
        ;


endchannel
        : ENDCHANNEL NAME opt_semicolon
                {
                  setTrailingComment(ps.channel,@2);
                }
        | ENDCHANNEL opt_semicolon
                {
                  setTrailingComment(ps.channel,@1);
                }
        ;


braceendchannel
        : '}' opt_semicolon
                {
                  setTrailingComment(ps.channel,@1);
                }
        ;

cherror
        : ERROR expression opt_semicolon
                {
                  ps.chanattr = addChanAttr(ps.channel,"error");
                  addExpression(ps.chanattr, "value",@2,$2);
                  setComments(ps.channel,@1,@3);
                }
        ;

chdelay
        : DELAY expression opt_semicolon
                {
                  ps.chanattr = addChanAttr(ps.channel,"delay");
                  addExpression(ps.chanattr, "value",@2,$2);
                  setComments(ps.channel,@1,@3);
                }
        ;

chdatarate
        : DATARATE expression opt_semicolon
                {
                  ps.chanattr = addChanAttr(ps.channel,"datarate");
                  addExpression(ps.chanattr, "value",@2,$2);
                  setComments(ps.channel,@1,@3);
                }
        ;

simpledefinition
        : simpleheader
            opt_machineblock
            opt_paramblock
            opt_gateblock
          endsimple
        | simpleheader '{'
            opt_machineblock
            opt_paramblock
            opt_gateblock
          '}' opt_semicolon
                {
                  setTrailingComment(ps.module,@6);
                }
        ;

simpleheader
        : SIMPLE NAME opt_semicolon
                {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(@2));
                  setComments(ps.module,@1,@3);
                }
        ;

endsimple
        : ENDSIMPLE NAME opt_semicolon
                {
                  setTrailingComment(ps.module,@2);
                }
        | ENDSIMPLE opt_semicolon
                {
                  setTrailingComment(ps.module,@1);
                }
        ;

moduledefinition
        : moduleheader
            opt_machineblock
            opt_paramblock
            opt_gateblock
            opt_submodblock
            opt_connblock
            opt_displayblock
          endmodule
        | moduleheader '{'
            opt_machineblock
            opt_paramblock
            opt_gateblock
            opt_submodblock
            opt_connblock
            opt_displayblock
          '}' opt_semicolon
                {
                  setTrailingComment(ps.module,@9);
                }
        ;

moduleheader
        : MODULE NAME opt_semicolon
                {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(@2));
                  setComments(ps.module,@1,@3);
                }
        ;

endmodule
        : ENDMODULE NAME opt_semicolon
                {
                  setTrailingComment(ps.module,@2);
                }
        | ENDMODULE opt_semicolon
                {
                  setTrailingComment(ps.module,@1);
                }
        ;

opt_machineblock
        : machineblock
        |
        ;

opt_displayblock : displayblock | ;
opt_paramblock   : paramblock   | ;
opt_gateblock    : gateblock    | ;
opt_submodblock  : submodblock  | ;
opt_connblock    : connblock    | ;

machineblock
        : MACHINES ':'
                {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,@1,@2);
                }
          opt_machinelist
                {
                }
        ;

opt_machinelist
        : machinelist
        |
        ;

machinelist
        : machine ',' machinelist
        | machine ';'
        ;

machine
        : NAME
                {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(@1));
                  setComments(ps.machine,@1,@1);
                }
        ;

displayblock
        : DISPLAY ':' STRING ';'
                {
                  addDisplayString(ps.module,@3);
                }
        ;

paramblock
        : PARAMETERS ':'
                {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,@1,@2);
                }
          opt_parameters
                {
                }
        ;

opt_parameters
        : parameters
        |
        ;

parameters
        : parameter ','
                {
                  setComments(ps.param,@1);
                }
          parameters
        | parameter ';'
                {
                  setComments(ps.param,@1);
                }
          parameters
        | parameter ';'
                {
                  setComments(ps.param,@1);
                }
        ;

parameter
        : NAME
                {
                  ps.param = addParameter(ps.params,@1,TYPE_NUMERIC);
                }
        | NAME ':' NUMERICTYPE
                {
                  ps.param = addParameter(ps.params,@1,TYPE_NUMERIC);
                }
        | CONSTDECL NAME /* for compatibility */
                {
                  ps.param = addParameter(ps.params,@2,TYPE_CONST_NUM);
                }
        | NAME ':' CONSTDECL
                {
                  ps.param = addParameter(ps.params,@1,TYPE_CONST_NUM);
                }
        | NAME ':' CONSTDECL NUMERICTYPE
                {
                  ps.param = addParameter(ps.params,@1,TYPE_CONST_NUM);
                }
        | NAME ':' NUMERICTYPE CONSTDECL
                {
                  ps.param = addParameter(ps.params,@1,TYPE_CONST_NUM);
                }
        | NAME ':' STRINGTYPE
                {
                  ps.param = addParameter(ps.params,@1,TYPE_STRING);
                }
        | NAME ':' BOOLTYPE
                {
                  ps.param = addParameter(ps.params,@1,TYPE_BOOL);
                }
        | NAME ':' ANYTYPE
                {
                  ps.param = addParameter(ps.params,@1,TYPE_ANYTYPE);
                }
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
        : IN  gatesI gates
        | IN  gatesI
        | OUT gatesO gates
        | OUT gatesO
        ;

gatesI
        : gateI ',' gatesI
        | gateI ';'
        ;

gateI
        : NAME '[' ']'
                {
                  ps.gate = addGate(ps.gates, @1, 1, 1 );
                  setComments(ps.gate,@1,@3);
                }
        | NAME
                {
                  ps.gate = addGate(ps.gates, @1, 1, 0 );
                  setComments(ps.gate,@1);
                }
        ;

gatesO
        : gateO ',' gatesO
        | gateO ';'
        ;

gateO
        : NAME '[' ']'
                {
                  ps.gate = addGate(ps.gates, @1, 0, 1 );
                  setComments(ps.gate,@1,@3);
                }
        | NAME
                {
                  ps.gate = addGate(ps.gates, @1, 0, 0 );
                  setComments(ps.gate,@1);
                }
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
        : submodule submodules
        | submodule
        ;

submodule
        : NAME ':' NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, NULLPOS);
                  setComments(ps.submod,@1,@4);
                }
          opt_on_blocks
          submodule_body
                {
                }
        | NAME ':' NAME vector opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, NULLPOS);
                  addVector(ps.submod, "vector-size",@4,$4);
                  setComments(ps.submod,@1,@5);
                }
          opt_on_blocks
          submodule_body
                {
                }
        | NAME ':' NAME LIKE NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, @5);
                  setComments(ps.submod,@1,@6);
                }
          opt_on_blocks
          submodule_body
                {
                }
        | NAME ':' NAME vector LIKE NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, @6);
                  addVector(ps.submod, "vector-size",@4,$4);
                  setComments(ps.submod,@1,@7);
                }
          opt_on_blocks
          submodule_body
                {
                }
        ;

submodule_body
        : opt_substparamblocks
          opt_gatesizeblocks
          opt_submod_displayblock
        ;

opt_on_blocks
        : on_blocks
        |
        ;

on_blocks
        : on_block on_blocks
        | on_block
        ;

on_block                            /* --LG */
        : ON ':'
                {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,@1,@2);
                }
          opt_on_list
                {
                }
        | ON IF expression ':'
                {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",@3,$3);
                  setComments(ps.substmachines,@1,@4);
                }
          opt_on_list
                {
                }
        ;

opt_on_list
        : on_list
        |
        ;

on_list
        : on_mach ',' on_list
        | on_mach ';'
        ;

on_mach
        : NAME
                {
                  ps.substmachine = addSubstmachine(ps.substmachines,@1);
                  setComments(ps.substmachine,@1);
                }
        ;

opt_substparamblocks
        : substparamblocks
        |
        ;

substparamblocks
        : substparamblock substparamblocks
        | substparamblock
        ;

substparamblock
        : PARAMETERS ':'
                {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,@1,@2);
                }
          opt_substparameters
                {
                }
        | PARAMETERS IF expression ':'
                {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",@3,$3);
                  setComments(ps.substparams,@1,@4);
                }
          opt_substparameters
                {
                }

        ;

opt_substparameters
        : substparameters
        |
        ;

substparameters
        : substparameter ',' substparameters
/*FIXME shift-reduce:
        | substparameter ';' substparameters
*/
        | substparameter ';'
        ;

substparameter
        : NAME '=' expression
                {
                  ps.substparam = addSubstparam(ps.substparams,@1);
                  addExpression(ps.substparam, "value",@3,$3);
                  setComments(ps.substparam,@1,@3);
                }
        ;

opt_gatesizeblocks
        : gatesizeblock opt_gatesizeblocks
        |
        ;

gatesizeblock
        : GATESIZES ':'
                {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,@1,@2);
                }
          opt_gatesizes
                {
                }
        | GATESIZES IF expression ':'
                {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",@3,$3);
                  setComments(ps.gatesizes,@1,@4);
                }
          opt_gatesizes
                {
                }
        ;

opt_gatesizes
        : gatesizes
        |
        ;

gatesizes
        : gatesize ',' gatesizes
        | gatesize ';'
        ;

gatesize
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

opt_submod_displayblock
        : DISPLAY ':' STRING ';'
                {
                  addDisplayString(ps.submod,@3);
                }
        |
        ;

connblock
        : CONNECTIONS NOCHECK ':'
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "false" );
                  setComments(ps.conns,@1,@3);
                }
          opt_connections
                {
                }
        | CONNECTIONS ':'
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "yes" );
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
        : connection comma_or_semicolon connections
        | connection ';'
        ;

connection
        : loopconnection
        | notloopconnection
        ;

loopconnection
        : FOR
                {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                }
          loopvarlist DO
                {
                  ps.inLoop=1;
                }
          notloopconnections ENDFOR
                {
                  ps.inLoop=0;
                  setComments(ps.forloop,@1,@4);
                  setTrailingComment(ps.forloop,@7);
                }
        ;

loopvarlist
        : loopvar ',' loopvarlist
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

opt_conn_condition
        : IF expression
                {
                  addExpression(ps.conn, "condition",@2,$2);
                }
        |
        ;

opt_conn_displaystr
        : DISPLAY STRING
                {
                  addDisplayString(ps.conn,@2);
                }
        |
        ;

notloopconnections
        : notloopconnection comma_or_semicolon notloopconnections
        | notloopconnection ';'
        ;

notloopconnection
        : gate_spec_L RIGHT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,@1,@5);
                }
        | gate_spec_L RIGHT_ARROW channeldescr RIGHT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,@1,@7);
                }
        | gate_spec_L LEFT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,@1,@5);
                }
        | gate_spec_L LEFT_ARROW channeldescr LEFT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,@1,@7);
                }
        ;

gate_spec_L
        : mod_L '.' gate_L
        | parentgate_L
        ;

mod_L
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

gate_L
        : NAME vector
                {
                  ps.conn->setSrcGate( toString( @1) );
                  addVector(ps.conn, "src-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setSrcGate( toString( @1) );
                }
        ;

parentgate_L
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
        ;

gate_spec_R
        : mod_R '.' gate_R
        | parentgate_R
        ;

mod_R
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

gate_R
        : NAME vector
                {
                  ps.conn->setDestGate( toString( @1) );
                  addVector(ps.conn, "dest-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setDestGate( toString( @1) );
                }
        ;

parentgate_R
        : NAME vector
                {
                  ps.conn->setDestGate( toString( @1) );
                  addVector(ps.conn, "dest-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setDestGate( toString( @1) );
                }
        ;


channeldescr
        :
        | cdname
        | cddelay
        | cderror
        | cddatarate

        | cddelay cderror
        | cddelay cddatarate
        | cderror cddatarate
        | cderror cddelay
        | cddatarate cddelay
        | cddatarate cderror

        | cddelay cderror cddatarate
        | cddelay cddatarate cderror
        | cderror cddelay cddatarate
        | cderror cddatarate cddelay
        | cddatarate cddelay cderror
        | cddatarate cderror cddelay
        ;

cdname
        : expression
                {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",@1,$1);
                }
        ;

cddelay
        : DELAY expression
                {
                  ps.connattr = addConnAttr(ps.conn,"delay");
                  addExpression(ps.connattr, "value",@2,$2);
                }
        ;

cderror
        : ERROR expression
                {
                  ps.connattr = addConnAttr(ps.conn,"error");
                  addExpression(ps.connattr, "value",@2,$2);
                }
        ;

cddatarate
        : DATARATE expression
                {
                  ps.connattr = addConnAttr(ps.conn,"datarate");
                  addExpression(ps.connattr, "value",@2,$2);
                }
        ;

network
        : networkheader
            opt_on_blocks
            opt_substparamblocks
          endnetwork
        | networkheader '{'
            opt_on_blocks
            opt_substparamblocks
          '}' opt_semicolon
                {
                  setTrailingComment(ps.network,@5);
                  ps.inNetwork=0;
                }
        ;

networkheader
        : NETWORK NAME ':' NAME opt_semicolon
                {
                  ps.network = addNetwork(ps.nedfile,@2,@4,NULLPOS);
                  setComments(ps.network,@1,@5);
                  ps.inNetwork=1;
                }
        | NETWORK NAME ':' NAME LIKE NAME opt_semicolon
                {
                  ps.network = addNetwork(ps.nedfile,@2,@4,@6);
                  setComments(ps.network,@1,@7);
                  ps.inNetwork=1;
                }
        ;

endnetwork
        : ENDNETWORK opt_semicolon
                {
                  setTrailingComment(ps.network,@1);
                  ps.inNetwork=0;
                }
        ;

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
        ;

inputvalue
        : INPUT '(' expr ',' expr ')'  /* input(defaultvalue, promptstring) */
                { if (ps.parseExpressions) $$ = createFunction("input", $3, $5); }
        | INPUT '(' expr ')'
                { if (ps.parseExpressions) $$ = createFunction("input", $3); }
        | INPUT '(' ')'
                { if (ps.parseExpressions) $$ = createFunction("input"); }
        | INPUT
                { if (ps.parseExpressions) $$ = createFunction("input"); }
        ;

expr
        : simple_expr
        | '(' expr ')'
                { $$ = $2; }

        | expr PLUS expr
                { if (ps.parseExpressions) $$ = createOperator("+", $1, $3); }
        | expr MIN expr
                { if (ps.parseExpressions) $$ = createOperator("-", $1, $3); }
        | expr MUL expr
                { if (ps.parseExpressions) $$ = createOperator("*", $1, $3); }
        | expr DIV expr
                { if (ps.parseExpressions) $$ = createOperator("/", $1, $3); }
        | expr MOD expr
                { if (ps.parseExpressions) $$ = createOperator("%", $1, $3); }
        | expr EXP expr
                { if (ps.parseExpressions) $$ = createOperator("^", $1, $3); }

        | MIN expr
                %prec UMIN
                { if (ps.parseExpressions) $$ = createOperator("-", $2); }

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
                  ((ParamRefNode *)$$)->setIsRef("true");
                }
        | REF ANCESTOR NAME
                {
                  $$ = createParamRef(toString(@3));
                  ((ParamRefNode *)$$)->setIsRef("true");
                  ((ParamRefNode *)$$)->setIsAncestor("true");
                }
        | ANCESTOR REF NAME
                {
                  $$ = createParamRef(toString(@3));
                  ((ParamRefNode *)$$)->setIsRef("true");
                  ((ParamRefNode *)$$)->setIsAncestor("true");
                }
        | ANCESTOR NAME
                {
                  $$ = createParamRef(toString(@2));
                  ((ParamRefNode *)$$)->setIsAncestor("true");
                }
        ;

string_expr
        : STRING
                { $$ = createConst(NED_CONST_STRING, toString(trimQuotes(@1))); }
        ;

boolconst_expr
        : _TRUE
                { $$ = createConst(NED_CONST_BOOL, "true"); }
        | _FALSE
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

numconst
        : INTCONSTANT
                { $$ = createConst(NED_CONST_INT, toString(@1)); }
        | REALCONSTANT
                { $$ = createConst(NED_CONST_REAL, toString(@1)); }
        | timeconstant
                { $$ = createTimeConst(toString(@1)); }
        ;

timeconstant
        : INTCONSTANT NAME timeconstant
        | REALCONSTANT NAME timeconstant
        | INTCONSTANT NAME
        | REALCONSTANT NAME
        ;

opt_semicolon : ';' | ;
comma_or_semicolon : ',' | ';' | ;

%%

//----------------------------------------------------------------------
// general bison/flex stuff:
//

char yyfailure[250] = "";
extern int yydebug; /* needed if compiled with yacc --VA */

extern char textbuf[];
extern char lasttextbuf[];

int runparse (NEDParser *p,NedFileNode *nf,bool parseexpr, const char *sourcefname)
{
#if YYDEBUG != 0      /* #if added --VA */
    yydebug = YYDEBUGGING_ON;
#endif
    pos.co = 0;
    pos.li = 1;
    prevpos = pos;

    strcpy (lasttextbuf, "");
    strcpy (yyfailure, "");

    if (yyin)
        yyrestart( yyin );

    np = p;
    ps.nedfile = nf;
    ps.parseExpressions = parseexpr;
    sourcefilename = sourcefname;
    return yyparse();
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

ChannelAttrNode *addChanAttr(NEDElement *channel, char *attrname)
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

SubmoduleNode *addSubmodule(NEDElement *submods, YYLTYPE namepos, YYLTYPE typepos,YYLTYPE likepos)
{
   SubmoduleNode *submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE,submods);
   submod->setName( toString( namepos) );
   submod->setTypeName( toString( typepos) );
   submod->setLikeName( toString( likepos) );

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

ConnAttrNode *addConnAttr(NEDElement *conn, char *attrname)
{
    ConnAttrNode *connattr = (ConnAttrNode *)createNodeWithTag(NED_CONN_ATTR,conn);
    connattr->setName( attrname );
    return connattr;
}


NetworkNode *addNetwork(NEDElement *nedfile, YYLTYPE namepos, YYLTYPE typepos, YYLTYPE likepos)
{
   NetworkNode *network = (NetworkNode *)createNodeWithTag(NED_NETWORK,nedfile);
   network->setName( toString( namepos) );
   network->setTypeName( toString( typepos) );
   network->setLikeName( toString( likepos) );
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

   swapExpressionChildren(conn, "src-module-index", "dest-module-index");
   swapExpressionChildren(conn, "src-gate-index", "dest-gate-index");
}

void swapAttributes(NEDElement *node, const char *attr1, const char *attr2)
{
   NEDString oldv1(node->getAttribute(attr1));
   node->setAttribute(attr1,node->getAttribute(attr2));
   node->setAttribute(attr2,oldv1);
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

FunctionNode *createFunction(const char *funcname, NEDElement *arg1, NEDElement *arg2, NEDElement *arg3)
{
   FunctionNode *funcnode = (FunctionNode *)createNodeWithTag(NED_FUNCTION);
   funcnode->setName(funcname);
   if (arg1) funcnode->appendChild(arg1);
   if (arg2) funcnode->appendChild(arg2);
   if (arg3) funcnode->appendChild(arg3);
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
   if (t<0) np->error("invalid time constant", pos.li);
   char buf[32];
   sprintf(buf,"%lg",t);
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

