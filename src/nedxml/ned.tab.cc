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

/* Substitute the variable and function names.  */
#define yyparse nedyyparse
#define yylex   nedyylex
#define yyerror nedyyerror
#define yylval  nedyylval
#define yychar  nedyychar
#define yydebug nedyydebug
#define yynerrs nedyynerrs
#define yylloc nedyylloc

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INCLUDE = 258,
     SIMPLE = 259,
     CHANNEL = 260,
     MODULE = 261,
     PARAMETERS = 262,
     GATES = 263,
     GATESIZES = 264,
     SUBMODULES = 265,
     CONNECTIONS = 266,
     DISPLAY = 267,
     IN = 268,
     OUT = 269,
     NOCHECK = 270,
     LEFT_ARROW = 271,
     RIGHT_ARROW = 272,
     FOR = 273,
     TO = 274,
     DO = 275,
     IF = 276,
     LIKE = 277,
     NETWORK = 278,
     ENDSIMPLE = 279,
     ENDMODULE = 280,
     ENDCHANNEL = 281,
     ENDNETWORK = 282,
     ENDFOR = 283,
     MACHINES = 284,
     ON = 285,
     CHANATTRNAME = 286,
     INTCONSTANT = 287,
     REALCONSTANT = 288,
     NAME = 289,
     STRINGCONSTANT = 290,
     CHARCONSTANT = 291,
     TRUE_ = 292,
     FALSE_ = 293,
     INPUT_ = 294,
     XMLDOC = 295,
     REF = 296,
     ANCESTOR = 297,
     CONSTDECL = 298,
     NUMERICTYPE = 299,
     STRINGTYPE = 300,
     BOOLTYPE = 301,
     XMLTYPE = 302,
     ANYTYPE = 303,
     CPLUSPLUS = 304,
     CPLUSPLUSBODY = 305,
     MESSAGE = 306,
     CLASS = 307,
     STRUCT = 308,
     ENUM = 309,
     NONCOBJECT = 310,
     EXTENDS = 311,
     FIELDS = 312,
     PROPERTIES = 313,
     ABSTRACT = 314,
     READONLY = 315,
     CHARTYPE = 316,
     SHORTTYPE = 317,
     INTTYPE = 318,
     LONGTYPE = 319,
     DOUBLETYPE = 320,
     UNSIGNED_ = 321,
     SIZEOF = 322,
     SUBMODINDEX = 323,
     PLUSPLUS = 324,
     EQ = 325,
     NE = 326,
     GT = 327,
     GE = 328,
     LS = 329,
     LE = 330,
     AND = 331,
     OR = 332,
     XOR = 333,
     NOT = 334,
     BIN_AND = 335,
     BIN_OR = 336,
     BIN_XOR = 337,
     BIN_COMPL = 338,
     SHIFT_LEFT = 339,
     SHIFT_RIGHT = 340,
     INVALID_CHAR = 341,
     UMIN = 342
   };
#endif
/* Tokens.  */
#define INCLUDE 258
#define SIMPLE 259
#define CHANNEL 260
#define MODULE 261
#define PARAMETERS 262
#define GATES 263
#define GATESIZES 264
#define SUBMODULES 265
#define CONNECTIONS 266
#define DISPLAY 267
#define IN 268
#define OUT 269
#define NOCHECK 270
#define LEFT_ARROW 271
#define RIGHT_ARROW 272
#define FOR 273
#define TO 274
#define DO 275
#define IF 276
#define LIKE 277
#define NETWORK 278
#define ENDSIMPLE 279
#define ENDMODULE 280
#define ENDCHANNEL 281
#define ENDNETWORK 282
#define ENDFOR 283
#define MACHINES 284
#define ON 285
#define CHANATTRNAME 286
#define INTCONSTANT 287
#define REALCONSTANT 288
#define NAME 289
#define STRINGCONSTANT 290
#define CHARCONSTANT 291
#define TRUE_ 292
#define FALSE_ 293
#define INPUT_ 294
#define XMLDOC 295
#define REF 296
#define ANCESTOR 297
#define CONSTDECL 298
#define NUMERICTYPE 299
#define STRINGTYPE 300
#define BOOLTYPE 301
#define XMLTYPE 302
#define ANYTYPE 303
#define CPLUSPLUS 304
#define CPLUSPLUSBODY 305
#define MESSAGE 306
#define CLASS 307
#define STRUCT 308
#define ENUM 309
#define NONCOBJECT 310
#define EXTENDS 311
#define FIELDS 312
#define PROPERTIES 313
#define ABSTRACT 314
#define READONLY 315
#define CHARTYPE 316
#define SHORTTYPE 317
#define INTTYPE 318
#define LONGTYPE 319
#define DOUBLETYPE 320
#define UNSIGNED_ 321
#define SIZEOF 322
#define SUBMODINDEX 323
#define PLUSPLUS 324
#define EQ 325
#define NE 326
#define GT 327
#define GE 328
#define LS 329
#define LE 330
#define AND 331
#define OR 332
#define XOR 333
#define NOT 334
#define BIN_AND 335
#define BIN_OR 336
#define BIN_XOR 337
#define BIN_COMPL 338
#define SHIFT_LEFT 339
#define SHIFT_RIGHT 340
#define INVALID_CHAR 341
#define UMIN 342




/* Copy the first part of user declarations.  */
#line 74 "ned.y"


/*
 * Note:
 * This file contains about 5 shift-reduce conflicts, 3 of them around 'expression'.
 *
 * Plus one (real) ambiguity exists between submodule display string
 * and compound module display string if no connections are present.
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
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
    bool inGroup;

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
void removeRedundantChanSpecParams();
void createSubstparamsNodeIfNotExists();
void createGatesizesNodeIfNotExists();



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
#line 389 "ned.tab.c"

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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   663

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  104
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  107
/* YYNRULES -- Number of rules. */
#define YYNRULES  238
/* YYNRULES -- Number of states. */
#define YYNSTATES  406

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   342

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    93,     2,     2,
     102,   103,    91,    89,    97,    90,   101,    92,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    88,    96,
       2,   100,     2,    87,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    98,     2,    99,    94,     2,     2,     2,     2,     2,
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
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    95
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     8,     9,    11,    13,    15,    17,
      19,    23,    27,    29,    31,    35,    38,    39,    41,    46,
      50,    54,    57,    62,    65,    69,    72,    80,    83,    87,
      90,    92,    93,    98,   100,   101,   102,   107,   110,   111,
     115,   117,   119,   123,   126,   130,   135,   140,   144,   148,
     152,   156,   158,   159,   160,   165,   167,   168,   173,   177,
     182,   186,   190,   192,   196,   198,   202,   204,   208,   210,
     212,   213,   214,   219,   221,   222,   225,   227,   228,   235,
     236,   244,   245,   254,   255,   265,   269,   271,   272,   275,
     277,   278,   283,   284,   291,   294,   295,   299,   301,   305,
     307,   308,   311,   313,   314,   319,   320,   327,   330,   331,
     335,   337,   340,   342,   347,   348,   350,   351,   352,   358,
     359,   364,   366,   367,   370,   372,   374,   376,   377,   385,
     389,   391,   397,   400,   401,   404,   405,   408,   410,   417,
     426,   433,   442,   446,   448,   451,   453,   456,   458,   461,
     464,   466,   469,   473,   475,   478,   480,   483,   485,   488,
     491,   493,   496,   498,   500,   502,   505,   508,   512,   518,
     521,   525,   527,   529,   531,   538,   543,   547,   549,   556,
     561,   563,   567,   571,   575,   579,   583,   587,   591,   594,
     598,   602,   606,   610,   614,   618,   622,   626,   630,   633,
     637,   641,   645,   648,   652,   656,   662,   666,   671,   678,
     687,   698,   700,   702,   704,   706,   709,   713,   717,   720,
     722,   726,   731,   733,   735,   737,   739,   741,   743,   745,
     747,   749,   753,   757,   760,   763,   765,   766,   768
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     105,     0,    -1,   106,    -1,   106,   107,    -1,    -1,   108,
      -1,   111,    -1,   116,    -1,   119,    -1,   193,    -1,     3,
     109,    96,    -1,   109,    97,   110,    -1,   110,    -1,    35,
      -1,   112,   113,   115,    -1,     5,    34,    -1,    -1,   114,
      -1,   114,    31,   197,   209,    -1,    31,   197,   209,    -1,
      26,    34,   209,    -1,    26,   209,    -1,   117,   124,   130,
     118,    -1,     4,    34,    -1,    24,    34,   209,    -1,    24,
     209,    -1,   120,   124,   130,   139,   167,   122,   121,    -1,
       6,    34,    -1,    25,    34,   209,    -1,    25,   209,    -1,
     123,    -1,    -1,    12,    88,    35,    96,    -1,   125,    -1,
      -1,    -1,     7,    88,   126,   127,    -1,   128,    96,    -1,
      -1,   128,    97,   129,    -1,   129,    -1,    34,    -1,    34,
      88,    44,    -1,    43,    34,    -1,    34,    88,    43,    -1,
      34,    88,    43,    44,    -1,    34,    88,    44,    43,    -1,
      34,    88,    45,    -1,    34,    88,    46,    -1,    34,    88,
      47,    -1,    34,    88,    48,    -1,   131,    -1,    -1,    -1,
       8,    88,   132,   133,    -1,   134,    -1,    -1,   134,    13,
     135,    96,    -1,    13,   135,    96,    -1,   134,    14,   137,
      96,    -1,    14,   137,    96,    -1,   135,    97,   136,    -1,
     136,    -1,    34,    98,    99,    -1,    34,    -1,   137,    97,
     138,    -1,   138,    -1,    34,    98,    99,    -1,    34,    -1,
     140,    -1,    -1,    -1,    10,    88,   141,   142,    -1,   143,
      -1,    -1,   143,   144,    -1,   144,    -1,    -1,    34,    88,
      34,   209,   145,   149,    -1,    -1,    34,    88,    34,   196,
     209,   146,   149,    -1,    -1,    34,    88,    34,    22,    34,
     209,   147,   149,    -1,    -1,    34,    88,    34,   196,    22,
      34,   209,   148,   149,    -1,   150,   158,   166,    -1,   151,
      -1,    -1,   151,   152,    -1,   152,    -1,    -1,     7,    88,
     153,   155,    -1,    -1,     7,    21,   197,    88,   154,   155,
      -1,   156,    96,    -1,    -1,   156,    97,   157,    -1,   157,
      -1,    34,   100,   197,    -1,   159,    -1,    -1,   159,   160,
      -1,   160,    -1,    -1,     9,    88,   161,   163,    -1,    -1,
       9,    21,   197,    88,   162,   163,    -1,   164,    96,    -1,
      -1,   164,    97,   165,    -1,   165,    -1,    34,   196,    -1,
      34,    -1,    12,    88,    35,    96,    -1,    -1,   168,    -1,
      -1,    -1,    11,    15,    88,   169,   171,    -1,    -1,    11,
      88,   170,   171,    -1,   172,    -1,    -1,   172,   173,    -1,
     173,    -1,   174,    -1,   181,    -1,    -1,    18,   175,   176,
      20,   180,    28,   209,    -1,   177,    97,   176,    -1,   177,
      -1,    34,   100,   197,    19,   197,    -1,    21,   197,    -1,
      -1,    12,    35,    -1,    -1,   180,   181,    -1,   181,    -1,
     182,    17,   186,   178,   179,   210,    -1,   182,    17,   190,
      17,   186,   178,   179,   210,    -1,   182,    16,   186,   178,
     179,   210,    -1,   182,    16,   190,    16,   186,   178,   179,
     210,    -1,   183,   101,   184,    -1,   185,    -1,    34,   196,
      -1,    34,    -1,    34,   196,    -1,    34,    -1,    34,    69,
      -1,    34,   196,    -1,    34,    -1,    34,    69,    -1,   187,
     101,   188,    -1,   189,    -1,    34,   196,    -1,    34,    -1,
      34,   196,    -1,    34,    -1,    34,    69,    -1,    34,   196,
      -1,    34,    -1,    34,    69,    -1,   191,    -1,    34,    -1,
     192,    -1,   191,   192,    -1,    31,   197,    -1,   194,   150,
     195,    -1,    23,    34,    88,    34,   209,    -1,    27,   209,
      -1,    98,   197,    99,    -1,   200,    -1,   198,    -1,   199,
      -1,    39,   102,   200,    97,   200,   103,    -1,    39,   102,
     200,   103,    -1,    39,   102,   103,    -1,    39,    -1,    40,
     102,   205,    97,   205,   103,    -1,    40,   102,   205,   103,
      -1,   201,    -1,   102,   200,   103,    -1,   200,    89,   200,
      -1,   200,    90,   200,    -1,   200,    91,   200,    -1,   200,
      92,   200,    -1,   200,    93,   200,    -1,   200,    94,   200,
      -1,    90,   200,    -1,   200,    70,   200,    -1,   200,    71,
     200,    -1,   200,    72,   200,    -1,   200,    73,   200,    -1,
     200,    74,   200,    -1,   200,    75,   200,    -1,   200,    76,
     200,    -1,   200,    77,   200,    -1,   200,    78,   200,    -1,
      79,   200,    -1,   200,    80,   200,    -1,   200,    81,   200,
      -1,   200,    82,   200,    -1,    83,   200,    -1,   200,    84,
     200,    -1,   200,    85,   200,    -1,   200,    87,   200,    88,
     200,    -1,    34,   102,   103,    -1,    34,   102,   200,   103,
      -1,    34,   102,   200,    97,   200,   103,    -1,    34,   102,
     200,    97,   200,    97,   200,   103,    -1,    34,   102,   200,
      97,   200,    97,   200,    97,   200,   103,    -1,   202,    -1,
     203,    -1,   204,    -1,    34,    -1,    41,    34,    -1,    41,
      42,    34,    -1,    42,    41,    34,    -1,    42,    34,    -1,
      68,    -1,    68,   102,   103,    -1,    67,   102,    34,   103,
      -1,   205,    -1,   206,    -1,   207,    -1,    35,    -1,    37,
      -1,    38,    -1,    32,    -1,    33,    -1,   208,    -1,   208,
      32,    34,    -1,   208,    33,    34,    -1,    32,    34,    -1,
      33,    34,    -1,    96,    -1,    -1,    97,    -1,    96,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   179,   179,   183,   184,   188,   190,   192,   194,   196,
     204,   209,   210,   214,   227,   232,   242,   244,   249,   257,
     268,   272,   282,   290,   299,   303,   313,   324,   333,   337,
     347,   348,   352,   377,   378,   383,   382,   394,   395,   399,
     403,   413,   420,   427,   433,   439,   445,   451,   457,   463,
     469,   479,   480,   485,   484,   496,   497,   501,   502,   503,
     504,   508,   509,   513,   521,   531,   532,   536,   544,   557,
     558,   563,   562,   574,   575,   579,   580,   585,   584,   596,
     595,   608,   607,   620,   619,   635,   644,   646,   650,   651,
     656,   655,   664,   663,   682,   683,   687,   688,   692,   706,
     708,   712,   713,   718,   717,   726,   725,   743,   744,   748,
     749,   753,   761,   773,   792,   799,   800,   805,   804,   815,
     814,   827,   828,   832,   833,   837,   838,   843,   842,   881,
     882,   886,   897,   918,   922,   937,   941,   942,   946,   952,
     959,   966,   977,   978,   982,   989,   998,  1003,  1007,  1015,
    1023,  1030,  1041,  1042,  1046,  1051,  1058,  1063,  1067,  1075,
    1080,  1084,  1093,  1102,  1108,  1109,  1113,  1127,  1134,  1148,
    1159,  1165,  1169,  1173,  1184,  1186,  1188,  1190,  1195,  1197,
    1202,  1203,  1206,  1208,  1210,  1212,  1214,  1216,  1219,  1223,
    1225,  1227,  1229,  1231,  1233,  1236,  1238,  1240,  1243,  1247,
    1249,  1251,  1254,  1257,  1259,  1261,  1264,  1266,  1268,  1270,
    1272,  1277,  1278,  1279,  1283,  1288,  1294,  1299,  1304,  1312,
    1314,  1316,  1321,  1322,  1323,  1327,  1332,  1334,  1339,  1341,
    1343,  1348,  1349,  1350,  1351,  1354,  1354,  1356,  1356
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INCLUDE", "SIMPLE", "CHANNEL", "MODULE",
  "PARAMETERS", "GATES", "GATESIZES", "SUBMODULES", "CONNECTIONS",
  "DISPLAY", "IN", "OUT", "NOCHECK", "LEFT_ARROW", "RIGHT_ARROW", "FOR",
  "TO", "DO", "IF", "LIKE", "NETWORK", "ENDSIMPLE", "ENDMODULE",
  "ENDCHANNEL", "ENDNETWORK", "ENDFOR", "MACHINES", "ON", "CHANATTRNAME",
  "INTCONSTANT", "REALCONSTANT", "NAME", "STRINGCONSTANT", "CHARCONSTANT",
  "TRUE_", "FALSE_", "INPUT_", "XMLDOC", "REF", "ANCESTOR", "CONSTDECL",
  "NUMERICTYPE", "STRINGTYPE", "BOOLTYPE", "XMLTYPE", "ANYTYPE",
  "CPLUSPLUS", "CPLUSPLUSBODY", "MESSAGE", "CLASS", "STRUCT", "ENUM",
  "NONCOBJECT", "EXTENDS", "FIELDS", "PROPERTIES", "ABSTRACT", "READONLY",
  "CHARTYPE", "SHORTTYPE", "INTTYPE", "LONGTYPE", "DOUBLETYPE",
  "UNSIGNED_", "SIZEOF", "SUBMODINDEX", "PLUSPLUS", "EQ", "NE", "GT", "GE",
  "LS", "LE", "AND", "OR", "XOR", "NOT", "BIN_AND", "BIN_OR", "BIN_XOR",
  "BIN_COMPL", "SHIFT_LEFT", "SHIFT_RIGHT", "INVALID_CHAR", "'?'", "':'",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "UMIN", "';'", "','", "'['",
  "']'", "'='", "'.'", "'('", "')'", "$accept", "networkdescription",
  "somedefinitions", "definition", "import", "filenames", "filename",
  "channeldefinition_old", "channelheader_old", "opt_channelattrblock_old",
  "channelattrblock_old", "endchannel_old", "simpledefinition_old",
  "simpleheader_old", "endsimple_old", "moduledefinition_old",
  "moduleheader_old", "endmodule_old", "opt_displayblock_old",
  "displayblock_old", "opt_paramblock_old", "paramblock_old", "@1",
  "opt_parameters_old", "parameters_old", "parameter_old",
  "opt_gateblock_old", "gateblock_old", "@2", "opt_gates_old", "gates_old",
  "gatesI_old", "gateI_old", "gatesO_old", "gateO_old",
  "opt_submodblock_old", "submodblock_old", "@3", "opt_submodules_old",
  "submodules_old", "submodule_old", "@4", "@5", "@6", "@7",
  "submodule_body_old", "opt_substparamblocks_old", "substparamblocks_old",
  "substparamblock_old", "@8", "@9", "opt_substparameters_old",
  "substparameters_old", "substparameter_old", "opt_gatesizeblocks_old",
  "gatesizeblocks_old", "gatesizeblock_old", "@10", "@11",
  "opt_gatesizes_old", "gatesizes_old", "gatesize_old",
  "opt_submod_displayblock_old", "opt_connblock_old", "connblock_old",
  "@12", "@13", "opt_connections_old", "connections_old", "connection_old",
  "loopconnection_old", "@14", "loopvarlist_old", "loopvar_old",
  "opt_conncondition_old", "opt_conn_displaystr_old",
  "notloopconnections_old", "notloopconnection_old", "leftgatespec_old",
  "leftmod_old", "leftgate_old", "parentleftgate_old", "rightgatespec_old",
  "rightmod_old", "rightgate_old", "parentrightgate_old",
  "channeldescr_old", "channelattrs_old", "chanattr_old",
  "networkdefinition_old", "networkheader_old", "endnetwork_old", "vector",
  "expression", "inputvalue", "xmldocvalue", "expr", "simple_expr",
  "parameter_expr", "special_expr", "literal", "stringliteral",
  "boolliteral", "numliteral", "quantity", "opt_semicolon",
  "comma_or_semicolon", 0
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
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,    63,    58,    43,
      45,    42,    47,    37,    94,   342,    59,    44,    91,    93,
      61,    46,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   104,   105,   106,   106,   107,   107,   107,   107,   107,
     108,   109,   109,   110,   111,   112,   113,   113,   114,   114,
     115,   115,   116,   117,   118,   118,   119,   120,   121,   121,
     122,   122,   123,   124,   124,   126,   125,   127,   127,   128,
     128,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   130,   130,   132,   131,   133,   133,   134,   134,   134,
     134,   135,   135,   136,   136,   137,   137,   138,   138,   139,
     139,   141,   140,   142,   142,   143,   143,   145,   144,   146,
     144,   147,   144,   148,   144,   149,   150,   150,   151,   151,
     153,   152,   154,   152,   155,   155,   156,   156,   157,   158,
     158,   159,   159,   161,   160,   162,   160,   163,   163,   164,
     164,   165,   165,   166,   166,   167,   167,   169,   168,   170,
     168,   171,   171,   172,   172,   173,   173,   175,   174,   176,
     176,   177,   178,   178,   179,   179,   180,   180,   181,   181,
     181,   181,   182,   182,   183,   183,   184,   184,   184,   185,
     185,   185,   186,   186,   187,   187,   188,   188,   188,   189,
     189,   189,   190,   191,   191,   191,   192,   193,   194,   195,
     196,   197,   197,   197,   198,   198,   198,   198,   199,   199,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   201,   201,   201,   202,   202,   202,   202,   202,   203,
     203,   203,   204,   204,   204,   205,   206,   206,   207,   207,
     207,   208,   208,   208,   208,   209,   209,   210,   210
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     0,     1,     1,     1,     1,     1,
       3,     3,     1,     1,     3,     2,     0,     1,     4,     3,
       3,     2,     4,     2,     3,     2,     7,     2,     3,     2,
       1,     0,     4,     1,     0,     0,     4,     2,     0,     3,
       1,     1,     3,     2,     3,     4,     4,     3,     3,     3,
       3,     1,     0,     0,     4,     1,     0,     4,     3,     4,
       3,     3,     1,     3,     1,     3,     1,     3,     1,     1,
       0,     0,     4,     1,     0,     2,     1,     0,     6,     0,
       7,     0,     8,     0,     9,     3,     1,     0,     2,     1,
       0,     4,     0,     6,     2,     0,     3,     1,     3,     1,
       0,     2,     1,     0,     4,     0,     6,     2,     0,     3,
       1,     2,     1,     4,     0,     1,     0,     0,     5,     0,
       4,     1,     0,     2,     1,     1,     1,     0,     7,     3,
       1,     5,     2,     0,     2,     0,     2,     1,     6,     8,
       6,     8,     3,     1,     2,     1,     2,     1,     2,     2,
       1,     2,     3,     1,     2,     1,     2,     1,     2,     2,
       1,     2,     1,     1,     1,     2,     2,     3,     5,     2,
       3,     1,     1,     1,     6,     4,     3,     1,     6,     4,
       1,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       3,     3,     2,     3,     3,     5,     3,     4,     6,     8,
      10,     1,     1,     1,     1,     2,     3,     3,     2,     1,
       3,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     2,     2,     1,     0,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       4,     0,     2,     1,     0,     0,     0,     0,     0,     3,
       5,     6,    16,     7,    34,     8,    34,     9,    87,    13,
       0,    12,    23,    15,    27,     0,     0,     0,    17,     0,
      52,    33,    52,     0,     0,    86,    89,    10,     0,     0,
     228,   229,   214,   225,   226,   227,   177,     0,     0,     0,
       0,   219,     0,     0,     0,     0,   236,   172,   173,   171,
     180,   211,   212,   213,   222,   223,   224,   230,   236,    14,
       0,    35,     0,     0,    51,    70,     0,    90,   236,   167,
      88,    11,   236,   233,   234,     0,     0,     0,   215,     0,
     218,     0,     0,     0,   198,   202,   188,     0,   235,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   236,    21,   236,    38,    53,   236,    22,
       0,   116,    69,     0,    95,   169,   168,   206,     0,   176,
       0,     0,   216,   217,     0,   220,   181,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   199,   200,   201,   203,
     204,     0,   182,   183,   184,   185,   186,   187,   231,   232,
      20,    18,    41,     0,    36,     0,    40,    56,   236,    25,
      71,     0,    31,   115,    92,     0,    91,     0,    97,     0,
     207,     0,   175,     0,   179,   221,     0,     0,    43,    37,
       0,     0,     0,    54,    55,    24,    74,     0,   119,     0,
       0,    30,    95,     0,    94,     0,     0,     0,     0,   205,
      44,    42,    47,    48,    49,    50,    39,    64,     0,    62,
      68,     0,    66,     0,     0,     0,    72,    73,    76,   117,
     122,     0,   236,    26,    93,    98,    96,     0,   208,   174,
     178,    45,    46,     0,    58,     0,     0,    60,     0,     0,
       0,     0,    75,   122,   127,   150,   120,   121,   124,   125,
     126,     0,     0,   143,     0,   236,    29,     0,    63,    61,
      67,    65,    57,    59,   236,   118,     0,   151,     0,   149,
     123,     0,     0,     0,    32,    28,     0,   209,     0,   236,
      77,     0,     0,   130,     0,     0,   160,   133,     0,   153,
       0,   162,   164,   133,     0,   147,   142,     0,   236,     0,
      79,    87,     0,     0,     0,   170,   166,   161,   159,     0,
     135,     0,     0,   165,   135,     0,   148,   146,   210,    81,
     236,    87,    78,   100,     0,     0,   137,   129,   132,     0,
       0,   157,   152,   160,   133,     0,   133,    87,    83,    80,
       0,   114,    99,   102,     0,   236,   136,   134,   238,   237,
     140,   158,   156,   135,   138,   135,    82,    87,     0,   103,
       0,    85,   101,   131,   128,     0,     0,    84,     0,   108,
       0,   141,   139,   105,   112,   104,     0,   110,     0,   108,
     111,   107,     0,   113,   106,   109
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,     2,     9,    10,    20,    21,    11,    12,    27,
      28,    69,    13,    14,   129,    15,    16,   243,   210,   211,
      30,    31,   126,   174,   175,   176,    73,    74,   177,   203,
     204,   228,   229,   231,   232,   131,   132,   206,   236,   237,
     238,   321,   341,   357,   377,   342,   343,    35,    36,   134,
     212,   186,   187,   188,   361,   362,   363,   389,   399,   395,
     396,   397,   381,   182,   183,   263,   240,   266,   267,   268,
     269,   286,   302,   303,   330,   350,   345,   270,   271,   272,
     316,   273,   307,   308,   352,   309,   310,   311,   312,    17,
      18,    79,   328,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    99,   370
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -330
static const short int yypact[] =
{
    -330,    13,   134,  -330,    18,     0,    14,    61,    72,  -330,
    -330,  -330,     5,  -330,   103,  -330,   103,  -330,   112,  -330,
     -64,  -330,  -330,  -330,  -330,    39,   154,   118,   115,    64,
     146,  -330,   146,     2,   131,   112,  -330,  -330,    18,   156,
     182,   197,   132,  -330,  -330,  -330,   136,   137,    21,    82,
     138,   139,   233,   233,   233,   233,   140,  -330,  -330,   519,
    -330,  -330,  -330,  -330,  -330,  -330,  -330,   109,    -3,  -330,
     154,  -330,   117,   218,  -330,   235,   154,  -330,   140,  -330,
    -330,  -330,   140,  -330,  -330,    66,    80,   208,  -330,   212,
    -330,   213,   214,   147,  -330,  -330,  -330,   392,  -330,  -330,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   215,   219,   140,  -330,   140,    -6,  -330,     1,  -330,
     166,   244,  -330,   169,   224,  -330,  -330,  -330,   256,  -330,
     290,    23,  -330,  -330,   158,  -330,  -330,   196,   196,   196,
     196,   196,   196,   569,   569,   569,    87,    87,    87,   108,
     108,   494,    73,    73,   168,   168,   168,   168,  -330,  -330,
    -330,  -330,   175,   230,  -330,    54,  -330,   148,   140,  -330,
    -330,    -1,   257,  -330,  -330,   172,  -330,    77,  -330,   233,
    -330,   233,  -330,   208,  -330,  -330,   233,   180,  -330,  -330,
      -6,   245,   248,  -330,   171,  -330,   249,   203,  -330,   204,
     259,  -330,   224,   154,  -330,   224,   324,   426,   190,   544,
     251,   253,  -330,  -330,  -330,  -330,  -330,   199,   107,  -330,
     201,   110,  -330,   245,   248,   216,  -330,   249,  -330,  -330,
       9,   267,     6,  -330,  -330,  -330,  -330,   233,  -330,  -330,
    -330,  -330,  -330,   206,  -330,   245,   207,  -330,   248,   113,
     116,   269,  -330,     9,  -330,   -60,  -330,     9,  -330,  -330,
    -330,   198,   209,  -330,   211,   140,  -330,   358,  -330,  -330,
    -330,  -330,  -330,  -330,    -2,  -330,   275,  -330,   154,   210,
    -330,    98,    98,   279,  -330,  -330,   233,  -330,   280,   -10,
    -330,   217,   295,   221,   220,   154,    -9,   299,   223,  -330,
     305,   291,  -330,   299,   308,   -59,  -330,   460,   140,   310,
    -330,   112,   154,   317,   275,  -330,  -330,  -330,   238,   154,
     330,   318,   320,  -330,   330,   320,  -330,  -330,  -330,  -330,
     140,   112,  -330,   346,   337,    96,  -330,  -330,  -330,   322,
     123,   -39,  -330,   -44,   299,   123,   299,   112,  -330,  -330,
       3,   357,   346,  -330,   154,   140,  -330,  -330,  -330,  -330,
    -330,  -330,  -330,   330,  -330,   330,  -330,   112,   154,  -330,
     270,  -330,  -330,  -330,  -330,   123,   123,  -330,   285,   342,
     343,  -330,  -330,  -330,   287,  -330,   133,  -330,   292,   342,
    -330,  -330,   342,  -330,  -330,  -330
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -330,  -330,  -330,  -330,  -330,  -330,   348,  -330,  -330,  -330,
    -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,
     373,  -330,  -330,  -330,  -330,   191,   360,  -330,  -330,  -330,
    -330,   157,   152,   176,   145,  -330,  -330,  -330,  -330,  -330,
     183,  -330,  -330,  -330,  -330,  -315,   394,  -330,   384,  -330,
    -330,   225,  -330,   226,  -330,  -330,    60,  -330,  -330,    24,
    -330,    22,  -330,  -330,  -330,  -330,  -330,   162,  -330,   159,
    -330,  -330,   120,  -330,  -309,  -329,  -330,  -294,  -330,  -330,
    -330,  -330,  -207,  -330,  -330,  -330,   161,  -330,   135,  -330,
    -330,  -330,  -263,   -70,  -330,  -330,   -36,  -330,  -330,  -330,
    -330,   -84,  -330,  -330,  -330,   -67,  -250
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -164
static const short int yytable[] =
{
     125,   124,   289,   141,   334,   355,   133,  -163,  -163,   287,
     336,   135,   319,     3,   207,   136,    94,    95,    96,    97,
     298,   299,  -163,    76,   378,   327,   359,   264,   172,   346,
     371,   123,    37,    38,    22,   178,    26,   173,   288,   288,
     275,  -145,   376,   265,   385,   373,   386,   375,    23,   138,
     140,   366,   337,    19,   288,    88,   170,  -155,   171,   288,
     327,   179,   387,    89,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   313,    98,   208,   372,   288,
      77,   379,  -155,    98,    98,    24,   288,    98,    40,    41,
      42,    43,    98,    44,    45,   374,    25,    48,    49,   218,
      29,   205,    40,    41,    42,    43,    90,    44,    45,    33,
     193,    48,    49,    91,   365,   354,   194,    39,   356,   305,
     265,   400,   306,    50,    51,   391,   392,     4,     5,     6,
       7,   121,   122,   245,    68,    52,    70,    50,    51,    53,
     199,   200,    71,   216,    72,   217,    54,     8,    78,    52,
     219,   201,   202,    53,   117,   118,   119,   120,    55,   137,
      54,   112,   113,   214,   215,   276,   115,   116,   117,   118,
     119,   120,    55,   139,   233,   234,    40,    41,    42,    43,
      82,    44,    45,    46,    47,    48,    49,   115,   116,   117,
     118,   119,   120,   254,   255,   127,   257,   258,   295,   282,
     255,   277,   283,   258,   291,   292,    83,   300,   304,   368,
     369,    50,    51,   220,   221,   222,   223,   224,   225,   401,
     402,    84,   320,    52,    85,   326,    98,    53,    86,    87,
      92,    93,   128,    43,    54,   130,   142,   143,   144,   168,
     145,   339,   344,   169,   180,   181,    55,   184,   185,   348,
     317,   195,   120,   197,   198,    40,    41,    42,    43,   209,
      44,    45,   213,   358,    48,    49,   109,   110,   111,   227,
     112,   113,   230,   235,   242,   115,   116,   117,   118,   119,
     120,   239,   241,   250,   383,   251,   252,   253,   384,   256,
      50,    51,   274,   284,   261,   278,   280,   294,   388,   301,
     293,  -144,    52,   315,   318,   323,    53,   322,   324,   325,
     329,   332,   305,    54,   331,   335,   100,   101,   102,   103,
     104,   105,   106,   107,   108,    55,   109,   110,   111,  -154,
     112,   113,   349,   114,   340,   115,   116,   117,   118,   119,
     120,   265,   351,   189,   353,   360,   364,   367,   390,   190,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   380,
     109,   110,   111,   393,   112,   113,   394,   114,   398,   115,
     116,   117,   118,   119,   120,   288,    81,   191,   403,    32,
     259,   226,    75,   192,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   281,   109,   110,   111,   279,   112,   113,
     260,   114,    34,   115,   116,   117,   118,   119,   120,    80,
     262,   247,   382,   404,   405,   285,   290,   248,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   244,   109,   110,
     111,   246,   112,   113,   347,   114,   333,   115,   116,   117,
     118,   119,   120,   314,     0,   296,     0,     0,     0,     0,
       0,   297,   100,   101,   102,   103,   104,   105,   106,   107,
     108,     0,   109,   110,   111,     0,   112,   113,     0,   114,
       0,   115,   116,   117,   118,   119,   120,     0,     0,     0,
       0,     0,     0,     0,     0,   146,   100,   101,   102,   103,
     104,   105,   106,   107,   108,     0,   109,   110,   111,     0,
     112,   113,     0,   114,     0,   115,   116,   117,   118,   119,
     120,     0,     0,     0,     0,     0,     0,     0,     0,   249,
     100,   101,   102,   103,   104,   105,   106,   107,   108,     0,
     109,   110,   111,     0,   112,   113,     0,   114,     0,   115,
     116,   117,   118,   119,   120,     0,     0,     0,     0,     0,
       0,     0,     0,   338,   100,   101,   102,   103,   104,   105,
     106,   107,   108,     0,   109,   110,   111,     0,   112,   113,
       0,   114,   196,   115,   116,   117,   118,   119,   120,   100,
     101,   102,   103,   104,   105,   106,   107,   108,     0,   109,
     110,   111,     0,   112,   113,     0,   114,     0,   115,   116,
     117,   118,   119,   120,   100,   101,   102,   103,   104,   105,
     106,   107,   108,     0,   109,   110,   111,     0,   112,   113,
       0,     0,     0,   115,   116,   117,   118,   119,   120,   100,
     101,   102,   103,   104,   105,     0,     0,     0,     0,   109,
     110,   111,     0,   112,   113,     0,     0,     0,   115,   116,
     117,   118,   119,   120
};

static const short int yycheck[] =
{
      70,    68,   265,    87,   313,   334,    76,    16,    17,    69,
      69,    78,    22,     0,    15,    82,    52,    53,    54,    55,
      22,   284,    31,    21,    21,    69,   341,    18,    34,   323,
      69,    34,    96,    97,    34,    34,    31,    43,    98,    98,
      34,   101,   357,    34,   373,   354,   375,   356,    34,    85,
      86,   345,   315,    35,    98,    34,   123,   101,   125,    98,
      69,   128,   377,    42,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   292,    96,    88,   351,    98,
      88,    88,   101,    96,    96,    34,    98,    96,    32,    33,
      34,    35,    96,    37,    38,   355,    34,    41,    42,   193,
       7,   178,    32,    33,    34,    35,    34,    37,    38,     7,
      97,    41,    42,    41,    28,   332,   103,    88,   335,    31,
      34,   394,    34,    67,    68,   385,   386,     3,     4,     5,
       6,    32,    33,   213,    26,    79,    31,    67,    68,    83,
      96,    97,    88,   189,     8,   191,    90,    23,    27,    79,
     196,    13,    14,    83,    91,    92,    93,    94,   102,   103,
      90,    84,    85,    96,    97,   242,    89,    90,    91,    92,
      93,    94,   102,   103,    13,    14,    32,    33,    34,    35,
      34,    37,    38,    39,    40,    41,    42,    89,    90,    91,
      92,    93,    94,    96,    97,    88,    96,    97,   275,    96,
      97,   247,    96,    97,    16,    17,    34,   284,   288,    96,
      97,    67,    68,    43,    44,    45,    46,    47,    48,    96,
      97,    34,   299,    79,   102,   305,    96,    83,   102,   102,
     102,   102,    24,    35,    90,    10,    34,    34,    34,    34,
     103,   318,   322,    34,    88,    11,   102,    88,    34,   329,
     296,   103,    94,    88,    34,    32,    33,    34,    35,    12,
      37,    38,   100,   340,    41,    42,    80,    81,    82,    34,
      84,    85,    34,    34,    25,    89,    90,    91,    92,    93,
      94,    88,    88,   103,   364,    44,    43,    98,   365,    98,
      67,    68,    35,    34,    88,    99,    99,    96,   378,    34,
     101,   101,    79,    34,    34,    20,    83,   100,    97,    99,
      21,    16,    31,    90,   101,    17,    70,    71,    72,    73,
      74,    75,    76,    77,    78,   102,    80,    81,    82,   101,
      84,    85,    12,    87,    34,    89,    90,    91,    92,    93,
      94,    34,    34,    97,    34,     9,    19,    35,    88,   103,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    12,
      80,    81,    82,    88,    84,    85,    34,    87,    35,    89,
      90,    91,    92,    93,    94,    98,    38,    97,    96,    16,
     233,   200,    32,   103,    70,    71,    72,    73,    74,    75,
      76,    77,    78,   258,    80,    81,    82,   255,    84,    85,
     234,    87,    18,    89,    90,    91,    92,    93,    94,    35,
     237,    97,   362,   399,   402,   263,   267,   103,    70,    71,
      72,    73,    74,    75,    76,    77,    78,   212,    80,    81,
      82,   215,    84,    85,   324,    87,   311,    89,    90,    91,
      92,    93,    94,   292,    -1,    97,    -1,    -1,    -1,    -1,
      -1,   103,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    -1,    80,    81,    82,    -1,    84,    85,    -1,    87,
      -1,    89,    90,    91,    92,    93,    94,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   103,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    -1,    80,    81,    82,    -1,
      84,    85,    -1,    87,    -1,    89,    90,    91,    92,    93,
      94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   103,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    -1,
      80,    81,    82,    -1,    84,    85,    -1,    87,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   103,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    -1,    80,    81,    82,    -1,    84,    85,
      -1,    87,    88,    89,    90,    91,    92,    93,    94,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    -1,    80,
      81,    82,    -1,    84,    85,    -1,    87,    -1,    89,    90,
      91,    92,    93,    94,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    -1,    80,    81,    82,    -1,    84,    85,
      -1,    -1,    -1,    89,    90,    91,    92,    93,    94,    70,
      71,    72,    73,    74,    75,    -1,    -1,    -1,    -1,    80,
      81,    82,    -1,    84,    85,    -1,    -1,    -1,    89,    90,
      91,    92,    93,    94
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,   105,   106,     0,     3,     4,     5,     6,    23,   107,
     108,   111,   112,   116,   117,   119,   120,   193,   194,    35,
     109,   110,    34,    34,    34,    34,    31,   113,   114,     7,
     124,   125,   124,     7,   150,   151,   152,    96,    97,    88,
      32,    33,    34,    35,    37,    38,    39,    40,    41,    42,
      67,    68,    79,    83,    90,   102,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,    26,   115,
      31,    88,     8,   130,   131,   130,    21,    88,    27,   195,
     152,   110,    34,    34,    34,   102,   102,   102,    34,    42,
      34,    41,   102,   102,   200,   200,   200,   200,    96,   209,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    80,
      81,    82,    84,    85,    87,    89,    90,    91,    92,    93,
      94,    32,    33,    34,   209,   197,   126,    88,    24,   118,
      10,   139,   140,   197,   153,   209,   209,   103,   200,   103,
     200,   205,    34,    34,    34,   103,   103,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,    34,    34,
     209,   209,    34,    43,   127,   128,   129,   132,    34,   209,
      88,    11,   167,   168,    88,    34,   155,   156,   157,    97,
     103,    97,   103,    97,   103,   103,    88,    88,    34,    96,
      97,    13,    14,   133,   134,   209,   141,    15,    88,    12,
     122,   123,   154,   100,    96,    97,   200,   200,   205,   200,
      43,    44,    45,    46,    47,    48,   129,    34,   135,   136,
      34,   137,   138,    13,    14,    34,   142,   143,   144,    88,
     170,    88,    25,   121,   155,   197,   157,    97,   103,   103,
     103,    44,    43,    98,    96,    97,    98,    96,    97,   135,
     137,    88,   144,   169,    18,    34,   171,   172,   173,   174,
     181,   182,   183,   185,    35,    34,   209,   200,    99,   136,
      99,   138,    96,    96,    34,   171,   175,    69,    98,   196,
     173,    16,    17,   101,    96,   209,    97,   103,    22,   196,
     209,    34,   176,   177,   197,    31,    34,   186,   187,   189,
     190,   191,   192,   186,   190,    34,   184,   200,    34,    22,
     209,   145,   100,    20,    97,    99,   197,    69,   196,    21,
     178,   101,    16,   192,   178,    17,    69,   196,   103,   209,
      34,   146,   149,   150,   197,   180,   181,   176,   197,    12,
     179,    34,   188,    34,   186,   179,   186,   147,   209,   149,
       9,   158,   159,   160,    19,    28,   181,    35,    96,    97,
     210,    69,   196,   178,   210,   178,   149,   148,    21,    88,
      12,   166,   160,   197,   209,   179,   179,   149,   197,   161,
      88,   210,   210,    88,    34,   163,   164,   165,    35,   162,
     196,    96,    97,    96,   163,   165
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
        case 6:
#line 191 "ned.y"
    { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.channel, (yylsp[0])); ;}
    break;

  case 7:
#line 193 "ned.y"
    { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.module, (yylsp[0])); ;}
    break;

  case 8:
#line 195 "ned.y"
    { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.module, (yylsp[0])); ;}
    break;

  case 9:
#line 197 "ned.y"
    { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.module, (yylsp[0])); ;}
    break;

  case 13:
#line 215 "ned.y"
    {
                  ps.import = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  ps.import->setFilename(toString(trimQuotes((yylsp[0]))));
                  setComments(ps.import,(yylsp[0]));
                  storePos(ps.import, (yyloc));
                ;}
    break;

  case 14:
#line 228 "ned.y"
    { storePos(ps.channel, (yyloc)); ;}
    break;

  case 15:
#line 233 "ned.y"
    {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile);
                  ps.channel->setName(toString((yylsp[0])));
                  ps.params = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.channel);
                  ps.params->setIsImplicit(true);
                  setComments(ps.channel,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 17:
#line 245 "ned.y"
    { storePos(ps.params, (yyloc)); ;}
    break;

  case 18:
#line 250 "ned.y"
    {
                  ps.params->setIsImplicit(false);
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  addExpression(ps.param, "value",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.param,(yylsp[-2]),(yylsp[-1]));
                  storePos(ps.param, (yylsp[-2])); // XXX rather: @2..@4
                ;}
    break;

  case 19:
#line 258 "ned.y"
    {
                  ps.params->setIsImplicit(false);
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  addExpression(ps.param, "value",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.param,(yylsp[-2]),(yylsp[-1]));
                  storePos(ps.param, (yyloc));
                ;}
    break;

  case 20:
#line 269 "ned.y"
    {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                ;}
    break;

  case 21:
#line 273 "ned.y"
    {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                ;}
    break;

  case 22:
#line 286 "ned.y"
    { storePos(ps.module, (yyloc)); ;}
    break;

  case 23:
#line 291 "ned.y"
    {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 24:
#line 300 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 25:
#line 304 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 26:
#line 320 "ned.y"
    { storePos(ps.module, (yyloc)); ;}
    break;

  case 27:
#line 325 "ned.y"
    {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 28:
#line 334 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 29:
#line 338 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 32:
#line 353 "ned.y"
    {
                  ps.property = addComponentProperty(ps.module, "display");
                  ps.params = (ParametersNode *)ps.module->getFirstChildWithTag(NED_PARAMETERS); // previous line doesn't set it
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[-1])), (yylsp[-1]));
                  ps.propkey->appendChild(literal);
                  storePos(ps.propkey, (yyloc));
                  storePos(literal, (yylsp[-1]));
                  storePos(ps.property, (yyloc));

                  // move parameters section in front of potential gates section
                  if (ps.params && ps.module->getFirstChild()!=ps.params)
                  {
                      assert(ps.params->getParent()==ps.module);
                      ps.module->removeChild(ps.params);
                      ps.module->insertChildBefore(ps.module->getFirstChild(), ps.params);
                  }
                ;}
    break;

  case 35:
#line 383 "ned.y"
    {
                  ps.params = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.module );
                  setComments(ps.params,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 36:
#line 388 "ned.y"
    {
                  storePos(ps.params, (yyloc));
                ;}
    break;

  case 39:
#line 400 "ned.y"
    {
                  setComments(ps.param,(yylsp[0]));
                ;}
    break;

  case 40:
#line 404 "ned.y"
    {
                  setComments(ps.param,(yylsp[0]));
                ;}
    break;

  case 41:
#line 414 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[0]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  ps.param->setIsFunction(true); // because CONST is missing
                  storePos(ps.param, (yyloc));
                ;}
    break;

  case 42:
#line 421 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  ps.param->setIsFunction(true); // because CONST is missing
                  storePos(ps.param, (yyloc));
                ;}
    break;

  case 43:
#line 428 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[0]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  storePos(ps.param, (yyloc));
                ;}
    break;

  case 44:
#line 434 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  storePos(ps.param, (yyloc));
                ;}
    break;

  case 45:
#line 440 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-3]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  storePos(ps.param, (yyloc));
                ;}
    break;

  case 46:
#line 446 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-3]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  storePos(ps.param, (yyloc));
                ;}
    break;

  case 47:
#line 452 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_STRING);
                  storePos(ps.param, (yyloc));
                ;}
    break;

  case 48:
#line 458 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_BOOL);
                  storePos(ps.param, (yyloc));
                ;}
    break;

  case 49:
#line 464 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_XML);
                  storePos(ps.param, (yyloc));
                ;}
    break;

  case 50:
#line 470 "ned.y"
    {
                  np->getErrors()->add(ps.params,"type 'anytype' no longer supported");
                ;}
    break;

  case 53:
#line 485 "ned.y"
    {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 54:
#line 490 "ned.y"
    {
                  storePos(ps.gates, (yyloc));
                ;}
    break;

  case 63:
#line 514 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[-2]));
                  ps.gate->setType(NED_GATETYPE_INPUT);
                  ps.gate->setIsVector(true);
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                  storePos(ps.gate, (yyloc));
                ;}
    break;

  case 64:
#line 522 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[0]));
                  ps.gate->setType(NED_GATETYPE_INPUT);
                  setComments(ps.gate,(yylsp[0]));
                  storePos(ps.gate, (yyloc));
                ;}
    break;

  case 67:
#line 537 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[-2]));
                  ps.gate->setType(NED_GATETYPE_OUTPUT);
                  ps.gate->setIsVector(true);
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                  storePos(ps.gate, (yyloc));
                ;}
    break;

  case 68:
#line 545 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[0]));
                  ps.gate->setType(NED_GATETYPE_OUTPUT);
                  setComments(ps.gate,(yylsp[0]),(yylsp[0]));
                  storePos(ps.gate, (yyloc));
                ;}
    break;

  case 71:
#line 563 "ned.y"
    {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 72:
#line 568 "ned.y"
    {
                  storePos(ps.submods, (yyloc));
                ;}
    break;

  case 77:
#line 585 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-3])));
                  ps.submod->setType(toString((yylsp[-1])));
                  setComments(ps.submod,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 78:
#line 592 "ned.y"
    {
                  storePos(ps.submod, (yyloc));
                ;}
    break;

  case 79:
#line 596 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-4])));
                  ps.submod->setType(toString((yylsp[-2])));
                  addVector(ps.submod, "vector-size",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.submod,(yylsp[-4]),(yylsp[0]));
                ;}
    break;

  case 80:
#line 604 "ned.y"
    {
                  storePos(ps.submod, (yyloc));
                ;}
    break;

  case 81:
#line 608 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-5])));
                  ps.submod->setLikeType(toString((yylsp[-1])));
                  ps.submod->setLikeParam(toString((yylsp[-3]))); //FIXME store as expression!!!
                  setComments(ps.submod,(yylsp[-5]),(yylsp[0]));
                ;}
    break;

  case 82:
#line 616 "ned.y"
    {
                  storePos(ps.submod, (yyloc));
                ;}
    break;

  case 83:
#line 620 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-6])));
                  ps.submod->setLikeType(toString((yylsp[-1])));
                  ps.submod->setLikeParam(toString((yylsp[-4]))); //FIXME store as expression!!!
                  addVector(ps.submod, "vector-size",(yylsp[-3]),(yyvsp[-3]));
                  setComments(ps.submod,(yylsp[-6]),(yylsp[0]));
                ;}
    break;

  case 84:
#line 629 "ned.y"
    {
                  storePos(ps.submod, (yyloc));
                ;}
    break;

  case 86:
#line 645 "ned.y"
    { storePos(ps.substparams, (yyloc)); /*must do it here because there might be multiple (conditional) gatesizes/parameters sections */ ;}
    break;

  case 90:
#line 656 "ned.y"
    {
                  createSubstparamsNodeIfNotExists();
                  setComments(ps.substparams,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 91:
#line 661 "ned.y"
    {
                ;}
    break;

  case 92:
#line 664 "ned.y"
    {
                  // make conditional paramgroup
                  createSubstparamsNodeIfNotExists();
                  ps.substparamgroup = (ParamGroupNode *)createNodeWithTag(NED_PARAM_GROUP, ps.substparams);
                  ps.inGroup = true;
                  setComments(ps.substparamgroup,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 93:
#line 672 "ned.y"
    {
                  ps.condition = (ConditionNode *)createNodeWithTag(NED_CONDITION, ps.substparamgroup);
                  addExpression(ps.condition, "condition",(yylsp[-3]),(yyvsp[-3]));
                  storePos(ps.substparamgroup, (yyloc));
                  ps.inGroup = false;
                ;}
    break;

  case 98:
#line 693 "ned.y"
    {
                  NEDElement *parent = ps.inGroup ? (NEDElement *)ps.substparamgroup : (NEDElement *)ps.substparams;
                  ps.substparam = addParameter(parent,(yylsp[-2]));
                  addExpression(ps.substparam, "value",(yylsp[0]),(yyvsp[0]));
                  setComments(ps.substparam,(yylsp[-2]),(yylsp[0]));
                  storePos(ps.substparam, (yyloc));
                ;}
    break;

  case 99:
#line 707 "ned.y"
    { storePos(ps.gatesizes, (yyloc)); /*must do it here because there might be multiple (conditional) gatesizes/parameters sections */ ;}
    break;

  case 103:
#line 718 "ned.y"
    {
                  createGatesizesNodeIfNotExists();
                  setComments(ps.gatesizes,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 104:
#line 723 "ned.y"
    {
                ;}
    break;

  case 105:
#line 726 "ned.y"
    {
                  // make conditional gategroup
                  createGatesizesNodeIfNotExists();
                  ps.gatesizesgroup = (GateGroupNode *)createNodeWithTag(NED_GATE_GROUP, ps.gatesizes);
                  ps.inGroup = true;
                  setComments(ps.gatesizesgroup,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 106:
#line 734 "ned.y"
    {
                  ps.condition = (ConditionNode *)createNodeWithTag(NED_CONDITION, ps.gatesizesgroup);
                  addExpression(ps.condition, "condition",(yylsp[-3]),(yyvsp[-3]));
                  ps.inGroup = false;
                  storePos(ps.gatesizesgroup, (yyloc));
                ;}
    break;

  case 111:
#line 754 "ned.y"
    {
                  NEDElement *parent = ps.inGroup ? (NEDElement *)ps.gatesizesgroup : (NEDElement *)ps.gatesizes;
                  ps.gatesize = addGate(parent,(yylsp[-1]));
                  addVector(ps.gatesize, "vector-size",(yylsp[0]),(yyvsp[0]));
                  setComments(ps.gatesize,(yylsp[-1]),(yylsp[0]));
                  storePos(ps.gatesize, (yyloc));
                ;}
    break;

  case 112:
#line 762 "ned.y"
    {
                  ps.gatesize = addGate(ps.gatesizes,(yylsp[0]));
                  setComments(ps.gatesize,(yylsp[0]));
                  storePos(ps.gatesize, (yyloc));
                ;}
    break;

  case 113:
#line 774 "ned.y"
    {
                  ps.property = addComponentProperty(ps.submod, "display");
                  ps.substparams = (ParametersNode *)ps.submod->getFirstChildWithTag(NED_PARAMETERS); // previous line doesn't set it
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[-1])), (yylsp[-1]));
                  ps.propkey->appendChild(literal);
                  storePos(ps.propkey, (yyloc));
                  storePos(literal, (yylsp[-1]));
                  storePos(ps.property, (yyloc));

                  // move parameters section in front of potential gatesizes section
                  if (ps.substparams && ps.submod->getFirstChild()!=ps.substparams)
                  {
                      assert(ps.substparams->getParent()==ps.submod);
                      ps.submod->removeChild(ps.substparams);
                      ps.submod->insertChildBefore(ps.submod->getFirstChild(), ps.substparams);
                  }
                ;}
    break;

  case 117:
#line 805 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setAllowUnconnected(true);
                  setComments(ps.conns,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 118:
#line 811 "ned.y"
    {
                  storePos(ps.conns, (yyloc));
                ;}
    break;

  case 119:
#line 815 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setAllowUnconnected(false);
                  setComments(ps.conns,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 120:
#line 821 "ned.y"
    {
                  storePos(ps.conns, (yyloc));
                ;}
    break;

  case 127:
#line 843 "ned.y"
    {
                  ps.conngroup = (ConnectionGroupNode *)createNodeWithTag(NED_CONNECTION_GROUP, ps.conns);
                  ps.where = (WhereNode *)createNodeWithTag(NED_WHERE, ps.conngroup);
                  ps.inLoop=1;
                ;}
    break;

  case 128:
#line 849 "ned.y"
    {
                  ps.inLoop=0;
                  setComments(ps.where,(yylsp[-6]),(yylsp[-3]));
                  //setTrailingComment(ps.where,@6);
                  storePos(ps.where, (yylsp[-4]));

                  // optimize: if there's exactly one connection inside the loop, eliminate conngroup
                  if (ps.conngroup->getNumChildrenWithTag(NED_CONNECTION)==1)
                  {
                      ps.conngroup->removeChild(ps.conn);
                      ps.conns->insertChildBefore(ps.conngroup, ps.conn);
                      ps.conn->appendChild(ps.conngroup->removeChild(ps.where));
                      delete ps.conns->removeChild(ps.conngroup);
                      storePos(ps.conn, (yyloc));
                  }
                  else
                  {
                      // move ps.where to the end
                      ps.conngroup->appendChild(ps.conngroup->removeChild(ps.where));
                      ps.where->setAtFront(true);
                      storePos(ps.conngroup, (yyloc));

                      // we're only prepared for "for" loops with 1 connection inside
                      if (ps.where->getNumChildrenWithTag(NED_CONDITION)!=0)
                          np->getErrors()->add(ps.conngroup, "cannot process NED-I syntax of several "
                                   "conditional connections within a `for' loop -- "
                                   "please split it to several `for' loops");
                  }
                ;}
    break;

  case 131:
#line 887 "ned.y"
    {
                  ps.loop = addLoop(ps.where,(yylsp[-4]));
                  addExpression(ps.loop, "from-value",(yylsp[-2]),(yyvsp[-2]));
                  addExpression(ps.loop, "to-value",(yylsp[0]),(yyvsp[0]));
                  setComments(ps.loop,(yylsp[-4]),(yylsp[0]));
                  storePos(ps.loop, (yyloc));
                ;}
    break;

  case 132:
#line 898 "ned.y"
    {
                  if (!ps.inLoop)
                  {
                      // add where+condition to conn
                      ps.where = (WhereNode *)createNodeWithTag(NED_WHERE, ps.conn);
                      ps.condition = (ConditionNode *)createNodeWithTag(NED_CONDITION, ps.where);
                      addExpression(ps.condition, "condition",(yylsp[0]),(yyvsp[0]));
                      storePos(ps.where, (yyloc));
                      storePos(ps.condition, (yyloc));
                  }
                  else
                  {
                      // inside a for loop: append condition to its "where".
                      // at the end we'll need to make sure there's only one connection in the loop!
                      // (otherwise we'd have to check all conns have exactly the same condition)
                      ps.condition = (ConditionNode *)createNodeWithTag(NED_CONDITION, ps.where);
                      addExpression(ps.condition, "condition",(yylsp[0]),(yyvsp[0]));
                      storePos(ps.condition, (yyloc));
                  }
                ;}
    break;

  case 134:
#line 923 "ned.y"
    {
                  bool hadChanSpec = ps.chanspec!=NULL;
                  if (!ps.chanspec)
                      ps.chanspec = createChannelSpec(ps.conn);
                  ps.property = addComponentProperty(ps.chanspec, "display");
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[0])), (yylsp[0]));
                  ps.propkey->appendChild(literal);
                  storePos(ps.propkey, (yyloc));
                  storePos(literal, (yylsp[0]));
                  storePos(ps.property, (yyloc));
                  if (!hadChanSpec)
                      storePos(ps.chanspec, (yyloc));
                ;}
    break;

  case 138:
#line 947 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                  storePos(ps.conn, (yyloc));
                ;}
    break;

  case 139:
#line 953 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                  removeRedundantChanSpecParams();
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                  storePos(ps.conn, (yyloc));
                ;}
    break;

  case 140:
#line 960 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                  storePos(ps.conn, (yyloc));
                ;}
    break;

  case 141:
#line 967 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                  removeRedundantChanSpecParams();
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                  storePos(ps.conn, (yyloc));
                ;}
    break;

  case 144:
#line 983 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "src-module-index",(yylsp[0]),(yyvsp[0]));
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                ;}
    break;

  case 145:
#line 990 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[0])) );
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                ;}
    break;

  case 146:
#line 999 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 147:
#line 1004 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[0])) );
                ;}
    break;

  case 148:
#line 1008 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  ps.conn->setSrcGatePlusplus(true);
                ;}
    break;

  case 149:
#line 1016 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),(yyvsp[0]));
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                ;}
    break;

  case 150:
#line 1024 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[0])));
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                ;}
    break;

  case 151:
#line 1031 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  ps.conn->setSrcGatePlusplus(true);
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                ;}
    break;

  case 154:
#line 1047 "ned.y"
    {
                  ps.conn->setDestModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "dest-module-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 155:
#line 1052 "ned.y"
    {
                  ps.conn->setDestModule( toString((yylsp[0])) );
                ;}
    break;

  case 156:
#line 1059 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 157:
#line 1064 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                ;}
    break;

  case 158:
#line 1068 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                ;}
    break;

  case 159:
#line 1076 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 160:
#line 1081 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                ;}
    break;

  case 161:
#line 1085 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                ;}
    break;

  case 162:
#line 1094 "ned.y"
    {
                  storePos(ps.chanspec, (yyloc));
                  if (ps.chanspec->getFirstChildWithTag(NED_PARAMETERS)!=NULL)
                      storePos(ps.params, (yyloc));
                ;}
    break;

  case 163:
#line 1103 "ned.y"
    {
                  if (!ps.chanspec)
                      ps.chanspec = createChannelSpec(ps.conn);
                  ps.chanspec->setType(toString((yylsp[0])));
                ;}
    break;

  case 166:
#line 1114 "ned.y"
    {
                  if (!ps.chanspec)
                      ps.chanspec = createChannelSpec(ps.conn);
                  ps.param = addParameter(ps.params, (yylsp[-1]));
                  addExpression(ps.param, "value",(yylsp[0]),(yyvsp[0]));
                  storePos(ps.param, (yyloc));
                ;}
    break;

  case 167:
#line 1130 "ned.y"
    { storePos(ps.module, (yyloc)); ;}
    break;

  case 168:
#line 1135 "ned.y"
    {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString((yylsp[-3])));
                  ((CompoundModuleNode *)ps.module)->setIsNetwork(true);
                  ps.extends = (ExtendsNode *)createNodeWithTag(NED_EXTENDS, ps.module);
                  ps.extends->setName(toString((yylsp[-1])));
                  setComments(ps.module,(yylsp[-4]),(yylsp[0]));
                  storePos(ps.extends, (yylsp[-1]));
                  ps.inNetwork=1;
                ;}
    break;

  case 169:
#line 1149 "ned.y"
    {
                  //setTrailingComment(ps.module,@1);
                  ps.inNetwork=0;
                ;}
    break;

  case 170:
#line 1160 "ned.y"
    { (yyval) = (yyvsp[-1]); ;}
    break;

  case 171:
#line 1166 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 172:
#line 1170 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 173:
#line 1174 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 174:
#line 1185 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("input", (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 175:
#line 1187 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("input", (yyvsp[-1])); ;}
    break;

  case 176:
#line 1189 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("input"); ;}
    break;

  case 177:
#line 1191 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("input"); ;}
    break;

  case 178:
#line 1196 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("xmldoc", (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 179:
#line 1198 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("xmldoc", (yyvsp[-1])); ;}
    break;

  case 181:
#line 1204 "ned.y"
    { (yyval) = (yyvsp[-1]); ;}
    break;

  case 182:
#line 1207 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("+", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 183:
#line 1209 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("-", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 184:
#line 1211 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("*", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 185:
#line 1213 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("/", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 186:
#line 1215 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("%", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 187:
#line 1217 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("^", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 188:
#line 1221 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = unaryMinus((yyvsp[0])); ;}
    break;

  case 189:
#line 1224 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("==", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 190:
#line 1226 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("!=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 191:
#line 1228 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator(">", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 192:
#line 1230 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator(">=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 193:
#line 1232 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("<", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 194:
#line 1234 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("<=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 195:
#line 1237 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("&&", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 196:
#line 1239 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("||", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 197:
#line 1241 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("##", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 198:
#line 1245 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("!", (yyvsp[0])); ;}
    break;

  case 199:
#line 1248 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("&", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 200:
#line 1250 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("|", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 201:
#line 1252 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("#", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 202:
#line 1256 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("~", (yyvsp[0])); ;}
    break;

  case 203:
#line 1258 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("<<", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 204:
#line 1260 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator(">>", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 205:
#line 1262 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("?:", (yyvsp[-4]), (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 206:
#line 1265 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction(toString((yylsp[-2]))); ;}
    break;

  case 207:
#line 1267 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction(toString((yylsp[-3])), (yyvsp[-1])); ;}
    break;

  case 208:
#line 1269 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction(toString((yylsp[-5])), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 209:
#line 1271 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction(toString((yylsp[-7])), (yyvsp[-5]), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 210:
#line 1273 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction(toString((yylsp[-9])), (yyvsp[-7]), (yyvsp[-5]), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 214:
#line 1284 "ned.y"
    {
                  // if there's no modifier, might be a loop variable too
                  if (np->getParseExpressionsFlag()) (yyval) = createIdent((yylsp[0]));
                ;}
    break;

  case 215:
#line 1289 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createIdent((yylsp[0]));
                  np->getErrors()->add(ps.params,"`ref' modifier no longer supported (add `function' "
                                     "modifier to destination parameter instead)");
                ;}
    break;

  case 216:
#line 1295 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createIdent((yylsp[0]));
                  np->getErrors()->add(ps.params,"`ancestor' and `ref' modifiers no longer supported");
                ;}
    break;

  case 217:
#line 1300 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createIdent((yylsp[0]));
                  np->getErrors()->add(ps.params,"`ancestor' and `ref' modifiers no longer supported");
                ;}
    break;

  case 218:
#line 1305 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createIdent((yylsp[0]));
                  np->getErrors()->add(ps.params,"`ancestor' modifier no longer supported");
                ;}
    break;

  case 219:
#line 1313 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("index"); ;}
    break;

  case 220:
#line 1315 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("index"); ;}
    break;

  case 221:
#line 1317 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("sizeof", createIdent((yylsp[-1]))); ;}
    break;

  case 225:
#line 1328 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[0])), (yylsp[0])); ;}
    break;

  case 226:
#line 1333 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createLiteral(NED_CONST_BOOL, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 227:
#line 1335 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createLiteral(NED_CONST_BOOL, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 228:
#line 1340 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createLiteral(NED_CONST_INT, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 229:
#line 1342 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createLiteral(NED_CONST_DOUBLE, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 230:
#line 1344 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createQuantity(toString((yylsp[0]))); ;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 3097 "ned.tab.c"

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


#line 1358 "ned.y"


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
    ps.nedfile->setVersion("1");

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

void createSubstparamsNodeIfNotExists()
{
   // check if already exists (multiple blocks must be merged)
   NEDElement *parent = ps.inNetwork ? (NEDElement *)ps.module : (NEDElement *)ps.submod;
   ps.substparams = (ParametersNode *)parent->getFirstChildWithTag(NED_PARAMETERS);
   if (!ps.substparams)
       ps.substparams = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, parent);
}

void createGatesizesNodeIfNotExists()
{
   // check if already exists (multiple blocks must be merged)
   ps.gatesizes = (GatesNode *)ps.submod->getFirstChildWithTag(NED_GATES);
   if (!ps.gatesizes)
       ps.gatesizes = (GatesNode *)createNodeWithTag(NED_GATES, ps.submod);
}

void removeRedundantChanSpecParams()
{
    if (ps.chanspec && !ps.params->getFirstChild())
        delete ps.chanspec->removeChild(ps.params);
}


