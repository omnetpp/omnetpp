/* A Bison parser, made from ned.y
   by GNU bison 1.34.  */

#define YYBISON 1  /* Identify Bison output.  */

#define YYLSP_NEEDED 1

# define	INCLUDE	257
# define	SIMPLE	258
# define	CHANNEL	259
# define	DELAY	260
# define	ERROR	261
# define	DATARATE	262
# define	MODULE	263
# define	PARAMETERS	264
# define	GATES	265
# define	GATESIZES	266
# define	SUBMODULES	267
# define	CONNECTIONS	268
# define	DISPLAY	269
# define	IN	270
# define	OUT	271
# define	NOCHECK	272
# define	LEFT_ARROW	273
# define	RIGHT_ARROW	274
# define	FOR	275
# define	TO	276
# define	DO	277
# define	IF	278
# define	LIKE	279
# define	NETWORK	280
# define	ENDSIMPLE	281
# define	ENDMODULE	282
# define	ENDCHANNEL	283
# define	ENDNETWORK	284
# define	ENDFOR	285
# define	MACHINES	286
# define	ON	287
# define	IO_INTERFACES	288
# define	IFPAIR	289
# define	INTCONSTANT	290
# define	REALCONSTANT	291
# define	NAME	292
# define	STRINGCONSTANT	293
# define	CHARCONSTANT	294
# define	_TRUE	295
# define	_FALSE	296
# define	INPUT	297
# define	REF	298
# define	ANCESTOR	299
# define	CONSTDECL	300
# define	NUMERICTYPE	301
# define	STRINGTYPE	302
# define	BOOLTYPE	303
# define	ANYTYPE	304
# define	CPPINCLUDE	305
# define	SYSINCFILENAME	306
# define	STRUCT	307
# define	COBJECT	308
# define	NONCOBJECT	309
# define	ENUM	310
# define	EXTENDS	311
# define	MESSAGE	312
# define	CLASS	313
# define	FIELDS	314
# define	PROPERTIES	315
# define	VIRTUAL	316
# define	CHARTYPE	317
# define	SHORTTYPE	318
# define	INTTYPE	319
# define	LONGTYPE	320
# define	DOUBLETYPE	321
# define	SIZEOF	322
# define	SUBMODINDEX	323
# define	EQ	324
# define	NE	325
# define	GT	326
# define	GE	327
# define	LS	328
# define	LE	329
# define	AND	330
# define	OR	331
# define	XOR	332
# define	NOT	333
# define	BIN_AND	334
# define	BIN_OR	335
# define	BIN_XOR	336
# define	BIN_COMPL	337
# define	SHIFT_LEFT	338
# define	SHIFT_RIGHT	339
# define	INVALID_CHAR	340
# define	UMIN	341

#line 76 "ned.y"


/*
 * Note:
 * This file contains 2 shift-reduce conflicts around 'timeconstant'.
 * 4 more at opt_semicolon's after module/submodule types.
 * Plus 3 more to track down.
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

ChannelAttrNode *addChanAttr(NEDElement *channel, char *attrname);
ParamNode *addParameter(NEDElement *params, YYLTYPE namepos, int type);
GateNode *addGate(NEDElement *gates, YYLTYPE namepos, int is_in, int is_vector );
SubmoduleNode *addSubmodule(NEDElement *submods, YYLTYPE namepos, YYLTYPE typepos,YYLTYPE likepos);
GatesizeNode *addGateSize(NEDElement *gatesizes, YYLTYPE namepos);
SubstparamNode *addSubstparam(NEDElement *substparams, YYLTYPE namepos);
SubstmachineNode *addSubstmachine(NEDElement *substmachines, YYLTYPE namepos);
ConnAttrNode *addConnAttr(NEDElement *conn, char *attrname);
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



#define	YYFINAL		686
#define	YYFLAG		-32768
#define	YYNTBASE	106

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 341 ? yytranslate[x] : 269)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    93,     2,     2,
     104,   105,    91,    89,    96,    90,   103,    92,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    88,    97,
       2,   102,     2,    87,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   100,     2,   101,    94,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    98,     2,    99,     2,     2,     2,     2,
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
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    95
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     3,     6,     8,    10,    12,    14,    16,
      18,    20,    22,    24,    26,    28,    30,    32,    34,    35,
      39,    43,    46,    48,    51,    55,    59,    63,    68,    73,
      78,    83,    88,    93,    99,   105,   111,   117,   123,   129,
     133,   138,   143,   148,   154,   160,   166,   172,   178,   184,
     191,   198,   205,   212,   219,   226,   229,   233,   236,   239,
     243,   247,   251,   257,   265,   269,   273,   276,   285,   296,
     300,   304,   307,   309,   310,   312,   313,   315,   316,   318,
     319,   321,   322,   324,   325,   326,   331,   333,   334,   338,
     341,   343,   348,   349,   354,   356,   357,   358,   363,   364,
     369,   372,   374,   378,   381,   385,   390,   395,   399,   403,
     407,   408,   413,   415,   416,   420,   423,   427,   430,   434,
     437,   441,   443,   447,   450,   454,   456,   457,   462,   464,
     465,   468,   470,   471,   479,   480,   489,   490,   500,   501,
     512,   516,   518,   519,   522,   524,   525,   530,   531,   538,
     540,   541,   545,   548,   550,   552,   553,   556,   558,   559,
     564,   565,   572,   574,   575,   579,   582,   586,   589,   590,
     591,   596,   597,   604,   606,   607,   611,   614,   617,   619,
     624,   625,   626,   632,   633,   638,   640,   641,   645,   648,
     650,   652,   653,   654,   662,   666,   668,   674,   677,   678,
     681,   682,   686,   689,   695,   703,   709,   717,   721,   723,
     726,   728,   731,   733,   736,   738,   742,   744,   747,   749,
     752,   754,   757,   759,   760,   762,   764,   766,   768,   771,
     774,   777,   780,   783,   786,   790,   794,   798,   802,   806,
     810,   812,   815,   818,   821,   826,   833,   839,   847,   850,
     854,   856,   858,   865,   870,   874,   876,   878,   882,   886,
     890,   894,   898,   902,   906,   909,   913,   917,   921,   925,
     929,   933,   937,   941,   945,   948,   952,   956,   960,   963,
     967,   971,   977,   981,   986,   993,  1002,  1004,  1006,  1008,
    1010,  1012,  1014,  1017,  1021,  1025,  1028,  1030,  1032,  1034,
    1036,  1038,  1042,  1047,  1049,  1051,  1053,  1057,  1061,  1064,
    1067,  1070,  1073,  1077,  1081,  1085,  1086,  1094,  1095,  1105,
    1107,  1108,  1111,  1113,  1116,  1121,  1122,  1131,  1132,  1143,
    1144,  1153,  1154,  1165,  1166,  1175,  1176,  1187,  1188,  1193,
    1194,  1196,  1197,  1200,  1202,  1207,  1209,  1211,  1213,  1215,
    1217,  1219,  1220,  1225,  1226,  1228,  1229,  1232,  1234,  1235,
    1243,  1244,  1253,  1255,  1258,  1260,  1262,  1264,  1266,  1268,
    1270,  1272,  1276,  1279,  1280,  1285,  1286,  1289,  1290,  1292,
    1294,  1296,  1298,  1300,  1302,  1304,  1306,  1307,  1309,  1311
};
static const short yyrhs[] =
{
     107,     0,     0,   108,   107,     0,   108,     0,   109,     0,
     113,     0,   120,     0,   123,     0,   215,     0,   230,     0,
     231,     0,   232,     0,   233,     0,   234,     0,   240,     0,
     243,     0,   246,     0,     0,     3,   110,   111,     0,   112,
      96,   111,     0,   112,    97,     0,    39,     0,   114,   115,
       0,   114,   118,   115,     0,   114,   117,   115,     0,   114,
     119,   115,     0,   114,   118,   117,   115,     0,   114,   118,
     119,   115,     0,   114,   117,   119,   115,     0,   114,   117,
     118,   115,     0,   114,   119,   118,   115,     0,   114,   119,
     117,   115,     0,   114,   118,   117,   119,   115,     0,   114,
     118,   119,   117,   115,     0,   114,   117,   118,   119,   115,
       0,   114,   117,   119,   118,   115,     0,   114,   119,   118,
     117,   115,     0,   114,   119,   117,   118,   115,     0,   114,
      98,   116,     0,   114,    98,   118,   116,     0,   114,    98,
     117,   116,     0,   114,    98,   119,   116,     0,   114,    98,
     118,   117,   116,     0,   114,    98,   118,   119,   116,     0,
     114,    98,   117,   119,   116,     0,   114,    98,   117,   118,
     116,     0,   114,    98,   119,   118,   116,     0,   114,    98,
     119,   117,   116,     0,   114,    98,   118,   117,   119,   116,
       0,   114,    98,   118,   119,   117,   116,     0,   114,    98,
     117,   118,   119,   116,     0,   114,    98,   117,   119,   118,
     116,     0,   114,    98,   119,   118,   117,   116,     0,   114,
      98,   119,   117,   118,   116,     0,     5,    38,     0,    29,
      38,   267,     0,    29,   267,     0,    99,   267,     0,     7,
     219,   267,     0,     6,   219,   267,     0,     8,   219,   267,
       0,   121,   126,   128,   129,   122,     0,   121,    98,   126,
     128,   129,    99,   267,     0,     4,    38,   267,     0,    27,
      38,   267,     0,    27,   267,     0,   124,   126,   128,   129,
     130,   131,   127,   125,     0,   124,    98,   126,   128,   129,
     130,   131,   127,    99,   267,     0,     9,    38,   267,     0,
      28,    38,   267,     0,    28,   267,     0,   132,     0,     0,
     137,     0,     0,   138,     0,     0,   145,     0,     0,   153,
       0,     0,   187,     0,     0,     0,    32,    88,   133,   134,
       0,   135,     0,     0,   136,    96,   135,     0,   136,    97,
       0,    38,     0,    15,    88,    39,    97,     0,     0,    10,
      88,   139,   140,     0,   141,     0,     0,     0,   144,    96,
     142,   141,     0,     0,   144,    97,   143,   141,     0,   144,
      97,     0,    38,     0,    38,    88,    47,     0,    46,    38,
       0,    38,    88,    46,     0,    38,    88,    46,    47,     0,
      38,    88,    47,    46,     0,    38,    88,    48,     0,    38,
      88,    49,     0,    38,    88,    50,     0,     0,    11,    88,
     146,   147,     0,   148,     0,     0,    16,   149,   148,     0,
      16,   149,     0,    17,   151,   148,     0,    17,   151,     0,
     150,    96,   149,     0,   150,    97,     0,    38,   100,   101,
       0,    38,     0,   152,    96,   151,     0,   152,    97,     0,
      38,   100,   101,     0,    38,     0,     0,    13,    88,   154,
     155,     0,   156,     0,     0,   157,   156,     0,   157,     0,
       0,    38,    88,    38,   267,   158,   163,   162,     0,     0,
      38,    88,    38,   218,   267,   159,   163,   162,     0,     0,
      38,    88,    38,    25,    38,   267,   160,   163,   162,     0,
       0,    38,    88,    38,   218,    25,    38,   267,   161,   163,
     162,     0,   171,   179,   186,     0,   164,     0,     0,   165,
     164,     0,   165,     0,     0,    33,    88,   166,   168,     0,
       0,    33,    24,   219,    88,   167,   168,     0,   169,     0,
       0,   170,    96,   169,     0,   170,    97,     0,    38,     0,
     172,     0,     0,   173,   172,     0,   173,     0,     0,    10,
      88,   174,   176,     0,     0,    10,    24,   219,    88,   175,
     176,     0,   177,     0,     0,   178,    96,   177,     0,   178,
      97,     0,    38,   102,   219,     0,   180,   179,     0,     0,
       0,    12,    88,   181,   183,     0,     0,    12,    24,   219,
      88,   182,   183,     0,   184,     0,     0,   185,    96,   184,
       0,   185,    97,     0,    38,   218,     0,    38,     0,    15,
      88,    39,    97,     0,     0,     0,    14,    18,    88,   188,
     190,     0,     0,    14,    88,   189,   190,     0,   191,     0,
       0,   192,   268,   191,     0,   192,    97,     0,   193,     0,
     201,     0,     0,     0,    21,   194,   196,    23,   195,   200,
      31,     0,   197,    96,   196,     0,   197,     0,    38,   102,
     219,    22,   219,     0,    24,   219,     0,     0,    15,    39,
       0,     0,   201,   268,   200,     0,   201,    97,     0,   202,
      20,   206,   198,   199,     0,   202,    20,   210,    20,   206,
     198,   199,     0,   202,    19,   206,   198,   199,     0,   202,
      19,   210,    19,   206,   198,   199,     0,   203,   103,   204,
       0,   205,     0,    38,   218,     0,    38,     0,    38,   218,
       0,    38,     0,    38,   218,     0,    38,     0,   207,   103,
     208,     0,   209,     0,    38,   218,     0,    38,     0,    38,
     218,     0,    38,     0,    38,   218,     0,    38,     0,     0,
     211,     0,   212,     0,   213,     0,   214,     0,   212,   213,
       0,   212,   214,     0,   213,   214,     0,   213,   212,     0,
     214,   212,     0,   214,   213,     0,   212,   213,   214,     0,
     212,   214,   213,     0,   213,   212,   214,     0,   213,   214,
     212,     0,   214,   212,   213,     0,   214,   213,   212,     0,
     219,     0,     6,   219,     0,     7,   219,     0,     8,   219,
       0,   216,   163,   171,   217,     0,   216,    98,   163,   171,
      99,   267,     0,    26,    38,    88,    38,   267,     0,    26,
      38,    88,    38,    25,    38,   267,     0,    30,   267,     0,
     100,   219,   101,     0,   221,     0,   220,     0,    43,   104,
     221,    96,   221,   105,     0,    43,   104,   221,   105,     0,
      43,   104,   105,     0,    43,     0,   222,     0,   104,   221,
     105,     0,   221,    89,   221,     0,   221,    90,   221,     0,
     221,    91,   221,     0,   221,    92,   221,     0,   221,    93,
     221,     0,   221,    94,   221,     0,    90,   221,     0,   221,
      70,   221,     0,   221,    71,   221,     0,   221,    72,   221,
       0,   221,    73,   221,     0,   221,    74,   221,     0,   221,
      75,   221,     0,   221,    76,   221,     0,   221,    77,   221,
       0,   221,    78,   221,     0,    79,   221,     0,   221,    80,
     221,     0,   221,    81,   221,     0,   221,    82,   221,     0,
      83,   221,     0,   221,    84,   221,     0,   221,    85,   221,
       0,   221,    87,   221,    88,   221,     0,    38,   104,   105,
       0,    38,   104,   221,   105,     0,    38,   104,   221,    96,
     221,   105,     0,    38,   104,   221,    96,   221,    96,   221,
     105,     0,   223,     0,   224,     0,   225,     0,   226,     0,
     227,     0,    38,     0,    44,    38,     0,    44,    45,    38,
       0,    45,    44,    38,     0,    45,    38,     0,    39,     0,
      41,     0,    42,     0,   228,     0,    69,     0,    69,   104,
     105,     0,    68,   104,    38,   105,     0,    36,     0,    37,
       0,   229,     0,    36,    38,   229,     0,    37,    38,   229,
       0,    36,    38,     0,    37,    38,     0,    51,    39,     0,
      51,    52,     0,    53,    38,    97,     0,    54,    38,    97,
       0,    55,    38,    97,     0,     0,    56,    38,    98,   235,
     237,    99,    97,     0,     0,    56,    38,    57,    38,    98,
     236,   237,    99,    97,     0,   238,     0,     0,   239,   238,
       0,   239,     0,    38,    97,     0,    38,   102,    36,    97,
       0,     0,    58,    38,    98,   241,   249,   255,    99,    97,
       0,     0,    58,    38,    57,    38,    98,   242,   249,   255,
      99,    97,     0,     0,    59,    38,    98,   244,   249,   255,
      99,    97,     0,     0,    59,    38,    57,    38,    98,   245,
     249,   255,    99,    97,     0,     0,    53,    38,    98,   247,
     249,   255,    99,    97,     0,     0,    53,    38,    57,    38,
      98,   248,   249,   255,    99,    97,     0,     0,    61,    88,
     250,   251,     0,     0,   252,     0,     0,   253,   252,     0,
     253,     0,    38,   102,   254,    97,     0,    39,     0,    36,
       0,    37,     0,   229,     0,    41,     0,    42,     0,     0,
      60,    88,   256,   257,     0,     0,   258,     0,     0,   259,
     258,     0,   259,     0,     0,   262,    38,   260,   263,   264,
     265,    97,     0,     0,    62,   262,    38,   261,   263,   264,
     265,    97,     0,    38,     0,    38,    91,     0,    63,     0,
      64,     0,    65,     0,    66,     0,    67,     0,    48,     0,
      49,     0,   100,    36,   101,     0,   100,   101,     0,     0,
      56,   104,    38,   105,     0,     0,   102,   266,     0,     0,
      39,     0,    40,     0,    36,     0,    37,     0,   229,     0,
      41,     0,    42,     0,    97,     0,     0,    96,     0,    97,
       0,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   218,   219,   223,   224,   228,   229,   230,   231,   232,
     234,   235,   236,   237,   238,   239,   240,   241,   245,   245,
     257,   258,   262,   271,   272,   273,   274,   275,   276,   277,
     278,   279,   280,   281,   282,   283,   284,   285,   286,   288,
     289,   290,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   307,   317,   321,   329,   336,
     345,   354,   363,   368,   379,   388,   392,   399,   407,   421,
     430,   434,   441,   442,   445,   445,   446,   446,   447,   447,
     448,   448,   449,   449,   452,   452,   463,   464,   468,   469,
     473,   482,   489,   489,   500,   501,   505,   505,   510,   510,
     515,   522,   526,   530,   534,   538,   542,   546,   550,   554,
     561,   561,   572,   573,   577,   578,   579,   580,   584,   585,
     589,   594,   602,   603,   607,   612,   620,   620,   631,   632,
     636,   637,   641,   641,   650,   650,   660,   660,   669,   669,
     682,   688,   689,   693,   694,   698,   698,   706,   706,   718,
     719,   723,   724,   728,   736,   737,   741,   742,   746,   746,
     754,   754,   767,   768,   772,   776,   780,   789,   790,   794,
     794,   802,   802,   814,   815,   819,   820,   824,   831,   839,
     843,   847,   847,   856,   856,   868,   869,   873,   874,   878,
     879,   883,   883,   883,   900,   901,   905,   915,   919,   923,
     927,   931,   932,   936,   941,   946,   952,   961,   962,   966,
     972,   980,   985,   992,   999,  1008,  1009,  1013,  1018,  1025,
    1030,  1037,  1042,  1050,  1051,  1052,  1053,  1054,  1056,  1057,
    1058,  1059,  1060,  1061,  1063,  1064,  1065,  1066,  1067,  1068,
    1072,  1080,  1088,  1096,  1104,  1108,  1119,  1125,  1134,  1142,
    1147,  1152,  1163,  1165,  1167,  1169,  1174,  1175,  1178,  1180,
    1182,  1184,  1186,  1188,  1191,  1195,  1197,  1199,  1201,  1203,
    1205,  1208,  1210,  1212,  1215,  1219,  1221,  1223,  1226,  1229,
    1231,  1233,  1236,  1238,  1240,  1242,  1247,  1248,  1249,  1250,
    1251,  1255,  1260,  1265,  1271,  1277,  1285,  1290,  1292,  1297,
    1301,  1303,  1305,  1310,  1312,  1314,  1319,  1320,  1321,  1322,
    1330,  1336,  1345,  1354,  1363,  1372,  1372,  1383,  1383,  1398,
    1399,  1403,  1404,  1408,  1413,  1422,  1422,  1432,  1432,  1446,
    1446,  1456,  1456,  1470,  1470,  1480,  1480,  1494,  1494,  1500,
    1504,  1505,  1509,  1510,  1514,  1524,  1525,  1526,  1527,  1528,
    1529,  1533,  1533,  1539,  1543,  1544,  1548,  1549,  1553,  1553,
    1563,  1563,  1577,  1578,  1579,  1580,  1581,  1582,  1583,  1584,
    1585,  1589,  1594,  1598,  1602,  1606,  1610,  1614,  1618,  1619,
    1620,  1621,  1622,  1623,  1624,  1627,  1627,  1628,  1628,  1628
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "INCLUDE", "SIMPLE", "CHANNEL", "DELAY", 
  "ERROR", "DATARATE", "MODULE", "PARAMETERS", "GATES", "GATESIZES", 
  "SUBMODULES", "CONNECTIONS", "DISPLAY", "IN", "OUT", "NOCHECK", 
  "LEFT_ARROW", "RIGHT_ARROW", "FOR", "TO", "DO", "IF", "LIKE", "NETWORK", 
  "ENDSIMPLE", "ENDMODULE", "ENDCHANNEL", "ENDNETWORK", "ENDFOR", 
  "MACHINES", "ON", "IO_INTERFACES", "IFPAIR", "INTCONSTANT", 
  "REALCONSTANT", "NAME", "STRINGCONSTANT", "CHARCONSTANT", "_TRUE", 
  "_FALSE", "INPUT", "REF", "ANCESTOR", "CONSTDECL", "NUMERICTYPE", 
  "STRINGTYPE", "BOOLTYPE", "ANYTYPE", "CPPINCLUDE", "SYSINCFILENAME", 
  "STRUCT", "COBJECT", "NONCOBJECT", "ENUM", "EXTENDS", "MESSAGE", 
  "CLASS", "FIELDS", "PROPERTIES", "VIRTUAL", "CHARTYPE", "SHORTTYPE", 
  "INTTYPE", "LONGTYPE", "DOUBLETYPE", "SIZEOF", "SUBMODINDEX", "EQ", 
  "NE", "GT", "GE", "LS", "LE", "AND", "OR", "XOR", "NOT", "BIN_AND", 
  "BIN_OR", "BIN_XOR", "BIN_COMPL", "SHIFT_LEFT", "SHIFT_RIGHT", 
  "INVALID_CHAR", "'?'", "':'", "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", 
  "UMIN", "','", "';'", "'{'", "'}'", "'['", "']'", "'='", "'.'", "'('", 
  "')'", "networkdescription", "somedefinitions", "definition", "import", 
  "@1", "filenames", "filename", "channeldefinition", "channelheader", 
  "endchannel", "braceendchannel", "cherror", "chdelay", "chdatarate", 
  "simpledefinition", "simpleheader", "endsimple", "moduledefinition", 
  "moduleheader", "endmodule", "opt_machineblock", "opt_displayblock", 
  "opt_paramblock", "opt_gateblock", "opt_submodblock", "opt_connblock", 
  "machineblock", "@2", "opt_machinelist", "machinelist", "machine", 
  "displayblock", "paramblock", "@3", "opt_parameters", "parameters", 
  "@4", "@5", "parameter", "gateblock", "@6", "opt_gates", "gates", 
  "gatesI", "gateI", "gatesO", "gateO", "submodblock", "@7", 
  "opt_submodules", "submodules", "submodule", "@8", "@9", "@10", "@11", 
  "submodule_body", "opt_on_blocks", "on_blocks", "on_block", "@12", 
  "@13", "opt_on_list", "on_list", "on_mach", "opt_substparamblocks", 
  "substparamblocks", "substparamblock", "@14", "@15", 
  "opt_substparameters", "substparameters", "substparameter", 
  "opt_gatesizeblocks", "gatesizeblock", "@16", "@17", "opt_gatesizes", 
  "gatesizes", "gatesize", "opt_submod_displayblock", "connblock", "@18", 
  "@19", "opt_connections", "connections", "connection", "loopconnection", 
  "@20", "@21", "loopvarlist", "loopvar", "opt_conn_condition", 
  "opt_conn_displaystr", "notloopconnections", "notloopconnection", 
  "gate_spec_L", "mod_L", "gate_L", "parentgate_L", "gate_spec_R", 
  "mod_R", "gate_R", "parentgate_R", "channeldescr", "cdname", "cddelay", 
  "cderror", "cddatarate", "network", "networkheader", "endnetwork", 
  "vector", "expression", "inputvalue", "expr", "simple_expr", 
  "parameter_expr", "string_expr", "boolconst_expr", "numconst_expr", 
  "special_expr", "numconst", "timeconstant", "cppinclude", "cppstruct", 
  "cppcobject", "cppnoncobject", "enum", "@22", "@23", "opt_enumfields", 
  "enumfields", "enumfield", "message", "@24", "@25", "class", "@26", 
  "@27", "struct", "@28", "@29", "opt_propertiesblock", "@30", 
  "opt_properties", "properties", "property", "propertyvalue", 
  "opt_fieldsblock", "@31", "opt_fields", "fields", "field", "@32", "@33", 
  "fielddatatype", "opt_fieldvector", "opt_fieldenum", "opt_fieldvalue", 
  "fieldvalue", "opt_semicolon", "comma_or_semicolon", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,   106,   106,   107,   107,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   110,   109,
     111,   111,   112,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   114,   115,   115,   116,   117,
     118,   119,   120,   120,   121,   122,   122,   123,   123,   124,
     125,   125,   126,   126,   127,   127,   128,   128,   129,   129,
     130,   130,   131,   131,   133,   132,   134,   134,   135,   135,
     136,   137,   139,   138,   140,   140,   142,   141,   143,   141,
     141,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     146,   145,   147,   147,   148,   148,   148,   148,   149,   149,
     150,   150,   151,   151,   152,   152,   154,   153,   155,   155,
     156,   156,   158,   157,   159,   157,   160,   157,   161,   157,
     162,   163,   163,   164,   164,   166,   165,   167,   165,   168,
     168,   169,   169,   170,   171,   171,   172,   172,   174,   173,
     175,   173,   176,   176,   177,   177,   178,   179,   179,   181,
     180,   182,   180,   183,   183,   184,   184,   185,   185,   186,
     186,   188,   187,   189,   187,   190,   190,   191,   191,   192,
     192,   194,   195,   193,   196,   196,   197,   198,   198,   199,
     199,   200,   200,   201,   201,   201,   201,   202,   202,   203,
     203,   204,   204,   205,   205,   206,   206,   207,   207,   208,
     208,   209,   209,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     211,   212,   213,   214,   215,   215,   216,   216,   217,   218,
     219,   219,   220,   220,   220,   220,   221,   221,   221,   221,
     221,   221,   221,   221,   221,   221,   221,   221,   221,   221,
     221,   221,   221,   221,   221,   221,   221,   221,   221,   221,
     221,   221,   221,   221,   221,   221,   222,   222,   222,   222,
     222,   223,   223,   223,   223,   223,   224,   225,   225,   226,
     227,   227,   227,   228,   228,   228,   229,   229,   229,   229,
     230,   230,   231,   232,   233,   235,   234,   236,   234,   237,
     237,   238,   238,   239,   239,   241,   240,   242,   240,   244,
     243,   245,   243,   247,   246,   248,   246,   250,   249,   249,
     251,   251,   252,   252,   253,   254,   254,   254,   254,   254,
     254,   256,   255,   255,   257,   257,   258,   258,   260,   259,
     261,   259,   262,   262,   262,   262,   262,   262,   262,   262,
     262,   263,   263,   263,   264,   264,   265,   265,   266,   266,
     266,   266,   266,   266,   266,   267,   267,   268,   268,   268
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     3,
       3,     2,     1,     2,     3,     3,     3,     4,     4,     4,
       4,     4,     4,     5,     5,     5,     5,     5,     5,     3,
       4,     4,     4,     5,     5,     5,     5,     5,     5,     6,
       6,     6,     6,     6,     6,     2,     3,     2,     2,     3,
       3,     3,     5,     7,     3,     3,     2,     8,    10,     3,
       3,     2,     1,     0,     1,     0,     1,     0,     1,     0,
       1,     0,     1,     0,     0,     4,     1,     0,     3,     2,
       1,     4,     0,     4,     1,     0,     0,     4,     0,     4,
       2,     1,     3,     2,     3,     4,     4,     3,     3,     3,
       0,     4,     1,     0,     3,     2,     3,     2,     3,     2,
       3,     1,     3,     2,     3,     1,     0,     4,     1,     0,
       2,     1,     0,     7,     0,     8,     0,     9,     0,    10,
       3,     1,     0,     2,     1,     0,     4,     0,     6,     1,
       0,     3,     2,     1,     1,     0,     2,     1,     0,     4,
       0,     6,     1,     0,     3,     2,     3,     2,     0,     0,
       4,     0,     6,     1,     0,     3,     2,     2,     1,     4,
       0,     0,     5,     0,     4,     1,     0,     3,     2,     1,
       1,     0,     0,     7,     3,     1,     5,     2,     0,     2,
       0,     3,     2,     5,     7,     5,     7,     3,     1,     2,
       1,     2,     1,     2,     1,     3,     1,     2,     1,     2,
       1,     2,     1,     0,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       1,     2,     2,     2,     4,     6,     5,     7,     2,     3,
       1,     1,     6,     4,     3,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     3,     2,     3,
       3,     5,     3,     4,     6,     8,     1,     1,     1,     1,
       1,     1,     2,     3,     3,     2,     1,     1,     1,     1,
       1,     3,     4,     1,     1,     1,     3,     3,     2,     2,
       2,     2,     3,     3,     3,     0,     7,     0,     9,     1,
       0,     2,     1,     2,     4,     0,     8,     0,    10,     0,
       8,     0,    10,     0,     8,     0,    10,     0,     4,     0,
       1,     0,     2,     1,     4,     1,     1,     1,     1,     1,
       1,     0,     4,     0,     1,     0,     2,     1,     0,     7,
       0,     8,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     3,     2,     0,     4,     0,     2,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     1,     1,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       2,    18,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     1,     4,     5,     6,     0,     7,    73,
       8,    73,     9,   142,    10,    11,    12,    13,    14,    15,
      16,    17,     0,   386,    55,   386,     0,   310,   311,     0,
       0,     0,     0,     0,     0,     3,     0,     0,     0,   386,
       0,    23,     0,     0,     0,     0,    73,    77,    72,    73,
      77,     0,   142,   155,   141,   144,    22,    19,     0,   385,
      64,    69,     0,     0,   312,   333,   313,   314,     0,   315,
       0,   325,     0,   329,   303,   304,   291,   296,   297,   298,
     255,     0,     0,     0,   300,     0,     0,     0,     0,   386,
     251,   250,   256,   286,   287,   288,   289,   290,   299,   305,
     386,   386,   386,    57,   386,    39,     0,     0,     0,    25,
       0,     0,    24,     0,     0,    26,     0,     0,    84,    77,
       0,    79,    76,    77,    79,     0,   145,   155,     0,     0,
     154,   157,   143,     0,    21,   386,     0,   339,     0,   320,
       0,   339,     0,   339,   308,   309,     0,     0,   292,     0,
     295,     0,     0,     0,   274,   278,   264,     0,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      59,    61,    56,    58,    41,     0,     0,    40,     0,     0,
      42,     0,     0,    30,     0,    29,     0,    27,     0,    28,
       0,    32,     0,    31,     0,    87,    79,    92,     0,     0,
      78,    79,    81,     0,   150,     0,     0,   158,   386,   244,
     156,    20,     0,   246,   335,     0,   353,   317,     0,     0,
     319,   322,   327,   353,   331,   353,     0,     0,   306,   307,
     282,     0,   254,     0,   293,   294,     0,   301,   257,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   275,   276,
     277,   279,   280,     0,   258,   259,   260,   261,   262,   263,
      46,     0,    45,     0,    43,     0,    44,     0,    48,     0,
      47,     0,    35,    36,    33,    34,    38,    37,    90,    85,
      86,     0,     0,    95,   110,   386,    62,    81,     0,    83,
      80,   147,   153,   146,   149,     0,   386,     0,   163,   248,
     386,   339,   337,     0,     0,   320,   323,     0,     0,   321,
     339,     0,   339,     0,     0,   283,     0,   253,   302,     0,
      51,    52,    49,    50,    54,    53,     0,    89,   386,   101,
       0,    93,    94,     0,   113,   386,    66,    83,   126,     0,
      75,    82,   150,     0,   152,   245,   160,     0,   159,   162,
       0,   247,   353,   341,   351,     0,     0,     0,   316,   353,
       0,   353,     0,     0,     0,   281,    88,    63,     0,   103,
      96,   100,     0,     0,   111,   112,    65,    75,   129,     0,
     183,     0,     0,    74,   148,   151,   163,     0,     0,   165,
       0,     0,   338,   340,   343,   355,   334,     0,   324,     0,
     326,     0,   330,     0,   284,   252,   104,   102,   107,   108,
     109,     0,     0,   121,   115,     0,   125,   117,     0,     0,
       0,   127,   128,   131,   181,   186,     0,   386,    67,   161,
     166,   164,     0,     0,   342,   362,   369,   370,     0,   364,
     365,   366,   367,   368,   352,   354,   357,     0,   318,     0,
       0,     0,   105,   106,    97,    99,     0,   114,     0,   119,
       0,   116,     0,   123,   386,     0,   130,   186,   191,   214,
     184,   185,   389,   189,   190,     0,     0,   208,     0,   386,
      71,   336,   346,   347,   345,   349,   350,   348,     0,   363,
       0,   356,   358,   328,   332,   285,   120,   118,   124,   122,
      68,   386,   182,     0,     0,   213,   387,   188,     0,   223,
     223,     0,    91,    70,   344,   360,   373,     0,   386,   132,
       0,     0,   195,     0,   187,     0,     0,     0,   291,   198,
       0,   216,     0,   224,   225,   226,   227,   240,   198,     0,
     212,   207,   373,     0,   375,   386,     0,   134,   142,     0,
     192,     0,   249,   241,   242,   243,   221,     0,   200,     0,
       0,   228,   229,   231,   230,   232,   233,   200,     0,   211,
     375,     0,   372,     0,   377,   136,   386,   142,   155,     0,
       0,   194,   197,     0,   205,   220,   215,   222,   198,   234,
     235,   236,   237,   238,   239,   203,   198,   377,   371,     0,
       0,     0,   142,   138,   155,   133,   168,     0,     0,   389,
     199,   219,   200,   200,     0,     0,   380,   381,   378,   379,
     383,   384,   382,   376,   359,   155,   142,   135,     0,   180,
     168,   196,   193,   202,     0,   206,   204,   361,   374,   137,
     155,     0,   169,     0,   140,   167,   201,   139,     0,   174,
       0,   171,   178,   170,   173,     0,     0,   174,   177,     0,
     176,   179,   172,   175,     0,     0,     0
};

static const short yydefgoto[] =
{
     684,    13,    14,    15,    32,    67,    68,    16,    17,    51,
     115,    52,    53,    54,    18,    19,   306,    20,    21,   448,
      57,   402,   131,   219,   309,   360,    58,   215,   299,   300,
     301,   403,   132,   303,   351,   352,   431,   432,   353,   220,
     354,   394,   395,   434,   435,   437,   438,   310,   398,   441,
     442,   443,   568,   597,   622,   646,   625,    63,    64,    65,
     224,   362,   313,   314,   315,   626,   140,   141,   318,   406,
     368,   369,   370,   649,   650,   669,   677,   673,   674,   675,
     664,   361,   487,   445,   490,   491,   492,   493,   523,   600,
     541,   542,   578,   604,   628,   494,   495,   496,   561,   497,
     549,   550,   606,   551,   552,   553,   554,   555,   556,    22,
      23,   229,   576,   557,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,    24,    25,    26,    27,    28,   149,
     325,   239,   240,   241,    29,   151,   330,    30,   153,   332,
      31,   147,   321,   236,   373,   412,   413,   414,   508,   324,
     415,   464,   465,   466,   536,   562,   467,   564,   594,   621,
     643,    70,   528
};

static const short yypact[] =
{
     421,-32768,    -7,    19,   140,   168,   162,   250,   285,   289,
     296,   299,   306,-32768,   421,-32768,-32768,    48,-32768,    52,
  -32768,    54,-32768,   115,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,   163,   113,-32768,   113,   267,-32768,-32768,   -25,
     262,   269,   -29,   117,   142,-32768,   350,   350,   350,   126,
      16,-32768,   249,   164,    30,   288,   348,   373,-32768,   348,
     373,   144,   352,   380,-32768,   352,-32768,-32768,   124,-32768,
  -32768,-32768,   368,   379,-32768,-32768,-32768,-32768,   389,-32768,
     393,-32768,   398,-32768,   399,   400,   305,-32768,-32768,-32768,
     335,    22,   233,   340,   342,   366,   366,   366,   366,   113,
  -32768,   693,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
     113,   113,   113,-32768,   113,-32768,    40,    12,    43,-32768,
     174,    35,-32768,   174,    45,-32768,    35,    45,-32768,   373,
     353,   417,-32768,   373,   417,   350,-32768,   380,   149,   418,
  -32768,   380,-32768,   163,-32768,     8,   354,   406,   357,   412,
     375,   406,   383,   406,   263,   263,   277,   294,-32768,   413,
  -32768,   431,   447,   384,-32768,-32768,-32768,   590,-32768,   366,
     366,   366,   366,   366,   366,   366,   366,   366,   366,   366,
     366,   366,   366,   366,   366,   366,   366,   366,   366,   366,
  -32768,-32768,-32768,-32768,-32768,     9,    41,-32768,     9,     3,
  -32768,    41,     3,-32768,   459,-32768,   459,-32768,   459,-32768,
     459,-32768,   459,-32768,   459,   460,   417,-32768,   409,   475,
  -32768,   417,   490,   420,   466,   407,   350,-32768,   113,-32768,
  -32768,-32768,   467,-32768,-32768,   424,   457,-32768,   223,   419,
  -32768,   412,-32768,   457,-32768,   457,   399,   400,-32768,-32768,
  -32768,   477,-32768,   518,-32768,-32768,   422,-32768,-32768,   402,
     402,   402,   402,   402,   402,   743,   743,   743,   258,   258,
     258,   217,   217,   668,   321,   321,   427,   427,   427,   427,
  -32768,   429,-32768,   429,-32768,   429,-32768,   429,-32768,   429,
  -32768,   429,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,   206,   430,   210,-32768,   146,-32768,   490,   438,   516,
  -32768,-32768,-32768,-32768,-32768,   232,   113,   444,   495,-32768,
     113,   406,-32768,   448,   436,   412,-32768,   501,   442,-32768,
     406,   441,   406,   443,   366,-32768,   366,-32768,-32768,   366,
  -32768,-32768,-32768,-32768,-32768,-32768,   460,-32768,   113,   453,
     505,-32768,-32768,   244,   337,   113,-32768,   516,-32768,   120,
     529,-32768,   466,   466,-32768,-32768,-32768,   454,-32768,-32768,
     261,-32768,   457,   507,-32768,   449,   461,   478,-32768,   457,
     479,   457,   480,   554,   616,   718,-32768,-32768,   416,-32768,
  -32768,   224,   534,   536,-32768,-32768,-32768,   529,   540,   491,
  -32768,   492,   555,-32768,-32768,-32768,   495,   350,   495,-32768,
     485,   484,-32768,-32768,   507,   231,-32768,   488,-32768,   498,
  -32768,   502,-32768,   366,-32768,-32768,   557,   560,-32768,-32768,
  -32768,   210,   210,   513,   337,   268,   517,   337,   272,   519,
     528,-32768,-32768,   540,-32768,   184,   580,   153,-32768,-32768,
  -32768,-32768,   523,   483,-32768,   530,-32768,-32768,   394,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,   231,   584,-32768,   543,
     545,   642,-32768,-32768,-32768,-32768,   532,-32768,   534,-32768,
     548,-32768,   536,-32768,   113,   599,-32768,   184,-32768,   172,
  -32768,-32768,   275,-32768,-32768,   355,   549,-32768,   556,   113,
  -32768,-32768,   399,   400,-32768,-32768,-32768,-32768,   558,-32768,
     613,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,     1,-32768,   618,   350,   551,-32768,   230,   184,   222,
     222,   619,-32768,-32768,-32768,-32768,   569,   620,    70,-32768,
     571,   653,   582,   598,-32768,   350,   350,   350,   138,   661,
     601,-32768,   683,-32768,   371,   276,   390,-32768,   661,   691,
     625,-32768,   569,   130,   672,   113,   692,-32768,   352,   350,
  -32768,   618,-32768,-32768,-32768,-32768,   634,   350,   736,   716,
     734,   768,   772,   768,   775,   772,   775,   736,   734,-32768,
     672,   696,-32768,   697,   702,-32768,   113,   352,   380,   783,
     781,-32768,-32768,   767,-32768,   625,-32768,   173,   661,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,   661,   702,-32768,   782,
     474,   724,   352,-32768,   380,-32768,   810,   350,   795,   304,
  -32768,-32768,   736,   736,   732,   725,   399,   400,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,   380,   352,-32768,   151,   816,
     810,-32768,-32768,   800,   781,-32768,-32768,-32768,-32768,-32768,
     380,   350,-32768,   751,-32768,-32768,-32768,-32768,   752,   803,
     804,-32768,   625,-32768,-32768,   324,   745,   803,-32768,   803,
  -32768,-32768,-32768,-32768,   844,   845,-32768
};

static const short yypgoto[] =
{
  -32768,   832,-32768,-32768,-32768,   704,-32768,-32768,-32768,   -14,
    -102,   -42,    91,    38,-32768,-32768,-32768,-32768,-32768,-32768,
     190,   451,    18,   -72,   542,   493,-32768,-32768,-32768,   506,
  -32768,-32768,-32768,-32768,-32768,    -9,-32768,-32768,-32768,-32768,
  -32768,-32768,  -376,   376,-32768,   369,-32768,-32768,-32768,-32768,
     410,-32768,-32768,-32768,-32768,-32768,  -594,   -59,   790,-32768,
  -32768,-32768,   494,   496,-32768,   -50,   717,-32768,-32768,-32768,
     455,   452,-32768,   207,-32768,-32768,-32768,   185,   186,-32768,
  -32768,-32768,-32768,-32768,   377,   338,-32768,-32768,-32768,-32768,
     292,-32768,  -389,  -562,   213,  -430,-32768,-32768,-32768,-32768,
    -335,-32768,-32768,-32768,   339,-32768,  -521,  -338,  -329,-32768,
  -32768,-32768,  -468,   -46,-32768,   -53,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,  -149,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,   546,   627,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,  -144,-32768,-32768,   456,-32768,-32768,  -216,
  -32768,-32768,   408,-32768,-32768,-32768,   414,   311,   286,   260,
  -32768,   -31,   246
};


#define	YYLAST		877


static const short yytable[] =
{
      99,   110,   111,   137,    71,   248,   249,   243,   116,   245,
      47,   123,   126,   139,   194,   197,   200,    48,   113,    47,
      48,   525,    46,    47,    48,   615,   537,   331,    78,   333,
     647,    33,    73,   232,   583,   585,    46,    47,   119,   122,
     125,    46,   164,   165,   166,   167,    46,    46,    48,    46,
      47,   659,    47,   538,    46,    47,    48,    34,   477,    49,
     158,   481,   222,   612,    49,   614,   667,   159,   168,    79,
     655,   656,    74,    75,    49,   198,   201,    49,   134,   190,
     191,   192,   210,   193,    55,   214,    55,   225,   118,   223,
     121,   124,   589,   280,   282,   566,   284,   286,    69,   288,
     290,   524,   114,   251,   253,    69,   203,   205,   114,   207,
     209,   114,   211,   213,   233,   114,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   279,   631,   399,   114,
     114,   117,   114,   120,   302,   127,    50,   216,    61,   307,
      56,   221,    59,  -222,   196,   199,   410,   287,   204,  -222,
     291,   208,  -222,   419,   112,   421,   591,    69,   135,   587,
     629,    47,    48,   226,    80,   661,  -222,   372,    35,   340,
     317,   341,    48,   342,   355,   343,   379,   344,   381,   345,
     292,   499,   293,    49,   294,   558,   295,   319,   296,    82,
     297,    37,    66,    49,   678,   488,    36,   195,   400,   202,
      69,    60,   206,    62,    38,    81,   581,   212,   586,   632,
     143,   144,   489,    69,   629,   582,   584,   633,   545,   546,
     547,   592,   136,   281,  -222,  -222,   285,   227,   524,   662,
      83,  -218,   156,    69,   610,   608,   129,   613,   349,   133,
      69,  -388,   609,   616,   611,    46,   350,    48,    84,    85,
     548,    87,   -98,    88,    89,    90,    91,    92,  -388,   455,
     -98,   160,   524,   524,   356,  -210,  -218,   161,    49,   456,
     457,   383,   545,   384,   547,   365,   385,   283,    39,   371,
      93,    94,   289,   458,   459,   460,   461,   462,   463,   246,
     247,    95,   346,   347,   507,    96,   184,   185,   186,   187,
     188,   189,    97,    84,    85,    86,    87,   387,    88,    89,
     326,    91,    92,    40,   396,   327,    98,    41,   363,   364,
      84,    85,    86,    87,    42,    88,    89,    43,    91,    92,
     390,   391,   181,   182,    44,    93,    94,   184,   185,   186,
     187,   188,   189,   392,   393,    72,    95,   408,   409,    76,
      96,   450,    93,    94,   478,   479,    77,    97,   482,   483,
     471,   526,   527,    95,   529,   530,   128,    96,   546,   547,
      55,    98,   250,   130,    97,    61,    84,    85,    86,    87,
     138,    88,    89,    90,    91,    92,   545,   546,    98,   252,
     526,   653,    84,    85,    86,    87,   145,    88,    89,   156,
      91,    92,   186,   187,   188,   189,   500,   146,    93,    94,
     679,   680,   474,   475,     1,     2,     3,   148,   218,    95,
       4,   150,   455,    96,    93,    94,   152,   154,   155,   157,
      97,   217,   456,   457,   162,    95,   163,     5,   228,    96,
     238,   254,   234,   520,    98,   237,    97,   459,   460,   461,
     462,   463,   426,   427,   428,   429,   430,   235,   533,   255,
      98,   642,     6,   242,     7,     8,     9,    10,   543,    11,
      12,   244,   178,   179,   180,   256,   181,   182,    49,   257,
     539,   184,   185,   186,   187,   188,   189,   304,   298,   573,
     574,   575,   305,   308,   312,   320,   316,   567,   311,   598,
     636,   637,   322,   638,   639,   640,   641,   323,   328,   502,
     503,   189,   504,   599,   505,   506,   358,   338,   114,   348,
     359,   602,   366,   367,   595,   375,   374,   377,   624,   378,
     380,   388,   382,   389,   401,   411,   416,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   407,   178,   179,   180,
     417,   181,   182,   645,   183,   623,   184,   185,   186,   187,
     188,   189,   433,   334,   436,   418,   420,   422,   440,   444,
     446,   651,   335,   447,   452,   468,   453,   660,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   469,   178,   179,
     180,   470,   181,   182,   472,   183,   473,   184,   185,   186,
     187,   188,   189,   476,   336,   668,   485,   480,   484,   498,
     501,   509,   512,   337,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   516,   178,   179,   180,   521,   181,   182,
     513,   183,   514,   184,   185,   186,   187,   188,   189,   518,
     423,   535,   531,   532,  -209,   534,   540,   560,   565,   424,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   563,
     178,   179,   180,   569,   181,   182,   570,   183,   571,   184,
     185,   186,   187,   188,   189,   577,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   258,   178,   179,   180,   572,
     181,   182,   580,   183,   579,   184,   185,   186,   187,   188,
     189,   588,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   425,   178,   179,   180,   524,   181,   182,   593,   183,
     596,   184,   185,   186,   187,   188,   189,  -217,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   515,   178,   179,
     180,   603,   181,   182,   605,   183,   339,   184,   185,   186,
     187,   188,   189,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   607,   178,   179,   180,   547,   181,   182,   546,
     183,   545,   184,   185,   186,   187,   188,   189,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   618,   178,   179,
     180,   619,   181,   182,   620,   627,   630,   184,   185,   186,
     187,   188,   189,   169,   170,   171,   172,   173,   174,   489,
     635,   644,   648,   178,   179,   180,   652,   181,   182,   657,
     658,   663,   184,   185,   186,   187,   188,   189,  -388,   670,
     671,   672,   681,   676,   685,   686,    45,   231,   439,   357,
     397,   519,   386,   486,   517,   142,   404,   665,   230,   405,
     451,   449,   682,   601,   522,   683,   544,   666,   329,   559,
     454,   376,   510,   590,   511,   654,   617,   634
};

static const short yycheck[] =
{
      46,    47,    48,    62,    35,   154,   155,   151,    50,   153,
       7,    53,    54,    63,   116,   117,   118,     8,    49,     7,
       8,   489,     6,     7,     8,   587,    25,   243,    57,   245,
     624,    38,    57,    25,   555,   556,     6,     7,    52,    53,
      54,     6,    95,    96,    97,    98,     6,     6,     8,     6,
       7,   645,     7,   521,     6,     7,     8,    38,   434,    29,
      38,   437,   134,   584,    29,   586,   660,    45,    99,    98,
     632,   633,    97,    98,    29,   117,   118,    29,    60,   110,
     111,   112,   124,   114,    32,   127,    32,   137,    50,   135,
      52,    53,   560,   195,   196,    25,   198,   199,    97,   201,
     202,   100,    99,   156,   157,    97,   120,   121,    99,   123,
     124,    99,   126,   127,   145,    99,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   605,    18,    99,
      99,    50,    99,    52,   216,    54,    98,   129,    33,   221,
      98,   133,    98,    15,   116,   117,   372,   199,   120,    21,
     202,   123,    24,   379,    38,   381,    36,    97,    24,   558,
     600,     7,     8,    24,    57,    24,    38,   321,    38,   281,
     226,   283,     8,   285,    38,   287,   330,   289,   332,   291,
     204,    38,   206,    29,   208,   530,   210,   228,   212,    57,
     214,    39,    39,    29,   672,    21,    38,   116,    88,   118,
      97,    21,   121,    98,    52,    98,   554,   126,   556,   608,
      96,    97,    38,    97,   654,   554,   555,   616,     6,     7,
       8,   101,    88,   195,    96,    97,   198,    88,   100,    88,
      98,   103,   104,    97,   582,   580,    56,   585,    38,    59,
      97,    21,   581,   588,   583,     6,    46,     8,    36,    37,
      38,    39,    38,    41,    42,    43,    44,    45,    38,    38,
      46,    38,   100,   100,   305,   103,   103,    44,    29,    48,
      49,   334,     6,   336,     8,   316,   339,   196,    38,   320,
      68,    69,   201,    62,    63,    64,    65,    66,    67,    36,
      37,    79,    96,    97,   453,    83,    89,    90,    91,    92,
      93,    94,    90,    36,    37,    38,    39,   348,    41,    42,
      97,    44,    45,    38,   355,   102,   104,    38,    96,    97,
      36,    37,    38,    39,    38,    41,    42,    38,    44,    45,
      96,    97,    84,    85,    38,    68,    69,    89,    90,    91,
      92,    93,    94,    16,    17,    88,    79,    96,    97,    97,
      83,   407,    68,    69,    96,    97,    97,    90,    96,    97,
     423,    96,    97,    79,    19,    20,    88,    83,     7,     8,
      32,   104,   105,    10,    90,    33,    36,    37,    38,    39,
      10,    41,    42,    43,    44,    45,     6,     7,   104,   105,
      96,    97,    36,    37,    38,    39,    38,    41,    42,   104,
      44,    45,    91,    92,    93,    94,   447,    38,    68,    69,
      96,    97,   431,   432,     3,     4,     5,    38,    11,    79,
       9,    38,    38,    83,    68,    69,    38,    38,    38,   104,
      90,    88,    48,    49,   104,    79,   104,    26,    30,    83,
      38,    38,    98,   484,   104,    98,    90,    63,    64,    65,
      66,    67,    46,    47,    48,    49,    50,    61,   499,    38,
     104,   620,    51,    98,    53,    54,    55,    56,   524,    58,
      59,    98,    80,    81,    82,    38,    84,    85,    29,   105,
     521,    89,    90,    91,    92,    93,    94,    88,    38,   545,
     546,   547,    27,    13,    38,    38,    99,   538,    88,   568,
      36,    37,    88,    39,    40,    41,    42,    60,    99,    36,
      37,    94,    39,   569,    41,    42,    88,   105,    99,    99,
      14,   577,    88,    38,   565,    99,    88,    36,   597,    97,
      99,    88,    99,    38,    15,    38,    97,    70,    71,    72,
      73,    74,    75,    76,    77,    78,   102,    80,    81,    82,
      99,    84,    85,   622,    87,   596,    89,    90,    91,    92,
      93,    94,    38,    96,    38,    97,    97,    97,    38,    88,
      88,   627,   105,    28,    99,    97,   102,   646,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    99,    80,    81,
      82,    99,    84,    85,    47,    87,    46,    89,    90,    91,
      92,    93,    94,   100,    96,   661,    88,   100,    99,    39,
      97,    91,    38,   105,    70,    71,    72,    73,    74,    75,
      76,    77,    78,   101,    80,    81,    82,    38,    84,    85,
      97,    87,    97,    89,    90,    91,    92,    93,    94,   101,
      96,    38,   103,    97,   103,    97,    38,    38,    38,   105,
      70,    71,    72,    73,    74,    75,    76,    77,    78,   100,
      80,    81,    82,   102,    84,    85,    23,    87,    96,    89,
      90,    91,    92,    93,    94,    24,    70,    71,    72,    73,
      74,    75,    76,    77,    78,   105,    80,    81,    82,   101,
      84,    85,    19,    87,   103,    89,    90,    91,    92,    93,
      94,    20,    70,    71,    72,    73,    74,    75,    76,    77,
      78,   105,    80,    81,    82,   100,    84,    85,    56,    87,
      38,    89,    90,    91,    92,    93,    94,   103,    70,    71,
      72,    73,    74,    75,    76,    77,    78,   105,    80,    81,
      82,    15,    84,    85,    38,    87,    88,    89,    90,    91,
      92,    93,    94,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    38,    80,    81,    82,     8,    84,    85,     7,
      87,     6,    89,    90,    91,    92,    93,    94,    70,    71,
      72,    73,    74,    75,    76,    77,    78,   101,    80,    81,
      82,   104,    84,    85,   102,    22,    39,    89,    90,    91,
      92,    93,    94,    70,    71,    72,    73,    74,    75,    38,
      38,    97,    12,    80,    81,    82,    31,    84,    85,    97,
     105,    15,    89,    90,    91,    92,    93,    94,    38,    88,
      88,    38,    97,    39,     0,     0,    14,   143,   397,   307,
     357,   482,   346,   443,   478,    65,   362,   650,   141,   363,
     408,   406,   677,   571,   487,   679,   528,   654,   241,   530,
     414,   325,   458,   562,   466,   629,   590,   617
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

case 18:
#line 246 "ned.y"
{
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                }
    break;
case 19:
#line 251 "ned.y"
{
                  /* no setTrailingComment(ps.imports,@3); comment already stored with last filename */
                }
    break;
case 22:
#line 263 "ned.y"
{
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                }
    break;
case 55:
#line 308 "ned.y"
{
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                }
    break;
case 56:
#line 318 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;
case 57:
#line 322 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;
case 58:
#line 330 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                }
    break;
case 59:
#line 337 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,"error");
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[0]);
                }
    break;
case 60:
#line 346 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,"delay");
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[0]);
                }
    break;
case 61:
#line 355 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,"datarate");
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[0]);
                }
    break;
case 63:
#line 373 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;
case 64:
#line 380 "ned.y"
{
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[-1]));
                  setComments(ps.module,yylsp[-2],yylsp[0]);
                }
    break;
case 65:
#line 389 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;
case 66:
#line 393 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;
case 68:
#line 415 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;
case 69:
#line 422 "ned.y"
{
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[-1]));
                  setComments(ps.module,yylsp[-2],yylsp[0]);
                }
    break;
case 70:
#line 431 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;
case 71:
#line 435 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                }
    break;
case 84:
#line 453 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                }
    break;
case 85:
#line 458 "ned.y"
{
                }
    break;
case 90:
#line 474 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                }
    break;
case 91:
#line 483 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                }
    break;
case 92:
#line 490 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                }
    break;
case 93:
#line 495 "ned.y"
{
                }
    break;
case 96:
#line 506 "ned.y"
{
                  setComments(ps.param,yylsp[-1]);
                }
    break;
case 98:
#line 511 "ned.y"
{
                  setComments(ps.param,yylsp[-1]);
                }
    break;
case 100:
#line 516 "ned.y"
{
                  setComments(ps.param,yylsp[-1]);
                }
    break;
case 101:
#line 523 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                }
    break;
case 102:
#line 527 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                }
    break;
case 103:
#line 531 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                }
    break;
case 104:
#line 535 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                }
    break;
case 105:
#line 539 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                }
    break;
case 106:
#line 543 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                }
    break;
case 107:
#line 547 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                }
    break;
case 108:
#line 551 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                }
    break;
case 109:
#line 555 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                }
    break;
case 110:
#line 562 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                }
    break;
case 111:
#line 567 "ned.y"
{
                }
    break;
case 120:
#line 590 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;
case 121:
#line 595 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;
case 124:
#line 608 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                }
    break;
case 125:
#line 613 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                }
    break;
case 126:
#line 621 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                }
    break;
case 127:
#line 626 "ned.y"
{
                }
    break;
case 132:
#line 642 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                }
    break;
case 133:
#line 648 "ned.y"
{
                }
    break;
case 134:
#line 651 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                }
    break;
case 135:
#line 658 "ned.y"
{
                }
    break;
case 136:
#line 661 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-3], yylsp[-1]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                }
    break;
case 137:
#line 667 "ned.y"
{
                }
    break;
case 138:
#line 670 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-4], yylsp[-1]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                }
    break;
case 139:
#line 677 "ned.y"
{
                }
    break;
case 145:
#line 699 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                }
    break;
case 146:
#line 704 "ned.y"
{
                }
    break;
case 147:
#line 707 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                }
    break;
case 148:
#line 713 "ned.y"
{
                }
    break;
case 153:
#line 729 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                }
    break;
case 158:
#line 747 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                }
    break;
case 159:
#line 752 "ned.y"
{
                }
    break;
case 160:
#line 755 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                }
    break;
case 161:
#line 761 "ned.y"
{
                }
    break;
case 166:
#line 781 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                }
    break;
case 169:
#line 795 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                }
    break;
case 170:
#line 800 "ned.y"
{
                }
    break;
case 171:
#line 803 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                }
    break;
case 172:
#line 809 "ned.y"
{
                }
    break;
case 177:
#line 825 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                }
    break;
case 178:
#line 832 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                }
    break;
case 179:
#line 840 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                }
    break;
case 181:
#line 848 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "false" );
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                }
    break;
case 182:
#line 854 "ned.y"
{
                }
    break;
case 183:
#line 857 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "yes" );
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                }
    break;
case 184:
#line 863 "ned.y"
{
                }
    break;
case 191:
#line 884 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                }
    break;
case 192:
#line 888 "ned.y"
{
                  ps.inLoop=1;
                }
    break;
case 193:
#line 892 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[0]);
                }
    break;
case 196:
#line 906 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                }
    break;
case 197:
#line 916 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                }
    break;
case 199:
#line 924 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                }
    break;
case 203:
#line 937 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-4],yylsp[0]);
                }
    break;
case 204:
#line 942 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-6],yylsp[0]);
                }
    break;
case 205:
#line 947 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-4],yylsp[0]);
                }
    break;
case 206:
#line 953 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-6],yylsp[0]);
                }
    break;
case 209:
#line 967 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                }
    break;
case 210:
#line 973 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                }
    break;
case 211:
#line 981 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;
case 212:
#line 986 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                }
    break;
case 213:
#line 993 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                }
    break;
case 214:
#line 1000 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                }
    break;
case 217:
#line 1014 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                }
    break;
case 218:
#line 1019 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                }
    break;
case 219:
#line 1026 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;
case 220:
#line 1031 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;
case 221:
#line 1038 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                }
    break;
case 222:
#line 1043 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                }
    break;
case 240:
#line 1073 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;
case 241:
#line 1081 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"delay");
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;
case 242:
#line 1089 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"error");
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;
case 243:
#line 1097 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"datarate");
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                }
    break;
case 245:
#line 1112 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                }
    break;
case 246:
#line 1120 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1],NULLPOS);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                }
    break;
case 247:
#line 1126 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-5],yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-6],yylsp[0]);
                  ps.inNetwork=1;
                }
    break;
case 248:
#line 1135 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                }
    break;
case 249:
#line 1143 "ned.y"
{ yyval = yyvsp[-1]; }
    break;
case 250:
#line 1149 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
    break;
case 251:
#line 1153 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
    break;
case 252:
#line 1164 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); }
    break;
case 253:
#line 1166 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); }
    break;
case 254:
#line 1168 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); }
    break;
case 255:
#line 1170 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); }
    break;
case 257:
#line 1176 "ned.y"
{ yyval = yyvsp[-1]; }
    break;
case 258:
#line 1179 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); }
    break;
case 259:
#line 1181 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); }
    break;
case 260:
#line 1183 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); }
    break;
case 261:
#line 1185 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); }
    break;
case 262:
#line 1187 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); }
    break;
case 263:
#line 1189 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); }
    break;
case 264:
#line 1193 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); }
    break;
case 265:
#line 1196 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); }
    break;
case 266:
#line 1198 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); }
    break;
case 267:
#line 1200 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); }
    break;
case 268:
#line 1202 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); }
    break;
case 269:
#line 1204 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); }
    break;
case 270:
#line 1206 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); }
    break;
case 271:
#line 1209 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); }
    break;
case 272:
#line 1211 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); }
    break;
case 273:
#line 1213 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); }
    break;
case 274:
#line 1217 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); }
    break;
case 275:
#line 1220 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); }
    break;
case 276:
#line 1222 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); }
    break;
case 277:
#line 1224 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); }
    break;
case 278:
#line 1228 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); }
    break;
case 279:
#line 1230 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); }
    break;
case 280:
#line 1232 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); }
    break;
case 281:
#line 1234 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); }
    break;
case 282:
#line 1237 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); }
    break;
case 283:
#line 1239 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); }
    break;
case 284:
#line 1241 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); }
    break;
case 285:
#line 1243 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); }
    break;
case 291:
#line 1256 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                }
    break;
case 292:
#line 1261 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                }
    break;
case 293:
#line 1266 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                }
    break;
case 294:
#line 1272 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                }
    break;
case 295:
#line 1278 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                }
    break;
case 296:
#line 1286 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); }
    break;
case 297:
#line 1291 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); }
    break;
case 298:
#line 1293 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); }
    break;
case 300:
#line 1302 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); }
    break;
case 301:
#line 1304 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); }
    break;
case 302:
#line 1306 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); }
    break;
case 303:
#line 1311 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); }
    break;
case 304:
#line 1313 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); }
    break;
case 305:
#line 1315 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); }
    break;
case 310:
#line 1331 "ned.y"
{
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                }
    break;
case 311:
#line 1337 "ned.y"
{
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                }
    break;
case 312:
#line 1346 "ned.y"
{
                  ps.cppstruct = (CppStructNode *)createNodeWithTag(NED_CPP_STRUCT, ps.nedfile );
                  ps.cppstruct->setName(toString(yylsp[-1]));
                  setComments(ps.cppstruct,yylsp[-2],yylsp[-1]);
                }
    break;
case 313:
#line 1355 "ned.y"
{
                  ps.cppcobject = (CppCobjectNode *)createNodeWithTag(NED_CPP_COBJECT, ps.nedfile );
                  ps.cppcobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppcobject,yylsp[-2],yylsp[-1]);
                }
    break;
case 314:
#line 1364 "ned.y"
{
                  ps.cppnoncobject = (CppNoncobjectNode *)createNodeWithTag(NED_CPP_NONCOBJECT, ps.nedfile );
                  ps.cppnoncobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppnoncobject,yylsp[-2],yylsp[-1]);
                }
    break;
case 315:
#line 1373 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
    break;
case 316:
#line 1380 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                }
    break;
case 317:
#line 1384 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
    break;
case 318:
#line 1392 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                }
    break;
case 323:
#line 1409 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                }
    break;
case 324:
#line 1414 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                }
    break;
case 325:
#line 1423 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                }
    break;
case 326:
#line 1429 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                }
    break;
case 327:
#line 1433 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                }
    break;
case 328:
#line 1440 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                }
    break;
case 329:
#line 1447 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                }
    break;
case 330:
#line 1453 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                }
    break;
case 331:
#line 1457 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                }
    break;
case 332:
#line 1464 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                }
    break;
case 333:
#line 1471 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                }
    break;
case 334:
#line 1477 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                }
    break;
case 335:
#line 1481 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                }
    break;
case 336:
#line 1488 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                }
    break;
case 337:
#line 1495 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                }
    break;
case 344:
#line 1515 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                }
    break;
case 351:
#line 1534 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                }
    break;
case 358:
#line 1554 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                }
    break;
case 359:
#line 1560 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                }
    break;
case 360:
#line 1564 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsVirtual(true);
                }
    break;
case 361:
#line 1571 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                }
    break;
case 371:
#line 1590 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                }
    break;
case 372:
#line 1595 "ned.y"
{
                  ps.field->setIsVector(true);
                }
    break;
case 374:
#line 1603 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                }
    break;
case 376:
#line 1611 "ned.y"
{
                  ps.field->setDefaultValue(toString(yylsp[0]));
                }
    break;
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
#line 1630 "ned.y"


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

ChannelAttrNode *addChanAttr(NEDElement *channel, char *attrname)
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

ConnAttrNode *addConnAttr(NEDElement *conn, char *attrname)
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

