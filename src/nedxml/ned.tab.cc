
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



#define	YYFINAL		866
#define	YYFLAG		-32768
#define	YYNTBASE	103

#define YYTRANSLATE(x) ((unsigned)(x) <= 338 ? yytranslate[x] : 349)

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
   769,   771,   773,   774,   782,   786,   788,   794,   797,   798,
   801,   802,   805,   807,   814,   823,   830,   839,   843,   845,
   848,   850,   853,   855,   858,   861,   863,   866,   870,   872,
   875,   877,   880,   882,   885,   888,   890,   893,   895,   898,
   902,   904,   905,   906,   912,   913,   918,   920,   921,   924,
   926,   928,   930,   931,   939,   943,   945,   951,   954,   955,
   958,   959,   962,   964,   971,   980,   987,   996,  1000,  1002,
  1005,  1007,  1010,  1012,  1015,  1018,  1020,  1023,  1027,  1029,
  1032,  1034,  1037,  1039,  1042,  1045,  1047,  1050,  1052,  1055,
  1059,  1064,  1070,  1073,  1080,  1086,  1090,  1092,  1094,  1101,
  1106,  1110,  1112,  1114,  1118,  1122,  1126,  1130,  1134,  1138,
  1142,  1145,  1149,  1153,  1157,  1161,  1165,  1169,  1173,  1177,
  1181,  1184,  1188,  1192,  1196,  1199,  1203,  1207,  1213,  1217,
  1222,  1229,  1238,  1249,  1251,  1253,  1255,  1257,  1259,  1261,
  1264,  1268,  1272,  1275,  1277,  1279,  1281,  1283,  1285,  1289,
  1294,  1296,  1298,  1300,  1304,  1308,  1311,  1314,  1318,  1322,
  1326,  1331,  1332,  1340,  1341,  1351,  1353,  1354,  1357,  1359,
  1362,  1367,  1368,  1377,  1378,  1389,  1390,  1399,  1400,  1411,
  1412,  1421,  1422,  1433,  1434,  1439,  1440,  1442,  1443,  1446,
  1448,  1453,  1455,  1457,  1459,  1461,  1463,  1465,  1466,  1471,
  1472,  1474,  1475,  1478,  1480,  1481,  1489,  1490,  1499,  1501,
  1504,  1506,  1508,  1510,  1512,  1515,  1518,  1521,  1524,  1526,
  1528,  1530,  1534,  1537,  1538,  1543,  1544,  1547,  1548,  1550,
  1552,  1554,  1557,  1559,  1562,  1564,  1566,  1568,  1570,  1572,
  1575,  1577,  1579,  1580,  1582
};

static const short yyrhs[] = {   104,
     0,   104,   105,     0,     0,   106,     0,   115,     0,   122,
     0,   127,     0,   296,     0,   110,     0,   119,     0,   124,
     0,   293,     0,   310,     0,   311,     0,   312,     0,   313,
     0,   319,     0,   322,     0,   325,     0,     0,     3,   107,
   108,    93,     0,   108,    94,   109,     0,   109,     0,    35,
     0,   111,   112,   114,     0,     5,    34,     0,     0,   113,
     0,   113,    31,   299,   347,     0,    31,   299,   347,     0,
    26,    34,   347,     0,    26,   347,     0,   116,    95,   117,
    96,   347,     0,     5,    34,     0,   118,     0,     0,   118,
    34,    97,   299,    93,     0,    34,    97,   299,    93,     0,
   120,   129,   145,   157,   121,     0,     4,    34,     0,    24,
    34,   347,     0,    24,   347,     0,   123,    95,   135,   150,
   166,    96,   347,     0,     4,    34,     0,   125,   129,   145,
   157,   175,   245,   141,   126,     0,     6,    34,     0,    25,
    34,   347,     0,    25,   347,     0,   128,    95,   135,   150,
   166,   186,   269,   143,    96,   347,     0,     6,    34,     0,
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
    85,    34,   347,   181,   197,   185,     0,     0,    34,    85,
    34,   298,   347,   182,   197,   185,     0,     0,    34,    85,
    34,    22,    34,   347,   183,   197,   185,     0,     0,    34,
    85,    34,   298,    22,    34,   347,   184,   197,   185,     0,
   213,   229,   243,     0,   187,     0,     0,     0,    10,    85,
   188,   189,     0,   190,     0,     0,   190,   191,     0,   191,
     0,     0,    34,    85,    34,   347,   192,   205,   196,     0,
     0,    34,    85,    34,   298,   347,   193,   205,   196,     0,
     0,    34,    85,    34,    22,    34,   347,   194,   205,   196,
     0,     0,    34,    85,    34,   298,    22,    34,   347,   195,
   205,   196,     0,   221,   236,   244,     0,   198,     0,     0,
   198,   199,     0,   199,     0,     0,    30,    85,   200,   202,
     0,     0,    30,    21,   299,    85,   201,   202,     0,   203,
    93,     0,     0,   203,    94,   204,     0,   204,     0,    34,
     0,   206,     0,     0,   206,   207,     0,   207,     0,     0,
    30,    85,   208,   210,     0,     0,    30,    21,   299,    85,
   209,   210,     0,   211,    93,     0,     0,   211,    94,   212,
     0,   212,     0,    34,     0,   214,     0,     0,   214,   215,
     0,   215,     0,     0,     7,    85,   216,   218,     0,     0,
     7,    21,   299,    85,   217,   218,     0,   219,    93,     0,
     0,   219,    94,   220,     0,   220,     0,    34,    97,   299,
     0,   222,     0,     0,   222,   223,     0,   223,     0,     0,
     7,    85,   224,   226,     0,     0,     7,    21,   299,    85,
   225,   226,     0,   227,     0,     0,   227,   228,     0,   228,
     0,    34,    97,   299,    93,     0,   229,   230,     0,     0,
     0,     9,    85,   231,   233,     0,     0,     9,    21,   299,
    85,   232,   233,     0,   234,    93,     0,     0,   234,    94,
   235,     0,   235,     0,    34,   298,     0,    34,     0,   236,
   237,     0,     0,     0,     9,    85,   238,   240,     0,     0,
     9,    21,   299,    85,   239,   240,     0,   241,    93,     0,
     0,   241,    94,   242,     0,   242,     0,    34,   298,     0,
    34,     0,    12,    85,    35,    93,     0,     0,    12,    85,
    35,    93,     0,     0,   246,     0,     0,     0,    11,    15,
    85,   247,   249,     0,     0,    11,    85,   248,   249,     0,
   250,     0,     0,   250,   251,     0,   251,     0,   252,     0,
   259,     0,     0,    18,   253,   254,    20,   258,    28,   347,
     0,   255,    94,   254,     0,   255,     0,    34,    97,   299,
    19,   299,     0,    21,   299,     0,     0,    12,    35,     0,
     0,   258,   259,     0,   259,     0,   260,    17,   264,   256,
   257,   348,     0,   260,    17,   268,    17,   264,   256,   257,
   348,     0,   260,    16,   264,   256,   257,   348,     0,   260,
    16,   268,    16,   264,   256,   257,   348,     0,   261,   100,
   262,     0,   263,     0,    34,   298,     0,    34,     0,    34,
   298,     0,    34,     0,    34,    66,     0,    34,   298,     0,
    34,     0,    34,    66,     0,   265,   100,   266,     0,   267,
     0,    34,   298,     0,    34,     0,    34,   298,     0,    34,
     0,    34,    66,     0,    34,   298,     0,    34,     0,    34,
    66,     0,    34,     0,    31,   299,     0,   268,    31,   299,
     0,   270,     0,     0,     0,    11,    15,    85,   271,   273,
     0,     0,    11,    85,   272,   273,     0,   274,     0,     0,
   274,   275,     0,   275,     0,   276,     0,   283,     0,     0,
    18,   277,   278,    20,   282,    28,    93,     0,   279,    94,
   278,     0,   279,     0,    34,    97,   299,    19,   299,     0,
    21,   299,     0,     0,    12,    35,     0,     0,   282,   283,
     0,   283,     0,   284,    17,   288,   280,   281,    93,     0,
   284,    17,   292,    17,   288,   280,   281,    93,     0,   284,
    16,   288,   280,   281,    93,     0,   284,    16,   292,    16,
   288,   280,   281,    93,     0,   285,   100,   286,     0,   287,
     0,    34,   298,     0,    34,     0,    34,   298,     0,    34,
     0,    34,    66,     0,    34,   298,     0,    34,     0,    34,
    66,     0,   289,   100,   290,     0,   291,     0,    34,   298,
     0,    34,     0,    34,   298,     0,    34,     0,    34,    66,
     0,    34,   298,     0,    34,     0,    34,    66,     0,    34,
     0,    34,   299,     0,   292,    34,   299,     0,   294,   197,
   213,   295,     0,    23,    34,    85,    34,   347,     0,    27,
   347,     0,   297,    95,   205,   221,    96,   347,     0,    23,
    34,    85,    34,   347,     0,    98,   299,    99,     0,   301,
     0,   300,     0,    39,   101,   301,    94,   301,   102,     0,
    39,   101,   301,   102,     0,    39,   101,   102,     0,    39,
     0,   302,     0,   101,   301,   102,     0,   301,    86,   301,
     0,   301,    87,   301,     0,   301,    88,   301,     0,   301,
    89,   301,     0,   301,    90,   301,     0,   301,    91,   301,
     0,    87,   301,     0,   301,    67,   301,     0,   301,    68,
   301,     0,   301,    69,   301,     0,   301,    70,   301,     0,
   301,    71,   301,     0,   301,    72,   301,     0,   301,    73,
   301,     0,   301,    74,   301,     0,   301,    75,   301,     0,
    76,   301,     0,   301,    77,   301,     0,   301,    78,   301,
     0,   301,    79,   301,     0,    80,   301,     0,   301,    81,
   301,     0,   301,    82,   301,     0,   301,    84,   301,    85,
   301,     0,    34,   101,   102,     0,    34,   101,   301,   102,
     0,    34,   101,   301,    94,   301,   102,     0,    34,   101,
   301,    94,   301,    94,   301,   102,     0,    34,   101,   301,
    94,   301,    94,   301,    94,   301,   102,     0,   303,     0,
   304,     0,   305,     0,   306,     0,   307,     0,    34,     0,
    40,    34,     0,    40,    41,    34,     0,    41,    40,    34,
     0,    41,    34,     0,    35,     0,    37,     0,    38,     0,
   308,     0,    65,     0,    65,   101,   102,     0,    64,   101,
    34,   102,     0,    32,     0,    33,     0,   309,     0,   309,
    32,    34,     0,   309,    33,    34,     0,    32,    34,     0,
    33,    34,     0,    47,    48,   347,     0,    51,    34,    93,
     0,    50,    34,    93,     0,    50,    53,    34,    93,     0,
     0,    52,    34,    95,   314,   316,    96,   347,     0,     0,
    52,    34,    54,    34,    95,   315,   316,    96,   347,     0,
   317,     0,     0,   317,   318,     0,   318,     0,    34,    93,
     0,    34,    97,   346,    93,     0,     0,    49,    34,    95,
   320,   328,   334,    96,   347,     0,     0,    49,    34,    54,
    34,    95,   321,   328,   334,    96,   347,     0,     0,    50,
    34,    95,   323,   328,   334,    96,   347,     0,     0,    50,
    34,    54,    34,    95,   324,   328,   334,    96,   347,     0,
     0,    51,    34,    95,   326,   328,   334,    96,   347,     0,
     0,    51,    34,    54,    34,    95,   327,   328,   334,    96,
   347,     0,     0,    56,    85,   329,   330,     0,     0,   331,
     0,     0,   331,   332,     0,   332,     0,    34,    97,   333,
    93,     0,    35,     0,    32,     0,    33,     0,   309,     0,
    37,     0,    38,     0,     0,    55,    85,   335,   336,     0,
     0,   337,     0,     0,   337,   338,     0,   338,     0,     0,
   341,    34,   339,   342,   343,   344,    93,     0,     0,    57,
   341,    34,   340,   342,   343,   344,    93,     0,    34,     0,
    34,    88,     0,    58,     0,    59,     0,    60,     0,    61,
     0,    63,    58,     0,    63,    59,     0,    63,    60,     0,
    63,    61,     0,    62,     0,    44,     0,    45,     0,    98,
    32,    99,     0,    98,    99,     0,     0,    52,   101,    34,
   102,     0,     0,    97,   345,     0,     0,    35,     0,    36,
     0,    32,     0,    87,    32,     0,    33,     0,    87,    33,
     0,   309,     0,    37,     0,    38,     0,    34,     0,    32,
     0,    87,    32,     0,    34,     0,    93,     0,     0,    94,
     0,    93,     0
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
  1400,  1401,  1405,  1410,  1419,  1420,  1424,  1434,  1438,  1442,
  1446,  1450,  1451,  1455,  1460,  1465,  1471,  1480,  1481,  1485,
  1491,  1499,  1504,  1508,  1516,  1523,  1529,  1539,  1540,  1544,
  1549,  1556,  1561,  1565,  1573,  1578,  1582,  1591,  1596,  1601,
  1612,  1613,  1617,  1624,  1626,  1633,  1638,  1639,  1643,  1644,
  1648,  1649,  1653,  1658,  1667,  1668,  1672,  1682,  1686,  1690,
  1694,  1698,  1699,  1703,  1708,  1713,  1719,  1728,  1729,  1733,
  1739,  1747,  1752,  1756,  1764,  1771,  1777,  1787,  1788,  1792,
  1797,  1804,  1809,  1813,  1821,  1826,  1830,  1839,  1844,  1849,
  1860,  1867,  1876,  1887,  1898,  1911,  1916,  1921,  1932,  1934,
  1936,  1938,  1943,  1944,  1947,  1949,  1951,  1953,  1955,  1957,
  1960,  1964,  1966,  1968,  1970,  1972,  1974,  1977,  1979,  1981,
  1984,  1988,  1990,  1992,  1995,  1998,  2000,  2002,  2005,  2007,
  2009,  2011,  2013,  2018,  2019,  2020,  2021,  2022,  2026,  2031,
  2036,  2042,  2048,  2056,  2061,  2063,  2068,  2072,  2074,  2076,
  2081,  2083,  2085,  2091,  2092,  2093,  2094,  2102,  2111,  2120,
  2127,  2137,  2144,  2148,  2156,  2163,  2164,  2168,  2169,  2173,
  2179,  2189,  2195,  2199,  2206,  2213,  2219,  2223,  2230,  2237,
  2243,  2247,  2254,  2261,  2267,  2267,  2271,  2272,  2276,  2277,
  2281,  2291,  2292,  2293,  2294,  2295,  2296,  2300,  2306,  2306,
  2310,  2311,  2315,  2316,  2320,  2326,  2330,  2337,  2344,  2345,
  2347,  2348,  2349,  2350,  2352,  2353,  2354,  2355,  2357,  2358,
  2359,  2364,  2369,  2373,  2377,  2381,  2385,  2389,  2393,  2394,
  2395,  2396,  2397,  2398,  2399,  2400,  2401,  2402,  2406,  2407,
  2408,  2411,  2411,  2413,  2413
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
"struct_decl","class_decl","enum","@36","@37","opt_enumfields","enumfields",
"enumfield","message","@38","@39","class","@40","@41","struct","@42","@43","opt_propertiesblock",
"@44","opt_properties","properties","property","propertyvalue","opt_fieldsblock",
"@45","opt_fields","fields","field","@46","@47","fielddatatype","opt_fieldvector",
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
   251,   251,   253,   252,   254,   254,   255,   256,   256,   257,
   257,   258,   258,   259,   259,   259,   259,   260,   260,   261,
   261,   262,   262,   262,   263,   263,   263,   264,   264,   265,
   265,   266,   266,   266,   267,   267,   267,   268,   268,   268,
   269,   269,   271,   270,   272,   270,   273,   273,   274,   274,
   275,   275,   277,   276,   278,   278,   279,   280,   280,   281,
   281,   282,   282,   283,   283,   283,   283,   284,   284,   285,
   285,   286,   286,   286,   287,   287,   287,   288,   288,   289,
   289,   290,   290,   290,   291,   291,   291,   292,   292,   292,
   293,   294,   295,   296,   297,   298,   299,   299,   300,   300,
   300,   300,   301,   301,   301,   301,   301,   301,   301,   301,
   301,   301,   301,   301,   301,   301,   301,   301,   301,   301,
   301,   301,   301,   301,   301,   301,   301,   301,   301,   301,
   301,   301,   301,   302,   302,   302,   302,   302,   303,   303,
   303,   303,   303,   304,   305,   305,   306,   307,   307,   307,
   308,   308,   308,   309,   309,   309,   309,   310,   311,   312,
   312,   314,   313,   315,   313,   316,   316,   317,   317,   318,
   318,   320,   319,   321,   319,   323,   322,   324,   322,   326,
   325,   327,   325,   329,   328,   328,   330,   330,   331,   331,
   332,   333,   333,   333,   333,   333,   333,   335,   334,   334,
   336,   336,   337,   337,   339,   338,   340,   338,   341,   341,
   341,   341,   341,   341,   341,   341,   341,   341,   341,   341,
   341,   342,   342,   342,   343,   343,   344,   344,   345,   345,
   345,   345,   345,   345,   345,   345,   345,   345,   346,   346,
   346,   347,   347,   348,   348
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
     1,     1,     0,     7,     3,     1,     5,     2,     0,     2,
     0,     2,     1,     6,     8,     6,     8,     3,     1,     2,
     1,     2,     1,     2,     2,     1,     2,     3,     1,     2,
     1,     2,     1,     2,     2,     1,     2,     1,     2,     3,
     1,     0,     0,     5,     0,     4,     1,     0,     2,     1,
     1,     1,     0,     7,     3,     1,     5,     2,     0,     2,
     0,     2,     1,     6,     8,     6,     8,     3,     1,     2,
     1,     2,     1,     2,     2,     1,     2,     3,     1,     2,
     1,     2,     1,     2,     2,     1,     2,     1,     2,     3,
     4,     5,     2,     6,     5,     3,     1,     1,     6,     4,
     3,     1,     1,     3,     3,     3,     3,     3,     3,     3,
     2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     2,     3,     3,     3,     2,     3,     3,     5,     3,     4,
     6,     8,    10,     1,     1,     1,     1,     1,     1,     2,
     3,     3,     2,     1,     1,     1,     1,     1,     3,     4,
     1,     1,     1,     3,     3,     2,     2,     3,     3,     3,
     4,     0,     7,     0,     9,     1,     0,     2,     1,     2,
     4,     0,     8,     0,    10,     0,     8,     0,    10,     0,
     8,     0,    10,     0,     4,     0,     1,     0,     2,     1,
     4,     1,     1,     1,     1,     1,     1,     0,     4,     0,
     1,     0,     2,     1,     0,     7,     0,     8,     1,     2,
     1,     1,     1,     1,     2,     2,     2,     2,     1,     1,
     1,     3,     2,     0,     4,     0,     2,     0,     1,     1,
     1,     2,     1,     2,     1,     1,     1,     1,     1,     2,
     1,     1,     0,     1,     1
};

static const short yydefact[] = {     3,
     1,    20,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     2,     4,     9,    27,     5,     0,    10,    52,     6,
     0,    11,    52,     7,     0,    12,   172,     8,     0,    13,
    14,    15,    16,    17,    18,    19,     0,    40,    26,    46,
     0,   503,     0,     0,     0,     0,     0,     0,     0,    28,
    36,     0,    76,    51,    61,    76,    61,     0,   198,   171,
   174,   185,    24,     0,    23,     0,   502,   418,     0,   432,
     0,   420,   436,     0,     0,   419,   440,     0,   422,   411,
   412,   399,   404,   405,   406,   362,     0,     0,     0,   408,
     0,     0,     0,     0,   503,   358,   357,   363,   394,   395,
   396,   397,   398,   407,   413,   503,    25,     0,     0,     0,
    35,    53,     0,   102,    75,     0,    84,    60,   102,    84,
     0,   175,     0,     0,   197,   200,   173,     0,   211,   184,
   187,    21,     0,   503,     0,   446,     0,   446,   421,     0,
   446,     0,   427,   416,   417,     0,     0,   400,     0,   403,
     0,     0,     0,   381,   385,   371,     0,    30,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   503,    32,   503,     0,   503,     0,    56,    77,     0,
     0,   101,    62,     0,   120,    83,   138,   120,     0,   180,
     0,   201,   503,   351,   199,     0,   188,     0,     0,   210,
   213,   186,    22,   352,   434,     0,   460,   438,   460,   442,
   460,   424,     0,     0,   426,   429,   389,     0,   361,     0,
   401,   402,     0,   409,   364,   372,   373,   374,   375,   376,
   377,   378,   379,   380,   382,   383,   384,   386,   387,     0,
   365,   366,   367,   368,   369,   370,   414,   415,    31,    29,
     0,    33,     0,    59,    54,     0,    58,    80,   103,   503,
    39,    65,    85,     0,     0,   119,     0,   252,   137,   155,
   177,   183,   176,     0,   182,     0,   206,   353,     0,   193,
     0,   214,   503,   212,   446,   444,     0,     0,   446,     0,
   446,     0,   427,   430,     0,   503,   428,     0,   390,     0,
   360,   410,     0,    38,     0,    55,     0,    92,     0,    78,
     0,    82,    91,   106,   503,    42,    68,    63,     0,    67,
    88,   121,   503,   139,     0,    70,   251,     0,   302,   154,
   180,   179,     0,   203,     0,   202,     0,   208,   190,   196,
   189,     0,   195,     0,   219,   354,   460,   448,   458,   503,
   460,   503,   460,   503,     0,   499,   501,     0,     0,   423,
     0,     0,   388,    37,    57,     0,    94,    79,     0,     0,
     0,   104,   105,    41,    64,     0,    86,     0,    90,   124,
    43,   142,     0,   255,     0,     0,    69,   156,     0,    73,
   301,   178,   181,   206,     0,   205,     0,   193,   192,     0,
   216,     0,   215,   218,   221,     0,     0,   445,   447,   450,
   462,   433,     0,   437,     0,   441,   503,   500,   431,     0,
   391,   359,    95,    93,    98,    99,   100,    81,   114,     0,
   112,   118,     0,   116,     0,     0,    66,    87,     0,     0,
   122,   123,     0,   140,   141,   144,   253,   258,     0,   503,
    45,   159,     0,   305,     0,     0,    72,   204,   209,   207,
   191,   194,   219,     0,   220,   503,     0,   449,   469,   480,
   481,     0,   471,   472,   473,   474,   479,     0,   459,   461,
   464,     0,   503,   503,   425,     0,    96,    97,     0,   108,
     0,     0,   110,     0,     0,     0,    89,   132,     0,   130,
   136,     0,   134,     0,     0,     0,   143,   258,   263,   286,
   256,   257,   260,   261,   262,     0,     0,   279,     0,   503,
    48,     0,   157,   158,   161,   303,   308,     0,   503,   217,
     0,   435,   453,   454,   452,   456,   457,   455,     0,   470,
     0,   475,   476,   477,   478,   463,   465,   439,   443,     0,
   392,   113,   111,   117,   115,   107,   109,     0,   126,     0,
     0,   128,     0,     0,     0,   503,   254,     0,   287,     0,
   285,   259,     0,     0,     0,    71,    47,     0,   160,   308,
   313,   336,   306,   307,   310,   311,   312,     0,     0,   329,
     0,    49,   222,   451,   467,   484,     0,   131,   129,   135,
   133,   125,   127,     0,   503,   145,     0,     0,   266,     0,
     0,   296,   269,     0,   289,     0,   269,     0,   283,   278,
   503,   304,     0,   337,   335,   309,     0,     0,     0,    74,
   484,     0,   486,   393,   503,     0,   147,   172,     0,     0,
     0,   356,   299,   297,   295,     0,   271,     0,     0,     0,
   271,     0,   284,   282,     0,   503,   162,     0,     0,   316,
   346,   319,     0,   339,     0,   319,     0,   333,   328,   486,
     0,   483,     0,   488,   149,   503,   172,   198,     0,     0,
   273,   265,   268,     0,     0,   293,   288,   296,   269,   300,
     0,   269,   503,     0,   164,   185,     0,     0,     0,   347,
   345,   349,     0,   321,     0,     0,     0,   321,     0,   334,
   332,   488,   482,     0,     0,     0,   172,   151,   198,   146,
   224,     0,   503,   272,   270,   505,   504,   276,   294,   292,
   271,   274,   271,   166,   503,   185,   211,     0,     0,   323,
   315,   318,     0,     0,   343,   338,   346,   319,   350,     0,
   319,     0,     0,   491,   493,   498,   489,   490,   496,   497,
     0,   495,   487,   466,   198,   172,   148,   248,   267,   264,
     0,     0,   185,   168,   211,   163,   236,     0,     0,   322,
   320,   326,   344,   342,   321,   324,   321,   468,   485,   492,
   494,   150,   198,     0,     0,   223,   153,   277,   275,   211,
   185,   165,   250,   317,   314,     0,     0,   152,     0,   225,
     0,   167,   211,     0,     0,   235,   170,   327,   325,     0,
   230,     0,   169,     0,   237,     0,   227,   234,   226,     0,
   232,   247,     0,   242,     0,   230,   233,   229,     0,   239,
   246,   238,     0,   244,   249,   228,   231,   242,   245,   241,
     0,   240,   243,     0,     0,     0
};

static const short yydefgoto[] = {   864,
     1,    12,    13,    37,    64,    65,    14,    15,    49,    50,
   107,    16,    17,   110,   111,    18,    19,   271,    20,    21,
    22,    23,   461,    24,    25,    53,    54,   188,   265,   266,
   267,   117,   118,   272,   328,   329,   330,   396,   397,   466,
   467,   114,   115,   268,   320,   321,   195,   196,   331,   387,
   388,   322,   323,   191,   192,   324,   382,   383,   440,   441,
   443,   444,   275,   276,   390,   451,   452,   509,   510,   512,
   513,   278,   279,   392,   454,   455,   456,   648,   687,   727,
   776,   730,   339,   340,   462,   533,   534,   535,   706,   746,
   783,   811,   786,    59,    60,    61,   200,   341,   283,   284,
   285,   129,   130,   131,   290,   408,   351,   352,   353,   731,
   125,   126,   287,   404,   346,   347,   348,   787,   210,   211,
   355,   473,   413,   414,   415,   778,   806,   831,   846,   839,
   840,   841,   813,   826,   844,   858,   852,   853,   854,   807,
   827,   336,   337,   518,   458,   521,   522,   523,   524,   578,
   618,   619,   657,   695,   690,   525,   526,   527,   630,   528,
   623,   624,   697,   625,   626,   400,   401,   590,   537,   593,
   594,   595,   596,   633,   669,   670,   714,   754,   749,   597,
   598,   599,   679,   600,   672,   673,   756,   674,   675,    26,
    27,   204,    28,    29,   655,    95,    96,    97,    98,    99,
   100,   101,   102,   103,   104,   105,    30,    31,    32,    33,
   143,   303,   224,   225,   226,    34,   136,   295,    35,   138,
   299,    36,   141,   301,   217,   358,   418,   419,   420,   549,
   298,   421,   489,   490,   491,   606,   641,   492,   643,   684,
   726,   773,   369,    68,   738
};

static const short yypact[] = {-32768,
   226,-32768,    31,   134,   142,   161,   159,   194,     2,   201,
   210,-32768,-32768,-32768,   232,-32768,   170,-32768,   238,-32768,
   197,-32768,   238,-32768,   217,-32768,   259,-32768,   233,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   261,   239,   250,   256,
   222,   221,   -24,   -31,   319,   -27,   -19,   309,   335,   336,
   363,   315,   402,-32768,   386,   402,   386,    -4,   411,   259,
-32768,   394,-32768,   125,-32768,   395,-32768,-32768,   420,-32768,
   424,-32768,-32768,   372,   437,-32768,-32768,   445,-32768,   447,
   448,   361,-32768,-32768,-32768,   393,   181,   163,   397,   400,
   379,   379,   379,   379,   221,-32768,   739,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   298,   -13,-32768,   309,   406,   387,
   476,-32768,   426,   506,-32768,   430,   510,-32768,   506,   510,
   309,-32768,    10,   491,   411,-32768,-32768,    57,   512,   394,
-32768,-32768,   261,   221,   427,   465,   429,   465,-32768,   431,
   465,   432,   494,-32768,-32768,   219,   253,-32768,   495,-32768,
   496,   497,   423,-32768,-32768,-32768,   626,-32768,   379,   379,
   379,   379,   379,   379,   379,   379,   379,   379,   379,   379,
   379,   379,   379,   379,   379,   379,   379,   379,   379,   499,
   500,   221,-32768,   221,   309,   221,   435,   501,-32768,   452,
   514,-32768,-32768,   455,   528,-32768,   532,   528,   458,   511,
   309,-32768,   221,-32768,-32768,   309,-32768,    68,   450,   512,
-32768,-32768,-32768,   449,-32768,   462,   502,-32768,   502,-32768,
   502,-32768,   165,   453,   494,-32768,-32768,   418,-32768,   498,
-32768,-32768,   454,-32768,-32768,   344,   344,   344,   344,   344,
   344,   789,   789,   789,   317,   317,   317,   214,   214,   714,
   191,   191,   459,   459,   459,   459,-32768,-32768,-32768,-32768,
   461,-32768,   309,-32768,-32768,   243,-32768,    11,-32768,   113,
-32768,   517,-32768,   470,   463,-32768,   473,   549,-32768,   551,
-32768,-32768,-32768,   245,-32768,   477,   530,-32768,   493,   547,
   309,-32768,   221,-32768,   465,-32768,   505,   487,   465,   507,
   465,   508,   494,-32768,   -12,   221,-32768,   379,-32768,   379,
-32768,-32768,   379,-32768,   503,-32768,   501,   513,   557,-32768,
   271,-32768,-32768,   369,   221,-32768,-32768,-32768,   294,-32768,
    11,-32768,   221,-32768,    22,   581,-32768,   516,   584,-32768,
   511,-32768,   511,-32768,   509,-32768,   297,-32768,-32768,-32768,
-32768,   299,-32768,   522,   563,-32768,   502,   568,-32768,   221,
   502,   221,   502,   221,   518,-32768,-32768,   576,   519,-32768,
   548,   662,   764,-32768,-32768,   314,-32768,-32768,    11,   579,
   594,-32768,   381,-32768,-32768,   517,-32768,   538,-32768,   414,
-32768,   599,   525,-32768,   555,   618,-32768,-32768,    54,   632,
-32768,-32768,-32768,   530,   309,-32768,   530,   547,-32768,   547,
-32768,   550,-32768,   563,-32768,   552,   554,-32768,   568,-32768,
   318,-32768,   553,-32768,   556,-32768,   221,-32768,-32768,   379,
-32768,-32768,   602,   604,-32768,-32768,-32768,-32768,   565,   343,
-32768,   569,   346,-32768,   579,   594,-32768,    11,   636,   638,
-32768,   428,   597,-32768,   599,-32768,-32768,    20,   644,   115,
-32768,   649,   600,-32768,   601,   588,-32768,-32768,-32768,-32768,
-32768,-32768,   563,   309,-32768,   221,   278,-32768,   603,-32768,
-32768,   264,-32768,-32768,-32768,-32768,-32768,   311,-32768,   318,
-32768,   653,   221,   221,-32768,   587,-32768,-32768,   589,-32768,
   579,   591,-32768,   594,   352,   355,-32768,   608,   357,-32768,
   611,   359,-32768,   636,   638,   658,-32768,    20,-32768,   -37,
-32768,    20,-32768,-32768,-32768,   440,   619,-32768,   609,   221,
-32768,   633,-32768,   649,-32768,-32768,    25,   685,   221,-32768,
   628,-32768,   447,   448,-32768,-32768,-32768,   298,   629,-32768,
   689,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   379,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   625,-32768,   636,
   639,-32768,   638,   367,   370,    45,-32768,   691,-32768,   309,
   627,-32768,   212,   212,   708,-32768,-32768,   711,-32768,    25,
-32768,    92,-32768,    25,-32768,-32768,-32768,   451,   647,-32768,
   661,-32768,-32768,-32768,-32768,   670,   688,-32768,-32768,-32768,
-32768,-32768,-32768,   737,   -11,-32768,   676,   760,   700,   698,
   309,     8,   794,   719,-32768,   186,   794,   209,   -38,-32768,
    48,-32768,   788,-32768,   724,-32768,   806,   806,   810,-32768,
   670,   -16,   795,-32768,   221,   814,-32768,   259,   309,   815,
   691,-32768,-32768,-32768,   762,   309,   851,   830,   831,   309,
   851,   831,-32768,-32768,   835,    12,-32768,   775,   853,   780,
   147,   860,   782,-32768,    24,   860,   182,   123,-32768,   795,
   784,-32768,   783,   790,-32768,   221,   259,   411,   866,    18,
-32768,-32768,-32768,   854,   376,   127,-32768,    96,   794,-32768,
   376,   794,   221,   852,-32768,   394,   309,   856,   788,-32768,
   791,-32768,   309,   876,   858,   859,   309,   876,   859,-32768,
-32768,   790,-32768,   861,   137,   801,   259,-32768,   411,-32768,
-32768,   309,   221,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   851,-32768,   851,-32768,   221,   394,   512,   877,   227,-32768,
-32768,-32768,   862,   805,   144,-32768,   139,   860,-32768,   807,
   860,   808,   797,   447,   448,-32768,-32768,-32768,-32768,-32768,
   441,   298,-32768,-32768,   411,   259,-32768,    32,-32768,-32768,
   376,   376,   394,-32768,   512,-32768,-32768,   309,   809,-32768,
-32768,-32768,-32768,-32768,   876,-32768,   876,-32768,-32768,-32768,
-32768,-32768,   411,   124,   818,-32768,-32768,-32768,-32768,   512,
   394,-32768,   262,-32768,-32768,   811,   812,-32768,   309,-32768,
   871,-32768,   512,   135,   822,-32768,-32768,-32768,-32768,   823,
   875,   817,-32768,   309,-32768,   878,-32768,   813,-32768,   382,
-32768,-32768,   827,   880,   824,   875,-32768,-32768,   875,-32768,
   813,-32768,   384,-32768,-32768,-32768,-32768,   880,-32768,-32768,
   880,-32768,-32768,   915,   916,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   785,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   896,-32768,-32768,-32768,-32768,
   605,   863,-32768,-32768,-32768,-32768,   535,-32768,-32768,-32768,
-32768,   867,-32768,-32768,-32768,-32768,   804,-32768,-32768,-32768,
-32768,   546,  -312,   816,-32768,-32768,-32768,-32768,   481,   433,
   482,   425,   729,-32768,-32768,-32768,-32768,   416,   362,   421,
   358,-32768,-32768,-32768,-32768,-32768,   478,-32768,-32768,-32768,
-32768,  -719,-32768,-32768,-32768,-32768,-32768,   403,-32768,-32768,
-32768,-32768,  -619,  -636,-32768,   879,-32768,-32768,   606,-32768,
   595,  -674,-32768,   819,-32768,-32768,   533,-32768,   534,   881,
-32768,   820,-32768,-32768,   539,-32768,   541,   821,-32768,   732,
-32768,-32768,   479,-32768,   537,-32768,-32768,-32768,-32768,   100,
-32768,   104,-32768,-32768,-32768,-32768,    97,-32768,    93,-32768,
-32768,-32768,-32768,-32768,-32768,   438,-32768,   436,-32768,-32768,
   306,-32768,  -548,  -656,-32768,  -490,-32768,-32768,-32768,-32768,
  -558,-32768,-32768,-32768,   375,-32768,-32768,-32768,-32768,   371,
-32768,   366,-32768,-32768,   254,-32768,  -661,  -709,-32768,  -551,
-32768,-32768,-32768,-32768,  -620,-32768,-32768,-32768,   324,-32768,
-32768,-32768,-32768,-32768,  -519,  -108,-32768,   -44,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,  -475,-32768,-32768,-32768,-32768,
-32768,-32768,   663,-32768,   740,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,  -134,-32768,-32768,-32768,   545,-32768,
  -213,-32768,-32768,-32768,   480,-32768,-32768,   485,   327,   289,
   249,-32768,-32768,   -92,  -668
};


#define	YYLAST		971


static const short yytable[] = {   184,
   581,   548,   158,   219,   701,   300,   221,   302,   760,   777,
   646,   688,   199,   183,   718,   681,   121,   676,   389,   366,
   182,   367,    71,  -298,  -298,   627,    75,   663,   579,    69,
   201,   747,   742,   704,    78,    44,   393,   519,  -298,   716,
   804,   214,   591,   805,   318,   733,   154,   155,   156,   157,
   729,   520,   319,   520,    45,   802,   615,   717,   592,   580,
   580,    72,  -281,    73,    38,    76,   614,    77,   463,   665,
    70,   785,   635,   654,   368,    79,   261,   206,   661,    67,
   122,    67,   682,   818,   781,   816,   782,   817,   291,   259,
   775,   260,   286,   262,   202,   758,   795,   289,   761,   797,
   699,   228,   230,   702,    67,   580,   394,  -291,   810,   664,
   288,   666,   808,   809,   236,   237,   238,   239,   240,   241,
   242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
   252,   253,   254,   255,   256,   507,   823,    67,   464,   803,
    67,   207,   580,   416,   819,   580,   325,   423,   530,   425,
   741,   711,   292,   743,   315,   834,   750,   634,   721,   691,
   357,   654,  -348,  -348,   361,   812,   363,    39,   764,   765,
   766,   767,   768,   769,   770,    40,   740,   326,    80,    81,
    82,    83,   354,    84,    85,    86,    87,    88,   720,   580,
   822,  -331,   739,   580,    41,  -291,   150,   790,   719,   734,
   356,   659,   151,   833,   710,    67,    42,    67,   820,   793,
    89,    90,   710,   370,   148,   717,   660,   132,   133,   835,
   580,   149,    91,   771,   580,   662,    92,    43,     2,     3,
     4,     5,   384,    93,    46,   794,   580,   711,  -341,   660,
   391,   580,   621,    47,   580,   622,  -341,    94,     6,   772,
    80,    81,    82,    83,   789,    84,    85,   304,    87,    88,
   592,   305,    48,   371,    51,   372,    52,   422,   373,   424,
   824,   426,     7,   825,     8,     9,    10,    11,   176,   177,
   178,   179,    89,    90,    80,    81,    82,    83,    58,    84,
    85,    55,    87,    88,    91,    63,   469,   479,    92,   174,
   175,   176,   177,   178,   179,    93,    66,   480,   481,   543,
   544,    57,   545,    67,   546,   547,    89,    90,   847,    94,
   227,   483,   484,   485,   486,   487,   488,    62,    91,   180,
   181,   859,    92,   -44,   495,   316,   317,   342,   343,    93,
    80,    81,    82,    83,   -34,    84,    85,    86,    87,    88,
   -50,   479,    74,    94,   229,   433,   434,   435,   436,   437,
   106,   480,   481,   378,   379,   541,   108,   531,   552,   553,
   554,   555,    89,    90,   482,   483,   484,   485,   486,   487,
   488,   380,   381,   542,    91,   496,   385,   386,    92,   406,
   407,   409,   410,   445,   446,    93,   109,   171,   172,   112,
   558,   559,   174,   175,   176,   177,   178,   179,   113,    94,
    80,    81,    82,    83,   116,    84,    85,   123,    87,    88,
   168,   169,   170,   128,   171,   172,   449,   450,   134,   174,
   175,   176,   177,   178,   179,   500,   501,   587,   503,   504,
   514,   515,    89,    90,   566,   501,   602,   567,   504,   569,
   570,   572,   573,   135,    91,   583,   584,   137,    92,   612,
   570,   146,   613,   573,   139,    93,   637,   638,   736,   737,
   140,   620,   800,   801,   848,   849,   860,   861,   142,    94,
   144,   145,   186,   616,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   147,   168,   169,   170,   152,   171,   172,
   153,   173,   185,   174,   175,   176,   177,   178,   179,   187,
   189,   308,   653,   190,   193,   607,   194,   203,   208,   309,
   216,   215,   647,   218,   234,   220,   222,   223,   231,   232,
   233,   263,   257,   258,   264,   274,   269,   270,   667,   273,
   689,   277,   281,  -355,   282,   293,   296,   693,   306,   179,
   327,   700,   685,   314,   332,   312,   297,   334,   333,   335,
   338,   344,   712,   345,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   705,   168,   169,   170,   349,   171,   172,
   350,   173,   360,   174,   175,   176,   177,   178,   179,   359,
   377,   310,   395,   728,   399,   374,   412,   376,   748,   311,
   398,   417,   362,   364,   752,   405,   411,   428,   759,   457,
   744,   429,   439,   427,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   779,   168,   169,   170,   442,   171,   172,
   448,   173,   453,   174,   175,   176,   177,   178,   179,   459,
   780,   430,   460,   465,   497,   498,   474,   476,   493,   431,
   477,   494,   784,   159,   160,   161,   162,   163,   164,   165,
   166,   167,   499,   168,   169,   170,   502,   171,   172,   508,
   173,   511,   174,   175,   176,   177,   178,   179,   529,   814,
   560,   516,   532,   539,   536,   538,   557,   562,   561,   564,
   550,   576,   159,   160,   161,   162,   163,   164,   165,   166,
   167,   586,   168,   169,   170,   568,   171,   172,   571,   173,
   830,   174,   175,   176,   177,   178,   179,   588,   585,   601,
   603,   604,   605,   608,   617,   843,  -280,   235,   159,   160,
   161,   162,   163,   164,   165,   166,   167,   610,   168,   169,
   170,   629,   171,   172,   631,   173,   639,   174,   175,   176,
   177,   178,   179,   640,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   432,   168,   169,   170,   642,   171,   172,
   645,   173,   649,   174,   175,   176,   177,   178,   179,   650,
   159,   160,   161,   162,   163,   164,   165,   166,   167,   644,
   168,   169,   170,   651,   171,   172,   652,   173,   313,   174,
   175,   176,   177,   178,   179,   159,   160,   161,   162,   163,
   164,   165,   166,   167,   656,   168,   169,   170,   658,   171,
   172,   668,   173,  -330,   174,   175,   176,   177,   178,   179,
   159,   160,   161,   162,   163,   164,   165,   166,   167,   671,
   168,   169,   170,   678,   171,   172,   683,   686,   520,   174,
   175,   176,   177,   178,   179,   159,   160,   161,   162,   163,
   164,  -290,   694,   696,   698,   168,   169,   170,   703,   171,
   172,   707,   708,   709,   174,   175,   176,   177,   178,   179,
   713,   715,   723,   724,   732,   745,   725,   753,   735,   592,
  -340,   755,   757,   774,   763,   788,   791,   792,   799,   796,
   798,   815,   821,   828,   829,   832,   836,   837,   838,   842,
   580,   850,   845,   851,   865,   866,   855,   213,    56,   120,
   447,   375,   119,   198,   438,   505,   280,   506,   565,   574,
   611,   609,   517,   563,   197,   575,   589,   403,   127,   124,
   471,   294,   468,   472,   205,   856,   402,   470,   212,   209,
   475,   540,   857,   863,   862,   577,   692,   582,   628,   636,
   632,   677,   751,   478,   307,   365,   551,   680,   722,   556,
   762
};

static const short yycheck[] = {   108,
   520,   477,    95,   138,   661,   219,   141,   221,   718,   729,
    22,   648,   121,   106,   676,    32,    21,   638,   331,    32,
    34,    34,    54,    16,    17,   584,    54,    66,    66,    54,
    21,   706,   701,    22,    54,    34,    15,    18,    31,    16,
     9,   134,    18,    12,    34,    28,    91,    92,    93,    94,
   687,    34,    42,    34,    53,   775,   576,    34,    34,    98,
    98,    93,   100,    95,    34,    93,    22,    95,    15,    22,
    95,   746,   592,    66,    87,    95,   185,    21,   627,    93,
    85,    93,    99,   803,   741,   795,   743,   797,    21,   182,
   727,   184,   201,   186,    85,   716,   758,   206,   719,   761,
   659,   146,   147,   662,    93,    98,    85,   100,   783,   629,
   203,   631,   781,   782,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
   175,   176,   177,   178,   179,   448,   811,    93,    85,   776,
    93,    85,    98,   357,    21,    98,    34,   361,    34,   363,
   699,   671,    85,   702,   263,    21,   708,    66,   678,   650,
   295,    66,    16,    17,   299,   785,   301,    34,    32,    33,
    34,    35,    36,    37,    38,    34,   696,   270,    32,    33,
    34,    35,   291,    37,    38,    39,    40,    41,    66,    98,
   810,   100,    66,    98,    34,   100,    34,   749,    17,   690,
   293,    16,    40,   823,    66,    93,    48,    93,    85,    66,
    64,    65,    66,   306,    34,    34,    31,    93,    94,    85,
    98,    41,    76,    87,    98,    17,    80,    34,     3,     4,
     5,     6,   325,    87,    34,   755,    98,   757,   100,    31,
   333,    98,    31,    34,    98,    34,   100,   101,    23,   725,
    32,    33,    34,    35,    28,    37,    38,    93,    40,    41,
    34,    97,    31,   308,    95,   310,    29,   360,   313,   362,
     9,   364,    47,    12,    49,    50,    51,    52,    88,    89,
    90,    91,    64,    65,    32,    33,    34,    35,    30,    37,
    38,    95,    40,    41,    76,    35,   405,    34,    80,    86,
    87,    88,    89,    90,    91,    87,    85,    44,    45,    32,
    33,    95,    35,    93,    37,    38,    64,    65,   838,   101,
   102,    58,    59,    60,    61,    62,    63,    95,    76,    32,
    33,   851,    80,    95,   427,    93,    94,    93,    94,    87,
    32,    33,    34,    35,    95,    37,    38,    39,    40,    41,
    95,    34,    34,   101,   102,    42,    43,    44,    45,    46,
    26,    44,    45,    93,    94,   474,    31,   460,    58,    59,
    60,    61,    64,    65,    57,    58,    59,    60,    61,    62,
    63,    13,    14,   476,    76,   430,    93,    94,    80,    93,
    94,    93,    94,    13,    14,    87,    34,    81,    82,    85,
   493,   494,    86,    87,    88,    89,    90,    91,     7,   101,
    32,    33,    34,    35,    29,    37,    38,     7,    40,    41,
    77,    78,    79,    30,    81,    82,    13,    14,    34,    86,
    87,    88,    89,    90,    91,    93,    94,   530,    93,    94,
    13,    14,    64,    65,    93,    94,   539,    93,    94,    93,
    94,    93,    94,    34,    76,    16,    17,    34,    80,    93,
    94,   101,    93,    94,    93,    87,    16,    17,    93,    94,
    34,   580,    32,    33,    93,    94,    93,    94,    34,   101,
    34,    34,    96,   576,    67,    68,    69,    70,    71,    72,
    73,    74,    75,   101,    77,    78,    79,   101,    81,    82,
   101,    84,    97,    86,    87,    88,    89,    90,    91,    34,
    85,    94,   621,     8,    85,   560,     7,    27,     7,   102,
    56,    95,   615,    95,   102,    95,    95,    34,    34,    34,
    34,    97,    34,    34,    34,     8,    85,    24,   631,    85,
   649,    10,    85,    95,    34,    96,    85,   656,    96,    91,
    34,   660,   645,    93,    85,   102,    55,    85,    96,    11,
    10,    85,   671,    34,    67,    68,    69,    70,    71,    72,
    73,    74,    75,   666,    77,    78,    79,    85,    81,    82,
    34,    84,    96,    86,    87,    88,    89,    90,    91,    85,
    34,    94,    12,   686,    11,    93,    34,    85,   707,   102,
    85,    34,    96,    96,   713,    97,    85,    32,   717,    85,
   703,    93,    34,    96,    67,    68,    69,    70,    71,    72,
    73,    74,    75,   732,    77,    78,    79,    34,    81,    82,
    93,    84,    34,    86,    87,    88,    89,    90,    91,    85,
   733,    94,    25,    12,    43,    42,    97,    96,    96,   102,
    97,    96,   745,    67,    68,    69,    70,    71,    72,    73,
    74,    75,    98,    77,    78,    79,    98,    81,    82,    34,
    84,    34,    86,    87,    88,    89,    90,    91,    35,   788,
    94,    85,    34,    96,    85,    85,    34,    99,   102,    99,
    88,    34,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    93,    77,    78,    79,    98,    81,    82,    98,    84,
   819,    86,    87,    88,    89,    90,    91,    85,   100,    35,
    93,    93,    34,    99,    34,   834,   100,   102,    67,    68,
    69,    70,    71,    72,    73,    74,    75,    99,    77,    78,
    79,    34,    81,    82,    34,    84,   100,    86,    87,    88,
    89,    90,    91,    93,    67,    68,    69,    70,    71,    72,
    73,    74,    75,   102,    77,    78,    79,    98,    81,    82,
    34,    84,    97,    86,    87,    88,    89,    90,    91,    20,
    67,    68,    69,    70,    71,    72,    73,    74,    75,   102,
    77,    78,    79,    94,    81,    82,    99,    84,    85,    86,
    87,    88,    89,    90,    91,    67,    68,    69,    70,    71,
    72,    73,    74,    75,    21,    77,    78,    79,   100,    81,
    82,    34,    84,   100,    86,    87,    88,    89,    90,    91,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    34,
    77,    78,    79,    34,    81,    82,    52,    34,    34,    86,
    87,    88,    89,    90,    91,    67,    68,    69,    70,    71,
    72,   100,    12,    34,    34,    77,    78,    79,    34,    81,
    82,    97,    20,    94,    86,    87,    88,    89,    90,    91,
    21,   100,    99,   101,    19,    34,    97,    12,    35,    34,
   100,    34,    34,    93,    34,    19,    35,    93,   102,    93,
    93,    93,    85,    93,    93,    35,    85,    85,    34,    93,
    98,    85,    35,    34,     0,     0,    93,   133,    23,    57,
   386,   317,    56,   120,   379,   445,   198,   446,   504,   514,
   573,   570,   455,   501,   119,   515,   534,   343,    60,    59,
   408,   210,   404,   410,   125,   846,   341,   407,   130,   129,
   414,   473,   849,   861,   858,   518,   651,   522,   584,   594,
   590,   638,   709,   419,   225,   303,   482,   641,   680,   490,
   722
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
                  ps.inLoop=1;
                ;
    break;}
case 264:
#line 1411 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 267:
#line 1425 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 268:
#line 1435 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 270:
#line 1443 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 274:
#line 1456 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 275:
#line 1461 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 276:
#line 1466 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 277:
#line 1472 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 280:
#line 1486 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 281:
#line 1492 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 282:
#line 1500 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 283:
#line 1505 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 284:
#line 1509 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 285:
#line 1517 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 286:
#line 1524 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 287:
#line 1530 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 290:
#line 1545 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 291:
#line 1550 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 292:
#line 1557 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 293:
#line 1562 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 294:
#line 1566 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 295:
#line 1574 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 296:
#line 1579 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 297:
#line 1583 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 298:
#line 1592 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 299:
#line 1597 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 300:
#line 1602 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 303:
#line 1618 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 304:
#line 1624 "ned.y"
{
                ;
    break;}
case 305:
#line 1627 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 306:
#line 1633 "ned.y"
{
                ;
    break;}
case 313:
#line 1654 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;
    break;}
case 314:
#line 1659 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 317:
#line 1673 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 318:
#line 1683 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 320:
#line 1691 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 324:
#line 1704 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 325:
#line 1709 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 326:
#line 1714 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 327:
#line 1720 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 330:
#line 1734 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 331:
#line 1740 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 332:
#line 1748 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 333:
#line 1753 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 334:
#line 1757 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 335:
#line 1765 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 336:
#line 1772 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 337:
#line 1778 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 340:
#line 1793 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 341:
#line 1798 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 342:
#line 1805 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 343:
#line 1810 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 344:
#line 1814 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 345:
#line 1822 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 346:
#line 1827 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 347:
#line 1831 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 348:
#line 1840 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 349:
#line 1845 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 350:
#line 1850 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 352:
#line 1868 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 353:
#line 1877 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 354:
#line 1891 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 355:
#line 1899 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 356:
#line 1912 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 357:
#line 1918 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 358:
#line 1922 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 359:
#line 1933 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 360:
#line 1935 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 361:
#line 1937 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 362:
#line 1939 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 364:
#line 1945 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 365:
#line 1948 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 366:
#line 1950 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 367:
#line 1952 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 368:
#line 1954 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 369:
#line 1956 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 370:
#line 1958 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 371:
#line 1962 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); ;
    break;}
case 372:
#line 1965 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 373:
#line 1967 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 374:
#line 1969 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 375:
#line 1971 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 376:
#line 1973 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 377:
#line 1975 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 378:
#line 1978 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 379:
#line 1980 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 380:
#line 1982 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 381:
#line 1986 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 382:
#line 1989 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 383:
#line 1991 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 384:
#line 1993 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 385:
#line 1997 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 386:
#line 1999 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 387:
#line 2001 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 388:
#line 2003 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 389:
#line 2006 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 390:
#line 2008 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 391:
#line 2010 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 392:
#line 2012 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 393:
#line 2014 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-9]), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 399:
#line 2027 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 400:
#line 2032 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                ;
    break;}
case 401:
#line 2037 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 402:
#line 2043 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 403:
#line 2049 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 404:
#line 2057 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 405:
#line 2062 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 406:
#line 2064 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 408:
#line 2073 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 409:
#line 2075 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 410:
#line 2077 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 411:
#line 2082 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 412:
#line 2084 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 413:
#line 2086 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 418:
#line 2103 "ned.y"
{
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(yylsp[-1])));
                  setComments(ps.cplusplus,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 419:
#line 2112 "ned.y"
{
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(yylsp[-1]));
                  setComments(ps.structdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 420:
#line 2121 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(yylsp[-1]));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 421:
#line 2128 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(yylsp[-1]));
                  setComments(ps.classdecl,yylsp[-3],yylsp[-2]);
                ;
    break;}
case 422:
#line 2138 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 423:
#line 2145 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 424:
#line 2149 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 425:
#line 2157 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 430:
#line 2174 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-1],yylsp[-1]);
                ;
    break;}
case 431:
#line 2180 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 432:
#line 2190 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 433:
#line 2196 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 434:
#line 2200 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 435:
#line 2207 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 436:
#line 2214 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 437:
#line 2220 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 438:
#line 2224 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 439:
#line 2231 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 440:
#line 2238 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 441:
#line 2244 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 442:
#line 2248 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 443:
#line 2255 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 444:
#line 2262 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 451:
#line 2282 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 458:
#line 2301 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 465:
#line 2321 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                ;
    break;}
case 466:
#line 2327 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 467:
#line 2331 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(true);
                ;
    break;}
case 468:
#line 2338 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 482:
#line 2365 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 483:
#line 2370 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 485:
#line 2378 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 487:
#line 2386 "ned.y"
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
#line 2415 "ned.y"


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

