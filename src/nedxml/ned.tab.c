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
# define	_TRUE	292
# define	_FALSE	293
# define	INPUT	294
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



#define	YYFINAL		613
#define	YYFLAG		-32768
#define	YYNTBASE	103

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 338 ? yytranslate[x] : 266)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    90,     2,     2,
     101,   102,    88,    86,    93,    87,   100,    89,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    85,    94,
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
       0,     0,     2,     3,     6,     8,    10,    12,    14,    16,
      18,    20,    22,    24,    26,    28,    30,    32,    34,    36,
      38,    40,    42,    43,    47,    51,    54,    56,    60,    66,
      69,    70,    72,    78,    83,    84,    86,    91,    95,    99,
     102,   108,   116,   120,   124,   127,   136,   147,   151,   155,
     158,   160,   161,   163,   164,   166,   167,   169,   170,   172,
     173,   175,   176,   177,   182,   184,   185,   189,   192,   194,
     199,   200,   205,   207,   208,   209,   214,   215,   220,   223,
     225,   229,   232,   236,   241,   246,   250,   254,   258,   259,
     264,   266,   267,   271,   274,   278,   281,   285,   288,   292,
     294,   298,   301,   305,   307,   308,   313,   315,   316,   319,
     321,   322,   330,   331,   340,   341,   351,   352,   363,   367,
     369,   370,   373,   375,   376,   381,   382,   389,   391,   392,
     396,   399,   401,   403,   404,   407,   409,   410,   415,   416,
     423,   425,   426,   430,   433,   437,   440,   441,   442,   447,
     448,   455,   457,   458,   462,   465,   468,   470,   475,   476,
     477,   483,   484,   489,   491,   492,   496,   499,   501,   503,
     504,   505,   513,   517,   519,   525,   528,   529,   532,   533,
     537,   540,   546,   554,   560,   568,   572,   574,   577,   579,
     582,   584,   587,   589,   593,   595,   598,   600,   603,   605,
     608,   610,   612,   615,   619,   624,   631,   637,   645,   648,
     652,   654,   656,   663,   668,   672,   674,   676,   680,   684,
     688,   692,   696,   700,   704,   707,   711,   715,   719,   723,
     727,   731,   735,   739,   743,   746,   750,   754,   758,   761,
     765,   769,   775,   779,   784,   791,   800,   802,   804,   806,
     808,   810,   812,   815,   819,   823,   826,   828,   830,   832,
     834,   836,   840,   845,   847,   849,   851,   855,   859,   862,
     865,   868,   871,   875,   879,   883,   884,   892,   893,   903,
     905,   906,   909,   911,   914,   919,   920,   929,   930,   941,
     942,   951,   952,   963,   964,   973,   974,   985,   986,   991,
     992,   994,   995,   998,  1000,  1005,  1007,  1009,  1011,  1013,
    1015,  1017,  1018,  1023,  1024,  1026,  1027,  1030,  1032,  1033,
    1041,  1042,  1051,  1053,  1056,  1058,  1060,  1062,  1064,  1066,
    1068,  1070,  1074,  1077,  1078,  1083,  1084,  1087,  1088,  1090,
    1092,  1094,  1096,  1098,  1100,  1102,  1104,  1105,  1107,  1109
};
static const short yyrhs[] =
{
     104,     0,     0,   105,   104,     0,   105,     0,   106,     0,
     111,     0,   119,     0,   123,     0,   212,     0,   110,     0,
     118,     0,   122,     0,   211,     0,   227,     0,   228,     0,
     229,     0,   230,     0,   231,     0,   237,     0,   240,     0,
     243,     0,     0,     3,   107,   108,     0,   109,    93,   108,
       0,   109,    94,     0,    36,     0,   112,   115,   117,     0,
     112,    95,   113,    96,   264,     0,     5,    35,     0,     0,
     114,     0,    35,    97,   216,    94,   114,     0,    35,    97,
     216,    94,     0,     0,   116,     0,    35,   216,    94,   116,
       0,    35,   216,    94,     0,    26,    35,   264,     0,    26,
     264,     0,   120,   126,   128,   129,   121,     0,   120,    95,
     126,   128,   129,    96,   264,     0,     4,    35,   264,     0,
      24,    35,   264,     0,    24,   264,     0,   124,   126,   128,
     129,   130,   131,   127,   125,     0,   124,    95,   126,   128,
     129,   130,   131,   127,    96,   264,     0,     6,    35,   264,
       0,    25,    35,   264,     0,    25,   264,     0,   132,     0,
       0,   137,     0,     0,   138,     0,     0,   145,     0,     0,
     153,     0,     0,   187,     0,     0,     0,    29,    85,   133,
     134,     0,   135,     0,     0,   136,    93,   135,     0,   136,
      94,     0,    35,     0,    12,    85,    36,    94,     0,     0,
       7,    85,   139,   140,     0,   141,     0,     0,     0,   144,
      93,   142,   141,     0,     0,   144,    94,   143,   141,     0,
     144,    94,     0,    35,     0,    35,    85,    44,     0,    43,
      35,     0,    35,    85,    43,     0,    35,    85,    43,    44,
       0,    35,    85,    44,    43,     0,    35,    85,    45,     0,
      35,    85,    46,     0,    35,    85,    47,     0,     0,     8,
      85,   146,   147,     0,   148,     0,     0,    13,   149,   148,
       0,    13,   149,     0,    14,   151,   148,     0,    14,   151,
       0,   150,    93,   149,     0,   150,    94,     0,    35,    98,
      99,     0,    35,     0,   152,    93,   151,     0,   152,    94,
       0,    35,    98,    99,     0,    35,     0,     0,    10,    85,
     154,   155,     0,   156,     0,     0,   157,   156,     0,   157,
       0,     0,    35,    85,    35,   264,   158,   163,   162,     0,
       0,    35,    85,    35,   215,   264,   159,   163,   162,     0,
       0,    35,    85,    35,    22,    35,   264,   160,   163,   162,
       0,     0,    35,    85,    35,   215,    22,    35,   264,   161,
     163,   162,     0,   171,   179,   186,     0,   164,     0,     0,
     165,   164,     0,   165,     0,     0,    30,    85,   166,   168,
       0,     0,    30,    21,   216,    85,   167,   168,     0,   169,
       0,     0,   170,    93,   169,     0,   170,    94,     0,    35,
       0,   172,     0,     0,   173,   172,     0,   173,     0,     0,
       7,    85,   174,   176,     0,     0,     7,    21,   216,    85,
     175,   176,     0,   177,     0,     0,   178,    93,   177,     0,
     178,    94,     0,    35,    97,   216,     0,   180,   179,     0,
       0,     0,     9,    85,   181,   183,     0,     0,     9,    21,
     216,    85,   182,   183,     0,   184,     0,     0,   185,    93,
     184,     0,   185,    94,     0,    35,   215,     0,    35,     0,
      12,    85,    36,    94,     0,     0,     0,    11,    15,    85,
     188,   190,     0,     0,    11,    85,   189,   190,     0,   191,
       0,     0,   192,   265,   191,     0,   192,    94,     0,   193,
       0,   201,     0,     0,     0,    18,   194,   196,    20,   195,
     200,    28,     0,   197,    93,   196,     0,   197,     0,    35,
      97,   216,    19,   216,     0,    21,   216,     0,     0,    12,
      36,     0,     0,   201,   265,   200,     0,   201,    94,     0,
     202,    17,   206,   198,   199,     0,   202,    17,   210,    17,
     206,   198,   199,     0,   202,    16,   206,   198,   199,     0,
     202,    16,   210,    16,   206,   198,   199,     0,   203,   100,
     204,     0,   205,     0,    35,   215,     0,    35,     0,    35,
     215,     0,    35,     0,    35,   215,     0,    35,     0,   207,
     100,   208,     0,   209,     0,    35,   215,     0,    35,     0,
      35,   215,     0,    35,     0,    35,   215,     0,    35,     0,
      35,     0,    35,   216,     0,    35,   216,   210,     0,   213,
     163,   171,   214,     0,   213,    95,   163,   171,    96,   264,
       0,    23,    35,    85,    35,   264,     0,    23,    35,    85,
      35,    22,    35,   264,     0,    27,   264,     0,    98,   216,
      99,     0,   218,     0,   217,     0,    40,   101,   218,    93,
     218,   102,     0,    40,   101,   218,   102,     0,    40,   101,
     102,     0,    40,     0,   219,     0,   101,   218,   102,     0,
     218,    86,   218,     0,   218,    87,   218,     0,   218,    88,
     218,     0,   218,    89,   218,     0,   218,    90,   218,     0,
     218,    91,   218,     0,    87,   218,     0,   218,    67,   218,
       0,   218,    68,   218,     0,   218,    69,   218,     0,   218,
      70,   218,     0,   218,    71,   218,     0,   218,    72,   218,
       0,   218,    73,   218,     0,   218,    74,   218,     0,   218,
      75,   218,     0,    76,   218,     0,   218,    77,   218,     0,
     218,    78,   218,     0,   218,    79,   218,     0,    80,   218,
       0,   218,    81,   218,     0,   218,    82,   218,     0,   218,
      84,   218,    85,   218,     0,    35,   101,   102,     0,    35,
     101,   218,   102,     0,    35,   101,   218,    93,   218,   102,
       0,    35,   101,   218,    93,   218,    93,   218,   102,     0,
     220,     0,   221,     0,   222,     0,   223,     0,   224,     0,
      35,     0,    41,    35,     0,    41,    42,    35,     0,    42,
      41,    35,     0,    42,    35,     0,    36,     0,    38,     0,
      39,     0,   225,     0,    66,     0,    66,   101,   102,     0,
      65,   101,    35,   102,     0,    33,     0,    34,     0,   226,
       0,    33,    35,   226,     0,    34,    35,   226,     0,    33,
      35,     0,    34,    35,     0,    48,    36,     0,    48,    49,
       0,    50,    35,    94,     0,    51,    35,    94,     0,    52,
      35,    94,     0,     0,    53,    35,    95,   232,   234,    96,
      94,     0,     0,    53,    35,    54,    35,    95,   233,   234,
      96,    94,     0,   235,     0,     0,   236,   235,     0,   236,
       0,    35,    94,     0,    35,    97,    33,    94,     0,     0,
      55,    35,    95,   238,   246,   252,    96,    94,     0,     0,
      55,    35,    54,    35,    95,   239,   246,   252,    96,    94,
       0,     0,    56,    35,    95,   241,   246,   252,    96,    94,
       0,     0,    56,    35,    54,    35,    95,   242,   246,   252,
      96,    94,     0,     0,    50,    35,    95,   244,   246,   252,
      96,    94,     0,     0,    50,    35,    54,    35,    95,   245,
     246,   252,    96,    94,     0,     0,    58,    85,   247,   248,
       0,     0,   249,     0,     0,   250,   249,     0,   250,     0,
      35,    97,   251,    94,     0,    36,     0,    33,     0,    34,
       0,   226,     0,    38,     0,    39,     0,     0,    57,    85,
     253,   254,     0,     0,   255,     0,     0,   256,   255,     0,
     256,     0,     0,   259,    35,   257,   260,   261,   262,    94,
       0,     0,    59,   259,    35,   258,   260,   261,   262,    94,
       0,    35,     0,    35,    88,     0,    60,     0,    61,     0,
      62,     0,    63,     0,    64,     0,    45,     0,    46,     0,
      98,    33,    99,     0,    98,    99,     0,     0,    53,   101,
      35,   102,     0,     0,    97,   263,     0,     0,    36,     0,
      37,     0,    33,     0,    34,     0,   226,     0,    38,     0,
      39,     0,    94,     0,     0,    93,     0,    94,     0,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   218,   219,   223,   224,   228,   229,   230,   231,   232,
     234,   235,   236,   237,   239,   240,   241,   242,   243,   244,
     245,   246,   250,   250,   262,   263,   267,   277,   281,   290,
     299,   300,   304,   310,   319,   320,   324,   330,   339,   343,
     350,   358,   369,   378,   382,   389,   400,   414,   423,   427,
     434,   435,   438,   438,   439,   439,   440,   440,   441,   441,
     442,   442,   445,   445,   456,   457,   461,   462,   466,   475,
     482,   482,   493,   494,   498,   498,   503,   503,   508,   515,
     519,   523,   527,   531,   535,   539,   543,   547,   554,   554,
     565,   566,   570,   571,   572,   573,   577,   578,   582,   587,
     595,   596,   600,   605,   613,   613,   624,   625,   629,   630,
     634,   634,   643,   643,   653,   653,   662,   662,   675,   681,
     682,   686,   687,   691,   691,   699,   699,   711,   712,   716,
     717,   721,   729,   730,   734,   735,   739,   739,   747,   747,
     760,   761,   765,   769,   773,   782,   783,   787,   787,   795,
     795,   807,   808,   812,   813,   817,   824,   832,   836,   840,
     840,   849,   849,   861,   862,   866,   867,   871,   872,   876,
     876,   876,   893,   894,   898,   908,   912,   916,   920,   924,
     925,   929,   934,   939,   945,   954,   955,   959,   965,   973,
     978,   985,   992,  1001,  1002,  1006,  1011,  1018,  1023,  1030,
    1035,  1043,  1048,  1053,  1061,  1068,  1079,  1085,  1094,  1102,
    1107,  1112,  1123,  1125,  1127,  1129,  1134,  1135,  1138,  1140,
    1142,  1144,  1146,  1148,  1151,  1155,  1157,  1159,  1161,  1163,
    1165,  1168,  1170,  1172,  1175,  1179,  1181,  1183,  1186,  1189,
    1191,  1193,  1196,  1198,  1200,  1202,  1207,  1208,  1209,  1210,
    1211,  1215,  1220,  1225,  1231,  1237,  1245,  1250,  1252,  1257,
    1261,  1263,  1265,  1270,  1272,  1274,  1279,  1280,  1281,  1282,
    1290,  1296,  1305,  1314,  1323,  1332,  1332,  1343,  1343,  1358,
    1359,  1363,  1364,  1368,  1373,  1382,  1382,  1392,  1392,  1406,
    1406,  1416,  1416,  1430,  1430,  1440,  1440,  1454,  1454,  1460,
    1464,  1465,  1469,  1470,  1474,  1484,  1485,  1486,  1487,  1488,
    1489,  1493,  1493,  1499,  1503,  1504,  1508,  1509,  1513,  1513,
    1523,  1523,  1537,  1538,  1539,  1540,  1541,  1542,  1543,  1544,
    1545,  1549,  1554,  1558,  1562,  1566,  1570,  1574,  1578,  1579,
    1580,  1581,  1582,  1583,  1584,  1587,  1587,  1588,  1588,  1588
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
  "CHARCONSTANT", "_TRUE", "_FALSE", "INPUT", "REF", "ANCESTOR", 
  "CONSTDECL", "NUMERICTYPE", "STRINGTYPE", "BOOLTYPE", "ANYTYPE", 
  "CPPINCLUDE", "SYSINCFILENAME", "STRUCT", "COBJECT", "NONCOBJECT", 
  "ENUM", "EXTENDS", "MESSAGE", "CLASS", "FIELDS", "PROPERTIES", 
  "VIRTUAL", "CHARTYPE", "SHORTTYPE", "INTTYPE", "LONGTYPE", "DOUBLETYPE", 
  "SIZEOF", "SUBMODINDEX", "EQ", "NE", "GT", "GE", "LS", "LE", "AND", 
  "OR", "XOR", "NOT", "BIN_AND", "BIN_OR", "BIN_XOR", "BIN_COMPL", 
  "SHIFT_LEFT", "SHIFT_RIGHT", "INVALID_CHAR", "'?'", "':'", "'+'", "'-'", 
  "'*'", "'/'", "'%'", "'^'", "UMIN", "','", "';'", "'{'", "'}'", "'='", 
  "'['", "']'", "'.'", "'('", "')'", "networkdescription", 
  "somedefinitions", "definition", "import", "@1", "filenames", 
  "filename", "channeldefinition_old", "channeldefinition", 
  "channelheader", "opt_channelattrblock", "channelattrblock", 
  "opt_channelattrblock_old", "channelattrblock_old", "endchannel", 
  "simpledefinition_old", "simpledefinition", "simpleheader", "endsimple", 
  "moduledefinition_old", "moduledefinition", "moduleheader", "endmodule", 
  "opt_machineblock", "opt_displayblock", "opt_paramblock", 
  "opt_gateblock", "opt_submodblock", "opt_connblock", "machineblock", 
  "@2", "opt_machinelist", "machinelist", "machine", "displayblock", 
  "paramblock", "@3", "opt_parameters", "parameters", "@4", "@5", 
  "parameter", "gateblock", "@6", "opt_gates", "gates", "gatesI", "gateI", 
  "gatesO", "gateO", "submodblock", "@7", "opt_submodules", "submodules", 
  "submodule", "@8", "@9", "@10", "@11", "submodule_body", 
  "opt_on_blocks", "on_blocks", "on_block", "@12", "@13", "opt_on_list", 
  "on_list", "on_mach", "opt_substparamblocks", "substparamblocks", 
  "substparamblock", "@14", "@15", "opt_substparameters", 
  "substparameters", "substparameter", "opt_gatesizeblocks", 
  "gatesizeblock", "@16", "@17", "opt_gatesizes", "gatesizes", "gatesize", 
  "opt_submod_displayblock", "connblock", "@18", "@19", "opt_connections", 
  "connections", "connection", "loopconnection", "@20", "@21", 
  "loopvarlist", "loopvar", "opt_conn_condition", "opt_conn_displaystr", 
  "notloopconnections", "notloopconnection", "gate_spec_L", "mod_L", 
  "gate_L", "parentgate_L", "gate_spec_R", "mod_R", "gate_R", 
  "parentgate_R", "channeldescr", "networkdefinition_old", 
  "networkdefinition", "networkheader", "endnetwork", "vector", 
  "expression", "inputvalue", "expr", "simple_expr", "parameter_expr", 
  "string_expr", "boolconst_expr", "numconst_expr", "special_expr", 
  "numconst", "timeconstant", "cppinclude", "cppstruct", "cppcobject", 
  "cppnoncobject", "enum", "@22", "@23", "opt_enumfields", "enumfields", 
  "enumfield", "message", "@24", "@25", "class", "@26", "@27", "struct", 
  "@28", "@29", "opt_propertiesblock", "@30", "opt_properties", 
  "properties", "property", "propertyvalue", "opt_fieldsblock", "@31", 
  "opt_fields", "fields", "field", "@32", "@33", "fielddatatype", 
  "opt_fieldvector", "opt_fieldenum", "opt_fieldvalue", "fieldvalue", 
  "opt_semicolon", "comma_or_semicolon", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,   103,   103,   104,   104,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   107,   106,   108,   108,   109,   110,   111,   112,
     113,   113,   114,   114,   115,   115,   116,   116,   117,   117,
     118,   119,   120,   121,   121,   122,   123,   124,   125,   125,
     126,   126,   127,   127,   128,   128,   129,   129,   130,   130,
     131,   131,   133,   132,   134,   134,   135,   135,   136,   137,
     139,   138,   140,   140,   142,   141,   143,   141,   141,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   146,   145,
     147,   147,   148,   148,   148,   148,   149,   149,   150,   150,
     151,   151,   152,   152,   154,   153,   155,   155,   156,   156,
     158,   157,   159,   157,   160,   157,   161,   157,   162,   163,
     163,   164,   164,   166,   165,   167,   165,   168,   168,   169,
     169,   170,   171,   171,   172,   172,   174,   173,   175,   173,
     176,   176,   177,   177,   178,   179,   179,   181,   180,   182,
     180,   183,   183,   184,   184,   185,   185,   186,   186,   188,
     187,   189,   187,   190,   190,   191,   191,   192,   192,   194,
     195,   193,   196,   196,   197,   198,   198,   199,   199,   200,
     200,   201,   201,   201,   201,   202,   202,   203,   203,   204,
     204,   205,   205,   206,   206,   207,   207,   208,   208,   209,
     209,   210,   210,   210,   211,   212,   213,   213,   214,   215,
     216,   216,   217,   217,   217,   217,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   219,   219,   219,   219,
     219,   220,   220,   220,   220,   220,   221,   222,   222,   223,
     224,   224,   224,   225,   225,   225,   226,   226,   226,   226,
     227,   227,   228,   229,   230,   232,   231,   233,   231,   234,
     234,   235,   235,   236,   236,   238,   237,   239,   237,   241,
     240,   242,   240,   244,   243,   245,   243,   247,   246,   246,
     248,   248,   249,   249,   250,   251,   251,   251,   251,   251,
     251,   253,   252,   252,   254,   254,   255,   255,   257,   256,
     258,   256,   259,   259,   259,   259,   259,   259,   259,   259,
     259,   260,   260,   260,   261,   261,   262,   262,   263,   263,
     263,   263,   263,   263,   263,   264,   264,   265,   265,   265
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     3,     3,     2,     1,     3,     5,     2,
       0,     1,     5,     4,     0,     1,     4,     3,     3,     2,
       5,     7,     3,     3,     2,     8,    10,     3,     3,     2,
       1,     0,     1,     0,     1,     0,     1,     0,     1,     0,
       1,     0,     0,     4,     1,     0,     3,     2,     1,     4,
       0,     4,     1,     0,     0,     4,     0,     4,     2,     1,
       3,     2,     3,     4,     4,     3,     3,     3,     0,     4,
       1,     0,     3,     2,     3,     2,     3,     2,     3,     1,
       3,     2,     3,     1,     0,     4,     1,     0,     2,     1,
       0,     7,     0,     8,     0,     9,     0,    10,     3,     1,
       0,     2,     1,     0,     4,     0,     6,     1,     0,     3,
       2,     1,     1,     0,     2,     1,     0,     4,     0,     6,
       1,     0,     3,     2,     3,     2,     0,     0,     4,     0,
       6,     1,     0,     3,     2,     2,     1,     4,     0,     0,
       5,     0,     4,     1,     0,     3,     2,     1,     1,     0,
       0,     7,     3,     1,     5,     2,     0,     2,     0,     3,
       2,     5,     7,     5,     7,     3,     1,     2,     1,     2,
       1,     2,     1,     3,     1,     2,     1,     2,     1,     2,
       1,     1,     2,     3,     4,     6,     5,     7,     2,     3,
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
       2,    22,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     1,     4,     5,    10,     6,    34,    11,
       7,    51,    12,     8,    51,    13,     9,   120,    14,    15,
      16,    17,    18,    19,    20,    21,     0,   346,    29,   346,
       0,   270,   271,     0,     0,     0,     0,     0,     0,     3,
       0,    30,     0,    35,     0,    51,    55,    50,    51,    55,
       0,   120,   133,   119,   122,    26,    23,     0,   345,    42,
      47,     0,     0,   272,   293,   273,   274,     0,   275,     0,
     285,     0,   289,   263,   264,   251,   256,   257,   258,   215,
       0,     0,     0,   260,     0,     0,     0,     0,     0,   211,
     210,   216,   246,   247,   248,   249,   250,   259,   265,     0,
       0,    31,   346,    27,    62,    55,     0,    57,    54,    55,
      57,     0,   123,   133,     0,     0,   132,   135,   121,     0,
      25,   346,     0,   299,     0,   280,     0,   299,     0,   299,
     268,   269,     0,     0,   252,     0,   255,     0,     0,     0,
     234,   238,   224,     0,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   346,   346,    39,
      65,    57,    70,     0,     0,    56,    57,    59,     0,   128,
       0,     0,   136,   346,   204,   134,    24,     0,   206,   295,
       0,   313,   277,     0,     0,   279,   282,   287,   313,   291,
     313,     0,     0,   266,   267,   242,     0,   214,     0,   253,
     254,     0,   261,   217,    36,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   235,   236,   237,   239,   240,     0,
     218,   219,   220,   221,   222,   223,     0,    28,    38,    68,
      63,    64,     0,     0,    73,    88,   346,    40,    59,     0,
      61,    58,   125,   131,   124,   127,     0,   346,     0,   141,
     208,   346,   299,   297,     0,     0,   280,   283,     0,     0,
     281,   299,     0,   299,     0,     0,   243,     0,   213,   262,
       0,    33,     0,    67,   346,    79,     0,    71,    72,     0,
      91,   346,    44,    61,   104,     0,    53,    60,   128,     0,
     130,   205,   138,     0,   137,   140,     0,   207,   313,   301,
     311,     0,     0,     0,   276,   313,     0,   313,     0,     0,
       0,   241,    32,    66,    41,     0,    81,    74,    78,     0,
       0,    89,    90,    43,    53,   107,     0,   161,     0,     0,
      52,   126,   129,   141,     0,     0,   143,     0,     0,   298,
     300,   303,   315,   294,     0,   284,     0,   286,     0,   290,
       0,   244,   212,    82,    80,    85,    86,    87,     0,     0,
      99,    93,     0,   103,    95,     0,     0,     0,   105,   106,
     109,   159,   164,     0,   346,    45,   139,   144,   142,     0,
       0,   302,   322,   329,   330,     0,   324,   325,   326,   327,
     328,   312,   314,   317,     0,   278,     0,     0,     0,    83,
      84,    75,    77,     0,    92,     0,    97,     0,    94,     0,
     101,   346,     0,   108,   164,   169,   192,   162,   163,   349,
     167,   168,     0,     0,   186,     0,   346,    49,   296,   306,
     307,   305,   309,   310,   308,     0,   323,     0,   316,   318,
     288,   292,   245,    98,    96,   102,   100,    46,   346,   160,
       0,     0,   191,   347,   166,     0,     0,     0,     0,    69,
      48,   304,   320,   333,     0,   346,   110,     0,     0,   173,
       0,   165,   200,   176,     0,   194,     0,   176,     0,   190,
     185,   333,     0,   335,   346,     0,   112,   120,     0,   170,
       0,   209,   199,   202,     0,   178,     0,     0,   178,     0,
     189,   335,     0,   332,     0,   337,   114,   346,   120,   133,
       0,     0,   172,   201,   203,   175,     0,   183,   198,   193,
     200,   176,   181,   176,   337,   331,     0,     0,     0,   120,
     116,   133,   111,   146,     0,     0,   349,   177,   197,   178,
     178,     0,     0,   340,   341,   338,   339,   343,   344,   342,
     336,   319,   133,   120,   113,     0,   158,   146,   174,   171,
     180,     0,   184,   182,   321,   334,   115,   133,     0,   147,
       0,   118,   145,   179,   117,     0,   152,     0,   149,   156,
     148,   151,     0,     0,   152,   155,     0,   154,   157,   150,
     153,     0,     0,     0
};

static const short yydefgoto[] =
{
     611,    13,    14,    15,    36,    66,    67,    16,    17,    18,
     110,   111,    52,    53,   113,    19,    20,    21,   257,    22,
      23,    24,   395,    56,   349,   117,   184,   260,   306,    57,
     180,   250,   251,   252,   350,   118,   254,   297,   298,   378,
     379,   299,   185,   300,   341,   342,   381,   382,   384,   385,
     261,   345,   388,   389,   390,   507,   528,   549,   573,   552,
      62,    63,    64,   189,   308,   264,   265,   266,   553,   126,
     127,   269,   353,   314,   315,   316,   576,   577,   596,   604,
     600,   601,   602,   591,   307,   434,   392,   437,   438,   439,
     440,   470,   531,   488,   489,   515,   537,   555,   441,   442,
     443,   500,   444,   493,   494,   539,   495,   496,    25,    26,
      27,   194,   512,   513,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,    28,    29,    30,    31,    32,   135,
     276,   204,   205,   206,    33,   137,   281,    34,   139,   283,
      35,   133,   272,   201,   319,   359,   360,   361,   455,   275,
     362,   411,   412,   413,   483,   501,   414,   503,   525,   548,
     570,    69,   475
};

static const short yypact[] =
{
     164,-32768,     7,   130,   136,   140,     5,   144,   148,   154,
     162,   166,   171,-32768,   164,-32768,-32768,-32768,    62,-32768,
  -32768,    -6,-32768,-32768,     0,-32768,-32768,   -14,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,   -15,   -24,-32768,   -24,
     124,-32768,-32768,   -19,   128,   133,   -36,    -4,   105,-32768,
     240,   183,   203,-32768,   146,   217,   258,-32768,   217,   258,
       3,   239,   265,-32768,   239,-32768,-32768,   -47,-32768,-32768,
  -32768,   242,   254,-32768,-32768,-32768,-32768,   257,-32768,   284,
  -32768,   286,-32768,   294,   301,   210,-32768,-32768,-32768,   268,
      10,    32,   272,   275,   252,   252,   252,   252,   246,-32768,
     592,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   261,
     282,-32768,    28,-32768,-32768,   258,   300,   379,-32768,   258,
     379,   240,-32768,   265,    39,   361,-32768,   265,-32768,   -15,
  -32768,   -11,   295,   331,   296,   359,   315,   331,   317,   331,
     127,   127,    97,   169,-32768,   381,-32768,   384,   385,   326,
  -32768,-32768,-32768,   489,   394,   252,   252,   252,   252,   252,
     252,   252,   252,   252,   252,   252,   252,   252,   252,   252,
     252,   252,   252,   252,   252,   252,   240,   -24,   -24,-32768,
     395,   379,-32768,   346,   408,-32768,   379,   424,   350,   401,
     341,   240,-32768,   -24,-32768,-32768,-32768,   403,-32768,-32768,
     354,   383,-32768,    58,   345,-32768,   359,-32768,   383,-32768,
     383,   294,   301,-32768,-32768,-32768,   293,-32768,   409,-32768,
  -32768,   340,-32768,-32768,-32768,   256,   256,   256,   256,   256,
     256,   336,   336,   336,   311,   311,   311,   212,   212,   567,
     266,   266,   352,   352,   352,   352,   351,-32768,-32768,-32768,
  -32768,-32768,    98,   348,     1,-32768,    99,-32768,   424,   363,
     435,-32768,-32768,-32768,-32768,-32768,   101,   -24,   364,   416,
  -32768,   -24,   331,-32768,   367,   357,   359,-32768,   421,   362,
  -32768,   331,   365,   331,   366,   252,-32768,   252,-32768,-32768,
     252,   183,   395,-32768,   -24,   370,   422,-32768,-32768,   132,
     230,   -24,-32768,   435,-32768,    -1,   447,-32768,   401,   401,
  -32768,-32768,-32768,   368,-32768,-32768,   161,-32768,   383,   425,
  -32768,   369,   371,   372,-32768,   383,   376,   383,   377,   453,
     515,   617,-32768,-32768,-32768,   305,-32768,-32768,     8,   437,
     438,-32768,-32768,-32768,   447,   439,   390,-32768,   400,   469,
  -32768,-32768,-32768,   416,   240,   416,-32768,   405,   406,-32768,
  -32768,   425,   178,-32768,   411,-32768,   410,-32768,   412,-32768,
     252,-32768,-32768,   463,   466,-32768,-32768,-32768,     1,     1,
     414,   230,   167,   417,   230,   170,   418,   431,-32768,-32768,
     439,-32768,    30,   474,   102,-32768,-32768,-32768,-32768,   423,
     276,-32768,   430,-32768,-32768,   262,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   178,   484,-32768,   442,   451,   541,-32768,
  -32768,-32768,-32768,   434,-32768,   437,-32768,   448,-32768,   438,
  -32768,   -24,   494,-32768,    30,-32768,    74,-32768,-32768,   174,
  -32768,-32768,   241,   449,-32768,   454,   -24,-32768,-32768,   294,
     301,-32768,-32768,-32768,-32768,   456,-32768,   516,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -16,-32768,
     517,   240,   465,-32768,   129,    30,   518,   518,   519,-32768,
  -32768,-32768,-32768,   471,   537,    -7,-32768,   477,   561,   502,
     499,-32768,    -8,   579,   507,-32768,   605,   579,   607,   528,
  -32768,   471,    22,   580,   -24,   612,-32768,   239,   240,-32768,
     517,-32768,   550,   633,   240,   660,   640,   642,   660,   642,
  -32768,   580,   594,-32768,   596,   603,-32768,   -24,   239,   265,
     682,   667,-32768,   240,-32768,-32768,   673,-32768,   528,-32768,
      78,   579,-32768,   579,   603,-32768,   675,   214,   618,   239,
  -32768,   265,-32768,   702,   240,   685,   190,-32768,-32768,   660,
     660,   620,   613,   294,   301,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,   265,   239,-32768,    40,   704,   702,-32768,-32768,
     683,   667,-32768,-32768,-32768,-32768,-32768,   265,   240,-32768,
     632,-32768,-32768,-32768,-32768,   634,   686,   684,-32768,   528,
  -32768,-32768,   202,   628,   686,-32768,   686,-32768,-32768,-32768,
  -32768,   723,   724,-32768
};

static const short yypgoto[] =
{
  -32768,   711,-32768,-32768,-32768,   597,-32768,-32768,-32768,-32768,
  -32768,   436,-32768,   574,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,    -2,   386,    35,   -58,   473,   426,-32768,
  -32768,-32768,   440,-32768,-32768,-32768,-32768,-32768,   -48,-32768,
  -32768,-32768,-32768,-32768,-32768,  -228,   308,-32768,   306,-32768,
  -32768,-32768,-32768,   344,-32768,-32768,-32768,-32768,-32768,  -406,
     -60,   672,-32768,-32768,-32768,   429,   432,-32768,   -49,   611,
  -32768,-32768,-32768,   387,   388,-32768,   165,-32768,-32768,-32768,
     135,   138,-32768,-32768,-32768,-32768,-32768,   312,   270,-32768,
  -32768,-32768,-32768,   237,-32768,  -477,  -374,   168,  -512,-32768,
  -32768,-32768,-32768,  -329,-32768,-32768,-32768,  -331,-32768,-32768,
  -32768,-32768,  -419,   -50,-32768,   -57,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,  -138,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,   472,   544,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,  -132,-32768,-32768,   391,-32768,-32768,  -198,
  -32768,-32768,   338,-32768,-32768,-32768,   349,   255,   232,   211,
  -32768,   -35,   201
};


#define	YYLAST		757


static const short yytable[] =
{
      98,   123,   213,   214,    70,   208,   484,   210,  -201,  -201,
     282,   197,   284,   125,   346,   505,    60,   472,    77,   556,
     518,    65,    59,    54,   121,    83,    84,    85,    86,    54,
      87,    88,    89,    90,    91,    72,   295,   150,   151,   152,
     153,    41,    37,   -76,   296,   144,   129,   130,   435,   485,
      79,   -76,   145,   115,    42,   522,   119,    92,    93,    78,
     191,   588,   187,   178,   559,   436,   560,   146,    94,   556,
      68,   188,    95,   147,   190,    73,    74,   179,    68,    96,
     520,    61,   471,    68,   347,   216,   218,    68,   122,    55,
     471,    80,  -196,    97,   120,    58,   198,    50,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   558,
     357,   523,    68,   253,   192,   589,   246,   366,   258,   368,
      83,    84,    85,    86,   301,    87,    88,   446,    90,    91,
     318,   268,   247,   248,   542,   574,   498,  -348,   497,   325,
     181,   327,   277,   424,   186,   278,   428,    51,   270,    81,
     211,   212,    92,    93,  -348,    38,   586,     1,     2,     3,
       4,    39,   471,    94,  -188,    40,   471,    95,  -196,    43,
     605,   594,   534,    44,    96,   582,   583,     5,   541,    45,
     543,   292,   293,    68,   309,   310,    68,    46,    97,   215,
      82,    47,    83,    84,    85,    86,    48,    87,    88,    71,
      90,    91,     6,   402,     7,     8,     9,    10,   109,    11,
      12,   302,    75,   403,   404,   337,   338,    76,   329,   112,
     330,   114,   311,   331,    92,    93,   317,   405,   406,   407,
     408,   409,   410,   339,   340,    94,    54,   563,   564,    95,
     565,   566,   567,   568,   355,   356,    96,   476,   477,   334,
     425,   426,   454,   429,   430,   116,   343,   473,   474,    60,
      97,   217,   124,    83,    84,    85,    86,   131,    87,    88,
      89,    90,    91,   473,   580,    83,    84,    85,    86,   132,
      87,    88,   134,    90,    91,   606,   607,   402,   170,   171,
     172,   173,   174,   175,   397,    92,    93,   403,   404,   449,
     450,   142,   451,   418,   452,   453,    94,    92,    93,   136,
      95,   138,   406,   407,   408,   409,   410,    96,    94,   140,
     421,   422,    95,   164,   165,   166,   141,   167,   168,    96,
     154,    97,   170,   171,   172,   173,   174,   175,   373,   374,
     375,   376,   377,    97,   172,   173,   174,   175,   176,   447,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   143,
     164,   165,   166,   148,   167,   168,   149,   169,   177,   170,
     171,   172,   173,   174,   175,   182,   285,   183,   193,   200,
     199,   202,   167,   168,   203,   286,   467,   170,   171,   172,
     173,   174,   175,   155,   156,   157,   158,   159,   160,   569,
     207,   480,   209,   164,   165,   166,   219,   167,   168,   220,
     221,   490,   170,   171,   172,   173,   174,   175,   222,    50,
     249,   255,   256,   486,   259,   262,   263,   267,   271,   273,
     274,   279,   289,   175,   294,   291,   305,   529,   304,   312,
     506,   313,   320,   321,   323,   335,   324,   336,   530,   348,
     358,   326,   328,   363,   535,   354,   365,   364,   551,   526,
     367,   369,   380,   383,   387,   391,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   393,   164,   165,   166,   572,
     167,   168,   550,   169,   394,   170,   171,   172,   173,   174,
     175,   399,   287,   400,   578,   415,   416,   419,   417,   420,
     445,   288,   423,   587,   431,   427,   432,   448,   456,   459,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   468,
     164,   165,   166,   463,   167,   168,   460,   169,   595,   170,
     171,   172,   173,   174,   175,   461,   370,   465,   479,   478,
     481,   482,   487,   492,   499,   371,   155,   156,   157,   158,
     159,   160,   161,   162,   163,  -187,   164,   165,   166,   502,
     167,   168,   504,   169,   508,   170,   171,   172,   173,   174,
     175,   509,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   223,   164,   165,   166,   510,   167,   168,   511,   169,
     514,   170,   171,   172,   173,   174,   175,   516,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   372,   164,   165,
     166,   517,   167,   168,   519,   169,   471,   170,   171,   172,
     173,   174,   175,   524,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   462,   164,   165,   166,   527,   167,   168,
    -195,   169,   290,   170,   171,   172,   173,   174,   175,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   533,   164,
     165,   166,   536,   167,   168,   538,   169,   540,   170,   171,
     172,   173,   174,   175,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   545,   164,   165,   166,   546,   167,   168,
     547,   554,   436,   170,   171,   172,   173,   174,   175,   557,
     562,   575,   571,   579,   584,   585,   590,   597,  -348,   598,
     603,   599,   608,   612,   613,    49,   196,   332,   224,   344,
     386,   303,   333,   464,   433,   466,   128,   351,   195,   609,
     396,   352,   592,   398,   610,   491,   469,   532,   322,   593,
     280,   458,   401,   544,   457,   561,   521,   581
};

static const short yycheck[] =
{
      50,    61,   140,   141,    39,   137,    22,   139,    16,    17,
     208,    22,   210,    62,    15,    22,    30,   436,    54,   531,
     497,    36,    24,    29,    21,    33,    34,    35,    36,    29,
      38,    39,    40,    41,    42,    54,    35,    94,    95,    96,
      97,    36,    35,    35,    43,    35,    93,    94,    18,   468,
      54,    43,    42,    55,    49,    33,    58,    65,    66,    95,
      21,    21,   120,    35,   541,    35,   543,    35,    76,   581,
      94,   121,    80,    41,   123,    94,    95,   112,    94,    87,
     499,    95,    98,    94,    85,   142,   143,    94,    85,    95,
      98,    95,   100,   101,    59,    95,   131,    35,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   538,
     318,    99,    94,   181,    85,    85,   176,   325,   186,   327,
      33,    34,    35,    36,    35,    38,    39,    35,    41,    42,
     272,   191,   177,   178,   518,   551,   477,    18,   477,   281,
     115,   283,    94,   381,   119,    97,   384,    95,   193,    54,
      33,    34,    65,    66,    35,    35,   572,     3,     4,     5,
       6,    35,    98,    76,   100,    35,    98,    80,   100,    35,
     599,   587,   513,    35,    87,   559,   560,    23,   517,    35,
     519,    93,    94,    94,    93,    94,    94,    35,   101,   102,
      95,    35,    33,    34,    35,    36,    35,    38,    39,    85,
      41,    42,    48,    35,    50,    51,    52,    53,    35,    55,
      56,   256,    94,    45,    46,    93,    94,    94,   285,    26,
     287,    85,   267,   290,    65,    66,   271,    59,    60,    61,
      62,    63,    64,    13,    14,    76,    29,    33,    34,    80,
      36,    37,    38,    39,    93,    94,    87,    16,    17,   294,
      93,    94,   400,    93,    94,     7,   301,    93,    94,    30,
     101,   102,     7,    33,    34,    35,    36,    35,    38,    39,
      40,    41,    42,    93,    94,    33,    34,    35,    36,    35,
      38,    39,    35,    41,    42,    93,    94,    35,    86,    87,
      88,    89,    90,    91,   354,    65,    66,    45,    46,    33,
      34,   101,    36,   370,    38,    39,    76,    65,    66,    35,
      80,    35,    60,    61,    62,    63,    64,    87,    76,    35,
     378,   379,    80,    77,    78,    79,    35,    81,    82,    87,
      94,   101,    86,    87,    88,    89,    90,    91,    43,    44,
      45,    46,    47,   101,    88,    89,    90,    91,    97,   394,
      67,    68,    69,    70,    71,    72,    73,    74,    75,   101,
      77,    78,    79,   101,    81,    82,   101,    84,    96,    86,
      87,    88,    89,    90,    91,    85,    93,     8,    27,    58,
      95,    95,    81,    82,    35,   102,   431,    86,    87,    88,
      89,    90,    91,    67,    68,    69,    70,    71,    72,   547,
      95,   446,    95,    77,    78,    79,    35,    81,    82,    35,
      35,   471,    86,    87,    88,    89,    90,    91,   102,    35,
      35,    85,    24,   468,    10,    85,    35,    96,    35,    85,
      57,    96,   102,    91,    96,    94,    11,   507,    85,    85,
     485,    35,    85,    96,    33,    85,    94,    35,   508,    12,
      35,    96,    96,    94,   514,    97,    94,    96,   528,   504,
      94,    94,    35,    35,    35,    85,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    85,    77,    78,    79,   549,
      81,    82,   527,    84,    25,    86,    87,    88,    89,    90,
      91,    96,    93,    97,   554,    94,    96,    44,    96,    43,
      36,   102,    98,   573,    96,    98,    85,    94,    88,    35,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    35,
      77,    78,    79,    99,    81,    82,    94,    84,   588,    86,
      87,    88,    89,    90,    91,    94,    93,    99,    94,   100,
      94,    35,    35,    35,    35,   102,    67,    68,    69,    70,
      71,    72,    73,    74,    75,   100,    77,    78,    79,    98,
      81,    82,    35,    84,    97,    86,    87,    88,    89,    90,
      91,    20,    67,    68,    69,    70,    71,    72,    73,    74,
      75,   102,    77,    78,    79,    93,    81,    82,    99,    84,
      21,    86,    87,    88,    89,    90,    91,   100,    67,    68,
      69,    70,    71,    72,    73,    74,    75,   102,    77,    78,
      79,    16,    81,    82,    17,    84,    98,    86,    87,    88,
      89,    90,    91,    53,    67,    68,    69,    70,    71,    72,
      73,    74,    75,   102,    77,    78,    79,    35,    81,    82,
     100,    84,    85,    86,    87,    88,    89,    90,    91,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    35,    77,
      78,    79,    12,    81,    82,    35,    84,    35,    86,    87,
      88,    89,    90,    91,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    99,    77,    78,    79,   101,    81,    82,
      97,    19,    35,    86,    87,    88,    89,    90,    91,    36,
      35,     9,    94,    28,    94,   102,    12,    85,    35,    85,
      36,    35,    94,     0,     0,    14,   129,   291,   154,   303,
     344,   258,   292,   425,   390,   429,    64,   308,   127,   604,
     353,   309,   577,   355,   606,   475,   434,   510,   276,   581,
     206,   413,   361,   521,   405,   544,   501,   556
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

case 22:
#line 251 "ned.y"
{
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                ;
    break;}
case 23:
#line 256 "ned.y"
{
                  /* no setTrailingComment(ps.imports,@3); comment already stored with last filename */
                ;
    break;}
case 26:
#line 268 "ned.y"
{
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                ;
    break;}
case 28:
#line 284 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 29:
#line 291 "ned.y"
{
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 32:
#line 305 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-4]));
                  addExpression(ps.chanattr, "value",yylsp[-2],yyvsp[-2]);
                  setComments(ps.channel,yylsp[-4],yylsp[-2]);
                ;
    break;}
case 33:
#line 311 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 36:
#line 325 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-2],yyvsp[-2]);
                  setComments(ps.channel,yylsp[-3],yylsp[-2]);
                ;
    break;}
case 37:
#line 331 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 38:
#line 340 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 39:
#line 344 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 41:
#line 363 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 42:
#line 370 "ned.y"
{
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[-1]));
                  setComments(ps.module,yylsp[-2],yylsp[0]);
                ;
    break;}
case 43:
#line 379 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 44:
#line 383 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 46:
#line 408 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 47:
#line 415 "ned.y"
{
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[-1]));
                  setComments(ps.module,yylsp[-2],yylsp[0]);
                ;
    break;}
case 48:
#line 424 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 49:
#line 428 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 62:
#line 446 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 63:
#line 451 "ned.y"
{
                ;
    break;}
case 68:
#line 467 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 69:
#line 476 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 70:
#line 483 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 71:
#line 488 "ned.y"
{
                ;
    break;}
case 74:
#line 499 "ned.y"
{
                  setComments(ps.param,yylsp[-1]);
                ;
    break;}
case 76:
#line 504 "ned.y"
{
                  setComments(ps.param,yylsp[-1]);
                ;
    break;}
case 78:
#line 509 "ned.y"
{
                  setComments(ps.param,yylsp[-1]);
                ;
    break;}
case 79:
#line 516 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                ;
    break;}
case 80:
#line 520 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                ;
    break;}
case 81:
#line 524 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                ;
    break;}
case 82:
#line 528 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                ;
    break;}
case 83:
#line 532 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 84:
#line 536 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 85:
#line 540 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                ;
    break;}
case 86:
#line 544 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                ;
    break;}
case 87:
#line 548 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                ;
    break;}
case 88:
#line 555 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 89:
#line 560 "ned.y"
{
                ;
    break;}
case 98:
#line 583 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 99:
#line 588 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 102:
#line 601 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 103:
#line 606 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 104:
#line 614 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 105:
#line 619 "ned.y"
{
                ;
    break;}
case 110:
#line 635 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 111:
#line 641 "ned.y"
{
                ;
    break;}
case 112:
#line 644 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 113:
#line 651 "ned.y"
{
                ;
    break;}
case 114:
#line 654 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-3], yylsp[-1]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 115:
#line 660 "ned.y"
{
                ;
    break;}
case 116:
#line 663 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-4], yylsp[-1]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 117:
#line 670 "ned.y"
{
                ;
    break;}
case 123:
#line 692 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 124:
#line 697 "ned.y"
{
                ;
    break;}
case 125:
#line 700 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 126:
#line 706 "ned.y"
{
                ;
    break;}
case 131:
#line 722 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 136:
#line 740 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 137:
#line 745 "ned.y"
{
                ;
    break;}
case 138:
#line 748 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 139:
#line 754 "ned.y"
{
                ;
    break;}
case 144:
#line 774 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                ;
    break;}
case 147:
#line 788 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 148:
#line 793 "ned.y"
{
                ;
    break;}
case 149:
#line 796 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 150:
#line 802 "ned.y"
{
                ;
    break;}
case 155:
#line 818 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 156:
#line 825 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 157:
#line 833 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 159:
#line 841 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "false" );
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 160:
#line 847 "ned.y"
{
                ;
    break;}
case 161:
#line 850 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "yes" );
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 162:
#line 856 "ned.y"
{
                ;
    break;}
case 169:
#line 877 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                ;
    break;}
case 170:
#line 881 "ned.y"
{
                  ps.inLoop=1;
                ;
    break;}
case 171:
#line 885 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[0]);
                ;
    break;}
case 174:
#line 899 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 175:
#line 909 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 177:
#line 917 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 181:
#line 930 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-4],yylsp[0]);
                ;
    break;}
case 182:
#line 935 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-6],yylsp[0]);
                ;
    break;}
case 183:
#line 940 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-4],yylsp[0]);
                ;
    break;}
case 184:
#line 946 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-6],yylsp[0]);
                ;
    break;}
case 187:
#line 960 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 188:
#line 966 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 189:
#line 974 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 190:
#line 979 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 191:
#line 986 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 192:
#line 993 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 195:
#line 1007 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 196:
#line 1012 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 197:
#line 1019 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 198:
#line 1024 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 199:
#line 1031 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 200:
#line 1036 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 201:
#line 1044 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 202:
#line 1049 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 203:
#line 1054 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-2]));
                  addExpression(ps.connattr, "value",yylsp[-1],yyvsp[-1]);
                ;
    break;}
case 205:
#line 1072 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 206:
#line 1080 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1],NULLPOS);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 207:
#line 1086 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-5],yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-6],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 208:
#line 1095 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 209:
#line 1103 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 210:
#line 1109 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 211:
#line 1113 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 212:
#line 1124 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 213:
#line 1126 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 214:
#line 1128 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 215:
#line 1130 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 217:
#line 1136 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 218:
#line 1139 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 219:
#line 1141 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 220:
#line 1143 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 221:
#line 1145 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 222:
#line 1147 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 223:
#line 1149 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 224:
#line 1153 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); ;
    break;}
case 225:
#line 1156 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 226:
#line 1158 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 227:
#line 1160 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 228:
#line 1162 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 229:
#line 1164 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 230:
#line 1166 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 231:
#line 1169 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 232:
#line 1171 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 233:
#line 1173 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 234:
#line 1177 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 235:
#line 1180 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 236:
#line 1182 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 237:
#line 1184 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 238:
#line 1188 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 239:
#line 1190 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 240:
#line 1192 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 241:
#line 1194 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 242:
#line 1197 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 243:
#line 1199 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 244:
#line 1201 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 245:
#line 1203 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 251:
#line 1216 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 252:
#line 1221 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                ;
    break;}
case 253:
#line 1226 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 254:
#line 1232 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 255:
#line 1238 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 256:
#line 1246 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 257:
#line 1251 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 258:
#line 1253 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 260:
#line 1262 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 261:
#line 1264 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 262:
#line 1266 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 263:
#line 1271 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 264:
#line 1273 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 265:
#line 1275 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 270:
#line 1291 "ned.y"
{
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                ;
    break;}
case 271:
#line 1297 "ned.y"
{
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                ;
    break;}
case 272:
#line 1306 "ned.y"
{
                  ps.cppstruct = (CppStructNode *)createNodeWithTag(NED_CPP_STRUCT, ps.nedfile );
                  ps.cppstruct->setName(toString(yylsp[-1]));
                  setComments(ps.cppstruct,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 273:
#line 1315 "ned.y"
{
                  ps.cppcobject = (CppCobjectNode *)createNodeWithTag(NED_CPP_COBJECT, ps.nedfile );
                  ps.cppcobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppcobject,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 274:
#line 1324 "ned.y"
{
                  ps.cppnoncobject = (CppNoncobjectNode *)createNodeWithTag(NED_CPP_NONCOBJECT, ps.nedfile );
                  ps.cppnoncobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppnoncobject,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 275:
#line 1333 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 276:
#line 1340 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 277:
#line 1344 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 278:
#line 1352 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 283:
#line 1369 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                ;
    break;}
case 284:
#line 1374 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                ;
    break;}
case 285:
#line 1383 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 286:
#line 1389 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 287:
#line 1393 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 288:
#line 1400 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 289:
#line 1407 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 290:
#line 1413 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 291:
#line 1417 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 292:
#line 1424 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 293:
#line 1431 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 294:
#line 1437 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 295:
#line 1441 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 296:
#line 1448 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 297:
#line 1455 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 304:
#line 1475 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 311:
#line 1494 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 318:
#line 1514 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                ;
    break;}
case 319:
#line 1520 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 320:
#line 1524 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsVirtual(true);
                ;
    break;}
case 321:
#line 1531 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 331:
#line 1550 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 332:
#line 1555 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 334:
#line 1563 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 336:
#line 1571 "ned.y"
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
#line 1590 "ned.y"


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

