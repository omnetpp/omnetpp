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
%token CHANATTRNAME

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
 * This file contains about 5 shift-reduce conflicts, 3 of them around 'expression'.
 *
 * Plus one (real) ambiguity exists between submodule display string
 * and compound module display string if no connections are present.
 */


#include <stdio.h>
#include <stdlib.h>
#include "nedyydefs.h"
#include "nederror.h"

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <string.h>         /* YYVERBOSE needs it */
#endif

#define yylloc nedyylloc
#define yyin nedyyin
#define yyout nedyyout
#define yyrestart nedyyrestart
#define yy_scan_string nedyy_scan_string
#define yy_delete_buffer nedyy_delete_buffer
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

static struct NEDParserState
{
    bool inLoop;
    bool inNetwork;

    /* tmp flags, used with msg fields */
    bool isAbstract;
    bool isReadonly;

    /* NED-I: modules, channels, networks */
    NedFileNode *nedfile;
    WhitespaceNode *whitespace;
    ImportNode *import;
    ExtendsNode *extends;
    ChannelNode *channel;
    NEDElement *module;  // in fact, CompoundModuleNode* or SimpleModule*
    ParametersNode *params;
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
} ps;

static void resetParserState()
{
    static NEDParserState cleanps;
    ps = cleanps;
}

ChannelSpecNode *createChannelSpec(NEDElement *conn);

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
                { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.channel, @1); }
        | simpledefinition_old
                { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.module, @1); }
        | moduledefinition_old
                { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.module, @1); }
        | networkdefinition_old
                { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.module, @1); }
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
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile);
                  ps.channel->setName(toString(@2));
                  ps.params = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.channel);
                  ps.params->setIsImplicit(true);
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
                  ps.params->setIsImplicit(false);
                  ps.param = addParameter(ps.params, @2);
                  addExpression(ps.param, "value",@3,$3);
                  setComments(ps.param,@2,@3);
                }
        | CHANATTRNAME expression opt_semicolon
                {
                  ps.params->setIsImplicit(false);
                  ps.param = addParameter(ps.params, @1);
                  addExpression(ps.param, "value",@2,$2);
                  setComments(ps.param,@1,@2);
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
                  np->getErrors()->add(ps.params,"type 'anytype' no longer supported");
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
                  setComments(ps.gate,@1,@3);
                }
        | NAME
                {
                  ps.gate = addGate(ps.gates, @1);
                  ps.gate->setType(NED_GATETYPE_INPUT);
                  setComments(ps.gate,@1);
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
                  setComments(ps.gate,@1,@3);
                }
        | NAME
                {
                  ps.gate = addGate(ps.gates, @1);
                  ps.gate->setType(NED_GATETYPE_OUTPUT);
                  setComments(ps.gate,@1,@1);
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
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString(@1));
                  ps.submod->setType(toString(@3));
                  setComments(ps.submod,@1,@4);
                }
          submodule_body_old
                {
                }
        | NAME ':' NAME vector opt_semicolon
                {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString(@1));
                  ps.submod->setType(toString(@3));
                  addVector(ps.submod, "vector-size",@4,$4);
                  setComments(ps.submod,@1,@5);
                }
          submodule_body_old
                {
                }
        | NAME ':' NAME LIKE NAME opt_semicolon
                {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString(@1));
                  ps.submod->setLikeType(toString(@5));
                  ps.submod->setLikeParam(toString(@3)); //FIXME store as expression!!!
                  setComments(ps.submod,@1,@6);
                }
          submodule_body_old
                {
                }
        | NAME ':' NAME vector LIKE NAME opt_semicolon
                {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString(@1));
                  ps.submod->setLikeType(toString(@5));
                  ps.submod->setLikeParam(toString(@3)); //FIXME store as expression!!!
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
                  // FIXME check if already exists!!! (multiple blocks must be merged)
                  ps.substparams = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.inNetwork ? (NEDElement *)ps.module : (NEDElement *)ps.submod);
                  setComments(ps.substparams,@1,@2);
                }
          opt_substparameters_old
                {
                }
        | PARAMETERS IF expression ':'
                {
                  // FIXME make conditional paramgroup!!!
                  ps.substparams = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.inNetwork ? (NEDElement *)ps.module : (NEDElement *)ps.submod);
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
                  ps.substparam = addParameter(ps.substparams,@1);
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
                  // FIXME check if already exists!!! (multiple blocks must be merged)
                  ps.gatesizes = (GatesNode *)createNodeWithTag(NED_GATES, ps.submod );
                  setComments(ps.gatesizes,@1,@2);
                }
          opt_gatesizes_old
                {
                }
        | GATESIZES IF expression ':'
                {
                  // FIXME make conditional gategroup!!!
                  ps.gatesizes = (GatesNode *)createNodeWithTag(NED_GATES, ps.submod);
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
                  ps.gatesize = addGate(ps.gatesizes,@1);
                  addVector(ps.gatesize, "vector-size",@2,$2);

                  setComments(ps.gatesize,@1,@2);
                }
        | NAME
                {
                  ps.gatesize = addGate(ps.gatesizes,@1);
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
                  ps.conngroup = (ConnectionGroupNode *)createNodeWithTag(NED_CONNECTION_GROUP, ps.conns);
                  ps.where = (WhereNode *)createNodeWithTag(NED_WHERE, ps.conngroup);
                  ps.inLoop=1;
                }
          loopvarlist_old DO notloopconnections_old ENDFOR opt_semicolon
                {
                  ps.inLoop=0;
                  setComments(ps.where,@1,@4);
                  //setTrailingComment(ps.where,@6);

                  // optimize: if there's exactly one connection inside the loop, eliminate conngroup
                  if (ps.conngroup->getNumChildrenWithTag(NED_CONNECTION)==1)
                  {
                      ps.conngroup->removeChild(ps.conn);
                      ps.conns->insertChildBefore(ps.conngroup, ps.conn);
                      ps.conn->appendChild(ps.conngroup->removeChild(ps.where));
                      delete ps.conns->removeChild(ps.conngroup);
                  }
                  else
                  {
                      // move ps.where to the end
                      ps.conngroup->appendChild(ps.conngroup->removeChild(ps.where));
                      ps.where->setAtFront(true);

                      // we're only prepared for "for" loops with 1 connection inside
                      if (ps.where->getNumChildrenWithTag(NED_CONDITION)!=0)
                          np->getErrors()->add(ps.conngroup, "cannot process NED-I syntax of several "
                                   "conditional connections within a `for' loop -- "
                                   "please split it to several `for' loops");
                  }
                }
        ;

loopvarlist_old
        : loopvar_old ',' loopvarlist_old
        | loopvar_old
        ;

loopvar_old
        : NAME '=' expression TO expression
                {
                  ps.loop = addLoop(ps.where,@1);
                  addExpression(ps.loop, "from-value",@3,$3);
                  addExpression(ps.loop, "to-value",@5,$5);
                  setComments(ps.loop,@1,@5);
                }
        ;

opt_conncondition_old
        : IF expression
                {
                  if (!ps.inLoop)
                  {
                      // add where+condition to conn
                      ps.where = (WhereNode *)createNodeWithTag(NED_WHERE, ps.conn);
                      ps.condition = (ConditionNode *)createNodeWithTag(NED_CONDITION, ps.where);
                      addExpression(ps.condition, "condition",@2,$2);
                  }
                  else
                  {
                      // inside a for loop: append condition to its "where".
                      // at the end we'll need to make sure there's only one connection in the loop!
                      // (otherwise we'd have to check all conns have exactly the same condition)
                      ps.condition = (ConditionNode *)createNodeWithTag(NED_CONDITION, ps.where);
                      addExpression(ps.condition, "condition",@2,$2);
                  }
                }
        |
        ;

opt_conn_displaystr_old
        : DISPLAY STRINGCONSTANT
                {
                  if (!ps.chanspec)
                      ps.chanspec = createChannelSpec(ps.conn);
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
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                  addVector(ps.conn, "src-module-index",@2,$2);
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                }
        | NAME
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
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
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                  addVector(ps.conn, "src-gate-index",@2,$2);
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                }
        | NAME
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                }
        | NAME PLUSPLUS
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                  ps.conn->setSrcGatePlusplus(true);
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
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
                  if (!ps.chanspec)
                      ps.chanspec = createChannelSpec(ps.conn);
                  ps.chanspec->setType(toString(@1));
                }
        | CHANATTRNAME expression
                {
                  if (!ps.chanspec)
                      ps.chanspec = createChannelSpec(ps.conn);
                  ps.param = addParameter(ps.params, @1);
                  addExpression(ps.param, "value",@2,$2);
                }
        | channeldescr_old CHANATTRNAME expression
                {
                  if (!ps.chanspec)
                      ps.chanspec = createChannelSpec(ps.conn);
                  ps.param = addParameter(ps.params, @2);
                  addExpression(ps.param, "value",@3,$3);
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
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(@2));
                  ((CompoundModuleNode *)ps.module)->setIsNetwork(true);
                  ps.extends = (ExtendsNode *)createNodeWithTag(NED_EXTENDS, ps.module);
                  ps.extends->setName(toString(@4));
                  setComments(ps.module,@1,@5);
                  ps.inNetwork=1;
                }
        ;

endnetwork_old
        : ENDNETWORK opt_semicolon
                {
                  //setTrailingComment(ps.module,@1);
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
                  if (np->getParseExpressionsFlag()) $$ = createExpression($1);
                }
        | inputvalue
                {
                  if (np->getParseExpressionsFlag()) $$ = createExpression($1);
                }
        | xmldocvalue
                {
                  if (np->getParseExpressionsFlag()) $$ = createExpression($1);
                }
        ;

/*
 * Expressions (3 shift-reduce conflicts here)
 */

inputvalue  /* FIXME turn to isDefault=true, @prompt!!! */
        : INPUT_ '(' expr ',' expr ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction("input", $3, $5); }
        | INPUT_ '(' expr ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction("input", $3); }
        | INPUT_ '(' ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction("input"); }
        | INPUT_
                { if (np->getParseExpressionsFlag()) $$ = createFunction("input"); }
        ;

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
        | expr GT expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator(">", $1, $3); }
        | expr GE expr
                { if (np->getParseExpressionsFlag()) $$ = createOperator(">=", $1, $3); }
        | expr LS expr
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
        : parameter_expr
        | special_expr
        | literal
        ;

parameter_expr
        : NAME
                {
                  // if there's no modifier, might be a loop variable too
                  if (np->getParseExpressionsFlag()) $$ = createIdent(@1);
                }
        | REF NAME
                {
                  if (np->getParseExpressionsFlag()) $$ = createIdent(@2);
                  np->getErrors()->add(ps.params,"`ref' modifier no longer supported (add `function' "
                                     "modifier to destination parameter instead)");
                }
        | REF ANCESTOR NAME
                {
                  if (np->getParseExpressionsFlag()) $$ = createIdent(@3);
                  np->getErrors()->add(ps.params,"`ancestor' and `ref' modifiers no longer supported");
                }
        | ANCESTOR REF NAME
                {
                  if (np->getParseExpressionsFlag()) $$ = createIdent(@3);
                  np->getErrors()->add(ps.params,"`ancestor' and `ref' modifiers no longer supported");
                }
        | ANCESTOR NAME
                {
                  if (np->getParseExpressionsFlag()) $$ = createIdent(@2);
                  np->getErrors()->add(ps.params,"`ancestor' modifier no longer supported");
                }
        ;

special_expr
        : SUBMODINDEX
                { if (np->getParseExpressionsFlag()) $$ = createFunction("index"); }
        | SUBMODINDEX '(' ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction("index"); }
        | SIZEOF '(' NAME ')'
                { if (np->getParseExpressionsFlag()) $$ = createFunction("sizeof", createIdent(@3)); }
        ;

literal
        : stringliteral
        | boolliteral
        | numliteral
        ;

stringliteral
        : STRINGCONSTANT
                { if (np->getParseExpressionsFlag()) $$ = createLiteral(NED_CONST_STRING, trimQuotes(@1), @1); }
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
                { if (np->getParseExpressionsFlag()) $$ = createQuantity(toString(@1)); }
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

NEDElement *doParseNED(NEDParser *p, const char *nedtext)
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
        {np->getErrors()->add(NULL, "unable to allocate work memory"); return false;}

    // create parser state and NEDFileNode
    np = p;
    resetParserState();
    ps.nedfile = new NedFileNode();

    // store file name with slashes always, even on Windows -- neddoc relies on that
    ps.nedfile->setFilename(slashifyFilename(np->getFileName()).c_str());

    // store file comment
    //FIXME ps.nedfile->setBannerComment(nedsource->getFileComment());

    if (np->getStoreSourceFlag())
        storeSourceCode(ps.nedfile, np->getSource()->getFullTextPos());

    // parse
    int ret;
    try
    {
        ret = yyparse();
    }
    catch (NEDException *e)
    {
        INTERNAL_ERROR1(NULL, "error during parsing: %s", e->errorMessage());
        yy_delete_buffer(handle);
        delete e;
        return 0;
    }

    yy_delete_buffer(handle);
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

// this function depends too much on ps, cannot be put into nedyylib.cc
ChannelSpecNode *createChannelSpec(NEDElement *conn)
{
   ChannelSpecNode *chanspec = (ChannelSpecNode *)createNodeWithTag(NED_CHANNEL_SPEC, ps.conn);
   ps.params = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, chanspec);
   ps.params->setIsImplicit(true);
   return chanspec;
}

