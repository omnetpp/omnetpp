
/*  A Bison parser, made from ned.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse nedyyparse
#define yylex nedyylex
#define yyerror nedyyerror
#define yylval nedyylval
#define yychar nedyychar
#define yydebug nedyydebug
#define yynerrs nedyynerrs
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
#define	READONLY	314
#define	CHARTYPE	315
#define	SHORTTYPE	316
#define	INTTYPE	317
#define	LONGTYPE	318
#define	DOUBLETYPE	319
#define	UNSIGNED_	320
#define	SIZEOF	321
#define	SUBMODINDEX	322
#define	PLUSPLUS	323
#define	EQ	324
#define	NE	325
#define	GT	326
#define	GE	327
#define	LS	328
#define	LE	329
#define	AND	330
#define	OR	331
#define	XOR	332
#define	NOT	333
#define	BIN_AND	334
#define	BIN_OR	335
#define	BIN_XOR	336
#define	BIN_COMPL	337
#define	SHIFT_LEFT	338
#define	SHIFT_RIGHT	339
#define	INVALID_CHAR	340
#define	UMIN	341

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



#define	YYFINAL		884
#define	YYFLAG		-32768
#define	YYNTBASE	106

#define YYTRANSLATE(x) ((unsigned)(x) <= 341 ? yytranslate[x] : 357)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    93,     2,     2,   104,
   105,    91,    89,    97,    90,   103,    92,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    88,    96,     2,
   100,     2,    87,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
   101,     2,   102,    94,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    98,     2,    99,     2,     2,     2,     2,     2,
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
    77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
    95
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     6,     8,    10,    12,    14,    16,    18,
    20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
    40,    42,    43,    48,    52,    54,    56,    60,    63,    64,
    66,    71,    75,    79,    82,    88,    91,    93,    94,   100,
   105,   111,   114,   118,   121,   129,   132,   141,   144,   148,
   151,   162,   165,   167,   168,   169,   174,   177,   178,   182,
   184,   186,   188,   189,   190,   195,   198,   199,   203,   205,
   207,   209,   210,   215,   217,   218,   223,   225,   226,   227,
   232,   235,   236,   240,   242,   244,   245,   246,   251,   254,
   255,   259,   261,   263,   265,   269,   272,   276,   281,   286,
   290,   294,   298,   302,   304,   305,   306,   311,   313,   314,
   319,   323,   328,   332,   336,   338,   342,   344,   348,   350,
   354,   356,   358,   359,   360,   365,   367,   368,   373,   377,
   382,   386,   390,   392,   396,   398,   402,   404,   408,   410,
   412,   413,   414,   419,   421,   422,   425,   427,   428,   436,
   437,   446,   447,   457,   458,   469,   473,   475,   476,   477,
   482,   484,   485,   488,   490,   491,   499,   500,   509,   510,
   520,   521,   532,   536,   538,   539,   542,   544,   545,   550,
   551,   558,   561,   562,   566,   568,   570,   572,   573,   576,
   578,   579,   584,   585,   592,   595,   596,   600,   602,   604,
   606,   607,   610,   612,   613,   618,   619,   626,   629,   630,
   634,   636,   640,   642,   643,   646,   648,   649,   654,   655,
   662,   664,   665,   668,   670,   675,   678,   679,   680,   685,
   686,   693,   696,   697,   701,   703,   706,   708,   711,   712,
   713,   718,   719,   726,   729,   730,   734,   736,   739,   741,
   746,   747,   752,   753,   755,   756,   757,   763,   764,   769,
   771,   772,   775,   777,   779,   781,   782,   790,   794,   796,
   802,   805,   806,   809,   810,   813,   815,   822,   831,   838,
   847,   851,   853,   856,   858,   861,   863,   866,   869,   871,
   874,   878,   880,   883,   885,   888,   890,   893,   896,   898,
   901,   903,   906,   910,   912,   913,   914,   920,   921,   926,
   928,   929,   932,   934,   936,   938,   939,   947,   951,   953,
   959,   962,   963,   966,   967,   970,   972,   979,   988,   995,
  1004,  1008,  1010,  1013,  1015,  1018,  1020,  1023,  1026,  1028,
  1031,  1035,  1037,  1040,  1042,  1045,  1047,  1050,  1053,  1055,
  1058,  1060,  1063,  1067,  1072,  1078,  1081,  1088,  1094,  1098,
  1100,  1102,  1104,  1111,  1116,  1120,  1122,  1129,  1134,  1136,
  1140,  1144,  1148,  1152,  1156,  1160,  1164,  1167,  1171,  1175,
  1179,  1183,  1187,  1191,  1195,  1199,  1203,  1206,  1210,  1214,
  1218,  1221,  1225,  1229,  1235,  1239,  1244,  1251,  1260,  1271,
  1273,  1275,  1277,  1279,  1281,  1283,  1286,  1290,  1294,  1297,
  1299,  1301,  1303,  1305,  1307,  1311,  1316,  1318,  1320,  1322,
  1324,  1328,  1332,  1335,  1338,  1342,  1346,  1350,  1355,  1359,
  1363,  1364,  1372,  1373,  1383,  1385,  1386,  1389,  1391,  1394,
  1399,  1400,  1409,  1410,  1421,  1422,  1431,  1432,  1443,  1444,
  1453,  1454,  1465,  1466,  1471,  1472,  1474,  1475,  1478,  1480,
  1485,  1487,  1489,  1491,  1493,  1495,  1497,  1498,  1503,  1504,
  1506,  1507,  1510,  1512,  1513,  1522,  1523,  1531,  1533,  1535,
  1538,  1541,  1542,  1544,  1547,  1549,  1551,  1553,  1555,  1558,
  1561,  1564,  1567,  1569,  1571,  1573,  1577,  1581,  1584,  1585,
  1590,  1591,  1594,  1595,  1597,  1599,  1601,  1604,  1606,  1609,
  1611,  1613,  1615,  1617,  1619,  1622,  1624,  1626,  1627,  1629
};

static const short yyrhs[] = {   107,
     0,   107,   108,     0,     0,   109,     0,   118,     0,   125,
     0,   130,     0,   299,     0,   113,     0,   122,     0,   127,
     0,   296,     0,   315,     0,   316,     0,   317,     0,   318,
     0,   319,     0,   320,     0,   326,     0,   329,     0,   332,
     0,     0,     3,   110,   111,    96,     0,   111,    97,   112,
     0,   112,     0,    35,     0,   114,   115,   117,     0,     5,
    34,     0,     0,   116,     0,   116,    31,   302,   355,     0,
    31,   302,   355,     0,    26,    34,   355,     0,    26,   355,
     0,   119,    98,   120,    99,   355,     0,     5,    34,     0,
   121,     0,     0,   121,    34,   100,   302,    96,     0,    34,
   100,   302,    96,     0,   123,   132,   148,   160,   124,     0,
     4,    34,     0,    24,    34,   355,     0,    24,   355,     0,
   126,    98,   138,   153,   169,    99,   355,     0,     4,    34,
     0,   128,   132,   148,   160,   178,   248,   144,   129,     0,
     6,    34,     0,    25,    34,   355,     0,    25,   355,     0,
   131,    98,   138,   153,   169,   189,   272,   146,    99,   355,
     0,     6,    34,     0,   133,     0,     0,     0,    29,    88,
   134,   135,     0,   136,    96,     0,     0,   136,    97,   137,
     0,   137,     0,    34,     0,   139,     0,     0,     0,    29,
    88,   140,   141,     0,   142,    96,     0,     0,   142,    97,
   143,     0,   143,     0,    34,     0,   145,     0,     0,    12,
    88,    35,    96,     0,   147,     0,     0,    12,    88,    35,
    96,     0,   149,     0,     0,     0,     7,    88,   150,   151,
     0,   152,    96,     0,     0,   152,    97,   158,     0,   158,
     0,   154,     0,     0,     0,     7,    88,   155,   156,     0,
   157,    96,     0,     0,   157,    96,   159,     0,   159,     0,
   159,     0,    34,     0,    34,    88,    44,     0,    43,    34,
     0,    34,    88,    43,     0,    34,    88,    43,    44,     0,
    34,    88,    44,    43,     0,    34,    88,    45,     0,    34,
    88,    46,     0,    34,    88,    47,     0,    34,    88,    48,
     0,   161,     0,     0,     0,     8,    88,   162,   163,     0,
   164,     0,     0,   164,    13,   165,    96,     0,    13,   165,
    96,     0,   164,    14,   167,    96,     0,    14,   167,    96,
     0,   165,    97,   166,     0,   166,     0,    34,   101,   102,
     0,    34,     0,   167,    97,   168,     0,   168,     0,    34,
   101,   102,     0,    34,     0,   170,     0,     0,     0,     8,
    88,   171,   172,     0,   173,     0,     0,   173,    13,   174,
    96,     0,    13,   174,    96,     0,   173,    14,   176,    96,
     0,    14,   176,    96,     0,   174,    97,   175,     0,   175,
     0,    34,   101,   102,     0,    34,     0,   176,    97,   177,
     0,   177,     0,    34,   101,   102,     0,    34,     0,   179,
     0,     0,     0,    10,    88,   180,   181,     0,   182,     0,
     0,   182,   183,     0,   183,     0,     0,    34,    88,    34,
   355,   184,   200,   188,     0,     0,    34,    88,    34,   301,
   355,   185,   200,   188,     0,     0,    34,    88,    34,    22,
    34,   355,   186,   200,   188,     0,     0,    34,    88,    34,
   301,    22,    34,   355,   187,   200,   188,     0,   216,   232,
   246,     0,   190,     0,     0,     0,    10,    88,   191,   192,
     0,   193,     0,     0,   193,   194,     0,   194,     0,     0,
    34,    88,    34,   355,   195,   208,   199,     0,     0,    34,
    88,    34,   301,   355,   196,   208,   199,     0,     0,    34,
    88,    34,    22,    34,   355,   197,   208,   199,     0,     0,
    34,    88,    34,   301,    22,    34,   355,   198,   208,   199,
     0,   224,   239,   247,     0,   201,     0,     0,   201,   202,
     0,   202,     0,     0,    30,    88,   203,   205,     0,     0,
    30,    21,   302,    88,   204,   205,     0,   206,    96,     0,
     0,   206,    97,   207,     0,   207,     0,    34,     0,   209,
     0,     0,   209,   210,     0,   210,     0,     0,    30,    88,
   211,   213,     0,     0,    30,    21,   302,    88,   212,   213,
     0,   214,    96,     0,     0,   214,    97,   215,     0,   215,
     0,    34,     0,   217,     0,     0,   217,   218,     0,   218,
     0,     0,     7,    88,   219,   221,     0,     0,     7,    21,
   302,    88,   220,   221,     0,   222,    96,     0,     0,   222,
    97,   223,     0,   223,     0,    34,   100,   302,     0,   225,
     0,     0,   225,   226,     0,   226,     0,     0,     7,    88,
   227,   229,     0,     0,     7,    21,   302,    88,   228,   229,
     0,   230,     0,     0,   230,   231,     0,   231,     0,    34,
   100,   302,    96,     0,   232,   233,     0,     0,     0,     9,
    88,   234,   236,     0,     0,     9,    21,   302,    88,   235,
   236,     0,   237,    96,     0,     0,   237,    97,   238,     0,
   238,     0,    34,   301,     0,    34,     0,   239,   240,     0,
     0,     0,     9,    88,   241,   243,     0,     0,     9,    21,
   302,    88,   242,   243,     0,   244,    96,     0,     0,   244,
    97,   245,     0,   245,     0,    34,   301,     0,    34,     0,
    12,    88,    35,    96,     0,     0,    12,    88,    35,    96,
     0,     0,   249,     0,     0,     0,    11,    15,    88,   250,
   252,     0,     0,    11,    88,   251,   252,     0,   253,     0,
     0,   253,   254,     0,   254,     0,   255,     0,   262,     0,
     0,    18,   256,   257,    20,   261,    28,   355,     0,   258,
    97,   257,     0,   258,     0,    34,   100,   302,    19,   302,
     0,    21,   302,     0,     0,    12,    35,     0,     0,   261,
   262,     0,   262,     0,   263,    17,   267,   259,   260,   356,
     0,   263,    17,   271,    17,   267,   259,   260,   356,     0,
   263,    16,   267,   259,   260,   356,     0,   263,    16,   271,
    16,   267,   259,   260,   356,     0,   264,   103,   265,     0,
   266,     0,    34,   301,     0,    34,     0,    34,   301,     0,
    34,     0,    34,    69,     0,    34,   301,     0,    34,     0,
    34,    69,     0,   268,   103,   269,     0,   270,     0,    34,
   301,     0,    34,     0,    34,   301,     0,    34,     0,    34,
    69,     0,    34,   301,     0,    34,     0,    34,    69,     0,
    34,     0,    31,   302,     0,   271,    31,   302,     0,   273,
     0,     0,     0,    11,    15,    88,   274,   276,     0,     0,
    11,    88,   275,   276,     0,   277,     0,     0,   277,   278,
     0,   278,     0,   279,     0,   286,     0,     0,    18,   280,
   281,    20,   285,    28,    96,     0,   282,    97,   281,     0,
   282,     0,    34,   100,   302,    19,   302,     0,    21,   302,
     0,     0,    12,    35,     0,     0,   285,   286,     0,   286,
     0,   287,    17,   291,   283,   284,    96,     0,   287,    17,
   295,    17,   291,   283,   284,    96,     0,   287,    16,   291,
   283,   284,    96,     0,   287,    16,   295,    16,   291,   283,
   284,    96,     0,   288,   103,   289,     0,   290,     0,    34,
   301,     0,    34,     0,    34,   301,     0,    34,     0,    34,
    69,     0,    34,   301,     0,    34,     0,    34,    69,     0,
   292,   103,   293,     0,   294,     0,    34,   301,     0,    34,
     0,    34,   301,     0,    34,     0,    34,    69,     0,    34,
   301,     0,    34,     0,    34,    69,     0,    34,     0,    34,
   302,     0,   295,    34,   302,     0,   297,   200,   216,   298,
     0,    23,    34,    88,    34,   355,     0,    27,   355,     0,
   300,    98,   208,   224,    99,   355,     0,    23,    34,    88,
    34,   355,     0,   101,   302,   102,     0,   305,     0,   303,
     0,   304,     0,    39,   104,   305,    97,   305,   105,     0,
    39,   104,   305,   105,     0,    39,   104,   105,     0,    39,
     0,    40,   104,   312,    97,   312,   105,     0,    40,   104,
   312,   105,     0,   306,     0,   104,   305,   105,     0,   305,
    89,   305,     0,   305,    90,   305,     0,   305,    91,   305,
     0,   305,    92,   305,     0,   305,    93,   305,     0,   305,
    94,   305,     0,    90,   305,     0,   305,    70,   305,     0,
   305,    71,   305,     0,   305,    72,   305,     0,   305,    73,
   305,     0,   305,    74,   305,     0,   305,    75,   305,     0,
   305,    76,   305,     0,   305,    77,   305,     0,   305,    78,
   305,     0,    79,   305,     0,   305,    80,   305,     0,   305,
    81,   305,     0,   305,    82,   305,     0,    83,   305,     0,
   305,    84,   305,     0,   305,    85,   305,     0,   305,    87,
   305,    88,   305,     0,    34,   104,   105,     0,    34,   104,
   305,   105,     0,    34,   104,   305,    97,   305,   105,     0,
    34,   104,   305,    97,   305,    97,   305,   105,     0,    34,
   104,   305,    97,   305,    97,   305,    97,   305,   105,     0,
   307,     0,   308,     0,   309,     0,   310,     0,   311,     0,
    34,     0,    41,    34,     0,    41,    42,    34,     0,    42,
    41,    34,     0,    42,    34,     0,   312,     0,    37,     0,
    38,     0,   313,     0,    68,     0,    68,   104,   105,     0,
    67,   104,    34,   105,     0,    35,     0,    32,     0,    33,
     0,   314,     0,   314,    32,    34,     0,   314,    33,    34,
     0,    32,    34,     0,    33,    34,     0,    49,    50,   355,
     0,    53,    34,    96,     0,    52,    34,    96,     0,    52,
    55,    34,    96,     0,    51,    34,    96,     0,    54,    34,
    96,     0,     0,    54,    34,    98,   321,   323,    99,   355,
     0,     0,    54,    34,    56,    34,    98,   322,   323,    99,
   355,     0,   324,     0,     0,   324,   325,     0,   325,     0,
    34,    96,     0,    34,   100,   354,    96,     0,     0,    51,
    34,    98,   327,   335,   341,    99,   355,     0,     0,    51,
    34,    56,    34,    98,   328,   335,   341,    99,   355,     0,
     0,    52,    34,    98,   330,   335,   341,    99,   355,     0,
     0,    52,    34,    56,    34,    98,   331,   335,   341,    99,
   355,     0,     0,    53,    34,    98,   333,   335,   341,    99,
   355,     0,     0,    53,    34,    56,    34,    98,   334,   335,
   341,    99,   355,     0,     0,    58,    88,   336,   337,     0,
     0,   338,     0,     0,   338,   339,     0,   339,     0,    34,
   100,   340,    96,     0,    35,     0,    32,     0,    33,     0,
   314,     0,    37,     0,    38,     0,     0,    57,    88,   342,
   343,     0,     0,   344,     0,     0,   344,   345,     0,   345,
     0,     0,   348,   349,    34,   346,   350,   351,   352,    96,
     0,     0,   348,    34,   347,   350,   351,   352,    96,     0,
    59,     0,    60,     0,    59,    60,     0,    60,    59,     0,
     0,    34,     0,    34,    91,     0,    61,     0,    62,     0,
    63,     0,    64,     0,    66,    61,     0,    66,    62,     0,
    66,    63,     0,    66,    64,     0,    65,     0,    45,     0,
    46,     0,   101,    32,   102,     0,   101,    34,   102,     0,
   101,   102,     0,     0,    54,   104,    34,   105,     0,     0,
   100,   353,     0,     0,    35,     0,    36,     0,    32,     0,
    90,    32,     0,    33,     0,    90,    33,     0,   314,     0,
    37,     0,    38,     0,    34,     0,    32,     0,    90,    32,
     0,    34,     0,    96,     0,     0,    97,     0,    96,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   243,   247,   248,   252,   254,   256,   258,   260,   263,   265,
   267,   269,   272,   273,   274,   275,   276,   278,   280,   282,
   284,   292,   297,   304,   305,   309,   321,   325,   334,   335,
   339,   345,   354,   358,   368,   377,   387,   388,   392,   398,
   410,   418,   428,   432,   442,   453,   466,   477,   487,   491,
   501,   515,   528,   529,   533,   539,   544,   545,   549,   550,
   554,   566,   567,   571,   577,   582,   583,   587,   588,   592,
   604,   605,   609,   619,   620,   624,   634,   635,   639,   645,
   650,   651,   655,   659,   669,   670,   674,   680,   685,   686,
   690,   694,   705,   709,   713,   717,   721,   725,   729,   733,
   737,   741,   745,   755,   756,   760,   766,   771,   772,   776,
   777,   778,   779,   783,   784,   788,   793,   801,   802,   806,
   811,   822,   823,   827,   833,   838,   839,   843,   844,   845,
   846,   850,   851,   855,   860,   868,   869,   873,   878,   889,
   890,   894,   900,   905,   906,   910,   911,   915,   921,   924,
   931,   934,   940,   943,   950,   956,   965,   966,   970,   976,
   981,   982,   986,   987,   991,   997,  1000,  1007,  1010,  1016,
  1019,  1026,  1032,  1041,  1042,  1046,  1047,  1051,  1057,  1059,
  1066,  1071,  1072,  1076,  1077,  1081,  1092,  1093,  1097,  1098,
  1102,  1108,  1110,  1117,  1122,  1123,  1127,  1128,  1132,  1143,
  1144,  1148,  1149,  1153,  1159,  1161,  1168,  1174,  1175,  1179,
  1180,  1184,  1196,  1197,  1201,  1202,  1206,  1212,  1214,  1221,
  1227,  1228,  1232,  1233,  1237,  1249,  1250,  1254,  1260,  1262,
  1269,  1274,  1275,  1279,  1280,  1284,  1291,  1302,  1303,  1307,
  1313,  1315,  1322,  1327,  1328,  1332,  1333,  1337,  1344,  1355,
  1359,  1366,  1370,  1377,  1378,  1382,  1389,  1391,  1398,  1403,
  1404,  1408,  1409,  1413,  1414,  1418,  1423,  1432,  1433,  1437,
  1447,  1451,  1455,  1459,  1463,  1464,  1468,  1473,  1478,  1484,
  1493,  1494,  1498,  1504,  1512,  1517,  1521,  1529,  1536,  1542,
  1552,  1553,  1557,  1562,  1569,  1574,  1578,  1586,  1591,  1595,
  1604,  1609,  1614,  1625,  1626,  1630,  1637,  1639,  1646,  1651,
  1652,  1656,  1657,  1661,  1662,  1666,  1671,  1680,  1681,  1685,
  1695,  1699,  1703,  1707,  1711,  1712,  1716,  1721,  1726,  1732,
  1741,  1742,  1746,  1752,  1760,  1765,  1769,  1777,  1784,  1790,
  1800,  1801,  1805,  1810,  1817,  1822,  1826,  1834,  1839,  1843,
  1852,  1857,  1862,  1873,  1880,  1889,  1900,  1911,  1924,  1929,
  1934,  1938,  1949,  1951,  1953,  1955,  1960,  1962,  1967,  1968,
  1971,  1973,  1975,  1977,  1979,  1981,  1984,  1988,  1990,  1992,
  1994,  1996,  1998,  2001,  2003,  2005,  2008,  2012,  2014,  2016,
  2019,  2022,  2024,  2026,  2029,  2031,  2033,  2035,  2037,  2042,
  2043,  2044,  2045,  2046,  2050,  2055,  2060,  2066,  2072,  2080,
  2084,  2086,  2091,  2095,  2097,  2099,  2104,  2109,  2111,  2113,
  2119,  2120,  2121,  2122,  2130,  2139,  2148,  2155,  2165,  2174,
  2183,  2190,  2194,  2202,  2209,  2210,  2214,  2215,  2219,  2225,
  2235,  2241,  2245,  2252,  2259,  2265,  2269,  2276,  2283,  2289,
  2293,  2300,  2307,  2313,  2313,  2317,  2318,  2322,  2323,  2327,
  2337,  2338,  2339,  2340,  2341,  2342,  2346,  2352,  2352,  2356,
  2357,  2361,  2362,  2366,  2374,  2378,  2385,  2392,  2394,  2396,
  2398,  2400,  2405,  2406,  2408,  2409,  2410,  2411,  2413,  2414,
  2415,  2416,  2418,  2419,  2420,  2425,  2430,  2435,  2439,  2443,
  2447,  2451,  2455,  2459,  2460,  2461,  2462,  2463,  2464,  2465,
  2466,  2467,  2468,  2472,  2473,  2474,  2477,  2477,  2479,  2479
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
"ABSTRACT","READONLY","CHARTYPE","SHORTTYPE","INTTYPE","LONGTYPE","DOUBLETYPE",
"UNSIGNED_","SIZEOF","SUBMODINDEX","PLUSPLUS","EQ","NE","GT","GE","LS","LE",
"AND","OR","XOR","NOT","BIN_AND","BIN_OR","BIN_XOR","BIN_COMPL","SHIFT_LEFT",
"SHIFT_RIGHT","INVALID_CHAR","'?'","':'","'+'","'-'","'*'","'/'","'%'","'^'",
"UMIN","';'","','","'{'","'}'","'='","'['","']'","'.'","'('","')'","networkdescription",
"somedefinitions","definition","import","@1","filenames","filename","channeldefinition_old",
"channelheader_old","opt_channelattrblock_old","channelattrblock_old","endchannel_old",
"channeldefinition","channelheader","opt_channelattrblock","channelattrblock",
"simpledefinition_old","simpleheader_old","endsimple_old","simpledefinition",
"simpleheader","moduledefinition_old","moduleheader_old","endmodule_old","moduledefinition",
"moduleheader","opt_machineblock_old","machineblock_old","@2","opt_machinelist_old",
"machinelist_old","machine_old","opt_machineblock","machineblock","@3","opt_machinelist",
"machinelist","machine","opt_displayblock_old","displayblock_old","opt_displayblock",
"displayblock","opt_paramblock_old","paramblock_old","@4","opt_parameters_old",
"parameters_old","opt_paramblock","paramblock","@5","opt_parameters","parameters",
"parameter_old","parameter","opt_gateblock_old","gateblock_old","@6","opt_gates_old",
"gates_old","gatesI_old","gateI_old","gatesO_old","gateO_old","opt_gateblock",
"gateblock","@7","opt_gates","gates","gatesI","gateI","gatesO","gateO","opt_submodblock_old",
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
"numconst","timeconstant","cplusplus","struct_decl","class_decl","message_decl",
"enum_decl","enum","@36","@37","opt_enumfields","enumfields","enumfield","message",
"@38","@39","class","@40","@41","struct","@42","@43","opt_propertiesblock","@44",
"opt_properties","properties","property","propertyvalue","opt_fieldsblock","@45",
"opt_fields","fields","field","@46","@47","fieldmodifiers","fielddatatype","opt_fieldvector",
"opt_fieldenum","opt_fieldvalue","fieldvalue","enumvalue","opt_semicolon","comma_or_semicolon", NULL
};
#endif

static const short yyr1[] = {     0,
   106,   107,   107,   108,   108,   108,   108,   108,   108,   108,
   108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
   108,   110,   109,   111,   111,   112,   113,   114,   115,   115,
   116,   116,   117,   117,   118,   119,   120,   120,   121,   121,
   122,   123,   124,   124,   125,   126,   127,   128,   129,   129,
   130,   131,   132,   132,   134,   133,   135,   135,   136,   136,
   137,   138,   138,   140,   139,   141,   141,   142,   142,   143,
   144,   144,   145,   146,   146,   147,   148,   148,   150,   149,
   151,   151,   152,   152,   153,   153,   155,   154,   156,   156,
   157,   157,   158,   159,   159,   159,   159,   159,   159,   159,
   159,   159,   159,   160,   160,   162,   161,   163,   163,   164,
   164,   164,   164,   165,   165,   166,   166,   167,   167,   168,
   168,   169,   169,   171,   170,   172,   172,   173,   173,   173,
   173,   174,   174,   175,   175,   176,   176,   177,   177,   178,
   178,   180,   179,   181,   181,   182,   182,   184,   183,   185,
   183,   186,   183,   187,   183,   188,   189,   189,   191,   190,
   192,   192,   193,   193,   195,   194,   196,   194,   197,   194,
   198,   194,   199,   200,   200,   201,   201,   203,   202,   204,
   202,   205,   205,   206,   206,   207,   208,   208,   209,   209,
   211,   210,   212,   210,   213,   213,   214,   214,   215,   216,
   216,   217,   217,   219,   218,   220,   218,   221,   221,   222,
   222,   223,   224,   224,   225,   225,   227,   226,   228,   226,
   229,   229,   230,   230,   231,   232,   232,   234,   233,   235,
   233,   236,   236,   237,   237,   238,   238,   239,   239,   241,
   240,   242,   240,   243,   243,   244,   244,   245,   245,   246,
   246,   247,   247,   248,   248,   250,   249,   251,   249,   252,
   252,   253,   253,   254,   254,   256,   255,   257,   257,   258,
   259,   259,   260,   260,   261,   261,   262,   262,   262,   262,
   263,   263,   264,   264,   265,   265,   265,   266,   266,   266,
   267,   267,   268,   268,   269,   269,   269,   270,   270,   270,
   271,   271,   271,   272,   272,   274,   273,   275,   273,   276,
   276,   277,   277,   278,   278,   280,   279,   281,   281,   282,
   283,   283,   284,   284,   285,   285,   286,   286,   286,   286,
   287,   287,   288,   288,   289,   289,   289,   290,   290,   290,
   291,   291,   292,   292,   293,   293,   293,   294,   294,   294,
   295,   295,   295,   296,   297,   298,   299,   300,   301,   302,
   302,   302,   303,   303,   303,   303,   304,   304,   305,   305,
   305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
   305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
   305,   305,   305,   305,   305,   305,   305,   305,   305,   306,
   306,   306,   306,   306,   307,   307,   307,   307,   307,   308,
   309,   309,   310,   311,   311,   311,   312,   313,   313,   313,
   314,   314,   314,   314,   315,   316,   317,   317,   318,   319,
   321,   320,   322,   320,   323,   323,   324,   324,   325,   325,
   327,   326,   328,   326,   330,   329,   331,   329,   333,   332,
   334,   332,   336,   335,   335,   337,   337,   338,   338,   339,
   340,   340,   340,   340,   340,   340,   342,   341,   341,   343,
   343,   344,   344,   346,   345,   347,   345,   348,   348,   348,
   348,   348,   349,   349,   349,   349,   349,   349,   349,   349,
   349,   349,   349,   349,   349,   350,   350,   350,   350,   351,
   351,   352,   352,   353,   353,   353,   353,   353,   353,   353,
   353,   353,   353,   354,   354,   354,   355,   355,   356,   356
};

static const short yyr2[] = {     0,
     1,     2,     0,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     0,     4,     3,     1,     1,     3,     2,     0,     1,
     4,     3,     3,     2,     5,     2,     1,     0,     5,     4,
     5,     2,     3,     2,     7,     2,     8,     2,     3,     2,
    10,     2,     1,     0,     0,     4,     2,     0,     3,     1,
     1,     1,     0,     0,     4,     2,     0,     3,     1,     1,
     1,     0,     4,     1,     0,     4,     1,     0,     0,     4,
     2,     0,     3,     1,     1,     0,     0,     4,     2,     0,
     3,     1,     1,     1,     3,     2,     3,     4,     4,     3,
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
     0,     2,     1,     1,     1,     0,     7,     3,     1,     5,
     2,     0,     2,     0,     2,     1,     6,     8,     6,     8,
     3,     1,     2,     1,     2,     1,     2,     2,     1,     2,
     3,     1,     2,     1,     2,     1,     2,     2,     1,     2,
     1,     2,     3,     1,     0,     0,     5,     0,     4,     1,
     0,     2,     1,     1,     1,     0,     7,     3,     1,     5,
     2,     0,     2,     0,     2,     1,     6,     8,     6,     8,
     3,     1,     2,     1,     2,     1,     2,     2,     1,     2,
     3,     1,     2,     1,     2,     1,     2,     2,     1,     2,
     1,     2,     3,     4,     5,     2,     6,     5,     3,     1,
     1,     1,     6,     4,     3,     1,     6,     4,     1,     3,
     3,     3,     3,     3,     3,     3,     2,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     2,     3,     3,     3,
     2,     3,     3,     5,     3,     4,     6,     8,    10,     1,
     1,     1,     1,     1,     1,     2,     3,     3,     2,     1,
     1,     1,     1,     1,     3,     4,     1,     1,     1,     1,
     3,     3,     2,     2,     3,     3,     3,     4,     3,     3,
     0,     7,     0,     9,     1,     0,     2,     1,     2,     4,
     0,     8,     0,    10,     0,     8,     0,    10,     0,     8,
     0,    10,     0,     4,     0,     1,     0,     2,     1,     4,
     1,     1,     1,     1,     1,     1,     0,     4,     0,     1,
     0,     2,     1,     0,     8,     0,     7,     1,     1,     2,
     2,     0,     1,     2,     1,     1,     1,     1,     2,     2,
     2,     2,     1,     1,     1,     3,     3,     2,     0,     4,
     0,     2,     0,     1,     1,     1,     2,     1,     2,     1,
     1,     1,     1,     1,     2,     1,     1,     0,     1,     1
};

static const short yydefact[] = {     3,
     1,    22,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     2,     4,     9,    29,     5,     0,    10,    54,     6,
     0,    11,    54,     7,     0,    12,   175,     8,     0,    13,
    14,    15,    16,    17,    18,    19,    20,    21,     0,    42,
    28,    48,     0,   518,     0,     0,     0,     0,     0,     0,
     0,    30,    38,     0,    78,    53,    63,    78,    63,     0,
   201,   174,   177,   188,    26,     0,    25,     0,   517,   425,
     0,   429,   441,     0,   427,   445,     0,     0,   426,   449,
     0,   430,   431,   418,   419,   405,   417,   411,   412,   366,
     0,     0,     0,     0,   414,     0,     0,     0,     0,   518,
   361,   362,   360,   369,   400,   401,   402,   403,   404,   410,
   413,   420,   518,    27,     0,     0,     0,    37,    55,     0,
   105,    77,     0,    86,    62,   105,    86,     0,   178,     0,
     0,   200,   203,   176,     0,   214,   187,   190,    23,     0,
   518,     0,   455,     0,   455,   428,     0,   455,     0,   436,
   423,   424,     0,     0,     0,   406,     0,   409,     0,     0,
     0,   387,   391,   377,     0,    32,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   518,
    34,   518,     0,   518,     0,    58,    79,     0,     0,   104,
    64,     0,   123,    85,   141,   123,     0,   183,     0,   204,
   518,   354,   202,     0,   191,     0,     0,   213,   216,   189,
    24,   355,   443,     0,   469,   447,   469,   451,   469,   433,
     0,     0,   435,   438,   395,     0,   365,     0,     0,   407,
   408,     0,   415,   370,   378,   379,   380,   381,   382,   383,
   384,   385,   386,   388,   389,   390,   392,   393,     0,   371,
   372,   373,   374,   375,   376,   421,   422,    33,    31,     0,
    35,     0,    61,    56,     0,    60,    82,   106,   518,    41,
    67,    87,     0,     0,   122,     0,   255,   140,   158,   180,
   186,   179,     0,   185,     0,   209,   356,     0,   196,     0,
   217,   518,   215,   455,   453,     0,     0,   455,     0,   455,
     0,   436,   439,     0,   518,   437,     0,   396,     0,   364,
     0,   368,   416,     0,    40,     0,    57,     0,    94,     0,
    80,     0,    84,    93,   109,   518,    44,    70,    65,     0,
    69,    90,   124,   518,   142,     0,    72,   254,     0,   305,
   157,   183,   182,     0,   206,     0,   205,     0,   211,   193,
   199,   192,     0,   198,     0,   222,   357,   469,   457,   467,
   518,   469,   518,   469,   518,     0,   514,   516,     0,     0,
   432,     0,     0,     0,   394,    39,    59,     0,    96,    81,
     0,     0,     0,   107,   108,    43,    66,     0,    88,     0,
    92,   127,    45,   145,     0,   258,     0,     0,    71,   159,
     0,    75,   304,   181,   184,   209,     0,   208,     0,   196,
   195,     0,   219,     0,   218,   221,   224,     0,     0,   454,
   456,   459,   482,   442,     0,   446,     0,   450,   518,   515,
   440,     0,   397,   363,   367,    97,    95,   100,   101,   102,
   103,    83,   117,     0,   115,   121,     0,   119,     0,     0,
    68,    89,     0,     0,   125,   126,     0,   143,   144,   147,
   256,   261,     0,   518,    47,   162,     0,   308,     0,     0,
    74,   207,   212,   210,   194,   197,   222,     0,   223,   518,
     0,   458,   478,   479,   468,   482,   473,     0,   518,   518,
   434,     0,    98,    99,     0,   111,     0,     0,   113,     0,
     0,     0,    91,   135,     0,   133,   139,     0,   137,     0,
     0,     0,   146,   261,   266,   289,   259,   260,   263,   264,
   265,     0,     0,   282,     0,   518,    50,     0,   160,   161,
   164,   306,   311,     0,   518,   220,     0,   444,   462,   463,
   461,   465,   466,   464,     0,   480,   481,   472,   476,   494,
   495,   485,   486,   487,   488,   493,     0,     0,   448,   452,
     0,   398,   116,   114,   120,   118,   110,   112,     0,   129,
     0,     0,   131,     0,     0,     0,   518,   257,     0,   290,
     0,   288,   262,     0,     0,     0,    73,    49,     0,   163,
   311,   316,   339,   309,   310,   313,   314,   315,     0,     0,
   332,     0,    51,   225,   460,   484,   499,   489,   490,   491,
   492,   474,     0,   134,   132,   138,   136,   128,   130,     0,
   518,   148,     0,     0,   269,     0,     0,   299,   272,     0,
   292,     0,   272,     0,   286,   281,   518,   307,     0,   340,
   338,   312,     0,     0,     0,    76,     0,   501,   499,   399,
   518,     0,   150,   175,     0,     0,     0,   359,   302,   300,
   298,     0,   274,     0,     0,     0,   274,     0,   287,   285,
     0,   518,   165,     0,     0,   319,   349,   322,     0,   342,
     0,   322,     0,   336,   331,     0,     0,   498,     0,   503,
   501,   152,   518,   175,   201,     0,     0,   276,   268,   271,
     0,     0,   296,   291,   299,   272,   303,     0,   272,   518,
     0,   167,   188,     0,     0,     0,   350,   348,   352,     0,
   324,     0,     0,     0,   324,     0,   337,   335,   496,   497,
     0,     0,     0,   503,   175,   154,   201,   149,   227,     0,
   518,   275,   273,   520,   519,   279,   297,   295,   274,   277,
   274,   169,   518,   188,   214,     0,     0,   326,   318,   321,
     0,     0,   346,   341,   349,   322,   353,     0,   322,     0,
   506,   508,   513,   504,   505,   511,   512,     0,   510,   502,
   477,     0,   201,   175,   151,   251,   270,   267,     0,     0,
   188,   171,   214,   166,   239,     0,     0,   325,   323,   329,
   347,   345,   324,   327,   324,   500,   507,   509,   475,   153,
   201,     0,     0,   226,   156,   280,   278,   214,   188,   168,
   253,   320,   317,     0,     0,   155,     0,   228,     0,   170,
   214,     0,     0,   238,   173,   330,   328,     0,   233,     0,
   172,     0,   240,     0,   230,   237,   229,     0,   235,   250,
     0,   245,     0,   233,   236,   232,     0,   242,   249,   241,
     0,   247,   252,   231,   234,   245,   248,   244,     0,   243,
   246,     0,     0,     0
};

static const short yydefgoto[] = {   882,
     1,    12,    13,    39,    66,    67,    14,    15,    51,    52,
   114,    16,    17,   117,   118,    18,    19,   280,    20,    21,
    22,    23,   475,    24,    25,    55,    56,   196,   274,   275,
   276,   124,   125,   281,   339,   340,   341,   408,   409,   480,
   481,   121,   122,   277,   331,   332,   203,   204,   342,   399,
   400,   333,   334,   199,   200,   335,   394,   395,   454,   455,
   457,   458,   284,   285,   402,   465,   466,   515,   516,   518,
   519,   287,   288,   404,   468,   469,   470,   664,   704,   745,
   794,   748,   350,   351,   476,   539,   540,   541,   723,   764,
   801,   829,   804,    61,    62,    63,   208,   352,   292,   293,
   294,   136,   137,   138,   299,   420,   362,   363,   364,   749,
   132,   133,   296,   416,   357,   358,   359,   805,   218,   219,
   366,   487,   425,   426,   427,   796,   824,   849,   864,   857,
   858,   859,   831,   844,   862,   876,   870,   871,   872,   825,
   845,   347,   348,   524,   472,   527,   528,   529,   530,   589,
   634,   635,   673,   712,   707,   531,   532,   533,   646,   534,
   639,   640,   714,   641,   642,   412,   413,   601,   543,   604,
   605,   606,   607,   649,   685,   686,   731,   772,   767,   608,
   609,   610,   695,   611,   688,   689,   774,   690,   691,    26,
    27,   212,    28,    29,   671,   100,   101,   102,   103,   104,
   105,   106,   107,   108,   109,   110,   111,   112,    30,    31,
    32,    33,    34,    35,   150,   312,   232,   233,   234,    36,
   143,   304,    37,   145,   308,    38,   148,   310,   225,   369,
   430,   431,   432,   555,   307,   433,   495,   496,   497,   659,
   617,   498,   568,   658,   700,   743,   790,   380,    70,   756
};

static const short yypact[] = {-32768,
   290,-32768,    24,    26,    36,   131,     1,   181,     0,   185,
   210,-32768,-32768,-32768,   175,-32768,   153,-32768,   234,-32768,
   177,-32768,   234,-32768,   188,-32768,   240,-32768,   199,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   238,   222,
   257,   274,   195,   196,   -29,   115,   291,   116,   145,   327,
   303,   307,   314,   230,   367,-32768,   351,   367,   351,    64,
   383,   240,-32768,   386,-32768,    87,-32768,   375,-32768,-32768,
   388,-32768,-32768,   392,-32768,-32768,   334,   413,-32768,-32768,
   415,-32768,-32768,   425,   429,   362,-32768,-32768,-32768,   364,
   371,    19,   205,   394,   398,   344,   344,   344,   344,   196,
-32768,-32768,   722,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   172,   106,-32768,   327,   384,   406,   473,-32768,   433,
   509,-32768,   435,   521,-32768,   509,   521,   327,-32768,   129,
   506,   383,-32768,-32768,   133,   524,   386,-32768,-32768,   238,
   196,   451,   500,   461,   500,-32768,   465,   500,   466,   526,
-32768,-32768,   247,   266,   530,-32768,   532,-32768,   533,   534,
   464,-32768,-32768,-32768,   634,-32768,   344,   344,   344,   344,
   344,   344,   344,   344,   344,   344,   344,   344,   344,   344,
   344,   344,   344,   344,   344,   344,   344,   536,   537,   196,
-32768,   196,   327,   196,   487,   539,-32768,   502,   575,-32768,
-32768,   512,   593,-32768,   592,   593,   515,   571,   327,-32768,
   196,-32768,-32768,   327,-32768,   134,   507,   524,-32768,-32768,
-32768,   510,-32768,   519,   553,-32768,   553,-32768,   553,-32768,
    67,   513,   526,-32768,-32768,   380,-32768,   419,    81,-32768,
-32768,   508,-32768,-32768,   445,   445,   445,   445,   445,   445,
   772,   772,   772,   462,   462,   462,   389,   389,   504,   305,
   305,   517,   517,   517,   517,-32768,-32768,-32768,-32768,   518,
-32768,   327,-32768,-32768,   160,-32768,    16,-32768,   124,-32768,
   582,-32768,   529,   541,-32768,   543,   607,-32768,   610,-32768,
-32768,-32768,   194,-32768,   550,   613,-32768,   560,   616,   327,
-32768,   196,-32768,   500,-32768,   563,   554,   500,   555,   500,
   556,   526,-32768,   -11,   196,-32768,   344,-32768,   344,-32768,
   530,-32768,-32768,   344,-32768,   562,-32768,   539,   568,   625,
-32768,   227,-32768,-32768,   292,   196,-32768,-32768,-32768,   231,
-32768,    16,-32768,   196,-32768,    50,   648,-32768,   573,   651,
-32768,   571,-32768,   571,-32768,   574,-32768,   235,-32768,-32768,
-32768,-32768,   239,-32768,   590,   629,-32768,   553,   647,-32768,
   196,   553,   196,   553,   196,   584,-32768,-32768,   658,   597,
-32768,   552,   670,   589,   747,-32768,-32768,   497,-32768,-32768,
    16,   661,   662,-32768,   333,-32768,-32768,   582,-32768,   601,
-32768,   340,-32768,   664,   611,-32768,   614,   676,-32768,-32768,
    58,   691,-32768,-32768,-32768,   613,   327,-32768,   613,   616,
-32768,   616,-32768,   617,-32768,   629,-32768,   621,   630,-32768,
   647,-32768,   -27,-32768,   632,-32768,   633,-32768,   196,-32768,
-32768,   344,-32768,-32768,-32768,   669,   686,-32768,-32768,-32768,
-32768,-32768,   635,   261,-32768,   637,   287,-32768,   661,   662,
-32768,    16,   699,   700,-32768,   374,   665,-32768,   664,-32768,
-32768,   141,   714,   132,-32768,   701,   668,-32768,   677,   659,
-32768,-32768,-32768,-32768,-32768,-32768,   629,   327,-32768,   196,
   284,-32768,   719,   723,-32768,   -22,-32768,   379,   196,   196,
-32768,   595,-32768,-32768,   682,-32768,   661,   689,-32768,   662,
   296,   311,-32768,   704,   318,-32768,   707,   322,-32768,   699,
   700,   776,-32768,   141,-32768,   107,-32768,   141,-32768,-32768,
-32768,   404,   727,-32768,   730,   196,-32768,   745,-32768,   701,
-32768,-32768,   220,   799,   196,-32768,   739,-32768,   425,   429,
-32768,-32768,-32768,   172,   752,-32768,-32768,-32768,    -9,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   341,   815,-32768,-32768,
   344,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   748,-32768,
   699,   749,-32768,   700,   332,   336,    46,-32768,   821,-32768,
   327,   755,-32768,   148,   148,   825,-32768,-32768,   826,-32768,
   220,-32768,   149,-32768,   220,-32768,-32768,-32768,   422,   764,
-32768,   773,-32768,-32768,-32768,-32768,   767,-32768,-32768,-32768,
-32768,-32768,   696,-32768,-32768,-32768,-32768,-32768,-32768,   836,
    -6,-32768,   771,   852,   777,   774,   327,    -5,   854,   770,
-32768,    25,   854,    32,   -21,-32768,    52,-32768,   843,-32768,
   775,-32768,   845,   845,   846,-32768,   -15,   827,   767,-32768,
   196,   848,-32768,   240,   327,   849,   821,-32768,-32768,-32768,
   781,   327,   873,   853,   855,   327,   873,   855,-32768,-32768,
   856,    17,-32768,   786,   868,   794,   157,   871,   790,-32768,
    20,   871,    23,   -17,-32768,   792,   793,-32768,   795,   796,
   827,-32768,   196,   240,   383,   878,   214,-32768,-32768,-32768,
   863,   339,    92,-32768,   208,   854,-32768,   339,   854,   196,
   866,-32768,   386,   327,   867,   843,-32768,   800,-32768,   327,
   890,   870,   872,   327,   890,   872,-32768,-32768,-32768,-32768,
   874,   197,   809,   796,   240,-32768,   383,-32768,-32768,   327,
   196,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   873,-32768,
   873,-32768,   196,   386,   524,   888,   221,-32768,-32768,-32768,
   875,   813,   108,-32768,   209,   871,-32768,   816,   871,   806,
   425,   429,-32768,-32768,-32768,-32768,-32768,   454,   172,-32768,
-32768,   817,   383,   240,-32768,   250,-32768,-32768,   339,   339,
   386,-32768,   524,-32768,-32768,   327,   818,-32768,-32768,-32768,
-32768,-32768,   890,-32768,   890,-32768,-32768,-32768,-32768,-32768,
   383,   135,   828,-32768,-32768,-32768,-32768,   524,   386,-32768,
   256,-32768,-32768,   819,   822,-32768,   327,-32768,   882,-32768,
   524,   139,   831,-32768,-32768,-32768,-32768,   832,   887,   829,
-32768,   327,-32768,   889,-32768,   830,-32768,   418,-32768,-32768,
   834,   892,   833,   887,-32768,-32768,   887,-32768,   830,-32768,
   423,-32768,-32768,-32768,-32768,   892,-32768,-32768,   892,-32768,
-32768,   923,   927,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   788,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   907,-32768,-32768,-32768,-32768,
   604,   876,-32768,-32768,-32768,-32768,   535,-32768,-32768,-32768,
-32768,   879,-32768,-32768,-32768,-32768,   807,-32768,-32768,-32768,
-32768,   545,  -327,   812,-32768,-32768,-32768,-32768,   480,   434,
   482,   430,   737,-32768,-32768,-32768,-32768,   424,   365,   426,
   361,-32768,-32768,-32768,-32768,-32768,   479,-32768,-32768,-32768,
-32768,  -718,-32768,-32768,-32768,-32768,-32768,   409,-32768,-32768,
-32768,-32768,  -641,  -657,-32768,   891,-32768,-32768,   598,-32768,
   600,  -693,-32768,   814,-32768,-32768,   538,-32768,   540,   894,
-32768,   820,-32768,-32768,   544,-32768,   542,   823,-32768,   738,
-32768,-32768,   470,-32768,   546,-32768,-32768,-32768,-32768,    99,
-32768,    97,-32768,-32768,-32768,-32768,    89,-32768,    88,-32768,
-32768,-32768,-32768,-32768,-32768,   442,-32768,   440,-32768,-32768,
   302,-32768,  -633,  -655,-32768,  -631,-32768,-32768,-32768,-32768,
  -586,-32768,-32768,-32768,   376,-32768,-32768,-32768,-32768,   369,
-32768,   368,-32768,-32768,   248,-32768,  -674,  -704,-32768,  -701,
-32768,-32768,-32768,-32768,  -626,-32768,-32768,-32768,   321,-32768,
-32768,-32768,-32768,-32768,  -506,  -115,-32768,-32768,   -53,-32768,
-32768,-32768,-32768,-32768,-32768,  -152,-32768,  -489,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   666,-32768,   743,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -140,-32768,
-32768,-32768,   548,-32768,  -223,-32768,-32768,-32768,   481,-32768,
-32768,-32768,-32768,   323,   279,   237,-32768,-32768,   -99,  -656
};


#define	YYLAST		982


static const short yytable[] = {   192,
   166,   554,   239,   309,   227,   311,   705,   229,   643,   677,
  -301,  -301,   207,   191,   401,   662,   696,   735,   697,   592,
   377,   718,   378,   768,  -483,  -301,    71,   692,   795,   765,
   778,   493,   494,    46,   708,   733,   493,   494,   721,   736,
   675,   222,   162,   163,   164,   165,   747,   679,   678,   329,
    44,   737,   156,   734,    47,   676,   734,    40,   330,    41,
   157,   760,   676,   670,   405,   808,    72,   630,    73,    42,
   803,  -471,   477,   681,   820,   752,  -470,   270,   379,   591,
   631,   616,   759,   591,   128,   761,   698,   793,   716,    69,
   268,   719,   269,   295,   271,   591,   651,  -294,   298,   236,
   238,   813,   836,   799,   815,   800,   776,   828,   834,   779,
   835,   297,    69,   245,   246,   247,   248,   249,   250,   251,
   252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
   262,   263,   264,   265,   513,   841,   821,   406,   680,   190,
   682,    69,   826,   827,   428,   478,   591,    69,   435,   209,
   437,   129,   591,   214,   300,   837,   326,   336,   525,   852,
   757,   830,   313,   368,    43,   536,   314,   372,   384,   374,
    74,    78,  -351,  -351,   526,   590,   811,   321,   637,   337,
   728,   638,   139,   140,   365,   322,   840,   738,    84,    85,
    86,    87,   591,    88,    89,    90,    91,    92,    93,   851,
    81,    69,   367,   188,   189,    50,   758,   591,   591,  -284,
    75,    79,    76,    80,    45,   381,   210,   650,    48,    69,
   215,   301,   838,    94,    95,   727,   853,    69,   781,   782,
   783,   784,   785,   786,   787,    96,   396,   602,   158,    97,
    82,   751,    83,    49,   403,   159,    98,   526,   807,   591,
    53,  -334,   789,   603,   603,   327,   328,   591,   822,  -344,
    99,   823,    54,   382,   842,   383,   812,   843,   728,    60,
   385,   434,    65,   436,    57,   438,   670,   727,    84,    85,
    86,    87,    68,    88,    89,    59,   788,    92,    93,   353,
   354,    69,     2,     3,     4,     5,    64,    84,    85,    86,
    87,   483,    88,    89,   392,   393,    92,    93,   591,   591,
  -294,  -344,     6,    94,    95,   549,   550,   119,   551,   -46,
   552,   553,   390,   391,    77,    96,   397,   398,   113,    97,
   418,   419,    94,    95,   421,   422,    98,   115,     7,   501,
     8,     9,    10,    11,    96,   459,   460,   116,    97,   865,
    99,   235,   463,   464,   -36,    98,   506,   507,    84,    85,
    86,    87,   877,    88,    89,    90,    91,    92,    93,    99,
   237,   -52,   547,   120,   537,    84,    85,    86,    87,   123,
    88,    89,   509,   510,    92,    93,   520,   521,   502,   130,
   548,   577,   507,    94,    95,   184,   185,   186,   187,   569,
   570,   618,   619,   620,   621,    96,   578,   510,   141,    97,
    94,    95,   559,   580,   581,   135,    98,   583,   584,   594,
   595,   142,    96,   560,   561,   144,    97,   628,   581,   146,
    99,   629,   584,    98,   754,   755,   598,   653,   654,   562,
   563,   564,   565,   566,   567,   613,   147,    99,   149,   167,
   168,   169,   170,   171,   172,   173,   174,   175,   151,   176,
   177,   178,   152,   179,   180,   153,   181,   154,   182,   183,
   184,   185,   186,   187,   155,   636,   317,   182,   183,   184,
   185,   186,   187,   193,   318,   817,   818,   632,   167,   168,
   169,   170,   171,   172,   173,   174,   175,   160,   176,   177,
   178,   161,   179,   180,   194,   181,   195,   182,   183,   184,
   185,   186,   187,   866,   867,   319,   198,   623,   878,   879,
   197,   669,   201,   320,   176,   177,   178,   202,   179,   180,
   216,   663,   211,   182,   183,   184,   185,   186,   187,   446,
   447,   448,   449,   450,   451,   179,   180,   683,   223,   706,
   182,   183,   184,   185,   186,   187,   710,   224,   226,   231,
   717,   702,   228,   230,    87,   240,   241,   242,   243,   266,
   267,   729,   273,   167,   168,   169,   170,   171,   172,   173,
   174,   175,   722,   176,   177,   178,   272,   179,   180,   278,
   181,   324,   182,   183,   184,   185,   186,   187,   279,   282,
   283,   286,   290,   746,   291,   302,   305,  -358,   766,   306,
   187,   315,   323,   325,   770,   338,   343,   346,   777,   349,
   762,   167,   168,   169,   170,   171,   172,   173,   174,   175,
   345,   176,   177,   178,   797,   179,   180,   355,   181,   344,
   182,   183,   184,   185,   186,   187,   356,   360,   442,   361,
   370,   798,   371,   373,   375,   388,   443,   386,   389,   407,
   410,   411,   424,   802,   167,   168,   169,   170,   171,   172,
   173,   174,   175,   417,   176,   177,   178,   423,   179,   180,
   429,   181,   439,   182,   183,   184,   185,   186,   187,   440,
   832,   571,   441,   445,   453,   456,   462,   467,   471,   572,
   474,   473,   479,   167,   168,   169,   170,   171,   172,   173,
   174,   175,   503,   176,   177,   178,   488,   179,   180,   490,
   181,   848,   182,   183,   184,   185,   186,   187,   504,   491,
   499,   500,   514,   517,   538,   505,   861,   508,   244,   167,
   168,   169,   170,   171,   172,   173,   174,   175,   535,   176,
   177,   178,   522,   179,   180,   542,   181,   545,   182,   183,
   184,   185,   186,   187,   544,   167,   168,   169,   170,   171,
   172,   173,   174,   175,   444,   176,   177,   178,   556,   179,
   180,   557,   181,   573,   182,   183,   184,   185,   186,   187,
   575,   167,   168,   169,   170,   171,   172,   173,   174,   175,
   660,   176,   177,   178,   579,   179,   180,   582,   181,   587,
   182,   183,   184,   185,   186,   187,   167,   168,   169,   170,
   171,   172,   173,   174,   175,   597,   176,   177,   178,   596,
   179,   180,   599,   612,   614,   182,   183,   184,   185,   186,
   187,   167,   168,   169,   170,   171,   172,   615,   622,   624,
   626,   176,   177,   178,   633,   179,   180,  -283,   645,   647,
   182,   183,   184,   185,   186,   187,   655,   657,   656,   661,
   665,   666,   674,   667,   672,   668,   684,  -333,   687,   694,
   699,   703,   526,  -293,   711,   724,   713,   725,   715,   720,
   726,   730,   732,   739,   740,   742,   750,   753,   741,   763,
   603,   771,  -343,   773,   791,   775,   806,   780,   810,   809,
   816,   814,   819,   833,   846,   839,   850,   847,   854,   855,
   856,   868,   883,   863,   860,   869,   884,   221,   873,    58,
   591,   387,   461,   206,   127,   452,   126,   205,   511,   576,
   574,   512,   289,   585,   627,   625,   586,   523,   600,   414,
   220,   213,   134,   415,   131,   303,   546,   485,   217,   482,
   484,   486,   874,   875,   880,   588,   881,   593,   709,   648,
   644,   489,   652,   769,   693,   316,   558,   376,   492,   744,
   792,   701
};

static const short yycheck[] = {   115,
   100,   491,   155,   227,   145,   229,   664,   148,   595,   643,
    16,    17,   128,   113,   342,    22,    32,   692,    34,   526,
    32,   677,    34,   725,    34,    31,    56,   654,   747,   723,
   735,    59,    60,    34,   666,    16,    59,    60,    22,    17,
    16,   141,    96,    97,    98,    99,   704,    69,    17,    34,
    50,    69,    34,    34,    55,    31,    34,    34,    43,    34,
    42,   718,    31,    69,    15,   767,    96,    22,    98,    34,
   764,    99,    15,    22,   793,   707,    99,   193,    90,   101,
   587,    91,   716,   101,    21,   719,   102,   745,   675,    96,
   190,   678,   192,   209,   194,   101,   603,   103,   214,   153,
   154,   776,   821,   759,   779,   761,   733,   801,   813,   736,
   815,   211,    96,   167,   168,   169,   170,   171,   172,   173,
   174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
   184,   185,   186,   187,   462,   829,   794,    88,   645,    34,
   647,    96,   799,   800,   368,    88,   101,    96,   372,    21,
   374,    88,   101,    21,    21,    21,   272,    34,    18,    21,
    69,   803,    96,   304,    34,    34,   100,   308,   321,   310,
    56,    56,    16,    17,    34,    69,    69,    97,    31,   279,
   687,    34,    96,    97,   300,   105,   828,   694,    32,    33,
    34,    35,   101,    37,    38,    39,    40,    41,    42,   841,
    56,    96,   302,    32,    33,    31,   713,   101,   101,   103,
    96,    96,    98,    98,    34,   315,    88,    69,    34,    96,
    88,    88,    88,    67,    68,    69,    88,    96,    32,    33,
    34,    35,    36,    37,    38,    79,   336,    18,    34,    83,
    96,    28,    98,    34,   344,    41,    90,    34,    28,   101,
    98,   103,   742,    34,    34,    96,    97,   101,     9,   103,
   104,    12,    29,   317,     9,   319,   773,    12,   775,    30,
   324,   371,    35,   373,    98,   375,    69,    69,    32,    33,
    34,    35,    88,    37,    38,    98,    90,    41,    42,    96,
    97,    96,     3,     4,     5,     6,    98,    32,    33,    34,
    35,   417,    37,    38,    13,    14,    41,    42,   101,   101,
   103,   103,    23,    67,    68,    32,    33,    88,    35,    98,
    37,    38,    96,    97,    34,    79,    96,    97,    26,    83,
    96,    97,    67,    68,    96,    97,    90,    31,    49,   439,
    51,    52,    53,    54,    79,    13,    14,    34,    83,   856,
   104,   105,    13,    14,    98,    90,    96,    97,    32,    33,
    34,    35,   869,    37,    38,    39,    40,    41,    42,   104,
   105,    98,   488,     7,   474,    32,    33,    34,    35,    29,
    37,    38,    96,    97,    41,    42,    13,    14,   442,     7,
   490,    96,    97,    67,    68,    91,    92,    93,    94,   499,
   500,    61,    62,    63,    64,    79,    96,    97,    34,    83,
    67,    68,    34,    96,    97,    30,    90,    96,    97,    16,
    17,    34,    79,    45,    46,    34,    83,    96,    97,    96,
   104,    96,    97,    90,    96,    97,   536,    16,    17,    61,
    62,    63,    64,    65,    66,   545,    34,   104,    34,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    34,    80,
    81,    82,    34,    84,    85,   104,    87,   104,    89,    90,
    91,    92,    93,    94,   104,   591,    97,    89,    90,    91,
    92,    93,    94,   100,   105,    32,    33,   587,    70,    71,
    72,    73,    74,    75,    76,    77,    78,   104,    80,    81,
    82,   104,    84,    85,    99,    87,    34,    89,    90,    91,
    92,    93,    94,    96,    97,    97,     8,   571,    96,    97,
    88,   637,    88,   105,    80,    81,    82,     7,    84,    85,
     7,   631,    27,    89,    90,    91,    92,    93,    94,    43,
    44,    45,    46,    47,    48,    84,    85,   647,    98,   665,
    89,    90,    91,    92,    93,    94,   672,    58,    98,    34,
   676,   661,    98,    98,    35,    34,    34,    34,   105,    34,
    34,   687,    34,    70,    71,    72,    73,    74,    75,    76,
    77,    78,   682,    80,    81,    82,   100,    84,    85,    88,
    87,    88,    89,    90,    91,    92,    93,    94,    24,    88,
     8,    10,    88,   703,    34,    99,    88,    98,   724,    57,
    94,    99,   105,    96,   730,    34,    88,    11,   734,    10,
   720,    70,    71,    72,    73,    74,    75,    76,    77,    78,
    88,    80,    81,    82,   750,    84,    85,    88,    87,    99,
    89,    90,    91,    92,    93,    94,    34,    88,    97,    34,
    88,   751,    99,    99,    99,    88,   105,    96,    34,    12,
    88,    11,    34,   763,    70,    71,    72,    73,    74,    75,
    76,    77,    78,   100,    80,    81,    82,    88,    84,    85,
    34,    87,    99,    89,    90,    91,    92,    93,    94,    32,
   806,    97,    96,   105,    34,    34,    96,    34,    88,   105,
    25,    88,    12,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    44,    80,    81,    82,   100,    84,    85,    99,
    87,   837,    89,    90,    91,    92,    93,    94,    43,   100,
    99,    99,    34,    34,    34,   101,   852,   101,   105,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    35,    80,
    81,    82,    88,    84,    85,    88,    87,    99,    89,    90,
    91,    92,    93,    94,    88,    70,    71,    72,    73,    74,
    75,    76,    77,    78,   105,    80,    81,    82,    60,    84,
    85,    59,    87,   102,    89,    90,    91,    92,    93,    94,
   102,    70,    71,    72,    73,    74,    75,    76,    77,    78,
   105,    80,    81,    82,   101,    84,    85,   101,    87,    34,
    89,    90,    91,    92,    93,    94,    70,    71,    72,    73,
    74,    75,    76,    77,    78,    96,    80,    81,    82,   103,
    84,    85,    88,    35,    96,    89,    90,    91,    92,    93,
    94,    70,    71,    72,    73,    74,    75,    96,    34,   102,
   102,    80,    81,    82,    34,    84,    85,   103,    34,    34,
    89,    90,    91,    92,    93,    94,   103,   101,    96,    34,
   100,    20,   103,    97,    21,   102,    34,   103,    34,    34,
    54,    34,    34,   103,    12,   100,    34,    20,    34,    34,
    97,    21,   103,   102,   102,   100,    19,    35,   104,    34,
    34,    12,   103,    34,    96,    34,    19,    34,    96,    35,
   105,    96,    96,    96,    96,    88,    35,    96,    88,    88,
    34,    88,     0,    35,    96,    34,     0,   140,    96,    23,
   101,   328,   398,   127,    59,   391,    58,   126,   459,   510,
   507,   460,   206,   520,   584,   581,   521,   469,   540,   352,
   137,   132,    62,   354,    61,   218,   487,   420,   136,   416,
   419,   422,   864,   867,   876,   524,   879,   528,   667,   601,
   595,   426,   605,   726,   654,   233,   496,   312,   431,   701,
   744,   659
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
#line 255 "ned.y"
{ if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); ;
    break;}
case 6:
#line 257 "ned.y"
{ if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 7:
#line 259 "ned.y"
{ if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 8:
#line 261 "ned.y"
{ if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); ;
    break;}
case 9:
#line 264 "ned.y"
{ if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); ;
    break;}
case 10:
#line 266 "ned.y"
{ if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 11:
#line 268 "ned.y"
{ if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 12:
#line 270 "ned.y"
{ if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); ;
    break;}
case 18:
#line 279 "ned.y"
{ if (ps.storeSourceCode) ps.enump->setSourceCode(toString(yylsp[0])); ;
    break;}
case 19:
#line 281 "ned.y"
{ if (ps.storeSourceCode) ps.messagep->setSourceCode(toString(yylsp[0])); ;
    break;}
case 20:
#line 283 "ned.y"
{ if (ps.storeSourceCode) ps.classp->setSourceCode(toString(yylsp[0])); ;
    break;}
case 21:
#line 285 "ned.y"
{ if (ps.storeSourceCode) ps.structp->setSourceCode(toString(yylsp[0])); ;
    break;}
case 22:
#line 293 "ned.y"
{
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                ;
    break;}
case 23:
#line 298 "ned.y"
{
                  // NOTE: no setTrailingComment(ps.imports,@3) -- comment already stored with last filename */
                ;
    break;}
case 26:
#line 310 "ned.y"
{
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                ;
    break;}
case 28:
#line 326 "ned.y"
{
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 31:
#line 340 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 32:
#line 346 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 33:
#line 355 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 34:
#line 359 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 35:
#line 371 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 36:
#line 378 "ned.y"
{
                  use_chanattrname_token = 0;
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 39:
#line 393 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 40:
#line 399 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 42:
#line 419 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 43:
#line 429 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 44:
#line 433 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 45:
#line 447 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 46:
#line 454 "ned.y"
{
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 48:
#line 478 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 49:
#line 488 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 50:
#line 492 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 51:
#line 509 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 52:
#line 516 "ned.y"
{
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 55:
#line 534 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 56:
#line 539 "ned.y"
{
                ;
    break;}
case 61:
#line 555 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 64:
#line 572 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 65:
#line 577 "ned.y"
{
                ;
    break;}
case 70:
#line 593 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 73:
#line 610 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 76:
#line 625 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 79:
#line 640 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 80:
#line 645 "ned.y"
{
                ;
    break;}
case 83:
#line 656 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 84:
#line 660 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 87:
#line 675 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 88:
#line 680 "ned.y"
{
                ;
    break;}
case 91:
#line 691 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 92:
#line 695 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 94:
#line 710 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                ;
    break;}
case 95:
#line 714 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                ;
    break;}
case 96:
#line 718 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                ;
    break;}
case 97:
#line 722 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                ;
    break;}
case 98:
#line 726 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 99:
#line 730 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 100:
#line 734 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                ;
    break;}
case 101:
#line 738 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                ;
    break;}
case 102:
#line 742 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_XML);
                ;
    break;}
case 103:
#line 746 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                ;
    break;}
case 106:
#line 761 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 107:
#line 766 "ned.y"
{
                ;
    break;}
case 116:
#line 789 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 117:
#line 794 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 120:
#line 807 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 121:
#line 812 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 124:
#line 828 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 125:
#line 833 "ned.y"
{
                ;
    break;}
case 134:
#line 856 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 135:
#line 861 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 138:
#line 874 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 139:
#line 879 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 142:
#line 895 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 143:
#line 900 "ned.y"
{
                ;
    break;}
case 148:
#line 916 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 149:
#line 922 "ned.y"
{
                ;
    break;}
case 150:
#line 925 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 151:
#line 932 "ned.y"
{
                ;
    break;}
case 152:
#line 935 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 153:
#line 941 "ned.y"
{
                ;
    break;}
case 154:
#line 944 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 155:
#line 951 "ned.y"
{
                ;
    break;}
case 159:
#line 971 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 160:
#line 976 "ned.y"
{
                ;
    break;}
case 165:
#line 992 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 166:
#line 998 "ned.y"
{
                ;
    break;}
case 167:
#line 1001 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 168:
#line 1008 "ned.y"
{
                ;
    break;}
case 169:
#line 1011 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 170:
#line 1017 "ned.y"
{
                ;
    break;}
case 171:
#line 1020 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 172:
#line 1027 "ned.y"
{
                ;
    break;}
case 178:
#line 1052 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 179:
#line 1057 "ned.y"
{
                ;
    break;}
case 180:
#line 1060 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 181:
#line 1066 "ned.y"
{
                ;
    break;}
case 186:
#line 1082 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 191:
#line 1103 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 192:
#line 1108 "ned.y"
{
                ;
    break;}
case 193:
#line 1111 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 194:
#line 1117 "ned.y"
{
                ;
    break;}
case 199:
#line 1133 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 204:
#line 1154 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 205:
#line 1159 "ned.y"
{
                ;
    break;}
case 206:
#line 1162 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 207:
#line 1168 "ned.y"
{
                ;
    break;}
case 212:
#line 1185 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                ;
    break;}
case 217:
#line 1207 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 218:
#line 1212 "ned.y"
{
                ;
    break;}
case 219:
#line 1215 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 220:
#line 1221 "ned.y"
{
                ;
    break;}
case 225:
#line 1238 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-3]);
                  addExpression(ps.substparam, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparam,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 228:
#line 1255 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 229:
#line 1260 "ned.y"
{
                ;
    break;}
case 230:
#line 1263 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 231:
#line 1269 "ned.y"
{
                ;
    break;}
case 236:
#line 1285 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 237:
#line 1292 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 240:
#line 1308 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 241:
#line 1313 "ned.y"
{
                ;
    break;}
case 242:
#line 1316 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 243:
#line 1322 "ned.y"
{
                ;
    break;}
case 248:
#line 1338 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 249:
#line 1345 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 250:
#line 1356 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 252:
#line 1367 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 256:
#line 1383 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 257:
#line 1389 "ned.y"
{
                ;
    break;}
case 258:
#line 1392 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 259:
#line 1398 "ned.y"
{
                ;
    break;}
case 266:
#line 1419 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;
    break;}
case 267:
#line 1424 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 270:
#line 1438 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 271:
#line 1448 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 273:
#line 1456 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 277:
#line 1469 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 278:
#line 1474 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 279:
#line 1479 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 280:
#line 1485 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 283:
#line 1499 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 284:
#line 1505 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 285:
#line 1513 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 286:
#line 1518 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 287:
#line 1522 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 288:
#line 1530 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 289:
#line 1537 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 290:
#line 1543 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 293:
#line 1558 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 294:
#line 1563 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 295:
#line 1570 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 296:
#line 1575 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 297:
#line 1579 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 298:
#line 1587 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 299:
#line 1592 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 300:
#line 1596 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 301:
#line 1605 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 302:
#line 1610 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 303:
#line 1615 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 306:
#line 1631 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 307:
#line 1637 "ned.y"
{
                ;
    break;}
case 308:
#line 1640 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 309:
#line 1646 "ned.y"
{
                ;
    break;}
case 316:
#line 1667 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;
    break;}
case 317:
#line 1672 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 320:
#line 1686 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 321:
#line 1696 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 323:
#line 1704 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 327:
#line 1717 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 328:
#line 1722 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 329:
#line 1727 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 330:
#line 1733 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 333:
#line 1747 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 334:
#line 1753 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 335:
#line 1761 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 336:
#line 1766 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 337:
#line 1770 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 338:
#line 1778 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 339:
#line 1785 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 340:
#line 1791 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 343:
#line 1806 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 344:
#line 1811 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 345:
#line 1818 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 346:
#line 1823 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 347:
#line 1827 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 348:
#line 1835 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 349:
#line 1840 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 350:
#line 1844 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 351:
#line 1853 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 352:
#line 1858 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 353:
#line 1863 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 355:
#line 1881 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 356:
#line 1890 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 357:
#line 1904 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 358:
#line 1912 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 359:
#line 1925 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 360:
#line 1931 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 361:
#line 1935 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 362:
#line 1939 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 363:
#line 1950 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 364:
#line 1952 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 365:
#line 1954 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 366:
#line 1956 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 367:
#line 1961 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 368:
#line 1963 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-1]); ;
    break;}
case 370:
#line 1969 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 371:
#line 1972 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 372:
#line 1974 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 373:
#line 1976 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 374:
#line 1978 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 375:
#line 1980 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 376:
#line 1982 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 377:
#line 1986 "ned.y"
{ if (ps.parseExpressions) yyval = unaryMinus(yyvsp[0]); ;
    break;}
case 378:
#line 1989 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 379:
#line 1991 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 380:
#line 1993 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 381:
#line 1995 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 382:
#line 1997 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 383:
#line 1999 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 384:
#line 2002 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 385:
#line 2004 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 386:
#line 2006 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 387:
#line 2010 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 388:
#line 2013 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 389:
#line 2015 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 390:
#line 2017 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 391:
#line 2021 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 392:
#line 2023 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 393:
#line 2025 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 394:
#line 2027 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 395:
#line 2030 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 396:
#line 2032 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 397:
#line 2034 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 398:
#line 2036 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 399:
#line 2038 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-9]), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 405:
#line 2051 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 406:
#line 2056 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                ;
    break;}
case 407:
#line 2061 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 408:
#line 2067 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 409:
#line 2073 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 411:
#line 2085 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 412:
#line 2087 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 414:
#line 2096 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 415:
#line 2098 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 416:
#line 2100 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 417:
#line 2105 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 418:
#line 2110 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 419:
#line 2112 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 420:
#line 2114 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 425:
#line 2131 "ned.y"
{
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(yylsp[-1])));
                  setComments(ps.cplusplus,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 426:
#line 2140 "ned.y"
{
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(yylsp[-1]));
                  setComments(ps.structdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 427:
#line 2149 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(yylsp[-1]));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 428:
#line 2156 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(yylsp[-1]));
                  setComments(ps.classdecl,yylsp[-3],yylsp[-2]);
                ;
    break;}
case 429:
#line 2166 "ned.y"
{
                  ps.messagedecl = (MessageDeclNode *)createNodeWithTag(NED_MESSAGE_DECL, ps.nedfile );
                  ps.messagedecl->setName(toString(yylsp[-1]));
                  setComments(ps.messagedecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 430:
#line 2175 "ned.y"
{
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString(yylsp[-1]));
                  setComments(ps.enumdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 431:
#line 2184 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 432:
#line 2191 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 433:
#line 2195 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 434:
#line 2203 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 439:
#line 2220 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-1],yylsp[-1]);
                ;
    break;}
case 440:
#line 2226 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 441:
#line 2236 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 442:
#line 2242 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 443:
#line 2246 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 444:
#line 2253 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 445:
#line 2260 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 446:
#line 2266 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 447:
#line 2270 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 448:
#line 2277 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 449:
#line 2284 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 450:
#line 2290 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 451:
#line 2294 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 452:
#line 2301 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 453:
#line 2308 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 460:
#line 2328 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 467:
#line 2347 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 474:
#line 2367 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                ;
    break;}
case 475:
#line 2375 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 476:
#line 2379 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                ;
    break;}
case 477:
#line 2386 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 478:
#line 2393 "ned.y"
{ ps.isAbstract = true; ps.isReadonly = false; ;
    break;}
case 479:
#line 2395 "ned.y"
{ ps.isAbstract = false; ps.isReadonly = true; ;
    break;}
case 480:
#line 2397 "ned.y"
{ ps.isAbstract = true; ps.isReadonly = true; ;
    break;}
case 481:
#line 2399 "ned.y"
{ ps.isAbstract = true; ps.isReadonly = true; ;
    break;}
case 482:
#line 2401 "ned.y"
{ ps.isAbstract = false; ps.isReadonly = false; ;
    break;}
case 496:
#line 2426 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 497:
#line 2431 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 498:
#line 2436 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 500:
#line 2444 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 502:
#line 2452 "ned.y"
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
#line 2481 "ned.y"


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
