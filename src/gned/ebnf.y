/***************************************************/
/*       OMNeT++/OMNEST NEDC (JAR) source          */
/*                                                 */
/*  File: ebnf.y                                   */
/*                                                 */
/*  Contents:                                      */
/*    parser source                                */
/*                                                 */
/*  Originally by                                  */
/*      Jan Heijmans                               */
/*      Alex Paalvast                              */
/*      Robert van der Leij                        */
/*        (JAR = Jan Alex Robert)                  */
/*  Rewritten                                      */
/*      Andras Varga 1996-99                       */
/*  Modifications                                  */
/*      Lencse Gabor 1998                          */
/*                                                 */
/***************************************************/

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

/*
 * IMPORTANT: This file is shared between NEDC and GNED.
 * The two copies must be kept identical!
 */

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
%token NED_CONST NUMERICTYPE STRINGTYPE BOOLTYPE XMLTYPE ANYTYPE

%token PLUS MIN MUL DIV MOD EXP
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
%left PLUS MIN
%left MUL DIV MOD
%right EXP
%left UMIN NOT BIN_COMPL

%start networkdescription


%{

/*
 * Note:
 * This file (ebnf.y) and ebnf.lex are shared between NEDC and GNED.
 * (Exactly the same files are present in both packages;
 * this is to eliminate the burden of keeping 2 sets of files in sync.)
 *
 * Naturally, NEDC and GNED want to do different things while parsing.
 * Both code is present in this file, using the GNED(), NEC(), DOING_GNED,
 * DOING_NEDC macros. These macros come from ebnfcfg.h which is different
 * in gned and nedc.
 *
 * IMPORTANT:
 * If a grammar rule contains code for both nedc and gned, DO _NOT_ PUT THE
 * NEDC() AND GNED() MACROS IN SEPARATE BRACE PAIRS!
 * So this is WRONG:
 *               {NEDC(...)}
 *               {GNED(...)}
 * This is right:
 *               {NEDC(...)
 *                GNED(...)}
 * The reason is that Bison doesn't handle multiple subsequent actions
 * very well; what's worse, it doesn't even warn about them.
 */


/*
 * Note 2:
 * This file contains 2 shift-reduce conflicts around 'timeconstant'.
 * 4 more at opt_semicolon's after module/submodule types.
 * Plus 3 more to track down.
 *
 * bison's "%expect nn" option cannot be used to suppress the
 * warning message because %expect is not recognized by yacc
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ebnf.h"           /* selects between NEDC and GNED */
#include "ebnfcfg.h"        /* selects between NEDC and GNED */

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#endif


int yylex (void);
void yyrestart(FILE *);
void yyerror (char *s);


//
// stuff needed by NEDC
//
#ifdef DOING_NEDC
#include "jar_func.h"
#include "jar_lib.h"
#endif


//
// stuff needed by GNED
//
#ifdef DOING_GNED

#include "parsened.h"
#include "nedfile.h"

static YYLTYPE NULLPOS={0,0,0,0,0,0};
static int in_loop;
static int in_network;

NEDParser *np;

#define NEDFILE_KEY       np->nedfile_key
#define IMPORTS_KEY       np->imports_key
#define IMPORT_KEY        np->import_key
#define CHANNEL_KEY       np->channel_key
#define CHANATTR_KEY      np->chanattr_key
#define NETWORK_KEY       np->network_key
#define MODULE_KEY        np->module_key  // also plays the role of SIMPLE_KEY
#define PARAMS_KEY        np->params_key
#define PARAM_KEY         np->param_key
#define GATES_KEY         np->gates_key
#define GATE_KEY          np->gate_key
#define MACHINES_KEY      np->machines_key
#define MACHINE_KEY       np->machine_key
#define SUBMODS_KEY       np->submods_key
#define SUBMOD_KEY        np->submod_key
#define SUBSTPARAMS_KEY   np->substparams_key
#define SUBSTPARAM_KEY    np->substparam_key
#define GATESIZES_KEY     np->gatesizes_key
#define GATESIZE_KEY      np->gatesize_key
#define SUBSTMACHINES_KEY np->substmachines_key
#define SUBSTMACHINE_KEY  np->substmachine_key
#define CONNS_KEY         np->conns_key
#define CONN_KEY          np->conn_key
#define CONNATTR_KEY      np->connattr_key
#define FORLOOP_KEY       np->forloop_key
#define LOOPVAR_KEY       np->loopvar_key

void setFileComment(int key);
void setBannerComment(int key, YYLTYPE tokenpos);
void setRightComment(int key, YYLTYPE tokenpos);
void setTrailingComment(int key, YYLTYPE tokenpos);
void setComments(int key, YYLTYPE pos);
void setComments(int key, YYLTYPE firstpos, YYLTYPE lastpos);
int addChanAttr(int channel_key, char *attrname, YYLTYPE valuepos);
int addParameter(int params_key, YYLTYPE namepos, int type);
int addGate(int gates_key, YYLTYPE namepos, int is_in, int is_vector );
int addSubmodule(int submods_key, YYLTYPE namepos, YYLTYPE vectorpos,
                                   YYLTYPE typepos,YYLTYPE likepos);
int addGateSize(int gatesizes_key, YYLTYPE namepos, YYLTYPE vectorpos);
int addSubstparam(int substparams_key, YYLTYPE namepos, YYLTYPE valuepos);
int addSubstmachine(int substmachines_key, YYLTYPE namepos);
int addConnAttr(int conn_key, char *attrname, YYLTYPE valuepos);
int addLoopVar(int forloop_key, YYLTYPE varnamepos, YYLTYPE frompos, YYLTYPE topos);
int addNetwork(int nedfile_key, YYLTYPE namepos, YYLTYPE typepos, YYLTYPE likepos);
int findSubmoduleKey(YYLTYPE modulenamepos);
YYLTYPE trimBrackets(YYLTYPE vectorpos);
YYLTYPE trimQuotes(YYLTYPE vectorpos);
void swapConnection(int conn_key);
void setDisplayString(int key, YYLTYPE dispstrpos);
#endif

%}

%%

networkdescription
        : somedefinitions
        |
        ;

somedefinitions
        : somedefinitions definition
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
                {GNED( IMPORTS_KEY = np->create("imports",NEDFILE_KEY);
                       setComments(IMPORTS_KEY,@1); )}
          filenames ';'
                {/* GNED( setTrailingComment(IMPORTS_KEY,@3); )
                  * comment already stored with last filename */}
        ;

filenames
        : filenames ',' filename
        | filename
        ;

filename
        : STRING
                {NEDC( do_include ($1); )
                 GNED( IMPORT_KEY = np->create("import",IMPORTS_KEY);
                       np->set(IMPORT_KEY,"name",trimQuotes(@1));
                       setComments(IMPORT_KEY,@1); )}
        ;

channeldefinition
                  /* do_channel args: (name,delay,error,datarate) */
        : channelheader endchannel
                {NEDC( do_channel ($1, NULL, NULL, NULL); )}

        | channelheader chdelay endchannel
                {NEDC( do_channel ($1, $2, NULL, NULL); )}
        | channelheader cherror endchannel
                {NEDC( do_channel ($1, NULL, $2, NULL); )}
        | channelheader chdatarate endchannel
                {NEDC( do_channel ($1, NULL, NULL, $2); )}

        | channelheader chdelay cherror endchannel
                {NEDC( do_channel ($1, $2, $3, NULL); )}
        | channelheader chdelay chdatarate endchannel
                {NEDC( do_channel ($1, $2, NULL, $3); )}
        | channelheader cherror chdatarate endchannel
                {NEDC( do_channel ($1, NULL, $2, $3); )}
        | channelheader cherror chdelay endchannel
                {NEDC( do_channel ($1, $3, $2, NULL); )}
        | channelheader chdatarate chdelay endchannel
                {NEDC( do_channel ($1, $3, NULL, $2); )}
        | channelheader chdatarate cherror endchannel
                {NEDC( do_channel ($1, NULL, $3, $2); )}

        | channelheader chdelay cherror chdatarate endchannel
                {NEDC( do_channel ($1, $2, $3, $4); )}
        | channelheader chdelay chdatarate cherror endchannel
                {NEDC( do_channel ($1, $2, $4, $3); )}
        | channelheader cherror chdelay chdatarate endchannel
                {NEDC( do_channel ($1, $3, $2, $4); )}
        | channelheader cherror chdatarate chdelay endchannel
                {NEDC( do_channel ($1, $4, $2, $3); )}
        | channelheader chdatarate chdelay cherror endchannel
                {NEDC( do_channel ($1, $3, $4, $2); )}
        | channelheader chdatarate cherror chdelay endchannel
                {NEDC( do_channel ($1, $4, $3, $2); )}
        ;

channelheader
        : CHANNEL NAME
                {NEDC( $$ = $2; inside_nonvoid_function=1;)
                 GNED( CHANNEL_KEY = np->create("channel",NEDFILE_KEY);
                       np->set(CHANNEL_KEY,"name",@2);
                       setComments(CHANNEL_KEY,@1,@2); )}
        ;


endchannel
        : ENDCHANNEL NAME opt_semicolon
                {NEDC( inside_nonvoid_function=0; )}
                {GNED( setTrailingComment(CHANNEL_KEY,@2); )}
        | ENDCHANNEL opt_semicolon
                {NEDC( inside_nonvoid_function=0; )}
                {GNED( setTrailingComment(CHANNEL_KEY,@1); )}
        ;


cherror
        : ERROR expression opt_semicolon
                {NEDC( $$ = $2; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"error",@2);
                       setComments(CHANATTR_KEY,@1,@3); )}
        ;

chdelay
        : DELAY expression opt_semicolon
                {NEDC( $$ = $2; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"delay",@2);
                       setComments(CHANATTR_KEY,@1,@3); )}
        ;

chdatarate
        : DATARATE expression opt_semicolon
                {NEDC( $$ = $2; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"datarate",@2);
                       setComments(CHANATTR_KEY,@1,@3); )}
        ;

simpledefinition  /* --LG */
        : simpleheader
            opt_machineblock
            opt_paramblock
            opt_gateblock
          endsimple
        ;

simpleheader
        : SIMPLE NAME opt_semicolon
                {NEDC( do_simple ($2); )
                 GNED( MODULE_KEY = np->create("simple",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", @2);
                       setComments(MODULE_KEY,@1,@3); )}
        ;


endsimple
        : ENDSIMPLE NAME opt_semicolon
                {NEDC( end_simple ($2); )
                 GNED( setTrailingComment(MODULE_KEY,@2); )}
        | ENDSIMPLE opt_semicolon
                {NEDC( end_simple (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,@1); )}
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
        ;

moduleheader
        : MODULE NAME opt_semicolon
                {NEDC( do_module ($2); )
                 GNED( MODULE_KEY = np->create("module",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", @2);
                       setComments(MODULE_KEY,@1,@3); )}
        ;

endmodule
        : ENDMODULE NAME opt_semicolon
                {NEDC( end_module ($2); )
                 GNED( setTrailingComment(MODULE_KEY,@2); )}
        | ENDMODULE opt_semicolon
                {NEDC( end_module (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,@1); )}
        ;

opt_machineblock
        : machineblock
        |
             {NEDC( do_machine(jar_strdup("default")); )}
        ;
opt_displayblock : displayblock | ;
opt_paramblock   : paramblock   | ;
opt_gateblock    : gateblock    | ;
opt_submodblock  : submodblock  | ;
opt_connblock    : connblock    | ;

machineblock
        : MACHINES ':'
                {GNED( MACHINES_KEY = np->create("machines",MODULE_KEY);
                       setComments(MACHINES_KEY,@1,@2); )}
          opt_machinelist
        ;

opt_machinelist
        : machinelist ';'
        |
        ;

machinelist
        : machinelist ',' machine
        | machine
        ;

machine
        : NAME
                {NEDC( do_machine ($1); )
                 GNED( MACHINE_KEY = np->create("machine",MACHINES_KEY);
                       np->set(MACHINE_KEY,"name", @1);
                       setComments(MACHINE_KEY,@1,@1); )}
        ;

displayblock
        : DISPLAY ':' STRING ';'
                {NEDC( do_displaystr_enclosing ($3); )
                 GNED( setDisplayString(MODULE_KEY,@3); )}
        ;

paramblock
        : PARAMETERS ':'
                {NEDC( do_parameters (); )
                 GNED( PARAMS_KEY = np->create("params",MODULE_KEY);
                       setComments(PARAMS_KEY,@1,@2); )}
          opt_parameters
        ;

opt_parameters
        : parameters ';'
        |
        ;

parameters
        : parameters ',' parameter
                {GNED( setComments(PARAM_KEY,@3); )}

        | parameter
                {GNED( setComments(PARAM_KEY,@1); )}
        ;

parameter
        : NAME
                {NEDC( do_parameter ($1, TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,@1,TYPE_NUMERIC); )}
        | NAME ':' NUMERICTYPE
                {NEDC( do_parameter ($1, TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,@1,TYPE_NUMERIC); )}
        | NED_CONST NAME /* for compatibility */
                {NEDC( do_parameter ($2, TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,@2,TYPE_CONST_NUM); )}
        | NAME ':' NED_CONST
                {NEDC( do_parameter ($1, TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,@1,TYPE_CONST_NUM); )}
        | NAME ':' NED_CONST NUMERICTYPE
                {NEDC( do_parameter ($1, TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,@1,TYPE_CONST_NUM); )}
        | NAME ':' NUMERICTYPE NED_CONST
                {NEDC( do_parameter ($1, TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,@1,TYPE_CONST_NUM); )}
        | NAME ':' STRINGTYPE
                {NEDC( do_parameter ($1, TYPE_STRING); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,@1,TYPE_STRING); )}
        | NAME ':' BOOLTYPE
                {NEDC( do_parameter ($1, TYPE_BOOL); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,@1,TYPE_BOOL); )}
        | NAME ':' XMLTYPE
                {NEDC( do_parameter ($1, TYPE_XML); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,@1,TYPE_XML); )}
        | NAME ':' ANYTYPE
                {NEDC( do_parameter ($1, TYPE_ANYTYPE); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,@1,TYPE_ANYTYPE); )}
        ;

gateblock
        : GATES ':'
                {NEDC( do_gates (); )
                 GNED( GATES_KEY = np->create("gates",MODULE_KEY);
                       setComments(GATES_KEY,@1,@2); )}
          opt_gates
        ;

opt_gates
        : gates
        |
        ;

gates
        : gates IN gatesI ';'
        | IN  gatesI ';'
        | gates OUT gatesO ';'
        | OUT gatesO ';'
        ;

gatesI
        : gatesI ',' gateI
        | gateI
        ;

gateI
        : NAME '[' ']'
                {NEDC( do_gatedecl( $1, 1, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  @1, 1, 1 );
                       setComments(GATE_KEY,@1,@3); )}
        | NAME
                {NEDC( do_gatedecl( $1, 1, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  @1, 1, 0 );
                       setComments(GATE_KEY,@1); )}
        ;

gatesO
        : gatesO ',' gateO
        | gateO
        ;

gateO
        : NAME '[' ']'
                {NEDC( do_gatedecl( $1, 0, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  @1, 0, 1 );
                       setComments(GATE_KEY,@1,@3); )}
        | NAME
                {NEDC( do_gatedecl( $1, 0, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  @1, 0, 0 );
                       setComments(GATE_KEY,@1); )}
        ;

submodblock
        : SUBMODULES ':'
                {NEDC( do_submodules (); )
                 GNED( SUBMODS_KEY = np->create("submods",MODULE_KEY);
                       setComments(SUBMODS_KEY,@1,@2); )}
          opt_submodules
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
        : NAME ':' NAME opt_semicolon
                {NEDC( do_submodule1 ($1, NULL, $3, NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, @1, NULLPOS, @3, NULLPOS);
                       setComments(SUBMOD_KEY,@1,@4);  )}
          opt_on_blocks
                {NEDC( do_submodule2 ($1, NULL, $3, NULL); )}
          submodule_body
        | NAME ':' NAME vector opt_semicolon
                {NEDC( do_submodule1 ($1, $4, $3, NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, @1, @4, @3, NULLPOS);
                       setComments(SUBMOD_KEY,@1,@5);  )}
          opt_on_blocks
                {NEDC( do_submodule2 ($1, $4, $3, NULL); )}
          submodule_body
        | NAME ':' NAME LIKE NAME opt_semicolon
                {NEDC( do_submodule1 ($1, NULL, $3, $5); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, @1, NULLPOS, @3, @5);
                       setComments(SUBMOD_KEY,@1,@6);  )}
          opt_on_blocks
                {NEDC( do_submodule2 ($1, NULL, $3, $5); )}
          submodule_body
        | NAME ':' NAME vector LIKE NAME opt_semicolon
                {NEDC( do_submodule1 ($1, $4, $3, $6); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, @1, @4, @3, @6);
                       setComments(SUBMOD_KEY,@1,@7);  )}
          opt_on_blocks
                {NEDC( do_submodule2 ($1, $4, $3, $6); )}
          submodule_body
        ;

submodule_body
        : opt_substparamblocks
                {NEDC( do_readallparameters(); )}
          opt_gatesizeblocks
          opt_submod_displayblock
                {NEDC( end_submodule (); )}
        ;

opt_on_blocks
        : on_blocks
        |
            {NEDC( do_empty_onlist(); )}
        ;

on_blocks
        : on_blocks on_block
        | on_block
        ;

on_block                            /* --LG */
        : ON ':'
                {NEDC( do_onlist(); )
                 GNED( SUBSTMACHINES_KEY = np->create("substmachines",in_network?NETWORK_KEY:SUBMOD_KEY);
                       setComments(SUBSTMACHINES_KEY,@1,@2); )}
          opt_on_list
        | ON IF expression ':'
                {NEDC( open_if($3); do_onlist(); )
                 GNED( SUBSTMACHINES_KEY = np->create("substmachines",in_network?NETWORK_KEY:SUBMOD_KEY);
                       np->set(SUBSTMACHINES_KEY,"condition",@3);
                       setComments(SUBSTMACHINES_KEY,@1,@4); )}
          opt_on_list
                {NEDC( close_if(); )}
        ;

opt_on_list
        : on_list ';'
        |
            {NEDC( do_empty_onlist(); )}
        ;

on_list
        : on_list ',' on_mach
        | on_mach
        ;

on_mach
        : NAME
                {NEDC( do_on_mach($1); )
                 GNED( SUBSTMACHINE_KEY = addSubstmachine(SUBSTMACHINES_KEY,@1);
                       setComments(SUBSTMACHINE_KEY,@1); )}
        ;

opt_substparamblocks
        : substparamblocks
        |
        ;

substparamblocks
        : substparamblocks substparamblock
        | substparamblock
        ;

substparamblock
        : PARAMETERS ':'
                {NEDC( do_substparams (); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       setComments(SUBSTPARAMS_KEY,@1,@2); )}
          opt_substparameters
        | PARAMETERS IF expression ':'
                {NEDC( open_if($3); do_substparams(); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       np->set(SUBSTPARAMS_KEY,"condition",@3);
                       setComments(SUBSTPARAMS_KEY,@1,@4); )}
          opt_substparameters
                {NEDC( close_if(); )}
        ;

opt_substparameters
        : substparameters ';'
        |
        ;

substparameters
        : substparameters ',' substparameter
        | substparameter
        ;

substparameter
        : NAME '=' inputvalue_or_expression
                {NEDC( do_substparameter ($1, $3);)
                 GNED( SUBSTPARAM_KEY = addSubstparam(SUBSTPARAMS_KEY,@1,@3);
                       setComments(SUBSTPARAM_KEY,@1,@3); )}
        ;

opt_gatesizeblocks
        : gatesizeblock opt_gatesizeblocks
        |
        ;

gatesizeblock
        : GATESIZES ':'
                {NEDC( do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       setComments(GATESIZES_KEY,@1,@2); )}
          opt_gatesizes
        | GATESIZES IF expression ':'
                {NEDC( open_if($3); do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       np->set(GATESIZES_KEY,"condition",@3);
                       setComments(GATESIZES_KEY,@1,@4); )}
          opt_gatesizes
                {NEDC( close_if(); )}
        ;

opt_gatesizes
        : gatesizes ';'
        |
        ;

gatesizes
        : gatesizes ',' gatesize
        | gatesize
        ;

gatesize
        : NAME vector
                {NEDC( do_gatesize ($1, $2); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,@1,@2);
                       setComments(GATESIZE_KEY,@1,@2); )}
        | NAME
                {NEDC( do_gatesize ($1, NULL); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,@1,NULLPOS);
                       setComments(GATESIZE_KEY,@1); )}
        ;

opt_submod_displayblock
        : DISPLAY ':' STRING ';'
                {NEDC( do_displaystr_submod ($3); )
                 GNED( setDisplayString(SUBMOD_KEY,@3); )}
        |
        ;

connblock
        : CONNECTIONS NOCHECK ':'
                {NEDC( set_checkconns(0); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","1");
                       setComments(CONNS_KEY,@1,@3); )}
          opt_connections
        | CONNECTIONS ':'
                {NEDC( set_checkconns(1); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","0");
                       setComments(CONNS_KEY,@1,@2); )}
          opt_connections
        ;

opt_connections
        : connections ';'
        |
        ;

connections
        : connections comma_or_semicolon connection
        | connection
        ;

connection
        : loopconnection
        | notloopconnection
        ;

loopconnection
        : FOR
                {NEDC( do_for (); )
                 GNED( FORLOOP_KEY = np->create("forloop",CONNS_KEY);
                       in_loop=1; setComments(FORLOOP_KEY,@1); )}
          loopvarlist DO notloopconnections ENDFOR /* --LG*/
                {NEDC( end_for (); )
                 GNED( in_loop=0; setTrailingComment(FORLOOP_KEY,@6); )}
        ;

loopvarlist
        : loopvarlist ',' loopvar
        | loopvar
        ;

loopvar
        : NAME '=' expression TO expression
                {NEDC( do_index ($1, $3, $5); )
                 GNED( LOOPVAR_KEY=addLoopVar(FORLOOP_KEY,@1,@3,@5);
                       setComments(LOOPVAR_KEY,@1,@5); )}
        ;

opt_conn_condition
        : IF expression
                {NEDC( do_condition($2); )
                 GNED( np->set(CONN_KEY,"condition",@2); )}
        |
                {NEDC( do_condition(NULL); )}
        ;

opt_conn_displaystr
        : DISPLAY STRING
                {NEDC( $$ = $2; )
                 GNED( setDisplayString(CONN_KEY,@2); )}
        |
                {NEDC( $$ = NULL; )}
        ;

notloopconnections /* it was "normalconnections" --LG*/
        : notloopconnections notloopconnection comma_or_semicolon
        | notloopconnection comma_or_semicolon
        ;

notloopconnection
        : gate_spec_L RIGHT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr
                {NEDC( end_connection (NULL, 'R', $5); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,@1,@5); )}
        | gate_spec_L RIGHT_ARROW channeldescr RIGHT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr
                {NEDC( end_connection ($3, 'R', $7); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,@1,@7); )}
        | gate_spec_L LEFT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr
                {NEDC( end_connection (NULL, 'L', $5); )
                 GNED( swapConnection(CONN_KEY);
                       np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,@1,@5); )}
        | gate_spec_L LEFT_ARROW channeldescr LEFT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr
                {NEDC( end_connection ($3, 'L', $7); )
                 GNED( swapConnection(CONN_KEY);
                 np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,@1,@7); )}
        ;

gate_spec_L
        : mod_L '.' gate_L
        | parentgate_L
        ;

mod_L
        : NAME vector
                {NEDC( do_mod_L ($1, $2); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey(@1));
                       np->set(CONN_KEY, "src-mod-index", trimBrackets(@2)); )}
        | NAME
                {NEDC( do_mod_L ($1, NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey(@1)); )}
        ;

gate_L
        : NAME vector
                {NEDC( do_gate_L ($1, $2); )
                 GNED( np->set(CONN_KEY, "srcgate", @1);
                       np->set(CONN_KEY, "src-gate-index", trimBrackets(@2)); )}
        | NAME
                {NEDC( do_gate_L ($1, NULL); )
                 GNED( np->set(CONN_KEY, "srcgate", @1); )}
        | NAME PLUSPLUS
                {NEDC( do_gate_L ($1, NULL); )
                 GNED( np->set(CONN_KEY, "srcgate", @1);
                       np->set(CONN_KEY, "src-gate-plusplus", "1"); )}
        ;

parentgate_L
        : NAME vector
                {NEDC( do_mod_L (NULL, NULL); do_gate_L ($1, $2); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", @1);
                       np->set(CONN_KEY, "src-gate-index", trimBrackets(@2)); )}
        | NAME
                {NEDC( do_mod_L (NULL, NULL); do_gate_L ($1, NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", @1); )}
        | NAME PLUSPLUS
                {NEDC( do_mod_L (NULL, NULL); do_gate_L ($1, NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", @1);
                       np->set(CONN_KEY, "src-gate-plusplus", "1");  )}
        ;

gate_spec_R
        : mod_R '.' gate_R
        | parentgate_R
        ;

mod_R
        : NAME vector
                {NEDC( do_mod_R ($1, $2); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey(@1));
                       np->set(CONN_KEY, "dest-mod-index", trimBrackets(@2)); )}
        | NAME
                {NEDC( do_mod_R ($1, NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey(@1)); )}
        ;

gate_R
        : NAME vector
                {NEDC( do_gate_R ($1, $2); )
                 GNED( np->set(CONN_KEY, "destgate", @1);
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets(@2)); )}
        | NAME
                {NEDC( do_gate_R ($1, NULL); )
                 GNED( np->set(CONN_KEY, "destgate", @1); )}
        | NAME PLUSPLUS
                {NEDC( do_gate_R ($1, NULL); )
                 GNED( np->set(CONN_KEY, "destgate", @1);
                       np->set(CONN_KEY, "dest-gate-plusplus", "1"); )}
        ;

parentgate_R
        : NAME vector
                {NEDC( do_mod_R (NULL, NULL); do_gate_R ($1, $2); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", @1);
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets(@2)); )}
        | NAME
                {NEDC( do_mod_R (NULL, NULL); do_gate_R ($1, NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", @1); )}
        | NAME PLUSPLUS
                {NEDC( do_mod_R (NULL, NULL); do_gate_R ($1, NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", @1);
                       np->set(CONN_KEY, "dest-gate-plusplus", "1"); )}
        ;


channeldescr
        :  /* do_channeldescr args: (name,delay,error,datarate) */
                {NEDC( $$ = do_channeldescr( NULL, NULL, NULL, NULL); )}
        | cdname
                {NEDC( $$ = do_channeldescr($1, NULL,NULL,NULL); )}
        | cddelay
                {NEDC( $$ = do_channeldescr( NULL, $1, NULL, NULL); )}
        | cderror
                {NEDC( $$ = do_channeldescr( NULL, NULL, $1, NULL); )}
        | cddatarate
                {NEDC( $$ = do_channeldescr( NULL, NULL, NULL, $1); )}

        | cddelay cderror
                {NEDC( $$ = do_channeldescr( NULL, $1, $2, NULL); )}
        | cddelay cddatarate
                {NEDC( $$ = do_channeldescr( NULL, $1, NULL, $2); )}
        | cderror cddatarate
                {NEDC( $$ = do_channeldescr( NULL, NULL, $1, $2); )}
        | cderror cddelay
                {NEDC( $$ = do_channeldescr( NULL, $2, $1, NULL); )}
        | cddatarate cddelay
                {NEDC( $$ = do_channeldescr( NULL, $2, NULL, $1); )}
        | cddatarate cderror
                {NEDC( $$ = do_channeldescr( NULL, NULL, $2, $1); )}

        | cddelay cderror cddatarate
                {NEDC( $$ = do_channeldescr( NULL, $1, $2, $3); )}
        | cddelay cddatarate cderror
                {NEDC( $$ = do_channeldescr( NULL, $1, $3, $2); )}
        | cderror cddelay cddatarate
                {NEDC( $$ = do_channeldescr( NULL, $2, $1, $3); )}
        | cderror cddatarate cddelay
                {NEDC( $$ = do_channeldescr( NULL, $3, $1, $2); )}
        | cddatarate cddelay cderror
                {NEDC( $$ = do_channeldescr( NULL, $2, $3, $1); )}
        | cddatarate cderror cddelay
                {NEDC( $$ = do_channeldescr( NULL, $3, $2, $1); )}
        ;

cdname
        : NAME
                {NEDC( $$ = $1; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"channel",@1); )}
        ;

cddelay
        : DELAY expression
                {NEDC( $$ = $2; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"delay",@2); )}
        ;

cderror
        : ERROR expression
                {NEDC( $$ = $2; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"error",@2); )}
        ;

cddatarate
        : DATARATE expression
                {NEDC( $$ = $2; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"datarate",@2); )}
        ;

network /* opt_on_blocks NOT GOOD!!! appends stuff to last _submodule_ */
        : NETWORK NAME ':' NAME opt_semicolon
                {NEDC( do_system ($2); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,@2,@4,NULLPOS);
                       setComments(NETWORK_KEY,@1,@5); in_network=1;)}
          opt_on_blocks
                {NEDC( do_systemmodule ($2, $4, NULL); )}
          opt_substparamblocks
                {NEDC( do_readallparameters(); )}
          endnetwork
                {NEDC( end_system (); )}
        | NETWORK NAME ':' NAME LIKE NAME opt_semicolon
                {NEDC( do_system ($2); )}
          opt_on_blocks
                {NEDC( do_systemmodule ($2, $4, $6); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,@2,@4,@6);
                       setComments(NETWORK_KEY,@1,@7); )}
          opt_substparamblocks
                {NEDC( do_readallparameters(); )}
          endnetwork
                {NEDC( end_system (); )}
        ;

endnetwork
        : ENDNETWORK opt_semicolon
                {GNED( setTrailingComment(NETWORK_KEY,@1); in_network=0; )}
        ;

vector
        : '[' expression ']'
                {NEDC( $$ = $2; )}
        ;

inputvalue_or_expression
        : inputvalue
        | expression
        ;

inputvalue
        : INPUT '(' expression ',' expression ')'
                {NEDC( $$ = do_inputvalue($3, $5); )}
        | INPUT '(' expression ')'
                {NEDC( $$ = do_inputvalue($3, NULL); )}
        | INPUT '(' ')'
                {NEDC( $$ = do_inputvalue(NULL, NULL); )}
        | INPUT
                {NEDC( $$ = do_inputvalue(NULL, NULL); )}
        ;

expression
        :
          expr
                {NEDC( $$ = end_expr( $1 ); )}
        ;

expr
        : simple_expr
                {NEDC( $$ = $1; )}
        | '(' expr ')'
                {NEDC( $$ = $2; )}

        | expr PLUS expr
                {NEDC( $$ = do_op( 2, '+', $1,$3,NULL); )}
        | expr MIN expr
                {NEDC( $$ = do_op( 2, '-', $1,$3,NULL); )}
        | expr MUL expr
                {NEDC( $$ = do_op( 2, '*', $1,$3,NULL); )}
        | expr DIV expr
                {NEDC( $$ = do_op( 2, '/', $1,$3,NULL); )}
        | expr MOD expr
                {NEDC( $$ = do_op( 2, '%', $1,$3,NULL); )}
        | expr EXP expr
                {NEDC( $$ = do_op( 2, '^', $1,$3,NULL); )}

        | MIN expr
                %prec UMIN
                {NEDC( $$ = do_addminus($2); )}

        | expr EQ expr
                {NEDC( $$ = do_op( 2, '=', $1,$3,NULL); )}
        | expr NE expr
                {NEDC( $$ = do_op( 2, '!', $1,$3,NULL); )}
        | expr GT expr
                {NEDC( $$ = do_op( 2, '>', $1,$3,NULL); )}
        | expr GE expr
                {NEDC( $$ = do_op( 2, '}', $1,$3,NULL); )}
        | expr LS expr
                {NEDC( $$ = do_op( 2, '<', $1,$3,NULL); )}
        | expr LE expr
                {NEDC( $$ = do_op( 2, '{', $1,$3,NULL); )}

        | expr AND expr
                {NEDC( $$ = do_func(2,jar_strdup("bool_and"), $1,$3,NULL,NULL); )}
        | expr OR expr
                {NEDC( $$ = do_func(2,jar_strdup("bool_or"), $1,$3,NULL,NULL); )}
        | expr XOR expr
                {NEDC( $$ = do_func(2,jar_strdup("bool_xor"), $1,$3,NULL,NULL); )}
        | NOT expr
                %prec UMIN
                {NEDC( $$ = do_func(1,jar_strdup("bool_not"), $2,NULL,NULL,NULL); )}

        | expr BIN_AND expr
                {NEDC( $$ = do_func(2,jar_strdup("bin_and"), $1,$3,NULL,NULL); )}
        | expr BIN_OR expr
                {NEDC( $$ = do_func(2,jar_strdup("bin_or"), $1,$3,NULL,NULL); )}
        | expr BIN_XOR expr
                {NEDC( $$ = do_func(2,jar_strdup("bin_xor"), $1,$3,NULL,NULL); )}
        | BIN_COMPL expr
                %prec UMIN
                {NEDC( $$ = do_func(1,jar_strdup("bin_compl"), $2,NULL,NULL,NULL); )}
        | expr SHIFT_LEFT expr
                {NEDC( $$ = do_func(2,jar_strdup("shift_left"), $1,$3,NULL,NULL); )}
        | expr SHIFT_RIGHT expr
                {NEDC( $$ = do_func(2,jar_strdup("shift_right"), $1,$3,NULL,NULL); )}
        | expr '?' expr ':' expr
                {NEDC( $$ = do_op( 3, '?', $1,$3,$5); )}
        | NAME '(' ')'
                {NEDC( $$ = do_func(0,$1, NULL,NULL,NULL,NULL); )}
        | NAME '(' expr ')'
                {NEDC( $$ = do_func(1,$1, $3,NULL,NULL,NULL); )}
        | NAME '(' expr ',' expr ')'
                {NEDC( $$ = do_func(2,$1, $3,$5,NULL,NULL); )}
        | NAME '(' expr ',' expr ',' expr ')'
                {NEDC( $$ = do_func(3,$1, $3,$5,$7,NULL); )}
        | NAME '(' expr ',' expr ',' expr ',' expr ')'
                {NEDC( $$ = do_func(3,$1, $3,$5,$7,$9); )}
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
                {NEDC( $$ = do_parname (0,0,$1,0,1); )}
        | REF NAME
                {NEDC( $$ = do_parname (0,0,$2,0,0); )}
        | ANCESTOR NAME
                {NEDC( $$ = do_parname (0,0,$2,1,1); )}
        | REF ANCESTOR NAME
                {NEDC( $$ = do_parname (0,0,$3,1,0); )}
        | ANCESTOR REF NAME
                {NEDC( $$ = do_parname (0,0,$3,1,0); )}

        | NAME '.' NAME
                {NEDC( $$ = do_parname ($1,0,$3,0,1); )}
        | REF NAME '.' NAME
                {NEDC( $$ = do_parname ($2,0,$4,0,0); )}

        | NAME vector '.' NAME
                {NEDC( $$ = do_parname ($1,$2,$4,0,1); )}
        | REF NAME vector '.' NAME
                {NEDC( $$ = do_parname ($2,$3,$5,0,0); )}
        ;

string_expr
        : STRING
                {NEDC( $$ = make_literal_expr(TYPE_STRING,$1); )}
        ;

boolconst_expr
        : _TRUE
                {NEDC( $$ = make_literal_expr(TYPE_BOOL,jar_strdup("true")); )}
        | _FALSE
                {NEDC( $$ = make_literal_expr(TYPE_BOOL,jar_strdup("false")); )}
        ;

numconst_expr
        : numconst
                {NEDC( $$ = make_literal_expr(TYPE_NUMERIC,$1); )}
        ;

special_expr
        : SUBMODINDEX
                {NEDC( $$ = make_literal_expr(TYPE_NUMERIC,jar_strdup("(long)sub_i")); )}
        | SIZEOF '(' NAME ')'
                {NEDC( $$ = do_sizeof ($3); )}
        ;

numconst
        : INTCONSTANT
        | REALCONSTANT
        | timeconstant
        ;

timeconstant
        : INTCONSTANT NAME timeconstant
                {NEDC( $$ = do_timeconstant($1,$2,$3); )}
        | REALCONSTANT NAME timeconstant
                {NEDC( $$ = do_timeconstant($1,$2,$3); )}
        | INTCONSTANT NAME
                {NEDC( $$ = do_timeconstant($1,$2,NULL); )}
        | REALCONSTANT NAME
                {NEDC( $$ = do_timeconstant($1,$2,NULL); )}
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

int runparse ()
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

        return yyparse();
}

#ifdef DOING_NEDC

extern char current_fname[];

void yyerror (char *s)
{
        fflush (stdout);
        if (strlen (s))
                strcpy (yyfailure, s);
        if (yyfailure [strlen (yyfailure) - 1] != '\n')
                strcat (yyfailure, "\n");
        if (yyfailure [0] == '!')
                fprintf(stderr,
                        "Warning %s %d: %s",
                        current_fname, pos.li, yyfailure + 1);
        else
                fprintf(stderr,
                        "Error %s %d: %s> %s\n  %*s\n",
                        current_fname, pos.li, yyfailure, textbuf, pos.co, "^");
}

#endif


#ifdef DOING_GNED

void yyerror (char *s)
{
        if (strlen (s))
                strcpy (yyfailure, s);
        if (yyfailure [strlen(yyfailure)-1] == '\n')
                yyfailure [strlen(yyfailure)-1] = '\0';

        if (yyfailure [0] == '!')
                np->error('W', yyfailure, pos.li, pos.co);
        else
                np->error('E', yyfailure, pos.li, pos.co);
}

#endif


//----------------------------------------------------------------------
//
// stuff needed by GNED
//

#ifdef DOING_GNED

void setFileComment(int key)
{
    np->set(key,"comment", np->nedsource->getFileComment());
}

void setBannerComment(int key, YYLTYPE tokenpos)
{
    np->set(key,"banner-comment", np->nedsource->getBannerComment(tokenpos));
}

void setRightComment(int key, YYLTYPE tokenpos)
{
    np->set(key,"right-comment", np->nedsource->getTrailingComment(tokenpos));
}

void setTrailingComment(int key, YYLTYPE tokenpos)
{
    np->set(key,"trailing-comment", np->nedsource->getTrailingComment(tokenpos));
}

void setComments(int key, YYLTYPE pos)
{
    setBannerComment(key, pos);
    setRightComment(key, pos);
}

void setComments(int key, YYLTYPE firstpos, YYLTYPE lastpos)
{
    YYLTYPE pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;

    setBannerComment(key, pos);
    setRightComment(key, pos);
}

int addChanAttr(int channel_key, char *attrname, YYLTYPE valuepos)
{
    int chanattr_key = np->create("chanattr", channel_key);
    np->set(chanattr_key,"name", attrname);
    np->set(chanattr_key,"value", valuepos);
    return chanattr_key;
}

int addParameter(int params_key, YYLTYPE namepos, int type)
{
   char *s;
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

   int param_key = np->create("param", params_key);
   np->set(param_key, "name", namepos);
   np->set(param_key, "datatype", s);
   return param_key;
}

int addGate(int gates_key, YYLTYPE namepos, int is_in, int is_vector )
{
   int gate_key = np->create("gate", gates_key);
   np->set(gate_key, "name", namepos);
   np->set(gate_key, "gatetype", const_cast<char*> (is_in ? "in" : "out"));
   np->set(gate_key, "isvector", const_cast<char*> (is_vector ? "1" : "0"));
   return gate_key;
}

int addSubmodule(int submods_key, YYLTYPE namepos, YYLTYPE vectorpos,
                                  YYLTYPE typepos,YYLTYPE likepos)
{
   // cut off "[" and "]"
   vectorpos.first_column++;
   vectorpos.last_column--;

   int submod_key = np->create("submod", submods_key);
   np->set(submod_key, "name", namepos);
   np->set(submod_key, "type-name", typepos);
   np->set(submod_key, "vectorsize", vectorpos);
   np->set(submod_key, "like-name", likepos);

   return submod_key;
}

int addGateSize(int gatesizes_key, YYLTYPE namepos, YYLTYPE vectorpos)
{
   // cut off "[" and "]"
   vectorpos.first_column++;
   vectorpos.last_column--;

   int gatesize_key = np->create("gatesize", gatesizes_key);
   np->set(gatesize_key, "name", namepos);
   np->set(gatesize_key, "size", vectorpos);
   return gatesize_key;
}

int addSubstparam(int substparams_key, YYLTYPE namepos, YYLTYPE valuepos)
{
   int substparam_key = np->create("substparam", substparams_key);
   np->set(substparam_key, "name", namepos);
   np->set(substparam_key, "value", valuepos);
   return substparam_key;
}

int addSubstmachine(int substmachines_key, YYLTYPE namepos)
{
   int substmachine_key = np->create("substmachine", substmachines_key);
   np->set(substmachine_key, "value", namepos);
   return substmachine_key;
}

int addLoopVar(int forloop_key, YYLTYPE varnamepos, YYLTYPE frompos, YYLTYPE topos)
{
   int loopvar_key = np->create("loopvar", forloop_key);
   np->set(loopvar_key, "name", varnamepos);
   np->set(loopvar_key, "fromvalue", frompos);
   np->set(loopvar_key, "tovalue", topos);
   return loopvar_key;
}

int addConnAttr(int conn_key, char *attrname, YYLTYPE valuepos)
{
    int connattr_key = np->create("connattr", conn_key);
    np->set(connattr_key,"name", attrname);
    np->set(connattr_key,"value", valuepos);
    return connattr_key;
}


int addNetwork(int nedfile_key, YYLTYPE namepos, YYLTYPE typepos, YYLTYPE likepos)
{
   int network_key = np->create("network", nedfile_key);
   np->set(network_key, "name", namepos);
   np->set(network_key, "type-name", typepos);
   np->set(network_key, "like-name", likepos);

   return network_key;
}

int findSubmoduleKey(YYLTYPE modulenamepos)
{
   int key = np->findChild(SUBMODS_KEY,"name",np->nedsource->get(modulenamepos));

   if (key==-1)
      yyerror("invalid submodule name");
   else if (key==-2)
      yyerror("nonunique submodule name");
   return key;
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

void swapConnection(int conn_key)
{
   np->swap(conn_key, "src-ownerkey", "dest-ownerkey");
   np->swap(conn_key, "srcgate", "destgate");
   np->swap(conn_key, "src-mod-index", "dest-mod-index");
   np->swap(conn_key, "src-gate-index", "dest-gate-index");
   np->swap(conn_key, "src-gate-plusplus", "dest-gate-plusplus");
}

void setDisplayString(int key, YYLTYPE dispstrpos)
{
  // cut off quotes
  dispstrpos.first_column++;
  dispstrpos.last_column--;

  np->set(key,"displaystring", np->nedsource->get(dispstrpos) );
}


#endif



