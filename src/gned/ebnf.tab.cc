/* A Bison parser, made by GNU Bison 1.875.  */

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
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

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
     PLUSPLUS = 313,
     EQ = 314,
     NE = 315,
     GT = 316,
     GE = 317,
     LS = 318,
     LE = 319,
     AND = 320,
     OR = 321,
     XOR = 322,
     NOT = 323,
     BIN_AND = 324,
     BIN_OR = 325,
     BIN_XOR = 326,
     BIN_COMPL = 327,
     SHIFT_LEFT = 328,
     SHIFT_RIGHT = 329,
     INVALID_CHAR = 330,
     UMIN = 331
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
#define PLUSPLUS 313
#define EQ 314
#define NE 315
#define GT 316
#define GE 317
#define LS 318
#define LE 319
#define AND 320
#define OR 321
#define XOR 322
#define NOT 323
#define BIN_AND 324
#define BIN_OR 325
#define BIN_XOR 326
#define BIN_COMPL 327
#define SHIFT_LEFT 328
#define SHIFT_RIGHT 329
#define INVALID_CHAR 330
#define UMIN 331




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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined (YYLTYPE) && ! defined (YYLTYPE_IS_DECLARED)
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 383 "ebnf.tab.c"

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
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAXIMUM)

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
	    (To)[yyi] = (From)[yyi];		\
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
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
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
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   664

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  87
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  136
/* YYNRULES -- Number of rules. */
#define YYNRULES  304
/* YYNRULES -- Number of states. */
#define YYNSTATES  510

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   331

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      85,    86,     2,     2,    80,     2,    84,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    77,    79,
       2,    83,     2,    76,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    81,     2,    82,     2,     2,     2,     2,     2,     2,
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
      75,    78
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
     607,   610,   613,   615,   618,   622,   624,   627,   629,   632,
     634,   637,   640,   642,   645,   646,   648,   650,   652,   654,
     657,   660,   663,   666,   669,   672,   676,   680,   684,   688,
     692,   696,   698,   701,   704,   707,   708,   709,   710,   722,
     723,   724,   725,   739,   742,   746,   748,   750,   757,   762,
     766,   768,   770,   772,   776,   780,   784,   788,   792,   796,
     800,   803,   807,   811,   815,   819,   823,   827,   831,   835,
     839,   842,   846,   850,   854,   857,   861,   865,   871,   875,
     880,   887,   896,   907,   909,   911,   913,   915,   917,   919,
     922,   925,   929,   933,   937,   942,   947,   953,   955,   957,
     959,   961,   963,   968,   970,   972,   974,   978,   982,   985,
     988,   990,   991,   993,   995
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
      88,     0,    -1,    89,    -1,    -1,    89,    90,    -1,    90,
      -1,    91,    -1,    95,    -1,   103,    -1,   106,    -1,   200,
      -1,    -1,     3,    92,    93,    79,    -1,    93,    80,    94,
      -1,    94,    -1,    39,    -1,    96,    97,    -1,    96,   101,
      97,    -1,    96,   100,    97,    -1,    96,   102,    97,    -1,
      96,   101,   100,    97,    -1,    96,   101,   102,    97,    -1,
      96,   100,   102,    97,    -1,    96,   100,   101,    97,    -1,
      96,   102,   101,    97,    -1,    96,   102,   100,    97,    -1,
      96,   101,   100,   102,    97,    -1,    96,   101,   102,   100,
      97,    -1,    96,   100,   101,   102,    97,    -1,    96,   100,
     102,   101,    97,    -1,    96,   102,   101,   100,    97,    -1,
      96,   102,   100,   101,    97,    -1,     5,    38,    -1,    -1,
      29,    38,   221,    98,    -1,    -1,    29,   221,    99,    -1,
       7,   211,   221,    -1,     6,   211,   221,    -1,     8,   211,
     221,    -1,   104,   109,   111,   112,   105,    -1,     4,    38,
     221,    -1,    27,    38,   221,    -1,    27,   221,    -1,   107,
     109,   111,   112,   113,   114,   110,   108,    -1,     9,    38,
     221,    -1,    28,    38,   221,    -1,    28,   221,    -1,   115,
      -1,    -1,   120,    -1,    -1,   121,    -1,    -1,   126,    -1,
      -1,   134,    -1,    -1,   173,    -1,    -1,    -1,    32,    77,
     116,   117,    -1,   118,    79,    -1,    -1,   118,    80,   119,
      -1,   119,    -1,    38,    -1,    15,    77,    39,    79,    -1,
      -1,    10,    77,   122,   123,    -1,   124,    79,    -1,    -1,
     124,    80,   125,    -1,   125,    -1,    38,    -1,    38,    77,
      46,    -1,    45,    38,    -1,    38,    77,    45,    -1,    38,
      77,    45,    46,    -1,    38,    77,    46,    45,    -1,    38,
      77,    47,    -1,    38,    77,    48,    -1,    38,    77,    49,
      -1,    -1,    11,    77,   127,   128,    -1,   129,    -1,    -1,
     129,    16,   130,    79,    -1,    16,   130,    79,    -1,   129,
      17,   132,    79,    -1,    17,   132,    79,    -1,   130,    80,
     131,    -1,   131,    -1,    38,    81,    82,    -1,    38,    -1,
     132,    80,   133,    -1,   133,    -1,    38,    81,    82,    -1,
      38,    -1,    -1,    13,    77,   135,   136,    -1,   137,    -1,
      -1,   137,   138,    -1,   138,    -1,    -1,    -1,    38,    77,
      38,   221,   139,   149,   140,   147,    -1,    -1,    -1,    38,
      77,    38,   208,   221,   141,   149,   142,   147,    -1,    -1,
      -1,    38,    77,    38,    25,    38,   221,   143,   149,   144,
     147,    -1,    -1,    -1,    38,    77,    38,   208,    25,    38,
     221,   145,   149,   146,   147,    -1,    -1,   157,   148,   165,
     172,    -1,   150,    -1,    -1,   150,   151,    -1,   151,    -1,
      -1,    33,    77,   152,   154,    -1,    -1,    33,    24,   211,
      77,   153,   154,    -1,   155,    79,    -1,    -1,   155,    80,
     156,    -1,   156,    -1,    38,    -1,   158,    -1,    -1,   158,
     159,    -1,   159,    -1,    -1,    10,    77,   160,   162,    -1,
      -1,    10,    24,   211,    77,   161,   162,    -1,   163,    79,
      -1,    -1,   163,    80,   164,    -1,   164,    -1,    38,    83,
     209,    -1,   166,   165,    -1,    -1,    -1,    12,    77,   167,
     169,    -1,    -1,    12,    24,   211,    77,   168,   169,    -1,
     170,    79,    -1,    -1,   170,    80,   171,    -1,   171,    -1,
      38,   208,    -1,    38,    -1,    15,    77,    39,    79,    -1,
      -1,    -1,    14,    18,    77,   174,   176,    -1,    -1,    14,
      77,   175,   176,    -1,   177,    79,    -1,    -1,   177,   222,
     178,    -1,   178,    -1,   179,    -1,   186,    -1,    -1,    21,
     180,   181,    23,   185,    31,    -1,   181,    80,   182,    -1,
     182,    -1,    38,    83,   211,    22,   211,    -1,    24,   211,
      -1,    -1,    15,    39,    -1,    -1,   185,   186,   222,    -1,
     186,   222,    -1,   187,    20,   191,   183,   184,    -1,   187,
      20,   195,    20,   191,   183,   184,    -1,   187,    19,   191,
     183,   184,    -1,   187,    19,   195,    19,   191,   183,   184,
      -1,   188,    84,   189,    -1,   190,    -1,    38,   208,    -1,
      38,    -1,    38,   208,    -1,    38,    -1,    38,    58,    -1,
      38,   208,    -1,    38,    -1,    38,    58,    -1,   192,    84,
     193,    -1,   194,    -1,    38,   208,    -1,    38,    -1,    38,
     208,    -1,    38,    -1,    38,    58,    -1,    38,   208,    -1,
      38,    -1,    38,    58,    -1,    -1,   196,    -1,   197,    -1,
     198,    -1,   199,    -1,   197,   198,    -1,   197,   199,    -1,
     198,   199,    -1,   198,   197,    -1,   199,   197,    -1,   199,
     198,    -1,   197,   198,   199,    -1,   197,   199,   198,    -1,
     198,   197,   199,    -1,   198,   199,   197,    -1,   199,   197,
     198,    -1,   199,   198,   197,    -1,    38,    -1,     6,   211,
      -1,     7,   211,    -1,     8,   211,    -1,    -1,    -1,    -1,
      26,    38,    77,    38,   221,   201,   149,   202,   157,   203,
     207,    -1,    -1,    -1,    -1,    26,    38,    77,    38,    25,
      38,   221,   204,   149,   205,   157,   206,   207,    -1,    30,
     221,    -1,    81,   211,    82,    -1,   210,    -1,   211,    -1,
      42,    85,   211,    80,   211,    86,    -1,    42,    85,   211,
      86,    -1,    42,    85,    86,    -1,    42,    -1,   212,    -1,
     213,    -1,    85,   212,    86,    -1,   212,    50,   212,    -1,
     212,    51,   212,    -1,   212,    52,   212,    -1,   212,    53,
     212,    -1,   212,    54,   212,    -1,   212,    55,   212,    -1,
      51,   212,    -1,   212,    59,   212,    -1,   212,    60,   212,
      -1,   212,    61,   212,    -1,   212,    62,   212,    -1,   212,
      63,   212,    -1,   212,    64,   212,    -1,   212,    65,   212,
      -1,   212,    66,   212,    -1,   212,    67,   212,    -1,    68,
     212,    -1,   212,    69,   212,    -1,   212,    70,   212,    -1,
     212,    71,   212,    -1,    72,   212,    -1,   212,    73,   212,
      -1,   212,    74,   212,    -1,   212,    76,   212,    77,   212,
      -1,    38,    85,    86,    -1,    38,    85,   212,    86,    -1,
      38,    85,   212,    80,   212,    86,    -1,    38,    85,   212,
      80,   212,    80,   212,    86,    -1,    38,    85,   212,    80,
     212,    80,   212,    80,   212,    86,    -1,   214,    -1,   215,
      -1,   216,    -1,   217,    -1,   218,    -1,    38,    -1,    43,
      38,    -1,    44,    38,    -1,    43,    44,    38,    -1,    44,
      43,    38,    -1,    38,    84,    38,    -1,    43,    38,    84,
      38,    -1,    38,   208,    84,    38,    -1,    43,    38,   208,
      84,    38,    -1,    39,    -1,    40,    -1,    41,    -1,   219,
      -1,    57,    -1,    56,    85,    38,    86,    -1,    36,    -1,
      37,    -1,   220,    -1,    36,    38,   220,    -1,    37,    38,
     220,    -1,    36,    38,    -1,    37,    38,    -1,    79,    -1,
      -1,    80,    -1,    79,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   211,   211,   212,   216,   217,   221,   222,   223,   224,
     225,   230,   229,   238,   239,   243,   252,   255,   257,   259,
     262,   264,   266,   268,   270,   272,   275,   277,   279,   281,
     283,   285,   290,   300,   299,   303,   302,   309,   316,   323,
     330,   338,   347,   350,   356,   367,   375,   378,   384,   386,
     388,   388,   389,   389,   390,   390,   391,   391,   392,   392,
     396,   395,   402,   403,   407,   408,   412,   420,   427,   426,
     434,   435,   439,   442,   447,   450,   453,   456,   459,   462,
     465,   468,   471,   478,   477,   485,   486,   490,   491,   492,
     493,   497,   498,   502,   506,   513,   514,   518,   522,   530,
     529,   537,   538,   542,   543,   548,   552,   547,   555,   559,
     554,   562,   566,   561,   569,   573,   568,   579,   578,   586,
     588,   592,   593,   598,   597,   603,   602,   612,   614,   618,
     619,   623,   630,   631,   635,   636,   641,   640,   646,   645,
     655,   656,   660,   661,   665,   672,   673,   678,   677,   683,
     682,   692,   693,   697,   698,   702,   706,   713,   716,   721,
     720,   727,   726,   735,   736,   740,   741,   745,   746,   751,
     750,   760,   761,   765,   772,   776,   780,   784,   788,   789,
     793,   796,   799,   803,   810,   811,   815,   820,   827,   831,
     834,   841,   847,   852,   861,   862,   866,   870,   876,   880,
     883,   890,   895,   899,   909,   910,   912,   914,   916,   919,
     921,   923,   925,   927,   929,   932,   934,   936,   938,   940,
     942,   947,   953,   959,   965,   972,   976,   978,   971,   982,
     984,   988,   981,   994,   999,  1004,  1005,  1009,  1011,  1013,
    1015,  1021,  1026,  1028,  1031,  1033,  1035,  1037,  1039,  1041,
    1044,  1048,  1050,  1052,  1054,  1056,  1058,  1061,  1063,  1065,
    1067,  1071,  1073,  1075,  1077,  1080,  1082,  1084,  1086,  1088,
    1090,  1092,  1094,  1099,  1100,  1101,  1102,  1103,  1107,  1109,
    1111,  1113,  1115,  1118,  1120,  1123,  1125,  1130,  1135,  1137,
    1142,  1147,  1149,  1154,  1155,  1156,  1160,  1162,  1164,  1166,
    1171,  1171,  1172,  1172,  1172
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
  "SUBMODINDEX", "PLUSPLUS", "EQ", "NE", "GT", "GE", "LS", "LE", "AND", 
  "OR", "XOR", "NOT", "BIN_AND", "BIN_OR", "BIN_XOR", "BIN_COMPL", 
  "SHIFT_LEFT", "SHIFT_RIGHT", "INVALID_CHAR", "'?'", "':'", "UMIN", 
  "';'", "','", "'['", "']'", "'='", "'.'", "'('", "')'", "$accept", 
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
     325,   326,   327,   328,   329,   330,    63,    58,   331,    59,
      44,    91,    93,    61,    46,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    87,    88,    88,    89,    89,    90,    90,    90,    90,
      90,    92,    91,    93,    93,    94,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    96,    98,    97,    99,    97,   100,   101,   102,
     103,   104,   105,   105,   106,   107,   108,   108,   109,   109,
     110,   110,   111,   111,   112,   112,   113,   113,   114,   114,
     116,   115,   117,   117,   118,   118,   119,   120,   122,   121,
     123,   123,   124,   124,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   127,   126,   128,   128,   129,   129,   129,
     129,   130,   130,   131,   131,   132,   132,   133,   133,   135,
     134,   136,   136,   137,   137,   139,   140,   138,   141,   142,
     138,   143,   144,   138,   145,   146,   138,   148,   147,   149,
     149,   150,   150,   152,   151,   153,   151,   154,   154,   155,
     155,   156,   157,   157,   158,   158,   160,   159,   161,   159,
     162,   162,   163,   163,   164,   165,   165,   167,   166,   168,
     166,   169,   169,   170,   170,   171,   171,   172,   172,   174,
     173,   175,   173,   176,   176,   177,   177,   178,   178,   180,
     179,   181,   181,   182,   183,   183,   184,   184,   185,   185,
     186,   186,   186,   186,   187,   187,   188,   188,   189,   189,
     189,   190,   190,   190,   191,   191,   192,   192,   193,   193,
     193,   194,   194,   194,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   196,   197,   198,   199,   201,   202,   203,   200,   204,
     205,   206,   200,   207,   208,   209,   209,   210,   210,   210,
     210,   211,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   213,   213,   213,   213,   213,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   215,   216,   216,
     217,   218,   218,   219,   219,   219,   220,   220,   220,   220,
     221,   221,   222,   222,   222
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
       2,     2,     1,     2,     3,     1,     2,     1,     2,     1,
       2,     2,     1,     2,     0,     1,     1,     1,     1,     2,
       2,     2,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     1,     2,     2,     2,     0,     0,     0,    11,     0,
       0,     0,    13,     2,     3,     1,     1,     6,     4,     3,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     3,     3,     2,     3,     3,     5,     3,     4,
       6,     8,    10,     1,     1,     1,     1,     1,     1,     2,
       2,     3,     3,     3,     4,     4,     5,     1,     1,     1,
       1,     1,     4,     1,     1,     1,     3,     3,     2,     2,
       1,     0,     1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       3,    11,     0,     0,     0,     0,     0,     2,     5,     6,
       7,     0,     8,    49,     9,    49,    10,     0,   301,    32,
     301,     0,     1,     4,     0,     0,     0,   301,    16,     0,
       0,     0,     0,    53,    48,    53,    15,     0,    14,   300,
      41,    45,     0,   293,   294,   278,   287,   288,   289,     0,
       0,     0,     0,   291,     0,     0,     0,   301,   241,   242,
     273,   274,   275,   276,   277,   290,   295,   301,   301,   301,
      35,    18,     0,     0,    17,     0,     0,    19,     0,     0,
      60,     0,    55,    52,    55,    12,     0,   301,   298,   299,
       0,     0,     0,     0,   279,     0,   280,     0,   250,     0,
     260,   264,     0,    38,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    37,    39,    33,    36,    23,
       0,    22,     0,    20,     0,    21,     0,    25,     0,    24,
       0,    63,    68,     0,     0,    54,    57,    13,     0,   225,
       0,     0,   296,   297,     0,   283,   268,     0,     0,     0,
       0,   281,   282,     0,   243,   244,   245,   246,   247,   248,
     249,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     261,   262,   263,   265,   266,     0,    34,    28,    29,    26,
      27,    31,    30,    66,    61,     0,    65,    71,    83,   301,
      40,     0,    59,    56,   301,   120,   234,     0,   269,   285,
     284,     0,   292,     0,    62,     0,    74,     0,    69,     0,
      73,    86,   301,    43,    99,     0,    51,    58,   229,     0,
     226,   119,   122,     0,   286,   267,    64,     0,    76,    70,
       0,     0,     0,    84,    85,    42,   102,     0,   161,     0,
       0,    50,   120,     0,   123,   133,   121,     0,   270,    77,
      75,    80,    81,    82,    72,    94,     0,    92,    98,     0,
      96,     0,     0,     0,   100,   101,   104,   159,   164,     0,
     301,    44,   230,     0,   128,     0,   227,   132,   135,     0,
      78,    79,     0,    88,     0,     0,    90,     0,     0,     0,
       0,   103,   164,   169,   192,   162,   304,   166,   167,   168,
       0,     0,   185,     0,   301,    47,   133,   125,   131,   124,
       0,   130,     0,   136,     0,   134,     0,   271,    93,    91,
      97,    95,    87,    89,   301,   160,     0,   193,   191,   163,
     302,     0,   204,   204,     0,    67,    46,   231,   128,   127,
       0,     0,   141,   301,   228,     0,     0,   301,   105,     0,
       0,   172,   165,     0,     0,     0,   202,   175,     0,   195,
       0,   205,   206,   207,   208,   175,     0,   189,   184,     0,
     126,   129,   138,     0,   137,     0,   143,   233,   272,   301,
       0,   108,   120,     0,     0,     0,   222,   223,   224,   203,
     201,     0,   177,     0,     0,   209,   210,   212,   211,   213,
     214,   177,     0,   190,   188,   232,   141,     0,   140,     0,
     111,   301,   120,   106,     0,     0,   304,   171,   174,     0,
     182,   199,   194,   202,   175,   215,   216,   217,   218,   219,
     220,   180,   175,   139,   240,   144,   235,   236,   142,   120,
     114,   109,   133,     0,   170,   304,   303,   179,   176,   200,
     198,   177,   177,     0,   112,   120,   133,   107,   117,   173,
     178,   183,   181,   239,     0,   133,   115,   110,   146,     0,
     238,   113,   133,     0,   158,   146,     0,   116,     0,   147,
       0,   118,   145,   237,     0,   152,     0,   149,   156,   148,
       0,   154,     0,   152,   155,   151,     0,   157,   150,   153
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     6,     7,     8,     9,    17,    37,    38,    10,    11,
      28,   186,   128,    29,    30,    31,    12,    13,   200,    14,
      15,   281,    33,   250,    82,   144,   202,   226,    34,   141,
     194,   195,   196,   251,    83,   197,   218,   219,   220,   145,
     221,   243,   244,   266,   267,   269,   270,   203,   246,   274,
     275,   276,   392,   452,   422,   466,   449,   475,   465,   482,
     467,   478,   230,   231,   232,   284,   348,   319,   320,   321,
     468,   287,   288,   352,   416,   384,   385,   386,   484,   485,
     495,   503,   499,   500,   501,   491,   227,   302,   278,   305,
     306,   307,   308,   336,   360,   361,   402,   430,   425,   309,
     310,   311,   378,   312,   367,   368,   432,   369,   370,   371,
     372,   373,   374,    16,   205,   255,   324,   252,   316,   379,
     354,   400,   445,   446,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    40,   341
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -410
static const short yypact[] =
{
      28,  -410,    -4,     0,    11,    80,   189,    28,  -410,  -410,
    -410,   234,  -410,   191,  -410,   191,  -410,   174,   148,  -410,
     148,   154,  -410,  -410,   181,   181,   181,   -32,  -410,   113,
     238,    15,   185,   241,  -410,   241,  -410,   196,  -410,  -410,
    -410,  -410,   226,   239,   249,   247,  -410,  -410,  -410,   -14,
     169,   181,   209,  -410,   181,   181,   181,   148,   490,  -410,
    -410,  -410,  -410,  -410,  -410,  -410,  -410,   148,   148,   148,
    -410,  -410,    12,    19,  -410,    12,     7,  -410,    19,     7,
    -410,   237,   285,  -410,   285,  -410,   174,    -7,   264,   264,
     181,   295,    96,   253,    91,   306,  -410,   331,  -410,   340,
    -410,  -410,   406,  -410,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,  -410,  -410,  -410,  -410,  -410,
     292,  -410,   292,  -410,   292,  -410,   292,  -410,   292,  -410,
     292,   357,  -410,   322,   378,  -410,   405,  -410,   394,  -410,
     239,   249,  -410,  -410,   355,  -410,  -410,   219,   397,   401,
     359,  -410,  -410,   358,  -410,   300,   300,   392,   392,   392,
     392,   542,   542,   542,   542,   542,   542,   256,   256,   256,
     548,   548,   548,   296,   296,   462,  -410,  -410,  -410,  -410,
    -410,  -410,  -410,  -410,  -410,   224,  -410,   132,  -410,   -27,
    -410,   368,   435,  -410,   148,   415,  -410,   181,  -410,  -410,
    -410,   412,  -410,   181,  -410,   357,   374,   414,  -410,   233,
    -410,   307,   148,  -410,  -410,    -8,   438,  -410,  -410,    26,
    -410,   415,  -410,   320,  -410,   517,  -410,   211,  -410,  -410,
     132,   424,   426,  -410,   319,  -410,   436,   404,  -410,   413,
     450,  -410,   415,   181,  -410,   473,  -410,   181,  -410,   445,
     457,  -410,  -410,  -410,  -410,   425,   263,  -410,   428,   277,
    -410,   424,   426,   441,  -410,   436,  -410,  -410,   133,   472,
     -26,  -410,  -410,   442,   492,    43,  -410,   473,  -410,   360,
    -410,  -410,   452,  -410,   424,   455,  -410,   426,   279,   281,
     508,  -410,   133,  -410,   -42,  -410,   283,  -410,  -410,  -410,
     345,   463,  -410,   469,   148,  -410,   473,  -410,  -410,  -410,
     287,  -410,   181,  -410,   528,  -410,   181,  -410,  -410,  -410,
    -410,  -410,  -410,  -410,    18,  -410,   524,  -410,   481,   145,
    -410,   133,   227,   227,   535,  -410,  -410,  -410,   492,  -410,
     492,   497,   537,   148,  -410,   434,   547,    -6,  -410,   506,
     -10,  -410,  -410,   181,   181,   181,    42,   580,   521,  -410,
     587,  -410,   390,   246,   395,   580,   588,    67,  -410,   528,
    -410,  -410,  -410,   526,  -410,   324,  -410,  -410,  -410,   148,
     569,  -410,   415,   181,   572,   524,  -410,  -410,  -410,  -410,
     530,   181,   602,   581,   582,   610,   616,   610,   618,   616,
     618,   602,   582,  -410,  -410,  -410,   537,    73,  -410,   537,
    -410,   148,   415,  -410,   603,   217,   328,  -410,  -410,   589,
    -410,    97,  -410,   122,   580,  -410,  -410,  -410,  -410,  -410,
    -410,  -410,   580,  -410,   541,  -410,  -410,  -410,  -410,   415,
    -410,  -410,   473,   181,  -410,   328,  -410,  -410,  -410,  -410,
    -410,   602,   602,   158,  -410,   415,   473,  -410,  -410,  -410,
    -410,  -410,  -410,  -410,   -51,   473,  -410,  -410,   615,   181,
    -410,  -410,   473,    74,   614,   615,   544,  -410,   181,  -410,
     554,  -410,  -410,  -410,   555,   595,   596,  -410,   553,  -410,
     337,  -410,   557,   595,  -410,  -410,   595,  -410,  -410,  -410
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -410,  -410,  -410,   630,  -410,  -410,  -410,   552,  -410,  -410,
      29,  -410,  -410,   160,   115,   175,  -410,  -410,  -410,  -410,
    -410,  -410,   624,  -410,   605,   558,  -410,  -410,  -410,  -410,
    -410,  -410,   429,  -410,  -410,  -410,  -410,  -410,   403,  -410,
    -410,  -410,  -410,   370,   351,   375,   349,  -410,  -410,  -410,
    -410,   373,  -410,  -410,  -410,  -410,  -410,  -410,  -410,  -410,
    -344,  -410,  -249,  -410,   418,  -410,  -410,   302,  -410,   301,
    -248,  -410,   365,  -410,  -410,   240,  -410,   235,   168,  -410,
    -410,  -410,   152,  -410,   151,  -410,  -410,  -410,  -410,   356,
    -410,   318,  -410,  -410,  -410,   265,  -371,  -305,  -410,  -385,
    -410,  -410,  -410,  -410,  -338,  -410,  -410,  -410,   321,  -410,
    -224,  -198,  -245,  -410,  -410,  -410,  -410,  -410,  -410,  -410,
     282,   -43,  -410,  -410,   -25,   -28,  -410,  -410,  -410,  -410,
    -410,  -410,  -410,   353,   -12,  -409
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -304
static const short yytable[] =
{
      67,    68,    93,   282,   411,   375,    69,   286,    41,   426,
     247,   222,   314,   394,    25,    70,   337,   457,   148,   390,
      26,    24,    25,    98,    94,    24,   100,   101,   102,   479,
      95,     1,     2,     3,    18,   480,    27,     4,    19,    90,
     455,    27,  -187,   356,    27,   103,   470,    39,    27,    20,
     253,   160,    39,    39,     5,   125,   126,   127,    71,    74,
      77,  -221,  -221,   461,   157,   154,   434,   322,   347,   248,
     395,   462,    39,    39,   442,   149,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,    39,   488,    90,
     399,   129,   131,   254,   133,   135,   441,   137,   139,    43,
      44,    45,    46,    47,    48,   444,    49,    50,    21,    24,
     323,    26,   477,    90,    51,   413,  -197,   406,   408,    52,
      53,   481,    43,    44,    45,    46,    47,    48,   487,    49,
      50,    54,    27,   423,    72,    55,    79,    51,    90,   407,
     409,   489,    52,    53,   303,   459,   471,   472,    56,   187,
     435,   188,   437,   189,    54,   190,  -303,   191,    55,   192,
     216,   304,    90,   451,   405,   159,   410,   217,    90,   233,
     399,    56,   156,  -303,   438,   235,   440,   223,   132,    22,
      75,    78,   228,   138,    43,    44,    45,    46,    47,    48,
     464,    49,    50,    90,    73,    76,  -197,    96,   436,    51,
     245,   439,    97,    36,    52,    53,   476,    43,    44,    45,
      46,    47,    48,    32,    49,    50,    54,    39,   283,   289,
      55,    42,    51,   363,   364,   365,   136,    52,    53,   140,
      24,    25,    26,    56,   473,    25,    26,   130,   454,    54,
     134,    81,   363,    55,   365,   304,   259,   260,   261,   262,
     263,   338,    80,    27,    87,   366,    56,    27,   315,   104,
     105,   106,   107,   108,   109,    85,    86,    88,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    89,   119,   120,
     121,   357,   122,   123,    99,   124,   143,   351,   355,   207,
     150,   151,   346,   214,   215,   208,   104,   105,   106,   107,
     108,   109,   239,   240,   142,   110,   111,   112,   113,   114,
     115,    27,   358,   241,   242,   119,   120,   121,    90,   122,
     123,    91,    92,   155,   414,   271,   272,   158,   396,   397,
     398,   387,   293,   294,   161,   391,   104,   105,   106,   107,
     108,   109,   106,   107,   108,   109,   296,   297,   332,   294,
     333,   297,   339,   340,   342,   343,   349,   350,   424,   162,
     104,   105,   106,   107,   108,   109,   428,   420,   163,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   460,   119,
     120,   121,   447,   122,   123,   193,   124,   364,   365,   198,
     257,   363,   364,   418,   419,   199,   258,   456,   340,   450,
     104,   105,   106,   107,   108,   109,   505,   506,   201,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   469,   119,
     120,   121,   204,   122,   123,   209,   124,   206,   474,   210,
     326,   152,   153,   211,   212,   224,   327,   109,   229,   225,
     234,   237,   238,   249,   486,   504,   104,   105,   106,   107,
     108,   109,   265,   494,   268,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   273,   119,   120,   121,   280,   122,
     123,   277,   124,   285,   104,   105,   106,   107,   108,   109,
     279,   290,   164,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   291,   119,   120,   121,   292,   122,   123,   295,
     124,   313,   104,   105,   106,   107,   108,   109,   300,   317,
     388,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     318,   119,   120,   121,   328,   122,   123,   330,   124,   213,
     104,   105,   106,   107,   108,   109,   334,   344,   345,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   353,   119,
     120,   121,   359,   122,   123,  -186,   124,   104,   105,   106,
     107,   108,   109,   377,   382,   383,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   389,   119,   120,   121,   393,
     122,   123,   104,   105,   106,   107,   108,   109,   104,   105,
     106,   107,   108,   109,   401,   403,   404,   421,   412,   417,
     304,   119,   120,   121,  -196,   122,   123,   429,   365,   431,
     433,   122,   123,   364,   363,   453,   463,   483,   458,   490,
     493,   496,   497,   498,    90,   502,   507,    23,   147,    35,
      84,   298,   146,   264,   236,   329,   331,   299,   301,   256,
     380,   381,   325,   492,   448,   508,   443,   509,   335,   362,
     427,   415,     0,     0,   376
};

static const short yycheck[] =
{
      25,    26,    45,   252,   375,   343,    38,   255,    20,   394,
      18,    38,    38,    23,     7,    27,    58,   426,    25,    25,
       8,     6,     7,    51,    38,     6,    54,    55,    56,    80,
      44,     3,     4,     5,    38,    86,    29,     9,    38,    81,
     425,    29,    84,    25,    29,    57,   455,    79,    29,    38,
      24,    94,    79,    79,    26,    67,    68,    69,    29,    30,
      31,    19,    20,   434,    92,    90,   404,    24,   316,    77,
      80,   442,    79,    79,   412,    87,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,    79,    24,    81,
      58,    72,    73,    77,    75,    76,   411,    78,    79,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    38,     6,
      77,     8,   466,    81,    51,    58,    84,   372,   373,    56,
      57,   475,    36,    37,    38,    39,    40,    41,   482,    43,
      44,    68,    29,   392,    29,    72,    31,    51,    81,   373,
     374,    77,    56,    57,    21,    58,   461,   462,    85,   130,
     405,   132,   407,   134,    68,   136,    21,   138,    72,   140,
      38,    38,    81,   422,   372,    84,   374,    45,    81,   207,
      58,    85,    86,    38,   408,   213,   410,   199,    73,     0,
      30,    31,   204,    78,    36,    37,    38,    39,    40,    41,
     449,    43,    44,    81,    29,    30,    84,    38,   406,    51,
     222,   409,    43,    39,    56,    57,   465,    36,    37,    38,
      39,    40,    41,    32,    43,    44,    68,    79,   253,   257,
      72,    77,    51,     6,     7,     8,    76,    56,    57,    79,
       6,     7,     8,    85,    86,     7,     8,    72,    31,    68,
      75,    10,     6,    72,     8,    38,    45,    46,    47,    48,
      49,   304,    77,    29,    38,    38,    85,    29,   280,    50,
      51,    52,    53,    54,    55,    79,    80,    38,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    38,    69,    70,
      71,   334,    73,    74,    85,    76,    11,   322,   326,    80,
      36,    37,   314,    79,    80,    86,    50,    51,    52,    53,
      54,    55,    79,    80,    77,    59,    60,    61,    62,    63,
      64,    29,   334,    16,    17,    69,    70,    71,    81,    73,
      74,    84,    85,    38,   377,    16,    17,    84,   363,   364,
     365,   353,    79,    80,    38,   357,    50,    51,    52,    53,
      54,    55,    52,    53,    54,    55,    79,    80,    79,    80,
      79,    80,    79,    80,    19,    20,    79,    80,   393,    38,
      50,    51,    52,    53,    54,    55,   401,   389,    38,    59,
      60,    61,    62,    63,    64,    65,    66,    67,   431,    69,
      70,    71,   417,    73,    74,    38,    76,     7,     8,    77,
      80,     6,     7,    79,    80,    27,    86,    79,    80,   421,
      50,    51,    52,    53,    54,    55,    79,    80,    13,    59,
      60,    61,    62,    63,    64,    65,    66,    67,   453,    69,
      70,    71,    38,    73,    74,    38,    76,    82,   463,    38,
      80,    88,    89,    84,    86,    77,    86,    55,    33,    14,
      38,    77,    38,    15,   479,   498,    50,    51,    52,    53,
      54,    55,    38,   488,    38,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    38,    69,    70,    71,    28,    73,
      74,    77,    76,    10,    50,    51,    52,    53,    54,    55,
      77,    46,    86,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    45,    69,    70,    71,    81,    73,    74,    81,
      76,    39,    50,    51,    52,    53,    54,    55,    77,    77,
      86,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      38,    69,    70,    71,    82,    73,    74,    82,    76,    77,
      50,    51,    52,    53,    54,    55,    38,    84,    79,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    30,    69,
      70,    71,    38,    73,    74,    84,    76,    50,    51,    52,
      53,    54,    55,    38,    77,    38,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    38,    69,    70,    71,    83,
      73,    74,    50,    51,    52,    53,    54,    55,    50,    51,
      52,    53,    54,    55,    24,    84,    19,    38,    20,    83,
      38,    69,    70,    71,    84,    73,    74,    15,     8,    38,
      38,    73,    74,     7,     6,    22,    85,    12,    39,    15,
      86,    77,    77,    38,    81,    39,    79,     7,    86,    15,
      35,   271,    84,   240,   215,   294,   297,   272,   275,   231,
     348,   350,   287,   485,   419,   503,   416,   506,   302,   341,
     395,   379,    -1,    -1,   343
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     4,     5,     9,    26,    88,    89,    90,    91,
      95,    96,   103,   104,   106,   107,   200,    92,    38,    38,
      38,    38,     0,    90,     6,     7,     8,    29,    97,   100,
     101,   102,    32,   109,   115,   109,    39,    93,    94,    79,
     221,   221,    77,    36,    37,    38,    39,    40,    41,    43,
      44,    51,    56,    57,    68,    72,    85,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   211,   211,    38,
     221,    97,   101,   102,    97,   100,   102,    97,   100,   101,
      77,    10,   111,   121,   111,    79,    80,    38,    38,    38,
      81,    84,    85,   208,    38,    44,    38,    43,   212,    85,
     212,   212,   212,   221,    50,    51,    52,    53,    54,    55,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    69,
      70,    71,    73,    74,    76,   221,   221,   221,    99,    97,
     102,    97,   101,    97,   102,    97,   100,    97,   101,    97,
     100,   116,    77,    11,   112,   126,   112,    94,    25,   221,
      36,    37,   220,   220,   211,    38,    86,   212,    84,    84,
     208,    38,    38,    38,    86,   212,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,    98,    97,    97,    97,
      97,    97,    97,    38,   117,   118,   119,   122,    77,    27,
     105,    13,   113,   134,    38,   201,    82,    80,    86,    38,
      38,    84,    86,    77,    79,    80,    38,    45,   123,   124,
     125,   127,    38,   221,    77,    14,   114,   173,   221,    33,
     149,   150,   151,   212,    38,   212,   119,    77,    38,    79,
      80,    16,    17,   128,   129,   221,   135,    18,    77,    15,
     110,   120,   204,    24,    77,   202,   151,    80,    86,    45,
      46,    47,    48,    49,   125,    38,   130,   131,    38,   132,
     133,    16,    17,    38,   136,   137,   138,    77,   175,    77,
      28,   108,   149,   211,   152,    10,   157,   158,   159,   212,
      46,    45,    81,    79,    80,    81,    79,    80,   130,   132,
      77,   138,   174,    21,    38,   176,   177,   178,   179,   186,
     187,   188,   190,    39,    38,   221,   205,    77,    38,   154,
     155,   156,    24,    77,   203,   159,    80,    86,    82,   131,
      82,   133,    79,    79,    38,   176,   180,    58,   208,    79,
      80,   222,    19,    20,    84,    79,   221,   157,   153,    79,
      80,   211,   160,    30,   207,   212,    25,   208,   221,    38,
     181,   182,   178,     6,     7,     8,    38,   191,   192,   194,
     195,   196,   197,   198,   199,   191,   195,    38,   189,   206,
     154,   156,    77,    38,   162,   163,   164,   221,    86,    38,
      25,   221,   139,    83,    23,    80,   211,   211,   211,    58,
     208,    24,   183,    84,    19,   198,   199,   197,   199,   197,
     198,   183,    20,    58,   208,   207,   161,    83,    79,    80,
     221,    38,   141,   149,   211,   185,   186,   182,   211,    15,
     184,    38,   193,    38,   191,   199,   198,   199,   197,   198,
     197,   184,   191,   162,    42,   209,   210,   211,   164,   143,
     221,   149,   140,    22,    31,   186,    79,   222,    39,    58,
     208,   183,   183,    85,   149,   145,   142,   147,   157,   211,
     222,   184,   184,    86,   211,   144,   149,   147,   148,    80,
      86,   147,   146,    12,   165,   166,   211,   147,    24,    77,
      15,   172,   165,    86,   211,   167,    77,    77,    38,   169,
     170,   171,    39,   168,   208,    79,    80,    79,   169,   171
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
#define YYEMPTY		(-2)
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
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

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

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value, Location);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
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
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;
  (void) yylocationp;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yytype, yyvaluep, yylocationp)
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;
  (void) yylocationp;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

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
  YYLTYPE *yylerrsp;

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

  if (yyss + yystacksize - 1 <= yyssp)
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
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
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

      if (yyss + yystacksize - 1 <= yyssp)
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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
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
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

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
  YY_REDUCE_PRINT (yyn);
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
#line 339 "ebnf.y"
    {NEDC( do_simple (yyvsp[-1]); )
                 GNED( MODULE_KEY = np->create("simple",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", yylsp[-1]);
                       setComments(MODULE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 42:
#line 348 "ebnf.y"
    {NEDC( end_simple (yyvsp[-1]); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 43:
#line 351 "ebnf.y"
    {NEDC( end_simple (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 45:
#line 368 "ebnf.y"
    {NEDC( do_module (yyvsp[-1]); )
                 GNED( MODULE_KEY = np->create("module",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", yylsp[-1]);
                       setComments(MODULE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 46:
#line 376 "ebnf.y"
    {NEDC( end_module (yyvsp[-1]); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 47:
#line 379 "ebnf.y"
    {NEDC( end_module (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,yylsp[-1]); )}
    break;

  case 49:
#line 386 "ebnf.y"
    {NEDC( do_machine(jar_strdup("default")); )}
    break;

  case 60:
#line 396 "ebnf.y"
    {GNED( MACHINES_KEY = np->create("machines",MODULE_KEY);
                       setComments(MACHINES_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 66:
#line 413 "ebnf.y"
    {NEDC( do_machine (yyvsp[0]); )
                 GNED( MACHINE_KEY = np->create("machine",MACHINES_KEY);
                       np->set(MACHINE_KEY,"name", yylsp[0]);
                       setComments(MACHINE_KEY,yylsp[0],yylsp[0]); )}
    break;

  case 67:
#line 421 "ebnf.y"
    {NEDC( do_displaystr_enclosing (yyvsp[-1]); )
                 GNED( setDisplayString(MODULE_KEY,yylsp[-1]); )}
    break;

  case 68:
#line 427 "ebnf.y"
    {NEDC( do_parameters (); )
                 GNED( PARAMS_KEY = np->create("params",MODULE_KEY);
                       setComments(PARAMS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 72:
#line 440 "ebnf.y"
    {GNED( setComments(PARAM_KEY,yylsp[0]); )}
    break;

  case 73:
#line 443 "ebnf.y"
    {GNED( setComments(PARAM_KEY,yylsp[0]); )}
    break;

  case 74:
#line 448 "ebnf.y"
    {NEDC( do_parameter (yyvsp[0], TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[0],TYPE_NUMERIC); )}
    break;

  case 75:
#line 451 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_NUMERIC); )}
    break;

  case 76:
#line 454 "ebnf.y"
    {NEDC( do_parameter (yyvsp[0], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[0],TYPE_CONST_NUM); )}
    break;

  case 77:
#line 457 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_CONST_NUM); )}
    break;

  case 78:
#line 460 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-3], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-3],TYPE_CONST_NUM); )}
    break;

  case 79:
#line 463 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-3], TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-3],TYPE_CONST_NUM); )}
    break;

  case 80:
#line 466 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_STRING); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_STRING); )}
    break;

  case 81:
#line 469 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_BOOL); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_BOOL); )}
    break;

  case 82:
#line 472 "ebnf.y"
    {NEDC( do_parameter (yyvsp[-2], TYPE_ANYTYPE); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,yylsp[-2],TYPE_ANYTYPE); )}
    break;

  case 83:
#line 478 "ebnf.y"
    {NEDC( do_gates (); )
                 GNED( GATES_KEY = np->create("gates",MODULE_KEY);
                       setComments(GATES_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 93:
#line 503 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[-2], 1, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[-2], 1, 1 );
                       setComments(GATE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 94:
#line 507 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[0], 1, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[0], 1, 0 );
                       setComments(GATE_KEY,yylsp[0]); )}
    break;

  case 97:
#line 519 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[-2], 0, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[-2], 0, 1 );
                       setComments(GATE_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 98:
#line 523 "ebnf.y"
    {NEDC( do_gatedecl( yyvsp[0], 0, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  yylsp[0], 0, 0 );
                       setComments(GATE_KEY,yylsp[0]); )}
    break;

  case 99:
#line 530 "ebnf.y"
    {NEDC( do_submodules (); )
                 GNED( SUBMODS_KEY = np->create("submods",MODULE_KEY);
                       setComments(SUBMODS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 105:
#line 548 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-3], NULL, yyvsp[-1], NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-3], NULLPOS, yylsp[-1], NULLPOS);
                       setComments(SUBMOD_KEY,yylsp[-3],yylsp[0]);  )}
    break;

  case 106:
#line 552 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-5], NULL, yyvsp[-3], NULL); )}
    break;

  case 108:
#line 555 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-4], yyvsp[-1], yyvsp[-2], NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-4], yylsp[-1], yylsp[-2], NULLPOS);
                       setComments(SUBMOD_KEY,yylsp[-4],yylsp[0]);  )}
    break;

  case 109:
#line 559 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-6], yyvsp[-3], yyvsp[-4], NULL); )}
    break;

  case 111:
#line 562 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-5], NULL, yyvsp[-3], yyvsp[-1]); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-5], NULLPOS, yylsp[-3], yylsp[-1]);
                       setComments(SUBMOD_KEY,yylsp[-5],yylsp[0]);  )}
    break;

  case 112:
#line 566 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-7], NULL, yyvsp[-5], yyvsp[-3]); )}
    break;

  case 114:
#line 569 "ebnf.y"
    {NEDC( do_submodule1 (yyvsp[-6], yyvsp[-3], yyvsp[-4], yyvsp[-1]); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, yylsp[-6], yylsp[-3], yylsp[-4], yylsp[-1]);
                       setComments(SUBMOD_KEY,yylsp[-6],yylsp[0]);  )}
    break;

  case 115:
#line 573 "ebnf.y"
    {NEDC( do_submodule2 (yyvsp[-8], yyvsp[-5], yyvsp[-6], yyvsp[-3]); )}
    break;

  case 117:
#line 579 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 118:
#line 582 "ebnf.y"
    {NEDC( end_submodule (); )}
    break;

  case 120:
#line 588 "ebnf.y"
    {NEDC( do_empty_onlist(); )}
    break;

  case 123:
#line 598 "ebnf.y"
    {NEDC( do_onlist(); )
                 GNED( SUBSTMACHINES_KEY = np->create("substmachines",in_network?NETWORK_KEY:SUBMOD_KEY);
                       setComments(SUBSTMACHINES_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 125:
#line 603 "ebnf.y"
    {NEDC( open_if(yyvsp[-1]); do_onlist(); )
                 GNED( SUBSTMACHINES_KEY = np->create("substmachines",in_network?NETWORK_KEY:SUBMOD_KEY);
                       np->set(SUBSTMACHINES_KEY,"condition",yylsp[-1]);
                       setComments(SUBSTMACHINES_KEY,yylsp[-3],yylsp[0]); )}
    break;

  case 126:
#line 608 "ebnf.y"
    {NEDC( close_if(); )}
    break;

  case 128:
#line 614 "ebnf.y"
    {NEDC( do_empty_onlist(); )}
    break;

  case 131:
#line 624 "ebnf.y"
    {NEDC( do_on_mach(yyvsp[0]); )
                 GNED( SUBSTMACHINE_KEY = addSubstmachine(SUBSTMACHINES_KEY,yylsp[0]);
                       setComments(SUBSTMACHINE_KEY,yylsp[0]); )}
    break;

  case 136:
#line 641 "ebnf.y"
    {NEDC( do_substparams (); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       setComments(SUBSTPARAMS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 138:
#line 646 "ebnf.y"
    {NEDC( open_if(yyvsp[-1]); do_substparams(); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       np->set(SUBSTPARAMS_KEY,"condition",yylsp[-1]);
                       setComments(SUBSTPARAMS_KEY,yylsp[-3],yylsp[0]); )}
    break;

  case 139:
#line 651 "ebnf.y"
    {NEDC( close_if(); )}
    break;

  case 144:
#line 666 "ebnf.y"
    {NEDC( do_substparameter (yyvsp[-2], yyvsp[0]);)
                 GNED( SUBSTPARAM_KEY = addSubstparam(SUBSTPARAMS_KEY,yylsp[-2],yylsp[0]);
                       setComments(SUBSTPARAM_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 147:
#line 678 "ebnf.y"
    {NEDC( do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       setComments(GATESIZES_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 149:
#line 683 "ebnf.y"
    {NEDC( open_if(yyvsp[-1]); do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       np->set(GATESIZES_KEY,"condition",yylsp[-1]);
                       setComments(GATESIZES_KEY,yylsp[-3],yylsp[0]); )}
    break;

  case 150:
#line 688 "ebnf.y"
    {NEDC( close_if(); )}
    break;

  case 155:
#line 703 "ebnf.y"
    {NEDC( do_gatesize (yyvsp[-1], yyvsp[0]); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,yylsp[-1],yylsp[0]);
                       setComments(GATESIZE_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 156:
#line 707 "ebnf.y"
    {NEDC( do_gatesize (yyvsp[0], NULL); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,yylsp[0],NULLPOS);
                       setComments(GATESIZE_KEY,yylsp[0]); )}
    break;

  case 157:
#line 714 "ebnf.y"
    {NEDC( do_displaystr_submod (yyvsp[-1]); )
                 GNED( setDisplayString(SUBMOD_KEY,yylsp[-1]); )}
    break;

  case 159:
#line 721 "ebnf.y"
    {NEDC( set_checkconns(0); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","1");
                       setComments(CONNS_KEY,yylsp[-2],yylsp[0]); )}
    break;

  case 161:
#line 727 "ebnf.y"
    {NEDC( set_checkconns(1); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","0");
                       setComments(CONNS_KEY,yylsp[-1],yylsp[0]); )}
    break;

  case 169:
#line 751 "ebnf.y"
    {NEDC( do_for (); )
                 GNED( FORLOOP_KEY = np->create("forloop",CONNS_KEY);
                       in_loop=1; setComments(FORLOOP_KEY,yylsp[0]); )}
    break;

  case 170:
#line 755 "ebnf.y"
    {NEDC( end_for (); )
                 GNED( in_loop=0; setTrailingComment(FORLOOP_KEY,yylsp[0]); )}
    break;

  case 173:
#line 766 "ebnf.y"
    {NEDC( do_index (yyvsp[-4], yyvsp[-2], yyvsp[0]); )
                 GNED( LOOPVAR_KEY=addLoopVar(FORLOOP_KEY,yylsp[-4],yylsp[-2],yylsp[0]);
                       setComments(LOOPVAR_KEY,yylsp[-4],yylsp[0]); )}
    break;

  case 174:
#line 773 "ebnf.y"
    {NEDC( do_condition(yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"condition",yylsp[0]); )}
    break;

  case 175:
#line 776 "ebnf.y"
    {NEDC( do_condition(NULL); )}
    break;

  case 176:
#line 781 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( setDisplayString(CONN_KEY,yylsp[0]); )}
    break;

  case 177:
#line 784 "ebnf.y"
    {NEDC( yyval = NULL; )}
    break;

  case 180:
#line 794 "ebnf.y"
    {NEDC( end_connection (NULL, 'R', yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,yylsp[-4],yylsp[0]); )}
    break;

  case 181:
#line 797 "ebnf.y"
    {NEDC( end_connection (yyvsp[-4], 'R', yyvsp[0]); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,yylsp[-6],yylsp[0]); )}
    break;

  case 182:
#line 800 "ebnf.y"
    {NEDC( end_connection (NULL, 'L', yyvsp[0]); )
                 GNED( swapConnection(CONN_KEY);
                       np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,yylsp[-4],yylsp[0]); )}
    break;

  case 183:
#line 804 "ebnf.y"
    {NEDC( end_connection (yyvsp[-4], 'L', yyvsp[0]); )
                 GNED( swapConnection(CONN_KEY);
                 np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,yylsp[-6],yylsp[0]); )}
    break;

  case 186:
#line 816 "ebnf.y"
    {NEDC( do_mod_L (yyvsp[-1], yyvsp[0]); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey(yylsp[-1]));
                       np->set(CONN_KEY, "src-mod-index", trimBrackets(yylsp[0])); )}
    break;

  case 187:
#line 821 "ebnf.y"
    {NEDC( do_mod_L (yyvsp[0], NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey(yylsp[0])); )}
    break;

  case 188:
#line 828 "ebnf.y"
    {NEDC( do_gate_L (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "srcgate", yylsp[-1]);
                       np->set(CONN_KEY, "src-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 189:
#line 832 "ebnf.y"
    {NEDC( do_gate_L (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "srcgate", yylsp[0]); )}
    break;

  case 190:
#line 835 "ebnf.y"
    {NEDC( do_gate_L (yyvsp[-1], NULL); )
                 GNED( np->set(CONN_KEY, "srcgate", yylsp[-1]);
                       np->set(CONN_KEY, "src-gate-plusplus", "1"); )}
    break;

  case 191:
#line 842 "ebnf.y"
    {NEDC( do_mod_L (NULL, NULL); do_gate_L (yyvsp[-1], yyvsp[0]); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", yylsp[-1]);
                       np->set(CONN_KEY, "src-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 192:
#line 848 "ebnf.y"
    {NEDC( do_mod_L (NULL, NULL); do_gate_L (yyvsp[0], NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", yylsp[0]); )}
    break;

  case 193:
#line 853 "ebnf.y"
    {NEDC( do_mod_L (NULL, NULL); do_gate_L (yyvsp[-1], NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", yylsp[-1]);
                       np->set(CONN_KEY, "src-gate-plusplus", "1");  )}
    break;

  case 196:
#line 867 "ebnf.y"
    {NEDC( do_mod_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey(yylsp[-1]));
                       np->set(CONN_KEY, "dest-mod-index", trimBrackets(yylsp[0])); )}
    break;

  case 197:
#line 871 "ebnf.y"
    {NEDC( do_mod_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey(yylsp[0])); )}
    break;

  case 198:
#line 877 "ebnf.y"
    {NEDC( do_gate_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "destgate", yylsp[-1]);
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 199:
#line 881 "ebnf.y"
    {NEDC( do_gate_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "destgate", yylsp[0]); )}
    break;

  case 200:
#line 884 "ebnf.y"
    {NEDC( do_gate_R (yyvsp[-1], NULL); )
                 GNED( np->set(CONN_KEY, "destgate", yylsp[-1]);
                       np->set(CONN_KEY, "dest-gate-plusplus", "1"); )}
    break;

  case 201:
#line 891 "ebnf.y"
    {NEDC( do_mod_R (NULL, NULL); do_gate_R (yyvsp[-1], yyvsp[0]); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", yylsp[-1]);
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets(yylsp[0])); )}
    break;

  case 202:
#line 896 "ebnf.y"
    {NEDC( do_mod_R (NULL, NULL); do_gate_R (yyvsp[0], NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", yylsp[0]); )}
    break;

  case 203:
#line 900 "ebnf.y"
    {NEDC( do_mod_R (NULL, NULL); do_gate_R (yyvsp[-1], NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", yylsp[-1]);
                       np->set(CONN_KEY, "dest-gate-plusplus", "1"); )}
    break;

  case 204:
#line 909 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, NULL, NULL); )}
    break;

  case 205:
#line 911 "ebnf.y"
    {NEDC( yyval = do_channeldescr(yyvsp[0], NULL,NULL,NULL); )}
    break;

  case 206:
#line 913 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], NULL, NULL); )}
    break;

  case 207:
#line 915 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[0], NULL); )}
    break;

  case 208:
#line 917 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, NULL, yyvsp[0]); )}
    break;

  case 209:
#line 920 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[0], NULL); )}
    break;

  case 210:
#line 922 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], NULL, yyvsp[0]); )}
    break;

  case 211:
#line 924 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[-1], yyvsp[0]); )}
    break;

  case 212:
#line 926 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-1], NULL); )}
    break;

  case 213:
#line 928 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], NULL, yyvsp[-1]); )}
    break;

  case 214:
#line 930 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, NULL, yyvsp[0], yyvsp[-1]); )}
    break;

  case 215:
#line 933 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-2], yyvsp[-1], yyvsp[0]); )}
    break;

  case 216:
#line 935 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-2], yyvsp[0], yyvsp[-1]); )}
    break;

  case 217:
#line 937 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[-2], yyvsp[0]); )}
    break;

  case 218:
#line 939 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-2], yyvsp[-1]); )}
    break;

  case 219:
#line 941 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[-1], yyvsp[0], yyvsp[-2]); )}
    break;

  case 220:
#line 943 "ebnf.y"
    {NEDC( yyval = do_channeldescr( NULL, yyvsp[0], yyvsp[-1], yyvsp[-2]); )}
    break;

  case 221:
#line 948 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"channel",yylsp[0]); )}
    break;

  case 222:
#line 954 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"delay",yylsp[0]); )}
    break;

  case 223:
#line 960 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"error",yylsp[0]); )}
    break;

  case 224:
#line 966 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"datarate",yylsp[0]); )}
    break;

  case 225:
#line 972 "ebnf.y"
    {NEDC( do_system (yyvsp[-3]); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,yylsp[-3],yylsp[-1],NULLPOS);
                       setComments(NETWORK_KEY,yylsp[-4],yylsp[0]); in_network=1;)}
    break;

  case 226:
#line 976 "ebnf.y"
    {NEDC( do_systemmodule (yyvsp[-5], yyvsp[-3], NULL); )}
    break;

  case 227:
#line 978 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 228:
#line 980 "ebnf.y"
    {NEDC( end_system (); )}
    break;

  case 229:
#line 982 "ebnf.y"
    {NEDC( do_system (yyvsp[-5]); )}
    break;

  case 230:
#line 984 "ebnf.y"
    {NEDC( do_systemmodule (yyvsp[-7], yyvsp[-5], yyvsp[-3]); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,yylsp[-7],yylsp[-5],yylsp[-3]);
                       setComments(NETWORK_KEY,yylsp[-8],yylsp[-2]); )}
    break;

  case 231:
#line 988 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 232:
#line 990 "ebnf.y"
    {NEDC( end_system (); )}
    break;

  case 233:
#line 995 "ebnf.y"
    {GNED( setTrailingComment(NETWORK_KEY,yylsp[-1]); in_network=0; )}
    break;

  case 234:
#line 1000 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )}
    break;

  case 237:
#line 1010 "ebnf.y"
    {NEDC( yyval = do_inputvalue(yyvsp[-3], yyvsp[-1]); )}
    break;

  case 238:
#line 1012 "ebnf.y"
    {NEDC( yyval = do_inputvalue(yyvsp[-1], NULL); )}
    break;

  case 239:
#line 1014 "ebnf.y"
    {NEDC( yyval = do_inputvalue(NULL, NULL); )}
    break;

  case 240:
#line 1016 "ebnf.y"
    {NEDC( yyval = do_inputvalue(NULL, NULL); )}
    break;

  case 241:
#line 1022 "ebnf.y"
    {NEDC( yyval = end_expr( yyvsp[0] ); )}
    break;

  case 242:
#line 1027 "ebnf.y"
    {NEDC( yyval = yyvsp[0]; )}
    break;

  case 243:
#line 1029 "ebnf.y"
    {NEDC( yyval = yyvsp[-1]; )}
    break;

  case 244:
#line 1032 "ebnf.y"
    {NEDC( yyval = do_op( 2, '+', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 245:
#line 1034 "ebnf.y"
    {NEDC( yyval = do_op( 2, '-', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 246:
#line 1036 "ebnf.y"
    {NEDC( yyval = do_op( 2, '*', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 247:
#line 1038 "ebnf.y"
    {NEDC( yyval = do_op( 2, '/', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 248:
#line 1040 "ebnf.y"
    {NEDC( yyval = do_op( 2, '%', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 249:
#line 1042 "ebnf.y"
    {NEDC( yyval = do_op( 2, '^', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 250:
#line 1046 "ebnf.y"
    {NEDC( yyval = do_addminus(yyvsp[0]); )}
    break;

  case 251:
#line 1049 "ebnf.y"
    {NEDC( yyval = do_op( 2, '=', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 252:
#line 1051 "ebnf.y"
    {NEDC( yyval = do_op( 2, '!', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 253:
#line 1053 "ebnf.y"
    {NEDC( yyval = do_op( 2, '>', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 254:
#line 1055 "ebnf.y"
    {NEDC( yyval = do_op( 2, '}', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 255:
#line 1057 "ebnf.y"
    {NEDC( yyval = do_op( 2, '<', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 256:
#line 1059 "ebnf.y"
    {NEDC( yyval = do_op( 2, '{', yyvsp[-2],yyvsp[0],NULL); )}
    break;

  case 257:
#line 1062 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bool_and"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 258:
#line 1064 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bool_or"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 259:
#line 1066 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bool_xor"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 260:
#line 1069 "ebnf.y"
    {NEDC( yyval = do_func(1,jar_strdup("bool_not"), yyvsp[0],NULL,NULL,NULL); )}
    break;

  case 261:
#line 1072 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bin_and"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 262:
#line 1074 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bin_or"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 263:
#line 1076 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("bin_xor"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 264:
#line 1079 "ebnf.y"
    {NEDC( yyval = do_func(1,jar_strdup("bin_compl"), yyvsp[0],NULL,NULL,NULL); )}
    break;

  case 265:
#line 1081 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("shift_left"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 266:
#line 1083 "ebnf.y"
    {NEDC( yyval = do_func(2,jar_strdup("shift_right"), yyvsp[-2],yyvsp[0],NULL,NULL); )}
    break;

  case 267:
#line 1085 "ebnf.y"
    {NEDC( yyval = do_op( 3, '?', yyvsp[-4],yyvsp[-2],yyvsp[0]); )}
    break;

  case 268:
#line 1087 "ebnf.y"
    {NEDC( yyval = do_func(0,yyvsp[-2], NULL,NULL,NULL,NULL); )}
    break;

  case 269:
#line 1089 "ebnf.y"
    {NEDC( yyval = do_func(1,yyvsp[-3], yyvsp[-1],NULL,NULL,NULL); )}
    break;

  case 270:
#line 1091 "ebnf.y"
    {NEDC( yyval = do_func(2,yyvsp[-5], yyvsp[-3],yyvsp[-1],NULL,NULL); )}
    break;

  case 271:
#line 1093 "ebnf.y"
    {NEDC( yyval = do_func(3,yyvsp[-7], yyvsp[-5],yyvsp[-3],yyvsp[-1],NULL); )}
    break;

  case 272:
#line 1095 "ebnf.y"
    {NEDC( yyval = do_func(3,yyvsp[-9], yyvsp[-7],yyvsp[-5],yyvsp[-3],yyvsp[-1]); )}
    break;

  case 278:
#line 1108 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],0,1); )}
    break;

  case 279:
#line 1110 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],0,0); )}
    break;

  case 280:
#line 1112 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],1,1); )}
    break;

  case 281:
#line 1114 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],1,0); )}
    break;

  case 282:
#line 1116 "ebnf.y"
    {NEDC( yyval = do_parname (0,0,yyvsp[0],1,0); )}
    break;

  case 283:
#line 1119 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-2],0,yyvsp[0],0,1); )}
    break;

  case 284:
#line 1121 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-2],0,yyvsp[0],0,0); )}
    break;

  case 285:
#line 1124 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-3],yyvsp[-2],yyvsp[0],0,1); )}
    break;

  case 286:
#line 1126 "ebnf.y"
    {NEDC( yyval = do_parname (yyvsp[-3],yyvsp[-2],yyvsp[0],0,0); )}
    break;

  case 287:
#line 1131 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_STRING,yyvsp[0]); )}
    break;

  case 288:
#line 1136 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_BOOL,jar_strdup("true")); )}
    break;

  case 289:
#line 1138 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_BOOL,jar_strdup("false")); )}
    break;

  case 290:
#line 1143 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_NUMERIC,yyvsp[0]); )}
    break;

  case 291:
#line 1148 "ebnf.y"
    {NEDC( yyval = make_literal_expr(TYPE_NUMERIC,jar_strdup("(long)sub_i")); )}
    break;

  case 292:
#line 1150 "ebnf.y"
    {NEDC( yyval = do_sizeof (yyvsp[-1]); )}
    break;

  case 296:
#line 1161 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-2],yyvsp[-1],yyvsp[0]); )}
    break;

  case 297:
#line 1163 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-2],yyvsp[-1],yyvsp[0]); )}
    break;

  case 298:
#line 1165 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-1],yyvsp[0],NULL); )}
    break;

  case 299:
#line 1167 "ebnf.y"
    {NEDC( yyval = do_timeconstant(yyvsp[-1],yyvsp[0],NULL); )}
    break;


    }

/* Line 991 of yacc.c.  */
#line 2964 "ebnf.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;
  yylsp -= yylen;

  YY_STACK_PRINT (yyss, yyssp);

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
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
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
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }

  yylerrsp = yylsp;

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
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp, yylsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval, &yylloc);
      yychar = YYEMPTY;
      *++yylerrsp = yylloc;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  */
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__) \
    && !defined __cplusplus
  __attribute__ ((__unused__))
#endif

  yylerrsp = yylsp;
  *++yylerrsp = yyloc;
  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp, yylsp);
      yyvsp--;
      yystate = *--yyssp;
      yylsp--;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
  YYLLOC_DEFAULT (yyloc, yylsp, (yylerrsp - yylsp));
  *++yylsp = yyloc;

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


#line 1174 "ebnf.y"


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




