
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
#define	EQ	320
#define	NE	321
#define	GT	322
#define	GE	323
#define	LS	324
#define	LE	325
#define	AND	326
#define	OR	327
#define	XOR	328
#define	NOT	329
#define	BIN_AND	330
#define	BIN_OR	331
#define	BIN_XOR	332
#define	BIN_COMPL	333
#define	SHIFT_LEFT	334
#define	SHIFT_RIGHT	335
#define	INVALID_CHAR	336
#define	UMIN	337

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



#define	YYFINAL		854
#define	YYFLAG		-32768
#define	YYNTBASE	102

#define YYTRANSLATE(x) ((unsigned)(x) <= 337 ? yytranslate[x] : 349)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    89,     2,     2,   100,
   101,    87,    85,    93,    86,    99,    88,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    84,    92,     2,
    96,     2,    83,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    97,     2,    98,    90,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    94,     2,    95,     2,     2,     2,     2,     2,
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
    77,    78,    79,    80,    81,    82,    91
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
   847,   850,   852,   855,   857,   860,   862,   866,   868,   871,
   873,   876,   878,   881,   883,   885,   888,   892,   894,   895,
   896,   902,   903,   908,   910,   911,   914,   916,   918,   920,
   921,   922,   931,   935,   937,   943,   946,   947,   950,   951,
   954,   956,   963,   972,   979,   988,   992,   994,   997,   999,
  1002,  1004,  1007,  1009,  1013,  1015,  1018,  1020,  1023,  1025,
  1028,  1030,  1032,  1035,  1039,  1044,  1050,  1058,  1061,  1068,
  1074,  1082,  1086,  1088,  1090,  1097,  1102,  1106,  1108,  1110,
  1114,  1118,  1122,  1126,  1130,  1134,  1138,  1141,  1145,  1149,
  1153,  1157,  1161,  1165,  1169,  1173,  1177,  1180,  1184,  1188,
  1192,  1195,  1199,  1203,  1209,  1213,  1218,  1225,  1234,  1245,
  1247,  1249,  1251,  1253,  1255,  1257,  1260,  1264,  1268,  1271,
  1273,  1275,  1277,  1279,  1281,  1285,  1290,  1292,  1294,  1296,
  1300,  1304,  1307,  1310,  1313,  1317,  1321,  1326,  1327,  1335,
  1336,  1346,  1348,  1349,  1352,  1354,  1357,  1362,  1363,  1372,
  1373,  1384,  1385,  1394,  1395,  1406,  1407,  1416,  1417,  1428,
  1429,  1434,  1435,  1437,  1438,  1441,  1443,  1448,  1450,  1452,
  1454,  1456,  1458,  1460,  1461,  1466,  1467,  1469,  1470,  1473,
  1475,  1476,  1484,  1485,  1494,  1496,  1499,  1501,  1503,  1505,
  1507,  1510,  1513,  1516,  1519,  1521,  1523,  1525,  1529,  1532,
  1533,  1538,  1539,  1542,  1543,  1545,  1547,  1549,  1551,  1553,
  1555,  1557,  1559,  1560,  1562
};

static const short yyrhs[] = {   103,
     0,   103,   104,     0,     0,   105,     0,   114,     0,   121,
     0,   126,     0,   297,     0,   109,     0,   118,     0,   123,
     0,   294,     0,   311,     0,   312,     0,   313,     0,   314,
     0,   320,     0,   323,     0,   326,     0,     0,     3,   106,
   107,    92,     0,   107,    93,   108,     0,   108,     0,    35,
     0,   110,   111,   113,     0,     5,    34,     0,     0,   112,
     0,   112,    31,   300,   347,     0,    31,   300,   347,     0,
    26,    34,   347,     0,    26,   347,     0,   115,    94,   116,
    95,   347,     0,     5,    34,     0,   117,     0,     0,   117,
    34,    96,   300,    92,     0,    34,    96,   300,    92,     0,
   119,   128,   144,   156,   120,     0,     4,    34,     0,    24,
    34,   347,     0,    24,   347,     0,   122,    94,   134,   149,
   165,    95,   347,     0,     4,    34,     0,   124,   128,   144,
   156,   174,   244,   140,   125,     0,     6,    34,     0,    25,
    34,   347,     0,    25,   347,     0,   127,    94,   134,   149,
   165,   185,   269,   142,    95,   347,     0,     6,    34,     0,
   129,     0,     0,     0,    29,    84,   130,   131,     0,   132,
    92,     0,     0,   132,    93,   133,     0,   133,     0,    34,
     0,   135,     0,     0,     0,    29,    84,   136,   137,     0,
   138,    92,     0,     0,   138,    93,   139,     0,   139,     0,
    34,     0,   141,     0,     0,    12,    84,    35,    92,     0,
   143,     0,     0,    12,    84,    35,    92,     0,   145,     0,
     0,     0,     7,    84,   146,   147,     0,   148,    92,     0,
     0,   148,    93,   154,     0,   154,     0,   150,     0,     0,
     0,     7,    84,   151,   152,     0,   153,    92,     0,     0,
   153,    92,   155,     0,   155,     0,   155,     0,    34,     0,
    34,    84,    43,     0,    42,    34,     0,    34,    84,    42,
     0,    34,    84,    42,    43,     0,    34,    84,    43,    42,
     0,    34,    84,    44,     0,    34,    84,    45,     0,    34,
    84,    46,     0,   157,     0,     0,     0,     8,    84,   158,
   159,     0,   160,     0,     0,   160,    13,   161,    92,     0,
    13,   161,    92,     0,   160,    14,   163,    92,     0,    14,
   163,    92,     0,   161,    93,   162,     0,   162,     0,    34,
    97,    98,     0,    34,     0,   163,    93,   164,     0,   164,
     0,    34,    97,    98,     0,    34,     0,   166,     0,     0,
     0,     8,    84,   167,   168,     0,   169,     0,     0,   169,
    13,   170,    92,     0,    13,   170,    92,     0,   169,    14,
   172,    92,     0,    14,   172,    92,     0,   170,    93,   171,
     0,   171,     0,    34,    97,    98,     0,    34,     0,   172,
    93,   173,     0,   173,     0,    34,    97,    98,     0,    34,
     0,   175,     0,     0,     0,    10,    84,   176,   177,     0,
   178,     0,     0,   178,   179,     0,   179,     0,     0,    34,
    84,    34,   347,   180,   196,   184,     0,     0,    34,    84,
    34,   299,   347,   181,   196,   184,     0,     0,    34,    84,
    34,    22,    34,   347,   182,   196,   184,     0,     0,    34,
    84,    34,   299,    22,    34,   347,   183,   196,   184,     0,
   212,   228,   242,     0,   186,     0,     0,     0,    10,    84,
   187,   188,     0,   189,     0,     0,   189,   190,     0,   190,
     0,     0,    34,    84,    34,   347,   191,   204,   195,     0,
     0,    34,    84,    34,   299,   347,   192,   204,   195,     0,
     0,    34,    84,    34,    22,    34,   347,   193,   204,   195,
     0,     0,    34,    84,    34,   299,    22,    34,   347,   194,
   204,   195,     0,   220,   235,   243,     0,   197,     0,     0,
   197,   198,     0,   198,     0,     0,    30,    84,   199,   201,
     0,     0,    30,    21,   300,    84,   200,   201,     0,   202,
    92,     0,     0,   202,    93,   203,     0,   203,     0,    34,
     0,   205,     0,     0,   205,   206,     0,   206,     0,     0,
    30,    84,   207,   209,     0,     0,    30,    21,   300,    84,
   208,   209,     0,   210,    92,     0,     0,   210,    93,   211,
     0,   211,     0,    34,     0,   213,     0,     0,   213,   214,
     0,   214,     0,     0,     7,    84,   215,   217,     0,     0,
     7,    21,   300,    84,   216,   217,     0,   218,    92,     0,
     0,   218,    93,   219,     0,   219,     0,    34,    96,   300,
     0,   221,     0,     0,   221,   222,     0,   222,     0,     0,
     7,    84,   223,   225,     0,     0,     7,    21,   300,    84,
   224,   225,     0,   226,     0,     0,   226,   227,     0,   227,
     0,    34,    96,   300,    92,     0,   228,   229,     0,     0,
     0,     9,    84,   230,   232,     0,     0,     9,    21,   300,
    84,   231,   232,     0,   233,    92,     0,     0,   233,    93,
   234,     0,   234,     0,    34,   299,     0,    34,     0,   235,
   236,     0,     0,     0,     9,    84,   237,   239,     0,     0,
     9,    21,   300,    84,   238,   239,     0,   240,    92,     0,
     0,   240,    93,   241,     0,   241,     0,    34,   299,     0,
    34,     0,    12,    84,    35,    92,     0,     0,    12,    84,
    35,    92,     0,     0,   245,     0,     0,     0,    11,    15,
    84,   246,   248,     0,     0,    11,    84,   247,   248,     0,
   249,     0,     0,   249,   250,     0,   250,     0,   251,     0,
   259,     0,     0,     0,    18,   252,   254,    20,   253,   258,
    28,   347,     0,   255,    93,   254,     0,   255,     0,    34,
    96,   300,    19,   300,     0,    21,   300,     0,     0,    12,
    35,     0,     0,   258,   259,     0,   259,     0,   260,    17,
   264,   256,   257,   348,     0,   260,    17,   268,    17,   264,
   256,   257,   348,     0,   260,    16,   264,   256,   257,   348,
     0,   260,    16,   268,    16,   264,   256,   257,   348,     0,
   261,    99,   262,     0,   263,     0,    34,   299,     0,    34,
     0,    34,   299,     0,    34,     0,    34,   299,     0,    34,
     0,   265,    99,   266,     0,   267,     0,    34,   299,     0,
    34,     0,    34,   299,     0,    34,     0,    34,   299,     0,
    34,     0,    34,     0,    31,   300,     0,   268,    31,   300,
     0,   270,     0,     0,     0,    11,    15,    84,   271,   273,
     0,     0,    11,    84,   272,   273,     0,   274,     0,     0,
   274,   275,     0,   275,     0,   276,     0,   284,     0,     0,
     0,    18,   277,   279,    20,   278,   283,    28,    92,     0,
   280,    93,   279,     0,   280,     0,    34,    96,   300,    19,
   300,     0,    21,   300,     0,     0,    12,    35,     0,     0,
   283,   284,     0,   284,     0,   285,    17,   289,   281,   282,
    92,     0,   285,    17,   293,    17,   289,   281,   282,    92,
     0,   285,    16,   289,   281,   282,    92,     0,   285,    16,
   293,    16,   289,   281,   282,    92,     0,   286,    99,   287,
     0,   288,     0,    34,   299,     0,    34,     0,    34,   299,
     0,    34,     0,    34,   299,     0,    34,     0,   290,    99,
   291,     0,   292,     0,    34,   299,     0,    34,     0,    34,
   299,     0,    34,     0,    34,   299,     0,    34,     0,    34,
     0,    34,   300,     0,   293,    34,   300,     0,   295,   196,
   212,   296,     0,    23,    34,    84,    34,   347,     0,    23,
    34,    84,    34,    22,    34,   347,     0,    27,   347,     0,
   298,    94,   204,   220,    95,   347,     0,    23,    34,    84,
    34,   347,     0,    23,    34,    84,    34,    22,    34,   347,
     0,    97,   300,    98,     0,   302,     0,   301,     0,    39,
   100,   302,    93,   302,   101,     0,    39,   100,   302,   101,
     0,    39,   100,   101,     0,    39,     0,   303,     0,   100,
   302,   101,     0,   302,    85,   302,     0,   302,    86,   302,
     0,   302,    87,   302,     0,   302,    88,   302,     0,   302,
    89,   302,     0,   302,    90,   302,     0,    86,   302,     0,
   302,    66,   302,     0,   302,    67,   302,     0,   302,    68,
   302,     0,   302,    69,   302,     0,   302,    70,   302,     0,
   302,    71,   302,     0,   302,    72,   302,     0,   302,    73,
   302,     0,   302,    74,   302,     0,    75,   302,     0,   302,
    76,   302,     0,   302,    77,   302,     0,   302,    78,   302,
     0,    79,   302,     0,   302,    80,   302,     0,   302,    81,
   302,     0,   302,    83,   302,    84,   302,     0,    34,   100,
   101,     0,    34,   100,   302,   101,     0,    34,   100,   302,
    93,   302,   101,     0,    34,   100,   302,    93,   302,    93,
   302,   101,     0,    34,   100,   302,    93,   302,    93,   302,
    93,   302,   101,     0,   304,     0,   305,     0,   306,     0,
   307,     0,   308,     0,    34,     0,    40,    34,     0,    40,
    41,    34,     0,    41,    40,    34,     0,    41,    34,     0,
    35,     0,    37,     0,    38,     0,   309,     0,    65,     0,
    65,   100,   101,     0,    64,   100,    34,   101,     0,    32,
     0,    33,     0,   310,     0,   310,    32,    34,     0,   310,
    33,    34,     0,    32,    34,     0,    33,    34,     0,    47,
    48,     0,    51,    34,    92,     0,    50,    34,    92,     0,
    50,    53,    34,    92,     0,     0,    52,    34,    94,   315,
   317,    95,    92,     0,     0,    52,    34,    54,    34,    94,
   316,   317,    95,    92,     0,   318,     0,     0,   318,   319,
     0,   319,     0,    34,    92,     0,    34,    96,    32,    92,
     0,     0,    49,    34,    94,   321,   329,   335,    95,    92,
     0,     0,    49,    34,    54,    34,    94,   322,   329,   335,
    95,    92,     0,     0,    50,    34,    94,   324,   329,   335,
    95,    92,     0,     0,    50,    34,    54,    34,    94,   325,
   329,   335,    95,    92,     0,     0,    51,    34,    94,   327,
   329,   335,    95,    92,     0,     0,    51,    34,    54,    34,
    94,   328,   329,   335,    95,    92,     0,     0,    56,    84,
   330,   331,     0,     0,   332,     0,     0,   332,   333,     0,
   333,     0,    34,    96,   334,    92,     0,    35,     0,    32,
     0,    33,     0,   310,     0,    37,     0,    38,     0,     0,
    55,    84,   336,   337,     0,     0,   338,     0,     0,   338,
   339,     0,   339,     0,     0,   342,    34,   340,   343,   344,
   345,    92,     0,     0,    57,   342,    34,   341,   343,   344,
   345,    92,     0,    34,     0,    34,    87,     0,    58,     0,
    59,     0,    60,     0,    61,     0,    63,    58,     0,    63,
    59,     0,    63,    60,     0,    63,    61,     0,    62,     0,
    44,     0,    45,     0,    97,    32,    98,     0,    97,    98,
     0,     0,    52,   100,    34,   101,     0,     0,    96,   346,
     0,     0,    35,     0,    36,     0,    32,     0,    33,     0,
   310,     0,    37,     0,    38,     0,    92,     0,     0,    93,
     0,    92,     0
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
  1488,  1494,  1502,  1507,  1514,  1521,  1530,  1531,  1535,  1540,
  1547,  1552,  1559,  1564,  1572,  1577,  1582,  1593,  1594,  1598,
  1605,  1607,  1614,  1619,  1620,  1624,  1625,  1629,  1630,  1634,
  1638,  1642,  1651,  1652,  1656,  1666,  1670,  1674,  1678,  1682,
  1683,  1687,  1692,  1697,  1703,  1712,  1713,  1717,  1723,  1731,
  1736,  1743,  1750,  1759,  1760,  1764,  1769,  1776,  1781,  1788,
  1793,  1801,  1806,  1811,  1822,  1829,  1835,  1844,  1855,  1866,
  1872,  1885,  1890,  1895,  1906,  1908,  1910,  1912,  1917,  1918,
  1921,  1923,  1925,  1927,  1929,  1931,  1934,  1938,  1940,  1942,
  1944,  1946,  1948,  1951,  1953,  1955,  1958,  1962,  1964,  1966,
  1969,  1972,  1974,  1976,  1979,  1981,  1983,  1985,  1987,  1992,
  1993,  1994,  1995,  1996,  2000,  2005,  2010,  2016,  2022,  2030,
  2035,  2037,  2042,  2046,  2048,  2050,  2055,  2057,  2059,  2065,
  2066,  2067,  2068,  2076,  2085,  2094,  2101,  2111,  2118,  2122,
  2130,  2137,  2138,  2142,  2143,  2147,  2153,  2163,  2169,  2173,
  2180,  2187,  2193,  2197,  2204,  2211,  2217,  2221,  2228,  2235,
  2241,  2241,  2245,  2246,  2250,  2251,  2255,  2265,  2266,  2267,
  2268,  2269,  2270,  2274,  2280,  2280,  2284,  2285,  2289,  2290,
  2294,  2300,  2304,  2311,  2318,  2319,  2321,  2322,  2323,  2324,
  2326,  2327,  2328,  2329,  2331,  2332,  2333,  2338,  2343,  2347,
  2351,  2355,  2359,  2363,  2367,  2368,  2369,  2370,  2371,  2372,
  2373,  2376,  2376,  2378,  2378
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
"EQ","NE","GT","GE","LS","LE","AND","OR","XOR","NOT","BIN_AND","BIN_OR","BIN_XOR",
"BIN_COMPL","SHIFT_LEFT","SHIFT_RIGHT","INVALID_CHAR","'?'","':'","'+'","'-'",
"'*'","'/'","'%'","'^'","UMIN","';'","','","'{'","'}'","'='","'['","']'","'.'",
"'('","')'","networkdescription","somedefinitions","definition","import","@1",
"filenames","filename","channeldefinition_old","channelheader_old","opt_channelattrblock_old",
"channelattrblock_old","endchannel_old","channeldefinition","channelheader",
"opt_channelattrblock","channelattrblock","simpledefinition_old","simpleheader_old",
"endsimple_old","simpledefinition","simpleheader","moduledefinition_old","moduleheader_old",
"endmodule_old","moduledefinition","moduleheader","opt_machineblock_old","machineblock_old",
"@2","opt_machinelist_old","machinelist_old","machine_old","opt_machineblock",
"machineblock","@3","opt_machinelist","machinelist","machine","opt_displayblock_old",
"displayblock_old","opt_displayblock","displayblock","opt_paramblock_old","paramblock_old",
"@4","opt_parameters_old","parameters_old","opt_paramblock","paramblock","@5",
"opt_parameters","parameters","parameter_old","parameter","opt_gateblock_old",
"gateblock_old","@6","opt_gates_old","gates_old","gatesI_old","gateI_old","gatesO_old",
"gateO_old","opt_gateblock","gateblock","@7","opt_gates","gates","gatesI","gateI",
"gatesO","gateO","opt_submodblock_old","submodblock_old","@8","opt_submodules_old",
"submodules_old","submodule_old","@9","@10","@11","@12","submodule_body_old",
"opt_submodblock","submodblock","@13","opt_submodules","submodules","submodule",
"@14","@15","@16","@17","submodule_body","opt_on_blocks_old","on_blocks_old",
"on_block_old","@18","@19","opt_on_list_old","on_list_old","on_mach_old","opt_on_blocks",
"on_blocks","on_block","@20","@21","opt_on_list","on_list","on_mach","opt_substparamblocks_old",
"substparamblocks_old","substparamblock_old","@22","@23","opt_substparameters_old",
"substparameters_old","substparameter_old","opt_substparamblocks","substparamblocks",
"substparamblock","@24","@25","opt_substparameters","substparameters","substparameter",
"opt_gatesizeblocks_old","gatesizeblock_old","@26","@27","opt_gatesizes_old",
"gatesizes_old","gatesize_old","opt_gatesizeblocks","gatesizeblock","@28","@29",
"opt_gatesizes","gatesizes","gatesize","opt_submod_displayblock_old","opt_submod_displayblock",
"opt_connblock_old","connblock_old","@30","@31","opt_connections_old","connections_old",
"connection_old","loopconnection_old","@32","@33","loopvarlist_old","loopvar_old",
"opt_conncondition_old","opt_conn_displaystr_old","notloopconnections_old","notloopconnection_old",
"leftgatespec_old","leftmod_old","leftgate_old","parentleftgate_old","rightgatespec_old",
"rightmod_old","rightgate_old","parentrightgate_old","channeldescr_old","opt_connblock",
"connblock","@34","@35","opt_connections","connections","connection","loopconnection",
"@36","@37","loopvarlist","loopvar","opt_conncondition","opt_conn_displaystr",
"notloopconnections","notloopconnection","leftgatespec","leftmod","leftgate",
"parentleftgate","rightgatespec","rightmod","rightgate","parentrightgate","channeldescr",
"networkdefinition_old","networkheader_old","endnetwork_old","networkdefinition",
"networkheader","vector","expression","inputvalue","expr","simple_expr","parameter_expr",
"string_expr","boolconst_expr","numconst_expr","special_expr","numconst","timeconstant",
"cplusplus","struct_decl","class_decl","enum","@38","@39","opt_enumfields","enumfields",
"enumfield","message","@40","@41","class","@42","@43","struct","@44","@45","opt_propertiesblock",
"@46","opt_properties","properties","property","propertyvalue","opt_fieldsblock",
"@47","opt_fields","fields","field","@48","@49","fielddatatype","opt_fieldvector",
"opt_fieldenum","opt_fieldvalue","fieldvalue","opt_semicolon","comma_or_semicolon", NULL
};
#endif

static const short yyr1[] = {     0,
   102,   103,   103,   104,   104,   104,   104,   104,   104,   104,
   104,   104,   104,   104,   104,   104,   104,   104,   104,   106,
   105,   107,   107,   108,   109,   110,   111,   111,   112,   112,
   113,   113,   114,   115,   116,   116,   117,   117,   118,   119,
   120,   120,   121,   122,   123,   124,   125,   125,   126,   127,
   128,   128,   130,   129,   131,   131,   132,   132,   133,   134,
   134,   136,   135,   137,   137,   138,   138,   139,   140,   140,
   141,   142,   142,   143,   144,   144,   146,   145,   147,   147,
   148,   148,   149,   149,   151,   150,   152,   152,   153,   153,
   154,   155,   155,   155,   155,   155,   155,   155,   155,   155,
   156,   156,   158,   157,   159,   159,   160,   160,   160,   160,
   161,   161,   162,   162,   163,   163,   164,   164,   165,   165,
   167,   166,   168,   168,   169,   169,   169,   169,   170,   170,
   171,   171,   172,   172,   173,   173,   174,   174,   176,   175,
   177,   177,   178,   178,   180,   179,   181,   179,   182,   179,
   183,   179,   184,   185,   185,   187,   186,   188,   188,   189,
   189,   191,   190,   192,   190,   193,   190,   194,   190,   195,
   196,   196,   197,   197,   199,   198,   200,   198,   201,   201,
   202,   202,   203,   204,   204,   205,   205,   207,   206,   208,
   206,   209,   209,   210,   210,   211,   212,   212,   213,   213,
   215,   214,   216,   214,   217,   217,   218,   218,   219,   220,
   220,   221,   221,   223,   222,   224,   222,   225,   225,   226,
   226,   227,   228,   228,   230,   229,   231,   229,   232,   232,
   233,   233,   234,   234,   235,   235,   237,   236,   238,   236,
   239,   239,   240,   240,   241,   241,   242,   242,   243,   243,
   244,   244,   246,   245,   247,   245,   248,   248,   249,   249,
   250,   250,   252,   253,   251,   254,   254,   255,   256,   256,
   257,   257,   258,   258,   259,   259,   259,   259,   260,   260,
   261,   261,   262,   262,   263,   263,   264,   264,   265,   265,
   266,   266,   267,   267,   268,   268,   268,   269,   269,   271,
   270,   272,   270,   273,   273,   274,   274,   275,   275,   277,
   278,   276,   279,   279,   280,   281,   281,   282,   282,   283,
   283,   284,   284,   284,   284,   285,   285,   286,   286,   287,
   287,   288,   288,   289,   289,   290,   290,   291,   291,   292,
   292,   293,   293,   293,   294,   295,   295,   296,   297,   298,
   298,   299,   300,   300,   301,   301,   301,   301,   302,   302,
   302,   302,   302,   302,   302,   302,   302,   302,   302,   302,
   302,   302,   302,   302,   302,   302,   302,   302,   302,   302,
   302,   302,   302,   302,   302,   302,   302,   302,   302,   303,
   303,   303,   303,   303,   304,   304,   304,   304,   304,   305,
   306,   306,   307,   308,   308,   308,   309,   309,   309,   310,
   310,   310,   310,   311,   312,   313,   313,   315,   314,   316,
   314,   317,   317,   318,   318,   319,   319,   321,   320,   322,
   320,   324,   323,   325,   323,   327,   326,   328,   326,   330,
   329,   329,   331,   331,   332,   332,   333,   334,   334,   334,
   334,   334,   334,   336,   335,   335,   337,   337,   338,   338,
   340,   339,   341,   339,   342,   342,   342,   342,   342,   342,
   342,   342,   342,   342,   342,   342,   342,   343,   343,   343,
   344,   344,   345,   345,   346,   346,   346,   346,   346,   346,
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
     1,     1,     0,     0,     8,     3,     1,     5,     2,     0,
     2,     0,     2,     1,     6,     8,     6,     8,     3,     1,
     2,     1,     2,     1,     2,     1,     3,     1,     2,     1,
     2,     1,     2,     1,     1,     2,     3,     1,     0,     0,
     5,     0,     4,     1,     0,     2,     1,     1,     1,     0,
     0,     8,     3,     1,     5,     2,     0,     2,     0,     2,
     1,     6,     8,     6,     8,     3,     1,     2,     1,     2,
     1,     2,     1,     3,     1,     2,     1,     2,     1,     2,
     1,     1,     2,     3,     4,     5,     7,     2,     6,     5,
     7,     3,     1,     1,     6,     4,     3,     1,     1,     3,
     3,     3,     3,     3,     3,     3,     2,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     2,     3,     3,     3,
     2,     3,     3,     5,     3,     4,     6,     8,    10,     1,
     1,     1,     1,     1,     1,     2,     3,     3,     2,     1,
     1,     1,     1,     1,     3,     4,     1,     1,     1,     3,
     3,     2,     2,     2,     3,     3,     4,     0,     7,     0,
     9,     1,     0,     2,     1,     2,     4,     0,     8,     0,
    10,     0,     8,     0,    10,     0,     8,     0,    10,     0,
     4,     0,     1,     0,     2,     1,     4,     1,     1,     1,
     1,     1,     1,     0,     4,     0,     1,     0,     2,     1,
     0,     7,     0,     8,     1,     2,     1,     1,     1,     1,
     2,     2,     2,     2,     1,     1,     1,     3,     2,     0,
     4,     0,     2,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     0,     1,     1
};

static const short yydefact[] = {     3,
     1,    20,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     2,     4,     9,    27,     5,     0,    10,    52,     6,
     0,    11,    52,     7,     0,    12,   172,     8,     0,    13,
    14,    15,    16,    17,    18,    19,     0,    40,    26,    46,
     0,   414,     0,     0,     0,     0,     0,     0,     0,    28,
    36,     0,    76,    51,    61,    76,    61,     0,   198,   171,
   174,   185,    24,     0,    23,     0,     0,   428,     0,   416,
   432,     0,     0,   415,   436,     0,   418,   407,   408,   395,
   400,   401,   402,   358,     0,     0,     0,   404,     0,     0,
     0,     0,   493,   354,   353,   359,   390,   391,   392,   393,
   394,   403,   409,   493,    25,     0,     0,     0,    35,    53,
     0,   102,    75,     0,    84,    60,   102,    84,     0,   175,
     0,     0,   197,   200,   173,     0,   211,   184,   187,    21,
     0,   493,     0,   442,     0,   442,   417,     0,   442,     0,
   423,   412,   413,     0,     0,   396,     0,   399,     0,     0,
     0,   377,   381,   367,     0,   492,    30,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   493,    32,   493,     0,   493,     0,    56,    77,     0,     0,
   101,    62,     0,   120,    83,   138,   120,     0,   180,     0,
   201,   493,   345,   199,     0,   188,     0,     0,   210,   213,
   186,    22,     0,   346,   430,     0,   456,   434,   456,   438,
   456,   420,     0,     0,   422,   425,   385,     0,   357,     0,
   397,   398,     0,   405,   360,   368,   369,   370,   371,   372,
   373,   374,   375,   376,   378,   379,   380,   382,   383,     0,
   361,   362,   363,   364,   365,   366,   410,   411,    31,    29,
     0,    33,     0,    59,    54,     0,    58,    80,   103,   493,
    39,    65,    85,     0,     0,   119,     0,   252,   137,   155,
   177,   183,   176,     0,   182,     0,   206,   348,     0,   193,
     0,   214,   493,   212,   493,   442,   440,     0,     0,   442,
     0,   442,     0,   423,   426,     0,     0,   424,     0,   386,
     0,   356,   406,     0,    38,     0,    55,     0,    92,     0,
    78,     0,    82,    91,   106,   493,    42,    68,    63,     0,
    67,    88,   121,   493,   139,     0,    70,   251,     0,   299,
   154,   180,   179,     0,   203,     0,   202,     0,   208,   190,
   196,   189,     0,   195,     0,   219,   349,   347,   456,   444,
   454,     0,   456,     0,   456,     0,     0,     0,   419,     0,
     0,   384,    37,    57,     0,    94,    79,     0,     0,     0,
   104,   105,    41,    64,     0,    86,     0,    90,   124,    43,
   142,     0,   255,     0,     0,    69,   156,     0,    73,   298,
   178,   181,   206,     0,   205,     0,   193,   192,     0,   216,
     0,   215,   218,   221,     0,     0,   441,   443,   446,   458,
   429,     0,   433,     0,   437,     0,   427,     0,   387,   355,
    95,    93,    98,    99,   100,    81,   114,     0,   112,   118,
     0,   116,     0,     0,    66,    87,     0,     0,   122,   123,
     0,   140,   141,   144,   253,   258,     0,   493,    45,   159,
     0,   302,     0,     0,    72,   204,   209,   207,   191,   194,
   219,     0,   220,     0,     0,   445,   465,   476,   477,     0,
   467,   468,   469,   470,   475,     0,   455,   457,   460,     0,
     0,     0,   421,     0,    96,    97,     0,   108,     0,     0,
   110,     0,     0,     0,    89,   132,     0,   130,   136,     0,
   134,     0,     0,     0,   143,   258,   263,   286,   256,   257,
   260,   261,   262,     0,     0,   280,     0,   493,    48,     0,
   157,   158,   161,   300,   305,     0,   493,   217,     0,   431,
   449,   450,   448,   452,   453,   451,     0,   466,     0,   471,
   472,   473,   474,   459,   461,   435,   439,     0,   388,   113,
   111,   117,   115,   107,   109,     0,   126,     0,     0,   128,
     0,     0,     0,   493,   254,     0,     0,   285,   259,     0,
     0,     0,    71,    47,     0,   160,   305,   310,   333,   303,
   304,   307,   308,   309,     0,     0,   327,     0,    49,   222,
   447,   463,   480,     0,   131,   129,   135,   133,   125,   127,
     0,   493,   145,     0,     0,   267,     0,     0,   294,   270,
     0,   288,     0,   270,     0,   284,   279,   493,   301,     0,
   332,   306,     0,     0,     0,    74,   480,     0,   482,   389,
   493,     0,   147,   172,     0,   264,     0,   352,   296,   293,
     0,   272,     0,     0,     0,   272,     0,   283,     0,   493,
   162,     0,     0,   314,   341,   317,     0,   335,     0,   317,
     0,   331,   326,   482,     0,   479,     0,   484,   149,   493,
   172,   198,     0,     0,   266,   269,     0,     0,   292,   287,
   294,   270,   297,     0,   270,   493,     0,   164,   185,     0,
   311,     0,   340,   343,     0,   319,     0,     0,     0,   319,
     0,   330,   484,   478,     0,     0,     0,   172,   151,   198,
   146,   224,     0,     0,   274,   271,   495,   494,   277,   291,
   272,   275,   272,   166,   493,   185,   211,     0,     0,   313,
   316,     0,     0,   339,   334,   341,   317,   344,     0,   317,
     0,     0,   487,   488,   485,   486,   490,   491,   489,   483,
   462,   198,   172,   148,   248,   268,   493,   273,     0,     0,
   185,   168,   211,   163,   236,     0,     0,   321,   318,   324,
   338,   319,   322,   319,   464,   481,   150,   198,     0,     0,
   223,   153,   265,   278,   276,   211,   185,   165,   250,   315,
     0,   320,     0,     0,   152,     0,   225,     0,   167,   211,
     0,     0,   235,   170,   312,   325,   323,     0,   230,     0,
   169,     0,   237,     0,   227,   234,   226,     0,   232,   247,
     0,   242,     0,   230,   233,   229,     0,   239,   246,   238,
     0,   244,   249,   228,   231,   242,   245,   241,     0,   240,
   243,     0,     0,     0
};

static const short yydefgoto[] = {   852,
     1,    12,    13,    37,    64,    65,    14,    15,    49,    50,
   105,    16,    17,   108,   109,    18,    19,   271,    20,    21,
    22,    23,   459,    24,    25,    53,    54,   187,   265,   266,
   267,   115,   116,   272,   329,   330,   331,   395,   396,   464,
   465,   112,   113,   268,   321,   322,   194,   195,   332,   386,
   387,   323,   324,   190,   191,   325,   381,   382,   438,   439,
   441,   442,   275,   276,   389,   449,   450,   507,   508,   510,
   511,   278,   279,   391,   452,   453,   454,   644,   681,   718,
   763,   721,   340,   341,   460,   531,   532,   533,   699,   736,
   771,   797,   774,    59,    60,    61,   199,   342,   283,   284,
   285,   127,   128,   129,   290,   407,   352,   353,   354,   722,
   123,   124,   287,   403,   347,   348,   349,   775,   209,   210,
   356,   471,   412,   413,   414,   765,   791,   819,   834,   827,
   828,   829,   799,   813,   832,   846,   840,   841,   842,   792,
   814,   337,   338,   516,   456,   519,   520,   521,   522,   576,
   684,   615,   616,   652,   688,   724,   523,   524,   525,   627,
   526,   620,   621,   690,   622,   623,   399,   400,   587,   535,
   590,   591,   592,   593,   630,   739,   663,   664,   706,   743,
   777,   594,   595,   596,   673,   597,   666,   667,   745,   668,
   669,    26,    27,   203,    28,    29,   650,    93,    94,    95,
    96,    97,    98,    99,   100,   101,   102,   103,    30,    31,
    32,    33,   141,   304,   224,   225,   226,    34,   134,   296,
    35,   136,   300,    36,   139,   302,   217,   360,   417,   418,
   419,   547,   299,   420,   487,   488,   489,   603,   637,   490,
   639,   678,   717,   760,   157,   729
};

static const short yypact[] = {-32768,
   332,-32768,    -6,   127,   163,   168,     1,   180,   -10,   198,
   206,-32768,-32768,-32768,   144,-32768,   149,-32768,   221,-32768,
   162,-32768,   221,-32768,   165,-32768,   231,-32768,   170,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   237,   186,   191,   196,
   218,-32768,   -27,   -40,   243,   -31,   -24,   254,   281,   286,
   294,   248,   327,-32768,   318,   327,   318,   -11,   346,   231,
-32768,   326,-32768,   -53,-32768,   316,   339,-32768,   355,-32768,
-32768,   288,   378,-32768,-32768,   388,-32768,   392,   405,   278,
-32768,-32768,-32768,   285,   121,    19,   380,   409,   311,   311,
   311,   311,   369,-32768,   703,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   193,   -17,-32768,   254,   414,   352,   444,-32768,
   412,   503,-32768,   430,   508,-32768,   503,   508,   254,-32768,
    60,   489,   346,-32768,-32768,   124,   510,   326,-32768,-32768,
   237,    33,   424,   463,   426,   463,-32768,   427,   463,   428,
   490,-32768,-32768,   183,   241,-32768,   491,-32768,   492,   493,
   422,-32768,-32768,-32768,   574,-32768,-32768,   311,   311,   311,
   311,   311,   311,   311,   311,   311,   311,   311,   311,   311,
   311,   311,   311,   311,   311,   311,   311,   311,   494,   496,
   369,-32768,   369,   254,   369,   435,   498,-32768,   449,   511,
-32768,-32768,   450,   528,-32768,   527,   528,   454,   506,   254,
-32768,   369,-32768,-32768,   254,-32768,   126,   446,   510,-32768,
-32768,-32768,   509,   452,-32768,   460,   495,-32768,   495,-32768,
   495,-32768,    88,   453,   490,-32768,-32768,   347,-32768,   396,
-32768,-32768,   451,-32768,-32768,   223,   223,   223,   223,   223,
   223,   753,   753,   753,   282,   282,   282,   306,   306,   678,
   320,   320,   457,   457,   457,   457,-32768,-32768,-32768,-32768,
   459,-32768,   254,-32768,-32768,   136,-32768,    16,-32768,   117,
-32768,   519,-32768,   470,   461,-32768,   471,   546,-32768,   548,
-32768,-32768,-32768,   161,-32768,   476,   542,-32768,   478,   545,
   254,-32768,   369,-32768,   369,   463,-32768,   488,   500,   463,
   501,   463,   505,   490,-32768,   556,   499,-32768,   311,-32768,
   311,-32768,-32768,   311,-32768,   521,-32768,   498,   517,   555,
-32768,   175,-32768,-32768,   257,   369,-32768,-32768,-32768,   204,
-32768,    16,-32768,   369,-32768,    56,   580,-32768,   518,   582,
-32768,   506,-32768,   506,-32768,   524,-32768,   222,-32768,-32768,
-32768,-32768,   238,-32768,   549,   588,-32768,   535,   495,   596,
-32768,   540,   495,   543,   495,   557,   539,   561,-32768,   497,
   616,   728,-32768,-32768,   279,-32768,-32768,    16,   603,   604,
-32768,   344,-32768,-32768,   519,-32768,   564,-32768,   351,-32768,
   624,   581,-32768,   583,   641,-32768,-32768,    57,   657,-32768,
-32768,-32768,   542,   254,-32768,   542,   545,-32768,   545,-32768,
   575,-32768,   588,-32768,   577,   578,-32768,   596,-32768,   343,
-32768,   584,-32768,   585,-32768,   586,-32768,   311,-32768,-32768,
   630,   634,-32768,-32768,-32768,-32768,   594,   268,-32768,   598,
   357,-32768,   603,   604,-32768,    16,   643,   647,-32768,   438,
   614,-32768,   624,-32768,-32768,    13,   672,   119,-32768,   674,
   625,-32768,   626,   617,-32768,-32768,-32768,-32768,-32768,-32768,
   588,   254,-32768,   619,   141,-32768,   627,-32768,-32768,   397,
-32768,-32768,-32768,-32768,-32768,   385,-32768,   343,-32768,   679,
   623,   635,-32768,   538,-32768,-32768,   633,-32768,   603,   636,
-32768,   604,   361,   395,-32768,   639,   398,-32768,   646,   400,
-32768,   643,   647,   723,-32768,    13,-32768,    71,-32768,    13,
-32768,-32768,-32768,   382,   661,-32768,   686,   369,-32768,   698,
-32768,   674,-32768,-32768,    23,   750,   369,-32768,   695,-32768,
   392,   405,-32768,-32768,-32768,   193,   711,-32768,   773,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   311,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   712,-32768,   643,   713,-32768,
   647,   402,   406,     0,-32768,   778,   254,   726,-32768,   125,
   125,   792,-32768,-32768,   793,-32768,    23,-32768,    89,-32768,
    23,-32768,-32768,-32768,   484,   729,-32768,   740,-32768,-32768,
-32768,-32768,   738,   652,-32768,-32768,-32768,-32768,-32768,-32768,
   802,    43,-32768,   741,   824,   752,   748,   254,     3,   826,
   749,-32768,    29,   826,    15,   754,-32768,    40,-32768,   815,
   751,-32768,   818,   818,   819,-32768,   738,    36,   803,-32768,
   369,   820,-32768,   231,   254,-32768,   778,-32768,-32768,   757,
   254,   845,   825,   827,   254,   845,   827,-32768,   828,    47,
-32768,   762,   840,   770,   166,   843,   766,-32768,    10,   843,
     8,   754,-32768,   803,   768,-32768,   767,   772,-32768,   369,
   231,   346,   850,   836,-32768,-32768,   837,   410,   754,-32768,
    96,   826,-32768,   410,   826,   369,   839,-32768,   326,   254,
-32768,   815,   775,-32768,   254,   859,   841,   842,   254,   859,
   842,-32768,   772,-32768,   844,   201,   785,   231,-32768,   346,
-32768,-32768,   254,   130,-32768,-32768,-32768,-32768,-32768,-32768,
   845,-32768,   845,-32768,   369,   326,   510,   860,   846,-32768,
-32768,   847,   789,   754,-32768,   152,   843,-32768,   791,   843,
   794,   783,   392,   405,-32768,-32768,-32768,-32768,   193,-32768,
-32768,   346,   231,-32768,   178,-32768,   369,-32768,   410,   410,
   326,-32768,   510,-32768,-32768,   254,   132,-32768,-32768,-32768,
-32768,   859,-32768,   859,-32768,-32768,-32768,   346,   128,   801,
-32768,-32768,-32768,-32768,-32768,   510,   326,-32768,   210,-32768,
   795,-32768,   796,   797,-32768,   254,-32768,   855,-32768,   510,
   129,   807,-32768,-32768,-32768,-32768,-32768,   808,   861,   804,
-32768,   254,-32768,   858,-32768,   754,-32768,   413,-32768,-32768,
   810,   863,   806,   861,-32768,-32768,   861,-32768,   754,-32768,
   415,-32768,-32768,-32768,-32768,   863,-32768,-32768,   863,-32768,
-32768,   899,   900,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   771,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   878,-32768,-32768,-32768,-32768,
   587,   849,-32768,-32768,-32768,-32768,   522,-32768,-32768,-32768,
-32768,   848,-32768,-32768,-32768,-32768,   790,-32768,-32768,-32768,
-32768,   525,  -317,   798,-32768,-32768,-32768,-32768,   466,   411,
   467,   416,   715,-32768,-32768,-32768,-32768,   401,   348,   404,
   349,-32768,-32768,-32768,-32768,-32768,   468,-32768,-32768,-32768,
-32768,  -711,-32768,-32768,-32768,-32768,-32768,   387,-32768,-32768,
-32768,-32768,  -619,  -633,-32768,   854,-32768,-32768,   589,-32768,
   579,  -670,-32768,   799,-32768,-32768,   515,-32768,   516,   865,
-32768,   805,-32768,-32768,   523,-32768,   526,   809,-32768,   720,
-32768,-32768,   462,-32768,   529,-32768,-32768,-32768,-32768,   100,
-32768,    93,-32768,-32768,-32768,-32768,    91,-32768,    86,-32768,
-32768,-32768,-32768,-32768,-32768,   423,-32768,   418,-32768,-32768,
-32768,   293,-32768,  -616,  -600,-32768,  -532,-32768,-32768,-32768,
-32768,  -574,-32768,-32768,-32768,   360,-32768,-32768,-32768,-32768,
   356,-32768,   353,-32768,-32768,-32768,   244,-32768,  -654,  -646,
-32768,  -596,-32768,-32768,-32768,-32768,  -613,-32768,-32768,-32768,
   313,-32768,-32768,-32768,-32768,-32768,  -500,  -106,-32768,   -54,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -474,-32768,-32768,
-32768,-32768,-32768,-32768,   644,-32768,   724,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,  -133,-32768,-32768,-32768,
   532,-32768,  -217,-32768,-32768,-32768,   464,-32768,-32768,   465,
   314,   280,   240,-32768,   -99,  -682
};


#define	YYLAST		954


static const short yytable[] = {   183,
   546,   301,   219,   303,   182,   221,   624,   656,   764,   119,
   682,   732,   198,    69,   388,   710,   181,   578,  -295,  -295,
   670,   611,    73,    44,   711,   708,    67,    38,   737,    76,
   517,   657,   214,  -295,   152,   153,   154,   155,   130,   131,
   588,   709,    45,   709,   654,   655,   518,   720,    42,   319,
   787,    70,   148,    71,   213,   694,   589,   320,   149,   655,
    74,   659,    75,   749,   642,   773,    68,   675,   697,    77,
   392,   461,   120,   612,   156,   731,   805,   261,   733,   692,
   200,   259,   695,   260,   762,   262,   794,   795,   631,   228,
   230,   156,   782,   286,   747,   784,   577,   750,   289,   577,
   796,  -290,   288,   236,   237,   238,   239,   240,   241,   242,
   243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
   253,   254,   255,   256,   156,   658,   810,   660,   505,   788,
   769,   156,   770,   676,   156,   803,   577,   804,   156,   393,
   462,   415,   778,   201,   205,   422,   291,   424,   806,   822,
   326,   725,   528,   798,   146,   618,   316,   767,   619,   801,
    39,   147,   359,   518,   703,   589,   363,   577,   365,  -282,
   327,   712,   541,   542,    48,   543,   809,   544,   545,   305,
   802,  -342,  -342,   306,   355,   577,   789,  -329,   730,   790,
   821,   768,   577,   357,  -290,   358,    40,    78,    79,    80,
    81,    41,    82,    83,    84,    85,    86,   206,   156,   292,
   156,   807,   823,    43,    78,    79,    80,    81,   811,    82,
    83,   812,    85,    86,   179,   180,   383,   317,   318,    87,
    88,    46,   753,   754,   390,   755,   756,   757,   758,    47,
    89,   759,    51,   781,    90,   703,    87,    88,   577,    52,
  -337,    91,   343,   344,   370,    55,   371,    89,    57,   372,
    58,    90,   577,    62,  -337,    92,   377,   378,    91,   379,
   380,    63,    78,    79,    80,    81,    72,    82,    83,   -44,
    85,    86,    92,   227,   -34,    78,    79,    80,    81,   -50,
    82,    83,    84,    85,    86,   384,   385,   467,   167,   168,
   169,    66,   170,   171,    87,    88,   104,   173,   174,   175,
   176,   177,   178,   405,   406,    89,   106,    87,    88,    90,
   431,   432,   433,   434,   435,   835,    91,   107,    89,   408,
   409,   110,    90,   111,     2,     3,     4,     5,   847,    91,
    92,   229,    78,    79,    80,    81,   114,    82,    83,   132,
    85,    86,   121,    92,     6,   126,   443,   444,   529,   498,
   499,   170,   171,   447,   448,   539,   173,   174,   175,   176,
   177,   178,   133,   494,    87,    88,   477,   144,     7,   137,
     8,     9,    10,    11,   145,    89,   478,   479,   135,    90,
   173,   174,   175,   176,   177,   178,    91,   580,   581,   480,
   481,   482,   483,   484,   485,   486,   175,   176,   177,   178,
    92,   138,   158,   159,   160,   161,   162,   163,   164,   165,
   166,   140,   167,   168,   169,   142,   170,   171,   584,   172,
   477,   173,   174,   175,   176,   177,   178,   599,   143,   309,
   478,   479,   550,   551,   552,   553,   185,   310,   501,   502,
   512,   513,   564,   499,   481,   482,   483,   484,   485,   486,
   156,   158,   159,   160,   161,   162,   163,   164,   165,   166,
   617,   167,   168,   169,   613,   170,   171,   186,   172,   150,
   173,   174,   175,   176,   177,   178,   565,   502,   311,   567,
   568,   570,   571,   609,   568,   188,   312,   610,   571,   633,
   634,   727,   728,   604,   836,   837,   848,   849,   151,   184,
   189,   649,   643,   192,   193,   202,   207,   215,   216,   218,
   220,   222,   234,   223,   231,   232,   233,   257,   661,   258,
   263,   264,   269,   273,   270,   274,   277,   281,   683,   282,
   293,   679,   295,   297,   686,  -350,   178,   307,   693,   298,
   315,   313,   328,   333,   335,   334,   336,   339,   704,   345,
   698,   350,   158,   159,   160,   161,   162,   163,   164,   165,
   166,   361,   167,   168,   169,   346,   170,   171,   351,   172,
   719,   173,   174,   175,   176,   177,   178,   368,   376,   428,
   369,   394,   398,   738,   362,   364,   734,   429,   741,   366,
   375,   397,   748,   158,   159,   160,   161,   162,   163,   164,
   165,   166,   373,   167,   168,   169,   766,   170,   171,   404,
   172,   411,   173,   174,   175,   176,   177,   178,  -351,   416,
   558,   421,   410,   426,   423,   772,   437,   440,   559,   158,
   159,   160,   161,   162,   163,   164,   165,   166,   425,   167,
   168,   169,   427,   170,   171,   446,   172,   451,   173,   174,
   175,   176,   177,   178,   455,   458,   457,   793,   463,   800,
   472,   474,   495,   475,   235,   496,   506,   493,   491,   492,
   509,   158,   159,   160,   161,   162,   163,   164,   165,   166,
   497,   167,   168,   169,   500,   170,   171,   514,   172,   818,
   173,   174,   175,   176,   177,   178,   527,   530,   534,   536,
   540,   537,   555,   548,   556,   831,   430,   158,   159,   160,
   161,   162,   163,   164,   165,   166,   557,   167,   168,   169,
   560,   170,   171,   562,   172,   566,   173,   174,   175,   176,
   177,   178,   569,   158,   159,   160,   161,   162,   163,   164,
   165,   166,   640,   167,   168,   169,   574,   170,   171,   582,
   172,   314,   173,   174,   175,   176,   177,   178,   158,   159,
   160,   161,   162,   163,   164,   165,   166,   583,   167,   168,
   169,   585,   170,   171,   598,   172,   600,   173,   174,   175,
   176,   177,   178,   158,   159,   160,   161,   162,   163,   164,
   165,   166,   601,   167,   168,   169,   602,   170,   171,   605,
   607,   614,   173,   174,   175,   176,   177,   178,   158,   159,
   160,   161,   162,   163,  -281,   626,   628,   635,   167,   168,
   169,   636,   170,   171,   638,   641,   645,   173,   174,   175,
   176,   177,   178,   646,   647,   648,   651,   653,   662,  -328,
   577,   665,   672,   680,   677,  -289,   687,   700,   689,   701,
   691,   696,   702,   705,   707,   714,   715,   716,   723,   518,
   742,   726,   735,  -336,   744,   746,   761,   752,   776,   589,
   780,   779,   783,   786,   808,   785,   815,   816,   817,   820,
   824,   825,   833,   838,   826,   830,   839,   843,   853,   854,
    56,   212,   436,   117,   374,   118,   445,   197,   503,   561,
   504,   280,   572,   125,   196,   606,   573,   563,   586,   608,
   515,   469,   402,   122,   470,   466,   211,   204,   294,   845,
   401,   468,   538,   844,   851,   208,   850,   579,   575,   685,
   625,   473,   629,   632,   549,   740,   671,   367,   308,   476,
   674,   554,   751,   713
};

static const short yycheck[] = {   106,
   475,   219,   136,   221,   104,   139,   581,   624,   720,    21,
   644,   694,   119,    54,   332,   670,    34,   518,    16,    17,
   634,    22,    54,    34,    17,    16,    54,    34,   699,    54,
    18,    17,   132,    31,    89,    90,    91,    92,    92,    93,
    18,    34,    53,    34,    16,    31,    34,   681,    48,    34,
   762,    92,    34,    94,    22,   656,    34,    42,    40,    31,
    92,    22,    94,   710,    22,   736,    94,    32,    22,    94,
    15,    15,    84,   574,    92,   692,   788,   184,   695,   654,
    21,   181,   657,   183,   718,   185,   769,   770,   589,   144,
   145,    92,   747,   200,   708,   750,    97,   711,   205,    97,
   771,    99,   202,   158,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
   175,   176,   177,   178,    92,   626,   797,   628,   446,   763,
   731,    92,   733,    98,    92,   782,    97,   784,    92,    84,
    84,   359,   739,    84,    21,   363,    21,   365,    21,    21,
    34,   684,    34,   773,    34,    31,   263,    28,    34,    28,
    34,    41,   296,    34,   665,    34,   300,    97,   302,    99,
   270,   672,    32,    33,    31,    35,   796,    37,    38,    92,
   777,    16,    17,    96,   291,    97,     9,    99,   689,    12,
   810,   724,    97,   293,    99,   295,    34,    32,    33,    34,
    35,    34,    37,    38,    39,    40,    41,    84,    92,    84,
    92,    84,    84,    34,    32,    33,    34,    35,     9,    37,
    38,    12,    40,    41,    32,    33,   326,    92,    93,    64,
    65,    34,    32,    33,   334,    35,    36,    37,    38,    34,
    75,   716,    94,   744,    79,   746,    64,    65,    97,    29,
    99,    86,    92,    93,   309,    94,   311,    75,    94,   314,
    30,    79,    97,    94,    99,   100,    92,    93,    86,    13,
    14,    35,    32,    33,    34,    35,    34,    37,    38,    94,
    40,    41,   100,   101,    94,    32,    33,    34,    35,    94,
    37,    38,    39,    40,    41,    92,    93,   404,    76,    77,
    78,    84,    80,    81,    64,    65,    26,    85,    86,    87,
    88,    89,    90,    92,    93,    75,    31,    64,    65,    79,
    42,    43,    44,    45,    46,   826,    86,    34,    75,    92,
    93,    84,    79,     7,     3,     4,     5,     6,   839,    86,
   100,   101,    32,    33,    34,    35,    29,    37,    38,    34,
    40,    41,     7,   100,    23,    30,    13,    14,   458,    92,
    93,    80,    81,    13,    14,   472,    85,    86,    87,    88,
    89,    90,    34,   428,    64,    65,    34,   100,    47,    92,
    49,    50,    51,    52,   100,    75,    44,    45,    34,    79,
    85,    86,    87,    88,    89,    90,    86,    16,    17,    57,
    58,    59,    60,    61,    62,    63,    87,    88,    89,    90,
   100,    34,    66,    67,    68,    69,    70,    71,    72,    73,
    74,    34,    76,    77,    78,    34,    80,    81,   528,    83,
    34,    85,    86,    87,    88,    89,    90,   537,    34,    93,
    44,    45,    58,    59,    60,    61,    95,   101,    92,    93,
    13,    14,    92,    93,    58,    59,    60,    61,    62,    63,
    92,    66,    67,    68,    69,    70,    71,    72,    73,    74,
   577,    76,    77,    78,   574,    80,    81,    34,    83,   100,
    85,    86,    87,    88,    89,    90,    92,    93,    93,    92,
    93,    92,    93,    92,    93,    84,   101,    92,    93,    16,
    17,    92,    93,   558,    92,    93,    92,    93,   100,    96,
     8,   618,   612,    84,     7,    27,     7,    94,    56,    94,
    94,    94,   101,    34,    34,    34,    34,    34,   628,    34,
    96,    34,    84,    84,    24,     8,    10,    84,   645,    34,
    95,   641,    34,    84,   651,    94,    90,    95,   655,    55,
    92,   101,    34,    84,    84,    95,    11,    10,   665,    84,
   660,    84,    66,    67,    68,    69,    70,    71,    72,    73,
    74,    84,    76,    77,    78,    34,    80,    81,    34,    83,
   680,    85,    86,    87,    88,    89,    90,    32,    34,    93,
    92,    12,    11,   700,    95,    95,   696,   101,   705,    95,
    84,    84,   709,    66,    67,    68,    69,    70,    71,    72,
    73,    74,    92,    76,    77,    78,   723,    80,    81,    96,
    83,    34,    85,    86,    87,    88,    89,    90,    94,    34,
    93,    92,    84,    95,    92,   735,    34,    34,   101,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    92,    76,
    77,    78,    92,    80,    81,    92,    83,    34,    85,    86,
    87,    88,    89,    90,    84,    25,    84,   767,    12,   776,
    96,    95,    43,    96,   101,    42,    34,    92,    95,    95,
    34,    66,    67,    68,    69,    70,    71,    72,    73,    74,
    97,    76,    77,    78,    97,    80,    81,    84,    83,   806,
    85,    86,    87,    88,    89,    90,    35,    34,    84,    84,
    92,    95,    34,    87,    92,   822,   101,    66,    67,    68,
    69,    70,    71,    72,    73,    74,    92,    76,    77,    78,
    98,    80,    81,    98,    83,    97,    85,    86,    87,    88,
    89,    90,    97,    66,    67,    68,    69,    70,    71,    72,
    73,    74,   101,    76,    77,    78,    34,    80,    81,    99,
    83,    84,    85,    86,    87,    88,    89,    90,    66,    67,
    68,    69,    70,    71,    72,    73,    74,    92,    76,    77,
    78,    84,    80,    81,    35,    83,    92,    85,    86,    87,
    88,    89,    90,    66,    67,    68,    69,    70,    71,    72,
    73,    74,    92,    76,    77,    78,    34,    80,    81,    98,
    98,    34,    85,    86,    87,    88,    89,    90,    66,    67,
    68,    69,    70,    71,    99,    34,    34,    99,    76,    77,
    78,    92,    80,    81,    97,    34,    96,    85,    86,    87,
    88,    89,    90,    20,    93,    98,    21,    99,    34,    99,
    97,    34,    34,    34,    52,    99,    12,    96,    34,    20,
    34,    34,    93,    21,    99,    98,   100,    96,    19,    34,
    12,    35,    34,    99,    34,    34,    92,    34,    19,    34,
    92,    35,    92,   101,    84,    92,    92,    92,    92,    35,
    84,    84,    35,    84,    34,    92,    34,    92,     0,     0,
    23,   131,   378,    56,   318,    57,   385,   118,   443,   499,
   444,   197,   512,    60,   117,   568,   513,   502,   532,   571,
   453,   407,   344,    59,   409,   403,   128,   123,   209,   837,
   342,   406,   471,   834,   849,   127,   846,   520,   516,   647,
   581,   413,   587,   591,   480,   702,   634,   304,   225,   418,
   637,   488,   713,   674
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
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-3], yylsp[-1]);
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
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-4], yylsp[-1]);
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
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-3], yylsp[-1]);
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
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-4], yylsp[-1]);
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
                  ps.conns->setCheckUnconnected( "false" );
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
                  ps.conns->setCheckUnconnected( "yes" );
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
#line 1515 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 286:
#line 1522 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 289:
#line 1536 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 290:
#line 1541 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 291:
#line 1548 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 292:
#line 1553 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
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
#line 1573 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 296:
#line 1578 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 297:
#line 1583 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 300:
#line 1599 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "false" );
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 301:
#line 1605 "ned.y"
{
                ;
    break;}
case 302:
#line 1608 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "yes" );
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 303:
#line 1614 "ned.y"
{
                ;
    break;}
case 310:
#line 1635 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                ;
    break;}
case 311:
#line 1639 "ned.y"
{
                  ps.inLoop=1;
                ;
    break;}
case 312:
#line 1643 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-7],yylsp[-4]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 315:
#line 1657 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 316:
#line 1667 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 318:
#line 1675 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 322:
#line 1688 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 323:
#line 1693 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 324:
#line 1698 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 325:
#line 1704 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 328:
#line 1718 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 329:
#line 1724 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 330:
#line 1732 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 331:
#line 1737 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 332:
#line 1744 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 333:
#line 1751 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 336:
#line 1765 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 337:
#line 1770 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 338:
#line 1777 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 339:
#line 1782 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 340:
#line 1789 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 341:
#line 1794 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 342:
#line 1802 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 343:
#line 1807 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 344:
#line 1812 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 346:
#line 1830 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1],NULLPOS);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 347:
#line 1836 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-5],yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-6],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 348:
#line 1845 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 349:
#line 1859 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 350:
#line 1867 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1],NULLPOS);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 351:
#line 1873 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-5],yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-6],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 352:
#line 1886 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 353:
#line 1892 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 354:
#line 1896 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 355:
#line 1907 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 356:
#line 1909 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 357:
#line 1911 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 358:
#line 1913 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 360:
#line 1919 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 361:
#line 1922 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 362:
#line 1924 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 363:
#line 1926 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 364:
#line 1928 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 365:
#line 1930 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 366:
#line 1932 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 367:
#line 1936 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); ;
    break;}
case 368:
#line 1939 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 369:
#line 1941 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 370:
#line 1943 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 371:
#line 1945 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 372:
#line 1947 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 373:
#line 1949 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 374:
#line 1952 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 375:
#line 1954 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 376:
#line 1956 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 377:
#line 1960 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 378:
#line 1963 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 379:
#line 1965 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 380:
#line 1967 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 381:
#line 1971 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 382:
#line 1973 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 383:
#line 1975 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 384:
#line 1977 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 385:
#line 1980 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 386:
#line 1982 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 387:
#line 1984 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 388:
#line 1986 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 389:
#line 1988 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-9]), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 395:
#line 2001 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 396:
#line 2006 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                ;
    break;}
case 397:
#line 2011 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 398:
#line 2017 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 399:
#line 2023 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 400:
#line 2031 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 401:
#line 2036 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 402:
#line 2038 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 404:
#line 2047 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 405:
#line 2049 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 406:
#line 2051 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 407:
#line 2056 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 408:
#line 2058 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 409:
#line 2060 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 414:
#line 2077 "ned.y"
{
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(yylsp[0])));
                  setComments(ps.cplusplus,yylsp[-1],yylsp[0]);
                ;
    break;}
case 415:
#line 2086 "ned.y"
{
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(yylsp[-1]));
                  setComments(ps.structdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 416:
#line 2095 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(yylsp[-1]));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 417:
#line 2102 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(yylsp[-1]));
                  setComments(ps.classdecl,yylsp[-3],yylsp[-2]);
                ;
    break;}
case 418:
#line 2112 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 419:
#line 2119 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 420:
#line 2123 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 421:
#line 2131 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 426:
#line 2148 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-1],yylsp[-1]);
                ;
    break;}
case 427:
#line 2154 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 428:
#line 2164 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 429:
#line 2170 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 430:
#line 2174 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 431:
#line 2181 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 432:
#line 2188 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 433:
#line 2194 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 434:
#line 2198 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 435:
#line 2205 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 436:
#line 2212 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 437:
#line 2218 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 438:
#line 2222 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 439:
#line 2229 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 440:
#line 2236 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 447:
#line 2256 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 454:
#line 2275 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 461:
#line 2295 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                ;
    break;}
case 462:
#line 2301 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 463:
#line 2305 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(true);
                ;
    break;}
case 464:
#line 2312 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 478:
#line 2339 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 479:
#line 2344 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 481:
#line 2352 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 483:
#line 2360 "ned.y"
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
#line 2380 "ned.y"


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

