/* A Bison parser, made from ebnf.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON	1

/* Pure parsers.  */
#define YYPURE	0

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INCLUDE = 258,
     SIMPLE = 259,
     CHANNEL = 260,
     DELAY = 261,
     ERROR = 262,
     DATARATE = 263,
     MODULE = 264,
     PARAMETERS = 265,
     GATES = 266,
     GATESIZES = 267,
     SUBMODULES = 268,
     CONNECTIONS = 269,
     DISPLAY = 270,
     IN = 271,
     OUT = 272,
     NOCHECK = 273,
     LEFT_ARROW = 274,
     RIGHT_ARROW = 275,
     FOR = 276,
     TO = 277,
     DO = 278,
     IF = 279,
     LIKE = 280,
     NETWORK = 281,
     ENDSIMPLE = 282,
     ENDMODULE = 283,
     ENDCHANNEL = 284,
     ENDNETWORK = 285,
     ENDFOR = 286,
     MACHINES = 287,
     ON = 288,
     IO_INTERFACES = 289,
     IFPAIR = 290,
     INTCONSTANT = 291,
     REALCONSTANT = 292,
     NAME = 293,
     STRING = 294,
     _TRUE = 295,
     _FALSE = 296,
     INPUT = 297,
     REF = 298,
     ANCESTOR = 299,
     NED_CONST = 300,
     NUMERICTYPE = 301,
     STRINGTYPE = 302,
     BOOLTYPE = 303,
     ANYTYPE = 304,
     PLUS = 305,
     MIN = 306,
     MUL = 307,
     DIV = 308,
     MOD = 309,
     EXP = 310,
     SIZEOF = 311,
     SUBMODINDEX = 312,
     EQ = 313,
     NE = 314,
     GT = 315,
     GE = 316,
     LS = 317,
     LE = 318,
     AND = 319,
     OR = 320,
     XOR = 321,
     NOT = 322,
     BIN_AND = 323,
     BIN_OR = 324,
     BIN_XOR = 325,
     BIN_COMPL = 326,
     SHIFT_LEFT = 327,
     SHIFT_RIGHT = 328,
     INVALID_CHAR = 329,
     UMIN = 330
   };
#endif
#define INCLUDE 258
#define SIMPLE 259
#define CHANNEL 260
#define DELAY 261
#define ERROR 262
#define DATARATE 263
#define MODULE 264
#define PARAMETERS 265
#define GATES 266
#define GATESIZES 267
#define SUBMODULES 268
#define CONNECTIONS 269
#define DISPLAY 270
#define IN 271
#define OUT 272
#define NOCHECK 273
#define LEFT_ARROW 274
#define RIGHT_ARROW 275
#define FOR 276
#define TO 277
#define DO 278
#define IF 279
#define LIKE 280
#define NETWORK 281
#define ENDSIMPLE 282
#define ENDMODULE 283
#define ENDCHANNEL 284
#define ENDNETWORK 285
#define ENDFOR 286
#define MACHINES 287
#define ON 288
#define IO_INTERFACES 289
#define IFPAIR 290
#define INTCONSTANT 291
#define REALCONSTANT 292
#define NAME 293
#define STRING 294
#define _TRUE 295
#define _FALSE 296
#define INPUT 297
#define REF 298
#define ANCESTOR 299
#define NED_CONST 300
#define NUMERICTYPE 301
#define STRINGTYPE 302
#define BOOLTYPE 303
#define ANYTYPE 304
#define PLUS 305
#define MIN 306
#define MUL 307
#define DIV 308
#define MOD 309
#define EXP 310
#define SIZEOF 311
#define SUBMODINDEX 312
#define EQ 313
#define NE 314
#define GT 315
#define GE 316
#define LS 317
#define LE 318
#define AND 319
#define OR 320
#define XOR 321
#define NOT 322
#define BIN_AND 323
#define BIN_OR 324
#define BIN_XOR 325
#define BIN_COMPL 326
#define SHIFT_LEFT 327
#define SHIFT_RIGHT 328
#define INVALID_CHAR 329
#define UMIN 330




/* Copy the first part of user declarations.  */
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
#include <malloc.h>         /* for alloca() */
#include "ebnf.h"           /* selects between NEDC and GNED */
#include "ebnfcfg.h"        /* selects between NEDC and GNED */

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <string.h>         /* YYVERBOSE needs it */
#ifdef CXX                  /* For TRU64 c++ compiler */
#include <malloc.h>
#include <stdlib.h>
#include <alloca.h>
#endif                      /* Required to compile ebnf.y derivatives */
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



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#ifndef YYSTYPE
typedef int yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} yyltype;
# define YYLTYPE yyltype
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* Copy the second part of user declarations.  */


/* Line 213 of /usr/share/bison/yacc.c.  */
#line 381 "ebnf.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];	\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  22
#define YYLAST   655

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  136
/* YYNRULES -- Number of rules. */
#define YYNRULES  300
/* YYNRULES -- Number of states. */
#define YYNSTATES  506

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   330

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      84,    85,     2,     2,    79,     2,    83,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    76,    78,
       2,    82,     2,    75,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    80,     2,    81,     2,     2,     2,     2,     2,     2,
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
      77
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    13,    15,    17,
      19,    21,    22,    27,    31,    33,    35,    38,    42,    46,
      50,    55,    60,    65,    70,    75,    80,    86,    92,    98,
     104,   110,   116,   119,   120,   125,   126,   130,   134,   138,
     142,   148,   152,   156,   159,   168,   172,   176,   179,   181,
     182,   184,   185,   187,   188,   190,   191,   193,   194,   196,
     197,   198,   203,   206,   207,   211,   213,   215,   220,   221,
     226,   229,   230,   234,   236,   238,   242,   245,   249,   254,
     259,   263,   267,   271,   272,   277,   279,   280,   285,   289,
     294,   298,   302,   304,   308,   310,   314,   316,   320,   322,
     323,   328,   330,   331,   334,   336,   337,   338,   347,   348,
     349,   359,   360,   361,   372,   373,   374,   386,   387,   392,
     394,   395,   398,   400,   401,   406,   407,   414,   417,   418,
     422,   424,   426,   428,   429,   432,   434,   435,   440,   441,
     448,   451,   452,   456,   458,   462,   465,   466,   467,   472,
     473,   480,   483,   484,   488,   490,   493,   495,   500,   501,
     502,   508,   509,   514,   517,   518,   522,   524,   526,   528,
     529,   536,   540,   542,   548,   551,   552,   555,   556,   560,
     563,   569,   577,   583,   591,   595,   597,   600,   602,   605,
     607,   610,   612,   616,   618,   621,   623,   626,   628,   631,
     633,   634,   636,   638,   640,   642,   645,   648,   651,   654,
     657,   660,   664,   668,   672,   676,   680,   684,   686,   689,
     692,   695,   696,   697,   698,   710,   711,   712,   713,   727,
     730,   734,   736,   738,   745,   750,   754,   756,   758,   760,
     764,   768,   772,   776,   780,   784,   788,   791,   795,   799,
     803,   807,   811,   815,   819,   823,   827,   830,   834,   838,
     842,   845,   849,   853,   859,   863,   868,   875,   884,   895,
     897,   899,   901,   903,   905,   907,   910,   913,   917,   921,
     925,   930,   935,   941,   943,   945,   947,   949,   951,   956,
     958,   960,   962,   966,   970,   973,   976,   978,   979,   981,
     983
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
      87,     0,    -1,    88,    -1,    -1,    88,    89,    -1,    89,
      -1,    90,    -1,    94,    -1,   102,    -1,   105,    -1,   199,
      -1,    -1,     3,    91,    92,    78,    -1,    92,    79,    93,
      -1,    93,    -1,    39,    -1,    95,    96,    -1,    95,   100,
      96,    -1,    95,    99,    96,    -1,    95,   101,    96,    -1,
      95,   100,    99,    96,    -1,    95,   100,   101,    96,    -1,
      95,    99,   101,    96,    -1,    95,    99,   100,    96,    -1,
      95,   101,   100,    96,    -1,    95,   101,    99,    96,    -1,
      95,   100,    99,   101,    96,    -1,    95,   100,   101,    99,
      96,    -1,    95,    99,   100,   101,    96,    -1,    95,    99,
     101,   100,    96,    -1,    95,   101,   100,    99,    96,    -1,
      95,   101,    99,   100,    96,    -1,     5,    38,    -1,    -1,
      29,    38,   220,    97,    -1,    -1,    29,   220,    98,    -1,
       7,   210,   220,    -1,     6,   210,   220,    -1,     8,   210,
     220,    -1,   103,   108,   110,   111,   104,    -1,     4,    38,
     220,    -1,    27,    38,   220,    -1,    27,   220,    -1,   106,
     108,   110,   111,   112,   113,   109,   107,    -1,     9,    38,
     220,    -1,    28,    38,   220,    -1,    28,   220,    -1,   114,
      -1,    -1,   119,    -1,    -1,   120,    -1,    -1,   125,    -1,
      -1,   133,    -1,    -1,   172,    -1,    -1,    -1,    32,    76,
     115,   116,    -1,   117,    78,    -1,    -1,   117,    79,   118,
      -1,   118,    -1,    38,    -1,    15,    76,    39,    78,    -1,
      -1,    10,    76,   121,   122,    -1,   123,    78,    -1,    -1,
     123,    79,   124,    -1,   124,    -1,    38,    -1,    38,    76,
      46,    -1,    45,    38,    -1,    38,    76,    45,    -1,    38,
      76,    45,    46,    -1,    38,    76,    46,    45,    -1,    38,
      76,    47,    -1,    38,    76,    48,    -1,    38,    76,    49,
      -1,    -1,    11,    76,   126,   127,    -1,   128,    -1,    -1,
     128,    16,   129,    78,    -1,    16,   129,    78,    -1,   128,
      17,   131,    78,    -1,    17,   131,    78,    -1,   129,    79,
     130,    -1,   130,    -1,    38,    80,    81,    -1,    38,    -1,
     131,    79,   132,    -1,   132,    -1,    38,    80,    81,    -1,
      38,    -1,    -1,    13,    76,   134,   135,    -1,   136,    -1,
      -1,   136,   137,    -1,   137,    -1,    -1,    -1,    38,    76,
      38,   220,   138,   148,   139,   146,    -1,    -1,    -1,    38,
      76,    38,   207,   220,   140,   148,   141,   146,    -1,    -1,
      -1,    38,    76,    38,    25,    38,   220,   142,   148,   143,
     146,    -1,    -1,    -1,    38,    76,    38,   207,    25,    38,
     220,   144,   148,   145,   146,    -1,    -1,   156,   147,   164,
     171,    -1,   149,    -1,    -1,   149,   150,    -1,   150,    -1,
      -1,    33,    76,   151,   153,    -1,    -1,    33,    24,   210,
      76,   152,   153,    -1,   154,    78,    -1,    -1,   154,    79,
     155,    -1,   155,    -1,    38,    -1,   157,    -1,    -1,   157,
     158,    -1,   158,    -1,    -1,    10,    76,   159,   161,    -1,
      -1,    10,    24,   210,    76,   160,   161,    -1,   162,    78,
      -1,    -1,   162,    79,   163,    -1,   163,    -1,    38,    82,
     208,    -1,   165,   164,    -1,    -1,    -1,    12,    76,   166,
     168,    -1,    -1,    12,    24,   210,    76,   167,   168,    -1,
     169,    78,    -1,    -1,   169,    79,   170,    -1,   170,    -1,
      38,   207,    -1,    38,    -1,    15,    76,    39,    78,    -1,
      -1,    -1,    14,    18,    76,   173,   175,    -1,    -1,    14,
      76,   174,   175,    -1,   176,    78,    -1,    -1,   176,   221,
     177,    -1,   177,    -1,   178,    -1,   185,    -1,    -1,    21,
     179,   180,    23,   184,    31,    -1,   180,    79,   181,    -1,
     181,    -1,    38,    82,   210,    22,   210,    -1,    24,   210,
      -1,    -1,    15,    39,    -1,    -1,   184,   185,   221,    -1,
     185,   221,    -1,   186,    20,   190,   182,   183,    -1,   186,
      20,   194,    20,   190,   182,   183,    -1,   186,    19,   190,
     182,   183,    -1,   186,    19,   194,    19,   190,   182,   183,
      -1,   187,    83,   188,    -1,   189,    -1,    38,   207,    -1,
      38,    -1,    38,   207,    -1,    38,    -1,    38,   207,    -1,
      38,    -1,   191,    83,   192,    -1,   193,    -1,    38,   207,
      -1,    38,    -1,    38,   207,    -1,    38,    -1,    38,   207,
      -1,    38,    -1,    -1,   195,    -1,   196,    -1,   197,    -1,
     198,    -1,   196,   197,    -1,   196,   198,    -1,   197,   198,
      -1,   197,   196,    -1,   198,   196,    -1,   198,   197,    -1,
     196,   197,   198,    -1,   196,   198,   197,    -1,   197,   196,
     198,    -1,   197,   198,   196,    -1,   198,   196,   197,    -1,
     198,   197,   196,    -1,    38,    -1,     6,   210,    -1,     7,
     210,    -1,     8,   210,    -1,    -1,    -1,    -1,    26,    38,
      76,    38,   220,   200,   148,   201,   156,   202,   206,    -1,
      -1,    -1,    -1,    26,    38,    76,    38,    25,    38,   220,
     203,   148,   204,   156,   205,   206,    -1,    30,   220,    -1,
      80,   210,    81,    -1,   209,    -1,   210,    -1,    42,    84,
     210,    79,   210,    85,    -1,    42,    84,   210,    85,    -1,
      42,    84,    85,    -1,    42,    -1,   211,    -1,   212,    -1,
      84,   211,    85,    -1,   211,    50,   211,    -1,   211,    51,
     211,    -1,   211,    52,   211,    -1,   211,    53,   211,    -1,
     211,    54,   211,    -1,   211,    55,   211,    -1,    51,   211,
      -1,   211,    58,   211,    -1,   211,    59,   211,    -1,   211,
      60,   211,    -1,   211,    61,   211,    -1,   211,    62,   211,
      -1,   211,    63,   211,    -1,   211,    64,   211,    -1,   211,
      65,   211,    -1,   211,    66,   211,    -1,    67,   211,    -1,
     211,    68,   211,    -1,   211,    69,   211,    -1,   211,    70,
     211,    -1,    71,   211,    -1,   211,    72,   211,    -1,   211,
      73,   211,    -1,   211,    75,   211,    76,   211,    -1,    38,
      84,    85,    -1,    38,    84,   211,    85,    -1,    38,    84,
     211,    79,   211,    85,    -1,    38,    84,   211,    79,   211,
      79,   211,    85,    -1,    38,    84,   211,    79,   211,    79,
     211,    79,   211,    85,    -1,   213,    -1,   214,    -1,   215,
      -1,   216,    -1,   217,    -1,    38,    -1,    43,    38,    -1,
      44,    38,    -1,    43,    44,    38,    -1,    44,    43,    38,
      -1,    38,    83,    38,    -1,    43,    38,    83,    38,    -1,
      38,   207,    83,    38,    -1,    43,    38,   207,    83,    38,
      -1,    39,    -1,    40,    -1,    41,    -1,   218,    -1,    57,
      -1,    56,    84,    38,    85,    -1,    36,    -1,    37,    -1,
     219,    -1,    36,    38,   219,    -1,    37,    38,   219,    -1,
      36,    38,    -1,    37,    38,    -1,    78,    -1,    -1,    79,
      -1,    78,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   216,   216,   217,   221,   222,   226,   227,   228,   229,
     230,   235,   234,   243,   244,   248,   257,   260,   262,   264,
     267,   269,   271,   273,   275,   277,   280,   282,   284,   286,
     288,   290,   295,   305,   304,   308,   307,   314,   321,   328,
     335,   343,   352,   355,   361,   372,   380,   383,   389,   390,
     393,   393,   394,   394,   395,   395,   396,   396,   397,   397,
     401,   400,   407,   408,   412,   413,   417,   425,   432,   431,
     439,   440,   444,   447,   452,   455,   458,   461,   464,   467,
     470,   473,   476,   483,   482,   490,   491,   495,   496,   497,
     498,   502,   503,   507,   511,   518,   519,   523,   527,   535,
     534,   542,   543,   547,   548,   553,   557,   552,   560,   564,
     559,   567,   571,   566,   574,   578,   573,   584,   583,   591,
     592,   597,   598,   603,   602,   608,   607,   617,   618,   623,
     624,   628,   635,   636,   640,   641,   646,   645,   651,   650,
     660,   661,   665,   666,   670,   677,   678,   683,   682,   688,
     687,   697,   698,   702,   703,   707,   711,   718,   721,   726,
     725,   732,   731,   740,   741,   745,   746,   750,   751,   756,
     755,   765,   766,   770,   777,   780,   785,   788,   793,   794,
     798,   801,   804,   808,   815,   816,   820,   825,   832,   836,
     842,   848,   856,   857,   861,   865,   871,   875,   881,   886,
     894,   896,   898,   900,   902,   905,   907,   909,   911,   913,
     915,   918,   920,   922,   924,   926,   928,   933,   939,   945,
     951,   958,   962,   964,   957,   968,   970,   974,   967,   980,
     985,   990,   991,   995,   997,   999,  1001,  1006,  1012,  1014,
    1017,  1019,  1021,  1023,  1025,  1027,  1030,  1034,  1036,  1038,
    1040,  1042,  1044,  1047,  1049,  1051,  1053,  1057,  1059,  1061,
    1063,  1066,  1068,  1070,  1072,  1074,  1076,  1078,  1080,  1085,
    1086,  1087,  1088,  1089,  1093,  1095,  1097,  1099,  1101,  1104,
    1106,  1109,  1111,  1116,  1121,  1123,  1128,  1133,  1135,  1140,
    1141,  1142,  1146,  1148,  1150,  1152,  1157,  1157,  1158,  1158,
    1158
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INCLUDE", "SIMPLE", "CHANNEL", "DELAY", 
  "ERROR", "DATARATE", "MODULE", "PARAMETERS", "GATES", "GATESIZES", 
  "SUBMODULES", "CONNECTIONS", "DISPLAY", "IN", "OUT", "NOCHECK", 
  "LEFT_ARROW", "RIGHT_ARROW", "FOR", "TO", "DO", "IF", "LIKE", "NETWORK", 
  "ENDSIMPLE", "ENDMODULE", "ENDCHANNEL", "ENDNETWORK", "ENDFOR", 
  "MACHINES", "ON", "IO_INTERFACES", "IFPAIR", "INTCONSTANT", 
  "REALCONSTANT", "NAME", "STRING", "_TRUE", "_FALSE", "INPUT", "REF", 
  "ANCESTOR", "NED_CONST", "NUMERICTYPE", "STRINGTYPE", "BOOLTYPE", 
  "ANYTYPE", "PLUS", "MIN", "MUL", "DIV", "MOD", "EXP", "SIZEOF", 
  "SUBMODINDEX", "EQ", "NE", "GT", "GE", "LS", "LE", "AND", "OR", "XOR", 
  "NOT", "BIN_AND", "BIN_OR", "BIN_XOR", "BIN_COMPL", "SHIFT_LEFT", 
  "SHIFT_RIGHT", "INVALID_CHAR", "'?'", "':'", "UMIN", "';'", "','", 
  "'['", "']'", "'='", "'.'", "'('", "')'", "$accept", 
  "networkdescription", "somedefinitions", "definition", "import", "@1", 
  "filenames", "filename", "channeldefinition", "channelheader", 
  "endchannel", "@2", "@3", "cherror", "chdelay", "chdatarate", 
  "simpledefinition", "simpleheader", "endsimple", "moduledefinition", 
  "moduleheader", "endmodule", "opt_machineblock", "opt_displayblock", 
  "opt_paramblock", "opt_gateblock", "opt_submodblock", "opt_connblock", 
  "machineblock", "@4", "opt_machinelist", "machinelist", "machine", 
  "displayblock", "paramblock", "@5", "opt_parameters", "parameters", 
  "parameter", "gateblock", "@6", "opt_gates", "gates", "gatesI", "gateI", 
  "gatesO", "gateO", "submodblock", "@7", "opt_submodules", "submodules", 
  "submodule", "@8", "@9", "@10", "@11", "@12", "@13", "@14", "@15", 
  "submodule_body", "@16", "opt_on_blocks", "on_blocks", "on_block", 
  "@17", "@18", "opt_on_list", "on_list", "on_mach", 
  "opt_substparamblocks", "substparamblocks", "substparamblock", "@19", 
  "@20", "opt_substparameters", "substparameters", "substparameter", 
  "opt_gatesizeblocks", "gatesizeblock", "@21", "@22", "opt_gatesizes", 
  "gatesizes", "gatesize", "opt_submod_displayblock", "connblock", "@23", 
  "@24", "opt_connections", "connections", "connection", "loopconnection", 
  "@25", "loopvarlist", "loopvar", "opt_conn_condition", 
  "opt_conn_displaystr", "notloopconnections", "notloopconnection", 
  "gate_spec_L", "mod_L", "gate_L", "parentgate_L", "gate_spec_R", 
  "mod_R", "gate_R", "parentgate_R", "channeldescr", "cdname", "cddelay", 
  "cderror", "cddatarate", "network", "@26", "@27", "@28", "@29", "@30", 
  "@31", "endnetwork", "vector", "inputvalue_or_expression", "inputvalue", 
  "expression", "expr", "simple_expr", "parameter_expr", "string_expr", 
  "boolconst_expr", "numconst_expr", "special_expr", "numconst", 
  "timeconstant", "opt_semicolon", "comma_or_semicolon", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,    63,    58,   330,    59,    44,
      91,    93,    61,    46,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    86,    87,    87,    88,    88,    89,    89,    89,    89,
      89,    91,    90,    92,    92,    93,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    95,    97,    96,    98,    96,    99,   100,   101,
     102,   103,   104,   104,   105,   106,   107,   107,   108,   108,
     109,   109,   110,   110,   111,   111,   112,   112,   113,   113,
     115,   114,   116,   116,   117,   117,   118,   119,   121,   120,
     122,   122,   123,   123,   124,   124,   124,   124,   124,   124,
     124,   124,   124,   126,   125,   127,   127,   128,   128,   128,
     128,   129,   129,   130,   130,   131,   131,   132,   132,   134,
     133,   135,   135,   136,   136,   138,   139,   137,   140,   141,
     137,   142,   143,   137,   144,   145,   137,   147,   146,   148,
     148,   149,   149,   151,   150,   152,   150,   153,   153,   154,
     154,   155,   156,   156,   157,   157,   159,   158,   160,   158,
     161,   161,   162,   162,   163,   164,   164,   166,   165,   167,
     165,   168,   168,   169,   169,   170,   170,   171,   171,   173,
     172,   174,   172,   175,   175,   176,   176,   177,   177,   179,
     178,   180,   180,   181,   182,   182,   183,   183,   184,   184,
     185,   185,   185,   185,   186,   186,   187,   187,   188,   188,
     189,   189,   190,   190,   191,   191,   192,   192,   193,   193,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   195,   196,   197,
     198,   200,   201,   202,   199,   203,   204,   205,   199,   206,
     207,   208,   208,   209,   209,   209,   209,   210,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   212,
     212,   212,   212,   212,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   214,   215,   215,   216,   217,   217,   218,
     218,   218,   219,   219,   219,   219,   220,   220,   221,   221,
     221
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     1,     1,
       1,     0,     4,     3,     1,     1,     2,     3,     3,     3,
       4,     4,     4,     4,     4,     4,     5,     5,     5,     5,
       5,     5,     2,     0,     4,     0,     3,     3,     3,     3,
       5,     3,     3,     2,     8,     3,     3,     2,     1,     0,
       1,     0,     1,     0,     1,     0,     1,     0,     1,     0,
       0,     4,     2,     0,     3,     1,     1,     4,     0,     4,
       2,     0,     3,     1,     1,     3,     2,     3,     4,     4,
       3,     3,     3,     0,     4,     1,     0,     4,     3,     4,
       3,     3,     1,     3,     1,     3,     1,     3,     1,     0,
       4,     1,     0,     2,     1,     0,     0,     8,     0,     0,
       9,     0,     0,    10,     0,     0,    11,     0,     4,     1,
       0,     2,     1,     0,     4,     0,     6,     2,     0,     3,
       1,     1,     1,     0,     2,     1,     0,     4,     0,     6,
       2,     0,     3,     1,     3,     2,     0,     0,     4,     0,
       6,     2,     0,     3,     1,     2,     1,     4,     0,     0,
       5,     0,     4,     2,     0,     3,     1,     1,     1,     0,
       6,     3,     1,     5,     2,     0,     2,     0,     3,     2,
       5,     7,     5,     7,     3,     1,     2,     1,     2,     1,
       2,     1,     3,     1,     2,     1,     2,     1,     2,     1,
       0,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     1,     2,     2,
       2,     0,     0,     0,    11,     0,     0,     0,    13,     2,
       3,     1,     1,     6,     4,     3,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     3,     3,     3,
       2,     3,     3,     5,     3,     4,     6,     8,    10,     1,
       1,     1,     1,     1,     1,     2,     2,     3,     3,     3,
       4,     4,     5,     1,     1,     1,     1,     1,     4,     1,
       1,     1,     3,     3,     2,     2,     1,     0,     1,     1,
       0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       3,    11,     0,     0,     0,     0,     0,     2,     5,     6,
       7,     0,     8,    49,     9,    49,    10,     0,   297,    32,
     297,     0,     1,     4,     0,     0,     0,   297,    16,     0,
       0,     0,     0,    53,    48,    53,    15,     0,    14,   296,
      41,    45,     0,   289,   290,   274,   283,   284,   285,     0,
       0,     0,     0,   287,     0,     0,     0,   297,   237,   238,
     269,   270,   271,   272,   273,   286,   291,   297,   297,   297,
      35,    18,     0,     0,    17,     0,     0,    19,     0,     0,
      60,     0,    55,    52,    55,    12,     0,   297,   294,   295,
       0,     0,     0,     0,   275,     0,   276,     0,   246,     0,
     256,   260,     0,    38,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    37,    39,    33,    36,    23,
       0,    22,     0,    20,     0,    21,     0,    25,     0,    24,
       0,    63,    68,     0,     0,    54,    57,    13,     0,   221,
       0,     0,   292,   293,     0,   279,   264,     0,     0,     0,
       0,   277,   278,     0,   239,   240,   241,   242,   243,   244,
     245,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     257,   258,   259,   261,   262,     0,    34,    28,    29,    26,
      27,    31,    30,    66,    61,     0,    65,    71,    83,   297,
      40,     0,    59,    56,   297,   120,   230,     0,   265,   281,
     280,     0,   288,     0,    62,     0,    74,     0,    69,     0,
      73,    86,   297,    43,    99,     0,    51,    58,   225,     0,
     222,   119,   122,     0,   282,   263,    64,     0,    76,    70,
       0,     0,     0,    84,    85,    42,   102,     0,   161,     0,
       0,    50,   120,     0,   123,   133,   121,     0,   266,    77,
      75,    80,    81,    82,    72,    94,     0,    92,    98,     0,
      96,     0,     0,     0,   100,   101,   104,   159,   164,     0,
     297,    44,   226,     0,   128,     0,   223,   132,   135,     0,
      78,    79,     0,    88,     0,     0,    90,     0,     0,     0,
       0,   103,   164,   169,   191,   162,   300,   166,   167,   168,
       0,     0,   185,     0,   297,    47,   133,   125,   131,   124,
       0,   130,     0,   136,     0,   134,     0,   267,    93,    91,
      97,    95,    87,    89,   297,   160,     0,   190,   163,   298,
       0,   200,   200,     0,    67,    46,   227,   128,   127,     0,
       0,   141,   297,   224,     0,     0,   297,   105,     0,     0,
     172,   165,     0,     0,     0,   199,   175,     0,   193,     0,
     201,   202,   203,   204,   175,     0,   189,   184,     0,   126,
     129,   138,     0,   137,     0,   143,   229,   268,   297,     0,
     108,   120,     0,     0,     0,   218,   219,   220,   198,     0,
     177,     0,     0,   205,   206,   208,   207,   209,   210,   177,
       0,   188,   228,   141,     0,   140,     0,   111,   297,   120,
     106,     0,     0,   300,   171,   174,     0,   182,   197,   192,
     199,   175,   211,   212,   213,   214,   215,   216,   180,   175,
     139,   236,   144,   231,   232,   142,   120,   114,   109,   133,
       0,   170,   300,   299,   179,   176,   196,   177,   177,     0,
     112,   120,   133,   107,   117,   173,   178,   183,   181,   235,
       0,   133,   115,   110,   146,     0,   234,   113,   133,     0,
     158,   146,     0,   116,     0,   147,     0,   118,   145,   233,
       0,   152,     0,   149,   156,   148,     0,   154,     0,   152,
     155,   151,     0,   157,   150,   153
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     6,     7,     8,     9,    17,    37,    38,    10,    11,
      28,   186,   128,    29,    30,    31,    12,    13,   200,    14,
      15,   281,    33,   250,    82,   144,   202,   226,    34,   141,
     194,   195,   196,   251,    83,   197,   218,   219,   220,   145,
     221,   243,   244,   266,   267,   269,   270,   203,   246,   274,
     275,   276,   391,   449,   419,   462,   446,   471,   461,   478,
     463,   474,   230,   231,   232,   284,   347,   319,   320,   321,
     464,   287,   288,   351,   413,   383,   384,   385,   480,   481,
     491,   499,   495,   496,   497,   487,   227,   302,   278,   305,
     306,   307,   308,   336,   359,   360,   400,   427,   422,   309,
     310,   311,   377,   312,   366,   367,   429,   368,   369,   370,
     371,   372,   373,    16,   205,   255,   324,   252,   316,   378,
     353,   398,   442,   443,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    40,   340
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -411
static const short yypact[] =
{
      27,  -411,    -3,    60,    83,    94,    33,    27,  -411,  -411,
    -411,    14,  -411,   132,  -411,   132,  -411,   145,   142,  -411,
     142,   154,  -411,  -411,   233,   233,   233,   -26,  -411,   127,
      17,   194,   166,   239,  -411,   239,  -411,   -30,  -411,  -411,
    -411,  -411,   227,   229,   237,   -46,  -411,  -411,  -411,   165,
      75,   233,   212,  -411,   233,   233,   233,   142,   521,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,   142,   142,   142,
    -411,  -411,    21,    12,  -411,    21,    10,  -411,    12,    10,
    -411,   202,   288,  -411,   288,  -411,   145,   -11,   223,   223,
     233,   265,    87,   218,    54,   273,  -411,   278,  -411,   298,
    -411,  -411,   438,  -411,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,  -411,  -411,  -411,  -411,  -411,
     297,  -411,   297,  -411,   297,  -411,   297,  -411,   297,  -411,
     297,   305,  -411,   269,   325,  -411,   340,  -411,   332,  -411,
     229,   237,  -411,  -411,   282,  -411,  -411,   296,   339,   341,
     299,  -411,  -411,   293,  -411,   172,   172,   328,   328,   328,
     328,   255,   255,   255,   255,   255,   255,   571,   571,   571,
     342,   342,   342,   203,   203,   494,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,   128,  -411,    91,  -411,    71,
    -411,   304,   370,  -411,   142,   353,  -411,   233,  -411,  -411,
    -411,   349,  -411,   233,  -411,   305,   312,   352,  -411,   185,
    -411,   264,   142,  -411,  -411,   -13,   376,  -411,  -411,    -5,
    -411,   353,  -411,   358,  -411,   547,  -411,   170,  -411,  -411,
      91,   360,   361,  -411,   266,  -411,   362,   326,  -411,   327,
     373,  -411,   353,   233,  -411,   394,  -411,   233,  -411,   359,
     384,  -411,  -411,  -411,  -411,   355,   207,  -411,   356,   209,
    -411,   360,   361,   331,  -411,   362,  -411,  -411,   101,   393,
      72,  -411,  -411,   363,   400,    30,  -411,   394,  -411,   402,
    -411,  -411,   364,  -411,   360,   365,  -411,   361,   214,   216,
     403,  -411,   101,  -411,   160,  -411,   234,  -411,  -411,  -411,
     295,   357,  -411,   366,   142,  -411,   394,  -411,  -411,  -411,
     240,  -411,   233,  -411,   412,  -411,   233,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,    19,  -411,   409,   375,   124,  -411,
     101,   109,   109,   410,  -411,  -411,  -411,   400,  -411,   400,
     397,   411,   142,  -411,   466,   431,    22,  -411,   396,    -7,
    -411,  -411,   233,   233,   233,   -10,   452,   399,  -411,   460,
    -411,   313,   242,   323,   452,   463,   404,  -411,   412,  -411,
    -411,  -411,   398,  -411,   253,  -411,  -411,  -411,   142,   447,
    -411,   353,   233,   448,   409,  -411,  -411,  -411,   422,   233,
     479,   457,   471,   504,   507,   504,   509,   507,   509,   479,
     471,  -411,  -411,   411,   195,  -411,   411,  -411,   142,   353,
    -411,   500,   158,   256,  -411,  -411,   498,  -411,   404,  -411,
     161,   452,  -411,  -411,  -411,  -411,  -411,  -411,  -411,   452,
    -411,   449,  -411,  -411,  -411,  -411,   353,  -411,  -411,   394,
     233,  -411,   256,  -411,  -411,  -411,  -411,   479,   479,   137,
    -411,   353,   394,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
     126,   394,  -411,  -411,   528,   233,  -411,  -411,   394,    38,
     527,   528,   458,  -411,   233,  -411,   474,  -411,  -411,  -411,
     485,   530,   526,  -411,   404,  -411,   263,  -411,   499,   530,
    -411,  -411,   530,  -411,  -411,  -411
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -411,  -411,  -411,   581,  -411,  -411,  -411,   492,  -411,  -411,
      29,  -411,  -411,    81,   117,   111,  -411,  -411,  -411,  -411,
    -411,  -411,   577,  -411,   560,   519,  -411,  -411,  -411,  -411,
    -411,  -411,   389,  -411,  -411,  -411,  -411,  -411,   374,  -411,
    -411,  -411,  -411,   347,   333,   377,   338,  -411,  -411,  -411,
    -411,   367,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -280,  -411,  -249,  -411,   405,  -411,  -411,   281,  -411,   289,
    -248,  -411,   350,  -411,  -411,   232,  -411,   230,   167,  -411,
    -411,  -411,   148,  -411,   149,  -411,  -411,  -411,  -411,   348,
    -411,   314,  -411,  -411,  -411,   258,  -370,  -306,  -411,  -382,
    -411,  -411,  -411,  -411,  -336,  -411,  -411,  -411,   311,  -411,
    -253,  -205,  -158,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
     277,   -43,  -411,  -411,   -25,   -28,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,   284,   -12,  -410
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -300
static const short yytable[] =
{
      67,    68,    93,   282,   409,   247,   374,   286,    41,  -217,
    -217,   423,    69,   454,   148,    70,   393,    25,    24,   253,
      24,    25,    26,    98,    25,    26,   100,   101,   102,    26,
       1,     2,     3,    22,    90,    18,     4,    91,    92,    27,
     452,    27,   466,    27,   355,   103,    27,   389,    85,    86,
      27,   160,    39,     5,   322,   125,   126,   127,    71,    74,
      77,   457,   484,   248,   157,   154,   431,    39,   346,   458,
      90,   254,   394,  -195,   439,   149,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,    39,    19,    90,
      39,   129,   131,   438,   133,   135,   323,   137,   139,   222,
     314,    75,    78,    96,   485,   362,   363,   364,    97,   405,
     407,    20,   303,    43,    44,    45,    46,    47,    48,   216,
      49,    50,    21,    24,    90,    26,   217,   159,    51,   304,
      73,    76,   420,    52,    53,  -299,    72,   365,    79,    39,
      39,   467,   468,   435,    54,   437,    27,   136,    55,   187,
     140,   188,  -299,   189,    32,   190,   403,   191,   408,   192,
     448,    56,   156,    43,    44,    45,    46,    47,    48,   233,
      49,    50,   473,   130,    36,   235,   134,   223,    51,   451,
     132,   477,   228,    52,    53,   138,   304,   460,   483,   433,
      24,    25,   436,    94,    54,   475,   214,   215,    55,    95,
     245,   476,   472,   404,   406,   259,   260,   261,   262,   263,
      39,    56,   469,    27,   106,   107,   108,   109,   283,   289,
      42,    43,    44,    45,    46,    47,    48,   441,    49,    50,
      90,    90,    80,  -187,  -195,   432,    51,   434,   362,    81,
     364,    52,    53,   104,   105,   106,   107,   108,   109,   150,
     151,   337,    54,   239,   240,    87,    55,    88,   315,    43,
      44,    45,    46,    47,    48,    89,    49,    50,   142,    56,
     241,   242,   271,   272,    51,   293,   294,   296,   297,    52,
      53,   356,   332,   294,   333,   297,    99,   350,   354,   143,
      54,   158,   345,   155,    55,   104,   105,   106,   107,   108,
     109,   161,   338,   339,   341,   342,   162,    56,   348,   349,
     363,   364,   357,   119,   120,   121,    27,   122,   123,   362,
     363,   415,   416,   411,   453,   339,   163,   395,   396,   397,
     386,   501,   502,   193,   390,   198,   104,   105,   106,   107,
     108,   109,   199,   201,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   206,   119,   120,   121,   421,   122,   123,
     204,   124,   152,   153,   425,   207,   417,   209,   212,   210,
     224,   208,   211,   109,   225,   456,   229,   234,   237,   444,
     238,   249,   104,   105,   106,   107,   108,   109,   265,   268,
     273,   280,   277,   279,   285,   290,   447,   300,   104,   105,
     106,   107,   108,   109,   122,   123,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   465,   119,   120,   121,   291,
     122,   123,   313,   124,   470,   292,   295,   257,   318,   317,
     343,   334,   352,   258,   344,   328,   330,   358,   376,   382,
     482,   500,   104,   105,   106,   107,   108,   109,  -186,   490,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   388,
     119,   120,   121,   381,   122,   123,   399,   124,   392,   402,
     414,   326,   401,   410,    90,   418,   304,   327,   104,   105,
     106,   107,   108,   109,   426,   428,   110,   111,   112,   113,
     114,   115,   116,   117,   118,  -194,   119,   120,   121,   430,
     122,   123,   364,   124,   363,   362,   104,   105,   106,   107,
     108,   109,   450,   164,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   459,   119,   120,   121,   455,   122,   123,
     479,   124,   486,   489,   104,   105,   106,   107,   108,   109,
     492,   387,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   493,   119,   120,   121,   498,   122,   123,   494,   124,
     213,   104,   105,   106,   107,   108,   109,   503,   147,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    23,   119,
     120,   121,    35,   122,   123,    84,   124,   104,   105,   106,
     107,   108,   109,   146,   236,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   264,   119,   120,   121,   298,   122,
     123,   104,   105,   106,   107,   108,   109,   329,   379,   110,
     111,   112,   113,   114,   115,   331,   256,   325,   380,   119,
     120,   121,   301,   122,   123,   440,   445,   504,   488,   299,
     335,   505,   424,   375,   361,   412
};

static const unsigned short yycheck[] =
{
      25,    26,    45,   252,   374,    18,   342,   255,    20,    19,
      20,   393,    38,   423,    25,    27,    23,     7,     6,    24,
       6,     7,     8,    51,     7,     8,    54,    55,    56,     8,
       3,     4,     5,     0,    80,    38,     9,    83,    84,    29,
     422,    29,   452,    29,    25,    57,    29,    25,    78,    79,
      29,    94,    78,    26,    24,    67,    68,    69,    29,    30,
      31,   431,    24,    76,    92,    90,   402,    78,   316,   439,
      80,    76,    79,    83,   410,    87,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,    78,    38,    80,
      78,    72,    73,   409,    75,    76,    76,    78,    79,    38,
      38,    30,    31,    38,    76,     6,     7,     8,    43,   372,
     373,    38,    21,    36,    37,    38,    39,    40,    41,    38,
      43,    44,    38,     6,    80,     8,    45,    83,    51,    38,
      29,    30,   391,    56,    57,    21,    29,    38,    31,    78,
      78,   457,   458,   406,    67,   408,    29,    76,    71,   130,
      79,   132,    38,   134,    32,   136,   371,   138,   373,   140,
     419,    84,    85,    36,    37,    38,    39,    40,    41,   207,
      43,    44,   462,    72,    39,   213,    75,   199,    51,    31,
      73,   471,   204,    56,    57,    78,    38,   446,   478,   404,
       6,     7,   407,    38,    67,    79,    78,    79,    71,    44,
     222,    85,   461,   371,   372,    45,    46,    47,    48,    49,
      78,    84,    85,    29,    52,    53,    54,    55,   253,   257,
      76,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      80,    80,    76,    83,    83,   403,    51,   405,     6,    10,
       8,    56,    57,    50,    51,    52,    53,    54,    55,    36,
      37,   304,    67,    78,    79,    38,    71,    38,   280,    36,
      37,    38,    39,    40,    41,    38,    43,    44,    76,    84,
      16,    17,    16,    17,    51,    78,    79,    78,    79,    56,
      57,   334,    78,    79,    78,    79,    84,   322,   326,    11,
      67,    83,   314,    38,    71,    50,    51,    52,    53,    54,
      55,    38,    78,    79,    19,    20,    38,    84,    78,    79,
       7,     8,   334,    68,    69,    70,    29,    72,    73,     6,
       7,    78,    79,   376,    78,    79,    38,   362,   363,   364,
     352,    78,    79,    38,   356,    76,    50,    51,    52,    53,
      54,    55,    27,    13,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    81,    68,    69,    70,   392,    72,    73,
      38,    75,    88,    89,   399,    79,   388,    38,    85,    38,
      76,    85,    83,    55,    14,   428,    33,    38,    76,   414,
      38,    15,    50,    51,    52,    53,    54,    55,    38,    38,
      38,    28,    76,    76,    10,    46,   418,    76,    50,    51,
      52,    53,    54,    55,    72,    73,    58,    59,    60,    61,
      62,    63,    64,    65,    66,   450,    68,    69,    70,    45,
      72,    73,    39,    75,   459,    80,    80,    79,    38,    76,
      83,    38,    30,    85,    78,    81,    81,    38,    38,    38,
     475,   494,    50,    51,    52,    53,    54,    55,    83,   484,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    38,
      68,    69,    70,    76,    72,    73,    24,    75,    82,    19,
      82,    79,    83,    20,    80,    38,    38,    85,    50,    51,
      52,    53,    54,    55,    15,    38,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    83,    68,    69,    70,    38,
      72,    73,     8,    75,     7,     6,    50,    51,    52,    53,
      54,    55,    22,    85,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    84,    68,    69,    70,    39,    72,    73,
      12,    75,    15,    85,    50,    51,    52,    53,    54,    55,
      76,    85,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    76,    68,    69,    70,    39,    72,    73,    38,    75,
      76,    50,    51,    52,    53,    54,    55,    78,    86,    58,
      59,    60,    61,    62,    63,    64,    65,    66,     7,    68,
      69,    70,    15,    72,    73,    35,    75,    50,    51,    52,
      53,    54,    55,    84,   215,    58,    59,    60,    61,    62,
      63,    64,    65,    66,   240,    68,    69,    70,   271,    72,
      73,    50,    51,    52,    53,    54,    55,   294,   347,    58,
      59,    60,    61,    62,    63,   297,   231,   287,   349,    68,
      69,    70,   275,    72,    73,   413,   416,   499,   481,   272,
     302,   502,   394,   342,   340,   378
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     4,     5,     9,    26,    87,    88,    89,    90,
      94,    95,   102,   103,   105,   106,   199,    91,    38,    38,
      38,    38,     0,    89,     6,     7,     8,    29,    96,    99,
     100,   101,    32,   108,   114,   108,    39,    92,    93,    78,
     220,   220,    76,    36,    37,    38,    39,    40,    41,    43,
      44,    51,    56,    57,    67,    71,    84,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   210,   210,    38,
     220,    96,   100,   101,    96,    99,   101,    96,    99,   100,
      76,    10,   110,   120,   110,    78,    79,    38,    38,    38,
      80,    83,    84,   207,    38,    44,    38,    43,   211,    84,
     211,   211,   211,   220,    50,    51,    52,    53,    54,    55,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    68,
      69,    70,    72,    73,    75,   220,   220,   220,    98,    96,
     101,    96,   100,    96,   101,    96,    99,    96,   100,    96,
      99,   115,    76,    11,   111,   125,   111,    93,    25,   220,
      36,    37,   219,   219,   210,    38,    85,   211,    83,    83,
     207,    38,    38,    38,    85,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,    97,    96,    96,    96,
      96,    96,    96,    38,   116,   117,   118,   121,    76,    27,
     104,    13,   112,   133,    38,   200,    81,    79,    85,    38,
      38,    83,    85,    76,    78,    79,    38,    45,   122,   123,
     124,   126,    38,   220,    76,    14,   113,   172,   220,    33,
     148,   149,   150,   211,    38,   211,   118,    76,    38,    78,
      79,    16,    17,   127,   128,   220,   134,    18,    76,    15,
     109,   119,   203,    24,    76,   201,   150,    79,    85,    45,
      46,    47,    48,    49,   124,    38,   129,   130,    38,   131,
     132,    16,    17,    38,   135,   136,   137,    76,   174,    76,
      28,   107,   148,   210,   151,    10,   156,   157,   158,   211,
      46,    45,    80,    78,    79,    80,    78,    79,   129,   131,
      76,   137,   173,    21,    38,   175,   176,   177,   178,   185,
     186,   187,   189,    39,    38,   220,   204,    76,    38,   153,
     154,   155,    24,    76,   202,   158,    79,    85,    81,   130,
      81,   132,    78,    78,    38,   175,   179,   207,    78,    79,
     221,    19,    20,    83,    78,   220,   156,   152,    78,    79,
     210,   159,    30,   206,   211,    25,   207,   220,    38,   180,
     181,   177,     6,     7,     8,    38,   190,   191,   193,   194,
     195,   196,   197,   198,   190,   194,    38,   188,   205,   153,
     155,    76,    38,   161,   162,   163,   220,    85,    38,    25,
     220,   138,    82,    23,    79,   210,   210,   210,   207,    24,
     182,    83,    19,   197,   198,   196,   198,   196,   197,   182,
      20,   207,   206,   160,    82,    78,    79,   220,    38,   140,
     148,   210,   184,   185,   181,   210,    15,   183,    38,   192,
      38,   190,   198,   197,   198,   196,   197,   196,   183,   190,
     161,    42,   208,   209,   210,   163,   142,   220,   148,   139,
      22,    31,   185,    78,   221,    39,   207,   182,   182,    84,
     148,   144,   141,   146,   156,   210,   221,   183,   183,    85,
     210,   143,   148,   146,   147,    79,    85,   146,   145,    12,
     164,   165,   210,   146,    24,    76,    15,   171,   164,    85,
     210,   166,    76,    76,    38,   168,   169,   170,    39,   167,
     207,    78,    79,    78,   168,   170
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)           \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#define YYLEX	yylex ()

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*-----------------------------.
| Print this symbol on YYOUT.  |
`-----------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yysymprint (FILE* yyout, int yytype, YYSTYPE yyvalue, YYLTYPE yylocation)
#else
yysymprint (yyout, yytype, yyvalue, yylocation)
    FILE* yyout;
    int yytype;
    YYSTYPE yyvalue;
    YYLTYPE yylocation;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;
  (void) yylocation;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyout, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyout, yytoknum[yytype], yyvalue);
# endif
    }
  else
    YYFPRINTF (yyout, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyout, ")");
}
#endif /* YYDEBUG. */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yydestruct (int yytype, YYSTYPE yyvalue, YYLTYPE yylocation)
#else
yydestruct (yytype, yyvalue, yylocation)
    int yytype;
    YYSTYPE yyvalue;
    YYLTYPE yylocation;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;
  (void) yylocation;

  switch (yytype)
    {
      default:
        break;
    }
}



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of parse errors so far.  */
int yynerrs;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;


int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with.  */

  if (yychar <= 0)		/* This means end of input.  */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more.  */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

      /* We have to keep this `#if YYDEBUG', since we use variables
	 which are defined only if `YYDEBUG' is set.  */
      YYDPRINTF ((stderr, "Next token is "));
      YYDSYMPRINT ((stderr, yychar1, yylval, yyloc));
      YYDPRINTF ((stderr, "\n"));
    }

  /* If the proper action on seeing token YYCHAR1 is to reduce or to
     detect an error, take that action.  */
  yyn += yychar1;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yychar1)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
  *++yylsp = yylloc;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn - 1, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] >= 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif
  switch (yyn)
    {
        case 11:
#line 235 "ebnf.y"
    {GNED( IMPORTS_KEY = np->create("imports",NEDFILE_KEY);
                       setComments(IMPORTS_KEY,yylsp[0]); )}
    break;

  case 12:
#line 238 "ebnf.y"
    {/* GNED( setTrailingComment(IMPORTS_KEY,@3); )
                  * comment already stored with last filename */}
    break;

  case 15:
#line 249 "ebnf.y"
    {NEDC( do_include (yyvsp[0]); )
                 GNED( IMPORT_KEY = np->create("import",IMPORTS_KEY);
                       np->set(IMPORT_KEY,"name",trimQuotes(yylsp[0]));
                       setComments(IMPORT_KEY,yylsp[0]); )}
    break;

  case 16:
#line 258 "ebnf.y"
    {NEDC( do_channel (yyvsp[-1], NULL, NULL, NULL); )}
    break;

  case 17:
#line 261 "ebnf.y"
    {NEDC( do_channel (yyvsp[-2], yyvsp[-1], NULL, NULL); )}
    break;

  case 18:
#line 263 "ebnf.y"
    {NEDC( do_channel (yyvsp[-2], NULL, yyvsp[-1], NULL); )}
    break;

  case 19:
#line 265 "ebnf.y"
    {NEDC( do_channel (yyvsp[-2], NULL, NULL, yyvsp[-1]); )}
    break;

  case 20:
#line 268 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], yyvsp[-2], yyvsp[-1], NULL); )}
    break;

  case 21:
#line 270 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], yyvsp[-2], NULL, yyvsp[-1]); )}
    break;

  case 22:
#line 272 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], NULL, yyvsp[-2], yyvsp[-1]); )}
    break;

  case 23:
#line 274 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], yyvsp[-1], yyvsp[-2], NULL); )}
    break;

  case 24:
#line 276 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], yyvsp[-1], NULL, yyvsp[-2]); )}
    break;

  case 25:
#line 278 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], NULL, yyvsp[-1], yyvsp[-2]); )}
    break;

  case 26:
#line 281 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-3], yyvsp[-2], yyvsp[-1]); )}
    break;

  case 27:
#line 283 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-3], yyvsp[-1], yyvsp[-2]); )}
    break;

  case 28:
#line 285 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-2], yyvsp[-3], yyvsp[-1]); )}
    break;

  case 29:
#line 287 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-1], yyvsp[-3], yyvsp[-2]); )}
    break;

  case 30:
#line 289 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-2], yyvsp[-1], yyvsp[-3]); )}
    break;

  case 31:
#line 291 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-1], yyvsp[-2], yyvsp[-3]); )}
    break;

  case 32:
#line 296 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; inside_nonvoid_function=1;)
                 GNED( CHANNEL_KEY = np->create("channel",NEDFILE_KEY);
                       np->set(CHANNEL_KEY,"name",yylsp[0]);
                       setComments(CHANNEL_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 33:
#line 305 "ebnf.y"
    {NEDC( inside_nonvoid_function=0; )}
    break;

  case 34:
#line 306 "ebnf.y"
    {GNED( setTrailingComment(CHANNEL_KEY,yylsp[-2]); )}
    break;

  case 35:
#line 308 "ebnf.y"
    {NEDC( inside_nonvoid_function=0; )}
    break;

  case 36:
#line 309 "ebnf.y"
    {GNED( setTrailingComment(CHANNEL_KEY,yylsp[-2]); )}
    break;

  case 37:
#line 315 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"error",yylsp[-1]);
                       setComments(CHANATTR_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 38:
#line 322 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"delay",yylsp[-1]);
                       setComments(CHANATTR_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 39:
#line 329 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"datarate",yylsp[-1]);
                       setComments(CHANATTR_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 41:
#line 344 "ebnf.y"
    {NEDC( do_simple (yyvsp[-1]); )
                 GNED( MODULE_KEY = np->create("simple",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", yylsp[-1]);
                       setComments(MODULE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 42:
#line 353 "ebnf.y"
    {NEDC( end_simple (yyvsp[-1]); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 43:
#line 356 "ebnf.y"
    {NEDC( end_simple (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 45:
#line 373 "ebnf.y"
    {NEDC( do_module (yyvsp[-1]); )
                 GNED( MODULE_KEY = np->create("module",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", yylsp[-1]);
                       setComments(MODULE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 46:
#line 381 "ebnf.y"
    {NEDC( end_module (yyvsp[-1]); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 47:
#line 384 "ebnf.y"
    {NEDC( end_module (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 49:
#line 391 "ebnf.y"
    {NEDC( do_machine(jar_strdup("default")); )}
    break;

  case 60:
#line 401 "ebnf.y"
    {GNED( MACHINES_KEY = np->create("machines",MODULE_KEY);
                       setComments(MACHINES_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 66:
#line 418 "ebnf.y"
    {NEDC( do_machine (yyvsp[0]); )
                 GNED( MACHINE_KEY = np->create("machine",MACHINES_KEY);
                       np->set(MACHINE_KEY,"name", yylsp[0]);
                       setComments(MACHINE_KEY,yylsp[0],yylsp[0]); )}
    break;

  case 67:
#line 426 "ebnf.y"
    {NEDC( do_displaystr_enclosing (yyvsp[-1]); )
                 GNED( setDisplayString(MODULE_KEY,yylsp[-1]); )}
    break;

  case 68:
#line 432 "ebnf.y"
    {NEDC( do_parameters (); )
                 GNED( PARAMS_KEY = np->create("params",MODULE_KEY);
                       setComments(PARAMS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 72:
#line 445 "ebnf.y"
    {GNED( setComments(PARAM_KEY,yylsp[0]); )}
    break;

  case 73:
#line 448 "ebnf.y"
    {GNED( setComments(PARAM_KEY,yylsp[0]); )}
    break;

  case 74:
#line 453 "ebnf.y"
    {NEDC( do_parameter (yyvsp[0], TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[0],TYPE_NUMERIC); )}
    break;

  case 75:
#line 456 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_NUMERIC); )}
    break;

  case 76:
#line 459 "ebnf.y"
    {NEDC( do_parameter (yyvsp[0], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[0],TYPE_CONST_NUM); )}
    break;

  case 77:
#line 462 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_CONST_NUM); )}
    break;

  case 78:
#line 465 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-3], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-3],TYPE_CONST_NUM); )}
    break;

  case 79:
#line 468 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-3], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-3],TYPE_CONST_NUM); )}
    break;

  case 80:
#line 471 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_STRING); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_STRING); )}
    break;

  case 81:
#line 474 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_BOOL); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_BOOL); )}
    break;

  case 82:
#line 477 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_ANYTYPE); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_ANYTYPE); )}
    break;

  case 83:
#line 483 "ebnf.y"
    {NEDC( do_gates (); )
                 GNED( GATES_KEY = np->create("gates",MODULE_KEY);
                       setComments(GATES_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 93:
#line 508 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[-2], 1, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[-2], 1, 1 );
                       setComments(GATE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 94:
#line 512 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[0], 1, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[0], 1, 0 );
                       setComments(GATE_KEY,yylsp[0]); )}
    break;

  case 97:
#line 524 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[-2], 0, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[-2], 0, 1 );
                       setComments(GATE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 98:
#line 528 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[0], 0, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[0], 0, 0 );
                       setComments(GATE_KEY,yylsp[0]); )}
    break;

  case 99:
#line 535 "ebnf.y"
    {NEDC( do_submodules (); )
                 GNED( SUBMODS_KEY = np->create("submods",MODULE_KEY);
                       setComments(SUBMODS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 105:
#line 553 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-3], NULL, yyvsp[-1], NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-3], NULLPOS, yylsp[-1], NULLPOS);
                       setComments(SUBMOD_KEY,yylsp[-3],yylsp[0]);  )}
    break;

  case 106:
#line 557 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-5], NULL, yyvsp[-3], NULL); )}
    break;

  case 108:
#line 560 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-4], yyvsp[-1], yyvsp[-2], NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-4], yylsp[-1], yylsp[-2], NULLPOS);
                       setComments(SUBMOD_KEY,yylsp[-4],yylsp[0]);  )}
    break;

  case 109:
#line 564 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-6], yyvsp[-3], yyvsp[-4], NULL); )}
    break;

  case 111:
#line 567 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-5], NULL, yyvsp[-3], yyvsp[-1]); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-5], NULLPOS, yylsp[-3], yylsp[-1]);
                       setComments(SUBMOD_KEY,yylsp[-5],yylsp[0]);  )}
    break;

  case 112:
#line 571 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-7], NULL, yyvsp[-5], yyvsp[-3]); )}
    break;

  case 114:
#line 574 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-6], yyvsp[-3], yyvsp[-4], yyvsp[-1]); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-6], yylsp[-3], yylsp[-4], yylsp[-1]);
                       setComments(SUBMOD_KEY,yylsp[-6],yylsp[0]);  )}
    break;

  case 115:
#line 578 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-8], yyvsp[-5], yyvsp[-6], yyvsp[-3]); )}
    break;

  case 117:
#line 584 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 118:
#line 587 "ebnf.y"
    {NEDC( end_submodule (); )}
    break;

  case 120:
#line 593 "ebnf.y"
    {NEDC( do_empty_onlist(); )}
    break;

  case 123:
#line 603 "ebnf.y"
    {NEDC( do_onlist(); )
                 GNED( SUBSTMACHINES_KEY = np->create("substmachines",in_network?NETWORK_KEY:SUBMOD_KEY);
                       setComments(SUBSTMACHINES_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 125:
#line 608 "ebnf.y"
    {NEDC( open_if(yyvsp[-1]); do_onlist(); )
                 GNED( SUBSTMACHINES_KEY = np->create("substmachines",in_network?NETWORK_KEY:SUBMOD_KEY);
                       np->set(SUBSTMACHINES_KEY,"condition",yylsp[-1]);
                       setComments(SUBSTMACHINES_KEY,yylsp[-3],yylsp[0]); )}
    break;

  case 126:
#line 613 "ebnf.y"
    {NEDC( close_if(); )}
    break;

  case 128:
#line 619 "ebnf.y"
    {NEDC( do_empty_onlist(); )}
    break;

  case 131:
#line 629 "ebnf.y"
    {NEDC( do_on_mach(yyvsp[0]); )
                 GNED( SUBSTMACHINE_KEY = addSubstmachine(SUBSTMACHINES_KEY,yylsp[0]);
                       setComments(SUBSTMACHINE_KEY,yylsp[0]); )}
    break;

  case 136:
#line 646 "ebnf.y"
    {NEDC( do_substparams (); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       setComments(SUBSTPARAMS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 138:
#line 651 "ebnf.y"
    {NEDC( open_if(yyvsp[-1]); do_substparams(); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       np->set(SUBSTPARAMS_KEY,"condition",yylsp[-1]);
                       setComments(SUBSTPARAMS_KEY,yylsp[-3],yylsp[0]); )}
    break;

  case 139:
#line 656 "ebnf.y"
    {NEDC( close_if(); )}
    break;

  case 144:
#line 671 "ebnf.y"
    {NEDC( do_substparameter (yyvsp[-2], yyvsp[0]);)
                 GNED( SUBSTPARAM_KEY = addSubstparam(SUBSTPARAMS_KEY,yylsp[-2],yylsp[0]);
                       setComments(SUBSTPARAM_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 147:
#line 683 "ebnf.y"
    {NEDC( do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       setComments(GATESIZES_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 149:
#line 688 "ebnf.y"
    {NEDC( open_if(yyvsp[-1]); do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       np->set(GATESIZES_KEY,"condition",yylsp[-1]);
                       setComments(GATESIZES_KEY,yylsp[-3],yylsp[0]); )}
    break;

  case 150:
#line 693 "ebnf.y"
    {NEDC( close_if(); )}
    break;

  case 155:
#line 708 "ebnf.y"
    {NEDC( do_gatesize (yyvsp[-1], yyvsp[0]); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,yylsp[-1],yylsp[0]);
                       setComments(GATESIZE_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 156:
#line 712 "ebnf.y"
    {NEDC( do_gatesize (yyvsp[0], NULL); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,yylsp[0],NULLPOS);
                       setComments(GATESIZE_KEY,yylsp[0]); )}
    break;

  case 157:
#line 719 "ebnf.y"
    {NEDC( do_displaystr_submod (yyvsp[-1]); )
                 GNED( setDisplayString(SUBMOD_KEY,yylsp[-1]); )}
    break;

  case 159:
#line 726 "ebnf.y"
    {NEDC( set_checkconns(0); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","1");
                       setComments(CONNS_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 161:
#line 732 "ebnf.y"
    {NEDC( set_checkconns(1); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","0");
                       setComments(CONNS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 169:
#line 756 "ebnf.y"
    {NEDC( do_for (); )
                 GNED( FORLOOP_KEY = np->create("forloop",CONNS_KEY);
                       in_loop=1; setComments(FORLOOP_KEY,yylsp[0]); )}
    break;

  case 170:
#line 760 "ebnf.y"
    {NEDC( end_for (); )
                 GNED( in_loop=0; setTrailingComment(FORLOOP_KEY,yylsp[0]); )}
    break;

  case 173:
#line 771 "ebnf.y"
    {NEDC( do_index (yyvsp[-4], yyvsp[-2], yyvsp[0]); )
                 GNED( LOOPVAR_KEY=addLoopVar(FORLOOP_KEY,yylsp[-4],yylsp[-2],yylsp[0]);
                       setComments(LOOPVAR_KEY,yylsp[-4],yylsp[0]); )}
    break;

  case 174:
#line 778 "ebnf.y"
    {NEDC( do_condition(yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"condition",yylsp[0]); )}
    break;

  case 175:
#line 781 "ebnf.y"
    {NEDC( do_condition(NULL); )}
    break;

  case 176:
#line 786 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( setDisplayString(CONN_KEY,yylsp[0]); )}
    break;

  case 177:
#line 789 "ebnf.y"
    {NEDC( yyval = NULL; )}
    break;

  case 180:
#line 799 "ebnf.y"
    {NEDC( end_connection (NULL, 'R', yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,yylsp[-4],yylsp[0]); )}
    break;

  case 181:
#line 802 "ebnf.y"
    {NEDC( end_connection (yyvsp[-4], 'R', yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,yylsp[-6],yylsp[0]); )}
    break;

  case 182:
#line 805 "ebnf.y"
    {NEDC( end_connection (NULL, 'L', yyvsp[0]); )
                 GNED( swapConnection(CONN_KEY);
                       np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,yylsp[-4],yylsp[0]); )}
    break;

  case 183:
#line 809 "ebnf.y"
    {NEDC( end_connection (yyvsp[-4], 'L', yyvsp[0]); )
                 GNED( swapConnection(CONN_KEY);
                 np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,yylsp[-6],yylsp[0]); )}
    break;

  case 186:
#line 821 "ebnf.y"
    {NEDC( do_mod_L (yyvsp[-1], yyvsp[0]); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey(yylsp[-1]));
                       np->set(CONN_KEY, "src-mod-index", trimBrackets(yylsp[0])); )}
    break;

  case 187:
#line 826 "ebnf.y"
    {NEDC( do_mod_L (yyvsp[0], NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey(yylsp[0])); )}
    break;

  case 188:
#line 833 "ebnf.y"
    {NEDC( do_gate_L (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "srcgate", yylsp[-1]);
                       np->set(CONN_KEY, "src-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 189:
#line 837 "ebnf.y"
    {NEDC( do_gate_L (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "srcgate", yylsp[0]); )}
    break;

  case 190:
#line 843 "ebnf.y"
    {NEDC( do_mod_L (NULL, NULL); do_gate_L (yyvsp[-1], yyvsp[0]); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", yylsp[-1]);
                       np->set(CONN_KEY, "src-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 191:
#line 849 "ebnf.y"
    {NEDC( do_mod_L (NULL, NULL); do_gate_L (yyvsp[0], NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", yylsp[0]); )}
    break;

  case 194:
#line 862 "ebnf.y"
    {NEDC( do_mod_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey(yylsp[-1]));
                       np->set(CONN_KEY, "dest-mod-index", trimBrackets(yylsp[0])); )}
    break;

  case 195:
#line 866 "ebnf.y"
    {NEDC( do_mod_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey(yylsp[0])); )}
    break;

  case 196:
#line 872 "ebnf.y"
    {NEDC( do_gate_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "destgate", yylsp[-1]);
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 197:
#line 876 "ebnf.y"
    {NEDC( do_gate_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "destgate", yylsp[0]); )}
    break;

  case 198:
#line 882 "ebnf.y"
    {NEDC( do_mod_R (NULL, NULL); do_gate_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", yylsp[-1]);
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 199:
#line 887 "ebnf.y"
    {NEDC( do_mod_R (NULL, NULL); do_gate_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", yylsp[0]); )}
    break;

  case 200:
#line 895 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, NULL, NULL); )}
    break;

  case 201:
#line 897 "ebnf.y"
    {NEDC( yyval = do_channeldescr(yyvsp[0], NULL,NULL,NULL); )}
    break;

  case 202:
#line 899 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], NULL, NULL); )}
    break;

  case 203:
#line 901 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[0], NULL); )}
    break;

  case 204:
#line 903 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, NULL, yyvsp[0]); )}
    break;

  case 205:
#line 906 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[0], NULL); )}
    break;

  case 206:
#line 908 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], NULL, yyvsp[0]); )}
    break;

  case 207:
#line 910 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[-1], yyvsp[0]); )}
    break;

  case 208:
#line 912 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-1], NULL); )}
    break;

  case 209:
#line 914 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], NULL, yyvsp[-1]); )}
    break;

  case 210:
#line 916 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[0], yyvsp[-1]); )}
    break;

  case 211:
#line 919 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-2], yyvsp[-1], yyvsp[0]); )}
    break;

  case 212:
#line 921 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-2], yyvsp[0], yyvsp[-1]); )}
    break;

  case 213:
#line 923 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[-2], yyvsp[0]); )}
    break;

  case 214:
#line 925 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-2], yyvsp[-1]); )}
    break;

  case 215:
#line 927 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[0], yyvsp[-2]); )}
    break;

  case 216:
#line 929 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-1], yyvsp[-2]); )}
    break;

  case 217:
#line 934 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"channel",yylsp[0]); )}
    break;

  case 218:
#line 940 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"delay",yylsp[0]); )}
    break;

  case 219:
#line 946 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"error",yylsp[0]); )}
    break;

  case 220:
#line 952 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"datarate",yylsp[0]); )}
    break;

  case 221:
#line 958 "ebnf.y"
    {NEDC( do_system (yyvsp[-3]); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,yylsp[-3],yylsp[-1],NULLPOS);
                       setComments(NETWORK_KEY,yylsp[-4],yylsp[0]); in_network=1;)}
    break;

  case 222:
#line 962 "ebnf.y"
    {NEDC( do_systemmodule (yyvsp[-5], yyvsp[-3], NULL); )}
    break;

  case 223:
#line 964 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 224:
#line 966 "ebnf.y"
    {NEDC( end_system (); )}
    break;

  case 225:
#line 968 "ebnf.y"
    {NEDC( do_system (yyvsp[-5]); )}
    break;

  case 226:
#line 970 "ebnf.y"
    {NEDC( do_systemmodule (yyvsp[-7], yyvsp[-5], yyvsp[-3]); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,yylsp[-7],yylsp[-5],yylsp[-3]);
                       setComments(NETWORK_KEY,yylsp[-8],yylsp[-2]); )}
    break;

  case 227:
#line 974 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 228:
#line 976 "ebnf.y"
    {NEDC( end_system (); )}
    break;

  case 229:
#line 981 "ebnf.y"
    {GNED( setTrailingComment(NETWORK_KEY,yylsp[-1]); in_network=0; )}
    break;

  case 230:
#line 986 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )}
    break;

  case 233:
#line 996 "ebnf.y"
    {NEDC( yyval = do_inputvalue(yyvsp[-3], yyvsp[-1]); )}
    break;

  case 234:
#line 998 "ebnf.y"
    {NEDC( yyval = do_inputvalue(yyvsp[-1], NULL); )}
    break;

  case 235:
#line 1000 "ebnf.y"
    {NEDC( yyval = do_inputvalue(NULL, NULL); )}
    break;

  case 236:
#line 1002 "ebnf.y"
    {NEDC( yyval = do_inputvalue(NULL, NULL); )}
    break;

  case 237:
#line 1008 "ebnf.y"
    {NEDC( yyval = end_expr( yyvsp[0] ); )}
    break;

  case 238:
#line 1013 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )}
    break;

  case 239:
#line 1015 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )}
    break;

  case 240:
#line 1018 "ebnf.y"
    {NEDC( yyval = do_op( 2, '+', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 241:
#line 1020 "ebnf.y"
    {NEDC( yyval = do_op( 2, '-', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 242:
#line 1022 "ebnf.y"
    {NEDC( yyval = do_op( 2, '*', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 243:
#line 1024 "ebnf.y"
    {NEDC( yyval = do_op( 2, '/', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 244:
#line 1026 "ebnf.y"
    {NEDC( yyval = do_op( 2, '%', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 245:
#line 1028 "ebnf.y"
    {NEDC( yyval = do_op( 2, '^', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 246:
#line 1032 "ebnf.y"
    {NEDC( yyval = do_addminus(yyvsp[0]); )}
    break;

  case 247:
#line 1035 "ebnf.y"
    {NEDC( yyval = do_op( 2, '=', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 248:
#line 1037 "ebnf.y"
    {NEDC( yyval = do_op( 2, '!', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 249:
#line 1039 "ebnf.y"
    {NEDC( yyval = do_op( 2, '>', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 250:
#line 1041 "ebnf.y"
    {NEDC( yyval = do_op( 2, '}', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 251:
#line 1043 "ebnf.y"
    {NEDC( yyval = do_op( 2, '<', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 252:
#line 1045 "ebnf.y"
    {NEDC( yyval = do_op( 2, '{', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 253:
#line 1048 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bool_and"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 254:
#line 1050 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bool_or"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 255:
#line 1052 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bool_xor"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 256:
#line 1055 "ebnf.y"
    {NEDC( yyval = do_func(1,jar_strdup("bool_not"), yyvsp[0],NULL,NULL,NULL); )}
    break;

  case 257:
#line 1058 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bin_and"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 258:
#line 1060 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bin_or"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 259:
#line 1062 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bin_xor"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 260:
#line 1065 "ebnf.y"
    {NEDC( yyval = do_func(1,jar_strdup("bin_compl"), yyvsp[0],NULL,NULL,NULL); )}
    break;

  case 261:
#line 1067 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("shift_left"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 262:
#line 1069 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("shift_right"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 263:
#line 1071 "ebnf.y"
    {NEDC( yyval = do_op( 3, '?', yyvsp[-4],yyvsp[-2],yyvsp[0]); )}
    break;

  case 264:
#line 1073 "ebnf.y"
    {NEDC( yyval = do_func(0,yyvsp[-2], NULL,NULL,NULL,NULL); )}
    break;

  case 265:
#line 1075 "ebnf.y"
    {NEDC( yyval = do_func(1,yyvsp[-3], yyvsp[-1],NULL,NULL,NULL); )}
    break;

  case 266:
#line 1077 "ebnf.y"
    {NEDC( yyval = do_func(2,yyvsp[-5], yyvsp[-3],yyvsp[-1],NULL,NULL); )}
    break;

  case 267:
#line 1079 "ebnf.y"
    {NEDC( yyval = do_func(3,yyvsp[-7], yyvsp[-5],yyvsp[-3],yyvsp[-1],NULL); )}
    break;

  case 268:
#line 1081 "ebnf.y"
    {NEDC( yyval = do_func(3,yyvsp[-9], yyvsp[-7],yyvsp[-5],yyvsp[-3],yyvsp[-1]); )}
    break;

  case 274:
#line 1094 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],0,1); )}
    break;

  case 275:
#line 1096 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],0,0); )}
    break;

  case 276:
#line 1098 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],1,1); )}
    break;

  case 277:
#line 1100 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],1,0); )}
    break;

  case 278:
#line 1102 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],1,0); )}
    break;

  case 279:
#line 1105 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-2],0,yyvsp[0],0,1); )}
    break;

  case 280:
#line 1107 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-2],0,yyvsp[0],0,0); )}
    break;

  case 281:
#line 1110 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-3],yyvsp[-2],yyvsp[0],0,1); )}
    break;

  case 282:
#line 1112 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-3],yyvsp[-2],yyvsp[0],0,0); )}
    break;

  case 283:
#line 1117 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_STRING,yyvsp[0]); )}
    break;

  case 284:
#line 1122 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_BOOL,jar_strdup("true")); )}
    break;

  case 285:
#line 1124 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_BOOL,jar_strdup("false")); )}
    break;

  case 286:
#line 1129 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_NUMERIC,yyvsp[0]); )}
    break;

  case 287:
#line 1134 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_NUMERIC,jar_strdup("(long)sub_i")); )}
    break;

  case 288:
#line 1136 "ebnf.y"
    {NEDC( yyval = do_sizeof (yyvsp[-1]); )}
    break;

  case 292:
#line 1147 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-2],yyvsp[-1],yyvsp[0]); )}
    break;

  case 293:
#line 1149 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-2],yyvsp[-1],yyvsp[0]); )}
    break;

  case 294:
#line 1151 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-1],yyvsp[0],NULL); )}
    break;

  case 295:
#line 1153 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-1],yyvsp[0],NULL); )}
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 2867 "ebnf.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;
  yylsp -= yylen;

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyssp > yyss)
	    {
	      YYDPRINTF ((stderr, "Error: popping "));
	      YYDSYMPRINT ((stderr,
			    yystos[*yyssp],
			    *yyvsp, *yylsp));
	      YYDPRINTF ((stderr, "\n"));
	      yydestruct (yystos[*yyssp], *yyvsp, *yylsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yydestruct (yychar1, yylval, yylloc);
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDPRINTF ((stderr, "Error: popping "));
      YYDSYMPRINT ((stderr,
		    yystos[*yyssp], *yyvsp, *yylsp));
      YYDPRINTF ((stderr, "\n"));

      yydestruct (yystos[yystate], *yyvsp, *yylsp);
      yyvsp--;
      yystate = *--yyssp;
      yylsp--;

#if YYDEBUG
      if (yydebug)
	{
	  short *yyssp1 = yyss - 1;
	  YYFPRINTF (stderr, "Error: state stack now");
	  while (yyssp1 != yyssp)
	    YYFPRINTF (stderr, " %d", *++yyssp1);
	  YYFPRINTF (stderr, "\n");
	}
#endif
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
  *++yylsp = yylloc;

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 1160 "ebnf.y"


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
}

void setDisplayString(int key, YYLTYPE dispstrpos)
{
  // cut off quotes
  dispstrpos.first_column++;
  dispstrpos.last_column--;

  np->set(key,"displaystring", np->nedsource->get(dispstrpos) );
}


#endif




