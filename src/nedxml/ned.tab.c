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



#define	YYFINAL		615
#define	YYFLAG		-32768
#define	YYNTBASE	103

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 338 ? yytranslate[x] : 263)

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
      38,    40,    41,    46,    50,    52,    54,    58,    64,    67,
      69,    70,    76,    81,    82,    84,    89,    93,    97,   100,
     106,   114,   117,   121,   124,   133,   144,   147,   151,   154,
     156,   157,   158,   163,   166,   167,   171,   173,   175,   177,
     178,   183,   185,   186,   187,   192,   195,   196,   200,   204,
     206,   208,   212,   215,   219,   224,   229,   233,   237,   241,
     243,   244,   245,   250,   252,   253,   258,   262,   267,   271,
     275,   277,   281,   283,   287,   289,   293,   295,   297,   298,
     299,   304,   306,   307,   310,   312,   313,   321,   322,   331,
     332,   342,   343,   354,   358,   360,   361,   364,   366,   367,
     372,   373,   380,   383,   384,   388,   390,   392,   394,   395,
     398,   400,   401,   406,   407,   414,   416,   417,   420,   422,
     427,   430,   431,   432,   437,   438,   445,   448,   449,   453,
     455,   458,   460,   465,   466,   468,   469,   470,   476,   477,
     482,   484,   485,   488,   490,   492,   494,   495,   496,   505,
     509,   511,   517,   520,   521,   524,   525,   528,   530,   537,
     546,   553,   562,   566,   568,   571,   573,   576,   578,   581,
     583,   587,   589,   592,   594,   597,   599,   602,   604,   606,
     609,   613,   618,   625,   631,   639,   642,   646,   648,   650,
     657,   662,   666,   668,   670,   674,   678,   682,   686,   690,
     694,   698,   701,   705,   709,   713,   717,   721,   725,   729,
     733,   737,   740,   744,   748,   752,   755,   759,   763,   769,
     773,   778,   785,   794,   796,   798,   800,   802,   804,   806,
     809,   813,   817,   820,   822,   824,   826,   828,   830,   834,
     839,   841,   843,   845,   849,   853,   856,   859,   862,   865,
     869,   873,   877,   878,   886,   887,   897,   899,   900,   903,
     905,   908,   913,   914,   923,   924,   935,   936,   945,   946,
     957,   958,   967,   968,   979,   980,   985,   986,   988,   989,
     992,   994,   999,  1001,  1003,  1005,  1007,  1009,  1011,  1012,
    1017,  1018,  1020,  1021,  1024,  1026,  1027,  1035,  1036,  1045,
    1047,  1050,  1052,  1054,  1056,  1058,  1060,  1062,  1064,  1068,
    1071,  1072,  1077,  1078,  1081,  1082,  1084,  1086,  1088,  1090,
    1092,  1094,  1096,  1098
};
static const short yyrhs[] =
{
     104,     0,   104,   105,     0,     0,   106,     0,   111,     0,
     119,     0,   123,     0,   210,     0,   110,     0,   118,     0,
     122,     0,   209,     0,   225,     0,   226,     0,   227,     0,
     228,     0,   229,     0,   235,     0,   238,     0,   241,     0,
       0,     3,   107,   108,    93,     0,   108,    94,   109,     0,
     109,     0,    36,     0,   112,   115,   117,     0,   112,    95,
     113,    96,   262,     0,     5,    35,     0,   114,     0,     0,
     114,    35,    97,   214,    93,     0,    35,    97,   214,    93,
       0,     0,   116,     0,   116,    35,   214,    93,     0,    35,
     214,    93,     0,    26,    35,   262,     0,    26,   262,     0,
     120,   126,   134,   140,   121,     0,   120,    95,   126,   134,
     140,    96,   262,     0,     4,    35,     0,    24,    35,   262,
       0,    24,   262,     0,   124,   126,   134,   140,   149,   184,
     132,   125,     0,   124,    95,   126,   134,   140,   149,   184,
     132,    96,   262,     0,     6,    35,     0,    25,    35,   262,
       0,    25,   262,     0,   127,     0,     0,     0,    29,    85,
     128,   129,     0,   130,    93,     0,     0,   130,    94,   131,
       0,   131,     0,    35,     0,   133,     0,     0,    12,    85,
      36,    93,     0,   135,     0,     0,     0,     7,    85,   136,
     137,     0,   138,    93,     0,     0,   138,    93,   139,     0,
     138,    94,   139,     0,   139,     0,    35,     0,    35,    85,
      44,     0,    43,    35,     0,    35,    85,    43,     0,    35,
      85,    43,    44,     0,    35,    85,    44,    43,     0,    35,
      85,    45,     0,    35,    85,    46,     0,    35,    85,    47,
       0,   141,     0,     0,     0,     8,    85,   142,   143,     0,
     144,     0,     0,   144,    13,   145,    93,     0,    13,   145,
      93,     0,   144,    14,   147,    93,     0,    14,   147,    93,
       0,   145,    94,   146,     0,   146,     0,    35,    98,    99,
       0,    35,     0,   147,    94,   148,     0,   148,     0,    35,
      98,    99,     0,    35,     0,   150,     0,     0,     0,    10,
      85,   151,   152,     0,   153,     0,     0,   153,   154,     0,
     154,     0,     0,    35,    85,    35,   262,   155,   160,   159,
       0,     0,    35,    85,    35,   213,   262,   156,   160,   159,
       0,     0,    35,    85,    35,    22,    35,   262,   157,   160,
     159,     0,     0,    35,    85,    35,   213,    22,    35,   262,
     158,   160,   159,     0,   168,   176,   183,     0,   161,     0,
       0,   161,   162,     0,   162,     0,     0,    30,    85,   163,
     165,     0,     0,    30,    21,   214,    85,   164,   165,     0,
     166,    93,     0,     0,   166,    94,   167,     0,   167,     0,
      35,     0,   169,     0,     0,   169,   170,     0,   170,     0,
       0,     7,    85,   171,   173,     0,     0,     7,    21,   214,
      85,   172,   173,     0,   174,     0,     0,   174,   175,     0,
     175,     0,    35,    97,   214,    93,     0,   176,   177,     0,
       0,     0,     9,    85,   178,   180,     0,     0,     9,    21,
     214,    85,   179,   180,     0,   181,    93,     0,     0,   181,
      94,   182,     0,   182,     0,    35,   213,     0,    35,     0,
      12,    85,    36,    93,     0,     0,   185,     0,     0,     0,
      11,    15,    85,   186,   188,     0,     0,    11,    85,   187,
     188,     0,   189,     0,     0,   189,   190,     0,   190,     0,
     191,     0,   199,     0,     0,     0,    18,   192,   194,    20,
     193,   198,    28,    93,     0,   195,    94,   194,     0,   195,
       0,    35,    97,   214,    19,   214,     0,    21,   214,     0,
       0,    12,    36,     0,     0,   198,   199,     0,   199,     0,
     200,    17,   204,   196,   197,    93,     0,   200,    17,   208,
      17,   204,   196,   197,    93,     0,   200,    16,   204,   196,
     197,    93,     0,   200,    16,   208,    16,   204,   196,   197,
      93,     0,   201,   100,   202,     0,   203,     0,    35,   213,
       0,    35,     0,    35,   213,     0,    35,     0,    35,   213,
       0,    35,     0,   205,   100,   206,     0,   207,     0,    35,
     213,     0,    35,     0,    35,   213,     0,    35,     0,    35,
     213,     0,    35,     0,    35,     0,    35,   214,     0,   208,
      35,   214,     0,   211,   160,   168,   212,     0,   211,    95,
     160,   168,    96,   262,     0,    23,    35,    85,    35,   262,
       0,    23,    35,    85,    35,    22,    35,   262,     0,    27,
     262,     0,    98,   214,    99,     0,   216,     0,   215,     0,
      40,   101,   216,    94,   216,   102,     0,    40,   101,   216,
     102,     0,    40,   101,   102,     0,    40,     0,   217,     0,
     101,   216,   102,     0,   216,    86,   216,     0,   216,    87,
     216,     0,   216,    88,   216,     0,   216,    89,   216,     0,
     216,    90,   216,     0,   216,    91,   216,     0,    87,   216,
       0,   216,    67,   216,     0,   216,    68,   216,     0,   216,
      69,   216,     0,   216,    70,   216,     0,   216,    71,   216,
       0,   216,    72,   216,     0,   216,    73,   216,     0,   216,
      74,   216,     0,   216,    75,   216,     0,    76,   216,     0,
     216,    77,   216,     0,   216,    78,   216,     0,   216,    79,
     216,     0,    80,   216,     0,   216,    81,   216,     0,   216,
      82,   216,     0,   216,    84,   216,    85,   216,     0,    35,
     101,   102,     0,    35,   101,   216,   102,     0,    35,   101,
     216,    94,   216,   102,     0,    35,   101,   216,    94,   216,
      94,   216,   102,     0,   218,     0,   219,     0,   220,     0,
     221,     0,   222,     0,    35,     0,    41,    35,     0,    41,
      42,    35,     0,    42,    41,    35,     0,    42,    35,     0,
      36,     0,    38,     0,    39,     0,   223,     0,    66,     0,
      66,   101,   102,     0,    65,   101,    35,   102,     0,    33,
       0,    34,     0,   224,     0,   224,    33,    35,     0,   224,
      34,    35,     0,    33,    35,     0,    34,    35,     0,    48,
      36,     0,    48,    49,     0,    50,    35,    93,     0,    51,
      35,    93,     0,    52,    35,    93,     0,     0,    53,    35,
      95,   230,   232,    96,    93,     0,     0,    53,    35,    54,
      35,    95,   231,   232,    96,    93,     0,   233,     0,     0,
     233,   234,     0,   234,     0,    35,    93,     0,    35,    97,
      33,    93,     0,     0,    55,    35,    95,   236,   244,   250,
      96,    93,     0,     0,    55,    35,    54,    35,    95,   237,
     244,   250,    96,    93,     0,     0,    56,    35,    95,   239,
     244,   250,    96,    93,     0,     0,    56,    35,    54,    35,
      95,   240,   244,   250,    96,    93,     0,     0,    50,    35,
      95,   242,   244,   250,    96,    93,     0,     0,    50,    35,
      54,    35,    95,   243,   244,   250,    96,    93,     0,     0,
      58,    85,   245,   246,     0,     0,   247,     0,     0,   247,
     248,     0,   248,     0,    35,    97,   249,    93,     0,    36,
       0,    33,     0,    34,     0,   224,     0,    38,     0,    39,
       0,     0,    57,    85,   251,   252,     0,     0,   253,     0,
       0,   253,   254,     0,   254,     0,     0,   257,    35,   255,
     258,   259,   260,    93,     0,     0,    59,   257,    35,   256,
     258,   259,   260,    93,     0,    35,     0,    35,    88,     0,
      60,     0,    61,     0,    62,     0,    63,     0,    64,     0,
      45,     0,    46,     0,    98,    33,    99,     0,    98,    99,
       0,     0,    53,   101,    35,   102,     0,     0,    97,   261,
       0,     0,    36,     0,    37,     0,    33,     0,    34,     0,
     224,     0,    38,     0,    39,     0,    93,     0,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   227,   231,   232,   236,   237,   238,   239,   240,   242,
     243,   244,   245,   247,   248,   249,   250,   251,   252,   253,
     254,   261,   261,   273,   274,   278,   290,   294,   303,   312,
     313,   317,   323,   332,   333,   337,   343,   352,   356,   363,
     371,   382,   391,   395,   402,   413,   427,   436,   440,   447,
     448,   452,   452,   463,   464,   468,   469,   473,   482,   483,
     487,   494,   495,   499,   499,   510,   511,   515,   519,   523,
     530,   534,   538,   542,   546,   550,   554,   558,   562,   569,
     570,   574,   574,   585,   586,   590,   591,   592,   593,   597,
     598,   602,   607,   615,   616,   620,   625,   633,   634,   638,
     638,   649,   650,   654,   655,   659,   659,   668,   668,   678,
     678,   687,   687,   700,   706,   707,   711,   712,   716,   716,
     724,   724,   736,   737,   741,   742,   746,   754,   755,   759,
     760,   764,   764,   772,   772,   785,   786,   790,   791,   795,
     804,   805,   809,   809,   817,   817,   829,   830,   834,   835,
     839,   846,   854,   858,   862,   863,   867,   867,   876,   876,
     891,   892,   896,   897,   901,   902,   906,   906,   906,   923,
     924,   928,   938,   942,   946,   950,   954,   955,   959,   964,
     969,   975,   984,   985,   989,   995,  1003,  1008,  1015,  1022,
    1031,  1032,  1036,  1041,  1048,  1053,  1060,  1065,  1073,  1078,
    1083,  1091,  1098,  1109,  1115,  1124,  1132,  1137,  1142,  1153,
    1155,  1157,  1159,  1164,  1165,  1168,  1170,  1172,  1174,  1176,
    1178,  1181,  1185,  1187,  1189,  1191,  1193,  1195,  1198,  1200,
    1202,  1205,  1209,  1211,  1213,  1216,  1219,  1221,  1223,  1226,
    1228,  1230,  1232,  1237,  1238,  1239,  1240,  1241,  1245,  1250,
    1255,  1261,  1267,  1275,  1280,  1282,  1287,  1291,  1293,  1295,
    1300,  1302,  1304,  1310,  1311,  1312,  1313,  1321,  1327,  1336,
    1345,  1354,  1363,  1363,  1374,  1374,  1389,  1390,  1394,  1395,
    1399,  1404,  1413,  1413,  1423,  1423,  1437,  1437,  1447,  1447,
    1461,  1461,  1471,  1471,  1485,  1485,  1491,  1495,  1496,  1500,
    1501,  1505,  1515,  1516,  1517,  1518,  1519,  1520,  1524,  1524,
    1530,  1534,  1535,  1539,  1540,  1544,  1544,  1554,  1554,  1568,
    1569,  1570,  1571,  1572,  1573,  1574,  1575,  1576,  1580,  1585,
    1589,  1593,  1597,  1601,  1605,  1609,  1610,  1611,  1612,  1613,
    1614,  1615,  1618,  1618
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
  "filename", "channeldefinition_old", "channeldefinition", 
  "channelheader", "opt_channelattrblock", "channelattrblock", 
  "opt_channelattrblock_old", "channelattrblock_old", "endchannel", 
  "simpledefinition_old", "simpledefinition", "simpleheader", "endsimple", 
  "moduledefinition_old", "moduledefinition", "moduleheader", "endmodule", 
  "opt_machineblock", "machineblock", "@2", "opt_machinelist", 
  "machinelist", "machine", "opt_displayblock", "displayblock", 
  "opt_paramblock", "paramblock", "@3", "opt_parameters", "parameters", 
  "parameter", "opt_gateblock", "gateblock", "@4", "opt_gates", "gates", 
  "gatesI", "gateI", "gatesO", "gateO", "opt_submodblock", "submodblock", 
  "@5", "opt_submodules", "submodules", "submodule", "@6", "@7", "@8", 
  "@9", "submodule_body", "opt_on_blocks", "on_blocks", "on_block", "@10", 
  "@11", "opt_on_list", "on_list", "on_mach", "opt_substparamblocks", 
  "substparamblocks", "substparamblock", "@12", "@13", 
  "opt_substparameters", "substparameters", "substparameter", 
  "opt_gatesizeblocks", "gatesizeblock", "@14", "@15", "opt_gatesizes", 
  "gatesizes", "gatesize", "opt_submod_displayblock", "opt_connblock", 
  "connblock", "@16", "@17", "opt_connections", "connections", 
  "connection", "loopconnection", "@18", "@19", "loopvarlist", "loopvar", 
  "opt_conn_condition", "opt_conn_displaystr", "notloopconnections", 
  "notloopconnection", "gate_spec_L", "mod_L", "gate_L", "parentgate_L", 
  "gate_spec_R", "mod_R", "gate_R", "parentgate_R", "channeldescr", 
  "networkdefinition_old", "networkdefinition", "networkheader", 
  "endnetwork", "vector", "expression", "inputvalue", "expr", 
  "simple_expr", "parameter_expr", "string_expr", "boolconst_expr", 
  "numconst_expr", "special_expr", "numconst", "timeconstant", 
  "cppinclude", "cppstruct", "cppcobject", "cppnoncobject", "enum", "@20", 
  "@21", "opt_enumfields", "enumfields", "enumfield", "message", "@22", 
  "@23", "class", "@24", "@25", "struct", "@26", "@27", 
  "opt_propertiesblock", "@28", "opt_properties", "properties", 
  "property", "propertyvalue", "opt_fieldsblock", "@29", "opt_fields", 
  "fields", "field", "@30", "@31", "fielddatatype", "opt_fieldvector", 
  "opt_fieldenum", "opt_fieldvalue", "fieldvalue", "opt_semicolon", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,   103,   104,   104,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   107,   106,   108,   108,   109,   110,   111,   112,   113,
     113,   114,   114,   115,   115,   116,   116,   117,   117,   118,
     119,   120,   121,   121,   122,   123,   124,   125,   125,   126,
     126,   128,   127,   129,   129,   130,   130,   131,   132,   132,
     133,   134,   134,   136,   135,   137,   137,   138,   138,   138,
     139,   139,   139,   139,   139,   139,   139,   139,   139,   140,
     140,   142,   141,   143,   143,   144,   144,   144,   144,   145,
     145,   146,   146,   147,   147,   148,   148,   149,   149,   151,
     150,   152,   152,   153,   153,   155,   154,   156,   154,   157,
     154,   158,   154,   159,   160,   160,   161,   161,   163,   162,
     164,   162,   165,   165,   166,   166,   167,   168,   168,   169,
     169,   171,   170,   172,   170,   173,   173,   174,   174,   175,
     176,   176,   178,   177,   179,   177,   180,   180,   181,   181,
     182,   182,   183,   183,   184,   184,   186,   185,   187,   185,
     188,   188,   189,   189,   190,   190,   192,   193,   191,   194,
     194,   195,   196,   196,   197,   197,   198,   198,   199,   199,
     199,   199,   200,   200,   201,   201,   202,   202,   203,   203,
     204,   204,   205,   205,   206,   206,   207,   207,   208,   208,
     208,   209,   210,   211,   211,   212,   213,   214,   214,   215,
     215,   215,   215,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   217,   217,   217,   217,   217,   218,   218,
     218,   218,   218,   219,   220,   220,   221,   222,   222,   222,
     223,   223,   223,   224,   224,   224,   224,   225,   225,   226,
     227,   228,   230,   229,   231,   229,   232,   232,   233,   233,
     234,   234,   236,   235,   237,   235,   239,   238,   240,   238,
     242,   241,   243,   241,   245,   244,   244,   246,   246,   247,
     247,   248,   249,   249,   249,   249,   249,   249,   251,   250,
     250,   252,   252,   253,   253,   255,   254,   256,   254,   257,
     257,   257,   257,   257,   257,   257,   257,   257,   258,   258,
     258,   259,   259,   260,   260,   261,   261,   261,   261,   261,
     261,   261,   262,   262
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     2,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     4,     3,     1,     1,     3,     5,     2,     1,
       0,     5,     4,     0,     1,     4,     3,     3,     2,     5,
       7,     2,     3,     2,     8,    10,     2,     3,     2,     1,
       0,     0,     4,     2,     0,     3,     1,     1,     1,     0,
       4,     1,     0,     0,     4,     2,     0,     3,     3,     1,
       1,     3,     2,     3,     4,     4,     3,     3,     3,     1,
       0,     0,     4,     1,     0,     4,     3,     4,     3,     3,
       1,     3,     1,     3,     1,     3,     1,     1,     0,     0,
       4,     1,     0,     2,     1,     0,     7,     0,     8,     0,
       9,     0,    10,     3,     1,     0,     2,     1,     0,     4,
       0,     6,     2,     0,     3,     1,     1,     1,     0,     2,
       1,     0,     4,     0,     6,     1,     0,     2,     1,     4,
       2,     0,     0,     4,     0,     6,     2,     0,     3,     1,
       2,     1,     4,     0,     1,     0,     0,     5,     0,     4,
       1,     0,     2,     1,     1,     1,     0,     0,     8,     3,
       1,     5,     2,     0,     2,     0,     2,     1,     6,     8,
       6,     8,     3,     1,     2,     1,     2,     1,     2,     1,
       3,     1,     2,     1,     2,     1,     2,     1,     1,     2,
       3,     4,     6,     5,     7,     2,     3,     1,     1,     6,
       4,     3,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     3,     3,     2,     3,     3,     5,     3,
       4,     6,     8,     1,     1,     1,     1,     1,     1,     2,
       3,     3,     2,     1,     1,     1,     1,     1,     3,     4,
       1,     1,     1,     3,     3,     2,     2,     2,     2,     3,
       3,     3,     0,     7,     0,     9,     1,     0,     2,     1,
       2,     4,     0,     8,     0,    10,     0,     8,     0,    10,
       0,     8,     0,    10,     0,     4,     0,     1,     0,     2,
       1,     4,     1,     1,     1,     1,     1,     1,     0,     4,
       0,     1,     0,     2,     1,     0,     7,     0,     8,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       0,     4,     0,     2,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       3,     1,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     2,     4,     9,     5,    33,    10,
       6,    50,    11,     7,    50,    12,     8,   115,    13,    14,
      15,    16,    17,    18,    19,    20,     0,    41,    28,    46,
       0,   267,   268,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    34,     0,    50,    62,    49,    50,    62,     0,
     115,   128,   114,   117,    25,     0,    24,     0,     0,   269,
     290,   270,   271,     0,   272,     0,   282,     0,   286,   260,
     261,   248,   253,   254,   255,   212,     0,     0,     0,   257,
       0,     0,     0,     0,     0,   208,   207,   213,   243,   244,
     245,   246,   247,   256,   262,     0,     0,    29,   343,    26,
       0,    51,    62,     0,    80,    61,    62,    80,     0,   118,
     128,     0,     0,   127,   130,   116,    22,     0,   343,     0,
     296,     0,   277,     0,   296,     0,   296,   265,   266,     0,
       0,   249,     0,   252,     0,     0,     0,   231,   235,   221,
       0,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   343,     0,   343,   342,
      38,     0,    54,    80,    63,     0,     0,    79,    80,    98,
       0,   123,     0,     0,   131,   343,   201,   129,    23,     0,
     203,   292,     0,   310,   274,     0,     0,   276,   279,   284,
     310,   288,   310,   239,     0,   211,     0,   250,   251,     0,
     258,   214,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   232,   233,   234,   236,   237,     0,   215,   216,   217,
     218,   219,   220,   263,   264,     0,    27,     0,    37,    35,
      57,    52,     0,    56,     0,    66,    81,   343,    39,    98,
       0,   155,    97,   120,   126,   119,     0,   125,   343,     0,
     136,   205,   343,   296,   294,     0,     0,   277,   280,     0,
       0,   278,   296,     0,   296,     0,     0,   240,     0,   210,
     259,     0,    32,     0,    53,     0,   343,    70,     0,    64,
       0,    69,    84,   343,    43,   155,    99,     0,    59,   154,
     123,   122,     0,   202,   133,     0,   132,   135,   138,   204,
     310,   298,   308,     0,     0,     0,   273,   310,     0,   310,
       0,     0,     0,   238,    31,    55,    40,     0,    72,    65,
       0,     0,     0,    82,    83,    42,    59,   102,     0,   158,
       0,     0,    58,   121,   124,   136,     0,   137,     0,     0,
     295,   297,   300,   312,   291,     0,   281,     0,   283,     0,
     287,     0,   241,   209,    73,    71,    76,    77,    78,    67,
      68,    92,     0,    90,    96,     0,    94,     0,     0,     0,
       0,   100,   101,   104,   156,   161,     0,   343,    44,   134,
       0,     0,     0,   299,   319,   326,   327,     0,   321,   322,
     323,   324,   325,   309,   311,   314,     0,   275,     0,     0,
       0,    74,    75,     0,    86,     0,     0,    88,     0,     0,
       0,   343,     0,   103,   161,   166,   189,   159,   160,   163,
     164,   165,     0,     0,   183,     0,   343,    48,   139,   293,
     303,   304,   302,   306,   307,   305,     0,   320,     0,   313,
     315,   285,   289,   242,    91,    89,    95,    93,    85,    87,
      45,   343,   157,     0,     0,   188,   162,     0,     0,     0,
      60,    47,   301,   317,   330,     0,   343,   105,     0,     0,
     170,     0,   197,   173,     0,   191,     0,   173,     0,   187,
     182,   330,     0,   332,   343,     0,   107,   115,     0,   167,
       0,   206,   196,   199,     0,   175,     0,     0,     0,   175,
       0,   186,   332,     0,   329,     0,   334,   109,   343,   115,
     128,     0,     0,   169,   172,     0,     0,   195,   190,   197,
     173,   200,     0,   173,   334,   328,     0,     0,     0,   115,
     111,   128,   106,   141,     0,     0,   177,   174,   180,   194,
     175,   178,   175,     0,     0,   337,   338,   335,   336,   340,
     341,   339,   333,   316,   128,   115,   108,   153,   171,     0,
     176,     0,     0,   318,   331,   110,   128,     0,     0,   140,
     113,   168,   181,   179,   112,     0,   142,     0,     0,   147,
       0,   144,   151,   143,     0,   149,   152,   147,   150,   146,
       0,   145,   148,     0,     0,     0
};

static const short yydefgoto[] =
{
     613,     1,    14,    15,    36,    65,    66,    16,    17,    18,
     106,   107,    51,    52,   109,    19,    20,    21,   258,    22,
      23,    24,   398,    55,    56,   182,   251,   252,   253,   351,
     352,   114,   115,   255,   299,   300,   301,   186,   187,   302,
     343,   344,   382,   383,   385,   386,   261,   262,   347,   391,
     392,   393,   507,   529,   549,   575,   552,    61,    62,    63,
     191,   310,   265,   266,   267,   553,   123,   124,   270,   355,
     316,   317,   318,   577,   589,   599,   607,   603,   604,   605,
     590,   308,   309,   434,   395,   437,   438,   439,   440,   473,
     532,   489,   490,   515,   536,   555,   441,   442,   443,   500,
     444,   493,   494,   538,   495,   496,    25,    26,    27,   196,
     512,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,    28,    29,    30,    31,    32,   132,   277,   206,
     207,   208,    33,   134,   282,    34,   136,   284,    35,   130,
     273,   203,   321,   360,   361,   362,   456,   276,   363,   413,
     414,   415,   484,   501,   416,   503,   526,   548,   572,   180
};

static const short yypact[] =
{
  -32768,   276,-32768,    -3,    29,    45,   105,    -6,   137,   148,
     153,   156,   166,   169,-32768,-32768,-32768,-32768,    26,-32768,
  -32768,    18,-32768,-32768,    19,-32768,-32768,    21,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,   101,-32768,-32768,-32768,
     -26,-32768,-32768,    91,   139,   144,   -25,   -21,   -16,   -20,
     204,   216,   215,   174,   233,   263,-32768,   233,   263,    -9,
     244,   282,   244,-32768,-32768,   129,-32768,   248,   268,-32768,
  -32768,-32768,-32768,   275,-32768,   290,-32768,   295,-32768,   309,
     311,   255,-32768,-32768,-32768,   262,     7,    -7,   296,   297,
     222,   222,   222,   222,   288,-32768,   526,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,   192,   303,   300,   366,    22,-32768,
     -20,-32768,   263,   318,   396,-32768,   263,   396,   -20,-32768,
     282,    32,   379,   282,-32768,-32768,-32768,   101,    14,   312,
     350,   315,   385,   329,   350,   332,   350,-32768,-32768,   127,
     210,-32768,   394,-32768,   401,   402,   337,-32768,-32768,-32768,
     423,-32768,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   405,   406,   -20,   349,   346,   349,-32768,
  -32768,   351,   413,   396,-32768,   364,   426,-32768,   396,   441,
     367,   427,   370,   -20,-32768,   349,-32768,-32768,-32768,   436,
  -32768,-32768,   393,   422,-32768,    39,   387,   385,-32768,-32768,
     422,-32768,   422,-32768,   280,-32768,   344,-32768,-32768,   380,
  -32768,-32768,   298,   298,   298,   298,   298,   298,   576,   576,
     576,   108,   108,   108,   252,   252,   501,   120,   120,   390,
     390,   390,   390,-32768,-32768,   391,-32768,   -20,-32768,-32768,
  -32768,-32768,   160,-32768,   389,    -8,-32768,    27,-32768,   441,
     414,   476,-32768,-32768,-32768,-32768,   172,-32768,   349,   418,
     471,-32768,   349,   350,-32768,   444,   412,   385,-32768,   499,
     448,-32768,   350,   438,   350,   459,   222,-32768,   222,-32768,
  -32768,   222,-32768,   465,-32768,   413,   349,   482,   525,-32768,
     175,-32768,   259,   349,-32768,   476,-32768,    24,   569,-32768,
     427,-32768,   427,-32768,-32768,   487,-32768,   471,-32768,-32768,
     422,   567,-32768,   513,   515,   516,-32768,   422,   534,   422,
     538,   386,   449,   551,-32768,-32768,-32768,   270,-32768,    -8,
      -8,   599,   600,-32768,   271,-32768,   569,   601,   564,-32768,
     565,   626,-32768,-32768,-32768,   471,   -20,-32768,   556,   559,
  -32768,   567,-32768,   232,-32768,   566,-32768,   572,-32768,   573,
  -32768,   222,-32768,-32768,   616,   618,-32768,-32768,-32768,-32768,
  -32768,   574,   207,-32768,   575,   213,-32768,   599,   600,   578,
     585,-32768,   601,-32768,-32768,    23,   635,    30,-32768,-32768,
     582,   583,   179,-32768,   589,-32768,-32768,   273,-32768,-32768,
  -32768,-32768,-32768,-32768,   232,-32768,   643,-32768,   586,   587,
     475,-32768,-32768,   584,-32768,   599,   588,-32768,   600,   228,
     279,   349,   646,-32768,    23,-32768,    41,-32768,    23,-32768,
  -32768,-32768,   374,   590,-32768,   591,   349,-32768,-32768,-32768,
     309,   311,-32768,-32768,-32768,   192,   592,-32768,   647,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,   -15,-32768,   651,   -20,   593,-32768,   653,   653,   654,
  -32768,-32768,-32768,-32768,   594,   656,    15,-32768,   597,   675,
     602,   598,   140,   677,   603,-32768,   114,   677,   116,   604,
  -32768,   594,    11,   648,   349,   664,-32768,   244,   -20,-32768,
     651,-32768,   605,-32768,   -20,   688,   669,   671,   -20,   688,
     671,-32768,   648,   608,-32768,   607,   612,-32768,   349,   244,
     282,   691,   676,-32768,-32768,   678,   619,   604,-32768,   102,
     677,-32768,   620,   677,   612,-32768,   680,   197,   623,   244,
  -32768,   282,-32768,-32768,   -20,   100,-32768,-32768,-32768,-32768,
     688,-32768,   688,   624,   617,   309,   311,-32768,-32768,-32768,
  -32768,   192,-32768,-32768,   282,   244,-32768,   117,-32768,   625,
  -32768,   627,   628,-32768,-32768,-32768,   282,    34,   637,-32768,
  -32768,-32768,-32768,-32768,-32768,   -20,-32768,   687,   639,   690,
     633,-32768,   604,-32768,   299,-32768,-32768,   690,-32768,-32768,
     690,-32768,-32768,   727,   728,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,-32768,-32768,   606,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   110,-32768,-32768,-32768,-32768,   434,   384,
  -32768,   -41,-32768,-32768,-32768,-32768,    55,  -106,-32768,-32768,
  -32768,-32768,   345,   306,   347,   308,   478,-32768,-32768,-32768,
  -32768,   342,-32768,-32768,-32768,-32768,  -427,   -60,-32768,   679,
  -32768,-32768,   428,-32768,   430,     2,-32768,   621,-32768,-32768,
     388,-32768,   429,-32768,-32768,-32768,-32768,   132,-32768,   130,
  -32768,   440,-32768,-32768,-32768,   313,-32768,   310,-32768,-32768,
  -32768,   239,-32768,  -385,  -375,-32768,  -501,-32768,-32768,-32768,
  -32768,  -367,-32768,-32768,-32768,   272,-32768,-32768,-32768,-32768,
    -431,  -109,-32768,   -67,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,  -399,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   474,
  -32768,   545,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,  -130,-32768,-32768,-32768,   392,-32768,  -202,-32768,-32768,
  -32768,   340,-32768,-32768,   348,   256,   234,   214,-32768,  -126
};


#define	YYLAST		758


static const short yytable[] =
{
     120,   181,   200,   455,   210,   475,   212,   485,   283,   190,
     285,   189,   118,    79,    80,    81,    82,   117,    83,    84,
      85,    86,    87,   147,   148,   149,   150,   297,   143,    73,
      41,   556,    37,    75,   144,   298,   199,   505,    77,   348,
     486,   435,   141,    42,   523,    88,    89,    53,    53,   142,
     246,    59,   248,   193,   580,   595,    90,   178,   436,    67,
      91,    49,   303,   122,    38,   446,   245,    92,   521,   271,
      74,   183,   214,   216,    76,   188,   119,   254,   179,    78,
      39,    93,   259,   474,   269,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   559,   179,   179,   349,
     524,   497,   519,    54,    57,   179,    60,   194,   358,   596,
     179,    50,   192,   179,   576,   367,   587,   369,   579,   588,
     517,   304,   278,   520,    58,   436,   279,    64,   293,   474,
      40,  -185,   313,   320,   542,    68,   319,   585,   571,   518,
     540,   518,   327,   543,   329,   560,  -198,  -198,   562,   594,
      79,    80,    81,    82,   112,    83,    84,   116,    86,    87,
     336,   608,    43,    79,    80,    81,    82,   345,    83,    84,
      85,    86,    87,    44,    69,   581,    70,   582,    45,   164,
     165,    46,    88,    89,   167,   168,   169,   170,   171,   172,
     474,    47,  -193,    90,    48,    88,    89,    91,   169,   170,
     171,   172,   450,   451,    92,   452,    90,   453,   454,   331,
      91,   332,   126,   127,   333,   173,   174,    92,    93,   213,
     565,   566,    71,   567,   568,   569,   570,    72,   474,   105,
    -193,    93,   108,    79,    80,    81,    82,   400,    83,    84,
     110,    86,    87,   294,   295,    79,    80,    81,    82,   111,
      83,    84,    53,    86,    87,   311,   312,   404,   339,   340,
     113,   447,   341,   342,    59,    88,    89,   405,   406,     2,
       3,     4,     5,   128,   387,   388,    90,    88,    89,   121,
      91,   407,   408,   409,   410,   411,   412,    92,    90,     6,
     424,   425,    91,   129,   420,   470,   427,   428,   404,    92,
     131,    93,   215,   374,   375,   376,   377,   378,   405,   406,
     481,   468,   425,    93,     7,   133,     8,     9,    10,    11,
     135,    12,    13,   408,   409,   410,   411,   412,   167,   168,
     169,   170,   171,   172,   137,   487,   138,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   139,   161,   162,   163,
     506,   164,   165,   140,   166,   491,   167,   168,   169,   170,
     171,   172,   469,   428,   286,   161,   162,   163,   527,   164,
     165,   151,   287,   513,   167,   168,   169,   170,   171,   172,
     477,   478,   609,   610,   379,   380,   176,   145,   146,   531,
     175,   177,   550,   184,   185,   534,   195,   201,   202,   541,
     204,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     205,   161,   162,   163,   209,   164,   165,   211,   166,   217,
     167,   168,   169,   170,   171,   172,   218,   219,   288,   220,
     243,   244,   179,   247,   249,   578,   289,   530,   250,   256,
     257,   260,   263,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   264,   161,   162,   163,   268,   164,   165,   551,
     166,   272,   167,   168,   169,   170,   171,   172,   274,   275,
     371,   172,   290,   280,   292,   296,   598,   307,   372,   574,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   306,
     161,   162,   163,   314,   164,   165,   315,   166,   323,   167,
     168,   169,   170,   171,   172,   586,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   221,   161,   162,   163,   322,
     164,   165,   325,   166,   328,   167,   168,   169,   170,   171,
     172,   326,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   373,   161,   162,   163,   330,   164,   165,   334,   166,
     338,   167,   168,   169,   170,   171,   172,   337,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   463,   161,   162,
     163,   350,   164,   165,   356,   166,   291,   167,   168,   169,
     170,   171,   172,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   359,   161,   162,   163,   364,   164,   165,   366,
     166,   365,   167,   168,   169,   170,   171,   172,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   368,   161,   162,
     163,   370,   164,   165,   381,   384,   390,   167,   168,   169,
     170,   171,   172,   152,   153,   154,   155,   156,   157,   394,
     396,   397,   401,   161,   162,   163,   402,   164,   165,   417,
     421,   422,   167,   168,   169,   170,   171,   172,   418,   419,
     432,   445,   423,   426,   431,   448,   449,   457,   460,   461,
     462,   471,   483,   464,   480,   482,   488,   466,   492,   499,
     479,   504,   502,  -184,   508,   509,   510,   511,   514,   528,
     535,   525,   474,   516,   537,  -192,   539,   545,   546,   547,
     554,   436,   558,   561,   557,   564,   573,   583,   591,   584,
     592,   593,   597,   600,   601,   602,   606,   614,   615,   335,
     389,   465,   429,   198,   433,   430,   467,   305,   353,   611,
     612,   125,   354,   399,   197,   346,   357,   472,   476,   533,
     498,   324,   281,   403,   459,   458,   544,   522,   563
};

static const short yycheck[] =
{
      60,   110,   128,   402,   134,   436,   136,    22,   210,   118,
     212,   117,    21,    33,    34,    35,    36,    58,    38,    39,
      40,    41,    42,    90,    91,    92,    93,    35,    35,    54,
      36,   532,    35,    54,    41,    43,    22,    22,    54,    15,
     471,    18,    35,    49,    33,    65,    66,    29,    29,    42,
     176,    30,   178,    21,   555,    21,    76,    35,    35,    85,
      80,    35,    35,    61,    35,    35,   175,    87,   499,   195,
      95,   112,   139,   140,    95,   116,    85,   183,    93,    95,
      35,   101,   188,    98,   193,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   537,    93,    93,    85,
      99,   478,   497,    95,    95,    93,    95,    85,   320,    85,
      93,    95,   120,    93,   551,   327,     9,   329,    28,    12,
      16,   257,    93,    17,    24,    35,    97,    36,   247,    98,
      35,   100,   268,   273,   519,    54,   272,   574,   547,    35,
     517,    35,   282,   520,   284,   540,    16,    17,   543,   586,
      33,    34,    35,    36,    54,    38,    39,    57,    41,    42,
     296,   602,    35,    33,    34,    35,    36,   303,    38,    39,
      40,    41,    42,    35,    93,   560,    95,   562,    35,    81,
      82,    35,    65,    66,    86,    87,    88,    89,    90,    91,
      98,    35,   100,    76,    35,    65,    66,    80,    88,    89,
      90,    91,    33,    34,    87,    36,    76,    38,    39,   286,
      80,   288,    93,    94,   291,    33,    34,    87,   101,   102,
      33,    34,    93,    36,    37,    38,    39,    93,    98,    35,
     100,   101,    26,    33,    34,    35,    36,   356,    38,    39,
      35,    41,    42,    93,    94,    33,    34,    35,    36,    85,
      38,    39,    29,    41,    42,    93,    94,    35,    93,    94,
       7,   397,    13,    14,    30,    65,    66,    45,    46,     3,
       4,     5,     6,    35,    13,    14,    76,    65,    66,     7,
      80,    59,    60,    61,    62,    63,    64,    87,    76,    23,
      93,    94,    80,    35,   371,   431,    93,    94,    35,    87,
      35,   101,   102,    43,    44,    45,    46,    47,    45,    46,
     446,    93,    94,   101,    48,    35,    50,    51,    52,    53,
      35,    55,    56,    60,    61,    62,    63,    64,    86,    87,
      88,    89,    90,    91,    35,   471,    35,    67,    68,    69,
      70,    71,    72,    73,    74,    75,   101,    77,    78,    79,
     486,    81,    82,   101,    84,   474,    86,    87,    88,    89,
      90,    91,    93,    94,    94,    77,    78,    79,   504,    81,
      82,    93,   102,   492,    86,    87,    88,    89,    90,    91,
      16,    17,    93,    94,   339,   340,    96,   101,   101,   508,
      97,    35,   528,    85,     8,   514,    27,    95,    58,   518,
      95,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      35,    77,    78,    79,    95,    81,    82,    95,    84,    35,
      86,    87,    88,    89,    90,    91,    35,    35,    94,   102,
      35,    35,    93,    97,    93,   554,   102,   507,    35,    85,
      24,    10,    85,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    35,    77,    78,    79,    96,    81,    82,   529,
      84,    35,    86,    87,    88,    89,    90,    91,    85,    57,
      94,    91,   102,    96,    93,    96,   595,    11,   102,   549,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    85,
      77,    78,    79,    85,    81,    82,    35,    84,    96,    86,
      87,    88,    89,    90,    91,   575,    67,    68,    69,    70,
      71,    72,    73,    74,    75,   102,    77,    78,    79,    85,
      81,    82,    33,    84,    96,    86,    87,    88,    89,    90,
      91,    93,    67,    68,    69,    70,    71,    72,    73,    74,
      75,   102,    77,    78,    79,    96,    81,    82,    93,    84,
      35,    86,    87,    88,    89,    90,    91,    85,    67,    68,
      69,    70,    71,    72,    73,    74,    75,   102,    77,    78,
      79,    12,    81,    82,    97,    84,    85,    86,    87,    88,
      89,    90,    91,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    35,    77,    78,    79,    93,    81,    82,    93,
      84,    96,    86,    87,    88,    89,    90,    91,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    93,    77,    78,
      79,    93,    81,    82,    35,    35,    35,    86,    87,    88,
      89,    90,    91,    67,    68,    69,    70,    71,    72,    85,
      85,    25,    96,    77,    78,    79,    97,    81,    82,    93,
      44,    43,    86,    87,    88,    89,    90,    91,    96,    96,
      85,    36,    98,    98,    96,    93,    93,    88,    35,    93,
      93,    35,    35,    99,    93,    93,    35,    99,    35,    35,
     100,    35,    98,   100,    97,    20,    94,    99,    21,    35,
      12,    53,    98,   100,    35,   100,    35,    99,   101,    97,
      19,    35,    93,    93,    36,    35,    93,    93,    93,   102,
      93,    93,    85,    36,    85,    35,    93,     0,     0,   295,
     346,   425,   387,   127,   392,   388,   428,   259,   310,   607,
     610,    62,   312,   355,   123,   305,   317,   434,   438,   510,
     478,   277,   207,   361,   414,   407,   522,   501,   544
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
#line 297 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 28:
#line 304 "ned.y"
{
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 31:
#line 318 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 32:
#line 324 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-3]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 35:
#line 338 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 36:
#line 344 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 37:
#line 353 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 38:
#line 357 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 40:
#line 376 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 41:
#line 383 "ned.y"
{
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 42:
#line 392 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 43:
#line 396 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 45:
#line 421 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 46:
#line 428 "ned.y"
{
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 47:
#line 437 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 48:
#line 441 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 51:
#line 453 "ned.y"
{
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 52:
#line 458 "ned.y"
{
                ;
    break;}
case 57:
#line 474 "ned.y"
{
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(yylsp[0]));
                  setComments(ps.machine,yylsp[0],yylsp[0]);
                ;
    break;}
case 60:
#line 488 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 63:
#line 500 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 64:
#line 505 "ned.y"
{
                ;
    break;}
case 67:
#line 516 "ned.y"
{
                  setComments(ps.param,yylsp[-1]);
                ;
    break;}
case 68:
#line 520 "ned.y"
{
                  setComments(ps.param,yylsp[-1]);
                ;
    break;}
case 69:
#line 524 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 70:
#line 531 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                ;
    break;}
case 71:
#line 535 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                ;
    break;}
case 72:
#line 539 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                ;
    break;}
case 73:
#line 543 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                ;
    break;}
case 74:
#line 547 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 75:
#line 551 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 76:
#line 555 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                ;
    break;}
case 77:
#line 559 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                ;
    break;}
case 78:
#line 563 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                ;
    break;}
case 81:
#line 575 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 82:
#line 580 "ned.y"
{
                ;
    break;}
case 91:
#line 603 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 92:
#line 608 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 95:
#line 621 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 96:
#line 626 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 99:
#line 639 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 100:
#line 644 "ned.y"
{
                ;
    break;}
case 105:
#line 660 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 106:
#line 666 "ned.y"
{
                ;
    break;}
case 107:
#line 669 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 108:
#line 676 "ned.y"
{
                ;
    break;}
case 109:
#line 679 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-3], yylsp[-1]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 110:
#line 685 "ned.y"
{
                ;
    break;}
case 111:
#line 688 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-4], yylsp[-1]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 112:
#line 695 "ned.y"
{
                ;
    break;}
case 118:
#line 717 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,yylsp[-1],yylsp[0]);
                ;
    break;}
case 119:
#line 722 "ned.y"
{
                ;
    break;}
case 120:
#line 725 "ned.y"
{
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substmachines,yylsp[-3],yylsp[0]);
                ;
    break;}
case 121:
#line 731 "ned.y"
{
                ;
    break;}
case 126:
#line 747 "ned.y"
{
                  ps.substmachine = addSubstmachine(ps.substmachines,yylsp[0]);
                  setComments(ps.substmachine,yylsp[0]);
                ;
    break;}
case 131:
#line 765 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 132:
#line 770 "ned.y"
{
                ;
    break;}
case 133:
#line 773 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 134:
#line 779 "ned.y"
{
                ;
    break;}
case 139:
#line 796 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-3]);
                  addExpression(ps.substparam, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparam,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 142:
#line 810 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 143:
#line 815 "ned.y"
{
                ;
    break;}
case 144:
#line 818 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 145:
#line 824 "ned.y"
{
                ;
    break;}
case 150:
#line 840 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 151:
#line 847 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 152:
#line 855 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 156:
#line 868 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "false" );
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 157:
#line 874 "ned.y"
{
                ;
    break;}
case 158:
#line 877 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected( "yes" );
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 159:
#line 883 "ned.y"
{
                ;
    break;}
case 166:
#line 907 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                ;
    break;}
case 167:
#line 911 "ned.y"
{
                  ps.inLoop=1;
                ;
    break;}
case 168:
#line 915 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-7],yylsp[-4]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 171:
#line 929 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 172:
#line 939 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 174:
#line 947 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 178:
#line 960 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 179:
#line 965 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 180:
#line 970 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 181:
#line 976 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 184:
#line 990 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 185:
#line 996 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 186:
#line 1004 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 187:
#line 1009 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 188:
#line 1016 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 189:
#line 1023 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 192:
#line 1037 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 193:
#line 1042 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 194:
#line 1049 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 195:
#line 1054 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 196:
#line 1061 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 197:
#line 1066 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 198:
#line 1074 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 199:
#line 1079 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 200:
#line 1084 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 202:
#line 1102 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 203:
#line 1110 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1],NULLPOS);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 204:
#line 1116 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-5],yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-6],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 205:
#line 1125 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 206:
#line 1133 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 207:
#line 1139 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 208:
#line 1143 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 209:
#line 1154 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 210:
#line 1156 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 211:
#line 1158 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 212:
#line 1160 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 214:
#line 1166 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 215:
#line 1169 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 216:
#line 1171 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 217:
#line 1173 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 218:
#line 1175 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 219:
#line 1177 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 220:
#line 1179 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 221:
#line 1183 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[0]); ;
    break;}
case 222:
#line 1186 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 223:
#line 1188 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 224:
#line 1190 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 225:
#line 1192 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 226:
#line 1194 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 227:
#line 1196 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 228:
#line 1199 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 229:
#line 1201 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 230:
#line 1203 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 231:
#line 1207 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 232:
#line 1210 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 233:
#line 1212 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 234:
#line 1214 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 235:
#line 1218 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 236:
#line 1220 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 237:
#line 1222 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 238:
#line 1224 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 239:
#line 1227 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 240:
#line 1229 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 241:
#line 1231 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 242:
#line 1233 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 248:
#line 1246 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 249:
#line 1251 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                ;
    break;}
case 250:
#line 1256 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 251:
#line 1262 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsRef("true");
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 252:
#line 1268 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((ParamRefNode *)yyval)->setIsAncestor("true");
                ;
    break;}
case 253:
#line 1276 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 254:
#line 1281 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 255:
#line 1283 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 257:
#line 1292 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 258:
#line 1294 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 259:
#line 1296 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 260:
#line 1301 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 261:
#line 1303 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 262:
#line 1305 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 267:
#line 1322 "ned.y"
{
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                ;
    break;}
case 268:
#line 1328 "ned.y"
{
                  ps.cppinclude = (CppincludeNode *)createNodeWithTag(NED_CPPINCLUDE, ps.nedfile );
                  ps.cppinclude->setFilename(toString(yylsp[0]));
                  setComments(ps.cppinclude,yylsp[-1],yylsp[0]);
                ;
    break;}
case 269:
#line 1337 "ned.y"
{
                  ps.cppstruct = (CppStructNode *)createNodeWithTag(NED_CPP_STRUCT, ps.nedfile );
                  ps.cppstruct->setName(toString(yylsp[-1]));
                  setComments(ps.cppstruct,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 270:
#line 1346 "ned.y"
{
                  ps.cppcobject = (CppCobjectNode *)createNodeWithTag(NED_CPP_COBJECT, ps.nedfile );
                  ps.cppcobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppcobject,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 271:
#line 1355 "ned.y"
{
                  ps.cppnoncobject = (CppNoncobjectNode *)createNodeWithTag(NED_CPP_NONCOBJECT, ps.nedfile );
                  ps.cppnoncobject->setName(toString(yylsp[-1]));
                  setComments(ps.cppnoncobject,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 272:
#line 1364 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 273:
#line 1371 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 274:
#line 1375 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 275:
#line 1383 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 280:
#line 1400 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                ;
    break;}
case 281:
#line 1405 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                ;
    break;}
case 282:
#line 1414 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 283:
#line 1420 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 284:
#line 1424 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 285:
#line 1431 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 286:
#line 1438 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 287:
#line 1444 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 288:
#line 1448 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 289:
#line 1455 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 290:
#line 1462 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 291:
#line 1468 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 292:
#line 1472 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 293:
#line 1479 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 294:
#line 1486 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 301:
#line 1506 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 308:
#line 1525 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 315:
#line 1545 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                ;
    break;}
case 316:
#line 1551 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 317:
#line 1555 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsVirtual(true);
                ;
    break;}
case 318:
#line 1562 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 328:
#line 1581 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 329:
#line 1586 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 331:
#line 1594 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 333:
#line 1602 "ned.y"
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
#line 1620 "ned.y"


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

