/* A Bison parser, made from ned.y, by GNU bison 1.75.  */

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
#line 51 "ned.y"


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
#include <malloc.h>         /* for alloca() */
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
#line 431 "ned.tab.c"

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
#define YYFINAL  3
#define YYLAST   983

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  105
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  250
/* YYNRULES -- Number of rules. */
#define YYNRULES  514
/* YYNRULES -- Number of states. */
#define YYNSTATES  880

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   341

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

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
      39,    41,    43,    44,    49,    53,    55,    57,    61,    64,
      65,    67,    72,    76,    80,    83,    89,    92,    94,    95,
     101,   106,   112,   115,   119,   122,   130,   133,   142,   145,
     149,   152,   163,   166,   168,   169,   170,   175,   178,   179,
     183,   185,   187,   189,   190,   191,   196,   199,   200,   204,
     206,   208,   210,   211,   216,   218,   219,   224,   226,   227,
     228,   233,   236,   237,   241,   243,   245,   246,   247,   252,
     255,   256,   260,   262,   264,   266,   270,   273,   277,   282,
     287,   291,   295,   299,   303,   305,   306,   307,   312,   314,
     315,   320,   324,   329,   333,   337,   339,   343,   345,   349,
     351,   355,   357,   359,   360,   361,   366,   368,   369,   374,
     378,   383,   387,   391,   393,   397,   399,   403,   405,   409,
     411,   413,   414,   415,   420,   422,   423,   426,   428,   429,
     437,   438,   447,   448,   458,   459,   470,   474,   476,   477,
     478,   483,   485,   486,   489,   491,   492,   500,   501,   510,
     511,   521,   522,   533,   537,   539,   540,   543,   545,   546,
     551,   552,   559,   562,   563,   567,   569,   571,   573,   574,
     577,   579,   580,   585,   586,   593,   596,   597,   601,   603,
     605,   607,   608,   611,   613,   614,   619,   620,   627,   630,
     631,   635,   637,   641,   643,   644,   647,   649,   650,   655,
     656,   663,   665,   666,   669,   671,   676,   679,   680,   681,
     686,   687,   694,   697,   698,   702,   704,   707,   709,   712,
     713,   714,   719,   720,   727,   730,   731,   735,   737,   740,
     742,   747,   748,   753,   754,   756,   757,   758,   764,   765,
     770,   772,   773,   776,   778,   780,   782,   783,   791,   795,
     797,   803,   806,   807,   810,   811,   814,   816,   823,   832,
     839,   848,   852,   854,   857,   859,   862,   864,   867,   870,
     872,   875,   879,   881,   884,   886,   889,   891,   894,   897,
     899,   902,   904,   907,   911,   913,   914,   915,   921,   922,
     927,   929,   930,   933,   935,   937,   939,   940,   948,   952,
     954,   960,   963,   964,   967,   968,   971,   973,   980,   989,
     996,  1005,  1009,  1011,  1014,  1016,  1019,  1021,  1024,  1027,
    1029,  1032,  1036,  1038,  1041,  1043,  1046,  1048,  1051,  1054,
    1056,  1059,  1061,  1064,  1068,  1073,  1079,  1082,  1089,  1095,
    1099,  1101,  1103,  1105,  1112,  1117,  1121,  1123,  1130,  1135,
    1137,  1141,  1145,  1149,  1153,  1157,  1161,  1165,  1168,  1172,
    1176,  1180,  1184,  1188,  1192,  1196,  1200,  1204,  1207,  1211,
    1215,  1219,  1222,  1226,  1230,  1236,  1240,  1245,  1252,  1261,
    1272,  1274,  1276,  1278,  1280,  1282,  1284,  1287,  1291,  1295,
    1298,  1300,  1302,  1304,  1306,  1308,  1312,  1317,  1319,  1321,
    1323,  1325,  1329,  1333,  1336,  1339,  1343,  1347,  1351,  1356,
    1360,  1361,  1369,  1370,  1380,  1382,  1383,  1386,  1388,  1391,
    1396,  1397,  1406,  1407,  1418,  1419,  1428,  1429,  1440,  1441,
    1450,  1451,  1462,  1463,  1468,  1469,  1471,  1472,  1475,  1477,
    1482,  1484,  1486,  1488,  1490,  1492,  1494,  1495,  1500,  1501,
    1503,  1504,  1507,  1509,  1510,  1518,  1519,  1528,  1530,  1533,
    1535,  1537,  1539,  1541,  1544,  1547,  1550,  1553,  1555,  1557,
    1559,  1563,  1567,  1570,  1571,  1576,  1577,  1580,  1581,  1583,
    1585,  1587,  1590,  1592,  1595,  1597,  1599,  1601,  1603,  1605,
    1608,  1610,  1612,  1613,  1615
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     106,     0,    -1,   107,    -1,   107,   108,    -1,    -1,   109,
      -1,   118,    -1,   125,    -1,   130,    -1,   299,    -1,   113,
      -1,   122,    -1,   127,    -1,   296,    -1,   315,    -1,   316,
      -1,   317,    -1,   318,    -1,   319,    -1,   325,    -1,   328,
      -1,   331,    -1,    -1,     3,   110,   111,    95,    -1,   111,
      96,   112,    -1,   112,    -1,    35,    -1,   114,   115,   117,
      -1,     5,    34,    -1,    -1,   116,    -1,   116,    31,   302,
     353,    -1,    31,   302,   353,    -1,    26,    34,   353,    -1,
      26,   353,    -1,   119,    97,   120,    98,   353,    -1,     5,
      34,    -1,   121,    -1,    -1,   121,    34,    99,   302,    95,
      -1,    34,    99,   302,    95,    -1,   123,   132,   148,   160,
     124,    -1,     4,    34,    -1,    24,    34,   353,    -1,    24,
     353,    -1,   126,    97,   138,   153,   169,    98,   353,    -1,
       4,    34,    -1,   128,   132,   148,   160,   178,   248,   144,
     129,    -1,     6,    34,    -1,    25,    34,   353,    -1,    25,
     353,    -1,   131,    97,   138,   153,   169,   189,   272,   146,
      98,   353,    -1,     6,    34,    -1,   133,    -1,    -1,    -1,
      29,    87,   134,   135,    -1,   136,    95,    -1,    -1,   136,
      96,   137,    -1,   137,    -1,    34,    -1,   139,    -1,    -1,
      -1,    29,    87,   140,   141,    -1,   142,    95,    -1,    -1,
     142,    96,   143,    -1,   143,    -1,    34,    -1,   145,    -1,
      -1,    12,    87,    35,    95,    -1,   147,    -1,    -1,    12,
      87,    35,    95,    -1,   149,    -1,    -1,    -1,     7,    87,
     150,   151,    -1,   152,    95,    -1,    -1,   152,    96,   158,
      -1,   158,    -1,   154,    -1,    -1,    -1,     7,    87,   155,
     156,    -1,   157,    95,    -1,    -1,   157,    95,   159,    -1,
     159,    -1,   159,    -1,    34,    -1,    34,    87,    44,    -1,
      43,    34,    -1,    34,    87,    43,    -1,    34,    87,    43,
      44,    -1,    34,    87,    44,    43,    -1,    34,    87,    45,
      -1,    34,    87,    46,    -1,    34,    87,    47,    -1,    34,
      87,    48,    -1,   161,    -1,    -1,    -1,     8,    87,   162,
     163,    -1,   164,    -1,    -1,   164,    13,   165,    95,    -1,
      13,   165,    95,    -1,   164,    14,   167,    95,    -1,    14,
     167,    95,    -1,   165,    96,   166,    -1,   166,    -1,    34,
     100,   101,    -1,    34,    -1,   167,    96,   168,    -1,   168,
      -1,    34,   100,   101,    -1,    34,    -1,   170,    -1,    -1,
      -1,     8,    87,   171,   172,    -1,   173,    -1,    -1,   173,
      13,   174,    95,    -1,    13,   174,    95,    -1,   173,    14,
     176,    95,    -1,    14,   176,    95,    -1,   174,    96,   175,
      -1,   175,    -1,    34,   100,   101,    -1,    34,    -1,   176,
      96,   177,    -1,   177,    -1,    34,   100,   101,    -1,    34,
      -1,   179,    -1,    -1,    -1,    10,    87,   180,   181,    -1,
     182,    -1,    -1,   182,   183,    -1,   183,    -1,    -1,    34,
      87,    34,   353,   184,   200,   188,    -1,    -1,    34,    87,
      34,   301,   353,   185,   200,   188,    -1,    -1,    34,    87,
      34,    22,    34,   353,   186,   200,   188,    -1,    -1,    34,
      87,    34,   301,    22,    34,   353,   187,   200,   188,    -1,
     216,   232,   246,    -1,   190,    -1,    -1,    -1,    10,    87,
     191,   192,    -1,   193,    -1,    -1,   193,   194,    -1,   194,
      -1,    -1,    34,    87,    34,   353,   195,   208,   199,    -1,
      -1,    34,    87,    34,   301,   353,   196,   208,   199,    -1,
      -1,    34,    87,    34,    22,    34,   353,   197,   208,   199,
      -1,    -1,    34,    87,    34,   301,    22,    34,   353,   198,
     208,   199,    -1,   224,   239,   247,    -1,   201,    -1,    -1,
     201,   202,    -1,   202,    -1,    -1,    30,    87,   203,   205,
      -1,    -1,    30,    21,   302,    87,   204,   205,    -1,   206,
      95,    -1,    -1,   206,    96,   207,    -1,   207,    -1,    34,
      -1,   209,    -1,    -1,   209,   210,    -1,   210,    -1,    -1,
      30,    87,   211,   213,    -1,    -1,    30,    21,   302,    87,
     212,   213,    -1,   214,    95,    -1,    -1,   214,    96,   215,
      -1,   215,    -1,    34,    -1,   217,    -1,    -1,   217,   218,
      -1,   218,    -1,    -1,     7,    87,   219,   221,    -1,    -1,
       7,    21,   302,    87,   220,   221,    -1,   222,    95,    -1,
      -1,   222,    96,   223,    -1,   223,    -1,    34,    99,   302,
      -1,   225,    -1,    -1,   225,   226,    -1,   226,    -1,    -1,
       7,    87,   227,   229,    -1,    -1,     7,    21,   302,    87,
     228,   229,    -1,   230,    -1,    -1,   230,   231,    -1,   231,
      -1,    34,    99,   302,    95,    -1,   232,   233,    -1,    -1,
      -1,     9,    87,   234,   236,    -1,    -1,     9,    21,   302,
      87,   235,   236,    -1,   237,    95,    -1,    -1,   237,    96,
     238,    -1,   238,    -1,    34,   301,    -1,    34,    -1,   239,
     240,    -1,    -1,    -1,     9,    87,   241,   243,    -1,    -1,
       9,    21,   302,    87,   242,   243,    -1,   244,    95,    -1,
      -1,   244,    96,   245,    -1,   245,    -1,    34,   301,    -1,
      34,    -1,    12,    87,    35,    95,    -1,    -1,    12,    87,
      35,    95,    -1,    -1,   249,    -1,    -1,    -1,    11,    15,
      87,   250,   252,    -1,    -1,    11,    87,   251,   252,    -1,
     253,    -1,    -1,   253,   254,    -1,   254,    -1,   255,    -1,
     262,    -1,    -1,    18,   256,   257,    20,   261,    28,   353,
      -1,   258,    96,   257,    -1,   258,    -1,    34,    99,   302,
      19,   302,    -1,    21,   302,    -1,    -1,    12,    35,    -1,
      -1,   261,   262,    -1,   262,    -1,   263,    17,   267,   259,
     260,   354,    -1,   263,    17,   271,    17,   267,   259,   260,
     354,    -1,   263,    16,   267,   259,   260,   354,    -1,   263,
      16,   271,    16,   267,   259,   260,   354,    -1,   264,   102,
     265,    -1,   266,    -1,    34,   301,    -1,    34,    -1,    34,
     301,    -1,    34,    -1,    34,    68,    -1,    34,   301,    -1,
      34,    -1,    34,    68,    -1,   268,   102,   269,    -1,   270,
      -1,    34,   301,    -1,    34,    -1,    34,   301,    -1,    34,
      -1,    34,    68,    -1,    34,   301,    -1,    34,    -1,    34,
      68,    -1,    34,    -1,    31,   302,    -1,   271,    31,   302,
      -1,   273,    -1,    -1,    -1,    11,    15,    87,   274,   276,
      -1,    -1,    11,    87,   275,   276,    -1,   277,    -1,    -1,
     277,   278,    -1,   278,    -1,   279,    -1,   286,    -1,    -1,
      18,   280,   281,    20,   285,    28,    95,    -1,   282,    96,
     281,    -1,   282,    -1,    34,    99,   302,    19,   302,    -1,
      21,   302,    -1,    -1,    12,    35,    -1,    -1,   285,   286,
      -1,   286,    -1,   287,    17,   291,   283,   284,    95,    -1,
     287,    17,   295,    17,   291,   283,   284,    95,    -1,   287,
      16,   291,   283,   284,    95,    -1,   287,    16,   295,    16,
     291,   283,   284,    95,    -1,   288,   102,   289,    -1,   290,
      -1,    34,   301,    -1,    34,    -1,    34,   301,    -1,    34,
      -1,    34,    68,    -1,    34,   301,    -1,    34,    -1,    34,
      68,    -1,   292,   102,   293,    -1,   294,    -1,    34,   301,
      -1,    34,    -1,    34,   301,    -1,    34,    -1,    34,    68,
      -1,    34,   301,    -1,    34,    -1,    34,    68,    -1,    34,
      -1,    34,   302,    -1,   295,    34,   302,    -1,   297,   200,
     216,   298,    -1,    23,    34,    87,    34,   353,    -1,    27,
     353,    -1,   300,    97,   208,   224,    98,   353,    -1,    23,
      34,    87,    34,   353,    -1,   100,   302,   101,    -1,   305,
      -1,   303,    -1,   304,    -1,    39,   103,   305,    96,   305,
     104,    -1,    39,   103,   305,   104,    -1,    39,   103,   104,
      -1,    39,    -1,    40,   103,   312,    96,   312,   104,    -1,
      40,   103,   312,   104,    -1,   306,    -1,   103,   305,   104,
      -1,   305,    88,   305,    -1,   305,    89,   305,    -1,   305,
      90,   305,    -1,   305,    91,   305,    -1,   305,    92,   305,
      -1,   305,    93,   305,    -1,    89,   305,    -1,   305,    69,
     305,    -1,   305,    70,   305,    -1,   305,    71,   305,    -1,
     305,    72,   305,    -1,   305,    73,   305,    -1,   305,    74,
     305,    -1,   305,    75,   305,    -1,   305,    76,   305,    -1,
     305,    77,   305,    -1,    78,   305,    -1,   305,    79,   305,
      -1,   305,    80,   305,    -1,   305,    81,   305,    -1,    82,
     305,    -1,   305,    83,   305,    -1,   305,    84,   305,    -1,
     305,    86,   305,    87,   305,    -1,    34,   103,   104,    -1,
      34,   103,   305,   104,    -1,    34,   103,   305,    96,   305,
     104,    -1,    34,   103,   305,    96,   305,    96,   305,   104,
      -1,    34,   103,   305,    96,   305,    96,   305,    96,   305,
     104,    -1,   307,    -1,   308,    -1,   309,    -1,   310,    -1,
     311,    -1,    34,    -1,    41,    34,    -1,    41,    42,    34,
      -1,    42,    41,    34,    -1,    42,    34,    -1,   312,    -1,
      37,    -1,    38,    -1,   313,    -1,    67,    -1,    67,   103,
     104,    -1,    66,   103,    34,   104,    -1,    35,    -1,    32,
      -1,    33,    -1,   314,    -1,   314,    32,    34,    -1,   314,
      33,    34,    -1,    32,    34,    -1,    33,    34,    -1,    49,
      50,   353,    -1,    53,    34,    95,    -1,    52,    34,    95,
      -1,    52,    55,    34,    95,    -1,    54,    34,    95,    -1,
      -1,    54,    34,    97,   320,   322,    98,   353,    -1,    -1,
      54,    34,    56,    34,    97,   321,   322,    98,   353,    -1,
     323,    -1,    -1,   323,   324,    -1,   324,    -1,    34,    95,
      -1,    34,    99,   352,    95,    -1,    -1,    51,    34,    97,
     326,   334,   340,    98,   353,    -1,    -1,    51,    34,    56,
      34,    97,   327,   334,   340,    98,   353,    -1,    -1,    52,
      34,    97,   329,   334,   340,    98,   353,    -1,    -1,    52,
      34,    56,    34,    97,   330,   334,   340,    98,   353,    -1,
      -1,    53,    34,    97,   332,   334,   340,    98,   353,    -1,
      -1,    53,    34,    56,    34,    97,   333,   334,   340,    98,
     353,    -1,    -1,    58,    87,   335,   336,    -1,    -1,   337,
      -1,    -1,   337,   338,    -1,   338,    -1,    34,    99,   339,
      95,    -1,    35,    -1,    32,    -1,    33,    -1,   314,    -1,
      37,    -1,    38,    -1,    -1,    57,    87,   341,   342,    -1,
      -1,   343,    -1,    -1,   343,   344,    -1,   344,    -1,    -1,
     347,    34,   345,   348,   349,   350,    95,    -1,    -1,    59,
     347,    34,   346,   348,   349,   350,    95,    -1,    34,    -1,
      34,    90,    -1,    60,    -1,    61,    -1,    62,    -1,    63,
      -1,    65,    60,    -1,    65,    61,    -1,    65,    62,    -1,
      65,    63,    -1,    64,    -1,    45,    -1,    46,    -1,   100,
      32,   101,    -1,   100,    34,   101,    -1,   100,   101,    -1,
      -1,    54,   103,    34,   104,    -1,    -1,    99,   351,    -1,
      -1,    35,    -1,    36,    -1,    32,    -1,    89,    32,    -1,
      33,    -1,    89,    33,    -1,   314,    -1,    37,    -1,    38,
      -1,    34,    -1,    32,    -1,    89,    32,    -1,    34,    -1,
      95,    -1,    -1,    96,    -1,    95,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   212,   212,   216,   217,   221,   223,   225,   227,   229,
     232,   234,   236,   238,   241,   242,   243,   244,   246,   248,
     250,   252,   259,   258,   270,   271,   275,   285,   289,   298,
     299,   303,   309,   318,   322,   330,   339,   349,   350,   354,
     360,   370,   378,   388,   392,   400,   411,   422,   433,   443,
     447,   455,   469,   480,   481,   486,   485,   496,   497,   501,
     502,   506,   516,   517,   522,   521,   532,   533,   537,   538,
     542,   552,   553,   557,   565,   566,   570,   578,   579,   584,
     583,   594,   595,   599,   603,   611,   612,   617,   616,   627,
     628,   632,   636,   645,   649,   653,   657,   661,   665,   669,
     673,   677,   681,   685,   693,   694,   699,   698,   709,   710,
     714,   715,   716,   717,   721,   722,   726,   731,   739,   740,
     744,   749,   758,   759,   764,   763,   774,   775,   779,   780,
     781,   782,   786,   787,   791,   796,   804,   805,   809,   814,
     823,   824,   829,   828,   839,   840,   844,   845,   850,   849,
     859,   858,   869,   868,   878,   877,   890,   897,   898,   903,
     902,   913,   914,   918,   919,   924,   923,   933,   932,   943,
     942,   952,   951,   964,   971,   972,   976,   977,   982,   981,
     990,   989,  1001,  1002,  1006,  1007,  1011,  1020,  1021,  1025,
    1026,  1031,  1030,  1039,  1038,  1050,  1051,  1055,  1056,  1060,
    1069,  1070,  1074,  1075,  1080,  1079,  1088,  1087,  1100,  1101,
    1105,  1106,  1110,  1120,  1121,  1125,  1126,  1131,  1130,  1139,
    1138,  1151,  1152,  1156,  1157,  1161,  1171,  1172,  1177,  1176,
    1185,  1184,  1196,  1197,  1201,  1202,  1206,  1213,  1222,  1223,
    1228,  1227,  1236,  1235,  1247,  1248,  1252,  1253,  1257,  1264,
    1273,  1277,  1282,  1286,  1291,  1292,  1297,  1296,  1306,  1305,
    1317,  1318,  1322,  1323,  1327,  1328,  1333,  1332,  1346,  1347,
    1351,  1361,  1365,  1369,  1373,  1377,  1378,  1382,  1387,  1392,
    1398,  1407,  1408,  1412,  1418,  1426,  1431,  1435,  1443,  1450,
    1456,  1466,  1467,  1471,  1476,  1483,  1488,  1492,  1500,  1505,
    1509,  1518,  1523,  1528,  1537,  1538,  1543,  1542,  1552,  1551,
    1563,  1564,  1568,  1569,  1573,  1574,  1579,  1578,  1592,  1593,
    1597,  1607,  1611,  1615,  1619,  1623,  1624,  1628,  1633,  1638,
    1644,  1653,  1654,  1658,  1664,  1672,  1677,  1681,  1689,  1696,
    1702,  1712,  1713,  1717,  1722,  1729,  1734,  1738,  1746,  1751,
    1755,  1764,  1769,  1774,  1783,  1790,  1799,  1808,  1819,  1830,
    1835,  1840,  1844,  1853,  1855,  1857,  1859,  1864,  1866,  1871,
    1872,  1875,  1877,  1879,  1881,  1883,  1885,  1888,  1892,  1894,
    1896,  1898,  1900,  1902,  1905,  1907,  1909,  1912,  1916,  1918,
    1920,  1923,  1926,  1928,  1930,  1933,  1935,  1937,  1939,  1941,
    1946,  1947,  1948,  1949,  1950,  1954,  1959,  1964,  1970,  1976,
    1984,  1988,  1990,  1995,  1999,  2001,  2003,  2008,  2013,  2015,
    2017,  2023,  2024,  2025,  2026,  2032,  2041,  2050,  2057,  2067,
    2077,  2076,  2088,  2087,  2102,  2103,  2107,  2108,  2112,  2118,
    2129,  2128,  2139,  2138,  2153,  2152,  2163,  2162,  2177,  2176,
    2187,  2186,  2201,  2200,  2206,  2210,  2211,  2215,  2216,  2220,
    2230,  2231,  2232,  2233,  2234,  2235,  2240,  2239,  2245,  2249,
    2250,  2254,  2255,  2260,  2259,  2270,  2269,  2283,  2284,  2286,
    2287,  2288,  2289,  2291,  2292,  2293,  2294,  2296,  2297,  2298,
    2303,  2308,  2313,  2317,  2321,  2325,  2329,  2333,  2337,  2338,
    2339,  2340,  2341,  2342,  2343,  2344,  2345,  2346,  2350,  2351,
    2352,  2355,  2355,  2357,  2357
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
  "cplusplus", "struct_decl", "class_decl", "enum_decl", "enum", "@36", 
  "@37", "opt_enumfields", "enumfields", "enumfield", "message", "@38", 
  "@39", "class", "@40", "@41", "struct", "@42", "@43", 
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
     108,   108,   110,   109,   111,   111,   112,   113,   114,   115,
     115,   116,   116,   117,   117,   118,   119,   120,   120,   121,
     121,   122,   123,   124,   124,   125,   126,   127,   128,   129,
     129,   130,   131,   132,   132,   134,   133,   135,   135,   136,
     136,   137,   138,   138,   140,   139,   141,   141,   142,   142,
     143,   144,   144,   145,   146,   146,   147,   148,   148,   150,
     149,   151,   151,   152,   152,   153,   153,   155,   154,   156,
     156,   157,   157,   158,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   160,   160,   162,   161,   163,   163,
     164,   164,   164,   164,   165,   165,   166,   166,   167,   167,
     168,   168,   169,   169,   171,   170,   172,   172,   173,   173,
     173,   173,   174,   174,   175,   175,   176,   176,   177,   177,
     178,   178,   180,   179,   181,   181,   182,   182,   184,   183,
     185,   183,   186,   183,   187,   183,   188,   189,   189,   191,
     190,   192,   192,   193,   193,   195,   194,   196,   194,   197,
     194,   198,   194,   199,   200,   200,   201,   201,   203,   202,
     204,   202,   205,   205,   206,   206,   207,   208,   208,   209,
     209,   211,   210,   212,   210,   213,   213,   214,   214,   215,
     216,   216,   217,   217,   219,   218,   220,   218,   221,   221,
     222,   222,   223,   224,   224,   225,   225,   227,   226,   228,
     226,   229,   229,   230,   230,   231,   232,   232,   234,   233,
     235,   233,   236,   236,   237,   237,   238,   238,   239,   239,
     241,   240,   242,   240,   243,   243,   244,   244,   245,   245,
     246,   246,   247,   247,   248,   248,   250,   249,   251,   249,
     252,   252,   253,   253,   254,   254,   256,   255,   257,   257,
     258,   259,   259,   260,   260,   261,   261,   262,   262,   262,
     262,   263,   263,   264,   264,   265,   265,   265,   266,   266,
     266,   267,   267,   268,   268,   269,   269,   269,   270,   270,
     270,   271,   271,   271,   272,   272,   274,   273,   275,   273,
     276,   276,   277,   277,   278,   278,   280,   279,   281,   281,
     282,   283,   283,   284,   284,   285,   285,   286,   286,   286,
     286,   287,   287,   288,   288,   289,   289,   289,   290,   290,
     290,   291,   291,   292,   292,   293,   293,   293,   294,   294,
     294,   295,   295,   295,   296,   297,   298,   299,   300,   301,
     302,   302,   302,   303,   303,   303,   303,   304,   304,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     306,   306,   306,   306,   306,   307,   307,   307,   307,   307,
     308,   309,   309,   310,   311,   311,   311,   312,   313,   313,
     313,   314,   314,   314,   314,   315,   316,   317,   317,   318,
     320,   319,   321,   319,   322,   322,   323,   323,   324,   324,
     326,   325,   327,   325,   329,   328,   330,   328,   332,   331,
     333,   331,   335,   334,   334,   336,   336,   337,   337,   338,
     339,   339,   339,   339,   339,   339,   341,   340,   340,   342,
     342,   343,   343,   345,   344,   346,   344,   347,   347,   347,
     347,   347,   347,   347,   347,   347,   347,   347,   347,   347,
     348,   348,   348,   348,   349,   349,   350,   350,   351,   351,
     351,   351,   351,   351,   351,   351,   351,   351,   352,   352,
     352,   353,   353,   354,   354
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     4,     3,     1,     1,     3,     2,     0,
       1,     4,     3,     3,     2,     5,     2,     1,     0,     5,
       4,     5,     2,     3,     2,     7,     2,     8,     2,     3,
       2,    10,     2,     1,     0,     0,     4,     2,     0,     3,
       1,     1,     1,     0,     0,     4,     2,     0,     3,     1,
       1,     1,     0,     4,     1,     0,     4,     1,     0,     0,
       4,     2,     0,     3,     1,     1,     0,     0,     4,     2,
       0,     3,     1,     1,     1,     3,     2,     3,     4,     4,
       3,     3,     3,     3,     1,     0,     0,     4,     1,     0,
       4,     3,     4,     3,     3,     1,     3,     1,     3,     1,
       3,     1,     1,     0,     0,     4,     1,     0,     4,     3,
       4,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       1,     0,     0,     4,     1,     0,     2,     1,     0,     7,
       0,     8,     0,     9,     0,    10,     3,     1,     0,     0,
       4,     1,     0,     2,     1,     0,     7,     0,     8,     0,
       9,     0,    10,     3,     1,     0,     2,     1,     0,     4,
       0,     6,     2,     0,     3,     1,     1,     1,     0,     2,
       1,     0,     4,     0,     6,     2,     0,     3,     1,     1,
       1,     0,     2,     1,     0,     4,     0,     6,     2,     0,
       3,     1,     3,     1,     0,     2,     1,     0,     4,     0,
       6,     1,     0,     2,     1,     4,     2,     0,     0,     4,
       0,     6,     2,     0,     3,     1,     2,     1,     2,     0,
       0,     4,     0,     6,     2,     0,     3,     1,     2,     1,
       4,     0,     4,     0,     1,     0,     0,     5,     0,     4,
       1,     0,     2,     1,     1,     1,     0,     7,     3,     1,
       5,     2,     0,     2,     0,     2,     1,     6,     8,     6,
       8,     3,     1,     2,     1,     2,     1,     2,     2,     1,
       2,     3,     1,     2,     1,     2,     1,     2,     2,     1,
       2,     1,     2,     3,     1,     0,     0,     5,     0,     4,
       1,     0,     2,     1,     1,     1,     0,     7,     3,     1,
       5,     2,     0,     2,     0,     2,     1,     6,     8,     6,
       8,     3,     1,     2,     1,     2,     1,     2,     2,     1,
       2,     3,     1,     2,     1,     2,     1,     2,     2,     1,
       2,     1,     2,     3,     4,     5,     2,     6,     5,     3,
       1,     1,     1,     6,     4,     3,     1,     6,     4,     1,
       3,     3,     3,     3,     3,     3,     3,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     3,     3,
       3,     2,     3,     3,     5,     3,     4,     6,     8,    10,
       1,     1,     1,     1,     1,     1,     2,     3,     3,     2,
       1,     1,     1,     1,     1,     3,     4,     1,     1,     1,
       1,     3,     3,     2,     2,     3,     3,     3,     4,     3,
       0,     7,     0,     9,     1,     0,     2,     1,     2,     4,
       0,     8,     0,    10,     0,     8,     0,    10,     0,     8,
       0,    10,     0,     4,     0,     1,     0,     2,     1,     4,
       1,     1,     1,     1,     1,     1,     0,     4,     0,     1,
       0,     2,     1,     0,     7,     0,     8,     1,     2,     1,
       1,     1,     1,     2,     2,     2,     2,     1,     1,     1,
       3,     3,     2,     0,     4,     0,     2,     0,     1,     1,
       1,     2,     1,     2,     1,     1,     1,     1,     1,     2,
       1,     1,     0,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       4,     0,     2,     1,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     3,     5,    10,    29,     6,     0,
      11,    54,     7,     0,    12,    54,     8,     0,    13,   175,
       9,     0,    14,    15,    16,    17,    18,    19,    20,    21,
       0,    42,    28,    48,     0,   512,     0,     0,     0,     0,
       0,     0,     0,    30,    38,     0,    78,    53,    63,    78,
      63,     0,   201,   174,   177,   188,    26,     0,    25,     0,
     511,   425,     0,   440,     0,   427,   444,     0,     0,   426,
     448,     0,   429,   430,   418,   419,   405,   417,   411,   412,
     366,     0,     0,     0,     0,   414,     0,     0,     0,     0,
     512,   361,   362,   360,   369,   400,   401,   402,   403,   404,
     410,   413,   420,   512,    27,     0,     0,     0,    37,    55,
       0,   105,    77,     0,    86,    62,   105,    86,     0,   178,
       0,     0,   200,   203,   176,     0,   214,   187,   190,    23,
       0,   512,     0,   454,     0,   454,   428,     0,   454,     0,
     435,   423,   424,     0,     0,     0,   406,     0,   409,     0,
       0,     0,   387,   391,   377,     0,    32,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     512,    34,   512,     0,   512,     0,    58,    79,     0,     0,
     104,    64,     0,   123,    85,   141,   123,     0,   183,     0,
     204,   512,   354,   202,     0,   191,     0,     0,   213,   216,
     189,    24,   355,   442,     0,   468,   446,   468,   450,   468,
     432,     0,     0,   434,   437,   395,     0,   365,     0,     0,
     407,   408,     0,   415,   370,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   388,   389,   390,   392,   393,     0,
     371,   372,   373,   374,   375,   376,   421,   422,    33,    31,
       0,    35,     0,    61,    56,     0,    60,    82,   106,   512,
      41,    67,    87,     0,     0,   122,     0,   255,   140,   158,
     180,   186,   179,     0,   185,     0,   209,   356,     0,   196,
       0,   217,   512,   215,   454,   452,     0,     0,   454,     0,
     454,     0,   435,   438,     0,   512,   436,     0,   396,     0,
     364,     0,   368,   416,     0,    40,     0,    57,     0,    94,
       0,    80,     0,    84,    93,   109,   512,    44,    70,    65,
       0,    69,    90,   124,   512,   142,     0,    72,   254,     0,
     305,   157,   183,   182,     0,   206,     0,   205,     0,   211,
     193,   199,   192,     0,   198,     0,   222,   357,   468,   456,
     466,   512,   468,   512,   468,   512,     0,   508,   510,     0,
       0,   431,     0,     0,     0,   394,    39,    59,     0,    96,
      81,     0,     0,     0,   107,   108,    43,    66,     0,    88,
       0,    92,   127,    45,   145,     0,   258,     0,     0,    71,
     159,     0,    75,   304,   181,   184,   209,     0,   208,     0,
     196,   195,     0,   219,     0,   218,   221,   224,     0,     0,
     453,   455,   458,   470,   441,     0,   445,     0,   449,   512,
     509,   439,     0,   397,   363,   367,    97,    95,   100,   101,
     102,   103,    83,   117,     0,   115,   121,     0,   119,     0,
       0,    68,    89,     0,     0,   125,   126,     0,   143,   144,
     147,   256,   261,     0,   512,    47,   162,     0,   308,     0,
       0,    74,   207,   212,   210,   194,   197,   222,     0,   223,
     512,     0,   457,   477,   488,   489,     0,   479,   480,   481,
     482,   487,     0,   467,   469,   472,     0,   512,   512,   433,
       0,    98,    99,     0,   111,     0,     0,   113,     0,     0,
       0,    91,   135,     0,   133,   139,     0,   137,     0,     0,
       0,   146,   261,   266,   289,   259,   260,   263,   264,   265,
       0,     0,   282,     0,   512,    50,     0,   160,   161,   164,
     306,   311,     0,   512,   220,     0,   443,   461,   462,   460,
     464,   465,   463,     0,   478,     0,   483,   484,   485,   486,
     471,   473,   447,   451,     0,   398,   116,   114,   120,   118,
     110,   112,     0,   129,     0,     0,   131,     0,     0,     0,
     512,   257,     0,   290,     0,   288,   262,     0,     0,     0,
      73,    49,     0,   163,   311,   316,   339,   309,   310,   313,
     314,   315,     0,     0,   332,     0,    51,   225,   459,   475,
     493,     0,   134,   132,   138,   136,   128,   130,     0,   512,
     148,     0,     0,   269,     0,     0,   299,   272,     0,   292,
       0,   272,     0,   286,   281,   512,   307,     0,   340,   338,
     312,     0,     0,     0,    76,   493,     0,   495,   399,   512,
       0,   150,   175,     0,     0,     0,   359,   302,   300,   298,
       0,   274,     0,     0,     0,   274,     0,   287,   285,     0,
     512,   165,     0,     0,   319,   349,   322,     0,   342,     0,
     322,     0,   336,   331,   495,     0,     0,   492,     0,   497,
     152,   512,   175,   201,     0,     0,   276,   268,   271,     0,
       0,   296,   291,   299,   272,   303,     0,   272,   512,     0,
     167,   188,     0,     0,     0,   350,   348,   352,     0,   324,
       0,     0,     0,   324,     0,   337,   335,   497,   490,   491,
       0,     0,     0,   175,   154,   201,   149,   227,     0,   512,
     275,   273,   514,   513,   279,   297,   295,   274,   277,   274,
     169,   512,   188,   214,     0,     0,   326,   318,   321,     0,
       0,   346,   341,   349,   322,   353,     0,   322,     0,     0,
     500,   502,   507,   498,   499,   505,   506,     0,   504,   496,
     474,   201,   175,   151,   251,   270,   267,     0,     0,   188,
     171,   214,   166,   239,     0,     0,   325,   323,   329,   347,
     345,   324,   327,   324,   476,   494,   501,   503,   153,   201,
       0,     0,   226,   156,   280,   278,   214,   188,   168,   253,
     320,   317,     0,     0,   155,     0,   228,     0,   170,   214,
       0,     0,   238,   173,   330,   328,     0,   233,     0,   172,
       0,   240,     0,   230,   237,   229,     0,   235,   250,     0,
     245,     0,   233,   236,   232,     0,   242,   249,   241,     0,
     247,   252,   231,   234,   245,   248,   244,     0,   243,   246
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     2,    14,    15,    40,    67,    68,    16,    17,
      52,    53,   114,    18,    19,   117,   118,    20,    21,   280,
      22,    23,    24,    25,   475,    26,    27,    56,    57,   196,
     274,   275,   276,   124,   125,   281,   339,   340,   341,   408,
     409,   480,   481,   121,   122,   277,   331,   332,   203,   204,
     342,   399,   400,   333,   334,   199,   200,   335,   394,   395,
     454,   455,   457,   458,   284,   285,   402,   465,   466,   523,
     524,   526,   527,   287,   288,   404,   468,   469,   470,   662,
     702,   743,   792,   746,   350,   351,   476,   547,   548,   549,
     721,   762,   799,   827,   802,    62,    63,    64,   208,   352,
     292,   293,   294,   136,   137,   138,   299,   420,   362,   363,
     364,   747,   132,   133,   296,   416,   357,   358,   359,   803,
     218,   219,   366,   487,   425,   426,   427,   794,   822,   847,
     862,   855,   856,   857,   829,   842,   860,   874,   868,   869,
     870,   823,   843,   347,   348,   532,   472,   535,   536,   537,
     538,   592,   632,   633,   671,   710,   705,   539,   540,   541,
     644,   542,   637,   638,   712,   639,   640,   412,   413,   604,
     551,   607,   608,   609,   610,   647,   683,   684,   729,   770,
     765,   611,   612,   613,   693,   614,   686,   687,   772,   688,
     689,    28,    29,   212,    30,    31,   669,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
      32,    33,    34,    35,    36,   150,   312,   232,   233,   234,
      37,   143,   304,    38,   145,   308,    39,   148,   310,   225,
     369,   430,   431,   432,   563,   307,   433,   503,   504,   505,
     620,   655,   506,   657,   699,   742,   789,   380,    71,   754
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -724
static const short yypact[] =
{
    -724,    39,    28,  -724,  -724,    -6,    29,   148,   155,   118,
     196,    -8,   198,   228,  -724,  -724,  -724,   153,  -724,   104,
    -724,   223,  -724,   171,  -724,   223,  -724,   178,  -724,   255,
    -724,   202,  -724,  -724,  -724,  -724,  -724,  -724,  -724,  -724,
     259,   206,   213,   229,   234,   233,   131,   -29,   299,   107,
     149,   321,   318,   326,   336,   303,   379,  -724,   371,   379,
     371,    -4,   389,   255,  -724,   377,  -724,   -34,  -724,   391,
    -724,  -724,   394,  -724,   441,  -724,  -724,   328,   448,  -724,
    -724,   458,  -724,  -724,   460,   482,   338,  -724,  -724,  -724,
     414,   415,   130,   132,   416,   419,   360,   360,   360,   360,
     233,  -724,  -724,   747,  -724,  -724,  -724,  -724,  -724,  -724,
    -724,  -724,   218,    42,  -724,   321,   424,   426,   491,  -724,
     439,   519,  -724,   442,   521,  -724,   519,   521,   321,  -724,
     120,   504,   389,  -724,  -724,   133,   525,   377,  -724,  -724,
     259,   233,   436,   476,   438,   476,  -724,   440,   476,   443,
     502,  -724,  -724,   245,   263,   506,  -724,   505,  -724,   508,
     509,   434,  -724,  -724,  -724,   669,  -724,   360,   360,   360,
     360,   360,   360,   360,   360,   360,   360,   360,   360,   360,
     360,   360,   360,   360,   360,   360,   360,   360,   510,   511,
     233,  -724,   233,   321,   233,   451,   513,  -724,   462,   527,
    -724,  -724,   465,   545,  -724,   544,   545,   469,   523,   321,
    -724,   233,  -724,  -724,   321,  -724,   134,   463,   525,  -724,
    -724,  -724,   461,  -724,   475,   507,  -724,   507,  -724,   507,
    -724,   164,   467,   502,  -724,  -724,   397,  -724,   551,    90,
    -724,  -724,   459,  -724,  -724,   367,   367,   367,   367,   367,
     367,   797,   797,   797,   225,   225,   225,   290,   290,   503,
     246,   246,   473,   473,   473,   473,  -724,  -724,  -724,  -724,
     474,  -724,   321,  -724,  -724,   197,  -724,    16,  -724,   124,
    -724,   533,  -724,   481,   487,  -724,   484,   577,  -724,   587,
    -724,  -724,  -724,   211,  -724,   512,   564,  -724,   514,   566,
     321,  -724,   233,  -724,   476,  -724,   516,   517,   476,   518,
     476,   520,   502,  -724,   144,   233,  -724,   360,  -724,   360,
    -724,   506,  -724,  -724,   360,  -724,   515,  -724,   513,   522,
     570,  -724,   224,  -724,  -724,   311,   233,  -724,  -724,  -724,
     236,  -724,    16,  -724,   233,  -724,     9,   593,  -724,   524,
     595,  -724,   523,  -724,   523,  -724,   530,  -724,   247,  -724,
    -724,  -724,  -724,   251,  -724,   549,   574,  -724,   507,   578,
    -724,   233,   507,   233,   507,   233,   540,  -724,  -724,   582,
     550,  -724,   594,   695,   542,   772,  -724,  -724,   374,  -724,
    -724,    16,   614,   615,  -724,   337,  -724,  -724,   533,  -724,
     556,  -724,   355,  -724,   618,   567,  -724,   569,   628,  -724,
    -724,    23,   645,  -724,  -724,  -724,   564,   321,  -724,   564,
     566,  -724,   566,  -724,   559,  -724,   574,  -724,   561,   562,
    -724,   578,  -724,   370,  -724,   581,  -724,   583,  -724,   233,
    -724,  -724,   360,  -724,  -724,  -724,   616,   629,  -724,  -724,
    -724,  -724,  -724,   576,   269,  -724,   588,   289,  -724,   614,
     615,  -724,    16,   657,   658,  -724,   392,   606,  -724,   618,
    -724,  -724,    22,   659,   128,  -724,   661,   609,  -724,   610,
     601,  -724,  -724,  -724,  -724,  -724,  -724,   574,   321,  -724,
     233,   339,  -724,   611,  -724,  -724,   450,  -724,  -724,  -724,
    -724,  -724,   351,  -724,   370,  -724,   666,   233,   233,  -724,
     633,  -724,  -724,   617,  -724,   614,   626,  -724,   615,   341,
     344,  -724,   630,   348,  -724,   631,   357,  -724,   657,   658,
     677,  -724,    22,  -724,   -45,  -724,    22,  -724,  -724,  -724,
     445,   613,  -724,   637,   233,  -724,   641,  -724,   661,  -724,
    -724,    31,   698,   233,  -724,   639,  -724,   460,   482,  -724,
    -724,  -724,   218,   652,  -724,   702,  -724,  -724,  -724,  -724,
    -724,  -724,  -724,  -724,   360,  -724,  -724,  -724,  -724,  -724,
    -724,  -724,   650,  -724,   657,   653,  -724,   658,   369,   402,
      45,  -724,   722,  -724,   321,   675,  -724,   200,   200,   729,
    -724,  -724,   746,  -724,    31,  -724,   167,  -724,    31,  -724,
    -724,  -724,   483,   680,  -724,   694,  -724,  -724,  -724,  -724,
     703,   721,  -724,  -724,  -724,  -724,  -724,  -724,   774,   -10,
    -724,   707,   795,   733,   731,   321,     4,   813,   748,  -724,
      21,   813,   150,   -27,  -724,    48,  -724,   820,  -724,   755,
    -724,   824,   824,   825,  -724,   703,   117,   818,  -724,   233,
     839,  -724,   255,   321,   840,   722,  -724,  -724,  -724,   773,
     321,   867,   848,   849,   321,   867,   849,  -724,  -724,   850,
      -3,  -724,   792,   872,   798,   158,   874,   791,  -724,   143,
     874,    19,   -14,  -724,   818,   796,   799,  -724,   793,   800,
    -724,   233,   255,   389,   879,   215,  -724,  -724,  -724,   866,
     407,   115,  -724,   188,   813,  -724,   407,   813,   233,   868,
    -724,   377,   321,   869,   820,  -724,   802,  -724,   321,   893,
     873,   875,   321,   893,   875,  -724,  -724,   800,  -724,  -724,
     876,   176,   811,   255,  -724,   389,  -724,  -724,   321,   233,
    -724,  -724,  -724,  -724,  -724,  -724,  -724,   867,  -724,   867,
    -724,   233,   377,   525,   889,   220,  -724,  -724,  -724,   877,
     816,   138,  -724,   189,   874,  -724,   819,   874,   821,   809,
     460,   482,  -724,  -724,  -724,  -724,  -724,   472,   218,  -724,
    -724,   389,   255,  -724,   261,  -724,  -724,   407,   407,   377,
    -724,   525,  -724,  -724,   321,   822,  -724,  -724,  -724,  -724,
    -724,   893,  -724,   893,  -724,  -724,  -724,  -724,  -724,   389,
     135,   828,  -724,  -724,  -724,  -724,   525,   377,  -724,   272,
    -724,  -724,   823,   826,  -724,   321,  -724,   884,  -724,   525,
     140,   833,  -724,  -724,  -724,  -724,   835,   890,   830,  -724,
     321,  -724,   888,  -724,   827,  -724,   411,  -724,  -724,   841,
     892,   834,   890,  -724,  -724,   890,  -724,   827,  -724,   413,
    -724,  -724,  -724,  -724,   892,  -724,  -724,   892,  -724,  -724
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -724,  -724,  -724,  -724,  -724,  -724,  -724,   790,  -724,  -724,
    -724,  -724,  -724,  -724,  -724,  -724,  -724,  -724,  -724,  -724,
    -724,  -724,  -724,  -724,  -724,  -724,  -724,   906,  -724,  -724,
    -724,  -724,   604,   878,  -724,  -724,  -724,  -724,   535,  -724,
    -724,  -724,  -724,   880,  -724,  -724,  -724,  -724,   807,  -724,
    -724,  -724,  -724,   546,  -327,   810,  -724,  -724,  -724,  -724,
     485,   420,   480,   423,   736,  -724,  -724,  -724,  -724,   417,
     359,   418,   361,  -724,  -724,  -724,  -724,  -724,   477,  -724,
    -724,  -724,  -724,  -716,  -724,  -724,  -724,  -724,  -724,   401,
    -724,  -724,  -724,  -724,  -584,  -654,  -724,   887,  -724,  -724,
     599,  -724,   598,  -691,  -724,   817,  -724,  -724,   536,  -724,
     531,   895,  -724,   829,  -724,  -724,   539,  -724,   541,   831,
    -724,   740,  -724,  -724,   478,  -724,   537,  -724,  -724,  -724,
    -724,    97,  -724,    99,  -724,  -724,  -724,  -724,    88,  -724,
      89,  -724,  -724,  -724,  -724,  -724,  -724,   437,  -724,   432,
    -724,  -724,   305,  -724,  -630,  -650,  -724,  -476,  -724,  -724,
    -724,  -724,  -529,  -724,  -724,  -724,   373,  -724,  -724,  -724,
    -724,   368,  -724,   365,  -724,  -724,   250,  -724,  -672,  -723,
    -724,  -701,  -724,  -724,  -724,  -724,  -592,  -724,  -724,  -724,
     323,  -724,  -724,  -724,  -724,  -724,  -532,  -115,  -724,  -724,
     -53,  -724,  -724,  -724,  -724,  -724,  -724,  -151,  -724,  -488,
    -724,  -724,  -724,  -724,  -724,  -724,  -724,   664,  -724,   744,
    -724,  -724,  -724,  -724,  -724,  -724,  -724,  -724,  -724,  -139,
    -724,  -724,  -724,   547,  -724,  -222,  -724,  -724,  -724,   479,
    -724,  -724,   486,   324,   286,   244,  -724,  -724,   -99,  -700
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -359
static const short yytable[] =
{
     192,   166,   595,   562,   239,   309,   227,   311,   703,   229,
     776,   675,   660,   207,   191,   401,   758,   128,   733,   719,
    -301,  -301,   766,   593,   405,   716,    47,    74,    41,   793,
     763,     4,     5,     6,     7,  -301,   734,   673,   477,     3,
     533,   677,   222,   162,   163,   164,   165,    48,   745,   605,
     329,     8,   674,   732,   735,   594,   534,  -284,   629,   330,
     690,   139,   140,    42,   806,   606,    75,   628,    76,   641,
     679,   801,   668,   594,   649,   818,   190,     9,   270,    10,
      11,    12,    13,   129,   757,    70,   594,   759,   832,   791,
     833,   268,    70,   269,   295,   271,   406,   824,   825,   298,
     236,   238,   811,   834,   594,   813,  -294,   797,   826,   798,
     478,   678,   297,   680,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   521,   839,    70,   819,   774,
      70,   209,   777,    70,   714,   594,   428,   717,   594,   695,
     435,   696,   437,   726,   214,   300,   835,   326,   336,   731,
     736,   850,   544,    78,   156,   368,   158,   676,    45,   372,
     384,   374,   157,   159,  -351,  -351,   377,   732,   378,   756,
     337,   674,    43,   755,    51,   365,   321,    72,   706,    44,
      84,    85,    86,    87,   322,    88,    89,    90,    91,    92,
      93,    54,    79,   367,    80,    81,   809,   210,   780,   781,
     782,   783,   784,   785,   786,   594,   381,   828,   697,    70,
     215,   301,   836,    70,    94,    95,   725,   851,    73,   750,
      46,   635,    49,   379,   636,   648,    96,   396,   594,   810,
      97,   726,   838,   749,    82,   403,    83,    98,   805,   534,
     188,   189,    55,   788,   606,   849,   668,   725,   594,   313,
    -344,    99,    50,   314,   382,   787,   383,   594,    58,  -334,
     820,   385,   434,   821,   436,    60,   438,    84,    85,    86,
      87,   840,    88,    89,   841,    61,    92,    93,   594,   594,
    -294,  -344,   327,   328,    66,    84,    85,    86,    87,    65,
      88,    89,   483,   -46,    92,    93,   353,   354,   179,   180,
     -36,    94,    95,   182,   183,   184,   185,   186,   187,   390,
     391,    69,   863,    96,   392,   393,   -52,    97,    70,    94,
      95,   397,   398,    77,    98,   875,   184,   185,   186,   187,
     509,    96,   418,   419,   113,    97,   421,   422,    99,   235,
     459,   460,    98,    84,    85,    86,    87,   115,    88,    89,
      90,    91,    92,    93,   514,   515,    99,   237,   463,   464,
     116,   557,   558,   555,   559,   545,   560,   561,   182,   183,
     184,   185,   186,   187,   517,   518,   120,    94,    95,   510,
     119,   556,    84,    85,    86,    87,   130,    88,    89,    96,
     123,    92,    93,    97,   493,   528,   529,   135,   572,   573,
      98,   566,   567,   568,   569,   494,   495,   446,   447,   448,
     449,   450,   451,   146,    99,   141,    94,    95,   142,   496,
     497,   498,   499,   500,   501,   502,   580,   515,    96,   581,
     518,   153,    97,   583,   584,   601,   176,   177,   178,    98,
     179,   180,   586,   587,   616,   182,   183,   184,   185,   186,
     187,   597,   598,    99,   626,   584,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   144,   176,   177,   178,   634,
     179,   180,   147,   181,   493,   182,   183,   184,   185,   186,
     187,   630,   149,   317,   151,   494,   495,   627,   587,   651,
     652,   318,   752,   753,   816,   817,   864,   865,   876,   877,
     497,   498,   499,   500,   501,   502,   152,   154,   155,   160,
     667,   621,   161,   193,   194,   195,   197,   198,   202,   201,
     661,   211,   216,   223,   224,   226,   231,   228,   243,   240,
     230,    87,   241,   242,   266,   267,   681,   273,   704,   278,
     272,   279,   282,   283,   286,   708,   290,   291,  -358,   715,
     700,   302,   305,   323,   306,   315,   187,   338,   343,   325,
     727,   345,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   720,   176,   177,   178,   344,   179,   180,   346,   181,
     324,   182,   183,   184,   185,   186,   187,   349,   356,   355,
     361,   360,   744,   370,   389,   407,   411,   764,   424,   388,
     386,   410,   429,   768,   440,   371,   373,   775,   375,   760,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   417,
     176,   177,   178,   795,   179,   180,   423,   181,   439,   182,
     183,   184,   185,   186,   187,   441,   445,   319,   453,   456,
     796,   462,   467,   474,   471,   320,   473,   479,   488,   490,
     511,   491,   800,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   512,   176,   177,   178,   513,   179,   180,   507,
     181,   508,   182,   183,   184,   185,   186,   187,   516,   830,
     442,   522,   525,   530,   543,   546,   550,   552,   443,   553,
     571,   564,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   590,   176,   177,   178,   599,   179,   180,   576,   181,
     846,   182,   183,   184,   185,   186,   187,   578,   602,   574,
     582,   585,   600,   615,   617,   859,   619,   575,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   618,   176,   177,
     178,   622,   179,   180,   624,   181,   631,   182,   183,   184,
     185,   186,   187,   643,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   244,   176,   177,   178,  -283,   179,   180,
     645,   181,   653,   182,   183,   184,   185,   186,   187,   654,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   444,
     176,   177,   178,   656,   179,   180,   663,   181,   659,   182,
     183,   184,   185,   186,   187,   664,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   658,   176,   177,   178,   665,
     179,   180,   666,   181,   670,   182,   183,   184,   185,   186,
     187,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     672,   176,   177,   178,   682,   179,   180,  -333,   685,   692,
     182,   183,   184,   185,   186,   187,   167,   168,   169,   170,
     171,   172,   698,   701,   534,  -293,   176,   177,   178,   709,
     179,   180,   711,   713,   718,   182,   183,   184,   185,   186,
     187,   722,   723,   730,   724,   728,   740,   738,   748,   741,
     739,   751,   761,   606,  -343,   769,   790,   771,   804,   773,
     779,   808,   807,   815,   812,   837,   814,   831,   844,   848,
     852,   845,   853,   861,   854,   858,   867,   594,   866,   871,
     221,    59,   387,   461,   206,   577,   205,   452,   127,   126,
     520,   579,   289,   623,   519,   588,   531,   589,   625,   603,
     134,   414,   415,   486,   220,   482,   485,   131,   303,   872,
     484,   213,   878,   489,   873,   554,   879,   217,   596,   591,
     707,   642,   646,   650,   767,   691,   376,   316,   492,   694,
     737,   778,   565,   570
};

static const unsigned short yycheck[] =
{
     115,   100,   534,   491,   155,   227,   145,   229,   662,   148,
     733,   641,    22,   128,   113,   342,   716,    21,   690,    22,
      16,    17,   723,    68,    15,   675,    34,    56,    34,   745,
     721,     3,     4,     5,     6,    31,    17,    16,    15,     0,
      18,    68,   141,    96,    97,    98,    99,    55,   702,    18,
      34,    23,    31,    34,    68,   100,    34,   102,   590,    43,
     652,    95,    96,    34,   765,    34,    95,    22,    97,   598,
      22,   762,    68,   100,   606,   791,    34,    49,   193,    51,
      52,    53,    54,    87,   714,    95,   100,   717,   811,   743,
     813,   190,    95,   192,   209,   194,    87,   797,   798,   214,
     153,   154,   774,   819,   100,   777,   102,   757,   799,   759,
      87,   643,   211,   645,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   462,   827,    95,   792,   731,
      95,    21,   734,    95,   673,   100,   368,   676,   100,    32,
     372,    34,   374,   685,    21,    21,    21,   272,    34,    16,
     692,    21,    34,    56,    34,   304,    34,    17,    50,   308,
     321,   310,    42,    41,    16,    17,    32,    34,    34,   711,
     279,    31,    34,    68,    31,   300,    96,    56,   664,    34,
      32,    33,    34,    35,   104,    37,    38,    39,    40,    41,
      42,    97,    95,   302,    97,    56,    68,    87,    32,    33,
      34,    35,    36,    37,    38,   100,   315,   801,   101,    95,
      87,    87,    87,    95,    66,    67,    68,    87,    97,   705,
      34,    31,    34,    89,    34,    68,    78,   336,   100,   771,
      82,   773,   826,    28,    95,   344,    97,    89,    28,    34,
      32,    33,    29,   741,    34,   839,    68,    68,   100,    95,
     102,   103,    34,    99,   317,    89,   319,   100,    97,   102,
       9,   324,   371,    12,   373,    97,   375,    32,    33,    34,
      35,     9,    37,    38,    12,    30,    41,    42,   100,   100,
     102,   102,    95,    96,    35,    32,    33,    34,    35,    97,
      37,    38,   417,    97,    41,    42,    95,    96,    83,    84,
      97,    66,    67,    88,    89,    90,    91,    92,    93,    95,
      96,    87,   854,    78,    13,    14,    97,    82,    95,    66,
      67,    95,    96,    34,    89,   867,    90,    91,    92,    93,
     439,    78,    95,    96,    26,    82,    95,    96,   103,   104,
      13,    14,    89,    32,    33,    34,    35,    31,    37,    38,
      39,    40,    41,    42,    95,    96,   103,   104,    13,    14,
      34,    32,    33,   488,    35,   474,    37,    38,    88,    89,
      90,    91,    92,    93,    95,    96,     7,    66,    67,   442,
      87,   490,    32,    33,    34,    35,     7,    37,    38,    78,
      29,    41,    42,    82,    34,    13,    14,    30,   507,   508,
      89,    60,    61,    62,    63,    45,    46,    43,    44,    45,
      46,    47,    48,    95,   103,    34,    66,    67,    34,    59,
      60,    61,    62,    63,    64,    65,    95,    96,    78,    95,
      96,   103,    82,    95,    96,   544,    79,    80,    81,    89,
      83,    84,    95,    96,   553,    88,    89,    90,    91,    92,
      93,    16,    17,   103,    95,    96,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    34,    79,    80,    81,   594,
      83,    84,    34,    86,    34,    88,    89,    90,    91,    92,
      93,   590,    34,    96,    34,    45,    46,    95,    96,    16,
      17,   104,    95,    96,    32,    33,    95,    96,    95,    96,
      60,    61,    62,    63,    64,    65,    34,   103,   103,   103,
     635,   574,   103,    99,    98,    34,    87,     8,     7,    87,
     629,    27,     7,    97,    58,    97,    34,    97,   104,    34,
      97,    35,    34,    34,    34,    34,   645,    34,   663,    87,
      99,    24,    87,     8,    10,   670,    87,    34,    97,   674,
     659,    98,    87,   104,    57,    98,    93,    34,    87,    95,
     685,    87,    69,    70,    71,    72,    73,    74,    75,    76,
      77,   680,    79,    80,    81,    98,    83,    84,    11,    86,
      87,    88,    89,    90,    91,    92,    93,    10,    34,    87,
      34,    87,   701,    87,    34,    12,    11,   722,    34,    87,
      95,    87,    34,   728,    32,    98,    98,   732,    98,   718,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    99,
      79,    80,    81,   748,    83,    84,    87,    86,    98,    88,
      89,    90,    91,    92,    93,    95,   104,    96,    34,    34,
     749,    95,    34,    25,    87,   104,    87,    12,    99,    98,
      44,    99,   761,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    43,    79,    80,    81,   100,    83,    84,    98,
      86,    98,    88,    89,    90,    91,    92,    93,   100,   804,
      96,    34,    34,    87,    35,    34,    87,    87,   104,    98,
      34,    90,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    34,    79,    80,    81,   102,    83,    84,   101,    86,
     835,    88,    89,    90,    91,    92,    93,   101,    87,    96,
     100,   100,    95,    35,    95,   850,    34,   104,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    95,    79,    80,
      81,   101,    83,    84,   101,    86,    34,    88,    89,    90,
      91,    92,    93,    34,    69,    70,    71,    72,    73,    74,
      75,    76,    77,   104,    79,    80,    81,   102,    83,    84,
      34,    86,   102,    88,    89,    90,    91,    92,    93,    95,
      69,    70,    71,    72,    73,    74,    75,    76,    77,   104,
      79,    80,    81,   100,    83,    84,    99,    86,    34,    88,
      89,    90,    91,    92,    93,    20,    69,    70,    71,    72,
      73,    74,    75,    76,    77,   104,    79,    80,    81,    96,
      83,    84,   101,    86,    21,    88,    89,    90,    91,    92,
      93,    69,    70,    71,    72,    73,    74,    75,    76,    77,
     102,    79,    80,    81,    34,    83,    84,   102,    34,    34,
      88,    89,    90,    91,    92,    93,    69,    70,    71,    72,
      73,    74,    54,    34,    34,   102,    79,    80,    81,    12,
      83,    84,    34,    34,    34,    88,    89,    90,    91,    92,
      93,    99,    20,   102,    96,    21,   103,   101,    19,    99,
     101,    35,    34,    34,   102,    12,    95,    34,    19,    34,
      34,    95,    35,   104,    95,    87,    95,    95,    95,    35,
      87,    95,    87,    35,    34,    95,    34,   100,    87,    95,
     140,    25,   328,   398,   127,   515,   126,   391,    60,    59,
     460,   518,   206,   584,   459,   528,   469,   529,   587,   548,
      63,   352,   354,   422,   137,   416,   420,    62,   218,   862,
     419,   132,   874,   426,   865,   487,   877,   136,   536,   532,
     665,   598,   604,   608,   724,   652,   312,   233,   431,   655,
     694,   737,   496,   504
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,   106,   107,     0,     3,     4,     5,     6,    23,    49,
      51,    52,    53,    54,   108,   109,   113,   114,   118,   119,
     122,   123,   125,   126,   127,   128,   130,   131,   296,   297,
     299,   300,   315,   316,   317,   318,   319,   325,   328,   331,
     110,    34,    34,    34,    34,    50,    34,    34,    55,    34,
      34,    31,   115,   116,    97,    29,   132,   133,    97,   132,
      97,    30,   200,   201,   202,    97,    35,   111,   112,    87,
      95,   353,    56,    97,    56,    95,    97,    34,    56,    95,
      97,    56,    95,    97,    32,    33,    34,    35,    37,    38,
      39,    40,    41,    42,    66,    67,    78,    82,    89,   103,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,    26,   117,    31,    34,   120,   121,    87,
       7,   148,   149,    29,   138,   139,   148,   138,    21,    87,
       7,   216,   217,   218,   202,    30,   208,   209,   210,    95,
      96,    34,    34,   326,    34,   329,    95,    34,   332,    34,
     320,    34,    34,   103,   103,   103,    34,    42,    34,    41,
     103,   103,   305,   305,   305,   305,   353,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    79,    80,    81,    83,
      84,    86,    88,    89,    90,    91,    92,    93,    32,    33,
      34,   353,   302,    99,    98,    34,   134,    87,     8,   160,
     161,    87,     7,   153,   154,   160,   153,   302,   203,    21,
      87,    27,   298,   218,    21,    87,     7,   224,   225,   226,
     210,   112,   353,    97,    58,   334,    97,   334,    97,   334,
      97,    34,   322,   323,   324,   104,   305,   104,   305,   312,
      34,    34,    34,   104,   104,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305,   305,    34,    34,   353,   353,
     302,   353,    99,    34,   135,   136,   137,   150,    87,    24,
     124,   140,    87,     8,   169,   170,    10,   178,   179,   169,
      87,    34,   205,   206,   207,   302,   219,   353,   302,   211,
      21,    87,    98,   226,   327,    87,    57,   340,   330,   340,
     333,   340,   321,    95,    99,    98,   324,    96,   104,    96,
     104,    96,   104,   104,    87,    95,   302,    95,    96,    34,
      43,   151,   152,   158,   159,   162,    34,   353,    34,   141,
     142,   143,   155,    87,    98,    87,    11,   248,   249,    10,
     189,   190,   204,    95,    96,    87,    34,   221,   222,   223,
      87,    34,   213,   214,   215,   302,   227,   353,   334,   335,
      87,    98,   334,    98,   334,    98,   322,    32,    34,    89,
     352,   353,   305,   305,   312,   305,    95,   137,    87,    34,
      95,    96,    13,    14,   163,   164,   353,    95,    96,   156,
     157,   159,   171,   353,   180,    15,    87,    12,   144,   145,
      87,    11,   272,   273,   205,   207,   220,    99,    95,    96,
     212,    95,    96,    87,    34,   229,   230,   231,   340,    34,
     336,   337,   338,   341,   353,   340,   353,   340,   353,    98,
      32,    95,    96,   104,   104,   104,    43,    44,    45,    46,
      47,    48,   158,    34,   165,   166,    34,   167,   168,    13,
      14,   143,    95,    13,    14,   172,   173,    34,   181,   182,
     183,    87,   251,    87,    25,   129,   191,    15,    87,    12,
     146,   147,   221,   302,   223,   213,   215,   228,    99,   231,
      98,    99,   338,    34,    45,    46,    59,    60,    61,    62,
      63,    64,    65,   342,   343,   344,   347,    98,    98,   353,
     305,    44,    43,   100,    95,    96,   100,    95,    96,   165,
     167,   159,    34,   174,   175,    34,   176,   177,    13,    14,
      87,   183,   250,    18,    34,   252,   253,   254,   255,   262,
     263,   264,   266,    35,    34,   353,    34,   192,   193,   194,
      87,   275,    87,    98,   229,   302,   353,    32,    33,    35,
      37,    38,   314,   339,    90,   347,    60,    61,    62,    63,
     344,    34,   353,   353,    96,   104,   101,   166,   101,   168,
      95,    95,   100,    95,    96,   100,    95,    96,   174,   176,
      34,   252,   256,    68,   100,   301,   254,    16,    17,   102,
      95,   353,    87,   194,   274,    18,    34,   276,   277,   278,
     279,   286,   287,   288,   290,    35,   353,    95,    95,    34,
     345,   305,   101,   175,   101,   177,    95,    95,    22,   301,
     353,    34,   257,   258,   302,    31,    34,   267,   268,   270,
     271,   267,   271,    34,   265,    34,   276,   280,    68,   301,
     278,    16,    17,   102,    95,   346,   100,   348,   104,    34,
      22,   353,   184,    99,    20,    96,   101,   302,    68,   301,
      21,   259,   102,    16,    31,   259,    17,    68,   301,    22,
     301,   353,    34,   281,   282,    34,   291,   292,   294,   295,
     291,   295,    34,   289,   348,    32,    34,   101,    54,   349,
     353,    34,   185,   200,   302,   261,   262,   257,   302,    12,
     260,    34,   269,    34,   267,   302,   260,   267,    34,    22,
     353,   195,    99,    20,    96,    68,   301,   302,    21,   283,
     102,    16,    34,   283,    17,    68,   301,   349,   101,   101,
     103,    99,   350,   186,   353,   200,   188,   216,    19,    28,
     262,    35,    95,    96,   354,    68,   301,   259,   354,   259,
     353,    34,   196,   208,   302,   285,   286,   281,   302,    12,
     284,    34,   293,    34,   291,   302,   284,   291,   350,    34,
      32,    33,    34,    35,    36,    37,    38,    89,   314,   351,
      95,   200,   187,   188,   232,   302,   353,   260,   260,   197,
     353,   208,   199,   224,    19,    28,   286,    35,    95,    68,
     301,   283,    95,   283,    95,   104,    32,    33,   188,   200,
       9,    12,   233,   246,   354,   354,   208,   198,   199,   239,
     302,    95,   284,   284,   188,    21,    87,    87,   199,   208,
       9,    12,   240,   247,    95,    95,   302,   234,    35,   199,
      21,    87,    87,    87,    34,   236,   237,   238,    95,   302,
     241,    35,   235,   301,    95,    96,    87,    34,   243,   244,
     245,    95,   236,   238,   242,   301,    95,    96,   243,   245
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
        case 6:
#line 224 "ned.y"
    { if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); }
    break;

  case 7:
#line 226 "ned.y"
    { if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); }
    break;

  case 8:
#line 228 "ned.y"
    { if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); }
    break;

  case 9:
#line 230 "ned.y"
    { if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); }
    break;

  case 10:
#line 233 "ned.y"
    { if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); }
    break;

  case 11:
#line 235 "ned.y"
    { if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); }
    break;

  case 12:
#line 237 "ned.y"
    { if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); }
    break;

  case 13:
#line 239 "ned.y"
    { if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); }
    break;

  case 18:
#line 247 "ned.y"
    { if (ps.storeSourceCode) ps.enump->setSourceCode(toString(yylsp[0])); }
    break;

  case 19:
#line 249 "ned.y"
    { if (ps.storeSourceCode) ps.messagep->setSourceCode(toString(yylsp[0])); }
    break;

  case 20:
#line 251 "ned.y"
    { if (ps.storeSourceCode) ps.classp->setSourceCode(toString(yylsp[0])); }
    break;

  case 21:
#line 253 "ned.y"
    { if (ps.storeSourceCode) ps.structp->setSourceCode(toString(yylsp[0])); }
    break;

  case 22:
#line 259 "ned.y"
    {
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                }
    break;

  case 23:
#line 264 "ned.y"
    {
                  /* no setTrailingComment(ps.imports,@3); comment already stored with last filename */
                }
    break;

  case 26:
#line 276 "ned.y"
    {
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                }
    break;

  case 28:
#line 290 "ned.y"
    {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                }
    break;

  case 31:
#line 304 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                }
    break;

  case 32:
#line 310 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                }
    break;

  case 33:
#line 319 "ned.y"
    {
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;

  case 34:
#line 323 "ned.y"
    {
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;

  case 35:
#line 333 "ned.y"
    {
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;

  case 36:
#line 340 "ned.y"
    {
                  use_chanattrname_token = 0;
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                }
    break;

  case 39:
#line 355 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                }
    break;

  case 40:
#line 361 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                }
    break;

  case 42:
#line 379 "ned.y"
    {
                  use_chanattrname_token = 1;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 43:
#line 389 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 44:
#line 393 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 45:
#line 405 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 46:
#line 412 "ned.y"
    {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 48:
#line 434 "ned.y"
    {
                  use_chanattrname_token = 1;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 49:
#line 444 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 50:
#line 448 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 51:
#line 463 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 52:
#line 470 "ned.y"
    {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 55:
#line 486 "ned.y"
    {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                }
    break;

  case 56:
#line 491 "ned.y"
    {
                }
    break;

  case 61:
#line 507 "ned.y"
    {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                }
    break;

  case 64:
#line 522 "ned.y"
    {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                }
    break;

  case 65:
#line 527 "ned.y"
    {
                }
    break;

  case 70:
#line 543 "ned.y"
    {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                }
    break;

  case 73:
#line 558 "ned.y"
    {
                  addDisplayString(ps.module,yylsp[-1]);
                }
    break;

  case 76:
#line 571 "ned.y"
    {
                  addDisplayString(ps.module,yylsp[-1]);
                }
    break;

  case 79:
#line 584 "ned.y"
    {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                }
    break;

  case 80:
#line 589 "ned.y"
    {
                }
    break;

  case 83:
#line 600 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 84:
#line 604 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 87:
#line 617 "ned.y"
    {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                }
    break;

  case 88:
#line 622 "ned.y"
    {
                }
    break;

  case 91:
#line 633 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 92:
#line 637 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 94:
#line 650 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                }
    break;

  case 95:
#line 654 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                }
    break;

  case 96:
#line 658 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                }
    break;

  case 97:
#line 662 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                }
    break;

  case 98:
#line 666 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                }
    break;

  case 99:
#line 670 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                }
    break;

  case 100:
#line 674 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                }
    break;

  case 101:
#line 678 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                }
    break;

  case 102:
#line 682 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_XML);
                }
    break;

  case 103:
#line 686 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                }
    break;

  case 106:
#line 699 "ned.y"
    {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                }
    break;

  case 107:
#line 704 "ned.y"
    {
                }
    break;

  case 116:
#line 727 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 117:
#line 732 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 120:
#line 745 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 121:
#line 750 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 124:
#line 764 "ned.y"
    {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                }
    break;

  case 125:
#line 769 "ned.y"
    {
                }
    break;

  case 134:
#line 792 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 135:
#line 797 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 138:
#line 810 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 139:
#line 815 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 142:
#line 829 "ned.y"
    {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                }
    break;

  case 143:
#line 834 "ned.y"
    {
                }
    break;

  case 148:
#line 850 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                }
    break;

  case 149:
#line 856 "ned.y"
    {
                }
    break;

  case 150:
#line 859 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                }
    break;

  case 151:
#line 866 "ned.y"
    {
                }
    break;

  case 152:
#line 869 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                }
    break;

  case 153:
#line 875 "ned.y"
    {
                }
    break;

  case 154:
#line 878 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                }
    break;

  case 155:
#line 885 "ned.y"
    {
                }
    break;

  case 159:
#line 903 "ned.y"
    {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                }
    break;

  case 160:
#line 908 "ned.y"
    {
                }
    break;

  case 165:
#line 924 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                }
    break;

  case 166:
#line 930 "ned.y"
    {
                }
    break;

  case 167:
#line 933 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                }
    break;

  case 168:
#line 940 "ned.y"
    {
                }
    break;

  case 169:
#line 943 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                }
    break;

  case 170:
#line 949 "ned.y"
    {
                }
    break;

  case 171:
#line 952 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                }
    break;

  case 172:
#line 959 "ned.y"
    {
                }
    break;

  case 178:
#line 982 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                }
    break;

  case 179:
#line 987 "ned.y"
    {
                }
    break;

  case 180:
#line 990 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                }
    break;

  case 181:
#line 996 "ned.y"
    {
                }
    break;

  case 186:
#line 1012 "ned.y"
    {
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                }
    break;

  case 191:
#line 1031 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                }
    break;

  case 192:
#line 1036 "ned.y"
    {
                }
    break;

  case 193:
#line 1039 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                }
    break;

  case 194:
#line 1045 "ned.y"
    {
                }
    break;

  case 199:
#line 1061 "ned.y"
    {
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                }
    break;

  case 204:
#line 1080 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                }
    break;

  case 205:
#line 1085 "ned.y"
    {
                }
    break;

  case 206:
#line 1088 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                }
    break;

  case 207:
#line 1094 "ned.y"
    {
                }
    break;

  case 212:
#line 1111 "ned.y"
    {
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                }
    break;

  case 217:
#line 1131 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                }
    break;

  case 218:
#line 1136 "ned.y"
    {
                }
    break;

  case 219:
#line 1139 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                }
    break;

  case 220:
#line 1145 "ned.y"
    {
                }
    break;

  case 225:
#line 1162 "ned.y"
    {
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-3]);
                  addExpression(ps.substparam, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparam,yylsp[-3],yylsp[-1]);
                }
    break;

  case 228:
#line 1177 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                }
    break;

  case 229:
#line 1182 "ned.y"
    {
                }
    break;

  case 230:
#line 1185 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                }
    break;

  case 231:
#line 1191 "ned.y"
    {
                }
    break;

  case 236:
#line 1207 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                }
    break;

  case 237:
#line 1214 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                }
    break;

  case 240:
#line 1228 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                }
    break;

  case 241:
#line 1233 "ned.y"
    {
                }
    break;

  case 242:
#line 1236 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                }
    break;

  case 243:
#line 1242 "ned.y"
    {
                }
    break;

  case 248:
#line 1258 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                }
    break;

  case 249:
#line 1265 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                }
    break;

  case 250:
#line 1274 "ned.y"
    {
                  addDisplayString(ps.submod,yylsp[-1]);
                }
    break;

  case 252:
#line 1283 "ned.y"
    {
                  addDisplayString(ps.submod,yylsp[-1]);
                }
    break;

  case 256:
#line 1297 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                }
    break;

  case 257:
#line 1303 "ned.y"
    {
                }
    break;

  case 258:
#line 1306 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                }
    break;

  case 259:
#line 1312 "ned.y"
    {
                }
    break;

  case 266:
#line 1333 "ned.y"
    {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                }
    break;

  case 267:
#line 1338 "ned.y"
    {
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                }
    break;

  case 270:
#line 1352 "ned.y"
    {
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                }
    break;

  case 271:
#line 1362 "ned.y"
    {
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                }
    break;

  case 273:
#line 1370 "ned.y"
    {
                  addDisplayString(ps.conn,yylsp[0]);
                }
    break;

  case 277:
#line 1383 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 278:
#line 1388 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 279:
#line 1393 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 280:
#line 1399 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 283:
#line 1413 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 284:
#line 1419 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                }
    break;

  case 285:
#line 1427 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 286:
#line 1432 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                }
    break;

  case 287:
#line 1436 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                }
    break;

  case 288:
#line 1444 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 289:
#line 1451 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                }
    break;

  case 290:
#line 1457 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                }
    break;

  case 293:
#line 1472 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 294:
#line 1477 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[0]) );
                }
    break;

  case 295:
#line 1484 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 296:
#line 1489 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 297:
#line 1493 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                }
    break;

  case 298:
#line 1501 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 299:
#line 1506 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 300:
#line 1510 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                }
    break;

  case 301:
#line 1519 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                }
    break;

  case 302:
#line 1524 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 303:
#line 1529 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 306:
#line 1543 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                }
    break;

  case 307:
#line 1549 "ned.y"
    {
                }
    break;

  case 308:
#line 1552 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                }
    break;

  case 309:
#line 1558 "ned.y"
    {
                }
    break;

  case 316:
#line 1579 "ned.y"
    {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                }
    break;

  case 317:
#line 1584 "ned.y"
    {
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                }
    break;

  case 320:
#line 1598 "ned.y"
    {
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                }
    break;

  case 321:
#line 1608 "ned.y"
    {
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                }
    break;

  case 323:
#line 1616 "ned.y"
    {
                  addDisplayString(ps.conn,yylsp[0]);
                }
    break;

  case 327:
#line 1629 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 328:
#line 1634 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 329:
#line 1639 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 330:
#line 1645 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 333:
#line 1659 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 334:
#line 1665 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                }
    break;

  case 335:
#line 1673 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 336:
#line 1678 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                }
    break;

  case 337:
#line 1682 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                }
    break;

  case 338:
#line 1690 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 339:
#line 1697 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                }
    break;

  case 340:
#line 1703 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                }
    break;

  case 343:
#line 1718 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 344:
#line 1723 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[0]) );
                }
    break;

  case 345:
#line 1730 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 346:
#line 1735 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 347:
#line 1739 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                }
    break;

  case 348:
#line 1747 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 349:
#line 1752 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 350:
#line 1756 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                }
    break;

  case 351:
#line 1765 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                }
    break;

  case 352:
#line 1770 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 353:
#line 1775 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 355:
#line 1791 "ned.y"
    {
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                }
    break;

  case 356:
#line 1800 "ned.y"
    {
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                }
    break;

  case 357:
#line 1812 "ned.y"
    {
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                }
    break;

  case 358:
#line 1820 "ned.y"
    {
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                }
    break;

  case 359:
#line 1831 "ned.y"
    { yyval = yyvsp[-1]; }
    break;

  case 360:
#line 1837 "ned.y"
    {
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
    break;

  case 361:
#line 1841 "ned.y"
    {
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
    break;

  case 362:
#line 1845 "ned.y"
    {
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
    break;

  case 363:
#line 1854 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); }
    break;

  case 364:
#line 1856 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); }
    break;

  case 365:
#line 1858 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input"); }
    break;

  case 366:
#line 1860 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input"); }
    break;

  case 367:
#line 1865 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-3], yyvsp[-1]); }
    break;

  case 368:
#line 1867 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-1]); }
    break;

  case 370:
#line 1873 "ned.y"
    { yyval = yyvsp[-1]; }
    break;

  case 371:
#line 1876 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); }
    break;

  case 372:
#line 1878 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); }
    break;

  case 373:
#line 1880 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); }
    break;

  case 374:
#line 1882 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); }
    break;

  case 375:
#line 1884 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); }
    break;

  case 376:
#line 1886 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); }
    break;

  case 377:
#line 1890 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); }
    break;

  case 378:
#line 1893 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); }
    break;

  case 379:
#line 1895 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); }
    break;

  case 380:
#line 1897 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); }
    break;

  case 381:
#line 1899 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); }
    break;

  case 382:
#line 1901 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); }
    break;

  case 383:
#line 1903 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); }
    break;

  case 384:
#line 1906 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); }
    break;

  case 385:
#line 1908 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); }
    break;

  case 386:
#line 1910 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); }
    break;

  case 387:
#line 1914 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); }
    break;

  case 388:
#line 1917 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); }
    break;

  case 389:
#line 1919 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); }
    break;

  case 390:
#line 1921 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); }
    break;

  case 391:
#line 1925 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); }
    break;

  case 392:
#line 1927 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); }
    break;

  case 393:
#line 1929 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); }
    break;

  case 394:
#line 1931 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); }
    break;

  case 395:
#line 1934 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); }
    break;

  case 396:
#line 1936 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); }
    break;

  case 397:
#line 1938 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); }
    break;

  case 398:
#line 1940 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); }
    break;

  case 399:
#line 1942 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-9]), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); }
    break;

  case 405:
#line 1955 "ned.y"
    {
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                }
    break;

  case 406:
#line 1960 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                }
    break;

  case 407:
#line 1965 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                }
    break;

  case 408:
#line 1971 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                }
    break;

  case 409:
#line 1977 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                }
    break;

  case 411:
#line 1989 "ned.y"
    { yyval = createConst(NED_CONST_BOOL, "true"); }
    break;

  case 412:
#line 1991 "ned.y"
    { yyval = createConst(NED_CONST_BOOL, "false"); }
    break;

  case 414:
#line 2000 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("index"); }
    break;

  case 415:
#line 2002 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("index"); }
    break;

  case 416:
#line 2004 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); }
    break;

  case 417:
#line 2009 "ned.y"
    { yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); }
    break;

  case 418:
#line 2014 "ned.y"
    { yyval = createConst(NED_CONST_INT, toString(yylsp[0])); }
    break;

  case 419:
#line 2016 "ned.y"
    { yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); }
    break;

  case 420:
#line 2018 "ned.y"
    { yyval = createTimeConst(toString(yylsp[0])); }
    break;

  case 425:
#line 2033 "ned.y"
    {
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(yylsp[-1])));
                  setComments(ps.cplusplus,yylsp[-2],yylsp[-1]);
                }
    break;

  case 426:
#line 2042 "ned.y"
    {
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(yylsp[-1]));
                  setComments(ps.structdecl,yylsp[-2],yylsp[-1]);
                }
    break;

  case 427:
#line 2051 "ned.y"
    {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(yylsp[-1]));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,yylsp[-2],yylsp[-1]);
                }
    break;

  case 428:
#line 2058 "ned.y"
    {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(yylsp[-1]));
                  setComments(ps.classdecl,yylsp[-3],yylsp[-2]);
                }
    break;

  case 429:
#line 2068 "ned.y"
    {
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString(yylsp[-1]));
                  setComments(ps.enumdecl,yylsp[-2],yylsp[-1]);
                }
    break;

  case 430:
#line 2077 "ned.y"
    {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
    break;

  case 431:
#line 2084 "ned.y"
    {
                  setTrailingComment(ps.enump,yylsp[-1]);
                }
    break;

  case 432:
#line 2088 "ned.y"
    {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
    break;

  case 433:
#line 2096 "ned.y"
    {
                  setTrailingComment(ps.enump,yylsp[-1]);
                }
    break;

  case 438:
#line 2113 "ned.y"
    {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-1],yylsp[-1]);
                }
    break;

  case 439:
#line 2119 "ned.y"
    {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-3],yylsp[-1]);
                }
    break;

  case 440:
#line 2129 "ned.y"
    {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                }
    break;

  case 441:
#line 2135 "ned.y"
    {
                  setTrailingComment(ps.messagep,yylsp[-1]);
                }
    break;

  case 442:
#line 2139 "ned.y"
    {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                }
    break;

  case 443:
#line 2146 "ned.y"
    {
                  setTrailingComment(ps.messagep,yylsp[-1]);
                }
    break;

  case 444:
#line 2153 "ned.y"
    {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                }
    break;

  case 445:
#line 2159 "ned.y"
    {
                  setTrailingComment(ps.classp,yylsp[-1]);
                }
    break;

  case 446:
#line 2163 "ned.y"
    {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                }
    break;

  case 447:
#line 2170 "ned.y"
    {
                  setTrailingComment(ps.classp,yylsp[-1]);
                }
    break;

  case 448:
#line 2177 "ned.y"
    {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                }
    break;

  case 449:
#line 2183 "ned.y"
    {
                  setTrailingComment(ps.structp,yylsp[-1]);
                }
    break;

  case 450:
#line 2187 "ned.y"
    {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                }
    break;

  case 451:
#line 2194 "ned.y"
    {
                  setTrailingComment(ps.structp,yylsp[-1]);
                }
    break;

  case 452:
#line 2201 "ned.y"
    {
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                }
    break;

  case 459:
#line 2221 "ned.y"
    {
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                }
    break;

  case 466:
#line 2240 "ned.y"
    {
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                }
    break;

  case 473:
#line 2260 "ned.y"
    {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                }
    break;

  case 474:
#line 2266 "ned.y"
    {
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                }
    break;

  case 475:
#line 2270 "ned.y"
    {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(true);
                }
    break;

  case 476:
#line 2277 "ned.y"
    {
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                }
    break;

  case 490:
#line 2304 "ned.y"
    {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                }
    break;

  case 491:
#line 2309 "ned.y"
    {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                }
    break;

  case 492:
#line 2314 "ned.y"
    {
                  ps.field->setIsVector(true);
                }
    break;

  case 494:
#line 2322 "ned.y"
    {
                  ps.field->setEnumName(toString(yylsp[-1]));
                }
    break;

  case 496:
#line 2330 "ned.y"
    {
                  ps.field->setDefaultValue(toString(yylsp[0]));
                }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 4267 "ned.tab.c"

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


#line 2359 "ned.y"


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


