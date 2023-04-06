/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 75 "ebnf.y"


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
void yyerror (const char *s);


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
int addChanAttr(int channel_key, const char *attrname, YYLTYPE valuepos);
int addParameter(int params_key, YYLTYPE namepos, int type);
int addGate(int gates_key, YYLTYPE namepos, int is_in, int is_vector );
int addSubmodule(int submods_key, YYLTYPE namepos, YYLTYPE vectorpos,
                                   YYLTYPE typepos,YYLTYPE likepos);
int addGateSize(int gatesizes_key, YYLTYPE namepos, YYLTYPE vectorpos);
int addSubstparam(int substparams_key, YYLTYPE namepos, YYLTYPE valuepos);
int addSubstmachine(int substmachines_key, YYLTYPE namepos);
int addConnAttr(int conn_key, const char *attrname, YYLTYPE valuepos);
int addLoopVar(int forloop_key, YYLTYPE varnamepos, YYLTYPE frompos, YYLTYPE topos);
int addNetwork(int nedfile_key, YYLTYPE namepos, YYLTYPE typepos, YYLTYPE likepos);
int findSubmoduleKey(YYLTYPE modulenamepos);
YYLTYPE trimBrackets(YYLTYPE vectorpos);
YYLTYPE trimQuotes(YYLTYPE vectorpos);
void swapConnection(int conn_key);
void setDisplayString(int key, YYLTYPE dispstrpos);
#endif


#line 204 "ebnf.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "ebnf.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INCLUDE = 3,                    /* INCLUDE  */
  YYSYMBOL_SIMPLE = 4,                     /* SIMPLE  */
  YYSYMBOL_CHANNEL = 5,                    /* CHANNEL  */
  YYSYMBOL_DELAY = 6,                      /* DELAY  */
  YYSYMBOL_ERROR = 7,                      /* ERROR  */
  YYSYMBOL_DATARATE = 8,                   /* DATARATE  */
  YYSYMBOL_MODULE = 9,                     /* MODULE  */
  YYSYMBOL_PARAMETERS = 10,                /* PARAMETERS  */
  YYSYMBOL_GATES = 11,                     /* GATES  */
  YYSYMBOL_GATESIZES = 12,                 /* GATESIZES  */
  YYSYMBOL_SUBMODULES = 13,                /* SUBMODULES  */
  YYSYMBOL_CONNECTIONS = 14,               /* CONNECTIONS  */
  YYSYMBOL_DISPLAY = 15,                   /* DISPLAY  */
  YYSYMBOL_IN = 16,                        /* IN  */
  YYSYMBOL_OUT = 17,                       /* OUT  */
  YYSYMBOL_NOCHECK = 18,                   /* NOCHECK  */
  YYSYMBOL_LEFT_ARROW = 19,                /* LEFT_ARROW  */
  YYSYMBOL_RIGHT_ARROW = 20,               /* RIGHT_ARROW  */
  YYSYMBOL_FOR = 21,                       /* FOR  */
  YYSYMBOL_TO = 22,                        /* TO  */
  YYSYMBOL_DO = 23,                        /* DO  */
  YYSYMBOL_IF = 24,                        /* IF  */
  YYSYMBOL_LIKE = 25,                      /* LIKE  */
  YYSYMBOL_NETWORK = 26,                   /* NETWORK  */
  YYSYMBOL_ENDSIMPLE = 27,                 /* ENDSIMPLE  */
  YYSYMBOL_ENDMODULE = 28,                 /* ENDMODULE  */
  YYSYMBOL_ENDCHANNEL = 29,                /* ENDCHANNEL  */
  YYSYMBOL_ENDNETWORK = 30,                /* ENDNETWORK  */
  YYSYMBOL_ENDFOR = 31,                    /* ENDFOR  */
  YYSYMBOL_MACHINES = 32,                  /* MACHINES  */
  YYSYMBOL_ON = 33,                        /* ON  */
  YYSYMBOL_IO_INTERFACES = 34,             /* IO_INTERFACES  */
  YYSYMBOL_IFPAIR = 35,                    /* IFPAIR  */
  YYSYMBOL_INTCONSTANT = 36,               /* INTCONSTANT  */
  YYSYMBOL_REALCONSTANT = 37,              /* REALCONSTANT  */
  YYSYMBOL_NAME = 38,                      /* NAME  */
  YYSYMBOL_STRING = 39,                    /* STRING  */
  YYSYMBOL__TRUE = 40,                     /* _TRUE  */
  YYSYMBOL__FALSE = 41,                    /* _FALSE  */
  YYSYMBOL_INPUT = 42,                     /* INPUT  */
  YYSYMBOL_REF = 43,                       /* REF  */
  YYSYMBOL_ANCESTOR = 44,                  /* ANCESTOR  */
  YYSYMBOL_NED_CONST = 45,                 /* NED_CONST  */
  YYSYMBOL_NUMERICTYPE = 46,               /* NUMERICTYPE  */
  YYSYMBOL_STRINGTYPE = 47,                /* STRINGTYPE  */
  YYSYMBOL_BOOLTYPE = 48,                  /* BOOLTYPE  */
  YYSYMBOL_XMLTYPE = 49,                   /* XMLTYPE  */
  YYSYMBOL_ANYTYPE = 50,                   /* ANYTYPE  */
  YYSYMBOL_PLUS = 51,                      /* PLUS  */
  YYSYMBOL_MIN = 52,                       /* MIN  */
  YYSYMBOL_MUL = 53,                       /* MUL  */
  YYSYMBOL_DIV = 54,                       /* DIV  */
  YYSYMBOL_MOD = 55,                       /* MOD  */
  YYSYMBOL_EXP = 56,                       /* EXP  */
  YYSYMBOL_SIZEOF = 57,                    /* SIZEOF  */
  YYSYMBOL_SUBMODINDEX = 58,               /* SUBMODINDEX  */
  YYSYMBOL_PLUSPLUS = 59,                  /* PLUSPLUS  */
  YYSYMBOL_EQ = 60,                        /* EQ  */
  YYSYMBOL_NE = 61,                        /* NE  */
  YYSYMBOL_GT = 62,                        /* GT  */
  YYSYMBOL_GE = 63,                        /* GE  */
  YYSYMBOL_LS = 64,                        /* LS  */
  YYSYMBOL_LE = 65,                        /* LE  */
  YYSYMBOL_AND = 66,                       /* AND  */
  YYSYMBOL_OR = 67,                        /* OR  */
  YYSYMBOL_XOR = 68,                       /* XOR  */
  YYSYMBOL_NOT = 69,                       /* NOT  */
  YYSYMBOL_BIN_AND = 70,                   /* BIN_AND  */
  YYSYMBOL_BIN_OR = 71,                    /* BIN_OR  */
  YYSYMBOL_BIN_XOR = 72,                   /* BIN_XOR  */
  YYSYMBOL_BIN_COMPL = 73,                 /* BIN_COMPL  */
  YYSYMBOL_SHIFT_LEFT = 74,                /* SHIFT_LEFT  */
  YYSYMBOL_SHIFT_RIGHT = 75,               /* SHIFT_RIGHT  */
  YYSYMBOL_INVALID_CHAR = 76,              /* INVALID_CHAR  */
  YYSYMBOL_77_ = 77,                       /* '?'  */
  YYSYMBOL_78_ = 78,                       /* ':'  */
  YYSYMBOL_UMIN = 79,                      /* UMIN  */
  YYSYMBOL_80_ = 80,                       /* ';'  */
  YYSYMBOL_81_ = 81,                       /* ','  */
  YYSYMBOL_82_ = 82,                       /* '['  */
  YYSYMBOL_83_ = 83,                       /* ']'  */
  YYSYMBOL_84_ = 84,                       /* '='  */
  YYSYMBOL_85_ = 85,                       /* '.'  */
  YYSYMBOL_86_ = 86,                       /* '('  */
  YYSYMBOL_87_ = 87,                       /* ')'  */
  YYSYMBOL_YYACCEPT = 88,                  /* $accept  */
  YYSYMBOL_networkdescription = 89,        /* networkdescription  */
  YYSYMBOL_somedefinitions = 90,           /* somedefinitions  */
  YYSYMBOL_definition = 91,                /* definition  */
  YYSYMBOL_import = 92,                    /* import  */
  YYSYMBOL_93_1 = 93,                      /* $@1  */
  YYSYMBOL_filenames = 94,                 /* filenames  */
  YYSYMBOL_filename = 95,                  /* filename  */
  YYSYMBOL_channeldefinition = 96,         /* channeldefinition  */
  YYSYMBOL_channelheader = 97,             /* channelheader  */
  YYSYMBOL_endchannel = 98,                /* endchannel  */
  YYSYMBOL_99_2 = 99,                      /* $@2  */
  YYSYMBOL_100_3 = 100,                    /* $@3  */
  YYSYMBOL_cherror = 101,                  /* cherror  */
  YYSYMBOL_chdelay = 102,                  /* chdelay  */
  YYSYMBOL_chdatarate = 103,               /* chdatarate  */
  YYSYMBOL_simpledefinition = 104,         /* simpledefinition  */
  YYSYMBOL_simpleheader = 105,             /* simpleheader  */
  YYSYMBOL_endsimple = 106,                /* endsimple  */
  YYSYMBOL_moduledefinition = 107,         /* moduledefinition  */
  YYSYMBOL_moduleheader = 108,             /* moduleheader  */
  YYSYMBOL_endmodule = 109,                /* endmodule  */
  YYSYMBOL_opt_machineblock = 110,         /* opt_machineblock  */
  YYSYMBOL_opt_displayblock = 111,         /* opt_displayblock  */
  YYSYMBOL_opt_paramblock = 112,           /* opt_paramblock  */
  YYSYMBOL_opt_gateblock = 113,            /* opt_gateblock  */
  YYSYMBOL_opt_submodblock = 114,          /* opt_submodblock  */
  YYSYMBOL_opt_connblock = 115,            /* opt_connblock  */
  YYSYMBOL_machineblock = 116,             /* machineblock  */
  YYSYMBOL_117_4 = 117,                    /* $@4  */
  YYSYMBOL_opt_machinelist = 118,          /* opt_machinelist  */
  YYSYMBOL_machinelist = 119,              /* machinelist  */
  YYSYMBOL_machine = 120,                  /* machine  */
  YYSYMBOL_displayblock = 121,             /* displayblock  */
  YYSYMBOL_paramblock = 122,               /* paramblock  */
  YYSYMBOL_123_5 = 123,                    /* $@5  */
  YYSYMBOL_opt_parameters = 124,           /* opt_parameters  */
  YYSYMBOL_parameters = 125,               /* parameters  */
  YYSYMBOL_parameter = 126,                /* parameter  */
  YYSYMBOL_gateblock = 127,                /* gateblock  */
  YYSYMBOL_128_6 = 128,                    /* $@6  */
  YYSYMBOL_opt_gates = 129,                /* opt_gates  */
  YYSYMBOL_gates = 130,                    /* gates  */
  YYSYMBOL_gatesI = 131,                   /* gatesI  */
  YYSYMBOL_gateI = 132,                    /* gateI  */
  YYSYMBOL_gatesO = 133,                   /* gatesO  */
  YYSYMBOL_gateO = 134,                    /* gateO  */
  YYSYMBOL_submodblock = 135,              /* submodblock  */
  YYSYMBOL_136_7 = 136,                    /* $@7  */
  YYSYMBOL_opt_submodules = 137,           /* opt_submodules  */
  YYSYMBOL_submodules = 138,               /* submodules  */
  YYSYMBOL_submodule = 139,                /* submodule  */
  YYSYMBOL_140_8 = 140,                    /* $@8  */
  YYSYMBOL_141_9 = 141,                    /* $@9  */
  YYSYMBOL_142_10 = 142,                   /* $@10  */
  YYSYMBOL_143_11 = 143,                   /* $@11  */
  YYSYMBOL_144_12 = 144,                   /* $@12  */
  YYSYMBOL_145_13 = 145,                   /* $@13  */
  YYSYMBOL_146_14 = 146,                   /* $@14  */
  YYSYMBOL_147_15 = 147,                   /* $@15  */
  YYSYMBOL_submodule_body = 148,           /* submodule_body  */
  YYSYMBOL_149_16 = 149,                   /* $@16  */
  YYSYMBOL_opt_on_blocks = 150,            /* opt_on_blocks  */
  YYSYMBOL_on_blocks = 151,                /* on_blocks  */
  YYSYMBOL_on_block = 152,                 /* on_block  */
  YYSYMBOL_153_17 = 153,                   /* $@17  */
  YYSYMBOL_154_18 = 154,                   /* $@18  */
  YYSYMBOL_opt_on_list = 155,              /* opt_on_list  */
  YYSYMBOL_on_list = 156,                  /* on_list  */
  YYSYMBOL_on_mach = 157,                  /* on_mach  */
  YYSYMBOL_opt_substparamblocks = 158,     /* opt_substparamblocks  */
  YYSYMBOL_substparamblocks = 159,         /* substparamblocks  */
  YYSYMBOL_substparamblock = 160,          /* substparamblock  */
  YYSYMBOL_161_19 = 161,                   /* $@19  */
  YYSYMBOL_162_20 = 162,                   /* $@20  */
  YYSYMBOL_opt_substparameters = 163,      /* opt_substparameters  */
  YYSYMBOL_substparameters = 164,          /* substparameters  */
  YYSYMBOL_substparameter = 165,           /* substparameter  */
  YYSYMBOL_opt_gatesizeblocks = 166,       /* opt_gatesizeblocks  */
  YYSYMBOL_gatesizeblock = 167,            /* gatesizeblock  */
  YYSYMBOL_168_21 = 168,                   /* $@21  */
  YYSYMBOL_169_22 = 169,                   /* $@22  */
  YYSYMBOL_opt_gatesizes = 170,            /* opt_gatesizes  */
  YYSYMBOL_gatesizes = 171,                /* gatesizes  */
  YYSYMBOL_gatesize = 172,                 /* gatesize  */
  YYSYMBOL_opt_submod_displayblock = 173,  /* opt_submod_displayblock  */
  YYSYMBOL_connblock = 174,                /* connblock  */
  YYSYMBOL_175_23 = 175,                   /* $@23  */
  YYSYMBOL_176_24 = 176,                   /* $@24  */
  YYSYMBOL_opt_connections = 177,          /* opt_connections  */
  YYSYMBOL_connections = 178,              /* connections  */
  YYSYMBOL_connection = 179,               /* connection  */
  YYSYMBOL_loopconnection = 180,           /* loopconnection  */
  YYSYMBOL_181_25 = 181,                   /* $@25  */
  YYSYMBOL_loopvarlist = 182,              /* loopvarlist  */
  YYSYMBOL_loopvar = 183,                  /* loopvar  */
  YYSYMBOL_opt_conn_condition = 184,       /* opt_conn_condition  */
  YYSYMBOL_opt_conn_displaystr = 185,      /* opt_conn_displaystr  */
  YYSYMBOL_notloopconnections = 186,       /* notloopconnections  */
  YYSYMBOL_notloopconnection = 187,        /* notloopconnection  */
  YYSYMBOL_gate_spec_L = 188,              /* gate_spec_L  */
  YYSYMBOL_mod_L = 189,                    /* mod_L  */
  YYSYMBOL_gate_L = 190,                   /* gate_L  */
  YYSYMBOL_parentgate_L = 191,             /* parentgate_L  */
  YYSYMBOL_gate_spec_R = 192,              /* gate_spec_R  */
  YYSYMBOL_mod_R = 193,                    /* mod_R  */
  YYSYMBOL_gate_R = 194,                   /* gate_R  */
  YYSYMBOL_parentgate_R = 195,             /* parentgate_R  */
  YYSYMBOL_channeldescr = 196,             /* channeldescr  */
  YYSYMBOL_cdname = 197,                   /* cdname  */
  YYSYMBOL_cddelay = 198,                  /* cddelay  */
  YYSYMBOL_cderror = 199,                  /* cderror  */
  YYSYMBOL_cddatarate = 200,               /* cddatarate  */
  YYSYMBOL_network = 201,                  /* network  */
  YYSYMBOL_202_26 = 202,                   /* $@26  */
  YYSYMBOL_203_27 = 203,                   /* $@27  */
  YYSYMBOL_204_28 = 204,                   /* $@28  */
  YYSYMBOL_205_29 = 205,                   /* $@29  */
  YYSYMBOL_206_30 = 206,                   /* $@30  */
  YYSYMBOL_207_31 = 207,                   /* $@31  */
  YYSYMBOL_endnetwork = 208,               /* endnetwork  */
  YYSYMBOL_vector = 209,                   /* vector  */
  YYSYMBOL_inputvalue_or_expression = 210, /* inputvalue_or_expression  */
  YYSYMBOL_inputvalue = 211,               /* inputvalue  */
  YYSYMBOL_expression = 212,               /* expression  */
  YYSYMBOL_expr = 213,                     /* expr  */
  YYSYMBOL_simple_expr = 214,              /* simple_expr  */
  YYSYMBOL_parameter_expr = 215,           /* parameter_expr  */
  YYSYMBOL_string_expr = 216,              /* string_expr  */
  YYSYMBOL_boolconst_expr = 217,           /* boolconst_expr  */
  YYSYMBOL_numconst_expr = 218,            /* numconst_expr  */
  YYSYMBOL_special_expr = 219,             /* special_expr  */
  YYSYMBOL_numconst = 220,                 /* numconst  */
  YYSYMBOL_timeconstant = 221,             /* timeconstant  */
  YYSYMBOL_opt_semicolon = 222,            /* opt_semicolon  */
  YYSYMBOL_comma_or_semicolon = 223        /* comma_or_semicolon  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  22
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   679

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  88
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  136
/* YYNRULES -- Number of rules.  */
#define YYNRULES  305
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  511

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   332


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      86,    87,     2,     2,    81,     2,    85,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    78,    80,
       2,    84,     2,    77,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    82,     2,    83,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    79
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   211,   211,   212,   216,   217,   221,   222,   223,   224,
     225,   230,   229,   238,   239,   243,   252,   255,   257,   259,
     262,   264,   266,   268,   270,   272,   275,   277,   279,   281,
     283,   285,   290,   300,   299,   303,   302,   309,   316,   323,
     330,   338,   347,   350,   356,   367,   375,   378,   384,   386,
     388,   388,   389,   389,   390,   390,   391,   391,   392,   392,
     396,   395,   402,   403,   407,   408,   412,   420,   427,   426,
     434,   435,   439,   442,   447,   450,   453,   456,   459,   462,
     465,   468,   471,   474,   481,   480,   488,   489,   493,   494,
     495,   496,   500,   501,   505,   509,   516,   517,   521,   525,
     533,   532,   540,   541,   545,   546,   551,   555,   550,   558,
     562,   557,   565,   569,   564,   572,   576,   571,   582,   581,
     589,   591,   595,   596,   601,   600,   606,   605,   615,   617,
     621,   622,   626,   633,   634,   638,   639,   644,   643,   649,
     648,   658,   659,   663,   664,   668,   675,   676,   681,   680,
     686,   685,   695,   696,   700,   701,   705,   709,   716,   719,
     724,   723,   730,   729,   738,   739,   743,   744,   748,   749,
     754,   753,   763,   764,   768,   775,   779,   783,   787,   791,
     792,   796,   799,   802,   806,   813,   814,   818,   823,   830,
     834,   837,   844,   850,   855,   864,   865,   869,   873,   879,
     883,   886,   893,   898,   902,   912,   913,   915,   917,   919,
     922,   924,   926,   928,   930,   932,   935,   937,   939,   941,
     943,   945,   950,   956,   962,   968,   975,   979,   981,   974,
     985,   987,   991,   984,   997,  1002,  1007,  1008,  1012,  1014,
    1016,  1018,  1024,  1029,  1031,  1034,  1036,  1038,  1040,  1042,
    1044,  1047,  1051,  1053,  1055,  1057,  1059,  1061,  1064,  1066,
    1068,  1070,  1074,  1076,  1078,  1080,  1083,  1085,  1087,  1089,
    1091,  1093,  1095,  1097,  1102,  1103,  1104,  1105,  1106,  1110,
    1112,  1114,  1116,  1118,  1121,  1123,  1126,  1128,  1133,  1138,
    1140,  1145,  1150,  1152,  1157,  1158,  1159,  1163,  1165,  1167,
    1169,  1174,  1174,  1175,  1175,  1175
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INCLUDE", "SIMPLE",
  "CHANNEL", "DELAY", "ERROR", "DATARATE", "MODULE", "PARAMETERS", "GATES",
  "GATESIZES", "SUBMODULES", "CONNECTIONS", "DISPLAY", "IN", "OUT",
  "NOCHECK", "LEFT_ARROW", "RIGHT_ARROW", "FOR", "TO", "DO", "IF", "LIKE",
  "NETWORK", "ENDSIMPLE", "ENDMODULE", "ENDCHANNEL", "ENDNETWORK",
  "ENDFOR", "MACHINES", "ON", "IO_INTERFACES", "IFPAIR", "INTCONSTANT",
  "REALCONSTANT", "NAME", "STRING", "_TRUE", "_FALSE", "INPUT", "REF",
  "ANCESTOR", "NED_CONST", "NUMERICTYPE", "STRINGTYPE", "BOOLTYPE",
  "XMLTYPE", "ANYTYPE", "PLUS", "MIN", "MUL", "DIV", "MOD", "EXP",
  "SIZEOF", "SUBMODINDEX", "PLUSPLUS", "EQ", "NE", "GT", "GE", "LS", "LE",
  "AND", "OR", "XOR", "NOT", "BIN_AND", "BIN_OR", "BIN_XOR", "BIN_COMPL",
  "SHIFT_LEFT", "SHIFT_RIGHT", "INVALID_CHAR", "'?'", "':'", "UMIN", "';'",
  "','", "'['", "']'", "'='", "'.'", "'('", "')'", "$accept",
  "networkdescription", "somedefinitions", "definition", "import", "$@1",
  "filenames", "filename", "channeldefinition", "channelheader",
  "endchannel", "$@2", "$@3", "cherror", "chdelay", "chdatarate",
  "simpledefinition", "simpleheader", "endsimple", "moduledefinition",
  "moduleheader", "endmodule", "opt_machineblock", "opt_displayblock",
  "opt_paramblock", "opt_gateblock", "opt_submodblock", "opt_connblock",
  "machineblock", "$@4", "opt_machinelist", "machinelist", "machine",
  "displayblock", "paramblock", "$@5", "opt_parameters", "parameters",
  "parameter", "gateblock", "$@6", "opt_gates", "gates", "gatesI", "gateI",
  "gatesO", "gateO", "submodblock", "$@7", "opt_submodules", "submodules",
  "submodule", "$@8", "$@9", "$@10", "$@11", "$@12", "$@13", "$@14",
  "$@15", "submodule_body", "$@16", "opt_on_blocks", "on_blocks",
  "on_block", "$@17", "$@18", "opt_on_list", "on_list", "on_mach",
  "opt_substparamblocks", "substparamblocks", "substparamblock", "$@19",
  "$@20", "opt_substparameters", "substparameters", "substparameter",
  "opt_gatesizeblocks", "gatesizeblock", "$@21", "$@22", "opt_gatesizes",
  "gatesizes", "gatesize", "opt_submod_displayblock", "connblock", "$@23",
  "$@24", "opt_connections", "connections", "connection", "loopconnection",
  "$@25", "loopvarlist", "loopvar", "opt_conn_condition",
  "opt_conn_displaystr", "notloopconnections", "notloopconnection",
  "gate_spec_L", "mod_L", "gate_L", "parentgate_L", "gate_spec_R", "mod_R",
  "gate_R", "parentgate_R", "channeldescr", "cdname", "cddelay", "cderror",
  "cddatarate", "network", "$@26", "$@27", "$@28", "$@29", "$@30", "$@31",
  "endnetwork", "vector", "inputvalue_or_expression", "inputvalue",
  "expression", "expr", "simple_expr", "parameter_expr", "string_expr",
  "boolconst_expr", "numconst_expr", "special_expr", "numconst",
  "timeconstant", "opt_semicolon", "comma_or_semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-443)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-305)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     210,  -443,    62,    73,    81,    92,   150,   210,  -443,  -443,
    -443,   224,  -443,   120,  -443,   120,  -443,   145,   106,  -443,
     106,   122,  -443,  -443,   274,   274,   274,   -26,  -443,    11,
     125,   133,   138,   154,  -443,   154,  -443,    57,  -443,  -443,
    -443,  -443,   184,   189,   202,   153,  -443,  -443,  -443,   144,
     180,   274,   156,  -443,   274,   274,   274,   106,   527,  -443,
    -443,  -443,  -443,  -443,  -443,  -443,  -443,   106,   106,   106,
    -443,  -443,   167,   137,  -443,   167,   119,  -443,   137,   119,
    -443,   146,   247,  -443,   247,  -443,   145,    18,   157,   157,
     274,   225,    84,   194,   -50,   278,  -443,   306,  -443,   307,
    -443,  -443,   443,  -443,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   274,   274,  -443,  -443,  -443,  -443,  -443,
     223,  -443,   223,  -443,   223,  -443,   223,  -443,   223,  -443,
     223,   318,  -443,   190,   264,  -443,   325,  -443,   319,  -443,
     189,   202,  -443,  -443,   275,  -443,  -443,   320,   323,   324,
     279,  -443,  -443,   272,  -443,   234,   234,   309,   309,   309,
     309,   230,   230,   230,   230,   230,   230,   579,   579,   579,
     604,   604,   604,   219,   219,   499,  -443,  -443,  -443,  -443,
    -443,  -443,  -443,  -443,  -443,   176,  -443,    -9,  -443,    76,
    -443,   285,   352,  -443,   106,   334,  -443,   274,  -443,  -443,
    -443,   330,  -443,   274,  -443,   318,   292,   341,  -443,   186,
    -443,   217,   106,  -443,  -443,    -8,   381,  -443,  -443,   -10,
    -443,   334,  -443,   360,  -443,   554,  -443,   303,  -443,  -443,
      -9,   361,   362,  -443,   244,  -443,   364,   326,  -443,   327,
     370,  -443,   334,   274,  -443,   393,  -443,   274,  -443,   363,
     372,  -443,  -443,  -443,  -443,  -443,   336,   197,  -443,   337,
     214,  -443,   361,   362,   328,  -443,   364,  -443,  -443,     0,
     369,    80,  -443,  -443,   355,   398,    28,  -443,   393,  -443,
     406,  -443,  -443,   357,  -443,   361,   359,  -443,   362,   216,
     226,   400,  -443,     0,  -443,   113,  -443,   228,  -443,  -443,
    -443,   300,   358,  -443,   365,   106,  -443,   393,  -443,  -443,
    -443,   241,  -443,   274,  -443,   414,  -443,   274,  -443,  -443,
    -443,  -443,  -443,  -443,  -443,    17,  -443,   408,  -443,   366,
       1,  -443,     0,   183,   183,   410,  -443,  -443,  -443,   398,
    -443,   398,   371,   412,   106,  -443,   471,   415,    23,  -443,
     368,   -14,  -443,  -443,   274,   274,   274,   -13,   430,   378,
    -443,   446,  -443,   317,    25,   321,   430,   455,   -35,  -443,
     414,  -443,  -443,  -443,   395,  -443,   249,  -443,  -443,  -443,
     106,   444,  -443,   334,   274,   447,   408,  -443,  -443,  -443,
    -443,   399,   274,   473,   448,   451,   482,   484,   482,   486,
     484,   486,   473,   451,  -443,  -443,  -443,   412,   207,  -443,
     412,  -443,   106,   334,  -443,   478,    -1,   253,  -443,  -443,
     462,  -443,    86,  -443,   117,   430,  -443,  -443,  -443,  -443,
    -443,  -443,  -443,   430,  -443,   416,  -443,  -443,  -443,  -443,
     334,  -443,  -443,   393,   274,  -443,   253,  -443,  -443,  -443,
    -443,  -443,   473,   473,   168,  -443,   334,   393,  -443,  -443,
    -443,  -443,  -443,  -443,  -443,   116,   393,  -443,  -443,   500,
     274,  -443,  -443,   393,    37,   501,   500,   432,  -443,   274,
    -443,   450,  -443,  -443,  -443,   466,   483,   490,  -443,   458,
    -443,   256,  -443,   467,   483,  -443,  -443,   483,  -443,  -443,
    -443
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       3,    11,     0,     0,     0,     0,     0,     2,     5,     6,
       7,     0,     8,    49,     9,    49,    10,     0,   302,    32,
     302,     0,     1,     4,     0,     0,     0,   302,    16,     0,
       0,     0,     0,    53,    48,    53,    15,     0,    14,   301,
      41,    45,     0,   294,   295,   279,   288,   289,   290,     0,
       0,     0,     0,   292,     0,     0,     0,   302,   242,   243,
     274,   275,   276,   277,   278,   291,   296,   302,   302,   302,
      35,    18,     0,     0,    17,     0,     0,    19,     0,     0,
      60,     0,    55,    52,    55,    12,     0,   302,   299,   300,
       0,     0,     0,     0,   280,     0,   281,     0,   251,     0,
     261,   265,     0,    38,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    37,    39,    33,    36,    23,
       0,    22,     0,    20,     0,    21,     0,    25,     0,    24,
       0,    63,    68,     0,     0,    54,    57,    13,     0,   226,
       0,     0,   297,   298,     0,   284,   269,     0,     0,     0,
       0,   282,   283,     0,   244,   245,   246,   247,   248,   249,
     250,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     262,   263,   264,   266,   267,     0,    34,    28,    29,    26,
      27,    31,    30,    66,    61,     0,    65,    71,    84,   302,
      40,     0,    59,    56,   302,   121,   235,     0,   270,   286,
     285,     0,   293,     0,    62,     0,    74,     0,    69,     0,
      73,    87,   302,    43,   100,     0,    51,    58,   230,     0,
     227,   120,   123,     0,   287,   268,    64,     0,    76,    70,
       0,     0,     0,    85,    86,    42,   103,     0,   162,     0,
       0,    50,   121,     0,   124,   134,   122,     0,   271,    77,
      75,    80,    81,    82,    83,    72,    95,     0,    93,    99,
       0,    97,     0,     0,     0,   101,   102,   105,   160,   165,
       0,   302,    44,   231,     0,   129,     0,   228,   133,   136,
       0,    78,    79,     0,    89,     0,     0,    91,     0,     0,
       0,     0,   104,   165,   170,   193,   163,   305,   167,   168,
     169,     0,     0,   186,     0,   302,    47,   134,   126,   132,
     125,     0,   131,     0,   137,     0,   135,     0,   272,    94,
      92,    98,    96,    88,    90,   302,   161,     0,   194,   192,
     164,   303,     0,   205,   205,     0,    67,    46,   232,   129,
     128,     0,     0,   142,   302,   229,     0,     0,   302,   106,
       0,     0,   173,   166,     0,     0,     0,   203,   176,     0,
     196,     0,   206,   207,   208,   209,   176,     0,   190,   185,
       0,   127,   130,   139,     0,   138,     0,   144,   234,   273,
     302,     0,   109,   121,     0,     0,     0,   223,   224,   225,
     204,   202,     0,   178,     0,     0,   210,   211,   213,   212,
     214,   215,   178,     0,   191,   189,   233,   142,     0,   141,
       0,   112,   302,   121,   107,     0,     0,   305,   172,   175,
       0,   183,   200,   195,   203,   176,   216,   217,   218,   219,
     220,   221,   181,   176,   140,   241,   145,   236,   237,   143,
     121,   115,   110,   134,     0,   171,   305,   304,   180,   177,
     201,   199,   178,   178,     0,   113,   121,   134,   108,   118,
     174,   179,   184,   182,   240,     0,   134,   116,   111,   147,
       0,   239,   114,   134,     0,   159,   147,     0,   117,     0,
     148,     0,   119,   146,   238,     0,   153,     0,   150,   157,
     149,     0,   155,     0,   153,   156,   152,     0,   158,   151,
     154
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -443,  -443,  -443,   542,  -443,  -443,  -443,   470,  -443,  -443,
      29,  -443,  -443,    79,   100,   105,  -443,  -443,  -443,  -443,
    -443,  -443,   553,  -443,   522,   488,  -443,  -443,  -443,  -443,
    -443,  -443,   385,  -443,  -443,  -443,  -443,  -443,   335,  -443,
    -443,  -443,  -443,   312,   290,   313,   298,  -443,  -443,  -443,
    -443,   347,  -443,  -443,  -443,  -443,  -443,  -443,  -443,  -443,
    -442,  -443,  -249,  -443,   380,  -443,  -443,   254,  -443,   261,
    -244,  -443,   339,  -443,  -443,   196,  -443,   218,   151,  -443,
    -443,  -443,   132,  -443,   139,  -443,  -443,  -443,  -443,   342,
    -443,   305,  -443,  -443,  -443,   252,  -372,  -350,  -443,  -382,
    -443,  -443,  -443,  -443,  -339,  -443,  -443,  -443,   308,  -443,
    -228,  -357,  -257,  -443,  -443,  -443,  -443,  -443,  -443,  -443,
     281,   -43,  -443,  -443,   -25,   -28,  -443,  -443,  -443,  -443,
    -443,  -443,  -443,   266,   -12,  -407
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     6,     7,     8,     9,    17,    37,    38,    10,    11,
      28,   186,   128,    29,    30,    31,    12,    13,   200,    14,
      15,   282,    33,   250,    82,   144,   202,   226,    34,   141,
     194,   195,   196,   251,    83,   197,   218,   219,   220,   145,
     221,   243,   244,   267,   268,   270,   271,   203,   246,   275,
     276,   277,   393,   453,   423,   467,   450,   476,   466,   483,
     468,   479,   230,   231,   232,   285,   349,   320,   321,   322,
     469,   288,   289,   353,   417,   385,   386,   387,   485,   486,
     496,   504,   500,   501,   502,   492,   227,   303,   279,   306,
     307,   308,   309,   337,   361,   362,   403,   431,   426,   310,
     311,   312,   379,   313,   368,   369,   433,   370,   371,   372,
     373,   374,   375,    16,   205,   255,   325,   252,   317,   380,
     355,   401,   446,   447,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    40,   342
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      67,    68,    93,   283,   412,   376,  -222,  -222,    41,   395,
     247,   287,    69,   427,   253,    70,   406,    24,   411,    26,
     458,   304,  -304,    98,   414,   478,   100,   101,   102,   216,
     455,   364,    90,   366,   482,   159,   217,   305,   305,  -304,
      27,   488,   357,   148,   456,   103,   400,    90,   391,   471,
     437,   160,   323,   440,    39,   125,   126,   127,    71,    74,
      77,   489,   442,   462,   157,   154,   435,   396,   254,    90,
     248,   463,  -198,   348,   443,   149,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,    39,    39,    90,
      18,   129,   131,    39,   133,   135,   324,   137,   139,    75,
      78,    19,   472,   473,   222,   490,   407,   409,   315,    20,
      43,    44,    45,    46,    47,    48,    25,    49,    50,    72,
      21,    79,    25,    26,    73,    76,    51,    85,    86,    24,
      25,    52,    53,    24,   424,   460,   408,   410,    27,   436,
      22,   438,    32,    54,    27,   136,    39,    55,   140,   187,
      39,   188,    27,   189,    81,   190,    27,   191,    90,   192,
      56,   156,   338,   132,   452,    26,   400,   130,   138,   233,
     134,   439,    94,   441,    36,   235,    39,   223,    95,   364,
     365,   366,   228,   150,   151,    90,    27,   480,  -188,    90,
      42,   465,  -198,   481,    43,    44,    45,    46,    47,    48,
     245,    49,    50,     1,     2,     3,    80,   477,    96,     4,
      51,   367,    87,    97,   142,    52,    53,    88,   284,   290,
      24,    25,    26,   241,   242,    90,     5,    54,    91,    92,
      89,    55,    99,    43,    44,    45,    46,    47,    48,   445,
      49,    50,    27,    27,    56,   474,   214,   215,   143,    51,
     272,   273,   339,   155,    52,    53,   239,   240,   198,   316,
     104,   105,   106,   107,   108,   109,    54,   294,   295,   158,
      55,   104,   105,   106,   107,   108,   109,   106,   107,   108,
     109,   199,   358,    56,   297,   298,   333,   295,   352,   356,
     119,   120,   121,   347,   122,   123,   334,   298,   340,   341,
      43,    44,    45,    46,    47,    48,   161,    49,    50,   343,
     344,   350,   351,   359,   365,   366,    51,   364,   365,   419,
     420,    52,    53,   457,   341,   415,   506,   507,   201,   397,
     398,   399,   388,    54,   162,   163,   392,    55,   259,   260,
     261,   262,   263,   264,   152,   153,   193,   204,   206,   212,
      56,   209,   210,   224,   211,   109,   225,   229,   234,   425,
     237,   104,   105,   106,   107,   108,   109,   429,   421,   238,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   461,
     119,   120,   121,   448,   122,   123,   249,   124,   281,   266,
     269,   207,   274,   286,   278,   280,   301,   208,   314,   291,
     451,   104,   105,   106,   107,   108,   109,   292,   293,   296,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   470,
     119,   120,   121,   318,   122,   123,   319,   124,   335,   475,
     329,   257,   331,   345,   354,   346,   360,   258,   378,   383,
     384,  -187,   394,   390,   402,   487,   505,   104,   105,   106,
     107,   108,   109,   404,   495,   405,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   413,   119,   120,   121,   418,
     122,   123,   422,   124,  -197,   305,   432,   327,   430,   434,
     366,   365,   364,   328,   104,   105,   106,   107,   108,   109,
     454,   459,   464,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   484,   119,   120,   121,   491,   122,   123,   494,
     124,   499,   104,   105,   106,   107,   108,   109,   497,   503,
     164,   110,   111,   112,   113,   114,   115,   116,   117,   118,
      90,   119,   120,   121,   498,   122,   123,   508,   124,    23,
     104,   105,   106,   107,   108,   109,   147,    84,   389,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    35,   119,
     120,   121,   146,   122,   123,   265,   124,   213,   104,   105,
     106,   107,   108,   109,   299,   330,   300,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   332,   119,   120,   121,
     236,   122,   123,   381,   124,   104,   105,   106,   107,   108,
     109,   256,   382,   444,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   302,   119,   120,   121,   326,   122,   123,
     104,   105,   106,   107,   108,   109,   509,   493,   449,   110,
     111,   112,   113,   114,   115,   336,   510,   363,   428,   119,
     120,   121,   377,   122,   123,   104,   105,   106,   107,   108,
     109,   416,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   122,   123
};

static const yytype_int16 yycheck[] =
{
      25,    26,    45,   252,   376,   344,    19,    20,    20,    23,
      18,   255,    38,   395,    24,    27,   373,     6,   375,     8,
     427,    21,    21,    51,    59,   467,    54,    55,    56,    38,
      31,     6,    82,     8,   476,    85,    45,    38,    38,    38,
      29,   483,    25,    25,   426,    57,    59,    82,    25,   456,
     407,    94,    24,   410,    80,    67,    68,    69,    29,    30,
      31,    24,   412,   435,    92,    90,   405,    81,    78,    82,
      78,   443,    85,   317,   413,    87,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,    80,    80,    82,
      38,    72,    73,    80,    75,    76,    78,    78,    79,    30,
      31,    38,   462,   463,    38,    78,   373,   374,    38,    38,
      36,    37,    38,    39,    40,    41,     7,    43,    44,    29,
      38,    31,     7,     8,    29,    30,    52,    80,    81,     6,
       7,    57,    58,     6,   393,    59,   374,   375,    29,   406,
       0,   408,    32,    69,    29,    76,    80,    73,    79,   130,
      80,   132,    29,   134,    10,   136,    29,   138,    82,   140,
      86,    87,    59,    73,   423,     8,    59,    72,    78,   207,
      75,   409,    38,   411,    39,   213,    80,   199,    44,     6,
       7,     8,   204,    36,    37,    82,    29,    81,    85,    82,
      78,   450,    85,    87,    36,    37,    38,    39,    40,    41,
     222,    43,    44,     3,     4,     5,    78,   466,    38,     9,
      52,    38,    38,    43,    78,    57,    58,    38,   253,   257,
       6,     7,     8,    16,    17,    82,    26,    69,    85,    86,
      38,    73,    86,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    29,    29,    86,    87,    80,    81,    11,    52,
      16,    17,   305,    38,    57,    58,    80,    81,    78,   281,
      51,    52,    53,    54,    55,    56,    69,    80,    81,    85,
      73,    51,    52,    53,    54,    55,    56,    53,    54,    55,
      56,    27,   335,    86,    80,    81,    80,    81,   323,   327,
      70,    71,    72,   315,    74,    75,    80,    81,    80,    81,
      36,    37,    38,    39,    40,    41,    38,    43,    44,    19,
      20,    80,    81,   335,     7,     8,    52,     6,     7,    80,
      81,    57,    58,    80,    81,   378,    80,    81,    13,   364,
     365,   366,   354,    69,    38,    38,   358,    73,    45,    46,
      47,    48,    49,    50,    88,    89,    38,    38,    83,    87,
      86,    38,    38,    78,    85,    56,    14,    33,    38,   394,
      78,    51,    52,    53,    54,    55,    56,   402,   390,    38,
      60,    61,    62,    63,    64,    65,    66,    67,    68,   432,
      70,    71,    72,   418,    74,    75,    15,    77,    28,    38,
      38,    81,    38,    10,    78,    78,    78,    87,    39,    46,
     422,    51,    52,    53,    54,    55,    56,    45,    82,    82,
      60,    61,    62,    63,    64,    65,    66,    67,    68,   454,
      70,    71,    72,    78,    74,    75,    38,    77,    38,   464,
      83,    81,    83,    85,    30,    80,    38,    87,    38,    78,
      38,    85,    84,    38,    24,   480,   499,    51,    52,    53,
      54,    55,    56,    85,   489,    19,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    20,    70,    71,    72,    84,
      74,    75,    38,    77,    85,    38,    38,    81,    15,    38,
       8,     7,     6,    87,    51,    52,    53,    54,    55,    56,
      22,    39,    86,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    12,    70,    71,    72,    15,    74,    75,    87,
      77,    38,    51,    52,    53,    54,    55,    56,    78,    39,
      87,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      82,    70,    71,    72,    78,    74,    75,    80,    77,     7,
      51,    52,    53,    54,    55,    56,    86,    35,    87,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    15,    70,
      71,    72,    84,    74,    75,   240,    77,    78,    51,    52,
      53,    54,    55,    56,   272,   295,   273,    60,    61,    62,
      63,    64,    65,    66,    67,    68,   298,    70,    71,    72,
     215,    74,    75,   349,    77,    51,    52,    53,    54,    55,
      56,   231,   351,   417,    60,    61,    62,    63,    64,    65,
      66,    67,    68,   276,    70,    71,    72,   288,    74,    75,
      51,    52,    53,    54,    55,    56,   504,   486,   420,    60,
      61,    62,    63,    64,    65,   303,   507,   342,   396,    70,
      71,    72,   344,    74,    75,    51,    52,    53,    54,    55,
      56,   380,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,    75
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     9,    26,    89,    90,    91,    92,
      96,    97,   104,   105,   107,   108,   201,    93,    38,    38,
      38,    38,     0,    91,     6,     7,     8,    29,    98,   101,
     102,   103,    32,   110,   116,   110,    39,    94,    95,    80,
     222,   222,    78,    36,    37,    38,    39,    40,    41,    43,
      44,    52,    57,    58,    69,    73,    86,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   212,   212,    38,
     222,    98,   102,   103,    98,   101,   103,    98,   101,   102,
      78,    10,   112,   122,   112,    80,    81,    38,    38,    38,
      82,    85,    86,   209,    38,    44,    38,    43,   213,    86,
     213,   213,   213,   222,    51,    52,    53,    54,    55,    56,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    70,
      71,    72,    74,    75,    77,   222,   222,   222,   100,    98,
     103,    98,   102,    98,   103,    98,   101,    98,   102,    98,
     101,   117,    78,    11,   113,   127,   113,    95,    25,   222,
      36,    37,   221,   221,   212,    38,    87,   213,    85,    85,
     209,    38,    38,    38,    87,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,    99,    98,    98,    98,
      98,    98,    98,    38,   118,   119,   120,   123,    78,    27,
     106,    13,   114,   135,    38,   202,    83,    81,    87,    38,
      38,    85,    87,    78,    80,    81,    38,    45,   124,   125,
     126,   128,    38,   222,    78,    14,   115,   174,   222,    33,
     150,   151,   152,   213,    38,   213,   120,    78,    38,    80,
      81,    16,    17,   129,   130,   222,   136,    18,    78,    15,
     111,   121,   205,    24,    78,   203,   152,    81,    87,    45,
      46,    47,    48,    49,    50,   126,    38,   131,   132,    38,
     133,   134,    16,    17,    38,   137,   138,   139,    78,   176,
      78,    28,   109,   150,   212,   153,    10,   158,   159,   160,
     213,    46,    45,    82,    80,    81,    82,    80,    81,   131,
     133,    78,   139,   175,    21,    38,   177,   178,   179,   180,
     187,   188,   189,   191,    39,    38,   222,   206,    78,    38,
     155,   156,   157,    24,    78,   204,   160,    81,    87,    83,
     132,    83,   134,    80,    80,    38,   177,   181,    59,   209,
      80,    81,   223,    19,    20,    85,    80,   222,   158,   154,
      80,    81,   212,   161,    30,   208,   213,    25,   209,   222,
      38,   182,   183,   179,     6,     7,     8,    38,   192,   193,
     195,   196,   197,   198,   199,   200,   192,   196,    38,   190,
     207,   155,   157,    78,    38,   163,   164,   165,   222,    87,
      38,    25,   222,   140,    84,    23,    81,   212,   212,   212,
      59,   209,    24,   184,    85,    19,   199,   200,   198,   200,
     198,   199,   184,    20,    59,   209,   208,   162,    84,    80,
      81,   222,    38,   142,   150,   212,   186,   187,   183,   212,
      15,   185,    38,   194,    38,   192,   200,   199,   200,   198,
     199,   198,   185,   192,   163,    42,   210,   211,   212,   165,
     144,   222,   150,   141,    22,    31,   187,    80,   223,    39,
      59,   209,   184,   184,    86,   150,   146,   143,   148,   158,
     212,   223,   185,   185,    87,   212,   145,   150,   148,   149,
      81,    87,   148,   147,    12,   166,   167,   212,   148,    24,
      78,    15,   173,   166,    87,   212,   168,    78,    78,    38,
     170,   171,   172,    39,   169,   209,    80,    81,    80,   170,
     172
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    88,    89,    89,    90,    90,    91,    91,    91,    91,
      91,    93,    92,    94,    94,    95,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    97,    99,    98,   100,    98,   101,   102,   103,
     104,   105,   106,   106,   107,   108,   109,   109,   110,   110,
     111,   111,   112,   112,   113,   113,   114,   114,   115,   115,
     117,   116,   118,   118,   119,   119,   120,   121,   123,   122,
     124,   124,   125,   125,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   128,   127,   129,   129,   130,   130,
     130,   130,   131,   131,   132,   132,   133,   133,   134,   134,
     136,   135,   137,   137,   138,   138,   140,   141,   139,   142,
     143,   139,   144,   145,   139,   146,   147,   139,   149,   148,
     150,   150,   151,   151,   153,   152,   154,   152,   155,   155,
     156,   156,   157,   158,   158,   159,   159,   161,   160,   162,
     160,   163,   163,   164,   164,   165,   166,   166,   168,   167,
     169,   167,   170,   170,   171,   171,   172,   172,   173,   173,
     175,   174,   176,   174,   177,   177,   178,   178,   179,   179,
     181,   180,   182,   182,   183,   184,   184,   185,   185,   186,
     186,   187,   187,   187,   187,   188,   188,   189,   189,   190,
     190,   190,   191,   191,   191,   192,   192,   193,   193,   194,
     194,   194,   195,   195,   195,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   197,   198,   199,   200,   202,   203,   204,   201,
     205,   206,   207,   201,   208,   209,   210,   210,   211,   211,
     211,   211,   212,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   214,   214,   214,   214,   214,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   216,   217,
     217,   218,   219,   219,   220,   220,   220,   221,   221,   221,
     221,   222,   222,   223,   223,   223
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     1,     1,
       1,     0,     4,     3,     1,     1,     2,     3,     3,     3,
       4,     4,     4,     4,     4,     4,     5,     5,     5,     5,
       5,     5,     2,     0,     4,     0,     3,     3,     3,     3,
       5,     3,     3,     2,     8,     3,     3,     2,     1,     0,
       1,     0,     1,     0,     1,     0,     1,     0,     1,     0,
       0,     4,     2,     0,     3,     1,     1,     4,     0,     4,
       2,     0,     3,     1,     1,     3,     2,     3,     4,     4,
       3,     3,     3,     3,     0,     4,     1,     0,     4,     3,
       4,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       0,     4,     1,     0,     2,     1,     0,     0,     8,     0,
       0,     9,     0,     0,    10,     0,     0,    11,     0,     4,
       1,     0,     2,     1,     0,     4,     0,     6,     2,     0,
       3,     1,     1,     1,     0,     2,     1,     0,     4,     0,
       6,     2,     0,     3,     1,     3,     2,     0,     0,     4,
       0,     6,     2,     0,     3,     1,     2,     1,     4,     0,
       0,     5,     0,     4,     2,     0,     3,     1,     1,     1,
       0,     6,     3,     1,     5,     2,     0,     2,     0,     3,
       2,     5,     7,     5,     7,     3,     1,     2,     1,     2,
       1,     2,     2,     1,     2,     3,     1,     2,     1,     2,
       1,     2,     2,     1,     2,     0,     1,     1,     1,     1,
       2,     2,     2,     2,     2,     2,     3,     3,     3,     3,
       3,     3,     1,     2,     2,     2,     0,     0,     0,    11,
       0,     0,     0,    13,     2,     3,     1,     1,     6,     4,
       3,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     3,     3,     2,     3,     3,     5,     3,
       4,     6,     8,    10,     1,     1,     1,     1,     1,     1,
       2,     2,     3,     3,     3,     4,     4,     5,     1,     1,
       1,     1,     1,     4,     1,     1,     1,     3,     3,     2,
       2,     1,     0,     1,     1,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 11: /* $@1: %empty  */
#line 230 "ebnf.y"
                {GNED( IMPORTS_KEY = np->create("imports",NEDFILE_KEY);
                       setComments(IMPORTS_KEY,(yylsp[0])); )}
#line 1966 "ebnf.tab.c"
    break;

  case 12: /* import: INCLUDE $@1 filenames ';'  */
#line 233 "ebnf.y"
                {/* GNED( setTrailingComment(IMPORTS_KEY,@3); )
                  * comment already stored with last filename */}
#line 1973 "ebnf.tab.c"
    break;

  case 15: /* filename: STRING  */
#line 244 "ebnf.y"
                {NEDC( do_include (yyvsp[0]); )
                 GNED( IMPORT_KEY = np->create("import",IMPORTS_KEY);
                       np->set(IMPORT_KEY,"name",trimQuotes((yylsp[0])));
                       setComments(IMPORT_KEY,(yylsp[0])); )}
#line 1982 "ebnf.tab.c"
    break;

  case 16: /* channeldefinition: channelheader endchannel  */
#line 253 "ebnf.y"
                {NEDC( do_channel (yyvsp[-1], NULL, NULL, NULL); )}
#line 1988 "ebnf.tab.c"
    break;

  case 17: /* channeldefinition: channelheader chdelay endchannel  */
#line 256 "ebnf.y"
                {NEDC( do_channel (yyvsp[-2], yyvsp[-1], NULL, NULL); )}
#line 1994 "ebnf.tab.c"
    break;

  case 18: /* channeldefinition: channelheader cherror endchannel  */
#line 258 "ebnf.y"
                {NEDC( do_channel (yyvsp[-2], NULL, yyvsp[-1], NULL); )}
#line 2000 "ebnf.tab.c"
    break;

  case 19: /* channeldefinition: channelheader chdatarate endchannel  */
#line 260 "ebnf.y"
                {NEDC( do_channel (yyvsp[-2], NULL, NULL, yyvsp[-1]); )}
#line 2006 "ebnf.tab.c"
    break;

  case 20: /* channeldefinition: channelheader chdelay cherror endchannel  */
#line 263 "ebnf.y"
                {NEDC( do_channel (yyvsp[-3], yyvsp[-2], yyvsp[-1], NULL); )}
#line 2012 "ebnf.tab.c"
    break;

  case 21: /* channeldefinition: channelheader chdelay chdatarate endchannel  */
#line 265 "ebnf.y"
                {NEDC( do_channel (yyvsp[-3], yyvsp[-2], NULL, yyvsp[-1]); )}
#line 2018 "ebnf.tab.c"
    break;

  case 22: /* channeldefinition: channelheader cherror chdatarate endchannel  */
#line 267 "ebnf.y"
                {NEDC( do_channel (yyvsp[-3], NULL, yyvsp[-2], yyvsp[-1]); )}
#line 2024 "ebnf.tab.c"
    break;

  case 23: /* channeldefinition: channelheader cherror chdelay endchannel  */
#line 269 "ebnf.y"
                {NEDC( do_channel (yyvsp[-3], yyvsp[-1], yyvsp[-2], NULL); )}
#line 2030 "ebnf.tab.c"
    break;

  case 24: /* channeldefinition: channelheader chdatarate chdelay endchannel  */
#line 271 "ebnf.y"
                {NEDC( do_channel (yyvsp[-3], yyvsp[-1], NULL, yyvsp[-2]); )}
#line 2036 "ebnf.tab.c"
    break;

  case 25: /* channeldefinition: channelheader chdatarate cherror endchannel  */
#line 273 "ebnf.y"
                {NEDC( do_channel (yyvsp[-3], NULL, yyvsp[-1], yyvsp[-2]); )}
#line 2042 "ebnf.tab.c"
    break;

  case 26: /* channeldefinition: channelheader chdelay cherror chdatarate endchannel  */
#line 276 "ebnf.y"
                {NEDC( do_channel (yyvsp[-4], yyvsp[-3], yyvsp[-2], yyvsp[-1]); )}
#line 2048 "ebnf.tab.c"
    break;

  case 27: /* channeldefinition: channelheader chdelay chdatarate cherror endchannel  */
#line 278 "ebnf.y"
                {NEDC( do_channel (yyvsp[-4], yyvsp[-3], yyvsp[-1], yyvsp[-2]); )}
#line 2054 "ebnf.tab.c"
    break;

  case 28: /* channeldefinition: channelheader cherror chdelay chdatarate endchannel  */
#line 280 "ebnf.y"
                {NEDC( do_channel (yyvsp[-4], yyvsp[-2], yyvsp[-3], yyvsp[-1]); )}
#line 2060 "ebnf.tab.c"
    break;

  case 29: /* channeldefinition: channelheader cherror chdatarate chdelay endchannel  */
#line 282 "ebnf.y"
                {NEDC( do_channel (yyvsp[-4], yyvsp[-1], yyvsp[-3], yyvsp[-2]); )}
#line 2066 "ebnf.tab.c"
    break;

  case 30: /* channeldefinition: channelheader chdatarate chdelay cherror endchannel  */
#line 284 "ebnf.y"
                {NEDC( do_channel (yyvsp[-4], yyvsp[-2], yyvsp[-1], yyvsp[-3]); )}
#line 2072 "ebnf.tab.c"
    break;

  case 31: /* channeldefinition: channelheader chdatarate cherror chdelay endchannel  */
#line 286 "ebnf.y"
                {NEDC( do_channel (yyvsp[-4], yyvsp[-1], yyvsp[-2], yyvsp[-3]); )}
#line 2078 "ebnf.tab.c"
    break;

  case 32: /* channelheader: CHANNEL NAME  */
#line 291 "ebnf.y"
                {NEDC( yyval = yyvsp[0]; inside_nonvoid_function=1;)
                 GNED( CHANNEL_KEY = np->create("channel",NEDFILE_KEY);
                       np->set(CHANNEL_KEY,"name",(yylsp[0]));
                       setComments(CHANNEL_KEY,(yylsp[-1]),(yylsp[0])); )}
#line 2087 "ebnf.tab.c"
    break;

  case 33: /* $@2: %empty  */
#line 300 "ebnf.y"
                {NEDC( inside_nonvoid_function=0; )}
#line 2093 "ebnf.tab.c"
    break;

  case 34: /* endchannel: ENDCHANNEL NAME opt_semicolon $@2  */
#line 301 "ebnf.y"
                {GNED( setTrailingComment(CHANNEL_KEY,(yylsp[-2])); )}
#line 2099 "ebnf.tab.c"
    break;

  case 35: /* $@3: %empty  */
#line 303 "ebnf.y"
                {NEDC( inside_nonvoid_function=0; )}
#line 2105 "ebnf.tab.c"
    break;

  case 36: /* endchannel: ENDCHANNEL opt_semicolon $@3  */
#line 304 "ebnf.y"
                {GNED( setTrailingComment(CHANNEL_KEY,(yylsp[-2])); )}
#line 2111 "ebnf.tab.c"
    break;

  case 37: /* cherror: ERROR expression opt_semicolon  */
#line 310 "ebnf.y"
                {NEDC( yyval = yyvsp[-1]; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"error",(yylsp[-1]));
                       setComments(CHANATTR_KEY,(yylsp[-2]),(yylsp[0])); )}
#line 2119 "ebnf.tab.c"
    break;

  case 38: /* chdelay: DELAY expression opt_semicolon  */
#line 317 "ebnf.y"
                {NEDC( yyval = yyvsp[-1]; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"delay",(yylsp[-1]));
                       setComments(CHANATTR_KEY,(yylsp[-2]),(yylsp[0])); )}
#line 2127 "ebnf.tab.c"
    break;

  case 39: /* chdatarate: DATARATE expression opt_semicolon  */
#line 324 "ebnf.y"
                {NEDC( yyval = yyvsp[-1]; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"datarate",(yylsp[-1]));
                       setComments(CHANATTR_KEY,(yylsp[-2]),(yylsp[0])); )}
#line 2135 "ebnf.tab.c"
    break;

  case 41: /* simpleheader: SIMPLE NAME opt_semicolon  */
#line 339 "ebnf.y"
                {NEDC( do_simple (yyvsp[-1]); )
                 GNED( MODULE_KEY = np->create("simple",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", (yylsp[-1]));
                       setComments(MODULE_KEY,(yylsp[-2]),(yylsp[0])); )}
#line 2144 "ebnf.tab.c"
    break;

  case 42: /* endsimple: ENDSIMPLE NAME opt_semicolon  */
#line 348 "ebnf.y"
                {NEDC( end_simple (yyvsp[-1]); )
                 GNED( setTrailingComment(MODULE_KEY,(yylsp[-1])); )}
#line 2151 "ebnf.tab.c"
    break;

  case 43: /* endsimple: ENDSIMPLE opt_semicolon  */
#line 351 "ebnf.y"
                {NEDC( end_simple (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,(yylsp[-1])); )}
#line 2158 "ebnf.tab.c"
    break;

  case 45: /* moduleheader: MODULE NAME opt_semicolon  */
#line 368 "ebnf.y"
                {NEDC( do_module (yyvsp[-1]); )
                 GNED( MODULE_KEY = np->create("module",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", (yylsp[-1]));
                       setComments(MODULE_KEY,(yylsp[-2]),(yylsp[0])); )}
#line 2167 "ebnf.tab.c"
    break;

  case 46: /* endmodule: ENDMODULE NAME opt_semicolon  */
#line 376 "ebnf.y"
                {NEDC( end_module (yyvsp[-1]); )
                 GNED( setTrailingComment(MODULE_KEY,(yylsp[-1])); )}
#line 2174 "ebnf.tab.c"
    break;

  case 47: /* endmodule: ENDMODULE opt_semicolon  */
#line 379 "ebnf.y"
                {NEDC( end_module (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,(yylsp[-1])); )}
#line 2181 "ebnf.tab.c"
    break;

  case 49: /* opt_machineblock: %empty  */
#line 386 "ebnf.y"
             {NEDC( do_machine(jar_strdup("default")); )}
#line 2187 "ebnf.tab.c"
    break;

  case 60: /* $@4: %empty  */
#line 396 "ebnf.y"
                {GNED( MACHINES_KEY = np->create("machines",MODULE_KEY);
                       setComments(MACHINES_KEY,(yylsp[-1]),(yylsp[0])); )}
#line 2194 "ebnf.tab.c"
    break;

  case 66: /* machine: NAME  */
#line 413 "ebnf.y"
                {NEDC( do_machine (yyvsp[0]); )
                 GNED( MACHINE_KEY = np->create("machine",MACHINES_KEY);
                       np->set(MACHINE_KEY,"name", (yylsp[0]));
                       setComments(MACHINE_KEY,(yylsp[0]),(yylsp[0])); )}
#line 2203 "ebnf.tab.c"
    break;

  case 67: /* displayblock: DISPLAY ':' STRING ';'  */
#line 421 "ebnf.y"
                {NEDC( do_displaystr_enclosing (yyvsp[-1]); )
                 GNED( setDisplayString(MODULE_KEY,(yylsp[-1])); )}
#line 2210 "ebnf.tab.c"
    break;

  case 68: /* $@5: %empty  */
#line 427 "ebnf.y"
                {NEDC( do_parameters (); )
                 GNED( PARAMS_KEY = np->create("params",MODULE_KEY);
                       setComments(PARAMS_KEY,(yylsp[-1]),(yylsp[0])); )}
#line 2218 "ebnf.tab.c"
    break;

  case 72: /* parameters: parameters ',' parameter  */
#line 440 "ebnf.y"
                {GNED( setComments(PARAM_KEY,(yylsp[0])); )}
#line 2224 "ebnf.tab.c"
    break;

  case 73: /* parameters: parameter  */
#line 443 "ebnf.y"
                {GNED( setComments(PARAM_KEY,(yylsp[0])); )}
#line 2230 "ebnf.tab.c"
    break;

  case 74: /* parameter: NAME  */
#line 448 "ebnf.y"
                {NEDC( do_parameter (yyvsp[0], TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[0]),TYPE_NUMERIC); )}
#line 2237 "ebnf.tab.c"
    break;

  case 75: /* parameter: NAME ':' NUMERICTYPE  */
#line 451 "ebnf.y"
                {NEDC( do_parameter (yyvsp[-2], TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_NUMERIC); )}
#line 2244 "ebnf.tab.c"
    break;

  case 76: /* parameter: NED_CONST NAME  */
#line 454 "ebnf.y"
                {NEDC( do_parameter (yyvsp[0], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[0]),TYPE_CONST_NUM); )}
#line 2251 "ebnf.tab.c"
    break;

  case 77: /* parameter: NAME ':' NED_CONST  */
#line 457 "ebnf.y"
                {NEDC( do_parameter (yyvsp[-2], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_CONST_NUM); )}
#line 2258 "ebnf.tab.c"
    break;

  case 78: /* parameter: NAME ':' NED_CONST NUMERICTYPE  */
#line 460 "ebnf.y"
                {NEDC( do_parameter (yyvsp[-3], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-3]),TYPE_CONST_NUM); )}
#line 2265 "ebnf.tab.c"
    break;

  case 79: /* parameter: NAME ':' NUMERICTYPE NED_CONST  */
#line 463 "ebnf.y"
                {NEDC( do_parameter (yyvsp[-3], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-3]),TYPE_CONST_NUM); )}
#line 2272 "ebnf.tab.c"
    break;

  case 80: /* parameter: NAME ':' STRINGTYPE  */
#line 466 "ebnf.y"
                {NEDC( do_parameter (yyvsp[-2], TYPE_STRING); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_STRING); )}
#line 2279 "ebnf.tab.c"
    break;

  case 81: /* parameter: NAME ':' BOOLTYPE  */
#line 469 "ebnf.y"
                {NEDC( do_parameter (yyvsp[-2], TYPE_BOOL); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_BOOL); )}
#line 2286 "ebnf.tab.c"
    break;

  case 82: /* parameter: NAME ':' XMLTYPE  */
#line 472 "ebnf.y"
                {NEDC( do_parameter (yyvsp[-2], TYPE_XML); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_XML); )}
#line 2293 "ebnf.tab.c"
    break;

  case 83: /* parameter: NAME ':' ANYTYPE  */
#line 475 "ebnf.y"
                {NEDC( do_parameter (yyvsp[-2], TYPE_ANYTYPE); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_ANYTYPE); )}
#line 2300 "ebnf.tab.c"
    break;

  case 84: /* $@6: %empty  */
#line 481 "ebnf.y"
                {NEDC( do_gates (); )
                 GNED( GATES_KEY = np->create("gates",MODULE_KEY);
                       setComments(GATES_KEY,(yylsp[-1]),(yylsp[0])); )}
#line 2308 "ebnf.tab.c"
    break;

  case 94: /* gateI: NAME '[' ']'  */
#line 506 "ebnf.y"
                {NEDC( do_gatedecl( yyvsp[-2], 1, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  (yylsp[-2]), 1, 1 );
                       setComments(GATE_KEY,(yylsp[-2]),(yylsp[0])); )}
#line 2316 "ebnf.tab.c"
    break;

  case 95: /* gateI: NAME  */
#line 510 "ebnf.y"
                {NEDC( do_gatedecl( yyvsp[0], 1, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  (yylsp[0]), 1, 0 );
                       setComments(GATE_KEY,(yylsp[0])); )}
#line 2324 "ebnf.tab.c"
    break;

  case 98: /* gateO: NAME '[' ']'  */
#line 522 "ebnf.y"
                {NEDC( do_gatedecl( yyvsp[-2], 0, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  (yylsp[-2]), 0, 1 );
                       setComments(GATE_KEY,(yylsp[-2]),(yylsp[0])); )}
#line 2332 "ebnf.tab.c"
    break;

  case 99: /* gateO: NAME  */
#line 526 "ebnf.y"
                {NEDC( do_gatedecl( yyvsp[0], 0, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  (yylsp[0]), 0, 0 );
                       setComments(GATE_KEY,(yylsp[0])); )}
#line 2340 "ebnf.tab.c"
    break;

  case 100: /* $@7: %empty  */
#line 533 "ebnf.y"
                {NEDC( do_submodules (); )
                 GNED( SUBMODS_KEY = np->create("submods",MODULE_KEY);
                       setComments(SUBMODS_KEY,(yylsp[-1]),(yylsp[0])); )}
#line 2348 "ebnf.tab.c"
    break;

  case 106: /* $@8: %empty  */
#line 551 "ebnf.y"
                {NEDC( do_submodule1 (yyvsp[-3], NULL, yyvsp[-1], NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, (yylsp[-3]), NULLPOS, (yylsp[-1]), NULLPOS);
                       setComments(SUBMOD_KEY,(yylsp[-3]),(yylsp[0]));  )}
#line 2356 "ebnf.tab.c"
    break;

  case 107: /* $@9: %empty  */
#line 555 "ebnf.y"
                {NEDC( do_submodule2 (yyvsp[-5], NULL, yyvsp[-3], NULL); )}
#line 2362 "ebnf.tab.c"
    break;

  case 109: /* $@10: %empty  */
#line 558 "ebnf.y"
                {NEDC( do_submodule1 (yyvsp[-4], yyvsp[-1], yyvsp[-2], NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, (yylsp[-4]), (yylsp[-1]), (yylsp[-2]), NULLPOS);
                       setComments(SUBMOD_KEY,(yylsp[-4]),(yylsp[0]));  )}
#line 2370 "ebnf.tab.c"
    break;

  case 110: /* $@11: %empty  */
#line 562 "ebnf.y"
                {NEDC( do_submodule2 (yyvsp[-6], yyvsp[-3], yyvsp[-4], NULL); )}
#line 2376 "ebnf.tab.c"
    break;

  case 112: /* $@12: %empty  */
#line 565 "ebnf.y"
                {NEDC( do_submodule1 (yyvsp[-5], NULL, yyvsp[-3], yyvsp[-1]); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, (yylsp[-5]), NULLPOS, (yylsp[-3]), (yylsp[-1]));
                       setComments(SUBMOD_KEY,(yylsp[-5]),(yylsp[0]));  )}
#line 2384 "ebnf.tab.c"
    break;

  case 113: /* $@13: %empty  */
#line 569 "ebnf.y"
                {NEDC( do_submodule2 (yyvsp[-7], NULL, yyvsp[-5], yyvsp[-3]); )}
#line 2390 "ebnf.tab.c"
    break;

  case 115: /* $@14: %empty  */
#line 572 "ebnf.y"
                {NEDC( do_submodule1 (yyvsp[-6], yyvsp[-3], yyvsp[-4], yyvsp[-1]); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, (yylsp[-6]), (yylsp[-3]), (yylsp[-4]), (yylsp[-1]));
                       setComments(SUBMOD_KEY,(yylsp[-6]),(yylsp[0]));  )}
#line 2398 "ebnf.tab.c"
    break;

  case 116: /* $@15: %empty  */
#line 576 "ebnf.y"
                {NEDC( do_submodule2 (yyvsp[-8], yyvsp[-5], yyvsp[-6], yyvsp[-3]); )}
#line 2404 "ebnf.tab.c"
    break;

  case 118: /* $@16: %empty  */
#line 582 "ebnf.y"
                {NEDC( do_readallparameters(); )}
#line 2410 "ebnf.tab.c"
    break;

  case 119: /* submodule_body: opt_substparamblocks $@16 opt_gatesizeblocks opt_submod_displayblock  */
#line 585 "ebnf.y"
                {NEDC( end_submodule (); )}
#line 2416 "ebnf.tab.c"
    break;

  case 121: /* opt_on_blocks: %empty  */
#line 591 "ebnf.y"
            {NEDC( do_empty_onlist(); )}
#line 2422 "ebnf.tab.c"
    break;

  case 124: /* $@17: %empty  */
#line 601 "ebnf.y"
                {NEDC( do_onlist(); )
                 GNED( SUBSTMACHINES_KEY = np->create("substmachines",in_network?NETWORK_KEY:SUBMOD_KEY);
                       setComments(SUBSTMACHINES_KEY,(yylsp[-1]),(yylsp[0])); )}
#line 2430 "ebnf.tab.c"
    break;

  case 126: /* $@18: %empty  */
#line 606 "ebnf.y"
                {NEDC( open_if(yyvsp[-1]); do_onlist(); )
                 GNED( SUBSTMACHINES_KEY = np->create("substmachines",in_network?NETWORK_KEY:SUBMOD_KEY);
                       np->set(SUBSTMACHINES_KEY,"condition",(yylsp[-1]));
                       setComments(SUBSTMACHINES_KEY,(yylsp[-3]),(yylsp[0])); )}
#line 2439 "ebnf.tab.c"
    break;

  case 127: /* on_block: ON IF expression ':' $@18 opt_on_list  */
#line 611 "ebnf.y"
                {NEDC( close_if(); )}
#line 2445 "ebnf.tab.c"
    break;

  case 129: /* opt_on_list: %empty  */
#line 617 "ebnf.y"
            {NEDC( do_empty_onlist(); )}
#line 2451 "ebnf.tab.c"
    break;

  case 132: /* on_mach: NAME  */
#line 627 "ebnf.y"
                {NEDC( do_on_mach(yyvsp[0]); )
                 GNED( SUBSTMACHINE_KEY = addSubstmachine(SUBSTMACHINES_KEY,(yylsp[0]));
                       setComments(SUBSTMACHINE_KEY,(yylsp[0])); )}
#line 2459 "ebnf.tab.c"
    break;

  case 137: /* $@19: %empty  */
#line 644 "ebnf.y"
                {NEDC( do_substparams (); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       setComments(SUBSTPARAMS_KEY,(yylsp[-1]),(yylsp[0])); )}
#line 2467 "ebnf.tab.c"
    break;

  case 139: /* $@20: %empty  */
#line 649 "ebnf.y"
                {NEDC( open_if(yyvsp[-1]); do_substparams(); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       np->set(SUBSTPARAMS_KEY,"condition",(yylsp[-1]));
                       setComments(SUBSTPARAMS_KEY,(yylsp[-3]),(yylsp[0])); )}
#line 2476 "ebnf.tab.c"
    break;

  case 140: /* substparamblock: PARAMETERS IF expression ':' $@20 opt_substparameters  */
#line 654 "ebnf.y"
                {NEDC( close_if(); )}
#line 2482 "ebnf.tab.c"
    break;

  case 145: /* substparameter: NAME '=' inputvalue_or_expression  */
#line 669 "ebnf.y"
                {NEDC( do_substparameter (yyvsp[-2], yyvsp[0]);)
                 GNED( SUBSTPARAM_KEY = addSubstparam(SUBSTPARAMS_KEY,(yylsp[-2]),(yylsp[0]));
                       setComments(SUBSTPARAM_KEY,(yylsp[-2]),(yylsp[0])); )}
#line 2490 "ebnf.tab.c"
    break;

  case 148: /* $@21: %empty  */
#line 681 "ebnf.y"
                {NEDC( do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       setComments(GATESIZES_KEY,(yylsp[-1]),(yylsp[0])); )}
#line 2498 "ebnf.tab.c"
    break;

  case 150: /* $@22: %empty  */
#line 686 "ebnf.y"
                {NEDC( open_if(yyvsp[-1]); do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       np->set(GATESIZES_KEY,"condition",(yylsp[-1]));
                       setComments(GATESIZES_KEY,(yylsp[-3]),(yylsp[0])); )}
#line 2507 "ebnf.tab.c"
    break;

  case 151: /* gatesizeblock: GATESIZES IF expression ':' $@22 opt_gatesizes  */
#line 691 "ebnf.y"
                {NEDC( close_if(); )}
#line 2513 "ebnf.tab.c"
    break;

  case 156: /* gatesize: NAME vector  */
#line 706 "ebnf.y"
                {NEDC( do_gatesize (yyvsp[-1], yyvsp[0]); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,(yylsp[-1]),(yylsp[0]));
                       setComments(GATESIZE_KEY,(yylsp[-1]),(yylsp[0])); )}
#line 2521 "ebnf.tab.c"
    break;

  case 157: /* gatesize: NAME  */
#line 710 "ebnf.y"
                {NEDC( do_gatesize (yyvsp[0], NULL); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,(yylsp[0]),NULLPOS);
                       setComments(GATESIZE_KEY,(yylsp[0])); )}
#line 2529 "ebnf.tab.c"
    break;

  case 158: /* opt_submod_displayblock: DISPLAY ':' STRING ';'  */
#line 717 "ebnf.y"
                {NEDC( do_displaystr_submod (yyvsp[-1]); )
                 GNED( setDisplayString(SUBMOD_KEY,(yylsp[-1])); )}
#line 2536 "ebnf.tab.c"
    break;

  case 160: /* $@23: %empty  */
#line 724 "ebnf.y"
                {NEDC( set_checkconns(0); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","1");
                       setComments(CONNS_KEY,(yylsp[-2]),(yylsp[0])); )}
#line 2545 "ebnf.tab.c"
    break;

  case 162: /* $@24: %empty  */
#line 730 "ebnf.y"
                {NEDC( set_checkconns(1); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","0");
                       setComments(CONNS_KEY,(yylsp[-1]),(yylsp[0])); )}
#line 2554 "ebnf.tab.c"
    break;

  case 170: /* $@25: %empty  */
#line 754 "ebnf.y"
                {NEDC( do_for (); )
                 GNED( FORLOOP_KEY = np->create("forloop",CONNS_KEY);
                       in_loop=1; setComments(FORLOOP_KEY,(yylsp[0])); )}
#line 2562 "ebnf.tab.c"
    break;

  case 171: /* loopconnection: FOR $@25 loopvarlist DO notloopconnections ENDFOR  */
#line 758 "ebnf.y"
                {NEDC( end_for (); )
                 GNED( in_loop=0; setTrailingComment(FORLOOP_KEY,(yylsp[0])); )}
#line 2569 "ebnf.tab.c"
    break;

  case 174: /* loopvar: NAME '=' expression TO expression  */
#line 769 "ebnf.y"
                {NEDC( do_index (yyvsp[-4], yyvsp[-2], yyvsp[0]); )
                 GNED( LOOPVAR_KEY=addLoopVar(FORLOOP_KEY,(yylsp[-4]),(yylsp[-2]),(yylsp[0]));
                       setComments(LOOPVAR_KEY,(yylsp[-4]),(yylsp[0])); )}
#line 2577 "ebnf.tab.c"
    break;

  case 175: /* opt_conn_condition: IF expression  */
#line 776 "ebnf.y"
                {NEDC( do_condition(yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"condition",(yylsp[0])); )}
#line 2584 "ebnf.tab.c"
    break;

  case 176: /* opt_conn_condition: %empty  */
#line 779 "ebnf.y"
                {NEDC( do_condition(NULL); )}
#line 2590 "ebnf.tab.c"
    break;

  case 177: /* opt_conn_displaystr: DISPLAY STRING  */
#line 784 "ebnf.y"
                {NEDC( yyval = yyvsp[0]; )
                 GNED( setDisplayString(CONN_KEY,(yylsp[0])); )}
#line 2597 "ebnf.tab.c"
    break;

  case 178: /* opt_conn_displaystr: %empty  */
#line 787 "ebnf.y"
                {NEDC( yyval = NULL; )}
#line 2603 "ebnf.tab.c"
    break;

  case 181: /* notloopconnection: gate_spec_L RIGHT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr  */
#line 797 "ebnf.y"
                {NEDC( end_connection (NULL, 'R', yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,(yylsp[-4]),(yylsp[0])); )}
#line 2610 "ebnf.tab.c"
    break;

  case 182: /* notloopconnection: gate_spec_L RIGHT_ARROW channeldescr RIGHT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr  */
#line 800 "ebnf.y"
                {NEDC( end_connection (yyvsp[-4], 'R', yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,(yylsp[-6]),(yylsp[0])); )}
#line 2617 "ebnf.tab.c"
    break;

  case 183: /* notloopconnection: gate_spec_L LEFT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr  */
#line 803 "ebnf.y"
                {NEDC( end_connection (NULL, 'L', yyvsp[0]); )
                 GNED( swapConnection(CONN_KEY);
                       np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,(yylsp[-4]),(yylsp[0])); )}
#line 2625 "ebnf.tab.c"
    break;

  case 184: /* notloopconnection: gate_spec_L LEFT_ARROW channeldescr LEFT_ARROW gate_spec_R opt_conn_condition opt_conn_displaystr  */
#line 807 "ebnf.y"
                {NEDC( end_connection (yyvsp[-4], 'L', yyvsp[0]); )
                 GNED( swapConnection(CONN_KEY);
                 np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,(yylsp[-6]),(yylsp[0])); )}
#line 2633 "ebnf.tab.c"
    break;

  case 187: /* mod_L: NAME vector  */
#line 819 "ebnf.y"
                {NEDC( do_mod_L (yyvsp[-1], yyvsp[0]); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey((yylsp[-1])));
                       np->set(CONN_KEY, "src-mod-index", trimBrackets((yylsp[0]))); )}
#line 2642 "ebnf.tab.c"
    break;

  case 188: /* mod_L: NAME  */
#line 824 "ebnf.y"
                {NEDC( do_mod_L (yyvsp[0], NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey((yylsp[0]))); )}
#line 2650 "ebnf.tab.c"
    break;

  case 189: /* gate_L: NAME vector  */
#line 831 "ebnf.y"
                {NEDC( do_gate_L (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "srcgate", (yylsp[-1]));
                       np->set(CONN_KEY, "src-gate-index", trimBrackets((yylsp[0]))); )}
#line 2658 "ebnf.tab.c"
    break;

  case 190: /* gate_L: NAME  */
#line 835 "ebnf.y"
                {NEDC( do_gate_L (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "srcgate", (yylsp[0])); )}
#line 2665 "ebnf.tab.c"
    break;

  case 191: /* gate_L: NAME PLUSPLUS  */
#line 838 "ebnf.y"
                {NEDC( do_gate_L (yyvsp[-1], NULL); )
                 GNED( np->set(CONN_KEY, "srcgate", (yylsp[-1]));
                       np->set(CONN_KEY, "src-gate-plusplus", "1"); )}
#line 2673 "ebnf.tab.c"
    break;

  case 192: /* parentgate_L: NAME vector  */
#line 845 "ebnf.y"
                {NEDC( do_mod_L (NULL, NULL); do_gate_L (yyvsp[-1], yyvsp[0]); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", (yylsp[-1]));
                       np->set(CONN_KEY, "src-gate-index", trimBrackets((yylsp[0]))); )}
#line 2683 "ebnf.tab.c"
    break;

  case 193: /* parentgate_L: NAME  */
#line 851 "ebnf.y"
                {NEDC( do_mod_L (NULL, NULL); do_gate_L (yyvsp[0], NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", (yylsp[0])); )}
#line 2692 "ebnf.tab.c"
    break;

  case 194: /* parentgate_L: NAME PLUSPLUS  */
#line 856 "ebnf.y"
                {NEDC( do_mod_L (NULL, NULL); do_gate_L (yyvsp[-1], NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", (yylsp[-1]));
                       np->set(CONN_KEY, "src-gate-plusplus", "1");  )}
#line 2702 "ebnf.tab.c"
    break;

  case 197: /* mod_R: NAME vector  */
#line 870 "ebnf.y"
                {NEDC( do_mod_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey((yylsp[-1])));
                       np->set(CONN_KEY, "dest-mod-index", trimBrackets((yylsp[0]))); )}
#line 2710 "ebnf.tab.c"
    break;

  case 198: /* mod_R: NAME  */
#line 874 "ebnf.y"
                {NEDC( do_mod_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey((yylsp[0]))); )}
#line 2717 "ebnf.tab.c"
    break;

  case 199: /* gate_R: NAME vector  */
#line 880 "ebnf.y"
                {NEDC( do_gate_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "destgate", (yylsp[-1]));
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets((yylsp[0]))); )}
#line 2725 "ebnf.tab.c"
    break;

  case 200: /* gate_R: NAME  */
#line 884 "ebnf.y"
                {NEDC( do_gate_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "destgate", (yylsp[0])); )}
#line 2732 "ebnf.tab.c"
    break;

  case 201: /* gate_R: NAME PLUSPLUS  */
#line 887 "ebnf.y"
                {NEDC( do_gate_R (yyvsp[-1], NULL); )
                 GNED( np->set(CONN_KEY, "destgate", (yylsp[-1]));
                       np->set(CONN_KEY, "dest-gate-plusplus", "1"); )}
#line 2740 "ebnf.tab.c"
    break;

  case 202: /* parentgate_R: NAME vector  */
#line 894 "ebnf.y"
                {NEDC( do_mod_R (NULL, NULL); do_gate_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", (yylsp[-1]));
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets((yylsp[0]))); )}
#line 2749 "ebnf.tab.c"
    break;

  case 203: /* parentgate_R: NAME  */
#line 899 "ebnf.y"
                {NEDC( do_mod_R (NULL, NULL); do_gate_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", (yylsp[0])); )}
#line 2757 "ebnf.tab.c"
    break;

  case 204: /* parentgate_R: NAME PLUSPLUS  */
#line 903 "ebnf.y"
                {NEDC( do_mod_R (NULL, NULL); do_gate_R (yyvsp[-1], NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", (yylsp[-1]));
                       np->set(CONN_KEY, "dest-gate-plusplus", "1"); )}
#line 2766 "ebnf.tab.c"
    break;

  case 205: /* channeldescr: %empty  */
#line 912 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, NULL, NULL, NULL); )}
#line 2772 "ebnf.tab.c"
    break;

  case 206: /* channeldescr: cdname  */
#line 914 "ebnf.y"
                {NEDC( yyval = do_channeldescr(yyvsp[0], NULL,NULL,NULL); )}
#line 2778 "ebnf.tab.c"
    break;

  case 207: /* channeldescr: cddelay  */
#line 916 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], NULL, NULL); )}
#line 2784 "ebnf.tab.c"
    break;

  case 208: /* channeldescr: cderror  */
#line 918 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[0], NULL); )}
#line 2790 "ebnf.tab.c"
    break;

  case 209: /* channeldescr: cddatarate  */
#line 920 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, NULL, NULL, yyvsp[0]); )}
#line 2796 "ebnf.tab.c"
    break;

  case 210: /* channeldescr: cddelay cderror  */
#line 923 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[0], NULL); )}
#line 2802 "ebnf.tab.c"
    break;

  case 211: /* channeldescr: cddelay cddatarate  */
#line 925 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], NULL, yyvsp[0]); )}
#line 2808 "ebnf.tab.c"
    break;

  case 212: /* channeldescr: cderror cddatarate  */
#line 927 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[-1], yyvsp[0]); )}
#line 2814 "ebnf.tab.c"
    break;

  case 213: /* channeldescr: cderror cddelay  */
#line 929 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-1], NULL); )}
#line 2820 "ebnf.tab.c"
    break;

  case 214: /* channeldescr: cddatarate cddelay  */
#line 931 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], NULL, yyvsp[-1]); )}
#line 2826 "ebnf.tab.c"
    break;

  case 215: /* channeldescr: cddatarate cderror  */
#line 933 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[0], yyvsp[-1]); )}
#line 2832 "ebnf.tab.c"
    break;

  case 216: /* channeldescr: cddelay cderror cddatarate  */
#line 936 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, yyvsp[-2], yyvsp[-1], yyvsp[0]); )}
#line 2838 "ebnf.tab.c"
    break;

  case 217: /* channeldescr: cddelay cddatarate cderror  */
#line 938 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, yyvsp[-2], yyvsp[0], yyvsp[-1]); )}
#line 2844 "ebnf.tab.c"
    break;

  case 218: /* channeldescr: cderror cddelay cddatarate  */
#line 940 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[-2], yyvsp[0]); )}
#line 2850 "ebnf.tab.c"
    break;

  case 219: /* channeldescr: cderror cddatarate cddelay  */
#line 942 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-2], yyvsp[-1]); )}
#line 2856 "ebnf.tab.c"
    break;

  case 220: /* channeldescr: cddatarate cddelay cderror  */
#line 944 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[0], yyvsp[-2]); )}
#line 2862 "ebnf.tab.c"
    break;

  case 221: /* channeldescr: cddatarate cderror cddelay  */
#line 946 "ebnf.y"
                {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-1], yyvsp[-2]); )}
#line 2868 "ebnf.tab.c"
    break;

  case 222: /* cdname: NAME  */
#line 951 "ebnf.y"
                {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"channel",(yylsp[0])); )}
#line 2875 "ebnf.tab.c"
    break;

  case 223: /* cddelay: DELAY expression  */
#line 957 "ebnf.y"
                {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"delay",(yylsp[0])); )}
#line 2882 "ebnf.tab.c"
    break;

  case 224: /* cderror: ERROR expression  */
#line 963 "ebnf.y"
                {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"error",(yylsp[0])); )}
#line 2889 "ebnf.tab.c"
    break;

  case 225: /* cddatarate: DATARATE expression  */
#line 969 "ebnf.y"
                {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"datarate",(yylsp[0])); )}
#line 2896 "ebnf.tab.c"
    break;

  case 226: /* $@26: %empty  */
#line 975 "ebnf.y"
                {NEDC( do_system (yyvsp[-3]); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,(yylsp[-3]),(yylsp[-1]),NULLPOS);
                       setComments(NETWORK_KEY,(yylsp[-4]),(yylsp[0])); in_network=1;)}
#line 2904 "ebnf.tab.c"
    break;

  case 227: /* $@27: %empty  */
#line 979 "ebnf.y"
                {NEDC( do_systemmodule (yyvsp[-5], yyvsp[-3], NULL); )}
#line 2910 "ebnf.tab.c"
    break;

  case 228: /* $@28: %empty  */
#line 981 "ebnf.y"
                {NEDC( do_readallparameters(); )}
#line 2916 "ebnf.tab.c"
    break;

  case 229: /* network: NETWORK NAME ':' NAME opt_semicolon $@26 opt_on_blocks $@27 opt_substparamblocks $@28 endnetwork  */
#line 983 "ebnf.y"
                {NEDC( end_system (); )}
#line 2922 "ebnf.tab.c"
    break;

  case 230: /* $@29: %empty  */
#line 985 "ebnf.y"
                {NEDC( do_system (yyvsp[-5]); )}
#line 2928 "ebnf.tab.c"
    break;

  case 231: /* $@30: %empty  */
#line 987 "ebnf.y"
                {NEDC( do_systemmodule (yyvsp[-7], yyvsp[-5], yyvsp[-3]); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,(yylsp[-7]),(yylsp[-5]),(yylsp[-3]));
                       setComments(NETWORK_KEY,(yylsp[-8]),(yylsp[-2])); )}
#line 2936 "ebnf.tab.c"
    break;

  case 232: /* $@31: %empty  */
#line 991 "ebnf.y"
                {NEDC( do_readallparameters(); )}
#line 2942 "ebnf.tab.c"
    break;

  case 233: /* network: NETWORK NAME ':' NAME LIKE NAME opt_semicolon $@29 opt_on_blocks $@30 opt_substparamblocks $@31 endnetwork  */
#line 993 "ebnf.y"
                {NEDC( end_system (); )}
#line 2948 "ebnf.tab.c"
    break;

  case 234: /* endnetwork: ENDNETWORK opt_semicolon  */
#line 998 "ebnf.y"
                {GNED( setTrailingComment(NETWORK_KEY,(yylsp[-1])); in_network=0; )}
#line 2954 "ebnf.tab.c"
    break;

  case 235: /* vector: '[' expression ']'  */
#line 1003 "ebnf.y"
                {NEDC( yyval = yyvsp[-1]; )}
#line 2960 "ebnf.tab.c"
    break;

  case 238: /* inputvalue: INPUT '(' expression ',' expression ')'  */
#line 1013 "ebnf.y"
                {NEDC( yyval = do_inputvalue(yyvsp[-3], yyvsp[-1]); )}
#line 2966 "ebnf.tab.c"
    break;

  case 239: /* inputvalue: INPUT '(' expression ')'  */
#line 1015 "ebnf.y"
                {NEDC( yyval = do_inputvalue(yyvsp[-1], NULL); )}
#line 2972 "ebnf.tab.c"
    break;

  case 240: /* inputvalue: INPUT '(' ')'  */
#line 1017 "ebnf.y"
                {NEDC( yyval = do_inputvalue(NULL, NULL); )}
#line 2978 "ebnf.tab.c"
    break;

  case 241: /* inputvalue: INPUT  */
#line 1019 "ebnf.y"
                {NEDC( yyval = do_inputvalue(NULL, NULL); )}
#line 2984 "ebnf.tab.c"
    break;

  case 242: /* expression: expr  */
#line 1025 "ebnf.y"
                {NEDC( yyval = end_expr( yyvsp[0] ); )}
#line 2990 "ebnf.tab.c"
    break;

  case 243: /* expr: simple_expr  */
#line 1030 "ebnf.y"
                {NEDC( yyval = yyvsp[0]; )}
#line 2996 "ebnf.tab.c"
    break;

  case 244: /* expr: '(' expr ')'  */
#line 1032 "ebnf.y"
                {NEDC( yyval = yyvsp[-1]; )}
#line 3002 "ebnf.tab.c"
    break;

  case 245: /* expr: expr PLUS expr  */
#line 1035 "ebnf.y"
                {NEDC( yyval = do_op( 2, '+', yyvsp[-2],yyvsp[0],NULL); )}
#line 3008 "ebnf.tab.c"
    break;

  case 246: /* expr: expr MIN expr  */
#line 1037 "ebnf.y"
                {NEDC( yyval = do_op( 2, '-', yyvsp[-2],yyvsp[0],NULL); )}
#line 3014 "ebnf.tab.c"
    break;

  case 247: /* expr: expr MUL expr  */
#line 1039 "ebnf.y"
                {NEDC( yyval = do_op( 2, '*', yyvsp[-2],yyvsp[0],NULL); )}
#line 3020 "ebnf.tab.c"
    break;

  case 248: /* expr: expr DIV expr  */
#line 1041 "ebnf.y"
                {NEDC( yyval = do_op( 2, '/', yyvsp[-2],yyvsp[0],NULL); )}
#line 3026 "ebnf.tab.c"
    break;

  case 249: /* expr: expr MOD expr  */
#line 1043 "ebnf.y"
                {NEDC( yyval = do_op( 2, '%', yyvsp[-2],yyvsp[0],NULL); )}
#line 3032 "ebnf.tab.c"
    break;

  case 250: /* expr: expr EXP expr  */
#line 1045 "ebnf.y"
                {NEDC( yyval = do_op( 2, '^', yyvsp[-2],yyvsp[0],NULL); )}
#line 3038 "ebnf.tab.c"
    break;

  case 251: /* expr: MIN expr  */
#line 1049 "ebnf.y"
                {NEDC( yyval = do_addminus(yyvsp[0]); )}
#line 3044 "ebnf.tab.c"
    break;

  case 252: /* expr: expr EQ expr  */
#line 1052 "ebnf.y"
                {NEDC( yyval = do_op( 2, '=', yyvsp[-2],yyvsp[0],NULL); )}
#line 3050 "ebnf.tab.c"
    break;

  case 253: /* expr: expr NE expr  */
#line 1054 "ebnf.y"
                {NEDC( yyval = do_op( 2, '!', yyvsp[-2],yyvsp[0],NULL); )}
#line 3056 "ebnf.tab.c"
    break;

  case 254: /* expr: expr GT expr  */
#line 1056 "ebnf.y"
                {NEDC( yyval = do_op( 2, '>', yyvsp[-2],yyvsp[0],NULL); )}
#line 3062 "ebnf.tab.c"
    break;

  case 255: /* expr: expr GE expr  */
#line 1058 "ebnf.y"
                {NEDC( yyval = do_op( 2, '}', yyvsp[-2],yyvsp[0],NULL); )}
#line 3068 "ebnf.tab.c"
    break;

  case 256: /* expr: expr LS expr  */
#line 1060 "ebnf.y"
                {NEDC( yyval = do_op( 2, '<', yyvsp[-2],yyvsp[0],NULL); )}
#line 3074 "ebnf.tab.c"
    break;

  case 257: /* expr: expr LE expr  */
#line 1062 "ebnf.y"
                {NEDC( yyval = do_op( 2, '{', yyvsp[-2],yyvsp[0],NULL); )}
#line 3080 "ebnf.tab.c"
    break;

  case 258: /* expr: expr AND expr  */
#line 1065 "ebnf.y"
                {NEDC( yyval = do_func(2,jar_strdup("bool_and"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
#line 3086 "ebnf.tab.c"
    break;

  case 259: /* expr: expr OR expr  */
#line 1067 "ebnf.y"
                {NEDC( yyval = do_func(2,jar_strdup("bool_or"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
#line 3092 "ebnf.tab.c"
    break;

  case 260: /* expr: expr XOR expr  */
#line 1069 "ebnf.y"
                {NEDC( yyval = do_func(2,jar_strdup("bool_xor"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
#line 3098 "ebnf.tab.c"
    break;

  case 261: /* expr: NOT expr  */
#line 1072 "ebnf.y"
                {NEDC( yyval = do_func(1,jar_strdup("bool_not"), yyvsp[0],NULL,NULL,NULL); )}
#line 3104 "ebnf.tab.c"
    break;

  case 262: /* expr: expr BIN_AND expr  */
#line 1075 "ebnf.y"
                {NEDC( yyval = do_func(2,jar_strdup("bin_and"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
#line 3110 "ebnf.tab.c"
    break;

  case 263: /* expr: expr BIN_OR expr  */
#line 1077 "ebnf.y"
                {NEDC( yyval = do_func(2,jar_strdup("bin_or"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
#line 3116 "ebnf.tab.c"
    break;

  case 264: /* expr: expr BIN_XOR expr  */
#line 1079 "ebnf.y"
                {NEDC( yyval = do_func(2,jar_strdup("bin_xor"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
#line 3122 "ebnf.tab.c"
    break;

  case 265: /* expr: BIN_COMPL expr  */
#line 1082 "ebnf.y"
                {NEDC( yyval = do_func(1,jar_strdup("bin_compl"), yyvsp[0],NULL,NULL,NULL); )}
#line 3128 "ebnf.tab.c"
    break;

  case 266: /* expr: expr SHIFT_LEFT expr  */
#line 1084 "ebnf.y"
                {NEDC( yyval = do_func(2,jar_strdup("shift_left"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
#line 3134 "ebnf.tab.c"
    break;

  case 267: /* expr: expr SHIFT_RIGHT expr  */
#line 1086 "ebnf.y"
                {NEDC( yyval = do_func(2,jar_strdup("shift_right"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
#line 3140 "ebnf.tab.c"
    break;

  case 268: /* expr: expr '?' expr ':' expr  */
#line 1088 "ebnf.y"
                {NEDC( yyval = do_op( 3, '?', yyvsp[-4],yyvsp[-2],yyvsp[0]); )}
#line 3146 "ebnf.tab.c"
    break;

  case 269: /* expr: NAME '(' ')'  */
#line 1090 "ebnf.y"
                {NEDC( yyval = do_func(0,yyvsp[-2], NULL,NULL,NULL,NULL); )}
#line 3152 "ebnf.tab.c"
    break;

  case 270: /* expr: NAME '(' expr ')'  */
#line 1092 "ebnf.y"
                {NEDC( yyval = do_func(1,yyvsp[-3], yyvsp[-1],NULL,NULL,NULL); )}
#line 3158 "ebnf.tab.c"
    break;

  case 271: /* expr: NAME '(' expr ',' expr ')'  */
#line 1094 "ebnf.y"
                {NEDC( yyval = do_func(2,yyvsp[-5], yyvsp[-3],yyvsp[-1],NULL,NULL); )}
#line 3164 "ebnf.tab.c"
    break;

  case 272: /* expr: NAME '(' expr ',' expr ',' expr ')'  */
#line 1096 "ebnf.y"
                {NEDC( yyval = do_func(3,yyvsp[-7], yyvsp[-5],yyvsp[-3],yyvsp[-1],NULL); )}
#line 3170 "ebnf.tab.c"
    break;

  case 273: /* expr: NAME '(' expr ',' expr ',' expr ',' expr ')'  */
#line 1098 "ebnf.y"
                {NEDC( yyval = do_func(3,yyvsp[-9], yyvsp[-7],yyvsp[-5],yyvsp[-3],yyvsp[-1]); )}
#line 3176 "ebnf.tab.c"
    break;

  case 279: /* parameter_expr: NAME  */
#line 1111 "ebnf.y"
                {NEDC( yyval = do_parname (0,0,yyvsp[0],0,1); )}
#line 3182 "ebnf.tab.c"
    break;

  case 280: /* parameter_expr: REF NAME  */
#line 1113 "ebnf.y"
                {NEDC( yyval = do_parname (0,0,yyvsp[0],0,0); )}
#line 3188 "ebnf.tab.c"
    break;

  case 281: /* parameter_expr: ANCESTOR NAME  */
#line 1115 "ebnf.y"
                {NEDC( yyval = do_parname (0,0,yyvsp[0],1,1); )}
#line 3194 "ebnf.tab.c"
    break;

  case 282: /* parameter_expr: REF ANCESTOR NAME  */
#line 1117 "ebnf.y"
                {NEDC( yyval = do_parname (0,0,yyvsp[0],1,0); )}
#line 3200 "ebnf.tab.c"
    break;

  case 283: /* parameter_expr: ANCESTOR REF NAME  */
#line 1119 "ebnf.y"
                {NEDC( yyval = do_parname (0,0,yyvsp[0],1,0); )}
#line 3206 "ebnf.tab.c"
    break;

  case 284: /* parameter_expr: NAME '.' NAME  */
#line 1122 "ebnf.y"
                {NEDC( yyval = do_parname (yyvsp[-2],0,yyvsp[0],0,1); )}
#line 3212 "ebnf.tab.c"
    break;

  case 285: /* parameter_expr: REF NAME '.' NAME  */
#line 1124 "ebnf.y"
                {NEDC( yyval = do_parname (yyvsp[-2],0,yyvsp[0],0,0); )}
#line 3218 "ebnf.tab.c"
    break;

  case 286: /* parameter_expr: NAME vector '.' NAME  */
#line 1127 "ebnf.y"
                {NEDC( yyval = do_parname (yyvsp[-3],yyvsp[-2],yyvsp[0],0,1); )}
#line 3224 "ebnf.tab.c"
    break;

  case 287: /* parameter_expr: REF NAME vector '.' NAME  */
#line 1129 "ebnf.y"
                {NEDC( yyval = do_parname (yyvsp[-3],yyvsp[-2],yyvsp[0],0,0); )}
#line 3230 "ebnf.tab.c"
    break;

  case 288: /* string_expr: STRING  */
#line 1134 "ebnf.y"
                {NEDC( yyval = make_literal_expr(TYPE_STRING,yyvsp[0]); )}
#line 3236 "ebnf.tab.c"
    break;

  case 289: /* boolconst_expr: _TRUE  */
#line 1139 "ebnf.y"
                {NEDC( yyval = make_literal_expr(TYPE_BOOL,jar_strdup("true")); )}
#line 3242 "ebnf.tab.c"
    break;

  case 290: /* boolconst_expr: _FALSE  */
#line 1141 "ebnf.y"
                {NEDC( yyval = make_literal_expr(TYPE_BOOL,jar_strdup("false")); )}
#line 3248 "ebnf.tab.c"
    break;

  case 291: /* numconst_expr: numconst  */
#line 1146 "ebnf.y"
                {NEDC( yyval = make_literal_expr(TYPE_NUMERIC,yyvsp[0]); )}
#line 3254 "ebnf.tab.c"
    break;

  case 292: /* special_expr: SUBMODINDEX  */
#line 1151 "ebnf.y"
                {NEDC( yyval = make_literal_expr(TYPE_NUMERIC,jar_strdup("(long)sub_i")); )}
#line 3260 "ebnf.tab.c"
    break;

  case 293: /* special_expr: SIZEOF '(' NAME ')'  */
#line 1153 "ebnf.y"
                {NEDC( yyval = do_sizeof (yyvsp[-1]); )}
#line 3266 "ebnf.tab.c"
    break;

  case 297: /* timeconstant: INTCONSTANT NAME timeconstant  */
#line 1164 "ebnf.y"
                {NEDC( yyval = do_timeconstant(yyvsp[-2],yyvsp[-1],yyvsp[0]); )}
#line 3272 "ebnf.tab.c"
    break;

  case 298: /* timeconstant: REALCONSTANT NAME timeconstant  */
#line 1166 "ebnf.y"
                {NEDC( yyval = do_timeconstant(yyvsp[-2],yyvsp[-1],yyvsp[0]); )}
#line 3278 "ebnf.tab.c"
    break;

  case 299: /* timeconstant: INTCONSTANT NAME  */
#line 1168 "ebnf.y"
                {NEDC( yyval = do_timeconstant(yyvsp[-1],yyvsp[0],NULL); )}
#line 3284 "ebnf.tab.c"
    break;

  case 300: /* timeconstant: REALCONSTANT NAME  */
#line 1170 "ebnf.y"
                {NEDC( yyval = do_timeconstant(yyvsp[-1],yyvsp[0],NULL); )}
#line 3290 "ebnf.tab.c"
    break;


#line 3294 "ebnf.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1177 "ebnf.y"


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

void yyerror (const char *s)
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

void yyerror (const char *s)
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

int addChanAttr(int channel_key, const char *attrname, YYLTYPE valuepos)
{
    int chanattr_key = np->create("chanattr", channel_key);
    np->set(chanattr_key,"name", attrname);
    np->set(chanattr_key,"value", valuepos);
    return chanattr_key;
}

int addParameter(int params_key, YYLTYPE namepos, int type)
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

int addConnAttr(int conn_key, const char *attrname, YYLTYPE valuepos)
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



