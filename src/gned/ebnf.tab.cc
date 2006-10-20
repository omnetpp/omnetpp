/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Bison version.  */
#define YYBISON_VERSION "2.1"

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
     XMLTYPE = 304,
     ANYTYPE = 305,
     PLUS = 306,
     MIN = 307,
     MUL = 308,
     DIV = 309,
     MOD = 310,
     EXP = 311,
     SIZEOF = 312,
     SUBMODINDEX = 313,
     PLUSPLUS = 314,
     EQ = 315,
     NE = 316,
     GT = 317,
     GE = 318,
     LS = 319,
     LE = 320,
     AND = 321,
     OR = 322,
     XOR = 323,
     NOT = 324,
     BIN_AND = 325,
     BIN_OR = 326,
     BIN_XOR = 327,
     BIN_COMPL = 328,
     SHIFT_LEFT = 329,
     SHIFT_RIGHT = 330,
     INVALID_CHAR = 331,
     UMIN = 332
   };
#endif
/* Tokens.  */
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
#define XMLTYPE 304
#define ANYTYPE 305
#define PLUS 306
#define MIN 307
#define MUL 308
#define DIV 309
#define MOD 310
#define EXP 311
#define SIZEOF 312
#define SUBMODINDEX 313
#define PLUSPLUS 314
#define EQ 315
#define NE 316
#define GT 317
#define GE 318
#define LS 319
#define LE 320
#define AND 321
#define OR 322
#define XOR 323
#define NOT 324
#define BIN_AND 325
#define BIN_OR 326
#define BIN_XOR 327
#define BIN_COMPL 328
#define SHIFT_LEFT 329
#define SHIFT_RIGHT 330
#define INVALID_CHAR 331
#define UMIN 332




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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
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


/* Line 219 of yacc.c.  */
#line 395 "ebnf.tab.c"

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYLTYPE_IS_TRIVIAL) && YYLTYPE_IS_TRIVIAL \
             && defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
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
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  22
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   679

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  88
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  136
/* YYNRULES -- Number of rules. */
#define YYNRULES  305
/* YYNRULES -- Number of states. */
#define YYNSTATES  511

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   332

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    13,    15,    17,
      19,    21,    22,    27,    31,    33,    35,    38,    42,    46,
      50,    55,    60,    65,    70,    75,    80,    86,    92,    98,
     104,   110,   116,   119,   120,   125,   126,   130,   134,   138,
     142,   148,   152,   156,   159,   168,   172,   176,   179,   181,
     182,   184,   185,   187,   188,   190,   191,   193,   194,   196,
     197,   198,   203,   206,   207,   211,   213,   215,   220,   221,
     226,   229,   230,   234,   236,   238,   242,   245,   249,   254,
     259,   263,   267,   271,   275,   276,   281,   283,   284,   289,
     293,   298,   302,   306,   308,   312,   314,   318,   320,   324,
     326,   327,   332,   334,   335,   338,   340,   341,   342,   351,
     352,   353,   363,   364,   365,   376,   377,   378,   390,   391,
     396,   398,   399,   402,   404,   405,   410,   411,   418,   421,
     422,   426,   428,   430,   432,   433,   436,   438,   439,   444,
     445,   452,   455,   456,   460,   462,   466,   469,   470,   471,
     476,   477,   484,   487,   488,   492,   494,   497,   499,   504,
     505,   506,   512,   513,   518,   521,   522,   526,   528,   530,
     532,   533,   540,   544,   546,   552,   555,   556,   559,   560,
     564,   567,   573,   581,   587,   595,   599,   601,   604,   606,
     609,   611,   614,   617,   619,   622,   626,   628,   631,   633,
     636,   638,   641,   644,   646,   649,   650,   652,   654,   656,
     658,   661,   664,   667,   670,   673,   676,   680,   684,   688,
     692,   696,   700,   702,   705,   708,   711,   712,   713,   714,
     726,   727,   728,   729,   743,   746,   750,   752,   754,   761,
     766,   770,   772,   774,   776,   780,   784,   788,   792,   796,
     800,   804,   807,   811,   815,   819,   823,   827,   831,   835,
     839,   843,   846,   850,   854,   858,   861,   865,   869,   875,
     879,   884,   891,   900,   911,   913,   915,   917,   919,   921,
     923,   926,   929,   933,   937,   941,   946,   951,   957,   959,
     961,   963,   965,   967,   972,   974,   976,   978,   982,   986,
     989,   992,   994,   995,   997,   999
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
      89,     0,    -1,    90,    -1,    -1,    90,    91,    -1,    91,
      -1,    92,    -1,    96,    -1,   104,    -1,   107,    -1,   201,
      -1,    -1,     3,    93,    94,    80,    -1,    94,    81,    95,
      -1,    95,    -1,    39,    -1,    97,    98,    -1,    97,   102,
      98,    -1,    97,   101,    98,    -1,    97,   103,    98,    -1,
      97,   102,   101,    98,    -1,    97,   102,   103,    98,    -1,
      97,   101,   103,    98,    -1,    97,   101,   102,    98,    -1,
      97,   103,   102,    98,    -1,    97,   103,   101,    98,    -1,
      97,   102,   101,   103,    98,    -1,    97,   102,   103,   101,
      98,    -1,    97,   101,   102,   103,    98,    -1,    97,   101,
     103,   102,    98,    -1,    97,   103,   102,   101,    98,    -1,
      97,   103,   101,   102,    98,    -1,     5,    38,    -1,    -1,
      29,    38,   222,    99,    -1,    -1,    29,   222,   100,    -1,
       7,   212,   222,    -1,     6,   212,   222,    -1,     8,   212,
     222,    -1,   105,   110,   112,   113,   106,    -1,     4,    38,
     222,    -1,    27,    38,   222,    -1,    27,   222,    -1,   108,
     110,   112,   113,   114,   115,   111,   109,    -1,     9,    38,
     222,    -1,    28,    38,   222,    -1,    28,   222,    -1,   116,
      -1,    -1,   121,    -1,    -1,   122,    -1,    -1,   127,    -1,
      -1,   135,    -1,    -1,   174,    -1,    -1,    -1,    32,    78,
     117,   118,    -1,   119,    80,    -1,    -1,   119,    81,   120,
      -1,   120,    -1,    38,    -1,    15,    78,    39,    80,    -1,
      -1,    10,    78,   123,   124,    -1,   125,    80,    -1,    -1,
     125,    81,   126,    -1,   126,    -1,    38,    -1,    38,    78,
      46,    -1,    45,    38,    -1,    38,    78,    45,    -1,    38,
      78,    45,    46,    -1,    38,    78,    46,    45,    -1,    38,
      78,    47,    -1,    38,    78,    48,    -1,    38,    78,    49,
      -1,    38,    78,    50,    -1,    -1,    11,    78,   128,   129,
      -1,   130,    -1,    -1,   130,    16,   131,    80,    -1,    16,
     131,    80,    -1,   130,    17,   133,    80,    -1,    17,   133,
      80,    -1,   131,    81,   132,    -1,   132,    -1,    38,    82,
      83,    -1,    38,    -1,   133,    81,   134,    -1,   134,    -1,
      38,    82,    83,    -1,    38,    -1,    -1,    13,    78,   136,
     137,    -1,   138,    -1,    -1,   138,   139,    -1,   139,    -1,
      -1,    -1,    38,    78,    38,   222,   140,   150,   141,   148,
      -1,    -1,    -1,    38,    78,    38,   209,   222,   142,   150,
     143,   148,    -1,    -1,    -1,    38,    78,    38,    25,    38,
     222,   144,   150,   145,   148,    -1,    -1,    -1,    38,    78,
      38,   209,    25,    38,   222,   146,   150,   147,   148,    -1,
      -1,   158,   149,   166,   173,    -1,   151,    -1,    -1,   151,
     152,    -1,   152,    -1,    -1,    33,    78,   153,   155,    -1,
      -1,    33,    24,   212,    78,   154,   155,    -1,   156,    80,
      -1,    -1,   156,    81,   157,    -1,   157,    -1,    38,    -1,
     159,    -1,    -1,   159,   160,    -1,   160,    -1,    -1,    10,
      78,   161,   163,    -1,    -1,    10,    24,   212,    78,   162,
     163,    -1,   164,    80,    -1,    -1,   164,    81,   165,    -1,
     165,    -1,    38,    84,   210,    -1,   167,   166,    -1,    -1,
      -1,    12,    78,   168,   170,    -1,    -1,    12,    24,   212,
      78,   169,   170,    -1,   171,    80,    -1,    -1,   171,    81,
     172,    -1,   172,    -1,    38,   209,    -1,    38,    -1,    15,
      78,    39,    80,    -1,    -1,    -1,    14,    18,    78,   175,
     177,    -1,    -1,    14,    78,   176,   177,    -1,   178,    80,
      -1,    -1,   178,   223,   179,    -1,   179,    -1,   180,    -1,
     187,    -1,    -1,    21,   181,   182,    23,   186,    31,    -1,
     182,    81,   183,    -1,   183,    -1,    38,    84,   212,    22,
     212,    -1,    24,   212,    -1,    -1,    15,    39,    -1,    -1,
     186,   187,   223,    -1,   187,   223,    -1,   188,    20,   192,
     184,   185,    -1,   188,    20,   196,    20,   192,   184,   185,
      -1,   188,    19,   192,   184,   185,    -1,   188,    19,   196,
      19,   192,   184,   185,    -1,   189,    85,   190,    -1,   191,
      -1,    38,   209,    -1,    38,    -1,    38,   209,    -1,    38,
      -1,    38,    59,    -1,    38,   209,    -1,    38,    -1,    38,
      59,    -1,   193,    85,   194,    -1,   195,    -1,    38,   209,
      -1,    38,    -1,    38,   209,    -1,    38,    -1,    38,    59,
      -1,    38,   209,    -1,    38,    -1,    38,    59,    -1,    -1,
     197,    -1,   198,    -1,   199,    -1,   200,    -1,   198,   199,
      -1,   198,   200,    -1,   199,   200,    -1,   199,   198,    -1,
     200,   198,    -1,   200,   199,    -1,   198,   199,   200,    -1,
     198,   200,   199,    -1,   199,   198,   200,    -1,   199,   200,
     198,    -1,   200,   198,   199,    -1,   200,   199,   198,    -1,
      38,    -1,     6,   212,    -1,     7,   212,    -1,     8,   212,
      -1,    -1,    -1,    -1,    26,    38,    78,    38,   222,   202,
     150,   203,   158,   204,   208,    -1,    -1,    -1,    -1,    26,
      38,    78,    38,    25,    38,   222,   205,   150,   206,   158,
     207,   208,    -1,    30,   222,    -1,    82,   212,    83,    -1,
     211,    -1,   212,    -1,    42,    86,   212,    81,   212,    87,
      -1,    42,    86,   212,    87,    -1,    42,    86,    87,    -1,
      42,    -1,   213,    -1,   214,    -1,    86,   213,    87,    -1,
     213,    51,   213,    -1,   213,    52,   213,    -1,   213,    53,
     213,    -1,   213,    54,   213,    -1,   213,    55,   213,    -1,
     213,    56,   213,    -1,    52,   213,    -1,   213,    60,   213,
      -1,   213,    61,   213,    -1,   213,    62,   213,    -1,   213,
      63,   213,    -1,   213,    64,   213,    -1,   213,    65,   213,
      -1,   213,    66,   213,    -1,   213,    67,   213,    -1,   213,
      68,   213,    -1,    69,   213,    -1,   213,    70,   213,    -1,
     213,    71,   213,    -1,   213,    72,   213,    -1,    73,   213,
      -1,   213,    74,   213,    -1,   213,    75,   213,    -1,   213,
      77,   213,    78,   213,    -1,    38,    86,    87,    -1,    38,
      86,   213,    87,    -1,    38,    86,   213,    81,   213,    87,
      -1,    38,    86,   213,    81,   213,    81,   213,    87,    -1,
      38,    86,   213,    81,   213,    81,   213,    81,   213,    87,
      -1,   215,    -1,   216,    -1,   217,    -1,   218,    -1,   219,
      -1,    38,    -1,    43,    38,    -1,    44,    38,    -1,    43,
      44,    38,    -1,    44,    43,    38,    -1,    38,    85,    38,
      -1,    43,    38,    85,    38,    -1,    38,   209,    85,    38,
      -1,    43,    38,   209,    85,    38,    -1,    39,    -1,    40,
      -1,    41,    -1,   220,    -1,    58,    -1,    57,    86,    38,
      87,    -1,    36,    -1,    37,    -1,   221,    -1,    36,    38,
     221,    -1,    37,    38,   221,    -1,    36,    38,    -1,    37,
      38,    -1,    80,    -1,    -1,    81,    -1,    80,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
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

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
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
  "XMLTYPE", "ANYTYPE", "PLUS", "MIN", "MUL", "DIV", "MOD", "EXP",
  "SIZEOF", "SUBMODINDEX", "PLUSPLUS", "EQ", "NE", "GT", "GE", "LS", "LE",
  "AND", "OR", "XOR", "NOT", "BIN_AND", "BIN_OR", "BIN_XOR", "BIN_COMPL",
  "SHIFT_LEFT", "SHIFT_RIGHT", "INVALID_CHAR", "'?'", "':'", "UMIN", "';'",
  "','", "'['", "']'", "'='", "'.'", "'('", "')'", "$accept",
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
  "submodule_body", "@16", "opt_on_blocks", "on_blocks", "on_block", "@17",
  "@18", "opt_on_list", "on_list", "on_mach", "opt_substparamblocks",
  "substparamblocks", "substparamblock", "@19", "@20",
  "opt_substparameters", "substparameters", "substparameter",
  "opt_gatesizeblocks", "gatesizeblock", "@21", "@22", "opt_gatesizes",
  "gatesizes", "gatesize", "opt_submod_displayblock", "connblock", "@23",
  "@24", "opt_connections", "connections", "connection", "loopconnection",
  "@25", "loopvarlist", "loopvar", "opt_conn_condition",
  "opt_conn_displaystr", "notloopconnections", "notloopconnection",
  "gate_spec_L", "mod_L", "gate_L", "parentgate_L", "gate_spec_R", "mod_R",
  "gate_R", "parentgate_R", "channeldescr", "cdname", "cddelay", "cderror",
  "cddatarate", "network", "@26", "@27", "@28", "@29", "@30", "@31",
  "endnetwork", "vector", "inputvalue_or_expression", "inputvalue",
  "expression", "expr", "simple_expr", "parameter_expr", "string_expr",
  "boolconst_expr", "numconst_expr", "special_expr", "numconst",
  "timeconstant", "opt_semicolon", "comma_or_semicolon", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,    63,    58,   332,
      59,    44,    91,    93,    61,    46,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
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

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
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

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     6,     7,     8,     9,    17,    37,    38,    10,    11,
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

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -443
static const short int yypact[] =
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

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
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

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -305
static const short int yytable[] =
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

static const short int yycheck[] =
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

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
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

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


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
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
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

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value, Location);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
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
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
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
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

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
  const char *yys = yystr;

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
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



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
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");

# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;
  (void) yylocationp;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

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



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
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
    ;
#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended. */
  YYLTYPE yyerror_range[2];

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
#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

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
	short int *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
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
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
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
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

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
  YYLLOC_DEFAULT (yyloc, yylsp - yylen, yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 11:
#line 230 "ebnf.y"
    {GNED( IMPORTS_KEY = np->create("imports",NEDFILE_KEY);
                       setComments(IMPORTS_KEY,(yylsp[0])); )}
    break;

  case 12:
#line 233 "ebnf.y"
    {/* GNED( setTrailingComment(IMPORTS_KEY,@3); )
                  * comment already stored with last filename */}
    break;

  case 15:
#line 244 "ebnf.y"
    {NEDC( do_include ((yyvsp[0])); )
                 GNED( IMPORT_KEY = np->create("import",IMPORTS_KEY);
                       np->set(IMPORT_KEY,"name",trimQuotes((yylsp[0])));
                       setComments(IMPORT_KEY,(yylsp[0])); )}
    break;

  case 16:
#line 253 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-1]), NULL, NULL, NULL); )}
    break;

  case 17:
#line 256 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-2]), (yyvsp[-1]), NULL, NULL); )}
    break;

  case 18:
#line 258 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-2]), NULL, (yyvsp[-1]), NULL); )}
    break;

  case 19:
#line 260 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-2]), NULL, NULL, (yyvsp[-1])); )}
    break;

  case 20:
#line 263 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-3]), (yyvsp[-2]), (yyvsp[-1]), NULL); )}
    break;

  case 21:
#line 265 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-3]), (yyvsp[-2]), NULL, (yyvsp[-1])); )}
    break;

  case 22:
#line 267 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-3]), NULL, (yyvsp[-2]), (yyvsp[-1])); )}
    break;

  case 23:
#line 269 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-3]), (yyvsp[-1]), (yyvsp[-2]), NULL); )}
    break;

  case 24:
#line 271 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-3]), (yyvsp[-1]), NULL, (yyvsp[-2])); )}
    break;

  case 25:
#line 273 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-3]), NULL, (yyvsp[-1]), (yyvsp[-2])); )}
    break;

  case 26:
#line 276 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-4]), (yyvsp[-3]), (yyvsp[-2]), (yyvsp[-1])); )}
    break;

  case 27:
#line 278 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-4]), (yyvsp[-3]), (yyvsp[-1]), (yyvsp[-2])); )}
    break;

  case 28:
#line 280 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-4]), (yyvsp[-2]), (yyvsp[-3]), (yyvsp[-1])); )}
    break;

  case 29:
#line 282 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-4]), (yyvsp[-1]), (yyvsp[-3]), (yyvsp[-2])); )}
    break;

  case 30:
#line 284 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-4]), (yyvsp[-2]), (yyvsp[-1]), (yyvsp[-3])); )}
    break;

  case 31:
#line 286 "ebnf.y"
    {NEDC( do_channel ((yyvsp[-4]), (yyvsp[-1]), (yyvsp[-2]), (yyvsp[-3])); )}
    break;

  case 32:
#line 291 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[0]); inside_nonvoid_function=1;)
                 GNED( CHANNEL_KEY = np->create("channel",NEDFILE_KEY);
                       np->set(CHANNEL_KEY,"name",(yylsp[0]));
                       setComments(CHANNEL_KEY,(yylsp[-1]),(yylsp[0])); )}
    break;

  case 33:
#line 300 "ebnf.y"
    {NEDC( inside_nonvoid_function=0; )}
    break;

  case 34:
#line 301 "ebnf.y"
    {GNED( setTrailingComment(CHANNEL_KEY,(yylsp[-2])); )}
    break;

  case 35:
#line 303 "ebnf.y"
    {NEDC( inside_nonvoid_function=0; )}
    break;

  case 36:
#line 304 "ebnf.y"
    {GNED( setTrailingComment(CHANNEL_KEY,(yylsp[-2])); )}
    break;

  case 37:
#line 310 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[-1]); )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"error",(yylsp[-1]));
                       setComments(CHANATTR_KEY,(yylsp[-2]),(yylsp[0])); )}
    break;

  case 38:
#line 317 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[-1]); )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"delay",(yylsp[-1]));
                       setComments(CHANATTR_KEY,(yylsp[-2]),(yylsp[0])); )}
    break;

  case 39:
#line 324 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[-1]); )
                 GNED( CHANATTR_KEY = addChanAttr(CHANNEL_KEY,"datarate",(yylsp[-1]));
                       setComments(CHANATTR_KEY,(yylsp[-2]),(yylsp[0])); )}
    break;

  case 41:
#line 339 "ebnf.y"
    {NEDC( do_simple ((yyvsp[-1])); )
                 GNED( MODULE_KEY = np->create("simple",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", (yylsp[-1]));
                       setComments(MODULE_KEY,(yylsp[-2]),(yylsp[0])); )}
    break;

  case 42:
#line 348 "ebnf.y"
    {NEDC( end_simple ((yyvsp[-1])); )
                 GNED( setTrailingComment(MODULE_KEY,(yylsp[-1])); )}
    break;

  case 43:
#line 351 "ebnf.y"
    {NEDC( end_simple (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,(yylsp[-1])); )}
    break;

  case 45:
#line 368 "ebnf.y"
    {NEDC( do_module ((yyvsp[-1])); )
                 GNED( MODULE_KEY = np->create("module",NEDFILE_KEY);
                       np->set(MODULE_KEY,"name", (yylsp[-1]));
                       setComments(MODULE_KEY,(yylsp[-2]),(yylsp[0])); )}
    break;

  case 46:
#line 376 "ebnf.y"
    {NEDC( end_module ((yyvsp[-1])); )
                 GNED( setTrailingComment(MODULE_KEY,(yylsp[-1])); )}
    break;

  case 47:
#line 379 "ebnf.y"
    {NEDC( end_module (NULL); )
                 GNED( setTrailingComment(MODULE_KEY,(yylsp[-1])); )}
    break;

  case 49:
#line 386 "ebnf.y"
    {NEDC( do_machine(jar_strdup("default")); )}
    break;

  case 60:
#line 396 "ebnf.y"
    {GNED( MACHINES_KEY = np->create("machines",MODULE_KEY);
                       setComments(MACHINES_KEY,(yylsp[-1]),(yylsp[0])); )}
    break;

  case 66:
#line 413 "ebnf.y"
    {NEDC( do_machine ((yyvsp[0])); )
                 GNED( MACHINE_KEY = np->create("machine",MACHINES_KEY);
                       np->set(MACHINE_KEY,"name", (yylsp[0]));
                       setComments(MACHINE_KEY,(yylsp[0]),(yylsp[0])); )}
    break;

  case 67:
#line 421 "ebnf.y"
    {NEDC( do_displaystr_enclosing ((yyvsp[-1])); )
                 GNED( setDisplayString(MODULE_KEY,(yylsp[-1])); )}
    break;

  case 68:
#line 427 "ebnf.y"
    {NEDC( do_parameters (); )
                 GNED( PARAMS_KEY = np->create("params",MODULE_KEY);
                       setComments(PARAMS_KEY,(yylsp[-1]),(yylsp[0])); )}
    break;

  case 72:
#line 440 "ebnf.y"
    {GNED( setComments(PARAM_KEY,(yylsp[0])); )}
    break;

  case 73:
#line 443 "ebnf.y"
    {GNED( setComments(PARAM_KEY,(yylsp[0])); )}
    break;

  case 74:
#line 448 "ebnf.y"
    {NEDC( do_parameter ((yyvsp[0]), TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[0]),TYPE_NUMERIC); )}
    break;

  case 75:
#line 451 "ebnf.y"
    {NEDC( do_parameter ((yyvsp[-2]), TYPE_NUMERIC); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_NUMERIC); )}
    break;

  case 76:
#line 454 "ebnf.y"
    {NEDC( do_parameter ((yyvsp[0]), TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[0]),TYPE_CONST_NUM); )}
    break;

  case 77:
#line 457 "ebnf.y"
    {NEDC( do_parameter ((yyvsp[-2]), TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_CONST_NUM); )}
    break;

  case 78:
#line 460 "ebnf.y"
    {NEDC( do_parameter ((yyvsp[-3]), TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-3]),TYPE_CONST_NUM); )}
    break;

  case 79:
#line 463 "ebnf.y"
    {NEDC( do_parameter ((yyvsp[-3]), TYPE_CONST_NUM); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-3]),TYPE_CONST_NUM); )}
    break;

  case 80:
#line 466 "ebnf.y"
    {NEDC( do_parameter ((yyvsp[-2]), TYPE_STRING); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_STRING); )}
    break;

  case 81:
#line 469 "ebnf.y"
    {NEDC( do_parameter ((yyvsp[-2]), TYPE_BOOL); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_BOOL); )}
    break;

  case 82:
#line 472 "ebnf.y"
    {NEDC( do_parameter ((yyvsp[-2]), TYPE_XML); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_XML); )}
    break;

  case 83:
#line 475 "ebnf.y"
    {NEDC( do_parameter ((yyvsp[-2]), TYPE_ANYTYPE); )
                 GNED( PARAM_KEY=addParameter(PARAMS_KEY,(yylsp[-2]),TYPE_ANYTYPE); )}
    break;

  case 84:
#line 481 "ebnf.y"
    {NEDC( do_gates (); )
                 GNED( GATES_KEY = np->create("gates",MODULE_KEY);
                       setComments(GATES_KEY,(yylsp[-1]),(yylsp[0])); )}
    break;

  case 94:
#line 506 "ebnf.y"
    {NEDC( do_gatedecl( (yyvsp[-2]), 1, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  (yylsp[-2]), 1, 1 );
                       setComments(GATE_KEY,(yylsp[-2]),(yylsp[0])); )}
    break;

  case 95:
#line 510 "ebnf.y"
    {NEDC( do_gatedecl( (yyvsp[0]), 1, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  (yylsp[0]), 1, 0 );
                       setComments(GATE_KEY,(yylsp[0])); )}
    break;

  case 98:
#line 522 "ebnf.y"
    {NEDC( do_gatedecl( (yyvsp[-2]), 0, 1 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  (yylsp[-2]), 0, 1 );
                       setComments(GATE_KEY,(yylsp[-2]),(yylsp[0])); )}
    break;

  case 99:
#line 526 "ebnf.y"
    {NEDC( do_gatedecl( (yyvsp[0]), 0, 0 ); )
                 GNED( GATE_KEY=addGate(GATES_KEY,  (yylsp[0]), 0, 0 );
                       setComments(GATE_KEY,(yylsp[0])); )}
    break;

  case 100:
#line 533 "ebnf.y"
    {NEDC( do_submodules (); )
                 GNED( SUBMODS_KEY = np->create("submods",MODULE_KEY);
                       setComments(SUBMODS_KEY,(yylsp[-1]),(yylsp[0])); )}
    break;

  case 106:
#line 551 "ebnf.y"
    {NEDC( do_submodule1 ((yyvsp[-3]), NULL, (yyvsp[-1]), NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, (yylsp[-3]), NULLPOS, (yylsp[-1]), NULLPOS);
                       setComments(SUBMOD_KEY,(yylsp[-3]),(yylsp[0]));  )}
    break;

  case 107:
#line 555 "ebnf.y"
    {NEDC( do_submodule2 ((yyvsp[-5]), NULL, (yyvsp[-3]), NULL); )}
    break;

  case 109:
#line 558 "ebnf.y"
    {NEDC( do_submodule1 ((yyvsp[-4]), (yyvsp[-1]), (yyvsp[-2]), NULL); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, (yylsp[-4]), (yylsp[-1]), (yylsp[-2]), NULLPOS);
                       setComments(SUBMOD_KEY,(yylsp[-4]),(yylsp[0]));  )}
    break;

  case 110:
#line 562 "ebnf.y"
    {NEDC( do_submodule2 ((yyvsp[-6]), (yyvsp[-3]), (yyvsp[-4]), NULL); )}
    break;

  case 112:
#line 565 "ebnf.y"
    {NEDC( do_submodule1 ((yyvsp[-5]), NULL, (yyvsp[-3]), (yyvsp[-1])); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, (yylsp[-5]), NULLPOS, (yylsp[-3]), (yylsp[-1]));
                       setComments(SUBMOD_KEY,(yylsp[-5]),(yylsp[0]));  )}
    break;

  case 113:
#line 569 "ebnf.y"
    {NEDC( do_submodule2 ((yyvsp[-7]), NULL, (yyvsp[-5]), (yyvsp[-3])); )}
    break;

  case 115:
#line 572 "ebnf.y"
    {NEDC( do_submodule1 ((yyvsp[-6]), (yyvsp[-3]), (yyvsp[-4]), (yyvsp[-1])); )
                 GNED( SUBMOD_KEY=addSubmodule(SUBMODS_KEY, (yylsp[-6]), (yylsp[-3]), (yylsp[-4]), (yylsp[-1]));
                       setComments(SUBMOD_KEY,(yylsp[-6]),(yylsp[0]));  )}
    break;

  case 116:
#line 576 "ebnf.y"
    {NEDC( do_submodule2 ((yyvsp[-8]), (yyvsp[-5]), (yyvsp[-6]), (yyvsp[-3])); )}
    break;

  case 118:
#line 582 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 119:
#line 585 "ebnf.y"
    {NEDC( end_submodule (); )}
    break;

  case 121:
#line 591 "ebnf.y"
    {NEDC( do_empty_onlist(); )}
    break;

  case 124:
#line 601 "ebnf.y"
    {NEDC( do_onlist(); )
                 GNED( SUBSTMACHINES_KEY = np->create("substmachines",in_network?NETWORK_KEY:SUBMOD_KEY);
                       setComments(SUBSTMACHINES_KEY,(yylsp[-1]),(yylsp[0])); )}
    break;

  case 126:
#line 606 "ebnf.y"
    {NEDC( open_if((yyvsp[-1])); do_onlist(); )
                 GNED( SUBSTMACHINES_KEY = np->create("substmachines",in_network?NETWORK_KEY:SUBMOD_KEY);
                       np->set(SUBSTMACHINES_KEY,"condition",(yylsp[-1]));
                       setComments(SUBSTMACHINES_KEY,(yylsp[-3]),(yylsp[0])); )}
    break;

  case 127:
#line 611 "ebnf.y"
    {NEDC( close_if(); )}
    break;

  case 129:
#line 617 "ebnf.y"
    {NEDC( do_empty_onlist(); )}
    break;

  case 132:
#line 627 "ebnf.y"
    {NEDC( do_on_mach((yyvsp[0])); )
                 GNED( SUBSTMACHINE_KEY = addSubstmachine(SUBSTMACHINES_KEY,(yylsp[0]));
                       setComments(SUBSTMACHINE_KEY,(yylsp[0])); )}
    break;

  case 137:
#line 644 "ebnf.y"
    {NEDC( do_substparams (); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       setComments(SUBSTPARAMS_KEY,(yylsp[-1]),(yylsp[0])); )}
    break;

  case 139:
#line 649 "ebnf.y"
    {NEDC( open_if((yyvsp[-1])); do_substparams(); )
                 GNED( SUBSTPARAMS_KEY = np->create("substparams",in_network?NETWORK_KEY:SUBMOD_KEY);
                       np->set(SUBSTPARAMS_KEY,"condition",(yylsp[-1]));
                       setComments(SUBSTPARAMS_KEY,(yylsp[-3]),(yylsp[0])); )}
    break;

  case 140:
#line 654 "ebnf.y"
    {NEDC( close_if(); )}
    break;

  case 145:
#line 669 "ebnf.y"
    {NEDC( do_substparameter ((yyvsp[-2]), (yyvsp[0]));)
                 GNED( SUBSTPARAM_KEY = addSubstparam(SUBSTPARAMS_KEY,(yylsp[-2]),(yylsp[0]));
                       setComments(SUBSTPARAM_KEY,(yylsp[-2]),(yylsp[0])); )}
    break;

  case 148:
#line 681 "ebnf.y"
    {NEDC( do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       setComments(GATESIZES_KEY,(yylsp[-1]),(yylsp[0])); )}
    break;

  case 150:
#line 686 "ebnf.y"
    {NEDC( open_if((yyvsp[-1])); do_gatesizes (); )
                 GNED( GATESIZES_KEY = np->create("gatesizes",SUBMOD_KEY);
                       np->set(GATESIZES_KEY,"condition",(yylsp[-1]));
                       setComments(GATESIZES_KEY,(yylsp[-3]),(yylsp[0])); )}
    break;

  case 151:
#line 691 "ebnf.y"
    {NEDC( close_if(); )}
    break;

  case 156:
#line 706 "ebnf.y"
    {NEDC( do_gatesize ((yyvsp[-1]), (yyvsp[0])); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,(yylsp[-1]),(yylsp[0]));
                       setComments(GATESIZE_KEY,(yylsp[-1]),(yylsp[0])); )}
    break;

  case 157:
#line 710 "ebnf.y"
    {NEDC( do_gatesize ((yyvsp[0]), NULL); )
                 GNED( GATESIZE_KEY = addGateSize(GATESIZES_KEY,(yylsp[0]),NULLPOS);
                       setComments(GATESIZE_KEY,(yylsp[0])); )}
    break;

  case 158:
#line 717 "ebnf.y"
    {NEDC( do_displaystr_submod ((yyvsp[-1])); )
                 GNED( setDisplayString(SUBMOD_KEY,(yylsp[-1])); )}
    break;

  case 160:
#line 724 "ebnf.y"
    {NEDC( set_checkconns(0); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","1");
                       setComments(CONNS_KEY,(yylsp[-2]),(yylsp[0])); )}
    break;

  case 162:
#line 730 "ebnf.y"
    {NEDC( set_checkconns(1); do_connections(); )
                 GNED( CONNS_KEY = np->create("conns",MODULE_KEY);
                       np->set(CONNS_KEY,"nocheck","0");
                       setComments(CONNS_KEY,(yylsp[-1]),(yylsp[0])); )}
    break;

  case 170:
#line 754 "ebnf.y"
    {NEDC( do_for (); )
                 GNED( FORLOOP_KEY = np->create("forloop",CONNS_KEY);
                       in_loop=1; setComments(FORLOOP_KEY,(yylsp[0])); )}
    break;

  case 171:
#line 758 "ebnf.y"
    {NEDC( end_for (); )
                 GNED( in_loop=0; setTrailingComment(FORLOOP_KEY,(yylsp[0])); )}
    break;

  case 174:
#line 769 "ebnf.y"
    {NEDC( do_index ((yyvsp[-4]), (yyvsp[-2]), (yyvsp[0])); )
                 GNED( LOOPVAR_KEY=addLoopVar(FORLOOP_KEY,(yylsp[-4]),(yylsp[-2]),(yylsp[0]));
                       setComments(LOOPVAR_KEY,(yylsp[-4]),(yylsp[0])); )}
    break;

  case 175:
#line 776 "ebnf.y"
    {NEDC( do_condition((yyvsp[0])); )
                 GNED( np->set(CONN_KEY,"condition",(yylsp[0])); )}
    break;

  case 176:
#line 779 "ebnf.y"
    {NEDC( do_condition(NULL); )}
    break;

  case 177:
#line 784 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[0]); )
                 GNED( setDisplayString(CONN_KEY,(yylsp[0])); )}
    break;

  case 178:
#line 787 "ebnf.y"
    {NEDC( (yyval) = NULL; )}
    break;

  case 181:
#line 797 "ebnf.y"
    {NEDC( end_connection (NULL, 'R', (yyvsp[0])); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,(yylsp[-4]),(yylsp[0])); )}
    break;

  case 182:
#line 800 "ebnf.y"
    {NEDC( end_connection ((yyvsp[-4]), 'R', (yyvsp[0])); )
                 GNED( np->set(CONN_KEY,"arrowdir-l2r",1L); setComments(CONN_KEY,(yylsp[-6]),(yylsp[0])); )}
    break;

  case 183:
#line 803 "ebnf.y"
    {NEDC( end_connection (NULL, 'L', (yyvsp[0])); )
                 GNED( swapConnection(CONN_KEY);
                       np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,(yylsp[-4]),(yylsp[0])); )}
    break;

  case 184:
#line 807 "ebnf.y"
    {NEDC( end_connection ((yyvsp[-4]), 'L', (yyvsp[0])); )
                 GNED( swapConnection(CONN_KEY);
                 np->set(CONN_KEY,"arrowdir-l2r",0L); setComments(CONN_KEY,(yylsp[-6]),(yylsp[0])); )}
    break;

  case 187:
#line 819 "ebnf.y"
    {NEDC( do_mod_L ((yyvsp[-1]), (yyvsp[0])); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey((yylsp[-1])));
                       np->set(CONN_KEY, "src-mod-index", trimBrackets((yylsp[0]))); )}
    break;

  case 188:
#line 824 "ebnf.y"
    {NEDC( do_mod_L ((yyvsp[0]), NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", findSubmoduleKey((yylsp[0]))); )}
    break;

  case 189:
#line 831 "ebnf.y"
    {NEDC( do_gate_L ((yyvsp[-1]), (yyvsp[0])); )
                 GNED( np->set(CONN_KEY, "srcgate", (yylsp[-1]));
                       np->set(CONN_KEY, "src-gate-index", trimBrackets((yylsp[0]))); )}
    break;

  case 190:
#line 835 "ebnf.y"
    {NEDC( do_gate_L ((yyvsp[0]), NULL); )
                 GNED( np->set(CONN_KEY, "srcgate", (yylsp[0])); )}
    break;

  case 191:
#line 838 "ebnf.y"
    {NEDC( do_gate_L ((yyvsp[-1]), NULL); )
                 GNED( np->set(CONN_KEY, "srcgate", (yylsp[-1]));
                       np->set(CONN_KEY, "src-gate-plusplus", "1"); )}
    break;

  case 192:
#line 845 "ebnf.y"
    {NEDC( do_mod_L (NULL, NULL); do_gate_L ((yyvsp[-1]), (yyvsp[0])); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", (yylsp[-1]));
                       np->set(CONN_KEY, "src-gate-index", trimBrackets((yylsp[0]))); )}
    break;

  case 193:
#line 851 "ebnf.y"
    {NEDC( do_mod_L (NULL, NULL); do_gate_L ((yyvsp[0]), NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", (yylsp[0])); )}
    break;

  case 194:
#line 856 "ebnf.y"
    {NEDC( do_mod_L (NULL, NULL); do_gate_L ((yyvsp[-1]), NULL); )
                 GNED( CONN_KEY = np->create("conn",in_loop?FORLOOP_KEY:CONNS_KEY);
                       np->set(CONN_KEY, "src-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "srcgate", (yylsp[-1]));
                       np->set(CONN_KEY, "src-gate-plusplus", "1");  )}
    break;

  case 197:
#line 870 "ebnf.y"
    {NEDC( do_mod_R ((yyvsp[-1]), (yyvsp[0])); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey((yylsp[-1])));
                       np->set(CONN_KEY, "dest-mod-index", trimBrackets((yylsp[0]))); )}
    break;

  case 198:
#line 874 "ebnf.y"
    {NEDC( do_mod_R ((yyvsp[0]), NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", findSubmoduleKey((yylsp[0]))); )}
    break;

  case 199:
#line 880 "ebnf.y"
    {NEDC( do_gate_R ((yyvsp[-1]), (yyvsp[0])); )
                 GNED( np->set(CONN_KEY, "destgate", (yylsp[-1]));
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets((yylsp[0]))); )}
    break;

  case 200:
#line 884 "ebnf.y"
    {NEDC( do_gate_R ((yyvsp[0]), NULL); )
                 GNED( np->set(CONN_KEY, "destgate", (yylsp[0])); )}
    break;

  case 201:
#line 887 "ebnf.y"
    {NEDC( do_gate_R ((yyvsp[-1]), NULL); )
                 GNED( np->set(CONN_KEY, "destgate", (yylsp[-1]));
                       np->set(CONN_KEY, "dest-gate-plusplus", "1"); )}
    break;

  case 202:
#line 894 "ebnf.y"
    {NEDC( do_mod_R (NULL, NULL); do_gate_R ((yyvsp[-1]), (yyvsp[0])); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", (yylsp[-1]));
                       np->set(CONN_KEY, "dest-gate-index", trimBrackets((yylsp[0]))); )}
    break;

  case 203:
#line 899 "ebnf.y"
    {NEDC( do_mod_R (NULL, NULL); do_gate_R ((yyvsp[0]), NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", (yylsp[0])); )}
    break;

  case 204:
#line 903 "ebnf.y"
    {NEDC( do_mod_R (NULL, NULL); do_gate_R ((yyvsp[-1]), NULL); )
                 GNED( np->set(CONN_KEY, "dest-ownerkey", MODULE_KEY);
                       np->set(CONN_KEY, "destgate", (yylsp[-1]));
                       np->set(CONN_KEY, "dest-gate-plusplus", "1"); )}
    break;

  case 205:
#line 912 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, NULL, NULL, NULL); )}
    break;

  case 206:
#line 914 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr((yyvsp[0]), NULL,NULL,NULL); )}
    break;

  case 207:
#line 916 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, (yyvsp[0]), NULL, NULL); )}
    break;

  case 208:
#line 918 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, NULL, (yyvsp[0]), NULL); )}
    break;

  case 209:
#line 920 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, NULL, NULL, (yyvsp[0])); )}
    break;

  case 210:
#line 923 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, (yyvsp[-1]), (yyvsp[0]), NULL); )}
    break;

  case 211:
#line 925 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, (yyvsp[-1]), NULL, (yyvsp[0])); )}
    break;

  case 212:
#line 927 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, NULL, (yyvsp[-1]), (yyvsp[0])); )}
    break;

  case 213:
#line 929 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, (yyvsp[0]), (yyvsp[-1]), NULL); )}
    break;

  case 214:
#line 931 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, (yyvsp[0]), NULL, (yyvsp[-1])); )}
    break;

  case 215:
#line 933 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, NULL, (yyvsp[0]), (yyvsp[-1])); )}
    break;

  case 216:
#line 936 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, (yyvsp[-2]), (yyvsp[-1]), (yyvsp[0])); )}
    break;

  case 217:
#line 938 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, (yyvsp[-2]), (yyvsp[0]), (yyvsp[-1])); )}
    break;

  case 218:
#line 940 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, (yyvsp[-1]), (yyvsp[-2]), (yyvsp[0])); )}
    break;

  case 219:
#line 942 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, (yyvsp[0]), (yyvsp[-2]), (yyvsp[-1])); )}
    break;

  case 220:
#line 944 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, (yyvsp[-1]), (yyvsp[0]), (yyvsp[-2])); )}
    break;

  case 221:
#line 946 "ebnf.y"
    {NEDC( (yyval) = do_channeldescr( NULL, (yyvsp[0]), (yyvsp[-1]), (yyvsp[-2])); )}
    break;

  case 222:
#line 951 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[0]); )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"channel",(yylsp[0])); )}
    break;

  case 223:
#line 957 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[0]); )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"delay",(yylsp[0])); )}
    break;

  case 224:
#line 963 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[0]); )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"error",(yylsp[0])); )}
    break;

  case 225:
#line 969 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[0]); )
                 GNED( CONNATTR_KEY=addConnAttr(CONN_KEY,"datarate",(yylsp[0])); )}
    break;

  case 226:
#line 975 "ebnf.y"
    {NEDC( do_system ((yyvsp[-3])); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,(yylsp[-3]),(yylsp[-1]),NULLPOS);
                       setComments(NETWORK_KEY,(yylsp[-4]),(yylsp[0])); in_network=1;)}
    break;

  case 227:
#line 979 "ebnf.y"
    {NEDC( do_systemmodule ((yyvsp[-5]), (yyvsp[-3]), NULL); )}
    break;

  case 228:
#line 981 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 229:
#line 983 "ebnf.y"
    {NEDC( end_system (); )}
    break;

  case 230:
#line 985 "ebnf.y"
    {NEDC( do_system ((yyvsp[-5])); )}
    break;

  case 231:
#line 987 "ebnf.y"
    {NEDC( do_systemmodule ((yyvsp[-7]), (yyvsp[-5]), (yyvsp[-3])); )
                 GNED( NETWORK_KEY = addNetwork(NEDFILE_KEY,(yylsp[-7]),(yylsp[-5]),(yylsp[-3]));
                       setComments(NETWORK_KEY,(yylsp[-8]),(yylsp[-2])); )}
    break;

  case 232:
#line 991 "ebnf.y"
    {NEDC( do_readallparameters(); )}
    break;

  case 233:
#line 993 "ebnf.y"
    {NEDC( end_system (); )}
    break;

  case 234:
#line 998 "ebnf.y"
    {GNED( setTrailingComment(NETWORK_KEY,(yylsp[-1])); in_network=0; )}
    break;

  case 235:
#line 1003 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[-1]); )}
    break;

  case 238:
#line 1013 "ebnf.y"
    {NEDC( (yyval) = do_inputvalue((yyvsp[-3]), (yyvsp[-1])); )}
    break;

  case 239:
#line 1015 "ebnf.y"
    {NEDC( (yyval) = do_inputvalue((yyvsp[-1]), NULL); )}
    break;

  case 240:
#line 1017 "ebnf.y"
    {NEDC( (yyval) = do_inputvalue(NULL, NULL); )}
    break;

  case 241:
#line 1019 "ebnf.y"
    {NEDC( (yyval) = do_inputvalue(NULL, NULL); )}
    break;

  case 242:
#line 1025 "ebnf.y"
    {NEDC( (yyval) = end_expr( (yyvsp[0]) ); )}
    break;

  case 243:
#line 1030 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[0]); )}
    break;

  case 244:
#line 1032 "ebnf.y"
    {NEDC( (yyval) = (yyvsp[-1]); )}
    break;

  case 245:
#line 1035 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '+', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 246:
#line 1037 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '-', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 247:
#line 1039 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '*', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 248:
#line 1041 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '/', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 249:
#line 1043 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '%', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 250:
#line 1045 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '^', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 251:
#line 1049 "ebnf.y"
    {NEDC( (yyval) = do_addminus((yyvsp[0])); )}
    break;

  case 252:
#line 1052 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '=', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 253:
#line 1054 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '!', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 254:
#line 1056 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '>', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 255:
#line 1058 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '}', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 256:
#line 1060 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '<', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 257:
#line 1062 "ebnf.y"
    {NEDC( (yyval) = do_op( 2, '{', (yyvsp[-2]),(yyvsp[0]),NULL); )}
    break;

  case 258:
#line 1065 "ebnf.y"
    {NEDC( (yyval) = do_func(2,jar_strdup("bool_and"), (yyvsp[-2]),(yyvsp[0]),NULL,NULL); )}
    break;

  case 259:
#line 1067 "ebnf.y"
    {NEDC( (yyval) = do_func(2,jar_strdup("bool_or"), (yyvsp[-2]),(yyvsp[0]),NULL,NULL); )}
    break;

  case 260:
#line 1069 "ebnf.y"
    {NEDC( (yyval) = do_func(2,jar_strdup("bool_xor"), (yyvsp[-2]),(yyvsp[0]),NULL,NULL); )}
    break;

  case 261:
#line 1072 "ebnf.y"
    {NEDC( (yyval) = do_func(1,jar_strdup("bool_not"), (yyvsp[0]),NULL,NULL,NULL); )}
    break;

  case 262:
#line 1075 "ebnf.y"
    {NEDC( (yyval) = do_func(2,jar_strdup("bin_and"), (yyvsp[-2]),(yyvsp[0]),NULL,NULL); )}
    break;

  case 263:
#line 1077 "ebnf.y"
    {NEDC( (yyval) = do_func(2,jar_strdup("bin_or"), (yyvsp[-2]),(yyvsp[0]),NULL,NULL); )}
    break;

  case 264:
#line 1079 "ebnf.y"
    {NEDC( (yyval) = do_func(2,jar_strdup("bin_xor"), (yyvsp[-2]),(yyvsp[0]),NULL,NULL); )}
    break;

  case 265:
#line 1082 "ebnf.y"
    {NEDC( (yyval) = do_func(1,jar_strdup("bin_compl"), (yyvsp[0]),NULL,NULL,NULL); )}
    break;

  case 266:
#line 1084 "ebnf.y"
    {NEDC( (yyval) = do_func(2,jar_strdup("shift_left"), (yyvsp[-2]),(yyvsp[0]),NULL,NULL); )}
    break;

  case 267:
#line 1086 "ebnf.y"
    {NEDC( (yyval) = do_func(2,jar_strdup("shift_right"), (yyvsp[-2]),(yyvsp[0]),NULL,NULL); )}
    break;

  case 268:
#line 1088 "ebnf.y"
    {NEDC( (yyval) = do_op( 3, '?', (yyvsp[-4]),(yyvsp[-2]),(yyvsp[0])); )}
    break;

  case 269:
#line 1090 "ebnf.y"
    {NEDC( (yyval) = do_func(0,(yyvsp[-2]), NULL,NULL,NULL,NULL); )}
    break;

  case 270:
#line 1092 "ebnf.y"
    {NEDC( (yyval) = do_func(1,(yyvsp[-3]), (yyvsp[-1]),NULL,NULL,NULL); )}
    break;

  case 271:
#line 1094 "ebnf.y"
    {NEDC( (yyval) = do_func(2,(yyvsp[-5]), (yyvsp[-3]),(yyvsp[-1]),NULL,NULL); )}
    break;

  case 272:
#line 1096 "ebnf.y"
    {NEDC( (yyval) = do_func(3,(yyvsp[-7]), (yyvsp[-5]),(yyvsp[-3]),(yyvsp[-1]),NULL); )}
    break;

  case 273:
#line 1098 "ebnf.y"
    {NEDC( (yyval) = do_func(3,(yyvsp[-9]), (yyvsp[-7]),(yyvsp[-5]),(yyvsp[-3]),(yyvsp[-1])); )}
    break;

  case 279:
#line 1111 "ebnf.y"
    {NEDC( (yyval) = do_parname (0,0,(yyvsp[0]),0,1); )}
    break;

  case 280:
#line 1113 "ebnf.y"
    {NEDC( (yyval) = do_parname (0,0,(yyvsp[0]),0,0); )}
    break;

  case 281:
#line 1115 "ebnf.y"
    {NEDC( (yyval) = do_parname (0,0,(yyvsp[0]),1,1); )}
    break;

  case 282:
#line 1117 "ebnf.y"
    {NEDC( (yyval) = do_parname (0,0,(yyvsp[0]),1,0); )}
    break;

  case 283:
#line 1119 "ebnf.y"
    {NEDC( (yyval) = do_parname (0,0,(yyvsp[0]),1,0); )}
    break;

  case 284:
#line 1122 "ebnf.y"
    {NEDC( (yyval) = do_parname ((yyvsp[-2]),0,(yyvsp[0]),0,1); )}
    break;

  case 285:
#line 1124 "ebnf.y"
    {NEDC( (yyval) = do_parname ((yyvsp[-2]),0,(yyvsp[0]),0,0); )}
    break;

  case 286:
#line 1127 "ebnf.y"
    {NEDC( (yyval) = do_parname ((yyvsp[-3]),(yyvsp[-2]),(yyvsp[0]),0,1); )}
    break;

  case 287:
#line 1129 "ebnf.y"
    {NEDC( (yyval) = do_parname ((yyvsp[-3]),(yyvsp[-2]),(yyvsp[0]),0,0); )}
    break;

  case 288:
#line 1134 "ebnf.y"
    {NEDC( (yyval) = make_literal_expr(TYPE_STRING,(yyvsp[0])); )}
    break;

  case 289:
#line 1139 "ebnf.y"
    {NEDC( (yyval) = make_literal_expr(TYPE_BOOL,jar_strdup("true")); )}
    break;

  case 290:
#line 1141 "ebnf.y"
    {NEDC( (yyval) = make_literal_expr(TYPE_BOOL,jar_strdup("false")); )}
    break;

  case 291:
#line 1146 "ebnf.y"
    {NEDC( (yyval) = make_literal_expr(TYPE_NUMERIC,(yyvsp[0])); )}
    break;

  case 292:
#line 1151 "ebnf.y"
    {NEDC( (yyval) = make_literal_expr(TYPE_NUMERIC,jar_strdup("(long)sub_i")); )}
    break;

  case 293:
#line 1153 "ebnf.y"
    {NEDC( (yyval) = do_sizeof ((yyvsp[-1])); )}
    break;

  case 297:
#line 1164 "ebnf.y"
    {NEDC( (yyval) = do_timeconstant((yyvsp[-2]),(yyvsp[-1]),(yyvsp[0])); )}
    break;

  case 298:
#line 1166 "ebnf.y"
    {NEDC( (yyval) = do_timeconstant((yyvsp[-2]),(yyvsp[-1]),(yyvsp[0])); )}
    break;

  case 299:
#line 1168 "ebnf.y"
    {NEDC( (yyval) = do_timeconstant((yyvsp[-1]),(yyvsp[0]),NULL); )}
    break;

  case 300:
#line 1170 "ebnf.y"
    {NEDC( (yyval) = do_timeconstant((yyvsp[-1]),(yyvsp[0]),NULL); )}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 3110 "ebnf.tab.c"

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
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  yylsp -= yylen;
  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping", yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though. */
  YYLLOC_DEFAULT (yyloc, yyerror_range - 1, 2);
  *++yylsp = yyloc;

  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK;
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




