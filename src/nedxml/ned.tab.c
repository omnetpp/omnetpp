/* A Bison parser, made from ned.y
   by GNU bison 1.34.  */

#define YYBISON 1  /* Identify Bison output.  */

#define YYLSP_NEEDED 1

# define	INCLUDE	257
# define	SIMPLE	258
# define	CHANNEL	259
# define	MODULE	260
# define	PARAMETERS	261
# define	GATES	262
# define	GATESIZES	263
# define	SUBMODULES	264
# define	CONNECTIONS	265
# define	DISPLAY	266
# define	IN	267
# define	OUT	268
# define	NOCHECK	269
# define	LEFT_ARROW	270
# define	RIGHT_ARROW	271
# define	FOR	272
# define	TO	273
# define	DO	274
# define	IF	275
# define	LIKE	276
# define	NETWORK	277
# define	ENDSIMPLE	278
# define	ENDMODULE	279
# define	ENDCHANNEL	280
# define	ENDNETWORK	281
# define	ENDFOR	282
# define	MACHINES	283
# define	ON	284
# define	IO_INTERFACES	285
# define	IFPAIR	286
# define	INTCONSTANT	287
# define	REALCONSTANT	288
# define	NAME	289
# define	STRINGCONSTANT	290
# define	CHARCONSTANT	291
# define	TRUE_	292
# define	FALSE_	293
# define	INPUT_	294
# define	REF	295
# define	ANCESTOR	296
# define	CONSTDECL	297
# define	NUMERICTYPE	298
# define	STRINGTYPE	299
# define	BOOLTYPE	300
# define	ANYTYPE	301
# define	CPPINCLUDE	302
# define	SYSINCFILENAME	303
# define	STRUCT	304
# define	COBJECT	305
# define	NONCOBJECT	306
# define	ENUM	307
# define	EXTENDS	308
# define	MESSAGE	309
# define	CLASS	310
# define	FIELDS	311
# define	PROPERTIES	312
# define	VIRTUAL	313
# define	CHARTYPE	314
# define	SHORTTYPE	315
# define	INTTYPE	316
# define	LONGTYPE	317
# define	DOUBLETYPE	318
# define	SIZEOF	319
# define	SUBMODINDEX	320
# define	EQ	321
# define	NE	322
# define	GT	323
# define	GE	324
# define	LS	325
# define	LE	326
# define	AND	327
# define	OR	328
# define	XOR	329
# define	NOT	330
# define	BIN_AND	331
# define	BIN_OR	332
# define	BIN_XOR	333
# define	BIN_COMPL	334
# define	SHIFT_LEFT	335
# define	SHIFT_RIGHT	336
# define	INVALID_CHAR	337
# define	UMIN	338

#line 76 "ned.y"


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

#ifndef YYSTYPE
#define YYSTYPE int
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
#endif

#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		849
#define	YYFLAG		-32768
#define	YYNTBASE	103

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 338 ? yytranslate[x] : 351)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    92
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     5,     6,     8,    10,    12,    14,    16,
      18,    20,    22,    24,    26,    28,    30,    32,    34,    36,
      38,    40,    41,    46,    50,    52,    54,    58,    61,    62,
      64,    69,    73,    77,    80,    86,    89,    91,    92,    98,
     103,   109,   112,   116,   119,   127,   130,   139,   142,   146,
     149,   160,   163,   165,   166,   167,   172,   175,   176,   180,
     182,   184,   186,   187,   188,   193,   196,   197,   201,   203,
     205,   207,   208,   213,   215,   216,   221,   223,   224,   225,
     230,   233,   234,   238,   240,   242,   243,   244,   249,   252,
     253,   257,   259,   261,   263,   267,   270,   274,   279,   284,
     288,   292,   296,   298,   299,   300,   305,   307,   308,   313,
     317,   322,   326,   330,   332,   336,   338,   342,   344,   348,
     350,   352,   353,   354,   359,   361,   362,   367,   371,   376,
     380,   384,   386,   390,   392,   396,   398,   402,   404,   406,
     407,   408,   413,   415,   416,   419,   421,   422,   430,   431,
     440,   441,   451,   452,   463,   467,   469,   470,   471,   476,
     478,   479,   482,   484,   485,   493,   494,   503,   504,   514,
     515,   526,   530,   532,   533,   536,   538,   539,   544,   545,
     552,   555,   556,   560,   562,   564,   566,   567,   570,   572,
     573,   578,   579,   586,   589,   590,   594,   596,   598,   600,
     601,   604,   606,   607,   612,   613,   620,   623,   624,   628,
     630,   634,   636,   637,   640,   642,   643,   648,   649,   656,
     658,   659,   662,   664,   669,   672,   673,   674,   679,   680,
     687,   690,   691,   695,   697,   700,   702,   705,   706,   707,
     712,   713,   720,   723,   724,   728,   730,   733,   735,   740,
     741,   746,   747,   749,   750,   751,   757,   758,   763,   765,
     766,   769,   771,   773,   775,   776,   777,   786,   790,   792,
     798,   801,   802,   805,   806,   809,   811,   818,   827,   834,
     843,   847,   849,   852,   854,   857,   859,   862,   864,   868,
     870,   873,   875,   878,   880,   883,   885,   887,   890,   894,
     896,   897,   898,   904,   905,   910,   912,   913,   916,   918,
     920,   922,   923,   924,   933,   937,   939,   945,   948,   949,
     952,   953,   956,   958,   965,   974,   981,   990,   994,   996,
     999,  1001,  1004,  1006,  1009,  1011,  1015,  1017,  1020,  1022,
    1025,  1027,  1030,  1032,  1034,  1037,  1041,  1046,  1052,  1060,
    1063,  1070,  1076,  1084,  1088,  1090,  1092,  1099,  1104,  1108,
    1110,  1112,  1116,  1120,  1124,  1128,  1132,  1136,  1140,  1143,
    1147,  1151,  1155,  1159,  1163,  1167,  1171,  1175,  1179,  1182,
    1186,  1190,  1194,  1197,  1201,  1205,  1211,  1215,  1220,  1227,
    1236,  1238,  1240,  1242,  1244,  1246,  1248,  1251,  1255,  1259,
    1262,  1264,  1266,  1268,  1270,  1272,  1276,  1281,  1283,  1285,
    1287,  1291,  1295,  1298,  1301,  1304,  1307,  1311,  1315,  1319,
    1320,  1328,  1329,  1339,  1341,  1342,  1345,  1347,  1350,  1355,
    1356,  1365,  1366,  1377,  1378,  1387,  1388,  1399,  1400,  1409,
    1410,  1421,  1422,  1427,  1428,  1430,  1431,  1434,  1436,  1441,
    1443,  1445,  1447,  1449,  1451,  1453,  1454,  1459,  1460,  1462,
    1463,  1466,  1468,  1469,  1477,  1478,  1487,  1489,  1492,  1494,
    1496,  1498,  1500,  1502,  1504,  1506,  1510,  1513,  1514,  1519,
    1520,  1523,  1524,  1526,  1528,  1530,  1532,  1534,  1536,  1538,
    1540,  1541,  1543
};
static const short yyrhs[] =
{
     104,     0,   104,   105,     0,     0,   106,     0,   115,     0,
     122,     0,   127,     0,   298,     0,   110,     0,   119,     0,
     124,     0,   295,     0,   312,     0,   313,     0,   314,     0,
     315,     0,   316,     0,   322,     0,   325,     0,   328,     0,
       0,     3,   107,   108,    93,     0,   108,    94,   109,     0,
     109,     0,    36,     0,   111,   112,   114,     0,     5,    35,
       0,     0,   113,     0,   113,    35,   301,   349,     0,    35,
     301,   349,     0,    26,    35,   349,     0,    26,   349,     0,
     116,    95,   117,    96,   349,     0,     5,    35,     0,   118,
       0,     0,   118,    35,    97,   301,    93,     0,    35,    97,
     301,    93,     0,   120,   129,   145,   157,   121,     0,     4,
      35,     0,    24,    35,   349,     0,    24,   349,     0,   123,
      95,   135,   150,   166,    96,   349,     0,     4,    35,     0,
     125,   129,   145,   157,   175,   245,   141,   126,     0,     6,
      35,     0,    25,    35,   349,     0,    25,   349,     0,   128,
      95,   135,   150,   166,   186,   270,   143,    96,   349,     0,
       6,    35,     0,   130,     0,     0,     0,    29,    85,   131,
     132,     0,   133,    93,     0,     0,   133,    94,   134,     0,
     134,     0,    35,     0,   136,     0,     0,     0,    29,    85,
     137,   138,     0,   139,    93,     0,     0,   139,    94,   140,
       0,   140,     0,    35,     0,   142,     0,     0,    12,    85,
      36,    93,     0,   144,     0,     0,    12,    85,    36,    93,
       0,   146,     0,     0,     0,     7,    85,   147,   148,     0,
     149,    93,     0,     0,   149,    94,   155,     0,   155,     0,
     151,     0,     0,     0,     7,    85,   152,   153,     0,   154,
      93,     0,     0,   154,    93,   156,     0,   156,     0,   156,
       0,    35,     0,    35,    85,    44,     0,    43,    35,     0,
      35,    85,    43,     0,    35,    85,    43,    44,     0,    35,
      85,    44,    43,     0,    35,    85,    45,     0,    35,    85,
      46,     0,    35,    85,    47,     0,   158,     0,     0,     0,
       8,    85,   159,   160,     0,   161,     0,     0,   161,    13,
     162,    93,     0,    13,   162,    93,     0,   161,    14,   164,
      93,     0,    14,   164,    93,     0,   162,    94,   163,     0,
     163,     0,    35,    98,    99,     0,    35,     0,   164,    94,
     165,     0,   165,     0,    35,    98,    99,     0,    35,     0,
     167,     0,     0,     0,     8,    85,   168,   169,     0,   170,
       0,     0,   170,    13,   171,    93,     0,    13,   171,    93,
       0,   170,    14,   173,    93,     0,    14,   173,    93,     0,
     171,    94,   172,     0,   172,     0,    35,    98,    99,     0,
      35,     0,   173,    94,   174,     0,   174,     0,    35,    98,
      99,     0,    35,     0,   176,     0,     0,     0,    10,    85,
     177,   178,     0,   179,     0,     0,   179,   180,     0,   180,
       0,     0,    35,    85,    35,   349,   181,   197,   185,     0,
       0,    35,    85,    35,   300,   349,   182,   197,   185,     0,
       0,    35,    85,    35,    22,    35,   349,   183,   197,   185,
       0,     0,    35,    85,    35,   300,    22,    35,   349,   184,
     197,   185,     0,   213,   229,   243,     0,   187,     0,     0,
       0,    10,    85,   188,   189,     0,   190,     0,     0,   190,
     191,     0,   191,     0,     0,    35,    85,    35,   349,   192,
     205,   196,     0,     0,    35,    85,    35,   300,   349,   193,
     205,   196,     0,     0,    35,    85,    35,    22,    35,   349,
     194,   205,   196,     0,     0,    35,    85,    35,   300,    22,
      35,   349,   195,   205,   196,     0,   221,   236,   244,     0,
     198,     0,     0,   198,   199,     0,   199,     0,     0,    30,
      85,   200,   202,     0,     0,    30,    21,   301,    85,   201,
     202,     0,   203,    93,     0,     0,   203,    94,   204,     0,
     204,     0,    35,     0,   206,     0,     0,   206,   207,     0,
     207,     0,     0,    30,    85,   208,   210,     0,     0,    30,
      21,   301,    85,   209,   210,     0,   211,    93,     0,     0,
     211,    94,   212,     0,   212,     0,    35,     0,   214,     0,
       0,   214,   215,     0,   215,     0,     0,     7,    85,   216,
     218,     0,     0,     7,    21,   301,    85,   217,   218,     0,
     219,    93,     0,     0,   219,    94,   220,     0,   220,     0,
      35,    97,   301,     0,   222,     0,     0,   222,   223,     0,
     223,     0,     0,     7,    85,   224,   226,     0,     0,     7,
      21,   301,    85,   225,   226,     0,   227,     0,     0,   227,
     228,     0,   228,     0,    35,    97,   301,    93,     0,   229,
     230,     0,     0,     0,     9,    85,   231,   233,     0,     0,
       9,    21,   301,    85,   232,   233,     0,   234,    93,     0,
       0,   234,    94,   235,     0,   235,     0,    35,   300,     0,
      35,     0,   236,   237,     0,     0,     0,     9,    85,   238,
     240,     0,     0,     9,    21,   301,    85,   239,   240,     0,
     241,    93,     0,     0,   241,    94,   242,     0,   242,     0,
      35,   300,     0,    35,     0,    12,    85,    36,    93,     0,
       0,    12,    85,    36,    93,     0,     0,   246,     0,     0,
       0,    11,    15,    85,   247,   249,     0,     0,    11,    85,
     248,   249,     0,   250,     0,     0,   250,   251,     0,   251,
       0,   252,     0,   260,     0,     0,     0,    18,   253,   255,
      20,   254,   259,    28,   349,     0,   256,    94,   255,     0,
     256,     0,    35,    97,   301,    19,   301,     0,    21,   301,
       0,     0,    12,    36,     0,     0,   259,   260,     0,   260,
       0,   261,    17,   265,   257,   258,   350,     0,   261,    17,
     269,    17,   265,   257,   258,   350,     0,   261,    16,   265,
     257,   258,   350,     0,   261,    16,   269,    16,   265,   257,
     258,   350,     0,   262,   100,   263,     0,   264,     0,    35,
     300,     0,    35,     0,    35,   300,     0,    35,     0,    35,
     300,     0,    35,     0,   266,   100,   267,     0,   268,     0,
      35,   300,     0,    35,     0,    35,   300,     0,    35,     0,
      35,   300,     0,    35,     0,    35,     0,    35,   301,     0,
     269,    35,   301,     0,   271,     0,     0,     0,    11,    15,
      85,   272,   274,     0,     0,    11,    85,   273,   274,     0,
     275,     0,     0,   275,   276,     0,   276,     0,   277,     0,
     285,     0,     0,     0,    18,   278,   280,    20,   279,   284,
      28,    93,     0,   281,    94,   280,     0,   281,     0,    35,
      97,   301,    19,   301,     0,    21,   301,     0,     0,    12,
      36,     0,     0,   284,   285,     0,   285,     0,   286,    17,
     290,   282,   283,    93,     0,   286,    17,   294,    17,   290,
     282,   283,    93,     0,   286,    16,   290,   282,   283,    93,
       0,   286,    16,   294,    16,   290,   282,   283,    93,     0,
     287,   100,   288,     0,   289,     0,    35,   300,     0,    35,
       0,    35,   300,     0,    35,     0,    35,   300,     0,    35,
       0,   291,   100,   292,     0,   293,     0,    35,   300,     0,
      35,     0,    35,   300,     0,    35,     0,    35,   300,     0,
      35,     0,    35,     0,    35,   301,     0,   294,    35,   301,
       0,   296,   197,   213,   297,     0,    23,    35,    85,    35,
     349,     0,    23,    35,    85,    35,    22,    35,   349,     0,
      27,   349,     0,   299,    95,   205,   221,    96,   349,     0,
      23,    35,    85,    35,   349,     0,    23,    35,    85,    35,
      22,    35,   349,     0,    98,   301,    99,     0,   303,     0,
     302,     0,    40,   101,   303,    94,   303,   102,     0,    40,
     101,   303,   102,     0,    40,   101,   102,     0,    40,     0,
     304,     0,   101,   303,   102,     0,   303,    86,   303,     0,
     303,    87,   303,     0,   303,    88,   303,     0,   303,    89,
     303,     0,   303,    90,   303,     0,   303,    91,   303,     0,
      87,   303,     0,   303,    67,   303,     0,   303,    68,   303,
       0,   303,    69,   303,     0,   303,    70,   303,     0,   303,
      71,   303,     0,   303,    72,   303,     0,   303,    73,   303,
       0,   303,    74,   303,     0,   303,    75,   303,     0,    76,
     303,     0,   303,    77,   303,     0,   303,    78,   303,     0,
     303,    79,   303,     0,    80,   303,     0,   303,    81,   303,
       0,   303,    82,   303,     0,   303,    84,   303,    85,   303,
       0,    35,   101,   102,     0,    35,   101,   303,   102,     0,
      35,   101,   303,    94,   303,   102,     0,    35,   101,   303,
      94,   303,    94,   303,   102,     0,   305,     0,   306,     0,
     307,     0,   308,     0,   309,     0,    35,     0,    41,    35,
       0,    41,    42,    35,     0,    42,    41,    35,     0,    42,
      35,     0,    36,     0,    38,     0,    39,     0,   310,     0,
      66,     0,    66,   101,   102,     0,    65,   101,    35,   102,
       0,    33,     0,    34,     0,   311,     0,   311,    33,    35,
       0,   311,    34,    35,     0,    33,    35,     0,    34,    35,
       0,    48,    36,     0,    48,    49,     0,    50,    35,    93,
       0,    51,    35,    93,     0,    52,    35,    93,     0,     0,
      53,    35,    95,   317,   319,    96,    93,     0,     0,    53,
      35,    54,    35,    95,   318,   319,    96,    93,     0,   320,
       0,     0,   320,   321,     0,   321,     0,    35,    93,     0,
      35,    97,    33,    93,     0,     0,    55,    35,    95,   323,
     331,   337,    96,    93,     0,     0,    55,    35,    54,    35,
      95,   324,   331,   337,    96,    93,     0,     0,    56,    35,
      95,   326,   331,   337,    96,    93,     0,     0,    56,    35,
      54,    35,    95,   327,   331,   337,    96,    93,     0,     0,
      50,    35,    95,   329,   331,   337,    96,    93,     0,     0,
      50,    35,    54,    35,    95,   330,   331,   337,    96,    93,
       0,     0,    58,    85,   332,   333,     0,     0,   334,     0,
       0,   334,   335,     0,   335,     0,    35,    97,   336,    93,
       0,    36,     0,    33,     0,    34,     0,   311,     0,    38,
       0,    39,     0,     0,    57,    85,   338,   339,     0,     0,
     340,     0,     0,   340,   341,     0,   341,     0,     0,   344,
      35,   342,   345,   346,   347,    93,     0,     0,    59,   344,
      35,   343,   345,   346,   347,    93,     0,    35,     0,    35,
      88,     0,    60,     0,    61,     0,    62,     0,    63,     0,
      64,     0,    45,     0,    46,     0,    98,    33,    99,     0,
      98,    99,     0,     0,    53,   101,    35,   102,     0,     0,
      97,   348,     0,     0,    36,     0,    37,     0,    33,     0,
      34,     0,   311,     0,    38,     0,    39,     0,    93,     0,
       0,    94,     0,    93,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   227,   231,   232,   236,   237,   238,   239,   240,   242,
     243,   244,   245,   247,   248,   249,   250,   251,   252,   253,
     254,   261,   261,   273,   274,   278,   290,   294,   303,   304,
     308,   314,   323,   327,   337,   346,   355,   356,   360,   366,
     378,   386,   395,   399,   409,   420,   433,   444,   453,   457,
     467,   481,   494,   495,   499,   499,   510,   511,   515,   516,
     520,   532,   533,   537,   537,   548,   549,   553,   554,   558,
     570,   571,   575,   585,   586,   590,   600,   601,   605,   605,
     616,   617,   621,   625,   635,   636,   640,   640,   651,   652,
     656,   660,   671,   675,   679,   683,   687,   691,   695,   699,
     703,   707,   717,   718,   722,   722,   733,   734,   738,   739,
     740,   741,   745,   746,   750,   755,   763,   764,   768,   773,
     784,   785,   789,   789,   800,   801,   805,   806,   807,   808,
     812,   813,   817,   822,   830,   831,   835,   840,   851,   852,
     856,   856,   867,   868,   872,   873,   877,   877,   886,   886,
     896,   896,   905,   905,   918,   927,   928,   932,   932,   943,
     944,   948,   949,   953,   953,   962,   962,   972,   972,   981,
     981,   994,  1003,  1004,  1008,  1009,  1013,  1013,  1021,  1021,
    1033,  1034,  1038,  1039,  1043,  1054,  1055,  1059,  1060,  1064,
    1064,  1072,  1072,  1084,  1085,  1089,  1090,  1094,  1105,  1106,
    1110,  1111,  1115,  1115,  1123,  1123,  1136,  1137,  1141,  1142,
    1146,  1158,  1159,  1163,  1164,  1168,  1168,  1176,  1176,  1189,
    1190,  1194,  1195,  1199,  1211,  1212,  1216,  1216,  1224,  1224,
    1236,  1237,  1241,  1242,  1246,  1253,  1264,  1265,  1269,  1269,
    1277,  1277,  1289,  1290,  1294,  1295,  1299,  1306,  1317,  1321,
    1328,  1332,  1339,  1340,  1344,  1344,  1353,  1353,  1365,  1366,
    1370,  1371,  1375,  1376,  1380,  1380,  1380,  1397,  1398,  1402,
    1412,  1416,  1420,  1424,  1428,  1429,  1433,  1438,  1443,  1449,
    1458,  1459,  1463,  1469,  1477,  1482,  1489,  1496,  1505,  1506,
    1510,  1515,  1522,  1527,  1534,  1539,  1547,  1552,  1557,  1568,
    1569,  1573,  1573,  1582,  1582,  1594,  1595,  1599,  1600,  1604,
    1605,  1609,  1609,  1609,  1626,  1627,  1631,  1641,  1645,  1649,
    1653,  1657,  1658,  1662,  1667,  1672,  1678,  1687,  1688,  1692,
    1698,  1706,  1711,  1718,  1725,  1734,  1735,  1739,  1744,  1751,
    1756,  1763,  1768,  1776,  1781,  1786,  1797,  1804,  1810,  1819,
    1830,  1841,  1847,  1860,  1865,  1870,  1881,  1883,  1885,  1887,
    1892,  1893,  1896,  1898,  1900,  1902,  1904,  1906,  1909,  1913,
    1915,  1917,  1919,  1921,  1923,  1926,  1928,  1930,  1933,  1937,
    1939,  1941,  1944,  1947,  1949,  1951,  1954,  1956,  1958,  1960,
    1965,  1966,  1967,  1968,  1969,  1973,  1978,  1983,  1989,  1995,
    2003,  2008,  2010,  2015,  2019,  2021,  2023,  2028,  2030,  2032,
    2038,  2039,  2040,  2041,  2049,  2055,  2064,  2073,  2082,  2091,
    2091,  2102,  2102,  2117,  2118,  2122,  2123,  2127,  2132,  2141,
    2141,  2151,  2151,  2165,  2165,  2175,  2175,  2189,  2189,  2199,
    2199,  2213,  2213,  2219,  2223,  2224,  2228,  2229,  2233,  2243,
    2244,  2245,  2246,  2247,  2248,  2252,  2252,  2258,  2262,  2263,
    2267,  2268,  2272,  2272,  2282,  2282,  2296,  2297,  2298,  2299,
    2300,  2301,  2302,  2303,  2304,  2308,  2313,  2317,  2321,  2325,
    2329,  2333,  2337,  2338,  2339,  2340,  2341,  2342,  2343,  2346,
    2346,  2348,  2348
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "INCLUDE", "SIMPLE", "CHANNEL", "MODULE", 
  "PARAMETERS", "GATES", "GATESIZES", "SUBMODULES", "CONNECTIONS", 
  "DISPLAY", "IN", "OUT", "NOCHECK", "LEFT_ARROW", "RIGHT_ARROW", "FOR", 
  "TO", "DO", "IF", "LIKE", "NETWORK", "ENDSIMPLE", "ENDMODULE", 
  "ENDCHANNEL", "ENDNETWORK", "ENDFOR", "MACHINES", "ON", "IO_INTERFACES", 
  "IFPAIR", "INTCONSTANT", "REALCONSTANT", "NAME", "STRINGCONSTANT", 
  "CHARCONSTANT", "TRUE_", "FALSE_", "INPUT_", "REF", "ANCESTOR", 
  "CONSTDECL", "NUMERICTYPE", "STRINGTYPE", "BOOLTYPE", "ANYTYPE", 
  "CPPINCLUDE", "SYSINCFILENAME", "STRUCT", "COBJECT", "NONCOBJECT", 
  "ENUM", "EXTENDS", "MESSAGE", "CLASS", "FIELDS", "PROPERTIES", 
  "VIRTUAL", "CHARTYPE", "SHORTTYPE", "INTTYPE", "LONGTYPE", "DOUBLETYPE", 
  "SIZEOF", "SUBMODINDEX", "EQ", "NE", "GT", "GE", "LS", "LE", "AND", 
  "OR", "XOR", "NOT", "BIN_AND", "BIN_OR", "BIN_XOR", "BIN_COMPL", 
  "SHIFT_LEFT", "SHIFT_RIGHT", "INVALID_CHAR", "'?'", "':'", "'+'", "'-'", 
  "'*'", "'/'", "'%'", "'^'", "UMIN", "';'", "','", "'{'", "'}'", "'='", 
  "'['", "']'", "'.'", "'('", "')'", "networkdescription", 
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

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,   103,   104,   104,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   107,   106,   108,   108,   109,   110,   111,   112,   112,
     113,   113,   114,   114,   115,   116,   117,   117,   118,   118,
     119,   120,   121,   121,   122,   123,   124,   125,   126,   126,
     127,   128,   129,   129,   131,   130,   132,   132,   133,   133,
     134,   135,   135,   137,   136,   138,   138,   139,   139,   140,
     141,   141,   142,   143,   143,   144,   145,   145,   147,   146,
     148,   148,   149,   149,   150,   150,   152,   151,   153,   153,
     154,   154,   155,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   157,   157,   159,   158,   160,   160,   161,   161,
     161,   161,   162,   162,   163,   163,   164,   164,   165,   165,
     166,   166,   168,   167,   169,   169,   170,   170,   170,   170,
     171,   171,   172,   172,   173,   173,   174,   174,   175,   175,
     177,   176,   178,   178,   179,   179,   181,   180,   182,   180,
     183,   180,   184,   180,   185,   186,   186,   188,   187,   189,
     189,   190,   190,   192,   191,   193,   191,   194,   191,   195,
     191,   196,   197,   197,   198,   198,   200,   199,   201,   199,
     202,   202,   203,   203,   204,   205,   205,   206,   206,   208,
     207,   209,   207,   210,   210,   211,   211,   212,   213,   213,
     214,   214,   216,   215,   217,   215,   218,   218,   219,   219,
     220,   221,   221,   222,   222,   224,   223,   225,   223,   226,
     226,   227,   227,   228,   229,   229,   231,   230,   232,   230,
     233,   233,   234,   234,   235,   235,   236,   236,   238,   237,
     239,   237,   240,   240,   241,   241,   242,   242,   243,   243,
     244,   244,   245,   245,   247,   246,   248,   246,   249,   249,
     250,   250,   251,   251,   253,   254,   252,   255,   255,   256,
     257,   257,   258,   258,   259,   259,   260,   260,   260,   260,
     261,   261,   262,   262,   263,   263,   264,   264,   265,   265,
     266,   266,   267,   267,   268,   268,   269,   269,   269,   270,
     270,   272,   271,   273,   271,   274,   274,   275,   275,   276,
     276,   278,   279,   277,   280,   280,   281,   282,   282,   283,
     283,   284,   284,   285,   285,   285,   285,   286,   286,   287,
     287,   288,   288,   289,   289,   290,   290,   291,   291,   292,
     292,   293,   293,   294,   294,   294,   295,   296,   296,   297,
     298,   299,   299,   300,   301,   301,   302,   302,   302,   302,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     304,   304,   304,   304,   304,   305,   305,   305,   305,   305,
     306,   307,   307,   308,   309,   309,   309,   310,   310,   310,
     311,   311,   311,   311,   312,   312,   313,   314,   315,   317,
     316,   318,   316,   319,   319,   320,   320,   321,   321,   323,
     322,   324,   322,   326,   325,   327,   325,   329,   328,   330,
     328,   332,   331,   331,   333,   333,   334,   334,   335,   336,
     336,   336,   336,   336,   336,   338,   337,   337,   339,   339,
     340,   340,   342,   341,   343,   341,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   345,   345,   345,   346,   346,
     347,   347,   348,   348,   348,   348,   348,   348,   348,   349,
     349,   350,   350
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     2,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     4,     3,     1,     1,     3,     2,     0,     1,
       4,     3,     3,     2,     5,     2,     1,     0,     5,     4,
       5,     2,     3,     2,     7,     2,     8,     2,     3,     2,
      10,     2,     1,     0,     0,     4,     2,     0,     3,     1,
       1,     1,     0,     0,     4,     2,     0,     3,     1,     1,
       1,     0,     4,     1,     0,     4,     1,     0,     0,     4,
       2,     0,     3,     1,     1,     0,     0,     4,     2,     0,
       3,     1,     1,     1,     3,     2,     3,     4,     4,     3,
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
       2,     1,     1,     1,     0,     0,     8,     3,     1,     5,
       2,     0,     2,     0,     2,     1,     6,     8,     6,     8,
       3,     1,     2,     1,     2,     1,     2,     1,     3,     1,
       2,     1,     2,     1,     2,     1,     1,     2,     3,     1,
       0,     0,     5,     0,     4,     1,     0,     2,     1,     1,
       1,     0,     0,     8,     3,     1,     5,     2,     0,     2,
       0,     2,     1,     6,     8,     6,     8,     3,     1,     2,
       1,     2,     1,     2,     1,     3,     1,     2,     1,     2,
       1,     2,     1,     1,     2,     3,     4,     5,     7,     2,
       6,     5,     7,     3,     1,     1,     6,     4,     3,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       3,     3,     2,     3,     3,     5,     3,     4,     6,     8,
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

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       3,     1,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     2,     4,     9,    28,     5,     0,
      10,    53,     6,     0,    11,    53,     7,     0,    12,   173,
       8,     0,    13,    14,    15,    16,    17,    18,    19,    20,
       0,    41,    27,    47,     0,   414,   415,     0,     0,     0,
       0,     0,     0,     0,     0,    29,    37,     0,    77,    52,
      62,    77,    62,     0,   199,   172,   175,   186,    25,     0,
      24,     0,     0,   416,   437,   417,   418,     0,   419,     0,
     429,     0,   433,   407,   408,   395,   400,   401,   402,   359,
       0,     0,     0,   404,     0,     0,     0,     0,   490,   355,
     354,   360,   390,   391,   392,   393,   394,   403,   409,   490,
      26,     0,     0,     0,    36,    54,     0,   103,    76,     0,
      85,    61,   103,    85,     0,   176,     0,     0,   198,   201,
     174,     0,   212,   185,   188,    22,     0,   490,     0,   443,
       0,   424,     0,   443,     0,   443,   412,   413,     0,     0,
     396,     0,   399,     0,     0,     0,   378,   382,   368,     0,
     489,    31,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   490,    33,   490,     0,   490,
       0,    57,    78,     0,     0,   102,    63,     0,   121,    84,
     139,   121,     0,   181,     0,   202,   490,   346,   200,     0,
     189,     0,     0,   211,   214,   187,    23,     0,   347,   439,
       0,   457,   421,     0,     0,   423,   426,   431,   457,   435,
     457,   386,     0,   358,     0,   397,   398,     0,   405,   361,
     369,   370,   371,   372,   373,   374,   375,   376,   377,   379,
     380,   381,   383,   384,     0,   362,   363,   364,   365,   366,
     367,   410,   411,    32,    30,     0,    34,     0,    60,    55,
       0,    59,    81,   104,   490,    40,    66,    86,     0,     0,
     120,     0,   253,   138,   156,   178,   184,   177,     0,   183,
       0,   207,   349,     0,   194,     0,   215,   490,   213,   490,
     443,   441,     0,     0,   424,   427,     0,     0,   425,   443,
       0,   443,     0,     0,   387,     0,   357,   406,     0,    39,
       0,    56,     0,    93,     0,    79,     0,    83,    92,   107,
     490,    43,    69,    64,     0,    68,    89,   122,   490,   140,
       0,    71,   252,     0,   300,   155,   181,   180,     0,   204,
       0,   203,     0,   209,   191,   197,   190,     0,   196,     0,
     220,   350,   348,   457,   445,   455,     0,     0,     0,   420,
     457,     0,   457,     0,     0,     0,   385,    38,    58,     0,
      95,    80,     0,     0,     0,   105,   106,    42,    65,     0,
      87,     0,    91,   125,    44,   143,     0,   256,     0,     0,
      70,   157,     0,    74,   299,   179,   182,   207,     0,   206,
       0,   194,   193,     0,   217,     0,   216,   219,   222,     0,
       0,   442,   444,   447,   459,   438,     0,   428,     0,   430,
       0,   434,     0,   388,   356,    96,    94,    99,   100,   101,
      82,   115,     0,   113,   119,     0,   117,     0,     0,    67,
      88,     0,     0,   123,   124,     0,   141,   142,   145,   254,
     259,     0,   490,    46,   160,     0,   303,     0,     0,    73,
     205,   210,   208,   192,   195,   220,     0,   221,     0,     0,
     446,   466,   473,   474,     0,   468,   469,   470,   471,   472,
     456,   458,   461,     0,   422,     0,     0,     0,    97,    98,
       0,   109,     0,     0,   111,     0,     0,     0,    90,   133,
       0,   131,   137,     0,   135,     0,     0,     0,   144,   259,
     264,   287,   257,   258,   261,   262,   263,     0,     0,   281,
       0,   490,    49,     0,   158,   159,   162,   301,   306,     0,
     490,   218,     0,   440,   450,   451,   449,   453,   454,   452,
       0,   467,     0,   460,   462,   432,   436,   389,   114,   112,
     118,   116,   108,   110,     0,   127,     0,     0,   129,     0,
       0,     0,   490,   255,     0,     0,   286,   260,     0,     0,
       0,    72,    48,     0,   161,   306,   311,   334,   304,   305,
     308,   309,   310,     0,     0,   328,     0,    50,   223,   448,
     464,   477,   132,   130,   136,   134,   126,   128,     0,   490,
     146,     0,     0,   268,     0,   295,   271,     0,   289,     0,
     271,     0,   285,   280,   490,   302,     0,   333,   307,     0,
       0,     0,    75,   477,     0,   479,   490,     0,   148,   173,
       0,   265,     0,   353,   294,   297,     0,   273,     0,     0,
       0,   273,     0,   284,     0,   490,   163,     0,     0,   315,
     342,   318,     0,   336,     0,   318,     0,   332,   327,   479,
       0,   476,     0,   481,   150,   490,   173,   199,     0,     0,
     267,   270,     0,     0,   293,   288,   295,   271,   298,     0,
     271,   490,     0,   165,   186,     0,   312,     0,   341,   344,
       0,   320,     0,     0,     0,   320,     0,   331,   481,   475,
       0,     0,     0,   173,   152,   199,   147,   225,     0,     0,
     275,   272,   492,   491,   278,   292,   273,   276,   273,   167,
     490,   186,   212,     0,     0,   314,   317,     0,     0,   340,
     335,   342,   318,   345,     0,   318,     0,     0,   484,   485,
     482,   483,   487,   488,   486,   480,   463,   199,   173,   149,
     249,   269,   490,   274,     0,     0,   186,   169,   212,   164,
     237,     0,     0,   322,   319,   325,   339,   320,   323,   320,
     465,   478,   151,   199,     0,     0,   224,   154,   266,   279,
     277,   212,   186,   166,   251,   316,     0,   321,     0,     0,
     153,     0,   226,     0,   168,   212,     0,     0,   236,   171,
     313,   326,   324,     0,   231,     0,   170,     0,   238,     0,
     228,   235,   227,     0,   233,   248,     0,   243,     0,   231,
     234,   230,     0,   240,   247,   239,     0,   245,   250,   229,
     232,   243,   246,   242,     0,   241,   244,     0,     0,     0
};

static const short yydefgoto[] =
{
     847,     1,    14,    15,    40,    69,    70,    16,    17,    54,
      55,   110,    18,    19,   113,   114,    20,    21,   275,    22,
      23,    24,    25,   463,    26,    27,    58,    59,   191,   269,
     270,   271,   120,   121,   276,   333,   334,   335,   399,   400,
     468,   469,   117,   118,   272,   325,   326,   198,   199,   336,
     390,   391,   327,   328,   194,   195,   329,   385,   386,   442,
     443,   445,   446,   279,   280,   393,   453,   454,   510,   511,
     513,   514,   282,   283,   395,   456,   457,   458,   639,   676,
     713,   758,   716,   344,   345,   464,   534,   535,   536,   694,
     731,   766,   792,   769,    64,    65,    66,   203,   346,   287,
     288,   289,   132,   133,   134,   294,   411,   356,   357,   358,
     717,   128,   129,   291,   407,   351,   352,   353,   770,   213,
     214,   360,   475,   416,   417,   418,   760,   786,   814,   829,
     822,   823,   824,   794,   808,   827,   841,   835,   836,   837,
     787,   809,   341,   342,   519,   460,   522,   523,   524,   525,
     574,   679,   612,   613,   647,   683,   719,   526,   527,   528,
     623,   529,   616,   617,   685,   618,   619,   403,   404,   585,
     538,   588,   589,   590,   591,   626,   734,   658,   659,   701,
     738,   772,   592,   593,   594,   668,   595,   661,   662,   740,
     663,   664,    28,    29,   207,    30,    31,   644,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,    32,
      33,    34,    35,    36,   141,   304,   224,   225,   226,    37,
     143,   309,    38,   145,   311,    39,   139,   300,   221,   364,
     421,   422,   423,   550,   303,   424,   490,   491,   492,   601,
     633,   493,   635,   673,   712,   755,   161,   724
};

static const short yypact[] =
{
  -32768,   443,-32768,    35,   105,   147,   173,    -4,   180,   182,
     188,   220,   235,   247,-32768,-32768,-32768,   252,-32768,    67,
  -32768,   144,-32768,   151,-32768,   144,-32768,   168,-32768,   191,
  -32768,   195,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
     272,   217,   219,   237,   258,-32768,-32768,   -28,   226,   236,
     -37,   -32,   -31,   364,   320,   314,   322,   274,   357,-32768,
     341,   357,   341,   122,   378,   191,-32768,   359,-32768,   -50,
  -32768,   355,   356,-32768,-32768,-32768,-32768,   360,-32768,   361,
  -32768,   366,-32768,   381,   384,   291,-32768,-32768,-32768,   293,
     119,   116,   312,   319,   376,   376,   376,   376,   328,-32768,
     718,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   134,   117,
  -32768,   364,   325,   327,   389,-32768,   340,   418,-32768,   343,
     425,-32768,   418,   425,   364,-32768,   124,   404,   378,-32768,
  -32768,   126,   426,   359,-32768,-32768,   272,   -15,   339,   377,
     342,   403,   344,   377,   348,   377,-32768,-32768,   250,   306,
  -32768,   410,-32768,   415,   419,   351,-32768,-32768,-32768,   605,
  -32768,-32768,   376,   376,   376,   376,   376,   376,   376,   376,
     376,   376,   376,   376,   376,   376,   376,   376,   376,   376,
     376,   376,   376,   420,   422,   328,-32768,   328,   364,   328,
     363,   423,-32768,   382,   437,-32768,-32768,   386,   454,-32768,
     462,   454,   388,   439,   364,-32768,   328,-32768,-32768,   364,
  -32768,   127,   379,   426,-32768,-32768,-32768,   441,   383,-32768,
     394,   432,-32768,    72,   385,   403,-32768,-32768,   432,-32768,
     432,-32768,   486,-32768,   527,-32768,-32768,   380,-32768,-32768,
     435,   435,   435,   435,   435,   435,   768,   768,   768,   214,
     214,   214,   190,   190,   693,    88,    88,   392,   392,   392,
     392,-32768,-32768,-32768,-32768,   387,-32768,   364,-32768,-32768,
     138,-32768,     7,-32768,   123,-32768,   455,-32768,   407,   401,
  -32768,   416,   489,-32768,   492,-32768,-32768,-32768,   148,-32768,
     421,   468,-32768,   430,   472,   364,-32768,   328,-32768,   328,
     377,-32768,   433,   412,   403,-32768,   476,   417,-32768,   377,
     431,   377,   434,   376,-32768,   376,-32768,-32768,   376,-32768,
     438,-32768,   423,   448,   484,-32768,   154,-32768,-32768,   239,
     328,-32768,-32768,-32768,   167,-32768,     7,-32768,   328,-32768,
       1,   499,-32768,   449,   517,-32768,   439,-32768,   439,-32768,
     440,-32768,   200,-32768,-32768,-32768,-32768,   213,-32768,   451,
     503,-32768,   445,   432,   506,-32768,   450,   446,   452,-32768,
     432,   453,   432,   457,   563,   641,   743,-32768,-32768,   192,
  -32768,-32768,     7,   509,   512,-32768,   244,-32768,-32768,   455,
  -32768,   459,-32768,   297,-32768,   513,   477,-32768,   481,   544,
  -32768,-32768,    46,   566,-32768,-32768,-32768,   468,   364,-32768,
     468,   472,-32768,   472,-32768,   482,-32768,   503,-32768,   485,
     488,-32768,   506,-32768,   315,-32768,   490,-32768,   494,-32768,
     495,-32768,   376,-32768,-32768,   538,   543,-32768,-32768,-32768,
  -32768,   505,   224,-32768,   514,   228,-32768,   509,   512,-32768,
       7,   557,   575,-32768,   310,   534,-32768,   513,-32768,-32768,
      16,   584,   125,-32768,   587,   539,-32768,   540,   531,-32768,
  -32768,-32768,-32768,-32768,-32768,   503,   364,-32768,   530,   186,
  -32768,   551,-32768,-32768,   424,-32768,-32768,-32768,-32768,-32768,
  -32768,   315,-32768,   593,-32768,   550,   553,   667,-32768,-32768,
     549,-32768,   509,   556,-32768,   512,   234,   242,-32768,   558,
     260,-32768,   561,   262,-32768,   557,   575,   626,-32768,    16,
  -32768,   -46,-32768,    16,-32768,-32768,-32768,   211,   562,-32768,
     570,   328,-32768,   579,-32768,   587,-32768,-32768,    39,   630,
     328,-32768,   574,-32768,   381,   384,-32768,-32768,-32768,   134,
     576,-32768,   633,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,   571,-32768,   557,   572,-32768,   575,
     269,   273,    34,-32768,   646,   364,   585,-32768,   653,   653,
     662,-32768,-32768,   663,-32768,    39,-32768,    55,-32768,    39,
  -32768,-32768,-32768,   352,   599,-32768,   607,-32768,-32768,-32768,
  -32768,   603,-32768,-32768,-32768,-32768,-32768,-32768,   668,     6,
  -32768,   608,   682,   610,   618,   164,   700,   624,-32768,    11,
     700,     4,   628,-32768,    37,-32768,   698,   647,-32768,   715,
     715,   717,-32768,   603,   -19,   706,   328,   738,-32768,   191,
     364,-32768,   646,-32768,   676,-32768,   364,   782,   763,   766,
     364,   782,   766,-32768,   784,    31,-32768,   726,   783,   732,
     233,   806,   728,-32768,    13,   806,    14,   628,-32768,   706,
     742,-32768,   741,   746,-32768,   328,   191,   378,   825,   813,
  -32768,-32768,   815,   287,   628,-32768,    85,   700,-32768,   287,
     700,   328,   817,-32768,   359,   364,-32768,   698,   753,-32768,
     364,   848,   826,   827,   364,   848,   827,-32768,   746,-32768,
     828,   153,   771,   191,-32768,   378,-32768,-32768,   364,    38,
  -32768,-32768,-32768,-32768,-32768,-32768,   782,-32768,   782,-32768,
     328,   359,   426,   846,   831,-32768,-32768,   832,   774,   628,
  -32768,    96,   806,-32768,   776,   806,   777,   769,   381,   384,
  -32768,-32768,-32768,-32768,   134,-32768,-32768,   378,   191,-32768,
      60,-32768,   328,-32768,   287,   287,   359,-32768,   426,-32768,
  -32768,   364,   131,-32768,-32768,-32768,-32768,   848,-32768,   848,
  -32768,-32768,-32768,   378,   128,   787,-32768,-32768,-32768,-32768,
  -32768,   426,   359,-32768,   130,-32768,   780,-32768,   781,   785,
  -32768,   364,-32768,   839,-32768,   426,   129,   791,-32768,-32768,
  -32768,-32768,-32768,   792,   844,   788,-32768,   364,-32768,   847,
  -32768,   628,-32768,   290,-32768,-32768,   795,   849,   789,   844,
  -32768,-32768,   844,-32768,   628,-32768,   294,-32768,-32768,-32768,
  -32768,   849,-32768,-32768,   849,-32768,-32768,   885,   886,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,-32768,-32768,   751,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,   863,-32768,-32768,-32768,
  -32768,   567,   829,-32768,-32768,-32768,-32768,   501,-32768,-32768,
  -32768,-32768,   833,-32768,-32768,-32768,-32768,   770,-32768,-32768,
  -32768,-32768,   510,  -325,   773,-32768,-32768,-32768,-32768,   456,
     395,   458,   391,   697,-32768,-32768,-32768,-32768,   390,   333,
     393,   331,-32768,-32768,-32768,-32768,-32768,   444,-32768,-32768,
  -32768,-32768,  -582,-32768,-32768,-32768,-32768,-32768,   367,-32768,
  -32768,-32768,-32768,  -750,  -629,-32768,   842,-32768,-32768,   564,
  -32768,   560,  -691,-32768,   778,-32768,-32768,   493,-32768,   500,
     850,-32768,   790,-32768,-32768,   508,-32768,   502,   793,-32768,
     703,-32768,-32768,   442,-32768,   504,-32768,-32768,-32768,-32768,
      90,-32768,    91,-32768,-32768,-32768,-32768,    79,-32768,    78,
  -32768,-32768,-32768,-32768,-32768,-32768,   405,-32768,   406,-32768,
  -32768,-32768,   284,-32768,  -549,  -632,-32768,  -659,-32768,-32768,
  -32768,-32768,  -570,-32768,-32768,-32768,   349,-32768,-32768,-32768,
  -32768,   345,-32768,   338,-32768,-32768,-32768,   238,-32768,  -650,
    -643,-32768,  -704,-32768,-32768,-32768,-32768,  -606,-32768,-32768,
  -32768,   301,-32768,-32768,-32768,-32768,-32768,  -496,  -111,-32768,
     -59,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -478,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,   629,-32768,   707,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -137,-32768,
  -32768,-32768,   515,-32768,  -226,-32768,-32768,-32768,   447,-32768,
  -32768,   460,   303,   265,   231,-32768,  -104,  -677
};


#define	YYLAST		944


static const short yytable[] =
{
     187,   549,   310,   732,   312,   186,   228,   217,   230,   620,
     677,   392,   727,   202,   670,   705,   396,    77,   793,   689,
     720,   652,    79,    81,   665,   576,    72,   649,   637,   703,
     773,   706,    45,   218,   520,   156,   157,   158,   159,   650,
     768,   804,   323,   135,   136,    46,   650,   715,   704,   704,
     324,   521,   575,   692,  -283,   816,   608,   586,    78,   654,
     763,   465,   744,    80,    82,    73,   762,    74,   797,   784,
      41,   651,   785,   521,   587,   791,   609,   265,   160,   687,
     671,   263,   690,   264,   757,   266,   397,   789,   790,   232,
     234,   627,   777,   290,   764,   779,   765,   742,   293,   160,
     745,   805,   292,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   160,   508,   653,   160,   655,   783,
     160,   466,   575,   759,   798,   575,   799,   419,   726,   806,
      42,   728,   807,   124,   428,   204,   430,   209,   295,   801,
     817,   152,   185,   575,   150,  -330,   320,   153,   330,   796,
     531,   151,    56,   363,   698,   305,   587,   183,   184,   306,
     331,   707,   370,    57,   372,   782,   179,   180,   181,   182,
    -296,  -296,    43,   575,   359,  -291,   748,   749,   725,   750,
     751,   752,   753,   361,   575,   362,  -338,    83,    84,    85,
      86,   800,    87,    88,    89,    90,    91,   125,    44,   205,
     160,   210,   296,   802,   818,    47,   160,    48,   160,   544,
     545,    63,   546,    49,   547,   548,   387,   578,   579,    92,
      93,   321,   322,   754,   394,   435,   436,   437,   438,   439,
      94,   347,   348,   776,    95,   698,    60,   381,   382,  -343,
    -343,    96,   383,   384,   374,    50,   375,   447,   448,   376,
     388,   389,   575,    62,  -291,    97,    83,    84,    85,    86,
      51,    87,    88,    89,    90,    91,   177,   178,   179,   180,
     181,   182,    52,    83,    84,    85,    86,    53,    87,    88,
      67,    90,    91,   409,   410,   174,   175,   471,    92,    93,
     177,   178,   179,   180,   181,   182,   412,   413,    68,    94,
     451,   452,   -45,    95,   -35,    92,    93,   501,   502,    75,
      96,   504,   505,   515,   516,   830,    94,   562,   502,    76,
      95,   575,   -51,  -338,    97,   563,   505,    96,   842,    83,
      84,    85,    86,    71,    87,    88,   109,    90,    91,   111,
     481,    97,   231,   565,   566,   568,   569,   112,   532,   115,
     482,   483,   606,   566,   116,   542,   607,   569,   629,   630,
     119,    92,    93,   497,   484,   485,   486,   487,   488,   489,
     722,   723,    94,   831,   832,   126,    95,   843,   844,   131,
     137,   138,   148,    96,   149,   140,   142,    83,    84,    85,
      86,   144,    87,    88,    89,    90,    91,    97,   233,    83,
      84,    85,    86,   154,    87,    88,   146,    90,    91,   147,
     155,   160,   188,   189,   190,   192,   193,   582,   196,    92,
      93,   206,   197,   211,   219,   220,   597,   222,   223,   227,
      94,    92,    93,   229,    95,   235,     2,     3,     4,     5,
     236,    96,    94,   238,   237,   261,    95,   262,   268,   481,
     267,   274,   278,    96,   614,    97,     6,   273,   610,   482,
     483,   277,   281,   285,   286,   297,   299,    97,  -351,   301,
     319,   307,   317,   182,   485,   486,   487,   488,   489,   302,
     332,     7,   337,     8,     9,    10,    11,   338,    12,    13,
     340,   339,   343,   350,   645,   638,   349,   355,   366,   368,
     369,   398,   171,   172,   173,   354,   174,   175,   365,   380,
     656,   177,   178,   179,   180,   181,   182,   371,   402,   678,
     373,   377,   674,   379,   401,   681,   414,   408,   415,   688,
    -352,   420,   426,   425,   441,   427,   429,   444,   455,   699,
     431,   693,   450,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   459,   171,   172,   173,   461,   174,   175,   462,
     176,   714,   177,   178,   179,   180,   181,   182,   467,   476,
     313,   478,   498,   494,   733,   479,   499,   729,   314,   736,
     495,   496,   509,   743,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   500,   171,   172,   173,   761,   174,   175,
     512,   176,   503,   177,   178,   179,   180,   181,   182,   517,
     530,   315,   533,   543,   537,   539,   767,   540,   554,   316,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   551,
     171,   172,   173,   555,   174,   175,   556,   176,   558,   177,
     178,   179,   180,   181,   182,   560,   564,   432,   788,   567,
     795,   572,   580,   581,   583,   433,   596,   598,   600,   599,
     602,   604,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   611,   171,   172,   173,  -282,   174,   175,   615,   176,
     813,   177,   178,   179,   180,   181,   182,   622,   624,   631,
     632,   634,   641,   636,   642,   640,   826,   239,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   643,   171,   172,
     173,   646,   174,   175,   648,   176,   575,   177,   178,   179,
     180,   181,   182,   657,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   434,   171,   172,   173,  -329,   174,   175,
     660,   176,   667,   177,   178,   179,   180,   181,   182,   672,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   557,
     171,   172,   173,   675,   174,   175,  -290,   176,   318,   177,
     178,   179,   180,   181,   182,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   682,   171,   172,   173,   684,   174,
     175,   686,   176,   696,   177,   178,   179,   180,   181,   182,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   691,
     171,   172,   173,   695,   174,   175,   697,   700,   702,   177,
     178,   179,   180,   181,   182,   162,   163,   164,   165,   166,
     167,   709,   710,   711,   718,   171,   172,   173,   521,   174,
     175,   721,   730,  -337,   177,   178,   179,   180,   181,   182,
     737,   739,   741,   747,   756,   771,   587,   775,   774,   778,
     780,   781,   803,   810,   811,   815,   819,   820,   812,   821,
     833,   825,   838,   828,   834,   848,   849,   216,    61,   378,
     449,   123,   440,   201,   122,   200,   561,   559,   284,   603,
     605,   518,   584,   506,   473,   570,   507,   130,   406,   571,
     405,   215,   472,   474,   127,   470,   298,   541,   208,   839,
     845,   477,   846,   840,   573,   212,   680,   628,   621,   577,
     625,   666,   308,   367,   708,   735,   669,   480,   553,   746,
       0,     0,     0,     0,   552
};

static const short yycheck[] =
{
     111,   479,   228,   694,   230,   109,   143,    22,   145,   579,
     639,   336,   689,   124,    33,   665,    15,    54,   768,   651,
     679,    17,    54,    54,   630,   521,    54,    16,    22,    16,
     734,    17,    36,   137,    18,    94,    95,    96,    97,    35,
     731,   791,    35,    93,    94,    49,    35,   676,    35,    35,
      43,    35,    98,    22,   100,   805,    22,    18,    95,    22,
     719,    15,   705,    95,    95,    93,    28,    95,   772,     9,
      35,   620,    12,    35,    35,   766,   572,   188,    93,   649,
      99,   185,   652,   187,   713,   189,    85,   764,   765,   148,
     149,   587,   742,   204,   726,   745,   728,   703,   209,    93,
     706,   792,   206,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,    93,   450,   622,    93,   624,   758,
      93,    85,    98,   715,   777,    98,   779,   363,   687,     9,
      35,   690,    12,    21,   370,    21,   372,    21,    21,    21,
      21,    35,    35,    98,    35,   100,   267,    41,    35,    28,
      35,    42,    95,   300,   660,    93,    35,    33,    34,    97,
     274,   667,   309,    29,   311,   757,    88,    89,    90,    91,
      16,    17,    35,    98,   295,   100,    33,    34,   684,    36,
      37,    38,    39,   297,    98,   299,   100,    33,    34,    35,
      36,   783,    38,    39,    40,    41,    42,    85,    35,    85,
      93,    85,    85,    85,    85,    35,    93,    35,    93,    33,
      34,    30,    36,    35,    38,    39,   330,    16,    17,    65,
      66,    93,    94,   711,   338,    43,    44,    45,    46,    47,
      76,    93,    94,   739,    80,   741,    95,    93,    94,    16,
      17,    87,    13,    14,   313,    35,   315,    13,    14,   318,
      93,    94,    98,    95,   100,   101,    33,    34,    35,    36,
      35,    38,    39,    40,    41,    42,    86,    87,    88,    89,
      90,    91,    35,    33,    34,    35,    36,    35,    38,    39,
      95,    41,    42,    93,    94,    81,    82,   408,    65,    66,
      86,    87,    88,    89,    90,    91,    93,    94,    36,    76,
      13,    14,    95,    80,    95,    65,    66,    93,    94,    93,
      87,    93,    94,    13,    14,   821,    76,    93,    94,    93,
      80,    98,    95,   100,   101,    93,    94,    87,   834,    33,
      34,    35,    36,    85,    38,    39,    26,    41,    42,    35,
      35,   101,   102,    93,    94,    93,    94,    35,   462,    85,
      45,    46,    93,    94,     7,   476,    93,    94,    16,    17,
      29,    65,    66,   432,    59,    60,    61,    62,    63,    64,
      93,    94,    76,    93,    94,     7,    80,    93,    94,    30,
      35,    35,   101,    87,   101,    35,    35,    33,    34,    35,
      36,    35,    38,    39,    40,    41,    42,   101,   102,    33,
      34,    35,    36,   101,    38,    39,    35,    41,    42,    35,
     101,    93,    97,    96,    35,    85,     8,   531,    85,    65,
      66,    27,     7,     7,    95,    58,   540,    95,    35,    95,
      76,    65,    66,    95,    80,    35,     3,     4,     5,     6,
      35,    87,    76,   102,    35,    35,    80,    35,    35,    35,
      97,    24,     8,    87,   575,   101,    23,    85,   572,    45,
      46,    85,    10,    85,    35,    96,    35,   101,    95,    85,
      93,    96,   102,    91,    60,    61,    62,    63,    64,    57,
      35,    48,    85,    50,    51,    52,    53,    96,    55,    56,
      11,    85,    10,    35,   615,   609,    85,    35,    96,    33,
      93,    12,    77,    78,    79,    85,    81,    82,    85,    35,
     624,    86,    87,    88,    89,    90,    91,    96,    11,   640,
      96,    93,   636,    85,    85,   646,    85,    97,    35,   650,
      95,    35,    96,    93,    35,    93,    93,    35,    35,   660,
      93,   655,    93,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    85,    77,    78,    79,    85,    81,    82,    25,
      84,   675,    86,    87,    88,    89,    90,    91,    12,    97,
      94,    96,    44,    93,   695,    97,    43,   691,   102,   700,
      96,    96,    35,   704,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    98,    77,    78,    79,   718,    81,    82,
      35,    84,    98,    86,    87,    88,    89,    90,    91,    85,
      36,    94,    35,    93,    85,    85,   730,    96,    35,   102,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    88,
      77,    78,    79,    93,    81,    82,    93,    84,    99,    86,
      87,    88,    89,    90,    91,    99,    98,    94,   762,    98,
     771,    35,   100,    93,    85,   102,    36,    93,    35,    93,
      99,    99,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    35,    77,    78,    79,   100,    81,    82,    35,    84,
     801,    86,    87,    88,    89,    90,    91,    35,    35,   100,
      93,    98,    20,    35,    94,    97,   817,   102,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    99,    77,    78,
      79,    21,    81,    82,   100,    84,    98,    86,    87,    88,
      89,    90,    91,    35,    67,    68,    69,    70,    71,    72,
      73,    74,    75,   102,    77,    78,    79,   100,    81,    82,
      35,    84,    35,    86,    87,    88,    89,    90,    91,    53,
      67,    68,    69,    70,    71,    72,    73,    74,    75,   102,
      77,    78,    79,    35,    81,    82,   100,    84,    85,    86,
      87,    88,    89,    90,    91,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    12,    77,    78,    79,    35,    81,
      82,    35,    84,    20,    86,    87,    88,    89,    90,    91,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    35,
      77,    78,    79,    97,    81,    82,    94,    21,   100,    86,
      87,    88,    89,    90,    91,    67,    68,    69,    70,    71,
      72,    99,   101,    97,    19,    77,    78,    79,    35,    81,
      82,    36,    35,   100,    86,    87,    88,    89,    90,    91,
      12,    35,    35,    35,    93,    19,    35,    93,    36,    93,
      93,   102,    85,    93,    93,    36,    85,    85,    93,    35,
      85,    93,    93,    36,    35,     0,     0,   136,    25,   322,
     389,    62,   382,   123,    61,   122,   505,   502,   201,   566,
     569,   457,   535,   447,   411,   515,   448,    65,   348,   516,
     346,   133,   410,   413,    64,   407,   213,   475,   128,   829,
     841,   417,   844,   832,   519,   132,   642,   589,   579,   523,
     585,   630,   225,   304,   669,   697,   633,   422,   491,   708,
      -1,    -1,    -1,    -1,   484
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

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

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Relocate the TYPE STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Type, Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	yymemcpy ((char *) yyptr, (char *) (Stack),			\
		  yysize * (YYSIZE_T) sizeof (Type));			\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (Type) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


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
#define YYABORT 	goto yyabortlab
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
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


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
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
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

#if ! defined (yyoverflow) && ! defined (yymemcpy)
# if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#  define yymemcpy __builtin_memcpy
# else				/* not GNU C or C++ */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
#  if defined (__STDC__) || defined (__cplusplus)
yymemcpy (char *yyto, const char *yyfrom, YYSIZE_T yycount)
#  else
yymemcpy (yyto, yyfrom, yycount)
     char *yyto;
     const char *yyfrom;
     YYSIZE_T yycount;
#  endif
{
  register const char *yyf = yyfrom;
  register char *yyt = yyto;
  register YYSIZE_T yyi = yycount;

  while (yyi-- != 0)
    *yyt++ = *yyf++;
}
# endif
#endif

#ifdef YYERROR_VERBOSE

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
#endif

#line 319 "/usr/share/bison/bison.simple"


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

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

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

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
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
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
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

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
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
	YYSTACK_RELOCATE (short, yyss);
	YYSTACK_RELOCATE (YYSTYPE, yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (YYLTYPE, yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

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
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
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
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 21:
#line 262 "ned.y"
{
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                ;
    break;}
case 22:
#line 267 "ned.y"
{
                  /* no setTrailingComment(ps.imports,@3); comment already stored with last filename */
                ;
    break;}
case 25:
#line 279 "ned.y"
{
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                ;
    break;}
case 27:
#line 295 "ned.y"
{
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 30:
#line 309 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 31:
#line 315 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 32:
#line 324 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 33:
#line 328 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 34:
#line 340 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 35:
#line 347 "ned.y"
{
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 38:
#line 361 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 39:
#line 367 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 41:
#line 387 "ned.y"
{
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 42:
#line 396 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 43:
#line 400 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 44:
#line 414 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 45:
#line 421 "ned.y"
{
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 47:
#line 445 "ned.y"
{
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 48:
#line 454 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 49:
#line 458 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 50:
#line 475 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 51:
#line 482 "ned.y"
{
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 54:
#line 500 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 55:
#line 505 "ned.y"
{
                ;
    break;}
case 60:
#line 521 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 63:
#line 538 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 64:
#line 543 "ned.y"
{
                ;
    break;}
case 69:
#line 559 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 72:
#line 576 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 75:
#line 591 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 78:
#line 606 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 79:
#line 611 "ned.y"
{
                ;
    break;}
case 82:
#line 622 "ned.y"
{
                  setComments(ps.param,yylsp[-1]);
                ;
    break;}
case 83:
#line 626 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 86:
#line 641 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 87:
#line 646 "ned.y"
{
                ;
    break;}
case 90:
#line 657 "ned.y"
{
                  setComments(ps.param,yylsp[-1]);
                ;
    break;}
case 91:
#line 661 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 93:
#line 676 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                ;
    break;}
case 94:
#line 680 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                ;
    break;}
case 95:
#line 684 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                ;
    break;}
case 96:
#line 688 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                ;
    break;}
case 97:
#line 692 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 98:
#line 696 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 99:
#line 700 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                ;
    break;}
case 100:
#line 704 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                ;
    break;}
case 101:
#line 708 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                ;
    break;}
case 104:
#line 723 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 105:
#line 728 "ned.y"
{
                ;
    break;}
case 114:
#line 751 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 115:
#line 756 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 118:
#line 769 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 119:
#line 774 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 122:
#line 790 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 123:
#line 795 "ned.y"
{
                ;
    break;}
case 132:
#line 818 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 133:
#line 823 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 136:
#line 836 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 137:
#line 841 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 140:
#line 857 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 141:
#line 862 "ned.y"
{
                ;
    break;}
case 146:
#line 878 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 147:
#line 884 "ned.y"
{
                ;
    break;}
case 148:
#line 887 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 149:
#line 894 "ned.y"
{
                ;
    break;}
case 150:
#line 897 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-3], yylsp[-1]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 151:
#line 903 "ned.y"
{
                ;
    break;}
case 152:
#line 906 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-4], yylsp[-1]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 153:
#line 913 "ned.y"
{
                ;
    break;}
case 157:
#line 933 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 158:
#line 938 "ned.y"
{
                ;
    break;}
case 163:
#line 954 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 164:
#line 960 "ned.y"
{
                ;
    break;}
case 165:
#line 963 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 166:
#line 970 "ned.y"
{
                ;
    break;}
case 167:
#line 973 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-3], yylsp[-1]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 168:
#line 979 "ned.y"
{
                ;
    break;}
case 169:
#line 982 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-4], yylsp[-1]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 170:
#line 989 "ned.y"
{
                ;
    break;}
case 176:
#line 1014 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 177:
#line 1019 "ned.y"
{
                ;
    break;}
case 178:
#line 1022 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 179:
#line 1028 "ned.y"
{
                ;
    break;}
case 184:
#line 1044 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 189:
#line 1065 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 190:
#line 1070 "ned.y"
{
                ;
    break;}
case 191:
#line 1073 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 192:
#line 1079 "ned.y"
{
                ;
    break;}
case 197:
#line 1095 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 202:
#line 1116 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 203:
#line 1121 "ned.y"
{
                ;
    break;}
case 204:
#line 1124 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 205:
#line 1130 "ned.y"
{
                ;
    break;}
case 210:
#line 1147 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                ;
    break;}
case 215:
#line 1169 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 216:
#line 1174 "ned.y"
{
                ;
    break;}
case 217:
#line 1177 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 218:
#line 1183 "ned.y"
{
                ;
    break;}
case 223:
#line 1200 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-3]);
                  addExpression(ps.substparam, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparam,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 226:
#line 1217 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 227:
#line 1222 "ned.y"
{
                ;
    break;}
case 228:
#line 1225 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 229:
#line 1231 "ned.y"
{
                ;
    break;}
case 234:
#line 1247 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 235:
#line 1254 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 238:
#line 1270 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 239:
#line 1275 "ned.y"
{
                ;
    break;}
case 240:
#line 1278 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 241:
#line 1284 "ned.y"
{
                ;
    break;}
case 246:
#line 1300 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 247:
#line 1307 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 248:
#line 1318 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 250:
#line 1329 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 254:
#line 1345 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "false" );
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 255:
#line 1351 "ned.y"
{
                ;
    break;}
case 256:
#line 1354 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "yes" );
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 257:
#line 1360 "ned.y"
{
                ;
    break;}
case 264:
#line 1381 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                ;
    break;}
case 265:
#line 1385 "ned.y"
{
                  ps.inLoop=1;
                ;
    break;}
case 266:
#line 1389 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-7],yylsp[-4]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 269:
#line 1403 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 270:
#line 1413 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 272:
#line 1421 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 276:
#line 1434 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 277:
#line 1439 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 278:
#line 1444 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 279:
#line 1450 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 282:
#line 1464 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 283:
#line 1470 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 284:
#line 1478 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 285:
#line 1483 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 286:
#line 1490 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 287:
#line 1497 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 290:
#line 1511 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 291:
#line 1516 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 292:
#line 1523 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 293:
#line 1528 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 294:
#line 1535 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 295:
#line 1540 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 296:
#line 1548 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 297:
#line 1553 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 298:
#line 1558 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 301:
#line 1574 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "false" );
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 302:
#line 1580 "ned.y"
{
                ;
    break;}
case 303:
#line 1583 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "yes" );
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 304:
#line 1589 "ned.y"
{
                ;
    break;}
case 311:
#line 1610 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                ;
    break;}
case 312:
#line 1614 "ned.y"
{
                  ps.inLoop=1;
                ;
    break;}
case 313:
#line 1618 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-7],yylsp[-4]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 316:
#line 1632 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 317:
#line 1642 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 319:
#line 1650 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 323:
#line 1663 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 324:
#line 1668 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 325:
#line 1673 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 326:
#line 1679 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 329:
#line 1693 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 330:
#line 1699 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 331:
#line 1707 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 332:
#line 1712 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 333:
#line 1719 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 334:
#line 1726 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 337:
#line 1740 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 338:
#line 1745 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 339:
#line 1752 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 340:
#line 1757 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 341:
#line 1764 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 342:
#line 1769 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 343:
#line 1777 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 344:
#line 1782 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 345:
#line 1787 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 347:
#line 1805 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1],NULLPOS);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 348:
#line 1811 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-5],yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-6],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 349:
#line 1820 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 350:
#line 1834 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 351:
#line 1842 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1],NULLPOS);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 352:
#line 1848 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-5],yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-6],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 353:
#line 1861 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 354:
#line 1867 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 355:
#line 1871 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 356:
#line 1882 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 357:
#line 1884 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 358:
#line 1886 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 359:
#line 1888 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 361:
#line 1894 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 362:
#line 1897 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 363:
#line 1899 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 364:
#line 1901 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 365:
#line 1903 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 366:
#line 1905 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 367:
#line 1907 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 368:
#line 1911 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); ;
    break;}
case 369:
#line 1914 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 370:
#line 1916 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 371:
#line 1918 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 372:
#line 1920 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 373:
#line 1922 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 374:
#line 1924 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 375:
#line 1927 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 376:
#line 1929 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 377:
#line 1931 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 378:
#line 1935 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 379:
#line 1938 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 380:
#line 1940 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 381:
#line 1942 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 382:
#line 1946 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 383:
#line 1948 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 384:
#line 1950 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 385:
#line 1952 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 386:
#line 1955 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 387:
#line 1957 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 388:
#line 1959 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 389:
#line 1961 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 395:
#line 1974 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 396:
#line 1979 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                ;
    break;}
case 397:
#line 1984 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 398:
#line 1990 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 399:
#line 1996 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 400:
#line 2004 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 401:
#line 2009 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 402:
#line 2011 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 404:
#line 2020 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 405:
#line 2022 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 406:
#line 2024 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 407:
#line 2029 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 408:
#line 2031 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 409:
#line 2033 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 414:
#line 2050 "ned.y"
{
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                ;
    break;}
case 415:
#line 2056 "ned.y"
{
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                ;
    break;}
case 416:
#line 2065 "ned.y"
{
                  ps.cppstruct = (CppStructNode *)createNodeWithTag(NED_CPP_STRUCT, ps.nedfile );
                  ps.cppstruct->setName(toString(yylsp[-1]));
                  setComments(ps.cppstruct,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 417:
#line 2074 "ned.y"
{
                  ps.cppcobject = (CppCobjectNode *)createNodeWithTag(NED_CPP_COBJECT, ps.nedfile );
                  ps.cppcobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppcobject,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 418:
#line 2083 "ned.y"
{
                  ps.cppnoncobject = (CppNoncobjectNode *)createNodeWithTag(NED_CPP_NONCOBJECT, ps.nedfile );
                  ps.cppnoncobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppnoncobject,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 419:
#line 2092 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 420:
#line 2099 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 421:
#line 2103 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 422:
#line 2111 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 427:
#line 2128 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                ;
    break;}
case 428:
#line 2133 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                ;
    break;}
case 429:
#line 2142 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 430:
#line 2148 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 431:
#line 2152 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 432:
#line 2159 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 433:
#line 2166 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 434:
#line 2172 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 435:
#line 2176 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 436:
#line 2183 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 437:
#line 2190 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 438:
#line 2196 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 439:
#line 2200 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 440:
#line 2207 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 441:
#line 2214 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 448:
#line 2234 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 455:
#line 2253 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 462:
#line 2273 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                ;
    break;}
case 463:
#line 2279 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 464:
#line 2283 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsVirtual(true);
                ;
    break;}
case 465:
#line 2290 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 475:
#line 2309 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 476:
#line 2314 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 478:
#line 2322 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 480:
#line 2330 "ned.y"
{
                  ps.field->setDefaultValue(toString(yylsp[0]));
                ;
    break;}
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

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
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
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
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

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

/*--------------.
| yyerrhandle.  |
`--------------*/
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

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 2350 "ned.y"


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

