
/*  A Bison parser, made from ned.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define YYLSP_NEEDED

#define	INCLUDE	257
#define	SIMPLE	258
#define	CHANNEL	259
#define	MODULE	260
#define	PARAMETERS	261
#define	GATES	262
#define	GATESIZES	263
#define	SUBMODULES	264
#define	CONNECTIONS	265
#define	DISPLAY	266
#define	IN	267
#define	OUT	268
#define	NOCHECK	269
#define	LEFT_ARROW	270
#define	RIGHT_ARROW	271
#define	FOR	272
#define	TO	273
#define	DO	274
#define	IF	275
#define	LIKE	276
#define	NETWORK	277
#define	ENDSIMPLE	278
#define	ENDMODULE	279
#define	ENDCHANNEL	280
#define	ENDNETWORK	281
#define	ENDFOR	282
#define	MACHINES	283
#define	ON	284
#define	CHANATTRNAME	285
#define	INTCONSTANT	286
#define	REALCONSTANT	287
#define	NAME	288
#define	STRINGCONSTANT	289
#define	CHARCONSTANT	290
#define	TRUE_	291
#define	FALSE_	292
#define	INPUT_	293
#define	REF	294
#define	ANCESTOR	295
#define	CONSTDECL	296
#define	NUMERICTYPE	297
#define	STRINGTYPE	298
#define	BOOLTYPE	299
#define	ANYTYPE	300
#define	CPLUSPLUS	301
#define	CPLUSPLUSBODY	302
#define	MESSAGE	303
#define	CLASS	304
#define	STRUCT	305
#define	ENUM	306
#define	NONCOBJECT	307
#define	EXTENDS	308
#define	FIELDS	309
#define	PROPERTIES	310
#define	ABSTRACT	311
#define	CHARTYPE	312
#define	SHORTTYPE	313
#define	INTTYPE	314
#define	LONGTYPE	315
#define	DOUBLETYPE	316
#define	UNSIGNED_	317
#define	SIZEOF	318
#define	SUBMODINDEX	319
#define	PLUSPLUS	320
#define	EQ	321
#define	NE	322
#define	GT	323
#define	GE	324
#define	LS	325
#define	LE	326
#define	AND	327
#define	OR	328
#define	XOR	329
#define	NOT	330
#define	BIN_AND	331
#define	BIN_OR	332
#define	BIN_XOR	333
#define	BIN_COMPL	334
#define	SHIFT_LEFT	335
#define	SHIFT_RIGHT	336
#define	INVALID_CHAR	337
#define	UMIN	338

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

#ifndef YYSTYPE
#define YYSTYPE int
#endif

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		870
#define	YYFLAG		-32768
#define	YYNTBASE	103

#define YYTRANSLATE(x) ((unsigned)(x) <= 338 ? yytranslate[x] : 350)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    90,     2,     2,   101,
   102,    88,    86,    94,    87,   100,    89,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    85,    93,     2,
    97,     2,    84,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    98,     2,    99,    91,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    95,     2,    96,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82,    83,    92
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     6,     8,    10,    12,    14,    16,    18,
    20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
    40,    41,    46,    50,    52,    54,    58,    61,    62,    64,
    69,    73,    77,    80,    86,    89,    91,    92,    98,   103,
   109,   112,   116,   119,   127,   130,   139,   142,   146,   149,
   160,   163,   165,   166,   167,   172,   175,   176,   180,   182,
   184,   186,   187,   188,   193,   196,   197,   201,   203,   205,
   207,   208,   213,   215,   216,   221,   223,   224,   225,   230,
   233,   234,   238,   240,   242,   243,   244,   249,   252,   253,
   257,   259,   261,   263,   267,   270,   274,   279,   284,   288,
   292,   296,   298,   299,   300,   305,   307,   308,   313,   317,
   322,   326,   330,   332,   336,   338,   342,   344,   348,   350,
   352,   353,   354,   359,   361,   362,   367,   371,   376,   380,
   384,   386,   390,   392,   396,   398,   402,   404,   406,   407,
   408,   413,   415,   416,   419,   421,   422,   430,   431,   440,
   441,   451,   452,   463,   467,   469,   470,   471,   476,   478,
   479,   482,   484,   485,   493,   494,   503,   504,   514,   515,
   526,   530,   532,   533,   536,   538,   539,   544,   545,   552,
   555,   556,   560,   562,   564,   566,   567,   570,   572,   573,
   578,   579,   586,   589,   590,   594,   596,   598,   600,   601,
   604,   606,   607,   612,   613,   620,   623,   624,   628,   630,
   634,   636,   637,   640,   642,   643,   648,   649,   656,   658,
   659,   662,   664,   669,   672,   673,   674,   679,   680,   687,
   690,   691,   695,   697,   700,   702,   705,   706,   707,   712,
   713,   720,   723,   724,   728,   730,   733,   735,   740,   741,
   746,   747,   749,   750,   751,   757,   758,   763,   765,   766,
   769,   771,   773,   775,   776,   784,   788,   790,   796,   799,
   800,   803,   804,   807,   809,   816,   825,   832,   841,   845,
   847,   850,   852,   855,   857,   860,   863,   865,   868,   872,
   874,   877,   879,   882,   884,   887,   890,   892,   895,   897,
   900,   904,   906,   907,   908,   914,   915,   920,   922,   923,
   926,   928,   930,   932,   933,   941,   945,   947,   953,   956,
   957,   960,   961,   964,   966,   973,   982,   989,   998,  1002,
  1004,  1007,  1009,  1012,  1014,  1017,  1020,  1022,  1025,  1029,
  1031,  1034,  1036,  1039,  1041,  1044,  1047,  1049,  1052,  1054,
  1057,  1061,  1066,  1072,  1075,  1082,  1088,  1092,  1094,  1096,
  1103,  1108,  1112,  1114,  1116,  1120,  1124,  1128,  1132,  1136,
  1140,  1144,  1147,  1151,  1155,  1159,  1163,  1167,  1171,  1175,
  1179,  1183,  1186,  1190,  1194,  1198,  1201,  1205,  1209,  1215,
  1219,  1224,  1231,  1240,  1251,  1253,  1255,  1257,  1259,  1261,
  1263,  1266,  1270,  1274,  1277,  1279,  1281,  1283,  1285,  1287,
  1291,  1296,  1298,  1300,  1302,  1306,  1310,  1313,  1316,  1320,
  1324,  1328,  1333,  1337,  1338,  1346,  1347,  1357,  1359,  1360,
  1363,  1365,  1368,  1373,  1374,  1383,  1384,  1395,  1396,  1405,
  1406,  1417,  1418,  1427,  1428,  1439,  1440,  1445,  1446,  1448,
  1449,  1452,  1454,  1459,  1461,  1463,  1465,  1467,  1469,  1471,
  1472,  1477,  1478,  1480,  1481,  1484,  1486,  1487,  1495,  1496,
  1505,  1507,  1510,  1512,  1514,  1516,  1518,  1521,  1524,  1527,
  1530,  1532,  1534,  1536,  1540,  1544,  1547,  1548,  1553,  1554,
  1557,  1558,  1560,  1562,  1564,  1567,  1569,  1572,  1574,  1576,
  1578,  1580,  1582,  1585,  1587,  1589,  1590,  1592
};

static const short yyrhs[] = {   104,
     0,   104,   105,     0,     0,   106,     0,   115,     0,   122,
     0,   127,     0,   296,     0,   110,     0,   119,     0,   124,
     0,   293,     0,   310,     0,   311,     0,   312,     0,   313,
     0,   314,     0,   320,     0,   323,     0,   326,     0,     0,
     3,   107,   108,    93,     0,   108,    94,   109,     0,   109,
     0,    35,     0,   111,   112,   114,     0,     5,    34,     0,
     0,   113,     0,   113,    31,   299,   348,     0,    31,   299,
   348,     0,    26,    34,   348,     0,    26,   348,     0,   116,
    95,   117,    96,   348,     0,     5,    34,     0,   118,     0,
     0,   118,    34,    97,   299,    93,     0,    34,    97,   299,
    93,     0,   120,   129,   145,   157,   121,     0,     4,    34,
     0,    24,    34,   348,     0,    24,   348,     0,   123,    95,
   135,   150,   166,    96,   348,     0,     4,    34,     0,   125,
   129,   145,   157,   175,   245,   141,   126,     0,     6,    34,
     0,    25,    34,   348,     0,    25,   348,     0,   128,    95,
   135,   150,   166,   186,   269,   143,    96,   348,     0,     6,
    34,     0,   130,     0,     0,     0,    29,    85,   131,   132,
     0,   133,    93,     0,     0,   133,    94,   134,     0,   134,
     0,    34,     0,   136,     0,     0,     0,    29,    85,   137,
   138,     0,   139,    93,     0,     0,   139,    94,   140,     0,
   140,     0,    34,     0,   142,     0,     0,    12,    85,    35,
    93,     0,   144,     0,     0,    12,    85,    35,    93,     0,
   146,     0,     0,     0,     7,    85,   147,   148,     0,   149,
    93,     0,     0,   149,    94,   155,     0,   155,     0,   151,
     0,     0,     0,     7,    85,   152,   153,     0,   154,    93,
     0,     0,   154,    93,   156,     0,   156,     0,   156,     0,
    34,     0,    34,    85,    43,     0,    42,    34,     0,    34,
    85,    42,     0,    34,    85,    42,    43,     0,    34,    85,
    43,    42,     0,    34,    85,    44,     0,    34,    85,    45,
     0,    34,    85,    46,     0,   158,     0,     0,     0,     8,
    85,   159,   160,     0,   161,     0,     0,   161,    13,   162,
    93,     0,    13,   162,    93,     0,   161,    14,   164,    93,
     0,    14,   164,    93,     0,   162,    94,   163,     0,   163,
     0,    34,    98,    99,     0,    34,     0,   164,    94,   165,
     0,   165,     0,    34,    98,    99,     0,    34,     0,   167,
     0,     0,     0,     8,    85,   168,   169,     0,   170,     0,
     0,   170,    13,   171,    93,     0,    13,   171,    93,     0,
   170,    14,   173,    93,     0,    14,   173,    93,     0,   171,
    94,   172,     0,   172,     0,    34,    98,    99,     0,    34,
     0,   173,    94,   174,     0,   174,     0,    34,    98,    99,
     0,    34,     0,   176,     0,     0,     0,    10,    85,   177,
   178,     0,   179,     0,     0,   179,   180,     0,   180,     0,
     0,    34,    85,    34,   348,   181,   197,   185,     0,     0,
    34,    85,    34,   298,   348,   182,   197,   185,     0,     0,
    34,    85,    34,    22,    34,   348,   183,   197,   185,     0,
     0,    34,    85,    34,   298,    22,    34,   348,   184,   197,
   185,     0,   213,   229,   243,     0,   187,     0,     0,     0,
    10,    85,   188,   189,     0,   190,     0,     0,   190,   191,
     0,   191,     0,     0,    34,    85,    34,   348,   192,   205,
   196,     0,     0,    34,    85,    34,   298,   348,   193,   205,
   196,     0,     0,    34,    85,    34,    22,    34,   348,   194,
   205,   196,     0,     0,    34,    85,    34,   298,    22,    34,
   348,   195,   205,   196,     0,   221,   236,   244,     0,   198,
     0,     0,   198,   199,     0,   199,     0,     0,    30,    85,
   200,   202,     0,     0,    30,    21,   299,    85,   201,   202,
     0,   203,    93,     0,     0,   203,    94,   204,     0,   204,
     0,    34,     0,   206,     0,     0,   206,   207,     0,   207,
     0,     0,    30,    85,   208,   210,     0,     0,    30,    21,
   299,    85,   209,   210,     0,   211,    93,     0,     0,   211,
    94,   212,     0,   212,     0,    34,     0,   214,     0,     0,
   214,   215,     0,   215,     0,     0,     7,    85,   216,   218,
     0,     0,     7,    21,   299,    85,   217,   218,     0,   219,
    93,     0,     0,   219,    94,   220,     0,   220,     0,    34,
    97,   299,     0,   222,     0,     0,   222,   223,     0,   223,
     0,     0,     7,    85,   224,   226,     0,     0,     7,    21,
   299,    85,   225,   226,     0,   227,     0,     0,   227,   228,
     0,   228,     0,    34,    97,   299,    93,     0,   229,   230,
     0,     0,     0,     9,    85,   231,   233,     0,     0,     9,
    21,   299,    85,   232,   233,     0,   234,    93,     0,     0,
   234,    94,   235,     0,   235,     0,    34,   298,     0,    34,
     0,   236,   237,     0,     0,     0,     9,    85,   238,   240,
     0,     0,     9,    21,   299,    85,   239,   240,     0,   241,
    93,     0,     0,   241,    94,   242,     0,   242,     0,    34,
   298,     0,    34,     0,    12,    85,    35,    93,     0,     0,
    12,    85,    35,    93,     0,     0,   246,     0,     0,     0,
    11,    15,    85,   247,   249,     0,     0,    11,    85,   248,
   249,     0,   250,     0,     0,   250,   251,     0,   251,     0,
   252,     0,   259,     0,     0,    18,   253,   254,    20,   258,
    28,   348,     0,   255,    94,   254,     0,   255,     0,    34,
    97,   299,    19,   299,     0,    21,   299,     0,     0,    12,
    35,     0,     0,   258,   259,     0,   259,     0,   260,    17,
   264,   256,   257,   349,     0,   260,    17,   268,    17,   264,
   256,   257,   349,     0,   260,    16,   264,   256,   257,   349,
     0,   260,    16,   268,    16,   264,   256,   257,   349,     0,
   261,   100,   262,     0,   263,     0,    34,   298,     0,    34,
     0,    34,   298,     0,    34,     0,    34,    66,     0,    34,
   298,     0,    34,     0,    34,    66,     0,   265,   100,   266,
     0,   267,     0,    34,   298,     0,    34,     0,    34,   298,
     0,    34,     0,    34,    66,     0,    34,   298,     0,    34,
     0,    34,    66,     0,    34,     0,    31,   299,     0,   268,
    31,   299,     0,   270,     0,     0,     0,    11,    15,    85,
   271,   273,     0,     0,    11,    85,   272,   273,     0,   274,
     0,     0,   274,   275,     0,   275,     0,   276,     0,   283,
     0,     0,    18,   277,   278,    20,   282,    28,    93,     0,
   279,    94,   278,     0,   279,     0,    34,    97,   299,    19,
   299,     0,    21,   299,     0,     0,    12,    35,     0,     0,
   282,   283,     0,   283,     0,   284,    17,   288,   280,   281,
    93,     0,   284,    17,   292,    17,   288,   280,   281,    93,
     0,   284,    16,   288,   280,   281,    93,     0,   284,    16,
   292,    16,   288,   280,   281,    93,     0,   285,   100,   286,
     0,   287,     0,    34,   298,     0,    34,     0,    34,   298,
     0,    34,     0,    34,    66,     0,    34,   298,     0,    34,
     0,    34,    66,     0,   289,   100,   290,     0,   291,     0,
    34,   298,     0,    34,     0,    34,   298,     0,    34,     0,
    34,    66,     0,    34,   298,     0,    34,     0,    34,    66,
     0,    34,     0,    34,   299,     0,   292,    34,   299,     0,
   294,   197,   213,   295,     0,    23,    34,    85,    34,   348,
     0,    27,   348,     0,   297,    95,   205,   221,    96,   348,
     0,    23,    34,    85,    34,   348,     0,    98,   299,    99,
     0,   301,     0,   300,     0,    39,   101,   301,    94,   301,
   102,     0,    39,   101,   301,   102,     0,    39,   101,   102,
     0,    39,     0,   302,     0,   101,   301,   102,     0,   301,
    86,   301,     0,   301,    87,   301,     0,   301,    88,   301,
     0,   301,    89,   301,     0,   301,    90,   301,     0,   301,
    91,   301,     0,    87,   301,     0,   301,    67,   301,     0,
   301,    68,   301,     0,   301,    69,   301,     0,   301,    70,
   301,     0,   301,    71,   301,     0,   301,    72,   301,     0,
   301,    73,   301,     0,   301,    74,   301,     0,   301,    75,
   301,     0,    76,   301,     0,   301,    77,   301,     0,   301,
    78,   301,     0,   301,    79,   301,     0,    80,   301,     0,
   301,    81,   301,     0,   301,    82,   301,     0,   301,    84,
   301,    85,   301,     0,    34,   101,   102,     0,    34,   101,
   301,   102,     0,    34,   101,   301,    94,   301,   102,     0,
    34,   101,   301,    94,   301,    94,   301,   102,     0,    34,
   101,   301,    94,   301,    94,   301,    94,   301,   102,     0,
   303,     0,   304,     0,   305,     0,   306,     0,   307,     0,
    34,     0,    40,    34,     0,    40,    41,    34,     0,    41,
    40,    34,     0,    41,    34,     0,    35,     0,    37,     0,
    38,     0,   308,     0,    65,     0,    65,   101,   102,     0,
    64,   101,    34,   102,     0,    32,     0,    33,     0,   309,
     0,   309,    32,    34,     0,   309,    33,    34,     0,    32,
    34,     0,    33,    34,     0,    47,    48,   348,     0,    51,
    34,    93,     0,    50,    34,    93,     0,    50,    53,    34,
    93,     0,    52,    34,    93,     0,     0,    52,    34,    95,
   315,   317,    96,   348,     0,     0,    52,    34,    54,    34,
    95,   316,   317,    96,   348,     0,   318,     0,     0,   318,
   319,     0,   319,     0,    34,    93,     0,    34,    97,   347,
    93,     0,     0,    49,    34,    95,   321,   329,   335,    96,
   348,     0,     0,    49,    34,    54,    34,    95,   322,   329,
   335,    96,   348,     0,     0,    50,    34,    95,   324,   329,
   335,    96,   348,     0,     0,    50,    34,    54,    34,    95,
   325,   329,   335,    96,   348,     0,     0,    51,    34,    95,
   327,   329,   335,    96,   348,     0,     0,    51,    34,    54,
    34,    95,   328,   329,   335,    96,   348,     0,     0,    56,
    85,   330,   331,     0,     0,   332,     0,     0,   332,   333,
     0,   333,     0,    34,    97,   334,    93,     0,    35,     0,
    32,     0,    33,     0,   309,     0,    37,     0,    38,     0,
     0,    55,    85,   336,   337,     0,     0,   338,     0,     0,
   338,   339,     0,   339,     0,     0,   342,    34,   340,   343,
   344,   345,    93,     0,     0,    57,   342,    34,   341,   343,
   344,   345,    93,     0,    34,     0,    34,    88,     0,    58,
     0,    59,     0,    60,     0,    61,     0,    63,    58,     0,
    63,    59,     0,    63,    60,     0,    63,    61,     0,    62,
     0,    44,     0,    45,     0,    98,    32,    99,     0,    98,
    34,    99,     0,    98,    99,     0,     0,    52,   101,    34,
   102,     0,     0,    97,   346,     0,     0,    35,     0,    36,
     0,    32,     0,    87,    32,     0,    33,     0,    87,    33,
     0,   309,     0,    37,     0,    38,     0,    34,     0,    32,
     0,    87,    32,     0,    34,     0,    93,     0,     0,    94,
     0,    93,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   237,   241,   242,   246,   248,   250,   252,   254,   257,   259,
   261,   263,   266,   267,   268,   269,   271,   273,   275,   277,
   285,   290,   297,   298,   302,   314,   318,   327,   328,   332,
   338,   347,   351,   361,   370,   380,   381,   385,   391,   403,
   411,   421,   425,   435,   446,   459,   470,   480,   484,   494,
   508,   521,   522,   526,   532,   537,   538,   542,   543,   547,
   559,   560,   564,   570,   575,   576,   580,   581,   585,   597,
   598,   602,   612,   613,   617,   627,   628,   632,   638,   643,
   644,   648,   652,   662,   663,   667,   673,   678,   679,   683,
   687,   698,   702,   706,   710,   714,   718,   722,   726,   730,
   734,   744,   745,   749,   755,   760,   761,   765,   766,   767,
   768,   772,   773,   777,   782,   790,   791,   795,   800,   811,
   812,   816,   822,   827,   828,   832,   833,   834,   835,   839,
   840,   844,   849,   857,   858,   862,   867,   878,   879,   883,
   889,   894,   895,   899,   900,   904,   910,   913,   920,   923,
   929,   932,   939,   945,   954,   955,   959,   965,   970,   971,
   975,   976,   980,   986,   989,   996,   999,  1005,  1008,  1015,
  1021,  1030,  1031,  1035,  1036,  1040,  1046,  1048,  1055,  1060,
  1061,  1065,  1066,  1070,  1081,  1082,  1086,  1087,  1091,  1097,
  1099,  1106,  1111,  1112,  1116,  1117,  1121,  1132,  1133,  1137,
  1138,  1142,  1148,  1150,  1157,  1163,  1164,  1168,  1169,  1173,
  1185,  1186,  1190,  1191,  1195,  1201,  1203,  1210,  1216,  1217,
  1221,  1222,  1226,  1238,  1239,  1243,  1249,  1251,  1258,  1263,
  1264,  1268,  1269,  1273,  1280,  1291,  1292,  1296,  1302,  1304,
  1311,  1316,  1317,  1321,  1322,  1326,  1333,  1344,  1348,  1355,
  1359,  1366,  1367,  1371,  1378,  1380,  1387,  1392,  1393,  1397,
  1398,  1402,  1403,  1407,  1412,  1421,  1422,  1426,  1436,  1440,
  1444,  1448,  1452,  1453,  1457,  1462,  1467,  1473,  1482,  1483,
  1487,  1493,  1501,  1506,  1510,  1518,  1525,  1531,  1541,  1542,
  1546,  1551,  1558,  1563,  1567,  1575,  1580,  1584,  1593,  1598,
  1603,  1614,  1615,  1619,  1626,  1628,  1635,  1640,  1641,  1645,
  1646,  1650,  1651,  1655,  1660,  1669,  1670,  1674,  1684,  1688,
  1692,  1696,  1700,  1701,  1705,  1710,  1715,  1721,  1730,  1731,
  1735,  1741,  1749,  1754,  1758,  1766,  1773,  1779,  1789,  1790,
  1794,  1799,  1806,  1811,  1815,  1823,  1828,  1832,  1841,  1846,
  1851,  1862,  1869,  1878,  1889,  1900,  1913,  1918,  1923,  1934,
  1936,  1938,  1940,  1945,  1946,  1949,  1951,  1953,  1955,  1957,
  1959,  1962,  1966,  1968,  1970,  1972,  1974,  1976,  1979,  1981,
  1983,  1986,  1990,  1992,  1994,  1997,  2000,  2002,  2004,  2007,
  2009,  2011,  2013,  2015,  2020,  2021,  2022,  2023,  2024,  2028,
  2033,  2038,  2044,  2050,  2058,  2063,  2065,  2070,  2074,  2076,
  2078,  2083,  2085,  2087,  2093,  2094,  2095,  2096,  2104,  2113,
  2122,  2129,  2139,  2148,  2155,  2159,  2167,  2174,  2175,  2179,
  2180,  2184,  2190,  2200,  2206,  2210,  2217,  2224,  2230,  2234,
  2241,  2248,  2254,  2258,  2265,  2272,  2278,  2278,  2282,  2283,
  2287,  2288,  2292,  2302,  2303,  2304,  2305,  2306,  2307,  2311,
  2317,  2317,  2321,  2322,  2326,  2327,  2331,  2337,  2341,  2348,
  2355,  2356,  2358,  2359,  2360,  2361,  2363,  2364,  2365,  2366,
  2368,  2369,  2370,  2375,  2380,  2385,  2389,  2393,  2397,  2401,
  2405,  2409,  2410,  2411,  2412,  2413,  2414,  2415,  2416,  2417,
  2418,  2422,  2423,  2424,  2427,  2427,  2429,  2429
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","INCLUDE",
"SIMPLE","CHANNEL","MODULE","PARAMETERS","GATES","GATESIZES","SUBMODULES","CONNECTIONS",
"DISPLAY","IN","OUT","NOCHECK","LEFT_ARROW","RIGHT_ARROW","FOR","TO","DO","IF",
"LIKE","NETWORK","ENDSIMPLE","ENDMODULE","ENDCHANNEL","ENDNETWORK","ENDFOR",
"MACHINES","ON","CHANATTRNAME","INTCONSTANT","REALCONSTANT","NAME","STRINGCONSTANT",
"CHARCONSTANT","TRUE_","FALSE_","INPUT_","REF","ANCESTOR","CONSTDECL","NUMERICTYPE",
"STRINGTYPE","BOOLTYPE","ANYTYPE","CPLUSPLUS","CPLUSPLUSBODY","MESSAGE","CLASS",
"STRUCT","ENUM","NONCOBJECT","EXTENDS","FIELDS","PROPERTIES","ABSTRACT","CHARTYPE",
"SHORTTYPE","INTTYPE","LONGTYPE","DOUBLETYPE","UNSIGNED_","SIZEOF","SUBMODINDEX",
"PLUSPLUS","EQ","NE","GT","GE","LS","LE","AND","OR","XOR","NOT","BIN_AND","BIN_OR",
"BIN_XOR","BIN_COMPL","SHIFT_LEFT","SHIFT_RIGHT","INVALID_CHAR","'?'","':'",
"'+'","'-'","'*'","'/'","'%'","'^'","UMIN","';'","','","'{'","'}'","'='","'['",
"']'","'.'","'('","')'","networkdescription","somedefinitions","definition",
"import","@1","filenames","filename","channeldefinition_old","channelheader_old",
"opt_channelattrblock_old","channelattrblock_old","endchannel_old","channeldefinition",
"channelheader","opt_channelattrblock","channelattrblock","simpledefinition_old",
"simpleheader_old","endsimple_old","simpledefinition","simpleheader","moduledefinition_old",
"moduleheader_old","endmodule_old","moduledefinition","moduleheader","opt_machineblock_old",
"machineblock_old","@2","opt_machinelist_old","machinelist_old","machine_old",
"opt_machineblock","machineblock","@3","opt_machinelist","machinelist","machine",
"opt_displayblock_old","displayblock_old","opt_displayblock","displayblock",
"opt_paramblock_old","paramblock_old","@4","opt_parameters_old","parameters_old",
"opt_paramblock","paramblock","@5","opt_parameters","parameters","parameter_old",
"parameter","opt_gateblock_old","gateblock_old","@6","opt_gates_old","gates_old",
"gatesI_old","gateI_old","gatesO_old","gateO_old","opt_gateblock","gateblock",
"@7","opt_gates","gates","gatesI","gateI","gatesO","gateO","opt_submodblock_old",
"submodblock_old","@8","opt_submodules_old","submodules_old","submodule_old",
"@9","@10","@11","@12","submodule_body_old","opt_submodblock","submodblock",
"@13","opt_submodules","submodules","submodule","@14","@15","@16","@17","submodule_body",
"opt_on_blocks_old","on_blocks_old","on_block_old","@18","@19","opt_on_list_old",
"on_list_old","on_mach_old","opt_on_blocks","on_blocks","on_block","@20","@21",
"opt_on_list","on_list","on_mach","opt_substparamblocks_old","substparamblocks_old",
"substparamblock_old","@22","@23","opt_substparameters_old","substparameters_old",
"substparameter_old","opt_substparamblocks","substparamblocks","substparamblock",
"@24","@25","opt_substparameters","substparameters","substparameter","opt_gatesizeblocks_old",
"gatesizeblock_old","@26","@27","opt_gatesizes_old","gatesizes_old","gatesize_old",
"opt_gatesizeblocks","gatesizeblock","@28","@29","opt_gatesizes","gatesizes",
"gatesize","opt_submod_displayblock_old","opt_submod_displayblock","opt_connblock_old",
"connblock_old","@30","@31","opt_connections_old","connections_old","connection_old",
"loopconnection_old","@32","loopvarlist_old","loopvar_old","opt_conncondition_old",
"opt_conn_displaystr_old","notloopconnections_old","notloopconnection_old","leftgatespec_old",
"leftmod_old","leftgate_old","parentleftgate_old","rightgatespec_old","rightmod_old",
"rightgate_old","parentrightgate_old","channeldescr_old","opt_connblock","connblock",
"@33","@34","opt_connections","connections","connection","loopconnection","@35",
"loopvarlist","loopvar","opt_conncondition","opt_conn_displaystr","notloopconnections",
"notloopconnection","leftgatespec","leftmod","leftgate","parentleftgate","rightgatespec",
"rightmod","rightgate","parentrightgate","channeldescr","networkdefinition_old",
"networkheader_old","endnetwork_old","networkdefinition","networkheader","vector",
"expression","inputvalue","expr","simple_expr","parameter_expr","string_expr",
"boolconst_expr","numconst_expr","special_expr","numconst","timeconstant","cplusplus",
"struct_decl","class_decl","enum_decl","enum","@36","@37","opt_enumfields","enumfields",
"enumfield","message","@38","@39","class","@40","@41","struct","@42","@43","opt_propertiesblock",
"@44","opt_properties","properties","property","propertyvalue","opt_fieldsblock",
"@45","opt_fields","fields","field","@46","@47","fielddatatype","opt_fieldvector",
"opt_fieldenum","opt_fieldvalue","fieldvalue","enumvalue","opt_semicolon","comma_or_semicolon", NULL
};
#endif

static const short yyr1[] = {     0,
   103,   104,   104,   105,   105,   105,   105,   105,   105,   105,
   105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
   107,   106,   108,   108,   109,   110,   111,   112,   112,   113,
   113,   114,   114,   115,   116,   117,   117,   118,   118,   119,
   120,   121,   121,   122,   123,   124,   125,   126,   126,   127,
   128,   129,   129,   131,   130,   132,   132,   133,   133,   134,
   135,   135,   137,   136,   138,   138,   139,   139,   140,   141,
   141,   142,   143,   143,   144,   145,   145,   147,   146,   148,
   148,   149,   149,   150,   150,   152,   151,   153,   153,   154,
   154,   155,   156,   156,   156,   156,   156,   156,   156,   156,
   156,   157,   157,   159,   158,   160,   160,   161,   161,   161,
   161,   162,   162,   163,   163,   164,   164,   165,   165,   166,
   166,   168,   167,   169,   169,   170,   170,   170,   170,   171,
   171,   172,   172,   173,   173,   174,   174,   175,   175,   177,
   176,   178,   178,   179,   179,   181,   180,   182,   180,   183,
   180,   184,   180,   185,   186,   186,   188,   187,   189,   189,
   190,   190,   192,   191,   193,   191,   194,   191,   195,   191,
   196,   197,   197,   198,   198,   200,   199,   201,   199,   202,
   202,   203,   203,   204,   205,   205,   206,   206,   208,   207,
   209,   207,   210,   210,   211,   211,   212,   213,   213,   214,
   214,   216,   215,   217,   215,   218,   218,   219,   219,   220,
   221,   221,   222,   222,   224,   223,   225,   223,   226,   226,
   227,   227,   228,   229,   229,   231,   230,   232,   230,   233,
   233,   234,   234,   235,   235,   236,   236,   238,   237,   239,
   237,   240,   240,   241,   241,   242,   242,   243,   243,   244,
   244,   245,   245,   247,   246,   248,   246,   249,   249,   250,
   250,   251,   251,   253,   252,   254,   254,   255,   256,   256,
   257,   257,   258,   258,   259,   259,   259,   259,   260,   260,
   261,   261,   262,   262,   262,   263,   263,   263,   264,   264,
   265,   265,   266,   266,   266,   267,   267,   267,   268,   268,
   268,   269,   269,   271,   270,   272,   270,   273,   273,   274,
   274,   275,   275,   277,   276,   278,   278,   279,   280,   280,
   281,   281,   282,   282,   283,   283,   283,   283,   284,   284,
   285,   285,   286,   286,   286,   287,   287,   287,   288,   288,
   289,   289,   290,   290,   290,   291,   291,   291,   292,   292,
   292,   293,   294,   295,   296,   297,   298,   299,   299,   300,
   300,   300,   300,   301,   301,   301,   301,   301,   301,   301,
   301,   301,   301,   301,   301,   301,   301,   301,   301,   301,
   301,   301,   301,   301,   301,   301,   301,   301,   301,   301,
   301,   301,   301,   301,   302,   302,   302,   302,   302,   303,
   303,   303,   303,   303,   304,   305,   305,   306,   307,   307,
   307,   308,   308,   308,   309,   309,   309,   309,   310,   311,
   312,   312,   313,   315,   314,   316,   314,   317,   317,   318,
   318,   319,   319,   321,   320,   322,   320,   324,   323,   325,
   323,   327,   326,   328,   326,   330,   329,   329,   331,   331,
   332,   332,   333,   334,   334,   334,   334,   334,   334,   336,
   335,   335,   337,   337,   338,   338,   340,   339,   341,   339,
   342,   342,   342,   342,   342,   342,   342,   342,   342,   342,
   342,   342,   342,   343,   343,   343,   343,   344,   344,   345,
   345,   346,   346,   346,   346,   346,   346,   346,   346,   346,
   346,   347,   347,   347,   348,   348,   349,   349
};

static const short yyr2[] = {     0,
     1,     2,     0,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     0,     4,     3,     1,     1,     3,     2,     0,     1,     4,
     3,     3,     2,     5,     2,     1,     0,     5,     4,     5,
     2,     3,     2,     7,     2,     8,     2,     3,     2,    10,
     2,     1,     0,     0,     4,     2,     0,     3,     1,     1,
     1,     0,     0,     4,     2,     0,     3,     1,     1,     1,
     0,     4,     1,     0,     4,     1,     0,     0,     4,     2,
     0,     3,     1,     1,     0,     0,     4,     2,     0,     3,
     1,     1,     1,     3,     2,     3,     4,     4,     3,     3,
     3,     1,     0,     0,     4,     1,     0,     4,     3,     4,
     3,     3,     1,     3,     1,     3,     1,     3,     1,     1,
     0,     0,     4,     1,     0,     4,     3,     4,     3,     3,
     1,     3,     1,     3,     1,     3,     1,     1,     0,     0,
     4,     1,     0,     2,     1,     0,     7,     0,     8,     0,
     9,     0,    10,     3,     1,     0,     0,     4,     1,     0,
     2,     1,     0,     7,     0,     8,     0,     9,     0,    10,
     3,     1,     0,     2,     1,     0,     4,     0,     6,     2,
     0,     3,     1,     1,     1,     0,     2,     1,     0,     4,
     0,     6,     2,     0,     3,     1,     1,     1,     0,     2,
     1,     0,     4,     0,     6,     2,     0,     3,     1,     3,
     1,     0,     2,     1,     0,     4,     0,     6,     1,     0,
     2,     1,     4,     2,     0,     0,     4,     0,     6,     2,
     0,     3,     1,     2,     1,     2,     0,     0,     4,     0,
     6,     2,     0,     3,     1,     2,     1,     4,     0,     4,
     0,     1,     0,     0,     5,     0,     4,     1,     0,     2,
     1,     1,     1,     0,     7,     3,     1,     5,     2,     0,
     2,     0,     2,     1,     6,     8,     6,     8,     3,     1,
     2,     1,     2,     1,     2,     2,     1,     2,     3,     1,
     2,     1,     2,     1,     2,     2,     1,     2,     1,     2,
     3,     1,     0,     0,     5,     0,     4,     1,     0,     2,
     1,     1,     1,     0,     7,     3,     1,     5,     2,     0,
     2,     0,     2,     1,     6,     8,     6,     8,     3,     1,
     2,     1,     2,     1,     2,     2,     1,     2,     3,     1,
     2,     1,     2,     1,     2,     2,     1,     2,     1,     2,
     3,     4,     5,     2,     6,     5,     3,     1,     1,     6,
     4,     3,     1,     1,     3,     3,     3,     3,     3,     3,
     3,     2,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     2,     3,     3,     3,     2,     3,     3,     5,     3,
     4,     6,     8,    10,     1,     1,     1,     1,     1,     1,
     2,     3,     3,     2,     1,     1,     1,     1,     1,     3,
     4,     1,     1,     1,     3,     3,     2,     2,     3,     3,
     3,     4,     3,     0,     7,     0,     9,     1,     0,     2,
     1,     2,     4,     0,     8,     0,    10,     0,     8,     0,
    10,     0,     8,     0,    10,     0,     4,     0,     1,     0,
     2,     1,     4,     1,     1,     1,     1,     1,     1,     0,
     4,     0,     1,     0,     2,     1,     0,     7,     0,     8,
     1,     2,     1,     1,     1,     1,     2,     2,     2,     2,
     1,     1,     1,     3,     3,     2,     0,     4,     0,     2,
     0,     1,     1,     1,     2,     1,     2,     1,     1,     1,
     1,     1,     2,     1,     1,     0,     1,     1
};

static const short yydefact[] = {     3,
     1,    21,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     2,     4,     9,    28,     5,     0,    10,    53,     6,
     0,    11,    53,     7,     0,    12,   173,     8,     0,    13,
    14,    15,    16,    17,    18,    19,    20,     0,    41,    27,
    47,     0,   506,     0,     0,     0,     0,     0,     0,     0,
    29,    37,     0,    77,    52,    62,    77,    62,     0,   199,
   172,   175,   186,    25,     0,    24,     0,   505,   419,     0,
   434,     0,   421,   438,     0,     0,   420,   442,     0,   423,
   424,   412,   413,   400,   405,   406,   407,   363,     0,     0,
     0,   409,     0,     0,     0,     0,   506,   359,   358,   364,
   395,   396,   397,   398,   399,   408,   414,   506,    26,     0,
     0,     0,    36,    54,     0,   103,    76,     0,    85,    61,
   103,    85,     0,   176,     0,     0,   198,   201,   174,     0,
   212,   185,   188,    22,     0,   506,     0,   448,     0,   448,
   422,     0,   448,     0,   429,   417,   418,     0,     0,   401,
     0,   404,     0,     0,     0,   382,   386,   372,     0,    31,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   506,    33,   506,     0,   506,     0,    57,
    78,     0,     0,   102,    63,     0,   121,    84,   139,   121,
     0,   181,     0,   202,   506,   352,   200,     0,   189,     0,
     0,   211,   214,   187,    23,   353,   436,     0,   462,   440,
   462,   444,   462,   426,     0,     0,   428,   431,   390,     0,
   362,     0,   402,   403,     0,   410,   365,   373,   374,   375,
   376,   377,   378,   379,   380,   381,   383,   384,   385,   387,
   388,     0,   366,   367,   368,   369,   370,   371,   415,   416,
    32,    30,     0,    34,     0,    60,    55,     0,    59,    81,
   104,   506,    40,    66,    86,     0,     0,   120,     0,   253,
   138,   156,   178,   184,   177,     0,   183,     0,   207,   354,
     0,   194,     0,   215,   506,   213,   448,   446,     0,     0,
   448,     0,   448,     0,   429,   432,     0,   506,   430,     0,
   391,     0,   361,   411,     0,    39,     0,    56,     0,    93,
     0,    79,     0,    83,    92,   107,   506,    43,    69,    64,
     0,    68,    89,   122,   506,   140,     0,    71,   252,     0,
   303,   155,   181,   180,     0,   204,     0,   203,     0,   209,
   191,   197,   190,     0,   196,     0,   220,   355,   462,   450,
   460,   506,   462,   506,   462,   506,     0,   502,   504,     0,
     0,   425,     0,     0,   389,    38,    58,     0,    95,    80,
     0,     0,     0,   105,   106,    42,    65,     0,    87,     0,
    91,   125,    44,   143,     0,   256,     0,     0,    70,   157,
     0,    74,   302,   179,   182,   207,     0,   206,     0,   194,
   193,     0,   217,     0,   216,   219,   222,     0,     0,   447,
   449,   452,   464,   435,     0,   439,     0,   443,   506,   503,
   433,     0,   392,   360,    96,    94,    99,   100,   101,    82,
   115,     0,   113,   119,     0,   117,     0,     0,    67,    88,
     0,     0,   123,   124,     0,   141,   142,   145,   254,   259,
     0,   506,    46,   160,     0,   306,     0,     0,    73,   205,
   210,   208,   192,   195,   220,     0,   221,   506,     0,   451,
   471,   482,   483,     0,   473,   474,   475,   476,   481,     0,
   461,   463,   466,     0,   506,   506,   427,     0,    97,    98,
     0,   109,     0,     0,   111,     0,     0,     0,    90,   133,
     0,   131,   137,     0,   135,     0,     0,     0,   144,   259,
   264,   287,   257,   258,   261,   262,   263,     0,     0,   280,
     0,   506,    49,     0,   158,   159,   162,   304,   309,     0,
   506,   218,     0,   437,   455,   456,   454,   458,   459,   457,
     0,   472,     0,   477,   478,   479,   480,   465,   467,   441,
   445,     0,   393,   114,   112,   118,   116,   108,   110,     0,
   127,     0,     0,   129,     0,     0,     0,   506,   255,     0,
   288,     0,   286,   260,     0,     0,     0,    72,    48,     0,
   161,   309,   314,   337,   307,   308,   311,   312,   313,     0,
     0,   330,     0,    50,   223,   453,   469,   487,     0,   132,
   130,   136,   134,   126,   128,     0,   506,   146,     0,     0,
   267,     0,     0,   297,   270,     0,   290,     0,   270,     0,
   284,   279,   506,   305,     0,   338,   336,   310,     0,     0,
     0,    75,   487,     0,   489,   394,   506,     0,   148,   173,
     0,     0,     0,   357,   300,   298,   296,     0,   272,     0,
     0,     0,   272,     0,   285,   283,     0,   506,   163,     0,
     0,   317,   347,   320,     0,   340,     0,   320,     0,   334,
   329,   489,     0,     0,   486,     0,   491,   150,   506,   173,
   199,     0,     0,   274,   266,   269,     0,     0,   294,   289,
   297,   270,   301,     0,   270,   506,     0,   165,   186,     0,
     0,     0,   348,   346,   350,     0,   322,     0,     0,     0,
   322,     0,   335,   333,   491,   484,   485,     0,     0,     0,
   173,   152,   199,   147,   225,     0,   506,   273,   271,   508,
   507,   277,   295,   293,   272,   275,   272,   167,   506,   186,
   212,     0,     0,   324,   316,   319,     0,     0,   344,   339,
   347,   320,   351,     0,   320,     0,     0,   494,   496,   501,
   492,   493,   499,   500,     0,   498,   490,   468,   199,   173,
   149,   249,   268,   265,     0,     0,   186,   169,   212,   164,
   237,     0,     0,   323,   321,   327,   345,   343,   322,   325,
   322,   470,   488,   495,   497,   151,   199,     0,     0,   224,
   154,   278,   276,   212,   186,   166,   251,   318,   315,     0,
     0,   153,     0,   226,     0,   168,   212,     0,     0,   236,
   171,   328,   326,     0,   231,     0,   170,     0,   238,     0,
   228,   235,   227,     0,   233,   248,     0,   243,     0,   231,
   234,   230,     0,   240,   247,   239,     0,   245,   250,   229,
   232,   243,   246,   242,     0,   241,   244,     0,     0,     0
};

static const short yydefgoto[] = {   868,
     1,    12,    13,    38,    65,    66,    14,    15,    50,    51,
   109,    16,    17,   112,   113,    18,    19,   273,    20,    21,
    22,    23,   463,    24,    25,    54,    55,   190,   267,   268,
   269,   119,   120,   274,   330,   331,   332,   398,   399,   468,
   469,   116,   117,   270,   322,   323,   197,   198,   333,   389,
   390,   324,   325,   193,   194,   326,   384,   385,   442,   443,
   445,   446,   277,   278,   392,   453,   454,   511,   512,   514,
   515,   280,   281,   394,   456,   457,   458,   650,   690,   731,
   780,   734,   341,   342,   464,   535,   536,   537,   709,   750,
   787,   815,   790,    60,    61,    62,   202,   343,   285,   286,
   287,   131,   132,   133,   292,   410,   353,   354,   355,   735,
   127,   128,   289,   406,   348,   349,   350,   791,   212,   213,
   357,   475,   415,   416,   417,   782,   810,   835,   850,   843,
   844,   845,   817,   830,   848,   862,   856,   857,   858,   811,
   831,   338,   339,   520,   460,   523,   524,   525,   526,   580,
   620,   621,   659,   698,   693,   527,   528,   529,   632,   530,
   625,   626,   700,   627,   628,   402,   403,   592,   539,   595,
   596,   597,   598,   635,   671,   672,   717,   758,   753,   599,
   600,   601,   681,   602,   674,   675,   760,   676,   677,    26,
    27,   206,    28,    29,   657,    97,    98,    99,   100,   101,
   102,   103,   104,   105,   106,   107,    30,    31,    32,    33,
    34,   145,   305,   226,   227,   228,    35,   138,   297,    36,
   140,   301,    37,   143,   303,   219,   360,   420,   421,   422,
   551,   300,   423,   491,   492,   493,   608,   643,   494,   645,
   687,   730,   777,   371,    69,   742
};

static const short yypact[] = {-32768,
   307,-32768,    10,    40,   135,   147,   167,   190,     1,   205,
   216,-32768,-32768,-32768,   143,-32768,   163,-32768,   223,-32768,
   176,-32768,   223,-32768,   182,-32768,   224,-32768,   185,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   252,   195,   198,
   201,   229,   230,   -20,   -33,   284,   -24,   107,   339,   283,
   295,   300,   244,   344,-32768,   312,   344,   312,    12,   353,
   224,-32768,   325,-32768,   117,-32768,   333,-32768,-32768,   341,
-32768,   349,-32768,-32768,   292,   358,-32768,-32768,   366,-32768,
-32768,   384,   391,   294,-32768,-32768,-32768,   328,    31,   142,
   332,   338,   356,   356,   356,   356,   230,-32768,   742,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   275,   -14,-32768,   339,
   290,   350,   422,-32768,   352,   450,-32768,   383,   468,-32768,
   450,   468,   339,-32768,   119,   458,   353,-32768,-32768,   123,
   486,   325,-32768,-32768,   252,   230,   404,   464,   437,   464,
-32768,   441,   464,   445,   515,-32768,-32768,   241,   251,-32768,
   516,-32768,   517,   520,   453,-32768,-32768,-32768,   664,-32768,
   356,   356,   356,   356,   356,   356,   356,   356,   356,   356,
   356,   356,   356,   356,   356,   356,   356,   356,   356,   356,
   356,   522,   523,   230,-32768,   230,   339,   230,   461,   525,
-32768,   476,   536,-32768,-32768,   477,   556,-32768,   568,   556,
   496,   557,   339,-32768,   230,-32768,-32768,   339,-32768,   128,
   494,   486,-32768,-32768,-32768,   497,-32768,   508,   539,-32768,
   539,-32768,   539,-32768,   139,   500,   515,-32768,-32768,   392,
-32768,   546,-32768,-32768,   495,-32768,-32768,   456,   456,   456,
   456,   456,   456,   440,   440,   440,   213,   213,   213,   319,
   319,   498,   172,   172,   507,   507,   507,   507,-32768,-32768,
-32768,-32768,   506,-32768,   339,-32768,-32768,   239,-32768,   122,
-32768,    80,-32768,   567,-32768,   518,   509,-32768,   519,   591,
-32768,   597,-32768,-32768,-32768,   243,-32768,   524,   574,-32768,
   526,   588,   339,-32768,   230,-32768,   464,-32768,   544,   535,
   464,   542,   464,   543,   515,-32768,   136,   230,-32768,   356,
-32768,   356,-32768,-32768,   356,-32768,   548,-32768,   525,   559,
   608,-32768,   246,-32768,-32768,   336,   230,-32768,-32768,-32768,
   269,-32768,   122,-32768,   230,-32768,    14,   633,-32768,   561,
   636,-32768,   557,-32768,   557,-32768,   552,-32768,   271,-32768,
-32768,-32768,-32768,   276,-32768,   565,   617,-32768,   539,   618,
-32768,   230,   539,   230,   539,   230,   558,-32768,-32768,   621,
   572,-32768,   589,   690,   767,-32768,-32768,   184,-32768,-32768,
   122,   635,   638,-32768,   368,-32768,-32768,   567,-32768,   581,
-32768,   385,-32768,   647,   599,-32768,   600,   661,-32768,-32768,
    23,   675,-32768,-32768,-32768,   574,   339,-32768,   574,   588,
-32768,   588,-32768,   592,-32768,   617,-32768,   594,   595,-32768,
   618,-32768,   443,-32768,   598,-32768,   612,-32768,   230,-32768,
-32768,   356,-32768,-32768,   645,   651,-32768,-32768,-32768,-32768,
   606,   318,-32768,   613,   320,-32768,   635,   638,-32768,   122,
   686,   687,-32768,   403,   639,-32768,   647,-32768,-32768,    19,
   688,   116,-32768,   691,   641,-32768,   642,   644,-32768,-32768,
-32768,-32768,-32768,-32768,   617,   339,-32768,   230,   287,-32768,
   656,-32768,-32768,   390,-32768,-32768,-32768,-32768,-32768,   286,
-32768,   443,-32768,   695,   230,   230,-32768,   628,-32768,-32768,
   648,-32768,   635,   650,-32768,   638,   329,   334,-32768,   658,
   337,-32768,   672,   348,-32768,   686,   687,   739,-32768,    19,
-32768,   -30,-32768,    19,-32768,-32768,-32768,   428,   682,-32768,
   703,   230,-32768,   714,-32768,   691,-32768,-32768,    27,   740,
   230,-32768,   708,-32768,   384,   391,-32768,-32768,-32768,   275,
   715,-32768,   788,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   356,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   726,
-32768,   686,   728,-32768,   687,   361,   396,   -11,-32768,   809,
-32768,   339,   747,-32768,   126,   126,   816,-32768,-32768,   817,
-32768,    27,-32768,   118,-32768,    27,-32768,-32768,-32768,   475,
   752,-32768,   766,-32768,-32768,-32768,-32768,   762,   716,-32768,
-32768,-32768,-32768,-32768,-32768,   827,   -13,-32768,   765,   843,
   770,   768,   339,    -9,   844,   769,-32768,   146,   844,    35,
   100,-32768,    54,-32768,   832,-32768,   771,-32768,   834,   834,
   836,-32768,   762,   -16,   820,-32768,   230,   839,-32768,   224,
   339,   840,   809,-32768,-32768,-32768,   775,   339,   864,   845,
   846,   339,   864,   846,-32768,-32768,   847,     2,-32768,   780,
   858,   789,   155,   861,   784,-32768,    25,   861,    22,   101,
-32768,   820,   786,   787,-32768,   790,   791,-32768,   230,   224,
   353,   868,   157,-32768,-32768,-32768,   854,   402,   109,-32768,
   140,   844,-32768,   402,   844,   230,   856,-32768,   325,   339,
   859,   832,-32768,   792,-32768,   339,   882,   862,   863,   339,
   882,   863,-32768,-32768,   791,-32768,-32768,   865,   211,   802,
   224,-32768,   353,-32768,-32768,   339,   230,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   864,-32768,   864,-32768,   230,   325,
   486,   879,   169,-32768,-32768,-32768,   866,   807,   114,-32768,
   159,   861,-32768,   810,   861,   811,   800,   384,   391,-32768,
-32768,-32768,-32768,-32768,   465,   275,-32768,-32768,   353,   224,
-32768,    34,-32768,-32768,   402,   402,   325,-32768,   486,-32768,
-32768,   339,   812,-32768,-32768,-32768,-32768,-32768,   882,-32768,
   882,-32768,-32768,-32768,-32768,-32768,   353,   132,   821,-32768,
-32768,-32768,-32768,   486,   325,-32768,   225,-32768,-32768,   814,
   815,-32768,   339,-32768,   874,-32768,   486,   137,   825,-32768,
-32768,-32768,-32768,   826,   878,   822,-32768,   339,-32768,   881,
-32768,   819,-32768,   421,-32768,-32768,   828,   880,   829,   878,
-32768,-32768,   878,-32768,   819,-32768,   431,-32768,-32768,-32768,
-32768,   880,-32768,-32768,   880,-32768,-32768,   918,   919,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   785,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   898,-32768,-32768,-32768,-32768,
   604,   867,-32768,-32768,-32768,-32768,   538,-32768,-32768,-32768,
-32768,   870,-32768,-32768,-32768,-32768,   806,-32768,-32768,-32768,
-32768,   549,  -314,   803,-32768,-32768,-32768,-32768,   482,   429,
   483,   427,   734,-32768,-32768,-32768,-32768,   419,   364,   420,
   363,-32768,-32768,-32768,-32768,-32768,   484,-32768,-32768,-32768,
-32768,  -721,-32768,-32768,-32768,-32768,-32768,   406,-32768,-32768,
-32768,-32768,  -763,  -635,-32768,   883,-32768,-32768,   596,-32768,
   601,  -677,-32768,   808,-32768,-32768,   533,-32768,   537,   885,
-32768,   823,-32768,-32768,   541,-32768,   545,   824,-32768,   736,
-32768,-32768,   478,-32768,   540,-32768,-32768,-32768,-32768,   102,
-32768,    98,-32768,-32768,-32768,-32768,    95,-32768,    93,-32768,
-32768,-32768,-32768,-32768,-32768,   439,-32768,   436,-32768,-32768,
   308,-32768,  -551,  -640,-32768,  -612,-32768,-32768,-32768,-32768,
  -576,-32768,-32768,-32768,   376,-32768,-32768,-32768,-32768,   371,
-32768,   369,-32768,-32768,   254,-32768,  -661,  -658,-32768,  -686,
-32768,-32768,-32768,-32768,  -613,-32768,-32768,-32768,   324,-32768,
-32768,-32768,-32768,-32768,  -494,  -110,-32768,   -46,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,  -478,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   662,-32768,   741,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,  -138,-32768,-32768,-32768,   550,
-32768,  -217,-32768,-32768,-32768,   480,-32768,-32768,   485,   327,
   291,   249,-32768,-32768,   -94,  -673
};


#define	YYLAST		974


static const short yytable[] = {   186,
   550,   221,   160,   302,   223,   304,  -299,  -299,   648,   629,
   616,   781,   201,   185,   691,   683,   721,   684,   391,   184,
    72,  -299,   704,   707,   754,   816,   678,   583,   395,    76,
   746,   751,   123,    70,    45,   581,   521,   465,   722,   694,
   719,   216,   808,    39,   593,   809,   156,   157,   158,   159,
   826,   664,   522,    46,   733,   720,   656,   806,   720,    73,
   594,    74,   764,   837,   150,   662,   794,   582,    77,  -282,
    78,   151,   789,    40,    71,   667,   263,   663,    68,    68,
   738,    68,   685,   617,   702,   822,   582,   705,   582,   261,
  -292,   262,   288,   264,    68,   779,   124,   291,   396,   637,
   799,   230,   232,   801,   785,   762,   786,   466,   765,   814,
   290,   812,   813,   327,   238,   239,   240,   241,   242,   243,
   244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
   254,   255,   256,   257,   258,   509,   666,   827,   668,   203,
   820,   418,   821,   208,   807,   425,    68,   427,   293,   532,
   745,   582,   823,   747,   317,   320,   623,   838,   359,   624,
    79,   661,   363,   321,   365,   665,   723,   368,    41,   369,
  -349,  -349,    68,    49,   743,   152,   662,   328,   714,   797,
    42,   153,   356,   636,   737,   724,    82,    83,    84,    85,
   522,    86,    87,    88,    89,    90,   793,   582,   582,    80,
   358,    81,   594,   204,   744,   656,   582,   209,    68,   134,
   135,   582,   294,   372,    43,   582,   824,  -332,    91,    92,
   713,   839,   370,    44,   713,   435,   436,   437,   438,   439,
    93,   306,   386,   828,    94,   307,   829,   582,    47,  -292,
   393,    95,   768,   769,   770,   771,   772,   773,   774,    48,
   776,    53,   582,    59,  -342,    96,   582,    52,  -342,   178,
   179,   180,   181,   373,   798,   374,   714,   424,   375,   426,
    56,   428,    82,    83,    84,    85,    58,    86,    87,    63,
    89,    90,    82,    83,    84,    85,    64,    86,    87,   -45,
    89,    90,   -35,   173,   174,   -51,   471,   775,   176,   177,
   178,   179,   180,   181,    91,    92,   182,   183,   108,     2,
     3,     4,     5,    67,    91,    92,    93,    75,   545,   546,
    94,   547,    68,   548,   549,   110,    93,    95,   114,     6,
    94,   318,   319,   111,   497,   344,   345,    95,   380,   381,
   118,    96,   229,   554,   555,   556,   557,   851,   382,   383,
   115,    96,   231,     7,   130,     8,     9,    10,    11,   125,
   863,   387,   388,   408,   409,   543,   136,   533,   411,   412,
    82,    83,    84,    85,   137,    86,    87,    88,    89,    90,
   447,   448,   139,   544,   141,   498,   187,    82,    83,    84,
    85,   142,    86,    87,   148,    89,    90,   451,   452,   144,
   560,   561,    91,    92,   176,   177,   178,   179,   180,   181,
   502,   503,   505,   506,    93,   516,   517,   146,    94,    91,
    92,   568,   503,   481,   147,    95,   569,   506,   149,   571,
   572,    93,   154,   482,   483,    94,   191,   589,   155,    96,
   574,   575,    95,   585,   586,   188,   604,   485,   486,   487,
   488,   489,   490,   614,   572,   189,    96,   192,   161,   162,
   163,   164,   165,   166,   167,   168,   169,   195,   170,   171,
   172,   622,   173,   174,   196,   175,   481,   176,   177,   178,
   179,   180,   181,   618,   205,   310,   482,   483,   615,   575,
   639,   640,   210,   311,   740,   741,   804,   805,   217,   484,
   485,   486,   487,   488,   489,   490,   161,   162,   163,   164,
   165,   166,   655,   852,   853,   609,   170,   171,   172,   218,
   173,   174,   649,   864,   865,   176,   177,   178,   179,   180,
   181,   220,   170,   171,   172,   222,   173,   174,   669,   224,
   692,   176,   177,   178,   179,   180,   181,   696,   225,   233,
   234,   703,   688,   235,   236,   259,   260,   265,   266,   272,
   271,   275,   715,   276,   161,   162,   163,   164,   165,   166,
   167,   168,   169,   708,   170,   171,   172,   279,   173,   174,
   283,   175,   315,   176,   177,   178,   179,   180,   181,   295,
   284,  -356,   298,   299,   732,   308,   314,   181,   316,   752,
   329,   337,   334,   336,   335,   756,   340,   347,   346,   763,
   351,   748,   161,   162,   163,   164,   165,   166,   167,   168,
   169,   352,   170,   171,   172,   783,   173,   174,   361,   175,
   362,   176,   177,   178,   179,   180,   181,   364,   366,   312,
   376,   379,   784,   378,   397,   400,   401,   313,   407,   413,
   414,   419,   430,   429,   788,   161,   162,   163,   164,   165,
   166,   167,   168,   169,   431,   170,   171,   172,   441,   173,
   174,   444,   175,   450,   176,   177,   178,   179,   180,   181,
   455,   818,   432,   459,   461,   462,   467,   499,   476,   478,
   433,   479,   500,   495,   161,   162,   163,   164,   165,   166,
   167,   168,   169,   501,   170,   171,   172,   496,   173,   174,
   504,   175,   834,   176,   177,   178,   179,   180,   181,   510,
   513,   562,   531,   518,   534,   538,   540,   847,   559,   563,
   161,   162,   163,   164,   165,   166,   167,   168,   169,   541,
   170,   171,   172,   552,   173,   174,   564,   175,   566,   176,
   177,   178,   179,   180,   181,   570,   161,   162,   163,   164,
   165,   166,   167,   168,   169,   237,   170,   171,   172,   573,
   173,   174,   578,   175,   603,   176,   177,   178,   179,   180,
   181,   587,   161,   162,   163,   164,   165,   166,   167,   168,
   169,   434,   170,   171,   172,   588,   173,   174,   590,   175,
   605,   176,   177,   178,   179,   180,   181,   606,   161,   162,
   163,   164,   165,   166,   167,   168,   169,   646,   170,   171,
   172,   607,   173,   174,   610,   175,   612,   176,   177,   178,
   179,   180,   181,   161,   162,   163,   164,   165,   166,   167,
   168,   169,   619,   170,   171,   172,  -281,   173,   174,   631,
   633,   641,   176,   177,   178,   179,   180,   181,   642,   644,
   647,   651,   652,   653,   658,   670,   654,   673,   660,   680,
  -331,   686,   689,   522,  -291,   697,   710,   711,   699,   701,
   706,   716,   712,   718,   726,   727,   736,   729,   739,   749,
   728,  -341,   594,   757,   778,   759,   761,   792,   767,   796,
   795,   803,   800,   802,   819,   825,   832,   833,   836,   840,
   841,   842,   854,   855,   846,   849,   582,   869,   870,   215,
    57,   859,   377,   199,   122,   449,   121,   200,   507,   440,
   508,   565,   567,   282,   576,   611,   577,   613,   404,   214,
   519,   591,   473,   129,   126,   405,   470,   296,   474,   207,
   861,   860,   542,   472,   211,   477,   866,   867,   579,   584,
   695,   630,   634,   679,   638,   755,   367,   309,   553,   682,
   480,   558,   725,   766
};

static const short yycheck[] = {   110,
   479,   140,    97,   221,   143,   223,    16,    17,    22,   586,
    22,   733,   123,   108,   650,    32,   678,    34,   333,    34,
    54,    31,   663,    22,   711,   789,   640,   522,    15,    54,
   704,   709,    21,    54,    34,    66,    18,    15,    17,   652,
    16,   136,     9,    34,    18,    12,    93,    94,    95,    96,
   814,    17,    34,    53,   690,    34,    66,   779,    34,    93,
    34,    95,   721,   827,    34,    31,   753,    98,    93,   100,
    95,    41,   750,    34,    95,    22,   187,   629,    93,    93,
   693,    93,    99,   578,   661,   807,    98,   664,    98,   184,
   100,   186,   203,   188,    93,   731,    85,   208,    85,   594,
   762,   148,   149,   765,   745,   719,   747,    85,   722,   787,
   205,   785,   786,    34,   161,   162,   163,   164,   165,   166,
   167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
   177,   178,   179,   180,   181,   450,   631,   815,   633,    21,
   799,   359,   801,    21,   780,   363,    93,   365,    21,    34,
   702,    98,    21,   705,   265,    34,    31,    21,   297,    34,
    54,    16,   301,    42,   303,    66,    66,    32,    34,    34,
    16,    17,    93,    31,    66,    34,    31,   272,   673,    66,
    34,    40,   293,    66,    28,   680,    32,    33,    34,    35,
    34,    37,    38,    39,    40,    41,    28,    98,    98,    93,
   295,    95,    34,    85,   699,    66,    98,    85,    93,    93,
    94,    98,    85,   308,    48,    98,    85,   100,    64,    65,
    66,    85,    87,    34,    66,    42,    43,    44,    45,    46,
    76,    93,   327,     9,    80,    97,    12,    98,    34,   100,
   335,    87,    32,    33,    34,    35,    36,    37,    38,    34,
   729,    29,    98,    30,   100,   101,    98,    95,   100,    88,
    89,    90,    91,   310,   759,   312,   761,   362,   315,   364,
    95,   366,    32,    33,    34,    35,    95,    37,    38,    95,
    40,    41,    32,    33,    34,    35,    35,    37,    38,    95,
    40,    41,    95,    81,    82,    95,   407,    87,    86,    87,
    88,    89,    90,    91,    64,    65,    32,    33,    26,     3,
     4,     5,     6,    85,    64,    65,    76,    34,    32,    33,
    80,    35,    93,    37,    38,    31,    76,    87,    85,    23,
    80,    93,    94,    34,   429,    93,    94,    87,    93,    94,
    29,   101,   102,    58,    59,    60,    61,   842,    13,    14,
     7,   101,   102,    47,    30,    49,    50,    51,    52,     7,
   855,    93,    94,    93,    94,   476,    34,   462,    93,    94,
    32,    33,    34,    35,    34,    37,    38,    39,    40,    41,
    13,    14,    34,   478,    93,   432,    97,    32,    33,    34,
    35,    34,    37,    38,   101,    40,    41,    13,    14,    34,
   495,   496,    64,    65,    86,    87,    88,    89,    90,    91,
    93,    94,    93,    94,    76,    13,    14,    34,    80,    64,
    65,    93,    94,    34,    34,    87,    93,    94,   101,    93,
    94,    76,   101,    44,    45,    80,    85,   532,   101,   101,
    93,    94,    87,    16,    17,    96,   541,    58,    59,    60,
    61,    62,    63,    93,    94,    34,   101,     8,    67,    68,
    69,    70,    71,    72,    73,    74,    75,    85,    77,    78,
    79,   582,    81,    82,     7,    84,    34,    86,    87,    88,
    89,    90,    91,   578,    27,    94,    44,    45,    93,    94,
    16,    17,     7,   102,    93,    94,    32,    33,    95,    57,
    58,    59,    60,    61,    62,    63,    67,    68,    69,    70,
    71,    72,   623,    93,    94,   562,    77,    78,    79,    56,
    81,    82,   617,    93,    94,    86,    87,    88,    89,    90,
    91,    95,    77,    78,    79,    95,    81,    82,   633,    95,
   651,    86,    87,    88,    89,    90,    91,   658,    34,    34,
    34,   662,   647,    34,   102,    34,    34,    97,    34,    24,
    85,    85,   673,     8,    67,    68,    69,    70,    71,    72,
    73,    74,    75,   668,    77,    78,    79,    10,    81,    82,
    85,    84,    85,    86,    87,    88,    89,    90,    91,    96,
    34,    95,    85,    55,   689,    96,   102,    91,    93,   710,
    34,    11,    85,    85,    96,   716,    10,    34,    85,   720,
    85,   706,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    34,    77,    78,    79,   736,    81,    82,    85,    84,
    96,    86,    87,    88,    89,    90,    91,    96,    96,    94,
    93,    34,   737,    85,    12,    85,    11,   102,    97,    85,
    34,    34,    32,    96,   749,    67,    68,    69,    70,    71,
    72,    73,    74,    75,    93,    77,    78,    79,    34,    81,
    82,    34,    84,    93,    86,    87,    88,    89,    90,    91,
    34,   792,    94,    85,    85,    25,    12,    43,    97,    96,
   102,    97,    42,    96,    67,    68,    69,    70,    71,    72,
    73,    74,    75,    98,    77,    78,    79,    96,    81,    82,
    98,    84,   823,    86,    87,    88,    89,    90,    91,    34,
    34,    94,    35,    85,    34,    85,    85,   838,    34,   102,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    96,
    77,    78,    79,    88,    81,    82,    99,    84,    99,    86,
    87,    88,    89,    90,    91,    98,    67,    68,    69,    70,
    71,    72,    73,    74,    75,   102,    77,    78,    79,    98,
    81,    82,    34,    84,    35,    86,    87,    88,    89,    90,
    91,   100,    67,    68,    69,    70,    71,    72,    73,    74,
    75,   102,    77,    78,    79,    93,    81,    82,    85,    84,
    93,    86,    87,    88,    89,    90,    91,    93,    67,    68,
    69,    70,    71,    72,    73,    74,    75,   102,    77,    78,
    79,    34,    81,    82,    99,    84,    99,    86,    87,    88,
    89,    90,    91,    67,    68,    69,    70,    71,    72,    73,
    74,    75,    34,    77,    78,    79,   100,    81,    82,    34,
    34,   100,    86,    87,    88,    89,    90,    91,    93,    98,
    34,    97,    20,    94,    21,    34,    99,    34,   100,    34,
   100,    52,    34,    34,   100,    12,    97,    20,    34,    34,
    34,    21,    94,   100,    99,    99,    19,    97,    35,    34,
   101,   100,    34,    12,    93,    34,    34,    19,    34,    93,
    35,   102,    93,    93,    93,    85,    93,    93,    35,    85,
    85,    34,    85,    34,    93,    35,    98,     0,     0,   135,
    23,    93,   319,   121,    58,   388,    57,   122,   447,   381,
   448,   503,   506,   200,   516,   572,   517,   575,   343,   132,
   457,   536,   410,    61,    60,   345,   406,   212,   412,   127,
   853,   850,   475,   409,   131,   416,   862,   865,   520,   524,
   653,   586,   592,   640,   596,   712,   305,   227,   484,   643,
   421,   492,   682,   725
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/local/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 5:
#line 249 "ned.y"
{ if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); ;
    break;}
case 6:
#line 251 "ned.y"
{ if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 7:
#line 253 "ned.y"
{ if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 8:
#line 255 "ned.y"
{ if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); ;
    break;}
case 9:
#line 258 "ned.y"
{ if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); ;
    break;}
case 10:
#line 260 "ned.y"
{ if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 11:
#line 262 "ned.y"
{ if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 12:
#line 264 "ned.y"
{ if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); ;
    break;}
case 17:
#line 272 "ned.y"
{ if (ps.storeSourceCode) ps.enump->setSourceCode(toString(yylsp[0])); ;
    break;}
case 18:
#line 274 "ned.y"
{ if (ps.storeSourceCode) ps.messagep->setSourceCode(toString(yylsp[0])); ;
    break;}
case 19:
#line 276 "ned.y"
{ if (ps.storeSourceCode) ps.classp->setSourceCode(toString(yylsp[0])); ;
    break;}
case 20:
#line 278 "ned.y"
{ if (ps.storeSourceCode) ps.structp->setSourceCode(toString(yylsp[0])); ;
    break;}
case 21:
#line 286 "ned.y"
{
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                ;
    break;}
case 22:
#line 291 "ned.y"
{
                  /* no setTrailingComment(ps.imports,@3); comment already stored with last filename */
                ;
    break;}
case 25:
#line 303 "ned.y"
{
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                ;
    break;}
case 27:
#line 319 "ned.y"
{
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 30:
#line 333 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 31:
#line 339 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 32:
#line 348 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 33:
#line 352 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 34:
#line 364 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 35:
#line 371 "ned.y"
{
                  use_chanattrname_token = 0;
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 38:
#line 386 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 39:
#line 392 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 41:
#line 412 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 42:
#line 422 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 43:
#line 426 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 44:
#line 440 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 45:
#line 447 "ned.y"
{
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 47:
#line 471 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 48:
#line 481 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 49:
#line 485 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 50:
#line 502 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 51:
#line 509 "ned.y"
{
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 54:
#line 527 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 55:
#line 532 "ned.y"
{
                ;
    break;}
case 60:
#line 548 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 63:
#line 565 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 64:
#line 570 "ned.y"
{
                ;
    break;}
case 69:
#line 586 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 72:
#line 603 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 75:
#line 618 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 78:
#line 633 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 79:
#line 638 "ned.y"
{
                ;
    break;}
case 82:
#line 649 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 83:
#line 653 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 86:
#line 668 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 87:
#line 673 "ned.y"
{
                ;
    break;}
case 90:
#line 684 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 91:
#line 688 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 93:
#line 703 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                ;
    break;}
case 94:
#line 707 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                ;
    break;}
case 95:
#line 711 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                ;
    break;}
case 96:
#line 715 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                ;
    break;}
case 97:
#line 719 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 98:
#line 723 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 99:
#line 727 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                ;
    break;}
case 100:
#line 731 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                ;
    break;}
case 101:
#line 735 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                ;
    break;}
case 104:
#line 750 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 105:
#line 755 "ned.y"
{
                ;
    break;}
case 114:
#line 778 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 115:
#line 783 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 118:
#line 796 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 119:
#line 801 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 122:
#line 817 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 123:
#line 822 "ned.y"
{
                ;
    break;}
case 132:
#line 845 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 133:
#line 850 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 136:
#line 863 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 137:
#line 868 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 140:
#line 884 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 141:
#line 889 "ned.y"
{
                ;
    break;}
case 146:
#line 905 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 147:
#line 911 "ned.y"
{
                ;
    break;}
case 148:
#line 914 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 149:
#line 921 "ned.y"
{
                ;
    break;}
case 150:
#line 924 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 151:
#line 930 "ned.y"
{
                ;
    break;}
case 152:
#line 933 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 153:
#line 940 "ned.y"
{
                ;
    break;}
case 157:
#line 960 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 158:
#line 965 "ned.y"
{
                ;
    break;}
case 163:
#line 981 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 164:
#line 987 "ned.y"
{
                ;
    break;}
case 165:
#line 990 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 166:
#line 997 "ned.y"
{
                ;
    break;}
case 167:
#line 1000 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 168:
#line 1006 "ned.y"
{
                ;
    break;}
case 169:
#line 1009 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 170:
#line 1016 "ned.y"
{
                ;
    break;}
case 176:
#line 1041 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 177:
#line 1046 "ned.y"
{
                ;
    break;}
case 178:
#line 1049 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 179:
#line 1055 "ned.y"
{
                ;
    break;}
case 184:
#line 1071 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 189:
#line 1092 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 190:
#line 1097 "ned.y"
{
                ;
    break;}
case 191:
#line 1100 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 192:
#line 1106 "ned.y"
{
                ;
    break;}
case 197:
#line 1122 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 202:
#line 1143 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 203:
#line 1148 "ned.y"
{
                ;
    break;}
case 204:
#line 1151 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 205:
#line 1157 "ned.y"
{
                ;
    break;}
case 210:
#line 1174 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                ;
    break;}
case 215:
#line 1196 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 216:
#line 1201 "ned.y"
{
                ;
    break;}
case 217:
#line 1204 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 218:
#line 1210 "ned.y"
{
                ;
    break;}
case 223:
#line 1227 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-3]);
                  addExpression(ps.substparam, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparam,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 226:
#line 1244 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 227:
#line 1249 "ned.y"
{
                ;
    break;}
case 228:
#line 1252 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 229:
#line 1258 "ned.y"
{
                ;
    break;}
case 234:
#line 1274 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 235:
#line 1281 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 238:
#line 1297 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 239:
#line 1302 "ned.y"
{
                ;
    break;}
case 240:
#line 1305 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 241:
#line 1311 "ned.y"
{
                ;
    break;}
case 246:
#line 1327 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 247:
#line 1334 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 248:
#line 1345 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 250:
#line 1356 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 254:
#line 1372 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 255:
#line 1378 "ned.y"
{
                ;
    break;}
case 256:
#line 1381 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 257:
#line 1387 "ned.y"
{
                ;
    break;}
case 264:
#line 1408 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;
    break;}
case 265:
#line 1413 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 268:
#line 1427 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 269:
#line 1437 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 271:
#line 1445 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 275:
#line 1458 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 276:
#line 1463 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 277:
#line 1468 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 278:
#line 1474 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 281:
#line 1488 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 282:
#line 1494 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 283:
#line 1502 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 284:
#line 1507 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 285:
#line 1511 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 286:
#line 1519 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 287:
#line 1526 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 288:
#line 1532 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 291:
#line 1547 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 292:
#line 1552 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 293:
#line 1559 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 294:
#line 1564 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 295:
#line 1568 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 296:
#line 1576 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 297:
#line 1581 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 298:
#line 1585 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 299:
#line 1594 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 300:
#line 1599 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 301:
#line 1604 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 304:
#line 1620 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 305:
#line 1626 "ned.y"
{
                ;
    break;}
case 306:
#line 1629 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 307:
#line 1635 "ned.y"
{
                ;
    break;}
case 314:
#line 1656 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;
    break;}
case 315:
#line 1661 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 318:
#line 1675 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 319:
#line 1685 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 321:
#line 1693 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 325:
#line 1706 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 326:
#line 1711 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 327:
#line 1716 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 328:
#line 1722 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 331:
#line 1736 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 332:
#line 1742 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 333:
#line 1750 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 334:
#line 1755 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 335:
#line 1759 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 336:
#line 1767 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 337:
#line 1774 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 338:
#line 1780 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 341:
#line 1795 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 342:
#line 1800 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 343:
#line 1807 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 344:
#line 1812 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 345:
#line 1816 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 346:
#line 1824 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 347:
#line 1829 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 348:
#line 1833 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 349:
#line 1842 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 350:
#line 1847 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 351:
#line 1852 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 353:
#line 1870 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 354:
#line 1879 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 355:
#line 1893 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 356:
#line 1901 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 357:
#line 1914 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 358:
#line 1920 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 359:
#line 1924 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 360:
#line 1935 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 361:
#line 1937 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 362:
#line 1939 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 363:
#line 1941 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 365:
#line 1947 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 366:
#line 1950 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 367:
#line 1952 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 368:
#line 1954 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 369:
#line 1956 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 370:
#line 1958 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 371:
#line 1960 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 372:
#line 1964 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); ;
    break;}
case 373:
#line 1967 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 374:
#line 1969 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 375:
#line 1971 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 376:
#line 1973 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 377:
#line 1975 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 378:
#line 1977 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 379:
#line 1980 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 380:
#line 1982 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 381:
#line 1984 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 382:
#line 1988 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 383:
#line 1991 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 384:
#line 1993 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 385:
#line 1995 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 386:
#line 1999 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 387:
#line 2001 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 388:
#line 2003 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 389:
#line 2005 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 390:
#line 2008 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 391:
#line 2010 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 392:
#line 2012 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 393:
#line 2014 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 394:
#line 2016 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-9]), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 400:
#line 2029 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 401:
#line 2034 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                ;
    break;}
case 402:
#line 2039 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 403:
#line 2045 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 404:
#line 2051 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 405:
#line 2059 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 406:
#line 2064 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 407:
#line 2066 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 409:
#line 2075 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 410:
#line 2077 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 411:
#line 2079 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 412:
#line 2084 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 413:
#line 2086 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 414:
#line 2088 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 419:
#line 2105 "ned.y"
{
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(yylsp[-1])));
                  setComments(ps.cplusplus,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 420:
#line 2114 "ned.y"
{
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(yylsp[-1]));
                  setComments(ps.structdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 421:
#line 2123 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(yylsp[-1]));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 422:
#line 2130 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(yylsp[-1]));
                  setComments(ps.classdecl,yylsp[-3],yylsp[-2]);
                ;
    break;}
case 423:
#line 2140 "ned.y"
{
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString(yylsp[-1]));
                  setComments(ps.enumdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 424:
#line 2149 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 425:
#line 2156 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 426:
#line 2160 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 427:
#line 2168 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 432:
#line 2185 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-1],yylsp[-1]);
                ;
    break;}
case 433:
#line 2191 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 434:
#line 2201 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 435:
#line 2207 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 436:
#line 2211 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 437:
#line 2218 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 438:
#line 2225 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 439:
#line 2231 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 440:
#line 2235 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 441:
#line 2242 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 442:
#line 2249 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 443:
#line 2255 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 444:
#line 2259 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 445:
#line 2266 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 446:
#line 2273 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 453:
#line 2293 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 460:
#line 2312 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 467:
#line 2332 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                ;
    break;}
case 468:
#line 2338 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 469:
#line 2342 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(true);
                ;
    break;}
case 470:
#line 2349 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 484:
#line 2376 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 485:
#line 2381 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 486:
#line 2386 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 488:
#line 2394 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 490:
#line 2402 "ned.y"
{
                  ps.field->setDefaultValue(toString(yylsp[0]));
                ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/local/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 2431 "ned.y"


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

