
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
#define	XMLDOC	294
#define	REF	295
#define	ANCESTOR	296
#define	CONSTDECL	297
#define	NUMERICTYPE	298
#define	STRINGTYPE	299
#define	BOOLTYPE	300
#define	XMLTYPE	301
#define	ANYTYPE	302
#define	CPLUSPLUS	303
#define	CPLUSPLUSBODY	304
#define	MESSAGE	305
#define	CLASS	306
#define	STRUCT	307
#define	ENUM	308
#define	NONCOBJECT	309
#define	EXTENDS	310
#define	FIELDS	311
#define	PROPERTIES	312
#define	ABSTRACT	313
#define	CHARTYPE	314
#define	SHORTTYPE	315
#define	INTTYPE	316
#define	LONGTYPE	317
#define	DOUBLETYPE	318
#define	UNSIGNED_	319
#define	SIZEOF	320
#define	SUBMODINDEX	321
#define	PLUSPLUS	322
#define	EQ	323
#define	NE	324
#define	GT	325
#define	GE	326
#define	LS	327
#define	LE	328
#define	AND	329
#define	OR	330
#define	XOR	331
#define	NOT	332
#define	BIN_AND	333
#define	BIN_OR	334
#define	BIN_XOR	335
#define	BIN_COMPL	336
#define	SHIFT_LEFT	337
#define	SHIFT_RIGHT	338
#define	INVALID_CHAR	339
#define	UMIN	340

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



#define	YYFINAL		880
#define	YYFLAG		-32768
#define	YYNTBASE	105

#define YYTRANSLATE(x) ((unsigned)(x) <= 340 ? yytranslate[x] : 354)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    92,     2,     2,   103,
   104,    90,    88,    96,    89,   102,    91,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    87,    95,     2,
    99,     2,    86,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
   100,     2,   101,    93,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    97,     2,    98,     2,     2,     2,     2,     2,
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
    77,    78,    79,    80,    81,    82,    83,    84,    85,    94
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
   292,   296,   300,   302,   303,   304,   309,   311,   312,   317,
   321,   326,   330,   334,   336,   340,   342,   346,   348,   352,
   354,   356,   357,   358,   363,   365,   366,   371,   375,   380,
   384,   388,   390,   394,   396,   400,   402,   406,   408,   410,
   411,   412,   417,   419,   420,   423,   425,   426,   434,   435,
   444,   445,   455,   456,   467,   471,   473,   474,   475,   480,
   482,   483,   486,   488,   489,   497,   498,   507,   508,   518,
   519,   530,   534,   536,   537,   540,   542,   543,   548,   549,
   556,   559,   560,   564,   566,   568,   570,   571,   574,   576,
   577,   582,   583,   590,   593,   594,   598,   600,   602,   604,
   605,   608,   610,   611,   616,   617,   624,   627,   628,   632,
   634,   638,   640,   641,   644,   646,   647,   652,   653,   660,
   662,   663,   666,   668,   673,   676,   677,   678,   683,   684,
   691,   694,   695,   699,   701,   704,   706,   709,   710,   711,
   716,   717,   724,   727,   728,   732,   734,   737,   739,   744,
   745,   750,   751,   753,   754,   755,   761,   762,   767,   769,
   770,   773,   775,   777,   779,   780,   788,   792,   794,   800,
   803,   804,   807,   808,   811,   813,   820,   829,   836,   845,
   849,   851,   854,   856,   859,   861,   864,   867,   869,   872,
   876,   878,   881,   883,   886,   888,   891,   894,   896,   899,
   901,   904,   908,   910,   911,   912,   918,   919,   924,   926,
   927,   930,   932,   934,   936,   937,   945,   949,   951,   957,
   960,   961,   964,   965,   968,   970,   977,   986,   993,  1002,
  1006,  1008,  1011,  1013,  1016,  1018,  1021,  1024,  1026,  1029,
  1033,  1035,  1038,  1040,  1043,  1045,  1048,  1051,  1053,  1056,
  1058,  1061,  1065,  1070,  1076,  1079,  1086,  1092,  1096,  1098,
  1100,  1102,  1109,  1114,  1118,  1120,  1127,  1132,  1134,  1138,
  1142,  1146,  1150,  1154,  1158,  1162,  1165,  1169,  1173,  1177,
  1181,  1185,  1189,  1193,  1197,  1201,  1204,  1208,  1212,  1216,
  1219,  1223,  1227,  1233,  1237,  1242,  1249,  1258,  1269,  1271,
  1273,  1275,  1277,  1279,  1281,  1284,  1288,  1292,  1295,  1297,
  1299,  1301,  1303,  1305,  1309,  1314,  1316,  1318,  1320,  1322,
  1326,  1330,  1333,  1336,  1340,  1344,  1348,  1353,  1357,  1358,
  1366,  1367,  1377,  1379,  1380,  1383,  1385,  1388,  1393,  1394,
  1403,  1404,  1415,  1416,  1425,  1426,  1437,  1438,  1447,  1448,
  1459,  1460,  1465,  1466,  1468,  1469,  1472,  1474,  1479,  1481,
  1483,  1485,  1487,  1489,  1491,  1492,  1497,  1498,  1500,  1501,
  1504,  1506,  1507,  1515,  1516,  1525,  1527,  1530,  1532,  1534,
  1536,  1538,  1541,  1544,  1547,  1550,  1552,  1554,  1556,  1560,
  1564,  1567,  1568,  1573,  1574,  1577,  1578,  1580,  1582,  1584,
  1587,  1589,  1592,  1594,  1596,  1598,  1600,  1602,  1605,  1607,
  1609,  1610,  1612
};

static const short yyrhs[] = {   106,
     0,   106,   107,     0,     0,   108,     0,   117,     0,   124,
     0,   129,     0,   298,     0,   112,     0,   121,     0,   126,
     0,   295,     0,   314,     0,   315,     0,   316,     0,   317,
     0,   318,     0,   324,     0,   327,     0,   330,     0,     0,
     3,   109,   110,    95,     0,   110,    96,   111,     0,   111,
     0,    35,     0,   113,   114,   116,     0,     5,    34,     0,
     0,   115,     0,   115,    31,   301,   352,     0,    31,   301,
   352,     0,    26,    34,   352,     0,    26,   352,     0,   118,
    97,   119,    98,   352,     0,     5,    34,     0,   120,     0,
     0,   120,    34,    99,   301,    95,     0,    34,    99,   301,
    95,     0,   122,   131,   147,   159,   123,     0,     4,    34,
     0,    24,    34,   352,     0,    24,   352,     0,   125,    97,
   137,   152,   168,    98,   352,     0,     4,    34,     0,   127,
   131,   147,   159,   177,   247,   143,   128,     0,     6,    34,
     0,    25,    34,   352,     0,    25,   352,     0,   130,    97,
   137,   152,   168,   188,   271,   145,    98,   352,     0,     6,
    34,     0,   132,     0,     0,     0,    29,    87,   133,   134,
     0,   135,    95,     0,     0,   135,    96,   136,     0,   136,
     0,    34,     0,   138,     0,     0,     0,    29,    87,   139,
   140,     0,   141,    95,     0,     0,   141,    96,   142,     0,
   142,     0,    34,     0,   144,     0,     0,    12,    87,    35,
    95,     0,   146,     0,     0,    12,    87,    35,    95,     0,
   148,     0,     0,     0,     7,    87,   149,   150,     0,   151,
    95,     0,     0,   151,    96,   157,     0,   157,     0,   153,
     0,     0,     0,     7,    87,   154,   155,     0,   156,    95,
     0,     0,   156,    95,   158,     0,   158,     0,   158,     0,
    34,     0,    34,    87,    44,     0,    43,    34,     0,    34,
    87,    43,     0,    34,    87,    43,    44,     0,    34,    87,
    44,    43,     0,    34,    87,    45,     0,    34,    87,    46,
     0,    34,    87,    47,     0,    34,    87,    48,     0,   160,
     0,     0,     0,     8,    87,   161,   162,     0,   163,     0,
     0,   163,    13,   164,    95,     0,    13,   164,    95,     0,
   163,    14,   166,    95,     0,    14,   166,    95,     0,   164,
    96,   165,     0,   165,     0,    34,   100,   101,     0,    34,
     0,   166,    96,   167,     0,   167,     0,    34,   100,   101,
     0,    34,     0,   169,     0,     0,     0,     8,    87,   170,
   171,     0,   172,     0,     0,   172,    13,   173,    95,     0,
    13,   173,    95,     0,   172,    14,   175,    95,     0,    14,
   175,    95,     0,   173,    96,   174,     0,   174,     0,    34,
   100,   101,     0,    34,     0,   175,    96,   176,     0,   176,
     0,    34,   100,   101,     0,    34,     0,   178,     0,     0,
     0,    10,    87,   179,   180,     0,   181,     0,     0,   181,
   182,     0,   182,     0,     0,    34,    87,    34,   352,   183,
   199,   187,     0,     0,    34,    87,    34,   300,   352,   184,
   199,   187,     0,     0,    34,    87,    34,    22,    34,   352,
   185,   199,   187,     0,     0,    34,    87,    34,   300,    22,
    34,   352,   186,   199,   187,     0,   215,   231,   245,     0,
   189,     0,     0,     0,    10,    87,   190,   191,     0,   192,
     0,     0,   192,   193,     0,   193,     0,     0,    34,    87,
    34,   352,   194,   207,   198,     0,     0,    34,    87,    34,
   300,   352,   195,   207,   198,     0,     0,    34,    87,    34,
    22,    34,   352,   196,   207,   198,     0,     0,    34,    87,
    34,   300,    22,    34,   352,   197,   207,   198,     0,   223,
   238,   246,     0,   200,     0,     0,   200,   201,     0,   201,
     0,     0,    30,    87,   202,   204,     0,     0,    30,    21,
   301,    87,   203,   204,     0,   205,    95,     0,     0,   205,
    96,   206,     0,   206,     0,    34,     0,   208,     0,     0,
   208,   209,     0,   209,     0,     0,    30,    87,   210,   212,
     0,     0,    30,    21,   301,    87,   211,   212,     0,   213,
    95,     0,     0,   213,    96,   214,     0,   214,     0,    34,
     0,   216,     0,     0,   216,   217,     0,   217,     0,     0,
     7,    87,   218,   220,     0,     0,     7,    21,   301,    87,
   219,   220,     0,   221,    95,     0,     0,   221,    96,   222,
     0,   222,     0,    34,    99,   301,     0,   224,     0,     0,
   224,   225,     0,   225,     0,     0,     7,    87,   226,   228,
     0,     0,     7,    21,   301,    87,   227,   228,     0,   229,
     0,     0,   229,   230,     0,   230,     0,    34,    99,   301,
    95,     0,   231,   232,     0,     0,     0,     9,    87,   233,
   235,     0,     0,     9,    21,   301,    87,   234,   235,     0,
   236,    95,     0,     0,   236,    96,   237,     0,   237,     0,
    34,   300,     0,    34,     0,   238,   239,     0,     0,     0,
     9,    87,   240,   242,     0,     0,     9,    21,   301,    87,
   241,   242,     0,   243,    95,     0,     0,   243,    96,   244,
     0,   244,     0,    34,   300,     0,    34,     0,    12,    87,
    35,    95,     0,     0,    12,    87,    35,    95,     0,     0,
   248,     0,     0,     0,    11,    15,    87,   249,   251,     0,
     0,    11,    87,   250,   251,     0,   252,     0,     0,   252,
   253,     0,   253,     0,   254,     0,   261,     0,     0,    18,
   255,   256,    20,   260,    28,   352,     0,   257,    96,   256,
     0,   257,     0,    34,    99,   301,    19,   301,     0,    21,
   301,     0,     0,    12,    35,     0,     0,   260,   261,     0,
   261,     0,   262,    17,   266,   258,   259,   353,     0,   262,
    17,   270,    17,   266,   258,   259,   353,     0,   262,    16,
   266,   258,   259,   353,     0,   262,    16,   270,    16,   266,
   258,   259,   353,     0,   263,   102,   264,     0,   265,     0,
    34,   300,     0,    34,     0,    34,   300,     0,    34,     0,
    34,    68,     0,    34,   300,     0,    34,     0,    34,    68,
     0,   267,   102,   268,     0,   269,     0,    34,   300,     0,
    34,     0,    34,   300,     0,    34,     0,    34,    68,     0,
    34,   300,     0,    34,     0,    34,    68,     0,    34,     0,
    31,   301,     0,   270,    31,   301,     0,   272,     0,     0,
     0,    11,    15,    87,   273,   275,     0,     0,    11,    87,
   274,   275,     0,   276,     0,     0,   276,   277,     0,   277,
     0,   278,     0,   285,     0,     0,    18,   279,   280,    20,
   284,    28,    95,     0,   281,    96,   280,     0,   281,     0,
    34,    99,   301,    19,   301,     0,    21,   301,     0,     0,
    12,    35,     0,     0,   284,   285,     0,   285,     0,   286,
    17,   290,   282,   283,    95,     0,   286,    17,   294,    17,
   290,   282,   283,    95,     0,   286,    16,   290,   282,   283,
    95,     0,   286,    16,   294,    16,   290,   282,   283,    95,
     0,   287,   102,   288,     0,   289,     0,    34,   300,     0,
    34,     0,    34,   300,     0,    34,     0,    34,    68,     0,
    34,   300,     0,    34,     0,    34,    68,     0,   291,   102,
   292,     0,   293,     0,    34,   300,     0,    34,     0,    34,
   300,     0,    34,     0,    34,    68,     0,    34,   300,     0,
    34,     0,    34,    68,     0,    34,     0,    34,   301,     0,
   294,    34,   301,     0,   296,   199,   215,   297,     0,    23,
    34,    87,    34,   352,     0,    27,   352,     0,   299,    97,
   207,   223,    98,   352,     0,    23,    34,    87,    34,   352,
     0,   100,   301,   101,     0,   304,     0,   302,     0,   303,
     0,    39,   103,   304,    96,   304,   104,     0,    39,   103,
   304,   104,     0,    39,   103,   104,     0,    39,     0,    40,
   103,   311,    96,   311,   104,     0,    40,   103,   311,   104,
     0,   305,     0,   103,   304,   104,     0,   304,    88,   304,
     0,   304,    89,   304,     0,   304,    90,   304,     0,   304,
    91,   304,     0,   304,    92,   304,     0,   304,    93,   304,
     0,    89,   304,     0,   304,    69,   304,     0,   304,    70,
   304,     0,   304,    71,   304,     0,   304,    72,   304,     0,
   304,    73,   304,     0,   304,    74,   304,     0,   304,    75,
   304,     0,   304,    76,   304,     0,   304,    77,   304,     0,
    78,   304,     0,   304,    79,   304,     0,   304,    80,   304,
     0,   304,    81,   304,     0,    82,   304,     0,   304,    83,
   304,     0,   304,    84,   304,     0,   304,    86,   304,    87,
   304,     0,    34,   103,   104,     0,    34,   103,   304,   104,
     0,    34,   103,   304,    96,   304,   104,     0,    34,   103,
   304,    96,   304,    96,   304,   104,     0,    34,   103,   304,
    96,   304,    96,   304,    96,   304,   104,     0,   306,     0,
   307,     0,   308,     0,   309,     0,   310,     0,    34,     0,
    41,    34,     0,    41,    42,    34,     0,    42,    41,    34,
     0,    42,    34,     0,   311,     0,    37,     0,    38,     0,
   312,     0,    67,     0,    67,   103,   104,     0,    66,   103,
    34,   104,     0,    35,     0,    32,     0,    33,     0,   313,
     0,   313,    32,    34,     0,   313,    33,    34,     0,    32,
    34,     0,    33,    34,     0,    49,    50,   352,     0,    53,
    34,    95,     0,    52,    34,    95,     0,    52,    55,    34,
    95,     0,    54,    34,    95,     0,     0,    54,    34,    97,
   319,   321,    98,   352,     0,     0,    54,    34,    56,    34,
    97,   320,   321,    98,   352,     0,   322,     0,     0,   322,
   323,     0,   323,     0,    34,    95,     0,    34,    99,   351,
    95,     0,     0,    51,    34,    97,   325,   333,   339,    98,
   352,     0,     0,    51,    34,    56,    34,    97,   326,   333,
   339,    98,   352,     0,     0,    52,    34,    97,   328,   333,
   339,    98,   352,     0,     0,    52,    34,    56,    34,    97,
   329,   333,   339,    98,   352,     0,     0,    53,    34,    97,
   331,   333,   339,    98,   352,     0,     0,    53,    34,    56,
    34,    97,   332,   333,   339,    98,   352,     0,     0,    58,
    87,   334,   335,     0,     0,   336,     0,     0,   336,   337,
     0,   337,     0,    34,    99,   338,    95,     0,    35,     0,
    32,     0,    33,     0,   313,     0,    37,     0,    38,     0,
     0,    57,    87,   340,   341,     0,     0,   342,     0,     0,
   342,   343,     0,   343,     0,     0,   346,    34,   344,   347,
   348,   349,    95,     0,     0,    59,   346,    34,   345,   347,
   348,   349,    95,     0,    34,     0,    34,    90,     0,    60,
     0,    61,     0,    62,     0,    63,     0,    65,    60,     0,
    65,    61,     0,    65,    62,     0,    65,    63,     0,    64,
     0,    45,     0,    46,     0,   100,    32,   101,     0,   100,
    34,   101,     0,   100,   101,     0,     0,    54,   103,    34,
   104,     0,     0,    99,   350,     0,     0,    35,     0,    36,
     0,    32,     0,    89,    32,     0,    33,     0,    89,    33,
     0,   313,     0,    37,     0,    38,     0,    34,     0,    32,
     0,    89,    32,     0,    34,     0,    95,     0,     0,    96,
     0,    95,     0
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
   734,   738,   748,   749,   753,   759,   764,   765,   769,   770,
   771,   772,   776,   777,   781,   786,   794,   795,   799,   804,
   815,   816,   820,   826,   831,   832,   836,   837,   838,   839,
   843,   844,   848,   853,   861,   862,   866,   871,   882,   883,
   887,   893,   898,   899,   903,   904,   908,   914,   917,   924,
   927,   933,   936,   943,   949,   958,   959,   963,   969,   974,
   975,   979,   980,   984,   990,   993,  1000,  1003,  1009,  1012,
  1019,  1025,  1034,  1035,  1039,  1040,  1044,  1050,  1052,  1059,
  1064,  1065,  1069,  1070,  1074,  1085,  1086,  1090,  1091,  1095,
  1101,  1103,  1110,  1115,  1116,  1120,  1121,  1125,  1136,  1137,
  1141,  1142,  1146,  1152,  1154,  1161,  1167,  1168,  1172,  1173,
  1177,  1189,  1190,  1194,  1195,  1199,  1205,  1207,  1214,  1220,
  1221,  1225,  1226,  1230,  1242,  1243,  1247,  1253,  1255,  1262,
  1267,  1268,  1272,  1273,  1277,  1284,  1295,  1296,  1300,  1306,
  1308,  1315,  1320,  1321,  1325,  1326,  1330,  1337,  1348,  1352,
  1359,  1363,  1370,  1371,  1375,  1382,  1384,  1391,  1396,  1397,
  1401,  1402,  1406,  1407,  1411,  1416,  1425,  1426,  1430,  1440,
  1444,  1448,  1452,  1456,  1457,  1461,  1466,  1471,  1477,  1486,
  1487,  1491,  1497,  1505,  1510,  1514,  1522,  1529,  1535,  1545,
  1546,  1550,  1555,  1562,  1567,  1571,  1579,  1584,  1588,  1597,
  1602,  1607,  1618,  1619,  1623,  1630,  1632,  1639,  1644,  1645,
  1649,  1650,  1654,  1655,  1659,  1664,  1673,  1674,  1678,  1688,
  1692,  1696,  1700,  1704,  1705,  1709,  1714,  1719,  1725,  1734,
  1735,  1739,  1745,  1753,  1758,  1762,  1770,  1777,  1783,  1793,
  1794,  1798,  1803,  1810,  1815,  1819,  1827,  1832,  1836,  1845,
  1850,  1855,  1866,  1873,  1882,  1893,  1904,  1917,  1922,  1927,
  1931,  1942,  1944,  1946,  1948,  1953,  1955,  1960,  1961,  1964,
  1966,  1968,  1970,  1972,  1974,  1977,  1981,  1983,  1985,  1987,
  1989,  1991,  1994,  1996,  1998,  2001,  2005,  2007,  2009,  2012,
  2015,  2017,  2019,  2022,  2024,  2026,  2028,  2030,  2035,  2036,
  2037,  2038,  2039,  2043,  2048,  2053,  2059,  2065,  2073,  2077,
  2079,  2084,  2088,  2090,  2092,  2097,  2102,  2104,  2106,  2112,
  2113,  2114,  2115,  2123,  2132,  2141,  2148,  2158,  2167,  2174,
  2178,  2186,  2193,  2194,  2198,  2199,  2203,  2209,  2219,  2225,
  2229,  2236,  2243,  2249,  2253,  2260,  2267,  2273,  2277,  2284,
  2291,  2297,  2297,  2301,  2302,  2306,  2307,  2311,  2321,  2322,
  2323,  2324,  2325,  2326,  2330,  2336,  2336,  2340,  2341,  2345,
  2346,  2350,  2356,  2360,  2367,  2374,  2375,  2377,  2378,  2379,
  2380,  2382,  2383,  2384,  2385,  2387,  2388,  2389,  2394,  2399,
  2404,  2408,  2412,  2416,  2420,  2424,  2428,  2429,  2430,  2431,
  2432,  2433,  2434,  2435,  2436,  2437,  2441,  2442,  2443,  2446,
  2446,  2448,  2448
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","INCLUDE",
"SIMPLE","CHANNEL","MODULE","PARAMETERS","GATES","GATESIZES","SUBMODULES","CONNECTIONS",
"DISPLAY","IN","OUT","NOCHECK","LEFT_ARROW","RIGHT_ARROW","FOR","TO","DO","IF",
"LIKE","NETWORK","ENDSIMPLE","ENDMODULE","ENDCHANNEL","ENDNETWORK","ENDFOR",
"MACHINES","ON","CHANATTRNAME","INTCONSTANT","REALCONSTANT","NAME","STRINGCONSTANT",
"CHARCONSTANT","TRUE_","FALSE_","INPUT_","XMLDOC","REF","ANCESTOR","CONSTDECL",
"NUMERICTYPE","STRINGTYPE","BOOLTYPE","XMLTYPE","ANYTYPE","CPLUSPLUS","CPLUSPLUSBODY",
"MESSAGE","CLASS","STRUCT","ENUM","NONCOBJECT","EXTENDS","FIELDS","PROPERTIES",
"ABSTRACT","CHARTYPE","SHORTTYPE","INTTYPE","LONGTYPE","DOUBLETYPE","UNSIGNED_",
"SIZEOF","SUBMODINDEX","PLUSPLUS","EQ","NE","GT","GE","LS","LE","AND","OR","XOR",
"NOT","BIN_AND","BIN_OR","BIN_XOR","BIN_COMPL","SHIFT_LEFT","SHIFT_RIGHT","INVALID_CHAR",
"'?'","':'","'+'","'-'","'*'","'/'","'%'","'^'","UMIN","';'","','","'{'","'}'",
"'='","'['","']'","'.'","'('","')'","networkdescription","somedefinitions","definition",
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
"expression","inputvalue","xmldocvalue","expr","simple_expr","parameter_expr",
"string_expr","boolconst_expr","numconst_expr","special_expr","stringconstant",
"numconst","timeconstant","cplusplus","struct_decl","class_decl","enum_decl",
"enum","@36","@37","opt_enumfields","enumfields","enumfield","message","@38",
"@39","class","@40","@41","struct","@42","@43","opt_propertiesblock","@44","opt_properties",
"properties","property","propertyvalue","opt_fieldsblock","@45","opt_fields",
"fields","field","@46","@47","fielddatatype","opt_fieldvector","opt_fieldenum",
"opt_fieldvalue","fieldvalue","enumvalue","opt_semicolon","comma_or_semicolon", NULL
};
#endif

static const short yyr1[] = {     0,
   105,   106,   106,   107,   107,   107,   107,   107,   107,   107,
   107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
   109,   108,   110,   110,   111,   112,   113,   114,   114,   115,
   115,   116,   116,   117,   118,   119,   119,   120,   120,   121,
   122,   123,   123,   124,   125,   126,   127,   128,   128,   129,
   130,   131,   131,   133,   132,   134,   134,   135,   135,   136,
   137,   137,   139,   138,   140,   140,   141,   141,   142,   143,
   143,   144,   145,   145,   146,   147,   147,   149,   148,   150,
   150,   151,   151,   152,   152,   154,   153,   155,   155,   156,
   156,   157,   158,   158,   158,   158,   158,   158,   158,   158,
   158,   158,   159,   159,   161,   160,   162,   162,   163,   163,
   163,   163,   164,   164,   165,   165,   166,   166,   167,   167,
   168,   168,   170,   169,   171,   171,   172,   172,   172,   172,
   173,   173,   174,   174,   175,   175,   176,   176,   177,   177,
   179,   178,   180,   180,   181,   181,   183,   182,   184,   182,
   185,   182,   186,   182,   187,   188,   188,   190,   189,   191,
   191,   192,   192,   194,   193,   195,   193,   196,   193,   197,
   193,   198,   199,   199,   200,   200,   202,   201,   203,   201,
   204,   204,   205,   205,   206,   207,   207,   208,   208,   210,
   209,   211,   209,   212,   212,   213,   213,   214,   215,   215,
   216,   216,   218,   217,   219,   217,   220,   220,   221,   221,
   222,   223,   223,   224,   224,   226,   225,   227,   225,   228,
   228,   229,   229,   230,   231,   231,   233,   232,   234,   232,
   235,   235,   236,   236,   237,   237,   238,   238,   240,   239,
   241,   239,   242,   242,   243,   243,   244,   244,   245,   245,
   246,   246,   247,   247,   249,   248,   250,   248,   251,   251,
   252,   252,   253,   253,   255,   254,   256,   256,   257,   258,
   258,   259,   259,   260,   260,   261,   261,   261,   261,   262,
   262,   263,   263,   264,   264,   264,   265,   265,   265,   266,
   266,   267,   267,   268,   268,   268,   269,   269,   269,   270,
   270,   270,   271,   271,   273,   272,   274,   272,   275,   275,
   276,   276,   277,   277,   279,   278,   280,   280,   281,   282,
   282,   283,   283,   284,   284,   285,   285,   285,   285,   286,
   286,   287,   287,   288,   288,   288,   289,   289,   289,   290,
   290,   291,   291,   292,   292,   292,   293,   293,   293,   294,
   294,   294,   295,   296,   297,   298,   299,   300,   301,   301,
   301,   302,   302,   302,   302,   303,   303,   304,   304,   304,
   304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
   304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
   304,   304,   304,   304,   304,   304,   304,   304,   305,   305,
   305,   305,   305,   306,   306,   306,   306,   306,   307,   308,
   308,   309,   310,   310,   310,   311,   312,   312,   312,   313,
   313,   313,   313,   314,   315,   316,   316,   317,   319,   318,
   320,   318,   321,   321,   322,   322,   323,   323,   325,   324,
   326,   324,   328,   327,   329,   327,   331,   330,   332,   330,
   334,   333,   333,   335,   335,   336,   336,   337,   338,   338,
   338,   338,   338,   338,   340,   339,   339,   341,   341,   342,
   342,   344,   343,   345,   343,   346,   346,   346,   346,   346,
   346,   346,   346,   346,   346,   346,   346,   346,   347,   347,
   347,   347,   348,   348,   349,   349,   350,   350,   350,   350,
   350,   350,   350,   350,   350,   350,   351,   351,   351,   352,
   352,   353,   353
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
     3,     3,     1,     0,     0,     4,     1,     0,     4,     3,
     4,     3,     3,     1,     3,     1,     3,     1,     3,     1,
     1,     0,     0,     4,     1,     0,     4,     3,     4,     3,
     3,     1,     3,     1,     3,     1,     3,     1,     1,     0,
     0,     4,     1,     0,     2,     1,     0,     7,     0,     8,
     0,     9,     0,    10,     3,     1,     0,     0,     4,     1,
     0,     2,     1,     0,     7,     0,     8,     0,     9,     0,
    10,     3,     1,     0,     2,     1,     0,     4,     0,     6,
     2,     0,     3,     1,     1,     1,     0,     2,     1,     0,
     4,     0,     6,     2,     0,     3,     1,     1,     1,     0,
     2,     1,     0,     4,     0,     6,     2,     0,     3,     1,
     3,     1,     0,     2,     1,     0,     4,     0,     6,     1,
     0,     2,     1,     4,     2,     0,     0,     4,     0,     6,
     2,     0,     3,     1,     2,     1,     2,     0,     0,     4,
     0,     6,     2,     0,     3,     1,     2,     1,     4,     0,
     4,     0,     1,     0,     0,     5,     0,     4,     1,     0,
     2,     1,     1,     1,     0,     7,     3,     1,     5,     2,
     0,     2,     0,     2,     1,     6,     8,     6,     8,     3,
     1,     2,     1,     2,     1,     2,     2,     1,     2,     3,
     1,     2,     1,     2,     1,     2,     2,     1,     2,     1,
     2,     3,     1,     0,     0,     5,     0,     4,     1,     0,
     2,     1,     1,     1,     0,     7,     3,     1,     5,     2,
     0,     2,     0,     2,     1,     6,     8,     6,     8,     3,
     1,     2,     1,     2,     1,     2,     2,     1,     2,     3,
     1,     2,     1,     2,     1,     2,     2,     1,     2,     1,
     2,     3,     4,     5,     2,     6,     5,     3,     1,     1,
     1,     6,     4,     3,     1,     6,     4,     1,     3,     3,
     3,     3,     3,     3,     3,     2,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     2,     3,     3,     3,     2,
     3,     3,     5,     3,     4,     6,     8,    10,     1,     1,
     1,     1,     1,     1,     2,     3,     3,     2,     1,     1,
     1,     1,     1,     3,     4,     1,     1,     1,     1,     3,
     3,     2,     2,     3,     3,     3,     4,     3,     0,     7,
     0,     9,     1,     0,     2,     1,     2,     4,     0,     8,
     0,    10,     0,     8,     0,    10,     0,     8,     0,    10,
     0,     4,     0,     1,     0,     2,     1,     4,     1,     1,
     1,     1,     1,     1,     0,     4,     0,     1,     0,     2,
     1,     0,     7,     0,     8,     1,     2,     1,     1,     1,
     1,     2,     2,     2,     2,     1,     1,     1,     3,     3,
     2,     0,     4,     0,     2,     0,     1,     1,     1,     2,
     1,     2,     1,     1,     1,     1,     1,     2,     1,     1,
     0,     1,     1
};

static const short yydefact[] = {     3,
     1,    21,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     2,     4,     9,    28,     5,     0,    10,    53,     6,
     0,    11,    53,     7,     0,    12,   174,     8,     0,    13,
    14,    15,    16,    17,    18,    19,    20,     0,    41,    27,
    47,     0,   511,     0,     0,     0,     0,     0,     0,     0,
    29,    37,     0,    77,    52,    62,    77,    62,     0,   200,
   173,   176,   187,    25,     0,    24,     0,   510,   424,     0,
   439,     0,   426,   443,     0,     0,   425,   447,     0,   428,
   429,   417,   418,   404,   416,   410,   411,   365,     0,     0,
     0,     0,   413,     0,     0,     0,     0,   511,   360,   361,
   359,   368,   399,   400,   401,   402,   403,   409,   412,   419,
   511,    26,     0,     0,     0,    36,    54,     0,   104,    76,
     0,    85,    61,   104,    85,     0,   177,     0,     0,   199,
   202,   175,     0,   213,   186,   189,    22,     0,   511,     0,
   453,     0,   453,   427,     0,   453,     0,   434,   422,   423,
     0,     0,     0,   405,     0,   408,     0,     0,     0,   386,
   390,   376,     0,    31,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   511,    33,   511,
     0,   511,     0,    57,    78,     0,     0,   103,    63,     0,
   122,    84,   140,   122,     0,   182,     0,   203,   511,   353,
   201,     0,   190,     0,     0,   212,   215,   188,    23,   354,
   441,     0,   467,   445,   467,   449,   467,   431,     0,     0,
   433,   436,   394,     0,   364,     0,     0,   406,   407,     0,
   414,   369,   377,   378,   379,   380,   381,   382,   383,   384,
   385,   387,   388,   389,   391,   392,     0,   370,   371,   372,
   373,   374,   375,   420,   421,    32,    30,     0,    34,     0,
    60,    55,     0,    59,    81,   105,   511,    40,    66,    86,
     0,     0,   121,     0,   254,   139,   157,   179,   185,   178,
     0,   184,     0,   208,   355,     0,   195,     0,   216,   511,
   214,   453,   451,     0,     0,   453,     0,   453,     0,   434,
   437,     0,   511,   435,     0,   395,     0,   363,     0,   367,
   415,     0,    39,     0,    56,     0,    93,     0,    79,     0,
    83,    92,   108,   511,    43,    69,    64,     0,    68,    89,
   123,   511,   141,     0,    71,   253,     0,   304,   156,   182,
   181,     0,   205,     0,   204,     0,   210,   192,   198,   191,
     0,   197,     0,   221,   356,   467,   455,   465,   511,   467,
   511,   467,   511,     0,   507,   509,     0,     0,   430,     0,
     0,     0,   393,    38,    58,     0,    95,    80,     0,     0,
     0,   106,   107,    42,    65,     0,    87,     0,    91,   126,
    44,   144,     0,   257,     0,     0,    70,   158,     0,    74,
   303,   180,   183,   208,     0,   207,     0,   195,   194,     0,
   218,     0,   217,   220,   223,     0,     0,   452,   454,   457,
   469,   440,     0,   444,     0,   448,   511,   508,   438,     0,
   396,   362,   366,    96,    94,    99,   100,   101,   102,    82,
   116,     0,   114,   120,     0,   118,     0,     0,    67,    88,
     0,     0,   124,   125,     0,   142,   143,   146,   255,   260,
     0,   511,    46,   161,     0,   307,     0,     0,    73,   206,
   211,   209,   193,   196,   221,     0,   222,   511,     0,   456,
   476,   487,   488,     0,   478,   479,   480,   481,   486,     0,
   466,   468,   471,     0,   511,   511,   432,     0,    97,    98,
     0,   110,     0,     0,   112,     0,     0,     0,    90,   134,
     0,   132,   138,     0,   136,     0,     0,     0,   145,   260,
   265,   288,   258,   259,   262,   263,   264,     0,     0,   281,
     0,   511,    49,     0,   159,   160,   163,   305,   310,     0,
   511,   219,     0,   442,   460,   461,   459,   463,   464,   462,
     0,   477,     0,   482,   483,   484,   485,   470,   472,   446,
   450,     0,   397,   115,   113,   119,   117,   109,   111,     0,
   128,     0,     0,   130,     0,     0,     0,   511,   256,     0,
   289,     0,   287,   261,     0,     0,     0,    72,    48,     0,
   162,   310,   315,   338,   308,   309,   312,   313,   314,     0,
     0,   331,     0,    50,   224,   458,   474,   492,     0,   133,
   131,   137,   135,   127,   129,     0,   511,   147,     0,     0,
   268,     0,     0,   298,   271,     0,   291,     0,   271,     0,
   285,   280,   511,   306,     0,   339,   337,   311,     0,     0,
     0,    75,   492,     0,   494,   398,   511,     0,   149,   174,
     0,     0,     0,   358,   301,   299,   297,     0,   273,     0,
     0,     0,   273,     0,   286,   284,     0,   511,   164,     0,
     0,   318,   348,   321,     0,   341,     0,   321,     0,   335,
   330,   494,     0,     0,   491,     0,   496,   151,   511,   174,
   200,     0,     0,   275,   267,   270,     0,     0,   295,   290,
   298,   271,   302,     0,   271,   511,     0,   166,   187,     0,
     0,     0,   349,   347,   351,     0,   323,     0,     0,     0,
   323,     0,   336,   334,   496,   489,   490,     0,     0,     0,
   174,   153,   200,   148,   226,     0,   511,   274,   272,   513,
   512,   278,   296,   294,   273,   276,   273,   168,   511,   187,
   213,     0,     0,   325,   317,   320,     0,     0,   345,   340,
   348,   321,   352,     0,   321,     0,     0,   499,   501,   506,
   497,   498,   504,   505,     0,   503,   495,   473,   200,   174,
   150,   250,   269,   266,     0,     0,   187,   170,   213,   165,
   238,     0,     0,   324,   322,   328,   346,   344,   323,   326,
   323,   475,   493,   500,   502,   152,   200,     0,     0,   225,
   155,   279,   277,   213,   187,   167,   252,   319,   316,     0,
     0,   154,     0,   227,     0,   169,   213,     0,     0,   237,
   172,   329,   327,     0,   232,     0,   171,     0,   239,     0,
   229,   236,   228,     0,   234,   249,     0,   244,     0,   232,
   235,   231,     0,   241,   248,   240,     0,   246,   251,   230,
   233,   244,   247,   243,     0,   242,   245,     0,     0,     0
};

static const short yydefgoto[] = {   878,
     1,    12,    13,    38,    65,    66,    14,    15,    50,    51,
   112,    16,    17,   115,   116,    18,    19,   278,    20,    21,
    22,    23,   473,    24,    25,    54,    55,   194,   272,   273,
   274,   122,   123,   279,   337,   338,   339,   406,   407,   478,
   479,   119,   120,   275,   329,   330,   201,   202,   340,   397,
   398,   331,   332,   197,   198,   333,   392,   393,   452,   453,
   455,   456,   282,   283,   400,   463,   464,   521,   522,   524,
   525,   285,   286,   402,   466,   467,   468,   660,   700,   741,
   790,   744,   348,   349,   474,   545,   546,   547,   719,   760,
   797,   825,   800,    60,    61,    62,   206,   350,   290,   291,
   292,   134,   135,   136,   297,   418,   360,   361,   362,   745,
   130,   131,   294,   414,   355,   356,   357,   801,   216,   217,
   364,   485,   423,   424,   425,   792,   820,   845,   860,   853,
   854,   855,   827,   840,   858,   872,   866,   867,   868,   821,
   841,   345,   346,   530,   470,   533,   534,   535,   536,   590,
   630,   631,   669,   708,   703,   537,   538,   539,   642,   540,
   635,   636,   710,   637,   638,   410,   411,   602,   549,   605,
   606,   607,   608,   645,   681,   682,   727,   768,   763,   609,
   610,   611,   691,   612,   684,   685,   770,   686,   687,    26,
    27,   210,    28,    29,   667,    98,    99,   100,   101,   102,
   103,   104,   105,   106,   107,   108,   109,   110,    30,    31,
    32,    33,    34,   148,   310,   230,   231,   232,    35,   141,
   302,    36,   143,   306,    37,   146,   308,   223,   367,   428,
   429,   430,   561,   305,   431,   501,   502,   503,   618,   653,
   504,   655,   697,   740,   787,   378,    69,   752
};

static const short yypact[] = {-32768,
    28,-32768,    -6,     5,    29,   148,   118,   155,    -8,   196,
   198,-32768,-32768,-32768,   153,-32768,   104,-32768,   223,-32768,
   165,-32768,   223,-32768,   171,-32768,   255,-32768,   178,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   259,   202,   206,
   213,   234,   231,   131,   -29,   294,   107,   149,   321,   307,
   313,   323,   283,   379,-32768,   361,   379,   361,    -4,   389,
   255,-32768,   377,-32768,   -34,-32768,   366,-32768,-32768,   391,
-32768,   394,-32768,-32768,   328,   441,-32768,-32768,   448,-32768,
-32768,   458,   460,   338,-32768,-32768,-32768,   414,   415,   130,
   132,   416,   419,   360,   360,   360,   360,   231,-32768,-32768,
   747,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   218,
    42,-32768,   321,   417,   425,   490,-32768,   438,   518,-32768,
   440,   521,-32768,   518,   521,   321,-32768,   120,   502,   389,
-32768,-32768,   133,   524,   377,-32768,-32768,   259,   231,   435,
   475,   437,   475,-32768,   439,   475,   442,   501,-32768,-32768,
   245,   263,   505,-32768,   504,-32768,   507,   508,   433,-32768,
-32768,-32768,   669,-32768,   360,   360,   360,   360,   360,   360,
   360,   360,   360,   360,   360,   360,   360,   360,   360,   360,
   360,   360,   360,   360,   360,   509,   510,   231,-32768,   231,
   321,   231,   446,   513,-32768,   462,   526,-32768,-32768,   464,
   544,-32768,   543,   544,   467,   522,   321,-32768,   231,-32768,
-32768,   321,-32768,   134,   459,   524,-32768,-32768,-32768,   461,
-32768,   474,   506,-32768,   506,-32768,   506,-32768,   164,   466,
   501,-32768,-32768,   397,-32768,   551,    90,-32768,-32768,   463,
-32768,-32768,   367,   367,   367,   367,   367,   367,   797,   797,
   797,   225,   225,   225,   290,   290,   503,   246,   246,   469,
   469,   469,   469,-32768,-32768,-32768,-32768,   470,-32768,   321,
-32768,-32768,   197,-32768,    16,-32768,   124,-32768,   532,-32768,
   481,   471,-32768,   484,   574,-32768,   578,-32768,-32768,-32768,
   211,-32768,   511,   563,-32768,   512,   566,   321,-32768,   231,
-32768,   475,-32768,   514,   516,   475,   517,   475,   520,   501,
-32768,   144,   231,-32768,   360,-32768,   360,-32768,   505,-32768,
-32768,   360,-32768,   515,-32768,   513,   519,   569,-32768,   224,
-32768,-32768,   311,   231,-32768,-32768,-32768,   236,-32768,    16,
-32768,   231,-32768,     9,   592,-32768,   525,   597,-32768,   522,
-32768,   522,-32768,   530,-32768,   247,-32768,-32768,-32768,-32768,
   251,-32768,   529,   571,-32768,   506,   575,-32768,   231,   506,
   231,   506,   231,   538,-32768,-32768,   579,   550,-32768,   594,
   695,   534,   772,-32768,-32768,   374,-32768,-32768,    16,   612,
   614,-32768,   337,-32768,-32768,   532,-32768,   554,-32768,   355,
-32768,   617,   565,-32768,   567,   628,-32768,-32768,    23,   644,
-32768,-32768,-32768,   563,   321,-32768,   563,   566,-32768,   566,
-32768,   558,-32768,   571,-32768,   560,   561,-32768,   575,-32768,
   370,-32768,   581,-32768,   583,-32768,   231,-32768,-32768,   360,
-32768,-32768,-32768,   615,   618,-32768,-32768,-32768,-32768,-32768,
   572,   269,-32768,   576,   289,-32768,   612,   614,-32768,    16,
   654,   657,-32768,   392,   605,-32768,   617,-32768,-32768,    22,
   658,   128,-32768,   660,   608,-32768,   609,   599,-32768,-32768,
-32768,-32768,-32768,-32768,   571,   321,-32768,   231,   339,-32768,
   610,-32768,-32768,   450,-32768,-32768,-32768,-32768,-32768,   351,
-32768,   370,-32768,   665,   231,   231,-32768,   633,-32768,-32768,
   600,-32768,   612,   626,-32768,   614,   341,   344,-32768,   611,
   348,-32768,   630,   357,-32768,   654,   657,   681,-32768,    22,
-32768,   -45,-32768,    22,-32768,-32768,-32768,   445,   616,-32768,
   636,   231,-32768,   641,-32768,   660,-32768,-32768,    31,   697,
   231,-32768,   638,-32768,   458,   460,-32768,-32768,-32768,   218,
   639,-32768,   702,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   360,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   646,
-32768,   654,   650,-32768,   657,   369,   402,    45,-32768,   720,
-32768,   321,   661,-32768,   200,   200,   722,-32768,-32768,   743,
-32768,    31,-32768,   167,-32768,    31,-32768,-32768,-32768,   483,
   678,-32768,   687,-32768,-32768,-32768,-32768,   689,   721,-32768,
-32768,-32768,-32768,-32768,-32768,   769,   -10,-32768,   707,   788,
   719,   728,   321,     4,   811,   732,-32768,    21,   811,   150,
   -27,-32768,    48,-32768,   816,-32768,   752,-32768,   823,   823,
   824,-32768,   689,   117,   805,-32768,   231,   838,-32768,   255,
   321,   839,   720,-32768,-32768,-32768,   773,   321,   862,   845,
   848,   321,   862,   848,-32768,-32768,   849,    -3,-32768,   785,
   871,   796,   158,   872,   792,-32768,   143,   872,    19,   -14,
-32768,   805,   794,   795,-32768,   798,   799,-32768,   231,   255,
   389,   878,   215,-32768,-32768,-32768,   864,   407,   115,-32768,
   188,   811,-32768,   407,   811,   231,   866,-32768,   377,   321,
   868,   816,-32768,   801,-32768,   321,   892,   873,   874,   321,
   892,   874,-32768,-32768,   799,-32768,-32768,   875,   176,   810,
   255,-32768,   389,-32768,-32768,   321,   231,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   862,-32768,   862,-32768,   231,   377,
   524,   887,   220,-32768,-32768,-32768,   876,   815,   138,-32768,
   189,   872,-32768,   817,   872,   818,   812,   458,   460,-32768,
-32768,-32768,-32768,-32768,   472,   218,-32768,-32768,   389,   255,
-32768,   261,-32768,-32768,   407,   407,   377,-32768,   524,-32768,
-32768,   321,   819,-32768,-32768,-32768,-32768,-32768,   892,-32768,
   892,-32768,-32768,-32768,-32768,-32768,   389,   135,   828,-32768,
-32768,-32768,-32768,   524,   377,-32768,   272,-32768,-32768,   822,
   825,-32768,   321,-32768,   883,-32768,   524,   140,   832,-32768,
-32768,-32768,-32768,   834,   888,   829,-32768,   321,-32768,   890,
-32768,   826,-32768,   411,-32768,-32768,   836,   893,   833,   888,
-32768,-32768,   888,-32768,   826,-32768,   413,-32768,-32768,-32768,
-32768,   893,-32768,-32768,   893,-32768,-32768,   929,   930,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   793,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   909,-32768,-32768,-32768,-32768,
   607,   877,-32768,-32768,-32768,-32768,   540,-32768,-32768,-32768,
-32768,   880,-32768,-32768,-32768,-32768,   809,-32768,-32768,-32768,
-32768,   549,  -325,   820,-32768,-32768,-32768,-32768,   482,   427,
   485,   426,   737,-32768,-32768,-32768,-32768,   420,   363,   421,
   362,-32768,-32768,-32768,-32768,-32768,   486,-32768,-32768,-32768,
-32768,  -714,-32768,-32768,-32768,-32768,-32768,   403,-32768,-32768,
-32768,-32768,  -582,  -652,-32768,   889,-32768,-32768,   601,-32768,
   602,  -689,-32768,   821,-32768,-32768,   537,-32768,   539,   897,
-32768,   830,-32768,-32768,   547,-32768,   535,   831,-32768,   742,
-32768,-32768,   477,-32768,   542,-32768,-32768,-32768,-32768,   103,
-32768,   101,-32768,-32768,-32768,-32768,    95,-32768,    93,-32768,
-32768,-32768,-32768,-32768,-32768,   443,-32768,   436,-32768,-32768,
   306,-32768,  -628,  -648,-32768,  -474,-32768,-32768,-32768,-32768,
  -527,-32768,-32768,-32768,   375,-32768,-32768,-32768,-32768,   372,
-32768,   371,-32768,-32768,   250,-32768,  -670,  -721,-32768,  -699,
-32768,-32768,-32768,-32768,  -590,-32768,-32768,-32768,   325,-32768,
-32768,-32768,-32768,-32768,  -530,  -113,-32768,-32768,   -51,-32768,
-32768,-32768,-32768,-32768,-32768,  -149,-32768,  -486,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   666,-32768,   748,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -137,-32768,-32768,
-32768,   552,-32768,  -220,-32768,-32768,-32768,   476,-32768,-32768,
   488,   327,   291,   249,-32768,-32768,   -97,  -698
};


#define	YYLAST		984


static const short yytable[] = {   190,
   164,   593,   560,   237,   307,   225,   309,   701,   227,   774,
   673,   658,   205,   189,   399,   756,   126,   731,   717,  -300,
  -300,   764,   591,   403,   714,    45,    72,    39,   791,   761,
     2,     3,     4,     5,  -300,   732,   671,   475,    40,   531,
   675,   220,   160,   161,   162,   163,    46,   743,   603,   327,
     6,   672,   730,   733,   592,   532,  -283,   627,   328,   688,
   137,   138,    41,   804,   604,    73,   626,    74,   639,   677,
   799,   666,   592,   647,   816,   188,     7,   268,     8,     9,
    10,    11,   127,   755,    68,   592,   757,   830,   789,   831,
   266,    68,   267,   293,   269,   404,   822,   823,   296,   234,
   236,   809,   832,   592,   811,  -293,   795,   824,   796,   476,
   676,   295,   678,   243,   244,   245,   246,   247,   248,   249,
   250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
   260,   261,   262,   263,   519,   837,    68,   817,   772,    68,
   207,   775,    68,   712,   592,   426,   715,   592,   693,   433,
   694,   435,   724,   212,   298,   833,   324,   334,   729,   734,
   848,   542,    76,   154,   366,   156,   674,    43,   370,   382,
   372,   155,   157,  -350,  -350,   375,   730,   376,   754,   335,
   672,    42,   753,    49,   363,   319,    70,   704,    44,    82,
    83,    84,    85,   320,    86,    87,    88,    89,    90,    91,
    52,    77,   365,    78,    79,   807,   208,   778,   779,   780,
   781,   782,   783,   784,   592,   379,   826,   695,    68,   213,
   299,   834,    68,    92,    93,   723,   849,    71,   748,    47,
   633,    48,   377,   634,   646,    94,   394,   592,   808,    95,
   724,   836,   747,    80,   401,    81,    96,   803,   532,   186,
   187,    53,   786,   604,   847,   666,   723,   592,   311,  -343,
    97,    56,   312,   380,   785,   381,   592,    58,  -333,   818,
   383,   432,   819,   434,    63,   436,    82,    83,    84,    85,
   838,    86,    87,   839,    59,    90,    91,   592,   592,  -293,
  -343,   325,   326,    64,    82,    83,    84,    85,   -45,    86,
    87,   481,   -35,    90,    91,   351,   352,   177,   178,   -51,
    92,    93,   180,   181,   182,   183,   184,   185,   388,   389,
    67,   861,    94,   390,   391,    68,    95,    75,    92,    93,
   395,   396,   111,    96,   873,   182,   183,   184,   185,   507,
    94,   416,   417,   113,    95,   419,   420,    97,   233,   457,
   458,    96,    82,    83,    84,    85,   114,    86,    87,    88,
    89,    90,    91,   512,   513,    97,   235,   461,   462,   117,
   555,   556,   553,   557,   543,   558,   559,   180,   181,   182,
   183,   184,   185,   515,   516,   118,    92,    93,   508,   121,
   554,    82,    83,    84,    85,   128,    86,    87,    94,   139,
    90,    91,    95,   491,   526,   527,   133,   570,   571,    96,
   564,   565,   566,   567,   492,   493,   444,   445,   446,   447,
   448,   449,   144,    97,   140,    92,    93,   142,   494,   495,
   496,   497,   498,   499,   500,   578,   513,    94,   579,   516,
   151,    95,   581,   582,   599,   174,   175,   176,    96,   177,
   178,   584,   585,   614,   180,   181,   182,   183,   184,   185,
   595,   596,    97,   624,   582,   165,   166,   167,   168,   169,
   170,   171,   172,   173,   145,   174,   175,   176,   632,   177,
   178,   147,   179,   491,   180,   181,   182,   183,   184,   185,
   628,   149,   315,   150,   492,   493,   625,   585,   649,   650,
   316,   750,   751,   814,   815,   862,   863,   874,   875,   495,
   496,   497,   498,   499,   500,   191,   152,   153,   158,   665,
   619,   159,   192,   193,   195,   196,   199,   200,   209,   659,
   214,   221,   222,   224,   229,   226,   241,   238,   228,    85,
   239,   240,   264,   265,   270,   679,   271,   702,   276,   277,
   280,   281,   284,   288,   706,   289,   300,  -357,   713,   698,
   303,   185,   304,   313,   323,   336,   321,   341,   342,   725,
   343,   165,   166,   167,   168,   169,   170,   171,   172,   173,
   718,   174,   175,   176,   344,   177,   178,   347,   179,   322,
   180,   181,   182,   183,   184,   185,   354,   353,   358,   359,
   368,   742,   387,   405,   422,   386,   762,   409,   427,   384,
   438,   408,   766,   369,   371,   421,   773,   373,   758,   165,
   166,   167,   168,   169,   170,   171,   172,   173,   415,   174,
   175,   176,   793,   177,   178,   437,   179,   443,   180,   181,
   182,   183,   184,   185,   439,   451,   317,   454,   460,   794,
   465,   469,   472,   471,   318,   477,   486,   488,   509,   489,
   510,   798,   165,   166,   167,   168,   169,   170,   171,   172,
   173,   511,   174,   175,   176,   514,   177,   178,   505,   179,
   506,   180,   181,   182,   183,   184,   185,   520,   828,   440,
   523,   528,   541,   544,   548,   550,   551,   441,   569,   562,
   574,   165,   166,   167,   168,   169,   170,   171,   172,   173,
   580,   174,   175,   176,   588,   177,   178,   597,   179,   844,
   180,   181,   182,   183,   184,   185,   576,   600,   572,   583,
   598,   613,   615,   616,   857,   617,   573,   165,   166,   167,
   168,   169,   170,   171,   172,   173,   620,   174,   175,   176,
   622,   177,   178,   629,   179,   641,   180,   181,   182,   183,
   184,   185,  -282,   165,   166,   167,   168,   169,   170,   171,
   172,   173,   242,   174,   175,   176,   643,   177,   178,   651,
   179,   652,   180,   181,   182,   183,   184,   185,   654,   165,
   166,   167,   168,   169,   170,   171,   172,   173,   442,   174,
   175,   176,   657,   177,   178,   661,   179,   662,   180,   181,
   182,   183,   184,   185,   663,   165,   166,   167,   168,   169,
   170,   171,   172,   173,   656,   174,   175,   176,   664,   177,
   178,   668,   179,   670,   180,   181,   182,   183,   184,   185,
   165,   166,   167,   168,   169,   170,   171,   172,   173,   680,
   174,   175,   176,  -332,   177,   178,   683,   690,   696,   180,
   181,   182,   183,   184,   185,   165,   166,   167,   168,   169,
   170,   699,   532,   707,  -292,   174,   175,   176,   709,   177,
   178,   711,   716,   720,   180,   181,   182,   183,   184,   185,
   721,   722,   726,   728,   736,   737,   746,   739,   749,   759,
   738,   604,  -342,   767,   788,   802,   769,   771,   777,   806,
   805,   810,   812,   829,   835,   813,   842,   846,   850,   843,
   851,   852,   864,   856,   859,   592,   865,   869,   879,   880,
   219,    57,   385,   204,   125,   459,   124,   450,   517,   575,
   287,   577,   518,   203,   621,   586,   623,   587,   601,   132,
   412,   482,   529,   413,   483,   218,   129,   301,   484,   211,
   480,   552,   870,   871,   215,   487,   876,   877,   705,   594,
   640,   765,   589,   644,   689,   374,   648,   568,   314,   692,
   490,   563,   735,   776
};

static const short yycheck[] = {   113,
    98,   532,   489,   153,   225,   143,   227,   660,   146,   731,
   639,    22,   126,   111,   340,   714,    21,   688,    22,    16,
    17,   721,    68,    15,   673,    34,    56,    34,   743,   719,
     3,     4,     5,     6,    31,    17,    16,    15,    34,    18,
    68,   139,    94,    95,    96,    97,    55,   700,    18,    34,
    23,    31,    34,    68,   100,    34,   102,   588,    43,   650,
    95,    96,    34,   763,    34,    95,    22,    97,   596,    22,
   760,    68,   100,   604,   789,    34,    49,   191,    51,    52,
    53,    54,    87,   712,    95,   100,   715,   809,   741,   811,
   188,    95,   190,   207,   192,    87,   795,   796,   212,   151,
   152,   772,   817,   100,   775,   102,   755,   797,   757,    87,
   641,   209,   643,   165,   166,   167,   168,   169,   170,   171,
   172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
   182,   183,   184,   185,   460,   825,    95,   790,   729,    95,
    21,   732,    95,   671,   100,   366,   674,   100,    32,   370,
    34,   372,   683,    21,    21,    21,   270,    34,    16,   690,
    21,    34,    56,    34,   302,    34,    17,    50,   306,   319,
   308,    42,    41,    16,    17,    32,    34,    34,   709,   277,
    31,    34,    68,    31,   298,    96,    56,   662,    34,    32,
    33,    34,    35,   104,    37,    38,    39,    40,    41,    42,
    97,    95,   300,    97,    56,    68,    87,    32,    33,    34,
    35,    36,    37,    38,   100,   313,   799,   101,    95,    87,
    87,    87,    95,    66,    67,    68,    87,    97,   703,    34,
    31,    34,    89,    34,    68,    78,   334,   100,   769,    82,
   771,   824,    28,    95,   342,    97,    89,    28,    34,    32,
    33,    29,   739,    34,   837,    68,    68,   100,    95,   102,
   103,    97,    99,   315,    89,   317,   100,    97,   102,     9,
   322,   369,    12,   371,    97,   373,    32,    33,    34,    35,
     9,    37,    38,    12,    30,    41,    42,   100,   100,   102,
   102,    95,    96,    35,    32,    33,    34,    35,    97,    37,
    38,   415,    97,    41,    42,    95,    96,    83,    84,    97,
    66,    67,    88,    89,    90,    91,    92,    93,    95,    96,
    87,   852,    78,    13,    14,    95,    82,    34,    66,    67,
    95,    96,    26,    89,   865,    90,    91,    92,    93,   437,
    78,    95,    96,    31,    82,    95,    96,   103,   104,    13,
    14,    89,    32,    33,    34,    35,    34,    37,    38,    39,
    40,    41,    42,    95,    96,   103,   104,    13,    14,    87,
    32,    33,   486,    35,   472,    37,    38,    88,    89,    90,
    91,    92,    93,    95,    96,     7,    66,    67,   440,    29,
   488,    32,    33,    34,    35,     7,    37,    38,    78,    34,
    41,    42,    82,    34,    13,    14,    30,   505,   506,    89,
    60,    61,    62,    63,    45,    46,    43,    44,    45,    46,
    47,    48,    95,   103,    34,    66,    67,    34,    59,    60,
    61,    62,    63,    64,    65,    95,    96,    78,    95,    96,
   103,    82,    95,    96,   542,    79,    80,    81,    89,    83,
    84,    95,    96,   551,    88,    89,    90,    91,    92,    93,
    16,    17,   103,    95,    96,    69,    70,    71,    72,    73,
    74,    75,    76,    77,    34,    79,    80,    81,   592,    83,
    84,    34,    86,    34,    88,    89,    90,    91,    92,    93,
   588,    34,    96,    34,    45,    46,    95,    96,    16,    17,
   104,    95,    96,    32,    33,    95,    96,    95,    96,    60,
    61,    62,    63,    64,    65,    99,   103,   103,   103,   633,
   572,   103,    98,    34,    87,     8,    87,     7,    27,   627,
     7,    97,    58,    97,    34,    97,   104,    34,    97,    35,
    34,    34,    34,    34,    99,   643,    34,   661,    87,    24,
    87,     8,    10,    87,   668,    34,    98,    97,   672,   657,
    87,    93,    57,    98,    95,    34,   104,    87,    98,   683,
    87,    69,    70,    71,    72,    73,    74,    75,    76,    77,
   678,    79,    80,    81,    11,    83,    84,    10,    86,    87,
    88,    89,    90,    91,    92,    93,    34,    87,    87,    34,
    87,   699,    34,    12,    34,    87,   720,    11,    34,    95,
    32,    87,   726,    98,    98,    87,   730,    98,   716,    69,
    70,    71,    72,    73,    74,    75,    76,    77,    99,    79,
    80,    81,   746,    83,    84,    98,    86,   104,    88,    89,
    90,    91,    92,    93,    95,    34,    96,    34,    95,   747,
    34,    87,    25,    87,   104,    12,    99,    98,    44,    99,
    43,   759,    69,    70,    71,    72,    73,    74,    75,    76,
    77,   100,    79,    80,    81,   100,    83,    84,    98,    86,
    98,    88,    89,    90,    91,    92,    93,    34,   802,    96,
    34,    87,    35,    34,    87,    87,    98,   104,    34,    90,
   101,    69,    70,    71,    72,    73,    74,    75,    76,    77,
   100,    79,    80,    81,    34,    83,    84,   102,    86,   833,
    88,    89,    90,    91,    92,    93,   101,    87,    96,   100,
    95,    35,    95,    95,   848,    34,   104,    69,    70,    71,
    72,    73,    74,    75,    76,    77,   101,    79,    80,    81,
   101,    83,    84,    34,    86,    34,    88,    89,    90,    91,
    92,    93,   102,    69,    70,    71,    72,    73,    74,    75,
    76,    77,   104,    79,    80,    81,    34,    83,    84,   102,
    86,    95,    88,    89,    90,    91,    92,    93,   100,    69,
    70,    71,    72,    73,    74,    75,    76,    77,   104,    79,
    80,    81,    34,    83,    84,    99,    86,    20,    88,    89,
    90,    91,    92,    93,    96,    69,    70,    71,    72,    73,
    74,    75,    76,    77,   104,    79,    80,    81,   101,    83,
    84,    21,    86,   102,    88,    89,    90,    91,    92,    93,
    69,    70,    71,    72,    73,    74,    75,    76,    77,    34,
    79,    80,    81,   102,    83,    84,    34,    34,    54,    88,
    89,    90,    91,    92,    93,    69,    70,    71,    72,    73,
    74,    34,    34,    12,   102,    79,    80,    81,    34,    83,
    84,    34,    34,    99,    88,    89,    90,    91,    92,    93,
    20,    96,    21,   102,   101,   101,    19,    99,    35,    34,
   103,    34,   102,    12,    95,    19,    34,    34,    34,    95,
    35,    95,    95,    95,    87,   104,    95,    35,    87,    95,
    87,    34,    87,    95,    35,   100,    34,    95,     0,     0,
   138,    23,   326,   125,    58,   396,    57,   389,   457,   513,
   204,   516,   458,   124,   582,   526,   585,   527,   546,    61,
   350,   417,   467,   352,   418,   135,    60,   216,   420,   130,
   414,   485,   860,   863,   134,   424,   872,   875,   663,   534,
   596,   722,   530,   602,   650,   310,   606,   502,   231,   653,
   429,   494,   692,   735
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
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_XML);
                ;
    break;}
case 102:
#line 739 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                ;
    break;}
case 105:
#line 754 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 106:
#line 759 "ned.y"
{
                ;
    break;}
case 115:
#line 782 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 116:
#line 787 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 119:
#line 800 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 120:
#line 805 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 123:
#line 821 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 124:
#line 826 "ned.y"
{
                ;
    break;}
case 133:
#line 849 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 134:
#line 854 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 137:
#line 867 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 138:
#line 872 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 141:
#line 888 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 142:
#line 893 "ned.y"
{
                ;
    break;}
case 147:
#line 909 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 148:
#line 915 "ned.y"
{
                ;
    break;}
case 149:
#line 918 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 150:
#line 925 "ned.y"
{
                ;
    break;}
case 151:
#line 928 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 152:
#line 934 "ned.y"
{
                ;
    break;}
case 153:
#line 937 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 154:
#line 944 "ned.y"
{
                ;
    break;}
case 158:
#line 964 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 159:
#line 969 "ned.y"
{
                ;
    break;}
case 164:
#line 985 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 165:
#line 991 "ned.y"
{
                ;
    break;}
case 166:
#line 994 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 167:
#line 1001 "ned.y"
{
                ;
    break;}
case 168:
#line 1004 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 169:
#line 1010 "ned.y"
{
                ;
    break;}
case 170:
#line 1013 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 171:
#line 1020 "ned.y"
{
                ;
    break;}
case 177:
#line 1045 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 178:
#line 1050 "ned.y"
{
                ;
    break;}
case 179:
#line 1053 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 180:
#line 1059 "ned.y"
{
                ;
    break;}
case 185:
#line 1075 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 190:
#line 1096 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 191:
#line 1101 "ned.y"
{
                ;
    break;}
case 192:
#line 1104 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 193:
#line 1110 "ned.y"
{
                ;
    break;}
case 198:
#line 1126 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 203:
#line 1147 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 204:
#line 1152 "ned.y"
{
                ;
    break;}
case 205:
#line 1155 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 206:
#line 1161 "ned.y"
{
                ;
    break;}
case 211:
#line 1178 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                ;
    break;}
case 216:
#line 1200 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 217:
#line 1205 "ned.y"
{
                ;
    break;}
case 218:
#line 1208 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 219:
#line 1214 "ned.y"
{
                ;
    break;}
case 224:
#line 1231 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-3]);
                  addExpression(ps.substparam, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparam,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 227:
#line 1248 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 228:
#line 1253 "ned.y"
{
                ;
    break;}
case 229:
#line 1256 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 230:
#line 1262 "ned.y"
{
                ;
    break;}
case 235:
#line 1278 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 236:
#line 1285 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 239:
#line 1301 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 240:
#line 1306 "ned.y"
{
                ;
    break;}
case 241:
#line 1309 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 242:
#line 1315 "ned.y"
{
                ;
    break;}
case 247:
#line 1331 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 248:
#line 1338 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 249:
#line 1349 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 251:
#line 1360 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 255:
#line 1376 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 256:
#line 1382 "ned.y"
{
                ;
    break;}
case 257:
#line 1385 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 258:
#line 1391 "ned.y"
{
                ;
    break;}
case 265:
#line 1412 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;
    break;}
case 266:
#line 1417 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 269:
#line 1431 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 270:
#line 1441 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 272:
#line 1449 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 276:
#line 1462 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 277:
#line 1467 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 278:
#line 1472 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 279:
#line 1478 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 282:
#line 1492 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 283:
#line 1498 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 284:
#line 1506 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 285:
#line 1511 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 286:
#line 1515 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 287:
#line 1523 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 288:
#line 1530 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 289:
#line 1536 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 292:
#line 1551 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 293:
#line 1556 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 294:
#line 1563 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 295:
#line 1568 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 296:
#line 1572 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 297:
#line 1580 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 298:
#line 1585 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 299:
#line 1589 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 300:
#line 1598 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 301:
#line 1603 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 302:
#line 1608 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 305:
#line 1624 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 306:
#line 1630 "ned.y"
{
                ;
    break;}
case 307:
#line 1633 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 308:
#line 1639 "ned.y"
{
                ;
    break;}
case 315:
#line 1660 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;
    break;}
case 316:
#line 1665 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 319:
#line 1679 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 320:
#line 1689 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 322:
#line 1697 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 326:
#line 1710 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 327:
#line 1715 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 328:
#line 1720 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 329:
#line 1726 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 332:
#line 1740 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 333:
#line 1746 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 334:
#line 1754 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 335:
#line 1759 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 336:
#line 1763 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 337:
#line 1771 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 338:
#line 1778 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 339:
#line 1784 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 342:
#line 1799 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 343:
#line 1804 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 344:
#line 1811 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 345:
#line 1816 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 346:
#line 1820 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 347:
#line 1828 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 348:
#line 1833 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 349:
#line 1837 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 350:
#line 1846 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 351:
#line 1851 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 352:
#line 1856 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 354:
#line 1874 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 355:
#line 1883 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 356:
#line 1897 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 357:
#line 1905 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 358:
#line 1918 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 359:
#line 1924 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 360:
#line 1928 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 361:
#line 1932 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 362:
#line 1943 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 363:
#line 1945 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 364:
#line 1947 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 365:
#line 1949 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 366:
#line 1954 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 367:
#line 1956 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-1]); ;
    break;}
case 369:
#line 1962 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 370:
#line 1965 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 371:
#line 1967 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 372:
#line 1969 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 373:
#line 1971 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 374:
#line 1973 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 375:
#line 1975 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 376:
#line 1979 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); ;
    break;}
case 377:
#line 1982 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 378:
#line 1984 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 379:
#line 1986 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 380:
#line 1988 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 381:
#line 1990 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 382:
#line 1992 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 383:
#line 1995 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 384:
#line 1997 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 385:
#line 1999 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 386:
#line 2003 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 387:
#line 2006 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 388:
#line 2008 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 389:
#line 2010 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 390:
#line 2014 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 391:
#line 2016 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 392:
#line 2018 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 393:
#line 2020 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 394:
#line 2023 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 395:
#line 2025 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 396:
#line 2027 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 397:
#line 2029 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 398:
#line 2031 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-9]), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 404:
#line 2044 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 405:
#line 2049 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                ;
    break;}
case 406:
#line 2054 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 407:
#line 2060 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 408:
#line 2066 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 410:
#line 2078 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 411:
#line 2080 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 413:
#line 2089 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 414:
#line 2091 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 415:
#line 2093 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 416:
#line 2098 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 417:
#line 2103 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 418:
#line 2105 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 419:
#line 2107 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 424:
#line 2124 "ned.y"
{
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(yylsp[-1])));
                  setComments(ps.cplusplus,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 425:
#line 2133 "ned.y"
{
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(yylsp[-1]));
                  setComments(ps.structdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 426:
#line 2142 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(yylsp[-1]));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 427:
#line 2149 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(yylsp[-1]));
                  setComments(ps.classdecl,yylsp[-3],yylsp[-2]);
                ;
    break;}
case 428:
#line 2159 "ned.y"
{
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString(yylsp[-1]));
                  setComments(ps.enumdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 429:
#line 2168 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 430:
#line 2175 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 431:
#line 2179 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 432:
#line 2187 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 437:
#line 2204 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-1],yylsp[-1]);
                ;
    break;}
case 438:
#line 2210 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 439:
#line 2220 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 440:
#line 2226 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 441:
#line 2230 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 442:
#line 2237 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 443:
#line 2244 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 444:
#line 2250 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 445:
#line 2254 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 446:
#line 2261 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 447:
#line 2268 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 448:
#line 2274 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 449:
#line 2278 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 450:
#line 2285 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 451:
#line 2292 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 458:
#line 2312 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 465:
#line 2331 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 472:
#line 2351 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                ;
    break;}
case 473:
#line 2357 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 474:
#line 2361 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(true);
                ;
    break;}
case 475:
#line 2368 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 489:
#line 2395 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 490:
#line 2400 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 491:
#line 2405 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 493:
#line 2413 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 495:
#line 2421 "ned.y"
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
#line 2450 "ned.y"


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

