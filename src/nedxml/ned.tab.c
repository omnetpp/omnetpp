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
     IO_INTERFACES = 286,
     IFPAIR = 287,
     INTCONSTANT = 288,
     REALCONSTANT = 289,
     NAME = 290,
     STRINGCONSTANT = 291,
     CHARCONSTANT = 292,
     TRUE_ = 293,
     FALSE_ = 294,
     INPUT_ = 295,
     REF = 296,
     ANCESTOR = 297,
     CONSTDECL = 298,
     NUMERICTYPE = 299,
     STRINGTYPE = 300,
     BOOLTYPE = 301,
     ANYTYPE = 302,
     CPPINCLUDE = 303,
     SYSINCFILENAME = 304,
     STRUCT = 305,
     COBJECT = 306,
     NONCOBJECT = 307,
     ENUM = 308,
     EXTENDS = 309,
     MESSAGE = 310,
     CLASS = 311,
     FIELDS = 312,
     PROPERTIES = 313,
     ABSTRACT = 314,
     CHARTYPE = 315,
     SHORTTYPE = 316,
     INTTYPE = 317,
     LONGTYPE = 318,
     DOUBLETYPE = 319,
     SIZEOF = 320,
     SUBMODINDEX = 321,
     EQ = 322,
     NE = 323,
     GT = 324,
     GE = 325,
     LS = 326,
     LE = 327,
     AND = 328,
     OR = 329,
     XOR = 330,
     NOT = 331,
     BIN_AND = 332,
     BIN_OR = 333,
     BIN_XOR = 334,
     BIN_COMPL = 335,
     SHIFT_LEFT = 336,
     SHIFT_RIGHT = 337,
     INVALID_CHAR = 338,
     UMIN = 339
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
#define IO_INTERFACES 286
#define IFPAIR 287
#define INTCONSTANT 288
#define REALCONSTANT 289
#define NAME 290
#define STRINGCONSTANT 291
#define CHARCONSTANT 292
#define TRUE_ 293
#define FALSE_ 294
#define INPUT_ 295
#define REF 296
#define ANCESTOR 297
#define CONSTDECL 298
#define NUMERICTYPE 299
#define STRINGTYPE 300
#define BOOLTYPE 301
#define ANYTYPE 302
#define CPPINCLUDE 303
#define SYSINCFILENAME 304
#define STRUCT 305
#define COBJECT 306
#define NONCOBJECT 307
#define ENUM 308
#define EXTENDS 309
#define MESSAGE 310
#define CLASS 311
#define FIELDS 312
#define PROPERTIES 313
#define ABSTRACT 314
#define CHARTYPE 315
#define SHORTTYPE 316
#define INTTYPE 317
#define LONGTYPE 318
#define DOUBLETYPE 319
#define SIZEOF 320
#define SUBMODINDEX 321
#define EQ 322
#define NE 323
#define GT 324
#define GE 325
#define LS 326
#define LE 327
#define AND 328
#define OR 329
#define XOR 330
#define NOT 331
#define BIN_AND 332
#define BIN_OR 333
#define BIN_XOR 334
#define BIN_COMPL 335
#define SHIFT_LEFT 336
#define SHIFT_RIGHT 337
#define INVALID_CHAR 338
#define UMIN 339




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

static YYLTYPE NULLPOS={0,0,0,0,0,0};

static const char *sourcefilename;

NEDParser *np;

struct ParserState
{
    bool parseExpressions;
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
    CppincludeNode *cppinclude;
    CppStructNode *cppstruct;
    CppCobjectNode *cppcobject;
    CppNoncobjectNode *cppnoncobject;
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
SubmoduleNode *addSubmodule(NEDElement *submods, YYLTYPE namepos, YYLTYPE typepos,YYLTYPE likepos);
GatesizeNode *addGateSize(NEDElement *gatesizes, YYLTYPE namepos);
SubstparamNode *addSubstparam(NEDElement *substparams, YYLTYPE namepos);
SubstmachineNode *addSubstmachine(NEDElement *substmachines, YYLTYPE namepos);
ConnAttrNode *addConnAttr(NEDElement *conn, const char *attrname);
LoopVarNode *addLoopVar(NEDElement *forloop, YYLTYPE varnamepos);
NetworkNode *addNetwork(NEDElement *nedfile, YYLTYPE namepos, YYLTYPE typepos, YYLTYPE likepos);
DisplayStringNode *addDisplayString(NEDElement *parent, YYLTYPE dispstrpos);

YYLTYPE trimBrackets(YYLTYPE vectorpos);
YYLTYPE trimQuotes(YYLTYPE vectorpos);
void swapAttributes(NEDElement *node, const char *attr1, const char *attr2);
void swapExpressionChildren(NEDElement *node, const char *attr1, const char *attr2);
void swapConnection(NEDElement *conn);

const char *toString(YYLTYPE);
const char *toString(long);

ExpressionNode *createExpression(NEDElement *expr);
OperatorNode *createOperator(const char *op, NEDElement *operand1, NEDElement *operand2=NULL, NEDElement *operand3=NULL);
FunctionNode *createFunction(const char *funcname, NEDElement *arg1=NULL, NEDElement *arg2=NULL, NEDElement *arg3=NULL);
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
#line 406 "ned.tab.c"

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
#define YYLAST   953

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  103
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  249
/* YYNRULES -- Number of rules. */
#define YYNRULES  493
/* YYNRULES -- Number of states. */
#define YYNSTATES  849

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   339

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    90,     2,     2,
     101,   102,    88,    86,    94,    87,   100,    89,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    85,    93,
       2,    97,     2,    84,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    98,     2,    99,    91,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    95,     2,    96,     2,     2,     2,     2,
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
      75,    76,    77,    78,    79,    80,    81,    82,    83,    92
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
     287,   291,   295,   299,   301,   302,   303,   308,   310,   311,
     316,   320,   325,   329,   333,   335,   339,   341,   345,   347,
     351,   353,   355,   356,   357,   362,   364,   365,   370,   374,
     379,   383,   387,   389,   393,   395,   399,   401,   405,   407,
     409,   410,   411,   416,   418,   419,   422,   424,   425,   433,
     434,   443,   444,   454,   455,   466,   470,   472,   473,   474,
     479,   481,   482,   485,   487,   488,   496,   497,   506,   507,
     517,   518,   529,   533,   535,   536,   539,   541,   542,   547,
     548,   555,   558,   559,   563,   565,   567,   569,   570,   573,
     575,   576,   581,   582,   589,   592,   593,   597,   599,   601,
     603,   604,   607,   609,   610,   615,   616,   623,   626,   627,
     631,   633,   637,   639,   640,   643,   645,   646,   651,   652,
     659,   661,   662,   665,   667,   672,   675,   676,   677,   682,
     683,   690,   693,   694,   698,   700,   703,   705,   708,   709,
     710,   715,   716,   723,   726,   727,   731,   733,   736,   738,
     743,   744,   749,   750,   752,   753,   754,   760,   761,   766,
     768,   769,   772,   774,   776,   778,   779,   780,   789,   793,
     795,   801,   804,   805,   808,   809,   812,   814,   821,   830,
     837,   846,   850,   852,   855,   857,   860,   862,   865,   867,
     871,   873,   876,   878,   881,   883,   886,   888,   890,   893,
     897,   899,   900,   901,   907,   908,   913,   915,   916,   919,
     921,   923,   925,   926,   927,   936,   940,   942,   948,   951,
     952,   955,   956,   959,   961,   968,   977,   984,   993,   997,
     999,  1002,  1004,  1007,  1009,  1012,  1014,  1018,  1020,  1023,
    1025,  1028,  1030,  1033,  1035,  1037,  1040,  1044,  1049,  1055,
    1063,  1066,  1073,  1079,  1087,  1091,  1093,  1095,  1102,  1107,
    1111,  1113,  1115,  1119,  1123,  1127,  1131,  1135,  1139,  1143,
    1146,  1150,  1154,  1158,  1162,  1166,  1170,  1174,  1178,  1182,
    1185,  1189,  1193,  1197,  1200,  1204,  1208,  1214,  1218,  1223,
    1230,  1239,  1241,  1243,  1245,  1247,  1249,  1251,  1254,  1258,
    1262,  1265,  1267,  1269,  1271,  1273,  1275,  1279,  1284,  1286,
    1288,  1290,  1294,  1298,  1301,  1304,  1307,  1310,  1314,  1318,
    1322,  1323,  1331,  1332,  1342,  1344,  1345,  1348,  1350,  1353,
    1358,  1359,  1368,  1369,  1380,  1381,  1390,  1391,  1402,  1403,
    1412,  1413,  1424,  1425,  1430,  1431,  1433,  1434,  1437,  1439,
    1444,  1446,  1448,  1450,  1452,  1454,  1456,  1457,  1462,  1463,
    1465,  1466,  1469,  1471,  1472,  1480,  1481,  1490,  1492,  1495,
    1497,  1499,  1501,  1503,  1505,  1507,  1509,  1513,  1516,  1517,
    1522,  1523,  1526,  1527,  1529,  1531,  1533,  1535,  1537,  1539,
    1541,  1543,  1544,  1546
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     104,     0,    -1,   105,    -1,   105,   106,    -1,    -1,   107,
      -1,   116,    -1,   123,    -1,   128,    -1,   299,    -1,   111,
      -1,   120,    -1,   125,    -1,   296,    -1,   313,    -1,   314,
      -1,   315,    -1,   316,    -1,   317,    -1,   323,    -1,   326,
      -1,   329,    -1,    -1,     3,   108,   109,    93,    -1,   109,
      94,   110,    -1,   110,    -1,    36,    -1,   112,   113,   115,
      -1,     5,    35,    -1,    -1,   114,    -1,   114,    35,   302,
     350,    -1,    35,   302,   350,    -1,    26,    35,   350,    -1,
      26,   350,    -1,   117,    95,   118,    96,   350,    -1,     5,
      35,    -1,   119,    -1,    -1,   119,    35,    97,   302,    93,
      -1,    35,    97,   302,    93,    -1,   121,   130,   146,   158,
     122,    -1,     4,    35,    -1,    24,    35,   350,    -1,    24,
     350,    -1,   124,    95,   136,   151,   167,    96,   350,    -1,
       4,    35,    -1,   126,   130,   146,   158,   176,   246,   142,
     127,    -1,     6,    35,    -1,    25,    35,   350,    -1,    25,
     350,    -1,   129,    95,   136,   151,   167,   187,   271,   144,
      96,   350,    -1,     6,    35,    -1,   131,    -1,    -1,    -1,
      29,    85,   132,   133,    -1,   134,    93,    -1,    -1,   134,
      94,   135,    -1,   135,    -1,    35,    -1,   137,    -1,    -1,
      -1,    29,    85,   138,   139,    -1,   140,    93,    -1,    -1,
     140,    94,   141,    -1,   141,    -1,    35,    -1,   143,    -1,
      -1,    12,    85,    36,    93,    -1,   145,    -1,    -1,    12,
      85,    36,    93,    -1,   147,    -1,    -1,    -1,     7,    85,
     148,   149,    -1,   150,    93,    -1,    -1,   150,    94,   156,
      -1,   156,    -1,   152,    -1,    -1,    -1,     7,    85,   153,
     154,    -1,   155,    93,    -1,    -1,   155,    93,   157,    -1,
     157,    -1,   157,    -1,    35,    -1,    35,    85,    44,    -1,
      43,    35,    -1,    35,    85,    43,    -1,    35,    85,    43,
      44,    -1,    35,    85,    44,    43,    -1,    35,    85,    45,
      -1,    35,    85,    46,    -1,    35,    85,    47,    -1,   159,
      -1,    -1,    -1,     8,    85,   160,   161,    -1,   162,    -1,
      -1,   162,    13,   163,    93,    -1,    13,   163,    93,    -1,
     162,    14,   165,    93,    -1,    14,   165,    93,    -1,   163,
      94,   164,    -1,   164,    -1,    35,    98,    99,    -1,    35,
      -1,   165,    94,   166,    -1,   166,    -1,    35,    98,    99,
      -1,    35,    -1,   168,    -1,    -1,    -1,     8,    85,   169,
     170,    -1,   171,    -1,    -1,   171,    13,   172,    93,    -1,
      13,   172,    93,    -1,   171,    14,   174,    93,    -1,    14,
     174,    93,    -1,   172,    94,   173,    -1,   173,    -1,    35,
      98,    99,    -1,    35,    -1,   174,    94,   175,    -1,   175,
      -1,    35,    98,    99,    -1,    35,    -1,   177,    -1,    -1,
      -1,    10,    85,   178,   179,    -1,   180,    -1,    -1,   180,
     181,    -1,   181,    -1,    -1,    35,    85,    35,   350,   182,
     198,   186,    -1,    -1,    35,    85,    35,   301,   350,   183,
     198,   186,    -1,    -1,    35,    85,    35,    22,    35,   350,
     184,   198,   186,    -1,    -1,    35,    85,    35,   301,    22,
      35,   350,   185,   198,   186,    -1,   214,   230,   244,    -1,
     188,    -1,    -1,    -1,    10,    85,   189,   190,    -1,   191,
      -1,    -1,   191,   192,    -1,   192,    -1,    -1,    35,    85,
      35,   350,   193,   206,   197,    -1,    -1,    35,    85,    35,
     301,   350,   194,   206,   197,    -1,    -1,    35,    85,    35,
      22,    35,   350,   195,   206,   197,    -1,    -1,    35,    85,
      35,   301,    22,    35,   350,   196,   206,   197,    -1,   222,
     237,   245,    -1,   199,    -1,    -1,   199,   200,    -1,   200,
      -1,    -1,    30,    85,   201,   203,    -1,    -1,    30,    21,
     302,    85,   202,   203,    -1,   204,    93,    -1,    -1,   204,
      94,   205,    -1,   205,    -1,    35,    -1,   207,    -1,    -1,
     207,   208,    -1,   208,    -1,    -1,    30,    85,   209,   211,
      -1,    -1,    30,    21,   302,    85,   210,   211,    -1,   212,
      93,    -1,    -1,   212,    94,   213,    -1,   213,    -1,    35,
      -1,   215,    -1,    -1,   215,   216,    -1,   216,    -1,    -1,
       7,    85,   217,   219,    -1,    -1,     7,    21,   302,    85,
     218,   219,    -1,   220,    93,    -1,    -1,   220,    94,   221,
      -1,   221,    -1,    35,    97,   302,    -1,   223,    -1,    -1,
     223,   224,    -1,   224,    -1,    -1,     7,    85,   225,   227,
      -1,    -1,     7,    21,   302,    85,   226,   227,    -1,   228,
      -1,    -1,   228,   229,    -1,   229,    -1,    35,    97,   302,
      93,    -1,   230,   231,    -1,    -1,    -1,     9,    85,   232,
     234,    -1,    -1,     9,    21,   302,    85,   233,   234,    -1,
     235,    93,    -1,    -1,   235,    94,   236,    -1,   236,    -1,
      35,   301,    -1,    35,    -1,   237,   238,    -1,    -1,    -1,
       9,    85,   239,   241,    -1,    -1,     9,    21,   302,    85,
     240,   241,    -1,   242,    93,    -1,    -1,   242,    94,   243,
      -1,   243,    -1,    35,   301,    -1,    35,    -1,    12,    85,
      36,    93,    -1,    -1,    12,    85,    36,    93,    -1,    -1,
     247,    -1,    -1,    -1,    11,    15,    85,   248,   250,    -1,
      -1,    11,    85,   249,   250,    -1,   251,    -1,    -1,   251,
     252,    -1,   252,    -1,   253,    -1,   261,    -1,    -1,    -1,
      18,   254,   256,    20,   255,   260,    28,   350,    -1,   257,
      94,   256,    -1,   257,    -1,    35,    97,   302,    19,   302,
      -1,    21,   302,    -1,    -1,    12,    36,    -1,    -1,   260,
     261,    -1,   261,    -1,   262,    17,   266,   258,   259,   351,
      -1,   262,    17,   270,    17,   266,   258,   259,   351,    -1,
     262,    16,   266,   258,   259,   351,    -1,   262,    16,   270,
      16,   266,   258,   259,   351,    -1,   263,   100,   264,    -1,
     265,    -1,    35,   301,    -1,    35,    -1,    35,   301,    -1,
      35,    -1,    35,   301,    -1,    35,    -1,   267,   100,   268,
      -1,   269,    -1,    35,   301,    -1,    35,    -1,    35,   301,
      -1,    35,    -1,    35,   301,    -1,    35,    -1,    35,    -1,
      35,   302,    -1,   270,    35,   302,    -1,   272,    -1,    -1,
      -1,    11,    15,    85,   273,   275,    -1,    -1,    11,    85,
     274,   275,    -1,   276,    -1,    -1,   276,   277,    -1,   277,
      -1,   278,    -1,   286,    -1,    -1,    -1,    18,   279,   281,
      20,   280,   285,    28,    93,    -1,   282,    94,   281,    -1,
     282,    -1,    35,    97,   302,    19,   302,    -1,    21,   302,
      -1,    -1,    12,    36,    -1,    -1,   285,   286,    -1,   286,
      -1,   287,    17,   291,   283,   284,    93,    -1,   287,    17,
     295,    17,   291,   283,   284,    93,    -1,   287,    16,   291,
     283,   284,    93,    -1,   287,    16,   295,    16,   291,   283,
     284,    93,    -1,   288,   100,   289,    -1,   290,    -1,    35,
     301,    -1,    35,    -1,    35,   301,    -1,    35,    -1,    35,
     301,    -1,    35,    -1,   292,   100,   293,    -1,   294,    -1,
      35,   301,    -1,    35,    -1,    35,   301,    -1,    35,    -1,
      35,   301,    -1,    35,    -1,    35,    -1,    35,   302,    -1,
     295,    35,   302,    -1,   297,   198,   214,   298,    -1,    23,
      35,    85,    35,   350,    -1,    23,    35,    85,    35,    22,
      35,   350,    -1,    27,   350,    -1,   300,    95,   206,   222,
      96,   350,    -1,    23,    35,    85,    35,   350,    -1,    23,
      35,    85,    35,    22,    35,   350,    -1,    98,   302,    99,
      -1,   304,    -1,   303,    -1,    40,   101,   304,    94,   304,
     102,    -1,    40,   101,   304,   102,    -1,    40,   101,   102,
      -1,    40,    -1,   305,    -1,   101,   304,   102,    -1,   304,
      86,   304,    -1,   304,    87,   304,    -1,   304,    88,   304,
      -1,   304,    89,   304,    -1,   304,    90,   304,    -1,   304,
      91,   304,    -1,    87,   304,    -1,   304,    67,   304,    -1,
     304,    68,   304,    -1,   304,    69,   304,    -1,   304,    70,
     304,    -1,   304,    71,   304,    -1,   304,    72,   304,    -1,
     304,    73,   304,    -1,   304,    74,   304,    -1,   304,    75,
     304,    -1,    76,   304,    -1,   304,    77,   304,    -1,   304,
      78,   304,    -1,   304,    79,   304,    -1,    80,   304,    -1,
     304,    81,   304,    -1,   304,    82,   304,    -1,   304,    84,
     304,    85,   304,    -1,    35,   101,   102,    -1,    35,   101,
     304,   102,    -1,    35,   101,   304,    94,   304,   102,    -1,
      35,   101,   304,    94,   304,    94,   304,   102,    -1,   306,
      -1,   307,    -1,   308,    -1,   309,    -1,   310,    -1,    35,
      -1,    41,    35,    -1,    41,    42,    35,    -1,    42,    41,
      35,    -1,    42,    35,    -1,    36,    -1,    38,    -1,    39,
      -1,   311,    -1,    66,    -1,    66,   101,   102,    -1,    65,
     101,    35,   102,    -1,    33,    -1,    34,    -1,   312,    -1,
     312,    33,    35,    -1,   312,    34,    35,    -1,    33,    35,
      -1,    34,    35,    -1,    48,    36,    -1,    48,    49,    -1,
      50,    35,    93,    -1,    51,    35,    93,    -1,    52,    35,
      93,    -1,    -1,    53,    35,    95,   318,   320,    96,    93,
      -1,    -1,    53,    35,    54,    35,    95,   319,   320,    96,
      93,    -1,   321,    -1,    -1,   321,   322,    -1,   322,    -1,
      35,    93,    -1,    35,    97,    33,    93,    -1,    -1,    55,
      35,    95,   324,   332,   338,    96,    93,    -1,    -1,    55,
      35,    54,    35,    95,   325,   332,   338,    96,    93,    -1,
      -1,    56,    35,    95,   327,   332,   338,    96,    93,    -1,
      -1,    56,    35,    54,    35,    95,   328,   332,   338,    96,
      93,    -1,    -1,    50,    35,    95,   330,   332,   338,    96,
      93,    -1,    -1,    50,    35,    54,    35,    95,   331,   332,
     338,    96,    93,    -1,    -1,    58,    85,   333,   334,    -1,
      -1,   335,    -1,    -1,   335,   336,    -1,   336,    -1,    35,
      97,   337,    93,    -1,    36,    -1,    33,    -1,    34,    -1,
     312,    -1,    38,    -1,    39,    -1,    -1,    57,    85,   339,
     340,    -1,    -1,   341,    -1,    -1,   341,   342,    -1,   342,
      -1,    -1,   345,    35,   343,   346,   347,   348,    93,    -1,
      -1,    59,   345,    35,   344,   346,   347,   348,    93,    -1,
      35,    -1,    35,    88,    -1,    60,    -1,    61,    -1,    62,
      -1,    63,    -1,    64,    -1,    45,    -1,    46,    -1,    98,
      33,    99,    -1,    98,    99,    -1,    -1,    53,   101,    35,
     102,    -1,    -1,    97,   349,    -1,    -1,    36,    -1,    37,
      -1,    33,    -1,    34,    -1,   312,    -1,    38,    -1,    39,
      -1,    93,    -1,    -1,    94,    -1,    93,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   225,   225,   229,   230,   234,   235,   236,   237,   238,
     240,   241,   242,   243,   245,   246,   247,   248,   249,   250,
     251,   252,   260,   259,   271,   272,   276,   288,   292,   301,
     302,   306,   312,   321,   325,   335,   344,   353,   354,   358,
     364,   376,   384,   393,   397,   407,   418,   431,   442,   451,
     455,   465,   479,   492,   493,   498,   497,   508,   509,   513,
     514,   518,   530,   531,   536,   535,   546,   547,   551,   552,
     556,   568,   569,   573,   583,   584,   588,   598,   599,   604,
     603,   614,   615,   619,   623,   633,   634,   639,   638,   649,
     650,   654,   658,   669,   673,   677,   681,   685,   689,   693,
     697,   701,   705,   715,   716,   721,   720,   731,   732,   736,
     737,   738,   739,   743,   744,   748,   753,   761,   762,   766,
     771,   782,   783,   788,   787,   798,   799,   803,   804,   805,
     806,   810,   811,   815,   820,   828,   829,   833,   838,   849,
     850,   855,   854,   865,   866,   870,   871,   876,   875,   885,
     884,   895,   894,   904,   903,   916,   925,   926,   931,   930,
     941,   942,   946,   947,   952,   951,   961,   960,   971,   970,
     980,   979,   992,  1001,  1002,  1006,  1007,  1012,  1011,  1020,
    1019,  1031,  1032,  1036,  1037,  1041,  1052,  1053,  1057,  1058,
    1063,  1062,  1071,  1070,  1082,  1083,  1087,  1088,  1092,  1103,
    1104,  1108,  1109,  1114,  1113,  1122,  1121,  1134,  1135,  1139,
    1140,  1144,  1156,  1157,  1161,  1162,  1167,  1166,  1175,  1174,
    1187,  1188,  1192,  1193,  1197,  1209,  1210,  1215,  1214,  1223,
    1222,  1234,  1235,  1239,  1240,  1244,  1251,  1262,  1263,  1268,
    1267,  1276,  1275,  1287,  1288,  1292,  1293,  1297,  1304,  1315,
    1319,  1326,  1330,  1337,  1338,  1343,  1342,  1352,  1351,  1363,
    1364,  1368,  1369,  1373,  1374,  1379,  1383,  1378,  1395,  1396,
    1400,  1410,  1414,  1418,  1422,  1426,  1427,  1431,  1436,  1441,
    1447,  1456,  1457,  1461,  1467,  1475,  1480,  1487,  1494,  1503,
    1504,  1508,  1513,  1520,  1525,  1532,  1537,  1545,  1550,  1555,
    1566,  1567,  1572,  1571,  1581,  1580,  1592,  1593,  1597,  1598,
    1602,  1603,  1608,  1612,  1607,  1624,  1625,  1629,  1639,  1643,
    1647,  1651,  1655,  1656,  1660,  1665,  1670,  1676,  1685,  1686,
    1690,  1696,  1704,  1709,  1716,  1723,  1732,  1733,  1737,  1742,
    1749,  1754,  1761,  1766,  1774,  1779,  1784,  1795,  1802,  1808,
    1817,  1828,  1839,  1845,  1858,  1863,  1868,  1879,  1881,  1883,
    1885,  1890,  1891,  1894,  1896,  1898,  1900,  1902,  1904,  1907,
    1911,  1913,  1915,  1917,  1919,  1921,  1924,  1926,  1928,  1931,
    1935,  1937,  1939,  1942,  1945,  1947,  1949,  1952,  1954,  1956,
    1958,  1963,  1964,  1965,  1966,  1967,  1971,  1976,  1981,  1987,
    1993,  2001,  2006,  2008,  2013,  2017,  2019,  2021,  2026,  2028,
    2030,  2036,  2037,  2038,  2039,  2047,  2053,  2062,  2071,  2080,
    2090,  2089,  2101,  2100,  2115,  2116,  2120,  2121,  2125,  2130,
    2140,  2139,  2150,  2149,  2164,  2163,  2174,  2173,  2188,  2187,
    2198,  2197,  2212,  2211,  2217,  2221,  2222,  2226,  2227,  2231,
    2241,  2242,  2243,  2244,  2245,  2246,  2251,  2250,  2256,  2260,
    2261,  2265,  2266,  2271,  2270,  2281,  2280,  2294,  2295,  2296,
    2297,  2298,  2299,  2300,  2301,  2302,  2306,  2311,  2315,  2319,
    2323,  2327,  2331,  2335,  2336,  2337,  2338,  2339,  2340,  2341,
    2344,  2344,  2346,  2346
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
  "ENDCHANNEL", "ENDNETWORK", "ENDFOR", "MACHINES", "ON", "IO_INTERFACES", 
  "IFPAIR", "INTCONSTANT", "REALCONSTANT", "NAME", "STRINGCONSTANT", 
  "CHARCONSTANT", "TRUE_", "FALSE_", "INPUT_", "REF", "ANCESTOR", 
  "CONSTDECL", "NUMERICTYPE", "STRINGTYPE", "BOOLTYPE", "ANYTYPE", 
  "CPPINCLUDE", "SYSINCFILENAME", "STRUCT", "COBJECT", "NONCOBJECT", 
  "ENUM", "EXTENDS", "MESSAGE", "CLASS", "FIELDS", "PROPERTIES", 
  "ABSTRACT", "CHARTYPE", "SHORTTYPE", "INTTYPE", "LONGTYPE", 
  "DOUBLETYPE", "SIZEOF", "SUBMODINDEX", "EQ", "NE", "GT", "GE", "LS", 
  "LE", "AND", "OR", "XOR", "NOT", "BIN_AND", "BIN_OR", "BIN_XOR", 
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
  "loopconnection_old", "@32", "@33", "loopvarlist_old", "loopvar_old", 
  "opt_conncondition_old", "opt_conn_displaystr_old", 
  "notloopconnections_old", "notloopconnection_old", "leftgatespec_old", 
  "leftmod_old", "leftgate_old", "parentleftgate_old", 
  "rightgatespec_old", "rightmod_old", "rightgate_old", 
  "parentrightgate_old", "channeldescr_old", "opt_connblock", "connblock", 
  "@34", "@35", "opt_connections", "connections", "connection", 
  "loopconnection", "@36", "@37", "loopvarlist", "loopvar", 
  "opt_conncondition", "opt_conn_displaystr", "notloopconnections", 
  "notloopconnection", "leftgatespec", "leftmod", "leftgate", 
  "parentleftgate", "rightgatespec", "rightmod", "rightgate", 
  "parentrightgate", "channeldescr", "networkdefinition_old", 
  "networkheader_old", "endnetwork_old", "networkdefinition", 
  "networkheader", "vector", "expression", "inputvalue", "expr", 
  "simple_expr", "parameter_expr", "string_expr", "boolconst_expr", 
  "numconst_expr", "special_expr", "numconst", "timeconstant", 
  "cppinclude", "cppstruct", "cppcobject", "cppnoncobject", "enum", "@38", 
  "@39", "opt_enumfields", "enumfields", "enumfield", "message", "@40", 
  "@41", "class", "@42", "@43", "struct", "@44", "@45", 
  "opt_propertiesblock", "@46", "opt_properties", "properties", 
  "property", "propertyvalue", "opt_fieldsblock", "@47", "opt_fields", 
  "fields", "field", "@48", "@49", "fielddatatype", "opt_fieldvector", 
  "opt_fieldenum", "opt_fieldvalue", "fieldvalue", "opt_semicolon", 
  "comma_or_semicolon", 0
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
     335,   336,   337,   338,    63,    58,    43,    45,    42,    47,
      37,    94,   339,    59,    44,   123,   125,    61,    91,    93,
      46,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   103,   104,   105,   105,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   108,   107,   109,   109,   110,   111,   112,   113,
     113,   114,   114,   115,   115,   116,   117,   118,   118,   119,
     119,   120,   121,   122,   122,   123,   124,   125,   126,   127,
     127,   128,   129,   130,   130,   132,   131,   133,   133,   134,
     134,   135,   136,   136,   138,   137,   139,   139,   140,   140,
     141,   142,   142,   143,   144,   144,   145,   146,   146,   148,
     147,   149,   149,   150,   150,   151,   151,   153,   152,   154,
     154,   155,   155,   156,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   158,   158,   160,   159,   161,   161,   162,
     162,   162,   162,   163,   163,   164,   164,   165,   165,   166,
     166,   167,   167,   169,   168,   170,   170,   171,   171,   171,
     171,   172,   172,   173,   173,   174,   174,   175,   175,   176,
     176,   178,   177,   179,   179,   180,   180,   182,   181,   183,
     181,   184,   181,   185,   181,   186,   187,   187,   189,   188,
     190,   190,   191,   191,   193,   192,   194,   192,   195,   192,
     196,   192,   197,   198,   198,   199,   199,   201,   200,   202,
     200,   203,   203,   204,   204,   205,   206,   206,   207,   207,
     209,   208,   210,   208,   211,   211,   212,   212,   213,   214,
     214,   215,   215,   217,   216,   218,   216,   219,   219,   220,
     220,   221,   222,   222,   223,   223,   225,   224,   226,   224,
     227,   227,   228,   228,   229,   230,   230,   232,   231,   233,
     231,   234,   234,   235,   235,   236,   236,   237,   237,   239,
     238,   240,   238,   241,   241,   242,   242,   243,   243,   244,
     244,   245,   245,   246,   246,   248,   247,   249,   247,   250,
     250,   251,   251,   252,   252,   254,   255,   253,   256,   256,
     257,   258,   258,   259,   259,   260,   260,   261,   261,   261,
     261,   262,   262,   263,   263,   264,   264,   265,   265,   266,
     266,   267,   267,   268,   268,   269,   269,   270,   270,   270,
     271,   271,   273,   272,   274,   272,   275,   275,   276,   276,
     277,   277,   279,   280,   278,   281,   281,   282,   283,   283,
     284,   284,   285,   285,   286,   286,   286,   286,   287,   287,
     288,   288,   289,   289,   290,   290,   291,   291,   292,   292,
     293,   293,   294,   294,   295,   295,   295,   296,   297,   297,
     298,   299,   300,   300,   301,   302,   302,   303,   303,   303,
     303,   304,   304,   304,   304,   304,   304,   304,   304,   304,
     304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
     304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
     304,   305,   305,   305,   305,   305,   306,   306,   306,   306,
     306,   307,   308,   308,   309,   310,   310,   310,   311,   311,
     311,   312,   312,   312,   312,   313,   313,   314,   315,   316,
     318,   317,   319,   317,   320,   320,   321,   321,   322,   322,
     324,   323,   325,   323,   327,   326,   328,   326,   330,   329,
     331,   329,   333,   332,   332,   334,   334,   335,   335,   336,
     337,   337,   337,   337,   337,   337,   339,   338,   338,   340,
     340,   341,   341,   343,   342,   344,   342,   345,   345,   345,
     345,   345,   345,   345,   345,   345,   346,   346,   346,   347,
     347,   348,   348,   349,   349,   349,   349,   349,   349,   349,
     350,   350,   351,   351
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
       3,     3,     3,     1,     0,     0,     4,     1,     0,     4,
       3,     4,     3,     3,     1,     3,     1,     3,     1,     3,
       1,     1,     0,     0,     4,     1,     0,     4,     3,     4,
       3,     3,     1,     3,     1,     3,     1,     3,     1,     1,
       0,     0,     4,     1,     0,     2,     1,     0,     7,     0,
       8,     0,     9,     0,    10,     3,     1,     0,     0,     4,
       1,     0,     2,     1,     0,     7,     0,     8,     0,     9,
       0,    10,     3,     1,     0,     2,     1,     0,     4,     0,
       6,     2,     0,     3,     1,     1,     1,     0,     2,     1,
       0,     4,     0,     6,     2,     0,     3,     1,     1,     1,
       0,     2,     1,     0,     4,     0,     6,     2,     0,     3,
       1,     3,     1,     0,     2,     1,     0,     4,     0,     6,
       1,     0,     2,     1,     4,     2,     0,     0,     4,     0,
       6,     2,     0,     3,     1,     2,     1,     2,     0,     0,
       4,     0,     6,     2,     0,     3,     1,     2,     1,     4,
       0,     4,     0,     1,     0,     0,     5,     0,     4,     1,
       0,     2,     1,     1,     1,     0,     0,     8,     3,     1,
       5,     2,     0,     2,     0,     2,     1,     6,     8,     6,
       8,     3,     1,     2,     1,     2,     1,     2,     1,     3,
       1,     2,     1,     2,     1,     2,     1,     1,     2,     3,
       1,     0,     0,     5,     0,     4,     1,     0,     2,     1,
       1,     1,     0,     0,     8,     3,     1,     5,     2,     0,
       2,     0,     2,     1,     6,     8,     6,     8,     3,     1,
       2,     1,     2,     1,     2,     1,     3,     1,     2,     1,
       2,     1,     2,     1,     1,     2,     3,     4,     5,     7,
       2,     6,     5,     7,     3,     1,     1,     6,     4,     3,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       3,     3,     3,     2,     3,     3,     5,     3,     4,     6,
       8,     1,     1,     1,     1,     1,     1,     2,     3,     3,
       2,     1,     1,     1,     1,     1,     3,     4,     1,     1,
       1,     3,     3,     2,     2,     2,     2,     3,     3,     3,
       0,     7,     0,     9,     1,     0,     2,     1,     2,     4,
       0,     8,     0,    10,     0,     8,     0,    10,     0,     8,
       0,    10,     0,     4,     0,     1,     0,     2,     1,     4,
       1,     1,     1,     1,     1,     1,     0,     4,     0,     1,
       0,     2,     1,     0,     7,     0,     8,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     3,     2,     0,     4,
       0,     2,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       4,     0,     2,     1,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     3,     5,    10,    29,
       6,     0,    11,    54,     7,     0,    12,    54,     8,     0,
      13,   174,     9,     0,    14,    15,    16,    17,    18,    19,
      20,    21,     0,    42,    28,    48,     0,   415,   416,     0,
       0,     0,     0,     0,     0,     0,     0,    30,    38,     0,
      78,    53,    63,    78,    63,     0,   200,   173,   176,   187,
      26,     0,    25,     0,     0,   417,   438,   418,   419,     0,
     420,     0,   430,     0,   434,   408,   409,   396,   401,   402,
     403,   360,     0,     0,     0,   405,     0,     0,     0,     0,
     491,   356,   355,   361,   391,   392,   393,   394,   395,   404,
     410,   491,    27,     0,     0,     0,    37,    55,     0,   104,
      77,     0,    86,    62,   104,    86,     0,   177,     0,     0,
     199,   202,   175,     0,   213,   186,   189,    23,     0,   491,
       0,   444,     0,   425,     0,   444,     0,   444,   413,   414,
       0,     0,   397,     0,   400,     0,     0,     0,   379,   383,
     369,     0,   490,    32,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   491,    34,   491,
       0,   491,     0,    58,    79,     0,     0,   103,    64,     0,
     122,    85,   140,   122,     0,   182,     0,   203,   491,   347,
     201,     0,   190,     0,     0,   212,   215,   188,    24,     0,
     348,   440,     0,   458,   422,     0,     0,   424,   427,   432,
     458,   436,   458,   387,     0,   359,     0,   398,   399,     0,
     406,   362,   370,   371,   372,   373,   374,   375,   376,   377,
     378,   380,   381,   382,   384,   385,     0,   363,   364,   365,
     366,   367,   368,   411,   412,    33,    31,     0,    35,     0,
      61,    56,     0,    60,    82,   105,   491,    41,    67,    87,
       0,     0,   121,     0,   254,   139,   157,   179,   185,   178,
       0,   184,     0,   208,   350,     0,   195,     0,   216,   491,
     214,   491,   444,   442,     0,     0,   425,   428,     0,     0,
     426,   444,     0,   444,     0,     0,   388,     0,   358,   407,
       0,    40,     0,    57,     0,    94,     0,    80,     0,    84,
      93,   108,   491,    44,    70,    65,     0,    69,    90,   123,
     491,   141,     0,    72,   253,     0,   301,   156,   182,   181,
       0,   205,     0,   204,     0,   210,   192,   198,   191,     0,
     197,     0,   221,   351,   349,   458,   446,   456,     0,     0,
       0,   421,   458,     0,   458,     0,     0,     0,   386,    39,
      59,     0,    96,    81,     0,     0,     0,   106,   107,    43,
      66,     0,    88,     0,    92,   126,    45,   144,     0,   257,
       0,     0,    71,   158,     0,    75,   300,   180,   183,   208,
       0,   207,     0,   195,   194,     0,   218,     0,   217,   220,
     223,     0,     0,   443,   445,   448,   460,   439,     0,   429,
       0,   431,     0,   435,     0,   389,   357,    97,    95,   100,
     101,   102,    83,   116,     0,   114,   120,     0,   118,     0,
       0,    68,    89,     0,     0,   124,   125,     0,   142,   143,
     146,   255,   260,     0,   491,    47,   161,     0,   304,     0,
       0,    74,   206,   211,   209,   193,   196,   221,     0,   222,
       0,     0,   447,   467,   474,   475,     0,   469,   470,   471,
     472,   473,   457,   459,   462,     0,   423,     0,     0,     0,
      98,    99,     0,   110,     0,     0,   112,     0,     0,     0,
      91,   134,     0,   132,   138,     0,   136,     0,     0,     0,
     145,   260,   265,   288,   258,   259,   262,   263,   264,     0,
       0,   282,     0,   491,    50,     0,   159,   160,   163,   302,
     307,     0,   491,   219,     0,   441,   451,   452,   450,   454,
     455,   453,     0,   468,     0,   461,   463,   433,   437,   390,
     115,   113,   119,   117,   109,   111,     0,   128,     0,     0,
     130,     0,     0,     0,   491,   256,     0,     0,   287,   261,
       0,     0,     0,    73,    49,     0,   162,   307,   312,   335,
     305,   306,   309,   310,   311,     0,     0,   329,     0,    51,
     224,   449,   465,   478,   133,   131,   137,   135,   127,   129,
       0,   491,   147,     0,     0,   269,     0,   296,   272,     0,
     290,     0,   272,     0,   286,   281,   491,   303,     0,   334,
     308,     0,     0,     0,    76,   478,     0,   480,   491,     0,
     149,   174,     0,   266,     0,   354,   295,   298,     0,   274,
       0,     0,     0,   274,     0,   285,     0,   491,   164,     0,
       0,   316,   343,   319,     0,   337,     0,   319,     0,   333,
     328,   480,     0,   477,     0,   482,   151,   491,   174,   200,
       0,     0,   268,   271,     0,     0,   294,   289,   296,   272,
     299,     0,   272,   491,     0,   166,   187,     0,   313,     0,
     342,   345,     0,   321,     0,     0,     0,   321,     0,   332,
     482,   476,     0,     0,     0,   174,   153,   200,   148,   226,
       0,     0,   276,   273,   493,   492,   279,   293,   274,   277,
     274,   168,   491,   187,   213,     0,     0,   315,   318,     0,
       0,   341,   336,   343,   319,   346,     0,   319,     0,     0,
     485,   486,   483,   484,   488,   489,   487,   481,   464,   200,
     174,   150,   250,   270,   491,   275,     0,     0,   187,   170,
     213,   165,   238,     0,     0,   323,   320,   326,   340,   321,
     324,   321,   466,   479,   152,   200,     0,     0,   225,   155,
     267,   280,   278,   213,   187,   167,   252,   317,     0,   322,
       0,     0,   154,     0,   227,     0,   169,   213,     0,     0,
     237,   172,   314,   327,   325,     0,   232,     0,   171,     0,
     239,     0,   229,   236,   228,     0,   234,   249,     0,   244,
       0,   232,   235,   231,     0,   241,   248,   240,     0,   246,
     251,   230,   233,   244,   247,   243,     0,   242,   245
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     2,    16,    17,    42,    71,    72,    18,    19,
      56,    57,   112,    20,    21,   115,   116,    22,    23,   277,
      24,    25,    26,    27,   465,    28,    29,    60,    61,   193,
     271,   272,   273,   122,   123,   278,   335,   336,   337,   401,
     402,   470,   471,   119,   120,   274,   327,   328,   200,   201,
     338,   392,   393,   329,   330,   196,   197,   331,   387,   388,
     444,   445,   447,   448,   281,   282,   395,   455,   456,   512,
     513,   515,   516,   284,   285,   397,   458,   459,   460,   641,
     678,   715,   760,   718,   346,   347,   466,   536,   537,   538,
     696,   733,   768,   794,   771,    66,    67,    68,   205,   348,
     289,   290,   291,   134,   135,   136,   296,   413,   358,   359,
     360,   719,   130,   131,   293,   409,   353,   354,   355,   772,
     215,   216,   362,   477,   418,   419,   420,   762,   788,   816,
     831,   824,   825,   826,   796,   810,   829,   843,   837,   838,
     839,   789,   811,   343,   344,   521,   462,   524,   525,   526,
     527,   576,   681,   614,   615,   649,   685,   721,   528,   529,
     530,   625,   531,   618,   619,   687,   620,   621,   405,   406,
     587,   540,   590,   591,   592,   593,   628,   736,   660,   661,
     703,   740,   774,   594,   595,   596,   670,   597,   663,   664,
     742,   665,   666,    30,    31,   209,    32,    33,   646,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
      34,    35,    36,    37,    38,   143,   306,   226,   227,   228,
      39,   145,   311,    40,   147,   313,    41,   141,   302,   223,
     366,   423,   424,   425,   552,   305,   426,   492,   493,   494,
     603,   635,   495,   637,   675,   714,   757,   163,   726
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -753
static const short yypact[] =
{
    -753,    70,   443,  -753,  -753,   105,   147,   173,   180,    -4,
     182,   188,   220,   235,   247,   252,  -753,  -753,  -753,   255,
    -753,    67,  -753,   144,  -753,   151,  -753,   144,  -753,   168,
    -753,   191,  -753,   217,  -753,  -753,  -753,  -753,  -753,  -753,
    -753,  -753,   272,   219,   237,   248,   261,  -753,  -753,   -28,
     226,   236,   -37,   -32,   -31,   364,   323,   322,   324,   279,
     363,  -753,   356,   363,   356,   122,   382,   191,  -753,   360,
    -753,   -50,  -753,   357,   359,  -753,  -753,  -753,  -753,   361,
    -753,   366,  -753,   378,  -753,   381,   384,   319,  -753,  -753,
    -753,   320,   119,   116,   321,   325,   376,   376,   376,   376,
     298,  -753,   718,  -753,  -753,  -753,  -753,  -753,  -753,  -753,
     134,   117,  -753,   364,   326,   299,   389,  -753,   340,   420,
    -753,   346,   425,  -753,   420,   425,   364,  -753,   124,   406,
     382,  -753,  -753,   126,   427,   360,  -753,  -753,   272,   -15,
     342,   377,   343,   404,   348,   377,   350,   377,  -753,  -753,
     250,   306,  -753,   415,  -753,   418,   419,   353,  -753,  -753,
    -753,   605,  -753,  -753,   376,   376,   376,   376,   376,   376,
     376,   376,   376,   376,   376,   376,   376,   376,   376,   376,
     376,   376,   376,   376,   376,   422,   423,   298,  -753,   298,
     364,   298,   365,   426,  -753,   375,   447,  -753,  -753,   387,
     459,  -753,   463,   459,   390,   439,   364,  -753,   298,  -753,
    -753,   364,  -753,   127,   380,   427,  -753,  -753,  -753,   444,
     383,  -753,   395,   432,  -753,    72,   385,   404,  -753,  -753,
     432,  -753,   432,  -753,   486,  -753,   527,  -753,  -753,   388,
    -753,  -753,   435,   435,   435,   435,   435,   435,   768,   768,
     768,   214,   214,   214,   190,   190,   693,    88,    88,   391,
     391,   391,   391,  -753,  -753,  -753,  -753,   399,  -753,   364,
    -753,  -753,   138,  -753,     7,  -753,   123,  -753,   448,  -753,
     412,   405,  -753,   417,   489,  -753,   493,  -753,  -753,  -753,
     148,  -753,   421,   472,  -753,   430,   473,   364,  -753,   298,
    -753,   298,   377,  -753,   433,   413,   404,  -753,   477,   434,
    -753,   377,   437,   377,   438,   376,  -753,   376,  -753,  -753,
     376,  -753,   445,  -753,   426,   446,   476,  -753,   154,  -753,
    -753,   239,   298,  -753,  -753,  -753,   167,  -753,     7,  -753,
     298,  -753,     1,   507,  -753,   451,   517,  -753,   439,  -753,
     439,  -753,   440,  -753,   200,  -753,  -753,  -753,  -753,   213,
    -753,   455,   495,  -753,   449,   432,   506,  -753,   450,   452,
     453,  -753,   432,   454,   432,   457,   563,   641,   743,  -753,
    -753,   192,  -753,  -753,     7,   510,   531,  -753,   244,  -753,
    -753,   448,  -753,   469,  -753,   297,  -753,   534,   467,  -753,
     494,   553,  -753,  -753,    46,   530,  -753,  -753,  -753,   472,
     364,  -753,   472,   473,  -753,   473,  -753,   484,  -753,   495,
    -753,   487,   485,  -753,   506,  -753,   315,  -753,   492,  -753,
     490,  -753,   496,  -753,   376,  -753,  -753,   546,   548,  -753,
    -753,  -753,  -753,   505,   224,  -753,   512,   228,  -753,   510,
     531,  -753,     7,   577,   584,  -753,   310,   535,  -753,   534,
    -753,  -753,    16,   586,   125,  -753,   588,   539,  -753,   540,
     532,  -753,  -753,  -753,  -753,  -753,  -753,   495,   364,  -753,
     550,   186,  -753,   551,  -753,  -753,   424,  -753,  -753,  -753,
    -753,  -753,  -753,   315,  -753,   592,  -753,   555,   562,   667,
    -753,  -753,   547,  -753,   510,   557,  -753,   531,   234,   242,
    -753,   561,   260,  -753,   564,   262,  -753,   577,   584,   626,
    -753,    16,  -753,   -46,  -753,    16,  -753,  -753,  -753,   211,
     566,  -753,   570,   298,  -753,   579,  -753,   588,  -753,  -753,
      39,   631,   298,  -753,   575,  -753,   381,   384,  -753,  -753,
    -753,   134,   576,  -753,   635,  -753,  -753,  -753,  -753,  -753,
    -753,  -753,  -753,  -753,  -753,  -753,   572,  -753,   577,   582,
    -753,   584,   269,   273,    34,  -753,   650,   364,   597,  -753,
     653,   653,   663,  -753,  -753,   664,  -753,    39,  -753,    55,
    -753,    39,  -753,  -753,  -753,   352,   600,  -753,   608,  -753,
    -753,  -753,  -753,   604,  -753,  -753,  -753,  -753,  -753,  -753,
     668,     6,  -753,   607,   685,   623,   622,   164,   703,   633,
    -753,    11,   703,     4,   628,  -753,    37,  -753,   712,   652,
    -753,   715,   715,   724,  -753,   604,   -19,   720,   298,   741,
    -753,   191,   364,  -753,   650,  -753,   694,  -753,   364,   786,
     766,   784,   364,   786,   784,  -753,   788,    31,  -753,   706,
     806,   733,   233,   807,   742,  -753,    13,   807,    14,   628,
    -753,   720,   744,  -753,   740,   747,  -753,   298,   191,   382,
     829,   816,  -753,  -753,   817,   287,   628,  -753,    85,   703,
    -753,   287,   703,   298,   825,  -753,   360,   364,  -753,   712,
     752,  -753,   364,   849,   827,   828,   364,   849,   828,  -753,
     747,  -753,   830,   153,   771,   191,  -753,   382,  -753,  -753,
     364,    38,  -753,  -753,  -753,  -753,  -753,  -753,   786,  -753,
     786,  -753,   298,   360,   427,   847,   832,  -753,  -753,   833,
     775,   628,  -753,    96,   807,  -753,   777,   807,   778,   770,
     381,   384,  -753,  -753,  -753,  -753,   134,  -753,  -753,   382,
     191,  -753,    60,  -753,   298,  -753,   287,   287,   360,  -753,
     427,  -753,  -753,   364,   131,  -753,  -753,  -753,  -753,   849,
    -753,   849,  -753,  -753,  -753,   382,   128,   789,  -753,  -753,
    -753,  -753,  -753,   427,   360,  -753,   130,  -753,   780,  -753,
     782,   783,  -753,   364,  -753,   841,  -753,   427,   129,   793,
    -753,  -753,  -753,  -753,  -753,   794,   845,   790,  -753,   364,
    -753,   846,  -753,   628,  -753,   290,  -753,  -753,   796,   850,
     791,   845,  -753,  -753,   845,  -753,   628,  -753,   294,  -753,
    -753,  -753,  -753,   850,  -753,  -753,   850,  -753,  -753
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -753,  -753,  -753,  -753,  -753,  -753,  -753,   748,  -753,  -753,
    -753,  -753,  -753,  -753,  -753,  -753,  -753,  -753,  -753,  -753,
    -753,  -753,  -753,  -753,  -753,  -753,  -753,   860,  -753,  -753,
    -753,  -753,   565,   824,  -753,  -753,  -753,  -753,   499,  -753,
    -753,  -753,  -753,   831,  -753,  -753,  -753,  -753,   767,  -753,
    -753,  -753,  -753,   509,  -327,   772,  -753,  -753,  -753,  -753,
     442,   393,   456,   392,   692,  -753,  -753,  -753,  -753,   386,
     330,   394,   329,  -753,  -753,  -753,  -753,  -753,   458,  -753,
    -753,  -753,  -753,  -584,  -753,  -753,  -753,  -753,  -753,   367,
    -753,  -753,  -753,  -753,  -752,  -631,  -753,   834,  -753,  -753,
     554,  -753,   558,  -693,  -753,   774,  -753,  -753,   497,  -753,
     498,   839,  -753,   781,  -753,  -753,   511,  -753,   502,   773,
    -753,   700,  -753,  -753,   441,  -753,   500,  -753,  -753,  -753,
    -753,    90,  -753,    82,  -753,  -753,  -753,  -753,    79,  -753,
      77,  -753,  -753,  -753,  -753,  -753,  -753,   403,  -753,   400,
    -753,  -753,  -753,   282,  -753,  -551,  -634,  -753,  -661,  -753,
    -753,  -753,  -753,  -572,  -753,  -753,  -753,   347,  -753,  -753,
    -753,  -753,   344,  -753,   336,  -753,  -753,  -753,   230,  -753,
    -652,  -645,  -753,  -706,  -753,  -753,  -753,  -753,  -608,  -753,
    -753,  -753,   300,  -753,  -753,  -753,  -753,  -753,  -498,  -113,
    -753,   -61,  -753,  -753,  -753,  -753,  -753,  -753,  -753,  -480,
    -753,  -753,  -753,  -753,  -753,  -753,  -753,   624,  -753,   707,
    -753,  -753,  -753,  -753,  -753,  -753,  -753,  -753,  -753,  -139,
    -753,  -753,  -753,   513,  -753,  -228,  -753,  -753,  -753,   460,
    -753,  -753,   461,   301,   264,   223,  -753,  -106,  -679
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -354
static const short yytable[] =
{
     189,   551,   312,   734,   314,   188,   230,   219,   232,   622,
     679,   394,   729,   204,   672,   707,   398,    79,   795,   691,
     722,   654,    81,    83,   667,   578,    74,   651,   639,   705,
     775,   708,    47,   220,   522,   158,   159,   160,   161,   652,
     770,   806,   325,   137,   138,    48,   652,   717,   706,   706,
     326,   523,   577,   694,  -284,   818,   610,   588,    80,   656,
     765,   467,   746,    82,    84,    75,   764,    76,   799,   786,
       3,   653,   787,   523,   589,   793,   611,   267,   162,   689,
     673,   265,   692,   266,   759,   268,   399,   791,   792,   234,
     236,   629,   779,   292,   766,   781,   767,   744,   295,   162,
     747,   807,   294,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   162,   510,   655,   162,   657,   785,
     162,   468,   577,   761,   800,   577,   801,   421,   728,   808,
      43,   730,   809,   126,   430,   206,   432,   211,   297,   803,
     819,   154,   187,   577,   152,  -331,   322,   155,   332,   798,
     533,   153,    58,   365,   700,   307,   589,   185,   186,   308,
     333,   709,   372,    59,   374,   784,   181,   182,   183,   184,
    -297,  -297,    44,   577,   361,  -292,   750,   751,   727,   752,
     753,   754,   755,   363,   577,   364,  -339,    85,    86,    87,
      88,   802,    89,    90,    91,    92,    93,   127,    45,   207,
     162,   212,   298,   804,   820,    46,   162,    49,   162,   546,
     547,    65,   548,    50,   549,   550,   389,   580,   581,    94,
      95,   323,   324,   756,   396,   437,   438,   439,   440,   441,
      96,   349,   350,   778,    97,   700,    62,   383,   384,  -344,
    -344,    98,   385,   386,   376,    51,   377,   449,   450,   378,
     390,   391,   577,    64,  -292,    99,    85,    86,    87,    88,
      52,    89,    90,    91,    92,    93,   179,   180,   181,   182,
     183,   184,    53,    85,    86,    87,    88,    54,    89,    90,
      55,    92,    93,   411,   412,   176,   177,   473,    94,    95,
     179,   180,   181,   182,   183,   184,   414,   415,    70,    96,
     453,   454,    69,    97,   -46,    94,    95,   503,   504,    77,
      98,   506,   507,   517,   518,   832,    96,   564,   504,    78,
      97,   577,   -36,  -339,    99,   565,   507,    98,   844,    85,
      86,    87,    88,   -52,    89,    90,    73,    92,    93,   111,
     483,    99,   233,   567,   568,   570,   571,   113,   534,   114,
     484,   485,   608,   568,   117,   544,   609,   571,   631,   632,
     118,    94,    95,   499,   486,   487,   488,   489,   490,   491,
     724,   725,    96,   833,   834,   121,    97,   845,   846,   128,
     133,   162,   139,    98,   140,   191,   142,    85,    86,    87,
      88,   144,    89,    90,    91,    92,    93,    99,   235,    85,
      86,    87,    88,   146,    89,    90,   148,    92,    93,   149,
     150,   151,   156,   190,   192,   194,   157,   584,   195,    94,
      95,   198,   199,   208,   213,   222,   599,   221,   224,   225,
      96,    94,    95,   229,    97,   231,     4,     5,     6,     7,
     237,    98,    96,   238,   239,   240,    97,   263,   264,   483,
     275,   270,   269,    98,   616,    99,     8,   280,   612,   484,
     485,   276,   279,   283,   288,   287,   299,    99,  -352,   301,
     303,   309,   184,   334,   487,   488,   489,   490,   491,   304,
     319,     9,   321,    10,    11,    12,    13,   339,    14,    15,
     342,   340,   341,   345,   647,   640,   351,   352,   357,   368,
     370,   382,   173,   174,   175,   356,   176,   177,   367,   400,
     658,   179,   180,   181,   182,   183,   184,   371,   404,   680,
     417,   381,   676,   373,   375,   683,   403,   410,   379,   690,
     416,   422,   469,   427,  -353,   443,   429,   431,   428,   701,
     433,   695,   461,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   452,   173,   174,   175,   446,   176,   177,   457,
     178,   716,   179,   180,   181,   182,   183,   184,   464,   463,
     315,   478,   481,   480,   735,   496,   497,   731,   316,   738,
     500,   501,   498,   745,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   502,   173,   174,   175,   763,   176,   177,
     505,   178,   511,   179,   180,   181,   182,   183,   184,   514,
     519,   317,   532,   535,   539,   541,   769,   556,   542,   318,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   553,
     173,   174,   175,   545,   176,   177,   560,   178,   557,   179,
     180,   181,   182,   183,   184,   558,   562,   434,   790,   566,
     797,   574,   569,   583,   585,   435,   582,   598,   600,   601,
     602,   604,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   606,   173,   174,   175,   613,   176,   177,   617,   178,
     815,   179,   180,   181,   182,   183,   184,  -283,   624,   626,
     633,   634,   636,   638,   642,   643,   828,   241,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   644,   173,   174,
     175,   645,   176,   177,   648,   178,   577,   179,   180,   181,
     182,   183,   184,   650,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   436,   173,   174,   175,   659,   176,   177,
     662,   178,  -330,   179,   180,   181,   182,   183,   184,   669,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   559,
     173,   174,   175,   674,   176,   177,   677,   178,   320,   179,
     180,   181,   182,   183,   184,   164,   165,   166,   167,   168,
     169,   170,   171,   172,  -291,   173,   174,   175,   684,   176,
     177,   686,   178,   697,   179,   180,   181,   182,   183,   184,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   688,
     173,   174,   175,   693,   176,   177,   698,   699,   702,   179,
     180,   181,   182,   183,   184,   164,   165,   166,   167,   168,
     169,   712,   704,   711,   713,   173,   174,   175,   720,   176,
     177,   523,  -338,   723,   179,   180,   181,   182,   183,   184,
     732,   739,   741,   743,   758,   749,   773,   589,   777,   776,
     780,   782,   783,   812,   805,   813,   814,   817,   821,   822,
     823,   835,   830,   827,   840,   836,   218,    63,   125,   380,
     451,   508,   203,   442,   124,   286,   202,   561,   605,   563,
     607,   132,   407,   572,   586,   129,   509,   214,   408,   217,
     475,   210,   573,   476,   474,   300,   842,   520,   543,   479,
     472,   841,   847,   848,   575,   579,   682,   630,   623,   737,
     369,   627,   668,   748,   310,   710,   671,   482,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   554,     0,     0,
       0,     0,     0,   555
};

static const short yycheck[] =
{
     113,   481,   230,   696,   232,   111,   145,    22,   147,   581,
     641,   338,   691,   126,    33,   667,    15,    54,   770,   653,
     681,    17,    54,    54,   632,   523,    54,    16,    22,    16,
     736,    17,    36,   139,    18,    96,    97,    98,    99,    35,
     733,   793,    35,    93,    94,    49,    35,   678,    35,    35,
      43,    35,    98,    22,   100,   807,    22,    18,    95,    22,
     721,    15,   707,    95,    95,    93,    28,    95,   774,     9,
       0,   622,    12,    35,    35,   768,   574,   190,    93,   651,
      99,   187,   654,   189,   715,   191,    85,   766,   767,   150,
     151,   589,   744,   206,   728,   747,   730,   705,   211,    93,
     708,   794,   208,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,    93,   452,   624,    93,   626,   760,
      93,    85,    98,   717,   779,    98,   781,   365,   689,     9,
      35,   692,    12,    21,   372,    21,   374,    21,    21,    21,
      21,    35,    35,    98,    35,   100,   269,    41,    35,    28,
      35,    42,    95,   302,   662,    93,    35,    33,    34,    97,
     276,   669,   311,    29,   313,   759,    88,    89,    90,    91,
      16,    17,    35,    98,   297,   100,    33,    34,   686,    36,
      37,    38,    39,   299,    98,   301,   100,    33,    34,    35,
      36,   785,    38,    39,    40,    41,    42,    85,    35,    85,
      93,    85,    85,    85,    85,    35,    93,    35,    93,    33,
      34,    30,    36,    35,    38,    39,   332,    16,    17,    65,
      66,    93,    94,   713,   340,    43,    44,    45,    46,    47,
      76,    93,    94,   741,    80,   743,    95,    93,    94,    16,
      17,    87,    13,    14,   315,    35,   317,    13,    14,   320,
      93,    94,    98,    95,   100,   101,    33,    34,    35,    36,
      35,    38,    39,    40,    41,    42,    86,    87,    88,    89,
      90,    91,    35,    33,    34,    35,    36,    35,    38,    39,
      35,    41,    42,    93,    94,    81,    82,   410,    65,    66,
      86,    87,    88,    89,    90,    91,    93,    94,    36,    76,
      13,    14,    95,    80,    95,    65,    66,    93,    94,    93,
      87,    93,    94,    13,    14,   823,    76,    93,    94,    93,
      80,    98,    95,   100,   101,    93,    94,    87,   836,    33,
      34,    35,    36,    95,    38,    39,    85,    41,    42,    26,
      35,   101,   102,    93,    94,    93,    94,    35,   464,    35,
      45,    46,    93,    94,    85,   478,    93,    94,    16,    17,
       7,    65,    66,   434,    59,    60,    61,    62,    63,    64,
      93,    94,    76,    93,    94,    29,    80,    93,    94,     7,
      30,    93,    35,    87,    35,    96,    35,    33,    34,    35,
      36,    35,    38,    39,    40,    41,    42,   101,   102,    33,
      34,    35,    36,    35,    38,    39,    35,    41,    42,    35,
     101,   101,   101,    97,    35,    85,   101,   533,     8,    65,
      66,    85,     7,    27,     7,    58,   542,    95,    95,    35,
      76,    65,    66,    95,    80,    95,     3,     4,     5,     6,
      35,    87,    76,    35,    35,   102,    80,    35,    35,    35,
      85,    35,    97,    87,   577,   101,    23,     8,   574,    45,
      46,    24,    85,    10,    35,    85,    96,   101,    95,    35,
      85,    96,    91,    35,    60,    61,    62,    63,    64,    57,
     102,    48,    93,    50,    51,    52,    53,    85,    55,    56,
      11,    96,    85,    10,   617,   611,    85,    35,    35,    96,
      33,    35,    77,    78,    79,    85,    81,    82,    85,    12,
     626,    86,    87,    88,    89,    90,    91,    93,    11,   642,
      35,    85,   638,    96,    96,   648,    85,    97,    93,   652,
      85,    35,    12,    93,    95,    35,    93,    93,    96,   662,
      93,   657,    85,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    93,    77,    78,    79,    35,    81,    82,    35,
      84,   677,    86,    87,    88,    89,    90,    91,    25,    85,
      94,    97,    97,    96,   697,    93,    96,   693,   102,   702,
      44,    43,    96,   706,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    98,    77,    78,    79,   720,    81,    82,
      98,    84,    35,    86,    87,    88,    89,    90,    91,    35,
      85,    94,    36,    35,    85,    85,   732,    35,    96,   102,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    88,
      77,    78,    79,    93,    81,    82,    99,    84,    93,    86,
      87,    88,    89,    90,    91,    93,    99,    94,   764,    98,
     773,    35,    98,    93,    85,   102,   100,    36,    93,    93,
      35,    99,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    99,    77,    78,    79,    35,    81,    82,    35,    84,
     803,    86,    87,    88,    89,    90,    91,   100,    35,    35,
     100,    93,    98,    35,    97,    20,   819,   102,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    94,    77,    78,
      79,    99,    81,    82,    21,    84,    98,    86,    87,    88,
      89,    90,    91,   100,    67,    68,    69,    70,    71,    72,
      73,    74,    75,   102,    77,    78,    79,    35,    81,    82,
      35,    84,   100,    86,    87,    88,    89,    90,    91,    35,
      67,    68,    69,    70,    71,    72,    73,    74,    75,   102,
      77,    78,    79,    53,    81,    82,    35,    84,    85,    86,
      87,    88,    89,    90,    91,    67,    68,    69,    70,    71,
      72,    73,    74,    75,   100,    77,    78,    79,    12,    81,
      82,    35,    84,    97,    86,    87,    88,    89,    90,    91,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    35,
      77,    78,    79,    35,    81,    82,    20,    94,    21,    86,
      87,    88,    89,    90,    91,    67,    68,    69,    70,    71,
      72,   101,   100,    99,    97,    77,    78,    79,    19,    81,
      82,    35,   100,    36,    86,    87,    88,    89,    90,    91,
      35,    12,    35,    35,    93,    35,    19,    35,    93,    36,
      93,    93,   102,    93,    85,    93,    93,    36,    85,    85,
      35,    85,    36,    93,    93,    35,   138,    27,    64,   324,
     391,   449,   125,   384,    63,   203,   124,   504,   568,   507,
     571,    67,   348,   517,   537,    66,   450,   134,   350,   135,
     413,   130,   518,   415,   412,   215,   834,   459,   477,   419,
     409,   831,   843,   846,   521,   525,   644,   591,   581,   699,
     306,   587,   632,   710,   227,   671,   635,   424,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   486,    -1,    -1,
      -1,    -1,    -1,   493
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,   104,   105,     0,     3,     4,     5,     6,    23,    48,
      50,    51,    52,    53,    55,    56,   106,   107,   111,   112,
     116,   117,   120,   121,   123,   124,   125,   126,   128,   129,
     296,   297,   299,   300,   313,   314,   315,   316,   317,   323,
     326,   329,   108,    35,    35,    35,    35,    36,    49,    35,
      35,    35,    35,    35,    35,    35,   113,   114,    95,    29,
     130,   131,    95,   130,    95,    30,   198,   199,   200,    95,
      36,   109,   110,    85,    54,    93,    95,    93,    93,    54,
      95,    54,    95,    54,    95,    33,    34,    35,    36,    38,
      39,    40,    41,    42,    65,    66,    76,    80,    87,   101,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,    26,   115,    35,    35,   118,   119,    85,     7,   146,
     147,    29,   136,   137,   146,   136,    21,    85,     7,   214,
     215,   216,   200,    30,   206,   207,   208,    93,    94,    35,
      35,   330,    35,   318,    35,   324,    35,   327,    35,    35,
     101,   101,    35,    42,    35,    41,   101,   101,   304,   304,
     304,   304,    93,   350,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    77,    78,    79,    81,    82,    84,    86,
      87,    88,    89,    90,    91,    33,    34,    35,   350,   302,
      97,    96,    35,   132,    85,     8,   158,   159,    85,     7,
     151,   152,   158,   151,   302,   201,    21,    85,    27,   298,
     216,    21,    85,     7,   222,   223,   224,   208,   110,    22,
     350,    95,    58,   332,    95,    35,   320,   321,   322,    95,
     332,    95,   332,   102,   304,   102,   304,    35,    35,    35,
     102,   102,   304,   304,   304,   304,   304,   304,   304,   304,
     304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
     304,   304,   304,    35,    35,   350,   350,   302,   350,    97,
      35,   133,   134,   135,   148,    85,    24,   122,   138,    85,
       8,   167,   168,    10,   176,   177,   167,    85,    35,   203,
     204,   205,   302,   217,   350,   302,   209,    21,    85,    96,
     224,    35,   331,    85,    57,   338,   319,    93,    97,    96,
     322,   325,   338,   328,   338,    94,   102,    94,   102,   102,
      85,    93,   302,    93,    94,    35,    43,   149,   150,   156,
     157,   160,    35,   350,    35,   139,   140,   141,   153,    85,
      96,    85,    11,   246,   247,    10,   187,   188,   202,    93,
      94,    85,    35,   219,   220,   221,    85,    35,   211,   212,
     213,   302,   225,   350,   350,   332,   333,    85,    96,   320,
      33,    93,   332,    96,   332,    96,   304,   304,   304,    93,
     135,    85,    35,    93,    94,    13,    14,   161,   162,   350,
      93,    94,   154,   155,   157,   169,   350,   178,    15,    85,
      12,   142,   143,    85,    11,   271,   272,   203,   205,   218,
      97,    93,    94,   210,    93,    94,    85,    35,   227,   228,
     229,   338,    35,   334,   335,   336,   339,    93,    96,    93,
     338,    93,   338,    93,    94,   102,   102,    43,    44,    45,
      46,    47,   156,    35,   163,   164,    35,   165,   166,    13,
      14,   141,    93,    13,    14,   170,   171,    35,   179,   180,
     181,    85,   249,    85,    25,   127,   189,    15,    85,    12,
     144,   145,   219,   302,   221,   211,   213,   226,    97,   229,
      96,    97,   336,    35,    45,    46,    59,    60,    61,    62,
      63,    64,   340,   341,   342,   345,    93,    96,    96,   304,
      44,    43,    98,    93,    94,    98,    93,    94,   163,   165,
     157,    35,   172,   173,    35,   174,   175,    13,    14,    85,
     181,   248,    18,    35,   250,   251,   252,   253,   261,   262,
     263,   265,    36,    35,   350,    35,   190,   191,   192,    85,
     274,    85,    96,   227,   302,    93,    33,    34,    36,    38,
      39,   312,   337,    88,   345,   342,    35,    93,    93,   102,
      99,   164,    99,   166,    93,    93,    98,    93,    94,    98,
      93,    94,   172,   174,    35,   250,   254,    98,   301,   252,
      16,    17,   100,    93,   350,    85,   192,   273,    18,    35,
     275,   276,   277,   278,   286,   287,   288,   290,    36,   350,
      93,    93,    35,   343,    99,   173,    99,   175,    93,    93,
      22,   301,   350,    35,   256,   257,   302,    35,   266,   267,
     269,   270,   266,   270,    35,   264,    35,   275,   279,   301,
     277,    16,    17,   100,    93,   344,    98,   346,    35,    22,
     350,   182,    97,    20,    94,    99,   301,   302,    21,   258,
     100,    16,    35,   258,    17,   301,    22,   301,   350,    35,
     281,   282,    35,   291,   292,   294,   295,   291,   295,    35,
     289,   346,    33,    99,    53,   347,   350,    35,   183,   198,
     302,   255,   256,   302,    12,   259,    35,   268,    35,   266,
     302,   259,   266,    35,    22,   350,   193,    97,    20,    94,
     301,   302,    21,   283,   100,    16,    35,   283,    17,   301,
     347,    99,   101,    97,   348,   184,   350,   198,   186,   214,
      19,   260,   261,    36,    93,    94,   351,   301,   258,   351,
     258,   350,    35,   194,   206,   302,   280,   281,   302,    12,
     284,    35,   293,    35,   291,   302,   284,   291,   348,    35,
      33,    34,    36,    37,    38,    39,   312,   349,    93,   198,
     185,   186,   230,   302,    28,   261,   259,   259,   195,   350,
     206,   197,   222,    19,   285,   286,    36,    93,   301,   283,
      93,   283,    93,   102,   186,   198,     9,    12,   231,   244,
     350,   351,   351,   206,   196,   197,   237,   302,    28,   286,
     284,   284,   186,    21,    85,    85,   197,   206,     9,    12,
     238,   245,    93,    93,    93,   302,   232,    36,   197,    21,
      85,    85,    85,    35,   234,   235,   236,    93,   302,   239,
      36,   233,   301,    93,    94,    85,    35,   241,   242,   243,
      93,   234,   236,   240,   301,    93,    94,   241,   243
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
        case 22:
#line 260 "ned.y"
    {
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                }
    break;

  case 23:
#line 265 "ned.y"
    {
                  /* no setTrailingComment(ps.imports,@3); comment already stored with last filename */
                }
    break;

  case 26:
#line 277 "ned.y"
    {
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                }
    break;

  case 28:
#line 293 "ned.y"
    {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                }
    break;

  case 31:
#line 307 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                }
    break;

  case 32:
#line 313 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                }
    break;

  case 33:
#line 322 "ned.y"
    {
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;

  case 34:
#line 326 "ned.y"
    {
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;

  case 35:
#line 338 "ned.y"
    {
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;

  case 36:
#line 345 "ned.y"
    {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                }
    break;

  case 39:
#line 359 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                }
    break;

  case 40:
#line 365 "ned.y"
    {
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                }
    break;

  case 42:
#line 385 "ned.y"
    {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 43:
#line 394 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 44:
#line 398 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 45:
#line 412 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 46:
#line 419 "ned.y"
    {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 48:
#line 443 "ned.y"
    {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 49:
#line 452 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 50:
#line 456 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 51:
#line 473 "ned.y"
    {
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;

  case 52:
#line 480 "ned.y"
    {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                }
    break;

  case 55:
#line 498 "ned.y"
    {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                }
    break;

  case 56:
#line 503 "ned.y"
    {
                }
    break;

  case 61:
#line 519 "ned.y"
    {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                }
    break;

  case 64:
#line 536 "ned.y"
    {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                }
    break;

  case 65:
#line 541 "ned.y"
    {
                }
    break;

  case 70:
#line 557 "ned.y"
    {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                }
    break;

  case 73:
#line 574 "ned.y"
    {
                  addDisplayString(ps.module,yylsp[-1]);
                }
    break;

  case 76:
#line 589 "ned.y"
    {
                  addDisplayString(ps.module,yylsp[-1]);
                }
    break;

  case 79:
#line 604 "ned.y"
    {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                }
    break;

  case 80:
#line 609 "ned.y"
    {
                }
    break;

  case 83:
#line 620 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 84:
#line 624 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 87:
#line 639 "ned.y"
    {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                }
    break;

  case 88:
#line 644 "ned.y"
    {
                }
    break;

  case 91:
#line 655 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 92:
#line 659 "ned.y"
    {
                  setComments(ps.param,yylsp[0]);
                }
    break;

  case 94:
#line 674 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                }
    break;

  case 95:
#line 678 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                }
    break;

  case 96:
#line 682 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                }
    break;

  case 97:
#line 686 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                }
    break;

  case 98:
#line 690 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                }
    break;

  case 99:
#line 694 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                }
    break;

  case 100:
#line 698 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                }
    break;

  case 101:
#line 702 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                }
    break;

  case 102:
#line 706 "ned.y"
    {
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                }
    break;

  case 105:
#line 721 "ned.y"
    {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                }
    break;

  case 106:
#line 726 "ned.y"
    {
                }
    break;

  case 115:
#line 749 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 116:
#line 754 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 119:
#line 767 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 120:
#line 772 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 123:
#line 788 "ned.y"
    {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                }
    break;

  case 124:
#line 793 "ned.y"
    {
                }
    break;

  case 133:
#line 816 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 134:
#line 821 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 137:
#line 834 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;

  case 138:
#line 839 "ned.y"
    {
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;

  case 141:
#line 855 "ned.y"
    {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                }
    break;

  case 142:
#line 860 "ned.y"
    {
                }
    break;

  case 147:
#line 876 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                }
    break;

  case 148:
#line 882 "ned.y"
    {
                }
    break;

  case 149:
#line 885 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                }
    break;

  case 150:
#line 892 "ned.y"
    {
                }
    break;

  case 151:
#line 895 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-3], yylsp[-1]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                }
    break;

  case 152:
#line 901 "ned.y"
    {
                }
    break;

  case 153:
#line 904 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-4], yylsp[-1]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                }
    break;

  case 154:
#line 911 "ned.y"
    {
                }
    break;

  case 158:
#line 931 "ned.y"
    {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                }
    break;

  case 159:
#line 936 "ned.y"
    {
                }
    break;

  case 164:
#line 952 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                }
    break;

  case 165:
#line 958 "ned.y"
    {
                }
    break;

  case 166:
#line 961 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                }
    break;

  case 167:
#line 968 "ned.y"
    {
                }
    break;

  case 168:
#line 971 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-3], yylsp[-1]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                }
    break;

  case 169:
#line 977 "ned.y"
    {
                }
    break;

  case 170:
#line 980 "ned.y"
    {
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-4], yylsp[-1]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                }
    break;

  case 171:
#line 987 "ned.y"
    {
                }
    break;

  case 177:
#line 1012 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                }
    break;

  case 178:
#line 1017 "ned.y"
    {
                }
    break;

  case 179:
#line 1020 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                }
    break;

  case 180:
#line 1026 "ned.y"
    {
                }
    break;

  case 185:
#line 1042 "ned.y"
    {
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                }
    break;

  case 190:
#line 1063 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                }
    break;

  case 191:
#line 1068 "ned.y"
    {
                }
    break;

  case 192:
#line 1071 "ned.y"
    {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                }
    break;

  case 193:
#line 1077 "ned.y"
    {
                }
    break;

  case 198:
#line 1093 "ned.y"
    {
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                }
    break;

  case 203:
#line 1114 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                }
    break;

  case 204:
#line 1119 "ned.y"
    {
                }
    break;

  case 205:
#line 1122 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                }
    break;

  case 206:
#line 1128 "ned.y"
    {
                }
    break;

  case 211:
#line 1145 "ned.y"
    {
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                }
    break;

  case 216:
#line 1167 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                }
    break;

  case 217:
#line 1172 "ned.y"
    {
                }
    break;

  case 218:
#line 1175 "ned.y"
    {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                }
    break;

  case 219:
#line 1181 "ned.y"
    {
                }
    break;

  case 224:
#line 1198 "ned.y"
    {
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-3]);
                  addExpression(ps.substparam, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparam,yylsp[-3],yylsp[-1]);
                }
    break;

  case 227:
#line 1215 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                }
    break;

  case 228:
#line 1220 "ned.y"
    {
                }
    break;

  case 229:
#line 1223 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                }
    break;

  case 230:
#line 1229 "ned.y"
    {
                }
    break;

  case 235:
#line 1245 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                }
    break;

  case 236:
#line 1252 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                }
    break;

  case 239:
#line 1268 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                }
    break;

  case 240:
#line 1273 "ned.y"
    {
                }
    break;

  case 241:
#line 1276 "ned.y"
    {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                }
    break;

  case 242:
#line 1282 "ned.y"
    {
                }
    break;

  case 247:
#line 1298 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                }
    break;

  case 248:
#line 1305 "ned.y"
    {
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                }
    break;

  case 249:
#line 1316 "ned.y"
    {
                  addDisplayString(ps.submod,yylsp[-1]);
                }
    break;

  case 251:
#line 1327 "ned.y"
    {
                  addDisplayString(ps.submod,yylsp[-1]);
                }
    break;

  case 255:
#line 1343 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "false" );
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                }
    break;

  case 256:
#line 1349 "ned.y"
    {
                }
    break;

  case 257:
#line 1352 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "yes" );
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                }
    break;

  case 258:
#line 1358 "ned.y"
    {
                }
    break;

  case 265:
#line 1379 "ned.y"
    {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                }
    break;

  case 266:
#line 1383 "ned.y"
    {
                  ps.inLoop=1;
                }
    break;

  case 267:
#line 1387 "ned.y"
    {
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-7],yylsp[-4]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                }
    break;

  case 270:
#line 1401 "ned.y"
    {
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                }
    break;

  case 271:
#line 1411 "ned.y"
    {
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                }
    break;

  case 273:
#line 1419 "ned.y"
    {
                  addDisplayString(ps.conn,yylsp[0]);
                }
    break;

  case 277:
#line 1432 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 278:
#line 1437 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 279:
#line 1442 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 280:
#line 1448 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 283:
#line 1462 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 284:
#line 1468 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                }
    break;

  case 285:
#line 1476 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 286:
#line 1481 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                }
    break;

  case 287:
#line 1488 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 288:
#line 1495 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                }
    break;

  case 291:
#line 1509 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 292:
#line 1514 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[0]) );
                }
    break;

  case 293:
#line 1521 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 294:
#line 1526 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 295:
#line 1533 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 296:
#line 1538 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 297:
#line 1546 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                }
    break;

  case 298:
#line 1551 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 299:
#line 1556 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 302:
#line 1572 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "false" );
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                }
    break;

  case 303:
#line 1578 "ned.y"
    {
                }
    break;

  case 304:
#line 1581 "ned.y"
    {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "yes" );
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                }
    break;

  case 305:
#line 1587 "ned.y"
    {
                }
    break;

  case 312:
#line 1608 "ned.y"
    {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                }
    break;

  case 313:
#line 1612 "ned.y"
    {
                  ps.inLoop=1;
                }
    break;

  case 314:
#line 1616 "ned.y"
    {
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-7],yylsp[-4]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                }
    break;

  case 317:
#line 1630 "ned.y"
    {
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                }
    break;

  case 318:
#line 1640 "ned.y"
    {
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                }
    break;

  case 320:
#line 1648 "ned.y"
    {
                  addDisplayString(ps.conn,yylsp[0]);
                }
    break;

  case 324:
#line 1661 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 325:
#line 1666 "ned.y"
    {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 326:
#line 1671 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                }
    break;

  case 327:
#line 1677 "ned.y"
    {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                }
    break;

  case 330:
#line 1691 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 331:
#line 1697 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                }
    break;

  case 332:
#line 1705 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 333:
#line 1710 "ned.y"
    {
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                }
    break;

  case 334:
#line 1717 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 335:
#line 1724 "ned.y"
    {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                }
    break;

  case 338:
#line 1738 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 339:
#line 1743 "ned.y"
    {
                  ps.conn->setDestModule( toString(yylsp[0]) );
                }
    break;

  case 340:
#line 1750 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 341:
#line 1755 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 342:
#line 1762 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;

  case 343:
#line 1767 "ned.y"
    {
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;

  case 344:
#line 1775 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                }
    break;

  case 345:
#line 1780 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 346:
#line 1785 "ned.y"
    {
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;

  case 348:
#line 1803 "ned.y"
    {
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1],NULLPOS);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                }
    break;

  case 349:
#line 1809 "ned.y"
    {
                  ps.network = addNetwork(ps.nedfile,yylsp[-5],yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-6],yylsp[0]);
                  ps.inNetwork=1;
                }
    break;

  case 350:
#line 1818 "ned.y"
    {
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                }
    break;

  case 351:
#line 1832 "ned.y"
    {
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                }
    break;

  case 352:
#line 1840 "ned.y"
    {
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1],NULLPOS);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                }
    break;

  case 353:
#line 1846 "ned.y"
    {
                  ps.network = addNetwork(ps.nedfile,yylsp[-5],yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-6],yylsp[0]);
                  ps.inNetwork=1;
                }
    break;

  case 354:
#line 1859 "ned.y"
    { yyval = yyvsp[-1]; }
    break;

  case 355:
#line 1865 "ned.y"
    {
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
    break;

  case 356:
#line 1869 "ned.y"
    {
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
    break;

  case 357:
#line 1880 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); }
    break;

  case 358:
#line 1882 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); }
    break;

  case 359:
#line 1884 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input"); }
    break;

  case 360:
#line 1886 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("input"); }
    break;

  case 362:
#line 1892 "ned.y"
    { yyval = yyvsp[-1]; }
    break;

  case 363:
#line 1895 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); }
    break;

  case 364:
#line 1897 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); }
    break;

  case 365:
#line 1899 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); }
    break;

  case 366:
#line 1901 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); }
    break;

  case 367:
#line 1903 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); }
    break;

  case 368:
#line 1905 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); }
    break;

  case 369:
#line 1909 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); }
    break;

  case 370:
#line 1912 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); }
    break;

  case 371:
#line 1914 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); }
    break;

  case 372:
#line 1916 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); }
    break;

  case 373:
#line 1918 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); }
    break;

  case 374:
#line 1920 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); }
    break;

  case 375:
#line 1922 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); }
    break;

  case 376:
#line 1925 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); }
    break;

  case 377:
#line 1927 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); }
    break;

  case 378:
#line 1929 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); }
    break;

  case 379:
#line 1933 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); }
    break;

  case 380:
#line 1936 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); }
    break;

  case 381:
#line 1938 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); }
    break;

  case 382:
#line 1940 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); }
    break;

  case 383:
#line 1944 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); }
    break;

  case 384:
#line 1946 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); }
    break;

  case 385:
#line 1948 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); }
    break;

  case 386:
#line 1950 "ned.y"
    { if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); }
    break;

  case 387:
#line 1953 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); }
    break;

  case 388:
#line 1955 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); }
    break;

  case 389:
#line 1957 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); }
    break;

  case 390:
#line 1959 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); }
    break;

  case 396:
#line 1972 "ned.y"
    {
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                }
    break;

  case 397:
#line 1977 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                }
    break;

  case 398:
#line 1982 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                }
    break;

  case 399:
#line 1988 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                }
    break;

  case 400:
#line 1994 "ned.y"
    {
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                }
    break;

  case 401:
#line 2002 "ned.y"
    { yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); }
    break;

  case 402:
#line 2007 "ned.y"
    { yyval = createConst(NED_CONST_BOOL, "true"); }
    break;

  case 403:
#line 2009 "ned.y"
    { yyval = createConst(NED_CONST_BOOL, "false"); }
    break;

  case 405:
#line 2018 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("index"); }
    break;

  case 406:
#line 2020 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("index"); }
    break;

  case 407:
#line 2022 "ned.y"
    { if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); }
    break;

  case 408:
#line 2027 "ned.y"
    { yyval = createConst(NED_CONST_INT, toString(yylsp[0])); }
    break;

  case 409:
#line 2029 "ned.y"
    { yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); }
    break;

  case 410:
#line 2031 "ned.y"
    { yyval = createTimeConst(toString(yylsp[0])); }
    break;

  case 415:
#line 2048 "ned.y"
    {
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                }
    break;

  case 416:
#line 2054 "ned.y"
    {
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                }
    break;

  case 417:
#line 2063 "ned.y"
    {
                  ps.cppstruct = (CppStructNode *)createNodeWithTag(NED_CPP_STRUCT, ps.nedfile );
                  ps.cppstruct->setName(toString(yylsp[-1]));
                  setComments(ps.cppstruct,yylsp[-2],yylsp[-1]);
                }
    break;

  case 418:
#line 2072 "ned.y"
    {
                  ps.cppcobject = (CppCobjectNode *)createNodeWithTag(NED_CPP_COBJECT, ps.nedfile );
                  ps.cppcobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppcobject,yylsp[-2],yylsp[-1]);
                }
    break;

  case 419:
#line 2081 "ned.y"
    {
                  ps.cppnoncobject = (CppNoncobjectNode *)createNodeWithTag(NED_CPP_NONCOBJECT, ps.nedfile );
                  ps.cppnoncobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppnoncobject,yylsp[-2],yylsp[-1]);
                }
    break;

  case 420:
#line 2090 "ned.y"
    {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
    break;

  case 421:
#line 2097 "ned.y"
    {
                  setTrailingComment(ps.enump,yylsp[-1]);
                }
    break;

  case 422:
#line 2101 "ned.y"
    {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
    break;

  case 423:
#line 2109 "ned.y"
    {
                  setTrailingComment(ps.enump,yylsp[-1]);
                }
    break;

  case 428:
#line 2126 "ned.y"
    {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                }
    break;

  case 429:
#line 2131 "ned.y"
    {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                }
    break;

  case 430:
#line 2140 "ned.y"
    {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                }
    break;

  case 431:
#line 2146 "ned.y"
    {
                  setTrailingComment(ps.messagep,yylsp[-1]);
                }
    break;

  case 432:
#line 2150 "ned.y"
    {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                }
    break;

  case 433:
#line 2157 "ned.y"
    {
                  setTrailingComment(ps.messagep,yylsp[-1]);
                }
    break;

  case 434:
#line 2164 "ned.y"
    {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                }
    break;

  case 435:
#line 2170 "ned.y"
    {
                  setTrailingComment(ps.classp,yylsp[-1]);
                }
    break;

  case 436:
#line 2174 "ned.y"
    {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                }
    break;

  case 437:
#line 2181 "ned.y"
    {
                  setTrailingComment(ps.classp,yylsp[-1]);
                }
    break;

  case 438:
#line 2188 "ned.y"
    {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                }
    break;

  case 439:
#line 2194 "ned.y"
    {
                  setTrailingComment(ps.structp,yylsp[-1]);
                }
    break;

  case 440:
#line 2198 "ned.y"
    {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                }
    break;

  case 441:
#line 2205 "ned.y"
    {
                  setTrailingComment(ps.structp,yylsp[-1]);
                }
    break;

  case 442:
#line 2212 "ned.y"
    {
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                }
    break;

  case 449:
#line 2232 "ned.y"
    {
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                }
    break;

  case 456:
#line 2251 "ned.y"
    {
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                }
    break;

  case 463:
#line 2271 "ned.y"
    {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                }
    break;

  case 464:
#line 2277 "ned.y"
    {
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                }
    break;

  case 465:
#line 2281 "ned.y"
    {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(true);
                }
    break;

  case 466:
#line 2288 "ned.y"
    {
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                }
    break;

  case 476:
#line 2307 "ned.y"
    {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                }
    break;

  case 477:
#line 2312 "ned.y"
    {
                  ps.field->setIsVector(true);
                }
    break;

  case 479:
#line 2320 "ned.y"
    {
                  ps.field->setEnumName(toString(yylsp[-1]));
                }
    break;

  case 481:
#line 2328 "ned.y"
    {
                  ps.field->setDefaultValue(toString(yylsp[0]));
                }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 4069 "ned.tab.c"

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


#line 2348 "ned.y"


//----------------------------------------------------------------------
// general bison/flex stuff:
//

char yyfailure[250] = "";
extern int yydebug; /* needed if compiled with yacc --VA */

extern char textbuf[];
extern char lasttextbuf[];

int runparse (NEDParser *p,NedFileNode *nf,bool parseexpr, const char *sourcefname)
{
#if YYDEBUG != 0      /* #if added --VA */
    yydebug = YYDEBUGGING_ON;
#endif
    pos.co = 0;
    pos.li = 1;
    prevpos = pos;

    strcpy (lasttextbuf, "");
    strcpy (yyfailure, "");

    if (yyin)
        yyrestart( yyin );

    np = p;
    ps.nedfile = nf;
    ps.parseExpressions = parseexpr;
    sourcefilename = sourcefname;

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

SubmoduleNode *addSubmodule(NEDElement *submods, YYLTYPE namepos, YYLTYPE typepos,YYLTYPE likepos)
{
   SubmoduleNode *submod = (SubmoduleNode *)createNodeWithTag(NED_SUBMODULE,submods);
   submod->setName( toString( namepos) );
   submod->setTypeName( toString( typepos) );
   submod->setLikeName( toString( likepos) );

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


NetworkNode *addNetwork(NEDElement *nedfile, YYLTYPE namepos, YYLTYPE typepos, YYLTYPE likepos)
{
   NetworkNode *network = (NetworkNode *)createNodeWithTag(NED_NETWORK,nedfile);
   network->setName( toString( namepos) );
   network->setTypeName( toString( typepos) );
   network->setLikeName( toString( likepos) );
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

FunctionNode *createFunction(const char *funcname, NEDElement *arg1, NEDElement *arg2, NEDElement *arg3)
{
   FunctionNode *funcnode = (FunctionNode *)createNodeWithTag(NED_FUNCTION);
   funcnode->setName(funcname);
   if (arg1) funcnode->appendChild(arg1);
   if (arg2) funcnode->appendChild(arg2);
   if (arg3) funcnode->appendChild(arg3);
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


