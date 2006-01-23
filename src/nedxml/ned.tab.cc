
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
//    bool inLoop;
//    bool inNetwork;

    /* tmp flags, used with msg fields */
    bool isAbstract;
    bool isReadonly;

    /* NED-I: modules, channels, networks */
    NedFileNode *nedfile;
    WhitespaceNode *whitespace;
    ImportNode *imports;
    PropertydefNode *propertydef;
    ExtendsNode *extends;
    InterfaceNameNode *interfacename;
    NEDElement *module;  // in fact, CompoundModuleNode* or SimpleModule*
    ModuleInterfaceNode *moduleinterface;
    ParametersNode *parameters;
    ParamGroupNode *paramgroup;
    ParamNode *param;
    PropertyNode *property;
    KeyValueNode *keyvalue;
    GatesNode *gates;
    GateGroupNode *gategroup;
    GateNode *gate;
    SubmodulesNode *submods;
    SubmoduleNode *submod;
    ConnectionsNode *conns;
    ConnectionNode *conn;
    ChannelInterfaceNode *channelinterface;
    ChannelNode *channel;
    ConnectionGroupNode *connectiongroup;
    LoopNode *loop;
    ConditionNode *condition;

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
    MsgpropertyNode *msgproperty;
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
RefNode *createParamRef(const char *param, const char *paramindex=NULL, const char *module=NULL, const char *moduleindex=NULL);
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



#define	YYFINAL		576
#define	YYFLAG		-32768
#define	YYNTBASE	106

#define YYTRANSLATE(x) ((unsigned)(x) <= 341 ? yytranslate[x] : 251)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    93,     2,     2,   102,
   103,    91,    89,    97,    90,   101,    92,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    88,    96,     2,
   100,     2,    87,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    98,     2,    99,    94,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,   104,     2,   105,     2,     2,     2,     2,     2,
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
    20,    22,    24,    26,    28,    30,    32,    34,    35,    40,
    44,    46,    48,    52,    55,    56,    58,    63,    67,    71,
    74,    79,    82,    86,    89,    97,   100,   104,   107,   109,
   110,   115,   117,   118,   119,   124,   127,   128,   132,   134,
   136,   140,   143,   147,   152,   157,   161,   165,   169,   173,
   175,   176,   177,   182,   184,   185,   190,   194,   199,   203,
   207,   209,   213,   215,   219,   221,   225,   227,   229,   230,
   231,   236,   238,   239,   242,   244,   245,   252,   253,   261,
   262,   271,   272,   282,   286,   288,   289,   292,   294,   295,
   300,   301,   308,   311,   312,   316,   318,   322,   325,   326,
   327,   332,   333,   340,   343,   344,   348,   350,   353,   355,
   360,   361,   363,   364,   365,   371,   372,   377,   379,   380,
   383,   385,   387,   389,   390,   398,   402,   404,   410,   413,
   414,   417,   418,   421,   423,   430,   439,   446,   455,   459,
   461,   464,   466,   469,   471,   474,   477,   479,   482,   486,
   488,   491,   493,   496,   498,   501,   504,   506,   509,   511,
   514,   518,   522,   528,   531,   535,   537,   539,   541,   548,
   553,   557,   559,   566,   571,   573,   577,   581,   585,   589,
   593,   597,   601,   604,   608,   612,   616,   620,   624,   628,
   632,   636,   640,   643,   647,   651,   655,   658,   662,   666,
   672,   676,   681,   688,   697,   708,   710,   712,   714,   716,
   718,   720,   723,   727,   731,   734,   736,   738,   740,   742,
   744,   748,   753,   755,   757,   759,   761,   765,   769,   772,
   775,   779,   783,   787,   792,   796,   800,   801,   809,   810,
   820,   822,   823,   826,   828,   831,   836,   837,   846,   847,
   858,   859,   868,   869,   880,   881,   890,   891,   902,   903,
   908,   909,   911,   912,   915,   917,   922,   924,   926,   928,
   930,   932,   934,   935,   940,   941,   943,   944,   947,   949,
   950,   959,   960,   968,   970,   972,   975,   978,   979,   981,
   984,   986,   988,   990,   992,   995,   998,  1001,  1004,  1006,
  1008,  1010,  1014,  1018,  1021,  1022,  1027,  1028,  1031,  1032,
  1034,  1036,  1038,  1041,  1043,  1046,  1048,  1050,  1052,  1054,
  1056,  1059,  1061,  1063,  1064,  1066
};

static const short yyrhs[] = {   107,
     0,   107,   108,     0,     0,   109,     0,   113,     0,   118,
     0,   121,     0,   192,     0,   209,     0,   210,     0,   211,
     0,   212,     0,   213,     0,   214,     0,   220,     0,   223,
     0,   226,     0,     0,     3,   110,   111,    96,     0,   111,
    97,   112,     0,   112,     0,    35,     0,   114,   115,   117,
     0,     5,    34,     0,     0,   116,     0,   116,    31,   196,
   249,     0,    31,   196,   249,     0,    26,    34,   249,     0,
    26,   249,     0,   119,   126,   132,   120,     0,     4,    34,
     0,    24,    34,   249,     0,    24,   249,     0,   122,   126,
   132,   141,   168,   124,   123,     0,     6,    34,     0,    25,
    34,   249,     0,    25,   249,     0,   125,     0,     0,    12,
    88,    35,    96,     0,   127,     0,     0,     0,     7,    88,
   128,   129,     0,   130,    96,     0,     0,   130,    97,   131,
     0,   131,     0,    34,     0,    34,    88,    44,     0,    43,
    34,     0,    34,    88,    43,     0,    34,    88,    43,    44,
     0,    34,    88,    44,    43,     0,    34,    88,    45,     0,
    34,    88,    46,     0,    34,    88,    47,     0,    34,    88,
    48,     0,   133,     0,     0,     0,     8,    88,   134,   135,
     0,   136,     0,     0,   136,    13,   137,    96,     0,    13,
   137,    96,     0,   136,    14,   139,    96,     0,    14,   139,
    96,     0,   137,    97,   138,     0,   138,     0,    34,    98,
    99,     0,    34,     0,   139,    97,   140,     0,   140,     0,
    34,    98,    99,     0,    34,     0,   142,     0,     0,     0,
    10,    88,   143,   144,     0,   145,     0,     0,   145,   146,
     0,   146,     0,     0,    34,    88,    34,   249,   147,   151,
     0,     0,    34,    88,    34,   195,   249,   148,   151,     0,
     0,    34,    88,    34,    22,    34,   249,   149,   151,     0,
     0,    34,    88,    34,   195,    22,    34,   249,   150,   151,
     0,   152,   160,   167,     0,   153,     0,     0,   153,   154,
     0,   154,     0,     0,     7,    88,   155,   157,     0,     0,
     7,    21,   196,    88,   156,   157,     0,   158,    96,     0,
     0,   158,    97,   159,     0,   159,     0,    34,   100,   196,
     0,   160,   161,     0,     0,     0,     9,    88,   162,   164,
     0,     0,     9,    21,   196,    88,   163,   164,     0,   165,
    96,     0,     0,   165,    97,   166,     0,   166,     0,    34,
   195,     0,    34,     0,    12,    88,    35,    96,     0,     0,
   169,     0,     0,     0,    11,    15,    88,   170,   172,     0,
     0,    11,    88,   171,   172,     0,   173,     0,     0,   173,
   174,     0,   174,     0,   175,     0,   182,     0,     0,    18,
   176,   177,    20,   181,    28,   249,     0,   178,    97,   177,
     0,   178,     0,    34,   100,   196,    19,   196,     0,    21,
   196,     0,     0,    12,    35,     0,     0,   181,   182,     0,
   182,     0,   183,    17,   187,   179,   180,   250,     0,   183,
    17,   191,    17,   187,   179,   180,   250,     0,   183,    16,
   187,   179,   180,   250,     0,   183,    16,   191,    16,   187,
   179,   180,   250,     0,   184,   101,   185,     0,   186,     0,
    34,   195,     0,    34,     0,    34,   195,     0,    34,     0,
    34,    69,     0,    34,   195,     0,    34,     0,    34,    69,
     0,   188,   101,   189,     0,   190,     0,    34,   195,     0,
    34,     0,    34,   195,     0,    34,     0,    34,    69,     0,
    34,   195,     0,    34,     0,    34,    69,     0,    34,     0,
    31,   196,     0,   191,    31,   196,     0,   193,   152,   194,
     0,    23,    34,    88,    34,   249,     0,    27,   249,     0,
    98,   196,    99,     0,   199,     0,   197,     0,   198,     0,
    39,   102,   199,    97,   199,   103,     0,    39,   102,   199,
   103,     0,    39,   102,   103,     0,    39,     0,    40,   102,
   206,    97,   206,   103,     0,    40,   102,   206,   103,     0,
   200,     0,   102,   199,   103,     0,   199,    89,   199,     0,
   199,    90,   199,     0,   199,    91,   199,     0,   199,    92,
   199,     0,   199,    93,   199,     0,   199,    94,   199,     0,
    90,   199,     0,   199,    70,   199,     0,   199,    71,   199,
     0,   199,    72,   199,     0,   199,    73,   199,     0,   199,
    74,   199,     0,   199,    75,   199,     0,   199,    76,   199,
     0,   199,    77,   199,     0,   199,    78,   199,     0,    79,
   199,     0,   199,    80,   199,     0,   199,    81,   199,     0,
   199,    82,   199,     0,    83,   199,     0,   199,    84,   199,
     0,   199,    85,   199,     0,   199,    87,   199,    88,   199,
     0,    34,   102,   103,     0,    34,   102,   199,   103,     0,
    34,   102,   199,    97,   199,   103,     0,    34,   102,   199,
    97,   199,    97,   199,   103,     0,    34,   102,   199,    97,
   199,    97,   199,    97,   199,   103,     0,   201,     0,   202,
     0,   203,     0,   204,     0,   205,     0,    34,     0,    41,
    34,     0,    41,    42,    34,     0,    42,    41,    34,     0,
    42,    34,     0,   206,     0,    37,     0,    38,     0,   207,
     0,    68,     0,    68,   102,   103,     0,    67,   102,    34,
   103,     0,    35,     0,    32,     0,    33,     0,   208,     0,
   208,    32,    34,     0,   208,    33,    34,     0,    32,    34,
     0,    33,    34,     0,    49,    50,   249,     0,    53,    34,
    96,     0,    52,    34,    96,     0,    52,    55,    34,    96,
     0,    51,    34,    96,     0,    54,    34,    96,     0,     0,
    54,    34,   104,   215,   217,   105,   249,     0,     0,    54,
    34,    56,    34,   104,   216,   217,   105,   249,     0,   218,
     0,     0,   218,   219,     0,   219,     0,    34,    96,     0,
    34,   100,   248,    96,     0,     0,    51,    34,   104,   221,
   229,   235,   105,   249,     0,     0,    51,    34,    56,    34,
   104,   222,   229,   235,   105,   249,     0,     0,    52,    34,
   104,   224,   229,   235,   105,   249,     0,     0,    52,    34,
    56,    34,   104,   225,   229,   235,   105,   249,     0,     0,
    53,    34,   104,   227,   229,   235,   105,   249,     0,     0,
    53,    34,    56,    34,   104,   228,   229,   235,   105,   249,
     0,     0,    58,    88,   230,   231,     0,     0,   232,     0,
     0,   232,   233,     0,   233,     0,    34,   100,   234,    96,
     0,    35,     0,    32,     0,    33,     0,   208,     0,    37,
     0,    38,     0,     0,    57,    88,   236,   237,     0,     0,
   238,     0,     0,   238,   239,     0,   239,     0,     0,   242,
   243,    34,   240,   244,   245,   246,    96,     0,     0,   242,
    34,   241,   244,   245,   246,    96,     0,    59,     0,    60,
     0,    59,    60,     0,    60,    59,     0,     0,    34,     0,
    34,    91,     0,    61,     0,    62,     0,    63,     0,    64,
     0,    66,    61,     0,    66,    62,     0,    66,    63,     0,
    66,    64,     0,    65,     0,    45,     0,    46,     0,    98,
    32,    99,     0,    98,    34,    99,     0,    98,    99,     0,
     0,    54,   102,    34,   103,     0,     0,   100,   247,     0,
     0,    35,     0,    36,     0,    32,     0,    90,    32,     0,
    33,     0,    90,    33,     0,   208,     0,    37,     0,    38,
     0,    34,     0,    32,     0,    90,    32,     0,    34,     0,
    96,     0,     0,    97,     0,    96,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   242,   246,   247,   251,   253,   255,   257,   259,   262,   263,
   264,   265,   266,   268,   270,   272,   274,   282,   287,   294,
   295,   299,   311,   315,   324,   325,   329,   335,   344,   348,
   358,   365,   375,   379,   389,   399,   409,   413,   423,   424,
   428,   438,   439,   443,   449,   454,   455,   459,   463,   473,
   477,   481,   485,   489,   493,   497,   501,   505,   509,   519,
   520,   524,   530,   535,   536,   540,   541,   542,   543,   547,
   548,   552,   557,   565,   566,   570,   575,   586,   587,   591,
   597,   602,   603,   607,   608,   612,   618,   620,   627,   629,
   635,   637,   644,   649,   658,   659,   663,   664,   668,   674,
   676,   683,   689,   690,   694,   695,   699,   711,   712,   716,
   722,   724,   731,   736,   737,   741,   742,   746,   753,   764,
   768,   775,   776,   780,   787,   789,   796,   801,   802,   806,
   807,   811,   812,   816,   821,   830,   831,   835,   845,   849,
   853,   857,   861,   862,   866,   871,   876,   882,   891,   892,
   896,   902,   910,   915,   919,   927,   934,   940,   950,   951,
   955,   960,   967,   972,   976,   984,   989,   993,  1002,  1007,
  1012,  1023,  1029,  1038,  1050,  1055,  1060,  1064,  1075,  1077,
  1079,  1081,  1086,  1088,  1093,  1094,  1097,  1099,  1101,  1103,
  1105,  1107,  1110,  1114,  1116,  1118,  1120,  1122,  1124,  1127,
  1129,  1131,  1134,  1138,  1140,  1142,  1145,  1148,  1150,  1152,
  1155,  1157,  1159,  1161,  1163,  1168,  1169,  1170,  1171,  1172,
  1176,  1181,  1186,  1192,  1198,  1206,  1210,  1212,  1217,  1221,
  1223,  1225,  1230,  1235,  1237,  1239,  1245,  1246,  1247,  1248,
  1256,  1265,  1274,  1281,  1291,  1300,  1309,  1316,  1320,  1328,
  1335,  1336,  1340,  1341,  1345,  1351,  1361,  1367,  1371,  1378,
  1385,  1391,  1395,  1402,  1409,  1415,  1419,  1426,  1433,  1439,
  1439,  1443,  1444,  1448,  1449,  1453,  1463,  1464,  1465,  1466,
  1467,  1468,  1472,  1478,  1478,  1482,  1483,  1487,  1488,  1492,
  1500,  1504,  1511,  1518,  1520,  1522,  1524,  1526,  1531,  1532,
  1534,  1535,  1536,  1537,  1539,  1540,  1541,  1542,  1544,  1545,
  1546,  1551,  1556,  1561,  1565,  1569,  1573,  1577,  1581,  1585,
  1586,  1587,  1588,  1589,  1590,  1591,  1592,  1593,  1594,  1598,
  1599,  1600,  1603,  1603,  1605,  1605
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
"UMIN","';'","','","'['","']'","'='","'.'","'('","')'","'{'","'}'","networkdescription",
"somedefinitions","definition","import","@1","filenames","filename","channeldefinition_old",
"channelheader_old","opt_channelattrblock_old","channelattrblock_old","endchannel_old",
"simpledefinition_old","simpleheader_old","endsimple_old","moduledefinition_old",
"moduleheader_old","endmodule_old","opt_displayblock_old","displayblock_old",
"opt_paramblock_old","paramblock_old","@2","opt_parameters_old","parameters_old",
"parameter_old","opt_gateblock_old","gateblock_old","@3","opt_gates_old","gates_old",
"gatesI_old","gateI_old","gatesO_old","gateO_old","opt_submodblock_old","submodblock_old",
"@4","opt_submodules_old","submodules_old","submodule_old","@5","@6","@7","@8",
"submodule_body_old","opt_substparamblocks_old","substparamblocks_old","substparamblock_old",
"@9","@10","opt_substparameters_old","substparameters_old","substparameter_old",
"opt_gatesizeblocks_old","gatesizeblock_old","@11","@12","opt_gatesizes_old",
"gatesizes_old","gatesize_old","opt_submod_displayblock_old","opt_connblock_old",
"connblock_old","@13","@14","opt_connections_old","connections_old","connection_old",
"loopconnection_old","@15","loopvarlist_old","loopvar_old","opt_conncondition_old",
"opt_conn_displaystr_old","notloopconnections_old","notloopconnection_old","leftgatespec_old",
"leftmod_old","leftgate_old","parentleftgate_old","rightgatespec_old","rightmod_old",
"rightgate_old","parentrightgate_old","channeldescr_old","networkdefinition_old",
"networkheader_old","endnetwork_old","vector","expression","inputvalue","xmldocvalue",
"expr","simple_expr","parameter_expr","string_expr","boolconst_expr","numconst_expr",
"special_expr","stringconstant","numconst","timeconstant","cplusplus","struct_decl",
"class_decl","message_decl","enum_decl","enum","@16","@17","opt_enumfields",
"enumfields","enumfield","message","@18","@19","class","@20","@21","struct",
"@22","@23","opt_propertiesblock","@24","opt_properties","properties","property",
"propertyvalue","opt_fieldsblock","@25","opt_fields","fields","field","@26",
"@27","fieldmodifiers","fielddatatype","opt_fieldvector","opt_fieldenum","opt_fieldvalue",
"fieldvalue","enumvalue","opt_semicolon","comma_or_semicolon", NULL
};
#endif

static const short yyr1[] = {     0,
   106,   107,   107,   108,   108,   108,   108,   108,   108,   108,
   108,   108,   108,   108,   108,   108,   108,   110,   109,   111,
   111,   112,   113,   114,   115,   115,   116,   116,   117,   117,
   118,   119,   120,   120,   121,   122,   123,   123,   124,   124,
   125,   126,   126,   128,   127,   129,   129,   130,   130,   131,
   131,   131,   131,   131,   131,   131,   131,   131,   131,   132,
   132,   134,   133,   135,   135,   136,   136,   136,   136,   137,
   137,   138,   138,   139,   139,   140,   140,   141,   141,   143,
   142,   144,   144,   145,   145,   147,   146,   148,   146,   149,
   146,   150,   146,   151,   152,   152,   153,   153,   155,   154,
   156,   154,   157,   157,   158,   158,   159,   160,   160,   162,
   161,   163,   161,   164,   164,   165,   165,   166,   166,   167,
   167,   168,   168,   170,   169,   171,   169,   172,   172,   173,
   173,   174,   174,   176,   175,   177,   177,   178,   179,   179,
   180,   180,   181,   181,   182,   182,   182,   182,   183,   183,
   184,   184,   185,   185,   185,   186,   186,   186,   187,   187,
   188,   188,   189,   189,   189,   190,   190,   190,   191,   191,
   191,   192,   193,   194,   195,   196,   196,   196,   197,   197,
   197,   197,   198,   198,   199,   199,   199,   199,   199,   199,
   199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
   199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
   199,   199,   199,   199,   199,   200,   200,   200,   200,   200,
   201,   201,   201,   201,   201,   202,   203,   203,   204,   205,
   205,   205,   206,   207,   207,   207,   208,   208,   208,   208,
   209,   210,   211,   211,   212,   213,   215,   214,   216,   214,
   217,   217,   218,   218,   219,   219,   221,   220,   222,   220,
   224,   223,   225,   223,   227,   226,   228,   226,   230,   229,
   229,   231,   231,   232,   232,   233,   234,   234,   234,   234,
   234,   234,   236,   235,   235,   237,   237,   238,   238,   240,
   239,   241,   239,   242,   242,   242,   242,   242,   243,   243,
   243,   243,   243,   243,   243,   243,   243,   243,   243,   243,
   243,   244,   244,   244,   244,   245,   245,   246,   246,   247,
   247,   247,   247,   247,   247,   247,   247,   247,   247,   248,
   248,   248,   249,   249,   250,   250
};

static const short yyr2[] = {     0,
     1,     2,     0,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     0,     4,     3,
     1,     1,     3,     2,     0,     1,     4,     3,     3,     2,
     4,     2,     3,     2,     7,     2,     3,     2,     1,     0,
     4,     1,     0,     0,     4,     2,     0,     3,     1,     1,
     3,     2,     3,     4,     4,     3,     3,     3,     3,     1,
     0,     0,     4,     1,     0,     4,     3,     4,     3,     3,
     1,     3,     1,     3,     1,     3,     1,     1,     0,     0,
     4,     1,     0,     2,     1,     0,     6,     0,     7,     0,
     8,     0,     9,     3,     1,     0,     2,     1,     0,     4,
     0,     6,     2,     0,     3,     1,     3,     2,     0,     0,
     4,     0,     6,     2,     0,     3,     1,     2,     1,     4,
     0,     1,     0,     0,     5,     0,     4,     1,     0,     2,
     1,     1,     1,     0,     7,     3,     1,     5,     2,     0,
     2,     0,     2,     1,     6,     8,     6,     8,     3,     1,
     2,     1,     2,     1,     2,     2,     1,     2,     3,     1,
     2,     1,     2,     1,     2,     2,     1,     2,     1,     2,
     3,     3,     5,     2,     3,     1,     1,     1,     6,     4,
     3,     1,     6,     4,     1,     3,     3,     3,     3,     3,
     3,     3,     2,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     2,     3,     3,     3,     2,     3,     3,     5,
     3,     4,     6,     8,    10,     1,     1,     1,     1,     1,
     1,     2,     3,     3,     2,     1,     1,     1,     1,     1,
     3,     4,     1,     1,     1,     1,     3,     3,     2,     2,
     3,     3,     3,     4,     3,     3,     0,     7,     0,     9,
     1,     0,     2,     1,     2,     4,     0,     8,     0,    10,
     0,     8,     0,    10,     0,     8,     0,    10,     0,     4,
     0,     1,     0,     2,     1,     4,     1,     1,     1,     1,
     1,     1,     0,     4,     0,     1,     0,     2,     1,     0,
     8,     0,     7,     1,     1,     2,     2,     0,     1,     2,
     1,     1,     1,     1,     2,     2,     2,     2,     1,     1,
     1,     3,     3,     2,     0,     4,     0,     2,     0,     1,
     1,     1,     2,     1,     2,     1,     1,     1,     1,     1,
     2,     1,     1,     0,     1,     1
};

static const short yydefact[] = {     3,
     1,    18,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     2,     4,     5,    25,     6,    43,     7,    43,     8,
    96,     9,    10,    11,    12,    13,    14,    15,    16,    17,
     0,    32,    24,    36,     0,   334,     0,     0,     0,     0,
     0,     0,     0,    26,     0,    61,    42,    61,     0,     0,
    95,    98,    22,     0,    21,     0,   333,   241,     0,   245,
   257,     0,   243,   261,     0,     0,   242,   265,     0,   246,
   247,   234,   235,   221,   233,   227,   228,   182,     0,     0,
     0,     0,   230,     0,     0,     0,     0,   334,   177,   178,
   176,   185,   216,   217,   218,   219,   220,   226,   229,   236,
   334,    23,     0,    44,     0,     0,    60,    79,     0,    99,
   334,   172,    97,    19,     0,   334,     0,   271,     0,   271,
   244,     0,   271,     0,   252,   239,   240,     0,     0,     0,
   222,     0,   225,     0,     0,     0,   203,   207,   193,     0,
    28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   334,    30,   334,    47,    62,   334,
    31,     0,   123,    78,     0,   104,   174,    20,   173,   259,
     0,   285,   263,   285,   267,   285,   249,     0,     0,   251,
   254,   211,     0,   181,     0,     0,   223,   224,     0,   231,
   186,   194,   195,   196,   197,   198,   199,   200,   201,   202,
   204,   205,   206,   208,   209,     0,   187,   188,   189,   190,
   191,   192,   237,   238,    29,    27,    50,     0,    45,     0,
    49,    65,   334,    34,    80,     0,    40,   122,   101,     0,
   100,     0,   106,   271,   269,     0,     0,   271,     0,   271,
     0,   252,   255,     0,   334,   253,     0,   212,     0,   180,
     0,   184,   232,     0,     0,    52,    46,     0,     0,     0,
    63,    64,    33,    83,     0,   126,     0,     0,    39,   104,
     0,   103,     0,   285,   273,   283,   334,   285,   334,   285,
   334,     0,   330,   332,     0,     0,   248,     0,     0,     0,
   210,    53,    51,    56,    57,    58,    59,    48,    73,     0,
    71,    77,     0,    75,     0,     0,     0,    81,    82,    85,
   124,   129,     0,   334,    35,   102,   107,   105,     0,     0,
   270,   272,   275,   298,   258,     0,   262,     0,   266,   334,
   331,   256,     0,   213,   179,   183,    54,    55,     0,    67,
     0,     0,    69,     0,     0,     0,     0,    84,   129,   134,
   157,   127,   128,   131,   132,   133,     0,     0,   150,     0,
   334,    38,   334,     0,   274,   294,   295,   284,   298,   289,
     0,   334,   334,   250,     0,    72,    70,    76,    74,    66,
    68,   334,   125,     0,   158,     0,   156,   130,     0,     0,
     0,    41,    37,   260,   278,   279,   277,   281,   282,   280,
     0,   296,   297,   288,   292,   310,   311,   301,   302,   303,
   304,   309,     0,     0,   264,   268,     0,   214,     0,   334,
    86,     0,     0,   137,     0,     0,   167,   140,     0,   160,
     0,   140,     0,   154,   149,   276,   300,   315,   305,   306,
   307,   308,   290,     0,   334,     0,    88,    96,     0,     0,
     0,   175,   170,   168,   166,     0,   142,     0,     0,     0,
   142,     0,   155,   153,     0,   317,   315,   215,    90,   334,
    96,    87,   109,     0,     0,   144,   136,   139,     0,     0,
   164,   159,   167,   140,   171,     0,   140,     0,     0,   314,
     0,   319,   317,    96,    92,    89,   121,     0,   334,   143,
   141,   336,   335,   147,   165,   163,   142,   145,   142,   312,
   313,     0,     0,     0,   319,    91,    96,     0,     0,   108,
    94,   138,   135,     0,     0,     0,   322,   324,   329,   320,
   321,   327,   328,     0,   326,   318,   293,     0,    93,     0,
   110,     0,   148,   146,   316,   323,   325,   291,     0,   115,
     0,   112,   119,   111,     0,   117,   120,   115,   118,   114,
     0,   113,   116,     0,     0,     0
};

static const short yydefgoto[] = {   574,
     1,    12,    13,    31,    54,    55,    14,    15,    43,    44,
   102,    16,    17,   171,    18,    19,   325,   278,   279,    46,
    47,   168,   229,   230,   231,   106,   107,   232,   271,   272,
   310,   311,   313,   314,   173,   174,   274,   318,   319,   320,
   458,   481,   504,   527,   482,   483,    51,    52,   176,   280,
   241,   242,   243,   507,   530,   560,   568,   564,   565,   566,
   531,   237,   238,   359,   322,   362,   363,   364,   365,   394,
   433,   434,   467,   490,   485,   366,   367,   368,   445,   369,
   438,   439,   492,   440,   441,    20,    21,   112,   465,    88,
    89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
    99,   100,    22,    23,    24,    25,    26,    27,   125,   252,
   189,   190,   191,    28,   118,   244,    29,   120,   248,    30,
   123,   250,   182,   285,   331,   332,   333,   411,   247,   334,
   378,   379,   380,   477,   448,   381,   424,   476,   502,   524,
   546,   296,    58,   514
};

static const short yypact[] = {-32768,
   265,-32768,     9,    33,    42,    52,   -30,   107,    -8,   110,
   114,-32768,-32768,-32768,   178,-32768,   153,-32768,   153,-32768,
   206,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   201,-32768,-32768,-32768,   157,   158,   -35,    91,   225,    94,
   103,   183,   237,   218,   179,   264,-32768,   264,    24,   249,
   206,-32768,-32768,    36,-32768,   244,-32768,-32768,   253,-32768,
-32768,   258,-32768,-32768,   187,   260,-32768,-32768,   267,-32768,
-32768,   281,   290,   197,-32768,-32768,-32768,   232,   236,     4,
    18,   269,   270,   223,   223,   223,   223,   158,-32768,-32768,
   573,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   194,
    21,-32768,   183,-32768,   238,   342,-32768,   364,   183,-32768,
   158,-32768,-32768,-32768,   201,   158,   280,   330,   287,   330,
-32768,   296,   330,   297,   369,-32768,-32768,    86,   129,   371,
-32768,   370,-32768,   373,   384,   322,-32768,-32768,-32768,   446,
-32768,   223,   223,   223,   223,   223,   223,   223,   223,   223,
   223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
   223,   223,   393,   400,   158,-32768,   158,     5,-32768,    26,
-32768,   347,   426,-32768,   350,   405,-32768,-32768,-32768,-32768,
   352,   386,-32768,   386,-32768,   386,-32768,   -56,   336,   369,
-32768,-32768,   305,-32768,   339,   149,-32768,-32768,   341,-32768,
-32768,   255,   255,   255,   255,   255,   255,   623,   623,   623,
   150,   150,   150,   261,   261,   548,    64,    64,   351,   351,
   351,   351,-32768,-32768,-32768,-32768,   358,   423,-32768,   151,
-32768,   215,   158,-32768,-32768,    -3,   449,-32768,-32768,   366,
-32768,   185,-32768,   330,-32768,   376,   368,   330,   372,   330,
   374,   369,-32768,    -9,   158,-32768,   223,-32768,   223,-32768,
   371,-32768,-32768,   223,   314,-32768,-32768,     5,   440,   442,
-32768,   284,-32768,   444,   392,-32768,   403,   470,-32768,   405,
   183,-32768,   405,   386,   464,-32768,   158,   386,   158,   386,
   158,   395,-32768,-32768,   475,   414,-32768,   378,   480,   408,
   598,   468,   465,-32768,-32768,-32768,-32768,-32768,   415,   207,
-32768,   416,   211,-32768,   440,   442,   437,-32768,   444,-32768,
-32768,    16,   494,    29,-32768,-32768,-32768,-32768,   427,   434,
-32768,   464,-32768,   -32,-32768,   436,-32768,   438,-32768,   158,
-32768,-32768,   223,-32768,-32768,-32768,-32768,-32768,   443,-32768,
   440,   445,-32768,   442,   213,   224,   511,-32768,    16,-32768,
    76,-32768,    16,-32768,-32768,-32768,   306,   447,-32768,   450,
   158,-32768,   158,   242,-32768,   487,   500,-32768,    70,-32768,
   266,   158,   158,-32768,   412,-32768,-32768,-32768,-32768,-32768,
-32768,   -19,-32768,   529,-32768,   183,   467,-32768,   109,   109,
   532,-32768,-32768,-32768,   281,   290,-32768,-32768,-32768,   194,
   479,-32768,-32768,-32768,   -21,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   130,   542,-32768,-32768,   223,-32768,   543,   -14,
-32768,   478,   559,   483,   482,   183,    41,   561,   492,-32768,
    25,   561,   121,   -47,-32768,-32768,-32768,   499,-32768,-32768,
-32768,-32768,-32768,   514,   158,   566,-32768,   206,   183,   568,
   529,-32768,-32768,-32768,   508,   183,   599,   576,   578,   183,
   599,   578,-32768,-32768,   -15,   560,   499,-32768,-32768,   158,
   206,-32768,-32768,   594,   145,-32768,-32768,-32768,   580,   245,
   -33,-32768,   132,   561,-32768,   245,   561,   517,   528,-32768,
   550,   531,   560,   206,-32768,-32768,   202,   183,   158,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   599,-32768,   599,-32768,
-32768,   600,   148,   563,   531,-32768,   206,    28,   589,-32768,
-32768,-32768,-32768,   245,   245,   553,   281,   290,-32768,-32768,
-32768,-32768,-32768,   332,   194,-32768,-32768,   565,-32768,   183,
-32768,   646,-32768,-32768,-32768,-32768,-32768,-32768,   596,   651,
   590,-32768,   601,-32768,   273,-32768,-32768,   651,-32768,-32768,
   651,-32768,-32768,   700,   701,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   587,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   687,
-32768,-32768,-32768,-32768,   441,   662,-32768,-32768,-32768,-32768,
   396,   367,   404,   365,-32768,-32768,-32768,-32768,-32768,   402,
-32768,-32768,-32768,-32768,  -355,   702,-32768,   671,-32768,-32768,
   448,-32768,   451,-32768,-32768,-32768,-32768,   156,-32768,   154,
-32768,-32768,-32768,-32768,-32768,   377,-32768,   363,-32768,-32768,
   268,-32768,  -426,  -453,-32768,  -423,-32768,-32768,-32768,-32768,
  -358,-32768,-32768,-32768,   327,-32768,-32768,-32768,  -357,  -103,
-32768,-32768,   -54,-32768,-32768,-32768,-32768,-32768,-32768,  -125,
-32768,  -372,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   481,-32768,   540,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,  -113,-32768,-32768,-32768,   399,-32768,  -175,-32768,
-32768,-32768,   353,-32768,-32768,-32768,-32768,   262,   234,   210,
-32768,-32768,   -87,  -481
};


#define	YYLAST		739


static const short yytable[] = {   167,
   141,   410,   429,   397,   196,   175,   184,   456,   249,   186,
   251,   275,  -299,   166,   518,   471,   498,   496,   499,    36,
    59,   473,   293,   177,   294,    38,   376,   377,   179,   137,
   138,   139,   140,   360,   430,   515,   486,   131,   227,   253,
   469,   442,    32,   254,   109,   132,    39,   228,   550,   361,
   396,   133,   553,   554,   165,   470,  -169,  -169,   134,   233,
    60,   510,   371,   534,   396,   535,    33,   517,    61,   447,
   519,  -169,  -287,   193,   195,    34,    57,   225,   396,   226,
   295,    57,   234,   500,   276,    35,   474,   202,   203,   204,
   205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
   215,   216,   217,   218,   219,   220,   221,   222,   329,   464,
   494,   110,   336,   497,   338,   551,    57,    72,    73,    74,
    75,    57,    76,    77,    57,   506,    80,    81,   376,   377,
   284,   114,   115,   516,   288,   300,   290,   472,   396,   436,
    37,  -162,   437,    40,   395,   273,    62,    41,   526,    66,
   545,   470,    82,    83,   159,   160,   161,   162,    69,    45,
    72,    73,    74,    75,    84,    76,    77,   297,    85,    80,
    81,   549,   509,   396,  -286,    86,  -152,   327,   361,   537,
   538,   539,   540,   541,   542,   543,    63,    87,   192,    67,
   449,   450,   451,   452,    64,    82,    83,    68,    70,   335,
   464,   337,   298,   339,   299,   569,    71,    84,    42,   301,
   528,    85,    49,   529,    72,    73,    74,    75,    86,    76,
    77,    78,    79,    80,    81,   163,   164,   269,   270,   396,
    87,   194,  -162,   154,   155,    53,   372,   544,   157,   158,
   159,   160,   161,   162,    56,   261,   267,   268,   103,    82,
    83,   262,   384,    57,    72,    73,    74,    75,    65,    76,
    77,    84,   101,    80,    81,    85,   104,     2,     3,     4,
     5,   105,    86,   405,   406,   111,   407,   116,   408,   409,
   282,   283,   121,   403,    87,   404,   117,     6,   385,    82,
    83,   119,   435,   122,   425,   426,   315,   316,   128,   415,
   124,    84,   350,   351,   431,    85,   353,   354,   390,   351,
   416,   417,    86,     7,   126,     8,     9,    10,    11,   391,
   354,   399,   400,   127,    87,   169,   418,   419,   420,   421,
   422,   423,   463,   129,   151,   152,   153,   130,   154,   155,
   512,   513,   457,   157,   158,   159,   160,   161,   162,   157,
   158,   159,   160,   161,   162,   484,   302,   303,   304,   305,
   306,   307,   488,   556,   557,   170,   495,   479,   570,   571,
   135,   136,   454,   172,   142,   143,   144,   145,   146,   147,
   148,   149,   150,   180,   151,   152,   153,   181,   154,   155,
   183,   156,   505,   157,   158,   159,   160,   161,   162,   185,
   187,   257,   188,   197,   532,    75,   198,   258,   142,   143,
   144,   145,   146,   147,   148,   149,   150,   199,   151,   152,
   153,   533,   154,   155,   200,   156,   223,   157,   158,   159,
   160,   161,   162,   224,   235,   259,   236,   239,   240,   245,
   255,   260,   246,   263,   162,   265,   559,   142,   143,   144,
   145,   146,   147,   148,   149,   150,   266,   151,   152,   153,
   277,   154,   155,   286,   156,   281,   157,   158,   159,   160,
   161,   162,   287,   309,   343,   312,   289,   317,   291,   321,
   344,   142,   143,   144,   145,   146,   147,   148,   149,   150,
   323,   151,   152,   153,   324,   154,   155,   330,   156,   340,
   157,   158,   159,   160,   161,   162,   341,   348,   427,   342,
   346,   347,   349,   352,   428,   142,   143,   144,   145,   146,
   147,   148,   149,   150,   357,   151,   152,   153,   370,   154,
   155,   373,   156,   374,   157,   158,   159,   160,   161,   162,
   382,   386,   383,   388,   392,   402,   412,   401,   201,   142,
   143,   144,   145,   146,   147,   148,   149,   150,   413,   151,
   152,   153,   432,   154,   155,   444,   156,  -151,   157,   158,
   159,   160,   161,   162,   446,   453,   455,   459,   460,   461,
   462,   466,   345,   142,   143,   144,   145,   146,   147,   148,
   149,   150,   468,   151,   152,   153,   475,   154,   155,   480,
   156,   361,   157,   158,   159,   160,   161,   162,  -161,   491,
   489,   493,   508,   501,   511,   520,   478,   142,   143,   144,
   145,   146,   147,   148,   149,   150,   521,   151,   152,   153,
   523,   154,   155,   536,   156,   264,   157,   158,   159,   160,
   161,   162,   142,   143,   144,   145,   146,   147,   148,   149,
   150,   522,   151,   152,   153,   555,   154,   155,   547,   156,
   558,   157,   158,   159,   160,   161,   162,   142,   143,   144,
   145,   146,   147,   148,   149,   150,   552,   151,   152,   153,
   561,   154,   155,   562,   563,   567,   157,   158,   159,   160,
   161,   162,   142,   143,   144,   145,   146,   147,   396,   575,
   576,   178,   151,   152,   153,    48,   154,   155,   308,   108,
   355,   157,   158,   159,   160,   161,   162,   387,   389,   356,
   358,   113,    50,   572,   573,   398,   443,   326,   487,   256,
   375,   414,   292,   328,   548,   393,   525,     0,   503
};

static const short yycheck[] = {   103,
    88,   374,    22,   361,   130,   109,   120,    22,   184,   123,
   186,    15,    34,   101,   496,   442,    32,   471,    34,    50,
    56,    69,    32,   111,    34,    34,    59,    60,   116,    84,
    85,    86,    87,    18,   392,    69,   460,    34,    34,    96,
    16,   400,    34,   100,    21,    42,    55,    43,    21,    34,
    98,    34,   534,   535,    34,    31,    16,    17,    41,    34,
    96,   485,    34,   517,    98,   519,    34,   494,   104,    91,
   497,    31,   105,   128,   129,    34,    96,   165,    98,   167,
    90,    96,   170,    99,    88,    34,   444,   142,   143,   144,
   145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
   155,   156,   157,   158,   159,   160,   161,   162,   284,    69,
   469,    88,   288,   472,   290,    88,    96,    32,    33,    34,
    35,    96,    37,    38,    96,   481,    41,    42,    59,    60,
   244,    96,    97,   491,   248,   261,   250,    17,    98,    31,
    34,   101,    34,    34,    69,   233,    56,    34,   504,    56,
   523,    31,    67,    68,    91,    92,    93,    94,    56,     7,
    32,    33,    34,    35,    79,    37,    38,   255,    83,    41,
    42,   527,    28,    98,   105,    90,   101,   281,    34,    32,
    33,    34,    35,    36,    37,    38,    96,   102,   103,    96,
    61,    62,    63,    64,   104,    67,    68,   104,    96,   287,
    69,   289,   257,   291,   259,   563,   104,    79,    31,   264,
     9,    83,     7,    12,    32,    33,    34,    35,    90,    37,
    38,    39,    40,    41,    42,    32,    33,    13,    14,    98,
   102,   103,   101,    84,    85,    35,   324,    90,    89,    90,
    91,    92,    93,    94,    88,    97,    96,    97,    31,    67,
    68,   103,   340,    96,    32,    33,    34,    35,    34,    37,
    38,    79,    26,    41,    42,    83,    88,     3,     4,     5,
     6,     8,    90,    32,    33,    27,    35,    34,    37,    38,
    96,    97,    96,   371,   102,   373,    34,    23,   343,    67,
    68,    34,   396,    34,   382,   383,    13,    14,   102,    34,
    34,    79,    96,    97,   392,    83,    96,    97,    96,    97,
    45,    46,    90,    49,    34,    51,    52,    53,    54,    96,
    97,    16,    17,    34,   102,    88,    61,    62,    63,    64,
    65,    66,   436,   102,    80,    81,    82,   102,    84,    85,
    96,    97,   430,    89,    90,    91,    92,    93,    94,    89,
    90,    91,    92,    93,    94,   459,    43,    44,    45,    46,
    47,    48,   466,    32,    33,    24,   470,   455,    96,    97,
   102,   102,   427,    10,    70,    71,    72,    73,    74,    75,
    76,    77,    78,   104,    80,    81,    82,    58,    84,    85,
   104,    87,   480,    89,    90,    91,    92,    93,    94,   104,
   104,    97,    34,    34,   508,    35,    34,   103,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    34,    80,    81,
    82,   509,    84,    85,   103,    87,    34,    89,    90,    91,
    92,    93,    94,    34,    88,    97,    11,    88,    34,    88,
   105,   103,    57,   103,    94,    88,   550,    70,    71,    72,
    73,    74,    75,    76,    77,    78,    34,    80,    81,    82,
    12,    84,    85,    88,    87,   100,    89,    90,    91,    92,
    93,    94,   105,    34,    97,    34,   105,    34,   105,    88,
   103,    70,    71,    72,    73,    74,    75,    76,    77,    78,
    88,    80,    81,    82,    25,    84,    85,    34,    87,   105,
    89,    90,    91,    92,    93,    94,    32,    43,    97,    96,
   103,    44,    98,    98,   103,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    88,    80,    81,    82,    35,    84,
    85,   105,    87,   100,    89,    90,    91,    92,    93,    94,
   105,    99,   105,    99,    34,    96,    60,   101,   103,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    59,    80,
    81,    82,    34,    84,    85,    34,    87,   101,    89,    90,
    91,    92,    93,    94,    96,    34,    34,   100,    20,    97,
    99,    21,   103,    70,    71,    72,    73,    74,    75,    76,
    77,    78,   101,    80,    81,    82,    98,    84,    85,    34,
    87,    34,    89,    90,    91,    92,    93,    94,   101,    34,
    12,    34,    19,    54,    35,    99,   103,    70,    71,    72,
    73,    74,    75,    76,    77,    78,    99,    80,    81,    82,
   100,    84,    85,    34,    87,    88,    89,    90,    91,    92,
    93,    94,    70,    71,    72,    73,    74,    75,    76,    77,
    78,   102,    80,    81,    82,   103,    84,    85,    96,    87,
    96,    89,    90,    91,    92,    93,    94,    70,    71,    72,
    73,    74,    75,    76,    77,    78,    88,    80,    81,    82,
    35,    84,    85,    88,    34,    96,    89,    90,    91,    92,
    93,    94,    70,    71,    72,    73,    74,    75,    98,     0,
     0,   115,    80,    81,    82,    19,    84,    85,   268,    48,
   315,    89,    90,    91,    92,    93,    94,   351,   354,   316,
   319,    51,    21,   568,   571,   363,   400,   280,   461,   190,
   332,   379,   252,   283,   525,   359,   503,    -1,   477
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
#line 254 "ned.y"
{ if (ps.storeSourceCode) ps.channel->setSourceCode(toString(yylsp[0])); ;
    break;}
case 6:
#line 256 "ned.y"
{ if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 7:
#line 258 "ned.y"
{ if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(yylsp[0])); ;
    break;}
case 8:
#line 260 "ned.y"
{ if (ps.storeSourceCode) ps.network->setSourceCode(toString(yylsp[0])); ;
    break;}
case 14:
#line 269 "ned.y"
{ if (ps.storeSourceCode) ps.enump->setSourceCode(toString(yylsp[0])); ;
    break;}
case 15:
#line 271 "ned.y"
{ if (ps.storeSourceCode) ps.messagep->setSourceCode(toString(yylsp[0])); ;
    break;}
case 16:
#line 273 "ned.y"
{ if (ps.storeSourceCode) ps.classp->setSourceCode(toString(yylsp[0])); ;
    break;}
case 17:
#line 275 "ned.y"
{ if (ps.storeSourceCode) ps.structp->setSourceCode(toString(yylsp[0])); ;
    break;}
case 18:
#line 283 "ned.y"
{
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,yylsp[0]);
                ;
    break;}
case 19:
#line 288 "ned.y"
{
                  // NOTE: no setTrailingComment(ps.imports,@3) -- comment already stored with last filename */
                ;
    break;}
case 22:
#line 300 "ned.y"
{
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(yylsp[0])));
                  setComments(ps.import,yylsp[0]);
                ;
    break;}
case 24:
#line 316 "ned.y"
{
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(yylsp[0]));
                  setComments(ps.channel,yylsp[-1],yylsp[0]);
                ;
    break;}
case 27:
#line 330 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 28:
#line 336 "ned.y"
{
                  ps.chanattr = addChanAttr(ps.channel,toString(yylsp[-2]));
                  addExpression(ps.chanattr, "value",yylsp[-1],yyvsp[-1]);
                  setComments(ps.channel,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 29:
#line 345 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 30:
#line 349 "ned.y"
{
                  setTrailingComment(ps.channel,yylsp[-1]);
                ;
    break;}
case 32:
#line 366 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 33:
#line 376 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 34:
#line 380 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 36:
#line 400 "ned.y"
{
                  use_chanattrname_token = 1;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(yylsp[0]));
                  setComments(ps.module,yylsp[-1],yylsp[0]);
                ;
    break;}
case 37:
#line 410 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 38:
#line 414 "ned.y"
{
                  setTrailingComment(ps.module,yylsp[-1]);
                ;
    break;}
case 41:
#line 429 "ned.y"
{
                  addDisplayString(ps.module,yylsp[-1]);
                ;
    break;}
case 44:
#line 444 "ned.y"
{
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,yylsp[-1],yylsp[0]);
                ;
    break;}
case 45:
#line 449 "ned.y"
{
                ;
    break;}
case 48:
#line 460 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 49:
#line 464 "ned.y"
{
                  setComments(ps.param,yylsp[0]);
                ;
    break;}
case 50:
#line 474 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_NUMERIC);
                ;
    break;}
case 51:
#line 478 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_NUMERIC);
                ;
    break;}
case 52:
#line 482 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[0],TYPE_CONST_NUM);
                ;
    break;}
case 53:
#line 486 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_CONST_NUM);
                ;
    break;}
case 54:
#line 490 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 55:
#line 494 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-3],TYPE_CONST_NUM);
                ;
    break;}
case 56:
#line 498 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_STRING);
                ;
    break;}
case 57:
#line 502 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_BOOL);
                ;
    break;}
case 58:
#line 506 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_XML);
                ;
    break;}
case 59:
#line 510 "ned.y"
{
                  ps.param = addParameter(ps.params,yylsp[-2],TYPE_ANYTYPE);
                ;
    break;}
case 62:
#line 525 "ned.y"
{
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,yylsp[-1],yylsp[0]);
                ;
    break;}
case 63:
#line 530 "ned.y"
{
                ;
    break;}
case 72:
#line 553 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 1, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 73:
#line 558 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 1, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 76:
#line 571 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[-2], 0, 1 );
                  setComments(ps.gate,yylsp[-2],yylsp[0]);
                ;
    break;}
case 77:
#line 576 "ned.y"
{
                  ps.gate = addGate(ps.gates, yylsp[0], 0, 0 );
                  setComments(ps.gate,yylsp[0]);
                ;
    break;}
case 80:
#line 592 "ned.y"
{
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,yylsp[-1],yylsp[0]);
                ;
    break;}
case 81:
#line 597 "ned.y"
{
                ;
    break;}
case 86:
#line 613 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-3], yylsp[-1], NULLPOS);
                  setComments(ps.submod,yylsp[-3],yylsp[0]);
                ;
    break;}
case 87:
#line 618 "ned.y"
{
                ;
    break;}
case 88:
#line 621 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-4], yylsp[-2], NULLPOS);
                  addVector(ps.submod, "vector-size",yylsp[-1],yyvsp[-1]);
                  setComments(ps.submod,yylsp[-4],yylsp[0]);
                ;
    break;}
case 89:
#line 627 "ned.y"
{
                ;
    break;}
case 90:
#line 630 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-5], yylsp[-1], yylsp[-3]);
                  setComments(ps.submod,yylsp[-5],yylsp[0]);
                ;
    break;}
case 91:
#line 635 "ned.y"
{
                ;
    break;}
case 92:
#line 638 "ned.y"
{
                  ps.submod = addSubmodule(ps.submods, yylsp[-6], yylsp[-1], yylsp[-4]);
                  addVector(ps.submod, "vector-size",yylsp[-3],yyvsp[-3]);
                  setComments(ps.submod,yylsp[-6],yylsp[0]);
                ;
    break;}
case 93:
#line 644 "ned.y"
{
                ;
    break;}
case 99:
#line 669 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,yylsp[-1],yylsp[0]);
                ;
    break;}
case 100:
#line 674 "ned.y"
{
                ;
    break;}
case 101:
#line 677 "ned.y"
{
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.substparams,yylsp[-3],yylsp[0]);
                ;
    break;}
case 102:
#line 683 "ned.y"
{
                ;
    break;}
case 107:
#line 700 "ned.y"
{
                  ps.substparam = addSubstparam(ps.substparams,yylsp[-2]);
                  addExpression(ps.substparam, "value",yylsp[0],yyvsp[0]);
                  setComments(ps.substparam,yylsp[-2],yylsp[0]);
                ;
    break;}
case 110:
#line 717 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,yylsp[-1],yylsp[0]);
                ;
    break;}
case 111:
#line 722 "ned.y"
{
                ;
    break;}
case 112:
#line 725 "ned.y"
{
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",yylsp[-1],yyvsp[-1]);
                  setComments(ps.gatesizes,yylsp[-3],yylsp[0]);
                ;
    break;}
case 113:
#line 731 "ned.y"
{
                ;
    break;}
case 118:
#line 747 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[-1]);
                  addVector(ps.gatesize, "vector-size",yylsp[0],yyvsp[0]);

                  setComments(ps.gatesize,yylsp[-1],yylsp[0]);
                ;
    break;}
case 119:
#line 754 "ned.y"
{
                  ps.gatesize = addGateSize(ps.gatesizes,yylsp[0]);
                  setComments(ps.gatesize,yylsp[0]);
                ;
    break;}
case 120:
#line 765 "ned.y"
{
                  addDisplayString(ps.submod,yylsp[-1]);
                ;
    break;}
case 124:
#line 781 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,yylsp[-2],yylsp[0]);
                ;
    break;}
case 125:
#line 787 "ned.y"
{
                ;
    break;}
case 126:
#line 790 "ned.y"
{
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,yylsp[-1],yylsp[0]);
                ;
    break;}
case 127:
#line 796 "ned.y"
{
                ;
    break;}
case 134:
#line 817 "ned.y"
{
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                ;
    break;}
case 135:
#line 822 "ned.y"
{
                  ps.inLoop=0;
                  setComments(ps.forloop,yylsp[-6],yylsp[-3]);
                  setTrailingComment(ps.forloop,yylsp[-1]);
                ;
    break;}
case 138:
#line 836 "ned.y"
{
                  ps.loopvar = addLoopVar(ps.forloop,yylsp[-4]);
                  addExpression(ps.loopvar, "from-value",yylsp[-2],yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",yylsp[0],yyvsp[0]);
                  setComments(ps.loopvar,yylsp[-4],yylsp[0]);
                ;
    break;}
case 139:
#line 846 "ned.y"
{
                  addExpression(ps.conn, "condition",yylsp[0],yyvsp[0]);
                ;
    break;}
case 141:
#line 854 "ned.y"
{
                  addDisplayString(ps.conn,yylsp[0]);
                ;
    break;}
case 145:
#line 867 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 146:
#line 872 "ned.y"
{
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 147:
#line 877 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-5],yylsp[-1]);
                ;
    break;}
case 148:
#line 883 "ned.y"
{
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 151:
#line 897 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "src-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 152:
#line 903 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(yylsp[0]) );
                ;
    break;}
case 153:
#line 911 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 154:
#line 916 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[0]) );
                ;
    break;}
case 155:
#line 920 "ned.y"
{
                  ps.conn->setSrcGate( toString( yylsp[-1]) );
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 156:
#line 928 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  addVector(ps.conn, "src-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 157:
#line 935 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[0]));
                ;
    break;}
case 158:
#line 941 "ned.y"
{
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(yylsp[-1]));
                  ps.conn->setSrcGatePlusplus(true);
                ;
    break;}
case 161:
#line 956 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[-1]) );
                  addVector(ps.conn, "dest-module-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 162:
#line 961 "ned.y"
{
                  ps.conn->setDestModule( toString(yylsp[0]) );
                ;
    break;}
case 163:
#line 968 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 164:
#line 973 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 165:
#line 977 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 166:
#line 985 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  addVector(ps.conn, "dest-gate-index",yylsp[0],yyvsp[0]);
                ;
    break;}
case 167:
#line 990 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[0]) );
                ;
    break;}
case 168:
#line 994 "ned.y"
{
                  ps.conn->setDestGate( toString( yylsp[-1]) );
                  ps.conn->setDestGatePlusplus(true);
                ;
    break;}
case 169:
#line 1003 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",yylsp[0],createExpression(createConst(NED_CONST_STRING, toString(yylsp[0]))));
                ;
    break;}
case 170:
#line 1008 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 171:
#line 1013 "ned.y"
{
                  ps.connattr = addConnAttr(ps.conn,toString(yylsp[-1]));
                  addExpression(ps.connattr, "value",yylsp[0],yyvsp[0]);
                ;
    break;}
case 173:
#line 1030 "ned.y"
{
                  ps.network = addNetwork(ps.nedfile,yylsp[-3],yylsp[-1]);
                  setComments(ps.network,yylsp[-4],yylsp[0]);
                  ps.inNetwork=1;
                ;
    break;}
case 174:
#line 1039 "ned.y"
{
                  setTrailingComment(ps.network,yylsp[-1]);
                  ps.inNetwork=0;
                ;
    break;}
case 175:
#line 1051 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 176:
#line 1057 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 177:
#line 1061 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 178:
#line 1065 "ned.y"
{
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                ;
    break;}
case 179:
#line 1076 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 180:
#line 1078 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); ;
    break;}
case 181:
#line 1080 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 182:
#line 1082 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("input"); ;
    break;}
case 183:
#line 1087 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-3], yyvsp[-1]); ;
    break;}
case 184:
#line 1089 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-1]); ;
    break;}
case 186:
#line 1095 "ned.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 187:
#line 1098 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); ;
    break;}
case 188:
#line 1100 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); ;
    break;}
case 189:
#line 1102 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); ;
    break;}
case 190:
#line 1104 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); ;
    break;}
case 191:
#line 1106 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); ;
    break;}
case 192:
#line 1108 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); ;
    break;}
case 193:
#line 1112 "ned.y"
{ if (ps.parseExpressions) yyval = unaryMinus(yyvsp[0]); ;
    break;}
case 194:
#line 1115 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); ;
    break;}
case 195:
#line 1117 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 196:
#line 1119 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); ;
    break;}
case 197:
#line 1121 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 198:
#line 1123 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 199:
#line 1125 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); ;
    break;}
case 200:
#line 1128 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 201:
#line 1130 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); ;
    break;}
case 202:
#line 1132 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); ;
    break;}
case 203:
#line 1136 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); ;
    break;}
case 204:
#line 1139 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); ;
    break;}
case 205:
#line 1141 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); ;
    break;}
case 206:
#line 1143 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); ;
    break;}
case 207:
#line 1147 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); ;
    break;}
case 208:
#line 1149 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); ;
    break;}
case 209:
#line 1151 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); ;
    break;}
case 210:
#line 1153 "ned.y"
{ if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); ;
    break;}
case 211:
#line 1156 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-2])); ;
    break;}
case 212:
#line 1158 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-3]), yyvsp[-1]); ;
    break;}
case 213:
#line 1160 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-5]), yyvsp[-3], yyvsp[-1]); ;
    break;}
case 214:
#line 1162 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-7]), yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 215:
#line 1164 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction(toString(yylsp[-9]), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 221:
#line 1177 "ned.y"
{
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString(yylsp[0]));
                ;
    break;}
case 222:
#line 1182 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((RefNode *)yyval)->setIsRef(true);
                ;
    break;}
case 223:
#line 1187 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((RefNode *)yyval)->setIsRef(true);
                  ((RefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 224:
#line 1193 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((RefNode *)yyval)->setIsRef(true);
                  ((RefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 225:
#line 1199 "ned.y"
{
                  yyval = createParamRef(toString(yylsp[0]));
                  ((RefNode *)yyval)->setIsAncestor(true);
                ;
    break;}
case 227:
#line 1211 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "true"); ;
    break;}
case 228:
#line 1213 "ned.y"
{ yyval = createConst(NED_CONST_BOOL, "false"); ;
    break;}
case 230:
#line 1222 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 231:
#line 1224 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("index"); ;
    break;}
case 232:
#line 1226 "ned.y"
{ if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString(yylsp[-1]))); ;
    break;}
case 233:
#line 1231 "ned.y"
{ yyval = createConst(NED_CONST_STRING, toString(trimQuotes(yylsp[0]))); ;
    break;}
case 234:
#line 1236 "ned.y"
{ yyval = createConst(NED_CONST_INT, toString(yylsp[0])); ;
    break;}
case 235:
#line 1238 "ned.y"
{ yyval = createConst(NED_CONST_REAL, toString(yylsp[0])); ;
    break;}
case 236:
#line 1240 "ned.y"
{ yyval = createTimeConst(toString(yylsp[0])); ;
    break;}
case 241:
#line 1257 "ned.y"
{
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(yylsp[-1])));
                  setComments(ps.cplusplus,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 242:
#line 1266 "ned.y"
{
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(yylsp[-1]));
                  setComments(ps.structdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 243:
#line 1275 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(yylsp[-1]));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 244:
#line 1282 "ned.y"
{
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(yylsp[-1]));
                  setComments(ps.classdecl,yylsp[-3],yylsp[-2]);
                ;
    break;}
case 245:
#line 1292 "ned.y"
{
                  ps.messagedecl = (MessageDeclNode *)createNodeWithTag(NED_MESSAGE_DECL, ps.nedfile );
                  ps.messagedecl->setName(toString(yylsp[-1]));
                  setComments(ps.messagedecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 246:
#line 1301 "ned.y"
{
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString(yylsp[-1]));
                  setComments(ps.enumdecl,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 247:
#line 1310 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-2],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 248:
#line 1317 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 249:
#line 1321 "ned.y"
{
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(yylsp[-3]));
                  ps.enump->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.enump,yylsp[-4],yylsp[-1]);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                ;
    break;}
case 250:
#line 1329 "ned.y"
{
                  setTrailingComment(ps.enump,yylsp[-1]);
                ;
    break;}
case 255:
#line 1346 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-1],yylsp[-1]);
                ;
    break;}
case 256:
#line 1352 "ned.y"
{
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(yylsp[-3]));
                  ps.enumfield->setValue(toString(yylsp[-1]));
                  setComments(ps.enumfield,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 257:
#line 1362 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 258:
#line 1368 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 259:
#line 1372 "ned.y"
{
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(yylsp[-3]));
                  ps.messagep->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.messagep,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 260:
#line 1379 "ned.y"
{
                  setTrailingComment(ps.messagep,yylsp[-1]);
                ;
    break;}
case 261:
#line 1386 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 262:
#line 1392 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 263:
#line 1396 "ned.y"
{
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(yylsp[-3]));
                  ps.classp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.classp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 264:
#line 1403 "ned.y"
{
                  setTrailingComment(ps.classp,yylsp[-1]);
                ;
    break;}
case 265:
#line 1410 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-2],yylsp[-1]);
                ;
    break;}
case 266:
#line 1416 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 267:
#line 1420 "ned.y"
{
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(yylsp[-3]));
                  ps.structp->setExtendsName(toString(yylsp[-1]));
                  setComments(ps.structp,yylsp[-4],yylsp[-1]);
                ;
    break;}
case 268:
#line 1427 "ned.y"
{
                  setTrailingComment(ps.structp,yylsp[-1]);
                ;
    break;}
case 269:
#line 1434 "ned.y"
{
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,yylsp[-1]);
                ;
    break;}
case 276:
#line 1454 "ned.y"
{
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(yylsp[-3]));
                  ps.property->setValue(toString(yylsp[-1]));
                  setComments(ps.property,yylsp[-3],yylsp[-1]);
                ;
    break;}
case 283:
#line 1473 "ned.y"
{
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,yylsp[-1]);
                ;
    break;}
case 290:
#line 1493 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setDataType(toString(yylsp[-1]));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                ;
    break;}
case 291:
#line 1501 "ned.y"
{
                  setComments(ps.field,yylsp[-7],yylsp[-1]);
                ;
    break;}
case 292:
#line 1505 "ned.y"
{
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(yylsp[0]));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                ;
    break;}
case 293:
#line 1512 "ned.y"
{
                  setComments(ps.field,yylsp[-6],yylsp[-1]);
                ;
    break;}
case 294:
#line 1519 "ned.y"
{ ps.isAbstract = true; ps.isReadonly = false; ;
    break;}
case 295:
#line 1521 "ned.y"
{ ps.isAbstract = false; ps.isReadonly = true; ;
    break;}
case 296:
#line 1523 "ned.y"
{ ps.isAbstract = true; ps.isReadonly = true; ;
    break;}
case 297:
#line 1525 "ned.y"
{ ps.isAbstract = true; ps.isReadonly = true; ;
    break;}
case 298:
#line 1527 "ned.y"
{ ps.isAbstract = false; ps.isReadonly = false; ;
    break;}
case 312:
#line 1552 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 313:
#line 1557 "ned.y"
{
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(yylsp[-1]));
                ;
    break;}
case 314:
#line 1562 "ned.y"
{
                  ps.field->setIsVector(true);
                ;
    break;}
case 316:
#line 1570 "ned.y"
{
                  ps.field->setEnumName(toString(yylsp[-1]));
                ;
    break;}
case 318:
#line 1578 "ned.y"
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
#line 1607 "ned.y"


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

RefNode *createParamRef(const char *param, const char *paramindex, const char *module, const char *moduleindex)
{
   RefNode *par = (RefNode *)createNodeWithTag(NED_PARAM_REF);
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
