
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



#define	YYFINAL		868
#define	YYFLAG		-32768
#define	YYNTBASE	103

#define YYTRANSLATE(x) ((unsigned)(x) <= 338 ? yytranslate[x] : 351)

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
    39,    44,    48,    50,    52,    56,    59,    60,    62,    67,
    71,    75,    78,    84,    87,    89,    90,    96,   101,   107,
   110,   114,   117,   125,   128,   137,   140,   144,   147,   158,
   161,   163,   164,   165,   170,   173,   174,   178,   180,   182,
   184,   185,   186,   191,   194,   195,   199,   201,   203,   205,
   206,   211,   213,   214,   219,   221,   222,   223,   228,   231,
   232,   236,   238,   240,   241,   242,   247,   250,   251,   255,
   257,   259,   261,   265,   268,   272,   277,   282,   286,   290,
   294,   296,   297,   298,   303,   305,   306,   311,   315,   320,
   324,   328,   330,   334,   336,   340,   342,   346,   348,   350,
   351,   352,   357,   359,   360,   365,   369,   374,   378,   382,
   384,   388,   390,   394,   396,   400,   402,   404,   405,   406,
   411,   413,   414,   417,   419,   420,   428,   429,   438,   439,
   449,   450,   461,   465,   467,   468,   469,   474,   476,   477,
   480,   482,   483,   491,   492,   501,   502,   512,   513,   524,
   528,   530,   531,   534,   536,   537,   542,   543,   550,   553,
   554,   558,   560,   562,   564,   565,   568,   570,   571,   576,
   577,   584,   587,   588,   592,   594,   596,   598,   599,   602,
   604,   605,   610,   611,   618,   621,   622,   626,   628,   632,
   634,   635,   638,   640,   641,   646,   647,   654,   656,   657,
   660,   662,   667,   670,   671,   672,   677,   678,   685,   688,
   689,   693,   695,   698,   700,   703,   704,   705,   710,   711,
   718,   721,   722,   726,   728,   731,   733,   738,   739,   744,
   745,   747,   748,   749,   755,   756,   761,   763,   764,   767,
   769,   771,   773,   774,   775,   784,   788,   790,   796,   799,
   800,   803,   804,   807,   809,   816,   825,   832,   841,   845,
   847,   850,   852,   855,   857,   860,   863,   865,   868,   872,
   874,   877,   879,   882,   884,   887,   890,   892,   895,   897,
   900,   904,   906,   907,   908,   914,   915,   920,   922,   923,
   926,   928,   930,   932,   933,   934,   943,   947,   949,   955,
   958,   959,   962,   963,   966,   968,   975,   984,   991,  1000,
  1004,  1006,  1009,  1011,  1014,  1016,  1019,  1022,  1024,  1027,
  1031,  1033,  1036,  1038,  1041,  1043,  1046,  1049,  1051,  1054,
  1056,  1059,  1063,  1068,  1074,  1077,  1084,  1090,  1094,  1096,
  1098,  1105,  1110,  1114,  1116,  1118,  1122,  1126,  1130,  1134,
  1138,  1142,  1146,  1149,  1153,  1157,  1161,  1165,  1169,  1173,
  1177,  1181,  1185,  1188,  1192,  1196,  1200,  1203,  1207,  1211,
  1217,  1221,  1226,  1233,  1242,  1253,  1255,  1257,  1259,  1261,
  1263,  1265,  1268,  1272,  1276,  1279,  1281,  1283,  1285,  1287,
  1289,  1293,  1298,  1300,  1302,  1304,  1308,  1312,  1315,  1318,
  1322,  1326,  1330,  1335,  1336,  1344,  1345,  1355,  1357,  1358,
  1361,  1363,  1366,  1371,  1372,  1381,  1382,  1393,  1394,  1403,
  1404,  1415,  1416,  1425,  1426,  1437,  1438,  1443,  1444,  1446,
  1447,  1450,  1452,  1457,  1459,  1461,  1463,  1465,  1467,  1469,
  1470,  1475,  1476,  1478,  1479,  1482,  1484,  1485,  1493,  1494,
  1503,  1505,  1508,  1510,  1512,  1514,  1516,  1519,  1522,  1525,
  1528,  1530,  1532,  1534,  1538,  1541,  1542,  1547,  1548,  1551,
  1552,  1554,  1556,  1558,  1561,  1563,  1566,  1568,  1570,  1572,
  1574,  1576,  1579,  1581,  1583,  1584,  1586
};

static const short yyrhs[] = {   104,
     0,   104,   105,     0,     0,   106,     0,   115,     0,   122,
     0,   127,     0,   298,     0,   110,     0,   119,     0,   124,
     0,   295,     0,   312,     0,   313,     0,   314,     0,   315,
     0,   321,     0,   324,     0,   327,     0,     0,     3,   107,
   108,    93,     0,   108,    94,   109,     0,   109,     0,    35,
     0,   111,   112,   114,     0,     5,    34,     0,     0,   113,
     0,   113,    31,   301,   349,     0,    31,   301,   349,     0,
    26,    34,   349,     0,    26,   349,     0,   116,    95,   117,
    96,   349,     0,     5,    34,     0,   118,     0,     0,   118,
    34,    97,   301,    93,     0,    34,    97,   301,    93,     0,
   120,   129,   145,   157,   121,     0,     4,    34,     0,    24,
    34,   349,     0,    24,   349,     0,   123,    95,   135,   150,
   166,    96,   349,     0,     4,    34,     0,   125,   129,   145,
   157,   175,   245,   141,   126,     0,     6,    34,     0,    25,
    34,   349,     0,    25,   349,     0,   128,    95,   135,   150,
   166,   186,   270,   143,    96,   349,     0,     6,    34,     0,
   130,     0,     0,     0,    29,    85,   131,   132,     0,   133,
    93,     0,     0,   133,    94,   134,     0,   134,     0,    34,
     0,   136,     0,     0,     0,    29,    85,   137,   138,     0,
   139,    93,     0,     0,   139,    94,   140,     0,   140,     0,
    34,     0,   142,     0,     0,    12,    85,    35,    93,     0,
   144,     0,     0,    12,    85,    35,    93,     0,   146,     0,
     0,     0,     7,    85,   147,   148,     0,   149,    93,     0,
     0,   149,    94,   155,     0,   155,     0,   151,     0,     0,
     0,     7,    85,   152,   153,     0,   154,    93,     0,     0,
   154,    93,   156,     0,   156,     0,   156,     0,    34,     0,
    34,    85,    43,     0,    42,    34,     0,    34,    85,    42,
     0,    34,    85,    42,    43,     0,    34,    85,    43,    42,
     0,    34,    85,    44,     0,    34,    85,    45,     0,    34,
    85,    46,     0,   158,     0,     0,     0,     8,    85,   159,
   160,     0,   161,     0,     0,   161,    13,   162,    93,     0,
    13,   162,    93,     0,   161,    14,   164,    93,     0,    14,
   164,    93,     0,   162,    94,   163,     0,   163,     0,    34,
    98,    99,     0,    34,     0,   164,    94,   165,     0,   165,
     0,    34,    98,    99,     0,    34,     0,   167,     0,     0,
     0,     8,    85,   168,   169,     0,   170,     0,     0,   170,
    13,   171,    93,     0,    13,   171,    93,     0,   170,    14,
   173,    93,     0,    14,   173,    93,     0,   171,    94,   172,
     0,   172,     0,    34,    98,    99,     0,    34,     0,   173,
    94,   174,     0,   174,     0,    34,    98,    99,     0,    34,
     0,   176,     0,     0,     0,    10,    85,   177,   178,     0,
   179,     0,     0,   179,   180,     0,   180,     0,     0,    34,
    85,    34,   349,   181,   197,   185,     0,     0,    34,    85,
    34,   300,   349,   182,   197,   185,     0,     0,    34,    85,
    34,    22,    34,   349,   183,   197,   185,     0,     0,    34,
    85,    34,   300,    22,    34,   349,   184,   197,   185,     0,
   213,   229,   243,     0,   187,     0,     0,     0,    10,    85,
   188,   189,     0,   190,     0,     0,   190,   191,     0,   191,
     0,     0,    34,    85,    34,   349,   192,   205,   196,     0,
     0,    34,    85,    34,   300,   349,   193,   205,   196,     0,
     0,    34,    85,    34,    22,    34,   349,   194,   205,   196,
     0,     0,    34,    85,    34,   300,    22,    34,   349,   195,
   205,   196,     0,   221,   236,   244,     0,   198,     0,     0,
   198,   199,     0,   199,     0,     0,    30,    85,   200,   202,
     0,     0,    30,    21,   301,    85,   201,   202,     0,   203,
    93,     0,     0,   203,    94,   204,     0,   204,     0,    34,
     0,   206,     0,     0,   206,   207,     0,   207,     0,     0,
    30,    85,   208,   210,     0,     0,    30,    21,   301,    85,
   209,   210,     0,   211,    93,     0,     0,   211,    94,   212,
     0,   212,     0,    34,     0,   214,     0,     0,   214,   215,
     0,   215,     0,     0,     7,    85,   216,   218,     0,     0,
     7,    21,   301,    85,   217,   218,     0,   219,    93,     0,
     0,   219,    94,   220,     0,   220,     0,    34,    97,   301,
     0,   222,     0,     0,   222,   223,     0,   223,     0,     0,
     7,    85,   224,   226,     0,     0,     7,    21,   301,    85,
   225,   226,     0,   227,     0,     0,   227,   228,     0,   228,
     0,    34,    97,   301,    93,     0,   229,   230,     0,     0,
     0,     9,    85,   231,   233,     0,     0,     9,    21,   301,
    85,   232,   233,     0,   234,    93,     0,     0,   234,    94,
   235,     0,   235,     0,    34,   300,     0,    34,     0,   236,
   237,     0,     0,     0,     9,    85,   238,   240,     0,     0,
     9,    21,   301,    85,   239,   240,     0,   241,    93,     0,
     0,   241,    94,   242,     0,   242,     0,    34,   300,     0,
    34,     0,    12,    85,    35,    93,     0,     0,    12,    85,
    35,    93,     0,     0,   246,     0,     0,     0,    11,    15,
    85,   247,   249,     0,     0,    11,    85,   248,   249,     0,
   250,     0,     0,   250,   251,     0,   251,     0,   252,     0,
   260,     0,     0,     0,    18,   253,   255,    20,   254,   259,
    28,   349,     0,   256,    94,   255,     0,   256,     0,    34,
    97,   301,    19,   301,     0,    21,   301,     0,     0,    12,
    35,     0,     0,   259,   260,     0,   260,     0,   261,    17,
   265,   257,   258,   350,     0,   261,    17,   269,    17,   265,
   257,   258,   350,     0,   261,    16,   265,   257,   258,   350,
     0,   261,    16,   269,    16,   265,   257,   258,   350,     0,
   262,   100,   263,     0,   264,     0,    34,   300,     0,    34,
     0,    34,   300,     0,    34,     0,    34,    66,     0,    34,
   300,     0,    34,     0,    34,    66,     0,   266,   100,   267,
     0,   268,     0,    34,   300,     0,    34,     0,    34,   300,
     0,    34,     0,    34,    66,     0,    34,   300,     0,    34,
     0,    34,    66,     0,    34,     0,    31,   301,     0,   269,
    31,   301,     0,   271,     0,     0,     0,    11,    15,    85,
   272,   274,     0,     0,    11,    85,   273,   274,     0,   275,
     0,     0,   275,   276,     0,   276,     0,   277,     0,   285,
     0,     0,     0,    18,   278,   280,    20,   279,   284,    28,
    93,     0,   281,    94,   280,     0,   281,     0,    34,    97,
   301,    19,   301,     0,    21,   301,     0,     0,    12,    35,
     0,     0,   284,   285,     0,   285,     0,   286,    17,   290,
   282,   283,    93,     0,   286,    17,   294,    17,   290,   282,
   283,    93,     0,   286,    16,   290,   282,   283,    93,     0,
   286,    16,   294,    16,   290,   282,   283,    93,     0,   287,
   100,   288,     0,   289,     0,    34,   300,     0,    34,     0,
    34,   300,     0,    34,     0,    34,    66,     0,    34,   300,
     0,    34,     0,    34,    66,     0,   291,   100,   292,     0,
   293,     0,    34,   300,     0,    34,     0,    34,   300,     0,
    34,     0,    34,    66,     0,    34,   300,     0,    34,     0,
    34,    66,     0,    34,     0,    34,   301,     0,   294,    34,
   301,     0,   296,   197,   213,   297,     0,    23,    34,    85,
    34,   349,     0,    27,   349,     0,   299,    95,   205,   221,
    96,   349,     0,    23,    34,    85,    34,   349,     0,    98,
   301,    99,     0,   303,     0,   302,     0,    39,   101,   303,
    94,   303,   102,     0,    39,   101,   303,   102,     0,    39,
   101,   102,     0,    39,     0,   304,     0,   101,   303,   102,
     0,   303,    86,   303,     0,   303,    87,   303,     0,   303,
    88,   303,     0,   303,    89,   303,     0,   303,    90,   303,
     0,   303,    91,   303,     0,    87,   303,     0,   303,    67,
   303,     0,   303,    68,   303,     0,   303,    69,   303,     0,
   303,    70,   303,     0,   303,    71,   303,     0,   303,    72,
   303,     0,   303,    73,   303,     0,   303,    74,   303,     0,
   303,    75,   303,     0,    76,   303,     0,   303,    77,   303,
     0,   303,    78,   303,     0,   303,    79,   303,     0,    80,
   303,     0,   303,    81,   303,     0,   303,    82,   303,     0,
   303,    84,   303,    85,   303,     0,    34,   101,   102,     0,
    34,   101,   303,   102,     0,    34,   101,   303,    94,   303,
   102,     0,    34,   101,   303,    94,   303,    94,   303,   102,
     0,    34,   101,   303,    94,   303,    94,   303,    94,   303,
   102,     0,   305,     0,   306,     0,   307,     0,   308,     0,
   309,     0,    34,     0,    40,    34,     0,    40,    41,    34,
     0,    41,    40,    34,     0,    41,    34,     0,    35,     0,
    37,     0,    38,     0,   310,     0,    65,     0,    65,   101,
   102,     0,    64,   101,    34,   102,     0,    32,     0,    33,
     0,   311,     0,   311,    32,    34,     0,   311,    33,    34,
     0,    32,    34,     0,    33,    34,     0,    47,    48,   349,
     0,    51,    34,    93,     0,    50,    34,    93,     0,    50,
    53,    34,    93,     0,     0,    52,    34,    95,   316,   318,
    96,   349,     0,     0,    52,    34,    54,    34,    95,   317,
   318,    96,   349,     0,   319,     0,     0,   319,   320,     0,
   320,     0,    34,    93,     0,    34,    97,   348,    93,     0,
     0,    49,    34,    95,   322,   330,   336,    96,   349,     0,
     0,    49,    34,    54,    34,    95,   323,   330,   336,    96,
   349,     0,     0,    50,    34,    95,   325,   330,   336,    96,
   349,     0,     0,    50,    34,    54,    34,    95,   326,   330,
   336,    96,   349,     0,     0,    51,    34,    95,   328,   330,
   336,    96,   349,     0,     0,    51,    34,    54,    34,    95,
   329,   330,   336,    96,   349,     0,     0,    56,    85,   331,
   332,     0,     0,   333,     0,     0,   333,   334,     0,   334,
     0,    34,    97,   335,    93,     0,    35,     0,    32,     0,
    33,     0,   311,     0,    37,     0,    38,     0,     0,    55,
    85,   337,   338,     0,     0,   339,     0,     0,   339,   340,
     0,   340,     0,     0,   343,    34,   341,   344,   345,   346,
    93,     0,     0,    57,   343,    34,   342,   344,   345,   346,
    93,     0,    34,     0,    34,    88,     0,    58,     0,    59,
     0,    60,     0,    61,     0,    63,    58,     0,    63,    59,
     0,    63,    60,     0,    63,    61,     0,    62,     0,    44,
     0,    45,     0,    98,    32,    99,     0,    98,    99,     0,
     0,    52,   101,    34,   102,     0,     0,    97,   347,     0,
     0,    35,     0,    36,     0,    32,     0,    87,    32,     0,
    33,     0,    87,    33,     0,   311,     0,    37,     0,    38,
     0,    34,     0,    32,     0,    87,    32,     0,    34,     0,
    93,     0,     0,    94,     0,    93,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   236,   240,   241,   245,   247,   249,   251,   253,   256,   258,
   260,   262,   265,   266,   267,   269,   271,   273,   275,   283,
   288,   295,   296,   300,   312,   316,   325,   326,   330,   336,
   345,   349,   359,   368,   378,   379,   383,   389,   401,   409,
   419,   423,   433,   444,   457,   468,   478,   482,   492,   506,
   519,   520,   524,   530,   535,   536,   540,   541,   545,   557,
   558,   562,   568,   573,   574,   578,   579,   583,   595,   596,
   600,   610,   611,   615,   625,   626,   630,   636,   641,   642,
   646,   650,   660,   661,   665,   671,   676,   677,   681,   685,
   696,   700,   704,   708,   712,   716,   720,   724,   728,   732,
   742,   743,   747,   753,   758,   759,   763,   764,   765,   766,
   770,   771,   775,   780,   788,   789,   793,   798,   809,   810,
   814,   820,   825,   826,   830,   831,   832,   833,   837,   838,
   842,   847,   855,   856,   860,   865,   876,   877,   881,   887,
   892,   893,   897,   898,   902,   908,   911,   918,   921,   927,
   930,   937,   943,   952,   953,   957,   963,   968,   969,   973,
   974,   978,   984,   987,   994,   997,  1003,  1006,  1013,  1019,
  1028,  1029,  1033,  1034,  1038,  1044,  1046,  1053,  1058,  1059,
  1063,  1064,  1068,  1079,  1080,  1084,  1085,  1089,  1095,  1097,
  1104,  1109,  1110,  1114,  1115,  1119,  1130,  1131,  1135,  1136,
  1140,  1146,  1148,  1155,  1161,  1162,  1166,  1167,  1171,  1183,
  1184,  1188,  1189,  1193,  1199,  1201,  1208,  1214,  1215,  1219,
  1220,  1224,  1236,  1237,  1241,  1247,  1249,  1256,  1261,  1262,
  1266,  1267,  1271,  1278,  1289,  1290,  1294,  1300,  1302,  1309,
  1314,  1315,  1319,  1320,  1324,  1331,  1342,  1346,  1353,  1357,
  1364,  1365,  1369,  1376,  1378,  1385,  1390,  1391,  1395,  1396,
  1400,  1401,  1405,  1409,  1413,  1422,  1423,  1427,  1437,  1441,
  1445,  1449,  1453,  1454,  1458,  1463,  1468,  1474,  1483,  1484,
  1488,  1494,  1502,  1507,  1511,  1519,  1526,  1532,  1542,  1543,
  1547,  1552,  1559,  1564,  1568,  1576,  1581,  1585,  1594,  1599,
  1604,  1615,  1616,  1620,  1627,  1629,  1636,  1641,  1642,  1646,
  1647,  1651,  1652,  1656,  1660,  1664,  1673,  1674,  1678,  1688,
  1692,  1696,  1700,  1704,  1705,  1709,  1714,  1719,  1725,  1734,
  1735,  1739,  1745,  1753,  1758,  1762,  1770,  1777,  1783,  1793,
  1794,  1798,  1803,  1810,  1815,  1819,  1827,  1832,  1836,  1845,
  1850,  1855,  1866,  1873,  1882,  1893,  1904,  1917,  1922,  1927,
  1938,  1940,  1942,  1944,  1949,  1950,  1953,  1955,  1957,  1959,
  1961,  1963,  1966,  1970,  1972,  1974,  1976,  1978,  1980,  1983,
  1985,  1987,  1990,  1994,  1996,  1998,  2001,  2004,  2006,  2008,
  2011,  2013,  2015,  2017,  2019,  2024,  2025,  2026,  2027,  2028,
  2032,  2037,  2042,  2048,  2054,  2062,  2067,  2069,  2074,  2078,
  2080,  2082,  2087,  2089,  2091,  2097,  2098,  2099,  2100,  2108,
  2117,  2126,  2133,  2143,  2150,  2154,  2162,  2169,  2170,  2174,
  2175,  2179,  2185,  2195,  2201,  2205,  2212,  2219,  2225,  2229,
  2236,  2243,  2249,  2253,  2260,  2267,  2273,  2273,  2277,  2278,
  2282,  2283,  2287,  2297,  2298,  2299,  2300,  2301,  2302,  2306,
  2312,  2312,  2316,  2317,  2321,  2322,  2326,  2332,  2336,  2343,
  2350,  2351,  2353,  2354,  2355,  2356,  2358,  2359,  2360,  2361,
  2363,  2364,  2365,  2370,  2375,  2379,  2383,  2387,  2391,  2395,
  2399,  2400,  2401,  2402,  2403,  2404,  2405,  2406,  2407,  2408,
  2412,  2413,  2414,  2417,  2417,  2419,  2419
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
"loopconnection_old","@32","@33","loopvarlist_old","loopvar_old","opt_conncondition_old",
"opt_conn_displaystr_old","notloopconnections_old","notloopconnection_old","leftgatespec_old",
"leftmod_old","leftgate_old","parentleftgate_old","rightgatespec_old","rightmod_old",
"rightgate_old","parentrightgate_old","channeldescr_old","opt_connblock","connblock",
"@34","@35","opt_connections","connections","connection","loopconnection","@36",
"@37","loopvarlist","loopvar","opt_conncondition","opt_conn_displaystr","notloopconnections",
"notloopconnection","leftgatespec","leftmod","leftgate","parentleftgate","rightgatespec",
"rightmod","rightgate","parentrightgate","channeldescr","networkdefinition_old",
"networkheader_old","endnetwork_old","networkdefinition","networkheader","vector",
"expression","inputvalue","expr","simple_expr","parameter_expr","string_expr",
"boolconst_expr","numconst_expr","special_expr","numconst","timeconstant","cplusplus",
"struct_decl","class_decl","enum","@38","@39","opt_enumfields","enumfields",
"enumfield","message","@40","@41","class","@42","@43","struct","@44","@45","opt_propertiesblock",
"@46","opt_properties","properties","property","propertyvalue","opt_fieldsblock",
"@47","opt_fields","fields","field","@48","@49","fielddatatype","opt_fieldvector",
"opt_fieldenum","opt_fieldvalue","fieldvalue","enumvalue","opt_semicolon","comma_or_semicolon", NULL
};
#endif

static const short yyr1[] = {     0,
   103,   104,   104,   105,   105,   105,   105,   105,   105,   105,
   105,   105,   105,   105,   105,   105,   105,   105,   105,   107,
   106,   108,   108,   109,   110,   111,   112,   112,   113,   113,
   114,   114,   115,   116,   117,   117,   118,   118,   119,   120,
   121,   121,   122,   123,   124,   125,   126,   126,   127,   128,
   129,   129,   131,   130,   132,   132,   133,   133,   134,   135,
   135,   137,   136,   138,   138,   139,   139,   140,   141,   141,
   142,   143,   143,   144,   145,   145,   147,   146,   148,   148,
   149,   149,   150,   150,   152,   151,   153,   153,   154,   154,
   155,   156,   156,   156,   156,   156,   156,   156,   156,   156,
   157,   157,   159,   158,   160,   160,   161,   161,   161,   161,
   162,   162,   163,   163,   164,   164,   165,   165,   166,   166,
   168,   167,   169,   169,   170,   170,   170,   170,   171,   171,
   172,   172,   173,   173,   174,   174,   175,   175,   177,   176,
   178,   178,   179,   179,   181,   180,   182,   180,   183,   180,
   184,   180,   185,   186,   186,   188,   187,   189,   189,   190,
   190,   192,   191,   193,   191,   194,   191,   195,   191,   196,
   197,   197,   198,   198,   200,   199,   201,   199,   202,   202,
   203,   203,   204,   205,   205,   206,   206,   208,   207,   209,
   207,   210,   210,   211,   211,   212,   213,   213,   214,   214,
   216,   215,   217,   215,   218,   218,   219,   219,   220,   221,
   221,   222,   222,   224,   223,   225,   223,   226,   226,   227,
   227,   228,   229,   229,   231,   230,   232,   230,   233,   233,
   234,   234,   235,   235,   236,   236,   238,   237,   239,   237,
   240,   240,   241,   241,   242,   242,   243,   243,   244,   244,
   245,   245,   247,   246,   248,   246,   249,   249,   250,   250,
   251,   251,   253,   254,   252,   255,   255,   256,   257,   257,
   258,   258,   259,   259,   260,   260,   260,   260,   261,   261,
   262,   262,   263,   263,   263,   264,   264,   264,   265,   265,
   266,   266,   267,   267,   267,   268,   268,   268,   269,   269,
   269,   270,   270,   272,   271,   273,   271,   274,   274,   275,
   275,   276,   276,   278,   279,   277,   280,   280,   281,   282,
   282,   283,   283,   284,   284,   285,   285,   285,   285,   286,
   286,   287,   287,   288,   288,   288,   289,   289,   289,   290,
   290,   291,   291,   292,   292,   292,   293,   293,   293,   294,
   294,   294,   295,   296,   297,   298,   299,   300,   301,   301,
   302,   302,   302,   302,   303,   303,   303,   303,   303,   303,
   303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
   303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
   303,   303,   303,   303,   303,   304,   304,   304,   304,   304,
   305,   305,   305,   305,   305,   306,   307,   307,   308,   309,
   309,   309,   310,   310,   310,   311,   311,   311,   311,   312,
   313,   314,   314,   316,   315,   317,   315,   318,   318,   319,
   319,   320,   320,   322,   321,   323,   321,   325,   324,   326,
   324,   328,   327,   329,   327,   331,   330,   330,   332,   332,
   333,   333,   334,   335,   335,   335,   335,   335,   335,   337,
   336,   336,   338,   338,   339,   339,   341,   340,   342,   340,
   343,   343,   343,   343,   343,   343,   343,   343,   343,   343,
   343,   343,   343,   344,   344,   344,   345,   345,   346,   346,
   347,   347,   347,   347,   347,   347,   347,   347,   347,   347,
   348,   348,   348,   349,   349,   350,   350
};

static const short yyr2[] = {     0,
     1,     2,     0,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
     4,     3,     1,     1,     3,     2,     0,     1,     4,     3,
     3,     2,     5,     2,     1,     0,     5,     4,     5,     2,
     3,     2,     7,     2,     8,     2,     3,     2,    10,     2,
     1,     0,     0,     4,     2,     0,     3,     1,     1,     1,
     0,     0,     4,     2,     0,     3,     1,     1,     1,     0,
     4,     1,     0,     4,     1,     0,     0,     4,     2,     0,
     3,     1,     1,     0,     0,     4,     2,     0,     3,     1,
     1,     1,     3,     2,     3,     4,     4,     3,     3,     3,
     1,     0,     0,     4,     1,     0,     4,     3,     4,     3,
     3,     1,     3,     1,     3,     1,     3,     1,     1,     0,
     0,     4,     1,     0,     4,     3,     4,     3,     3,     1,
     3,     1,     3,     1,     3,     1,     1,     0,     0,     4,
     1,     0,     2,     1,     0,     7,     0,     8,     0,     9,
     0,    10,     3,     1,     0,     0,     4,     1,     0,     2,
     1,     0,     7,     0,     8,     0,     9,     0,    10,     3,
     1,     0,     2,     1,     0,     4,     0,     6,     2,     0,
     3,     1,     1,     1,     0,     2,     1,     0,     4,     0,
     6,     2,     0,     3,     1,     1,     1,     0,     2,     1,
     0,     4,     0,     6,     2,     0,     3,     1,     3,     1,
     0,     2,     1,     0,     4,     0,     6,     1,     0,     2,
     1,     4,     2,     0,     0,     4,     0,     6,     2,     0,
     3,     1,     2,     1,     2,     0,     0,     4,     0,     6,
     2,     0,     3,     1,     2,     1,     4,     0,     4,     0,
     1,     0,     0,     5,     0,     4,     1,     0,     2,     1,
     1,     1,     0,     0,     8,     3,     1,     5,     2,     0,
     2,     0,     2,     1,     6,     8,     6,     8,     3,     1,
     2,     1,     2,     1,     2,     2,     1,     2,     3,     1,
     2,     1,     2,     1,     2,     2,     1,     2,     1,     2,
     3,     1,     0,     0,     5,     0,     4,     1,     0,     2,
     1,     1,     1,     0,     0,     8,     3,     1,     5,     2,
     0,     2,     0,     2,     1,     6,     8,     6,     8,     3,
     1,     2,     1,     2,     1,     2,     2,     1,     2,     3,
     1,     2,     1,     2,     1,     2,     2,     1,     2,     1,
     2,     3,     4,     5,     2,     6,     5,     3,     1,     1,
     6,     4,     3,     1,     1,     3,     3,     3,     3,     3,
     3,     3,     2,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     2,     3,     3,     3,     2,     3,     3,     5,
     3,     4,     6,     8,    10,     1,     1,     1,     1,     1,
     1,     2,     3,     3,     2,     1,     1,     1,     1,     1,
     3,     4,     1,     1,     1,     3,     3,     2,     2,     3,
     3,     3,     4,     0,     7,     0,     9,     1,     0,     2,
     1,     2,     4,     0,     8,     0,    10,     0,     8,     0,
    10,     0,     8,     0,    10,     0,     4,     0,     1,     0,
     2,     1,     4,     1,     1,     1,     1,     1,     1,     0,
     4,     0,     1,     0,     2,     1,     0,     7,     0,     8,
     1,     2,     1,     1,     1,     1,     2,     2,     2,     2,
     1,     1,     1,     3,     2,     0,     4,     0,     2,     0,
     1,     1,     1,     2,     1,     2,     1,     1,     1,     1,
     1,     2,     1,     1,     0,     1,     1
};

static const short yydefact[] = {     3,
     1,    20,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     2,     4,     9,    27,     5,     0,    10,    52,     6,
     0,    11,    52,     7,     0,    12,   172,     8,     0,    13,
    14,    15,    16,    17,    18,    19,     0,    40,    26,    46,
     0,   505,     0,     0,     0,     0,     0,     0,     0,    28,
    36,     0,    76,    51,    61,    76,    61,     0,   198,   171,
   174,   185,    24,     0,    23,     0,   504,   420,     0,   434,
     0,   422,   438,     0,     0,   421,   442,     0,   424,   413,
   414,   401,   406,   407,   408,   364,     0,     0,     0,   410,
     0,     0,     0,     0,   505,   360,   359,   365,   396,   397,
   398,   399,   400,   409,   415,   505,    25,     0,     0,     0,
    35,    53,     0,   102,    75,     0,    84,    60,   102,    84,
     0,   175,     0,     0,   197,   200,   173,     0,   211,   184,
   187,    21,     0,   505,     0,   448,     0,   448,   423,     0,
   448,     0,   429,   418,   419,     0,     0,   402,     0,   405,
     0,     0,     0,   383,   387,   373,     0,    30,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   505,    32,   505,     0,   505,     0,    56,    77,     0,
     0,   101,    62,     0,   120,    83,   138,   120,     0,   180,
     0,   201,   505,   353,   199,     0,   188,     0,     0,   210,
   213,   186,    22,   354,   436,     0,   462,   440,   462,   444,
   462,   426,     0,     0,   428,   431,   391,     0,   363,     0,
   403,   404,     0,   411,   366,   374,   375,   376,   377,   378,
   379,   380,   381,   382,   384,   385,   386,   388,   389,     0,
   367,   368,   369,   370,   371,   372,   416,   417,    31,    29,
     0,    33,     0,    59,    54,     0,    58,    80,   103,   505,
    39,    65,    85,     0,     0,   119,     0,   252,   137,   155,
   177,   183,   176,     0,   182,     0,   206,   355,     0,   193,
     0,   214,   505,   212,   448,   446,     0,     0,   448,     0,
   448,     0,   429,   432,     0,   505,   430,     0,   392,     0,
   362,   412,     0,    38,     0,    55,     0,    92,     0,    78,
     0,    82,    91,   106,   505,    42,    68,    63,     0,    67,
    88,   121,   505,   139,     0,    70,   251,     0,   303,   154,
   180,   179,     0,   203,     0,   202,     0,   208,   190,   196,
   189,     0,   195,     0,   219,   356,   462,   450,   460,   505,
   462,   505,   462,   505,     0,   501,   503,     0,     0,   425,
     0,     0,   390,    37,    57,     0,    94,    79,     0,     0,
     0,   104,   105,    41,    64,     0,    86,     0,    90,   124,
    43,   142,     0,   255,     0,     0,    69,   156,     0,    73,
   302,   178,   181,   206,     0,   205,     0,   193,   192,     0,
   216,     0,   215,   218,   221,     0,     0,   447,   449,   452,
   464,   435,     0,   439,     0,   443,   505,   502,   433,     0,
   393,   361,    95,    93,    98,    99,   100,    81,   114,     0,
   112,   118,     0,   116,     0,     0,    66,    87,     0,     0,
   122,   123,     0,   140,   141,   144,   253,   258,     0,   505,
    45,   159,     0,   306,     0,     0,    72,   204,   209,   207,
   191,   194,   219,     0,   220,   505,     0,   451,   471,   482,
   483,     0,   473,   474,   475,   476,   481,     0,   461,   463,
   466,     0,   505,   505,   427,     0,    96,    97,     0,   108,
     0,     0,   110,     0,     0,     0,    89,   132,     0,   130,
   136,     0,   134,     0,     0,     0,   143,   258,   263,   287,
   256,   257,   260,   261,   262,     0,     0,   280,     0,   505,
    48,     0,   157,   158,   161,   304,   309,     0,   505,   217,
     0,   437,   455,   456,   454,   458,   459,   457,     0,   472,
     0,   477,   478,   479,   480,   465,   467,   441,   445,     0,
   394,   113,   111,   117,   115,   107,   109,     0,   126,     0,
     0,   128,     0,     0,     0,   505,   254,     0,   288,     0,
   286,   259,     0,     0,     0,    71,    47,     0,   160,   309,
   314,   338,   307,   308,   311,   312,   313,     0,     0,   331,
     0,    49,   222,   453,   469,   486,     0,   131,   129,   135,
   133,   125,   127,     0,   505,   145,     0,     0,   267,     0,
     0,   297,   270,     0,   290,     0,   270,     0,   284,   279,
   505,   305,     0,   339,   337,   310,     0,     0,     0,    74,
   486,     0,   488,   395,   505,     0,   147,   172,     0,   264,
     0,   358,   300,   298,   296,     0,   272,     0,     0,     0,
   272,     0,   285,   283,     0,   505,   162,     0,     0,   318,
   348,   321,     0,   341,     0,   321,     0,   335,   330,   488,
     0,   485,     0,   490,   149,   505,   172,   198,     0,     0,
   266,   269,     0,     0,   294,   289,   297,   270,   301,     0,
   270,   505,     0,   164,   185,     0,   315,     0,   349,   347,
   351,     0,   323,     0,     0,     0,   323,     0,   336,   334,
   490,   484,     0,     0,     0,   172,   151,   198,   146,   224,
     0,     0,   274,   271,   507,   506,   277,   295,   293,   272,
   275,   272,   166,   505,   185,   211,     0,     0,   317,   320,
     0,     0,   345,   340,   348,   321,   352,     0,   321,     0,
     0,   493,   495,   500,   491,   492,   498,   499,     0,   497,
   489,   468,   198,   172,   148,   248,   268,   505,   273,     0,
     0,   185,   168,   211,   163,   236,     0,     0,   325,   322,
   328,   346,   344,   323,   326,   323,   470,   487,   494,   496,
   150,   198,     0,     0,   223,   153,   265,   278,   276,   211,
   185,   165,   250,   319,     0,   324,     0,     0,   152,     0,
   225,     0,   167,   211,     0,     0,   235,   170,   316,   329,
   327,     0,   230,     0,   169,     0,   237,     0,   227,   234,
   226,     0,   232,   247,     0,   242,     0,   230,   233,   229,
     0,   239,   246,   238,     0,   244,   249,   228,   231,   242,
   245,   241,     0,   240,   243,     0,     0,     0
};

static const short yydefgoto[] = {   866,
     1,    12,    13,    37,    64,    65,    14,    15,    49,    50,
   107,    16,    17,   110,   111,    18,    19,   271,    20,    21,
    22,    23,   461,    24,    25,    53,    54,   188,   265,   266,
   267,   117,   118,   272,   328,   329,   330,   396,   397,   466,
   467,   114,   115,   268,   320,   321,   195,   196,   331,   387,
   388,   322,   323,   191,   192,   324,   382,   383,   440,   441,
   443,   444,   275,   276,   390,   451,   452,   509,   510,   512,
   513,   278,   279,   392,   454,   455,   456,   648,   687,   726,
   774,   729,   339,   340,   462,   533,   534,   535,   705,   745,
   782,   811,   785,    59,    60,    61,   200,   341,   283,   284,
   285,   129,   130,   131,   290,   408,   351,   352,   353,   730,
   125,   126,   287,   404,   346,   347,   348,   786,   210,   211,
   355,   473,   413,   414,   415,   776,   805,   833,   848,   841,
   842,   843,   813,   827,   846,   860,   854,   855,   856,   806,
   828,   336,   337,   518,   458,   521,   522,   523,   524,   578,
   690,   618,   619,   657,   694,   732,   525,   526,   527,   630,
   528,   623,   624,   696,   625,   626,   400,   401,   590,   537,
   593,   594,   595,   596,   633,   748,   669,   670,   713,   752,
   788,   597,   598,   599,   679,   600,   672,   673,   754,   674,
   675,    26,    27,   204,    28,    29,   655,    95,    96,    97,
    98,    99,   100,   101,   102,   103,   104,   105,    30,    31,
    32,    33,   143,   303,   224,   225,   226,    34,   136,   295,
    35,   138,   299,    36,   141,   301,   217,   358,   418,   419,
   420,   549,   298,   421,   489,   490,   491,   606,   641,   492,
   643,   684,   725,   771,   369,    68,   737
};

static const short yypact[] = {-32768,
   289,-32768,    29,    41,   161,   166,   -15,   187,    -2,   196,
   209,-32768,-32768,-32768,    15,-32768,    65,-32768,   236,-32768,
    67,-32768,   236,-32768,   178,-32768,   248,-32768,   186,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   261,   213,   219,   247,
   215,   211,   -29,   -31,   303,   -19,   -26,   313,   291,   318,
   325,   277,   358,-32768,   338,   358,   338,    57,   376,   248,
-32768,   355,-32768,   191,-32768,   372,-32768,-32768,   375,-32768,
   379,-32768,-32768,   299,   411,-32768,-32768,   426,-32768,   429,
   431,   341,-32768,-32768,-32768,   387,   138,   137,   390,   427,
   323,   323,   323,   323,   211,-32768,   714,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   287,   -13,-32768,   313,   440,   413,
   504,-32768,   455,   534,-32768,   458,   537,-32768,   534,   537,
   313,-32768,    60,   518,   376,-32768,-32768,   126,   539,   355,
-32768,-32768,   261,   211,   452,   493,   456,   493,-32768,   460,
   493,   461,   516,-32768,-32768,   222,   242,-32768,   520,-32768,
   523,   524,   462,-32768,-32768,-32768,   586,-32768,   323,   323,
   323,   323,   323,   323,   323,   323,   323,   323,   323,   323,
   323,   323,   323,   323,   323,   323,   323,   323,   323,   525,
   526,   211,-32768,   211,   313,   211,   464,   528,-32768,   496,
   554,-32768,-32768,   505,   575,-32768,   581,   575,   508,   561,
   313,-32768,   211,-32768,-32768,   313,-32768,   128,   500,   539,
-32768,-32768,-32768,   502,-32768,   514,   546,-32768,   546,-32768,
   546,-32768,   -38,   506,   516,-32768,-32768,   362,-32768,   408,
-32768,-32768,   501,-32768,-32768,   380,   380,   380,   380,   380,
   380,   764,   764,   764,   443,   443,   443,   283,   283,   689,
   157,   157,   515,   515,   515,   515,-32768,-32768,-32768,-32768,
   512,-32768,   313,-32768,-32768,   232,-32768,   132,-32768,   119,
-32768,   573,-32768,   527,   513,-32768,   542,   600,-32768,   603,
-32768,-32768,-32768,   234,-32768,   545,   598,-32768,   555,   605,
   313,-32768,   211,-32768,   493,-32768,   557,   548,   493,   549,
   493,   550,   516,-32768,    -8,   211,-32768,   323,-32768,   323,
-32768,-32768,   323,-32768,   558,-32768,   528,   562,   609,-32768,
   237,-32768,-32768,   319,   211,-32768,-32768,-32768,   282,-32768,
   132,-32768,   211,-32768,    21,   636,-32768,   565,   651,-32768,
   561,-32768,   561,-32768,   569,-32768,   286,-32768,-32768,-32768,
-32768,   288,-32768,   584,   637,-32768,   546,   644,-32768,   211,
   546,   211,   546,   211,   585,-32768,-32768,   648,   589,-32768,
   498,   627,   739,-32768,-32768,   181,-32768,-32768,   132,   649,
   650,-32768,   377,-32768,-32768,   573,-32768,   592,-32768,   382,
-32768,   653,   604,-32768,   606,   665,-32768,-32768,    28,   680,
-32768,-32768,-32768,   598,   313,-32768,   598,   605,-32768,   605,
-32768,   596,-32768,   637,-32768,   607,   610,-32768,   644,-32768,
   360,-32768,   614,-32768,   623,-32768,   211,-32768,-32768,   323,
-32768,-32768,   677,   679,-32768,-32768,-32768,-32768,   624,   304,
-32768,   625,   314,-32768,   649,   650,-32768,   132,   690,   691,
-32768,   398,   641,-32768,   653,-32768,-32768,    11,   692,   129,
-32768,   705,   658,-32768,   661,   652,-32768,-32768,-32768,-32768,
-32768,-32768,   637,   313,-32768,   211,   278,-32768,   667,-32768,
-32768,   459,-32768,-32768,-32768,-32768,-32768,   230,-32768,   360,
-32768,   735,   211,   211,-32768,   547,-32768,-32768,   673,-32768,
   649,   695,-32768,   650,   322,   332,-32768,   699,   334,-32768,
   701,   361,-32768,   690,   691,   756,-32768,    11,-32768,    98,
-32768,    11,-32768,-32768,-32768,   457,   715,-32768,   726,   211,
-32768,   737,-32768,   705,-32768,-32768,    34,   788,   211,-32768,
   731,-32768,   429,   431,-32768,-32768,-32768,   287,   744,-32768,
   804,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   323,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   740,-32768,   690,
   741,-32768,   691,   407,   412,    16,-32768,   810,-32768,   313,
   747,-32768,    10,    10,   814,-32768,-32768,   815,-32768,    34,
-32768,   144,-32768,    34,-32768,-32768,-32768,   491,   757,-32768,
   763,-32768,-32768,-32768,-32768,   760,   663,-32768,-32768,-32768,
-32768,-32768,-32768,   825,   -11,-32768,   765,   840,   767,   766,
   313,    -1,   842,   768,-32768,   142,   842,    22,   104,-32768,
    48,-32768,   830,-32768,   769,-32768,   832,   832,   833,-32768,
   760,    40,   818,-32768,   211,   837,-32768,   248,   313,-32768,
   810,-32768,-32768,-32768,   772,   313,   861,   841,   843,   313,
   861,   843,-32768,-32768,   844,    12,-32768,   777,   856,   785,
   152,   859,   781,-32768,    24,   859,    20,   133,-32768,   818,
   783,-32768,   782,   787,-32768,   211,   248,   376,   866,   852,
-32768,-32768,   853,   418,   153,-32768,   201,   842,-32768,   418,
   842,   211,   855,-32768,   355,   313,-32768,   830,-32768,   790,
-32768,   313,   875,   857,   858,   313,   875,   858,-32768,-32768,
   787,-32768,   860,   171,   800,   248,-32768,   376,-32768,-32768,
   313,   147,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   861,
-32768,   861,-32768,   211,   355,   539,   876,   862,-32768,-32768,
   863,   806,   163,-32768,   205,   859,-32768,   807,   859,   808,
   795,   429,   431,-32768,-32768,-32768,-32768,-32768,   482,   287,
-32768,-32768,   376,   248,-32768,   226,-32768,   211,-32768,   418,
   418,   355,-32768,   539,-32768,-32768,   313,   154,-32768,-32768,
-32768,-32768,-32768,   875,-32768,   875,-32768,-32768,-32768,-32768,
-32768,   376,   130,   817,-32768,-32768,-32768,-32768,-32768,   539,
   355,-32768,   228,-32768,   811,-32768,   812,   813,-32768,   313,
-32768,   868,-32768,   539,   135,   822,-32768,-32768,-32768,-32768,
-32768,   823,   877,   816,-32768,   313,-32768,   878,-32768,   819,
-32768,   433,-32768,-32768,   827,   880,   826,   877,-32768,-32768,
   877,-32768,   819,-32768,   442,-32768,-32768,-32768,-32768,   880,
-32768,-32768,   880,-32768,-32768,   910,   915,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   789,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   893,-32768,-32768,-32768,-32768,
   601,   864,-32768,-32768,-32768,-32768,   538,-32768,-32768,-32768,
-32768,   867,-32768,-32768,-32768,-32768,   805,-32768,-32768,-32768,
-32768,   541,  -312,   809,-32768,-32768,-32768,-32768,   481,   428,
   484,   423,   733,-32768,-32768,-32768,-32768,   419,   364,   417,
   363,-32768,-32768,-32768,-32768,-32768,   480,-32768,-32768,-32768,
-32768,  -706,-32768,-32768,-32768,-32768,-32768,   403,-32768,-32768,
-32768,-32768,  -630,  -631,-32768,   879,-32768,-32768,   597,-32768,
   599,  -674,-32768,   820,-32768,-32768,   532,-32768,   531,   884,
-32768,   821,-32768,-32768,   540,-32768,   544,   824,-32768,   738,
-32768,-32768,   472,-32768,   533,-32768,-32768,-32768,-32768,   101,
-32768,   103,-32768,-32768,-32768,-32768,    92,-32768,    93,-32768,
-32768,-32768,-32768,-32768,-32768,   437,-32768,   435,-32768,-32768,
-32768,   307,-32768,  -615,  -651,-32768,  -672,-32768,-32768,-32768,
-32768,  -575,-32768,-32768,-32768,   378,-32768,-32768,-32768,-32768,
   369,-32768,   366,-32768,-32768,-32768,   253,-32768,  -671,  -656,
-32768,  -591,-32768,-32768,-32768,-32768,  -611,-32768,-32768,-32768,
   326,-32768,-32768,-32768,-32768,-32768,  -519,  -108,-32768,   -44,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -475,-32768,-32768,
-32768,-32768,-32768,-32768,   660,-32768,   742,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,  -134,-32768,-32768,-32768,
   551,-32768,  -213,-32768,-32768,-32768,   475,-32768,-32768,   486,
   328,   292,   245,-32768,-32768,   -92,  -680
};


#define	YYLAST		972


static const short yytable[] = {   184,
   581,   548,   158,   219,   717,   300,   221,   302,   627,   700,
   646,   661,   199,   183,  -299,  -299,   688,   733,   389,   741,
   182,   775,    71,   366,    69,   367,   676,    78,   519,  -299,
   746,    44,    42,   703,    75,   393,   718,   614,   662,   715,
   621,   214,   463,   622,   520,    48,   154,   155,   156,   157,
    45,   591,   660,   716,   304,   728,   615,   716,   305,   779,
   758,    72,    38,    73,   654,    70,   801,   592,    79,   665,
   784,   681,   635,    76,    39,    77,   261,   121,   368,    67,
   201,    67,   740,   698,   794,   742,   701,   796,   780,   259,
   781,   260,   286,   262,   773,   819,   580,   289,  -292,   808,
   809,   228,   230,   756,    67,   394,   759,   810,    67,   664,
   288,   666,   464,   580,   236,   237,   238,   239,   240,   241,
   242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
   252,   253,   254,   255,   256,   507,   824,   817,   682,   818,
    67,   122,   802,   416,   202,   580,   206,   423,   291,   425,
   820,   710,   325,   812,   315,   836,   789,   659,   720,    51,
   357,    55,   530,   579,   361,   318,   363,  -350,  -350,   663,
   150,   148,   660,   319,   778,   739,   151,   326,   149,   823,
   520,   815,   354,    80,    81,    82,    83,   592,    84,    85,
    86,    87,    88,   835,    40,   580,   816,  -282,   719,    41,
   356,   580,   762,   763,   764,   765,   766,   767,   768,   634,
   207,    67,   292,   370,   821,    89,    90,   709,   738,   837,
    43,    67,   433,   434,   435,   436,   437,    91,   792,    46,
   580,    92,   384,   793,   803,   710,   825,   804,    93,   826,
   391,   580,    47,  -333,   176,   177,   178,   179,   770,   580,
   580,  -343,    94,    80,    81,    82,    83,   769,    84,    85,
   580,    87,    88,   371,    52,   372,   654,   422,   373,   424,
   709,   426,    57,    80,    81,    82,    83,    58,    84,    85,
    62,    87,    88,   132,   133,    89,    90,   552,   553,   554,
   555,     2,     3,     4,     5,    63,   469,    91,   580,    66,
  -292,    92,   580,    67,  -343,    89,    90,   -44,    93,   543,
   544,     6,   545,   -34,   546,   547,   106,    91,   180,   181,
   849,    92,    94,   227,   316,   317,   342,   343,    93,   378,
   379,   380,   381,   861,   495,     7,    74,     8,     9,    10,
    11,   -50,    94,   229,    80,    81,    82,    83,   108,    84,
    85,    86,    87,    88,    80,    81,    82,    83,   109,    84,
    85,   112,    87,    88,   113,   541,   116,   531,   174,   175,
   176,   177,   178,   179,   385,   386,    89,    90,   406,   407,
   409,   410,   123,   542,   128,   496,    89,    90,    91,   445,
   446,   139,    92,   479,   449,   450,   500,   501,    91,    93,
   558,   559,    92,   480,   481,   134,   503,   504,   135,    93,
   514,   515,   137,    94,   566,   501,   482,   483,   484,   485,
   486,   487,   488,    94,   567,   504,   569,   570,   159,   160,
   161,   162,   163,   164,   165,   166,   167,   587,   168,   169,
   170,   146,   171,   172,   140,   173,   602,   174,   175,   176,
   177,   178,   179,   572,   573,   308,   168,   169,   170,   142,
   171,   172,   144,   309,   145,   174,   175,   176,   177,   178,
   179,   620,   583,   584,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   616,   168,   169,   170,   147,   171,   172,
   152,   173,   479,   174,   175,   176,   177,   178,   179,   612,
   570,   310,   480,   481,   613,   573,   637,   638,   186,   311,
   735,   736,   653,   799,   800,   607,   483,   484,   485,   486,
   487,   488,   647,   171,   172,   850,   851,   153,   174,   175,
   176,   177,   178,   179,   862,   863,   185,   187,   667,   189,
   689,   190,   193,   194,   203,   208,   215,   692,   216,   223,
   218,   699,   685,   231,   220,   222,   232,   233,   257,   258,
   263,   264,   711,   234,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   704,   168,   169,   170,   270,   171,   172,
   269,   173,   274,   174,   175,   176,   177,   178,   179,   273,
   277,   430,   281,   727,   282,   293,  -357,   747,   296,   431,
   297,   306,   312,   750,   314,   179,   327,   757,   333,   743,
   335,   332,   338,   159,   160,   161,   162,   163,   164,   165,
   166,   167,   777,   168,   169,   170,   334,   171,   172,   344,
   173,   345,   174,   175,   176,   177,   178,   179,   350,   349,
   560,   359,   377,   360,   362,   364,   376,   395,   561,   398,
   374,   783,   159,   160,   161,   162,   163,   164,   165,   166,
   167,   399,   168,   169,   170,   405,   171,   172,   411,   173,
   412,   174,   175,   176,   177,   178,   179,   417,   814,   428,
   427,   429,   439,   442,   448,   807,   453,   235,   457,   460,
   459,   465,   474,   159,   160,   161,   162,   163,   164,   165,
   166,   167,   476,   168,   169,   170,   477,   171,   172,   493,
   173,   832,   174,   175,   176,   177,   178,   179,   494,   497,
   498,   499,   502,   508,   511,   516,   529,   845,   432,   159,
   160,   161,   162,   163,   164,   165,   166,   167,   532,   168,
   169,   170,   536,   171,   172,   538,   173,   539,   174,   175,
   176,   177,   178,   179,   550,   159,   160,   161,   162,   163,
   164,   165,   166,   167,   644,   168,   169,   170,   557,   171,
   172,   562,   173,   313,   174,   175,   176,   177,   178,   179,
   159,   160,   161,   162,   163,   164,   165,   166,   167,   576,
   168,   169,   170,   564,   171,   172,   568,   173,   571,   174,
   175,   176,   177,   178,   179,   159,   160,   161,   162,   163,
   164,   165,   166,   167,   585,   168,   169,   170,   586,   171,
   172,   588,   601,   603,   174,   175,   176,   177,   178,   179,
   159,   160,   161,   162,   163,   164,   604,   605,   608,   610,
   168,   169,   170,   617,   171,   172,  -281,   629,   631,   174,
   175,   176,   177,   178,   179,   640,   639,   642,   645,   650,
   651,   649,   656,   668,   652,   671,   678,   658,  -332,   683,
   686,  -291,   693,   706,   695,   707,   697,   702,   708,   712,
   714,   722,   723,   724,   731,   520,   751,   734,   744,  -342,
   753,   755,   772,   761,   787,   592,   798,   790,   791,   795,
   797,   822,   834,   829,   830,   831,   838,   839,   844,   867,
   840,   852,   847,   853,   868,    56,   580,   375,   857,   438,
   120,   213,   119,   447,   198,   505,   565,   197,   563,   506,
   280,   575,   574,   609,   517,   611,   589,   402,   127,   471,
   472,   403,   124,   468,   540,   205,   475,   294,   858,   212,
   470,   864,   209,   859,   577,   865,   582,   691,   632,   636,
   749,   628,   365,   677,   556,   760,   307,   551,   680,   478,
     0,   721
};

static const short yycheck[] = {   108,
   520,   477,    95,   138,   676,   219,   141,   221,   584,   661,
    22,   627,   121,   106,    16,    17,   648,   690,   331,   700,
    34,   728,    54,    32,    54,    34,   638,    54,    18,    31,
   705,    34,    48,    22,    54,    15,    17,    22,    17,    16,
    31,   134,    15,    34,    34,    31,    91,    92,    93,    94,
    53,    18,    31,    34,    93,   687,   576,    34,    97,   732,
   717,    93,    34,    95,    66,    95,   773,    34,    95,    22,
   745,    32,   592,    93,    34,    95,   185,    21,    87,    93,
    21,    93,   698,   659,   756,   701,   662,   759,   740,   182,
   742,   184,   201,   186,   726,   802,    98,   206,   100,   780,
   781,   146,   147,   715,    93,    85,   718,   782,    93,   629,
   203,   631,    85,    98,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
   175,   176,   177,   178,   179,   448,   811,   794,    99,   796,
    93,    85,   774,   357,    85,    98,    21,   361,    21,   363,
    21,   671,    34,   784,   263,    21,   748,    16,   678,    95,
   295,    95,    34,    66,   299,    34,   301,    16,    17,    66,
    34,    34,    31,    42,    28,   695,    40,   270,    41,   810,
    34,    28,   291,    32,    33,    34,    35,    34,    37,    38,
    39,    40,    41,   824,    34,    98,   788,   100,    66,    34,
   293,    98,    32,    33,    34,    35,    36,    37,    38,    66,
    85,    93,    85,   306,    85,    64,    65,    66,    66,    85,
    34,    93,    42,    43,    44,    45,    46,    76,    66,    34,
    98,    80,   325,   753,     9,   755,     9,    12,    87,    12,
   333,    98,    34,   100,    88,    89,    90,    91,   724,    98,
    98,   100,   101,    32,    33,    34,    35,    87,    37,    38,
    98,    40,    41,   308,    29,   310,    66,   360,   313,   362,
    66,   364,    95,    32,    33,    34,    35,    30,    37,    38,
    95,    40,    41,    93,    94,    64,    65,    58,    59,    60,
    61,     3,     4,     5,     6,    35,   405,    76,    98,    85,
   100,    80,    98,    93,   100,    64,    65,    95,    87,    32,
    33,    23,    35,    95,    37,    38,    26,    76,    32,    33,
   840,    80,   101,   102,    93,    94,    93,    94,    87,    93,
    94,    13,    14,   853,   427,    47,    34,    49,    50,    51,
    52,    95,   101,   102,    32,    33,    34,    35,    31,    37,
    38,    39,    40,    41,    32,    33,    34,    35,    34,    37,
    38,    85,    40,    41,     7,   474,    29,   460,    86,    87,
    88,    89,    90,    91,    93,    94,    64,    65,    93,    94,
    93,    94,     7,   476,    30,   430,    64,    65,    76,    13,
    14,    93,    80,    34,    13,    14,    93,    94,    76,    87,
   493,   494,    80,    44,    45,    34,    93,    94,    34,    87,
    13,    14,    34,   101,    93,    94,    57,    58,    59,    60,
    61,    62,    63,   101,    93,    94,    93,    94,    67,    68,
    69,    70,    71,    72,    73,    74,    75,   530,    77,    78,
    79,   101,    81,    82,    34,    84,   539,    86,    87,    88,
    89,    90,    91,    93,    94,    94,    77,    78,    79,    34,
    81,    82,    34,   102,    34,    86,    87,    88,    89,    90,
    91,   580,    16,    17,    67,    68,    69,    70,    71,    72,
    73,    74,    75,   576,    77,    78,    79,   101,    81,    82,
   101,    84,    34,    86,    87,    88,    89,    90,    91,    93,
    94,    94,    44,    45,    93,    94,    16,    17,    96,   102,
    93,    94,   621,    32,    33,   560,    58,    59,    60,    61,
    62,    63,   615,    81,    82,    93,    94,   101,    86,    87,
    88,    89,    90,    91,    93,    94,    97,    34,   631,    85,
   649,     8,    85,     7,    27,     7,    95,   656,    56,    34,
    95,   660,   645,    34,    95,    95,    34,    34,    34,    34,
    97,    34,   671,   102,    67,    68,    69,    70,    71,    72,
    73,    74,    75,   666,    77,    78,    79,    24,    81,    82,
    85,    84,     8,    86,    87,    88,    89,    90,    91,    85,
    10,    94,    85,   686,    34,    96,    95,   706,    85,   102,
    55,    96,   102,   712,    93,    91,    34,   716,    96,   702,
    11,    85,    10,    67,    68,    69,    70,    71,    72,    73,
    74,    75,   731,    77,    78,    79,    85,    81,    82,    85,
    84,    34,    86,    87,    88,    89,    90,    91,    34,    85,
    94,    85,    34,    96,    96,    96,    85,    12,   102,    85,
    93,   744,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    11,    77,    78,    79,    97,    81,    82,    85,    84,
    34,    86,    87,    88,    89,    90,    91,    34,   787,    32,
    96,    93,    34,    34,    93,   778,    34,   102,    85,    25,
    85,    12,    97,    67,    68,    69,    70,    71,    72,    73,
    74,    75,    96,    77,    78,    79,    97,    81,    82,    96,
    84,   820,    86,    87,    88,    89,    90,    91,    96,    43,
    42,    98,    98,    34,    34,    85,    35,   836,   102,    67,
    68,    69,    70,    71,    72,    73,    74,    75,    34,    77,
    78,    79,    85,    81,    82,    85,    84,    96,    86,    87,
    88,    89,    90,    91,    88,    67,    68,    69,    70,    71,
    72,    73,    74,    75,   102,    77,    78,    79,    34,    81,
    82,    99,    84,    85,    86,    87,    88,    89,    90,    91,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    34,
    77,    78,    79,    99,    81,    82,    98,    84,    98,    86,
    87,    88,    89,    90,    91,    67,    68,    69,    70,    71,
    72,    73,    74,    75,   100,    77,    78,    79,    93,    81,
    82,    85,    35,    93,    86,    87,    88,    89,    90,    91,
    67,    68,    69,    70,    71,    72,    93,    34,    99,    99,
    77,    78,    79,    34,    81,    82,   100,    34,    34,    86,
    87,    88,    89,    90,    91,    93,   100,    98,    34,    20,
    94,    97,    21,    34,    99,    34,    34,   100,   100,    52,
    34,   100,    12,    97,    34,    20,    34,    34,    94,    21,
   100,    99,   101,    97,    19,    34,    12,    35,    34,   100,
    34,    34,    93,    34,    19,    34,   102,    35,    93,    93,
    93,    85,    35,    93,    93,    93,    85,    85,    93,     0,
    34,    85,    35,    34,     0,    23,    98,   317,    93,   379,
    57,   133,    56,   386,   120,   445,   504,   119,   501,   446,
   198,   515,   514,   570,   455,   573,   534,   341,    60,   408,
   410,   343,    59,   404,   473,   125,   414,   210,   848,   130,
   407,   860,   129,   851,   518,   863,   522,   651,   590,   594,
   708,   584,   303,   638,   490,   721,   225,   482,   641,   419,
    -1,   680
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
#line 248 "ned.y"
{ if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); ;
    break;}
case 6:
#line 250 "ned.y"
{ if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 7:
#line 252 "ned.y"
{ if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 8:
#line 254 "ned.y"
{ if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); ;
    break;}
case 9:
#line 257 "ned.y"
{ if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); ;
    break;}
case 10:
#line 259 "ned.y"
{ if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 11:
#line 261 "ned.y"
{ if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 12:
#line 263 "ned.y"
{ if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); ;
    break;}
case 16:
#line 270 "ned.y"
{ if (ps.storeSourceCode) ps.enump->setSourceCode(toString(yylsp[0])); ;
    break;}
case 17:
#line 272 "ned.y"
{ if (ps.storeSourceCode) ps.messagep->setSourceCode(toString(yylsp[0])); ;
    break;}
case 18:
#line 274 "ned.y"
{ if (ps.storeSourceCode) ps.classp->setSourceCode(toString(yylsp[0])); ;
    break;}
case 19:
#line 276 "ned.y"
{ if (ps.storeSourceCode) ps.structp->setSourceCode(toString(yylsp[0])); ;
    break;}
case 20:
#line 284 "ned.y"
{
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                ;
    break;}
case 21:
#line 289 "ned.y"
{
                  /* no setTrailingComment(ps.imports,@3); comment already stored with last filename */
                ;
    break;}
case 24:
#line 301 "ned.y"
{
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                ;
    break;}
case 26:
#line 317 "ned.y"
{
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 29:
#line 331 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 30:
#line 337 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 31:
#line 346 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 32:
#line 350 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 33:
#line 362 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 34:
#line 369 "ned.y"
{
                  use_chanattrname_token = 0;
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 37:
#line 384 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 38:
#line 390 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 40:
#line 410 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 41:
#line 420 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 42:
#line 424 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 43:
#line 438 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 44:
#line 445 "ned.y"
{
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 46:
#line 469 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 47:
#line 479 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 48:
#line 483 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 49:
#line 500 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 50:
#line 507 "ned.y"
{
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 53:
#line 525 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 54:
#line 530 "ned.y"
{
                ;
    break;}
case 59:
#line 546 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 62:
#line 563 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 63:
#line 568 "ned.y"
{
                ;
    break;}
case 68:
#line 584 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 71:
#line 601 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 74:
#line 616 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 77:
#line 631 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 78:
#line 636 "ned.y"
{
                ;
    break;}
case 81:
#line 647 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 82:
#line 651 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 85:
#line 666 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 86:
#line 671 "ned.y"
{
                ;
    break;}
case 89:
#line 682 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 90:
#line 686 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 92:
#line 701 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                ;
    break;}
case 93:
#line 705 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                ;
    break;}
case 94:
#line 709 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                ;
    break;}
case 95:
#line 713 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                ;
    break;}
case 96:
#line 717 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 97:
#line 721 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 98:
#line 725 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                ;
    break;}
case 99:
#line 729 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                ;
    break;}
case 100:
#line 733 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                ;
    break;}
case 103:
#line 748 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 104:
#line 753 "ned.y"
{
                ;
    break;}
case 113:
#line 776 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 114:
#line 781 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 117:
#line 794 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 118:
#line 799 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 121:
#line 815 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 122:
#line 820 "ned.y"
{
                ;
    break;}
case 131:
#line 843 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 132:
#line 848 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 135:
#line 861 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 136:
#line 866 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 139:
#line 882 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 140:
#line 887 "ned.y"
{
                ;
    break;}
case 145:
#line 903 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 146:
#line 909 "ned.y"
{
                ;
    break;}
case 147:
#line 912 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 148:
#line 919 "ned.y"
{
                ;
    break;}
case 149:
#line 922 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 150:
#line 928 "ned.y"
{
                ;
    break;}
case 151:
#line 931 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 152:
#line 938 "ned.y"
{
                ;
    break;}
case 156:
#line 958 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 157:
#line 963 "ned.y"
{
                ;
    break;}
case 162:
#line 979 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 163:
#line 985 "ned.y"
{
                ;
    break;}
case 164:
#line 988 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 165:
#line 995 "ned.y"
{
                ;
    break;}
case 166:
#line 998 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 167:
#line 1004 "ned.y"
{
                ;
    break;}
case 168:
#line 1007 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 169:
#line 1014 "ned.y"
{
                ;
    break;}
case 175:
#line 1039 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 176:
#line 1044 "ned.y"
{
                ;
    break;}
case 177:
#line 1047 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 178:
#line 1053 "ned.y"
{
                ;
    break;}
case 183:
#line 1069 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 188:
#line 1090 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 189:
#line 1095 "ned.y"
{
                ;
    break;}
case 190:
#line 1098 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 191:
#line 1104 "ned.y"
{
                ;
    break;}
case 196:
#line 1120 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 201:
#line 1141 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 202:
#line 1146 "ned.y"
{
                ;
    break;}
case 203:
#line 1149 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 204:
#line 1155 "ned.y"
{
                ;
    break;}
case 209:
#line 1172 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                ;
    break;}
case 214:
#line 1194 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 215:
#line 1199 "ned.y"
{
                ;
    break;}
case 216:
#line 1202 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 217:
#line 1208 "ned.y"
{
                ;
    break;}
case 222:
#line 1225 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-3]);
                  addExpression(ps.substparam, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparam,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 225:
#line 1242 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 226:
#line 1247 "ned.y"
{
                ;
    break;}
case 227:
#line 1250 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 228:
#line 1256 "ned.y"
{
                ;
    break;}
case 233:
#line 1272 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 234:
#line 1279 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 237:
#line 1295 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 238:
#line 1300 "ned.y"
{
                ;
    break;}
case 239:
#line 1303 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 240:
#line 1309 "ned.y"
{
                ;
    break;}
case 245:
#line 1325 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 246:
#line 1332 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 247:
#line 1343 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 249:
#line 1354 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 253:
#line 1370 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 254:
#line 1376 "ned.y"
{
                ;
    break;}
case 255:
#line 1379 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 256:
#line 1385 "ned.y"
{
                ;
    break;}
case 263:
#line 1406 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                ;
    break;}
case 264:
#line 1410 "ned.y"
{
                  ps.inLoop=1;
                ;
    break;}
case 265:
#line 1414 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-7],yylsp[-4]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 268:
#line 1428 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 269:
#line 1438 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 271:
#line 1446 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 275:
#line 1459 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 276:
#line 1464 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 277:
#line 1469 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 278:
#line 1475 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 281:
#line 1489 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 282:
#line 1495 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 283:
#line 1503 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 284:
#line 1508 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 285:
#line 1512 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 286:
#line 1520 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 287:
#line 1527 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 288:
#line 1533 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 291:
#line 1548 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 292:
#line 1553 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 293:
#line 1560 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 294:
#line 1565 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 295:
#line 1569 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 296:
#line 1577 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 297:
#line 1582 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 298:
#line 1586 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 299:
#line 1595 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 300:
#line 1600 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 301:
#line 1605 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 304:
#line 1621 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 305:
#line 1627 "ned.y"
{
                ;
    break;}
case 306:
#line 1630 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 307:
#line 1636 "ned.y"
{
                ;
    break;}
case 314:
#line 1657 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                ;
    break;}
case 315:
#line 1661 "ned.y"
{
                  ps.inLoop=1;
                ;
    break;}
case 316:
#line 1665 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-7],yylsp[-4]);
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
#line 1939 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 362:
#line 1941 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 363:
#line 1943 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 364:
#line 1945 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 366:
#line 1951 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 367:
#line 1954 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 368:
#line 1956 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 369:
#line 1958 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 370:
#line 1960 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 371:
#line 1962 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 372:
#line 1964 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 373:
#line 1968 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); ;
    break;}
case 374:
#line 1971 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 375:
#line 1973 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 376:
#line 1975 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 377:
#line 1977 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 378:
#line 1979 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 379:
#line 1981 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 380:
#line 1984 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 381:
#line 1986 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 382:
#line 1988 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 383:
#line 1992 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 384:
#line 1995 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 385:
#line 1997 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 386:
#line 1999 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 387:
#line 2003 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 388:
#line 2005 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 389:
#line 2007 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 390:
#line 2009 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 391:
#line 2012 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 392:
#line 2014 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 393:
#line 2016 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 394:
#line 2018 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 395:
#line 2020 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-9]), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 401:
#line 2033 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 402:
#line 2038 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                ;
    break;}
case 403:
#line 2043 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 404:
#line 2049 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 405:
#line 2055 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 406:
#line 2063 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 407:
#line 2068 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 408:
#line 2070 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 410:
#line 2079 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 411:
#line 2081 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 412:
#line 2083 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 413:
#line 2088 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 414:
#line 2090 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 415:
#line 2092 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 420:
#line 2109 "ned.y"
{
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(yylsp[-1])));
                  setComments(ps.cplusplus,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 421:
#line 2118 "ned.y"
{
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(yylsp[-1]));
                  setComments(ps.structdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 422:
#line 2127 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(yylsp[-1]));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 423:
#line 2134 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(yylsp[-1]));
                  setComments(ps.classdecl,yylsp[-3],yylsp[-2]);
                ;
    break;}
case 424:
#line 2144 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 425:
#line 2151 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 426:
#line 2155 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 427:
#line 2163 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 432:
#line 2180 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-1],yylsp[-1]);
                ;
    break;}
case 433:
#line 2186 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 434:
#line 2196 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 435:
#line 2202 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 436:
#line 2206 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 437:
#line 2213 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 438:
#line 2220 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 439:
#line 2226 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 440:
#line 2230 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 441:
#line 2237 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 442:
#line 2244 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 443:
#line 2250 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 444:
#line 2254 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 445:
#line 2261 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 446:
#line 2268 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 453:
#line 2288 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 460:
#line 2307 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 467:
#line 2327 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                ;
    break;}
case 468:
#line 2333 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 469:
#line 2337 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(true);
                ;
    break;}
case 470:
#line 2344 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 484:
#line 2371 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 485:
#line 2376 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 487:
#line 2384 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 489:
#line 2392 "ned.y"
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
#line 2421 "ned.y"


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

