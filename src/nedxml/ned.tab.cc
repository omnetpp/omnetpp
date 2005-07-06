
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



#define	YYFINAL		882
#define	YYFLAG		-32768
#define	YYNTBASE	105

#define YYTRANSLATE(x) ((unsigned)(x) <= 340 ? yytranslate[x] : 355)

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
  1506,  1507,  1510,  1512,  1513,  1521,  1522,  1531,  1533,  1536,
  1538,  1540,  1542,  1544,  1547,  1550,  1553,  1556,  1558,  1560,
  1562,  1566,  1570,  1573,  1574,  1579,  1580,  1583,  1584,  1586,
  1588,  1590,  1593,  1595,  1598,  1600,  1602,  1604,  1606,  1608,
  1611,  1613,  1615,  1616,  1618
};

static const short yyrhs[] = {   106,
     0,   106,   107,     0,     0,   108,     0,   117,     0,   124,
     0,   129,     0,   298,     0,   112,     0,   121,     0,   126,
     0,   295,     0,   314,     0,   315,     0,   316,     0,   317,
     0,   318,     0,   319,     0,   325,     0,   328,     0,   331,
     0,     0,     3,   109,   110,    95,     0,   110,    96,   111,
     0,   111,     0,    35,     0,   113,   114,   116,     0,     5,
    34,     0,     0,   115,     0,   115,    31,   301,   353,     0,
    31,   301,   353,     0,    26,    34,   353,     0,    26,   353,
     0,   118,    97,   119,    98,   353,     0,     5,    34,     0,
   120,     0,     0,   120,    34,    99,   301,    95,     0,    34,
    99,   301,    95,     0,   122,   131,   147,   159,   123,     0,
     4,    34,     0,    24,    34,   353,     0,    24,   353,     0,
   125,    97,   137,   152,   168,    98,   353,     0,     4,    34,
     0,   127,   131,   147,   159,   177,   247,   143,   128,     0,
     6,    34,     0,    25,    34,   353,     0,    25,   353,     0,
   130,    97,   137,   152,   168,   188,   271,   145,    98,   353,
     0,     6,    34,     0,   132,     0,     0,     0,    29,    87,
   133,   134,     0,   135,    95,     0,     0,   135,    96,   136,
     0,   136,     0,    34,     0,   138,     0,     0,     0,    29,
    87,   139,   140,     0,   141,    95,     0,     0,   141,    96,
   142,     0,   142,     0,    34,     0,   144,     0,     0,    12,
    87,    35,    95,     0,   146,     0,     0,    12,    87,    35,
    95,     0,   148,     0,     0,     0,     7,    87,   149,   150,
     0,   151,    95,     0,     0,   151,    96,   157,     0,   157,
     0,   153,     0,     0,     0,     7,    87,   154,   155,     0,
   156,    95,     0,     0,   156,    95,   158,     0,   158,     0,
   158,     0,    34,     0,    34,    87,    44,     0,    43,    34,
     0,    34,    87,    43,     0,    34,    87,    43,    44,     0,
    34,    87,    44,    43,     0,    34,    87,    45,     0,    34,
    87,    46,     0,    34,    87,    47,     0,    34,    87,    48,
     0,   160,     0,     0,     0,     8,    87,   161,   162,     0,
   163,     0,     0,   163,    13,   164,    95,     0,    13,   164,
    95,     0,   163,    14,   166,    95,     0,    14,   166,    95,
     0,   164,    96,   165,     0,   165,     0,    34,   100,   101,
     0,    34,     0,   166,    96,   167,     0,   167,     0,    34,
   100,   101,     0,    34,     0,   169,     0,     0,     0,     8,
    87,   170,   171,     0,   172,     0,     0,   172,    13,   173,
    95,     0,    13,   173,    95,     0,   172,    14,   175,    95,
     0,    14,   175,    95,     0,   173,    96,   174,     0,   174,
     0,    34,   100,   101,     0,    34,     0,   175,    96,   176,
     0,   176,     0,    34,   100,   101,     0,    34,     0,   178,
     0,     0,     0,    10,    87,   179,   180,     0,   181,     0,
     0,   181,   182,     0,   182,     0,     0,    34,    87,    34,
   353,   183,   199,   187,     0,     0,    34,    87,    34,   300,
   353,   184,   199,   187,     0,     0,    34,    87,    34,    22,
    34,   353,   185,   199,   187,     0,     0,    34,    87,    34,
   300,    22,    34,   353,   186,   199,   187,     0,   215,   231,
   245,     0,   189,     0,     0,     0,    10,    87,   190,   191,
     0,   192,     0,     0,   192,   193,     0,   193,     0,     0,
    34,    87,    34,   353,   194,   207,   198,     0,     0,    34,
    87,    34,   300,   353,   195,   207,   198,     0,     0,    34,
    87,    34,    22,    34,   353,   196,   207,   198,     0,     0,
    34,    87,    34,   300,    22,    34,   353,   197,   207,   198,
     0,   223,   238,   246,     0,   200,     0,     0,   200,   201,
     0,   201,     0,     0,    30,    87,   202,   204,     0,     0,
    30,    21,   301,    87,   203,   204,     0,   205,    95,     0,
     0,   205,    96,   206,     0,   206,     0,    34,     0,   208,
     0,     0,   208,   209,     0,   209,     0,     0,    30,    87,
   210,   212,     0,     0,    30,    21,   301,    87,   211,   212,
     0,   213,    95,     0,     0,   213,    96,   214,     0,   214,
     0,    34,     0,   216,     0,     0,   216,   217,     0,   217,
     0,     0,     7,    87,   218,   220,     0,     0,     7,    21,
   301,    87,   219,   220,     0,   221,    95,     0,     0,   221,
    96,   222,     0,   222,     0,    34,    99,   301,     0,   224,
     0,     0,   224,   225,     0,   225,     0,     0,     7,    87,
   226,   228,     0,     0,     7,    21,   301,    87,   227,   228,
     0,   229,     0,     0,   229,   230,     0,   230,     0,    34,
    99,   301,    95,     0,   231,   232,     0,     0,     0,     9,
    87,   233,   235,     0,     0,     9,    21,   301,    87,   234,
   235,     0,   236,    95,     0,     0,   236,    96,   237,     0,
   237,     0,    34,   300,     0,    34,     0,   238,   239,     0,
     0,     0,     9,    87,   240,   242,     0,     0,     9,    21,
   301,    87,   241,   242,     0,   243,    95,     0,     0,   243,
    96,   244,     0,   244,     0,    34,   300,     0,    34,     0,
    12,    87,    35,    95,     0,     0,    12,    87,    35,    95,
     0,     0,   248,     0,     0,     0,    11,    15,    87,   249,
   251,     0,     0,    11,    87,   250,   251,     0,   252,     0,
     0,   252,   253,     0,   253,     0,   254,     0,   261,     0,
     0,    18,   255,   256,    20,   260,    28,   353,     0,   257,
    96,   256,     0,   257,     0,    34,    99,   301,    19,   301,
     0,    21,   301,     0,     0,    12,    35,     0,     0,   260,
   261,     0,   261,     0,   262,    17,   266,   258,   259,   354,
     0,   262,    17,   270,    17,   266,   258,   259,   354,     0,
   262,    16,   266,   258,   259,   354,     0,   262,    16,   270,
    16,   266,   258,   259,   354,     0,   263,   102,   264,     0,
   265,     0,    34,   300,     0,    34,     0,    34,   300,     0,
    34,     0,    34,    68,     0,    34,   300,     0,    34,     0,
    34,    68,     0,   267,   102,   268,     0,   269,     0,    34,
   300,     0,    34,     0,    34,   300,     0,    34,     0,    34,
    68,     0,    34,   300,     0,    34,     0,    34,    68,     0,
    34,     0,    31,   301,     0,   270,    31,   301,     0,   272,
     0,     0,     0,    11,    15,    87,   273,   275,     0,     0,
    11,    87,   274,   275,     0,   276,     0,     0,   276,   277,
     0,   277,     0,   278,     0,   285,     0,     0,    18,   279,
   280,    20,   284,    28,    95,     0,   281,    96,   280,     0,
   281,     0,    34,    99,   301,    19,   301,     0,    21,   301,
     0,     0,    12,    35,     0,     0,   284,   285,     0,   285,
     0,   286,    17,   290,   282,   283,    95,     0,   286,    17,
   294,    17,   290,   282,   283,    95,     0,   286,    16,   290,
   282,   283,    95,     0,   286,    16,   294,    16,   290,   282,
   283,    95,     0,   287,   102,   288,     0,   289,     0,    34,
   300,     0,    34,     0,    34,   300,     0,    34,     0,    34,
    68,     0,    34,   300,     0,    34,     0,    34,    68,     0,
   291,   102,   292,     0,   293,     0,    34,   300,     0,    34,
     0,    34,   300,     0,    34,     0,    34,    68,     0,    34,
   300,     0,    34,     0,    34,    68,     0,    34,     0,    34,
   301,     0,   294,    34,   301,     0,   296,   199,   215,   297,
     0,    23,    34,    87,    34,   353,     0,    27,   353,     0,
   299,    97,   207,   223,    98,   353,     0,    23,    34,    87,
    34,   353,     0,   100,   301,   101,     0,   304,     0,   302,
     0,   303,     0,    39,   103,   304,    96,   304,   104,     0,
    39,   103,   304,   104,     0,    39,   103,   104,     0,    39,
     0,    40,   103,   311,    96,   311,   104,     0,    40,   103,
   311,   104,     0,   305,     0,   103,   304,   104,     0,   304,
    88,   304,     0,   304,    89,   304,     0,   304,    90,   304,
     0,   304,    91,   304,     0,   304,    92,   304,     0,   304,
    93,   304,     0,    89,   304,     0,   304,    69,   304,     0,
   304,    70,   304,     0,   304,    71,   304,     0,   304,    72,
   304,     0,   304,    73,   304,     0,   304,    74,   304,     0,
   304,    75,   304,     0,   304,    76,   304,     0,   304,    77,
   304,     0,    78,   304,     0,   304,    79,   304,     0,   304,
    80,   304,     0,   304,    81,   304,     0,    82,   304,     0,
   304,    83,   304,     0,   304,    84,   304,     0,   304,    86,
   304,    87,   304,     0,    34,   103,   104,     0,    34,   103,
   304,   104,     0,    34,   103,   304,    96,   304,   104,     0,
    34,   103,   304,    96,   304,    96,   304,   104,     0,    34,
   103,   304,    96,   304,    96,   304,    96,   304,   104,     0,
   306,     0,   307,     0,   308,     0,   309,     0,   310,     0,
    34,     0,    41,    34,     0,    41,    42,    34,     0,    42,
    41,    34,     0,    42,    34,     0,   311,     0,    37,     0,
    38,     0,   312,     0,    67,     0,    67,   103,   104,     0,
    66,   103,    34,   104,     0,    35,     0,    32,     0,    33,
     0,   313,     0,   313,    32,    34,     0,   313,    33,    34,
     0,    32,    34,     0,    33,    34,     0,    49,    50,   353,
     0,    53,    34,    95,     0,    52,    34,    95,     0,    52,
    55,    34,    95,     0,    51,    34,    95,     0,    54,    34,
    95,     0,     0,    54,    34,    97,   320,   322,    98,   353,
     0,     0,    54,    34,    56,    34,    97,   321,   322,    98,
   353,     0,   323,     0,     0,   323,   324,     0,   324,     0,
    34,    95,     0,    34,    99,   352,    95,     0,     0,    51,
    34,    97,   326,   334,   340,    98,   353,     0,     0,    51,
    34,    56,    34,    97,   327,   334,   340,    98,   353,     0,
     0,    52,    34,    97,   329,   334,   340,    98,   353,     0,
     0,    52,    34,    56,    34,    97,   330,   334,   340,    98,
   353,     0,     0,    53,    34,    97,   332,   334,   340,    98,
   353,     0,     0,    53,    34,    56,    34,    97,   333,   334,
   340,    98,   353,     0,     0,    58,    87,   335,   336,     0,
     0,   337,     0,     0,   337,   338,     0,   338,     0,    34,
    99,   339,    95,     0,    35,     0,    32,     0,    33,     0,
   313,     0,    37,     0,    38,     0,     0,    57,    87,   341,
   342,     0,     0,   343,     0,     0,   343,   344,     0,   344,
     0,     0,   347,    34,   345,   348,   349,   350,    95,     0,
     0,    59,   347,    34,   346,   348,   349,   350,    95,     0,
    34,     0,    34,    90,     0,    60,     0,    61,     0,    62,
     0,    63,     0,    65,    60,     0,    65,    61,     0,    65,
    62,     0,    65,    63,     0,    64,     0,    45,     0,    46,
     0,   100,    32,   101,     0,   100,    34,   101,     0,   100,
   101,     0,     0,    54,   103,    34,   104,     0,     0,    99,
   351,     0,     0,    35,     0,    36,     0,    32,     0,    89,
    32,     0,    33,     0,    89,    33,     0,   313,     0,    37,
     0,    38,     0,    34,     0,    32,     0,    89,    32,     0,
    34,     0,    95,     0,     0,    96,     0,    95,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   239,   243,   244,   248,   250,   252,   254,   256,   259,   261,
   263,   265,   268,   269,   270,   271,   272,   274,   276,   278,
   280,   288,   293,   300,   301,   305,   317,   321,   330,   331,
   335,   341,   350,   354,   364,   373,   383,   384,   388,   394,
   406,   414,   424,   428,   438,   449,   462,   473,   483,   487,
   497,   511,   524,   525,   529,   535,   540,   541,   545,   546,
   550,   562,   563,   567,   573,   578,   579,   583,   584,   588,
   600,   601,   605,   615,   616,   620,   630,   631,   635,   641,
   646,   647,   651,   655,   665,   666,   670,   676,   681,   682,
   686,   690,   701,   705,   709,   713,   717,   721,   725,   729,
   733,   737,   741,   751,   752,   756,   762,   767,   768,   772,
   773,   774,   775,   779,   780,   784,   789,   797,   798,   802,
   807,   818,   819,   823,   829,   834,   835,   839,   840,   841,
   842,   846,   847,   851,   856,   864,   865,   869,   874,   885,
   886,   890,   896,   901,   902,   906,   907,   911,   917,   920,
   927,   930,   936,   939,   946,   952,   961,   962,   966,   972,
   977,   978,   982,   983,   987,   993,   996,  1003,  1006,  1012,
  1015,  1022,  1028,  1037,  1038,  1042,  1043,  1047,  1053,  1055,
  1062,  1067,  1068,  1072,  1073,  1077,  1088,  1089,  1093,  1094,
  1098,  1104,  1106,  1113,  1118,  1119,  1123,  1124,  1128,  1139,
  1140,  1144,  1145,  1149,  1155,  1157,  1164,  1170,  1171,  1175,
  1176,  1180,  1192,  1193,  1197,  1198,  1202,  1208,  1210,  1217,
  1223,  1224,  1228,  1229,  1233,  1245,  1246,  1250,  1256,  1258,
  1265,  1270,  1271,  1275,  1276,  1280,  1287,  1298,  1299,  1303,
  1309,  1311,  1318,  1323,  1324,  1328,  1329,  1333,  1340,  1351,
  1355,  1362,  1366,  1373,  1374,  1378,  1385,  1387,  1394,  1399,
  1400,  1404,  1405,  1409,  1410,  1414,  1419,  1428,  1429,  1433,
  1443,  1447,  1451,  1455,  1459,  1460,  1464,  1469,  1474,  1480,
  1489,  1490,  1494,  1500,  1508,  1513,  1517,  1525,  1532,  1538,
  1548,  1549,  1553,  1558,  1565,  1570,  1574,  1582,  1587,  1591,
  1600,  1605,  1610,  1621,  1622,  1626,  1633,  1635,  1642,  1647,
  1648,  1652,  1653,  1657,  1658,  1662,  1667,  1676,  1677,  1681,
  1691,  1695,  1699,  1703,  1707,  1708,  1712,  1717,  1722,  1728,
  1737,  1738,  1742,  1748,  1756,  1761,  1765,  1773,  1780,  1786,
  1796,  1797,  1801,  1806,  1813,  1818,  1822,  1830,  1835,  1839,
  1848,  1853,  1858,  1869,  1876,  1885,  1896,  1907,  1920,  1925,
  1930,  1934,  1945,  1947,  1949,  1951,  1956,  1958,  1963,  1964,
  1967,  1969,  1971,  1973,  1975,  1977,  1980,  1984,  1986,  1988,
  1990,  1992,  1994,  1997,  1999,  2001,  2004,  2008,  2010,  2012,
  2015,  2018,  2020,  2022,  2025,  2027,  2029,  2031,  2033,  2038,
  2039,  2040,  2041,  2042,  2046,  2051,  2056,  2062,  2068,  2076,
  2080,  2082,  2087,  2091,  2093,  2095,  2100,  2105,  2107,  2109,
  2115,  2116,  2117,  2118,  2126,  2135,  2144,  2151,  2161,  2170,
  2179,  2186,  2190,  2198,  2205,  2206,  2210,  2211,  2215,  2221,
  2231,  2237,  2241,  2248,  2255,  2261,  2265,  2272,  2279,  2285,
  2289,  2296,  2303,  2309,  2309,  2313,  2314,  2318,  2319,  2323,
  2333,  2334,  2335,  2336,  2337,  2338,  2342,  2348,  2348,  2352,
  2353,  2357,  2358,  2362,  2368,  2372,  2379,  2386,  2387,  2389,
  2390,  2391,  2392,  2394,  2395,  2396,  2397,  2399,  2400,  2401,
  2406,  2411,  2416,  2420,  2424,  2428,  2432,  2436,  2440,  2441,
  2442,  2443,  2444,  2445,  2446,  2447,  2448,  2449,  2453,  2454,
  2455,  2458,  2458,  2460,  2460
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
"numconst","timeconstant","cplusplus","struct_decl","class_decl","message_decl",
"enum_decl","enum","@36","@37","opt_enumfields","enumfields","enumfield","message",
"@38","@39","class","@40","@41","struct","@42","@43","opt_propertiesblock","@44",
"opt_properties","properties","property","propertyvalue","opt_fieldsblock","@45",
"opt_fields","fields","field","@46","@47","fielddatatype","opt_fieldvector",
"opt_fieldenum","opt_fieldvalue","fieldvalue","enumvalue","opt_semicolon","comma_or_semicolon", NULL
};
#endif

static const short yyr1[] = {     0,
   105,   106,   106,   107,   107,   107,   107,   107,   107,   107,
   107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
   107,   109,   108,   110,   110,   111,   112,   113,   114,   114,
   115,   115,   116,   116,   117,   118,   119,   119,   120,   120,
   121,   122,   123,   123,   124,   125,   126,   127,   128,   128,
   129,   130,   131,   131,   133,   132,   134,   134,   135,   135,
   136,   137,   137,   139,   138,   140,   140,   141,   141,   142,
   143,   143,   144,   145,   145,   146,   147,   147,   149,   148,
   150,   150,   151,   151,   152,   152,   154,   153,   155,   155,
   156,   156,   157,   158,   158,   158,   158,   158,   158,   158,
   158,   158,   158,   159,   159,   161,   160,   162,   162,   163,
   163,   163,   163,   164,   164,   165,   165,   166,   166,   167,
   167,   168,   168,   170,   169,   171,   171,   172,   172,   172,
   172,   173,   173,   174,   174,   175,   175,   176,   176,   177,
   177,   179,   178,   180,   180,   181,   181,   183,   182,   184,
   182,   185,   182,   186,   182,   187,   188,   188,   190,   189,
   191,   191,   192,   192,   194,   193,   195,   193,   196,   193,
   197,   193,   198,   199,   199,   200,   200,   202,   201,   203,
   201,   204,   204,   205,   205,   206,   207,   207,   208,   208,
   210,   209,   211,   209,   212,   212,   213,   213,   214,   215,
   215,   216,   216,   218,   217,   219,   217,   220,   220,   221,
   221,   222,   223,   223,   224,   224,   226,   225,   227,   225,
   228,   228,   229,   229,   230,   231,   231,   233,   232,   234,
   232,   235,   235,   236,   236,   237,   237,   238,   238,   240,
   239,   241,   239,   242,   242,   243,   243,   244,   244,   245,
   245,   246,   246,   247,   247,   249,   248,   250,   248,   251,
   251,   252,   252,   253,   253,   255,   254,   256,   256,   257,
   258,   258,   259,   259,   260,   260,   261,   261,   261,   261,
   262,   262,   263,   263,   264,   264,   264,   265,   265,   265,
   266,   266,   267,   267,   268,   268,   268,   269,   269,   269,
   270,   270,   270,   271,   271,   273,   272,   274,   272,   275,
   275,   276,   276,   277,   277,   279,   278,   280,   280,   281,
   282,   282,   283,   283,   284,   284,   285,   285,   285,   285,
   286,   286,   287,   287,   288,   288,   288,   289,   289,   289,
   290,   290,   291,   291,   292,   292,   292,   293,   293,   293,
   294,   294,   294,   295,   296,   297,   298,   299,   300,   301,
   301,   301,   302,   302,   302,   302,   303,   303,   304,   304,
   304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
   304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
   304,   304,   304,   304,   304,   304,   304,   304,   304,   305,
   305,   305,   305,   305,   306,   306,   306,   306,   306,   307,
   308,   308,   309,   310,   310,   310,   311,   312,   312,   312,
   313,   313,   313,   313,   314,   315,   316,   316,   317,   318,
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
     0,     2,     1,     0,     7,     0,     8,     1,     2,     1,
     1,     1,     1,     2,     2,     2,     2,     1,     1,     1,
     3,     3,     2,     0,     4,     0,     2,     0,     1,     1,
     1,     2,     1,     2,     1,     1,     1,     1,     1,     2,
     1,     1,     0,     1,     1
};

static const short yydefact[] = {     3,
     1,    22,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     2,     4,     9,    29,     5,     0,    10,    54,     6,
     0,    11,    54,     7,     0,    12,   175,     8,     0,    13,
    14,    15,    16,    17,    18,    19,    20,    21,     0,    42,
    28,    48,     0,   513,     0,     0,     0,     0,     0,     0,
     0,    30,    38,     0,    78,    53,    63,    78,    63,     0,
   201,   174,   177,   188,    26,     0,    25,     0,   512,   425,
     0,   429,   441,     0,   427,   445,     0,     0,   426,   449,
     0,   430,   431,   418,   419,   405,   417,   411,   412,   366,
     0,     0,     0,     0,   414,     0,     0,     0,     0,   513,
   361,   362,   360,   369,   400,   401,   402,   403,   404,   410,
   413,   420,   513,    27,     0,     0,     0,    37,    55,     0,
   105,    77,     0,    86,    62,   105,    86,     0,   178,     0,
     0,   200,   203,   176,     0,   214,   187,   190,    23,     0,
   513,     0,   455,     0,   455,   428,     0,   455,     0,   436,
   423,   424,     0,     0,     0,   406,     0,   409,     0,     0,
     0,   387,   391,   377,     0,    32,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   513,
    34,   513,     0,   513,     0,    58,    79,     0,     0,   104,
    64,     0,   123,    85,   141,   123,     0,   183,     0,   204,
   513,   354,   202,     0,   191,     0,     0,   213,   216,   189,
    24,   355,   443,     0,   469,   447,   469,   451,   469,   433,
     0,     0,   435,   438,   395,     0,   365,     0,     0,   407,
   408,     0,   415,   370,   378,   379,   380,   381,   382,   383,
   384,   385,   386,   388,   389,   390,   392,   393,     0,   371,
   372,   373,   374,   375,   376,   421,   422,    33,    31,     0,
    35,     0,    61,    56,     0,    60,    82,   106,   513,    41,
    67,    87,     0,     0,   122,     0,   255,   140,   158,   180,
   186,   179,     0,   185,     0,   209,   356,     0,   196,     0,
   217,   513,   215,   455,   453,     0,     0,   455,     0,   455,
     0,   436,   439,     0,   513,   437,     0,   396,     0,   364,
     0,   368,   416,     0,    40,     0,    57,     0,    94,     0,
    80,     0,    84,    93,   109,   513,    44,    70,    65,     0,
    69,    90,   124,   513,   142,     0,    72,   254,     0,   305,
   157,   183,   182,     0,   206,     0,   205,     0,   211,   193,
   199,   192,     0,   198,     0,   222,   357,   469,   457,   467,
   513,   469,   513,   469,   513,     0,   509,   511,     0,     0,
   432,     0,     0,     0,   394,    39,    59,     0,    96,    81,
     0,     0,     0,   107,   108,    43,    66,     0,    88,     0,
    92,   127,    45,   145,     0,   258,     0,     0,    71,   159,
     0,    75,   304,   181,   184,   209,     0,   208,     0,   196,
   195,     0,   219,     0,   218,   221,   224,     0,     0,   454,
   456,   459,   471,   442,     0,   446,     0,   450,   513,   510,
   440,     0,   397,   363,   367,    97,    95,   100,   101,   102,
   103,    83,   117,     0,   115,   121,     0,   119,     0,     0,
    68,    89,     0,     0,   125,   126,     0,   143,   144,   147,
   256,   261,     0,   513,    47,   162,     0,   308,     0,     0,
    74,   207,   212,   210,   194,   197,   222,     0,   223,   513,
     0,   458,   478,   489,   490,     0,   480,   481,   482,   483,
   488,     0,   468,   470,   473,     0,   513,   513,   434,     0,
    98,    99,     0,   111,     0,     0,   113,     0,     0,     0,
    91,   135,     0,   133,   139,     0,   137,     0,     0,     0,
   146,   261,   266,   289,   259,   260,   263,   264,   265,     0,
     0,   282,     0,   513,    50,     0,   160,   161,   164,   306,
   311,     0,   513,   220,     0,   444,   462,   463,   461,   465,
   466,   464,     0,   479,     0,   484,   485,   486,   487,   472,
   474,   448,   452,     0,   398,   116,   114,   120,   118,   110,
   112,     0,   129,     0,     0,   131,     0,     0,     0,   513,
   257,     0,   290,     0,   288,   262,     0,     0,     0,    73,
    49,     0,   163,   311,   316,   339,   309,   310,   313,   314,
   315,     0,     0,   332,     0,    51,   225,   460,   476,   494,
     0,   134,   132,   138,   136,   128,   130,     0,   513,   148,
     0,     0,   269,     0,     0,   299,   272,     0,   292,     0,
   272,     0,   286,   281,   513,   307,     0,   340,   338,   312,
     0,     0,     0,    76,   494,     0,   496,   399,   513,     0,
   150,   175,     0,     0,     0,   359,   302,   300,   298,     0,
   274,     0,     0,     0,   274,     0,   287,   285,     0,   513,
   165,     0,     0,   319,   349,   322,     0,   342,     0,   322,
     0,   336,   331,   496,     0,     0,   493,     0,   498,   152,
   513,   175,   201,     0,     0,   276,   268,   271,     0,     0,
   296,   291,   299,   272,   303,     0,   272,   513,     0,   167,
   188,     0,     0,     0,   350,   348,   352,     0,   324,     0,
     0,     0,   324,     0,   337,   335,   498,   491,   492,     0,
     0,     0,   175,   154,   201,   149,   227,     0,   513,   275,
   273,   515,   514,   279,   297,   295,   274,   277,   274,   169,
   513,   188,   214,     0,     0,   326,   318,   321,     0,     0,
   346,   341,   349,   322,   353,     0,   322,     0,     0,   501,
   503,   508,   499,   500,   506,   507,     0,   505,   497,   475,
   201,   175,   151,   251,   270,   267,     0,     0,   188,   171,
   214,   166,   239,     0,     0,   325,   323,   329,   347,   345,
   324,   327,   324,   477,   495,   502,   504,   153,   201,     0,
     0,   226,   156,   280,   278,   214,   188,   168,   253,   320,
   317,     0,     0,   155,     0,   228,     0,   170,   214,     0,
     0,   238,   173,   330,   328,     0,   233,     0,   172,     0,
   240,     0,   230,   237,   229,     0,   235,   250,     0,   245,
     0,   233,   236,   232,     0,   242,   249,   241,     0,   247,
   252,   231,   234,   245,   248,   244,     0,   243,   246,     0,
     0,     0
};

static const short yydefgoto[] = {   880,
     1,    12,    13,    39,    66,    67,    14,    15,    51,    52,
   114,    16,    17,   117,   118,    18,    19,   280,    20,    21,
    22,    23,   475,    24,    25,    55,    56,   196,   274,   275,
   276,   124,   125,   281,   339,   340,   341,   408,   409,   480,
   481,   121,   122,   277,   331,   332,   203,   204,   342,   399,
   400,   333,   334,   199,   200,   335,   394,   395,   454,   455,
   457,   458,   284,   285,   402,   465,   466,   523,   524,   526,
   527,   287,   288,   404,   468,   469,   470,   662,   702,   743,
   792,   746,   350,   351,   476,   547,   548,   549,   721,   762,
   799,   827,   802,    61,    62,    63,   208,   352,   292,   293,
   294,   136,   137,   138,   299,   420,   362,   363,   364,   747,
   132,   133,   296,   416,   357,   358,   359,   803,   218,   219,
   366,   487,   425,   426,   427,   794,   822,   847,   862,   855,
   856,   857,   829,   842,   860,   874,   868,   869,   870,   823,
   843,   347,   348,   532,   472,   535,   536,   537,   538,   592,
   632,   633,   671,   710,   705,   539,   540,   541,   644,   542,
   637,   638,   712,   639,   640,   412,   413,   604,   551,   607,
   608,   609,   610,   647,   683,   684,   729,   770,   765,   611,
   612,   613,   693,   614,   686,   687,   772,   688,   689,    26,
    27,   212,    28,    29,   669,   100,   101,   102,   103,   104,
   105,   106,   107,   108,   109,   110,   111,   112,    30,    31,
    32,    33,    34,    35,   150,   312,   232,   233,   234,    36,
   143,   304,    37,   145,   308,    38,   148,   310,   225,   369,
   430,   431,   432,   563,   307,   433,   503,   504,   505,   620,
   655,   506,   657,   699,   742,   789,   380,    70,   754
};

static const short yypact[] = {-32768,
    28,-32768,    -6,     5,    29,   209,   118,   234,    -8,   254,
   256,-32768,-32768,-32768,   170,-32768,    87,-32768,   230,-32768,
   166,-32768,   230,-32768,   184,-32768,   221,-32768,   202,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   259,   206,
   211,   213,   219,   220,   -29,   107,   283,   149,   159,   321,
   300,   297,   299,   257,   350,-32768,   345,   350,   345,    -4,
   363,   221,-32768,   356,-32768,   -34,-32768,   362,-32768,-32768,
   373,-32768,-32768,   389,-32768,-32768,   295,   391,-32768,-32768,
   394,-32768,-32768,   407,   441,   379,-32768,-32768,-32768,   419,
   420,   130,   132,   422,   424,   360,   360,   360,   360,   220,
-32768,-32768,   747,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   217,    42,-32768,   321,   301,   396,   458,-32768,   437,
   518,-32768,   442,   521,-32768,   518,   521,   321,-32768,   120,
   504,   363,-32768,-32768,   133,   525,   356,-32768,-32768,   259,
   220,   438,   475,   439,   475,-32768,   440,   475,   443,   500,
-32768,-32768,   245,   263,   506,-32768,   505,-32768,   508,   509,
   434,-32768,-32768,-32768,   669,-32768,   360,   360,   360,   360,
   360,   360,   360,   360,   360,   360,   360,   360,   360,   360,
   360,   360,   360,   360,   360,   360,   360,   510,   511,   220,
-32768,   220,   321,   220,   448,   515,-32768,   463,   527,-32768,
-32768,   465,   545,-32768,   544,   545,   469,   523,   321,-32768,
   220,-32768,-32768,   321,-32768,   134,   460,   525,-32768,-32768,
-32768,   464,-32768,   477,   512,-32768,   512,-32768,   512,-32768,
   153,   467,   500,-32768,-32768,   397,-32768,   551,    90,-32768,
-32768,   462,-32768,-32768,   367,   367,   367,   367,   367,   367,
   797,   797,   797,   288,   288,   288,   329,   329,   503,   246,
   246,   470,   470,   470,   470,-32768,-32768,-32768,-32768,   472,
-32768,   321,-32768,-32768,   197,-32768,    16,-32768,   124,-32768,
   528,-32768,   484,   487,-32768,   501,   557,-32768,   587,-32768,
-32768,-32768,   225,-32768,   513,   564,-32768,   514,   565,   321,
-32768,   220,-32768,   475,-32768,   516,   507,   475,   517,   475,
   520,   500,-32768,   144,   220,-32768,   360,-32768,   360,-32768,
   506,-32768,-32768,   360,-32768,   519,-32768,   515,   522,   570,
-32768,   229,-32768,-32768,   271,   220,-32768,-32768,-32768,   236,
-32768,    16,-32768,   220,-32768,     9,   596,-32768,   524,   595,
-32768,   523,-32768,   523,-32768,   530,-32768,   247,-32768,-32768,
-32768,-32768,   251,-32768,   529,   576,-32768,   512,   578,-32768,
   220,   512,   220,   512,   220,   538,-32768,-32768,   606,   550,
-32768,   594,   695,   542,   772,-32768,-32768,   459,-32768,-32768,
    16,   614,   615,-32768,   337,-32768,-32768,   528,-32768,   556,
-32768,   351,-32768,   618,   566,-32768,   567,   631,-32768,-32768,
    23,   645,-32768,-32768,-32768,   564,   321,-32768,   564,   565,
-32768,   565,-32768,   559,-32768,   576,-32768,   561,   562,-32768,
   578,-32768,   370,-32768,   574,-32768,   581,-32768,   220,-32768,
-32768,   360,-32768,-32768,-32768,   616,   638,-32768,-32768,-32768,
-32768,-32768,   588,   273,-32768,   591,   310,-32768,   614,   615,
-32768,    16,   642,   658,-32768,   398,   607,-32768,   618,-32768,
-32768,    22,   660,   128,-32768,   659,   609,-32768,   610,   601,
-32768,-32768,-32768,-32768,-32768,-32768,   576,   321,-32768,   220,
   281,-32768,   611,-32768,-32768,   450,-32768,-32768,-32768,-32768,
-32768,   322,-32768,   370,-32768,   666,   220,   220,-32768,   633,
-32768,-32768,   617,-32768,   614,   626,-32768,   615,   318,   341,
-32768,   628,   344,-32768,   630,   348,-32768,   642,   658,   677,
-32768,    22,-32768,   -45,-32768,    22,-32768,-32768,-32768,   436,
   613,-32768,   636,   220,-32768,   646,-32768,   659,-32768,-32768,
    31,   697,   220,-32768,   639,-32768,   407,   441,-32768,-32768,
-32768,   217,   641,-32768,   713,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   360,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   650,-32768,   642,   653,-32768,   658,   366,   369,    45,
-32768,   722,-32768,   321,   661,-32768,   198,   198,   743,-32768,
-32768,   746,-32768,    31,-32768,   167,-32768,    31,-32768,-32768,
-32768,   481,   680,-32768,   694,-32768,-32768,-32768,-32768,   703,
   721,-32768,-32768,-32768,-32768,-32768,-32768,   774,   -10,-32768,
   707,   795,   733,   731,   321,     4,   813,   748,-32768,    21,
   813,   150,   -27,-32768,    48,-32768,   820,-32768,   755,-32768,
   824,   824,   825,-32768,   703,   117,   818,-32768,   220,   839,
-32768,   221,   321,   840,   722,-32768,-32768,-32768,   773,   321,
   867,   848,   849,   321,   867,   849,-32768,-32768,   850,    -3,
-32768,   792,   872,   798,   158,   874,   791,-32768,   143,   874,
    19,   -14,-32768,   818,   796,   799,-32768,   793,   800,-32768,
   220,   221,   363,   879,   154,-32768,-32768,-32768,   866,   404,
   138,-32768,   189,   813,-32768,   404,   813,   220,   868,-32768,
   356,   321,   869,   820,-32768,   802,-32768,   321,   893,   873,
   875,   321,   893,   875,-32768,-32768,   800,-32768,-32768,   876,
   176,   811,   221,-32768,   363,-32768,-32768,   321,   220,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   867,-32768,   867,-32768,
   220,   356,   525,   889,   155,-32768,-32768,-32768,   877,   816,
   162,-32768,   207,   874,-32768,   819,   874,   821,   809,   407,
   441,-32768,-32768,-32768,-32768,-32768,   476,   217,-32768,-32768,
   363,   221,-32768,   222,-32768,-32768,   404,   404,   356,-32768,
   525,-32768,-32768,   321,   822,-32768,-32768,-32768,-32768,-32768,
   893,-32768,   893,-32768,-32768,-32768,-32768,-32768,   363,   135,
   828,-32768,-32768,-32768,-32768,   525,   356,-32768,   261,-32768,
-32768,   823,   826,-32768,   321,-32768,   884,-32768,   525,   140,
   833,-32768,-32768,-32768,-32768,   835,   890,   830,-32768,   321,
-32768,   888,-32768,   827,-32768,   421,-32768,-32768,   841,   892,
   834,   890,-32768,-32768,   890,-32768,   827,-32768,   423,-32768,
-32768,-32768,-32768,   892,-32768,-32768,   892,-32768,-32768,   930,
   931,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   794,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   909,-32768,-32768,-32768,-32768,
   605,   878,-32768,-32768,-32768,-32768,   537,-32768,-32768,-32768,
-32768,   880,-32768,-32768,-32768,-32768,   812,-32768,-32768,-32768,
-32768,   549,  -327,   810,-32768,-32768,-32768,-32768,   482,   427,
   483,   426,   739,-32768,-32768,-32768,-32768,   418,   364,   425,
   365,-32768,-32768,-32768,-32768,-32768,   478,-32768,-32768,-32768,
-32768,  -716,-32768,-32768,-32768,-32768,-32768,   401,-32768,-32768,
-32768,-32768,  -584,  -654,-32768,   891,-32768,-32768,   598,-32768,
   597,  -691,-32768,   829,-32768,-32768,   535,-32768,   534,   896,
-32768,   831,-32768,-32768,   543,-32768,   539,   832,-32768,   742,
-32768,-32768,   474,-32768,   536,-32768,-32768,-32768,-32768,   102,
-32768,   100,-32768,-32768,-32768,-32768,    93,-32768,    92,-32768,
-32768,-32768,-32768,-32768,-32768,   444,-32768,   435,-32768,-32768,
   305,-32768,  -630,  -650,-32768,  -477,-32768,-32768,-32768,-32768,
  -529,-32768,-32768,-32768,   374,-32768,-32768,-32768,-32768,   371,
-32768,   372,-32768,-32768,   249,-32768,  -672,  -723,-32768,  -701,
-32768,-32768,-32768,-32768,  -592,-32768,-32768,-32768,   325,-32768,
-32768,-32768,-32768,-32768,  -532,  -115,-32768,-32768,   -53,-32768,
-32768,-32768,-32768,-32768,-32768,  -151,-32768,  -488,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   662,-32768,   745,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -139,-32768,
-32768,-32768,   548,-32768,  -222,-32768,-32768,-32768,   479,-32768,
-32768,   485,   327,   290,   248,-32768,-32768,   -99,  -700
};


#define	YYLAST		985


static const short yytable[] = {   192,
   166,   595,   562,   239,   309,   227,   311,   703,   229,   776,
   675,   660,   207,   191,   401,   758,   128,   733,   719,  -301,
  -301,   766,   593,   405,   716,    46,    71,    40,   793,   763,
     2,     3,     4,     5,  -301,   734,   673,   477,    41,   533,
   677,   222,   162,   163,   164,   165,    47,   745,   605,   329,
     6,   674,   732,   735,   594,   534,  -284,   629,   330,   690,
   139,   140,    42,   806,   606,    72,   628,    73,   641,   679,
   801,   668,   594,   649,   818,   190,     7,   270,     8,     9,
    10,    11,   129,   757,    69,   594,   759,   832,   791,   833,
   268,    69,   269,   295,   271,   406,   824,   825,   298,   236,
   238,   811,   834,   594,   813,  -294,   797,   826,   798,   478,
   678,   297,   680,   245,   246,   247,   248,   249,   250,   251,
   252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
   262,   263,   264,   265,   521,   839,    69,   819,   774,    69,
   209,   777,    69,   714,   594,   428,   717,   594,   695,   435,
   696,   437,   726,   214,   300,   835,   326,   336,   731,   736,
   850,   544,    74,   156,   368,   158,   676,    44,   372,   384,
   374,   157,   159,  -351,  -351,   377,   732,   378,   756,   337,
   674,   749,   805,    53,   365,   321,   706,   534,   606,    84,
    85,    86,    87,   322,    88,    89,    90,    91,    92,    93,
    50,    75,   367,    76,    78,   755,   210,   780,   781,   782,
   783,   784,   785,   786,    81,   381,   828,   697,    69,   215,
   301,   836,    69,    94,    95,   725,   851,   750,   635,   809,
   820,   636,   379,   821,   648,    96,   396,   594,   810,    97,
   726,   838,    43,    79,   403,    80,    98,   313,   188,   189,
    60,   314,   788,    82,   849,    83,   668,   594,    54,  -344,
    99,   594,    57,   382,   787,   383,   594,    45,  -334,   840,
   385,   434,   841,   436,   725,   438,    84,    85,    86,    87,
    59,    88,    89,   392,   393,    92,    93,    48,   594,    49,
  -294,   327,   328,    65,    84,    85,    86,    87,    64,    88,
    89,   483,   -46,    92,    93,    68,   594,   -36,  -344,   -52,
    94,    95,   557,   558,    69,   559,    77,   560,   561,   353,
   354,   863,    96,   390,   391,   113,    97,   115,    94,    95,
   397,   398,   116,    98,   875,   184,   185,   186,   187,   509,
    96,   418,   419,   119,    97,   421,   422,    99,   235,   459,
   460,    98,    84,    85,    86,    87,   120,    88,    89,    90,
    91,    92,    93,   463,   464,    99,   237,   514,   515,   130,
   179,   180,   555,   123,   545,   182,   183,   184,   185,   186,
   187,   566,   567,   568,   569,   135,    94,    95,   510,   146,
   556,    84,    85,    86,    87,   141,    88,    89,    96,   193,
    92,    93,    97,   493,   517,   518,   142,   572,   573,    98,
   528,   529,   580,   515,   494,   495,   182,   183,   184,   185,
   186,   187,   144,    99,   147,    94,    95,   149,   496,   497,
   498,   499,   500,   501,   502,   581,   518,    96,   583,   584,
   151,    97,   586,   587,   601,   176,   177,   178,    98,   179,
   180,   597,   598,   616,   182,   183,   184,   185,   186,   187,
   626,   584,    99,   627,   587,   167,   168,   169,   170,   171,
   172,   173,   174,   175,   152,   176,   177,   178,   634,   179,
   180,   153,   181,   493,   182,   183,   184,   185,   186,   187,
   630,   195,   317,   194,   494,   495,   651,   652,   752,   753,
   318,   446,   447,   448,   449,   450,   451,   816,   817,   497,
   498,   499,   500,   501,   502,   864,   865,   876,   877,   667,
   621,   154,   155,   197,   160,   198,   161,   202,   201,   661,
   211,   216,   224,   231,   223,   226,   228,   243,   240,   230,
    87,   241,   242,   266,   267,   681,   272,   704,   273,   278,
   279,   282,   283,   286,   708,   290,   291,   302,   715,   700,
  -358,   338,   187,   305,   315,   323,   325,   346,   306,   727,
   343,   167,   168,   169,   170,   171,   172,   173,   174,   175,
   720,   176,   177,   178,   344,   179,   180,   345,   181,   324,
   182,   183,   184,   185,   186,   187,   349,   356,   361,   355,
   360,   744,   370,   389,   371,   411,   764,   407,   388,   424,
   410,   429,   768,   386,   373,   423,   775,   375,   760,   167,
   168,   169,   170,   171,   172,   173,   174,   175,   417,   176,
   177,   178,   795,   179,   180,   439,   181,   440,   182,   183,
   184,   185,   186,   187,   441,   445,   319,   453,   456,   796,
   462,   467,   471,   473,   320,   474,   479,   488,   490,   511,
   491,   800,   167,   168,   169,   170,   171,   172,   173,   174,
   175,   507,   176,   177,   178,   522,   179,   180,   508,   181,
   512,   182,   183,   184,   185,   186,   187,   513,   830,   442,
   516,   525,   546,   530,   543,   550,   552,   443,   553,   571,
   564,   167,   168,   169,   170,   171,   172,   173,   174,   175,
   590,   176,   177,   178,   599,   179,   180,   576,   181,   846,
   182,   183,   184,   185,   186,   187,   578,   582,   574,   585,
   600,   615,   602,   617,   859,   618,   575,   167,   168,   169,
   170,   171,   172,   173,   174,   175,   619,   176,   177,   178,
   622,   179,   180,   624,   181,   631,   182,   183,   184,   185,
   186,   187,  -283,   167,   168,   169,   170,   171,   172,   173,
   174,   175,   244,   176,   177,   178,   643,   179,   180,   645,
   181,   653,   182,   183,   184,   185,   186,   187,   654,   167,
   168,   169,   170,   171,   172,   173,   174,   175,   444,   176,
   177,   178,   656,   179,   180,   663,   181,   659,   182,   183,
   184,   185,   186,   187,   664,   167,   168,   169,   170,   171,
   172,   173,   174,   175,   658,   176,   177,   178,   665,   179,
   180,   666,   181,   670,   182,   183,   184,   185,   186,   187,
   167,   168,   169,   170,   171,   172,   173,   174,   175,   672,
   176,   177,   178,   682,   179,   180,  -333,   685,   692,   182,
   183,   184,   185,   186,   187,   167,   168,   169,   170,   171,
   172,   698,   701,   534,  -293,   176,   177,   178,   709,   179,
   180,   711,   713,   718,   182,   183,   184,   185,   186,   187,
   722,   723,   730,   724,   728,   740,   738,   748,   741,   739,
   751,   761,   606,  -343,   769,   790,   771,   804,   773,   779,
   808,   807,   815,   812,   837,   814,   831,   844,   848,   852,
   845,   853,   861,   854,   858,   867,   594,   866,   871,   881,
   882,    58,   387,   221,   461,   205,   127,   126,   206,   452,
   519,   577,   520,   579,   289,   588,   531,   623,   603,   414,
   415,   625,   134,   589,   485,   486,   131,   484,   482,   303,
   554,   489,   213,   872,   873,   220,   878,   217,   879,   707,
   596,   642,   767,   376,   646,   591,   691,   316,   492,   650,
   565,   694,   570,   737,   778
};

static const short yycheck[] = {   115,
   100,   534,   491,   155,   227,   145,   229,   662,   148,   733,
   641,    22,   128,   113,   342,   716,    21,   690,    22,    16,
    17,   723,    68,    15,   675,    34,    56,    34,   745,   721,
     3,     4,     5,     6,    31,    17,    16,    15,    34,    18,
    68,   141,    96,    97,    98,    99,    55,   702,    18,    34,
    23,    31,    34,    68,   100,    34,   102,   590,    43,   652,
    95,    96,    34,   765,    34,    95,    22,    97,   598,    22,
   762,    68,   100,   606,   791,    34,    49,   193,    51,    52,
    53,    54,    87,   714,    95,   100,   717,   811,   743,   813,
   190,    95,   192,   209,   194,    87,   797,   798,   214,   153,
   154,   774,   819,   100,   777,   102,   757,   799,   759,    87,
   643,   211,   645,   167,   168,   169,   170,   171,   172,   173,
   174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
   184,   185,   186,   187,   462,   827,    95,   792,   731,    95,
    21,   734,    95,   673,   100,   368,   676,   100,    32,   372,
    34,   374,   685,    21,    21,    21,   272,    34,    16,   692,
    21,    34,    56,    34,   304,    34,    17,    50,   308,   321,
   310,    42,    41,    16,    17,    32,    34,    34,   711,   279,
    31,    28,    28,    97,   300,    96,   664,    34,    34,    32,
    33,    34,    35,   104,    37,    38,    39,    40,    41,    42,
    31,    95,   302,    97,    56,    68,    87,    32,    33,    34,
    35,    36,    37,    38,    56,   315,   801,   101,    95,    87,
    87,    87,    95,    66,    67,    68,    87,   705,    31,    68,
     9,    34,    89,    12,    68,    78,   336,   100,   771,    82,
   773,   826,    34,    95,   344,    97,    89,    95,    32,    33,
    30,    99,   741,    95,   839,    97,    68,   100,    29,   102,
   103,   100,    97,   317,    89,   319,   100,    34,   102,     9,
   324,   371,    12,   373,    68,   375,    32,    33,    34,    35,
    97,    37,    38,    13,    14,    41,    42,    34,   100,    34,
   102,    95,    96,    35,    32,    33,    34,    35,    97,    37,
    38,   417,    97,    41,    42,    87,   100,    97,   102,    97,
    66,    67,    32,    33,    95,    35,    34,    37,    38,    95,
    96,   854,    78,    95,    96,    26,    82,    31,    66,    67,
    95,    96,    34,    89,   867,    90,    91,    92,    93,   439,
    78,    95,    96,    87,    82,    95,    96,   103,   104,    13,
    14,    89,    32,    33,    34,    35,     7,    37,    38,    39,
    40,    41,    42,    13,    14,   103,   104,    95,    96,     7,
    83,    84,   488,    29,   474,    88,    89,    90,    91,    92,
    93,    60,    61,    62,    63,    30,    66,    67,   442,    95,
   490,    32,    33,    34,    35,    34,    37,    38,    78,    99,
    41,    42,    82,    34,    95,    96,    34,   507,   508,    89,
    13,    14,    95,    96,    45,    46,    88,    89,    90,    91,
    92,    93,    34,   103,    34,    66,    67,    34,    59,    60,
    61,    62,    63,    64,    65,    95,    96,    78,    95,    96,
    34,    82,    95,    96,   544,    79,    80,    81,    89,    83,
    84,    16,    17,   553,    88,    89,    90,    91,    92,    93,
    95,    96,   103,    95,    96,    69,    70,    71,    72,    73,
    74,    75,    76,    77,    34,    79,    80,    81,   594,    83,
    84,   103,    86,    34,    88,    89,    90,    91,    92,    93,
   590,    34,    96,    98,    45,    46,    16,    17,    95,    96,
   104,    43,    44,    45,    46,    47,    48,    32,    33,    60,
    61,    62,    63,    64,    65,    95,    96,    95,    96,   635,
   574,   103,   103,    87,   103,     8,   103,     7,    87,   629,
    27,     7,    58,    34,    97,    97,    97,   104,    34,    97,
    35,    34,    34,    34,    34,   645,    99,   663,    34,    87,
    24,    87,     8,    10,   670,    87,    34,    98,   674,   659,
    97,    34,    93,    87,    98,   104,    95,    11,    57,   685,
    87,    69,    70,    71,    72,    73,    74,    75,    76,    77,
   680,    79,    80,    81,    98,    83,    84,    87,    86,    87,
    88,    89,    90,    91,    92,    93,    10,    34,    34,    87,
    87,   701,    87,    34,    98,    11,   722,    12,    87,    34,
    87,    34,   728,    95,    98,    87,   732,    98,   718,    69,
    70,    71,    72,    73,    74,    75,    76,    77,    99,    79,
    80,    81,   748,    83,    84,    98,    86,    32,    88,    89,
    90,    91,    92,    93,    95,   104,    96,    34,    34,   749,
    95,    34,    87,    87,   104,    25,    12,    99,    98,    44,
    99,   761,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    98,    79,    80,    81,    34,    83,    84,    98,    86,
    43,    88,    89,    90,    91,    92,    93,   100,   804,    96,
   100,    34,    34,    87,    35,    87,    87,   104,    98,    34,
    90,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    34,    79,    80,    81,   102,    83,    84,   101,    86,   835,
    88,    89,    90,    91,    92,    93,   101,   100,    96,   100,
    95,    35,    87,    95,   850,    95,   104,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    34,    79,    80,    81,
   101,    83,    84,   101,    86,    34,    88,    89,    90,    91,
    92,    93,   102,    69,    70,    71,    72,    73,    74,    75,
    76,    77,   104,    79,    80,    81,    34,    83,    84,    34,
    86,   102,    88,    89,    90,    91,    92,    93,    95,    69,
    70,    71,    72,    73,    74,    75,    76,    77,   104,    79,
    80,    81,   100,    83,    84,    99,    86,    34,    88,    89,
    90,    91,    92,    93,    20,    69,    70,    71,    72,    73,
    74,    75,    76,    77,   104,    79,    80,    81,    96,    83,
    84,   101,    86,    21,    88,    89,    90,    91,    92,    93,
    69,    70,    71,    72,    73,    74,    75,    76,    77,   102,
    79,    80,    81,    34,    83,    84,   102,    34,    34,    88,
    89,    90,    91,    92,    93,    69,    70,    71,    72,    73,
    74,    54,    34,    34,   102,    79,    80,    81,    12,    83,
    84,    34,    34,    34,    88,    89,    90,    91,    92,    93,
    99,    20,   102,    96,    21,   103,   101,    19,    99,   101,
    35,    34,    34,   102,    12,    95,    34,    19,    34,    34,
    95,    35,   104,    95,    87,    95,    95,    95,    35,    87,
    95,    87,    35,    34,    95,    34,   100,    87,    95,     0,
     0,    23,   328,   140,   398,   126,    59,    58,   127,   391,
   459,   515,   460,   518,   206,   528,   469,   584,   548,   352,
   354,   587,    62,   529,   420,   422,    61,   419,   416,   218,
   487,   426,   132,   862,   865,   137,   874,   136,   877,   665,
   536,   598,   724,   312,   604,   532,   652,   233,   431,   608,
   496,   655,   504,   694,   737
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
#line 251 "ned.y"
{ if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); ;
    break;}
case 6:
#line 253 "ned.y"
{ if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 7:
#line 255 "ned.y"
{ if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 8:
#line 257 "ned.y"
{ if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); ;
    break;}
case 9:
#line 260 "ned.y"
{ if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); ;
    break;}
case 10:
#line 262 "ned.y"
{ if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 11:
#line 264 "ned.y"
{ if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 12:
#line 266 "ned.y"
{ if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); ;
    break;}
case 18:
#line 275 "ned.y"
{ if (ps.storeSourceCode) ps.enump->setSourceCode(toString(yylsp[0])); ;
    break;}
case 19:
#line 277 "ned.y"
{ if (ps.storeSourceCode) ps.messagep->setSourceCode(toString(yylsp[0])); ;
    break;}
case 20:
#line 279 "ned.y"
{ if (ps.storeSourceCode) ps.classp->setSourceCode(toString(yylsp[0])); ;
    break;}
case 21:
#line 281 "ned.y"
{ if (ps.storeSourceCode) ps.structp->setSourceCode(toString(yylsp[0])); ;
    break;}
case 22:
#line 289 "ned.y"
{
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                ;
    break;}
case 23:
#line 294 "ned.y"
{
                  // NOTE: no setTrailingComment(ps.imports,@3) -- comment already stored with last filename */
                ;
    break;}
case 26:
#line 306 "ned.y"
{
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                ;
    break;}
case 28:
#line 322 "ned.y"
{
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 31:
#line 336 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 32:
#line 342 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 33:
#line 351 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 34:
#line 355 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 35:
#line 367 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 36:
#line 374 "ned.y"
{
                  use_chanattrname_token = 0;
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 39:
#line 389 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 40:
#line 395 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 42:
#line 415 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 43:
#line 425 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 44:
#line 429 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 45:
#line 443 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 46:
#line 450 "ned.y"
{
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 48:
#line 474 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 49:
#line 484 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 50:
#line 488 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 51:
#line 505 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 52:
#line 512 "ned.y"
{
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 55:
#line 530 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 56:
#line 535 "ned.y"
{
                ;
    break;}
case 61:
#line 551 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 64:
#line 568 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 65:
#line 573 "ned.y"
{
                ;
    break;}
case 70:
#line 589 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 73:
#line 606 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 76:
#line 621 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 79:
#line 636 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 80:
#line 641 "ned.y"
{
                ;
    break;}
case 83:
#line 652 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 84:
#line 656 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 87:
#line 671 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 88:
#line 676 "ned.y"
{
                ;
    break;}
case 91:
#line 687 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 92:
#line 691 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 94:
#line 706 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                ;
    break;}
case 95:
#line 710 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                ;
    break;}
case 96:
#line 714 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                ;
    break;}
case 97:
#line 718 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                ;
    break;}
case 98:
#line 722 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 99:
#line 726 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 100:
#line 730 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                ;
    break;}
case 101:
#line 734 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                ;
    break;}
case 102:
#line 738 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_XML);
                ;
    break;}
case 103:
#line 742 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                ;
    break;}
case 106:
#line 757 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 107:
#line 762 "ned.y"
{
                ;
    break;}
case 116:
#line 785 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 117:
#line 790 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 120:
#line 803 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 121:
#line 808 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 124:
#line 824 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 125:
#line 829 "ned.y"
{
                ;
    break;}
case 134:
#line 852 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 135:
#line 857 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 138:
#line 870 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 139:
#line 875 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 142:
#line 891 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 143:
#line 896 "ned.y"
{
                ;
    break;}
case 148:
#line 912 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 149:
#line 918 "ned.y"
{
                ;
    break;}
case 150:
#line 921 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 151:
#line 928 "ned.y"
{
                ;
    break;}
case 152:
#line 931 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 153:
#line 937 "ned.y"
{
                ;
    break;}
case 154:
#line 940 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 155:
#line 947 "ned.y"
{
                ;
    break;}
case 159:
#line 967 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 160:
#line 972 "ned.y"
{
                ;
    break;}
case 165:
#line 988 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 166:
#line 994 "ned.y"
{
                ;
    break;}
case 167:
#line 997 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 168:
#line 1004 "ned.y"
{
                ;
    break;}
case 169:
#line 1007 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 170:
#line 1013 "ned.y"
{
                ;
    break;}
case 171:
#line 1016 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 172:
#line 1023 "ned.y"
{
                ;
    break;}
case 178:
#line 1048 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 179:
#line 1053 "ned.y"
{
                ;
    break;}
case 180:
#line 1056 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 181:
#line 1062 "ned.y"
{
                ;
    break;}
case 186:
#line 1078 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 191:
#line 1099 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 192:
#line 1104 "ned.y"
{
                ;
    break;}
case 193:
#line 1107 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 194:
#line 1113 "ned.y"
{
                ;
    break;}
case 199:
#line 1129 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 204:
#line 1150 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 205:
#line 1155 "ned.y"
{
                ;
    break;}
case 206:
#line 1158 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 207:
#line 1164 "ned.y"
{
                ;
    break;}
case 212:
#line 1181 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                ;
    break;}
case 217:
#line 1203 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 218:
#line 1208 "ned.y"
{
                ;
    break;}
case 219:
#line 1211 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 220:
#line 1217 "ned.y"
{
                ;
    break;}
case 225:
#line 1234 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-3]);
                  addExpression(ps.substparam, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparam,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 228:
#line 1251 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 229:
#line 1256 "ned.y"
{
                ;
    break;}
case 230:
#line 1259 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 231:
#line 1265 "ned.y"
{
                ;
    break;}
case 236:
#line 1281 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 237:
#line 1288 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 240:
#line 1304 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 241:
#line 1309 "ned.y"
{
                ;
    break;}
case 242:
#line 1312 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 243:
#line 1318 "ned.y"
{
                ;
    break;}
case 248:
#line 1334 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 249:
#line 1341 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 250:
#line 1352 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 252:
#line 1363 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 256:
#line 1379 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 257:
#line 1385 "ned.y"
{
                ;
    break;}
case 258:
#line 1388 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 259:
#line 1394 "ned.y"
{
                ;
    break;}
case 266:
#line 1415 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;
    break;}
case 267:
#line 1420 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 270:
#line 1434 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 271:
#line 1444 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 273:
#line 1452 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 277:
#line 1465 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 278:
#line 1470 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 279:
#line 1475 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 280:
#line 1481 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 283:
#line 1495 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 284:
#line 1501 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 285:
#line 1509 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 286:
#line 1514 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 287:
#line 1518 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 288:
#line 1526 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 289:
#line 1533 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 290:
#line 1539 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 293:
#line 1554 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 294:
#line 1559 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 295:
#line 1566 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 296:
#line 1571 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 297:
#line 1575 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 298:
#line 1583 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 299:
#line 1588 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 300:
#line 1592 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 301:
#line 1601 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 302:
#line 1606 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 303:
#line 1611 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 306:
#line 1627 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 307:
#line 1633 "ned.y"
{
                ;
    break;}
case 308:
#line 1636 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 309:
#line 1642 "ned.y"
{
                ;
    break;}
case 316:
#line 1663 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;
    break;}
case 317:
#line 1668 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 320:
#line 1682 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 321:
#line 1692 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 323:
#line 1700 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 327:
#line 1713 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 328:
#line 1718 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 329:
#line 1723 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 330:
#line 1729 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 333:
#line 1743 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 334:
#line 1749 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 335:
#line 1757 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 336:
#line 1762 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 337:
#line 1766 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 338:
#line 1774 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 339:
#line 1781 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 340:
#line 1787 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 343:
#line 1802 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 344:
#line 1807 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 345:
#line 1814 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 346:
#line 1819 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 347:
#line 1823 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 348:
#line 1831 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 349:
#line 1836 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 350:
#line 1840 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 351:
#line 1849 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 352:
#line 1854 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 353:
#line 1859 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 355:
#line 1877 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 356:
#line 1886 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 357:
#line 1900 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 358:
#line 1908 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 359:
#line 1921 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 360:
#line 1927 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 361:
#line 1931 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 362:
#line 1935 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 363:
#line 1946 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 364:
#line 1948 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 365:
#line 1950 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 366:
#line 1952 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 367:
#line 1957 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 368:
#line 1959 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-1]); ;
    break;}
case 370:
#line 1965 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 371:
#line 1968 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 372:
#line 1970 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 373:
#line 1972 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 374:
#line 1974 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 375:
#line 1976 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 376:
#line 1978 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 377:
#line 1982 "ned.y"
{ if (ps.parseExpressions) yyval = unaryMinus(yyvsp[0]); ;
    break;}
case 378:
#line 1985 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 379:
#line 1987 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 380:
#line 1989 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 381:
#line 1991 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 382:
#line 1993 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 383:
#line 1995 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 384:
#line 1998 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 385:
#line 2000 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 386:
#line 2002 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 387:
#line 2006 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 388:
#line 2009 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 389:
#line 2011 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 390:
#line 2013 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 391:
#line 2017 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 392:
#line 2019 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 393:
#line 2021 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 394:
#line 2023 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 395:
#line 2026 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 396:
#line 2028 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 397:
#line 2030 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 398:
#line 2032 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 399:
#line 2034 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-9]), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 405:
#line 2047 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 406:
#line 2052 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                ;
    break;}
case 407:
#line 2057 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 408:
#line 2063 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 409:
#line 2069 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 411:
#line 2081 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 412:
#line 2083 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 414:
#line 2092 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 415:
#line 2094 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 416:
#line 2096 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 417:
#line 2101 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 418:
#line 2106 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 419:
#line 2108 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 420:
#line 2110 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 425:
#line 2127 "ned.y"
{
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(yylsp[-1])));
                  setComments(ps.cplusplus,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 426:
#line 2136 "ned.y"
{
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(yylsp[-1]));
                  setComments(ps.structdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 427:
#line 2145 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(yylsp[-1]));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 428:
#line 2152 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(yylsp[-1]));
                  setComments(ps.classdecl,yylsp[-3],yylsp[-2]);
                ;
    break;}
case 429:
#line 2162 "ned.y"
{
                  ps.messagedecl = (MessageDeclNode *)createNodeWithTag(NED_MESSAGE_DECL, ps.nedfile );
                  ps.messagedecl->setName(toString(yylsp[-1]));
                  setComments(ps.messagedecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 430:
#line 2171 "ned.y"
{
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString(yylsp[-1]));
                  setComments(ps.enumdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 431:
#line 2180 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 432:
#line 2187 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 433:
#line 2191 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 434:
#line 2199 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 439:
#line 2216 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-1],yylsp[-1]);
                ;
    break;}
case 440:
#line 2222 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 441:
#line 2232 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 442:
#line 2238 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 443:
#line 2242 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 444:
#line 2249 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 445:
#line 2256 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 446:
#line 2262 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 447:
#line 2266 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 448:
#line 2273 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 449:
#line 2280 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 450:
#line 2286 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 451:
#line 2290 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 452:
#line 2297 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 453:
#line 2304 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 460:
#line 2324 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 467:
#line 2343 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 474:
#line 2363 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                ;
    break;}
case 475:
#line 2369 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 476:
#line 2373 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(true);
                ;
    break;}
case 477:
#line 2380 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 491:
#line 2407 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 492:
#line 2412 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 493:
#line 2417 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 495:
#line 2425 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 497:
#line 2433 "ned.y"
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
