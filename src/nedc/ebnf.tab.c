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
#define SUBMODS_KEY       np->submods_key
#define SUBMOD_KEY        np->submod_key
#define SUBSTPARAMS_KEY   np->substparams_key
#define SUBSTPARAM_KEY    np->substparam_key
#define GATESIZES_KEY     np->gatesizes_key
#define GATESIZE_KEY      np->gatesize_key
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
#line 376 "ebnf.tab.c"

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
#define YYLAST   632

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  122
/* YYNRULES -- Number of rules. */
#define YYNRULES  278
/* YYNRULES -- Number of states. */
#define YYNSTATES  469

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
     142,   147,   151,   155,   158,   166,   170,   174,   177,   179,
     180,   182,   183,   185,   186,   188,   189,   191,   192,   197,
     198,   203,   206,   207,   211,   213,   215,   219,   222,   226,
     231,   236,   240,   244,   248,   249,   254,   256,   257,   262,
     266,   271,   275,   279,   281,   285,   287,   291,   293,   297,
     299,   300,   305,   307,   308,   311,   313,   314,   315,   323,
     324,   325,   334,   335,   336,   346,   347,   348,   359,   360,
     365,   367,   368,   371,   373,   374,   379,   380,   387,   390,
     391,   395,   397,   401,   404,   405,   406,   411,   412,   419,
     422,   423,   427,   429,   432,   434,   439,   440,   441,   447,
     448,   453,   456,   457,   461,   463,   465,   467,   468,   475,
     479,   481,   487,   490,   491,   494,   495,   499,   502,   508,
     516,   522,   530,   534,   536,   539,   541,   544,   546,   549,
     551,   555,   557,   560,   562,   565,   567,   570,   572,   573,
     575,   577,   579,   581,   584,   587,   590,   593,   596,   599,
     603,   607,   611,   615,   619,   623,   625,   628,   631,   634,
     635,   636,   637,   648,   649,   650,   651,   664,   667,   671,
     673,   675,   682,   687,   691,   693,   695,   697,   701,   705,
     709,   713,   717,   721,   725,   728,   732,   736,   740,   744,
     748,   752,   756,   760,   764,   767,   771,   775,   779,   782,
     786,   790,   796,   800,   805,   812,   821,   832,   834,   836,
     838,   840,   842,   844,   847,   850,   854,   858,   862,   867,
     872,   878,   880,   882,   884,   886,   888,   893,   895,   897,
     899,   903,   907,   910,   913,   915,   916,   918,   920
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
      87,     0,    -1,    88,    -1,    -1,    88,    89,    -1,    89,
      -1,    90,    -1,    94,    -1,   102,    -1,   105,    -1,   185,
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
      29,    38,   206,    97,    -1,    -1,    29,   206,    98,    -1,
       7,   196,   206,    -1,     6,   196,   206,    -1,     8,   196,
     206,    -1,   103,   109,   110,   104,    -1,     4,    38,   206,
      -1,    27,    38,   206,    -1,    27,   206,    -1,   106,   109,
     110,   111,   112,   108,   107,    -1,     9,    38,   206,    -1,
      28,    38,   206,    -1,    28,   206,    -1,   113,    -1,    -1,
     114,    -1,    -1,   119,    -1,    -1,   127,    -1,    -1,   158,
      -1,    -1,    15,    76,    39,    78,    -1,    -1,    10,    76,
     115,   116,    -1,   117,    78,    -1,    -1,   117,    79,   118,
      -1,   118,    -1,    38,    -1,    38,    76,    46,    -1,    45,
      38,    -1,    38,    76,    45,    -1,    38,    76,    45,    46,
      -1,    38,    76,    46,    45,    -1,    38,    76,    47,    -1,
      38,    76,    48,    -1,    38,    76,    49,    -1,    -1,    11,
      76,   120,   121,    -1,   122,    -1,    -1,   122,    16,   123,
      78,    -1,    16,   123,    78,    -1,   122,    17,   125,    78,
      -1,    17,   125,    78,    -1,   123,    79,   124,    -1,   124,
      -1,    38,    80,    81,    -1,    38,    -1,   125,    79,   126,
      -1,   126,    -1,    38,    80,    81,    -1,    38,    -1,    -1,
      13,    76,   128,   129,    -1,   130,    -1,    -1,   130,   131,
      -1,   131,    -1,    -1,    -1,    38,    76,    38,   206,   132,
     133,   140,    -1,    -1,    -1,    38,    76,    38,   193,   206,
     134,   135,   140,    -1,    -1,    -1,    38,    76,    38,    25,
      38,   206,   136,   137,   140,    -1,    -1,    -1,    38,    76,
      38,   193,    25,    38,   206,   138,   139,   140,    -1,    -1,
     142,   141,   150,   157,    -1,   143,    -1,    -1,   143,   144,
      -1,   144,    -1,    -1,    10,    76,   145,   147,    -1,    -1,
      10,    24,   196,    76,   146,   147,    -1,   148,    78,    -1,
      -1,   148,    79,   149,    -1,   149,    -1,    38,    82,   194,
      -1,   151,   150,    -1,    -1,    -1,    12,    76,   152,   154,
      -1,    -1,    12,    24,   196,    76,   153,   154,    -1,   155,
      78,    -1,    -1,   155,    79,   156,    -1,   156,    -1,    38,
     193,    -1,    38,    -1,    15,    76,    39,    78,    -1,    -1,
      -1,    14,    18,    76,   159,   161,    -1,    -1,    14,    76,
     160,   161,    -1,   162,    78,    -1,    -1,   162,   207,   163,
      -1,   163,    -1,   164,    -1,   171,    -1,    -1,    21,   165,
     166,    23,   170,    31,    -1,   166,    79,   167,    -1,   167,
      -1,    38,    82,   196,    22,   196,    -1,    24,   196,    -1,
      -1,    15,    39,    -1,    -1,   170,   171,   207,    -1,   171,
     207,    -1,   172,    20,   176,   168,   169,    -1,   172,    20,
     180,    20,   176,   168,   169,    -1,   172,    19,   176,   168,
     169,    -1,   172,    19,   180,    19,   176,   168,   169,    -1,
     173,    83,   174,    -1,   175,    -1,    38,   193,    -1,    38,
      -1,    38,   193,    -1,    38,    -1,    38,   193,    -1,    38,
      -1,   177,    83,   178,    -1,   179,    -1,    38,   193,    -1,
      38,    -1,    38,   193,    -1,    38,    -1,    38,   193,    -1,
      38,    -1,    -1,   181,    -1,   182,    -1,   183,    -1,   184,
      -1,   182,   183,    -1,   182,   184,    -1,   183,   184,    -1,
     183,   182,    -1,   184,   182,    -1,   184,   183,    -1,   182,
     183,   184,    -1,   182,   184,   183,    -1,   183,   182,   184,
      -1,   183,   184,   182,    -1,   184,   182,   183,    -1,   184,
     183,   182,    -1,    38,    -1,     6,   196,    -1,     7,   196,
      -1,     8,   196,    -1,    -1,    -1,    -1,    26,    38,    76,
      38,   206,   186,   187,   142,   188,   192,    -1,    -1,    -1,
      -1,    26,    38,    76,    38,    25,    38,   206,   189,   190,
     142,   191,   192,    -1,    30,   206,    -1,    80,   196,    81,
      -1,   195,    -1,   196,    -1,    42,    84,   196,    79,   196,
      85,    -1,    42,    84,   196,    85,    -1,    42,    84,    85,
      -1,    42,    -1,   197,    -1,   198,    -1,    84,   197,    85,
      -1,   197,    50,   197,    -1,   197,    51,   197,    -1,   197,
      52,   197,    -1,   197,    53,   197,    -1,   197,    54,   197,
      -1,   197,    55,   197,    -1,    51,   197,    -1,   197,    58,
     197,    -1,   197,    59,   197,    -1,   197,    60,   197,    -1,
     197,    61,   197,    -1,   197,    62,   197,    -1,   197,    63,
     197,    -1,   197,    64,   197,    -1,   197,    65,   197,    -1,
     197,    66,   197,    -1,    67,   197,    -1,   197,    68,   197,
      -1,   197,    69,   197,    -1,   197,    70,   197,    -1,    71,
     197,    -1,   197,    72,   197,    -1,   197,    73,   197,    -1,
     197,    75,   197,    76,   197,    -1,    38,    84,    85,    -1,
      38,    84,   197,    85,    -1,    38,    84,   197,    79,   197,
      85,    -1,    38,    84,   197,    79,   197,    79,   197,    85,
      -1,    38,    84,   197,    79,   197,    79,   197,    79,   197,
      85,    -1,   199,    -1,   200,    -1,   201,    -1,   202,    -1,
     203,    -1,    38,    -1,    43,    38,    -1,    44,    38,    -1,
      43,    44,    38,    -1,    44,    43,    38,    -1,    38,    83,
      38,    -1,    43,    38,    83,    38,    -1,    38,   193,    83,
      38,    -1,    43,    38,   193,    83,    38,    -1,    39,    -1,
      40,    -1,    41,    -1,   204,    -1,    57,    -1,    56,    84,
      38,    85,    -1,    36,    -1,    37,    -1,   205,    -1,    36,
      38,   205,    -1,    37,    38,   205,    -1,    36,    38,    -1,
      37,    38,    -1,    78,    -1,    -1,    79,    -1,    78,    -1,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   211,   211,   212,   216,   217,   221,   222,   223,   224,
     225,   230,   229,   238,   239,   243,   252,   255,   257,   259,
     262,   264,   266,   268,   270,   272,   275,   277,   279,   281,
     283,   285,   290,   300,   299,   303,   302,   309,   316,   323,
     330,   337,   346,   349,   355,   365,   373,   376,   381,   381,
     382,   382,   383,   383,   384,   384,   385,   385,   388,   395,
     394,   402,   403,   407,   410,   415,   418,   421,   424,   427,
     430,   433,   436,   439,   446,   445,   453,   454,   458,   459,
     460,   461,   465,   466,   470,   474,   481,   482,   486,   490,
     498,   497,   505,   506,   510,   511,   516,   519,   515,   522,
     525,   521,   528,   531,   527,   534,   537,   533,   543,   542,
     550,   551,   555,   556,   561,   560,   566,   565,   575,   576,
     580,   581,   585,   592,   593,   598,   597,   603,   602,   612,
     613,   617,   618,   622,   626,   633,   636,   641,   640,   647,
     646,   655,   656,   660,   661,   665,   666,   671,   670,   680,
     681,   685,   692,   695,   700,   703,   708,   709,   713,   716,
     719,   723,   730,   731,   735,   740,   747,   751,   757,   763,
     771,   772,   776,   780,   786,   790,   796,   801,   809,   811,
     813,   815,   817,   820,   822,   824,   826,   828,   830,   833,
     835,   837,   839,   841,   843,   848,   854,   860,   866,   873,
     876,   878,   872,   882,   883,   887,   881,   893,   898,   903,
     904,   908,   910,   912,   914,   919,   925,   927,   930,   932,
     934,   936,   938,   940,   943,   947,   949,   951,   953,   955,
     957,   960,   962,   964,   966,   970,   972,   974,   976,   979,
     981,   983,   985,   987,   989,   991,   993,   998,   999,  1000,
    1001,  1002,  1006,  1008,  1010,  1012,  1014,  1017,  1019,  1022,
    1024,  1029,  1034,  1036,  1041,  1046,  1048,  1053,  1054,  1055,
    1059,  1061,  1063,  1065,  1070,  1070,  1071,  1071,  1071
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
  "moduleheader", "endmodule", "opt_displayblock", "opt_paramblock", 
  "opt_gateblock", "opt_submodblock", "opt_connblock", "displayblock", 
  "paramblock", "@4", "opt_parameters", "parameters", "parameter", 
  "gateblock", "@5", "opt_gates", "gates", "gatesI", "gateI", "gatesO", 
  "gateO", "submodblock", "@6", "opt_submodules", "submodules", 
  "submodule", "@7", "@8", "@9", "@10", "@11", "@12", "@13", "@14", 
  "submodule_body", "@15", "opt_substparamblocks", "substparamblocks", 
  "substparamblock", "@16", "@17", "opt_substparameters", 
  "substparameters", "substparameter", "opt_gatesizeblocks", 
  "gatesizeblock", "@18", "@19", "opt_gatesizes", "gatesizes", "gatesize", 
  "opt_submod_displayblock", "connblock", "@20", "@21", "opt_connections", 
  "connections", "connection", "loopconnection", "@22", "loopvarlist", 
  "loopvar", "opt_conn_condition", "opt_conn_displaystr", 
  "notloopconnections", "notloopconnection", "gate_spec_L", "mod_L", 
  "gate_L", "parentgate_L", "gate_spec_R", "mod_R", "gate_R", 
  "parentgate_R", "channeldescr", "cdname", "cddelay", "cderror", 
  "cddatarate", "network", "@23", "@24", "@25", "@26", "@27", "@28", 
  "endnetwork", "vector", "inputvalue_or_expression", "inputvalue", 
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
     109,   109,   110,   110,   111,   111,   112,   112,   113,   115,
     114,   116,   116,   117,   117,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   120,   119,   121,   121,   122,   122,
     122,   122,   123,   123,   124,   124,   125,   125,   126,   126,
     128,   127,   129,   129,   130,   130,   132,   133,   131,   134,
     135,   131,   136,   137,   131,   138,   139,   131,   141,   140,
     142,   142,   143,   143,   145,   144,   146,   144,   147,   147,
     148,   148,   149,   150,   150,   152,   151,   153,   151,   154,
     154,   155,   155,   156,   156,   157,   157,   159,   158,   160,
     158,   161,   161,   162,   162,   163,   163,   165,   164,   166,
     166,   167,   168,   168,   169,   169,   170,   170,   171,   171,
     171,   171,   172,   172,   173,   173,   174,   174,   175,   175,
     176,   176,   177,   177,   178,   178,   179,   179,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   181,   182,   183,   184,   186,
     187,   188,   185,   189,   190,   191,   185,   192,   193,   194,
     194,   195,   195,   195,   195,   196,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   198,   198,   198,
     198,   198,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   200,   201,   201,   202,   203,   203,   204,   204,   204,
     205,   205,   205,   205,   206,   206,   207,   207,   207
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     1,     1,
       1,     0,     4,     3,     1,     1,     2,     3,     3,     3,
       4,     4,     4,     4,     4,     4,     5,     5,     5,     5,
       5,     5,     2,     0,     4,     0,     3,     3,     3,     3,
       4,     3,     3,     2,     7,     3,     3,     2,     1,     0,
       1,     0,     1,     0,     1,     0,     1,     0,     4,     0,
       4,     2,     0,     3,     1,     1,     3,     2,     3,     4,
       4,     3,     3,     3,     0,     4,     1,     0,     4,     3,
       4,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       0,     4,     1,     0,     2,     1,     0,     0,     7,     0,
       0,     8,     0,     0,     9,     0,     0,    10,     0,     4,
       1,     0,     2,     1,     0,     4,     0,     6,     2,     0,
       3,     1,     3,     2,     0,     0,     4,     0,     6,     2,
       0,     3,     1,     2,     1,     4,     0,     0,     5,     0,
       4,     2,     0,     3,     1,     1,     1,     0,     6,     3,
       1,     5,     2,     0,     2,     0,     3,     2,     5,     7,
       5,     7,     3,     1,     2,     1,     2,     1,     2,     1,
       3,     1,     2,     1,     2,     1,     2,     1,     0,     1,
       1,     1,     1,     2,     2,     2,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     1,     2,     2,     2,     0,
       0,     0,    10,     0,     0,     0,    12,     2,     3,     1,
       1,     6,     4,     3,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     3,     2,     3,
       3,     5,     3,     4,     6,     8,    10,     1,     1,     1,
       1,     1,     1,     2,     2,     3,     3,     3,     4,     4,
       5,     1,     1,     1,     1,     1,     4,     1,     1,     1,
       3,     3,     2,     2,     1,     0,     1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       3,    11,     0,     0,     0,     0,     0,     2,     5,     6,
       7,     0,     8,    51,     9,    51,    10,     0,   275,    32,
     275,     0,     1,     4,     0,     0,     0,   275,    16,     0,
       0,     0,     0,    53,    50,    53,    15,     0,    14,   274,
      41,    45,     0,   267,   268,   252,   261,   262,   263,     0,
       0,     0,     0,   265,     0,     0,     0,   275,   215,   216,
     247,   248,   249,   250,   251,   264,   269,   275,   275,   275,
      35,    18,     0,     0,    17,     0,     0,    19,     0,     0,
      59,     0,     0,    52,    55,    12,     0,   275,   272,   273,
       0,     0,     0,     0,   253,     0,   254,     0,   224,     0,
     234,   238,     0,    38,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    37,    39,    33,    36,    23,
       0,    22,     0,    20,     0,    21,     0,    25,     0,    24,
       0,    62,    74,   275,    40,     0,    57,    54,    13,     0,
     199,     0,     0,   270,   271,     0,   257,   242,     0,     0,
       0,     0,   255,   256,     0,   217,   218,   219,   220,   221,
     222,   223,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   235,   236,   237,   239,   240,     0,    34,    28,    29,
      26,    27,    31,    30,    65,     0,    60,     0,    64,    77,
     275,    43,    90,     0,    49,    56,   275,   200,   208,     0,
     243,   259,   258,     0,   266,     0,     0,    67,    61,     0,
       0,     0,    75,    76,    42,    93,     0,   139,     0,     0,
      48,   203,   111,     0,   260,   241,    68,    66,    71,    72,
      73,    63,    85,     0,    83,    89,     0,    87,     0,     0,
       0,    91,    92,    95,   137,   142,     0,   275,    44,   204,
       0,   201,   110,   113,     0,   244,    69,    70,     0,    79,
       0,     0,    81,     0,     0,     0,     0,    94,   142,   147,
     169,   140,   278,   144,   145,   146,     0,     0,   163,     0,
     275,    47,   111,     0,   114,     0,   112,     0,    84,    82,
      88,    86,    78,    80,   275,   138,     0,   168,   141,   276,
       0,   178,   178,     0,    58,    46,   205,     0,   119,   275,
     202,     0,   245,     0,   275,    96,     0,     0,   150,   143,
       0,     0,     0,   177,   153,     0,   171,     0,   179,   180,
     181,   182,   153,     0,   167,   162,     0,   116,     0,   115,
       0,   121,   207,     0,   275,     0,    99,    97,     0,     0,
       0,   196,   197,   198,   176,     0,   155,     0,     0,   183,
     184,   186,   185,   187,   188,   155,     0,   166,   206,   119,
       0,   118,     0,   246,   102,   275,   100,   111,     0,     0,
     278,   149,   152,     0,   160,   175,   170,   177,   153,   189,
     190,   191,   192,   193,   194,   158,   153,   117,   214,   122,
     209,   210,   120,   103,   105,   111,    98,   108,     0,   148,
     278,   277,   157,   154,   174,   155,   155,     0,   111,   106,
     101,   124,   151,   156,   161,   159,   213,     0,   104,   111,
       0,   136,   124,     0,   212,   107,     0,   125,     0,   109,
     123,     0,     0,   130,     0,   211,   127,   134,   126,     0,
     132,     0,   130,   133,   129,     0,   135,   128,   131
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     6,     7,     8,     9,    17,    37,    38,    10,    11,
      28,   187,   128,    29,    30,    31,    12,    13,   144,    14,
      15,   258,   229,    33,    82,   146,   204,   230,    34,   141,
     196,   197,   198,    83,   199,   222,   223,   243,   244,   246,
     247,   147,   225,   251,   252,   253,   357,   387,   386,   415,
     413,   428,   429,   439,   416,   431,   417,   262,   263,   318,
     379,   349,   350,   351,   441,   442,   453,   462,   458,   459,
     460,   449,   205,   278,   255,   281,   282,   283,   284,   306,
     327,   328,   366,   394,   389,   285,   286,   287,   345,   288,
     334,   335,   396,   336,   337,   338,   339,   340,   341,    16,
     207,   232,   295,   259,   292,   346,   320,   364,   409,   410,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      40,   310
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -397
static const short yypact[] =
{
     226,  -397,   -11,    10,    68,    77,    22,   226,  -397,  -397,
    -397,   247,  -397,   119,  -397,   119,  -397,    86,    71,  -397,
      71,    82,  -397,  -397,   165,   165,   165,   -29,  -397,   271,
     231,   161,    89,   170,  -397,   170,  -397,    78,  -397,  -397,
    -397,  -397,   196,   205,   207,    90,  -397,  -397,  -397,    73,
     -12,   165,   157,  -397,   165,   165,   165,    71,   491,  -397,
    -397,  -397,  -397,  -397,  -397,  -397,  -397,    71,    71,    71,
    -397,  -397,   101,     4,  -397,   101,   221,  -397,     4,   221,
    -397,   171,   232,  -397,   265,  -397,    86,    19,   127,   127,
     165,   261,    95,   227,   203,   291,  -397,   298,  -397,   300,
    -397,  -397,   408,  -397,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,  -397,  -397,  -397,  -397,  -397,
     325,  -397,   325,  -397,   325,  -397,   325,  -397,   325,  -397,
     325,   -17,  -397,   -20,  -397,   293,   366,  -397,  -397,   346,
    -397,   205,   207,  -397,  -397,   306,  -397,  -397,   262,   353,
     361,   320,  -397,  -397,   319,  -397,   296,   296,   350,   350,
     350,   350,   212,   212,   212,   212,   212,   212,   541,   541,
     541,   219,   219,   219,   243,   243,   464,  -397,  -397,  -397,
    -397,  -397,  -397,  -397,   330,   369,  -397,    98,  -397,   166,
      71,  -397,  -397,   -10,   393,  -397,    71,  -397,  -397,   165,
    -397,  -397,  -397,   371,  -397,   165,   297,  -397,  -397,   -17,
     373,   374,  -397,   194,  -397,   375,   334,  -397,   339,   388,
    -397,  -397,   407,   313,  -397,   517,   382,   384,  -397,  -397,
    -397,  -397,   340,   179,  -397,   359,   209,  -397,   373,   374,
     343,  -397,   375,  -397,  -397,    -4,   404,    30,  -397,  -397,
     -21,  -397,   407,  -397,   165,  -397,  -397,  -397,   365,  -397,
     373,   367,  -397,   374,   224,   240,   411,  -397,    -4,  -397,
     228,  -397,   279,  -397,  -397,  -397,    16,   370,  -397,   376,
      71,  -397,   407,   165,  -397,   420,  -397,   372,  -397,  -397,
    -397,  -397,  -397,  -397,    -9,  -397,   414,   381,     8,  -397,
      -4,   218,   218,   417,  -397,  -397,  -397,   380,   427,    71,
    -397,   165,  -397,   437,    20,  -397,   397,    17,  -397,  -397,
     165,   165,   165,   -13,   458,   401,  -397,   466,  -397,   352,
     115,   355,   458,   472,   423,  -397,   420,  -397,   425,  -397,
     311,  -397,  -397,   436,    71,   474,  -397,  -397,   165,   475,
     414,  -397,  -397,  -397,   448,   165,   495,   482,   497,   530,
     540,   530,   542,   540,   542,   495,   497,  -397,  -397,   427,
     156,  -397,   427,  -397,  -397,    71,  -397,   407,   536,    -1,
     316,  -397,  -397,   523,  -397,   423,  -397,   273,   458,  -397,
    -397,  -397,  -397,  -397,  -397,  -397,   458,  -397,   481,  -397,
    -397,  -397,  -397,  -397,  -397,   407,  -397,  -397,   165,  -397,
     316,  -397,  -397,  -397,  -397,   495,   495,   104,   407,  -397,
    -397,   561,  -397,  -397,  -397,  -397,  -397,    34,  -397,   407,
      23,   559,   561,   165,  -397,  -397,   165,  -397,   508,  -397,
    -397,   503,   521,   560,   566,  -397,  -397,   423,  -397,   318,
    -397,   528,   560,  -397,  -397,   560,  -397,  -397,  -397
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -397,  -397,  -397,   600,  -397,  -397,  -397,   522,  -397,  -397,
     -16,  -397,  -397,    74,    81,    97,  -397,  -397,  -397,  -397,
    -397,  -397,  -397,   597,   580,  -397,  -397,  -397,  -397,  -397,
    -397,  -397,   398,  -397,  -397,  -397,  -397,   368,   348,   377,
     347,  -397,  -397,  -397,  -397,   378,  -397,  -397,  -397,  -397,
    -397,  -397,  -397,  -397,  -396,  -397,  -228,  -397,   357,  -397,
    -397,   242,  -397,   241,   180,  -397,  -397,  -397,   162,  -397,
     160,  -397,  -397,  -397,  -397,   349,  -397,   321,  -397,  -397,
    -397,   268,  -304,  -325,  -397,  -348,  -397,  -397,  -397,  -397,
    -273,  -397,  -397,  -397,   317,  -397,  -154,  -122,  -125,  -397,
    -397,  -397,  -397,  -397,  -397,  -397,   286,   -43,  -397,  -397,
     -25,   -31,  -397,  -397,  -397,  -397,  -397,  -397,  -397,   312,
     -15,  -283
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -278
static const short yytable[] =
{
      67,    68,    93,   293,   261,    41,  -195,  -195,   226,    69,
      24,   390,    70,    71,    74,    77,   323,   279,   200,   430,
      98,   194,    22,   100,   101,   102,    96,    18,   195,  -277,
     419,    97,   438,    27,   280,   311,   312,   280,   375,   342,
     359,   420,   103,   445,   149,   355,  -277,   446,    19,    39,
     405,   161,   125,   126,   127,   294,   129,   131,    39,   133,
     135,   158,   137,   139,   316,   155,   227,    90,   290,    39,
    -173,    90,   150,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   425,   398,   360,    39,    39,   447,
     434,   435,   426,   406,    75,    78,    20,   422,    39,    26,
      72,    94,    79,   443,   188,    21,   189,    95,   190,   444,
     191,   330,   192,   332,   193,    36,    73,    76,   201,    32,
      27,    43,    44,    45,    46,    47,    48,   433,    49,    50,
      43,    44,    45,    46,    47,    48,    51,    49,    50,    39,
     136,    52,    53,   140,   132,    51,    85,    86,    42,   138,
      52,    53,    54,   151,   152,    80,    55,    24,    25,   130,
      90,    54,   134,    91,    92,    55,   218,   219,   233,    56,
     157,    81,   220,   221,   235,   224,   371,   373,    56,   436,
      27,   231,    43,    44,    45,    46,    47,    48,   408,    49,
      50,    43,    44,    45,    46,    47,    48,    51,    49,    50,
     248,   249,    52,    53,   370,   372,    51,   369,   402,   374,
     404,    52,    53,    54,   330,   331,   332,    55,    25,     1,
       2,     3,    54,   297,    87,     4,    55,   307,    25,    26,
      56,    99,   291,    88,   399,    89,   401,   142,   400,    56,
      27,   403,     5,    24,    25,    26,   333,   269,   270,   143,
      27,   324,   104,   105,   106,   107,   108,   109,   317,   104,
     105,   106,   107,   108,   109,   315,    27,    24,   145,    26,
     119,   120,   121,    90,   122,   123,   160,   272,   273,   325,
     353,   122,   123,   104,   105,   106,   107,   108,   109,   156,
      27,   377,   302,   270,   352,   361,   362,   363,    90,   356,
     159,  -165,   104,   105,   106,   107,   108,   109,   303,   273,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   162,
     119,   120,   121,   388,   122,   123,   163,   124,   164,   384,
     392,   209,   236,   237,   238,   239,   240,   210,   106,   107,
     108,   109,   424,    90,    27,   411,  -173,   308,   309,   331,
     332,   330,   331,   104,   105,   106,   107,   108,   109,   202,
     414,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     203,   119,   120,   121,   206,   122,   123,   208,   124,   381,
     382,   211,   264,   432,   421,   309,   464,   465,   265,   212,
     153,   154,   437,   213,   214,   109,   216,   217,   228,   234,
     254,   242,   245,   250,   463,   256,   257,   260,   451,   276,
     268,   452,   104,   105,   106,   107,   108,   109,   266,   267,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   271,
     119,   120,   121,   289,   122,   123,   298,   124,   300,   304,
     319,   321,   326,   313,   314,   344,   347,   322,   104,   105,
     106,   107,   108,   109,  -164,   348,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   354,   119,   120,   121,   358,
     122,   123,   365,   124,   367,   368,   104,   105,   106,   107,
     108,   109,   376,   165,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    90,   119,   120,   121,   380,   122,   123,
     393,   124,   385,   280,   104,   105,   106,   107,   108,   109,
     395,   383,   110,   111,   112,   113,   114,   115,   116,   117,
     118,  -172,   119,   120,   121,   397,   122,   123,   332,   124,
     215,   104,   105,   106,   107,   108,   109,   331,   330,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   418,   119,
     120,   121,   423,   122,   123,   427,   124,   104,   105,   106,
     107,   108,   109,   440,   448,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   454,   119,   120,   121,   455,   122,
     123,   104,   105,   106,   107,   108,   109,   456,   457,   110,
     111,   112,   113,   114,   115,   461,   466,    23,   148,   119,
     120,   121,    35,   122,   123,    84,   274,   241,   299,   296,
     301,   407,   450,   412,   467,   468,   275,   305,   391,   343,
     277,   329,   378
};

static const unsigned short yycheck[] =
{
      25,    26,    45,    24,   232,    20,    19,    20,    18,    38,
       6,   359,    27,    29,    30,    31,    25,    21,    38,   415,
      51,    38,     0,    54,    55,    56,    38,    38,    45,    21,
      31,    43,   428,    29,    38,    19,    20,    38,   342,   312,
      23,   389,    57,   439,    25,    25,    38,    24,    38,    78,
     375,    94,    67,    68,    69,    76,    72,    73,    78,    75,
      76,    92,    78,    79,   292,    90,    76,    80,    38,    78,
      83,    80,    87,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   398,   368,    79,    78,    78,    76,
     425,   426,   406,   376,    30,    31,    38,   390,    78,     8,
      29,    38,    31,    79,   130,    38,   132,    44,   134,    85,
     136,     6,   138,     8,   140,    39,    29,    30,   143,    10,
      29,    36,    37,    38,    39,    40,    41,   420,    43,    44,
      36,    37,    38,    39,    40,    41,    51,    43,    44,    78,
      76,    56,    57,    79,    73,    51,    78,    79,    76,    78,
      56,    57,    67,    36,    37,    76,    71,     6,     7,    72,
      80,    67,    75,    83,    84,    71,    78,    79,   209,    84,
      85,    11,    16,    17,   215,   200,   340,   341,    84,    85,
      29,   206,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    36,    37,    38,    39,    40,    41,    51,    43,    44,
      16,    17,    56,    57,   339,   340,    51,   339,   372,   341,
     374,    56,    57,    67,     6,     7,     8,    71,     7,     3,
       4,     5,    67,   264,    38,     9,    71,   280,     7,     8,
      84,    84,   257,    38,   369,    38,   371,    76,   370,    84,
      29,   373,    26,     6,     7,     8,    38,    78,    79,    27,
      29,   304,    50,    51,    52,    53,    54,    55,   293,    50,
      51,    52,    53,    54,    55,   290,    29,     6,    13,     8,
      68,    69,    70,    80,    72,    73,    83,    78,    79,   304,
     321,    72,    73,    50,    51,    52,    53,    54,    55,    38,
      29,   344,    78,    79,   319,   330,   331,   332,    80,   324,
      83,    83,    50,    51,    52,    53,    54,    55,    78,    79,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    38,
      68,    69,    70,   358,    72,    73,    38,    75,    38,   354,
     365,    79,    45,    46,    47,    48,    49,    85,    52,    53,
      54,    55,   395,    80,    29,   380,    83,    78,    79,     7,
       8,     6,     7,    50,    51,    52,    53,    54,    55,    76,
     385,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      14,    68,    69,    70,    38,    72,    73,    81,    75,    78,
      79,    38,    79,   418,    78,    79,    78,    79,    85,    38,
      88,    89,   427,    83,    85,    55,    76,    38,    15,    38,
      76,    38,    38,    38,   457,    76,    28,    10,   443,    76,
      80,   446,    50,    51,    52,    53,    54,    55,    46,    45,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    80,
      68,    69,    70,    39,    72,    73,    81,    75,    81,    38,
      30,    79,    38,    83,    78,    38,    76,    85,    50,    51,
      52,    53,    54,    55,    83,    38,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    38,    68,    69,    70,    82,
      72,    73,    24,    75,    83,    19,    50,    51,    52,    53,
      54,    55,    20,    85,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    80,    68,    69,    70,    82,    72,    73,
      15,    75,    38,    38,    50,    51,    52,    53,    54,    55,
      38,    85,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    83,    68,    69,    70,    38,    72,    73,     8,    75,
      76,    50,    51,    52,    53,    54,    55,     7,     6,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    22,    68,
      69,    70,    39,    72,    73,    84,    75,    50,    51,    52,
      53,    54,    55,    12,    15,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    76,    68,    69,    70,    85,    72,
      73,    50,    51,    52,    53,    54,    55,    76,    38,    58,
      59,    60,    61,    62,    63,    39,    78,     7,    86,    68,
      69,    70,    15,    72,    73,    35,   248,   219,   270,   262,
     273,   379,   442,   382,   462,   465,   249,   278,   360,   312,
     252,   310,   346
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     4,     5,     9,    26,    87,    88,    89,    90,
      94,    95,   102,   103,   105,   106,   185,    91,    38,    38,
      38,    38,     0,    89,     6,     7,     8,    29,    96,    99,
     100,   101,    10,   109,   114,   109,    39,    92,    93,    78,
     206,   206,    76,    36,    37,    38,    39,    40,    41,    43,
      44,    51,    56,    57,    67,    71,    84,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   196,   196,    38,
     206,    96,   100,   101,    96,    99,   101,    96,    99,   100,
      76,    11,   110,   119,   110,    78,    79,    38,    38,    38,
      80,    83,    84,   193,    38,    44,    38,    43,   197,    84,
     197,   197,   197,   206,    50,    51,    52,    53,    54,    55,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    68,
      69,    70,    72,    73,    75,   206,   206,   206,    98,    96,
     101,    96,   100,    96,   101,    96,    99,    96,   100,    96,
      99,   115,    76,    27,   104,    13,   111,   127,    93,    25,
     206,    36,    37,   205,   205,   196,    38,    85,   197,    83,
      83,   193,    38,    38,    38,    85,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,    97,    96,    96,
      96,    96,    96,    96,    38,    45,   116,   117,   118,   120,
      38,   206,    76,    14,   112,   158,    38,   186,    81,    79,
      85,    38,    38,    83,    85,    76,    76,    38,    78,    79,
      16,    17,   121,   122,   206,   128,    18,    76,    15,   108,
     113,   206,   187,   197,    38,   197,    45,    46,    47,    48,
      49,   118,    38,   123,   124,    38,   125,   126,    16,    17,
      38,   129,   130,   131,    76,   160,    76,    28,   107,   189,
      10,   142,   143,   144,    79,    85,    46,    45,    80,    78,
      79,    80,    78,    79,   123,   125,    76,   131,   159,    21,
      38,   161,   162,   163,   164,   171,   172,   173,   175,    39,
      38,   206,   190,    24,    76,   188,   144,   197,    81,   124,
      81,   126,    78,    78,    38,   161,   165,   193,    78,    79,
     207,    19,    20,    83,    78,   206,   142,   196,   145,    30,
     192,    79,    85,    25,   193,   206,    38,   166,   167,   163,
       6,     7,     8,    38,   176,   177,   179,   180,   181,   182,
     183,   184,   176,   180,    38,   174,   191,    76,    38,   147,
     148,   149,   206,   197,    38,    25,   206,   132,    82,    23,
      79,   196,   196,   196,   193,    24,   168,    83,    19,   183,
     184,   182,   184,   182,   183,   168,    20,   193,   192,   146,
      82,    78,    79,    85,   206,    38,   134,   133,   196,   170,
     171,   167,   196,    15,   169,    38,   178,    38,   176,   184,
     183,   184,   182,   183,   182,   169,   176,   147,    42,   194,
     195,   196,   149,   136,   206,   135,   140,   142,    22,    31,
     171,    78,   207,    39,   193,   168,   168,    84,   137,   138,
     140,   141,   196,   207,   169,   169,    85,   196,   140,   139,
      12,   150,   151,    79,    85,   140,    24,    76,    15,   157,
     150,   196,   196,   152,    76,    85,    76,    38,   154,   155,
     156,    39,   153,   193,    78,    79,    78,   154,   156
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
#line 230 "ebnf.y"
    {GNED( IMPORTS_KEY = np->create("imports",NEDFILE_KEY);
                       setComments(IMPORTS_KEY,yylsp[0]); )}
    break;

  case 12:
#line 233 "ebnf.y"
    {/* GNED( setTrailingComment(IMPORTS_KEY,@3); )
                  * comment already stored with last filename */}
    break;

  case 15:
#line 244 "ebnf.y"
    {NEDC( do_include (yyvsp[0]); )
                 GNED( IMPORT_KEY = np->create("import",IMPORTS_KEY);
                       np->set(IMPORT_KEY,"name",trimQuotes(yylsp[0]));
                       setComments(IMPORT_KEY,yylsp[0]); )}
    break;

  case 16:
#line 253 "ebnf.y"
    {NEDC( do_channel (yyvsp[-1], NULL, NULL, NULL); )}
    break;

  case 17:
#line 256 "ebnf.y"
    {NEDC( do_channel (yyvsp[-2], yyvsp[-1], NULL, NULL); )}
    break;

  case 18:
#line 258 "ebnf.y"
    {NEDC( do_channel (yyvsp[-2], NULL, yyvsp[-1], NULL); )}
    break;

  case 19:
#line 260 "ebnf.y"
    {NEDC( do_channel (yyvsp[-2], NULL, NULL, yyvsp[-1]); )}
    break;

  case 20:
#line 263 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], yyvsp[-2], yyvsp[-1], NULL); )}
    break;

  case 21:
#line 265 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], yyvsp[-2], NULL, yyvsp[-1]); )}
    break;

  case 22:
#line 267 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], NULL, yyvsp[-2], yyvsp[-1]); )}
    break;

  case 23:
#line 269 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], yyvsp[-1], yyvsp[-2], NULL); )}
    break;

  case 24:
#line 271 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], yyvsp[-1], NULL, yyvsp[-2]); )}
    break;

  case 25:
#line 273 "ebnf.y"
    {NEDC( do_channel (yyvsp[-3], NULL, yyvsp[-1], yyvsp[-2]); )}
    break;

  case 26:
#line 276 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-3], yyvsp[-2], yyvsp[-1]); )}
    break;

  case 27:
#line 278 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-3], yyvsp[-1], yyvsp[-2]); )}
    break;

  case 28:
#line 280 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-2], yyvsp[-3], yyvsp[-1]); )}
    break;

  case 29:
#line 282 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-1], yyvsp[-3], yyvsp[-2]); )}
    break;

  case 30:
#line 284 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-2], yyvsp[-1], yyvsp[-3]); )}
    break;

  case 31:
#line 286 "ebnf.y"
    {NEDC( do_channel (yyvsp[-4], yyvsp[-1], yyvsp[-2], yyvsp[-3]); )}
    break;

  case 32:
#line 291 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; inside_nonvoid_function=1;)
                 GNED( CHANNEL_KEY = np->create("channel",NEDFILE_KEY);
                       np->set(CHANNEL_KEY,"name",yylsp[0]);
                       setComments(CHANNEL_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 33:
#line 300 "ebnf.y"
    {NEDC( inside_nonvoid_function=0; )}
    break;

  case 34:
#line 301 "ebnf.y"
    {GNED( setTrailingComment(CHANNEL_KEY,yylsp[-2]); )}
    break;

  case 35:
#line 303 "ebnf.y"
    {NEDC( inside_nonvoid_function=0; )}
    break;

  case 36:
#line 304 "ebnf.y"
    {GNED( setTrailingComment(CHANNEL_KEY,yylsp[-2]); )}
    break;

  case 37:
#line 310 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"error",yylsp[-1]);
                       setComments(CHANATTR_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 38:
#line 317 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"delay",yylsp[-1]);
                       setComments(CHANATTR_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 39:
#line 324 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"datarate",yylsp[-1]);
                       setComments(CHANATTR_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 41:
#line 338 "ebnf.y"
    {NEDC( do_simple (yyvsp[-1]); )
                 GNED( MODULE_KEY = np->create("simple",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", yylsp[-1]);
                       setComments(MODULE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 42:
#line 347 "ebnf.y"
    {NEDC( end_simple (yyvsp[-1]); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 43:
#line 350 "ebnf.y"
    {NEDC( end_simple (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 45:
#line 366 "ebnf.y"
    {NEDC( do_module (yyvsp[-1]); )
                 GNED( MODULE_KEY = np->create("module",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", yylsp[-1]);
                       setComments(MODULE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 46:
#line 374 "ebnf.y"
    {NEDC( end_module (yyvsp[-1]); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 47:
#line 377 "ebnf.y"
    {NEDC( end_module (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 58:
#line 389 "ebnf.y"
    {NEDC( do_displaystr_enclosing (yyvsp[-1]); )
                 GNED( setDisplayString(MODULE_KEY,yylsp[-1]); )}
    break;

  case 59:
#line 395 "ebnf.y"
    {NEDC( do_parameters (); )
                 GNED( PARAMS_KEY = np->create("params",MODULE_KEY);
                       setComments(PARAMS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 63:
#line 408 "ebnf.y"
    {GNED( setComments(PARAM_KEY,yylsp[0]); )}
    break;

  case 64:
#line 411 "ebnf.y"
    {GNED( setComments(PARAM_KEY,yylsp[0]); )}
    break;

  case 65:
#line 416 "ebnf.y"
    {NEDC( do_parameter (yyvsp[0], TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[0],TYPE_NUMERIC); )}
    break;

  case 66:
#line 419 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_NUMERIC); )}
    break;

  case 67:
#line 422 "ebnf.y"
    {NEDC( do_parameter (yyvsp[0], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[0],TYPE_CONST_NUM); )}
    break;

  case 68:
#line 425 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_CONST_NUM); )}
    break;

  case 69:
#line 428 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-3], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-3],TYPE_CONST_NUM); )}
    break;

  case 70:
#line 431 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-3], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-3],TYPE_CONST_NUM); )}
    break;

  case 71:
#line 434 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_STRING); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_STRING); )}
    break;

  case 72:
#line 437 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_BOOL); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_BOOL); )}
    break;

  case 73:
#line 440 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_ANYTYPE); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_ANYTYPE); )}
    break;

  case 74:
#line 446 "ebnf.y"
    {NEDC( do_gates (); )
                 GNED( GATES_KEY = np->create("gates",MODULE_KEY);
                       setComments(GATES_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 84:
#line 471 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[-2], 1, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[-2], 1, 1 );
                       setComments(GATE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 85:
#line 475 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[0], 1, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[0], 1, 0 );
                       setComments(GATE_KEY,yylsp[0]); )}
    break;

  case 88:
#line 487 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[-2], 0, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[-2], 0, 1 );
                       setComments(GATE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 89:
#line 491 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[0], 0, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[0], 0, 0 );
                       setComments(GATE_KEY,yylsp[0]); )}
    break;

  case 90:
#line 498 "ebnf.y"
    {NEDC( do_submodules (); )
                 GNED( SUBMODS_KEY = np->create("submods",MODULE_KEY);
                       setComments(SUBMODS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 96:
#line 516 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-3], NULL, yyvsp[-1], NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-3], NULLPOS, yylsp[-1], NULLPOS);
                       setComments(SUBMOD_KEY,yylsp[-3],yylsp[0]);  )}
    break;

  case 97:
#line 519 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-4], NULL, yyvsp[-2], NULL); )}
    break;

  case 99:
#line 522 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-4], yyvsp[-1], yyvsp[-2], NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-4], yylsp[-1], yylsp[-2], NULLPOS);
                       setComments(SUBMOD_KEY,yylsp[-4],yylsp[0]);  )}
    break;

  case 100:
#line 525 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-5], yyvsp[-2], yyvsp[-3], NULL); )}
    break;

  case 102:
#line 528 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-5], NULL, yyvsp[-3], yyvsp[-1]); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-5], NULLPOS, yylsp[-3], yylsp[-1]);
                       setComments(SUBMOD_KEY,yylsp[-5],yylsp[0]);  )}
    break;

  case 103:
#line 531 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-6], NULL, yyvsp[-4], yyvsp[-2]); )}
    break;

  case 105:
#line 534 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-6], yyvsp[-3], yyvsp[-4], yyvsp[-1]); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-6], yylsp[-3], yylsp[-4], yylsp[-1]);
                       setComments(SUBMOD_KEY,yylsp[-6],yylsp[0]);  )}
    break;

  case 106:
#line 537 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-7], yyvsp[-4], yyvsp[-5], yyvsp[-2]); )}
    break;

  case 108:
#line 543 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 109:
#line 546 "ebnf.y"
    {NEDC( end_submodule (); )}
    break;

  case 114:
#line 561 "ebnf.y"
    {NEDC( do_substparams (); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       setComments(SUBSTPARAMS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 116:
#line 566 "ebnf.y"
    {NEDC( open_if(yyvsp[-1]); do_substparams(); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       np->set(SUBSTPARAMS_KEY,"condition",yylsp[-1]);
                       setComments(SUBSTPARAMS_KEY,yylsp[-3],yylsp[0]); )}
    break;

  case 117:
#line 571 "ebnf.y"
    {NEDC( close_if(); )}
    break;

  case 122:
#line 586 "ebnf.y"
    {NEDC( do_substparameter (yyvsp[-2], yyvsp[0]);)
                 GNED( SUBSTPARAM_KEY = addSubstparam(SUBSTPARAMS_KEY,yylsp[-2],yylsp[0]);
                       setComments(SUBSTPARAM_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 125:
#line 598 "ebnf.y"
    {NEDC( do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       setComments(GATESIZES_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 127:
#line 603 "ebnf.y"
    {NEDC( open_if(yyvsp[-1]); do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       np->set(GATESIZES_KEY,"condition",yylsp[-1]);
                       setComments(GATESIZES_KEY,yylsp[-3],yylsp[0]); )}
    break;

  case 128:
#line 608 "ebnf.y"
    {NEDC( close_if(); )}
    break;

  case 133:
#line 623 "ebnf.y"
    {NEDC( do_gatesize (yyvsp[-1], yyvsp[0]); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,yylsp[-1],yylsp[0]);
                       setComments(GATESIZE_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 134:
#line 627 "ebnf.y"
    {NEDC( do_gatesize (yyvsp[0], NULL); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,yylsp[0],NULLPOS);
                       setComments(GATESIZE_KEY,yylsp[0]); )}
    break;

  case 135:
#line 634 "ebnf.y"
    {NEDC( do_displaystr_submod (yyvsp[-1]); )
                 GNED( setDisplayString(SUBMOD_KEY,yylsp[-1]); )}
    break;

  case 137:
#line 641 "ebnf.y"
    {NEDC( set_checkconns(0); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","1");
                       setComments(CONNS_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 139:
#line 647 "ebnf.y"
    {NEDC( set_checkconns(1); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","0");
                       setComments(CONNS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 147:
#line 671 "ebnf.y"
    {NEDC( do_for (); )
                 GNED( FORLOOP_KEY = np->create("forloop",CONNS_KEY);
                       in_loop=1; setComments(FORLOOP_KEY,yylsp[0]); )}
    break;

  case 148:
#line 675 "ebnf.y"
    {NEDC( end_for (); )
                 GNED( in_loop=0; setTrailingComment(FORLOOP_KEY,yylsp[0]); )}
    break;

  case 151:
#line 686 "ebnf.y"
    {NEDC( do_index (yyvsp[-4], yyvsp[-2], yyvsp[0]); )
                 GNED( LOOPVAR_KEY=addLoopVar(FORLOOP_KEY,yylsp[-4],yylsp[-2],yylsp[0]);
                       setComments(LOOPVAR_KEY,yylsp[-4],yylsp[0]); )}
    break;

  case 152:
#line 693 "ebnf.y"
    {NEDC( do_condition(yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"condition",yylsp[0]); )}
    break;

  case 153:
#line 696 "ebnf.y"
    {NEDC( do_condition(NULL); )}
    break;

  case 154:
#line 701 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( setDisplayString(CONN_KEY,yylsp[0]); )}
    break;

  case 155:
#line 704 "ebnf.y"
    {NEDC( yyval = NULL; )}
    break;

  case 158:
#line 714 "ebnf.y"
    {NEDC( end_connection (NULL, 'R', yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,yylsp[-4],yylsp[0]); )}
    break;

  case 159:
#line 717 "ebnf.y"
    {NEDC( end_connection (yyvsp[-4], 'R', yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,yylsp[-6],yylsp[0]); )}
    break;

  case 160:
#line 720 "ebnf.y"
    {NEDC( end_connection (NULL, 'L', yyvsp[0]); )
                 GNED( swapConnection(CONN_KEY);
                       np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,yylsp[-4],yylsp[0]); )}
    break;

  case 161:
#line 724 "ebnf.y"
    {NEDC( end_connection (yyvsp[-4], 'L', yyvsp[0]); )
                 GNED( swapConnection(CONN_KEY);
                 np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,yylsp[-6],yylsp[0]); )}
    break;

  case 164:
#line 736 "ebnf.y"
    {NEDC( do_mod_L (yyvsp[-1], yyvsp[0]); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey(yylsp[-1]));
                       np->set(CONN_KEY, "src-mod-index", trimBrackets(yylsp[0])); )}
    break;

  case 165:
#line 741 "ebnf.y"
    {NEDC( do_mod_L (yyvsp[0], NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey(yylsp[0])); )}
    break;

  case 166:
#line 748 "ebnf.y"
    {NEDC( do_gate_L (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "srcgate", yylsp[-1]);
                       np->set(CONN_KEY, "src-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 167:
#line 752 "ebnf.y"
    {NEDC( do_gate_L (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "srcgate", yylsp[0]); )}
    break;

  case 168:
#line 758 "ebnf.y"
    {NEDC( do_mod_L (NULL, NULL); do_gate_L (yyvsp[-1], yyvsp[0]); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", yylsp[-1]);
                       np->set(CONN_KEY, "src-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 169:
#line 764 "ebnf.y"
    {NEDC( do_mod_L (NULL, NULL); do_gate_L (yyvsp[0], NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", yylsp[0]); )}
    break;

  case 172:
#line 777 "ebnf.y"
    {NEDC( do_mod_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey(yylsp[-1]));
                       np->set(CONN_KEY, "dest-mod-index", trimBrackets(yylsp[0])); )}
    break;

  case 173:
#line 781 "ebnf.y"
    {NEDC( do_mod_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey(yylsp[0])); )}
    break;

  case 174:
#line 787 "ebnf.y"
    {NEDC( do_gate_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "destgate", yylsp[-1]);
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 175:
#line 791 "ebnf.y"
    {NEDC( do_gate_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "destgate", yylsp[0]); )}
    break;

  case 176:
#line 797 "ebnf.y"
    {NEDC( do_mod_R (NULL, NULL); do_gate_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", yylsp[-1]);
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 177:
#line 802 "ebnf.y"
    {NEDC( do_mod_R (NULL, NULL); do_gate_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", yylsp[0]); )}
    break;

  case 178:
#line 810 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, NULL, NULL); )}
    break;

  case 179:
#line 812 "ebnf.y"
    {NEDC( yyval = do_channeldescr(yyvsp[0], NULL,NULL,NULL); )}
    break;

  case 180:
#line 814 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], NULL, NULL); )}
    break;

  case 181:
#line 816 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[0], NULL); )}
    break;

  case 182:
#line 818 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, NULL, yyvsp[0]); )}
    break;

  case 183:
#line 821 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[0], NULL); )}
    break;

  case 184:
#line 823 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], NULL, yyvsp[0]); )}
    break;

  case 185:
#line 825 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[-1], yyvsp[0]); )}
    break;

  case 186:
#line 827 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-1], NULL); )}
    break;

  case 187:
#line 829 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], NULL, yyvsp[-1]); )}
    break;

  case 188:
#line 831 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[0], yyvsp[-1]); )}
    break;

  case 189:
#line 834 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-2], yyvsp[-1], yyvsp[0]); )}
    break;

  case 190:
#line 836 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-2], yyvsp[0], yyvsp[-1]); )}
    break;

  case 191:
#line 838 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[-2], yyvsp[0]); )}
    break;

  case 192:
#line 840 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-2], yyvsp[-1]); )}
    break;

  case 193:
#line 842 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[0], yyvsp[-2]); )}
    break;

  case 194:
#line 844 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-1], yyvsp[-2]); )}
    break;

  case 195:
#line 849 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"channel",yylsp[0]); )}
    break;

  case 196:
#line 855 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"delay",yylsp[0]); )}
    break;

  case 197:
#line 861 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"error",yylsp[0]); )}
    break;

  case 198:
#line 867 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"datarate",yylsp[0]); )}
    break;

  case 199:
#line 873 "ebnf.y"
    {NEDC( do_system (yyvsp[-3]); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,yylsp[-3],yylsp[-1],NULLPOS);
                       setComments(NETWORK_KEY,yylsp[-4],yylsp[0]); in_network=1;)}
    break;

  case 200:
#line 876 "ebnf.y"
    {NEDC( do_systemmodule (yyvsp[-4], yyvsp[-2], NULL); )}
    break;

  case 201:
#line 878 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 202:
#line 880 "ebnf.y"
    {NEDC( end_system (); )}
    break;

  case 203:
#line 882 "ebnf.y"
    {NEDC( do_system (yyvsp[-5]); )}
    break;

  case 204:
#line 883 "ebnf.y"
    {NEDC( do_systemmodule (yyvsp[-6], yyvsp[-4], yyvsp[-2]); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,yylsp[-6],yylsp[-4],yylsp[-2]);
                       setComments(NETWORK_KEY,yylsp[-7],yylsp[-1]); )}
    break;

  case 205:
#line 887 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 206:
#line 889 "ebnf.y"
    {NEDC( end_system (); )}
    break;

  case 207:
#line 894 "ebnf.y"
    {GNED( setTrailingComment(NETWORK_KEY,yylsp[-1]); in_network=0; )}
    break;

  case 208:
#line 899 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )}
    break;

  case 211:
#line 909 "ebnf.y"
    {NEDC( yyval = do_inputvalue(yyvsp[-3], yyvsp[-1]); )}
    break;

  case 212:
#line 911 "ebnf.y"
    {NEDC( yyval = do_inputvalue(yyvsp[-1], NULL); )}
    break;

  case 213:
#line 913 "ebnf.y"
    {NEDC( yyval = do_inputvalue(NULL, NULL); )}
    break;

  case 214:
#line 915 "ebnf.y"
    {NEDC( yyval = do_inputvalue(NULL, NULL); )}
    break;

  case 215:
#line 921 "ebnf.y"
    {NEDC( yyval = end_expr( yyvsp[0] ); )}
    break;

  case 216:
#line 926 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )}
    break;

  case 217:
#line 928 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )}
    break;

  case 218:
#line 931 "ebnf.y"
    {NEDC( yyval = do_op( 2, '+', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 219:
#line 933 "ebnf.y"
    {NEDC( yyval = do_op( 2, '-', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 220:
#line 935 "ebnf.y"
    {NEDC( yyval = do_op( 2, '*', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 221:
#line 937 "ebnf.y"
    {NEDC( yyval = do_op( 2, '/', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 222:
#line 939 "ebnf.y"
    {NEDC( yyval = do_op( 2, '%', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 223:
#line 941 "ebnf.y"
    {NEDC( yyval = do_op( 2, '^', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 224:
#line 945 "ebnf.y"
    {NEDC( yyval = do_addminus(yyvsp[0]); )}
    break;

  case 225:
#line 948 "ebnf.y"
    {NEDC( yyval = do_op( 2, '=', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 226:
#line 950 "ebnf.y"
    {NEDC( yyval = do_op( 2, '!', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 227:
#line 952 "ebnf.y"
    {NEDC( yyval = do_op( 2, '>', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 228:
#line 954 "ebnf.y"
    {NEDC( yyval = do_op( 2, '}', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 229:
#line 956 "ebnf.y"
    {NEDC( yyval = do_op( 2, '<', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 230:
#line 958 "ebnf.y"
    {NEDC( yyval = do_op( 2, '{', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 231:
#line 961 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bool_and"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 232:
#line 963 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bool_or"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 233:
#line 965 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bool_xor"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 234:
#line 968 "ebnf.y"
    {NEDC( yyval = do_func(1,jar_strdup("bool_not"), yyvsp[0],NULL,NULL,NULL); )}
    break;

  case 235:
#line 971 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bin_and"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 236:
#line 973 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bin_or"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 237:
#line 975 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bin_xor"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 238:
#line 978 "ebnf.y"
    {NEDC( yyval = do_func(1,jar_strdup("bin_compl"), yyvsp[0],NULL,NULL,NULL); )}
    break;

  case 239:
#line 980 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("shift_left"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 240:
#line 982 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("shift_right"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 241:
#line 984 "ebnf.y"
    {NEDC( yyval = do_op( 3, '?', yyvsp[-4],yyvsp[-2],yyvsp[0]); )}
    break;

  case 242:
#line 986 "ebnf.y"
    {NEDC( yyval = do_func(0,yyvsp[-2], NULL,NULL,NULL,NULL); )}
    break;

  case 243:
#line 988 "ebnf.y"
    {NEDC( yyval = do_func(1,yyvsp[-3], yyvsp[-1],NULL,NULL,NULL); )}
    break;

  case 244:
#line 990 "ebnf.y"
    {NEDC( yyval = do_func(2,yyvsp[-5], yyvsp[-3],yyvsp[-1],NULL,NULL); )}
    break;

  case 245:
#line 992 "ebnf.y"
    {NEDC( yyval = do_func(3,yyvsp[-7], yyvsp[-5],yyvsp[-3],yyvsp[-1],NULL); )}
    break;

  case 246:
#line 994 "ebnf.y"
    {NEDC( yyval = do_func(4,yyvsp[-9], yyvsp[-7],yyvsp[-5],yyvsp[-3],yyvsp[-1]); )}
    break;

  case 252:
#line 1007 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],0,1); )}
    break;

  case 253:
#line 1009 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],0,0); )}
    break;

  case 254:
#line 1011 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],1,1); )}
    break;

  case 255:
#line 1013 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],1,0); )}
    break;

  case 256:
#line 1015 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],1,0); )}
    break;

  case 257:
#line 1018 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-2],0,yyvsp[0],0,1); )}
    break;

  case 258:
#line 1020 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-2],0,yyvsp[0],0,0); )}
    break;

  case 259:
#line 1023 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-3],yyvsp[-2],yyvsp[0],0,1); )}
    break;

  case 260:
#line 1025 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-3],yyvsp[-2],yyvsp[0],0,0); )}
    break;

  case 261:
#line 1030 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_STRING,yyvsp[0]); )}
    break;

  case 262:
#line 1035 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_BOOL,jar_strdup("true")); )}
    break;

  case 263:
#line 1037 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_BOOL,jar_strdup("false")); )}
    break;

  case 264:
#line 1042 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_NUMERIC,yyvsp[0]); )}
    break;

  case 265:
#line 1047 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_NUMERIC,jar_strdup("(long)sub_i")); )}
    break;

  case 266:
#line 1049 "ebnf.y"
    {NEDC( yyval = do_sizeof (yyvsp[-1]); )}
    break;

  case 270:
#line 1060 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-2],yyvsp[-1],yyvsp[0]); )}
    break;

  case 271:
#line 1062 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-2],yyvsp[-1],yyvsp[0]); )}
    break;

  case 272:
#line 1064 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-1],yyvsp[0],NULL); )}
    break;

  case 273:
#line 1066 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-1],yyvsp[0],NULL); )}
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 2768 "ebnf.tab.c"

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


#line 1073 "ebnf.y"


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




