/***************************************************/
/*            OMNeT++ NEDC (JAR) source            */
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
/*  Rewritten                                      */
/*      Andras Varga 1996-97                       */
/*                                                 */
/***************************************************/

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
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
%token CONST NUMERICTYPE STRINGTYPE BOOLTYPE ANYTYPE

%token PLUS MIN MUL DIV MOD EXP
%token SIZEOF SUBMODINDEX
%token EQ NE GT GE LS LE
%token AND OR XOR NOT
%token BIN_AND BIN_OR BIN_XOR BIN_COMPL
%token SHIFT_LEFT SHIFT_RIGHT

/* Operator precedences (low to high) and associativity */
%left AND OR XOR
%left '?' ':'
%left EQ NE GT GE LS LE
%left BIN_AND BIN_OR BIN_XOR
%left SHIFT_LEFT SHIFT_RIGHT
%left PLUS MIN
%left MUL DIV MOD
%right EXP
%left UMIN NOT BIN_COMPL

%start networkdescription

/*
 * This file contains 2 shift-reduce conflicts around 'timeconstant'.
 * 4 more at opt_semicolon's after module/submodule types.
 *
 * bison's "%expect nn" option cannot be used to suppress the
 * warning message because %expect is not recognized by yacc
 */

%{

#include <stdio.h>
#include <malloc.h>  /* for alloca() */

#include "ebnfcfg.h"  /* for NEDC() or GNED() */

#ifdef DOING_NEDC
# include "jar_func.h"
#endif
#ifdef DOING_GNED
# include "parsened.h"
#endif

/* beware: this typedef is replicated in ebnf.lex */
typedef struct {int li; int co;} LineColumn;

extern LineColumn pos,prevpos;

int yylex (void);
void jar_yyrestart(FILE *);
void yyerror (char *s);

/* define YYSTYPE to 'char *'; must be consistent in ebnf.lex/ebnf.y --VA */
#define YYSTYPE   char *

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <string.h>         /* YYVERBOSE needs it */
#endif
%}

%%

networkdescription            /* created --VA */
        : somedefinitions
        ;

somedefinitions
        : definition somedefinitions
        | definition
        ;

definition
        : include                  /* --VA */
             /* | machinesdefinition removed --VA */
        | channeldefinition
        | simpledefinition
        | moduledefinition
        | network                   /* --VA */
        ;

include
        : INCLUDE filenames
        ;

filenames
        : filename ',' filenames
        | filename ';'
        ;

filename
        : STRING
                {NEDC( do_include ($1); )}
        ;

/* Distributed execution and statistical synch. things start here --LG */

/*machinesdefinition
        : MAPPING machinedefinitionentries
        ;
*/

/*machinedefinitionentries
        : physicalmachine ':' virtualmachinelist ';' machinedefinitionentries
        | physicalmachine ':' virtualmachinelist ';'
        ;
*/
/*physicalmachine
        : STRING
                {NEDC( do_physicalmachine ($1); )}
        ;
*/
/*virtualmachinelist
        : NAME ',' virtualmachinelist
                {NEDC( do_addvirtualmachine ($1); )}
        | NAME
                {NEDC( do_addvirtualmachine ($1); )}
        ;
*/

/* Statistical synch. things end here --LG */

channeldefinition
                  /* do_channel args: (name,delay,error,datarate) */
        : CHANNEL NAME ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, NULL, NULL, NULL); )}

        | CHANNEL NAME chdelay ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, $3, NULL, NULL); )}
        | CHANNEL NAME cherror ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, NULL, $3, NULL); )}
        | CHANNEL NAME chdatarate ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, NULL, NULL, $3); )}

        | CHANNEL NAME chdelay cherror ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, $3, $4, NULL); )}
        | CHANNEL NAME chdelay chdatarate ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, $3, NULL, $4); )}
        | CHANNEL NAME cherror chdatarate ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, NULL, $3, $4); )}
        | CHANNEL NAME cherror chdelay ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, $4, $3, NULL); )}
        | CHANNEL NAME chdatarate chdelay ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, $4, NULL, $3); )}
        | CHANNEL NAME chdatarate cherror ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, NULL, $4, $3); )}

        | CHANNEL NAME chdelay cherror chdatarate ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, $3, $4, $5); )}
        | CHANNEL NAME chdelay chdatarate cherror ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, $3, $5, $4); )}
        | CHANNEL NAME cherror chdelay chdatarate ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, $4, $3, $5); )}
        | CHANNEL NAME cherror chdatarate chdelay ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, $5, $3, $4); )}
        | CHANNEL NAME chdatarate chdelay cherror ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, $4, $5, $3); )}
        | CHANNEL NAME chdatarate cherror chdelay ENDCHANNEL opt_semicolon
                {NEDC( do_channel ($2, $5, $4, $3); )}
        ;

cherror
        : ERROR expression opt_semicolon
                {NEDC( $$ = $2; )}
        ;

chdelay
        : DELAY expression opt_semicolon
                {NEDC( $$ = $2; )}
        ;

chdatarate
        : DATARATE expression opt_semicolon
                {NEDC( $$ = $2; )}
        ;

simpledefinition  /* --LG */
        : SIMPLE simplename opt_semicolon
            opt_machineblock
            opt_paramblock
            opt_gateblock
          endsimple opt_semicolon
        ;

simplename
        : NAME
                {NEDC( do_simple ($1); )}
        ;

endsimple
        : ENDSIMPLE NAME
                {NEDC( end_simple ($2); )}
        | ENDSIMPLE
                {NEDC( end_simple (NULL); )}
        ;

moduledefinition
        : MODULE modulename opt_semicolon
            opt_machineblock
            opt_paramblock
            opt_gateblock
            opt_submodblock
            opt_ioifblock
            opt_connblock
            opt_displayblock
          endmodule opt_semicolon /* --LG */
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
opt_ioifblock    : ioifblock    | ;
opt_connblock    : connblock    | ;

machineblock
        : MACHINES ':' machinelist ';'
        ;
machinelist
        : machine ',' machinelist
        | machine

machine
        : NAME
                {NEDC( do_machine ($1); )}
        ;

modulename
        : NAME
                {NEDC( do_module ($1); )}
        ;

endmodule
        : ENDMODULE NAME
                {NEDC( end_module ($2); )}
        | ENDMODULE
                {NEDC( end_module (NULL); )}
        ;

displayblock
        : DISPLAY ':' STRING ';'
                {NEDC( do_displaystr_enclosing ($3); )}
        ;

paramblock
        : PARAMETERS ':'
                {NEDC( do_parameters (); )}
          parameters
        ;

parameters
        : parameter ',' parameters
        | parameter ';'
        ;

parameter
        : NAME
                {NEDC( do_parameter ($1, TYPE_NUMERIC); )}
        | NAME ':' NUMERICTYPE
                {NEDC( do_parameter ($1, TYPE_NUMERIC); )}
        | CONST NAME /* for compatibility */
                {NEDC( do_parameter ($2, TYPE_CONST_NUM); )}
        | NAME ':' CONST
                {NEDC( do_parameter ($1, TYPE_CONST_NUM); )}
        | NAME ':' CONST NUMERICTYPE
                {NEDC( do_parameter ($1, TYPE_CONST_NUM); )}
        | NAME ':' NUMERICTYPE CONST
                {NEDC( do_parameter ($1, TYPE_CONST_NUM); )}
        | NAME ':' STRINGTYPE
                {NEDC( do_parameter ($1, TYPE_STRING); )}
        | NAME ':' BOOLTYPE
                {NEDC( do_parameter ($1, TYPE_BOOL); )}
        | NAME ':' ANYTYPE
                {NEDC( do_parameter ($1, TYPE_ANYTYPE); )}
        ;

gateblock
        : GATES ':'
                {NEDC( do_gates (); )}
          gates
        ;

/* `gates' used to be this:
 *  gates : IN gatesI OUT gatesO  | OUT gatesO | IN gatesI;
 *     (--VA, 8 Nov 96)
 */

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
                {NEDC( do_gatedecl( $1, 1, 1 ); )}
        | NAME
                {NEDC( do_gatedecl( $1, 1, 0 ); )}
        ;

gatesO
        : gateO ',' gatesO
        | gateO ';'
        ;

gateO
        : NAME '[' ']'
                {NEDC( do_gatedecl( $1, 0, 1 ); )}
        | NAME
                {NEDC( do_gatedecl( $1, 0, 0 ); )}
        ;

submodblock
        : SUBMODULES ':'
                {NEDC( do_submodules (); )}
          submodules
        ;

submodules
        : submodule submodules
        | submodule
        ;

submodule
        : NAME ':' NAME opt_semicolon
                {NEDC( do_submodule1 ($1, NULL, $3, NULL); )}
          opt_on_blocks
                {NEDC( do_submodule2 ($1, NULL, $3, NULL); )}
          submodule_body
        | NAME ':' NAME vector opt_semicolon
                {NEDC( do_submodule1 ($1, $4, $3, NULL); )}
          opt_on_blocks
                {NEDC( do_submodule2 ($1, $4, $3, NULL); )}
          submodule_body
        | NAME ':' NAME LIKE NAME opt_semicolon
                {NEDC( do_submodule1 ($1, NULL, $3, $5); )}
          opt_on_blocks
                {NEDC( do_submodule2 ($1, NULL, $3, $5); )}
          submodule_body
        | NAME ':' NAME vector LIKE NAME opt_semicolon
                {NEDC( do_submodule1 ($1, $4, $3, $6); )}
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
          opt_semicolon
        ;

opt_on_blocks
        : on_blocks
        |
            {NEDC( do_empty_onlist(); )}
        ;

on_blocks
        : on_block on_blocks
        | on_block
        ;

opt_substparamblocks
        : substparamblock opt_substparamblocks
        |
        ;

opt_gatesizeblocks
        : gatesizeblock opt_gatesizeblocks
        |
        ;

on_block                            /* --LG */
        : ON ':'
                {NEDC( do_onlist(); )}
          on_list
        | ON IF expression ':'
                {NEDC( open_if($3); do_onlist(); )}
          on_list
                {NEDC( close_if(); )}
        ;

on_list
        : on_mach ',' on_list
        | on_mach ';'
        ;
on_mach
        : NAME
                {NEDC( do_on_mach($1); )}
        ;

substparamblock
        : PARAMETERS ':'
                {NEDC( do_substparams (); )}
          substparameters
        | PARAMETERS IF expression ':'
                {NEDC( open_if($3); do_substparams(); )}
          substparameters
                {NEDC( close_if(); )}


substparameters
        : substparameter ',' substparameters
        | substparameter ';'
        ;

substparameter
        : NAME '=' inputvalue_or_expression
                {NEDC( do_substparameter ($1, $3);)}
        ;

gatesizeblock
        : GATESIZES ':'
                {NEDC( do_gatesizes (); )}
          gatesizes
        | GATESIZES IF expression ':'
                {NEDC( open_if($3); do_gatesizes (); )}
          gatesizes
                {NEDC( close_if(); )}

gatesizes
        : gatesize ',' gatesizes
        | gatesize ';'
        ;

gatesize
        : NAME vector
                {NEDC( do_gatesize ($1, $2); )}
        | NAME
                {NEDC( do_gatesize ($1, NULL); )}
        ;

ioifblock
        : IO_INTERFACES ':'
                {NEDC( do_iointerfaces (); )}
          submodules
        ;

opt_submod_displayblock
        : DISPLAY ':' STRING ';'
                {NEDC( do_displaystr_submod ($3); )}
        |
        ;


/*
* ioifblock  --LG
*       : IO_INTERFACES ':'
*             {NEDC( do_iointerfaces(); )}
*         iointerfaces
*       ;
*
*iointerfaces  --LG
*       : iointerface iointerfaces
*       | iointerface
*
*iointerface  --LG
*       : ioifheader gatesizeblock substparamblock
*             {NEDC( end_iointerface (); )}
*       | ioifheader gatesizeblock
*             {NEDC( end_iointerface (); )}
*       | ioifheader substparamblock
*             {NEDC( end_iointerface (); )}
*       | ioifheader ';'
*             {NEDC( end_iointerface (); )}
*       ;
*
*ioifheader   --LG
*       : NAME ':' NAME vector
*             {NEDC( do_submodule ($1, $3, $4); )}
*       | NAME ':' NAME
*             {NEDC( do_submodule ($1, $3, NULL); )}
*       ;
*/

connblock
        : CONNECTIONS opt_conncheck ':'
                {NEDC( do_connections (); )}
          connections
        ;

opt_conncheck
        : NOCHECK
                {NEDC( set_checkconns(0); )}
        |
                {NEDC( set_checkconns(1); )}
        ;
connections
        : connection comma_or_semicolon connections
        | connection ';'
        ;

connection
        : loopconnection
        | normalconnection
        /* | ifnormalconnection*/
        ;

loopconnection
        : FOR
                {NEDC( do_for (); )}
          indexlist DO notloopconnections ENDFOR /* --LG*/
                {NEDC( end_for (); )}
        ;

indexlist
        : index ',' indexlist
        | index
        ;
index
        : NAME '=' expression TO expression
                {NEDC( do_index ($1, $3, $5); )}
        ;

opt_condition
        : IF expression
                {NEDC( do_condition($2); )}
        |
                {NEDC( do_condition(NULL); )}
        ;

opt_displaystr
        : DISPLAY STRING
                {NEDC( $$ = $2; )}
        |
                {NEDC( $$ = NULL; )}
        ;

notloopconnections /* it was "normalconnections" --LG*/
        : normalconnection comma_or_semicolon notloopconnections
        | normalconnection ';'
        /* | ifnormalconnection comma_or_semicolon notloopconnections */
        /* | ifnormalconnection ';' */
        ;

normalconnection
        : gateL RIGHT_ARROW gateR opt_condition opt_displaystr
                {NEDC( end_connection (NULL, 'R', $5); )}
        | gateL RIGHT_ARROW channeldescr RIGHT_ARROW gateR opt_condition opt_displaystr
                {NEDC( end_connection ($3, 'R', $7); )}
        | gateL LEFT_ARROW gateR opt_condition opt_displaystr
                {NEDC( end_connection (NULL, 'L', $5); )}
        | gateL LEFT_ARROW channeldescr LEFT_ARROW gateR opt_condition opt_displaystr
                {NEDC( end_connection ($3, 'L', $7); )}
        ;
/*
*ifnormalconnection       --LG
*       : gateL RIGHT_ARROW gateR ifpairblock opt_condition
*               {NEDC( end_connection (NULL, 'R' ); )}
*       | gateL RIGHT_ARROW NAME RIGHT_ARROW gateR ifpairblock opt_condition
*               {NEDC( end_connection ($3, 'R' ); )}
*       | gateL LEFT_ARROW gateR ifpairblock opt_condition
*               {NEDC( end_connection (NULL, 'L'); )}
*       | gateL LEFT_ARROW NAME LEFT_ARROW gateR ifpairblock opt_condition
*               {NEDC( end_connection ($3, 'L'); )}
*       ;
*
*ifpairblock
*       : IFPAIR '(' outif '.' NAME ',' inif '.' NAME ')'
*       ;
*
*
*outif   --LG
*       : NAME vector
*               {NEDC( do_outif ($1, $3, $4); )}
*       | NAME
*               {NEDC( do_outif ($1, $3, NULL); )}
*
*
*inif    --LG
*       : NAME vector
*               {NEDC( do_inif ($1, $3, $4); )}
*       | NAME
*               {NEDC( do_inif ($1, $3, NULL); )}
*/

gateL
        : mod_gate_L '.' gate_L
        | mod_gate_L
        ;

mod_gate_L
        : NAME vector
                {NEDC( do_mod_gate_L ($1, $2); )}
        | NAME
                {NEDC( do_mod_gate_L ($1, NULL); )}
        ;

gate_L
        : NAME vector
                {NEDC( do_gate_L ($1, $2); )}
        | NAME
                {NEDC( do_gate_L ($1, NULL); )}
        ;

gateR
        : mod_gate_R '.' gate_R
        | mod_gate_R
        ;

mod_gate_R
        : NAME vector
                {NEDC( do_mod_gate_R ($1, $2); )}
        | NAME
                {NEDC( do_mod_gate_R ($1, NULL); )}
        ;

gate_R
        : NAME vector
                {NEDC( do_gate_R ($1, $2); )}
        | NAME
                {NEDC( do_gate_R ($1, NULL); )}
        ;


channeldescr
        :  /* do_channeldescr args: (name,delay,error,datarate) */
                {NEDC( $$ = do_channeldescr( NULL, NULL, NULL, NULL); )}
        | NAME
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

cddelay
        : DELAY expression
                {NEDC( $$ = $2; )}
        ;

cderror
        : ERROR expression
                {NEDC( $$ = $2; )}
        ;

cddatarate
        : DATARATE expression
                {NEDC( $$ = $2; )}
        ;

network
        : NETWORK NAME ':' NAME opt_semicolon
                {NEDC( do_system ($2); )}
          opt_on_blocks
                {NEDC( do_systemmodule ($2, $4, NULL); )}
          opt_substparamblocks
                {NEDC( do_readallparameters(); )}
          ENDNETWORK
                {NEDC( end_system (); )}
        | NETWORK NAME ':' NAME LIKE NAME opt_semicolon
                {NEDC( do_system ($2); )}
          opt_on_blocks
                {NEDC( do_systemmodule ($2, $4, $6); )}
          opt_substparamblocks
                {NEDC( do_readallparameters(); )}
          ENDNETWORK
                {NEDC( end_system (); )}
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
        : expr
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
                {NEDC( $$ = do_func(2,jar_strdup("bool_and"), $1,$3,NULL); )}
        | expr OR expr
                {NEDC( $$ = do_func(2,jar_strdup("bool_or"), $1,$3,NULL); )}
        | expr XOR expr
                {NEDC( $$ = do_func(2,jar_strdup("bool_xor"), $1,$3,NULL); )}
        | NOT expr
                %prec UMIN
                {NEDC( $$ = do_func(1,jar_strdup("bool_not"), $2,NULL,NULL); )}

        | expr BIN_AND expr
                {NEDC( $$ = do_func(2,jar_strdup("bin_and"), $1,$3,NULL); )}
        | expr BIN_OR expr
                {NEDC( $$ = do_func(2,jar_strdup("bin_or"), $1,$3,NULL); )}
        | expr BIN_XOR expr
                {NEDC( $$ = do_func(2,jar_strdup("bin_xor"), $1,$3,NULL); )}
        | BIN_COMPL expr
                %prec UMIN
                {NEDC( $$ = do_func(1,jar_strdup("bin_compl"), $2,NULL,NULL); )}

        | expr SHIFT_LEFT expr
                {NEDC( $$ = do_func(2,jar_strdup("shift_left"), $1,$3,NULL); )}
        | expr SHIFT_RIGHT expr
                {NEDC( $$ = do_func(2,jar_strdup("shift_right"), $1,$3,NULL); )}

        | expr '?' expr ':' expr
                {NEDC( $$ = do_op( 3, '?', $1,$3,$5); )}

        | NAME '(' ')'
                {NEDC( $$ = do_func(0,$1, NULL,NULL,NULL); )}
        | NAME '(' expr ')'
                {NEDC( $$ = do_func(1,$1, $3,NULL,NULL); )}
        | NAME '(' expr ',' expr ')'
                {NEDC( $$ = do_func(2,$1, $3,$5,NULL); )}
        | NAME '(' expr ',' expr ',' expr ')'
                {NEDC( $$ = do_func(3,$1, $3,$5,$7); )}
        ;

simple_expr
        : parameter_expr
                {NEDC( $$ = $1; )}
        | string_expr
                {NEDC( $$ = $1; )}
        | boolconst_expr
                {NEDC( $$ = $1; )}
        | numconst_expr
                {NEDC( $$ = $1; )}
        | special_expr
                {NEDC( $$ = $1; )}
        ;

parameter_expr
        : REF NAME
                {NEDC( $$ = do_parname ($2,0,0); )}
        | NAME
                {NEDC( $$ = do_parname ($1,0,1); )}
        | REF ANCESTOR NAME
                {NEDC( $$ = do_parname ($3,1,0); )}
        | ANCESTOR REF NAME
                {NEDC( $$ = do_parname ($3,1,0); )}
        | ANCESTOR NAME
                {NEDC( $$ = do_parname ($2,1,1); )}
        ;

string_expr
        : STRING
                {NEDC( $$ = make_literal_expr(TYPE_STRING,$1); )}
        ;

boolconst_expr
        : _TRUE
                {NEDC( $$ = make_literal_expr(TYPE_BOOL,jar_strdup("TRUE")); )}
        | _FALSE
                {NEDC( $$ = make_literal_expr(TYPE_BOOL,jar_strdup("FALSE")); )}
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
                {NEDC( $$ = $1; )}
        | REALCONSTANT
                {NEDC( $$ = $1; )}
        | timeconstant
                {NEDC( $$ = $1; )}
        ;

timeconstant
        : INTCONSTANT NAME opt_timeconstant
                {NEDC( $$ = do_timeconstant($1,$2,$3); )}
        | REALCONSTANT NAME opt_timeconstant
                {NEDC( $$ = do_timeconstant($1,$2,$3); )}
        ;

opt_timeconstant
        : timeconstant
                {NEDC( $$ = $1; )}
        |
                {NEDC( $$ = NULL; )}
        ;

opt_semicolon : ';' | ;
comma_or_semicolon : ',' | ';' | ;

%%

char yyfailure[250] = "";
extern int yydebug; /* needed if compiled by yacc --VA */

extern char textbuf[];
extern char lasttextbuf[];

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
                /*-- fprintf(stderr,
                        "line %d: (E) %s----\t%s\n----\t%s\n\t%*s\n",
                        pos.li, yyfailure, lasttextbuf, textbuf, pos.co, "^");
                 -- shortened --VA*/
                fprintf(stderr,
                        "Error %s %d: %s> %s\n  %*s\n",
                        current_fname, pos.li, yyfailure, textbuf, pos.co, "^");
}

#endif


#ifdef DOING_GNED

void yyerror (char *s)
{
}

#endif


int runparse ()
{
#if YYDEBUG != 0      /* #if added --VA */
        yydebug = YYDEBUGGING_ON;
#endif
        pos.co = 0;
        pos.li = 1;
        prevpos = pos;

        strcpy (lasttextbuf, "");
        strcpy (yyfailure, "");

        jar_yyrestart( yyin );  /* added --VA */

        return yyparse ();
}

