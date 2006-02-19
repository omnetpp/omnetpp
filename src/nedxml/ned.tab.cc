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
 * This file contains about 3 shift-reduce conflicts around 'expression'.
 * The rest (7-8 shift-reduce conflicts) are because for some reason
 * (without reason, actually) the grammar has difficulty recognizing
 * submodule boundaries. You can verify this by temporarily allowing only
 * one submodule (in rule for 'opt_submodules', replace 'submodules' with
 * 'submodule'). I couldn't figure out how to solve this yet.
 *
 * Plus one (real) ambiguity exists between submodule display string
 * and compound module display string if no connections are present.
 *
 * bison's "%expect nn" option cannot be used to suppress the
 * warning message because %expect is not recognized by yacc
 */


#include <stdio.h>
#include <stdlib.h>
#include "nedgrammar.h"
#include "nederror.h"

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <string.h>         /* YYVERBOSE needs it */
#endif


int yylex (void);
void yyrestart(FILE *);
void yyerror (const char *s);


#include "nedparser.h"
#include "nedfilebuffer.h"
#include "nedelements.h"
#include "nedutil.h"

static YYLTYPE NULLPOS={0,0,0,0,0,0};

static const char *sourcefilename;

NEDParser *np; // FIXME will clash with new one

struct ParserState
{
    bool parseExpressions;
    bool storeSourceCode;
    bool inLoop;
    bool inNetwork;

    /* tmp flags, used with msg fields */
    bool isAbstract;
    bool isReadonly;

    /* NED-I: modules, channels, networks */
    NedFileNode *nedfile;
    WhitespaceNode *whitespace;
    ImportNode *import;
    //PropertyDeclNode *propertydecl;
    //ExtendsNode *extends;
    //InterfaceNameNode *interfacename;
    ChannelNode *channel;
    NEDElement *module;  // in fact, CompoundModuleNode* or SimpleModule*
    //ModuleInterfaceNode *moduleinterface;
    ParametersNode *params;
    //ParamGroupNode *paramgroup;
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

    /* NED-II: message subclassing */
    CplusplusNode *cplusplus;
    StructDeclNode *structdecl;
    ClassDeclNode *classdecl;
    MessageDeclNode *messagedecl;
    EnumDeclNode *enumdecl;
    EnumNode *enump;
    MessageNode *messagep;
    ClassNode *classp;
    StructNode *structp;
    NEDElement *msgclassorstruct;
    EnumFieldsNode *enumfields;
    EnumFieldNode *enumfield;
    PropertiesNode *properties;
    MsgpropertyNode *msgproperty;
    FieldsNode *fields;
    FieldNode *field;
} ps;

NEDElement *createNodeWithTag(int tagcode, NEDElement *parent=NULL);

PropertyNode *addProperty(NEDElement *node, const char *name);  // directly under the node
PropertyNode *addComponentProperty(NEDElement *node, const char *name); // into ParametersNode child of node

PropertyNode *storeSourceCode(NEDElement *node, YYLTYPE tokenpos);  // directly under the node
PropertyNode *storeComponentSourceCode(NEDElement *node, YYLTYPE tokenpos); // into ParametersNode child

void setFileComment(NEDElement *node);
void setBannerComment(NEDElement *node, YYLTYPE tokenpos);
void setRightComment(NEDElement *node, YYLTYPE tokenpos);
void setTrailingComment(NEDElement *node, YYLTYPE tokenpos);
void setComments(NEDElement *node, YYLTYPE pos);
void setComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos);

ParamNode *addParameter(NEDElement *params, YYLTYPE namepos);
GateNode *addGate(NEDElement *gates, YYLTYPE namepos);
LoopNode *addLoop(NEDElement *conngroup, YYLTYPE varnamepos);

YYLTYPE trimBrackets(YYLTYPE vectorpos);
YYLTYPE trimAngleBrackets(YYLTYPE vectorpos);
YYLTYPE trimQuotes(YYLTYPE vectorpos);
YYLTYPE trimDoubleBraces(YYLTYPE vectorpos);
void swapAttributes(NEDElement *node, const char *attr1, const char *attr2);
void swapExpressionChildren(NEDElement *node, const char *attr1, const char *attr2);
void swapConnection(NEDElement *conn);

const char *toString(YYLTYPE);
const char *toString(long);

ExpressionNode *createExpression(NEDElement *expr);
OperatorNode *createOperator(const char *op, NEDElement *operand1, NEDElement *operand2=NULL, NEDElement *operand3=NULL);
FunctionNode *createFunction(const char *funcname, NEDElement *arg1=NULL, NEDElement *arg2=NULL, NEDElement *arg3=NULL, NEDElement *arg4=NULL);
IdentNode *createIdent(const char *param, const char *paramindex=NULL, const char *module=NULL, const char *moduleindex=NULL);
LiteralNode *createLiteral(int type, YYLTYPE valuepos, YYLTYPE textpos);
LiteralNode *createQuantity(const char *text);
NEDElement *unaryMinus(NEDElement *node);

void addVector(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);
void addLikeParam(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);
void addExpression(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);



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
#line 446 "ned.tab.c"

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
#define YYLAST   741

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  106
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  144
/* YYNRULES -- Number of rules. */
#define YYNRULES  335
/* YYNRULES -- Number of states. */
#define YYNSTATES  574

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
       2,     2,     2,   104,     2,   105,     2,     2,     2,     2,
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
      19,    21,    23,    25,    27,    29,    31,    33,    35,    37,
      41,    45,    47,    49,    53,    56,    57,    59,    64,    68,
      72,    75,    80,    83,    87,    90,    98,   101,   105,   108,
     110,   111,   116,   118,   119,   120,   125,   128,   129,   133,
     135,   137,   141,   144,   148,   153,   158,   162,   166,   170,
     174,   176,   177,   178,   183,   185,   186,   191,   195,   200,
     204,   208,   210,   214,   216,   220,   222,   226,   228,   230,
     231,   232,   237,   239,   240,   243,   245,   246,   253,   254,
     262,   263,   272,   273,   283,   287,   289,   290,   293,   295,
     296,   301,   302,   309,   312,   313,   317,   319,   323,   326,
     327,   328,   333,   334,   341,   344,   345,   349,   351,   354,
     356,   361,   362,   364,   365,   366,   372,   373,   378,   380,
     381,   384,   386,   388,   390,   391,   399,   403,   405,   411,
     414,   415,   418,   419,   422,   424,   431,   440,   447,   456,
     460,   462,   465,   467,   470,   472,   475,   478,   480,   483,
     487,   489,   492,   494,   497,   499,   502,   505,   507,   510,
     512,   515,   519,   523,   529,   532,   536,   538,   540,   542,
     549,   554,   558,   560,   567,   572,   574,   578,   582,   586,
     590,   594,   598,   602,   605,   609,   613,   617,   621,   625,
     629,   633,   637,   641,   644,   648,   652,   656,   659,   663,
     667,   673,   677,   682,   689,   698,   709,   711,   713,   715,
     717,   720,   724,   728,   731,   733,   737,   742,   744,   746,
     748,   750,   752,   754,   756,   758,   760,   764,   768,   771,
     774,   778,   782,   786,   791,   795,   799,   800,   808,   809,
     819,   821,   822,   825,   827,   830,   835,   836,   845,   846,
     857,   858,   867,   868,   879,   880,   889,   890,   901,   902,
     907,   908,   910,   911,   914,   916,   921,   923,   925,   927,
     929,   931,   933,   934,   939,   940,   942,   943,   946,   948,
     949,   958,   959,   967,   969,   971,   974,   977,   978,   980,
     983,   985,   987,   989,   991,   994,   997,  1000,  1003,  1005,
    1007,  1009,  1013,  1017,  1020,  1021,  1026,  1027,  1030,  1031,
    1033,  1035,  1037,  1040,  1042,  1045,  1047,  1049,  1051,  1053,
    1055,  1058,  1060,  1062,  1063,  1065
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     107,     0,    -1,   108,    -1,   108,   109,    -1,    -1,   110,
      -1,   113,    -1,   118,    -1,   121,    -1,   192,    -1,   208,
      -1,   209,    -1,   210,    -1,   211,    -1,   212,    -1,   213,
      -1,   219,    -1,   222,    -1,   225,    -1,     3,   111,    96,
      -1,   111,    97,   112,    -1,   112,    -1,    35,    -1,   114,
     115,   117,    -1,     5,    34,    -1,    -1,   116,    -1,   116,
      31,   196,   248,    -1,    31,   196,   248,    -1,    26,    34,
     248,    -1,    26,   248,    -1,   119,   126,   132,   120,    -1,
       4,    34,    -1,    24,    34,   248,    -1,    24,   248,    -1,
     122,   126,   132,   141,   168,   124,   123,    -1,     6,    34,
      -1,    25,    34,   248,    -1,    25,   248,    -1,   125,    -1,
      -1,    12,    88,    35,    96,    -1,   127,    -1,    -1,    -1,
       7,    88,   128,   129,    -1,   130,    96,    -1,    -1,   130,
      97,   131,    -1,   131,    -1,    34,    -1,    34,    88,    44,
      -1,    43,    34,    -1,    34,    88,    43,    -1,    34,    88,
      43,    44,    -1,    34,    88,    44,    43,    -1,    34,    88,
      45,    -1,    34,    88,    46,    -1,    34,    88,    47,    -1,
      34,    88,    48,    -1,   133,    -1,    -1,    -1,     8,    88,
     134,   135,    -1,   136,    -1,    -1,   136,    13,   137,    96,
      -1,    13,   137,    96,    -1,   136,    14,   139,    96,    -1,
      14,   139,    96,    -1,   137,    97,   138,    -1,   138,    -1,
      34,    98,    99,    -1,    34,    -1,   139,    97,   140,    -1,
     140,    -1,    34,    98,    99,    -1,    34,    -1,   142,    -1,
      -1,    -1,    10,    88,   143,   144,    -1,   145,    -1,    -1,
     145,   146,    -1,   146,    -1,    -1,    34,    88,    34,   248,
     147,   151,    -1,    -1,    34,    88,    34,   195,   248,   148,
     151,    -1,    -1,    34,    88,    34,    22,    34,   248,   149,
     151,    -1,    -1,    34,    88,    34,   195,    22,    34,   248,
     150,   151,    -1,   152,   160,   167,    -1,   153,    -1,    -1,
     153,   154,    -1,   154,    -1,    -1,     7,    88,   155,   157,
      -1,    -1,     7,    21,   196,    88,   156,   157,    -1,   158,
      96,    -1,    -1,   158,    97,   159,    -1,   159,    -1,    34,
     100,   196,    -1,   160,   161,    -1,    -1,    -1,     9,    88,
     162,   164,    -1,    -1,     9,    21,   196,    88,   163,   164,
      -1,   165,    96,    -1,    -1,   165,    97,   166,    -1,   166,
      -1,    34,   195,    -1,    34,    -1,    12,    88,    35,    96,
      -1,    -1,   169,    -1,    -1,    -1,    11,    15,    88,   170,
     172,    -1,    -1,    11,    88,   171,   172,    -1,   173,    -1,
      -1,   173,   174,    -1,   174,    -1,   175,    -1,   182,    -1,
      -1,    18,   176,   177,    20,   181,    28,   248,    -1,   178,
      97,   177,    -1,   178,    -1,    34,   100,   196,    19,   196,
      -1,    21,   196,    -1,    -1,    12,    35,    -1,    -1,   181,
     182,    -1,   182,    -1,   183,    17,   187,   179,   180,   249,
      -1,   183,    17,   191,    17,   187,   179,   180,   249,    -1,
     183,    16,   187,   179,   180,   249,    -1,   183,    16,   191,
      16,   187,   179,   180,   249,    -1,   184,   101,   185,    -1,
     186,    -1,    34,   195,    -1,    34,    -1,    34,   195,    -1,
      34,    -1,    34,    69,    -1,    34,   195,    -1,    34,    -1,
      34,    69,    -1,   188,   101,   189,    -1,   190,    -1,    34,
     195,    -1,    34,    -1,    34,   195,    -1,    34,    -1,    34,
      69,    -1,    34,   195,    -1,    34,    -1,    34,    69,    -1,
      34,    -1,    31,   196,    -1,   191,    31,   196,    -1,   193,
     152,   194,    -1,    23,    34,    88,    34,   248,    -1,    27,
     248,    -1,    98,   196,    99,    -1,   199,    -1,   197,    -1,
     198,    -1,    39,   102,   199,    97,   199,   103,    -1,    39,
     102,   199,   103,    -1,    39,   102,   103,    -1,    39,    -1,
      40,   102,   204,    97,   204,   103,    -1,    40,   102,   204,
     103,    -1,   200,    -1,   102,   199,   103,    -1,   199,    89,
     199,    -1,   199,    90,   199,    -1,   199,    91,   199,    -1,
     199,    92,   199,    -1,   199,    93,   199,    -1,   199,    94,
     199,    -1,    90,   199,    -1,   199,    70,   199,    -1,   199,
      71,   199,    -1,   199,    72,   199,    -1,   199,    73,   199,
      -1,   199,    74,   199,    -1,   199,    75,   199,    -1,   199,
      76,   199,    -1,   199,    77,   199,    -1,   199,    78,   199,
      -1,    79,   199,    -1,   199,    80,   199,    -1,   199,    81,
     199,    -1,   199,    82,   199,    -1,    83,   199,    -1,   199,
      84,   199,    -1,   199,    85,   199,    -1,   199,    87,   199,
      88,   199,    -1,    34,   102,   103,    -1,    34,   102,   199,
     103,    -1,    34,   102,   199,    97,   199,   103,    -1,    34,
     102,   199,    97,   199,    97,   199,   103,    -1,    34,   102,
     199,    97,   199,    97,   199,    97,   199,   103,    -1,   201,
      -1,   202,    -1,   203,    -1,    34,    -1,    41,    34,    -1,
      41,    42,    34,    -1,    42,    41,    34,    -1,    42,    34,
      -1,    68,    -1,    68,   102,   103,    -1,    67,   102,    34,
     103,    -1,   204,    -1,   205,    -1,   206,    -1,    35,    -1,
      37,    -1,    38,    -1,    32,    -1,    33,    -1,   207,    -1,
     207,    32,    34,    -1,   207,    33,    34,    -1,    32,    34,
      -1,    33,    34,    -1,    49,    50,   248,    -1,    53,    34,
      96,    -1,    52,    34,    96,    -1,    52,    55,    34,    96,
      -1,    51,    34,    96,    -1,    54,    34,    96,    -1,    -1,
      54,    34,   104,   214,   216,   105,   248,    -1,    -1,    54,
      34,    56,    34,   104,   215,   216,   105,   248,    -1,   217,
      -1,    -1,   217,   218,    -1,   218,    -1,    34,    96,    -1,
      34,   100,   247,    96,    -1,    -1,    51,    34,   104,   220,
     228,   234,   105,   248,    -1,    -1,    51,    34,    56,    34,
     104,   221,   228,   234,   105,   248,    -1,    -1,    52,    34,
     104,   223,   228,   234,   105,   248,    -1,    -1,    52,    34,
      56,    34,   104,   224,   228,   234,   105,   248,    -1,    -1,
      53,    34,   104,   226,   228,   234,   105,   248,    -1,    -1,
      53,    34,    56,    34,   104,   227,   228,   234,   105,   248,
      -1,    -1,    58,    88,   229,   230,    -1,    -1,   231,    -1,
      -1,   231,   232,    -1,   232,    -1,    34,   100,   233,    96,
      -1,    35,    -1,    32,    -1,    33,    -1,   207,    -1,    37,
      -1,    38,    -1,    -1,    57,    88,   235,   236,    -1,    -1,
     237,    -1,    -1,   237,   238,    -1,   238,    -1,    -1,   241,
     242,    34,   239,   243,   244,   245,    96,    -1,    -1,   241,
      34,   240,   243,   244,   245,    96,    -1,    59,    -1,    60,
      -1,    59,    60,    -1,    60,    59,    -1,    -1,    34,    -1,
      34,    91,    -1,    61,    -1,    62,    -1,    63,    -1,    64,
      -1,    66,    61,    -1,    66,    62,    -1,    66,    63,    -1,
      66,    64,    -1,    65,    -1,    45,    -1,    46,    -1,    98,
      32,    99,    -1,    98,    34,    99,    -1,    98,    99,    -1,
      -1,    54,   102,    34,   103,    -1,    -1,   100,   246,    -1,
      -1,    35,    -1,    36,    -1,    32,    -1,    90,    32,    -1,
      33,    -1,    90,    33,    -1,   207,    -1,    37,    -1,    38,
      -1,    34,    -1,    32,    -1,    90,    32,    -1,    34,    -1,
      96,    -1,    -1,    97,    -1,    96,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   236,   236,   240,   241,   245,   247,   249,   251,   253,
     256,   257,   258,   259,   260,   262,   264,   266,   268,   276,
     281,   282,   286,   298,   302,   310,   312,   316,   322,   331,
     335,   345,   352,   361,   365,   375,   385,   394,   398,   408,
     409,   413,   426,   427,   432,   431,   442,   443,   447,   451,
     461,   467,   473,   478,   483,   488,   493,   498,   503,   508,
     518,   519,   524,   523,   534,   535,   539,   540,   541,   542,
     546,   547,   551,   558,   567,   568,   572,   579,   591,   592,
     597,   596,   607,   608,   612,   613,   618,   617,   628,   627,
     639,   638,   650,   649,   664,   673,   674,   678,   679,   684,
     683,   692,   691,   704,   705,   709,   710,   714,   726,   727,
     732,   731,   740,   739,   751,   752,   756,   757,   761,   768,
     779,   786,   793,   794,   799,   798,   808,   807,   819,   820,
     824,   825,   829,   830,   835,   834,   849,   850,   854,   864,
     868,   872,   880,   884,   885,   889,   894,   899,   905,   914,
     915,   919,   925,   933,   938,   942,   950,   957,   963,   973,
     974,   978,   983,   990,   995,   999,  1007,  1012,  1016,  1025,
    1030,  1035,  1046,  1052,  1061,  1072,  1078,  1082,  1086,  1097,
    1099,  1101,  1103,  1108,  1110,  1115,  1116,  1119,  1121,  1123,
    1125,  1127,  1129,  1132,  1136,  1138,  1140,  1142,  1144,  1146,
    1149,  1151,  1153,  1156,  1160,  1162,  1164,  1167,  1170,  1172,
    1174,  1177,  1179,  1181,  1183,  1185,  1190,  1191,  1192,  1196,
    1201,  1207,  1212,  1217,  1225,  1227,  1229,  1234,  1235,  1236,
    1240,  1245,  1247,  1252,  1254,  1256,  1261,  1262,  1263,  1264,
    1272,  1281,  1290,  1297,  1307,  1316,  1326,  1325,  1337,  1336,
    1351,  1352,  1356,  1357,  1361,  1367,  1378,  1377,  1388,  1387,
    1402,  1401,  1412,  1411,  1426,  1425,  1436,  1435,  1450,  1449,
    1455,  1459,  1460,  1464,  1465,  1469,  1479,  1480,  1481,  1482,
    1483,  1484,  1489,  1488,  1494,  1498,  1499,  1503,  1504,  1509,
    1508,  1521,  1520,  1534,  1536,  1538,  1540,  1543,  1547,  1548,
    1550,  1551,  1552,  1553,  1555,  1556,  1557,  1558,  1560,  1561,
    1562,  1567,  1572,  1577,  1581,  1585,  1589,  1593,  1597,  1601,
    1602,  1603,  1604,  1605,  1606,  1607,  1608,  1609,  1610,  1614,
    1615,  1616,  1619,  1619,  1621,  1621
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
  "']'", "'='", "'.'", "'('", "')'", "'{'", "'}'", "$accept",
  "networkdescription", "somedefinitions", "definition", "import",
  "filenames", "filename", "channeldefinition_old", "channelheader_old",
  "opt_channelattrblock_old", "channelattrblock_old", "endchannel_old",
  "simpledefinition_old", "simpleheader_old", "endsimple_old",
  "moduledefinition_old", "moduleheader_old", "endmodule_old",
  "opt_displayblock_old", "displayblock_old", "opt_paramblock_old",
  "paramblock_old", "@1", "opt_parameters_old", "parameters_old",
  "parameter_old", "opt_gateblock_old", "gateblock_old", "@2",
  "opt_gates_old", "gates_old", "gatesI_old", "gateI_old", "gatesO_old",
  "gateO_old", "opt_submodblock_old", "submodblock_old", "@3",
  "opt_submodules_old", "submodules_old", "submodule_old", "@4", "@5",
  "@6", "@7", "submodule_body_old", "opt_substparamblocks_old",
  "substparamblocks_old", "substparamblock_old", "@8", "@9",
  "opt_substparameters_old", "substparameters_old", "substparameter_old",
  "opt_gatesizeblocks_old", "gatesizeblock_old", "@10", "@11",
  "opt_gatesizes_old", "gatesizes_old", "gatesize_old",
  "opt_submod_displayblock_old", "opt_connblock_old", "connblock_old",
  "@12", "@13", "opt_connections_old", "connections_old", "connection_old",
  "loopconnection_old", "@14", "loopvarlist_old", "loopvar_old",
  "opt_conncondition_old", "opt_conn_displaystr_old",
  "notloopconnections_old", "notloopconnection_old", "leftgatespec_old",
  "leftmod_old", "leftgate_old", "parentleftgate_old", "rightgatespec_old",
  "rightmod_old", "rightgate_old", "parentrightgate_old",
  "channeldescr_old", "networkdefinition_old", "networkheader_old",
  "endnetwork_old", "vector", "expression", "inputvalue", "xmldocvalue",
  "expr", "simple_expr", "parameter_expr", "special_expr", "literal",
  "stringliteral", "boolliteral", "numliteral", "quantity", "cplusplus",
  "struct_decl", "class_decl", "message_decl", "enum_decl", "enum", "@15",
  "@16", "opt_enumfields", "enumfields", "enumfield", "message", "@17",
  "@18", "class", "@19", "@20", "struct", "@21", "@22",
  "opt_propertiesblock", "@23", "opt_properties", "properties", "property",
  "propertyvalue", "opt_fieldsblock", "@24", "opt_fields", "fields",
  "field", "@25", "@26", "fieldmodifiers", "fielddatatype",
  "opt_fieldvector", "opt_fieldenum", "opt_fieldvalue", "fieldvalue",
  "enumvalue", "opt_semicolon", "comma_or_semicolon", 0
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
      61,    46,    40,    41,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   106,   107,   108,   108,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   110,
     111,   111,   112,   113,   114,   115,   115,   116,   116,   117,
     117,   118,   119,   120,   120,   121,   122,   123,   123,   124,
     124,   125,   126,   126,   128,   127,   129,   129,   130,   130,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     132,   132,   134,   133,   135,   135,   136,   136,   136,   136,
     137,   137,   138,   138,   139,   139,   140,   140,   141,   141,
     143,   142,   144,   144,   145,   145,   147,   146,   148,   146,
     149,   146,   150,   146,   151,   152,   152,   153,   153,   155,
     154,   156,   154,   157,   157,   158,   158,   159,   160,   160,
     162,   161,   163,   161,   164,   164,   165,   165,   166,   166,
     167,   167,   168,   168,   170,   169,   171,   169,   172,   172,
     173,   173,   174,   174,   176,   175,   177,   177,   178,   179,
     179,   180,   180,   181,   181,   182,   182,   182,   182,   183,
     183,   184,   184,   185,   185,   185,   186,   186,   186,   187,
     187,   188,   188,   189,   189,   189,   190,   190,   190,   191,
     191,   191,   192,   193,   194,   195,   196,   196,   196,   197,
     197,   197,   197,   198,   198,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   200,   200,   200,   201,
     201,   201,   201,   201,   202,   202,   202,   203,   203,   203,
     204,   205,   205,   206,   206,   206,   207,   207,   207,   207,
     208,   209,   210,   210,   211,   212,   214,   213,   215,   213,
     216,   216,   217,   217,   218,   218,   220,   219,   221,   219,
     223,   222,   224,   222,   226,   225,   227,   225,   229,   228,
     228,   230,   230,   231,   231,   232,   233,   233,   233,   233,
     233,   233,   235,   234,   234,   236,   236,   237,   237,   239,
     238,   240,   238,   241,   241,   241,   241,   241,   242,   242,
     242,   242,   242,   242,   242,   242,   242,   242,   242,   242,
     242,   243,   243,   243,   243,   244,   244,   245,   245,   246,
     246,   246,   246,   246,   246,   246,   246,   246,   246,   247,
     247,   247,   248,   248,   249,   249
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     1,     1,     3,     2,     0,     1,     4,     3,     3,
       2,     4,     2,     3,     2,     7,     2,     3,     2,     1,
       0,     4,     1,     0,     0,     4,     2,     0,     3,     1,
       1,     3,     2,     3,     4,     4,     3,     3,     3,     3,
       1,     0,     0,     4,     1,     0,     4,     3,     4,     3,
       3,     1,     3,     1,     3,     1,     3,     1,     1,     0,
       0,     4,     1,     0,     2,     1,     0,     6,     0,     7,
       0,     8,     0,     9,     3,     1,     0,     2,     1,     0,
       4,     0,     6,     2,     0,     3,     1,     3,     2,     0,
       0,     4,     0,     6,     2,     0,     3,     1,     2,     1,
       4,     0,     1,     0,     0,     5,     0,     4,     1,     0,
       2,     1,     1,     1,     0,     7,     3,     1,     5,     2,
       0,     2,     0,     2,     1,     6,     8,     6,     8,     3,
       1,     2,     1,     2,     1,     2,     2,     1,     2,     3,
       1,     2,     1,     2,     1,     2,     2,     1,     2,     1,
       2,     3,     3,     5,     2,     3,     1,     1,     1,     6,
       4,     3,     1,     6,     4,     1,     3,     3,     3,     3,
       3,     3,     3,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     3,     3,     3,     2,     3,     3,
       5,     3,     4,     6,     8,    10,     1,     1,     1,     1,
       2,     3,     3,     2,     1,     3,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     2,     2,
       3,     3,     3,     4,     3,     3,     0,     7,     0,     9,
       1,     0,     2,     1,     2,     4,     0,     8,     0,    10,
       0,     8,     0,    10,     0,     8,     0,    10,     0,     4,
       0,     1,     0,     2,     1,     4,     1,     1,     1,     1,
       1,     1,     0,     4,     0,     1,     0,     2,     1,     0,
       8,     0,     7,     1,     1,     2,     2,     0,     1,     2,
       1,     1,     1,     1,     2,     2,     2,     2,     1,     1,
       1,     3,     3,     2,     0,     4,     0,     2,     0,     1,
       1,     1,     2,     1,     2,     1,     1,     1,     1,     1,
       2,     1,     1,     0,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       4,     0,     2,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     3,     5,     6,    25,     7,    43,
       8,    43,     9,    96,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    22,     0,    21,    32,    24,    36,     0,
     333,     0,     0,     0,     0,     0,     0,     0,    26,     0,
      61,    42,    61,     0,     0,    95,    98,    19,     0,     0,
     332,   240,     0,   244,   256,     0,   242,   260,     0,     0,
     241,   264,     0,   245,   246,   233,   234,   219,   230,   231,
     232,   182,     0,     0,     0,     0,   224,     0,     0,     0,
       0,   333,   177,   178,   176,   185,   216,   217,   218,   227,
     228,   229,   235,   333,    23,     0,    44,     0,     0,    60,
      79,     0,    99,   333,   172,    97,    20,   333,     0,   270,
       0,   270,   243,     0,   270,     0,   251,   238,   239,     0,
       0,     0,   220,     0,   223,     0,     0,     0,   203,   207,
     193,     0,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   333,    30,   333,    47,
      62,   333,    31,     0,   123,    78,     0,   104,   174,   173,
     258,     0,   284,   262,   284,   266,   284,   248,     0,     0,
     250,   253,   211,     0,   181,     0,     0,   221,   222,     0,
     225,   186,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   204,   205,   206,   208,   209,     0,   187,   188,   189,
     190,   191,   192,   236,   237,    29,    27,    50,     0,    45,
       0,    49,    65,   333,    34,    80,     0,    40,   122,   101,
       0,   100,     0,   106,   270,   268,     0,     0,   270,     0,
     270,     0,   251,   254,     0,   333,   252,     0,   212,     0,
     180,     0,   184,   226,     0,     0,    52,    46,     0,     0,
       0,    63,    64,    33,    83,     0,   126,     0,     0,    39,
     104,     0,   103,     0,   284,   272,   282,   333,   284,   333,
     284,   333,     0,   329,   331,     0,     0,   247,     0,     0,
       0,   210,    53,    51,    56,    57,    58,    59,    48,    73,
       0,    71,    77,     0,    75,     0,     0,     0,    81,    82,
      85,   124,   129,     0,   333,    35,   102,   107,   105,     0,
       0,   269,   271,   274,   297,   257,     0,   261,     0,   265,
     333,   330,   255,     0,   213,   179,   183,    54,    55,     0,
      67,     0,     0,    69,     0,     0,     0,     0,    84,   129,
     134,   157,   127,   128,   131,   132,   133,     0,     0,   150,
       0,   333,    38,   333,     0,   273,   293,   294,   283,   297,
     288,     0,   333,   333,   249,     0,    72,    70,    76,    74,
      66,    68,   333,   125,     0,   158,     0,   156,   130,     0,
       0,     0,    41,    37,   259,   277,   278,   276,   280,   281,
     279,     0,   295,   296,   287,   291,   309,   310,   300,   301,
     302,   303,   308,     0,     0,   263,   267,     0,   214,     0,
     333,    86,     0,     0,   137,     0,     0,   167,   140,     0,
     160,     0,   140,     0,   154,   149,   275,   299,   314,   304,
     305,   306,   307,   289,     0,   333,     0,    88,    96,     0,
       0,     0,   175,   170,   168,   166,     0,   142,     0,     0,
       0,   142,     0,   155,   153,     0,   316,   314,   215,    90,
     333,    96,    87,   109,     0,     0,   144,   136,   139,     0,
       0,   164,   159,   167,   140,   171,     0,   140,     0,     0,
     313,     0,   318,   316,    96,    92,    89,   121,     0,   333,
     143,   141,   335,   334,   147,   165,   163,   142,   145,   142,
     311,   312,     0,     0,     0,   318,    91,    96,     0,     0,
     108,    94,   138,   135,     0,     0,     0,   321,   323,   328,
     319,   320,   326,   327,     0,   325,   317,   292,     0,    93,
       0,   110,     0,   148,   146,   315,   322,   324,   290,     0,
     115,     0,   112,   119,   111,     0,   117,   120,   115,   118,
     114,     0,   113,   116
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,     2,    14,    15,    34,    35,    16,    17,    47,
      48,   104,    18,    19,   172,    20,    21,   325,   278,   279,
      50,    51,   169,   229,   230,   231,   108,   109,   232,   271,
     272,   310,   311,   313,   314,   174,   175,   274,   318,   319,
     320,   458,   481,   504,   527,   482,   483,    55,    56,   177,
     280,   241,   242,   243,   507,   530,   560,   568,   564,   565,
     566,   531,   237,   238,   359,   322,   362,   363,   364,   365,
     394,   433,   434,   467,   490,   485,   366,   367,   368,   445,
     369,   438,   439,   492,   440,   441,    22,    23,   114,   465,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,    24,    25,    26,    27,    28,    29,   126,   252,
     189,   190,   191,    30,   119,   244,    31,   121,   248,    32,
     124,   250,   182,   285,   331,   332,   333,   411,   247,   334,
     378,   379,   380,   477,   448,   381,   424,   476,   502,   524,
     546,   296,    61,   514
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -465
static const short int yypact[] =
{
    -465,    38,   167,  -465,    27,    47,    93,    97,   127,    16,
     130,    -7,   134,   171,  -465,  -465,  -465,   182,  -465,   210,
    -465,   210,  -465,   217,  -465,  -465,  -465,  -465,  -465,  -465,
    -465,  -465,  -465,  -465,    46,  -465,  -465,  -465,  -465,   141,
     140,   -36,   -35,   214,    73,    79,   221,   226,   213,   169,
     262,  -465,   262,    26,   246,   217,  -465,  -465,    27,   249,
    -465,  -465,   256,  -465,  -465,   258,  -465,  -465,   199,   282,
    -465,  -465,   288,  -465,  -465,   298,   330,   265,  -465,  -465,
    -465,   268,   269,    -1,    23,   270,   280,   234,   234,   234,
     234,   140,  -465,  -465,   571,  -465,  -465,  -465,  -465,  -465,
    -465,  -465,   164,   -23,  -465,   221,  -465,   301,   344,  -465,
     388,   221,  -465,   140,  -465,  -465,  -465,   140,   295,   328,
     297,   328,  -465,   300,   328,   319,   368,  -465,  -465,    84,
     147,   370,  -465,   382,  -465,   391,   398,   332,  -465,  -465,
    -465,   444,  -465,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   399,   402,   140,  -465,   140,     3,
    -465,   -22,  -465,   349,   427,  -465,   351,   407,  -465,  -465,
    -465,   354,   386,  -465,   386,  -465,   386,  -465,   -61,   339,
     368,  -465,  -465,   303,  -465,   337,   -45,  -465,  -465,   352,
    -465,  -465,   253,   253,   253,   253,   253,   253,   621,   621,
     621,   236,   236,   236,   259,   259,   546,   187,   187,   365,
     365,   365,   365,  -465,  -465,  -465,  -465,   374,   430,  -465,
     107,  -465,   195,   140,  -465,  -465,    21,   459,  -465,  -465,
     372,  -465,   131,  -465,   328,  -465,   387,   369,   328,   371,
     328,   373,   368,  -465,    33,   140,  -465,   234,  -465,   234,
    -465,   370,  -465,  -465,   234,   312,  -465,  -465,     3,   443,
     455,  -465,   218,  -465,   462,   405,  -465,   417,   473,  -465,
     407,   221,  -465,   407,   386,   472,  -465,   140,   386,   140,
     386,   140,   403,  -465,  -465,   477,   414,  -465,   376,   478,
     408,   596,   468,   480,  -465,  -465,  -465,  -465,  -465,   429,
     150,  -465,   432,   168,  -465,   443,   455,   451,  -465,   462,
    -465,  -465,     8,   497,   -21,  -465,  -465,  -465,  -465,   435,
     441,  -465,   472,  -465,    87,  -465,   437,  -465,   438,  -465,
     140,  -465,  -465,   234,  -465,  -465,  -465,  -465,  -465,   445,
    -465,   443,   446,  -465,   455,   190,   209,   523,  -465,     8,
    -465,   109,  -465,     8,  -465,  -465,  -465,   291,   460,  -465,
     450,   140,  -465,   140,   277,  -465,   504,   507,  -465,    89,
    -465,   177,   140,   140,  -465,   410,  -465,  -465,  -465,  -465,
    -465,  -465,   -18,  -465,   539,  -465,   221,   474,  -465,   105,
     105,   540,  -465,  -465,  -465,   298,   330,  -465,  -465,  -465,
     164,   481,  -465,  -465,  -465,   -15,  -465,  -465,  -465,  -465,
    -465,  -465,  -465,   235,   542,  -465,  -465,   234,  -465,   544,
     -12,  -465,   479,   560,   494,   496,   221,    39,   577,   499,
    -465,    18,   577,    28,   -54,  -465,  -465,  -465,   509,  -465,
    -465,  -465,  -465,  -465,   512,   140,   574,  -465,   217,   221,
     575,   539,  -465,  -465,  -465,   510,   221,   598,   578,   579,
     221,   598,   579,  -465,  -465,   -16,   600,   509,  -465,  -465,
     140,   217,  -465,  -465,   595,    96,  -465,  -465,  -465,   590,
     222,   -47,  -465,   176,   577,  -465,   222,   577,   530,   533,
    -465,   548,   557,   600,   217,  -465,  -465,   129,   221,   140,
    -465,  -465,  -465,  -465,  -465,  -465,  -465,   598,  -465,   598,
    -465,  -465,   625,   122,   583,   557,  -465,   217,    32,   587,
    -465,  -465,  -465,  -465,   222,   222,   580,   298,   330,  -465,
    -465,  -465,  -465,  -465,   307,   164,  -465,  -465,   586,  -465,
     221,  -465,   649,  -465,  -465,  -465,  -465,  -465,  -465,   609,
     664,   603,  -465,   602,  -465,   266,  -465,  -465,   664,  -465,
    -465,   664,  -465,  -465
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -465,  -465,  -465,  -465,  -465,  -465,   646,  -465,  -465,  -465,
    -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,
     686,  -465,  -465,  -465,  -465,   440,   657,  -465,  -465,  -465,
    -465,   401,   366,   404,   364,  -465,  -465,  -465,  -465,  -465,
     400,  -465,  -465,  -465,  -465,  -464,   698,  -465,   667,  -465,
    -465,   447,  -465,   442,  -465,  -465,  -465,  -465,   155,  -465,
     153,  -465,  -465,  -465,  -465,  -465,   367,  -465,   375,  -465,
    -465,   267,  -465,  -332,  -326,  -465,  -435,  -465,  -465,  -465,
    -465,  -357,  -465,  -465,  -465,   329,  -465,  -465,  -465,  -338,
    -105,  -465,  -465,   -58,  -465,  -465,  -465,  -465,  -128,  -465,
    -465,  -373,  -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,
     482,  -465,   541,  -465,  -465,  -465,  -465,  -465,  -465,  -465,
    -465,  -465,  -116,  -465,  -465,  -465,   409,  -465,  -177,  -465,
    -465,  -465,   353,  -465,  -465,  -465,  -465,   260,   227,   208,
    -465,  -465,   -89,  -301
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -299
static const short int yytable[] =
{
     168,   410,   142,   196,   429,   184,   176,   249,   186,   251,
     456,   166,   233,   371,   167,   473,   498,   506,   499,  -298,
      62,    65,   515,   397,   178,   486,   360,    42,   179,   138,
     139,   140,   141,   132,   469,   253,   275,   227,     3,   254,
     526,   133,   361,   442,   396,   472,   228,   111,    43,   470,
     510,   396,   261,   550,   430,  -169,  -169,   134,   262,   470,
      63,    66,    33,   549,   135,   293,    40,   294,    64,    67,
    -169,   193,   195,    60,    60,    60,   447,   225,    60,   226,
     396,    36,   234,   500,    60,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   474,   329,   464,   276,
     471,   336,   494,   338,   112,   497,    75,    76,    77,    78,
     551,    79,    80,   295,   509,    83,    84,    37,   284,    69,
     361,    38,   288,   300,   290,    72,   436,   396,   528,   437,
    -162,   529,    57,    58,   273,   496,   376,   377,   376,   377,
     545,    85,    86,   516,   537,   538,   539,   540,   541,   542,
     543,    39,   517,    87,    41,   519,   297,    88,    44,    70,
       4,     5,     6,     7,    89,    73,   327,    71,   395,    75,
      76,    77,    78,    74,    79,    80,    90,   192,    83,    84,
       8,   534,  -286,   535,  -285,   518,   164,   165,   335,   298,
     337,   299,   339,   267,   268,    45,   301,   396,   269,   270,
    -152,   415,   544,    46,    85,    86,     9,    49,    10,    11,
      12,    13,   416,   417,    53,   569,    87,   282,   283,    59,
      88,   315,   316,   553,   554,   372,    60,    89,   418,   419,
     420,   421,   422,   423,   105,   464,   350,   351,    68,    90,
     194,   384,   103,    75,    76,    77,    78,   106,    79,    80,
      81,    82,    83,    84,   353,   354,    75,    76,    77,    78,
     107,    79,    80,   113,   396,    83,    84,  -162,   160,   161,
     162,   163,   403,   117,   404,   385,   390,   351,    85,    86,
     118,   435,   120,   425,   426,   122,   449,   450,   451,   452,
      87,    85,    86,   431,    88,   391,   354,   399,   400,   405,
     406,    89,   407,    87,   408,   409,   123,    88,   512,   513,
     155,   156,   125,    90,    89,   158,   159,   160,   161,   162,
     163,   463,   127,   152,   153,   154,    90,   155,   156,   556,
     557,   457,   158,   159,   160,   161,   162,   163,   158,   159,
     160,   161,   162,   163,   484,   302,   303,   304,   305,   306,
     307,   488,   570,   571,   128,   495,   479,   129,   171,   454,
     130,   131,   136,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   137,   152,   153,   154,   181,   155,   156,   170,
     157,   505,   158,   159,   160,   161,   162,   163,   173,   180,
     257,   183,   188,   532,   185,    78,   258,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   197,   152,   153,   154,
     533,   155,   156,   187,   157,   198,   158,   159,   160,   161,
     162,   163,   199,   223,   259,   200,   224,   235,   236,   239,
     260,   240,   245,   246,   255,   559,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   263,   152,   153,   154,   163,
     155,   156,   265,   157,   266,   158,   159,   160,   161,   162,
     163,   277,   281,   343,   287,   286,   289,   309,   291,   344,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   312,
     152,   153,   154,   321,   155,   156,   317,   157,   324,   158,
     159,   160,   161,   162,   163,   323,   330,   427,   340,   341,
     342,   346,   347,   428,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   348,   152,   153,   154,   349,   155,   156,
     352,   157,   370,   158,   159,   160,   161,   162,   163,   357,
     373,   374,   382,   383,   386,   388,   402,   201,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   392,   152,   153,
     154,   401,   155,   156,   412,   157,   413,   158,   159,   160,
     161,   162,   163,   432,   444,  -151,   453,   446,   455,   459,
     460,   345,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   461,   152,   153,   154,   462,   155,   156,   466,   157,
     468,   158,   159,   160,   161,   162,   163,   475,   480,   361,
     489,  -161,   491,   493,   508,   478,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   511,   152,   153,   154,   520,
     155,   156,   521,   157,   264,   158,   159,   160,   161,   162,
     163,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     522,   152,   153,   154,   501,   155,   156,   523,   157,   536,
     158,   159,   160,   161,   162,   163,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   552,   152,   153,   154,   547,
     155,   156,   558,   555,   561,   158,   159,   160,   161,   162,
     163,   143,   144,   145,   146,   147,   148,   562,   563,   567,
     396,   152,   153,   154,   116,   155,   156,    52,   308,   110,
     158,   159,   160,   161,   162,   163,   355,   387,   389,   358,
     356,    54,   115,   572,   573,   328,   393,   326,   487,   443,
     525,   256,   414,   548,   292,     0,     0,   503,   398,     0,
       0,   375
};

static const short int yycheck[] =
{
     105,   374,    91,   131,    22,   121,   111,   184,   124,   186,
      22,    34,    34,    34,   103,    69,    32,   481,    34,    34,
      56,    56,    69,   361,   113,   460,    18,    34,   117,    87,
      88,    89,    90,    34,    16,    96,    15,    34,     0,   100,
     504,    42,    34,   400,    98,    17,    43,    21,    55,    31,
     485,    98,    97,    21,   392,    16,    17,    34,   103,    31,
      96,    96,    35,   527,    41,    32,    50,    34,   104,   104,
      31,   129,   130,    96,    96,    96,    91,   166,    96,   168,
      98,    34,   171,    99,    96,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   444,   284,    69,    88,
     442,   288,   469,   290,    88,   472,    32,    33,    34,    35,
      88,    37,    38,    90,    28,    41,    42,    34,   244,    56,
      34,    34,   248,   261,   250,    56,    31,    98,     9,    34,
     101,    12,    96,    97,   233,   471,    59,    60,    59,    60,
     523,    67,    68,   491,    32,    33,    34,    35,    36,    37,
      38,    34,   494,    79,    34,   497,   255,    83,    34,    96,
       3,     4,     5,     6,    90,    96,   281,   104,    69,    32,
      33,    34,    35,   104,    37,    38,   102,   103,    41,    42,
      23,   517,   105,   519,   105,   496,    32,    33,   287,   257,
     289,   259,   291,    96,    97,    34,   264,    98,    13,    14,
     101,    34,    90,    31,    67,    68,    49,     7,    51,    52,
      53,    54,    45,    46,     7,   563,    79,    96,    97,    88,
      83,    13,    14,   534,   535,   324,    96,    90,    61,    62,
      63,    64,    65,    66,    31,    69,    96,    97,    34,   102,
     103,   340,    26,    32,    33,    34,    35,    88,    37,    38,
      39,    40,    41,    42,    96,    97,    32,    33,    34,    35,
       8,    37,    38,    27,    98,    41,    42,   101,    91,    92,
      93,    94,   371,    34,   373,   343,    96,    97,    67,    68,
      34,   396,    34,   382,   383,    96,    61,    62,    63,    64,
      79,    67,    68,   392,    83,    96,    97,    16,    17,    32,
      33,    90,    35,    79,    37,    38,    34,    83,    96,    97,
      84,    85,    34,   102,    90,    89,    90,    91,    92,    93,
      94,   436,    34,    80,    81,    82,   102,    84,    85,    32,
      33,   430,    89,    90,    91,    92,    93,    94,    89,    90,
      91,    92,    93,    94,   459,    43,    44,    45,    46,    47,
      48,   466,    96,    97,    34,   470,   455,   102,    24,   427,
     102,   102,   102,    70,    71,    72,    73,    74,    75,    76,
      77,    78,   102,    80,    81,    82,    58,    84,    85,    88,
      87,   480,    89,    90,    91,    92,    93,    94,    10,   104,
      97,   104,    34,   508,   104,    35,   103,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    34,    80,    81,    82,
     509,    84,    85,   104,    87,    34,    89,    90,    91,    92,
      93,    94,    34,    34,    97,   103,    34,    88,    11,    88,
     103,    34,    88,    57,   105,   550,    70,    71,    72,    73,
      74,    75,    76,    77,    78,   103,    80,    81,    82,    94,
      84,    85,    88,    87,    34,    89,    90,    91,    92,    93,
      94,    12,   100,    97,   105,    88,   105,    34,   105,   103,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    34,
      80,    81,    82,    88,    84,    85,    34,    87,    25,    89,
      90,    91,    92,    93,    94,    88,    34,    97,   105,    32,
      96,   103,    44,   103,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    43,    80,    81,    82,    98,    84,    85,
      98,    87,    35,    89,    90,    91,    92,    93,    94,    88,
     105,   100,   105,   105,    99,    99,    96,   103,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    34,    80,    81,
      82,   101,    84,    85,    60,    87,    59,    89,    90,    91,
      92,    93,    94,    34,    34,   101,    34,    96,    34,   100,
      20,   103,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    97,    80,    81,    82,    99,    84,    85,    21,    87,
     101,    89,    90,    91,    92,    93,    94,    98,    34,    34,
      12,   101,    34,    34,    19,   103,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    35,    80,    81,    82,    99,
      84,    85,    99,    87,    88,    89,    90,    91,    92,    93,
      94,    70,    71,    72,    73,    74,    75,    76,    77,    78,
     102,    80,    81,    82,    54,    84,    85,   100,    87,    34,
      89,    90,    91,    92,    93,    94,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    88,    80,    81,    82,    96,
      84,    85,    96,   103,    35,    89,    90,    91,    92,    93,
      94,    70,    71,    72,    73,    74,    75,    88,    34,    96,
      98,    80,    81,    82,    58,    84,    85,    21,   268,    52,
      89,    90,    91,    92,    93,    94,   315,   351,   354,   319,
     316,    23,    55,   568,   571,   283,   359,   280,   461,   400,
     503,   190,   379,   525,   252,    -1,    -1,   477,   363,    -1,
      -1,   332
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,   107,   108,     0,     3,     4,     5,     6,    23,    49,
      51,    52,    53,    54,   109,   110,   113,   114,   118,   119,
     121,   122,   192,   193,   208,   209,   210,   211,   212,   213,
     219,   222,   225,    35,   111,   112,    34,    34,    34,    34,
      50,    34,    34,    55,    34,    34,    31,   115,   116,     7,
     126,   127,   126,     7,   152,   153,   154,    96,    97,    88,
      96,   248,    56,    96,   104,    56,    96,   104,    34,    56,
      96,   104,    56,    96,   104,    32,    33,    34,    35,    37,
      38,    39,    40,    41,    42,    67,    68,    79,    83,    90,
     102,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,    26,   117,    31,    88,     8,   132,   133,
     132,    21,    88,    27,   194,   154,   112,    34,    34,   220,
      34,   223,    96,    34,   226,    34,   214,    34,    34,   102,
     102,   102,    34,    42,    34,    41,   102,   102,   199,   199,
     199,   199,   248,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    80,    81,    82,    84,    85,    87,    89,    90,
      91,    92,    93,    94,    32,    33,    34,   248,   196,   128,
      88,    24,   120,    10,   141,   142,   196,   155,   248,   248,
     104,    58,   228,   104,   228,   104,   228,   104,    34,   216,
     217,   218,   103,   199,   103,   199,   204,    34,    34,    34,
     103,   103,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,    34,    34,   248,   248,    34,    43,   129,
     130,   131,   134,    34,   248,    88,    11,   168,   169,    88,
      34,   157,   158,   159,   221,    88,    57,   234,   224,   234,
     227,   234,   215,    96,   100,   105,   218,    97,   103,    97,
     103,    97,   103,   103,    88,    88,    34,    96,    97,    13,
      14,   135,   136,   248,   143,    15,    88,    12,   124,   125,
     156,   100,    96,    97,   228,   229,    88,   105,   228,   105,
     228,   105,   216,    32,    34,    90,   247,   248,   199,   199,
     204,   199,    43,    44,    45,    46,    47,    48,   131,    34,
     137,   138,    34,   139,   140,    13,    14,    34,   144,   145,
     146,    88,   171,    88,    25,   123,   157,   196,   159,   234,
      34,   230,   231,   232,   235,   248,   234,   248,   234,   248,
     105,    32,    96,    97,   103,   103,   103,    44,    43,    98,
      96,    97,    98,    96,    97,   137,   139,    88,   146,   170,
      18,    34,   172,   173,   174,   175,   182,   183,   184,   186,
      35,    34,   248,   105,   100,   232,    59,    60,   236,   237,
     238,   241,   105,   105,   248,   199,    99,   138,    99,   140,
      96,    96,    34,   172,   176,    69,    98,   195,   174,    16,
      17,   101,    96,   248,   248,    32,    33,    35,    37,    38,
     207,   233,    60,    59,   238,    34,    45,    46,    61,    62,
      63,    64,    65,    66,   242,   248,   248,    97,   103,    22,
     195,   248,    34,   177,   178,   196,    31,    34,   187,   188,
     190,   191,   187,   191,    34,   185,    96,    91,   240,    61,
      62,    63,    64,    34,   199,    34,    22,   248,   147,   100,
      20,    97,    99,   196,    69,   195,    21,   179,   101,    16,
      31,   179,    17,    69,   195,    98,   243,   239,   103,   248,
      34,   148,   151,   152,   196,   181,   182,   177,   196,    12,
     180,    34,   189,    34,   187,   196,   180,   187,    32,    34,
      99,    54,   244,   243,   149,   248,   151,   160,    19,    28,
     182,    35,    96,    97,   249,    69,   195,   179,   249,   179,
      99,    99,   102,   100,   245,   244,   151,   150,     9,    12,
     161,   167,   196,   248,   180,   180,    34,    32,    33,    34,
      35,    36,    37,    38,    90,   207,   246,    96,   245,   151,
      21,    88,    88,   249,   249,   103,    32,    33,    96,   196,
     162,    35,    88,    34,   164,   165,   166,    96,   163,   195,
      96,    97,   164,   166
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
#line 248 "ned.y"
    { if (ps.storeSourceCode) storeComponentSourceCode(ps.channel, (yylsp[0])); ;}
    break;

  case 7:
#line 250 "ned.y"
    { if (ps.storeSourceCode) storeComponentSourceCode(ps.module, (yylsp[0])); ;}
    break;

  case 8:
#line 252 "ned.y"
    { if (ps.storeSourceCode) storeComponentSourceCode(ps.module, (yylsp[0])); ;}
    break;

  case 9:
#line 254 "ned.y"
    { if (ps.storeSourceCode) storeComponentSourceCode(ps.module, (yylsp[0])); ;}
    break;

  case 15:
#line 263 "ned.y"
    { if (ps.storeSourceCode) storeComponentSourceCode(ps.enump, (yylsp[0])); ;}
    break;

  case 16:
#line 265 "ned.y"
    { if (ps.storeSourceCode) storeComponentSourceCode(ps.messagep, (yylsp[0])); ;}
    break;

  case 17:
#line 267 "ned.y"
    { if (ps.storeSourceCode) storeComponentSourceCode(ps.classp, (yylsp[0])); ;}
    break;

  case 18:
#line 269 "ned.y"
    { if (ps.storeSourceCode) storeComponentSourceCode(ps.structp, (yylsp[0])); ;}
    break;

  case 22:
#line 287 "ned.y"
    {
                  ps.import = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  ps.import->setFilename(toString(trimQuotes((yylsp[0]))));
                  setComments(ps.import,(yylsp[0]));
                ;}
    break;

  case 24:
#line 303 "ned.y"
    {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString((yylsp[0])));
                  setComments(ps.channel,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 27:
#line 317 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString((yylsp[-2])));
                  addExpression(ps.chanattr, "value",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.channel,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 28:
#line 323 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString((yylsp[-2])));
                  addExpression(ps.chanattr, "value",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.channel,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 29:
#line 332 "ned.y"
    {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                ;}
    break;

  case 30:
#line 336 "ned.y"
    {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                ;}
    break;

  case 32:
#line 353 "ned.y"
    {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 33:
#line 362 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 34:
#line 366 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 36:
#line 386 "ned.y"
    {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 37:
#line 395 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 38:
#line 399 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 41:
#line 414 "ned.y"
    {
                  ps.property = addComponentProperty(ps.module, "display");
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[-1])), (yylsp[-1]));
                  ps.propkey->appendChild(literal);
                ;}
    break;

  case 44:
#line 432 "ned.y"
    {
                  ps.params = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.module );
                  setComments(ps.params,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 45:
#line 437 "ned.y"
    {
                ;}
    break;

  case 48:
#line 448 "ned.y"
    {
                  setComments(ps.param,(yylsp[0]));
                ;}
    break;

  case 49:
#line 452 "ned.y"
    {
                  setComments(ps.param,(yylsp[0]));
                ;}
    break;

  case 50:
#line 462 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[0]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  ps.param->setIsFunction(true); // because CONST is missing
                ;}
    break;

  case 51:
#line 468 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                  ps.param->setIsFunction(true); // because CONST is missing
                ;}
    break;

  case 52:
#line 474 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-1]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                ;}
    break;

  case 53:
#line 479 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                ;}
    break;

  case 54:
#line 484 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-3]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                ;}
    break;

  case 55:
#line 489 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-3]));
                  ps.param->setType(NED_PARTYPE_DOUBLE);
                ;}
    break;

  case 56:
#line 494 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_STRING);
                ;}
    break;

  case 57:
#line 499 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_BOOL);
                ;}
    break;

  case 58:
#line 504 "ned.y"
    {
                  ps.param = addParameter(ps.params, (yylsp[-2]));
                  ps.param->setType(NED_PARTYPE_XML);
                ;}
    break;

  case 59:
#line 509 "ned.y"
    {
                  NEDError(ps.params,"type 'anytype' no longer supported");
                ;}
    break;

  case 62:
#line 524 "ned.y"
    {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 63:
#line 529 "ned.y"
    {
                ;}
    break;

  case 72:
#line 552 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[-2]));
                  ps.gate->setType(NED_GATETYPE_INPUT);
                  ps.gate->setIsVector(true);
                  //setComments(ps.gate,@1,@3);
                ;}
    break;

  case 73:
#line 559 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[0]));
                  ps.gate->setType(NED_GATETYPE_INPUT);
                  //setComments(ps.gate,@1);
                ;}
    break;

  case 76:
#line 573 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[-2]));
                  ps.gate->setType(NED_GATETYPE_OUTPUT);
                  ps.gate->setIsVector(true);
                  //setComments(ps.gate,@1,@3);
                ;}
    break;

  case 77:
#line 580 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[0]));
                  ps.gate->setType(NED_GATETYPE_OUTPUT);
                  //setComments(ps.gate,@1,@3);
                ;}
    break;

  case 80:
#line 597 "ned.y"
    {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 81:
#line 602 "ned.y"
    {
                ;}
    break;

  case 86:
#line 618 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-3])));
                  ps.submod->setType(toString((yylsp[-1])));
                  //setComments(ps.submod,@1,@4);
                ;}
    break;

  case 87:
#line 625 "ned.y"
    {
                ;}
    break;

  case 88:
#line 628 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-4])));
                  ps.submod->setType(toString((yylsp[-2])));
                  addVector(ps.submod, "vector-size",(yylsp[-1]),(yyvsp[-1]));
                  //setComments(ps.submod,@1,@5);
                ;}
    break;

  case 89:
#line 636 "ned.y"
    {
                ;}
    break;

  case 90:
#line 639 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-5])));
                  ps.submod->setLikeType(toString((yylsp[-1])));
                  ps.submod->setLikeParam(toString((yylsp[-3]))); //FIXME store as expression!!!
                  //setComments(ps.submod,@1,@6);
                ;}
    break;

  case 91:
#line 647 "ned.y"
    {
                ;}
    break;

  case 92:
#line 650 "ned.y"
    {
                  ps.submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE, ps.submods);
                  ps.submod->setName(toString((yylsp[-6])));
                  ps.submod->setLikeType(toString((yylsp[-2])));
                  ps.submod->setLikeParam(toString((yylsp[-4]))); //FIXME store as expression!!!
                  addVector(ps.submod, "vector-size",(yylsp[-3]),(yyvsp[-3]));
                  //setComments(ps.submod,@1,@7);
                ;}
    break;

  case 93:
#line 659 "ned.y"
    {
                ;}
    break;

  case 99:
#line 684 "ned.y"
    {
                  ps.substparams = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 100:
#line 689 "ned.y"
    {
                ;}
    break;

  case 101:
#line 692 "ned.y"
    {
                  ps.substparams = (ParametersNode *)createNodeWithTag(NED_PARAMETERS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.substparams,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 102:
#line 698 "ned.y"
    {
                ;}
    break;

  case 107:
#line 715 "ned.y"
    {
                  ps.substparam = addSubstparam(ps.substparams,(yylsp[-2]));
                  addExpression(ps.substparam, "value",(yylsp[0]),(yyvsp[0]));
                  setComments(ps.substparam,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 110:
#line 732 "ned.y"
    {
                  ps.gatesizes = (GatesNode *)createNodeWithTag(NED_GATES, ps.submod );
                  setComments(ps.gatesizes,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 111:
#line 737 "ned.y"
    {
                ;}
    break;

  case 112:
#line 740 "ned.y"
    {
                  ps.gatesizes = (GatesNode *)createNodeWithTag(NED_GATES, ps.submod );
                  addExpression(ps.gatesizes, "condition",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.gatesizes,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 113:
#line 746 "ned.y"
    {
                ;}
    break;

  case 118:
#line 762 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,(yylsp[-1]));
                  addVector(ps.gatesize, "vector-size",(yylsp[0]),(yyvsp[0]));

                  setComments(ps.gatesize,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 119:
#line 769 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,(yylsp[0]));
                  setComments(ps.gatesize,(yylsp[0]));
                ;}
    break;

  case 120:
#line 780 "ned.y"
    {
                  ps.property = addComponentProperty(ps.submod, "display");
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[-1])), (yylsp[-1]));
                  ps.propkey->appendChild(literal);
                ;}
    break;

  case 124:
#line 799 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setAllowUnconnected(true);
                  setComments(ps.conns,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 125:
#line 805 "ned.y"
    {
                ;}
    break;

  case 126:
#line 808 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setAllowUnconnected(false);
                  setComments(ps.conns,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 127:
#line 814 "ned.y"
    {
                ;}
    break;

  case 134:
#line 835 "ned.y"
    {
                  ps.conngroup = (ConnectionGroupNode *)createNodeWithTag(NED_CONNECTION_GROUP, ps.conns);
                  ps.where = (WhereNode *)createNodeWithTag(NED_WHERE, ps.conngroup);
                  ps.inLoop=1;
                ;}
    break;

  case 135:
#line 841 "ned.y"
    {
                  ps.inLoop=0;
                  //setComments(ps.where,@1,@4);
                  //setTrailingComment(ps.where,@6);
                ;}
    break;

  case 138:
#line 855 "ned.y"
    {
                  ps.loop = addLoop(ps.where,(yylsp[-4]));
                  addExpression(ps.loop, "from-value",(yylsp[-2]),(yyvsp[-2]));
                  addExpression(ps.loop, "to-value",(yylsp[0]),(yyvsp[0]));
                  setComments(ps.loop,(yylsp[-4]),(yylsp[0]));
                ;}
    break;

  case 139:
#line 865 "ned.y"
    {
                  addExpression(ps.conn, "condition",(yylsp[0]),(yyvsp[0])); //FIXME is condition in a conngroup allowed?
                ;}
    break;

  case 141:
#line 873 "ned.y"
    {
                  //FIXME add chanspec here if doesn't exist!
                  ps.property = addComponentProperty(ps.chanspec, "display");
                  ps.propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, ps.property);
                  LiteralNode *literal = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[0])), (yylsp[0]));
                  ps.propkey->appendChild(literal);
                ;}
    break;

  case 145:
#line 890 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                ;}
    break;

  case 146:
#line 895 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_L2R);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                ;}
    break;

  case 147:
#line 900 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                ;}
    break;

  case 148:
#line 906 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_R2L);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                ;}
    break;

  case 151:
#line 920 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "src-module-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 152:
#line 926 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[0])) );
                ;}
    break;

  case 153:
#line 934 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 154:
#line 939 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[0])) );
                ;}
    break;

  case 155:
#line 943 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  ps.conn->setSrcGatePlusplus(true);
                ;}
    break;

  case 156:
#line 951 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 157:
#line 958 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[0])));
                ;}
    break;

  case 158:
#line 964 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement *)ps.conngroup : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  ps.conn->setSrcGatePlusplus(true);
                ;}
    break;

  case 161:
#line 979 "ned.y"
    {
                  ps.conn->setDestModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "dest-module-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 162:
#line 984 "ned.y"
    {
                  ps.conn->setDestModule( toString((yylsp[0])) );
                ;}
    break;

  case 163:
#line 991 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 164:
#line 996 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                ;}
    break;

  case 165:
#line 1000 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                ;}
    break;

  case 166:
#line 1008 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 167:
#line 1013 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                ;}
    break;

  case 168:
#line 1017 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                ;}
    break;

  case 169:
#line 1026 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",(yylsp[0]),createExpression(createLiteral(NED_CONST_STRING, toString((yylsp[0])))));
                ;}
    break;

  case 170:
#line 1031 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString((yylsp[-1])));
                  addExpression(ps.connattr, "value",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 171:
#line 1036 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString((yylsp[-1])));
                  addExpression(ps.connattr, "value",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 173:
#line 1053 "ned.y"
    {
                  ps.network = addNetwork(ps.nedfile,(yylsp[-3]),(yylsp[-1]));
                  setComments(ps.network,(yylsp[-4]),(yylsp[0]));
                  ps.inNetwork=1;
                ;}
    break;

  case 174:
#line 1062 "ned.y"
    {
                  setTrailingComment(ps.network,(yylsp[-1]));
                  ps.inNetwork=0;
                ;}
    break;

  case 175:
#line 1073 "ned.y"
    { (yyval) = (yyvsp[-1]); ;}
    break;

  case 176:
#line 1079 "ned.y"
    {
                  if (ps.parseExpressions) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 177:
#line 1083 "ned.y"
    {
                  if (ps.parseExpressions) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 178:
#line 1087 "ned.y"
    {
                  if (ps.parseExpressions) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 179:
#line 1098 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("input", (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 180:
#line 1100 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("input", (yyvsp[-1])); ;}
    break;

  case 181:
#line 1102 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("input"); ;}
    break;

  case 182:
#line 1104 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("input"); ;}
    break;

  case 183:
#line 1109 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("xmldoc", (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 184:
#line 1111 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("xmldoc", (yyvsp[-1])); ;}
    break;

  case 186:
#line 1117 "ned.y"
    { (yyval) = (yyvsp[-1]); ;}
    break;

  case 187:
#line 1120 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("+", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 188:
#line 1122 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("-", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 189:
#line 1124 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("*", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 190:
#line 1126 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("/", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 191:
#line 1128 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("%", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 192:
#line 1130 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("^", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 193:
#line 1134 "ned.y"
    { if (ps.parseExpressions) (yyval) = unaryMinus((yyvsp[0])); ;}
    break;

  case 194:
#line 1137 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("==", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 195:
#line 1139 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("!=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 196:
#line 1141 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator(">", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 197:
#line 1143 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator(">=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 198:
#line 1145 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("<", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 199:
#line 1147 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("<=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 200:
#line 1150 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("&&", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 201:
#line 1152 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("||", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 202:
#line 1154 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("##", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 203:
#line 1158 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("!", (yyvsp[0])); ;}
    break;

  case 204:
#line 1161 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("&", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 205:
#line 1163 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("|", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 206:
#line 1165 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("#", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 207:
#line 1169 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("~", (yyvsp[0])); ;}
    break;

  case 208:
#line 1171 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("<<", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 209:
#line 1173 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator(">>", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 210:
#line 1175 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("?:", (yyvsp[-4]), (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 211:
#line 1178 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction(toString((yylsp[-2]))); ;}
    break;

  case 212:
#line 1180 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction(toString((yylsp[-3])), (yyvsp[-1])); ;}
    break;

  case 213:
#line 1182 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction(toString((yylsp[-5])), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 214:
#line 1184 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction(toString((yylsp[-7])), (yyvsp[-5]), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 215:
#line 1186 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction(toString((yylsp[-9])), (yyvsp[-7]), (yyvsp[-5]), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 219:
#line 1197 "ned.y"
    {
                  // if there's no modifier, might be a loop variable too
                  if (ps.parseExpressions) (yyval) = createIdent(toString((yylsp[0])));
                ;}
    break;

  case 220:
#line 1202 "ned.y"
    {
                  if (ps.parseExpressions) (yyval) = createIdent(toString((yylsp[0])));
                  NEDError(ps.params,"`ref' modifier no longer supported (add `function' "
                                     "modifier to destination parameter instead)");
                ;}
    break;

  case 221:
#line 1208 "ned.y"
    {
                  if (ps.parseExpressions) (yyval) = createIdent(toString((yylsp[0])));
                  NEDError(ps.params,"`ancestor' and `ref' modifiers no longer supported");
                ;}
    break;

  case 222:
#line 1213 "ned.y"
    {
                  if (ps.parseExpressions) (yyval) = createIdent(toString((yylsp[0])));
                  NEDError(ps.params,"`ancestor' and `ref' modifiers no longer supported");
                ;}
    break;

  case 223:
#line 1218 "ned.y"
    {
                  if (ps.parseExpressions) (yyval) = createIdent(toString((yylsp[0])));
                  NEDError(ps.params,"`ancestor' modifier no longer supported");
                ;}
    break;

  case 224:
#line 1226 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("index"); ;}
    break;

  case 225:
#line 1228 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("index"); ;}
    break;

  case 226:
#line 1230 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("sizeof", createIdent(toString((yylsp[-1])))); ;}
    break;

  case 230:
#line 1241 "ned.y"
    { if (ps.parseExpressions) (yyval) = createLiteral(NED_CONST_STRING, trimQuotes((yylsp[0])), (yylsp[0])); ;}
    break;

  case 231:
#line 1246 "ned.y"
    { if (ps.parseExpressions) (yyval) = createLiteral(NED_CONST_BOOL, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 232:
#line 1248 "ned.y"
    { if (ps.parseExpressions) (yyval) = createLiteral(NED_CONST_BOOL, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 233:
#line 1253 "ned.y"
    { if (ps.parseExpressions) (yyval) = createLiteral(NED_CONST_INT, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 234:
#line 1255 "ned.y"
    { if (ps.parseExpressions) (yyval) = createLiteral(NED_CONST_DOUBLE, (yylsp[0]), (yylsp[0])); ;}
    break;

  case 235:
#line 1257 "ned.y"
    { if (ps.parseExpressions) (yyval) = createQuantity(toString((yylsp[0]))); ;}
    break;

  case 240:
#line 1273 "ned.y"
    {
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces((yylsp[-1]))));
                  setComments(ps.cplusplus,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 241:
#line 1282 "ned.y"
    {
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString((yylsp[-1])));
                  setComments(ps.structdecl,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 242:
#line 1291 "ned.y"
    {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString((yylsp[-1])));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 243:
#line 1298 "ned.y"
    {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString((yylsp[-1])));
                  setComments(ps.classdecl,(yylsp[-3]),(yylsp[-2]));
                ;}
    break;

  case 244:
#line 1308 "ned.y"
    {
                  ps.messagedecl = (MessageDeclNode *)createNodeWithTag(NED_MESSAGE_DECL, ps.nedfile );
                  ps.messagedecl->setName(toString((yylsp[-1])));
                  setComments(ps.messagedecl,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 245:
#line 1317 "ned.y"
    {
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString((yylsp[-1])));
                  setComments(ps.enumdecl,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 246:
#line 1326 "ned.y"
    {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString((yylsp[-1])));
                  setComments(ps.enump,(yylsp[-2]),(yylsp[-1]));
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;}
    break;

  case 247:
#line 1333 "ned.y"
    {
                  setTrailingComment(ps.enump,(yylsp[-1]));
                ;}
    break;

  case 248:
#line 1337 "ned.y"
    {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString((yylsp[-3])));
                  ps.enump->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.enump,(yylsp[-4]),(yylsp[-1]));
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;}
    break;

  case 249:
#line 1345 "ned.y"
    {
                  setTrailingComment(ps.enump,(yylsp[-1]));
                ;}
    break;

  case 254:
#line 1362 "ned.y"
    {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString((yylsp[-1])));
                  setComments(ps.enumfield,(yylsp[-1]),(yylsp[-1]));
                ;}
    break;

  case 255:
#line 1368 "ned.y"
    {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString((yylsp[-3])));
                  ps.enumfield->setValue(toString((yylsp[-1])));
                  setComments(ps.enumfield,(yylsp[-3]),(yylsp[-1]));
                ;}
    break;

  case 256:
#line 1378 "ned.y"
    {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString((yylsp[-1])));
                  setComments(ps.messagep,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 257:
#line 1384 "ned.y"
    {
                  setTrailingComment(ps.messagep,(yylsp[-1]));
                ;}
    break;

  case 258:
#line 1388 "ned.y"
    {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString((yylsp[-3])));
                  ps.messagep->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.messagep,(yylsp[-4]),(yylsp[-1]));
                ;}
    break;

  case 259:
#line 1395 "ned.y"
    {
                  setTrailingComment(ps.messagep,(yylsp[-1]));
                ;}
    break;

  case 260:
#line 1402 "ned.y"
    {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString((yylsp[-1])));
                  setComments(ps.classp,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 261:
#line 1408 "ned.y"
    {
                  setTrailingComment(ps.classp,(yylsp[-1]));
                ;}
    break;

  case 262:
#line 1412 "ned.y"
    {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString((yylsp[-3])));
                  ps.classp->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.classp,(yylsp[-4]),(yylsp[-1]));
                ;}
    break;

  case 263:
#line 1419 "ned.y"
    {
                  setTrailingComment(ps.classp,(yylsp[-1]));
                ;}
    break;

  case 264:
#line 1426 "ned.y"
    {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString((yylsp[-1])));
                  setComments(ps.structp,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 265:
#line 1432 "ned.y"
    {
                  setTrailingComment(ps.structp,(yylsp[-1]));
                ;}
    break;

  case 266:
#line 1436 "ned.y"
    {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString((yylsp[-3])));
                  ps.structp->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.structp,(yylsp[-4]),(yylsp[-1]));
                ;}
    break;

  case 267:
#line 1443 "ned.y"
    {
                  setTrailingComment(ps.structp,(yylsp[-1]));
                ;}
    break;

  case 268:
#line 1450 "ned.y"
    {
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,(yylsp[-1]));
                ;}
    break;

  case 275:
#line 1470 "ned.y"
    {
                  ps.msgproperty = (MsgpropertyNode *)createNodeWithTag(NED_MSGPROPERTY, ps.properties);
                  ps.msgproperty->setName(toString((yylsp[-3])));
                  ps.msgproperty->setValue(toString((yylsp[-1])));
                  setComments(ps.msgproperty,(yylsp[-3]),(yylsp[-1]));
                ;}
    break;

  case 282:
#line 1489 "ned.y"
    {
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,(yylsp[-1]));
                ;}
    break;

  case 289:
#line 1509 "ned.y"
    {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString((yylsp[0])));
                  ps.field->setDataType(toString((yylsp[-1])));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                ;}
    break;

  case 290:
#line 1517 "ned.y"
    {
                  setComments(ps.field,(yylsp[-7]),(yylsp[-1]));
                ;}
    break;

  case 291:
#line 1521 "ned.y"
    {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString((yylsp[0])));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                ;}
    break;

  case 292:
#line 1528 "ned.y"
    {
                  setComments(ps.field,(yylsp[-6]),(yylsp[-1]));
                ;}
    break;

  case 293:
#line 1535 "ned.y"
    { ps.isAbstract = true; ps.isReadonly = false; ;}
    break;

  case 294:
#line 1537 "ned.y"
    { ps.isAbstract = false; ps.isReadonly = true; ;}
    break;

  case 295:
#line 1539 "ned.y"
    { ps.isAbstract = true; ps.isReadonly = true; ;}
    break;

  case 296:
#line 1541 "ned.y"
    { ps.isAbstract = true; ps.isReadonly = true; ;}
    break;

  case 297:
#line 1543 "ned.y"
    { ps.isAbstract = false; ps.isReadonly = false; ;}
    break;

  case 311:
#line 1568 "ned.y"
    {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString((yylsp[-1])));
                ;}
    break;

  case 312:
#line 1573 "ned.y"
    {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString((yylsp[-1])));
                ;}
    break;

  case 313:
#line 1578 "ned.y"
    {
                  ps.field->setIsVector(true);
                ;}
    break;

  case 315:
#line 1586 "ned.y"
    {
                  ps.field->setEnumName(toString((yylsp[-1])));
                ;}
    break;

  case 317:
#line 1594 "ned.y"
    {
                  ps.field->setDefaultValue(toString((yylsp[0])));
                ;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 3483 "ned.tab.c"

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


#line 1623 "ned.y"


//----------------------------------------------------------------------
// general bison/flex stuff:
//

char yyfailure[250] = "";
extern int yydebug; /* needed if compiled with yacc --VA */

extern char textbuf[];

int runparse (NEDParser *p,NedFileNode *nf,bool parseexpr, bool storesrc, const char *sourcefname)
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

    // create parser state and NEDFileNode
    np = p;
    ps.nedfile = nf;
    ps.parseExpressions = parseexpr;
    ps.storeSourceCode = storesrc;
    sourcefilename = sourcefname;

    if (storesrc)
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
        delete e;
        return 0;
    }

    return ret;
}


void yyerror (const char *s)
{
    if (strlen(s))
        strcpy(yyfailure, s);

    // chop newline
    if (yyfailure[strlen(yyfailure)-1] == '\n')
        yyfailure[strlen(yyfailure)-1] = '\0';

    np->error(yyfailure, pos.li);
}

const char *toString(YYLTYPE pos)
{
    return np->nedsource->get(pos);
}

const char *toString(long l)
{
    static char buf[32];
    sprintf(buf,"%ld", l);
    return buf;
}

NEDElement *createNodeWithTag(int tagcode, NEDElement *parent)
{
    // create via a factory
    NEDElement *e = NEDElementFactory::getInstance()->createNodeWithTag(tagcode);

    // "debug info"
    char buf[200];
    sprintf(buf,"%s:%d",sourcefilename, pos.li);
    e->setSourceLocation(buf);

    // add to parent
    if (parent)
       parent->appendChild(e);

    return e;
}

//
// Properties
//

PropertyNode *addProperty(NEDElement *node, const char *name)
{
    PropertyNode *prop = (PropertyNode *)createNodeWithTag(NED_PROPERTY, node);
    prop->setName(name);
    return prop;
}

PropertyNode *addComponentProperty(NEDElement *node, const char *name)
{
    // add propery under the ParametersNode; create that if not yet exists
    NEDElement *params = node->getFirstChildWithTag(NED_PARAMETERS);
    if (!params)
        params = createNodeWithTag(NED_PARAMETERS, node);
    PropertyNode *prop = (PropertyNode *)createNodeWithTag(NED_PROPERTY, params);
    prop->setName(name);
    return prop;
}

//
// Spec Properties: source code, display string
//

PropertyNode *storeSourceCode(NEDElement *node, YYLTYPE tokenpos)
{
     PropertyNode *prop = addProperty(node, "sourcecode");
     prop->setIsImplicit(true);
     PropertyKeyNode *propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, prop);
     propkey->appendChild(createLiteral(NED_CONST_STRING, tokenpos, tokenpos));  // FIXME don't store it twice
     return prop;
}

PropertyNode *storeComponentSourceCode(NEDElement *node, YYLTYPE tokenpos)
{
     PropertyNode *prop = addComponentProperty(node, "sourcecode");
     prop->setIsImplicit(true);
     PropertyKeyNode *propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, prop);
     propkey->appendChild(createLiteral(NED_CONST_STRING, tokenpos, tokenpos));  // FIXME don't store it twice
     return prop;
}

//
// Comments
//

void setFileComment(NEDElement *node)
{
//XXX    node->setAttribute("file-comment", np->nedsource->getFileComment() );
}

void setBannerComment(NEDElement *node, YYLTYPE tokenpos)
{
//XXX    node->setAttribute("banner-comment", np->nedsource->getBannerComment(tokenpos) );
}

void setRightComment(NEDElement *node, YYLTYPE tokenpos)
{
//XXX    node->setAttribute("right-comment", np->nedsource->getTrailingComment(tokenpos) );
}

void setTrailingComment(NEDElement *node, YYLTYPE tokenpos)
{
//XXX    node->setAttribute("trailing-comment", np->nedsource->getTrailingComment(tokenpos) );
}

void setComments(NEDElement *node, YYLTYPE pos)
{
//XXX    setBannerComment(node, pos);
//XXX    setRightComment(node, pos);
}

void setComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos)
{
    YYLTYPE pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;

//XXX    setBannerComment(node, pos);
//XXX    setRightComment(node, pos);
}

ParamNode *addParameter(NEDElement *params, YYLTYPE namepos)
{
   ParamNode *param = (ParamNode *)createNodeWithTag(NED_PARAM,params);
   param->setName( toString( namepos) );
   return param;
}

GateNode *addGate(NEDElement *gates, YYLTYPE namepos)
{
   GateNode *gate = (GateNode *)createNodeWithTag(NED_GATE,gates);
   gate->setName( toString( namepos) );
   return gate;
}

LoopNode *addLoop(NEDElement *conngroup, YYLTYPE varnamepos)
{
   LoopNode *loop = (LoopNode *)createNodeWithTag(NED_LOOP,conngroup);
   loop->setParamName( toString( varnamepos) );
   return loop;
}

YYLTYPE trimBrackets(YYLTYPE vectorpos)
{
   // should check it's really brackets that get chopped off
   vectorpos.first_column++;
   vectorpos.last_column--;
   return vectorpos;
}

YYLTYPE trimAngleBrackets(YYLTYPE vectorpos)
{
   // should check it's really angle brackets that get chopped off
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

YYLTYPE trimDoubleBraces(YYLTYPE vectorpos)
{
   // should check it's really '{{' and '}}' that get chopped off
   vectorpos.first_column+=2;
   vectorpos.last_column-=2;
   return vectorpos;
}

void addVector(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr)
{
   addExpression(elem, attrname, trimBrackets(exprpos), expr);
}

void addLikeParam(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr)
{
   if (ps.parseExpressions && !expr)
       elem->setAttribute(attrname, toString(trimAngleBrackets(exprpos)));
   else
       addExpression(elem, attrname, trimAngleBrackets(exprpos), expr);
}

void addExpression(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr)
{
   if (ps.parseExpressions) {
       elem->appendChild(expr);
       ((ExpressionNode *)expr)->setTarget(attrname);
   } else {
       elem->setAttribute(attrname, toString(exprpos));
   }
}

void swapConnection(NEDElement *conn)
{
   swapAttributes(conn, "src-module", "dest-module");
   swapAttributes(conn, "src-module-index", "dest-module-index");
   swapAttributes(conn, "src-gate", "dest-gate");
   swapAttributes(conn, "src-gate-index", "dest-gate-index");
   swapAttributes(conn, "src-gate-plusplus", "dest-gate-plusplus");
   swapAttributes(conn, "src-gate-subg", "dest-gate-subg");

   swapExpressionChildren(conn, "src-module-index", "dest-module-index");
   swapExpressionChildren(conn, "src-gate-index", "dest-gate-index");
}

void swapAttributes(NEDElement *node, const char *attr1, const char *attr2)
{
   std::string oldv1(node->getAttribute(attr1));
   node->setAttribute(attr1,node->getAttribute(attr2));
   node->setAttribute(attr2,oldv1.c_str());
}

void swapExpressionChildren(NEDElement *node, const char *attr1, const char *attr2)
{
   ExpressionNode *expr1, *expr2;
   for (expr1=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr1; expr1=expr1->getNextExpressionNodeSibling())
      if (!strcmp(expr1->getTarget(),attr1))
          break;
   for (expr2=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr2; expr2=expr2->getNextExpressionNodeSibling())
      if (!strcmp(expr2->getTarget(),attr2))
          break;

   if (expr1) expr1->setTarget(attr2);
   if (expr2) expr2->setTarget(attr1);
}

OperatorNode *createOperator(const char *op, NEDElement *operand1, NEDElement *operand2, NEDElement *operand3)
{
   OperatorNode *opnode = (OperatorNode *)createNodeWithTag(NED_OPERATOR);
   opnode->setName(op);
   opnode->appendChild(operand1);
   if (operand2) opnode->appendChild(operand2);
   if (operand3) opnode->appendChild(operand3);
   return opnode;
}

FunctionNode *createFunction(const char *funcname, NEDElement *arg1, NEDElement *arg2, NEDElement *arg3, NEDElement *arg4)
{
   FunctionNode *funcnode = (FunctionNode *)createNodeWithTag(NED_FUNCTION);
   funcnode->setName(funcname);
   if (arg1) funcnode->appendChild(arg1);
   if (arg2) funcnode->appendChild(arg2);
   if (arg3) funcnode->appendChild(arg3);
   if (arg4) funcnode->appendChild(arg4);
   return funcnode;
}

ExpressionNode *createExpression(NEDElement *expr)
{
   ExpressionNode *expression = (ExpressionNode *)createNodeWithTag(NED_EXPRESSION);
   expression->appendChild(expr);
   return expression;
}

IdentNode *createIdent(const char *param, const char *paramindex, const char *module, const char *moduleindex)
{
   IdentNode *par = (IdentNode *)createNodeWithTag(NED_IDENT);
   par->setName(param);
   // if (paramindex) par->setParamIndex(paramindex);
   if (module) par->setModule(module);
   if (moduleindex) par->setModuleIndex(moduleindex);
   return par;
}

LiteralNode *createLiteral(int type, YYLTYPE valuepos, YYLTYPE textpos)
{
   LiteralNode *c = (LiteralNode *)createNodeWithTag(NED_LITERAL);
   c->setType(type);
   c->setValue(toString(valuepos));
   c->setText(toString(textpos));
   return c;
}

LiteralNode *createQuantity(const char *text)
{
   LiteralNode *c = (LiteralNode *)createNodeWithTag(NED_LITERAL);
   c->setType(NED_CONST_UNIT);
   if (text) c->setText(text);

   double t = NEDStrToSimtime(text);
   if (t<0)
   {
       char msg[130];
       sprintf(msg,"invalid time constant '%.100s'",text);
       np->error(msg, pos.li);
   }
   char buf[32];
   sprintf(buf,"%g",t);
   c->setValue(buf);

   return c;
}

NEDElement *unaryMinus(NEDElement *node)
{
    // if not a constant, must appy unary minus operator
    if (node->getTagCode()!=NED_LITERAL)
        return createOperator("-", node);

    LiteralNode *constNode = (LiteralNode *)node;

    // only int and real constants can be negative, string, bool, etc cannot
    if (constNode->getType()!=NED_CONST_INT && constNode->getType()!=NED_CONST_DOUBLE)
    {
       char msg[140];
       sprintf(msg,"unary minus not accepted before '%.100s'",constNode->getValue());
       np->error(msg, pos.li);
       return node;
    }

    // prepend the constant with a '-'
    char *buf = new char[strlen(constNode->getValue())+2];
    buf[0] = '-';
    strcpy(buf+1, constNode->getValue());
    constNode->setValue(buf);
    delete [] buf;

    return node;
}

