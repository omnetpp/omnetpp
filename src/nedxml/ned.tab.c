
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
#define	CPPINCLUDE	301
#define	SYSINCFILENAME	302
#define	STRUCT	303
#define	COBJECT	304
#define	NONCOBJECT	305
#define	ENUM	306
#define	EXTENDS	307
#define	MESSAGE	308
#define	CLASS	309
#define	FIELDS	310
#define	PROPERTIES	311
#define	ABSTRACT	312
#define	CHARTYPE	313
#define	SHORTTYPE	314
#define	INTTYPE	315
#define	LONGTYPE	316
#define	DOUBLETYPE	317
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

#line 73 "ned.y"


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



#define	YYFINAL		853
#define	YYFLAG		-32768
#define	YYNTBASE	102

#define YYTRANSLATE(x) ((unsigned)(x) <= 337 ? yytranslate[x] : 350)

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
   769,   771,   773,   775,   776,   777,   786,   790,   792,   798,
   801,   802,   805,   806,   809,   811,   818,   827,   834,   843,
   847,   849,   852,   854,   857,   859,   862,   864,   868,   870,
   873,   875,   878,   880,   883,   885,   887,   890,   894,   896,
   897,   898,   904,   905,   910,   912,   913,   916,   918,   920,
   922,   923,   924,   933,   937,   939,   945,   948,   949,   952,
   953,   956,   958,   965,   974,   981,   990,   994,   996,   999,
  1001,  1004,  1006,  1009,  1011,  1015,  1017,  1020,  1022,  1025,
  1027,  1030,  1032,  1034,  1037,  1041,  1046,  1052,  1060,  1063,
  1070,  1076,  1084,  1088,  1090,  1092,  1099,  1104,  1108,  1110,
  1112,  1116,  1120,  1124,  1128,  1132,  1136,  1140,  1143,  1147,
  1151,  1155,  1159,  1163,  1167,  1171,  1175,  1179,  1182,  1186,
  1190,  1194,  1197,  1201,  1205,  1211,  1215,  1220,  1227,  1236,
  1247,  1249,  1251,  1253,  1255,  1257,  1259,  1262,  1266,  1270,
  1273,  1275,  1277,  1279,  1281,  1283,  1287,  1292,  1294,  1296,
  1298,  1302,  1306,  1309,  1312,  1315,  1318,  1322,  1326,  1330,
  1331,  1339,  1340,  1350,  1352,  1353,  1356,  1358,  1361,  1366,
  1367,  1376,  1377,  1388,  1389,  1398,  1399,  1410,  1411,  1420,
  1421,  1432,  1433,  1438,  1439,  1441,  1442,  1445,  1447,  1452,
  1454,  1456,  1458,  1460,  1462,  1464,  1465,  1470,  1471,  1473,
  1474,  1477,  1479,  1480,  1488,  1489,  1498,  1500,  1503,  1505,
  1507,  1509,  1511,  1513,  1515,  1517,  1521,  1524,  1525,  1530,
  1531,  1534,  1535,  1537,  1539,  1541,  1543,  1545,  1547,  1549,
  1551,  1552,  1554
};

static const short yyrhs[] = {   103,
     0,   103,   104,     0,     0,   105,     0,   114,     0,   121,
     0,   126,     0,   297,     0,   109,     0,   118,     0,   123,
     0,   294,     0,   311,     0,   312,     0,   313,     0,   314,
     0,   315,     0,   321,     0,   324,     0,   327,     0,     0,
     3,   106,   107,    92,     0,   107,    93,   108,     0,   108,
     0,    35,     0,   110,   111,   113,     0,     5,    34,     0,
     0,   112,     0,   112,    31,   300,   348,     0,    31,   300,
   348,     0,    26,    34,   348,     0,    26,   348,     0,   115,
    94,   116,    95,   348,     0,     5,    34,     0,   117,     0,
     0,   117,    34,    96,   300,    92,     0,    34,    96,   300,
    92,     0,   119,   128,   144,   156,   120,     0,     4,    34,
     0,    24,    34,   348,     0,    24,   348,     0,   122,    94,
   134,   149,   165,    95,   348,     0,     4,    34,     0,   124,
   128,   144,   156,   174,   244,   140,   125,     0,     6,    34,
     0,    25,    34,   348,     0,    25,   348,     0,   127,    94,
   134,   149,   165,   185,   269,   142,    95,   348,     0,     6,
    34,     0,   129,     0,     0,     0,    29,    84,   130,   131,
     0,   132,    92,     0,     0,   132,    93,   133,     0,   133,
     0,    34,     0,   135,     0,     0,     0,    29,    84,   136,
   137,     0,   138,    92,     0,     0,   138,    93,   139,     0,
   139,     0,    34,     0,   141,     0,     0,    12,    84,    35,
    92,     0,   143,     0,     0,    12,    84,    35,    92,     0,
   145,     0,     0,     0,     7,    84,   146,   147,     0,   148,
    92,     0,     0,   148,    93,   154,     0,   154,     0,   150,
     0,     0,     0,     7,    84,   151,   152,     0,   153,    92,
     0,     0,   153,    92,   155,     0,   155,     0,   155,     0,
    34,     0,    34,    84,    43,     0,    42,    34,     0,    34,
    84,    42,     0,    34,    84,    42,    43,     0,    34,    84,
    43,    42,     0,    34,    84,    44,     0,    34,    84,    45,
     0,    34,    84,    46,     0,   157,     0,     0,     0,     8,
    84,   158,   159,     0,   160,     0,     0,   160,    13,   161,
    92,     0,    13,   161,    92,     0,   160,    14,   163,    92,
     0,    14,   163,    92,     0,   161,    93,   162,     0,   162,
     0,    34,    97,    98,     0,    34,     0,   163,    93,   164,
     0,   164,     0,    34,    97,    98,     0,    34,     0,   166,
     0,     0,     0,     8,    84,   167,   168,     0,   169,     0,
     0,   169,    13,   170,    92,     0,    13,   170,    92,     0,
   169,    14,   172,    92,     0,    14,   172,    92,     0,   170,
    93,   171,     0,   171,     0,    34,    97,    98,     0,    34,
     0,   172,    93,   173,     0,   173,     0,    34,    97,    98,
     0,    34,     0,   175,     0,     0,     0,    10,    84,   176,
   177,     0,   178,     0,     0,   178,   179,     0,   179,     0,
     0,    34,    84,    34,   348,   180,   196,   184,     0,     0,
    34,    84,    34,   299,   348,   181,   196,   184,     0,     0,
    34,    84,    34,    22,    34,   348,   182,   196,   184,     0,
     0,    34,    84,    34,   299,    22,    34,   348,   183,   196,
   184,     0,   212,   228,   242,     0,   186,     0,     0,     0,
    10,    84,   187,   188,     0,   189,     0,     0,   189,   190,
     0,   190,     0,     0,    34,    84,    34,   348,   191,   204,
   195,     0,     0,    34,    84,    34,   299,   348,   192,   204,
   195,     0,     0,    34,    84,    34,    22,    34,   348,   193,
   204,   195,     0,     0,    34,    84,    34,   299,    22,    34,
   348,   194,   204,   195,     0,   220,   235,   243,     0,   197,
     0,     0,   197,   198,     0,   198,     0,     0,    30,    84,
   199,   201,     0,     0,    30,    21,   300,    84,   200,   201,
     0,   202,    92,     0,     0,   202,    93,   203,     0,   203,
     0,    34,     0,   205,     0,     0,   205,   206,     0,   206,
     0,     0,    30,    84,   207,   209,     0,     0,    30,    21,
   300,    84,   208,   209,     0,   210,    92,     0,     0,   210,
    93,   211,     0,   211,     0,    34,     0,   213,     0,     0,
   213,   214,     0,   214,     0,     0,     7,    84,   215,   217,
     0,     0,     7,    21,   300,    84,   216,   217,     0,   218,
    92,     0,     0,   218,    93,   219,     0,   219,     0,    34,
    96,   300,     0,   221,     0,     0,   221,   222,     0,   222,
     0,     0,     7,    84,   223,   225,     0,     0,     7,    21,
   300,    84,   224,   225,     0,   226,     0,     0,   226,   227,
     0,   227,     0,    34,    96,   300,    92,     0,   228,   229,
     0,     0,     0,     9,    84,   230,   232,     0,     0,     9,
    21,   300,    84,   231,   232,     0,   233,    92,     0,     0,
   233,    93,   234,     0,   234,     0,    34,   299,     0,    34,
     0,   235,   236,     0,     0,     0,     9,    84,   237,   239,
     0,     0,     9,    21,   300,    84,   238,   239,     0,   240,
    92,     0,     0,   240,    93,   241,     0,   241,     0,    34,
   299,     0,    34,     0,    12,    84,    35,    92,     0,     0,
    12,    84,    35,    92,     0,     0,   245,     0,     0,     0,
    11,    15,    84,   246,   248,     0,     0,    11,    84,   247,
   248,     0,   249,     0,     0,   249,   250,     0,   250,     0,
   251,     0,   259,     0,     0,     0,    18,   252,   254,    20,
   253,   258,    28,   348,     0,   255,    93,   254,     0,   255,
     0,    34,    96,   300,    19,   300,     0,    21,   300,     0,
     0,    12,    35,     0,     0,   258,   259,     0,   259,     0,
   260,    17,   264,   256,   257,   349,     0,   260,    17,   268,
    17,   264,   256,   257,   349,     0,   260,    16,   264,   256,
   257,   349,     0,   260,    16,   268,    16,   264,   256,   257,
   349,     0,   261,    99,   262,     0,   263,     0,    34,   299,
     0,    34,     0,    34,   299,     0,    34,     0,    34,   299,
     0,    34,     0,   265,    99,   266,     0,   267,     0,    34,
   299,     0,    34,     0,    34,   299,     0,    34,     0,    34,
   299,     0,    34,     0,    34,     0,    31,   300,     0,   268,
    31,   300,     0,   270,     0,     0,     0,    11,    15,    84,
   271,   273,     0,     0,    11,    84,   272,   273,     0,   274,
     0,     0,   274,   275,     0,   275,     0,   276,     0,   284,
     0,     0,     0,    18,   277,   279,    20,   278,   283,    28,
    92,     0,   280,    93,   279,     0,   280,     0,    34,    96,
   300,    19,   300,     0,    21,   300,     0,     0,    12,    35,
     0,     0,   283,   284,     0,   284,     0,   285,    17,   289,
   281,   282,    92,     0,   285,    17,   293,    17,   289,   281,
   282,    92,     0,   285,    16,   289,   281,   282,    92,     0,
   285,    16,   293,    16,   289,   281,   282,    92,     0,   286,
    99,   287,     0,   288,     0,    34,   299,     0,    34,     0,
    34,   299,     0,    34,     0,    34,   299,     0,    34,     0,
   290,    99,   291,     0,   292,     0,    34,   299,     0,    34,
     0,    34,   299,     0,    34,     0,    34,   299,     0,    34,
     0,    34,     0,    34,   300,     0,   293,    34,   300,     0,
   295,   196,   212,   296,     0,    23,    34,    84,    34,   348,
     0,    23,    34,    84,    34,    22,    34,   348,     0,    27,
   348,     0,   298,    94,   204,   220,    95,   348,     0,    23,
    34,    84,    34,   348,     0,    23,    34,    84,    34,    22,
    34,   348,     0,    97,   300,    98,     0,   302,     0,   301,
     0,    39,   100,   302,    93,   302,   101,     0,    39,   100,
   302,   101,     0,    39,   100,   101,     0,    39,     0,   303,
     0,   100,   302,   101,     0,   302,    85,   302,     0,   302,
    86,   302,     0,   302,    87,   302,     0,   302,    88,   302,
     0,   302,    89,   302,     0,   302,    90,   302,     0,    86,
   302,     0,   302,    66,   302,     0,   302,    67,   302,     0,
   302,    68,   302,     0,   302,    69,   302,     0,   302,    70,
   302,     0,   302,    71,   302,     0,   302,    72,   302,     0,
   302,    73,   302,     0,   302,    74,   302,     0,    75,   302,
     0,   302,    76,   302,     0,   302,    77,   302,     0,   302,
    78,   302,     0,    79,   302,     0,   302,    80,   302,     0,
   302,    81,   302,     0,   302,    83,   302,    84,   302,     0,
    34,   100,   101,     0,    34,   100,   302,   101,     0,    34,
   100,   302,    93,   302,   101,     0,    34,   100,   302,    93,
   302,    93,   302,   101,     0,    34,   100,   302,    93,   302,
    93,   302,    93,   302,   101,     0,   304,     0,   305,     0,
   306,     0,   307,     0,   308,     0,    34,     0,    40,    34,
     0,    40,    41,    34,     0,    41,    40,    34,     0,    41,
    34,     0,    35,     0,    37,     0,    38,     0,   309,     0,
    65,     0,    65,   100,   101,     0,    64,   100,    34,   101,
     0,    32,     0,    33,     0,   310,     0,   310,    32,    34,
     0,   310,    33,    34,     0,    32,    34,     0,    33,    34,
     0,    47,    35,     0,    47,    48,     0,    49,    34,    92,
     0,    50,    34,    92,     0,    51,    34,    92,     0,     0,
    52,    34,    94,   316,   318,    95,    92,     0,     0,    52,
    34,    53,    34,    94,   317,   318,    95,    92,     0,   319,
     0,     0,   319,   320,     0,   320,     0,    34,    92,     0,
    34,    96,    32,    92,     0,     0,    54,    34,    94,   322,
   330,   336,    95,    92,     0,     0,    54,    34,    53,    34,
    94,   323,   330,   336,    95,    92,     0,     0,    55,    34,
    94,   325,   330,   336,    95,    92,     0,     0,    55,    34,
    53,    34,    94,   326,   330,   336,    95,    92,     0,     0,
    49,    34,    94,   328,   330,   336,    95,    92,     0,     0,
    49,    34,    53,    34,    94,   329,   330,   336,    95,    92,
     0,     0,    57,    84,   331,   332,     0,     0,   333,     0,
     0,   333,   334,     0,   334,     0,    34,    96,   335,    92,
     0,    35,     0,    32,     0,    33,     0,   310,     0,    37,
     0,    38,     0,     0,    56,    84,   337,   338,     0,     0,
   339,     0,     0,   339,   340,     0,   340,     0,     0,   343,
    34,   341,   344,   345,   346,    92,     0,     0,    58,   343,
    34,   342,   344,   345,   346,    92,     0,    34,     0,    34,
    87,     0,    59,     0,    60,     0,    61,     0,    62,     0,
    63,     0,    44,     0,    45,     0,    97,    32,    98,     0,
    97,    98,     0,     0,    52,   100,    34,   101,     0,     0,
    96,   347,     0,     0,    35,     0,    36,     0,    32,     0,
    33,     0,   310,     0,    37,     0,    38,     0,    92,     0,
     0,    93,     0,    92,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   234,   238,   239,   243,   244,   245,   246,   247,   249,   250,
   251,   252,   254,   255,   256,   257,   258,   259,   260,   261,
   268,   273,   280,   281,   285,   297,   301,   311,   312,   316,
   322,   331,   335,   345,   354,   364,   365,   369,   375,   387,
   395,   405,   409,   419,   430,   444,   455,   465,   469,   479,
   493,   507,   508,   512,   518,   523,   524,   528,   529,   533,
   545,   546,   550,   556,   561,   562,   566,   567,   571,   583,
   584,   588,   598,   599,   603,   613,   614,   618,   624,   629,
   630,   634,   638,   648,   649,   653,   659,   664,   665,   669,
   673,   684,   688,   692,   696,   700,   704,   708,   712,   716,
   720,   730,   731,   735,   741,   746,   747,   751,   752,   753,
   754,   758,   759,   763,   768,   776,   777,   781,   786,   797,
   798,   802,   808,   813,   814,   818,   819,   820,   821,   825,
   826,   830,   835,   843,   844,   848,   853,   864,   865,   869,
   875,   880,   881,   885,   886,   890,   896,   899,   906,   909,
   915,   918,   925,   931,   940,   941,   945,   951,   956,   957,
   961,   962,   966,   972,   975,   982,   985,   991,   994,  1001,
  1007,  1016,  1017,  1021,  1022,  1026,  1032,  1034,  1041,  1046,
  1047,  1051,  1052,  1056,  1067,  1068,  1072,  1073,  1077,  1083,
  1085,  1092,  1097,  1098,  1102,  1103,  1107,  1118,  1119,  1123,
  1124,  1128,  1134,  1136,  1143,  1149,  1150,  1154,  1155,  1159,
  1171,  1172,  1176,  1177,  1181,  1187,  1189,  1196,  1202,  1203,
  1207,  1208,  1212,  1224,  1225,  1229,  1235,  1237,  1244,  1249,
  1250,  1254,  1255,  1259,  1266,  1277,  1278,  1282,  1288,  1290,
  1297,  1302,  1303,  1307,  1308,  1312,  1319,  1330,  1334,  1341,
  1345,  1352,  1353,  1357,  1364,  1366,  1373,  1378,  1379,  1383,
  1384,  1388,  1389,  1393,  1397,  1401,  1410,  1411,  1415,  1425,
  1429,  1433,  1437,  1441,  1442,  1446,  1451,  1456,  1462,  1471,
  1472,  1476,  1482,  1490,  1495,  1502,  1509,  1518,  1519,  1523,
  1528,  1535,  1540,  1547,  1552,  1560,  1565,  1570,  1581,  1582,
  1586,  1593,  1595,  1602,  1607,  1608,  1612,  1613,  1617,  1618,
  1622,  1626,  1630,  1639,  1640,  1644,  1654,  1658,  1662,  1666,
  1670,  1671,  1675,  1680,  1685,  1691,  1700,  1701,  1705,  1711,
  1719,  1724,  1731,  1738,  1747,  1748,  1752,  1757,  1764,  1769,
  1776,  1781,  1789,  1794,  1799,  1810,  1817,  1823,  1832,  1843,
  1854,  1860,  1873,  1878,  1883,  1894,  1896,  1898,  1900,  1905,
  1906,  1909,  1911,  1913,  1915,  1917,  1919,  1922,  1926,  1928,
  1930,  1932,  1934,  1936,  1939,  1941,  1943,  1946,  1950,  1952,
  1954,  1957,  1960,  1962,  1964,  1967,  1969,  1971,  1973,  1975,
  1980,  1981,  1982,  1983,  1984,  1988,  1993,  1998,  2004,  2010,
  2018,  2023,  2025,  2030,  2034,  2036,  2038,  2043,  2045,  2047,
  2053,  2054,  2055,  2056,  2064,  2070,  2079,  2088,  2097,  2106,
  2113,  2117,  2125,  2132,  2133,  2137,  2138,  2142,  2147,  2156,
  2162,  2166,  2173,  2180,  2186,  2190,  2197,  2204,  2210,  2214,
  2221,  2228,  2234,  2234,  2238,  2239,  2243,  2244,  2248,  2258,
  2259,  2260,  2261,  2262,  2263,  2267,  2273,  2273,  2277,  2278,
  2282,  2283,  2287,  2293,  2297,  2304,  2311,  2312,  2313,  2314,
  2315,  2316,  2317,  2318,  2319,  2323,  2328,  2332,  2336,  2340,
  2344,  2348,  2352,  2353,  2354,  2355,  2356,  2357,  2358,  2361,
  2361,  2363,  2363
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","INCLUDE",
"SIMPLE","CHANNEL","MODULE","PARAMETERS","GATES","GATESIZES","SUBMODULES","CONNECTIONS",
"DISPLAY","IN","OUT","NOCHECK","LEFT_ARROW","RIGHT_ARROW","FOR","TO","DO","IF",
"LIKE","NETWORK","ENDSIMPLE","ENDMODULE","ENDCHANNEL","ENDNETWORK","ENDFOR",
"MACHINES","ON","CHANATTRNAME","INTCONSTANT","REALCONSTANT","NAME","STRINGCONSTANT",
"CHARCONSTANT","TRUE_","FALSE_","INPUT_","REF","ANCESTOR","CONSTDECL","NUMERICTYPE",
"STRINGTYPE","BOOLTYPE","ANYTYPE","CPPINCLUDE","SYSINCFILENAME","STRUCT","COBJECT",
"NONCOBJECT","ENUM","EXTENDS","MESSAGE","CLASS","FIELDS","PROPERTIES","ABSTRACT",
"CHARTYPE","SHORTTYPE","INTTYPE","LONGTYPE","DOUBLETYPE","SIZEOF","SUBMODINDEX",
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
"cppinclude","cppstruct","cppcobject","cppnoncobject","enum","@38","@39","opt_enumfields",
"enumfields","enumfield","message","@40","@41","class","@42","@43","struct",
"@44","@45","opt_propertiesblock","@46","opt_properties","properties","property",
"propertyvalue","opt_fieldsblock","@47","opt_fields","fields","field","@48",
"@49","fielddatatype","opt_fieldvector","opt_fieldenum","opt_fieldvalue","fieldvalue",
"opt_semicolon","comma_or_semicolon", NULL
};
#endif

static const short yyr1[] = {     0,
   102,   103,   103,   104,   104,   104,   104,   104,   104,   104,
   104,   104,   104,   104,   104,   104,   104,   104,   104,   104,
   106,   105,   107,   107,   108,   109,   110,   111,   111,   112,
   112,   113,   113,   114,   115,   116,   116,   117,   117,   118,
   119,   120,   120,   121,   122,   123,   124,   125,   125,   126,
   127,   128,   128,   130,   129,   131,   131,   132,   132,   133,
   134,   134,   136,   135,   137,   137,   138,   138,   139,   140,
   140,   141,   142,   142,   143,   144,   144,   146,   145,   147,
   147,   148,   148,   149,   149,   151,   150,   152,   152,   153,
   153,   154,   155,   155,   155,   155,   155,   155,   155,   155,
   155,   156,   156,   158,   157,   159,   159,   160,   160,   160,
   160,   161,   161,   162,   162,   163,   163,   164,   164,   165,
   165,   167,   166,   168,   168,   169,   169,   169,   169,   170,
   170,   171,   171,   172,   172,   173,   173,   174,   174,   176,
   175,   177,   177,   178,   178,   180,   179,   181,   179,   182,
   179,   183,   179,   184,   185,   185,   187,   186,   188,   188,
   189,   189,   191,   190,   192,   190,   193,   190,   194,   190,
   195,   196,   196,   197,   197,   199,   198,   200,   198,   201,
   201,   202,   202,   203,   204,   204,   205,   205,   207,   206,
   208,   206,   209,   209,   210,   210,   211,   212,   212,   213,
   213,   215,   214,   216,   214,   217,   217,   218,   218,   219,
   220,   220,   221,   221,   223,   222,   224,   222,   225,   225,
   226,   226,   227,   228,   228,   230,   229,   231,   229,   232,
   232,   233,   233,   234,   234,   235,   235,   237,   236,   238,
   236,   239,   239,   240,   240,   241,   241,   242,   242,   243,
   243,   244,   244,   246,   245,   247,   245,   248,   248,   249,
   249,   250,   250,   252,   253,   251,   254,   254,   255,   256,
   256,   257,   257,   258,   258,   259,   259,   259,   259,   260,
   260,   261,   261,   262,   262,   263,   263,   264,   264,   265,
   265,   266,   266,   267,   267,   268,   268,   268,   269,   269,
   271,   270,   272,   270,   273,   273,   274,   274,   275,   275,
   277,   278,   276,   279,   279,   280,   281,   281,   282,   282,
   283,   283,   284,   284,   284,   284,   285,   285,   286,   286,
   287,   287,   288,   288,   289,   289,   290,   290,   291,   291,
   292,   292,   293,   293,   293,   294,   295,   295,   296,   297,
   298,   298,   299,   300,   300,   301,   301,   301,   301,   302,
   302,   302,   302,   302,   302,   302,   302,   302,   302,   302,
   302,   302,   302,   302,   302,   302,   302,   302,   302,   302,
   302,   302,   302,   302,   302,   302,   302,   302,   302,   302,
   303,   303,   303,   303,   303,   304,   304,   304,   304,   304,
   305,   306,   306,   307,   308,   308,   308,   309,   309,   309,
   310,   310,   310,   310,   311,   311,   312,   313,   314,   316,
   315,   317,   315,   318,   318,   319,   319,   320,   320,   322,
   321,   323,   321,   325,   324,   326,   324,   328,   327,   329,
   327,   331,   330,   330,   332,   332,   333,   333,   334,   335,
   335,   335,   335,   335,   335,   337,   336,   336,   338,   338,
   339,   339,   341,   340,   342,   340,   343,   343,   343,   343,
   343,   343,   343,   343,   343,   344,   344,   344,   345,   345,
   346,   346,   347,   347,   347,   347,   347,   347,   347,   348,
   348,   349,   349
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
     1,     1,     1,     0,     0,     8,     3,     1,     5,     2,
     0,     2,     0,     2,     1,     6,     8,     6,     8,     3,
     1,     2,     1,     2,     1,     2,     1,     3,     1,     2,
     1,     2,     1,     2,     1,     1,     2,     3,     1,     0,
     0,     5,     0,     4,     1,     0,     2,     1,     1,     1,
     0,     0,     8,     3,     1,     5,     2,     0,     2,     0,
     2,     1,     6,     8,     6,     8,     3,     1,     2,     1,
     2,     1,     2,     1,     3,     1,     2,     1,     2,     1,
     2,     1,     1,     2,     3,     4,     5,     7,     2,     6,
     5,     7,     3,     1,     1,     6,     4,     3,     1,     1,
     3,     3,     3,     3,     3,     3,     3,     2,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     2,     3,     3,
     3,     2,     3,     3,     5,     3,     4,     6,     8,    10,
     1,     1,     1,     1,     1,     1,     2,     3,     3,     2,
     1,     1,     1,     1,     1,     3,     4,     1,     1,     1,
     3,     3,     2,     2,     2,     2,     3,     3,     3,     0,
     7,     0,     9,     1,     0,     2,     1,     2,     4,     0,
     8,     0,    10,     0,     8,     0,    10,     0,     8,     0,
    10,     0,     4,     0,     1,     0,     2,     1,     4,     1,
     1,     1,     1,     1,     1,     0,     4,     0,     1,     0,
     2,     1,     0,     7,     0,     8,     1,     2,     1,     1,
     1,     1,     1,     1,     1,     3,     2,     0,     4,     0,
     2,     0,     1,     1,     1,     1,     1,     1,     1,     1,
     0,     1,     1
};

static const short yydefact[] = {     3,
     1,    21,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     2,     4,     9,    28,     5,     0,    10,
    53,     6,     0,    11,    53,     7,     0,    12,   173,     8,
     0,    13,    14,    15,    16,    17,    18,    19,    20,     0,
    41,    27,    47,     0,   415,   416,     0,     0,     0,     0,
     0,     0,     0,     0,    29,    37,     0,    77,    52,    62,
    77,    62,     0,   199,   172,   175,   186,    25,     0,    24,
     0,     0,   417,   438,   418,   419,     0,   420,     0,   430,
     0,   434,   408,   409,   396,   401,   402,   403,   359,     0,
     0,     0,   405,     0,     0,     0,     0,   491,   355,   354,
   360,   391,   392,   393,   394,   395,   404,   410,   491,    26,
     0,     0,     0,    36,    54,     0,   103,    76,     0,    85,
    61,   103,    85,     0,   176,     0,     0,   198,   201,   174,
     0,   212,   185,   188,    22,     0,   491,     0,   444,     0,
   425,     0,   444,     0,   444,   413,   414,     0,     0,   397,
     0,   400,     0,     0,     0,   378,   382,   368,     0,   490,
    31,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   491,    33,   491,     0,   491,     0,
    57,    78,     0,     0,   102,    63,     0,   121,    84,   139,
   121,     0,   181,     0,   202,   491,   346,   200,     0,   189,
     0,     0,   211,   214,   187,    23,     0,   347,   440,     0,
   458,   422,     0,     0,   424,   427,   432,   458,   436,   458,
   386,     0,   358,     0,   398,   399,     0,   406,   361,   369,
   370,   371,   372,   373,   374,   375,   376,   377,   379,   380,
   381,   383,   384,     0,   362,   363,   364,   365,   366,   367,
   411,   412,    32,    30,     0,    34,     0,    60,    55,     0,
    59,    81,   104,   491,    40,    66,    86,     0,     0,   120,
     0,   253,   138,   156,   178,   184,   177,     0,   183,     0,
   207,   349,     0,   194,     0,   215,   491,   213,   491,   444,
   442,     0,     0,   425,   428,     0,     0,   426,   444,     0,
   444,     0,     0,   387,     0,   357,   407,     0,    39,     0,
    56,     0,    93,     0,    79,     0,    83,    92,   107,   491,
    43,    69,    64,     0,    68,    89,   122,   491,   140,     0,
    71,   252,     0,   300,   155,   181,   180,     0,   204,     0,
   203,     0,   209,   191,   197,   190,     0,   196,     0,   220,
   350,   348,   458,   446,   456,     0,     0,     0,   421,   458,
     0,   458,     0,     0,     0,   385,    38,    58,     0,    95,
    80,     0,     0,     0,   105,   106,    42,    65,     0,    87,
     0,    91,   125,    44,   143,     0,   256,     0,     0,    70,
   157,     0,    74,   299,   179,   182,   207,     0,   206,     0,
   194,   193,     0,   217,     0,   216,   219,   222,     0,     0,
   443,   445,   448,   460,   439,     0,   429,     0,   431,     0,
   435,     0,   388,   356,    96,    94,    99,   100,   101,    82,
   115,     0,   113,   119,     0,   117,     0,     0,    67,    88,
     0,     0,   123,   124,     0,   141,   142,   145,   254,   259,
     0,   491,    46,   160,     0,   303,     0,     0,    73,   205,
   210,   208,   192,   195,   220,     0,   221,     0,     0,   447,
   467,   474,   475,     0,   469,   470,   471,   472,   473,   457,
   459,   462,     0,   423,     0,     0,     0,    97,    98,     0,
   109,     0,     0,   111,     0,     0,     0,    90,   133,     0,
   131,   137,     0,   135,     0,     0,     0,   144,   259,   264,
   287,   257,   258,   261,   262,   263,     0,     0,   281,     0,
   491,    49,     0,   158,   159,   162,   301,   306,     0,   491,
   218,     0,   441,   451,   452,   450,   454,   455,   453,     0,
   468,     0,   461,   463,   433,   437,     0,   389,   114,   112,
   118,   116,   108,   110,     0,   127,     0,     0,   129,     0,
     0,     0,   491,   255,     0,     0,   286,   260,     0,     0,
     0,    72,    48,     0,   161,   306,   311,   334,   304,   305,
   308,   309,   310,     0,     0,   328,     0,    50,   223,   449,
   465,   478,     0,   132,   130,   136,   134,   126,   128,     0,
   491,   146,     0,     0,   268,     0,     0,   295,   271,     0,
   289,     0,   271,     0,   285,   280,   491,   302,     0,   333,
   307,     0,     0,     0,    75,   478,     0,   480,   390,   491,
     0,   148,   173,     0,   265,     0,   353,   297,   294,     0,
   273,     0,     0,     0,   273,     0,   284,     0,   491,   163,
     0,     0,   315,   342,   318,     0,   336,     0,   318,     0,
   332,   327,   480,     0,   477,     0,   482,   150,   491,   173,
   199,     0,     0,   267,   270,     0,     0,   293,   288,   295,
   271,   298,     0,   271,   491,     0,   165,   186,     0,   312,
     0,   341,   344,     0,   320,     0,     0,     0,   320,     0,
   331,   482,   476,     0,     0,     0,   173,   152,   199,   147,
   225,     0,     0,   275,   272,   493,   492,   278,   292,   273,
   276,   273,   167,   491,   186,   212,     0,     0,   314,   317,
     0,     0,   340,   335,   342,   318,   345,     0,   318,     0,
     0,   485,   486,   483,   484,   488,   489,   487,   481,   464,
   199,   173,   149,   249,   269,   491,   274,     0,     0,   186,
   169,   212,   164,   237,     0,     0,   322,   319,   325,   339,
   320,   323,   320,   466,   479,   151,   199,     0,     0,   224,
   154,   266,   279,   277,   212,   186,   166,   251,   316,     0,
   321,     0,     0,   153,     0,   226,     0,   168,   212,     0,
     0,   236,   171,   313,   326,   324,     0,   231,     0,   170,
     0,   238,     0,   228,   235,   227,     0,   233,   248,     0,
   243,     0,   231,   234,   230,     0,   240,   247,   239,     0,
   245,   250,   229,   232,   243,   246,   242,     0,   241,   244,
     0,     0,     0
};

static const short yydefgoto[] = {   851,
     1,    14,    15,    40,    69,    70,    16,    17,    54,    55,
   110,    18,    19,   113,   114,    20,    21,   275,    22,    23,
    24,    25,   463,    26,    27,    58,    59,   191,   269,   270,
   271,   120,   121,   276,   333,   334,   335,   399,   400,   468,
   469,   117,   118,   272,   325,   326,   198,   199,   336,   390,
   391,   327,   328,   194,   195,   329,   385,   386,   442,   443,
   445,   446,   279,   280,   393,   453,   454,   510,   511,   513,
   514,   282,   283,   395,   456,   457,   458,   643,   680,   717,
   762,   720,   344,   345,   464,   534,   535,   536,   698,   735,
   770,   796,   773,    64,    65,    66,   203,   346,   287,   288,
   289,   132,   133,   134,   294,   411,   356,   357,   358,   721,
   128,   129,   291,   407,   351,   352,   353,   774,   213,   214,
   360,   475,   416,   417,   418,   764,   790,   818,   833,   826,
   827,   828,   798,   812,   831,   845,   839,   840,   841,   791,
   813,   341,   342,   519,   460,   522,   523,   524,   525,   575,
   683,   614,   615,   651,   687,   723,   526,   527,   528,   626,
   529,   619,   620,   689,   621,   622,   403,   404,   586,   538,
   589,   590,   591,   592,   629,   738,   662,   663,   705,   742,
   776,   593,   594,   595,   672,   596,   665,   666,   744,   667,
   668,    28,    29,   207,    30,    31,   649,    98,    99,   100,
   101,   102,   103,   104,   105,   106,   107,   108,    32,    33,
    34,    35,    36,   141,   304,   224,   225,   226,    37,   143,
   309,    38,   145,   311,    39,   139,   300,   221,   364,   421,
   422,   423,   550,   303,   424,   490,   491,   492,   602,   636,
   493,   638,   677,   716,   759,   161,   728
};

static const short yypact[] = {-32768,
   357,-32768,   -10,    17,    46,   157,    23,   162,   179,   205,
   216,   221,   224,-32768,-32768,-32768,   142,-32768,   -53,-32768,
   231,-32768,    74,-32768,   231,-32768,   114,-32768,   233,-32768,
   185,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   236,
   190,   195,   207,   192,-32768,-32768,   -26,   214,   235,   -35,
   -27,   -22,   253,   290,   306,   295,   265,   339,-32768,   330,
   339,   330,   117,   345,   233,-32768,   334,-32768,    69,-32768,
   337,   347,-32768,-32768,-32768,-32768,   350,-32768,   371,-32768,
   383,-32768,   400,   413,   288,-32768,-32768,-32768,   331,   119,
    15,   372,   374,   310,   310,   310,   310,   360,-32768,   697,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   153,   -14,-32768,
   253,   409,   391,   474,-32768,   425,   502,-32768,   427,   505,
-32768,   502,   505,   253,-32768,   126,   486,   345,-32768,-32768,
   127,   507,   334,-32768,-32768,   236,   -13,   421,   459,   423,
   484,   426,   459,   428,   459,-32768,-32768,   182,   240,-32768,
   485,-32768,   487,   490,   424,-32768,-32768,-32768,   568,-32768,
-32768,   310,   310,   310,   310,   310,   310,   310,   310,   310,
   310,   310,   310,   310,   310,   310,   310,   310,   310,   310,
   310,   310,   492,   493,   360,-32768,   360,   253,   360,   432,
   495,-32768,   446,   508,-32768,-32768,   447,   526,-32768,   525,
   526,   453,   504,   253,-32768,   360,-32768,-32768,   253,-32768,
   128,   445,   507,-32768,-32768,-32768,   510,   448,-32768,   457,
   489,-32768,   -50,   451,   484,-32768,-32768,   489,-32768,   489,
-32768,   352,-32768,   390,-32768,-32768,   449,-32768,-32768,   222,
   222,   222,   222,   222,   222,   747,   747,   747,   407,   407,
   407,   414,   414,   672,    89,    89,   458,   458,   458,   458,
-32768,-32768,-32768,-32768,   455,-32768,   253,-32768,-32768,   135,
-32768,    10,-32768,   132,-32768,   515,-32768,   467,   461,-32768,
   468,   543,-32768,   556,-32768,-32768,-32768,   149,-32768,   498,
   536,-32768,   499,   539,   253,-32768,   360,-32768,   360,   459,
-32768,   501,   494,   484,-32768,   554,   503,-32768,   459,   512,
   459,   519,   310,-32768,   310,-32768,-32768,   310,-32768,   524,
-32768,   495,   506,   553,-32768,   156,-32768,-32768,   218,   360,
-32768,-32768,-32768,   160,-32768,    10,-32768,   360,-32768,    -8,
   582,-32768,   540,   585,-32768,   504,-32768,   504,-32768,   527,
-32768,   174,-32768,-32768,-32768,-32768,   177,-32768,   542,   593,
-32768,   534,   489,   595,-32768,   551,   537,   555,-32768,   489,
   558,   489,   560,   491,   610,   722,-32768,-32768,   278,-32768,
-32768,    10,   597,   625,-32768,   282,-32768,-32768,   515,-32768,
   569,-32768,   300,-32768,   626,   579,-32768,   581,   641,-32768,
-32768,     7,   655,-32768,-32768,-32768,   536,   253,-32768,   536,
   539,-32768,   539,-32768,   572,-32768,   593,-32768,   575,   576,
-32768,   595,-32768,   332,-32768,   583,-32768,   578,-32768,   590,
-32768,   310,-32768,-32768,   628,   632,-32768,-32768,-32768,-32768,
   592,   262,-32768,   604,   264,-32768,   597,   625,-32768,    10,
   658,   668,-32768,   354,   619,-32768,   626,-32768,-32768,    14,
   669,   133,-32768,   671,   622,-32768,   623,   613,-32768,-32768,
-32768,-32768,-32768,-32768,   593,   253,-32768,   617,   411,-32768,
   634,-32768,-32768,   338,-32768,-32768,-32768,-32768,-32768,-32768,
   332,-32768,   691,-32768,   636,   638,   532,-32768,-32768,   639,
-32768,   597,   653,-32768,   625,   277,   286,-32768,   657,   294,
-32768,   675,   321,-32768,   658,   668,   742,-32768,    14,-32768,
    95,-32768,    14,-32768,-32768,-32768,   386,   680,-32768,   689,
   360,-32768,   713,-32768,   671,-32768,-32768,    16,   766,   360,
-32768,   712,-32768,   400,   413,-32768,-32768,-32768,   153,   714,
-32768,   771,-32768,-32768,-32768,-32768,   310,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   721,-32768,   658,   723,-32768,   668,
   323,   358,    42,-32768,   786,   253,   727,-32768,    29,    29,
   788,-32768,-32768,   795,-32768,    16,-32768,   110,-32768,    16,
-32768,-32768,-32768,   438,   731,-32768,   739,-32768,-32768,-32768,
-32768,   741,   646,-32768,-32768,-32768,-32768,-32768,-32768,   805,
     8,-32768,   744,   821,   749,   745,   253,    -2,   823,   746,
-32768,   138,   823,    22,   750,-32768,    43,-32768,   812,   751,
-32768,   814,   814,   815,-32768,   741,    38,   799,-32768,   360,
   818,-32768,   233,   253,-32768,   786,-32768,-32768,   754,   253,
   842,   822,   824,   253,   842,   824,-32768,   825,    40,-32768,
   759,   837,   767,   165,   840,   763,-32768,     9,   840,    11,
   750,-32768,   799,   765,-32768,   764,   769,-32768,   360,   233,
   345,   847,   833,-32768,-32768,   834,   389,   750,-32768,   136,
   823,-32768,   389,   823,   360,   836,-32768,   334,   253,-32768,
   812,   772,-32768,   253,   856,   838,   839,   253,   856,   839,
-32768,   769,-32768,   841,   298,   782,   233,-32768,   345,-32768,
-32768,   253,   123,-32768,-32768,-32768,-32768,-32768,-32768,   842,
-32768,   842,-32768,   360,   334,   507,   857,   843,-32768,-32768,
   844,   789,   750,-32768,   139,   840,-32768,   790,   840,   791,
   777,   400,   413,-32768,-32768,-32768,-32768,   153,-32768,-32768,
   345,   233,-32768,   171,-32768,   360,-32768,   389,   389,   334,
-32768,   507,-32768,-32768,   253,   131,-32768,-32768,-32768,-32768,
   856,-32768,   856,-32768,-32768,-32768,   345,   134,   796,-32768,
-32768,-32768,-32768,-32768,   507,   334,-32768,   178,-32768,   792,
-32768,   793,   794,-32768,   253,-32768,   852,-32768,   507,   137,
   804,-32768,-32768,-32768,-32768,-32768,   806,   855,   800,-32768,
   253,-32768,   858,-32768,   750,-32768,   392,-32768,-32768,   807,
   860,   803,   855,-32768,-32768,   855,-32768,   750,-32768,   397,
-32768,-32768,-32768,-32768,   860,-32768,-32768,   860,-32768,-32768,
   896,   897,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   762,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   874,-32768,-32768,-32768,-32768,
   580,   845,-32768,-32768,-32768,-32768,   511,-32768,-32768,-32768,
-32768,   848,-32768,-32768,-32768,-32768,   778,-32768,-32768,-32768,
-32768,   521,  -324,   783,-32768,-32768,-32768,-32768,   463,   402,
   460,   401,   710,-32768,-32768,-32768,-32768,   398,   348,   396,
   344,-32768,-32768,-32768,-32768,-32768,   462,-32768,-32768,-32768,
-32768,  -637,-32768,-32768,-32768,-32768,-32768,   381,-32768,-32768,
-32768,-32768,  -620,  -633,-32768,   853,-32768,-32768,   571,-32768,
   573,  -695,-32768,   787,-32768,-32768,   513,-32768,   509,   859,
-32768,   797,-32768,-32768,   520,-32768,   516,   798,-32768,   715,
-32768,-32768,   454,-32768,   514,-32768,-32768,-32768,-32768,    99,
-32768,    97,-32768,-32768,-32768,-32768,    90,-32768,    86,-32768,
-32768,-32768,-32768,-32768,-32768,   417,-32768,   415,-32768,-32768,
-32768,   291,-32768,  -549,  -638,-32768,  -667,-32768,-32768,-32768,
-32768,  -523,-32768,-32768,-32768,   359,-32768,-32768,-32768,-32768,
   355,-32768,   353,-32768,-32768,-32768,   239,-32768,  -650,  -640,
-32768,  -715,-32768,-32768,-32768,-32768,  -579,-32768,-32768,-32768,
   309,-32768,-32768,-32768,-32768,-32768,  -500,  -111,-32768,   -59,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -478,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   640,-32768,   720,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -137,-32768,-32768,
-32768,   528,-32768,  -226,-32768,-32768,-32768,   456,-32768,-32768,
   464,   313,   273,   241,-32768,  -104,  -682
};


#define	YYLAST		953


static const short yytable[] = {   187,
   549,   310,   736,   312,   186,   228,   396,   230,   217,   681,
   731,   392,   202,  -296,  -296,   724,   693,    77,   709,   185,
   577,   465,   777,    41,   707,    79,    72,   710,  -296,   641,
    81,   520,   218,   587,   156,   157,   158,   159,   656,   772,
    56,   305,   708,   323,   708,   306,   719,   521,   152,   588,
    42,   324,   654,   669,   153,   767,   623,    45,    78,   617,
   801,   696,   618,   610,   658,    73,    80,    74,   748,   674,
    46,    82,   611,   655,   795,   397,   265,   160,   160,    43,
   263,   763,   264,   761,   266,   793,   794,   630,   232,   234,
   466,   768,   290,   769,   576,   781,  -291,   293,   783,   160,
   809,   292,   240,   241,   242,   243,   244,   245,   246,   247,
   248,   249,   250,   251,   252,   253,   254,   255,   256,   257,
   258,   259,   260,   786,   657,   508,   659,   746,   787,   691,
   749,   160,   694,   160,   160,   675,   419,   124,   576,   576,
   802,   730,   803,   428,   732,   430,   204,   209,   295,   804,
   766,   797,   150,   653,   805,   320,   521,   821,   800,   151,
   135,   136,   363,   702,   588,   330,   531,    60,   654,   331,
   711,   370,    53,   372,   808,   179,   180,   181,   182,   788,
  -343,  -343,   789,   359,   183,   184,   810,   729,   820,   811,
    44,   576,   361,  -283,   362,    47,    83,    84,    85,    86,
   125,    87,    88,    89,    90,    91,   576,    62,  -330,   205,
   210,   296,    48,    83,    84,    85,    86,   806,    87,    88,
   822,    90,    91,   160,   160,   387,   321,   322,    92,    93,
   383,   384,   576,   394,  -291,   576,   758,  -338,    49,    94,
   347,   348,   780,    95,   702,    92,    93,   381,   382,    50,
    96,   388,   389,   374,    51,   375,    94,    52,   376,    57,
    95,   576,    63,  -338,    97,   409,   410,    96,   412,   413,
    68,    83,    84,    85,    86,    71,    87,    88,    67,    90,
    91,    97,   231,   -45,    83,    84,    85,    86,   -35,    87,
    88,    89,    90,    91,   447,   448,   471,   171,   172,   173,
   -51,   174,   175,    92,    93,    75,   177,   178,   179,   180,
   181,   182,   451,   452,    94,   109,    92,    93,    95,   435,
   436,   437,   438,   439,   834,    96,    76,    94,   112,   752,
   753,    95,   754,   755,   756,   757,   111,   846,    96,    97,
   233,    83,    84,    85,    86,   116,    87,    88,   115,    90,
    91,   126,    97,   501,   502,   504,   505,   532,   119,     2,
     3,     4,     5,   131,   542,   481,   515,   516,   563,   502,
   137,   481,   497,    92,    93,   482,   483,   564,   505,     6,
   138,   482,   483,   140,    94,   566,   567,   148,    95,   484,
   485,   486,   487,   488,   489,    96,   485,   486,   487,   488,
   489,   579,   580,     7,   142,     8,     9,    10,    11,    97,
    12,    13,   569,   570,   608,   567,   144,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   583,   171,   172,   173,
   149,   174,   175,   146,   176,   598,   177,   178,   179,   180,
   181,   182,   544,   545,   313,   546,   147,   547,   548,   609,
   570,   160,   314,   632,   633,   162,   163,   164,   165,   166,
   167,   168,   169,   170,   616,   171,   172,   173,   612,   174,
   175,   154,   176,   155,   177,   178,   179,   180,   181,   182,
   726,   727,   315,   835,   836,   189,   174,   175,   847,   848,
   316,   177,   178,   179,   180,   181,   182,   603,   177,   178,
   179,   180,   181,   182,   188,   648,   642,   190,   192,   193,
   196,   197,   206,   211,   219,   220,   222,   223,   235,   227,
   236,   229,   660,   237,   238,   261,   262,   267,   268,   273,
   277,   274,   682,   278,   281,   678,   285,   286,   685,   297,
   301,  -351,   692,   299,   302,   307,   319,   182,   332,   317,
   337,   339,   703,   340,   697,   338,   162,   163,   164,   165,
   166,   167,   168,   169,   170,   343,   171,   172,   173,   350,
   174,   175,   355,   176,   718,   177,   178,   179,   180,   181,
   182,   349,   354,   432,   365,   368,   380,   737,   366,   379,
   733,   433,   740,   398,   369,   402,   747,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   371,   171,   172,   173,
   765,   174,   175,   373,   176,   377,   177,   178,   179,   180,
   181,   182,   408,   401,   557,   414,   415,  -352,   420,   771,
   441,   426,   558,   162,   163,   164,   165,   166,   167,   168,
   169,   170,   425,   171,   172,   173,   427,   174,   175,   429,
   176,   431,   177,   178,   179,   180,   181,   182,   444,   455,
   450,   792,   459,   799,   461,   462,   467,   476,   239,   478,
   498,   479,   495,   499,   494,   162,   163,   164,   165,   166,
   167,   168,   169,   170,   496,   171,   172,   173,   500,   174,
   175,   509,   176,   817,   177,   178,   179,   180,   181,   182,
   503,   512,   517,   530,   533,   537,   539,   540,   543,   830,
   434,   162,   163,   164,   165,   166,   167,   168,   169,   170,
   551,   171,   172,   173,   554,   174,   175,   555,   176,   556,
   177,   178,   179,   180,   181,   182,   559,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   639,   171,   172,   173,
   561,   174,   175,   565,   176,   318,   177,   178,   179,   180,
   181,   182,   162,   163,   164,   165,   166,   167,   168,   169,
   170,   568,   171,   172,   173,   573,   174,   175,   581,   176,
   582,   177,   178,   179,   180,   181,   182,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   584,   171,   172,   173,
   597,   174,   175,   599,   601,   600,   177,   178,   179,   180,
   181,   182,   162,   163,   164,   165,   166,   167,   604,   613,
   606,   625,   171,   172,   173,  -282,   174,   175,   627,   634,
   635,   177,   178,   179,   180,   181,   182,   637,   640,   644,
   645,   646,   647,   650,   652,   661,   576,   664,   671,  -329,
   676,   679,  -290,   686,   699,   688,   700,   690,   695,   701,
   704,   706,   713,   714,   715,   722,   521,   741,   725,   734,
  -337,   743,   745,   760,   751,   775,   588,   785,   778,   807,
   779,   782,   784,   814,   815,   816,   819,   823,   825,   824,
   837,   829,   832,   838,   842,   852,   853,   216,    61,   449,
   201,   378,   440,   560,   200,   562,   123,   507,   122,   506,
   284,   572,   571,   607,   605,   585,   405,   130,   518,   215,
   406,   474,   127,   473,   208,   472,   470,   298,   541,   212,
   477,   843,   844,   850,   849,   574,   684,   578,   624,   739,
   628,   670,   631,   367,   308,   712,   553,   552,   673,   480,
     0,     0,   750
};

static const short yycheck[] = {   111,
   479,   228,   698,   230,   109,   143,    15,   145,    22,   643,
   693,   336,   124,    16,    17,   683,   655,    53,   669,    34,
   521,    15,   738,    34,    16,    53,    53,    17,    31,    22,
    53,    18,   137,    18,    94,    95,    96,    97,    17,   735,
    94,    92,    34,    34,    34,    96,   680,    34,    34,    34,
    34,    42,    31,   633,    40,   723,   580,    35,    94,    31,
   776,    22,    34,    22,    22,    92,    94,    94,   709,    32,
    48,    94,   573,   623,   770,    84,   188,    92,    92,    34,
   185,   719,   187,   717,   189,   768,   769,   588,   148,   149,
    84,   730,   204,   732,    97,   746,    99,   209,   749,    92,
   796,   206,   162,   163,   164,   165,   166,   167,   168,   169,
   170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
   180,   181,   182,   761,   625,   450,   627,   707,   762,   653,
   710,    92,   656,    92,    92,    98,   363,    21,    97,    97,
   781,   691,   783,   370,   694,   372,    21,    21,    21,   787,
    28,   772,    34,    16,    21,   267,    34,    21,    28,    41,
    92,    93,   300,   664,    34,    34,    34,    94,    31,   274,
   671,   309,    31,   311,   795,    87,    88,    89,    90,     9,
    16,    17,    12,   295,    32,    33,     9,   688,   809,    12,
    34,    97,   297,    99,   299,    34,    32,    33,    34,    35,
    84,    37,    38,    39,    40,    41,    97,    94,    99,    84,
    84,    84,    34,    32,    33,    34,    35,    84,    37,    38,
    84,    40,    41,    92,    92,   330,    92,    93,    64,    65,
    13,    14,    97,   338,    99,    97,   715,    99,    34,    75,
    92,    93,   743,    79,   745,    64,    65,    92,    93,    34,
    86,    92,    93,   313,    34,   315,    75,    34,   318,    29,
    79,    97,    30,    99,   100,    92,    93,    86,    92,    93,
    35,    32,    33,    34,    35,    84,    37,    38,    94,    40,
    41,   100,   101,    94,    32,    33,    34,    35,    94,    37,
    38,    39,    40,    41,    13,    14,   408,    76,    77,    78,
    94,    80,    81,    64,    65,    92,    85,    86,    87,    88,
    89,    90,    13,    14,    75,    26,    64,    65,    79,    42,
    43,    44,    45,    46,   825,    86,    92,    75,    34,    32,
    33,    79,    35,    36,    37,    38,    31,   838,    86,   100,
   101,    32,    33,    34,    35,     7,    37,    38,    84,    40,
    41,     7,   100,    92,    93,    92,    93,   462,    29,     3,
     4,     5,     6,    30,   476,    34,    13,    14,    92,    93,
    34,    34,   432,    64,    65,    44,    45,    92,    93,    23,
    34,    44,    45,    34,    75,    92,    93,   100,    79,    58,
    59,    60,    61,    62,    63,    86,    59,    60,    61,    62,
    63,    16,    17,    47,    34,    49,    50,    51,    52,   100,
    54,    55,    92,    93,    92,    93,    34,    66,    67,    68,
    69,    70,    71,    72,    73,    74,   531,    76,    77,    78,
   100,    80,    81,    34,    83,   540,    85,    86,    87,    88,
    89,    90,    32,    33,    93,    35,    34,    37,    38,    92,
    93,    92,   101,    16,    17,    66,    67,    68,    69,    70,
    71,    72,    73,    74,   576,    76,    77,    78,   573,    80,
    81,   100,    83,   100,    85,    86,    87,    88,    89,    90,
    92,    93,    93,    92,    93,    95,    80,    81,    92,    93,
   101,    85,    86,    87,    88,    89,    90,   557,    85,    86,
    87,    88,    89,    90,    96,   617,   611,    34,    84,     8,
    84,     7,    27,     7,    94,    57,    94,    34,    34,    94,
    34,    94,   627,    34,   101,    34,    34,    96,    34,    84,
    84,    24,   644,     8,    10,   640,    84,    34,   650,    95,
    84,    94,   654,    34,    56,    95,    92,    90,    34,   101,
    84,    84,   664,    11,   659,    95,    66,    67,    68,    69,
    70,    71,    72,    73,    74,    10,    76,    77,    78,    34,
    80,    81,    34,    83,   679,    85,    86,    87,    88,    89,
    90,    84,    84,    93,    84,    32,    34,   699,    95,    84,
   695,   101,   704,    12,    92,    11,   708,    66,    67,    68,
    69,    70,    71,    72,    73,    74,    95,    76,    77,    78,
   722,    80,    81,    95,    83,    92,    85,    86,    87,    88,
    89,    90,    96,    84,    93,    84,    34,    94,    34,   734,
    34,    95,   101,    66,    67,    68,    69,    70,    71,    72,
    73,    74,    92,    76,    77,    78,    92,    80,    81,    92,
    83,    92,    85,    86,    87,    88,    89,    90,    34,    34,
    92,   766,    84,   775,    84,    25,    12,    96,   101,    95,
    43,    96,    95,    42,    92,    66,    67,    68,    69,    70,
    71,    72,    73,    74,    95,    76,    77,    78,    97,    80,
    81,    34,    83,   805,    85,    86,    87,    88,    89,    90,
    97,    34,    84,    35,    34,    84,    84,    95,    92,   821,
   101,    66,    67,    68,    69,    70,    71,    72,    73,    74,
    87,    76,    77,    78,    34,    80,    81,    92,    83,    92,
    85,    86,    87,    88,    89,    90,    98,    66,    67,    68,
    69,    70,    71,    72,    73,    74,   101,    76,    77,    78,
    98,    80,    81,    97,    83,    84,    85,    86,    87,    88,
    89,    90,    66,    67,    68,    69,    70,    71,    72,    73,
    74,    97,    76,    77,    78,    34,    80,    81,    99,    83,
    92,    85,    86,    87,    88,    89,    90,    66,    67,    68,
    69,    70,    71,    72,    73,    74,    84,    76,    77,    78,
    35,    80,    81,    92,    34,    92,    85,    86,    87,    88,
    89,    90,    66,    67,    68,    69,    70,    71,    98,    34,
    98,    34,    76,    77,    78,    99,    80,    81,    34,    99,
    92,    85,    86,    87,    88,    89,    90,    97,    34,    96,
    20,    93,    98,    21,    99,    34,    97,    34,    34,    99,
    52,    34,    99,    12,    96,    34,    20,    34,    34,    93,
    21,    99,    98,   100,    96,    19,    34,    12,    35,    34,
    99,    34,    34,    92,    34,    19,    34,   101,    35,    84,
    92,    92,    92,    92,    92,    92,    35,    84,    34,    84,
    84,    92,    35,    34,    92,     0,     0,   136,    25,   389,
   123,   322,   382,   502,   122,   505,    62,   448,    61,   447,
   201,   516,   515,   570,   567,   535,   346,    65,   457,   133,
   348,   413,    64,   411,   128,   410,   407,   213,   475,   132,
   417,   833,   836,   848,   845,   519,   646,   523,   580,   701,
   586,   633,   590,   304,   225,   673,   491,   484,   636,   422,
    -1,    -1,   712
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

case 21:
#line 269 "ned.y"
{
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                ;
    break;}
case 22:
#line 274 "ned.y"
{
                  /* no setTrailingComment(ps.imports,@3); comment already stored with last filename */
                ;
    break;}
case 25:
#line 286 "ned.y"
{
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                ;
    break;}
case 27:
#line 302 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 30:
#line 317 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 31:
#line 323 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 32:
#line 332 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 33:
#line 336 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 34:
#line 348 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 35:
#line 355 "ned.y"
{
                  use_chanattrname_token = 0;
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 38:
#line 370 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 39:
#line 376 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 41:
#line 396 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 42:
#line 406 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 43:
#line 410 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 44:
#line 424 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 45:
#line 431 "ned.y"
{
                  use_chanattrname_token = 0;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 47:
#line 456 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 48:
#line 466 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 49:
#line 470 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 50:
#line 487 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 51:
#line 494 "ned.y"
{
                  use_chanattrname_token = 0;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 54:
#line 513 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 55:
#line 518 "ned.y"
{
                ;
    break;}
case 60:
#line 534 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 63:
#line 551 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 64:
#line 556 "ned.y"
{
                ;
    break;}
case 69:
#line 572 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 72:
#line 589 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 75:
#line 604 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 78:
#line 619 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 79:
#line 624 "ned.y"
{
                ;
    break;}
case 82:
#line 635 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 83:
#line 639 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 86:
#line 654 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 87:
#line 659 "ned.y"
{
                ;
    break;}
case 90:
#line 670 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 91:
#line 674 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 93:
#line 689 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                ;
    break;}
case 94:
#line 693 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                ;
    break;}
case 95:
#line 697 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                ;
    break;}
case 96:
#line 701 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                ;
    break;}
case 97:
#line 705 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 98:
#line 709 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 99:
#line 713 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                ;
    break;}
case 100:
#line 717 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                ;
    break;}
case 101:
#line 721 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                ;
    break;}
case 104:
#line 736 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 105:
#line 741 "ned.y"
{
                ;
    break;}
case 114:
#line 764 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 115:
#line 769 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 118:
#line 782 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 119:
#line 787 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 122:
#line 803 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 123:
#line 808 "ned.y"
{
                ;
    break;}
case 132:
#line 831 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 133:
#line 836 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 136:
#line 849 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 137:
#line 854 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 140:
#line 870 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 141:
#line 875 "ned.y"
{
                ;
    break;}
case 146:
#line 891 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 147:
#line 897 "ned.y"
{
                ;
    break;}
case 148:
#line 900 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 149:
#line 907 "ned.y"
{
                ;
    break;}
case 150:
#line 910 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-3], yylsp[-1]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 151:
#line 916 "ned.y"
{
                ;
    break;}
case 152:
#line 919 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-4], yylsp[-1]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 153:
#line 926 "ned.y"
{
                ;
    break;}
case 157:
#line 946 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 158:
#line 951 "ned.y"
{
                ;
    break;}
case 163:
#line 967 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 164:
#line 973 "ned.y"
{
                ;
    break;}
case 165:
#line 976 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 166:
#line 983 "ned.y"
{
                ;
    break;}
case 167:
#line 986 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-3], yylsp[-1]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 168:
#line 992 "ned.y"
{
                ;
    break;}
case 169:
#line 995 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-4], yylsp[-1]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 170:
#line 1002 "ned.y"
{
                ;
    break;}
case 176:
#line 1027 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 177:
#line 1032 "ned.y"
{
                ;
    break;}
case 178:
#line 1035 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 179:
#line 1041 "ned.y"
{
                ;
    break;}
case 184:
#line 1057 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 189:
#line 1078 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 190:
#line 1083 "ned.y"
{
                ;
    break;}
case 191:
#line 1086 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 192:
#line 1092 "ned.y"
{
                ;
    break;}
case 197:
#line 1108 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 202:
#line 1129 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 203:
#line 1134 "ned.y"
{
                ;
    break;}
case 204:
#line 1137 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 205:
#line 1143 "ned.y"
{
                ;
    break;}
case 210:
#line 1160 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                ;
    break;}
case 215:
#line 1182 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 216:
#line 1187 "ned.y"
{
                ;
    break;}
case 217:
#line 1190 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 218:
#line 1196 "ned.y"
{
                ;
    break;}
case 223:
#line 1213 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-3]);
                  addExpression(ps.substparam, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparam,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 226:
#line 1230 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 227:
#line 1235 "ned.y"
{
                ;
    break;}
case 228:
#line 1238 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 229:
#line 1244 "ned.y"
{
                ;
    break;}
case 234:
#line 1260 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 235:
#line 1267 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 238:
#line 1283 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 239:
#line 1288 "ned.y"
{
                ;
    break;}
case 240:
#line 1291 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 241:
#line 1297 "ned.y"
{
                ;
    break;}
case 246:
#line 1313 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 247:
#line 1320 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 248:
#line 1331 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 250:
#line 1342 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 254:
#line 1358 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "false" );
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 255:
#line 1364 "ned.y"
{
                ;
    break;}
case 256:
#line 1367 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "yes" );
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 257:
#line 1373 "ned.y"
{
                ;
    break;}
case 264:
#line 1394 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                ;
    break;}
case 265:
#line 1398 "ned.y"
{
                  ps.inLoop=1;
                ;
    break;}
case 266:
#line 1402 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-7],yylsp[-4]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 269:
#line 1416 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 270:
#line 1426 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 272:
#line 1434 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 276:
#line 1447 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 277:
#line 1452 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 278:
#line 1457 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 279:
#line 1463 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 282:
#line 1477 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 283:
#line 1483 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 284:
#line 1491 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 285:
#line 1496 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 286:
#line 1503 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 287:
#line 1510 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 290:
#line 1524 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 291:
#line 1529 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 292:
#line 1536 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 293:
#line 1541 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 294:
#line 1548 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 295:
#line 1553 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 296:
#line 1561 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 297:
#line 1566 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 298:
#line 1571 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 301:
#line 1587 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "false" );
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 302:
#line 1593 "ned.y"
{
                ;
    break;}
case 303:
#line 1596 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "yes" );
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 304:
#line 1602 "ned.y"
{
                ;
    break;}
case 311:
#line 1623 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                ;
    break;}
case 312:
#line 1627 "ned.y"
{
                  ps.inLoop=1;
                ;
    break;}
case 313:
#line 1631 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-7],yylsp[-4]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 316:
#line 1645 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 317:
#line 1655 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 319:
#line 1663 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 323:
#line 1676 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 324:
#line 1681 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 325:
#line 1686 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 326:
#line 1692 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 329:
#line 1706 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 330:
#line 1712 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 331:
#line 1720 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 332:
#line 1725 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 333:
#line 1732 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 334:
#line 1739 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 337:
#line 1753 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 338:
#line 1758 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 339:
#line 1765 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 340:
#line 1770 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 341:
#line 1777 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 342:
#line 1782 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 343:
#line 1790 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 344:
#line 1795 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 345:
#line 1800 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 347:
#line 1818 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1],NULLPOS);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 348:
#line 1824 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-5],yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-6],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 349:
#line 1833 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 350:
#line 1847 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 351:
#line 1855 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1],NULLPOS);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 352:
#line 1861 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-5],yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-6],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 353:
#line 1874 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 354:
#line 1880 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 355:
#line 1884 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 356:
#line 1895 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 357:
#line 1897 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 358:
#line 1899 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 359:
#line 1901 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 361:
#line 1907 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 362:
#line 1910 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 363:
#line 1912 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 364:
#line 1914 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 365:
#line 1916 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 366:
#line 1918 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 367:
#line 1920 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 368:
#line 1924 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); ;
    break;}
case 369:
#line 1927 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 370:
#line 1929 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 371:
#line 1931 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 372:
#line 1933 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 373:
#line 1935 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 374:
#line 1937 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 375:
#line 1940 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 376:
#line 1942 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 377:
#line 1944 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 378:
#line 1948 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 379:
#line 1951 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 380:
#line 1953 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 381:
#line 1955 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 382:
#line 1959 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 383:
#line 1961 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 384:
#line 1963 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 385:
#line 1965 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 386:
#line 1968 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 387:
#line 1970 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 388:
#line 1972 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 389:
#line 1974 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 390:
#line 1976 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-9]), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 396:
#line 1989 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 397:
#line 1994 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                ;
    break;}
case 398:
#line 1999 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 399:
#line 2005 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 400:
#line 2011 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 401:
#line 2019 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 402:
#line 2024 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 403:
#line 2026 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 405:
#line 2035 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 406:
#line 2037 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 407:
#line 2039 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 408:
#line 2044 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 409:
#line 2046 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 410:
#line 2048 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 415:
#line 2065 "ned.y"
{
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                ;
    break;}
case 416:
#line 2071 "ned.y"
{
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                ;
    break;}
case 417:
#line 2080 "ned.y"
{
                  ps.cppstruct = (CppStructNode *)createNodeWithTag(NED_CPP_STRUCT, ps.nedfile );
                  ps.cppstruct->setName(toString(yylsp[-1]));
                  setComments(ps.cppstruct,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 418:
#line 2089 "ned.y"
{
                  ps.cppcobject = (CppCobjectNode *)createNodeWithTag(NED_CPP_COBJECT, ps.nedfile );
                  ps.cppcobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppcobject,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 419:
#line 2098 "ned.y"
{
                  ps.cppnoncobject = (CppNoncobjectNode *)createNodeWithTag(NED_CPP_NONCOBJECT, ps.nedfile );
                  ps.cppnoncobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppnoncobject,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 420:
#line 2107 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 421:
#line 2114 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 422:
#line 2118 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 423:
#line 2126 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 428:
#line 2143 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                ;
    break;}
case 429:
#line 2148 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                ;
    break;}
case 430:
#line 2157 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 431:
#line 2163 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 432:
#line 2167 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 433:
#line 2174 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 434:
#line 2181 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 435:
#line 2187 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 436:
#line 2191 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 437:
#line 2198 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 438:
#line 2205 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 439:
#line 2211 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 440:
#line 2215 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 441:
#line 2222 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 442:
#line 2229 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 449:
#line 2249 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 456:
#line 2268 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 463:
#line 2288 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                ;
    break;}
case 464:
#line 2294 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 465:
#line 2298 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(true);
                ;
    break;}
case 466:
#line 2305 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 476:
#line 2324 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 477:
#line 2329 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 479:
#line 2337 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 481:
#line 2345 "ned.y"
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
#line 2365 "ned.y"


//----------------------------------------------------------------------
// general bison/flex stuff:
//

char yyfailure[250] = "";
extern int yydebug; /* needed if compiled with yacc --VA */

extern char textbuf[];

int runparse (NEDParser *p,NedFileNode *nf,bool parseexpr, const char *sourcefname)
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

