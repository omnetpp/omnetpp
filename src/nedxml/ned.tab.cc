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

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
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
     CHARTYPE = 315,
     SHORTTYPE = 316,
     INTTYPE = 317,
     LONGTYPE = 318,
     DOUBLETYPE = 319,
     UNSIGNED_ = 320,
     SIZEOF = 321,
     SUBMODINDEX = 322,
     PLUSPLUS = 323,
     EQ = 324,
     NE = 325,
     GT = 326,
     GE = 327,
     LS = 328,
     LE = 329,
     AND = 330,
     OR = 331,
     XOR = 332,
     NOT = 333,
     BIN_AND = 334,
     BIN_OR = 335,
     BIN_XOR = 336,
     BIN_COMPL = 337,
     SHIFT_LEFT = 338,
     SHIFT_RIGHT = 339,
     INVALID_CHAR = 340,
     UMIN = 341
   };
#endif
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
#define CHARTYPE 315
#define SHORTTYPE 316
#define INTTYPE 317
#define LONGTYPE 318
#define DOUBLETYPE 319
#define UNSIGNED_ 320
#define SIZEOF 321
#define SUBMODINDEX 322
#define PLUSPLUS 323
#define EQ 324
#define NE 325
#define GT 326
#define GE 327
#define LS 328
#define LE 329
#define AND 330
#define OR 331
#define XOR 332
#define NOT 333
#define BIN_AND 334
#define BIN_OR 335
#define BIN_XOR 336
#define BIN_COMPL 337
#define SHIFT_LEFT 338
#define SHIFT_RIGHT 339
#define INVALID_CHAR 340
#define UMIN 341




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
#line 438 "ned.tab.c"

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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   989

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  105
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  251
/* YYNRULES -- Number of rules. */
#define YYNRULES  516
/* YYNRULES -- Number of states. */
#define YYNSTATES  882

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   341

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    92,     2,     2,
     103,   104,    90,    88,    96,    89,   102,    91,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    87,    95,
       2,    99,     2,    86,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   100,     2,   101,    93,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    97,     2,    98,     2,     2,     2,     2,
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
      85,    94
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
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
    1506,  1507,  1509,  1510,  1513,  1515,  1516,  1524,  1525,  1534,
    1536,  1539,  1541,  1543,  1545,  1547,  1550,  1553,  1556,  1559,
    1561,  1563,  1565,  1569,  1573,  1576,  1577,  1582,  1583,  1586,
    1587,  1589,  1591,  1593,  1596,  1598,  1601,  1603,  1605,  1607,
    1609,  1611,  1614,  1616,  1618,  1619,  1621
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     106,     0,    -1,   107,    -1,   107,   108,    -1,    -1,   109,
      -1,   118,    -1,   125,    -1,   130,    -1,   299,    -1,   113,
      -1,   122,    -1,   127,    -1,   296,    -1,   315,    -1,   316,
      -1,   317,    -1,   318,    -1,   319,    -1,   320,    -1,   326,
      -1,   329,    -1,   332,    -1,    -1,     3,   110,   111,    95,
      -1,   111,    96,   112,    -1,   112,    -1,    35,    -1,   114,
     115,   117,    -1,     5,    34,    -1,    -1,   116,    -1,   116,
      31,   302,   354,    -1,    31,   302,   354,    -1,    26,    34,
     354,    -1,    26,   354,    -1,   119,    97,   120,    98,   354,
      -1,     5,    34,    -1,   121,    -1,    -1,   121,    34,    99,
     302,    95,    -1,    34,    99,   302,    95,    -1,   123,   132,
     148,   160,   124,    -1,     4,    34,    -1,    24,    34,   354,
      -1,    24,   354,    -1,   126,    97,   138,   153,   169,    98,
     354,    -1,     4,    34,    -1,   128,   132,   148,   160,   178,
     248,   144,   129,    -1,     6,    34,    -1,    25,    34,   354,
      -1,    25,   354,    -1,   131,    97,   138,   153,   169,   189,
     272,   146,    98,   354,    -1,     6,    34,    -1,   133,    -1,
      -1,    -1,    29,    87,   134,   135,    -1,   136,    95,    -1,
      -1,   136,    96,   137,    -1,   137,    -1,    34,    -1,   139,
      -1,    -1,    -1,    29,    87,   140,   141,    -1,   142,    95,
      -1,    -1,   142,    96,   143,    -1,   143,    -1,    34,    -1,
     145,    -1,    -1,    12,    87,    35,    95,    -1,   147,    -1,
      -1,    12,    87,    35,    95,    -1,   149,    -1,    -1,    -1,
       7,    87,   150,   151,    -1,   152,    95,    -1,    -1,   152,
      96,   158,    -1,   158,    -1,   154,    -1,    -1,    -1,     7,
      87,   155,   156,    -1,   157,    95,    -1,    -1,   157,    95,
     159,    -1,   159,    -1,   159,    -1,    34,    -1,    34,    87,
      44,    -1,    43,    34,    -1,    34,    87,    43,    -1,    34,
      87,    43,    44,    -1,    34,    87,    44,    43,    -1,    34,
      87,    45,    -1,    34,    87,    46,    -1,    34,    87,    47,
      -1,    34,    87,    48,    -1,   161,    -1,    -1,    -1,     8,
      87,   162,   163,    -1,   164,    -1,    -1,   164,    13,   165,
      95,    -1,    13,   165,    95,    -1,   164,    14,   167,    95,
      -1,    14,   167,    95,    -1,   165,    96,   166,    -1,   166,
      -1,    34,   100,   101,    -1,    34,    -1,   167,    96,   168,
      -1,   168,    -1,    34,   100,   101,    -1,    34,    -1,   170,
      -1,    -1,    -1,     8,    87,   171,   172,    -1,   173,    -1,
      -1,   173,    13,   174,    95,    -1,    13,   174,    95,    -1,
     173,    14,   176,    95,    -1,    14,   176,    95,    -1,   174,
      96,   175,    -1,   175,    -1,    34,   100,   101,    -1,    34,
      -1,   176,    96,   177,    -1,   177,    -1,    34,   100,   101,
      -1,    34,    -1,   179,    -1,    -1,    -1,    10,    87,   180,
     181,    -1,   182,    -1,    -1,   182,   183,    -1,   183,    -1,
      -1,    34,    87,    34,   354,   184,   200,   188,    -1,    -1,
      34,    87,    34,   301,   354,   185,   200,   188,    -1,    -1,
      34,    87,    34,    22,    34,   354,   186,   200,   188,    -1,
      -1,    34,    87,    34,   301,    22,    34,   354,   187,   200,
     188,    -1,   216,   232,   246,    -1,   190,    -1,    -1,    -1,
      10,    87,   191,   192,    -1,   193,    -1,    -1,   193,   194,
      -1,   194,    -1,    -1,    34,    87,    34,   354,   195,   208,
     199,    -1,    -1,    34,    87,    34,   301,   354,   196,   208,
     199,    -1,    -1,    34,    87,    34,    22,    34,   354,   197,
     208,   199,    -1,    -1,    34,    87,    34,   301,    22,    34,
     354,   198,   208,   199,    -1,   224,   239,   247,    -1,   201,
      -1,    -1,   201,   202,    -1,   202,    -1,    -1,    30,    87,
     203,   205,    -1,    -1,    30,    21,   302,    87,   204,   205,
      -1,   206,    95,    -1,    -1,   206,    96,   207,    -1,   207,
      -1,    34,    -1,   209,    -1,    -1,   209,   210,    -1,   210,
      -1,    -1,    30,    87,   211,   213,    -1,    -1,    30,    21,
     302,    87,   212,   213,    -1,   214,    95,    -1,    -1,   214,
      96,   215,    -1,   215,    -1,    34,    -1,   217,    -1,    -1,
     217,   218,    -1,   218,    -1,    -1,     7,    87,   219,   221,
      -1,    -1,     7,    21,   302,    87,   220,   221,    -1,   222,
      95,    -1,    -1,   222,    96,   223,    -1,   223,    -1,    34,
      99,   302,    -1,   225,    -1,    -1,   225,   226,    -1,   226,
      -1,    -1,     7,    87,   227,   229,    -1,    -1,     7,    21,
     302,    87,   228,   229,    -1,   230,    -1,    -1,   230,   231,
      -1,   231,    -1,    34,    99,   302,    95,    -1,   232,   233,
      -1,    -1,    -1,     9,    87,   234,   236,    -1,    -1,     9,
      21,   302,    87,   235,   236,    -1,   237,    95,    -1,    -1,
     237,    96,   238,    -1,   238,    -1,    34,   301,    -1,    34,
      -1,   239,   240,    -1,    -1,    -1,     9,    87,   241,   243,
      -1,    -1,     9,    21,   302,    87,   242,   243,    -1,   244,
      95,    -1,    -1,   244,    96,   245,    -1,   245,    -1,    34,
     301,    -1,    34,    -1,    12,    87,    35,    95,    -1,    -1,
      12,    87,    35,    95,    -1,    -1,   249,    -1,    -1,    -1,
      11,    15,    87,   250,   252,    -1,    -1,    11,    87,   251,
     252,    -1,   253,    -1,    -1,   253,   254,    -1,   254,    -1,
     255,    -1,   262,    -1,    -1,    18,   256,   257,    20,   261,
      28,   354,    -1,   258,    96,   257,    -1,   258,    -1,    34,
      99,   302,    19,   302,    -1,    21,   302,    -1,    -1,    12,
      35,    -1,    -1,   261,   262,    -1,   262,    -1,   263,    17,
     267,   259,   260,   355,    -1,   263,    17,   271,    17,   267,
     259,   260,   355,    -1,   263,    16,   267,   259,   260,   355,
      -1,   263,    16,   271,    16,   267,   259,   260,   355,    -1,
     264,   102,   265,    -1,   266,    -1,    34,   301,    -1,    34,
      -1,    34,   301,    -1,    34,    -1,    34,    68,    -1,    34,
     301,    -1,    34,    -1,    34,    68,    -1,   268,   102,   269,
      -1,   270,    -1,    34,   301,    -1,    34,    -1,    34,   301,
      -1,    34,    -1,    34,    68,    -1,    34,   301,    -1,    34,
      -1,    34,    68,    -1,    34,    -1,    31,   302,    -1,   271,
      31,   302,    -1,   273,    -1,    -1,    -1,    11,    15,    87,
     274,   276,    -1,    -1,    11,    87,   275,   276,    -1,   277,
      -1,    -1,   277,   278,    -1,   278,    -1,   279,    -1,   286,
      -1,    -1,    18,   280,   281,    20,   285,    28,    95,    -1,
     282,    96,   281,    -1,   282,    -1,    34,    99,   302,    19,
     302,    -1,    21,   302,    -1,    -1,    12,    35,    -1,    -1,
     285,   286,    -1,   286,    -1,   287,    17,   291,   283,   284,
      95,    -1,   287,    17,   295,    17,   291,   283,   284,    95,
      -1,   287,    16,   291,   283,   284,    95,    -1,   287,    16,
     295,    16,   291,   283,   284,    95,    -1,   288,   102,   289,
      -1,   290,    -1,    34,   301,    -1,    34,    -1,    34,   301,
      -1,    34,    -1,    34,    68,    -1,    34,   301,    -1,    34,
      -1,    34,    68,    -1,   292,   102,   293,    -1,   294,    -1,
      34,   301,    -1,    34,    -1,    34,   301,    -1,    34,    -1,
      34,    68,    -1,    34,   301,    -1,    34,    -1,    34,    68,
      -1,    34,    -1,    34,   302,    -1,   295,    34,   302,    -1,
     297,   200,   216,   298,    -1,    23,    34,    87,    34,   354,
      -1,    27,   354,    -1,   300,    97,   208,   224,    98,   354,
      -1,    23,    34,    87,    34,   354,    -1,   100,   302,   101,
      -1,   305,    -1,   303,    -1,   304,    -1,    39,   103,   305,
      96,   305,   104,    -1,    39,   103,   305,   104,    -1,    39,
     103,   104,    -1,    39,    -1,    40,   103,   312,    96,   312,
     104,    -1,    40,   103,   312,   104,    -1,   306,    -1,   103,
     305,   104,    -1,   305,    88,   305,    -1,   305,    89,   305,
      -1,   305,    90,   305,    -1,   305,    91,   305,    -1,   305,
      92,   305,    -1,   305,    93,   305,    -1,    89,   305,    -1,
     305,    69,   305,    -1,   305,    70,   305,    -1,   305,    71,
     305,    -1,   305,    72,   305,    -1,   305,    73,   305,    -1,
     305,    74,   305,    -1,   305,    75,   305,    -1,   305,    76,
     305,    -1,   305,    77,   305,    -1,    78,   305,    -1,   305,
      79,   305,    -1,   305,    80,   305,    -1,   305,    81,   305,
      -1,    82,   305,    -1,   305,    83,   305,    -1,   305,    84,
     305,    -1,   305,    86,   305,    87,   305,    -1,    34,   103,
     104,    -1,    34,   103,   305,   104,    -1,    34,   103,   305,
      96,   305,   104,    -1,    34,   103,   305,    96,   305,    96,
     305,   104,    -1,    34,   103,   305,    96,   305,    96,   305,
      96,   305,   104,    -1,   307,    -1,   308,    -1,   309,    -1,
     310,    -1,   311,    -1,    34,    -1,    41,    34,    -1,    41,
      42,    34,    -1,    42,    41,    34,    -1,    42,    34,    -1,
     312,    -1,    37,    -1,    38,    -1,   313,    -1,    67,    -1,
      67,   103,   104,    -1,    66,   103,    34,   104,    -1,    35,
      -1,    32,    -1,    33,    -1,   314,    -1,   314,    32,    34,
      -1,   314,    33,    34,    -1,    32,    34,    -1,    33,    34,
      -1,    49,    50,   354,    -1,    53,    34,    95,    -1,    52,
      34,    95,    -1,    52,    55,    34,    95,    -1,    51,    34,
      95,    -1,    54,    34,    95,    -1,    -1,    54,    34,    97,
     321,   323,    98,   354,    -1,    -1,    54,    34,    56,    34,
      97,   322,   323,    98,   354,    -1,   324,    -1,    -1,   324,
     325,    -1,   325,    -1,    34,    95,    -1,    34,    99,   353,
      95,    -1,    -1,    51,    34,    97,   327,   335,   341,    98,
     354,    -1,    -1,    51,    34,    56,    34,    97,   328,   335,
     341,    98,   354,    -1,    -1,    52,    34,    97,   330,   335,
     341,    98,   354,    -1,    -1,    52,    34,    56,    34,    97,
     331,   335,   341,    98,   354,    -1,    -1,    53,    34,    97,
     333,   335,   341,    98,   354,    -1,    -1,    53,    34,    56,
      34,    97,   334,   335,   341,    98,   354,    -1,    -1,    58,
      87,   336,   337,    -1,    -1,   338,    -1,    -1,   338,   339,
      -1,   339,    -1,    34,    99,   340,    95,    -1,    35,    -1,
      32,    -1,    33,    -1,   314,    -1,    37,    -1,    38,    -1,
      -1,    57,    87,   342,   343,    -1,    -1,   344,    -1,    -1,
     344,   345,    -1,   345,    -1,    -1,   348,    34,   346,   349,
     350,   351,    95,    -1,    -1,    59,   348,    34,   347,   349,
     350,   351,    95,    -1,    34,    -1,    34,    90,    -1,    60,
      -1,    61,    -1,    62,    -1,    63,    -1,    65,    60,    -1,
      65,    61,    -1,    65,    62,    -1,    65,    63,    -1,    64,
      -1,    45,    -1,    46,    -1,   100,    32,   101,    -1,   100,
      34,   101,    -1,   100,   101,    -1,    -1,    54,   103,    34,
     104,    -1,    -1,    99,   352,    -1,    -1,    35,    -1,    36,
      -1,    32,    -1,    89,    32,    -1,    33,    -1,    89,    33,
      -1,   314,    -1,    37,    -1,    38,    -1,    34,    -1,    32,
      -1,    89,    32,    -1,    34,    -1,    95,    -1,    -1,    96,
      -1,    95,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   239,   239,   243,   244,   248,   250,   252,   254,   256,
     259,   261,   263,   265,   268,   269,   270,   271,   272,   274,
     276,   278,   280,   289,   288,   300,   301,   305,   317,   321,
     329,   331,   335,   341,   350,   354,   364,   373,   383,   384,
     388,   394,   406,   414,   424,   428,   438,   449,   462,   473,
     483,   487,   497,   511,   524,   525,   530,   529,   540,   541,
     545,   546,   550,   562,   563,   568,   567,   578,   579,   583,
     584,   588,   600,   601,   605,   615,   616,   620,   630,   631,
     636,   635,   646,   647,   651,   655,   665,   666,   671,   670,
     681,   682,   686,   690,   701,   705,   709,   713,   717,   721,
     725,   729,   733,   737,   741,   751,   752,   757,   756,   767,
     768,   772,   773,   774,   775,   779,   780,   784,   789,   797,
     798,   802,   807,   818,   819,   824,   823,   834,   835,   839,
     840,   841,   842,   846,   847,   851,   856,   864,   865,   869,
     874,   885,   886,   891,   890,   901,   902,   906,   907,   912,
     911,   921,   920,   931,   930,   940,   939,   952,   961,   962,
     967,   966,   977,   978,   982,   983,   988,   987,   997,   996,
    1007,  1006,  1016,  1015,  1028,  1037,  1038,  1042,  1043,  1048,
    1047,  1056,  1055,  1067,  1068,  1072,  1073,  1077,  1088,  1089,
    1093,  1094,  1099,  1098,  1107,  1106,  1118,  1119,  1123,  1124,
    1128,  1139,  1140,  1144,  1145,  1150,  1149,  1158,  1157,  1170,
    1171,  1175,  1176,  1180,  1192,  1193,  1197,  1198,  1203,  1202,
    1211,  1210,  1223,  1224,  1228,  1229,  1233,  1245,  1246,  1251,
    1250,  1259,  1258,  1270,  1271,  1275,  1276,  1280,  1287,  1298,
    1299,  1304,  1303,  1312,  1311,  1323,  1324,  1328,  1329,  1333,
    1340,  1351,  1355,  1362,  1366,  1373,  1374,  1379,  1378,  1388,
    1387,  1399,  1400,  1404,  1405,  1409,  1410,  1415,  1414,  1428,
    1429,  1433,  1443,  1447,  1451,  1455,  1459,  1460,  1464,  1469,
    1474,  1480,  1489,  1490,  1494,  1500,  1508,  1513,  1517,  1525,
    1532,  1538,  1548,  1549,  1553,  1558,  1565,  1570,  1574,  1582,
    1587,  1591,  1600,  1605,  1610,  1621,  1622,  1627,  1626,  1636,
    1635,  1647,  1648,  1652,  1653,  1657,  1658,  1663,  1662,  1676,
    1677,  1681,  1691,  1695,  1699,  1703,  1707,  1708,  1712,  1717,
    1722,  1728,  1737,  1738,  1742,  1748,  1756,  1761,  1765,  1773,
    1780,  1786,  1796,  1797,  1801,  1806,  1813,  1818,  1822,  1830,
    1835,  1839,  1848,  1853,  1858,  1869,  1876,  1885,  1896,  1907,
    1920,  1926,  1930,  1934,  1945,  1947,  1949,  1951,  1956,  1958,
    1963,  1964,  1967,  1969,  1971,  1973,  1975,  1977,  1980,  1984,
    1986,  1988,  1990,  1992,  1994,  1997,  1999,  2001,  2004,  2008,
    2010,  2012,  2015,  2018,  2020,  2022,  2025,  2027,  2029,  2031,
    2033,  2038,  2039,  2040,  2041,  2042,  2046,  2051,  2056,  2062,
    2068,  2076,  2080,  2082,  2087,  2091,  2093,  2095,  2100,  2105,
    2107,  2109,  2115,  2116,  2117,  2118,  2126,  2135,  2144,  2151,
    2161,  2170,  2180,  2179,  2191,  2190,  2205,  2206,  2210,  2211,
    2215,  2221,  2232,  2231,  2242,  2241,  2256,  2255,  2266,  2265,
    2280,  2279,  2290,  2289,  2304,  2303,  2309,  2313,  2314,  2318,
    2319,  2323,  2333,  2334,  2335,  2336,  2337,  2338,  2343,  2342,
    2348,  2352,  2353,  2357,  2358,  2363,  2362,  2373,  2372,  2386,
    2387,  2389,  2390,  2391,  2392,  2394,  2395,  2396,  2397,  2399,
    2400,  2401,  2406,  2411,  2416,  2420,  2424,  2428,  2432,  2436,
    2440,  2441,  2442,  2443,  2444,  2445,  2446,  2447,  2448,  2449,
    2453,  2454,  2455,  2458,  2458,  2460,  2460
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
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
  "NONCOBJECT", "EXTENDS", "FIELDS", "PROPERTIES", "ABSTRACT", "CHARTYPE", 
  "SHORTTYPE", "INTTYPE", "LONGTYPE", "DOUBLETYPE", "UNSIGNED_", "SIZEOF", 
  "SUBMODINDEX", "PLUSPLUS", "EQ", "NE", "GT", "GE", "LS", "LE", "AND", 
  "OR", "XOR", "NOT", "BIN_AND", "BIN_OR", "BIN_XOR", "BIN_COMPL", 
  "SHIFT_LEFT", "SHIFT_RIGHT", "INVALID_CHAR", "'?'", "':'", "'+'", "'-'", 
  "'*'", "'/'", "'%'", "'^'", "UMIN", "';'", "','", "'{'", "'}'", "'='", 
  "'['", "']'", "'.'", "'('", "')'", "$accept", "networkdescription", 
  "somedefinitions", "definition", "import", "@1", "filenames", 
  "filename", "channeldefinition_old", "channelheader_old", 
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
  "parameters_old", "opt_paramblock", "paramblock", "@5", 
  "opt_parameters", "parameters", "parameter_old", "parameter", 
  "opt_gateblock_old", "gateblock_old", "@6", "opt_gates_old", 
  "gates_old", "gatesI_old", "gateI_old", "gatesO_old", "gateO_old", 
  "opt_gateblock", "gateblock", "@7", "opt_gates", "gates", "gatesI", 
  "gateI", "gatesO", "gateO", "opt_submodblock_old", "submodblock_old", 
  "@8", "opt_submodules_old", "submodules_old", "submodule_old", "@9", 
  "@10", "@11", "@12", "submodule_body_old", "opt_submodblock", 
  "submodblock", "@13", "opt_submodules", "submodules", "submodule", 
  "@14", "@15", "@16", "@17", "submodule_body", "opt_on_blocks_old", 
  "on_blocks_old", "on_block_old", "@18", "@19", "opt_on_list_old", 
  "on_list_old", "on_mach_old", "opt_on_blocks", "on_blocks", "on_block", 
  "@20", "@21", "opt_on_list", "on_list", "on_mach", 
  "opt_substparamblocks_old", "substparamblocks_old", 
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
  "leftmod_old", "leftgate_old", "parentleftgate_old", 
  "rightgatespec_old", "rightmod_old", "rightgate_old", 
  "parentrightgate_old", "channeldescr_old", "opt_connblock", "connblock", 
  "@33", "@34", "opt_connections", "connections", "connection", 
  "loopconnection", "@35", "loopvarlist", "loopvar", "opt_conncondition", 
  "opt_conn_displaystr", "notloopconnections", "notloopconnection", 
  "leftgatespec", "leftmod", "leftgate", "parentleftgate", 
  "rightgatespec", "rightmod", "rightgate", "parentrightgate", 
  "channeldescr", "networkdefinition_old", "networkheader_old", 
  "endnetwork_old", "networkdefinition", "networkheader", "vector", 
  "expression", "inputvalue", "xmldocvalue", "expr", "simple_expr", 
  "parameter_expr", "string_expr", "boolconst_expr", "numconst_expr", 
  "special_expr", "stringconstant", "numconst", "timeconstant", 
  "cplusplus", "struct_decl", "class_decl", "message_decl", "enum_decl", 
  "enum", "@36", "@37", "opt_enumfields", "enumfields", "enumfield", 
  "message", "@38", "@39", "class", "@40", "@41", "struct", "@42", "@43", 
  "opt_propertiesblock", "@44", "opt_properties", "properties", 
  "property", "propertyvalue", "opt_fieldsblock", "@45", "opt_fields", 
  "fields", "field", "@46", "@47", "fielddatatype", "opt_fieldvector", 
  "opt_fieldenum", "opt_fieldvalue", "fieldvalue", "enumvalue", 
  "opt_semicolon", "comma_or_semicolon", 0
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
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,    63,    58,    43,    45,
      42,    47,    37,    94,   341,    59,    44,   123,   125,    61,
      91,    93,    46,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   105,   106,   107,   107,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   110,   109,   111,   111,   112,   113,   114,
     115,   115,   116,   116,   117,   117,   118,   119,   120,   120,
     121,   121,   122,   123,   124,   124,   125,   126,   127,   128,
     129,   129,   130,   131,   132,   132,   134,   133,   135,   135,
     136,   136,   137,   138,   138,   140,   139,   141,   141,   142,
     142,   143,   144,   144,   145,   146,   146,   147,   148,   148,
     150,   149,   151,   151,   152,   152,   153,   153,   155,   154,
     156,   156,   157,   157,   158,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   160,   160,   162,   161,   163,
     163,   164,   164,   164,   164,   165,   165,   166,   166,   167,
     167,   168,   168,   169,   169,   171,   170,   172,   172,   173,
     173,   173,   173,   174,   174,   175,   175,   176,   176,   177,
     177,   178,   178,   180,   179,   181,   181,   182,   182,   184,
     183,   185,   183,   186,   183,   187,   183,   188,   189,   189,
     191,   190,   192,   192,   193,   193,   195,   194,   196,   194,
     197,   194,   198,   194,   199,   200,   200,   201,   201,   203,
     202,   204,   202,   205,   205,   206,   206,   207,   208,   208,
     209,   209,   211,   210,   212,   210,   213,   213,   214,   214,
     215,   216,   216,   217,   217,   219,   218,   220,   218,   221,
     221,   222,   222,   223,   224,   224,   225,   225,   227,   226,
     228,   226,   229,   229,   230,   230,   231,   232,   232,   234,
     233,   235,   233,   236,   236,   237,   237,   238,   238,   239,
     239,   241,   240,   242,   240,   243,   243,   244,   244,   245,
     245,   246,   246,   247,   247,   248,   248,   250,   249,   251,
     249,   252,   252,   253,   253,   254,   254,   256,   255,   257,
     257,   258,   259,   259,   260,   260,   261,   261,   262,   262,
     262,   262,   263,   263,   264,   264,   265,   265,   265,   266,
     266,   266,   267,   267,   268,   268,   269,   269,   269,   270,
     270,   270,   271,   271,   271,   272,   272,   274,   273,   275,
     273,   276,   276,   277,   277,   278,   278,   280,   279,   281,
     281,   282,   283,   283,   284,   284,   285,   285,   286,   286,
     286,   286,   287,   287,   288,   288,   289,   289,   289,   290,
     290,   290,   291,   291,   292,   292,   293,   293,   293,   294,
     294,   294,   295,   295,   295,   296,   297,   298,   299,   300,
     301,   302,   302,   302,   303,   303,   303,   303,   304,   304,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   306,   306,   306,   306,   306,   307,   307,   307,   307,
     307,   308,   309,   309,   310,   311,   311,   311,   312,   313,
     313,   313,   314,   314,   314,   314,   315,   316,   317,   317,
     318,   319,   321,   320,   322,   320,   323,   323,   324,   324,
     325,   325,   327,   326,   328,   326,   330,   329,   331,   329,
     333,   332,   334,   332,   336,   335,   335,   337,   337,   338,
     338,   339,   340,   340,   340,   340,   340,   340,   342,   341,
     341,   343,   343,   344,   344,   346,   345,   347,   345,   348,
     348,   348,   348,   348,   348,   348,   348,   348,   348,   348,
     348,   348,   349,   349,   349,   349,   350,   350,   351,   351,
     352,   352,   352,   352,   352,   352,   352,   352,   352,   352,
     353,   353,   353,   354,   354,   355,   355
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
       0,     1,     0,     2,     1,     0,     7,     0,     8,     1,
       2,     1,     1,     1,     1,     2,     2,     2,     2,     1,
       1,     1,     3,     3,     2,     0,     4,     0,     2,     0,
       1,     1,     1,     2,     1,     2,     1,     1,     1,     1,
       1,     2,     1,     1,     0,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       4,     0,     2,     1,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     3,     5,    10,    30,     6,     0,
      11,    55,     7,     0,    12,    55,     8,     0,    13,   176,
       9,     0,    14,    15,    16,    17,    18,    19,    20,    21,
      22,     0,    43,    29,    49,     0,   514,     0,     0,     0,
       0,     0,     0,     0,    31,    39,     0,    79,    54,    64,
      79,    64,     0,   202,   175,   178,   189,    27,     0,    26,
       0,   513,   426,     0,   430,   442,     0,   428,   446,     0,
       0,   427,   450,     0,   431,   432,   419,   420,   406,   418,
     412,   413,   367,     0,     0,     0,     0,   415,     0,     0,
       0,     0,   514,   362,   363,   361,   370,   401,   402,   403,
     404,   405,   411,   414,   421,   514,    28,     0,     0,     0,
      38,    56,     0,   106,    78,     0,    87,    63,   106,    87,
       0,   179,     0,     0,   201,   204,   177,     0,   215,   188,
     191,    24,     0,   514,     0,   456,     0,   456,   429,     0,
     456,     0,   437,   424,   425,     0,     0,     0,   407,     0,
     410,     0,     0,     0,   388,   392,   378,     0,    33,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   514,    35,   514,     0,   514,     0,    59,    80,
       0,     0,   105,    65,     0,   124,    86,   142,   124,     0,
     184,     0,   205,   514,   355,   203,     0,   192,     0,     0,
     214,   217,   190,    25,   356,   444,     0,   470,   448,   470,
     452,   470,   434,     0,     0,   436,   439,   396,     0,   366,
       0,     0,   408,   409,     0,   416,   371,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   389,   390,   391,   393,
     394,     0,   372,   373,   374,   375,   376,   377,   422,   423,
      34,    32,     0,    36,     0,    62,    57,     0,    61,    83,
     107,   514,    42,    68,    88,     0,     0,   123,     0,   256,
     141,   159,   181,   187,   180,     0,   186,     0,   210,   357,
       0,   197,     0,   218,   514,   216,   456,   454,     0,     0,
     456,     0,   456,     0,   437,   440,     0,   514,   438,     0,
     397,     0,   365,     0,   369,   417,     0,    41,     0,    58,
       0,    95,     0,    81,     0,    85,    94,   110,   514,    45,
      71,    66,     0,    70,    91,   125,   514,   143,     0,    73,
     255,     0,   306,   158,   184,   183,     0,   207,     0,   206,
       0,   212,   194,   200,   193,     0,   199,     0,   223,   358,
     470,   458,   468,   514,   470,   514,   470,   514,     0,   510,
     512,     0,     0,   433,     0,     0,     0,   395,    40,    60,
       0,    97,    82,     0,     0,     0,   108,   109,    44,    67,
       0,    89,     0,    93,   128,    46,   146,     0,   259,     0,
       0,    72,   160,     0,    76,   305,   182,   185,   210,     0,
     209,     0,   197,   196,     0,   220,     0,   219,   222,   225,
       0,     0,   455,   457,   460,   472,   443,     0,   447,     0,
     451,   514,   511,   441,     0,   398,   364,   368,    98,    96,
     101,   102,   103,   104,    84,   118,     0,   116,   122,     0,
     120,     0,     0,    69,    90,     0,     0,   126,   127,     0,
     144,   145,   148,   257,   262,     0,   514,    48,   163,     0,
     309,     0,     0,    75,   208,   213,   211,   195,   198,   223,
       0,   224,   514,     0,   459,   479,   490,   491,     0,   481,
     482,   483,   484,   489,     0,   469,   471,   474,     0,   514,
     514,   435,     0,    99,   100,     0,   112,     0,     0,   114,
       0,     0,     0,    92,   136,     0,   134,   140,     0,   138,
       0,     0,     0,   147,   262,   267,   290,   260,   261,   264,
     265,   266,     0,     0,   283,     0,   514,    51,     0,   161,
     162,   165,   307,   312,     0,   514,   221,     0,   445,   463,
     464,   462,   466,   467,   465,     0,   480,     0,   485,   486,
     487,   488,   473,   475,   449,   453,     0,   399,   117,   115,
     121,   119,   111,   113,     0,   130,     0,     0,   132,     0,
       0,     0,   514,   258,     0,   291,     0,   289,   263,     0,
       0,     0,    74,    50,     0,   164,   312,   317,   340,   310,
     311,   314,   315,   316,     0,     0,   333,     0,    52,   226,
     461,   477,   495,     0,   135,   133,   139,   137,   129,   131,
       0,   514,   149,     0,     0,   270,     0,     0,   300,   273,
       0,   293,     0,   273,     0,   287,   282,   514,   308,     0,
     341,   339,   313,     0,     0,     0,    77,   495,     0,   497,
     400,   514,     0,   151,   176,     0,     0,     0,   360,   303,
     301,   299,     0,   275,     0,     0,     0,   275,     0,   288,
     286,     0,   514,   166,     0,     0,   320,   350,   323,     0,
     343,     0,   323,     0,   337,   332,   497,     0,     0,   494,
       0,   499,   153,   514,   176,   202,     0,     0,   277,   269,
     272,     0,     0,   297,   292,   300,   273,   304,     0,   273,
     514,     0,   168,   189,     0,     0,     0,   351,   349,   353,
       0,   325,     0,     0,     0,   325,     0,   338,   336,   499,
     492,   493,     0,     0,     0,   176,   155,   202,   150,   228,
       0,   514,   276,   274,   516,   515,   280,   298,   296,   275,
     278,   275,   170,   514,   189,   215,     0,     0,   327,   319,
     322,     0,     0,   347,   342,   350,   323,   354,     0,   323,
       0,     0,   502,   504,   509,   500,   501,   507,   508,     0,
     506,   498,   476,   202,   176,   152,   252,   271,   268,     0,
       0,   189,   172,   215,   167,   240,     0,     0,   326,   324,
     330,   348,   346,   325,   328,   325,   478,   496,   503,   505,
     154,   202,     0,     0,   227,   157,   281,   279,   215,   189,
     169,   254,   321,   318,     0,     0,   156,     0,   229,     0,
     171,   215,     0,     0,   239,   174,   331,   329,     0,   234,
       0,   173,     0,   241,     0,   231,   238,   230,     0,   236,
     251,     0,   246,     0,   234,   237,   233,     0,   243,   250,
     242,     0,   248,   253,   232,   235,   246,   249,   245,     0,
     244,   247
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     2,    14,    15,    41,    68,    69,    16,    17,
      53,    54,   116,    18,    19,   119,   120,    20,    21,   282,
      22,    23,    24,    25,   477,    26,    27,    57,    58,   198,
     276,   277,   278,   126,   127,   283,   341,   342,   343,   410,
     411,   482,   483,   123,   124,   279,   333,   334,   205,   206,
     344,   401,   402,   335,   336,   201,   202,   337,   396,   397,
     456,   457,   459,   460,   286,   287,   404,   467,   468,   525,
     526,   528,   529,   289,   290,   406,   470,   471,   472,   664,
     704,   745,   794,   748,   352,   353,   478,   549,   550,   551,
     723,   764,   801,   829,   804,    63,    64,    65,   210,   354,
     294,   295,   296,   138,   139,   140,   301,   422,   364,   365,
     366,   749,   134,   135,   298,   418,   359,   360,   361,   805,
     220,   221,   368,   489,   427,   428,   429,   796,   824,   849,
     864,   857,   858,   859,   831,   844,   862,   876,   870,   871,
     872,   825,   845,   349,   350,   534,   474,   537,   538,   539,
     540,   594,   634,   635,   673,   712,   707,   541,   542,   543,
     646,   544,   639,   640,   714,   641,   642,   414,   415,   606,
     553,   609,   610,   611,   612,   649,   685,   686,   731,   772,
     767,   613,   614,   615,   695,   616,   688,   689,   774,   690,
     691,    28,    29,   214,    30,    31,   671,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
      32,    33,    34,    35,    36,    37,   152,   314,   234,   235,
     236,    38,   145,   306,    39,   147,   310,    40,   150,   312,
     227,   371,   432,   433,   434,   565,   309,   435,   505,   506,
     507,   622,   657,   508,   659,   701,   744,   791,   382,    72,
     756
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -726
static const short yypact[] =
{
    -726,    39,    28,  -726,  -726,    -6,    29,   209,   234,   118,
     254,    -8,   256,   260,  -726,  -726,  -726,   170,  -726,    87,
    -726,   230,  -726,   166,  -726,   230,  -726,   184,  -726,   221,
    -726,   202,  -726,  -726,  -726,  -726,  -726,  -726,  -726,  -726,
    -726,   268,   211,   213,   218,   219,   231,   -29,   107,   283,
     149,   159,   321,   302,   313,   299,   270,   363,  -726,   345,
     363,   345,    -4,   379,   221,  -726,   377,  -726,   -34,  -726,
     356,  -726,  -726,   362,  -726,  -726,   389,  -726,  -726,   305,
     391,  -726,  -726,   394,  -726,  -726,   407,   441,   419,  -726,
    -726,  -726,   420,   422,   130,   132,   424,   425,   360,   360,
     360,   360,   231,  -726,  -726,   747,  -726,  -726,  -726,  -726,
    -726,  -726,  -726,  -726,   217,    42,  -726,   321,   383,   396,
     458,  -726,   437,   518,  -726,   442,   524,  -726,   518,   524,
     321,  -726,   120,   505,   379,  -726,  -726,   133,   526,   377,
    -726,  -726,   268,   231,   438,   478,   440,   478,  -726,   443,
     478,   444,   500,  -726,  -726,   245,   263,   504,  -726,   508,
    -726,   509,   510,   434,  -726,  -726,  -726,   669,  -726,   360,
     360,   360,   360,   360,   360,   360,   360,   360,   360,   360,
     360,   360,   360,   360,   360,   360,   360,   360,   360,   360,
     511,   513,   231,  -726,   231,   321,   231,   451,   515,  -726,
     464,   528,  -726,  -726,   466,   546,  -726,   547,   546,   469,
     527,   321,  -726,   231,  -726,  -726,   321,  -726,   134,   460,
     526,  -726,  -726,  -726,   465,  -726,   477,   506,  -726,   506,
    -726,   506,  -726,   153,   467,   500,  -726,  -726,   397,  -726,
     551,    90,  -726,  -726,   462,  -726,  -726,   367,   367,   367,
     367,   367,   367,   797,   797,   797,   288,   288,   288,   329,
     329,   503,   246,   246,   474,   474,   474,   474,  -726,  -726,
    -726,  -726,   473,  -726,   321,  -726,  -726,   197,  -726,    16,
    -726,   124,  -726,   535,  -726,   484,   487,  -726,   501,   586,
    -726,   588,  -726,  -726,  -726,   225,  -726,   512,   566,  -726,
     514,   569,   321,  -726,   231,  -726,   478,  -726,   517,   507,
     478,   516,   478,   520,   500,  -726,   144,   231,  -726,   360,
    -726,   360,  -726,   504,  -726,  -726,   360,  -726,   521,  -726,
     515,   519,   574,  -726,   229,  -726,  -726,   271,   231,  -726,
    -726,  -726,   236,  -726,    16,  -726,   231,  -726,     9,   597,
    -726,   523,   600,  -726,   527,  -726,   527,  -726,   530,  -726,
     247,  -726,  -726,  -726,  -726,   251,  -726,   525,   581,  -726,
     506,   602,  -726,   231,   506,   231,   506,   231,   540,  -726,
    -726,   613,   553,  -726,   594,   695,   542,   772,  -726,  -726,
     459,  -726,  -726,    16,   615,   617,  -726,   337,  -726,  -726,
     535,  -726,   557,  -726,   351,  -726,   619,   567,  -726,   570,
     631,  -726,  -726,    23,   646,  -726,  -726,  -726,   566,   321,
    -726,   566,   569,  -726,   569,  -726,   560,  -726,   581,  -726,
     562,   573,  -726,   602,  -726,   370,  -726,   563,  -726,   578,
    -726,   231,  -726,  -726,   360,  -726,  -726,  -726,   635,   638,
    -726,  -726,  -726,  -726,  -726,   591,   273,  -726,   592,   310,
    -726,   615,   617,  -726,    16,   654,   659,  -726,   398,   607,
    -726,   619,  -726,  -726,    22,   660,   128,  -726,   662,   610,
    -726,   612,   603,  -726,  -726,  -726,  -726,  -726,  -726,   581,
     321,  -726,   231,   281,  -726,   621,  -726,  -726,   450,  -726,
    -726,  -726,  -726,  -726,   322,  -726,   370,  -726,   666,   231,
     231,  -726,   633,  -726,  -726,   614,  -726,   615,   626,  -726,
     617,   318,   341,  -726,   618,   344,  -726,   628,   348,  -726,
     654,   659,   696,  -726,    22,  -726,   -45,  -726,    22,  -726,
    -726,  -726,   436,   629,  -726,   637,   231,  -726,   647,  -726,
     662,  -726,  -726,    31,   698,   231,  -726,   641,  -726,   407,
     441,  -726,  -726,  -726,   217,   652,  -726,   717,  -726,  -726,
    -726,  -726,  -726,  -726,  -726,  -726,   360,  -726,  -726,  -726,
    -726,  -726,  -726,  -726,   653,  -726,   654,   655,  -726,   659,
     366,   369,    45,  -726,   729,  -726,   321,   675,  -726,   198,
     198,   746,  -726,  -726,   748,  -726,    31,  -726,   167,  -726,
      31,  -726,  -726,  -726,   481,   687,  -726,   708,  -726,  -726,
    -726,  -726,   706,   721,  -726,  -726,  -726,  -726,  -726,  -726,
     774,   -10,  -726,   716,   809,   736,   733,   321,     4,   829,
     752,  -726,    21,   829,   150,   -27,  -726,    48,  -726,   823,
    -726,   756,  -726,   825,   825,   838,  -726,   706,   117,   819,
    -726,   231,   840,  -726,   221,   321,   841,   729,  -726,  -726,
    -726,   777,   321,   870,   849,   850,   321,   870,   850,  -726,
    -726,   857,    -3,  -726,   793,   873,   798,   158,   874,   794,
    -726,   143,   874,    19,   -14,  -726,   819,   796,   799,  -726,
     795,   800,  -726,   231,   221,   379,   882,   154,  -726,  -726,
    -726,   867,   404,   138,  -726,   189,   829,  -726,   404,   829,
     231,   869,  -726,   377,   321,   871,   823,  -726,   802,  -726,
     321,   894,   875,   876,   321,   894,   876,  -726,  -726,   800,
    -726,  -726,   877,   176,   812,   221,  -726,   379,  -726,  -726,
     321,   231,  -726,  -726,  -726,  -726,  -726,  -726,  -726,   870,
    -726,   870,  -726,   231,   377,   526,   889,   155,  -726,  -726,
    -726,   878,   817,   162,  -726,   207,   874,  -726,   820,   874,
     821,   810,   407,   441,  -726,  -726,  -726,  -726,  -726,   476,
     217,  -726,  -726,   379,   221,  -726,   222,  -726,  -726,   404,
     404,   377,  -726,   526,  -726,  -726,   321,   822,  -726,  -726,
    -726,  -726,  -726,   894,  -726,   894,  -726,  -726,  -726,  -726,
    -726,   379,   135,   831,  -726,  -726,  -726,  -726,   526,   377,
    -726,   261,  -726,  -726,   824,   826,  -726,   321,  -726,   885,
    -726,   526,   140,   835,  -726,  -726,  -726,  -726,   836,   890,
     830,  -726,   321,  -726,   891,  -726,   827,  -726,   421,  -726,
    -726,   842,   896,   833,   890,  -726,  -726,   890,  -726,   827,
    -726,   423,  -726,  -726,  -726,  -726,   896,  -726,  -726,   896,
    -726,  -726
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -726,  -726,  -726,  -726,  -726,  -726,  -726,   789,  -726,  -726,
    -726,  -726,  -726,  -726,  -726,  -726,  -726,  -726,  -726,  -726,
    -726,  -726,  -726,  -726,  -726,  -726,  -726,   907,  -726,  -726,
    -726,  -726,   604,   872,  -726,  -726,  -726,  -726,   536,  -726,
    -726,  -726,  -726,   879,  -726,  -726,  -726,  -726,   806,  -726,
    -726,  -726,  -726,   544,  -329,   813,  -726,  -726,  -726,  -726,
     479,   426,   480,   418,   737,  -726,  -726,  -726,  -726,   414,
     361,   415,   359,  -726,  -726,  -726,  -726,  -726,   482,  -726,
    -726,  -726,  -726,  -718,  -726,  -726,  -726,  -726,  -726,   399,
    -726,  -726,  -726,  -726,  -586,  -656,  -726,   886,  -726,  -726,
     598,  -726,   595,  -693,  -726,   815,  -726,  -726,   533,  -726,
     532,   895,  -726,   828,  -726,  -726,   539,  -726,   538,   832,
    -726,   740,  -726,  -726,   472,  -726,   537,  -726,  -726,  -726,
    -726,    99,  -726,    97,  -726,  -726,  -726,  -726,    91,  -726,
      89,  -726,  -726,  -726,  -726,  -726,  -726,   432,  -726,   431,
    -726,  -726,   304,  -726,  -632,  -652,  -726,  -479,  -726,  -726,
    -726,  -726,  -531,  -726,  -726,  -726,   372,  -726,  -726,  -726,
    -726,   368,  -726,   365,  -726,  -726,   250,  -726,  -674,  -725,
    -726,  -703,  -726,  -726,  -726,  -726,  -594,  -726,  -726,  -726,
     319,  -726,  -726,  -726,  -726,  -726,  -534,  -117,  -726,  -726,
     -55,  -726,  -726,  -726,  -726,  -726,  -726,  -153,  -726,  -490,
    -726,  -726,  -726,  -726,  -726,  -726,  -726,  -726,   663,  -726,
     743,  -726,  -726,  -726,  -726,  -726,  -726,  -726,  -726,  -726,
    -141,  -726,  -726,  -726,   548,  -726,  -224,  -726,  -726,  -726,
     483,  -726,  -726,   485,   323,   286,   240,  -726,  -726,  -101,
    -702
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -360
static const short yytable[] =
{
     194,   168,   597,   564,   241,   311,   229,   313,   705,   231,
     778,   677,   662,   209,   193,   403,   760,   130,   735,   721,
    -302,  -302,   768,   595,   407,   718,    48,    73,    42,   795,
     765,     4,     5,     6,     7,  -302,   736,   675,   479,     3,
     535,   679,   224,   164,   165,   166,   167,    49,   747,   607,
     331,     8,   676,   734,   737,   596,   536,  -285,   631,   332,
     692,   141,   142,    43,   808,   608,    74,   630,    75,   643,
     681,   803,   670,   596,   651,   820,   192,     9,   272,    10,
      11,    12,    13,   131,   759,    71,   596,   761,   834,   793,
     835,   270,    71,   271,   297,   273,   408,   826,   827,   300,
     238,   240,   813,   836,   596,   815,  -295,   799,   828,   800,
     480,   680,   299,   682,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   523,   841,    71,   821,   776,
      71,   211,   779,    71,   716,   596,   430,   719,   596,   697,
     437,   698,   439,   728,   216,   302,   837,   328,   338,   733,
     738,   852,   546,    76,   158,   370,   160,   678,    46,   374,
     386,   376,   159,   161,  -352,  -352,   379,   734,   380,   758,
     339,   676,   751,   807,    55,   367,   323,   708,   536,   608,
      86,    87,    88,    89,   324,    90,    91,    92,    93,    94,
      95,    52,    77,   369,    78,    80,   757,   212,   782,   783,
     784,   785,   786,   787,   788,    83,   383,   830,   699,    71,
     217,   303,   838,    71,    96,    97,   727,   853,   752,   637,
     811,   822,   638,   381,   823,   650,    98,   398,   596,   812,
      99,   728,   840,    44,    81,   405,    82,   100,   315,   190,
     191,    62,   316,   790,    84,   851,    85,   670,   596,    56,
    -345,   101,   596,    59,   384,   789,   385,   596,    45,  -335,
     842,   387,   436,   843,   438,   727,   440,    86,    87,    88,
      89,    61,    90,    91,   394,   395,    94,    95,    47,   596,
      50,  -295,   329,   330,    51,    86,    87,    88,    89,    66,
      90,    91,   485,    67,    94,    95,    70,   596,   -47,  -345,
     -37,    96,    97,   559,   560,   -53,   561,    79,   562,   563,
     355,   356,   865,    98,   392,   393,    71,    99,   115,    96,
      97,   399,   400,   118,   100,   877,   186,   187,   188,   189,
     511,    98,   420,   421,   117,    99,   423,   424,   101,   237,
     461,   462,   100,    86,    87,    88,    89,   121,    90,    91,
      92,    93,    94,    95,   465,   466,   101,   239,   516,   517,
     122,   181,   182,   557,   125,   547,   184,   185,   186,   187,
     188,   189,   568,   569,   570,   571,   132,    96,    97,   512,
     143,   558,    86,    87,    88,    89,   144,    90,    91,    98,
     148,    94,    95,    99,   495,   519,   520,   137,   574,   575,
     100,   530,   531,   582,   517,   496,   497,   184,   185,   186,
     187,   188,   189,   146,   101,   149,    96,    97,   151,   498,
     499,   500,   501,   502,   503,   504,   583,   520,    98,   585,
     586,   153,    99,   588,   589,   603,   178,   179,   180,   100,
     181,   182,   599,   600,   618,   184,   185,   186,   187,   188,
     189,   628,   586,   101,   629,   589,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   154,   178,   179,   180,   636,
     181,   182,   195,   183,   495,   184,   185,   186,   187,   188,
     189,   632,   197,   319,   196,   496,   497,   653,   654,   754,
     755,   320,   448,   449,   450,   451,   452,   453,   818,   819,
     499,   500,   501,   502,   503,   504,   866,   867,   878,   879,
     669,   623,   155,   156,   199,   157,   200,   162,   163,   203,
     663,   204,   213,   218,   233,   225,   226,   228,   245,    89,
     230,   232,   242,   243,   244,   268,   683,   269,   706,   275,
     274,   280,   281,   284,   285,   710,   292,   288,   304,   717,
     702,   293,  -359,   308,   307,   317,   325,   189,   327,   340,
     729,   345,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   722,   178,   179,   180,   346,   181,   182,   347,   183,
     326,   184,   185,   186,   187,   188,   189,   348,   351,   357,
     358,   362,   746,   363,   372,   373,   390,   766,   391,   409,
     412,   413,   425,   770,   375,   426,   388,   777,   377,   762,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   419,
     178,   179,   180,   797,   181,   182,   431,   183,   441,   184,
     185,   186,   187,   188,   189,   442,   447,   321,   443,   455,
     798,   458,   464,   469,   473,   322,   476,   475,   481,   490,
     492,   509,   802,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   493,   178,   179,   180,   510,   181,   182,   513,
     183,   514,   184,   185,   186,   187,   188,   189,   524,   832,
     444,   515,   518,   527,   532,   545,   548,   552,   445,   554,
     573,   555,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   566,   178,   179,   180,   578,   181,   182,   584,   183,
     848,   184,   185,   186,   187,   188,   189,   580,   587,   576,
     592,   601,   602,   617,   604,   861,   619,   577,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   620,   178,   179,
     180,   621,   181,   182,   624,   183,   626,   184,   185,   186,
     187,   188,   189,   633,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   246,   178,   179,   180,  -284,   181,   182,
     645,   183,   647,   184,   185,   186,   187,   188,   189,   655,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   446,
     178,   179,   180,   656,   181,   182,   658,   183,   661,   184,
     185,   186,   187,   188,   189,   665,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   660,   178,   179,   180,   666,
     181,   182,   667,   183,   668,   184,   185,   186,   187,   188,
     189,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     672,   178,   179,   180,   674,   181,   182,   684,  -334,   687,
     184,   185,   186,   187,   188,   189,   169,   170,   171,   172,
     173,   174,   694,   700,   703,   536,   178,   179,   180,  -294,
     181,   182,   711,   713,   715,   184,   185,   186,   187,   188,
     189,   720,   724,   725,   726,   730,   732,   740,   742,   743,
     741,   750,   753,   763,  -344,   608,   771,   792,   806,   773,
     775,   781,   810,   809,   817,   814,   816,   833,   839,   846,
     850,   847,   854,   855,   856,   860,   863,   596,   873,   868,
     869,   223,    60,   129,   389,   208,   463,   454,   581,   128,
     521,   207,   522,   579,   590,   291,   591,   625,   627,   605,
     136,   417,   416,   533,   222,   487,   488,   484,   133,   486,
     305,   556,   215,   874,   875,   491,   593,   880,   881,   598,
     219,   709,   644,   693,   648,   652,   769,   378,   318,   780,
     696,   494,   739,   567,     0,     0,     0,     0,     0,   572
};

static const short yycheck[] =
{
     117,   102,   536,   493,   157,   229,   147,   231,   664,   150,
     735,   643,    22,   130,   115,   344,   718,    21,   692,    22,
      16,    17,   725,    68,    15,   677,    34,    56,    34,   747,
     723,     3,     4,     5,     6,    31,    17,    16,    15,     0,
      18,    68,   143,    98,    99,   100,   101,    55,   704,    18,
      34,    23,    31,    34,    68,   100,    34,   102,   592,    43,
     654,    95,    96,    34,   767,    34,    95,    22,    97,   600,
      22,   764,    68,   100,   608,   793,    34,    49,   195,    51,
      52,    53,    54,    87,   716,    95,   100,   719,   813,   745,
     815,   192,    95,   194,   211,   196,    87,   799,   800,   216,
     155,   156,   776,   821,   100,   779,   102,   759,   801,   761,
      87,   645,   213,   647,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   464,   829,    95,   794,   733,
      95,    21,   736,    95,   675,   100,   370,   678,   100,    32,
     374,    34,   376,   687,    21,    21,    21,   274,    34,    16,
     694,    21,    34,    56,    34,   306,    34,    17,    50,   310,
     323,   312,    42,    41,    16,    17,    32,    34,    34,   713,
     281,    31,    28,    28,    97,   302,    96,   666,    34,    34,
      32,    33,    34,    35,   104,    37,    38,    39,    40,    41,
      42,    31,    95,   304,    97,    56,    68,    87,    32,    33,
      34,    35,    36,    37,    38,    56,   317,   803,   101,    95,
      87,    87,    87,    95,    66,    67,    68,    87,   707,    31,
      68,     9,    34,    89,    12,    68,    78,   338,   100,   773,
      82,   775,   828,    34,    95,   346,    97,    89,    95,    32,
      33,    30,    99,   743,    95,   841,    97,    68,   100,    29,
     102,   103,   100,    97,   319,    89,   321,   100,    34,   102,
       9,   326,   373,    12,   375,    68,   377,    32,    33,    34,
      35,    97,    37,    38,    13,    14,    41,    42,    34,   100,
      34,   102,    95,    96,    34,    32,    33,    34,    35,    97,
      37,    38,   419,    35,    41,    42,    87,   100,    97,   102,
      97,    66,    67,    32,    33,    97,    35,    34,    37,    38,
      95,    96,   856,    78,    95,    96,    95,    82,    26,    66,
      67,    95,    96,    34,    89,   869,    90,    91,    92,    93,
     441,    78,    95,    96,    31,    82,    95,    96,   103,   104,
      13,    14,    89,    32,    33,    34,    35,    87,    37,    38,
      39,    40,    41,    42,    13,    14,   103,   104,    95,    96,
       7,    83,    84,   490,    29,   476,    88,    89,    90,    91,
      92,    93,    60,    61,    62,    63,     7,    66,    67,   444,
      34,   492,    32,    33,    34,    35,    34,    37,    38,    78,
      95,    41,    42,    82,    34,    95,    96,    30,   509,   510,
      89,    13,    14,    95,    96,    45,    46,    88,    89,    90,
      91,    92,    93,    34,   103,    34,    66,    67,    34,    59,
      60,    61,    62,    63,    64,    65,    95,    96,    78,    95,
      96,    34,    82,    95,    96,   546,    79,    80,    81,    89,
      83,    84,    16,    17,   555,    88,    89,    90,    91,    92,
      93,    95,    96,   103,    95,    96,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    34,    79,    80,    81,   596,
      83,    84,    99,    86,    34,    88,    89,    90,    91,    92,
      93,   592,    34,    96,    98,    45,    46,    16,    17,    95,
      96,   104,    43,    44,    45,    46,    47,    48,    32,    33,
      60,    61,    62,    63,    64,    65,    95,    96,    95,    96,
     637,   576,   103,   103,    87,   103,     8,   103,   103,    87,
     631,     7,    27,     7,    34,    97,    58,    97,   104,    35,
      97,    97,    34,    34,    34,    34,   647,    34,   665,    34,
      99,    87,    24,    87,     8,   672,    87,    10,    98,   676,
     661,    34,    97,    57,    87,    98,   104,    93,    95,    34,
     687,    87,    69,    70,    71,    72,    73,    74,    75,    76,
      77,   682,    79,    80,    81,    98,    83,    84,    87,    86,
      87,    88,    89,    90,    91,    92,    93,    11,    10,    87,
      34,    87,   703,    34,    87,    98,    87,   724,    34,    12,
      87,    11,    87,   730,    98,    34,    95,   734,    98,   720,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    99,
      79,    80,    81,   750,    83,    84,    34,    86,    98,    88,
      89,    90,    91,    92,    93,    32,   104,    96,    95,    34,
     751,    34,    95,    34,    87,   104,    25,    87,    12,    99,
      98,    98,   763,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    99,    79,    80,    81,    98,    83,    84,    44,
      86,    43,    88,    89,    90,    91,    92,    93,    34,   806,
      96,   100,   100,    34,    87,    35,    34,    87,   104,    87,
      34,    98,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    90,    79,    80,    81,   101,    83,    84,   100,    86,
     837,    88,    89,    90,    91,    92,    93,   101,   100,    96,
      34,   102,    95,    35,    87,   852,    95,   104,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    95,    79,    80,
      81,    34,    83,    84,   101,    86,   101,    88,    89,    90,
      91,    92,    93,    34,    69,    70,    71,    72,    73,    74,
      75,    76,    77,   104,    79,    80,    81,   102,    83,    84,
      34,    86,    34,    88,    89,    90,    91,    92,    93,   102,
      69,    70,    71,    72,    73,    74,    75,    76,    77,   104,
      79,    80,    81,    95,    83,    84,   100,    86,    34,    88,
      89,    90,    91,    92,    93,    99,    69,    70,    71,    72,
      73,    74,    75,    76,    77,   104,    79,    80,    81,    20,
      83,    84,    96,    86,   101,    88,    89,    90,    91,    92,
      93,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      21,    79,    80,    81,   102,    83,    84,    34,   102,    34,
      88,    89,    90,    91,    92,    93,    69,    70,    71,    72,
      73,    74,    34,    54,    34,    34,    79,    80,    81,   102,
      83,    84,    12,    34,    34,    88,    89,    90,    91,    92,
      93,    34,    99,    20,    96,    21,   102,   101,   103,    99,
     101,    19,    35,    34,   102,    34,    12,    95,    19,    34,
      34,    34,    95,    35,   104,    95,    95,    95,    87,    95,
      35,    95,    87,    87,    34,    95,    35,   100,    95,    87,
      34,   142,    25,    61,   330,   129,   400,   393,   520,    60,
     461,   128,   462,   517,   530,   208,   531,   586,   589,   550,
      64,   356,   354,   471,   139,   422,   424,   418,    63,   421,
     220,   489,   134,   864,   867,   428,   534,   876,   879,   538,
     138,   667,   600,   654,   606,   610,   726,   314,   235,   739,
     657,   433,   696,   498,    -1,    -1,    -1,    -1,    -1,   506
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,   106,   107,     0,     3,     4,     5,     6,    23,    49,
      51,    52,    53,    54,   108,   109,   113,   114,   118,   119,
     122,   123,   125,   126,   127,   128,   130,   131,   296,   297,
     299,   300,   315,   316,   317,   318,   319,   320,   326,   329,
     332,   110,    34,    34,    34,    34,    50,    34,    34,    55,
      34,    34,    31,   115,   116,    97,    29,   132,   133,    97,
     132,    97,    30,   200,   201,   202,    97,    35,   111,   112,
      87,    95,   354,    56,    95,    97,    56,    95,    97,    34,
      56,    95,    97,    56,    95,    97,    32,    33,    34,    35,
      37,    38,    39,    40,    41,    42,    66,    67,    78,    82,
      89,   103,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,    26,   117,    31,    34,   120,
     121,    87,     7,   148,   149,    29,   138,   139,   148,   138,
      21,    87,     7,   216,   217,   218,   202,    30,   208,   209,
     210,    95,    96,    34,    34,   327,    34,   330,    95,    34,
     333,    34,   321,    34,    34,   103,   103,   103,    34,    42,
      34,    41,   103,   103,   305,   305,   305,   305,   354,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    79,    80,
      81,    83,    84,    86,    88,    89,    90,    91,    92,    93,
      32,    33,    34,   354,   302,    99,    98,    34,   134,    87,
       8,   160,   161,    87,     7,   153,   154,   160,   153,   302,
     203,    21,    87,    27,   298,   218,    21,    87,     7,   224,
     225,   226,   210,   112,   354,    97,    58,   335,    97,   335,
      97,   335,    97,    34,   323,   324,   325,   104,   305,   104,
     305,   312,    34,    34,    34,   104,   104,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,    34,    34,
     354,   354,   302,   354,    99,    34,   135,   136,   137,   150,
      87,    24,   124,   140,    87,     8,   169,   170,    10,   178,
     179,   169,    87,    34,   205,   206,   207,   302,   219,   354,
     302,   211,    21,    87,    98,   226,   328,    87,    57,   341,
     331,   341,   334,   341,   322,    95,    99,    98,   325,    96,
     104,    96,   104,    96,   104,   104,    87,    95,   302,    95,
      96,    34,    43,   151,   152,   158,   159,   162,    34,   354,
      34,   141,   142,   143,   155,    87,    98,    87,    11,   248,
     249,    10,   189,   190,   204,    95,    96,    87,    34,   221,
     222,   223,    87,    34,   213,   214,   215,   302,   227,   354,
     335,   336,    87,    98,   335,    98,   335,    98,   323,    32,
      34,    89,   353,   354,   305,   305,   312,   305,    95,   137,
      87,    34,    95,    96,    13,    14,   163,   164,   354,    95,
      96,   156,   157,   159,   171,   354,   180,    15,    87,    12,
     144,   145,    87,    11,   272,   273,   205,   207,   220,    99,
      95,    96,   212,    95,    96,    87,    34,   229,   230,   231,
     341,    34,   337,   338,   339,   342,   354,   341,   354,   341,
     354,    98,    32,    95,    96,   104,   104,   104,    43,    44,
      45,    46,    47,    48,   158,    34,   165,   166,    34,   167,
     168,    13,    14,   143,    95,    13,    14,   172,   173,    34,
     181,   182,   183,    87,   251,    87,    25,   129,   191,    15,
      87,    12,   146,   147,   221,   302,   223,   213,   215,   228,
      99,   231,    98,    99,   339,    34,    45,    46,    59,    60,
      61,    62,    63,    64,    65,   343,   344,   345,   348,    98,
      98,   354,   305,    44,    43,   100,    95,    96,   100,    95,
      96,   165,   167,   159,    34,   174,   175,    34,   176,   177,
      13,    14,    87,   183,   250,    18,    34,   252,   253,   254,
     255,   262,   263,   264,   266,    35,    34,   354,    34,   192,
     193,   194,    87,   275,    87,    98,   229,   302,   354,    32,
      33,    35,    37,    38,   314,   340,    90,   348,    60,    61,
      62,    63,   345,    34,   354,   354,    96,   104,   101,   166,
     101,   168,    95,    95,   100,    95,    96,   100,    95,    96,
     174,   176,    34,   252,   256,    68,   100,   301,   254,    16,
      17,   102,    95,   354,    87,   194,   274,    18,    34,   276,
     277,   278,   279,   286,   287,   288,   290,    35,   354,    95,
      95,    34,   346,   305,   101,   175,   101,   177,    95,    95,
      22,   301,   354,    34,   257,   258,   302,    31,    34,   267,
     268,   270,   271,   267,   271,    34,   265,    34,   276,   280,
      68,   301,   278,    16,    17,   102,    95,   347,   100,   349,
     104,    34,    22,   354,   184,    99,    20,    96,   101,   302,
      68,   301,    21,   259,   102,    16,    31,   259,    17,    68,
     301,    22,   301,   354,    34,   281,   282,    34,   291,   292,
     294,   295,   291,   295,    34,   289,   349,    32,    34,   101,
      54,   350,   354,    34,   185,   200,   302,   261,   262,   257,
     302,    12,   260,    34,   269,    34,   267,   302,   260,   267,
      34,    22,   354,   195,    99,    20,    96,    68,   301,   302,
      21,   283,   102,    16,    34,   283,    17,    68,   301,   350,
     101,   101,   103,    99,   351,   186,   354,   200,   188,   216,
      19,    28,   262,    35,    95,    96,   355,    68,   301,   259,
     355,   259,   354,    34,   196,   208,   302,   285,   286,   281,
     302,    12,   284,    34,   293,    34,   291,   302,   284,   291,
     351,    34,    32,    33,    34,    35,    36,    37,    38,    89,
     314,   352,    95,   200,   187,   188,   232,   302,   354,   260,
     260,   197,   354,   208,   199,   224,    19,    28,   286,    35,
      95,    68,   301,   283,    95,   283,    95,   104,    32,    33,
     188,   200,     9,    12,   233,   246,   355,   355,   208,   198,
     199,   239,   302,    95,   284,   284,   188,    21,    87,    87,
     199,   208,     9,    12,   240,   247,    95,    95,   302,   234,
      35,   199,    21,    87,    87,    87,    34,   236,   237,   238,
      95,   302,   241,    35,   235,   301,    95,    96,    87,    34,
     243,   244,   245,    95,   236,   238,   242,   301,    95,    96,
     243,   245
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
        case 6:
#line 251 "ned.y"
    { if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); }
    break;

  case 7:
#line 253 "ned.y"
    { if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); }
    break;

  case 8:
#line 255 "ned.y"
    { if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); }
    break;

  case 9:
#line 257 "ned.y"
    { if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); }
    break;

  case 10:
#line 260 "ned.y"
    { if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); }
    break;

  case 11:
#line 262 "ned.y"
    { if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); }
    break;

  case 12:
#line 264 "ned.y"
    { if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); }
    break;

  case 13:
#line 266 "ned.y"
    { if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); }
    break;

  case 19:
#line 275 "ned.y"
    { if (ps.storeSourceCode) ps.enump->setSourceCode(toString(yylsp[0])); }
    break;

  case 20:
#line 277 "ned.y"
    { if (ps.storeSourceCode) ps.messagep->setSourceCode(toString(yylsp[0])); }
    break;

  case 21:
#line 279 "ned.y"
    { if (ps.storeSourceCode) ps.classp->setSourceCode(toString(yylsp[0])); }
    break;

  case 22:
#line 281 "ned.y"
    { if (ps.storeSourceCode) ps.structp->setSourceCode(toString(yylsp[0])); }
    break;

  case 23:
#line 289 "ned.y"
    {
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                }
    break;

  case 24:
#line 294 "ned.y"
    {
                  // NOTE: no setTrailingComment(ps.imports,@3) -- comment already stored with last filename */
                }
    break;

  case 27:
#line 306 "ned.y"
    {
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                }
    break;

  case 29:
#line 322 "ned.y"
    {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                }
    break;

  case 32:
#line 336 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                }
    break;

  case 33:
#line 342 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                }
    break;

  case 34:
#line 351 "ned.y"
    {
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;

  case 35:
#line 355 "ned.y"
    {
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;

  case 36:
#line 367 "ned.y"
    {
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;

  case 37:
#line 374 "ned.y"
    {
                  use_chanattrname_token = 0;
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                }
    break;

  case 40:
#line 389 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                }
    break;

  case 41:
#line 395 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                }
    break;

  case 43:
#line 415 "ned.y"
    {
                  use_chanattrname_token = 1;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 44:
#line 425 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 45:
#line 429 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 46:
#line 443 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 47:
#line 450 "ned.y"
    {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 49:
#line 474 "ned.y"
    {
                  use_chanattrname_token = 1;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 50:
#line 484 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 51:
#line 488 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 52:
#line 505 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 53:
#line 512 "ned.y"
    {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 56:
#line 530 "ned.y"
    {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                }
    break;

  case 57:
#line 535 "ned.y"
    {
                }
    break;

  case 62:
#line 551 "ned.y"
    {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                }
    break;

  case 65:
#line 568 "ned.y"
    {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                }
    break;

  case 66:
#line 573 "ned.y"
    {
                }
    break;

  case 71:
#line 589 "ned.y"
    {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                }
    break;

  case 74:
#line 606 "ned.y"
    {
                  addDisplayString(ps.module,yylsp[-1]);
                }
    break;

  case 77:
#line 621 "ned.y"
    {
                  addDisplayString(ps.module,yylsp[-1]);
                }
    break;

  case 80:
#line 636 "ned.y"
    {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                }
    break;

  case 81:
#line 641 "ned.y"
    {
                }
    break;

  case 84:
#line 652 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 85:
#line 656 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 88:
#line 671 "ned.y"
    {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                }
    break;

  case 89:
#line 676 "ned.y"
    {
                }
    break;

  case 92:
#line 687 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 93:
#line 691 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 95:
#line 706 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                }
    break;

  case 96:
#line 710 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                }
    break;

  case 97:
#line 714 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                }
    break;

  case 98:
#line 718 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                }
    break;

  case 99:
#line 722 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                }
    break;

  case 100:
#line 726 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                }
    break;

  case 101:
#line 730 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                }
    break;

  case 102:
#line 734 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                }
    break;

  case 103:
#line 738 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_XML);
                }
    break;

  case 104:
#line 742 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                }
    break;

  case 107:
#line 757 "ned.y"
    {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                }
    break;

  case 108:
#line 762 "ned.y"
    {
                }
    break;

  case 117:
#line 785 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 118:
#line 790 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 121:
#line 803 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 122:
#line 808 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 125:
#line 824 "ned.y"
    {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                }
    break;

  case 126:
#line 829 "ned.y"
    {
                }
    break;

  case 135:
#line 852 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 136:
#line 857 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 139:
#line 870 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 140:
#line 875 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 143:
#line 891 "ned.y"
    {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                }
    break;

  case 144:
#line 896 "ned.y"
    {
                }
    break;

  case 149:
#line 912 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                }
    break;

  case 150:
#line 918 "ned.y"
    {
                }
    break;

  case 151:
#line 921 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                }
    break;

  case 152:
#line 928 "ned.y"
    {
                }
    break;

  case 153:
#line 931 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                }
    break;

  case 154:
#line 937 "ned.y"
    {
                }
    break;

  case 155:
#line 940 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                }
    break;

  case 156:
#line 947 "ned.y"
    {
                }
    break;

  case 160:
#line 967 "ned.y"
    {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                }
    break;

  case 161:
#line 972 "ned.y"
    {
                }
    break;

  case 166:
#line 988 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                }
    break;

  case 167:
#line 994 "ned.y"
    {
                }
    break;

  case 168:
#line 997 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                }
    break;

  case 169:
#line 1004 "ned.y"
    {
                }
    break;

  case 170:
#line 1007 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                }
    break;

  case 171:
#line 1013 "ned.y"
    {
                }
    break;

  case 172:
#line 1016 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                }
    break;

  case 173:
#line 1023 "ned.y"
    {
                }
    break;

  case 179:
#line 1048 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                }
    break;

  case 180:
#line 1053 "ned.y"
    {
                }
    break;

  case 181:
#line 1056 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                }
    break;

  case 182:
#line 1062 "ned.y"
    {
                }
    break;

  case 187:
#line 1078 "ned.y"
    {
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                }
    break;

  case 192:
#line 1099 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                }
    break;

  case 193:
#line 1104 "ned.y"
    {
                }
    break;

  case 194:
#line 1107 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                }
    break;

  case 195:
#line 1113 "ned.y"
    {
                }
    break;

  case 200:
#line 1129 "ned.y"
    {
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                }
    break;

  case 205:
#line 1150 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                }
    break;

  case 206:
#line 1155 "ned.y"
    {
                }
    break;

  case 207:
#line 1158 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                }
    break;

  case 208:
#line 1164 "ned.y"
    {
                }
    break;

  case 213:
#line 1181 "ned.y"
    {
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                }
    break;

  case 218:
#line 1203 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                }
    break;

  case 219:
#line 1208 "ned.y"
    {
                }
    break;

  case 220:
#line 1211 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                }
    break;

  case 221:
#line 1217 "ned.y"
    {
                }
    break;

  case 226:
#line 1234 "ned.y"
    {
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-3]);
                  addExpression(ps.substparam, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparam,yylsp[-3],yylsp[-1]);
                }
    break;

  case 229:
#line 1251 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                }
    break;

  case 230:
#line 1256 "ned.y"
    {
                }
    break;

  case 231:
#line 1259 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                }
    break;

  case 232:
#line 1265 "ned.y"
    {
                }
    break;

  case 237:
#line 1281 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                }
    break;

  case 238:
#line 1288 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                }
    break;

  case 241:
#line 1304 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                }
    break;

  case 242:
#line 1309 "ned.y"
    {
                }
    break;

  case 243:
#line 1312 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                }
    break;

  case 244:
#line 1318 "ned.y"
    {
                }
    break;

  case 249:
#line 1334 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                }
    break;

  case 250:
#line 1341 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                }
    break;

  case 251:
#line 1352 "ned.y"
    {
                  addDisplayString(ps.submod,yylsp[-1]);
                }
    break;

  case 253:
#line 1363 "ned.y"
    {
                  addDisplayString(ps.submod,yylsp[-1]);
                }
    break;

  case 257:
#line 1379 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                }
    break;

  case 258:
#line 1385 "ned.y"
    {
                }
    break;

  case 259:
#line 1388 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                }
    break;

  case 260:
#line 1394 "ned.y"
    {
                }
    break;

  case 267:
#line 1415 "ned.y"
    {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                }
    break;

  case 268:
#line 1420 "ned.y"
    {
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                }
    break;

  case 271:
#line 1434 "ned.y"
    {
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                }
    break;

  case 272:
#line 1444 "ned.y"
    {
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                }
    break;

  case 274:
#line 1452 "ned.y"
    {
                  addDisplayString(ps.conn,yylsp[0]);
                }
    break;

  case 278:
#line 1465 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 279:
#line 1470 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 280:
#line 1475 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 281:
#line 1481 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 284:
#line 1495 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 285:
#line 1501 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                }
    break;

  case 286:
#line 1509 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 287:
#line 1514 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                }
    break;

  case 288:
#line 1518 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                }
    break;

  case 289:
#line 1526 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 290:
#line 1533 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                }
    break;

  case 291:
#line 1539 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                }
    break;

  case 294:
#line 1554 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 295:
#line 1559 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[0]) );
                }
    break;

  case 296:
#line 1566 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 297:
#line 1571 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 298:
#line 1575 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                }
    break;

  case 299:
#line 1583 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 300:
#line 1588 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 301:
#line 1592 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                }
    break;

  case 302:
#line 1601 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                }
    break;

  case 303:
#line 1606 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 304:
#line 1611 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 307:
#line 1627 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                }
    break;

  case 308:
#line 1633 "ned.y"
    {
                }
    break;

  case 309:
#line 1636 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                }
    break;

  case 310:
#line 1642 "ned.y"
    {
                }
    break;

  case 317:
#line 1663 "ned.y"
    {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                }
    break;

  case 318:
#line 1668 "ned.y"
    {
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                }
    break;

  case 321:
#line 1682 "ned.y"
    {
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                }
    break;

  case 322:
#line 1692 "ned.y"
    {
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                }
    break;

  case 324:
#line 1700 "ned.y"
    {
                  addDisplayString(ps.conn,yylsp[0]);
                }
    break;

  case 328:
#line 1713 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 329:
#line 1718 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 330:
#line 1723 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 331:
#line 1729 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 334:
#line 1743 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 335:
#line 1749 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                }
    break;

  case 336:
#line 1757 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 337:
#line 1762 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                }
    break;

  case 338:
#line 1766 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                }
    break;

  case 339:
#line 1774 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 340:
#line 1781 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                }
    break;

  case 341:
#line 1787 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                }
    break;

  case 344:
#line 1802 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 345:
#line 1807 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[0]) );
                }
    break;

  case 346:
#line 1814 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 347:
#line 1819 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 348:
#line 1823 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                }
    break;

  case 349:
#line 1831 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 350:
#line 1836 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 351:
#line 1840 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                }
    break;

  case 352:
#line 1849 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                }
    break;

  case 353:
#line 1854 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 354:
#line 1859 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 356:
#line 1877 "ned.y"
    {
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                }
    break;

  case 357:
#line 1886 "ned.y"
    {
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                }
    break;

  case 358:
#line 1900 "ned.y"
    {
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                }
    break;

  case 359:
#line 1908 "ned.y"
    {
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                }
    break;

  case 360:
#line 1921 "ned.y"
    { yyval = yyvsp[-1]; }
    break;

  case 361:
#line 1927 "ned.y"
    {
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
    break;

  case 362:
#line 1931 "ned.y"
    {
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
    break;

  case 363:
#line 1935 "ned.y"
    {
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
    break;

  case 364:
#line 1946 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); }
    break;

  case 365:
#line 1948 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); }
    break;

  case 366:
#line 1950 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input"); }
    break;

  case 367:
#line 1952 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input"); }
    break;

  case 368:
#line 1957 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-3], yyvsp[-1]); }
    break;

  case 369:
#line 1959 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-1]); }
    break;

  case 371:
#line 1965 "ned.y"
    { yyval = yyvsp[-1]; }
    break;

  case 372:
#line 1968 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); }
    break;

  case 373:
#line 1970 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); }
    break;

  case 374:
#line 1972 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); }
    break;

  case 375:
#line 1974 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); }
    break;

  case 376:
#line 1976 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); }
    break;

  case 377:
#line 1978 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); }
    break;

  case 378:
#line 1982 "ned.y"
    { if (ps.parseExpressions) yyval = unaryMinus(yyvsp[0]); }
    break;

  case 379:
#line 1985 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); }
    break;

  case 380:
#line 1987 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); }
    break;

  case 381:
#line 1989 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); }
    break;

  case 382:
#line 1991 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); }
    break;

  case 383:
#line 1993 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); }
    break;

  case 384:
#line 1995 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); }
    break;

  case 385:
#line 1998 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); }
    break;

  case 386:
#line 2000 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); }
    break;

  case 387:
#line 2002 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); }
    break;

  case 388:
#line 2006 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); }
    break;

  case 389:
#line 2009 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); }
    break;

  case 390:
#line 2011 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); }
    break;

  case 391:
#line 2013 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); }
    break;

  case 392:
#line 2017 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); }
    break;

  case 393:
#line 2019 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); }
    break;

  case 394:
#line 2021 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); }
    break;

  case 395:
#line 2023 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); }
    break;

  case 396:
#line 2026 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); }
    break;

  case 397:
#line 2028 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); }
    break;

  case 398:
#line 2030 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); }
    break;

  case 399:
#line 2032 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); }
    break;

  case 400:
#line 2034 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-9]), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); }
    break;

  case 406:
#line 2047 "ned.y"
    {
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                }
    break;

  case 407:
#line 2052 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                }
    break;

  case 408:
#line 2057 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                }
    break;

  case 409:
#line 2063 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                }
    break;

  case 410:
#line 2069 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                }
    break;

  case 412:
#line 2081 "ned.y"
    { yyval = createConst(NED_CONST_BOOL, "true"); }
    break;

  case 413:
#line 2083 "ned.y"
    { yyval = createConst(NED_CONST_BOOL, "false"); }
    break;

  case 415:
#line 2092 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("index"); }
    break;

  case 416:
#line 2094 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("index"); }
    break;

  case 417:
#line 2096 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); }
    break;

  case 418:
#line 2101 "ned.y"
    { yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); }
    break;

  case 419:
#line 2106 "ned.y"
    { yyval = createConst(NED_CONST_INT, toString(yylsp[0])); }
    break;

  case 420:
#line 2108 "ned.y"
    { yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); }
    break;

  case 421:
#line 2110 "ned.y"
    { yyval = createTimeConst(toString(yylsp[0])); }
    break;

  case 426:
#line 2127 "ned.y"
    {
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(yylsp[-1])));
                  setComments(ps.cplusplus,yylsp[-2],yylsp[-1]);
                }
    break;

  case 427:
#line 2136 "ned.y"
    {
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(yylsp[-1]));
                  setComments(ps.structdecl,yylsp[-2],yylsp[-1]);
                }
    break;

  case 428:
#line 2145 "ned.y"
    {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(yylsp[-1]));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,yylsp[-2],yylsp[-1]);
                }
    break;

  case 429:
#line 2152 "ned.y"
    {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(yylsp[-1]));
                  setComments(ps.classdecl,yylsp[-3],yylsp[-2]);
                }
    break;

  case 430:
#line 2162 "ned.y"
    {
                  ps.messagedecl = (MessageDeclNode *)createNodeWithTag(NED_MESSAGE_DECL, ps.nedfile );
                  ps.messagedecl->setName(toString(yylsp[-1]));
                  setComments(ps.messagedecl,yylsp[-2],yylsp[-1]);
                }
    break;

  case 431:
#line 2171 "ned.y"
    {
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString(yylsp[-1]));
                  setComments(ps.enumdecl,yylsp[-2],yylsp[-1]);
                }
    break;

  case 432:
#line 2180 "ned.y"
    {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
    break;

  case 433:
#line 2187 "ned.y"
    {
                  setTrailingComment(ps.enump,yylsp[-1]);
                }
    break;

  case 434:
#line 2191 "ned.y"
    {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
    break;

  case 435:
#line 2199 "ned.y"
    {
                  setTrailingComment(ps.enump,yylsp[-1]);
                }
    break;

  case 440:
#line 2216 "ned.y"
    {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-1],yylsp[-1]);
                }
    break;

  case 441:
#line 2222 "ned.y"
    {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-3],yylsp[-1]);
                }
    break;

  case 442:
#line 2232 "ned.y"
    {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                }
    break;

  case 443:
#line 2238 "ned.y"
    {
                  setTrailingComment(ps.messagep,yylsp[-1]);
                }
    break;

  case 444:
#line 2242 "ned.y"
    {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                }
    break;

  case 445:
#line 2249 "ned.y"
    {
                  setTrailingComment(ps.messagep,yylsp[-1]);
                }
    break;

  case 446:
#line 2256 "ned.y"
    {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                }
    break;

  case 447:
#line 2262 "ned.y"
    {
                  setTrailingComment(ps.classp,yylsp[-1]);
                }
    break;

  case 448:
#line 2266 "ned.y"
    {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                }
    break;

  case 449:
#line 2273 "ned.y"
    {
                  setTrailingComment(ps.classp,yylsp[-1]);
                }
    break;

  case 450:
#line 2280 "ned.y"
    {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                }
    break;

  case 451:
#line 2286 "ned.y"
    {
                  setTrailingComment(ps.structp,yylsp[-1]);
                }
    break;

  case 452:
#line 2290 "ned.y"
    {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                }
    break;

  case 453:
#line 2297 "ned.y"
    {
                  setTrailingComment(ps.structp,yylsp[-1]);
                }
    break;

  case 454:
#line 2304 "ned.y"
    {
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                }
    break;

  case 461:
#line 2324 "ned.y"
    {
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                }
    break;

  case 468:
#line 2343 "ned.y"
    {
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                }
    break;

  case 475:
#line 2363 "ned.y"
    {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                }
    break;

  case 476:
#line 2369 "ned.y"
    {
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                }
    break;

  case 477:
#line 2373 "ned.y"
    {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(true);
                }
    break;

  case 478:
#line 2380 "ned.y"
    {
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                }
    break;

  case 492:
#line 2407 "ned.y"
    {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                }
    break;

  case 493:
#line 2412 "ned.y"
    {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                }
    break;

  case 494:
#line 2417 "ned.y"
    {
                  ps.field->setIsVector(true);
                }
    break;

  case 496:
#line 2425 "ned.y"
    {
                  ps.field->setEnumName(toString(yylsp[-1]));
                }
    break;

  case 498:
#line 2433 "ned.y"
    {
                  ps.field->setDefaultValue(toString(yylsp[0]));
                }
    break;


    }

/* Line 991 of yacc.c.  */
#line 4350 "ned.tab.c"

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


#line 2462 "ned.y"


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
   sprintf(buf,"%lg",t);
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

