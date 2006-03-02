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
#line 384 "ned.tab.c"

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
#define YYLAST   633

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  104
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  104
/* YYNRULES -- Number of rules. */
#define YYNRULES  234
/* YYNRULES -- Number of states. */
#define YYNSTATES  403

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
     308,   309,   310,   315,   316,   323,   326,   327,   331,   333,
     336,   338,   343,   344,   346,   347,   348,   354,   355,   360,
     362,   363,   366,   368,   370,   372,   373,   381,   385,   387,
     393,   396,   397,   400,   401,   404,   406,   413,   422,   429,
     438,   442,   444,   447,   449,   452,   454,   457,   460,   462,
     465,   469,   471,   474,   476,   479,   481,   484,   487,   489,
     492,   494,   497,   501,   505,   511,   514,   518,   520,   522,
     524,   531,   536,   540,   542,   549,   554,   556,   560,   564,
     568,   572,   576,   580,   584,   587,   591,   595,   599,   603,
     607,   611,   615,   619,   623,   626,   630,   634,   638,   641,
     645,   649,   655,   659,   664,   671,   680,   691,   693,   695,
     697,   699,   702,   706,   710,   713,   715,   719,   724,   726,
     728,   730,   732,   734,   736,   738,   740,   742,   746,   750,
     753,   756,   758,   759,   761
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     105,     0,    -1,   106,    -1,   106,   107,    -1,    -1,   108,
      -1,   111,    -1,   116,    -1,   119,    -1,   190,    -1,     3,
     109,    96,    -1,   109,    97,   110,    -1,   110,    -1,    35,
      -1,   112,   113,   115,    -1,     5,    34,    -1,    -1,   114,
      -1,   114,    31,   194,   206,    -1,    31,   194,   206,    -1,
      26,    34,   206,    -1,    26,   206,    -1,   117,   124,   130,
     118,    -1,     4,    34,    -1,    24,    34,   206,    -1,    24,
     206,    -1,   120,   124,   130,   139,   166,   122,   121,    -1,
       6,    34,    -1,    25,    34,   206,    -1,    25,   206,    -1,
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
      34,   206,   145,   149,    -1,    -1,    34,    88,    34,   193,
     206,   146,   149,    -1,    -1,    34,    88,    34,    22,    34,
     206,   147,   149,    -1,    -1,    34,    88,    34,   193,    22,
      34,   206,   148,   149,    -1,   150,   158,   165,    -1,   151,
      -1,    -1,   151,   152,    -1,   152,    -1,    -1,     7,    88,
     153,   155,    -1,    -1,     7,    21,   194,    88,   154,   155,
      -1,   156,    96,    -1,    -1,   156,    97,   157,    -1,   157,
      -1,    34,   100,   194,    -1,   158,   159,    -1,    -1,    -1,
       9,    88,   160,   162,    -1,    -1,     9,    21,   194,    88,
     161,   162,    -1,   163,    96,    -1,    -1,   163,    97,   164,
      -1,   164,    -1,    34,   193,    -1,    34,    -1,    12,    88,
      35,    96,    -1,    -1,   167,    -1,    -1,    -1,    11,    15,
      88,   168,   170,    -1,    -1,    11,    88,   169,   170,    -1,
     171,    -1,    -1,   171,   172,    -1,   172,    -1,   173,    -1,
     180,    -1,    -1,    18,   174,   175,    20,   179,    28,   206,
      -1,   176,    97,   175,    -1,   176,    -1,    34,   100,   194,
      19,   194,    -1,    21,   194,    -1,    -1,    12,    35,    -1,
      -1,   179,   180,    -1,   180,    -1,   181,    17,   185,   177,
     178,   207,    -1,   181,    17,   189,    17,   185,   177,   178,
     207,    -1,   181,    16,   185,   177,   178,   207,    -1,   181,
      16,   189,    16,   185,   177,   178,   207,    -1,   182,   101,
     183,    -1,   184,    -1,    34,   193,    -1,    34,    -1,    34,
     193,    -1,    34,    -1,    34,    69,    -1,    34,   193,    -1,
      34,    -1,    34,    69,    -1,   186,   101,   187,    -1,   188,
      -1,    34,   193,    -1,    34,    -1,    34,   193,    -1,    34,
      -1,    34,    69,    -1,    34,   193,    -1,    34,    -1,    34,
      69,    -1,    34,    -1,    31,   194,    -1,   189,    31,   194,
      -1,   191,   150,   192,    -1,    23,    34,    88,    34,   206,
      -1,    27,   206,    -1,    98,   194,    99,    -1,   197,    -1,
     195,    -1,   196,    -1,    39,   102,   197,    97,   197,   103,
      -1,    39,   102,   197,   103,    -1,    39,   102,   103,    -1,
      39,    -1,    40,   102,   202,    97,   202,   103,    -1,    40,
     102,   202,   103,    -1,   198,    -1,   102,   197,   103,    -1,
     197,    89,   197,    -1,   197,    90,   197,    -1,   197,    91,
     197,    -1,   197,    92,   197,    -1,   197,    93,   197,    -1,
     197,    94,   197,    -1,    90,   197,    -1,   197,    70,   197,
      -1,   197,    71,   197,    -1,   197,    72,   197,    -1,   197,
      73,   197,    -1,   197,    74,   197,    -1,   197,    75,   197,
      -1,   197,    76,   197,    -1,   197,    77,   197,    -1,   197,
      78,   197,    -1,    79,   197,    -1,   197,    80,   197,    -1,
     197,    81,   197,    -1,   197,    82,   197,    -1,    83,   197,
      -1,   197,    84,   197,    -1,   197,    85,   197,    -1,   197,
      87,   197,    88,   197,    -1,    34,   102,   103,    -1,    34,
     102,   197,   103,    -1,    34,   102,   197,    97,   197,   103,
      -1,    34,   102,   197,    97,   197,    97,   197,   103,    -1,
      34,   102,   197,    97,   197,    97,   197,    97,   197,   103,
      -1,   199,    -1,   200,    -1,   201,    -1,    34,    -1,    41,
      34,    -1,    41,    42,    34,    -1,    42,    41,    34,    -1,
      42,    34,    -1,    68,    -1,    68,   102,   103,    -1,    67,
     102,    34,   103,    -1,   202,    -1,   203,    -1,   204,    -1,
      35,    -1,    37,    -1,    38,    -1,    32,    -1,    33,    -1,
     205,    -1,   205,    32,    34,    -1,   205,    33,    34,    -1,
      32,    34,    -1,    33,    34,    -1,    96,    -1,    -1,    97,
      -1,    96,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   174,   174,   178,   179,   183,   185,   187,   189,   191,
     199,   204,   205,   209,   221,   225,   235,   237,   241,   248,
     258,   262,   272,   279,   288,   292,   302,   312,   321,   325,
     335,   336,   340,   353,   354,   359,   358,   369,   370,   374,
     378,   388,   394,   400,   405,   410,   415,   420,   425,   430,
     435,   445,   446,   451,   450,   461,   462,   466,   467,   468,
     469,   473,   474,   478,   485,   494,   495,   499,   506,   518,
     519,   524,   523,   534,   535,   539,   540,   545,   544,   555,
     554,   566,   565,   577,   576,   591,   600,   601,   605,   606,
     611,   610,   620,   619,   633,   634,   638,   639,   643,   655,
     656,   661,   660,   670,   669,   682,   683,   687,   688,   692,
     699,   710,   717,   724,   725,   730,   729,   739,   738,   750,
     751,   755,   756,   760,   761,   766,   765,   801,   802,   806,
     816,   834,   838,   847,   851,   852,   856,   861,   866,   872,
     881,   882,   886,   893,   902,   907,   911,   919,   927,   934,
     945,   946,   950,   955,   962,   967,   971,   979,   984,   988,
     997,  1003,  1010,  1023,  1029,  1042,  1053,  1059,  1063,  1067,
    1078,  1080,  1082,  1084,  1089,  1091,  1096,  1097,  1100,  1102,
    1104,  1106,  1108,  1110,  1113,  1117,  1119,  1121,  1123,  1125,
    1127,  1130,  1132,  1134,  1137,  1141,  1143,  1145,  1148,  1151,
    1153,  1155,  1158,  1160,  1162,  1164,  1166,  1171,  1172,  1173,
    1177,  1182,  1188,  1193,  1198,  1206,  1208,  1210,  1215,  1216,
    1217,  1221,  1226,  1228,  1233,  1235,  1237,  1242,  1243,  1244,
    1245,  1248,  1248,  1250,  1250
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
  "gatesizeblock_old", "@10", "@11", "opt_gatesizes_old", "gatesizes_old",
  "gatesize_old", "opt_submod_displayblock_old", "opt_connblock_old",
  "connblock_old", "@12", "@13", "opt_connections_old", "connections_old",
  "connection_old", "loopconnection_old", "@14", "loopvarlist_old",
  "loopvar_old", "opt_conncondition_old", "opt_conn_displaystr_old",
  "notloopconnections_old", "notloopconnection_old", "leftgatespec_old",
  "leftmod_old", "leftgate_old", "parentleftgate_old", "rightgatespec_old",
  "rightmod_old", "rightgate_old", "parentrightgate_old",
  "channeldescr_old", "networkdefinition_old", "networkheader_old",
  "endnetwork_old", "vector", "expression", "inputvalue", "xmldocvalue",
  "expr", "simple_expr", "parameter_expr", "special_expr", "literal",
  "stringliteral", "boolliteral", "numliteral", "quantity",
  "opt_semicolon", "comma_or_semicolon", 0
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
     158,   160,   159,   161,   159,   162,   162,   163,   163,   164,
     164,   165,   165,   166,   166,   168,   167,   169,   167,   170,
     170,   171,   171,   172,   172,   174,   173,   175,   175,   176,
     177,   177,   178,   178,   179,   179,   180,   180,   180,   180,
     181,   181,   182,   182,   183,   183,   183,   184,   184,   184,
     185,   185,   186,   186,   187,   187,   187,   188,   188,   188,
     189,   189,   189,   190,   191,   192,   193,   194,   194,   194,
     195,   195,   195,   195,   196,   196,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   198,   198,   198,
     199,   199,   199,   199,   199,   200,   200,   200,   201,   201,
     201,   202,   203,   203,   204,   204,   204,   205,   205,   205,
     205,   206,   206,   207,   207
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
       0,     4,     0,     6,     2,     0,     3,     1,     3,     2,
       0,     0,     4,     0,     6,     2,     0,     3,     1,     2,
       1,     4,     0,     1,     0,     0,     5,     0,     4,     1,
       0,     2,     1,     1,     1,     0,     7,     3,     1,     5,
       2,     0,     2,     0,     2,     1,     6,     8,     6,     8,
       3,     1,     2,     1,     2,     1,     2,     2,     1,     2,
       3,     1,     2,     1,     2,     1,     2,     2,     1,     2,
       1,     2,     3,     3,     5,     2,     3,     1,     1,     1,
       6,     4,     3,     1,     6,     4,     1,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     3,     2,     3,
       3,     5,     3,     4,     6,     8,    10,     1,     1,     1,
       1,     2,     3,     3,     2,     1,     3,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     2,
       2,     1,     0,     1,     1
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
     224,   225,   210,   221,   222,   223,   173,     0,     0,     0,
       0,   215,     0,     0,     0,     0,   232,   168,   169,   167,
     176,   207,   208,   209,   218,   219,   220,   226,   232,    14,
       0,    35,     0,     0,    51,    70,     0,    90,   232,   163,
      88,    11,   232,   229,   230,     0,     0,     0,   211,     0,
     214,     0,     0,     0,   194,   198,   184,     0,   231,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   232,    21,   232,    38,    53,   232,    22,
       0,   114,    69,     0,    95,   165,   164,   202,     0,   172,
       0,     0,   212,   213,     0,   216,   177,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   195,   196,   197,   199,
     200,     0,   178,   179,   180,   181,   182,   183,   227,   228,
      20,    18,    41,     0,    36,     0,    40,    56,   232,    25,
      71,     0,    31,   113,    92,     0,    91,     0,    97,     0,
     203,     0,   171,     0,   175,   217,     0,     0,    43,    37,
       0,     0,     0,    54,    55,    24,    74,     0,   117,     0,
       0,    30,    95,     0,    94,     0,     0,     0,     0,   201,
      44,    42,    47,    48,    49,    50,    39,    64,     0,    62,
      68,     0,    66,     0,     0,     0,    72,    73,    76,   115,
     120,     0,   232,    26,    93,    98,    96,     0,   204,   170,
     174,    45,    46,     0,    58,     0,     0,    60,     0,     0,
       0,     0,    75,   120,   125,   148,   118,   119,   122,   123,
     124,     0,     0,   141,     0,   232,    29,     0,    63,    61,
      67,    65,    57,    59,   232,   116,     0,   149,     0,   147,
     121,     0,     0,     0,    32,    28,     0,   205,     0,   232,
      77,     0,     0,   128,     0,     0,   158,   131,     0,   151,
       0,   131,     0,   145,   140,     0,   232,     0,    79,    87,
       0,     0,     0,   166,   161,   159,   157,     0,   133,     0,
       0,     0,   133,     0,   146,   144,   206,    81,   232,    87,
      78,   100,     0,     0,   135,   127,   130,     0,     0,   155,
     150,   158,   131,   162,     0,   131,    87,    83,    80,   112,
       0,   232,   134,   132,   234,   233,   138,   156,   154,   133,
     136,   133,    82,    87,     0,     0,    99,    85,   129,   126,
       0,     0,    84,     0,   101,     0,   139,   137,     0,   106,
       0,   103,   110,   102,     0,   108,   111,   106,   109,   105,
       0,   104,   107
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,     2,     9,    10,    20,    21,    11,    12,    27,
      28,    69,    13,    14,   129,    15,    16,   243,   210,   211,
      30,    31,   126,   174,   175,   176,    73,    74,   177,   203,
     204,   228,   229,   231,   232,   131,   132,   206,   236,   237,
     238,   319,   339,   356,   373,   340,   341,    35,    36,   134,
     212,   186,   187,   188,   359,   376,   389,   397,   393,   394,
     395,   377,   182,   183,   263,   240,   266,   267,   268,   269,
     286,   302,   303,   328,   348,   343,   270,   271,   272,   314,
     273,   307,   308,   350,   309,   310,    17,    18,    79,   326,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    99,   366
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -341
static const short int yypact[] =
{
    -341,     8,   134,  -341,    24,    51,   100,   114,   118,  -341,
    -341,  -341,    32,  -341,    26,  -341,    26,  -341,   135,  -341,
      36,  -341,  -341,  -341,  -341,    71,   154,   128,   125,    76,
     182,  -341,   182,     2,   176,   135,  -341,  -341,    24,   178,
     197,   200,   136,  -341,  -341,  -341,   141,   152,    10,    88,
     164,   165,   172,   172,   172,   172,   140,  -341,  -341,   474,
    -341,  -341,  -341,  -341,  -341,  -341,  -341,   113,     0,  -341,
     154,  -341,   177,   244,  -341,   259,   154,  -341,   140,  -341,
    -341,  -341,   140,  -341,  -341,    68,    82,   235,  -341,   238,
    -341,   239,   241,   173,  -341,  -341,  -341,   347,  -341,  -341,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   243,   246,   140,  -341,   140,    78,  -341,     1,  -341,
     190,   268,  -341,   209,   265,  -341,  -341,  -341,   211,  -341,
     245,    28,  -341,  -341,   203,  -341,  -341,   539,   539,   539,
     539,   539,   539,   524,   524,   524,    89,    89,    89,   108,
     108,   449,    75,    75,   213,   213,   213,   213,  -341,  -341,
    -341,  -341,   221,   276,  -341,    66,  -341,   163,   140,  -341,
    -341,     5,   299,  -341,  -341,   212,  -341,   119,  -341,   172,
    -341,   172,  -341,   235,  -341,  -341,   172,   180,  -341,  -341,
      78,   290,   294,  -341,   206,  -341,   297,   252,  -341,   253,
     308,  -341,   265,   154,  -341,   265,   279,   381,   240,   499,
     300,   302,  -341,  -341,  -341,  -341,  -341,   248,   133,  -341,
     249,   145,  -341,   290,   294,   270,  -341,   297,  -341,  -341,
      20,   327,     3,  -341,  -341,  -341,  -341,   172,  -341,  -341,
    -341,  -341,  -341,   266,  -341,   290,   275,  -341,   294,   149,
     151,   333,  -341,    20,  -341,   -62,  -341,    20,  -341,  -341,
    -341,   236,   274,  -341,   281,   140,  -341,   313,  -341,  -341,
    -341,  -341,  -341,  -341,   -10,  -341,   344,  -341,   154,   278,
    -341,    -2,    -2,   346,  -341,  -341,   172,  -341,   358,    -1,
    -341,   296,   361,   304,   309,   154,    -7,   378,   310,  -341,
      11,   378,    87,   -43,  -341,   415,   140,   375,  -341,   135,
     154,   379,   344,  -341,  -341,  -341,   311,   154,   402,   392,
     396,   154,   402,   396,  -341,  -341,  -341,  -341,   140,   135,
    -341,  -341,   414,    79,  -341,  -341,  -341,   380,   162,    29,
    -341,   -41,   378,  -341,   162,   378,   135,  -341,  -341,   132,
     154,   140,  -341,  -341,  -341,  -341,  -341,  -341,  -341,   402,
    -341,   402,  -341,   135,     4,   354,  -341,  -341,  -341,  -341,
     162,   162,  -341,   154,  -341,   400,  -341,  -341,   355,   410,
     349,  -341,   348,  -341,   167,  -341,  -341,   410,  -341,  -341,
     410,  -341,  -341
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -341,  -341,  -341,  -341,  -341,  -341,   409,  -341,  -341,  -341,
    -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,
     432,  -341,  -341,  -341,  -341,   260,   417,  -341,  -341,  -341,
    -341,   231,   214,   233,   218,  -341,  -341,  -341,  -341,  -341,
     242,  -341,  -341,  -341,  -341,  -326,   459,  -341,   443,  -341,
    -341,   269,  -341,   267,  -341,  -341,  -341,  -341,    83,  -341,
      94,  -341,  -341,  -341,  -341,  -341,   220,  -341,   234,  -341,
    -341,   181,  -341,  -307,  -243,  -341,  -290,  -341,  -341,  -341,
    -341,  -287,  -341,  -341,  -341,   219,  -341,  -341,  -341,  -262,
     -70,  -341,  -341,   -36,  -341,  -341,  -341,  -341,   -85,  -341,
    -341,  -341,   -67,  -340
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -161
static const short int yytable[] =
{
     125,   124,   141,   289,   332,   311,   133,   287,     3,  -160,
    -160,   135,   298,   358,   370,   136,    94,    95,    96,    97,
     207,   317,   299,    76,  -160,   383,   334,   330,   325,   305,
     372,   344,   306,    29,   123,   178,   288,   275,   264,  -143,
     386,   387,   331,   352,    88,   369,   355,   382,   371,   138,
     140,   335,    89,   362,   265,   288,   170,   288,   171,    19,
    -153,   179,   325,    26,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,    22,    98,   368,   288,   354,
      77,   288,   384,   208,  -153,    98,    98,    98,   367,    98,
      40,    41,    42,    43,   333,    44,    45,   361,   218,    48,
      49,   205,   172,   265,    40,    41,    42,    43,   331,    44,
      45,   173,    90,    48,    49,   193,   380,   288,   381,    91,
     398,   194,    37,    38,    23,    50,    51,     4,     5,     6,
       7,   374,    33,   245,   375,   121,   122,    52,    24,    50,
      51,    53,    25,   216,    68,   217,    70,     8,    54,    39,
     219,    52,   199,   200,    71,    53,   117,   118,   119,   120,
      55,   137,    54,   112,   113,   276,   201,   202,   115,   116,
     117,   118,   119,   120,    55,   139,    40,    41,    42,    43,
      72,    44,    45,    46,    47,    48,    49,   115,   116,   117,
     118,   119,   120,    78,    40,    41,    42,    43,   295,    44,
      45,   277,    82,    48,    49,   214,   215,   300,   304,   233,
     234,    50,    51,   220,   221,   222,   223,   224,   225,   254,
     255,    83,   318,    52,    84,   324,    98,    53,    85,    50,
      51,   257,   258,    86,    54,   282,   255,   283,   258,   337,
     342,    52,   291,   292,    87,    53,    55,   346,   364,   365,
     315,   353,    54,   399,   400,   127,    92,    93,   128,   130,
      43,   357,   142,   143,    55,   144,   145,   168,   180,   181,
     169,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     378,   109,   110,   111,   379,   112,   113,   184,   114,   185,
     115,   116,   117,   118,   119,   120,   195,   120,   189,   197,
     198,   209,   213,   388,   190,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   227,   109,   110,   111,   230,   112,
     113,   235,   114,   242,   115,   116,   117,   118,   119,   120,
     239,   241,   191,   250,   251,   252,   253,   256,   192,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   261,   109,
     110,   111,   274,   112,   113,   278,   114,   284,   115,   116,
     117,   118,   119,   120,   280,   293,   247,   294,   301,  -142,
     313,   321,   248,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   316,   109,   110,   111,   320,   112,   113,   327,
     114,   322,   115,   116,   117,   118,   119,   120,   323,   338,
     296,   329,  -152,   265,   347,   363,   297,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   349,   109,   110,   111,
     351,   112,   113,   360,   114,   390,   115,   116,   117,   118,
     119,   120,   385,   391,   392,   396,   288,    81,    32,    75,
     146,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     226,   109,   110,   111,   259,   112,   113,   260,   114,   279,
     115,   116,   117,   118,   119,   120,   281,    34,    80,   262,
     401,   244,   246,   285,   249,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   402,   109,   110,   111,     0,   112,
     113,   290,   114,   345,   115,   116,   117,   118,   119,   120,
       0,   312,     0,     0,     0,     0,     0,     0,   336,   100,
     101,   102,   103,   104,   105,   106,   107,   108,     0,   109,
     110,   111,     0,   112,   113,     0,   114,   196,   115,   116,
     117,   118,   119,   120,   100,   101,   102,   103,   104,   105,
     106,   107,   108,     0,   109,   110,   111,     0,   112,   113,
       0,   114,     0,   115,   116,   117,   118,   119,   120,   100,
     101,   102,   103,   104,   105,   106,   107,   108,     0,   109,
     110,   111,     0,   112,   113,     0,     0,     0,   115,   116,
     117,   118,   119,   120,   100,   101,   102,   103,   104,   105,
       0,     0,     0,     0,   109,   110,   111,     0,   112,   113,
       0,     0,     0,   115,   116,   117,   118,   119,   120,   109,
     110,   111,     0,   112,   113,     0,     0,     0,   115,   116,
     117,   118,   119,   120
};

static const short int yycheck[] =
{
      70,    68,    87,   265,   311,   292,    76,    69,     0,    16,
      17,    78,    22,   339,   354,    82,    52,    53,    54,    55,
      15,    22,   284,    21,    31,    21,    69,    16,    69,    31,
     356,   321,    34,     7,    34,    34,    98,    34,    18,   101,
     380,   381,    31,   330,    34,   352,   333,   373,   355,    85,
      86,   313,    42,   343,    34,    98,   123,    98,   125,    35,
     101,   128,    69,    31,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,    34,    96,   349,    98,   332,
      88,    98,    88,    88,   101,    96,    96,    96,    69,    96,
      32,    33,    34,    35,    17,    37,    38,    28,   193,    41,
      42,   178,    34,    34,    32,    33,    34,    35,    31,    37,
      38,    43,    34,    41,    42,    97,   369,    98,   371,    41,
     392,   103,    96,    97,    34,    67,    68,     3,     4,     5,
       6,     9,     7,   213,    12,    32,    33,    79,    34,    67,
      68,    83,    34,   189,    26,   191,    31,    23,    90,    88,
     196,    79,    96,    97,    88,    83,    91,    92,    93,    94,
     102,   103,    90,    84,    85,   242,    13,    14,    89,    90,
      91,    92,    93,    94,   102,   103,    32,    33,    34,    35,
       8,    37,    38,    39,    40,    41,    42,    89,    90,    91,
      92,    93,    94,    27,    32,    33,    34,    35,   275,    37,
      38,   247,    34,    41,    42,    96,    97,   284,   288,    13,
      14,    67,    68,    43,    44,    45,    46,    47,    48,    96,
      97,    34,   299,    79,    34,   305,    96,    83,   102,    67,
      68,    96,    97,   102,    90,    96,    97,    96,    97,   316,
     320,    79,    16,    17,   102,    83,   102,   327,    96,    97,
     296,   331,    90,    96,    97,    88,   102,   102,    24,    10,
      35,   338,    34,    34,   102,    34,   103,    34,    88,    11,
      34,    70,    71,    72,    73,    74,    75,    76,    77,    78,
     360,    80,    81,    82,   361,    84,    85,    88,    87,    34,
      89,    90,    91,    92,    93,    94,   103,    94,    97,    88,
      34,    12,   100,   383,   103,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    34,    80,    81,    82,    34,    84,
      85,    34,    87,    25,    89,    90,    91,    92,    93,    94,
      88,    88,    97,   103,    44,    43,    98,    98,   103,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    88,    80,
      81,    82,    35,    84,    85,    99,    87,    34,    89,    90,
      91,    92,    93,    94,    99,   101,    97,    96,    34,   101,
      34,    20,   103,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    34,    80,    81,    82,   100,    84,    85,    21,
      87,    97,    89,    90,    91,    92,    93,    94,    99,    34,
      97,   101,   101,    34,    12,    35,   103,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    34,    80,    81,    82,
      34,    84,    85,    19,    87,    35,    89,    90,    91,    92,
      93,    94,    88,    88,    34,    96,    98,    38,    16,    32,
     103,    70,    71,    72,    73,    74,    75,    76,    77,    78,
     200,    80,    81,    82,   233,    84,    85,   234,    87,   255,
      89,    90,    91,    92,    93,    94,   258,    18,    35,   237,
     397,   212,   215,   263,   103,    70,    71,    72,    73,    74,
      75,    76,    77,    78,   400,    80,    81,    82,    -1,    84,
      85,   267,    87,   322,    89,    90,    91,    92,    93,    94,
      -1,   292,    -1,    -1,    -1,    -1,    -1,    -1,   103,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    -1,    80,
      81,    82,    -1,    84,    85,    -1,    87,    88,    89,    90,
      91,    92,    93,    94,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    -1,    80,    81,    82,    -1,    84,    85,
      -1,    87,    -1,    89,    90,    91,    92,    93,    94,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    -1,    80,
      81,    82,    -1,    84,    85,    -1,    -1,    -1,    89,    90,
      91,    92,    93,    94,    70,    71,    72,    73,    74,    75,
      -1,    -1,    -1,    -1,    80,    81,    82,    -1,    84,    85,
      -1,    -1,    -1,    89,    90,    91,    92,    93,    94,    80,
      81,    82,    -1,    84,    85,    -1,    -1,    -1,    89,    90,
      91,    92,    93,    94
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,   105,   106,     0,     3,     4,     5,     6,    23,   107,
     108,   111,   112,   116,   117,   119,   120,   190,   191,    35,
     109,   110,    34,    34,    34,    34,    31,   113,   114,     7,
     124,   125,   124,     7,   150,   151,   152,    96,    97,    88,
      32,    33,    34,    35,    37,    38,    39,    40,    41,    42,
      67,    68,    79,    83,    90,   102,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,    26,   115,
      31,    88,     8,   130,   131,   130,    21,    88,    27,   192,
     152,   110,    34,    34,    34,   102,   102,   102,    34,    42,
      34,    41,   102,   102,   197,   197,   197,   197,    96,   206,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    80,
      81,    82,    84,    85,    87,    89,    90,    91,    92,    93,
      94,    32,    33,    34,   206,   194,   126,    88,    24,   118,
      10,   139,   140,   194,   153,   206,   206,   103,   197,   103,
     197,   202,    34,    34,    34,   103,   103,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,    34,    34,
     206,   206,    34,    43,   127,   128,   129,   132,    34,   206,
      88,    11,   166,   167,    88,    34,   155,   156,   157,    97,
     103,    97,   103,    97,   103,   103,    88,    88,    34,    96,
      97,    13,    14,   133,   134,   206,   141,    15,    88,    12,
     122,   123,   154,   100,    96,    97,   197,   197,   202,   197,
      43,    44,    45,    46,    47,    48,   129,    34,   135,   136,
      34,   137,   138,    13,    14,    34,   142,   143,   144,    88,
     169,    88,    25,   121,   155,   194,   157,    97,   103,   103,
     103,    44,    43,    98,    96,    97,    98,    96,    97,   135,
     137,    88,   144,   168,    18,    34,   170,   171,   172,   173,
     180,   181,   182,   184,    35,    34,   206,   197,    99,   136,
      99,   138,    96,    96,    34,   170,   174,    69,    98,   193,
     172,    16,    17,   101,    96,   206,    97,   103,    22,   193,
     206,    34,   175,   176,   194,    31,    34,   185,   186,   188,
     189,   185,   189,    34,   183,   197,    34,    22,   206,   145,
     100,    20,    97,    99,   194,    69,   193,    21,   177,   101,
      16,    31,   177,    17,    69,   193,   103,   206,    34,   146,
     149,   150,   194,   179,   180,   175,   194,    12,   178,    34,
     187,    34,   185,   194,   178,   185,   147,   206,   149,   158,
      19,    28,   180,    35,    96,    97,   207,    69,   193,   177,
     207,   177,   149,   148,     9,    12,   159,   165,   194,   206,
     178,   178,   149,    21,    88,    88,   207,   207,   194,   160,
      35,    88,    34,   162,   163,   164,    96,   161,   193,    96,
      97,   162,   164
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
#line 186 "ned.y"
    { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.channel, (yylsp[0])); ;}
    break;

  case 7:
#line 188 "ned.y"
    { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.module, (yylsp[0])); ;}
    break;

  case 8:
#line 190 "ned.y"
    { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.module, (yylsp[0])); ;}
    break;

  case 9:
#line 192 "ned.y"
    { if (np->getStoreSourceFlag()) storeComponentSourceCode(ps.module, (yylsp[0])); ;}
    break;

  case 13:
#line 210 "ned.y"
    {
                  ps.import = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  ps.import->setFilename(toString(trimQuotes((yylsp[0]))));
                  setComments(ps.import,(yylsp[0]));
                ;}
    break;

  case 15:
#line 226 "ned.y"
    {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile);
                  ps.channel->setName(toString((yylsp[0])));
                  ps.params = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.channel);
                  ps.params->setIsImplicit(true);
                  setComments(ps.channel,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 18:
#line 242 "ned.y"
    {
                  ps.params->setIsImplicit(false);
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  addExpression(ps.param, "value",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.param,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 19:
#line 249 "ned.y"
    {
                  ps.params->setIsImplicit(false);
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  addExpression(ps.param, "value",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.param,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 20:
#line 259 "ned.y"
    {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                ;}
    break;

  case 21:
#line 263 "ned.y"
    {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                ;}
    break;

  case 23:
#line 280 "ned.y"
    {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 24:
#line 289 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 25:
#line 293 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 27:
#line 313 "ned.y"
    {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 28:
#line 322 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 29:
#line 326 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 32:
#line 341 "ned.y"
    {
                  ps.property = addComponentProperty(ps.module, "display");
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[-1])), (yylsp[-1]));
                  ps.propkey->appendChild(literal);
                ;}
    break;

  case 35:
#line 359 "ned.y"
    {
                  ps.params = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.module );
                  setComments(ps.params,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 36:
#line 364 "ned.y"
    {
                ;}
    break;

  case 39:
#line 375 "ned.y"
    {
                  setComments(ps.param,(yylsp[0]));
                ;}
    break;

  case 40:
#line 379 "ned.y"
    {
                  setComments(ps.param,(yylsp[0]));
                ;}
    break;

  case 41:
#line 389 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[0]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  ps.param->setIsFunction(true); // because CONST is missing
                ;}
    break;

  case 42:
#line 395 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  ps.param->setIsFunction(true); // because CONST is missing
                ;}
    break;

  case 43:
#line 401 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-1]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                ;}
    break;

  case 44:
#line 406 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                ;}
    break;

  case 45:
#line 411 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-3]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                ;}
    break;

  case 46:
#line 416 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-3]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                ;}
    break;

  case 47:
#line 421 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_STRING);
                ;}
    break;

  case 48:
#line 426 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_BOOL);
                ;}
    break;

  case 49:
#line 431 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_XML);
                ;}
    break;

  case 50:
#line 436 "ned.y"
    {
                  NEDError(ps.params,"type 'anytype' no longer supported");
                ;}
    break;

  case 53:
#line 451 "ned.y"
    {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 54:
#line 456 "ned.y"
    {
                ;}
    break;

  case 63:
#line 479 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[-2]));
                  ps.gate->setType(NED_GATETYPE_INPUT);
                  ps.gate->setIsVector(true);
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 64:
#line 486 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[0]));
                  ps.gate->setType(NED_GATETYPE_INPUT);
                  setComments(ps.gate,(yylsp[0]));
                ;}
    break;

  case 67:
#line 500 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[-2]));
                  ps.gate->setType(NED_GATETYPE_OUTPUT);
                  ps.gate->setIsVector(true);
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 68:
#line 507 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[0]));
                  ps.gate->setType(NED_GATETYPE_OUTPUT);
                  setComments(ps.gate,(yylsp[0]),(yylsp[0]));
                ;}
    break;

  case 71:
#line 524 "ned.y"
    {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 72:
#line 529 "ned.y"
    {
                ;}
    break;

  case 77:
#line 545 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-3])));
                  ps.submod->setType(toString((yylsp[-1])));
                  setComments(ps.submod,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 78:
#line 552 "ned.y"
    {
                ;}
    break;

  case 79:
#line 555 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-4])));
                  ps.submod->setType(toString((yylsp[-2])));
                  addVector(ps.submod, "vector-size",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.submod,(yylsp[-4]),(yylsp[0]));
                ;}
    break;

  case 80:
#line 563 "ned.y"
    {
                ;}
    break;

  case 81:
#line 566 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-5])));
                  ps.submod->setLikeType(toString((yylsp[-1])));
                  ps.submod->setLikeParam(toString((yylsp[-3]))); //FIXME store as expression!!!
                  setComments(ps.submod,(yylsp[-5]),(yylsp[0]));
                ;}
    break;

  case 82:
#line 574 "ned.y"
    {
                ;}
    break;

  case 83:
#line 577 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-6])));
                  ps.submod->setLikeType(toString((yylsp[-2])));
                  ps.submod->setLikeParam(toString((yylsp[-4]))); //FIXME store as expression!!!
                  addVector(ps.submod, "vector-size",(yylsp[-3]),(yyvsp[-3]));
                  setComments(ps.submod,(yylsp[-6]),(yylsp[0]));
                ;}
    break;

  case 84:
#line 586 "ned.y"
    {
                ;}
    break;

  case 90:
#line 611 "ned.y"
    {
                  // FIXME check if already exists!!! (multiple blocks must be merged)
                  ps.substparams = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.inNetwork ? (NEDElement *)ps.module : (NEDElement *)ps.submod);
                  setComments(ps.substparams,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 91:
#line 617 "ned.y"
    {
                ;}
    break;

  case 92:
#line 620 "ned.y"
    {
                  // FIXME make conditional paramgroup!!!
                  ps.substparams = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.inNetwork ? (NEDElement *)ps.module : (NEDElement *)ps.submod);
                  addExpression(ps.substparams, "condition",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.substparams,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 93:
#line 627 "ned.y"
    {
                ;}
    break;

  case 98:
#line 644 "ned.y"
    {
                  ps.substparam = addParameter(ps.substparams,(yylsp[-2]));
                  addExpression(ps.substparam, "value",(yylsp[0]),(yyvsp[0]));
                  setComments(ps.substparam,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 101:
#line 661 "ned.y"
    {
                  // FIXME check if already exists!!! (multiple blocks must be merged)
                  ps.gatesizes = (GatesNode *)createNodeWithTag(NED_GATES, ps.submod );
                  setComments(ps.gatesizes,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 102:
#line 667 "ned.y"
    {
                ;}
    break;

  case 103:
#line 670 "ned.y"
    {
                  // FIXME make conditional gategroup!!!
                  ps.gatesizes = (GatesNode *)createNodeWithTag(NED_GATES, ps.submod);
                  addExpression(ps.gatesizes, "condition",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.gatesizes,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 104:
#line 677 "ned.y"
    {
                ;}
    break;

  case 109:
#line 693 "ned.y"
    {
                  ps.gatesize = addGate(ps.gatesizes,(yylsp[-1]));
                  addVector(ps.gatesize, "vector-size",(yylsp[0]),(yyvsp[0]));

                  setComments(ps.gatesize,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 110:
#line 700 "ned.y"
    {
                  ps.gatesize = addGate(ps.gatesizes,(yylsp[0]));
                  setComments(ps.gatesize,(yylsp[0]));
                ;}
    break;

  case 111:
#line 711 "ned.y"
    {
                  ps.property = addComponentProperty(ps.submod, "display");
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[-1])), (yylsp[-1]));
                  ps.propkey->appendChild(literal);
                ;}
    break;

  case 115:
#line 730 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setAllowUnconnected(true);
                  setComments(ps.conns,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 116:
#line 736 "ned.y"
    {
                ;}
    break;

  case 117:
#line 739 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setAllowUnconnected(false);
                  setComments(ps.conns,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 118:
#line 745 "ned.y"
    {
                ;}
    break;

  case 125:
#line 766 "ned.y"
    {
                  ps.conngroup = (ConnectionGroupNode *)createNodeWithTag(NED_CONNECTION_GROUP, ps.conns);
                  ps.where = (WhereNode *)createNodeWithTag(NED_WHERE, ps.conngroup);
                  ps.inLoop=1;
                ;}
    break;

  case 126:
#line 772 "ned.y"
    {
                  ps.inLoop=0;
                  setComments(ps.where,(yylsp[-6]),(yylsp[-3]));
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
                          NEDError(ps.conngroup, "cannot process NED-I syntax of several "
                                   "conditional connections within a `for' loop -- "
                                   "please split it to several `for' loops");
                  }
                ;}
    break;

  case 129:
#line 807 "ned.y"
    {
                  ps.loop = addLoop(ps.where,(yylsp[-4]));
                  addExpression(ps.loop, "from-value",(yylsp[-2]),(yyvsp[-2]));
                  addExpression(ps.loop, "to-value",(yylsp[0]),(yyvsp[0]));
                  setComments(ps.loop,(yylsp[-4]),(yylsp[0]));
                ;}
    break;

  case 130:
#line 817 "ned.y"
    {
                  if (!ps.inLoop)
                  {
                      // add where+condition to conn
                      ps.where = (WhereNode *)createNodeWithTag(NED_WHERE, ps.conn);
                      ps.condition = (ConditionNode *)createNodeWithTag(NED_CONDITION, ps.where);
                      addExpression(ps.condition, "condition",(yylsp[0]),(yyvsp[0]));
                  }
                  else
                  {
                      // inside a for loop: append condition to its "where".
                      // at the end we'll need to make sure there's only one connection in the loop!
                      // (otherwise we'd have to check all conns have exactly the same condition)
                      ps.condition = (ConditionNode *)createNodeWithTag(NED_CONDITION, ps.where);
                      addExpression(ps.condition, "condition",(yylsp[0]),(yyvsp[0]));
                  }
                ;}
    break;

  case 132:
#line 839 "ned.y"
    {
                  if (!ps.chanspec)
                      ps.chanspec = createChannelSpec(ps.conn);
                  ps.property = addComponentProperty(ps.chanspec, "display");
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[0])), (yylsp[0]));
                  ps.propkey->appendChild(literal);
                ;}
    break;

  case 136:
#line 857 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                ;}
    break;

  case 137:
#line 862 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                ;}
    break;

  case 138:
#line 867 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                ;}
    break;

  case 139:
#line 873 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                ;}
    break;

  case 142:
#line 887 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "src-module-index",(yylsp[0]),(yyvsp[0]));
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                ;}
    break;

  case 143:
#line 894 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[0])) );
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                ;}
    break;

  case 144:
#line 903 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 145:
#line 908 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[0])) );
                ;}
    break;

  case 146:
#line 912 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  ps.conn->setSrcGatePlusplus(true);
                ;}
    break;

  case 147:
#line 920 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),(yyvsp[0]));
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                ;}
    break;

  case 148:
#line 928 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[0])));
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                ;}
    break;

  case 149:
#line 935 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  ps.conn->setSrcGatePlusplus(true);
                  ps.chanspec = NULL;   // signal that there's no chanspec for this conn yet
                ;}
    break;

  case 152:
#line 951 "ned.y"
    {
                  ps.conn->setDestModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "dest-module-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 153:
#line 956 "ned.y"
    {
                  ps.conn->setDestModule( toString((yylsp[0])) );
                ;}
    break;

  case 154:
#line 963 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 155:
#line 968 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                ;}
    break;

  case 156:
#line 972 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                ;}
    break;

  case 157:
#line 980 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 158:
#line 985 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                ;}
    break;

  case 159:
#line 989 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                ;}
    break;

  case 160:
#line 998 "ned.y"
    {
                  if (!ps.chanspec)
                      ps.chanspec = createChannelSpec(ps.conn);
                  ps.chanspec->setType(toString((yylsp[0])));
                ;}
    break;

  case 161:
#line 1004 "ned.y"
    {
                  if (!ps.chanspec)
                      ps.chanspec = createChannelSpec(ps.conn);
                  ps.param = addParameter(ps.params, (yylsp[-1]));
                  addExpression(ps.param, "value",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 162:
#line 1011 "ned.y"
    {
                  if (!ps.chanspec)
                      ps.chanspec = createChannelSpec(ps.conn);
                  ps.param = addParameter(ps.params, (yylsp[-1]));
                  addExpression(ps.param, "value",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 164:
#line 1030 "ned.y"
    {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString((yylsp[-3])));
                  ((CompoundModuleNode *)ps.module)->setIsNetwork(true);
                  ps.extends = (ExtendsNode *)createNodeWithTag(NED_EXTENDS, ps.module);
                  ps.extends->setName(toString((yylsp[-1])));
                  setComments(ps.module,(yylsp[-4]),(yylsp[0]));
                  ps.inNetwork=1;
                ;}
    break;

  case 165:
#line 1043 "ned.y"
    {
                  //setTrailingComment(ps.module,@1);
                  ps.inNetwork=0;
                ;}
    break;

  case 166:
#line 1054 "ned.y"
    { (yyval) = (yyvsp[-1]); ;}
    break;

  case 167:
#line 1060 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 168:
#line 1064 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 169:
#line 1068 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 170:
#line 1079 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("input", (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 171:
#line 1081 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("input", (yyvsp[-1])); ;}
    break;

  case 172:
#line 1083 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("input"); ;}
    break;

  case 173:
#line 1085 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("input"); ;}
    break;

  case 174:
#line 1090 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("xmldoc", (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 175:
#line 1092 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("xmldoc", (yyvsp[-1])); ;}
    break;

  case 177:
#line 1098 "ned.y"
    { (yyval) = (yyvsp[-1]); ;}
    break;

  case 178:
#line 1101 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("+", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 179:
#line 1103 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("-", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 180:
#line 1105 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("*", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 181:
#line 1107 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("/", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 182:
#line 1109 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("%", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 183:
#line 1111 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("^", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 184:
#line 1115 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = unaryMinus((yyvsp[0])); ;}
    break;

  case 185:
#line 1118 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("==", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 186:
#line 1120 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("!=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 187:
#line 1122 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator(">", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 188:
#line 1124 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator(">=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 189:
#line 1126 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("<", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 190:
#line 1128 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("<=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 191:
#line 1131 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("&&", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 192:
#line 1133 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("||", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 193:
#line 1135 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("##", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 194:
#line 1139 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("!", (yyvsp[0])); ;}
    break;

  case 195:
#line 1142 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("&", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 196:
#line 1144 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("|", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 197:
#line 1146 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("#", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 198:
#line 1150 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("~", (yyvsp[0])); ;}
    break;

  case 199:
#line 1152 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("<<", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 200:
#line 1154 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator(">>", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 201:
#line 1156 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createOperator("?:", (yyvsp[-4]), (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 202:
#line 1159 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction(toString((yylsp[-2]))); ;}
    break;

  case 203:
#line 1161 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction(toString((yylsp[-3])), (yyvsp[-1])); ;}
    break;

  case 204:
#line 1163 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction(toString((yylsp[-5])), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 205:
#line 1165 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction(toString((yylsp[-7])), (yyvsp[-5]), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 206:
#line 1167 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction(toString((yylsp[-9])), (yyvsp[-7]), (yyvsp[-5]), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 210:
#line 1178 "ned.y"
    {
                  // if there's no modifier, might be a loop variable too
                  if (np->getParseExpressionsFlag()) (yyval) = createIdent((yylsp[0]));
                ;}
    break;

  case 211:
#line 1183 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createIdent((yylsp[0]));
                  NEDError(ps.params,"`ref' modifier no longer supported (add `function' "
                                     "modifier to destination parameter instead)");
                ;}
    break;

  case 212:
#line 1189 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createIdent((yylsp[0]));
                  NEDError(ps.params,"`ancestor' and `ref' modifiers no longer supported");
                ;}
    break;

  case 213:
#line 1194 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createIdent((yylsp[0]));
                  NEDError(ps.params,"`ancestor' and `ref' modifiers no longer supported");
                ;}
    break;

  case 214:
#line 1199 "ned.y"
    {
                  if (np->getParseExpressionsFlag()) (yyval) = createIdent((yylsp[0]));
                  NEDError(ps.params,"`ancestor' modifier no longer supported");
                ;}
    break;

  case 215:
#line 1207 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("index"); ;}
    break;

  case 216:
#line 1209 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("index"); ;}
    break;

  case 217:
#line 1211 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createFunction("sizeof", createIdent((yylsp[-1]))); ;}
    break;

  case 221:
#line 1222 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[0])), (yylsp[0])); ;}
    break;

  case 222:
#line 1227 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createLiteral(NED_CONST_BOOL, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 223:
#line 1229 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createLiteral(NED_CONST_BOOL, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 224:
#line 1234 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createLiteral(NED_CONST_INT, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 225:
#line 1236 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createLiteral(NED_CONST_DOUBLE, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 226:
#line 1238 "ned.y"
    { if (np->getParseExpressionsFlag()) (yyval) = createQuantity(toString((yylsp[0]))); ;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 2968 "ned.tab.c"

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


#line 1252 "ned.y"


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
        {NEDError(NULL, "unable to allocate work memory"); return false;}

    // create parser state and NEDFileNode
    np = p;
    resetParserState();
    ps.nedfile = new NedFileNode();

    // store file name with slashes always, even on Windows -- neddoc relies on that
    ps.nedfile->setFilename(slashifyFilename(np->getFileName()).c_str());

    // store file comment
    //FIXME ps.nedfile->setBannerComment(nedsource->getFileComment());

    if (np->getStoreSourceFlag())
        storeSourceCode(ps.nedfile, np->nedsource->getFullTextPos());

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


