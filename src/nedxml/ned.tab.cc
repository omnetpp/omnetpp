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
void yyerror (char *s);


#include "nedparser.h"
#include "nedfilebuffer.h"
#include "nedelements.h"
#include "nedutil.h"

/*
 * use_chanattrname_token:
 * It turns on/off recognizing the "delay", "error", "datarate" words
 * as CHANATTRNAME tokens. If it's off, they're simply returned as NAME.
 * The CHANATTRNAME token is necessary to get the parsing of connection
 * attributes right with the old NED syntax -- in the new syntax
 * they should be treated as any arbitrary word (NAME).
 */
int use_chanattrname_token;

static YYLTYPE NULLPOS={0,0,0,0,0,0};

static const char *sourcefilename;

NEDParser *np;

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
    ImportNode *imports;
    ImportedFileNode *import;
    ChannelNode *channel;
    ChannelAttrNode *chanattr;
    NetworkNode *network;
    NEDElement *module;  // in fact, CompoundModuleNode* or SimpleModule*
    ParamsNode *params;
    ParamNode *param;
    GatesNode *gates;
    GateNode *gate;
    MachinesNode *machines;
    MachineNode *machine;
    SubmodulesNode *submods;
    SubmoduleNode *submod;
    SubstparamsNode *substparams;
    SubstparamNode *substparam;
    GatesizesNode *gatesizes;
    GatesizeNode *gatesize;
    SubstmachinesNode *substmachines;
    SubstmachineNode *substmachine;
    ConnectionsNode *conns;
    ConnectionNode *conn;
    ConnAttrNode *connattr;
    ForLoopNode *forloop;
    LoopVarNode *loopvar;

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
    PropertyNode *property;
    FieldsNode *fields;
    FieldNode *field;
} ps;

NEDElement *createNodeWithTag(int tagcode, NEDElement *parent=NULL);

void setFileComment(NEDElement *node);
void setBannerComment(NEDElement *node, YYLTYPE tokenpos);
void setRightComment(NEDElement *node, YYLTYPE tokenpos);
void setTrailingComment(NEDElement *node, YYLTYPE tokenpos);
void setComments(NEDElement *node, YYLTYPE pos);
void setComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos);

ChannelAttrNode *addChanAttr(NEDElement *channel, const char *attrname);
ParamNode *addParameter(NEDElement *params, YYLTYPE namepos, int type);
GateNode *addGate(NEDElement *gates, YYLTYPE namepos, int is_in, int is_vector );
SubmoduleNode *addSubmodule(NEDElement *submods, YYLTYPE namepos, YYLTYPE typepos,YYLTYPE likeparampos);
GatesizeNode *addGateSize(NEDElement *gatesizes, YYLTYPE namepos);
SubstparamNode *addSubstparam(NEDElement *substparams, YYLTYPE namepos);
SubstmachineNode *addSubstmachine(NEDElement *substmachines, YYLTYPE namepos);
ConnAttrNode *addConnAttr(NEDElement *conn, const char *attrname);
LoopVarNode *addLoopVar(NEDElement *forloop, YYLTYPE varnamepos);
NetworkNode *addNetwork(NEDElement *nedfile, YYLTYPE namepos, YYLTYPE typepos);
DisplayStringNode *addDisplayString(NEDElement *parent, YYLTYPE dispstrpos);

YYLTYPE trimBrackets(YYLTYPE vectorpos);
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
ParamRefNode *createParamRef(const char *param, const char *paramindex=NULL, const char *module=NULL, const char *moduleindex=NULL);
IdentNode *createIdent(const char *name);
ConstNode *createConst(int type, const char *value, const char *text=NULL);
ConstNode *createTimeConst(const char *text);
NEDElement *createParamRefOrIdent(const char *name);
NEDElement *unaryMinus(NEDElement *node);

void addVector(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);
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
#line 453 "ned.tab.c"

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
#define YYLAST   1025

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  106
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  253
/* YYNRULES -- Number of rules. */
#define YYNRULES  547
/* YYNRULES -- Number of states. */
#define YYNSTATES  909

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
     104,   105,    91,    89,    97,    90,   103,    92,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    88,    96,
       2,   100,     2,    87,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   101,     2,   102,    94,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    98,     2,    99,     2,     2,     2,     2,
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
      39,    41,    43,    45,    46,    51,    55,    57,    59,    63,
      66,    67,    69,    74,    78,    82,    85,    91,    94,    96,
      97,   103,   108,   114,   117,   121,   124,   132,   135,   144,
     147,   151,   154,   165,   168,   170,   171,   172,   177,   180,
     181,   185,   187,   189,   191,   192,   193,   198,   201,   202,
     206,   208,   210,   212,   213,   218,   220,   221,   226,   228,
     229,   230,   235,   238,   239,   243,   245,   247,   248,   249,
     254,   257,   258,   262,   264,   266,   268,   272,   275,   279,
     284,   289,   293,   297,   301,   305,   307,   308,   309,   314,
     316,   317,   322,   326,   331,   335,   339,   341,   345,   347,
     351,   353,   357,   359,   361,   362,   363,   368,   370,   371,
     376,   380,   385,   389,   393,   395,   399,   401,   405,   407,
     411,   413,   415,   416,   417,   422,   424,   425,   428,   430,
     431,   439,   440,   449,   450,   460,   461,   472,   476,   478,
     479,   480,   485,   487,   488,   491,   493,   494,   502,   503,
     512,   513,   523,   524,   535,   539,   541,   542,   545,   547,
     548,   553,   554,   561,   564,   565,   569,   571,   573,   575,
     576,   579,   581,   582,   587,   588,   595,   598,   599,   603,
     605,   607,   609,   610,   613,   615,   616,   621,   622,   629,
     632,   633,   637,   639,   643,   645,   646,   649,   651,   652,
     657,   658,   665,   667,   668,   671,   673,   678,   681,   682,
     683,   688,   689,   696,   699,   700,   704,   706,   709,   711,
     714,   715,   716,   721,   722,   729,   732,   733,   737,   739,
     742,   744,   749,   750,   755,   756,   758,   759,   760,   766,
     767,   772,   774,   775,   778,   780,   782,   784,   785,   793,
     797,   799,   805,   808,   809,   812,   813,   816,   818,   825,
     834,   841,   850,   854,   856,   859,   861,   864,   866,   869,
     872,   874,   877,   881,   883,   886,   888,   891,   893,   896,
     899,   901,   904,   906,   909,   913,   915,   916,   917,   923,
     924,   929,   931,   932,   935,   937,   939,   941,   942,   950,
     954,   956,   962,   965,   966,   969,   970,   973,   975,   982,
     991,   998,  1007,  1011,  1013,  1016,  1018,  1021,  1023,  1026,
    1029,  1031,  1034,  1038,  1040,  1043,  1045,  1048,  1050,  1053,
    1056,  1058,  1061,  1063,  1066,  1070,  1075,  1081,  1084,  1091,
    1097,  1101,  1103,  1105,  1107,  1114,  1119,  1123,  1125,  1132,
    1137,  1139,  1143,  1147,  1151,  1155,  1159,  1163,  1167,  1170,
    1174,  1178,  1182,  1186,  1190,  1194,  1198,  1202,  1206,  1209,
    1213,  1217,  1221,  1224,  1228,  1232,  1238,  1242,  1247,  1254,
    1263,  1274,  1276,  1278,  1280,  1282,  1284,  1286,  1289,  1293,
    1297,  1300,  1302,  1304,  1306,  1308,  1310,  1314,  1319,  1321,
    1323,  1325,  1327,  1331,  1335,  1338,  1341,  1345,  1349,  1353,
    1358,  1362,  1366,  1367,  1375,  1376,  1386,  1388,  1389,  1392,
    1394,  1397,  1402,  1403,  1412,  1413,  1424,  1425,  1434,  1435,
    1446,  1447,  1456,  1457,  1468,  1469,  1474,  1475,  1477,  1478,
    1481,  1483,  1488,  1490,  1492,  1494,  1496,  1498,  1500,  1501,
    1506,  1507,  1509,  1510,  1513,  1515,  1516,  1525,  1526,  1534,
    1536,  1538,  1541,  1544,  1545,  1547,  1550,  1552,  1554,  1556,
    1558,  1561,  1564,  1567,  1570,  1572,  1574,  1576,  1580,  1584,
    1587,  1588,  1593,  1594,  1597,  1598,  1601,  1603,  1605,  1607,
    1609,  1611,  1613,  1615,  1617,  1619,  1621,  1623,  1625,  1627,
    1629,  1631,  1633,  1635,  1637,  1639,  1641,  1643,  1645,  1647,
    1649,  1651,  1653,  1655,  1657,  1659,  1661,  1663,  1665,  1667,
    1669,  1671,  1673,  1676,  1678,  1680,  1681,  1683
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     107,     0,    -1,   108,    -1,   108,   109,    -1,    -1,   110,
      -1,   119,    -1,   126,    -1,   131,    -1,   300,    -1,   114,
      -1,   123,    -1,   128,    -1,   297,    -1,   316,    -1,   317,
      -1,   318,    -1,   319,    -1,   320,    -1,   321,    -1,   327,
      -1,   330,    -1,   333,    -1,    -1,     3,   111,   112,    96,
      -1,   112,    97,   113,    -1,   113,    -1,    35,    -1,   115,
     116,   118,    -1,     5,    34,    -1,    -1,   117,    -1,   117,
      31,   303,   357,    -1,    31,   303,   357,    -1,    26,    34,
     357,    -1,    26,   357,    -1,   120,    98,   121,    99,   357,
      -1,     5,    34,    -1,   122,    -1,    -1,   122,    34,   100,
     303,    96,    -1,    34,   100,   303,    96,    -1,   124,   133,
     149,   161,   125,    -1,     4,    34,    -1,    24,    34,   357,
      -1,    24,   357,    -1,   127,    98,   139,   154,   170,    99,
     357,    -1,     4,    34,    -1,   129,   133,   149,   161,   179,
     249,   145,   130,    -1,     6,    34,    -1,    25,    34,   357,
      -1,    25,   357,    -1,   132,    98,   139,   154,   170,   190,
     273,   147,    99,   357,    -1,     6,    34,    -1,   134,    -1,
      -1,    -1,    29,    88,   135,   136,    -1,   137,    96,    -1,
      -1,   137,    97,   138,    -1,   138,    -1,    34,    -1,   140,
      -1,    -1,    -1,    29,    88,   141,   142,    -1,   143,    96,
      -1,    -1,   143,    97,   144,    -1,   144,    -1,    34,    -1,
     146,    -1,    -1,    12,    88,    35,    96,    -1,   148,    -1,
      -1,    12,    88,    35,    96,    -1,   150,    -1,    -1,    -1,
       7,    88,   151,   152,    -1,   153,    96,    -1,    -1,   153,
      97,   159,    -1,   159,    -1,   155,    -1,    -1,    -1,     7,
      88,   156,   157,    -1,   158,    96,    -1,    -1,   158,    96,
     160,    -1,   160,    -1,   160,    -1,    34,    -1,    34,    88,
      44,    -1,    43,    34,    -1,    34,    88,    43,    -1,    34,
      88,    43,    44,    -1,    34,    88,    44,    43,    -1,    34,
      88,    45,    -1,    34,    88,    46,    -1,    34,    88,    47,
      -1,    34,    88,    48,    -1,   162,    -1,    -1,    -1,     8,
      88,   163,   164,    -1,   165,    -1,    -1,   165,    13,   166,
      96,    -1,    13,   166,    96,    -1,   165,    14,   168,    96,
      -1,    14,   168,    96,    -1,   166,    97,   167,    -1,   167,
      -1,    34,   101,   102,    -1,    34,    -1,   168,    97,   169,
      -1,   169,    -1,    34,   101,   102,    -1,    34,    -1,   171,
      -1,    -1,    -1,     8,    88,   172,   173,    -1,   174,    -1,
      -1,   174,    13,   175,    96,    -1,    13,   175,    96,    -1,
     174,    14,   177,    96,    -1,    14,   177,    96,    -1,   175,
      97,   176,    -1,   176,    -1,    34,   101,   102,    -1,    34,
      -1,   177,    97,   178,    -1,   178,    -1,    34,   101,   102,
      -1,    34,    -1,   180,    -1,    -1,    -1,    10,    88,   181,
     182,    -1,   183,    -1,    -1,   183,   184,    -1,   184,    -1,
      -1,    34,    88,    34,   357,   185,   201,   189,    -1,    -1,
      34,    88,    34,   302,   357,   186,   201,   189,    -1,    -1,
      34,    88,    34,    22,    34,   357,   187,   201,   189,    -1,
      -1,    34,    88,    34,   302,    22,    34,   357,   188,   201,
     189,    -1,   217,   233,   247,    -1,   191,    -1,    -1,    -1,
      10,    88,   192,   193,    -1,   194,    -1,    -1,   194,   195,
      -1,   195,    -1,    -1,    34,    88,    34,   357,   196,   209,
     200,    -1,    -1,    34,    88,    34,   302,   357,   197,   209,
     200,    -1,    -1,    34,    88,    34,    22,    34,   357,   198,
     209,   200,    -1,    -1,    34,    88,    34,   302,    22,    34,
     357,   199,   209,   200,    -1,   225,   240,   248,    -1,   202,
      -1,    -1,   202,   203,    -1,   203,    -1,    -1,    30,    88,
     204,   206,    -1,    -1,    30,    21,   303,    88,   205,   206,
      -1,   207,    96,    -1,    -1,   207,    97,   208,    -1,   208,
      -1,    34,    -1,   210,    -1,    -1,   210,   211,    -1,   211,
      -1,    -1,    30,    88,   212,   214,    -1,    -1,    30,    21,
     303,    88,   213,   214,    -1,   215,    96,    -1,    -1,   215,
      97,   216,    -1,   216,    -1,    34,    -1,   218,    -1,    -1,
     218,   219,    -1,   219,    -1,    -1,     7,    88,   220,   222,
      -1,    -1,     7,    21,   303,    88,   221,   222,    -1,   223,
      96,    -1,    -1,   223,    97,   224,    -1,   224,    -1,    34,
     100,   303,    -1,   226,    -1,    -1,   226,   227,    -1,   227,
      -1,    -1,     7,    88,   228,   230,    -1,    -1,     7,    21,
     303,    88,   229,   230,    -1,   231,    -1,    -1,   231,   232,
      -1,   232,    -1,    34,   100,   303,    96,    -1,   233,   234,
      -1,    -1,    -1,     9,    88,   235,   237,    -1,    -1,     9,
      21,   303,    88,   236,   237,    -1,   238,    96,    -1,    -1,
     238,    97,   239,    -1,   239,    -1,    34,   302,    -1,    34,
      -1,   240,   241,    -1,    -1,    -1,     9,    88,   242,   244,
      -1,    -1,     9,    21,   303,    88,   243,   244,    -1,   245,
      96,    -1,    -1,   245,    97,   246,    -1,   246,    -1,    34,
     302,    -1,    34,    -1,    12,    88,    35,    96,    -1,    -1,
      12,    88,    35,    96,    -1,    -1,   250,    -1,    -1,    -1,
      11,    15,    88,   251,   253,    -1,    -1,    11,    88,   252,
     253,    -1,   254,    -1,    -1,   254,   255,    -1,   255,    -1,
     256,    -1,   263,    -1,    -1,    18,   257,   258,    20,   262,
      28,   357,    -1,   259,    97,   258,    -1,   259,    -1,    34,
     100,   303,    19,   303,    -1,    21,   303,    -1,    -1,    12,
      35,    -1,    -1,   262,   263,    -1,   263,    -1,   264,    17,
     268,   260,   261,   358,    -1,   264,    17,   272,    17,   268,
     260,   261,   358,    -1,   264,    16,   268,   260,   261,   358,
      -1,   264,    16,   272,    16,   268,   260,   261,   358,    -1,
     265,   103,   266,    -1,   267,    -1,    34,   302,    -1,    34,
      -1,    34,   302,    -1,    34,    -1,    34,    69,    -1,    34,
     302,    -1,    34,    -1,    34,    69,    -1,   269,   103,   270,
      -1,   271,    -1,    34,   302,    -1,    34,    -1,    34,   302,
      -1,    34,    -1,    34,    69,    -1,    34,   302,    -1,    34,
      -1,    34,    69,    -1,    34,    -1,    31,   303,    -1,   272,
      31,   303,    -1,   274,    -1,    -1,    -1,    11,    15,    88,
     275,   277,    -1,    -1,    11,    88,   276,   277,    -1,   278,
      -1,    -1,   278,   279,    -1,   279,    -1,   280,    -1,   287,
      -1,    -1,    18,   281,   282,    20,   286,    28,    96,    -1,
     283,    97,   282,    -1,   283,    -1,    34,   100,   303,    19,
     303,    -1,    21,   303,    -1,    -1,    12,    35,    -1,    -1,
     286,   287,    -1,   287,    -1,   288,    17,   292,   284,   285,
      96,    -1,   288,    17,   296,    17,   292,   284,   285,    96,
      -1,   288,    16,   292,   284,   285,    96,    -1,   288,    16,
     296,    16,   292,   284,   285,    96,    -1,   289,   103,   290,
      -1,   291,    -1,    34,   302,    -1,    34,    -1,    34,   302,
      -1,    34,    -1,    34,    69,    -1,    34,   302,    -1,    34,
      -1,    34,    69,    -1,   293,   103,   294,    -1,   295,    -1,
      34,   302,    -1,    34,    -1,    34,   302,    -1,    34,    -1,
      34,    69,    -1,    34,   302,    -1,    34,    -1,    34,    69,
      -1,    34,    -1,    34,   303,    -1,   296,    34,   303,    -1,
     298,   201,   217,   299,    -1,    23,    34,    88,    34,   357,
      -1,    27,   357,    -1,   301,    98,   209,   225,    99,   357,
      -1,    23,    34,    88,    34,   357,    -1,   101,   303,   102,
      -1,   306,    -1,   304,    -1,   305,    -1,    39,   104,   306,
      97,   306,   105,    -1,    39,   104,   306,   105,    -1,    39,
     104,   105,    -1,    39,    -1,    40,   104,   313,    97,   313,
     105,    -1,    40,   104,   313,   105,    -1,   307,    -1,   104,
     306,   105,    -1,   306,    89,   306,    -1,   306,    90,   306,
      -1,   306,    91,   306,    -1,   306,    92,   306,    -1,   306,
      93,   306,    -1,   306,    94,   306,    -1,    90,   306,    -1,
     306,    70,   306,    -1,   306,    71,   306,    -1,   306,    72,
     306,    -1,   306,    73,   306,    -1,   306,    74,   306,    -1,
     306,    75,   306,    -1,   306,    76,   306,    -1,   306,    77,
     306,    -1,   306,    78,   306,    -1,    79,   306,    -1,   306,
      80,   306,    -1,   306,    81,   306,    -1,   306,    82,   306,
      -1,    83,   306,    -1,   306,    84,   306,    -1,   306,    85,
     306,    -1,   306,    87,   306,    88,   306,    -1,    34,   104,
     105,    -1,    34,   104,   306,   105,    -1,    34,   104,   306,
      97,   306,   105,    -1,    34,   104,   306,    97,   306,    97,
     306,   105,    -1,    34,   104,   306,    97,   306,    97,   306,
      97,   306,   105,    -1,   308,    -1,   309,    -1,   310,    -1,
     311,    -1,   312,    -1,    34,    -1,    41,    34,    -1,    41,
      42,    34,    -1,    42,    41,    34,    -1,    42,    34,    -1,
     313,    -1,    37,    -1,    38,    -1,   314,    -1,    68,    -1,
      68,   104,   105,    -1,    67,   104,    34,   105,    -1,    35,
      -1,    32,    -1,    33,    -1,   315,    -1,   315,    32,    34,
      -1,   315,    33,    34,    -1,    32,    34,    -1,    33,    34,
      -1,    49,    50,   357,    -1,    53,    34,    96,    -1,    52,
      34,    96,    -1,    52,    55,    34,    96,    -1,    51,    34,
      96,    -1,    54,    34,    96,    -1,    -1,    54,    34,    98,
     322,   324,    99,   357,    -1,    -1,    54,    34,    56,    34,
      98,   323,   324,    99,   357,    -1,   325,    -1,    -1,   325,
     326,    -1,   326,    -1,    34,    96,    -1,    34,   100,   356,
      96,    -1,    -1,    51,    34,    98,   328,   336,   342,    99,
     357,    -1,    -1,    51,    34,    56,    34,    98,   329,   336,
     342,    99,   357,    -1,    -1,    52,    34,    98,   331,   336,
     342,    99,   357,    -1,    -1,    52,    34,    56,    34,    98,
     332,   336,   342,    99,   357,    -1,    -1,    53,    34,    98,
     334,   336,   342,    99,   357,    -1,    -1,    53,    34,    56,
      34,    98,   335,   336,   342,    99,   357,    -1,    -1,    58,
      88,   337,   338,    -1,    -1,   339,    -1,    -1,   339,   340,
      -1,   340,    -1,    34,   100,   341,    96,    -1,    35,    -1,
      32,    -1,    33,    -1,   315,    -1,    37,    -1,    38,    -1,
      -1,    57,    88,   343,   344,    -1,    -1,   345,    -1,    -1,
     345,   346,    -1,   346,    -1,    -1,   349,   350,    34,   347,
     351,   352,   353,    96,    -1,    -1,   349,    34,   348,   351,
     352,   353,    96,    -1,    59,    -1,    60,    -1,    59,    60,
      -1,    60,    59,    -1,    -1,    34,    -1,    34,    91,    -1,
      61,    -1,    62,    -1,    63,    -1,    64,    -1,    66,    61,
      -1,    66,    62,    -1,    66,    63,    -1,    66,    64,    -1,
      65,    -1,    45,    -1,    46,    -1,   101,    32,   102,    -1,
     101,    34,   102,    -1,   101,   102,    -1,    -1,    54,   104,
      34,   105,    -1,    -1,   100,   354,    -1,    -1,   355,   354,
      -1,   355,    -1,    35,    -1,    36,    -1,    32,    -1,    33,
      -1,   315,    -1,    37,    -1,    38,    -1,    34,    -1,   104,
      -1,   105,    -1,   103,    -1,    97,    -1,    87,    -1,    88,
      -1,    76,    -1,    77,    -1,    78,    -1,    70,    -1,    71,
      -1,    72,    -1,    73,    -1,    74,    -1,    75,    -1,    80,
      -1,    81,    -1,    82,    -1,    84,    -1,    85,    -1,    89,
      -1,    90,    -1,    91,    -1,    92,    -1,    93,    -1,    94,
      -1,    32,    -1,    90,    32,    -1,    34,    -1,    96,    -1,
      -1,    97,    -1,    96,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   243,   243,   247,   248,   252,   254,   256,   258,   260,
     263,   265,   267,   269,   272,   273,   274,   275,   276,   278,
     280,   282,   284,   293,   292,   304,   305,   309,   321,   325,
     333,   335,   339,   345,   354,   358,   368,   377,   387,   388,
     392,   398,   410,   418,   428,   432,   442,   453,   466,   477,
     487,   491,   501,   515,   528,   529,   534,   533,   544,   545,
     549,   550,   554,   566,   567,   572,   571,   582,   583,   587,
     588,   592,   604,   605,   609,   619,   620,   624,   634,   635,
     640,   639,   650,   651,   655,   659,   669,   670,   675,   674,
     685,   686,   690,   694,   705,   709,   713,   717,   721,   725,
     729,   733,   737,   741,   745,   755,   756,   761,   760,   771,
     772,   776,   777,   778,   779,   783,   784,   788,   793,   801,
     802,   806,   811,   822,   823,   828,   827,   838,   839,   843,
     844,   845,   846,   850,   851,   855,   860,   868,   869,   873,
     878,   889,   890,   895,   894,   905,   906,   910,   911,   916,
     915,   925,   924,   935,   934,   944,   943,   956,   965,   966,
     971,   970,   981,   982,   986,   987,   992,   991,  1001,  1000,
    1011,  1010,  1020,  1019,  1032,  1041,  1042,  1046,  1047,  1052,
    1051,  1060,  1059,  1071,  1072,  1076,  1077,  1081,  1092,  1093,
    1097,  1098,  1103,  1102,  1111,  1110,  1122,  1123,  1127,  1128,
    1132,  1143,  1144,  1148,  1149,  1154,  1153,  1162,  1161,  1174,
    1175,  1179,  1180,  1184,  1196,  1197,  1201,  1202,  1207,  1206,
    1215,  1214,  1227,  1228,  1232,  1233,  1237,  1249,  1250,  1255,
    1254,  1263,  1262,  1274,  1275,  1279,  1280,  1284,  1291,  1302,
    1303,  1308,  1307,  1316,  1315,  1327,  1328,  1332,  1333,  1337,
    1344,  1355,  1359,  1366,  1370,  1377,  1378,  1383,  1382,  1392,
    1391,  1403,  1404,  1408,  1409,  1413,  1414,  1419,  1418,  1432,
    1433,  1437,  1447,  1451,  1455,  1459,  1463,  1464,  1468,  1473,
    1478,  1484,  1493,  1494,  1498,  1504,  1512,  1517,  1521,  1529,
    1536,  1542,  1552,  1553,  1557,  1562,  1569,  1574,  1578,  1586,
    1591,  1595,  1604,  1609,  1614,  1625,  1626,  1631,  1630,  1640,
    1639,  1651,  1652,  1656,  1657,  1661,  1662,  1667,  1666,  1680,
    1681,  1685,  1695,  1699,  1703,  1707,  1711,  1712,  1716,  1721,
    1726,  1732,  1741,  1742,  1746,  1752,  1760,  1765,  1769,  1777,
    1784,  1790,  1800,  1801,  1805,  1810,  1817,  1822,  1826,  1834,
    1839,  1843,  1852,  1857,  1862,  1873,  1880,  1889,  1900,  1911,
    1924,  1930,  1934,  1938,  1949,  1951,  1953,  1955,  1960,  1962,
    1967,  1968,  1971,  1973,  1975,  1977,  1979,  1981,  1984,  1988,
    1990,  1992,  1994,  1996,  1998,  2001,  2003,  2005,  2008,  2012,
    2014,  2016,  2019,  2022,  2024,  2026,  2029,  2031,  2033,  2035,
    2037,  2042,  2043,  2044,  2045,  2046,  2050,  2055,  2060,  2066,
    2072,  2080,  2084,  2086,  2091,  2095,  2097,  2099,  2104,  2109,
    2111,  2113,  2119,  2120,  2121,  2122,  2130,  2139,  2148,  2155,
    2165,  2174,  2184,  2183,  2195,  2194,  2209,  2210,  2214,  2215,
    2219,  2225,  2236,  2235,  2246,  2245,  2260,  2259,  2270,  2269,
    2284,  2283,  2294,  2293,  2308,  2307,  2313,  2317,  2318,  2322,
    2323,  2327,  2337,  2338,  2339,  2340,  2341,  2342,  2347,  2346,
    2352,  2356,  2357,  2361,  2362,  2367,  2366,  2379,  2378,  2392,
    2394,  2396,  2398,  2401,  2405,  2406,  2408,  2409,  2410,  2411,
    2413,  2414,  2415,  2416,  2418,  2419,  2420,  2425,  2430,  2435,
    2439,  2443,  2447,  2451,  2455,  2472,  2473,  2477,  2478,  2479,
    2480,  2481,  2482,  2483,  2484,  2485,  2485,  2485,  2485,  2486,
    2486,  2487,  2487,  2487,  2488,  2488,  2488,  2488,  2488,  2488,
    2489,  2489,  2489,  2490,  2490,  2491,  2491,  2491,  2491,  2491,
    2491,  2496,  2497,  2498,  2501,  2501,  2503,  2503
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
  "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "UMIN", "';'", "','", "'{'",
  "'}'", "'='", "'['", "']'", "'.'", "'('", "')'", "$accept",
  "networkdescription", "somedefinitions", "definition", "import", "@1",
  "filenames", "filename", "channeldefinition_old", "channelheader_old",
  "opt_channelattrblock_old", "channelattrblock_old", "endchannel_old",
  "channeldefinition", "channelheader", "opt_channelattrblock",
  "channelattrblock", "simpledefinition_old", "simpleheader_old",
  "endsimple_old", "simpledefinition", "simpleheader",
  "moduledefinition_old", "moduleheader_old", "endmodule_old",
  "moduledefinition", "moduleheader", "opt_machineblock_old",
  "machineblock_old", "@2", "opt_machinelist_old", "machinelist_old",
  "machine_old", "opt_machineblock", "machineblock", "@3",
  "opt_machinelist", "machinelist", "machine", "opt_displayblock_old",
  "displayblock_old", "opt_displayblock", "displayblock",
  "opt_paramblock_old", "paramblock_old", "@4", "opt_parameters_old",
  "parameters_old", "opt_paramblock", "paramblock", "@5", "opt_parameters",
  "parameters", "parameter_old", "parameter", "opt_gateblock_old",
  "gateblock_old", "@6", "opt_gates_old", "gates_old", "gatesI_old",
  "gateI_old", "gatesO_old", "gateO_old", "opt_gateblock", "gateblock",
  "@7", "opt_gates", "gates", "gatesI", "gateI", "gatesO", "gateO",
  "opt_submodblock_old", "submodblock_old", "@8", "opt_submodules_old",
  "submodules_old", "submodule_old", "@9", "@10", "@11", "@12",
  "submodule_body_old", "opt_submodblock", "submodblock", "@13",
  "opt_submodules", "submodules", "submodule", "@14", "@15", "@16", "@17",
  "submodule_body", "opt_on_blocks_old", "on_blocks_old", "on_block_old",
  "@18", "@19", "opt_on_list_old", "on_list_old", "on_mach_old",
  "opt_on_blocks", "on_blocks", "on_block", "@20", "@21", "opt_on_list",
  "on_list", "on_mach", "opt_substparamblocks_old", "substparamblocks_old",
  "substparamblock_old", "@22", "@23", "opt_substparameters_old",
  "substparameters_old", "substparameter_old", "opt_substparamblocks",
  "substparamblocks", "substparamblock", "@24", "@25",
  "opt_substparameters", "substparameters", "substparameter",
  "opt_gatesizeblocks_old", "gatesizeblock_old", "@26", "@27",
  "opt_gatesizes_old", "gatesizes_old", "gatesize_old",
  "opt_gatesizeblocks", "gatesizeblock", "@28", "@29", "opt_gatesizes",
  "gatesizes", "gatesize", "opt_submod_displayblock_old",
  "opt_submod_displayblock", "opt_connblock_old", "connblock_old", "@30",
  "@31", "opt_connections_old", "connections_old", "connection_old",
  "loopconnection_old", "@32", "loopvarlist_old", "loopvar_old",
  "opt_conncondition_old", "opt_conn_displaystr_old",
  "notloopconnections_old", "notloopconnection_old", "leftgatespec_old",
  "leftmod_old", "leftgate_old", "parentleftgate_old", "rightgatespec_old",
  "rightmod_old", "rightgate_old", "parentrightgate_old",
  "channeldescr_old", "opt_connblock", "connblock", "@33", "@34",
  "opt_connections", "connections", "connection", "loopconnection", "@35",
  "loopvarlist", "loopvar", "opt_conncondition", "opt_conn_displaystr",
  "notloopconnections", "notloopconnection", "leftgatespec", "leftmod",
  "leftgate", "parentleftgate", "rightgatespec", "rightmod", "rightgate",
  "parentrightgate", "channeldescr", "networkdefinition_old",
  "networkheader_old", "endnetwork_old", "networkdefinition",
  "networkheader", "vector", "expression", "inputvalue", "xmldocvalue",
  "expr", "simple_expr", "parameter_expr", "string_expr", "boolconst_expr",
  "numconst_expr", "special_expr", "stringconstant", "numconst",
  "timeconstant", "cplusplus", "struct_decl", "class_decl", "message_decl",
  "enum_decl", "enum", "@36", "@37", "opt_enumfields", "enumfields",
  "enumfield", "message", "@38", "@39", "class", "@40", "@41", "struct",
  "@42", "@43", "opt_propertiesblock", "@44", "opt_properties",
  "properties", "property", "propertyvalue", "opt_fieldsblock", "@45",
  "opt_fields", "fields", "field", "@46", "@47", "fieldmodifiers",
  "fielddatatype", "opt_fieldvector", "opt_fieldenum", "opt_fieldvalue",
  "fieldvalue", "fieldvalue_token", "enumvalue", "opt_semicolon",
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
      45,    42,    47,    37,    94,   342,    59,    44,   123,   125,
      61,    91,    93,    46,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short int yyr1[] =
{
       0,   106,   107,   108,   108,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   111,   110,   112,   112,   113,   114,   115,
     116,   116,   117,   117,   118,   118,   119,   120,   121,   121,
     122,   122,   123,   124,   125,   125,   126,   127,   128,   129,
     130,   130,   131,   132,   133,   133,   135,   134,   136,   136,
     137,   137,   138,   139,   139,   141,   140,   142,   142,   143,
     143,   144,   145,   145,   146,   147,   147,   148,   149,   149,
     151,   150,   152,   152,   153,   153,   154,   154,   156,   155,
     157,   157,   158,   158,   159,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   161,   161,   163,   162,   164,
     164,   165,   165,   165,   165,   166,   166,   167,   167,   168,
     168,   169,   169,   170,   170,   172,   171,   173,   173,   174,
     174,   174,   174,   175,   175,   176,   176,   177,   177,   178,
     178,   179,   179,   181,   180,   182,   182,   183,   183,   185,
     184,   186,   184,   187,   184,   188,   184,   189,   190,   190,
     192,   191,   193,   193,   194,   194,   196,   195,   197,   195,
     198,   195,   199,   195,   200,   201,   201,   202,   202,   204,
     203,   205,   203,   206,   206,   207,   207,   208,   209,   209,
     210,   210,   212,   211,   213,   211,   214,   214,   215,   215,
     216,   217,   217,   218,   218,   220,   219,   221,   219,   222,
     222,   223,   223,   224,   225,   225,   226,   226,   228,   227,
     229,   227,   230,   230,   231,   231,   232,   233,   233,   235,
     234,   236,   234,   237,   237,   238,   238,   239,   239,   240,
     240,   242,   241,   243,   241,   244,   244,   245,   245,   246,
     246,   247,   247,   248,   248,   249,   249,   251,   250,   252,
     250,   253,   253,   254,   254,   255,   255,   257,   256,   258,
     258,   259,   260,   260,   261,   261,   262,   262,   263,   263,
     263,   263,   264,   264,   265,   265,   266,   266,   266,   267,
     267,   267,   268,   268,   269,   269,   270,   270,   270,   271,
     271,   271,   272,   272,   272,   273,   273,   275,   274,   276,
     274,   277,   277,   278,   278,   279,   279,   281,   280,   282,
     282,   283,   284,   284,   285,   285,   286,   286,   287,   287,
     287,   287,   288,   288,   289,   289,   290,   290,   290,   291,
     291,   291,   292,   292,   293,   293,   294,   294,   294,   295,
     295,   295,   296,   296,   296,   297,   298,   299,   300,   301,
     302,   303,   303,   303,   304,   304,   304,   304,   305,   305,
     306,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   307,   307,   307,   307,   307,   308,   308,   308,   308,
     308,   309,   310,   310,   311,   312,   312,   312,   313,   314,
     314,   314,   315,   315,   315,   315,   316,   317,   318,   318,
     319,   320,   322,   321,   323,   321,   324,   324,   325,   325,
     326,   326,   328,   327,   329,   327,   331,   330,   332,   330,
     334,   333,   335,   333,   337,   336,   336,   338,   338,   339,
     339,   340,   341,   341,   341,   341,   341,   341,   343,   342,
     342,   344,   344,   345,   345,   347,   346,   348,   346,   349,
     349,   349,   349,   349,   350,   350,   350,   350,   350,   350,
     350,   350,   350,   350,   350,   350,   350,   351,   351,   351,
     351,   352,   352,   353,   353,   354,   354,   355,   355,   355,
     355,   355,   355,   355,   355,   355,   355,   355,   355,   355,
     355,   355,   355,   355,   355,   355,   355,   355,   355,   355,
     355,   355,   355,   355,   355,   355,   355,   355,   355,   355,
     355,   356,   356,   356,   357,   357,   358,   358
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     4,     3,     1,     1,     3,     2,
       0,     1,     4,     3,     3,     2,     5,     2,     1,     0,
       5,     4,     5,     2,     3,     2,     7,     2,     8,     2,
       3,     2,    10,     2,     1,     0,     0,     4,     2,     0,
       3,     1,     1,     1,     0,     0,     4,     2,     0,     3,
       1,     1,     1,     0,     4,     1,     0,     4,     1,     0,
       0,     4,     2,     0,     3,     1,     1,     0,     0,     4,
       2,     0,     3,     1,     1,     1,     3,     2,     3,     4,
       4,     3,     3,     3,     3,     1,     0,     0,     4,     1,
       0,     4,     3,     4,     3,     3,     1,     3,     1,     3,
       1,     3,     1,     1,     0,     0,     4,     1,     0,     4,
       3,     4,     3,     3,     1,     3,     1,     3,     1,     3,
       1,     1,     0,     0,     4,     1,     0,     2,     1,     0,
       7,     0,     8,     0,     9,     0,    10,     3,     1,     0,
       0,     4,     1,     0,     2,     1,     0,     7,     0,     8,
       0,     9,     0,    10,     3,     1,     0,     2,     1,     0,
       4,     0,     6,     2,     0,     3,     1,     1,     1,     0,
       2,     1,     0,     4,     0,     6,     2,     0,     3,     1,
       1,     1,     0,     2,     1,     0,     4,     0,     6,     2,
       0,     3,     1,     3,     1,     0,     2,     1,     0,     4,
       0,     6,     1,     0,     2,     1,     4,     2,     0,     0,
       4,     0,     6,     2,     0,     3,     1,     2,     1,     2,
       0,     0,     4,     0,     6,     2,     0,     3,     1,     2,
       1,     4,     0,     4,     0,     1,     0,     0,     5,     0,
       4,     1,     0,     2,     1,     1,     1,     0,     7,     3,
       1,     5,     2,     0,     2,     0,     2,     1,     6,     8,
       6,     8,     3,     1,     2,     1,     2,     1,     2,     2,
       1,     2,     3,     1,     2,     1,     2,     1,     2,     2,
       1,     2,     1,     2,     3,     1,     0,     0,     5,     0,
       4,     1,     0,     2,     1,     1,     1,     0,     7,     3,
       1,     5,     2,     0,     2,     0,     2,     1,     6,     8,
       6,     8,     3,     1,     2,     1,     2,     1,     2,     2,
       1,     2,     3,     1,     2,     1,     2,     1,     2,     2,
       1,     2,     1,     2,     3,     4,     5,     2,     6,     5,
       3,     1,     1,     1,     6,     4,     3,     1,     6,     4,
       1,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       3,     3,     2,     3,     3,     5,     3,     4,     6,     8,
      10,     1,     1,     1,     1,     1,     1,     2,     3,     3,
       2,     1,     1,     1,     1,     1,     3,     4,     1,     1,
       1,     1,     3,     3,     2,     2,     3,     3,     3,     4,
       3,     3,     0,     7,     0,     9,     1,     0,     2,     1,
       2,     4,     0,     8,     0,    10,     0,     8,     0,    10,
       0,     8,     0,    10,     0,     4,     0,     1,     0,     2,
       1,     4,     1,     1,     1,     1,     1,     1,     0,     4,
       0,     1,     0,     2,     1,     0,     8,     0,     7,     1,
       1,     2,     2,     0,     1,     2,     1,     1,     1,     1,
       2,     2,     2,     2,     1,     1,     1,     3,     3,     2,
       0,     4,     0,     2,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     0,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       4,     0,     2,     1,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     3,     5,    10,    30,     6,     0,
      11,    55,     7,     0,    12,    55,     8,     0,    13,   176,
       9,     0,    14,    15,    16,    17,    18,    19,    20,    21,
      22,     0,    43,    29,    49,     0,   545,     0,     0,     0,
       0,     0,     0,     0,    31,    39,     0,    79,    54,    64,
      79,    64,     0,   202,   175,   178,   189,    27,     0,    26,
       0,   544,   426,     0,   430,   442,     0,   428,   446,     0,
       0,   427,   450,     0,   431,   432,   419,   420,   406,   418,
     412,   413,   367,     0,     0,     0,     0,   415,     0,     0,
       0,     0,   545,   362,   363,   361,   370,   401,   402,   403,
     404,   405,   411,   414,   421,   545,    28,     0,     0,     0,
      38,    56,     0,   106,    78,     0,    87,    63,   106,    87,
       0,   179,     0,     0,   201,   204,   177,     0,   215,   188,
     191,    24,     0,   545,     0,   456,     0,   456,   429,     0,
     456,     0,   437,   424,   425,     0,     0,     0,   407,     0,
     410,     0,     0,     0,   388,   392,   378,     0,    33,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   545,    35,   545,     0,   545,     0,    59,    80,
       0,     0,   105,    65,     0,   124,    86,   142,   124,     0,
     184,     0,   205,   545,   355,   203,     0,   192,     0,     0,
     214,   217,   190,    25,   356,   444,     0,   470,   448,   470,
     452,   470,   434,     0,     0,   436,   439,   396,     0,   366,
       0,     0,   408,   409,     0,   416,   371,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   389,   390,   391,   393,
     394,     0,   372,   373,   374,   375,   376,   377,   422,   423,
      34,    32,     0,    36,     0,    62,    57,     0,    61,    83,
     107,   545,    42,    68,    88,     0,     0,   123,     0,   256,
     141,   159,   181,   187,   180,     0,   186,     0,   210,   357,
       0,   197,     0,   218,   545,   216,   456,   454,     0,     0,
     456,     0,   456,     0,   437,   440,     0,   545,   438,     0,
     397,     0,   365,     0,   369,   417,     0,    41,     0,    58,
       0,    95,     0,    81,     0,    85,    94,   110,   545,    45,
      71,    66,     0,    70,    91,   125,   545,   143,     0,    73,
     255,     0,   306,   158,   184,   183,     0,   207,     0,   206,
       0,   212,   194,   200,   193,     0,   199,     0,   223,   358,
     470,   458,   468,   545,   470,   545,   470,   545,     0,   541,
     543,     0,     0,   433,     0,     0,     0,   395,    40,    60,
       0,    97,    82,     0,     0,     0,   108,   109,    44,    67,
       0,    89,     0,    93,   128,    46,   146,     0,   259,     0,
       0,    72,   160,     0,    76,   305,   182,   185,   210,     0,
     209,     0,   197,   196,     0,   220,     0,   219,   222,   225,
       0,     0,   455,   457,   460,   483,   443,     0,   447,     0,
     451,   545,   542,   441,     0,   398,   364,   368,    98,    96,
     101,   102,   103,   104,    84,   118,     0,   116,   122,     0,
     120,     0,     0,    69,    90,     0,     0,   126,   127,     0,
     144,   145,   148,   257,   262,     0,   545,    48,   163,     0,
     309,     0,     0,    75,   208,   213,   211,   195,   198,   223,
       0,   224,   545,     0,   459,   479,   480,   469,   483,   474,
       0,   545,   545,   435,     0,    99,   100,     0,   112,     0,
       0,   114,     0,     0,     0,    92,   136,     0,   134,   140,
       0,   138,     0,     0,     0,   147,   262,   267,   290,   260,
     261,   264,   265,   266,     0,     0,   283,     0,   545,    51,
       0,   161,   162,   165,   307,   312,     0,   545,   221,     0,
     445,   463,   464,   462,   466,   467,   465,     0,   481,   482,
     473,   477,   495,   496,   486,   487,   488,   489,   494,     0,
       0,   449,   453,     0,   399,   117,   115,   121,   119,   111,
     113,     0,   130,     0,     0,   132,     0,     0,     0,   545,
     258,     0,   291,     0,   289,   263,     0,     0,     0,    74,
      50,     0,   164,   312,   317,   340,   310,   311,   314,   315,
     316,     0,     0,   333,     0,    52,   226,   461,   485,   500,
     490,   491,   492,   493,   475,     0,   135,   133,   139,   137,
     129,   131,     0,   545,   149,     0,     0,   270,     0,     0,
     300,   273,     0,   293,     0,   273,     0,   287,   282,   545,
     308,     0,   341,   339,   313,     0,     0,     0,    77,     0,
     502,   500,   400,   545,     0,   151,   176,     0,     0,     0,
     360,   303,   301,   299,     0,   275,     0,     0,     0,   275,
       0,   288,   286,     0,   545,   166,     0,     0,   320,   350,
     323,     0,   343,     0,   323,     0,   337,   332,     0,     0,
     499,     0,   504,   502,   153,   545,   176,   202,     0,     0,
     277,   269,   272,     0,     0,   297,   292,   300,   273,   304,
       0,   273,   545,     0,   168,   189,     0,     0,     0,   351,
     349,   353,     0,   325,     0,     0,     0,   325,     0,   338,
     336,   497,   498,     0,     0,     0,   504,   176,   155,   202,
     150,   228,     0,   545,   276,   274,   547,   546,   280,   298,
     296,   275,   278,   275,   170,   545,   189,   215,     0,     0,
     327,   319,   322,     0,     0,   347,   342,   350,   323,   354,
       0,   323,     0,   509,   510,   514,   507,   508,   512,   513,
     524,   525,   526,   527,   528,   529,   521,   522,   523,   530,
     531,   532,   533,   534,   519,   520,   535,   536,   537,   538,
     539,   540,   518,   517,   515,   516,   511,   503,   506,   478,
       0,   202,   176,   152,   252,   271,   268,     0,     0,   189,
     172,   215,   167,   240,     0,     0,   326,   324,   330,   348,
     346,   325,   328,   325,   501,   505,   476,   154,   202,     0,
       0,   227,   157,   281,   279,   215,   189,   169,   254,   321,
     318,     0,     0,   156,     0,   229,     0,   171,   215,     0,
       0,   239,   174,   331,   329,     0,   234,     0,   173,     0,
     241,     0,   231,   238,   230,     0,   236,   251,     0,   246,
       0,   234,   237,   233,     0,   243,   250,   242,     0,   248,
     253,   232,   235,   246,   249,   245,     0,   244,   247
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,     2,    14,    15,    41,    68,    69,    16,    17,
      53,    54,   116,    18,    19,   119,   120,    20,    21,   282,
      22,    23,    24,    25,   477,    26,    27,    57,    58,   198,
     276,   277,   278,   126,   127,   283,   341,   342,   343,   410,
     411,   482,   483,   123,   124,   279,   333,   334,   205,   206,
     344,   401,   402,   335,   336,   201,   202,   337,   396,   397,
     456,   457,   459,   460,   286,   287,   404,   467,   468,   517,
     518,   520,   521,   289,   290,   406,   470,   471,   472,   666,
     706,   747,   822,   750,   352,   353,   478,   541,   542,   543,
     725,   766,   829,   856,   832,    63,    64,    65,   210,   354,
     294,   295,   296,   138,   139,   140,   301,   422,   364,   365,
     366,   751,   134,   135,   298,   418,   359,   360,   361,   833,
     220,   221,   368,   489,   427,   428,   429,   824,   851,   876,
     891,   884,   885,   886,   858,   871,   889,   903,   897,   898,
     899,   852,   872,   349,   350,   526,   474,   529,   530,   531,
     532,   591,   636,   637,   675,   714,   709,   533,   534,   535,
     648,   536,   641,   642,   716,   643,   644,   414,   415,   603,
     545,   606,   607,   608,   609,   651,   687,   688,   733,   774,
     769,   610,   611,   612,   697,   613,   690,   691,   776,   692,
     693,    28,    29,   214,    30,    31,   673,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
      32,    33,    34,    35,    36,    37,   152,   314,   234,   235,
     236,    38,   145,   306,    39,   147,   310,    40,   150,   312,
     227,   371,   432,   433,   434,   557,   309,   435,   497,   498,
     499,   661,   619,   500,   570,   660,   702,   745,   817,   818,
     382,    72,   758
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -798
static const short int yypact[] =
{
    -798,    81,   187,  -798,  -798,    29,   127,   161,   170,   121,
     175,   142,   231,   265,  -798,  -798,  -798,   288,  -798,   181,
    -798,   293,  -798,   236,  -798,   293,  -798,   249,  -798,   313,
    -798,   254,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,   303,   261,   276,   284,   292,   270,   -36,   -23,   356,
     104,   133,   452,   372,   394,   396,   368,   456,  -798,   431,
     456,   431,    18,   468,   313,  -798,   449,  -798,   -45,  -798,
     461,  -798,  -798,   470,  -798,  -798,   471,  -798,  -798,   387,
     472,  -798,  -798,   473,  -798,  -798,   474,   475,   407,  -798,
    -798,  -798,   408,   409,   164,   227,   410,   411,   140,   140,
     140,   140,   270,  -798,  -798,   793,  -798,  -798,  -798,  -798,
    -798,  -798,  -798,  -798,   282,   -13,  -798,   452,   416,   418,
     476,  -798,   433,   515,  -798,   436,   518,  -798,   515,   518,
     452,  -798,    56,   499,   468,  -798,  -798,    63,   520,   449,
    -798,  -798,   303,   270,   430,   478,   432,   478,  -798,   435,
     478,   439,   495,  -798,  -798,   256,   316,   503,  -798,   500,
    -798,   505,   506,   438,  -798,  -798,  -798,   715,  -798,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     507,   510,   270,  -798,   270,   452,   270,   445,   512,  -798,
     459,   525,  -798,  -798,   463,   544,  -798,   543,   544,   466,
     521,   452,  -798,   270,  -798,  -798,   452,  -798,   134,   460,
     520,  -798,  -798,  -798,   462,  -798,   477,   501,  -798,   501,
    -798,   501,  -798,   147,   464,   495,  -798,  -798,   552,  -798,
     595,   130,  -798,  -798,   465,  -798,  -798,   342,   342,   342,
     342,   342,   342,   843,   843,   843,   278,   278,   278,   349,
     349,   504,   212,   212,   479,   479,   479,   479,  -798,  -798,
    -798,  -798,   491,  -798,   452,  -798,  -798,   220,  -798,    16,
    -798,    -6,  -798,   530,  -798,   480,   467,  -798,   481,   556,
    -798,   561,  -798,  -798,  -798,   229,  -798,   502,   565,  -798,
     513,   566,   452,  -798,   270,  -798,   478,  -798,   514,   508,
     478,   509,   478,   511,   495,  -798,   152,   270,  -798,   140,
    -798,   140,  -798,   503,  -798,  -798,   140,  -798,   516,  -798,
     512,   517,   569,  -798,   240,  -798,  -798,   328,   270,  -798,
    -798,  -798,   248,  -798,    16,  -798,   270,  -798,    -3,   594,
    -798,   523,   602,  -798,   521,  -798,   521,  -798,   531,  -798,
     259,  -798,  -798,  -798,  -798,   268,  -798,   526,   582,  -798,
     501,   583,  -798,   270,   501,   270,   501,   270,   519,  -798,
    -798,   588,   542,  -798,   631,   741,   535,   818,  -798,  -798,
     453,  -798,  -798,    16,   613,   614,  -798,   363,  -798,  -798,
     530,  -798,   554,  -798,   365,  -798,   617,   567,  -798,   568,
     628,  -798,  -798,     4,   642,  -798,  -798,  -798,   565,   452,
    -798,   565,   566,  -798,   566,  -798,   558,  -798,   582,  -798,
     560,   562,  -798,   583,  -798,   -19,  -798,   564,  -798,   575,
    -798,   270,  -798,  -798,   140,  -798,  -798,  -798,   616,   618,
    -798,  -798,  -798,  -798,  -798,   577,   289,  -798,   580,   291,
    -798,   613,   614,  -798,    16,   649,   656,  -798,   379,   603,
    -798,   617,  -798,  -798,    20,   658,    15,  -798,   660,   607,
    -798,   608,   598,  -798,  -798,  -798,  -798,  -798,  -798,   582,
     452,  -798,   270,   245,  -798,   638,   640,  -798,   106,  -798,
     267,   270,   270,  -798,   679,  -798,  -798,   612,  -798,   613,
     624,  -798,   614,   300,   306,  -798,   609,   308,  -798,   626,
     311,  -798,   649,   656,   685,  -798,    20,  -798,    98,  -798,
      20,  -798,  -798,  -798,   400,   627,  -798,   633,   270,  -798,
     643,  -798,   660,  -798,  -798,   178,   697,   270,  -798,   637,
    -798,   474,   475,  -798,  -798,  -798,   282,   639,  -798,  -798,
    -798,    22,  -798,  -798,  -798,  -798,  -798,  -798,  -798,   247,
     700,  -798,  -798,   140,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,   635,  -798,   649,   636,  -798,   656,   322,   332,    45,
    -798,   705,  -798,   452,   641,  -798,   190,   190,   706,  -798,
    -798,   707,  -798,   178,  -798,   114,  -798,   178,  -798,  -798,
    -798,   428,   645,  -798,   646,  -798,  -798,  -798,  -798,   644,
    -798,  -798,  -798,  -798,  -798,   767,  -798,  -798,  -798,  -798,
    -798,  -798,   709,     0,  -798,   665,   726,   661,   672,   452,
       1,   746,   674,  -798,    48,   746,   201,   -33,  -798,    52,
    -798,   744,  -798,   676,  -798,   747,   747,   748,  -798,    -5,
     721,   644,  -798,   270,   749,  -798,   313,   452,   760,   705,
    -798,  -798,  -798,   677,   452,   786,   769,   776,   452,   786,
     776,  -798,  -798,   790,     2,  -798,   701,   807,   732,   217,
     815,   750,  -798,    19,   815,    31,   -32,  -798,   753,   774,
    -798,   758,   779,   721,  -798,   270,   313,   468,   831,   235,
    -798,  -798,  -798,   846,   376,   110,  -798,   145,   746,  -798,
     376,   746,   270,   863,  -798,   449,   452,   867,   744,  -798,
     801,  -798,   452,   893,   872,   885,   452,   893,   885,  -798,
    -798,  -798,  -798,   886,   377,   825,   779,   313,  -798,   468,
    -798,  -798,   452,   270,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,   786,  -798,   786,  -798,   270,   449,   520,   903,   253,
    -798,  -798,  -798,   891,   833,   119,  -798,   159,   815,  -798,
     834,   815,   826,   474,   475,  -798,  -798,  -798,  -798,  -798,
    -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,  -798,  -798,  -798,  -798,  -798,   282,  -798,   377,  -798,
     842,   468,   313,  -798,   258,  -798,  -798,   376,   376,   449,
    -798,   520,  -798,  -798,   452,   844,  -798,  -798,  -798,  -798,
    -798,   893,  -798,   893,  -798,  -798,  -798,  -798,   468,   137,
     851,  -798,  -798,  -798,  -798,   520,   449,  -798,   283,  -798,
    -798,   845,   847,  -798,   452,  -798,   907,  -798,   520,   138,
     856,  -798,  -798,  -798,  -798,   857,   912,   852,  -798,   452,
    -798,   914,  -798,   849,  -798,   381,  -798,  -798,   859,   917,
     858,   912,  -798,  -798,   912,  -798,   849,  -798,   406,  -798,
    -798,  -798,  -798,   917,  -798,  -798,   917,  -798,  -798
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -798,  -798,  -798,  -798,  -798,  -798,  -798,   810,  -798,  -798,
    -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,  -798,  -798,  -798,  -798,  -798,  -798,   928,  -798,  -798,
    -798,  -798,   625,   895,  -798,  -798,  -798,  -798,   557,  -798,
    -798,  -798,  -798,   898,  -798,  -798,  -798,  -798,   830,  -798,
    -798,  -798,  -798,   570,  -328,   832,  -798,  -798,  -798,  -798,
     522,   455,   524,   450,   757,  -798,  -798,  -798,  -798,   444,
     378,   446,   382,  -798,  -798,  -798,  -798,  -798,   496,  -798,
    -798,  -798,  -798,  -739,  -798,  -798,  -798,  -798,  -798,   429,
    -798,  -798,  -798,  -798,  -797,  -659,  -798,   906,  -798,  -798,
     619,  -798,   620,  -694,  -798,   835,  -798,  -798,   550,  -798,
     551,   915,  -798,   848,  -798,  -798,   559,  -798,   563,   841,
    -798,   761,  -798,  -798,   498,  -798,   571,  -798,  -798,  -798,
    -798,    89,  -798,    91,  -798,  -798,  -798,  -798,    85,  -798,
      83,  -798,  -798,  -798,  -798,  -798,  -798,   469,  -798,   482,
    -798,  -798,   321,  -798,  -569,  -656,  -798,  -643,  -798,  -798,
    -798,  -798,  -591,  -798,  -798,  -798,   395,  -798,  -798,  -798,
    -798,   388,  -798,   386,  -798,  -798,   266,  -798,  -639,  -733,
    -798,  -712,  -798,  -798,  -798,  -798,  -595,  -798,  -798,  -798,
     340,  -798,  -798,  -798,  -798,  -798,  -502,  -117,  -798,  -798,
     -55,  -798,  -798,  -798,  -798,  -798,  -798,  -154,  -798,  -491,
    -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,   683,  -798,
     763,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,
    -142,  -798,  -798,  -798,   572,  -798,  -220,  -798,  -798,  -798,
     527,  -798,  -798,  -798,  -798,   339,   298,   257,   184,  -798,
    -798,  -101,  -690
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -485
static const short int yytable[] =
{
     194,   168,   556,   241,   780,   229,   645,   707,   231,   311,
     823,   313,   407,   209,   193,   770,   403,  -302,  -302,   479,
      73,   192,   664,   720,   723,   710,   594,   698,   338,   699,
     762,   767,  -302,    76,   857,   735,   681,   739,   527,   130,
     495,   496,   224,   164,   165,   166,   167,   749,   738,   538,
     331,   141,   142,   736,   528,   737,  -484,   836,   867,   332,
      74,   694,    75,    42,   677,   736,   754,   632,   593,   593,
     672,   878,   831,    77,   683,    78,   679,   211,   272,   678,
    -472,     3,   847,    71,   216,   408,   718,   633,   821,   721,
      71,   270,   480,   271,   297,   273,    71,   700,    71,   300,
     238,   240,   593,   653,  -295,   827,   131,   828,   861,   863,
     862,    71,   299,   618,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   855,   515,   853,   854,   841,
     778,    71,   843,   781,   212,   682,   593,   684,    71,   761,
     430,   217,   763,   593,   437,   302,   439,   328,   864,   879,
      80,    43,   868,   848,   370,   495,   496,   592,   374,   386,
     376,    46,    86,    87,    88,    89,    48,    90,    91,   759,
     339,    94,    95,   652,   379,   367,   380,   730,   839,    83,
       4,     5,     6,     7,   740,    44,   604,    49,   158,   593,
      81,  -285,    82,   369,    45,  -471,   159,    96,    97,    47,
       8,   593,   605,   760,   672,   593,   383,  -335,   680,    98,
     593,   639,   303,    99,   640,   865,   880,   323,   729,    84,
     100,    85,   678,  -352,  -352,   324,     9,   398,    10,    11,
      12,    13,   381,   315,   101,   405,   593,   316,  -295,    86,
      87,    88,    89,   816,    90,    91,    92,    93,    94,    95,
     593,   160,  -345,   753,   384,    50,   385,   849,   161,   528,
     850,   387,   436,   840,   438,   730,   440,   551,   552,    55,
     553,   835,   554,   555,    96,    97,   729,   605,    86,    87,
      88,    89,   869,    90,    91,   870,    98,    94,    95,    51,
      99,   561,   485,   186,   187,   188,   189,   100,   620,   621,
     622,   623,   562,   563,   190,   191,   329,   330,   593,    52,
    -345,   101,    56,    96,    97,   355,   356,   816,   564,   565,
     566,   567,   568,   569,    59,    98,   392,   393,    67,    99,
     503,   394,   395,    62,   399,   400,   100,    61,    86,    87,
      88,    89,    66,    90,    91,   420,   421,    94,    95,   -47,
     101,   237,   181,   182,   423,   424,    71,   184,   185,   186,
     187,   188,   189,   549,   -37,   539,   461,   462,   465,   466,
      70,   892,   -53,    96,    97,   508,   509,   511,   512,   504,
      79,   550,   522,   523,   904,    98,   579,   509,   115,    99,
     571,   572,   580,   512,   582,   583,   100,   585,   586,   783,
     784,   785,   786,   787,   788,   789,   596,   597,   630,   583,
     101,   239,   178,   179,   180,   117,   181,   182,   631,   586,
     118,   184,   185,   186,   187,   188,   189,   600,   184,   185,
     186,   187,   188,   189,   655,   656,   615,   790,   791,   792,
     793,   794,   795,   796,   797,   798,   121,   799,   800,   801,
     125,   802,   803,   122,   804,   805,   806,   807,   808,   809,
     810,   811,   756,   757,   812,   132,   638,   893,   894,   137,
     813,   814,   815,   148,    86,    87,    88,    89,   634,    90,
      91,    92,    93,    94,    95,   143,   448,   449,   450,   451,
     452,   453,   905,   906,   144,   146,   149,   151,   153,   154,
     197,   155,   156,   157,   162,   163,   195,   196,   625,    96,
      97,   199,   671,   200,   203,   204,   213,   218,   225,   233,
     228,    98,   665,   230,   242,    99,   226,   232,    89,   243,
     244,   268,   100,   245,   269,   274,   275,   280,   685,   281,
     708,   284,   285,   288,   292,   293,   101,   712,   308,   304,
    -359,   719,   704,   317,   340,   307,   346,   348,   345,   347,
     325,   351,   731,   189,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   724,   178,   179,   180,   327,   181,   182,
     357,   183,   326,   184,   185,   186,   187,   188,   189,   358,
     363,   362,   372,   391,   748,   390,   409,   373,   375,   768,
     377,   412,   388,   413,   425,   772,   426,   431,   441,   779,
     442,   764,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   419,   178,   179,   180,   825,   181,   182,   443,   183,
     447,   184,   185,   186,   187,   188,   189,   455,   458,   319,
     464,   469,   826,   476,   481,   473,   475,   320,   490,   492,
     505,   506,   493,   501,   830,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   502,   178,   179,   180,   507,   181,
     182,   510,   183,   516,   184,   185,   186,   187,   188,   189,
     519,   524,   321,   537,   540,   544,   546,   547,   558,   559,
     322,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     581,   178,   179,   180,   575,   181,   182,   859,   183,   589,
     184,   185,   186,   187,   188,   189,   577,   584,   444,   599,
     598,   601,   614,   616,   624,   617,   445,   626,   628,   635,
     647,   649,   658,   663,  -284,   659,   668,   875,   657,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   669,   178,
     179,   180,   888,   181,   182,   667,   183,   674,   184,   185,
     186,   187,   188,   189,   670,   701,   573,   676,   686,  -334,
    -294,   689,   696,   705,   574,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   528,   178,   179,   180,   713,   181,
     182,   726,   183,   715,   184,   185,   186,   187,   188,   189,
     717,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     246,   178,   179,   180,   722,   181,   182,   727,   183,   728,
     184,   185,   186,   187,   188,   189,   732,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   446,   178,   179,   180,
     752,   181,   182,   734,   183,   741,   184,   185,   186,   187,
     188,   189,   743,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   662,   178,   179,   180,   742,   181,   182,   744,
     183,   755,   184,   185,   186,   187,   188,   189,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   765,   178,   179,
     180,   605,   181,   182,  -344,   773,   775,   184,   185,   186,
     187,   188,   189,   169,   170,   171,   172,   173,   174,   777,
     782,   819,   834,   178,   179,   180,   837,   181,   182,   838,
     842,   844,   184,   185,   186,   187,   188,   189,   846,   866,
     860,   873,   877,   874,   881,   882,   883,   895,   887,   890,
     593,   896,   223,    60,   900,   389,   129,   463,   128,   208,
     207,   627,   578,   454,   576,   291,   587,   525,   629,   588,
     136,   602,   487,   416,   222,   488,   417,   484,   133,   219,
     901,   305,   215,   513,   486,   902,   514,   548,   907,   908,
     711,   650,   646,   654,   771,   590,   695,   378,   318,   491,
     703,   746,   845,   820,     0,   494,     0,     0,     0,     0,
       0,     0,   595,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   560
};

static const short int yycheck[] =
{
     117,   102,   493,   157,   737,   147,   597,   666,   150,   229,
     749,   231,    15,   130,   115,   727,   344,    16,    17,    15,
      56,    34,    22,   679,    22,   668,   528,    32,    34,    34,
     720,   725,    31,    56,   831,    16,    69,    69,    18,    21,
      59,    60,   143,    98,    99,   100,   101,   706,    17,    34,
      34,    96,    97,    34,    34,   694,    34,   769,   855,    43,
      96,   656,    98,    34,    16,    34,   709,    22,   101,   101,
      69,   868,   766,    96,    22,    98,   645,    21,   195,    31,
      99,     0,   821,    96,    21,    88,   677,   589,   747,   680,
      96,   192,    88,   194,   211,   196,    96,   102,    96,   216,
     155,   156,   101,   605,   103,   761,    88,   763,   841,   848,
     843,    96,   213,    91,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   829,   464,   827,   828,   778,
     735,    96,   781,   738,    88,   647,   101,   649,    96,   718,
     370,    88,   721,   101,   374,    21,   376,   274,    21,    21,
      56,    34,   856,   822,   306,    59,    60,    69,   310,   323,
     312,    50,    32,    33,    34,    35,    34,    37,    38,    69,
     281,    41,    42,    69,    32,   302,    34,   689,    69,    56,
       3,     4,     5,     6,   696,    34,    18,    55,    34,   101,
      96,   103,    98,   304,    34,    99,    42,    67,    68,    34,
      23,   101,    34,   715,    69,   101,   317,   103,    17,    79,
     101,    31,    88,    83,    34,    88,    88,    97,    69,    96,
      90,    98,    31,    16,    17,   105,    49,   338,    51,    52,
      53,    54,    90,    96,   104,   346,   101,   100,   103,    32,
      33,    34,    35,   744,    37,    38,    39,    40,    41,    42,
     101,    34,   103,    28,   319,    34,   321,     9,    41,    34,
      12,   326,   373,   775,   375,   777,   377,    32,    33,    98,
      35,    28,    37,    38,    67,    68,    69,    34,    32,    33,
      34,    35,     9,    37,    38,    12,    79,    41,    42,    34,
      83,    34,   419,    91,    92,    93,    94,    90,    61,    62,
      63,    64,    45,    46,    32,    33,    96,    97,   101,    31,
     103,   104,    29,    67,    68,    96,    97,   818,    61,    62,
      63,    64,    65,    66,    98,    79,    96,    97,    35,    83,
     441,    13,    14,    30,    96,    97,    90,    98,    32,    33,
      34,    35,    98,    37,    38,    96,    97,    41,    42,    98,
     104,   105,    84,    85,    96,    97,    96,    89,    90,    91,
      92,    93,    94,   490,    98,   476,    13,    14,    13,    14,
      88,   883,    98,    67,    68,    96,    97,    96,    97,   444,
      34,   492,    13,    14,   896,    79,    96,    97,    26,    83,
     501,   502,    96,    97,    96,    97,    90,    96,    97,    32,
      33,    34,    35,    36,    37,    38,    16,    17,    96,    97,
     104,   105,    80,    81,    82,    31,    84,    85,    96,    97,
      34,    89,    90,    91,    92,    93,    94,   538,    89,    90,
      91,    92,    93,    94,    16,    17,   547,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    88,    80,    81,    82,
      29,    84,    85,     7,    87,    88,    89,    90,    91,    92,
      93,    94,    96,    97,    97,     7,   593,    96,    97,    30,
     103,   104,   105,    96,    32,    33,    34,    35,   589,    37,
      38,    39,    40,    41,    42,    34,    43,    44,    45,    46,
      47,    48,    96,    97,    34,    34,    34,    34,    34,    34,
      34,   104,   104,   104,   104,   104,   100,    99,   573,    67,
      68,    88,   639,     8,    88,     7,    27,     7,    98,    34,
      98,    79,   633,    98,    34,    83,    58,    98,    35,    34,
      34,    34,    90,   105,    34,   100,    34,    88,   649,    24,
     667,    88,     8,    10,    88,    34,   104,   674,    57,    99,
      98,   678,   663,    99,    34,    88,    99,    11,    88,    88,
     105,    10,   689,    94,    70,    71,    72,    73,    74,    75,
      76,    77,    78,   684,    80,    81,    82,    96,    84,    85,
      88,    87,    88,    89,    90,    91,    92,    93,    94,    34,
      34,    88,    88,    34,   705,    88,    12,    99,    99,   726,
      99,    88,    96,    11,    88,   732,    34,    34,    99,   736,
      32,   722,    70,    71,    72,    73,    74,    75,    76,    77,
      78,   100,    80,    81,    82,   752,    84,    85,    96,    87,
     105,    89,    90,    91,    92,    93,    94,    34,    34,    97,
      96,    34,   753,    25,    12,    88,    88,   105,   100,    99,
      44,    43,   100,    99,   765,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    99,    80,    81,    82,   101,    84,
      85,   101,    87,    34,    89,    90,    91,    92,    93,    94,
      34,    88,    97,    35,    34,    88,    88,    99,    60,    59,
     105,    70,    71,    72,    73,    74,    75,    76,    77,    78,
     101,    80,    81,    82,   102,    84,    85,   834,    87,    34,
      89,    90,    91,    92,    93,    94,   102,   101,    97,    96,
     103,    88,    35,    96,    34,    96,   105,   102,   102,    34,
      34,    34,    96,    34,   103,   101,    20,   864,   103,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    97,    80,
      81,    82,   879,    84,    85,   100,    87,    21,    89,    90,
      91,    92,    93,    94,   102,    54,    97,   103,    34,   103,
     103,    34,    34,    34,   105,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    34,    80,    81,    82,    12,    84,
      85,   100,    87,    34,    89,    90,    91,    92,    93,    94,
      34,    70,    71,    72,    73,    74,    75,    76,    77,    78,
     105,    80,    81,    82,    34,    84,    85,    20,    87,    97,
      89,    90,    91,    92,    93,    94,    21,    70,    71,    72,
      73,    74,    75,    76,    77,    78,   105,    80,    81,    82,
      19,    84,    85,   103,    87,   102,    89,    90,    91,    92,
      93,    94,   104,    70,    71,    72,    73,    74,    75,    76,
      77,    78,   105,    80,    81,    82,   102,    84,    85,   100,
      87,    35,    89,    90,    91,    92,    93,    94,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    34,    80,    81,
      82,    34,    84,    85,   103,    12,    34,    89,    90,    91,
      92,    93,    94,    70,    71,    72,    73,    74,    75,    34,
      34,    96,    19,    80,    81,    82,    35,    84,    85,    96,
      96,   105,    89,    90,    91,    92,    93,    94,    96,    88,
      96,    96,    35,    96,    88,    88,    34,    88,    96,    35,
     101,    34,   142,    25,    96,   330,    61,   400,    60,   129,
     128,   583,   512,   393,   509,   208,   522,   471,   586,   523,
      64,   542,   422,   354,   139,   424,   356,   418,    63,   138,
     891,   220,   134,   461,   421,   894,   462,   489,   903,   906,
     669,   603,   597,   607,   728,   526,   656,   314,   235,   428,
     661,   703,   818,   746,    -1,   433,    -1,    -1,    -1,    -1,
      -1,    -1,   530,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   498
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short int yystos[] =
{
       0,   107,   108,     0,     3,     4,     5,     6,    23,    49,
      51,    52,    53,    54,   109,   110,   114,   115,   119,   120,
     123,   124,   126,   127,   128,   129,   131,   132,   297,   298,
     300,   301,   316,   317,   318,   319,   320,   321,   327,   330,
     333,   111,    34,    34,    34,    34,    50,    34,    34,    55,
      34,    34,    31,   116,   117,    98,    29,   133,   134,    98,
     133,    98,    30,   201,   202,   203,    98,    35,   112,   113,
      88,    96,   357,    56,    96,    98,    56,    96,    98,    34,
      56,    96,    98,    56,    96,    98,    32,    33,    34,    35,
      37,    38,    39,    40,    41,    42,    67,    68,    79,    83,
      90,   104,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,    26,   118,    31,    34,   121,
     122,    88,     7,   149,   150,    29,   139,   140,   149,   139,
      21,    88,     7,   217,   218,   219,   203,    30,   209,   210,
     211,    96,    97,    34,    34,   328,    34,   331,    96,    34,
     334,    34,   322,    34,    34,   104,   104,   104,    34,    42,
      34,    41,   104,   104,   306,   306,   306,   306,   357,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    80,    81,
      82,    84,    85,    87,    89,    90,    91,    92,    93,    94,
      32,    33,    34,   357,   303,   100,    99,    34,   135,    88,
       8,   161,   162,    88,     7,   154,   155,   161,   154,   303,
     204,    21,    88,    27,   299,   219,    21,    88,     7,   225,
     226,   227,   211,   113,   357,    98,    58,   336,    98,   336,
      98,   336,    98,    34,   324,   325,   326,   105,   306,   105,
     306,   313,    34,    34,    34,   105,   105,   306,   306,   306,
     306,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   306,   306,   306,   306,   306,    34,    34,
     357,   357,   303,   357,   100,    34,   136,   137,   138,   151,
      88,    24,   125,   141,    88,     8,   170,   171,    10,   179,
     180,   170,    88,    34,   206,   207,   208,   303,   220,   357,
     303,   212,    21,    88,    99,   227,   329,    88,    57,   342,
     332,   342,   335,   342,   323,    96,   100,    99,   326,    97,
     105,    97,   105,    97,   105,   105,    88,    96,   303,    96,
      97,    34,    43,   152,   153,   159,   160,   163,    34,   357,
      34,   142,   143,   144,   156,    88,    99,    88,    11,   249,
     250,    10,   190,   191,   205,    96,    97,    88,    34,   222,
     223,   224,    88,    34,   214,   215,   216,   303,   228,   357,
     336,   337,    88,    99,   336,    99,   336,    99,   324,    32,
      34,    90,   356,   357,   306,   306,   313,   306,    96,   138,
      88,    34,    96,    97,    13,    14,   164,   165,   357,    96,
      97,   157,   158,   160,   172,   357,   181,    15,    88,    12,
     145,   146,    88,    11,   273,   274,   206,   208,   221,   100,
      96,    97,   213,    96,    97,    88,    34,   230,   231,   232,
     342,    34,   338,   339,   340,   343,   357,   342,   357,   342,
     357,    99,    32,    96,    97,   105,   105,   105,    43,    44,
      45,    46,    47,    48,   159,    34,   166,   167,    34,   168,
     169,    13,    14,   144,    96,    13,    14,   173,   174,    34,
     182,   183,   184,    88,   252,    88,    25,   130,   192,    15,
      88,    12,   147,   148,   222,   303,   224,   214,   216,   229,
     100,   232,    99,   100,   340,    59,    60,   344,   345,   346,
     349,    99,    99,   357,   306,    44,    43,   101,    96,    97,
     101,    96,    97,   166,   168,   160,    34,   175,   176,    34,
     177,   178,    13,    14,    88,   184,   251,    18,    34,   253,
     254,   255,   256,   263,   264,   265,   267,    35,    34,   357,
      34,   193,   194,   195,    88,   276,    88,    99,   230,   303,
     357,    32,    33,    35,    37,    38,   315,   341,    60,    59,
     346,    34,    45,    46,    61,    62,    63,    64,    65,    66,
     350,   357,   357,    97,   105,   102,   167,   102,   169,    96,
      96,   101,    96,    97,   101,    96,    97,   175,   177,    34,
     253,   257,    69,   101,   302,   255,    16,    17,   103,    96,
     357,    88,   195,   275,    18,    34,   277,   278,   279,   280,
     287,   288,   289,   291,    35,   357,    96,    96,    91,   348,
      61,    62,    63,    64,    34,   306,   102,   176,   102,   178,
      96,    96,    22,   302,   357,    34,   258,   259,   303,    31,
      34,   268,   269,   271,   272,   268,   272,    34,   266,    34,
     277,   281,    69,   302,   279,    16,    17,   103,    96,   101,
     351,   347,   105,    34,    22,   357,   185,   100,    20,    97,
     102,   303,    69,   302,    21,   260,   103,    16,    31,   260,
      17,    69,   302,    22,   302,   357,    34,   282,   283,    34,
     292,   293,   295,   296,   292,   296,    34,   290,    32,    34,
     102,    54,   352,   351,   357,    34,   186,   201,   303,   262,
     263,   258,   303,    12,   261,    34,   270,    34,   268,   303,
     261,   268,    34,    22,   357,   196,   100,    20,    97,    69,
     302,   303,    21,   284,   103,    16,    34,   284,    17,    69,
     302,   102,   102,   104,   100,   353,   352,   187,   357,   201,
     189,   217,    19,    28,   263,    35,    96,    97,   358,    69,
     302,   260,   358,   260,   357,    34,   197,   209,   303,   286,
     287,   282,   303,    12,   285,    34,   294,    34,   292,   303,
     285,   292,    34,    32,    33,    34,    35,    36,    37,    38,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    80,
      81,    82,    84,    85,    87,    88,    89,    90,    91,    92,
      93,    94,    97,   103,   104,   105,   315,   354,   355,    96,
     353,   201,   188,   189,   233,   303,   357,   261,   261,   198,
     357,   209,   200,   225,    19,    28,   287,    35,    96,    69,
     302,   284,    96,   284,   105,   354,    96,   189,   201,     9,
      12,   234,   247,   358,   358,   209,   199,   200,   240,   303,
      96,   285,   285,   189,    21,    88,    88,   200,   209,     9,
      12,   241,   248,    96,    96,   303,   235,    35,   200,    21,
      88,    88,    88,    34,   237,   238,   239,    96,   303,   242,
      35,   236,   302,    96,    97,    88,    34,   244,   245,   246,
      96,   237,   239,   243,   302,    96,    97,   244,   246
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
#line 255 "ned.y"
    { if (ps.storeSourceCode) ps.channel->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 7:
#line 257 "ned.y"
    { if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 8:
#line 259 "ned.y"
    { if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 9:
#line 261 "ned.y"
    { if (ps.storeSourceCode) ps.network->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 10:
#line 264 "ned.y"
    { if (ps.storeSourceCode) ps.channel->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 11:
#line 266 "ned.y"
    { if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 12:
#line 268 "ned.y"
    { if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 13:
#line 270 "ned.y"
    { if (ps.storeSourceCode) ps.network->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 19:
#line 279 "ned.y"
    { if (ps.storeSourceCode) ps.enump->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 20:
#line 281 "ned.y"
    { if (ps.storeSourceCode) ps.messagep->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 21:
#line 283 "ned.y"
    { if (ps.storeSourceCode) ps.classp->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 22:
#line 285 "ned.y"
    { if (ps.storeSourceCode) ps.structp->setSourceCode(toString((yylsp[0]))); ;}
    break;

  case 23:
#line 293 "ned.y"
    {
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,(yylsp[0]));
                ;}
    break;

  case 24:
#line 298 "ned.y"
    {
                  // NOTE: no setTrailingComment(ps.imports,@3) -- comment already stored with last filename */
                ;}
    break;

  case 27:
#line 310 "ned.y"
    {
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes((yylsp[0]))));
                  setComments(ps.import,(yylsp[0]));
                ;}
    break;

  case 29:
#line 326 "ned.y"
    {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString((yylsp[0])));
                  setComments(ps.channel,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 32:
#line 340 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString((yylsp[-2])));
                  addExpression(ps.chanattr, "value",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.channel,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 33:
#line 346 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString((yylsp[-2])));
                  addExpression(ps.chanattr, "value",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.channel,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 34:
#line 355 "ned.y"
    {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                ;}
    break;

  case 35:
#line 359 "ned.y"
    {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                ;}
    break;

  case 36:
#line 371 "ned.y"
    {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                ;}
    break;

  case 37:
#line 378 "ned.y"
    {
                  use_chanattrname_token = 0;
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString((yylsp[0])));
                  setComments(ps.channel,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 40:
#line 393 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString((yylsp[-3])));
                  addExpression(ps.chanattr, "value",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.channel,(yylsp[-3]),(yylsp[-1]));
                ;}
    break;

  case 41:
#line 399 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString((yylsp[-3])));
                  addExpression(ps.chanattr, "value",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.channel,(yylsp[-3]),(yylsp[-1]));
                ;}
    break;

  case 43:
#line 419 "ned.y"
    {
                  use_chanattrname_token = 1;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 44:
#line 429 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 45:
#line 433 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 46:
#line 447 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 47:
#line 454 "ned.y"
    {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 49:
#line 478 "ned.y"
    {
                  use_chanattrname_token = 1;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 50:
#line 488 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 51:
#line 492 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 52:
#line 509 "ned.y"
    {
                  setTrailingComment(ps.module,(yylsp[-1]));
                ;}
    break;

  case 53:
#line 516 "ned.y"
    {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 56:
#line 534 "ned.y"
    {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 57:
#line 539 "ned.y"
    {
                ;}
    break;

  case 62:
#line 555 "ned.y"
    {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString((yylsp[0])));
                  setComments(ps.machine,(yylsp[0]),(yylsp[0]));
                ;}
    break;

  case 65:
#line 572 "ned.y"
    {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 66:
#line 577 "ned.y"
    {
                ;}
    break;

  case 71:
#line 593 "ned.y"
    {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString((yylsp[0])));
                  setComments(ps.machine,(yylsp[0]),(yylsp[0]));
                ;}
    break;

  case 74:
#line 610 "ned.y"
    {
                  addDisplayString(ps.module,(yylsp[-1]));
                ;}
    break;

  case 77:
#line 625 "ned.y"
    {
                  addDisplayString(ps.module,(yylsp[-1]));
                ;}
    break;

  case 80:
#line 640 "ned.y"
    {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 81:
#line 645 "ned.y"
    {
                ;}
    break;

  case 84:
#line 656 "ned.y"
    {
                  setComments(ps.param,(yylsp[0]));
                ;}
    break;

  case 85:
#line 660 "ned.y"
    {
                  setComments(ps.param,(yylsp[0]));
                ;}
    break;

  case 88:
#line 675 "ned.y"
    {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 89:
#line 680 "ned.y"
    {
                ;}
    break;

  case 92:
#line 691 "ned.y"
    {
                  setComments(ps.param,(yylsp[0]));
                ;}
    break;

  case 93:
#line 695 "ned.y"
    {
                  setComments(ps.param,(yylsp[0]));
                ;}
    break;

  case 95:
#line 710 "ned.y"
    {
                  ps.param = addParameter(ps.params,(yylsp[0]),TYPE_NUMERIC);
                ;}
    break;

  case 96:
#line 714 "ned.y"
    {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_NUMERIC);
                ;}
    break;

  case 97:
#line 718 "ned.y"
    {
                  ps.param = addParameter(ps.params,(yylsp[0]),TYPE_CONST_NUM);
                ;}
    break;

  case 98:
#line 722 "ned.y"
    {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_CONST_NUM);
                ;}
    break;

  case 99:
#line 726 "ned.y"
    {
                  ps.param = addParameter(ps.params,(yylsp[-3]),TYPE_CONST_NUM);
                ;}
    break;

  case 100:
#line 730 "ned.y"
    {
                  ps.param = addParameter(ps.params,(yylsp[-3]),TYPE_CONST_NUM);
                ;}
    break;

  case 101:
#line 734 "ned.y"
    {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_STRING);
                ;}
    break;

  case 102:
#line 738 "ned.y"
    {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_BOOL);
                ;}
    break;

  case 103:
#line 742 "ned.y"
    {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_XML);
                ;}
    break;

  case 104:
#line 746 "ned.y"
    {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_ANYTYPE);
                ;}
    break;

  case 107:
#line 761 "ned.y"
    {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 108:
#line 766 "ned.y"
    {
                ;}
    break;

  case 117:
#line 789 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[-2]), 1, 1 );
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 118:
#line 794 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[0]), 1, 0 );
                  setComments(ps.gate,(yylsp[0]));
                ;}
    break;

  case 121:
#line 807 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[-2]), 0, 1 );
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 122:
#line 812 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[0]), 0, 0 );
                  setComments(ps.gate,(yylsp[0]));
                ;}
    break;

  case 125:
#line 828 "ned.y"
    {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 126:
#line 833 "ned.y"
    {
                ;}
    break;

  case 135:
#line 856 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[-2]), 1, 1 );
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 136:
#line 861 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[0]), 1, 0 );
                  setComments(ps.gate,(yylsp[0]));
                ;}
    break;

  case 139:
#line 874 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[-2]), 0, 1 );
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 140:
#line 879 "ned.y"
    {
                  ps.gate = addGate(ps.gates, (yylsp[0]), 0, 0 );
                  setComments(ps.gate,(yylsp[0]));
                ;}
    break;

  case 143:
#line 895 "ned.y"
    {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 144:
#line 900 "ned.y"
    {
                ;}
    break;

  case 149:
#line 916 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-3]), (yylsp[-1]), NULLPOS);
                  setComments(ps.submod,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 150:
#line 922 "ned.y"
    {
                ;}
    break;

  case 151:
#line 925 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-4]), (yylsp[-2]), NULLPOS);
                  addVector(ps.submod, "vector-size",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.submod,(yylsp[-4]),(yylsp[0]));
                ;}
    break;

  case 152:
#line 932 "ned.y"
    {
                ;}
    break;

  case 153:
#line 935 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-5]), (yylsp[-1]), (yylsp[-3]));
                  setComments(ps.submod,(yylsp[-5]),(yylsp[0]));
                ;}
    break;

  case 154:
#line 941 "ned.y"
    {
                ;}
    break;

  case 155:
#line 944 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-6]), (yylsp[-1]), (yylsp[-4]));
                  addVector(ps.submod, "vector-size",(yylsp[-3]),(yyvsp[-3]));
                  setComments(ps.submod,(yylsp[-6]),(yylsp[0]));
                ;}
    break;

  case 156:
#line 951 "ned.y"
    {
                ;}
    break;

  case 160:
#line 971 "ned.y"
    {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 161:
#line 976 "ned.y"
    {
                ;}
    break;

  case 166:
#line 992 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-3]), (yylsp[-1]), NULLPOS);
                  setComments(ps.submod,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 167:
#line 998 "ned.y"
    {
                ;}
    break;

  case 168:
#line 1001 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-4]), (yylsp[-2]), NULLPOS);
                  addVector(ps.submod, "vector-size",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.submod,(yylsp[-4]),(yylsp[0]));
                ;}
    break;

  case 169:
#line 1008 "ned.y"
    {
                ;}
    break;

  case 170:
#line 1011 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-5]), (yylsp[-1]), (yylsp[-3]));
                  setComments(ps.submod,(yylsp[-5]),(yylsp[0]));
                ;}
    break;

  case 171:
#line 1017 "ned.y"
    {
                ;}
    break;

  case 172:
#line 1020 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-6]), (yylsp[-1]), (yylsp[-4]));
                  addVector(ps.submod, "vector-size",(yylsp[-3]),(yyvsp[-3]));
                  setComments(ps.submod,(yylsp[-6]),(yylsp[0]));
                ;}
    break;

  case 173:
#line 1027 "ned.y"
    {
                ;}
    break;

  case 179:
#line 1052 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 180:
#line 1057 "ned.y"
    {
                ;}
    break;

  case 181:
#line 1060 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.substmachines,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 182:
#line 1066 "ned.y"
    {
                ;}
    break;

  case 187:
#line 1082 "ned.y"
    {
                  ps.substmachine = addSubstmachine(ps.substmachines,(yylsp[0]));
                  setComments(ps.substmachine,(yylsp[0]));
                ;}
    break;

  case 192:
#line 1103 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 193:
#line 1108 "ned.y"
    {
                ;}
    break;

  case 194:
#line 1111 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.substmachines,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 195:
#line 1117 "ned.y"
    {
                ;}
    break;

  case 200:
#line 1133 "ned.y"
    {
                  ps.substmachine = addSubstmachine(ps.substmachines,(yylsp[0]));
                  setComments(ps.substmachine,(yylsp[0]));
                ;}
    break;

  case 205:
#line 1154 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 206:
#line 1159 "ned.y"
    {
                ;}
    break;

  case 207:
#line 1162 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.substparams,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 208:
#line 1168 "ned.y"
    {
                ;}
    break;

  case 213:
#line 1185 "ned.y"
    {
                  ps.substparam = addSubstparam(ps.substparams,(yylsp[-2]));
                  addExpression(ps.substparam, "value",(yylsp[0]),(yyvsp[0]));
                  setComments(ps.substparam,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 218:
#line 1207 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 219:
#line 1212 "ned.y"
    {
                ;}
    break;

  case 220:
#line 1215 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.substparams,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 221:
#line 1221 "ned.y"
    {
                ;}
    break;

  case 226:
#line 1238 "ned.y"
    {
                  ps.substparam = addSubstparam(ps.substparams,(yylsp[-3]));
                  addExpression(ps.substparam, "value",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.substparam,(yylsp[-3]),(yylsp[-1]));
                ;}
    break;

  case 229:
#line 1255 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 230:
#line 1260 "ned.y"
    {
                ;}
    break;

  case 231:
#line 1263 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.gatesizes,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 232:
#line 1269 "ned.y"
    {
                ;}
    break;

  case 237:
#line 1285 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,(yylsp[-1]));
                  addVector(ps.gatesize, "vector-size",(yylsp[0]),(yyvsp[0]));

                  setComments(ps.gatesize,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 238:
#line 1292 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,(yylsp[0]));
                  setComments(ps.gatesize,(yylsp[0]));
                ;}
    break;

  case 241:
#line 1308 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 242:
#line 1313 "ned.y"
    {
                ;}
    break;

  case 243:
#line 1316 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",(yylsp[-1]),(yyvsp[-1]));
                  setComments(ps.gatesizes,(yylsp[-3]),(yylsp[0]));
                ;}
    break;

  case 244:
#line 1322 "ned.y"
    {
                ;}
    break;

  case 249:
#line 1338 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,(yylsp[-1]));
                  addVector(ps.gatesize, "vector-size",(yylsp[0]),(yyvsp[0]));

                  setComments(ps.gatesize,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 250:
#line 1345 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,(yylsp[0]));
                  setComments(ps.gatesize,(yylsp[0]));
                ;}
    break;

  case 251:
#line 1356 "ned.y"
    {
                  addDisplayString(ps.submod,(yylsp[-1]));
                ;}
    break;

  case 253:
#line 1367 "ned.y"
    {
                  addDisplayString(ps.submod,(yylsp[-1]));
                ;}
    break;

  case 257:
#line 1383 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 258:
#line 1389 "ned.y"
    {
                ;}
    break;

  case 259:
#line 1392 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 260:
#line 1398 "ned.y"
    {
                ;}
    break;

  case 267:
#line 1419 "ned.y"
    {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;}
    break;

  case 268:
#line 1424 "ned.y"
    {
                  ps.inLoop=0;
                  setComments(ps.forloop,(yylsp[-6]),(yylsp[-3]));
                  setTrailingComment(ps.forloop,(yylsp[-1]));
                ;}
    break;

  case 271:
#line 1438 "ned.y"
    {
                  ps.loopvar = addLoopVar(ps.forloop,(yylsp[-4]));
                  addExpression(ps.loopvar, "from-value",(yylsp[-2]),(yyvsp[-2]));
                  addExpression(ps.loopvar, "to-value",(yylsp[0]),(yyvsp[0]));
                  setComments(ps.loopvar,(yylsp[-4]),(yylsp[0]));
                ;}
    break;

  case 272:
#line 1448 "ned.y"
    {
                  addExpression(ps.conn, "condition",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 274:
#line 1456 "ned.y"
    {
                  addDisplayString(ps.conn,(yylsp[0]));
                ;}
    break;

  case 278:
#line 1469 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                ;}
    break;

  case 279:
#line 1474 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                ;}
    break;

  case 280:
#line 1479 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                ;}
    break;

  case 281:
#line 1485 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                ;}
    break;

  case 284:
#line 1499 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "src-module-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 285:
#line 1505 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[0])) );
                ;}
    break;

  case 286:
#line 1513 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 287:
#line 1518 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[0])) );
                ;}
    break;

  case 288:
#line 1522 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  ps.conn->setSrcGatePlusplus(true);
                ;}
    break;

  case 289:
#line 1530 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 290:
#line 1537 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[0])));
                ;}
    break;

  case 291:
#line 1543 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  ps.conn->setSrcGatePlusplus(true);
                ;}
    break;

  case 294:
#line 1558 "ned.y"
    {
                  ps.conn->setDestModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "dest-module-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 295:
#line 1563 "ned.y"
    {
                  ps.conn->setDestModule( toString((yylsp[0])) );
                ;}
    break;

  case 296:
#line 1570 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 297:
#line 1575 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                ;}
    break;

  case 298:
#line 1579 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                ;}
    break;

  case 299:
#line 1587 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 300:
#line 1592 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                ;}
    break;

  case 301:
#line 1596 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                ;}
    break;

  case 302:
#line 1605 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",(yylsp[0]),createExpression(createConst(NED_CONST_STRING, toString((yylsp[0])))));
                ;}
    break;

  case 303:
#line 1610 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString((yylsp[-1])));
                  addExpression(ps.connattr, "value",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 304:
#line 1615 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString((yylsp[-1])));
                  addExpression(ps.connattr, "value",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 307:
#line 1631 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,(yylsp[-2]),(yylsp[0]));
                ;}
    break;

  case 308:
#line 1637 "ned.y"
    {
                ;}
    break;

  case 309:
#line 1640 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,(yylsp[-1]),(yylsp[0]));
                ;}
    break;

  case 310:
#line 1646 "ned.y"
    {
                ;}
    break;

  case 317:
#line 1667 "ned.y"
    {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;}
    break;

  case 318:
#line 1672 "ned.y"
    {
                  ps.inLoop=0;
                  setComments(ps.forloop,(yylsp[-6]),(yylsp[-3]));
                  setTrailingComment(ps.forloop,(yylsp[-1]));
                ;}
    break;

  case 321:
#line 1686 "ned.y"
    {
                  ps.loopvar = addLoopVar(ps.forloop,(yylsp[-4]));
                  addExpression(ps.loopvar, "from-value",(yylsp[-2]),(yyvsp[-2]));
                  addExpression(ps.loopvar, "to-value",(yylsp[0]),(yyvsp[0]));
                  setComments(ps.loopvar,(yylsp[-4]),(yylsp[0]));
                ;}
    break;

  case 322:
#line 1696 "ned.y"
    {
                  addExpression(ps.conn, "condition",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 324:
#line 1704 "ned.y"
    {
                  addDisplayString(ps.conn,(yylsp[0]));
                ;}
    break;

  case 328:
#line 1717 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                ;}
    break;

  case 329:
#line 1722 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                ;}
    break;

  case 330:
#line 1727 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                ;}
    break;

  case 331:
#line 1733 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                ;}
    break;

  case 334:
#line 1747 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "src-module-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 335:
#line 1753 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[0])) );
                ;}
    break;

  case 336:
#line 1761 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 337:
#line 1766 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[0])) );
                ;}
    break;

  case 338:
#line 1770 "ned.y"
    {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  ps.conn->setSrcGatePlusplus(true);
                ;}
    break;

  case 339:
#line 1778 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 340:
#line 1785 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[0])));
                ;}
    break;

  case 341:
#line 1791 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  ps.conn->setSrcGatePlusplus(true);
                ;}
    break;

  case 344:
#line 1806 "ned.y"
    {
                  ps.conn->setDestModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "dest-module-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 345:
#line 1811 "ned.y"
    {
                  ps.conn->setDestModule( toString((yylsp[0])) );
                ;}
    break;

  case 346:
#line 1818 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 347:
#line 1823 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                ;}
    break;

  case 348:
#line 1827 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                ;}
    break;

  case 349:
#line 1835 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 350:
#line 1840 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                ;}
    break;

  case 351:
#line 1844 "ned.y"
    {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                ;}
    break;

  case 352:
#line 1853 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",(yylsp[0]),createExpression(createConst(NED_CONST_STRING, toString((yylsp[0])))));
                ;}
    break;

  case 353:
#line 1858 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString((yylsp[-1])));
                  addExpression(ps.connattr, "value",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 354:
#line 1863 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString((yylsp[-1])));
                  addExpression(ps.connattr, "value",(yylsp[0]),(yyvsp[0]));
                ;}
    break;

  case 356:
#line 1881 "ned.y"
    {
                  ps.network = addNetwork(ps.nedfile,(yylsp[-3]),(yylsp[-1]));
                  setComments(ps.network,(yylsp[-4]),(yylsp[0]));
                  ps.inNetwork=1;
                ;}
    break;

  case 357:
#line 1890 "ned.y"
    {
                  setTrailingComment(ps.network,(yylsp[-1]));
                  ps.inNetwork=0;
                ;}
    break;

  case 358:
#line 1904 "ned.y"
    {
                  setTrailingComment(ps.network,(yylsp[-1]));
                  ps.inNetwork=0;
                ;}
    break;

  case 359:
#line 1912 "ned.y"
    {
                  ps.network = addNetwork(ps.nedfile,(yylsp[-3]),(yylsp[-1]));
                  setComments(ps.network,(yylsp[-4]),(yylsp[0]));
                  ps.inNetwork=1;
                ;}
    break;

  case 360:
#line 1925 "ned.y"
    { (yyval) = (yyvsp[-1]); ;}
    break;

  case 361:
#line 1931 "ned.y"
    {
                  if (ps.parseExpressions) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 362:
#line 1935 "ned.y"
    {
                  if (ps.parseExpressions) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 363:
#line 1939 "ned.y"
    {
                  if (ps.parseExpressions) (yyval) = createExpression((yyvsp[0]));
                ;}
    break;

  case 364:
#line 1950 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("input", (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 365:
#line 1952 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("input", (yyvsp[-1])); ;}
    break;

  case 366:
#line 1954 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("input"); ;}
    break;

  case 367:
#line 1956 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("input"); ;}
    break;

  case 368:
#line 1961 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("xmldoc", (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 369:
#line 1963 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("xmldoc", (yyvsp[-1])); ;}
    break;

  case 371:
#line 1969 "ned.y"
    { (yyval) = (yyvsp[-1]); ;}
    break;

  case 372:
#line 1972 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("+", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 373:
#line 1974 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("-", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 374:
#line 1976 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("*", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 375:
#line 1978 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("/", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 376:
#line 1980 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("%", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 377:
#line 1982 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("^", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 378:
#line 1986 "ned.y"
    { if (ps.parseExpressions) (yyval) = unaryMinus((yyvsp[0])); ;}
    break;

  case 379:
#line 1989 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("==", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 380:
#line 1991 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("!=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 381:
#line 1993 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator(">", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 382:
#line 1995 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator(">=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 383:
#line 1997 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("<", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 384:
#line 1999 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("<=", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 385:
#line 2002 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("&&", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 386:
#line 2004 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("||", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 387:
#line 2006 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("##", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 388:
#line 2010 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("!", (yyvsp[0])); ;}
    break;

  case 389:
#line 2013 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("&", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 390:
#line 2015 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("|", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 391:
#line 2017 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("#", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 392:
#line 2021 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("~", (yyvsp[0])); ;}
    break;

  case 393:
#line 2023 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("<<", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 394:
#line 2025 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator(">>", (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 395:
#line 2027 "ned.y"
    { if (ps.parseExpressions) (yyval) = createOperator("?:", (yyvsp[-4]), (yyvsp[-2]), (yyvsp[0])); ;}
    break;

  case 396:
#line 2030 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction(toString((yylsp[-2]))); ;}
    break;

  case 397:
#line 2032 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction(toString((yylsp[-3])), (yyvsp[-1])); ;}
    break;

  case 398:
#line 2034 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction(toString((yylsp[-5])), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 399:
#line 2036 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction(toString((yylsp[-7])), (yyvsp[-5]), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 400:
#line 2038 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction(toString((yylsp[-9])), (yyvsp[-7]), (yyvsp[-5]), (yyvsp[-3]), (yyvsp[-1])); ;}
    break;

  case 406:
#line 2051 "ned.y"
    {
                  // if there's no modifier, might be a loop variable too
                  (yyval) = createParamRefOrIdent(toString((yylsp[0])));
                ;}
    break;

  case 407:
#line 2056 "ned.y"
    {
                  (yyval) = createParamRef(toString((yylsp[0])));
                  ((ParamRefNode *)(yyval))->setIsRef(true);
                ;}
    break;

  case 408:
#line 2061 "ned.y"
    {
                  (yyval) = createParamRef(toString((yylsp[0])));
                  ((ParamRefNode *)(yyval))->setIsRef(true);
                  ((ParamRefNode *)(yyval))->setIsAncestor(true);
                ;}
    break;

  case 409:
#line 2067 "ned.y"
    {
                  (yyval) = createParamRef(toString((yylsp[0])));
                  ((ParamRefNode *)(yyval))->setIsRef(true);
                  ((ParamRefNode *)(yyval))->setIsAncestor(true);
                ;}
    break;

  case 410:
#line 2073 "ned.y"
    {
                  (yyval) = createParamRef(toString((yylsp[0])));
                  ((ParamRefNode *)(yyval))->setIsAncestor(true);
                ;}
    break;

  case 412:
#line 2085 "ned.y"
    { (yyval) = createConst(NED_CONST_BOOL, "true"); ;}
    break;

  case 413:
#line 2087 "ned.y"
    { (yyval) = createConst(NED_CONST_BOOL, "false"); ;}
    break;

  case 415:
#line 2096 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("index"); ;}
    break;

  case 416:
#line 2098 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("index"); ;}
    break;

  case 417:
#line 2100 "ned.y"
    { if (ps.parseExpressions) (yyval) = createFunction("sizeof", createIdent(toString((yylsp[-1])))); ;}
    break;

  case 418:
#line 2105 "ned.y"
    { (yyval) = createConst(NED_CONST_STRING, toString(trimQuotes((yylsp[0])))); ;}
    break;

  case 419:
#line 2110 "ned.y"
    { (yyval) = createConst(NED_CONST_INT, toString((yylsp[0]))); ;}
    break;

  case 420:
#line 2112 "ned.y"
    { (yyval) = createConst(NED_CONST_REAL, toString((yylsp[0]))); ;}
    break;

  case 421:
#line 2114 "ned.y"
    { (yyval) = createTimeConst(toString((yylsp[0]))); ;}
    break;

  case 426:
#line 2131 "ned.y"
    {
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces((yylsp[-1]))));
                  setComments(ps.cplusplus,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 427:
#line 2140 "ned.y"
    {
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString((yylsp[-1])));
                  setComments(ps.structdecl,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 428:
#line 2149 "ned.y"
    {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString((yylsp[-1])));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 429:
#line 2156 "ned.y"
    {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString((yylsp[-1])));
                  setComments(ps.classdecl,(yylsp[-3]),(yylsp[-2]));
                ;}
    break;

  case 430:
#line 2166 "ned.y"
    {
                  ps.messagedecl = (MessageDeclNode *)createNodeWithTag(NED_MESSAGE_DECL, ps.nedfile );
                  ps.messagedecl->setName(toString((yylsp[-1])));
                  setComments(ps.messagedecl,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 431:
#line 2175 "ned.y"
    {
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString((yylsp[-1])));
                  setComments(ps.enumdecl,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 432:
#line 2184 "ned.y"
    {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString((yylsp[-1])));
                  setComments(ps.enump,(yylsp[-2]),(yylsp[-1]));
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;}
    break;

  case 433:
#line 2191 "ned.y"
    {
                  setTrailingComment(ps.enump,(yylsp[-1]));
                ;}
    break;

  case 434:
#line 2195 "ned.y"
    {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString((yylsp[-3])));
                  ps.enump->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.enump,(yylsp[-4]),(yylsp[-1]));
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;}
    break;

  case 435:
#line 2203 "ned.y"
    {
                  setTrailingComment(ps.enump,(yylsp[-1]));
                ;}
    break;

  case 440:
#line 2220 "ned.y"
    {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString((yylsp[-1])));
                  setComments(ps.enumfield,(yylsp[-1]),(yylsp[-1]));
                ;}
    break;

  case 441:
#line 2226 "ned.y"
    {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString((yylsp[-3])));
                  ps.enumfield->setValue(toString((yylsp[-1])));
                  setComments(ps.enumfield,(yylsp[-3]),(yylsp[-1]));
                ;}
    break;

  case 442:
#line 2236 "ned.y"
    {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString((yylsp[-1])));
                  setComments(ps.messagep,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 443:
#line 2242 "ned.y"
    {
                  setTrailingComment(ps.messagep,(yylsp[-1]));
                ;}
    break;

  case 444:
#line 2246 "ned.y"
    {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString((yylsp[-3])));
                  ps.messagep->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.messagep,(yylsp[-4]),(yylsp[-1]));
                ;}
    break;

  case 445:
#line 2253 "ned.y"
    {
                  setTrailingComment(ps.messagep,(yylsp[-1]));
                ;}
    break;

  case 446:
#line 2260 "ned.y"
    {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString((yylsp[-1])));
                  setComments(ps.classp,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 447:
#line 2266 "ned.y"
    {
                  setTrailingComment(ps.classp,(yylsp[-1]));
                ;}
    break;

  case 448:
#line 2270 "ned.y"
    {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString((yylsp[-3])));
                  ps.classp->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.classp,(yylsp[-4]),(yylsp[-1]));
                ;}
    break;

  case 449:
#line 2277 "ned.y"
    {
                  setTrailingComment(ps.classp,(yylsp[-1]));
                ;}
    break;

  case 450:
#line 2284 "ned.y"
    {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString((yylsp[-1])));
                  setComments(ps.structp,(yylsp[-2]),(yylsp[-1]));
                ;}
    break;

  case 451:
#line 2290 "ned.y"
    {
                  setTrailingComment(ps.structp,(yylsp[-1]));
                ;}
    break;

  case 452:
#line 2294 "ned.y"
    {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString((yylsp[-3])));
                  ps.structp->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.structp,(yylsp[-4]),(yylsp[-1]));
                ;}
    break;

  case 453:
#line 2301 "ned.y"
    {
                  setTrailingComment(ps.structp,(yylsp[-1]));
                ;}
    break;

  case 454:
#line 2308 "ned.y"
    {
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,(yylsp[-1]));
                ;}
    break;

  case 461:
#line 2328 "ned.y"
    {
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString((yylsp[-3])));
                  ps.property->setValue(toString((yylsp[-1])));
                  setComments(ps.property,(yylsp[-3]),(yylsp[-1]));
                ;}
    break;

  case 468:
#line 2347 "ned.y"
    {
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,(yylsp[-1]));
                ;}
    break;

  case 475:
#line 2367 "ned.y"
    {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString((yylsp[0])));
                  ps.field->setDataType(toString((yylsp[-1])));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                ;}
    break;

  case 476:
#line 2375 "ned.y"
    {
                  setComments(ps.field,(yylsp[-7]),(yylsp[-1]));
                ;}
    break;

  case 477:
#line 2379 "ned.y"
    {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString((yylsp[0])));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                ;}
    break;

  case 478:
#line 2386 "ned.y"
    {
                  setComments(ps.field,(yylsp[-6]),(yylsp[-1]));
                ;}
    break;

  case 479:
#line 2393 "ned.y"
    { ps.isAbstract = true; ps.isReadonly = false; ;}
    break;

  case 480:
#line 2395 "ned.y"
    { ps.isAbstract = false; ps.isReadonly = true; ;}
    break;

  case 481:
#line 2397 "ned.y"
    { ps.isAbstract = true; ps.isReadonly = true; ;}
    break;

  case 482:
#line 2399 "ned.y"
    { ps.isAbstract = true; ps.isReadonly = true; ;}
    break;

  case 483:
#line 2401 "ned.y"
    { ps.isAbstract = false; ps.isReadonly = false; ;}
    break;

  case 497:
#line 2426 "ned.y"
    {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString((yylsp[-1])));
                ;}
    break;

  case 498:
#line 2431 "ned.y"
    {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString((yylsp[-1])));
                ;}
    break;

  case 499:
#line 2436 "ned.y"
    {
                  ps.field->setIsVector(true);
                ;}
    break;

  case 501:
#line 2444 "ned.y"
    {
                  ps.field->setEnumName(toString((yylsp[-1])));
                ;}
    break;

  case 503:
#line 2452 "ned.y"
    {
                  ps.field->setDefaultValue(toString((yylsp[0])));
                ;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 4546 "ned.tab.c"

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


#line 2505 "ned.y"


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

    np = p;
    ps.nedfile = nf;
    ps.parseExpressions = parseexpr;
    ps.storeSourceCode = storesrc;
    sourcefilename = sourcefname;

    if (storesrc)
        ps.nedfile->setSourceCode(np->nedsource->getFullText());

    try {
        return yyparse();
    } catch (NEDException *e) {
        NEDError(NULL, "internal error while parsing: %s", e->errorMessage());
        delete e;
        return 0;
    }
}


void yyerror (char *s)
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

void setFileComment(NEDElement *node)
{
    node->setAttribute("file-comment", np->nedsource->getFileComment() );
}

void setBannerComment(NEDElement *node, YYLTYPE tokenpos)
{
    node->setAttribute("banner-comment", np->nedsource->getBannerComment(tokenpos) );
}

void setRightComment(NEDElement *node, YYLTYPE tokenpos)
{
    node->setAttribute("right-comment", np->nedsource->getTrailingComment(tokenpos) );
}

void setTrailingComment(NEDElement *node, YYLTYPE tokenpos)
{
    node->setAttribute("trailing-comment", np->nedsource->getTrailingComment(tokenpos) );
}

void setComments(NEDElement *node, YYLTYPE pos)
{
    setBannerComment(node, pos);
    setRightComment(node, pos);
}

void setComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos)
{
    YYLTYPE pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;

    setBannerComment(node, pos);
    setRightComment(node, pos);
}

ChannelAttrNode *addChanAttr(NEDElement *channel, const char *attrname)
{
    ChannelAttrNode *chanattr = (ChannelAttrNode *)createNodeWithTag(NED_CHANNEL_ATTR, channel );
    chanattr->setName( attrname );
    return chanattr;
}

ParamNode *addParameter(NEDElement *params, YYLTYPE namepos, int type)
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

   ParamNode *param = (ParamNode *)createNodeWithTag(NED_PARAM,params);
   param->setName( toString( namepos) );
   param->setDataType( s );
   return param;
}

GateNode *addGate(NEDElement *gates, YYLTYPE namepos, int is_in, int is_vector )
{
   GateNode *gate = (GateNode *)createNodeWithTag(NED_GATE,gates);
   gate->setName( toString( namepos) );
   gate->setDirection(is_in ? NED_GATEDIR_INPUT : NED_GATEDIR_OUTPUT);
   gate->setIsVector(is_vector);
   return gate;
}

SubmoduleNode *addSubmodule(NEDElement *submods, YYLTYPE namepos, YYLTYPE typepos,YYLTYPE likeparampos)
{
   SubmoduleNode *submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE,submods);
   submod->setName( toString( namepos) );
   submod->setTypeName( toString( typepos) );
   submod->setLikeParam( toString( likeparampos) );

   return submod;
}

GatesizeNode *addGateSize(NEDElement *gatesizes, YYLTYPE namepos)
{
   GatesizeNode *gatesize = (GatesizeNode *)createNodeWithTag(NED_GATESIZE,gatesizes);
   gatesize->setName( toString( namepos) );
   return gatesize;
}

SubstparamNode *addSubstparam(NEDElement *substparams, YYLTYPE namepos)
{
   SubstparamNode *substparam = (SubstparamNode *)createNodeWithTag(NED_SUBSTPARAM,substparams);
   substparam->setName( toString( namepos) );
   return substparam;
}

SubstmachineNode *addSubstmachine(NEDElement *substmachines, YYLTYPE namepos)
{
   SubstmachineNode *substmachine = (SubstmachineNode *)createNodeWithTag(NED_SUBSTMACHINE,substmachines);
   substmachine->setName( toString( namepos) );
   return substmachine;
}

LoopVarNode *addLoopVar(NEDElement *forloop, YYLTYPE varnamepos)
{
   LoopVarNode *loopvar = (LoopVarNode *)createNodeWithTag(NED_LOOP_VAR,forloop);
   loopvar->setParamName( toString( varnamepos) );
   return loopvar;
}

ConnAttrNode *addConnAttr(NEDElement *conn, const char *attrname)
{
    ConnAttrNode *connattr = (ConnAttrNode *)createNodeWithTag(NED_CONN_ATTR,conn);
    connattr->setName( attrname );
    return connattr;
}


NetworkNode *addNetwork(NEDElement *nedfile, YYLTYPE namepos, YYLTYPE typepos)
{
   NetworkNode *network = (NetworkNode *)createNodeWithTag(NED_NETWORK,nedfile);
   network->setName( toString( namepos) );
   network->setTypeName( toString( typepos) );
   return network;
}

DisplayStringNode *addDisplayString(NEDElement *parent, YYLTYPE dispstrpos)
{
   DisplayStringNode *dispstr = (DisplayStringNode *)createNodeWithTag(NED_DISPLAY_STRING,parent);
   dispstr->setValue( toString( trimQuotes(dispstrpos)) );
   return dispstr;
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

ParamRefNode *createParamRef(const char *param, const char *paramindex, const char *module, const char *moduleindex)
{
   ParamRefNode *par = (ParamRefNode *)createNodeWithTag(NED_PARAM_REF);
   par->setParamName(param);
   if (paramindex) par->setParamIndex(paramindex);
   if (module) par->setModule(module);
   if (moduleindex) par->setModuleIndex(moduleindex);
   return par;
}

IdentNode *createIdent(const char *name)
{
   IdentNode *ident = (IdentNode *)createNodeWithTag(NED_IDENT);
   ident->setName(name);
   return ident;
}

ConstNode *createConst(int type, const char *value, const char *text)
{
   ConstNode *c = (ConstNode *)createNodeWithTag(NED_CONST);
   c->setType(type);
   if (value) c->setValue(value);
   if (text) c->setText(text);
   return c;
}

ConstNode *createTimeConst(const char *text)
{
   ConstNode *c = (ConstNode *)createNodeWithTag(NED_CONST);
   c->setType(NED_CONST_TIME);
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

NEDElement *createParamRefOrIdent(const char *name)
{
    // determine if 'name' can be a loop variable. if so, use createIdent()
    bool isvar = false;
    if (ps.inLoop)
    {
        for (NEDElement *child=ps.forloop->getFirstChildWithTag(NED_LOOP_VAR); child; child=child->getNextSiblingWithTag(NED_LOOP_VAR))
        {
            LoopVarNode *loopvar = (LoopVarNode *)child;
            if (!strcmp(loopvar->getParamName(),name))
                isvar = true;
        }
    }
    return isvar ? (NEDElement *)createIdent(name) : (NEDElement *)createParamRef(name);
}

NEDElement *unaryMinus(NEDElement *node)
{
    // if not a constant, must appy unary minus operator
    if (node->getTagCode()!=NED_CONST)
        return createOperator("-", node);

    ConstNode *constNode = (ConstNode *)node;

    // only int and real constants can be negative, string, bool, etc cannot
    if (constNode->getType()!=NED_CONST_INT && constNode->getType()!=NED_CONST_REAL)
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

