/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         nedyyparse
#define yylex           nedyylex
#define yyerror         nedyyerror
#define yydebug         nedyydebug
#define yynerrs         nedyynerrs
#define yylval          nedyylval
#define yychar          nedyychar
#define yylloc          nedyylloc

/* First part of user prologue.  */
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


#line 242 "ned.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "ned.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INCLUDE = 3,                    /* INCLUDE  */
  YYSYMBOL_SIMPLE = 4,                     /* SIMPLE  */
  YYSYMBOL_CHANNEL = 5,                    /* CHANNEL  */
  YYSYMBOL_MODULE = 6,                     /* MODULE  */
  YYSYMBOL_PARAMETERS = 7,                 /* PARAMETERS  */
  YYSYMBOL_GATES = 8,                      /* GATES  */
  YYSYMBOL_GATESIZES = 9,                  /* GATESIZES  */
  YYSYMBOL_SUBMODULES = 10,                /* SUBMODULES  */
  YYSYMBOL_CONNECTIONS = 11,               /* CONNECTIONS  */
  YYSYMBOL_DISPLAY = 12,                   /* DISPLAY  */
  YYSYMBOL_IN = 13,                        /* IN  */
  YYSYMBOL_OUT = 14,                       /* OUT  */
  YYSYMBOL_NOCHECK = 15,                   /* NOCHECK  */
  YYSYMBOL_LEFT_ARROW = 16,                /* LEFT_ARROW  */
  YYSYMBOL_RIGHT_ARROW = 17,               /* RIGHT_ARROW  */
  YYSYMBOL_FOR = 18,                       /* FOR  */
  YYSYMBOL_TO = 19,                        /* TO  */
  YYSYMBOL_DO = 20,                        /* DO  */
  YYSYMBOL_IF = 21,                        /* IF  */
  YYSYMBOL_LIKE = 22,                      /* LIKE  */
  YYSYMBOL_NETWORK = 23,                   /* NETWORK  */
  YYSYMBOL_ENDSIMPLE = 24,                 /* ENDSIMPLE  */
  YYSYMBOL_ENDMODULE = 25,                 /* ENDMODULE  */
  YYSYMBOL_ENDCHANNEL = 26,                /* ENDCHANNEL  */
  YYSYMBOL_ENDNETWORK = 27,                /* ENDNETWORK  */
  YYSYMBOL_ENDFOR = 28,                    /* ENDFOR  */
  YYSYMBOL_MACHINES = 29,                  /* MACHINES  */
  YYSYMBOL_ON = 30,                        /* ON  */
  YYSYMBOL_CHANATTRNAME = 31,              /* CHANATTRNAME  */
  YYSYMBOL_INTCONSTANT = 32,               /* INTCONSTANT  */
  YYSYMBOL_REALCONSTANT = 33,              /* REALCONSTANT  */
  YYSYMBOL_NAME = 34,                      /* NAME  */
  YYSYMBOL_STRINGCONSTANT = 35,            /* STRINGCONSTANT  */
  YYSYMBOL_CHARCONSTANT = 36,              /* CHARCONSTANT  */
  YYSYMBOL_TRUE_ = 37,                     /* TRUE_  */
  YYSYMBOL_FALSE_ = 38,                    /* FALSE_  */
  YYSYMBOL_INPUT_ = 39,                    /* INPUT_  */
  YYSYMBOL_XMLDOC = 40,                    /* XMLDOC  */
  YYSYMBOL_REF = 41,                       /* REF  */
  YYSYMBOL_ANCESTOR = 42,                  /* ANCESTOR  */
  YYSYMBOL_CONSTDECL = 43,                 /* CONSTDECL  */
  YYSYMBOL_NUMERICTYPE = 44,               /* NUMERICTYPE  */
  YYSYMBOL_STRINGTYPE = 45,                /* STRINGTYPE  */
  YYSYMBOL_BOOLTYPE = 46,                  /* BOOLTYPE  */
  YYSYMBOL_XMLTYPE = 47,                   /* XMLTYPE  */
  YYSYMBOL_ANYTYPE = 48,                   /* ANYTYPE  */
  YYSYMBOL_CPLUSPLUS = 49,                 /* CPLUSPLUS  */
  YYSYMBOL_CPLUSPLUSBODY = 50,             /* CPLUSPLUSBODY  */
  YYSYMBOL_MESSAGE = 51,                   /* MESSAGE  */
  YYSYMBOL_CLASS = 52,                     /* CLASS  */
  YYSYMBOL_STRUCT = 53,                    /* STRUCT  */
  YYSYMBOL_ENUM = 54,                      /* ENUM  */
  YYSYMBOL_NONCOBJECT = 55,                /* NONCOBJECT  */
  YYSYMBOL_EXTENDS = 56,                   /* EXTENDS  */
  YYSYMBOL_FIELDS = 57,                    /* FIELDS  */
  YYSYMBOL_PROPERTIES = 58,                /* PROPERTIES  */
  YYSYMBOL_ABSTRACT = 59,                  /* ABSTRACT  */
  YYSYMBOL_READONLY = 60,                  /* READONLY  */
  YYSYMBOL_CHARTYPE = 61,                  /* CHARTYPE  */
  YYSYMBOL_SHORTTYPE = 62,                 /* SHORTTYPE  */
  YYSYMBOL_INTTYPE = 63,                   /* INTTYPE  */
  YYSYMBOL_LONGTYPE = 64,                  /* LONGTYPE  */
  YYSYMBOL_DOUBLETYPE = 65,                /* DOUBLETYPE  */
  YYSYMBOL_UNSIGNED_ = 66,                 /* UNSIGNED_  */
  YYSYMBOL_SIZEOF = 67,                    /* SIZEOF  */
  YYSYMBOL_SUBMODINDEX = 68,               /* SUBMODINDEX  */
  YYSYMBOL_PLUSPLUS = 69,                  /* PLUSPLUS  */
  YYSYMBOL_EQ = 70,                        /* EQ  */
  YYSYMBOL_NE = 71,                        /* NE  */
  YYSYMBOL_GT = 72,                        /* GT  */
  YYSYMBOL_GE = 73,                        /* GE  */
  YYSYMBOL_LS = 74,                        /* LS  */
  YYSYMBOL_LE = 75,                        /* LE  */
  YYSYMBOL_AND = 76,                       /* AND  */
  YYSYMBOL_OR = 77,                        /* OR  */
  YYSYMBOL_XOR = 78,                       /* XOR  */
  YYSYMBOL_NOT = 79,                       /* NOT  */
  YYSYMBOL_BIN_AND = 80,                   /* BIN_AND  */
  YYSYMBOL_BIN_OR = 81,                    /* BIN_OR  */
  YYSYMBOL_BIN_XOR = 82,                   /* BIN_XOR  */
  YYSYMBOL_BIN_COMPL = 83,                 /* BIN_COMPL  */
  YYSYMBOL_SHIFT_LEFT = 84,                /* SHIFT_LEFT  */
  YYSYMBOL_SHIFT_RIGHT = 85,               /* SHIFT_RIGHT  */
  YYSYMBOL_INVALID_CHAR = 86,              /* INVALID_CHAR  */
  YYSYMBOL_87_ = 87,                       /* '?'  */
  YYSYMBOL_88_ = 88,                       /* ':'  */
  YYSYMBOL_89_ = 89,                       /* '+'  */
  YYSYMBOL_90_ = 90,                       /* '-'  */
  YYSYMBOL_91_ = 91,                       /* '*'  */
  YYSYMBOL_92_ = 92,                       /* '/'  */
  YYSYMBOL_93_ = 93,                       /* '%'  */
  YYSYMBOL_94_ = 94,                       /* '^'  */
  YYSYMBOL_UMIN = 95,                      /* UMIN  */
  YYSYMBOL_96_ = 96,                       /* ';'  */
  YYSYMBOL_97_ = 97,                       /* ','  */
  YYSYMBOL_98_ = 98,                       /* '{'  */
  YYSYMBOL_99_ = 99,                       /* '}'  */
  YYSYMBOL_100_ = 100,                     /* '='  */
  YYSYMBOL_101_ = 101,                     /* '['  */
  YYSYMBOL_102_ = 102,                     /* ']'  */
  YYSYMBOL_103_ = 103,                     /* '.'  */
  YYSYMBOL_104_ = 104,                     /* '('  */
  YYSYMBOL_105_ = 105,                     /* ')'  */
  YYSYMBOL_YYACCEPT = 106,                 /* $accept  */
  YYSYMBOL_networkdescription = 107,       /* networkdescription  */
  YYSYMBOL_somedefinitions = 108,          /* somedefinitions  */
  YYSYMBOL_definition = 109,               /* definition  */
  YYSYMBOL_import = 110,                   /* import  */
  YYSYMBOL_111_1 = 111,                    /* $@1  */
  YYSYMBOL_filenames = 112,                /* filenames  */
  YYSYMBOL_filename = 113,                 /* filename  */
  YYSYMBOL_channeldefinition_old = 114,    /* channeldefinition_old  */
  YYSYMBOL_channelheader_old = 115,        /* channelheader_old  */
  YYSYMBOL_opt_channelattrblock_old = 116, /* opt_channelattrblock_old  */
  YYSYMBOL_channelattrblock_old = 117,     /* channelattrblock_old  */
  YYSYMBOL_endchannel_old = 118,           /* endchannel_old  */
  YYSYMBOL_channeldefinition = 119,        /* channeldefinition  */
  YYSYMBOL_channelheader = 120,            /* channelheader  */
  YYSYMBOL_opt_channelattrblock = 121,     /* opt_channelattrblock  */
  YYSYMBOL_channelattrblock = 122,         /* channelattrblock  */
  YYSYMBOL_simpledefinition_old = 123,     /* simpledefinition_old  */
  YYSYMBOL_simpleheader_old = 124,         /* simpleheader_old  */
  YYSYMBOL_endsimple_old = 125,            /* endsimple_old  */
  YYSYMBOL_simpledefinition = 126,         /* simpledefinition  */
  YYSYMBOL_simpleheader = 127,             /* simpleheader  */
  YYSYMBOL_moduledefinition_old = 128,     /* moduledefinition_old  */
  YYSYMBOL_moduleheader_old = 129,         /* moduleheader_old  */
  YYSYMBOL_endmodule_old = 130,            /* endmodule_old  */
  YYSYMBOL_moduledefinition = 131,         /* moduledefinition  */
  YYSYMBOL_moduleheader = 132,             /* moduleheader  */
  YYSYMBOL_opt_machineblock_old = 133,     /* opt_machineblock_old  */
  YYSYMBOL_machineblock_old = 134,         /* machineblock_old  */
  YYSYMBOL_135_2 = 135,                    /* $@2  */
  YYSYMBOL_opt_machinelist_old = 136,      /* opt_machinelist_old  */
  YYSYMBOL_machinelist_old = 137,          /* machinelist_old  */
  YYSYMBOL_machine_old = 138,              /* machine_old  */
  YYSYMBOL_opt_machineblock = 139,         /* opt_machineblock  */
  YYSYMBOL_machineblock = 140,             /* machineblock  */
  YYSYMBOL_141_3 = 141,                    /* $@3  */
  YYSYMBOL_opt_machinelist = 142,          /* opt_machinelist  */
  YYSYMBOL_machinelist = 143,              /* machinelist  */
  YYSYMBOL_machine = 144,                  /* machine  */
  YYSYMBOL_opt_displayblock_old = 145,     /* opt_displayblock_old  */
  YYSYMBOL_displayblock_old = 146,         /* displayblock_old  */
  YYSYMBOL_opt_displayblock = 147,         /* opt_displayblock  */
  YYSYMBOL_displayblock = 148,             /* displayblock  */
  YYSYMBOL_opt_paramblock_old = 149,       /* opt_paramblock_old  */
  YYSYMBOL_paramblock_old = 150,           /* paramblock_old  */
  YYSYMBOL_151_4 = 151,                    /* $@4  */
  YYSYMBOL_opt_parameters_old = 152,       /* opt_parameters_old  */
  YYSYMBOL_parameters_old = 153,           /* parameters_old  */
  YYSYMBOL_opt_paramblock = 154,           /* opt_paramblock  */
  YYSYMBOL_paramblock = 155,               /* paramblock  */
  YYSYMBOL_156_5 = 156,                    /* $@5  */
  YYSYMBOL_opt_parameters = 157,           /* opt_parameters  */
  YYSYMBOL_parameters = 158,               /* parameters  */
  YYSYMBOL_parameter_old = 159,            /* parameter_old  */
  YYSYMBOL_parameter = 160,                /* parameter  */
  YYSYMBOL_opt_gateblock_old = 161,        /* opt_gateblock_old  */
  YYSYMBOL_gateblock_old = 162,            /* gateblock_old  */
  YYSYMBOL_163_6 = 163,                    /* $@6  */
  YYSYMBOL_opt_gates_old = 164,            /* opt_gates_old  */
  YYSYMBOL_gates_old = 165,                /* gates_old  */
  YYSYMBOL_gatesI_old = 166,               /* gatesI_old  */
  YYSYMBOL_gateI_old = 167,                /* gateI_old  */
  YYSYMBOL_gatesO_old = 168,               /* gatesO_old  */
  YYSYMBOL_gateO_old = 169,                /* gateO_old  */
  YYSYMBOL_opt_gateblock = 170,            /* opt_gateblock  */
  YYSYMBOL_gateblock = 171,                /* gateblock  */
  YYSYMBOL_172_7 = 172,                    /* $@7  */
  YYSYMBOL_opt_gates = 173,                /* opt_gates  */
  YYSYMBOL_gates = 174,                    /* gates  */
  YYSYMBOL_gatesI = 175,                   /* gatesI  */
  YYSYMBOL_gateI = 176,                    /* gateI  */
  YYSYMBOL_gatesO = 177,                   /* gatesO  */
  YYSYMBOL_gateO = 178,                    /* gateO  */
  YYSYMBOL_opt_submodblock_old = 179,      /* opt_submodblock_old  */
  YYSYMBOL_submodblock_old = 180,          /* submodblock_old  */
  YYSYMBOL_181_8 = 181,                    /* $@8  */
  YYSYMBOL_opt_submodules_old = 182,       /* opt_submodules_old  */
  YYSYMBOL_submodules_old = 183,           /* submodules_old  */
  YYSYMBOL_submodule_old = 184,            /* submodule_old  */
  YYSYMBOL_185_9 = 185,                    /* $@9  */
  YYSYMBOL_186_10 = 186,                   /* $@10  */
  YYSYMBOL_187_11 = 187,                   /* $@11  */
  YYSYMBOL_188_12 = 188,                   /* $@12  */
  YYSYMBOL_submodule_body_old = 189,       /* submodule_body_old  */
  YYSYMBOL_opt_submodblock = 190,          /* opt_submodblock  */
  YYSYMBOL_submodblock = 191,              /* submodblock  */
  YYSYMBOL_192_13 = 192,                   /* $@13  */
  YYSYMBOL_opt_submodules = 193,           /* opt_submodules  */
  YYSYMBOL_submodules = 194,               /* submodules  */
  YYSYMBOL_submodule = 195,                /* submodule  */
  YYSYMBOL_196_14 = 196,                   /* $@14  */
  YYSYMBOL_197_15 = 197,                   /* $@15  */
  YYSYMBOL_198_16 = 198,                   /* $@16  */
  YYSYMBOL_199_17 = 199,                   /* $@17  */
  YYSYMBOL_submodule_body = 200,           /* submodule_body  */
  YYSYMBOL_opt_on_blocks_old = 201,        /* opt_on_blocks_old  */
  YYSYMBOL_on_blocks_old = 202,            /* on_blocks_old  */
  YYSYMBOL_on_block_old = 203,             /* on_block_old  */
  YYSYMBOL_204_18 = 204,                   /* $@18  */
  YYSYMBOL_205_19 = 205,                   /* $@19  */
  YYSYMBOL_opt_on_list_old = 206,          /* opt_on_list_old  */
  YYSYMBOL_on_list_old = 207,              /* on_list_old  */
  YYSYMBOL_on_mach_old = 208,              /* on_mach_old  */
  YYSYMBOL_opt_on_blocks = 209,            /* opt_on_blocks  */
  YYSYMBOL_on_blocks = 210,                /* on_blocks  */
  YYSYMBOL_on_block = 211,                 /* on_block  */
  YYSYMBOL_212_20 = 212,                   /* $@20  */
  YYSYMBOL_213_21 = 213,                   /* $@21  */
  YYSYMBOL_opt_on_list = 214,              /* opt_on_list  */
  YYSYMBOL_on_list = 215,                  /* on_list  */
  YYSYMBOL_on_mach = 216,                  /* on_mach  */
  YYSYMBOL_opt_substparamblocks_old = 217, /* opt_substparamblocks_old  */
  YYSYMBOL_substparamblocks_old = 218,     /* substparamblocks_old  */
  YYSYMBOL_substparamblock_old = 219,      /* substparamblock_old  */
  YYSYMBOL_220_22 = 220,                   /* $@22  */
  YYSYMBOL_221_23 = 221,                   /* $@23  */
  YYSYMBOL_opt_substparameters_old = 222,  /* opt_substparameters_old  */
  YYSYMBOL_substparameters_old = 223,      /* substparameters_old  */
  YYSYMBOL_substparameter_old = 224,       /* substparameter_old  */
  YYSYMBOL_opt_substparamblocks = 225,     /* opt_substparamblocks  */
  YYSYMBOL_substparamblocks = 226,         /* substparamblocks  */
  YYSYMBOL_substparamblock = 227,          /* substparamblock  */
  YYSYMBOL_228_24 = 228,                   /* $@24  */
  YYSYMBOL_229_25 = 229,                   /* $@25  */
  YYSYMBOL_opt_substparameters = 230,      /* opt_substparameters  */
  YYSYMBOL_substparameters = 231,          /* substparameters  */
  YYSYMBOL_substparameter = 232,           /* substparameter  */
  YYSYMBOL_opt_gatesizeblocks_old = 233,   /* opt_gatesizeblocks_old  */
  YYSYMBOL_gatesizeblock_old = 234,        /* gatesizeblock_old  */
  YYSYMBOL_235_26 = 235,                   /* $@26  */
  YYSYMBOL_236_27 = 236,                   /* $@27  */
  YYSYMBOL_opt_gatesizes_old = 237,        /* opt_gatesizes_old  */
  YYSYMBOL_gatesizes_old = 238,            /* gatesizes_old  */
  YYSYMBOL_gatesize_old = 239,             /* gatesize_old  */
  YYSYMBOL_opt_gatesizeblocks = 240,       /* opt_gatesizeblocks  */
  YYSYMBOL_gatesizeblock = 241,            /* gatesizeblock  */
  YYSYMBOL_242_28 = 242,                   /* $@28  */
  YYSYMBOL_243_29 = 243,                   /* $@29  */
  YYSYMBOL_opt_gatesizes = 244,            /* opt_gatesizes  */
  YYSYMBOL_gatesizes = 245,                /* gatesizes  */
  YYSYMBOL_gatesize = 246,                 /* gatesize  */
  YYSYMBOL_opt_submod_displayblock_old = 247, /* opt_submod_displayblock_old  */
  YYSYMBOL_opt_submod_displayblock = 248,  /* opt_submod_displayblock  */
  YYSYMBOL_opt_connblock_old = 249,        /* opt_connblock_old  */
  YYSYMBOL_connblock_old = 250,            /* connblock_old  */
  YYSYMBOL_251_30 = 251,                   /* $@30  */
  YYSYMBOL_252_31 = 252,                   /* $@31  */
  YYSYMBOL_opt_connections_old = 253,      /* opt_connections_old  */
  YYSYMBOL_connections_old = 254,          /* connections_old  */
  YYSYMBOL_connection_old = 255,           /* connection_old  */
  YYSYMBOL_loopconnection_old = 256,       /* loopconnection_old  */
  YYSYMBOL_257_32 = 257,                   /* $@32  */
  YYSYMBOL_loopvarlist_old = 258,          /* loopvarlist_old  */
  YYSYMBOL_loopvar_old = 259,              /* loopvar_old  */
  YYSYMBOL_opt_conncondition_old = 260,    /* opt_conncondition_old  */
  YYSYMBOL_opt_conn_displaystr_old = 261,  /* opt_conn_displaystr_old  */
  YYSYMBOL_notloopconnections_old = 262,   /* notloopconnections_old  */
  YYSYMBOL_notloopconnection_old = 263,    /* notloopconnection_old  */
  YYSYMBOL_leftgatespec_old = 264,         /* leftgatespec_old  */
  YYSYMBOL_leftmod_old = 265,              /* leftmod_old  */
  YYSYMBOL_leftgate_old = 266,             /* leftgate_old  */
  YYSYMBOL_parentleftgate_old = 267,       /* parentleftgate_old  */
  YYSYMBOL_rightgatespec_old = 268,        /* rightgatespec_old  */
  YYSYMBOL_rightmod_old = 269,             /* rightmod_old  */
  YYSYMBOL_rightgate_old = 270,            /* rightgate_old  */
  YYSYMBOL_parentrightgate_old = 271,      /* parentrightgate_old  */
  YYSYMBOL_channeldescr_old = 272,         /* channeldescr_old  */
  YYSYMBOL_opt_connblock = 273,            /* opt_connblock  */
  YYSYMBOL_connblock = 274,                /* connblock  */
  YYSYMBOL_275_33 = 275,                   /* $@33  */
  YYSYMBOL_276_34 = 276,                   /* $@34  */
  YYSYMBOL_opt_connections = 277,          /* opt_connections  */
  YYSYMBOL_connections = 278,              /* connections  */
  YYSYMBOL_connection = 279,               /* connection  */
  YYSYMBOL_loopconnection = 280,           /* loopconnection  */
  YYSYMBOL_281_35 = 281,                   /* $@35  */
  YYSYMBOL_loopvarlist = 282,              /* loopvarlist  */
  YYSYMBOL_loopvar = 283,                  /* loopvar  */
  YYSYMBOL_opt_conncondition = 284,        /* opt_conncondition  */
  YYSYMBOL_opt_conn_displaystr = 285,      /* opt_conn_displaystr  */
  YYSYMBOL_notloopconnections = 286,       /* notloopconnections  */
  YYSYMBOL_notloopconnection = 287,        /* notloopconnection  */
  YYSYMBOL_leftgatespec = 288,             /* leftgatespec  */
  YYSYMBOL_leftmod = 289,                  /* leftmod  */
  YYSYMBOL_leftgate = 290,                 /* leftgate  */
  YYSYMBOL_parentleftgate = 291,           /* parentleftgate  */
  YYSYMBOL_rightgatespec = 292,            /* rightgatespec  */
  YYSYMBOL_rightmod = 293,                 /* rightmod  */
  YYSYMBOL_rightgate = 294,                /* rightgate  */
  YYSYMBOL_parentrightgate = 295,          /* parentrightgate  */
  YYSYMBOL_channeldescr = 296,             /* channeldescr  */
  YYSYMBOL_networkdefinition_old = 297,    /* networkdefinition_old  */
  YYSYMBOL_networkheader_old = 298,        /* networkheader_old  */
  YYSYMBOL_endnetwork_old = 299,           /* endnetwork_old  */
  YYSYMBOL_networkdefinition = 300,        /* networkdefinition  */
  YYSYMBOL_networkheader = 301,            /* networkheader  */
  YYSYMBOL_vector = 302,                   /* vector  */
  YYSYMBOL_expression = 303,               /* expression  */
  YYSYMBOL_inputvalue = 304,               /* inputvalue  */
  YYSYMBOL_xmldocvalue = 305,              /* xmldocvalue  */
  YYSYMBOL_expr = 306,                     /* expr  */
  YYSYMBOL_simple_expr = 307,              /* simple_expr  */
  YYSYMBOL_parameter_expr = 308,           /* parameter_expr  */
  YYSYMBOL_string_expr = 309,              /* string_expr  */
  YYSYMBOL_boolconst_expr = 310,           /* boolconst_expr  */
  YYSYMBOL_numconst_expr = 311,            /* numconst_expr  */
  YYSYMBOL_special_expr = 312,             /* special_expr  */
  YYSYMBOL_stringconstant = 313,           /* stringconstant  */
  YYSYMBOL_numconst = 314,                 /* numconst  */
  YYSYMBOL_timeconstant = 315,             /* timeconstant  */
  YYSYMBOL_cplusplus = 316,                /* cplusplus  */
  YYSYMBOL_struct_decl = 317,              /* struct_decl  */
  YYSYMBOL_class_decl = 318,               /* class_decl  */
  YYSYMBOL_message_decl = 319,             /* message_decl  */
  YYSYMBOL_enum_decl = 320,                /* enum_decl  */
  YYSYMBOL_enum = 321,                     /* enum  */
  YYSYMBOL_322_36 = 322,                   /* $@36  */
  YYSYMBOL_323_37 = 323,                   /* $@37  */
  YYSYMBOL_opt_enumfields = 324,           /* opt_enumfields  */
  YYSYMBOL_enumfields = 325,               /* enumfields  */
  YYSYMBOL_enumfield = 326,                /* enumfield  */
  YYSYMBOL_message = 327,                  /* message  */
  YYSYMBOL_328_38 = 328,                   /* $@38  */
  YYSYMBOL_329_39 = 329,                   /* $@39  */
  YYSYMBOL_class = 330,                    /* class  */
  YYSYMBOL_331_40 = 331,                   /* $@40  */
  YYSYMBOL_332_41 = 332,                   /* $@41  */
  YYSYMBOL_struct = 333,                   /* struct  */
  YYSYMBOL_334_42 = 334,                   /* $@42  */
  YYSYMBOL_335_43 = 335,                   /* $@43  */
  YYSYMBOL_opt_propertiesblock = 336,      /* opt_propertiesblock  */
  YYSYMBOL_337_44 = 337,                   /* $@44  */
  YYSYMBOL_opt_properties = 338,           /* opt_properties  */
  YYSYMBOL_properties = 339,               /* properties  */
  YYSYMBOL_property = 340,                 /* property  */
  YYSYMBOL_propertyvalue = 341,            /* propertyvalue  */
  YYSYMBOL_opt_fieldsblock = 342,          /* opt_fieldsblock  */
  YYSYMBOL_343_45 = 343,                   /* $@45  */
  YYSYMBOL_opt_fields = 344,               /* opt_fields  */
  YYSYMBOL_fields = 345,                   /* fields  */
  YYSYMBOL_field = 346,                    /* field  */
  YYSYMBOL_347_46 = 347,                   /* $@46  */
  YYSYMBOL_348_47 = 348,                   /* $@47  */
  YYSYMBOL_fieldmodifiers = 349,           /* fieldmodifiers  */
  YYSYMBOL_fielddatatype = 350,            /* fielddatatype  */
  YYSYMBOL_opt_fieldvector = 351,          /* opt_fieldvector  */
  YYSYMBOL_opt_fieldenum = 352,            /* opt_fieldenum  */
  YYSYMBOL_opt_fieldvalue = 353,           /* opt_fieldvalue  */
  YYSYMBOL_fieldvalue = 354,               /* fieldvalue  */
  YYSYMBOL_fieldvalue_token = 355,         /* fieldvalue_token  */
  YYSYMBOL_enumvalue = 356,                /* enumvalue  */
  YYSYMBOL_opt_semicolon = 357,            /* opt_semicolon  */
  YYSYMBOL_comma_or_semicolon = 358        /* comma_or_semicolon  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1025

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  106
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  253
/* YYNRULES -- Number of rules.  */
#define YYNRULES  547
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  909

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   342


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    93,     2,     2,
     104,   105,    91,    89,    97,    90,   103,    92,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    88,    96,
       2,   100,     2,    87,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   101,     2,   102,    94,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    95
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   243,   243,   247,   248,   252,   254,   256,   258,   260,
     263,   265,   267,   269,   272,   273,   274,   275,   276,   278,
     280,   282,   284,   293,   292,   304,   305,   309,   321,   325,
     334,   335,   339,   345,   354,   358,   368,   377,   387,   388,
     392,   398,   410,   418,   428,   432,   442,   453,   466,   477,
     487,   491,   501,   515,   528,   529,   534,   533,   544,   545,
     549,   550,   554,   566,   567,   572,   571,   582,   583,   587,
     588,   592,   604,   605,   609,   619,   620,   624,   634,   635,
     640,   639,   650,   651,   655,   659,   669,   670,   675,   674,
     685,   686,   690,   694,   705,   709,   713,   717,   721,   725,
     729,   733,   737,   741,   745,   755,   756,   761,   760,   771,
     772,   776,   777,   778,   779,   783,   784,   788,   793,   801,
     802,   806,   811,   822,   823,   828,   827,   838,   839,   843,
     844,   845,   846,   850,   851,   855,   860,   868,   869,   873,
     878,   889,   890,   895,   894,   905,   906,   910,   911,   916,
     915,   925,   924,   935,   934,   944,   943,   956,   965,   966,
     971,   970,   981,   982,   986,   987,   992,   991,  1001,  1000,
    1011,  1010,  1020,  1019,  1032,  1041,  1042,  1046,  1047,  1052,
    1051,  1060,  1059,  1071,  1072,  1076,  1077,  1081,  1092,  1093,
    1097,  1098,  1103,  1102,  1111,  1110,  1122,  1123,  1127,  1128,
    1132,  1143,  1144,  1148,  1149,  1154,  1153,  1162,  1161,  1174,
    1175,  1179,  1180,  1184,  1196,  1197,  1201,  1202,  1207,  1206,
    1215,  1214,  1227,  1228,  1232,  1233,  1237,  1249,  1250,  1255,
    1254,  1263,  1262,  1274,  1275,  1279,  1280,  1284,  1291,  1302,
    1303,  1308,  1307,  1316,  1315,  1327,  1328,  1332,  1333,  1337,
    1344,  1355,  1359,  1366,  1370,  1377,  1378,  1383,  1382,  1392,
    1391,  1403,  1404,  1408,  1409,  1413,  1414,  1419,  1418,  1432,
    1433,  1437,  1447,  1451,  1455,  1459,  1463,  1464,  1468,  1473,
    1478,  1484,  1493,  1494,  1498,  1504,  1512,  1517,  1521,  1529,
    1536,  1542,  1552,  1553,  1557,  1562,  1569,  1574,  1578,  1586,
    1591,  1595,  1604,  1609,  1614,  1625,  1626,  1631,  1630,  1640,
    1639,  1651,  1652,  1656,  1657,  1661,  1662,  1667,  1666,  1680,
    1681,  1685,  1695,  1699,  1703,  1707,  1711,  1712,  1716,  1721,
    1726,  1732,  1741,  1742,  1746,  1752,  1760,  1765,  1769,  1777,
    1784,  1790,  1800,  1801,  1805,  1810,  1817,  1822,  1826,  1834,
    1839,  1843,  1852,  1857,  1862,  1873,  1880,  1889,  1900,  1911,
    1924,  1930,  1934,  1938,  1949,  1951,  1953,  1955,  1960,  1962,
    1967,  1968,  1971,  1973,  1975,  1977,  1979,  1981,  1984,  1988,
    1990,  1992,  1994,  1996,  1998,  2001,  2003,  2005,  2008,  2012,
    2014,  2016,  2019,  2022,  2024,  2026,  2029,  2031,  2033,  2035,
    2037,  2042,  2043,  2044,  2045,  2046,  2050,  2055,  2060,  2066,
    2072,  2080,  2084,  2086,  2091,  2095,  2097,  2099,  2104,  2109,
    2111,  2113,  2119,  2120,  2121,  2122,  2130,  2139,  2148,  2155,
    2165,  2174,  2184,  2183,  2195,  2194,  2209,  2210,  2214,  2215,
    2219,  2225,  2236,  2235,  2246,  2245,  2260,  2259,  2270,  2269,
    2284,  2283,  2294,  2293,  2308,  2307,  2313,  2317,  2318,  2322,
    2323,  2327,  2337,  2338,  2339,  2340,  2341,  2342,  2347,  2346,
    2352,  2356,  2357,  2361,  2362,  2367,  2366,  2379,  2378,  2392,
    2394,  2396,  2398,  2401,  2405,  2406,  2408,  2409,  2410,  2411,
    2413,  2414,  2415,  2416,  2418,  2419,  2420,  2425,  2430,  2435,
    2439,  2443,  2447,  2451,  2455,  2472,  2473,  2477,  2478,  2479,
    2480,  2481,  2482,  2483,  2484,  2485,  2485,  2485,  2485,  2486,
    2486,  2487,  2487,  2487,  2488,  2488,  2488,  2488,  2488,  2488,
    2489,  2489,  2489,  2490,  2490,  2491,  2491,  2491,  2491,  2491,
    2491,  2496,  2497,  2498,  2501,  2501,  2503,  2503
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INCLUDE", "SIMPLE",
  "CHANNEL", "MODULE", "PARAMETERS", "GATES", "GATESIZES", "SUBMODULES",
  "CONNECTIONS", "DISPLAY", "IN", "OUT", "NOCHECK", "LEFT_ARROW",
  "RIGHT_ARROW", "FOR", "TO", "DO", "IF", "LIKE", "NETWORK", "ENDSIMPLE",
  "ENDMODULE", "ENDCHANNEL", "ENDNETWORK", "ENDFOR", "MACHINES", "ON",
  "CHANATTRNAME", "INTCONSTANT", "REALCONSTANT", "NAME", "STRINGCONSTANT",
  "CHARCONSTANT", "TRUE_", "FALSE_", "INPUT_", "XMLDOC", "REF", "ANCESTOR",
  "CONSTDECL", "NUMERICTYPE", "STRINGTYPE", "BOOLTYPE", "XMLTYPE",
  "ANYTYPE", "CPLUSPLUS", "CPLUSPLUSBODY", "MESSAGE", "CLASS", "STRUCT",
  "ENUM", "NONCOBJECT", "EXTENDS", "FIELDS", "PROPERTIES", "ABSTRACT",
  "READONLY", "CHARTYPE", "SHORTTYPE", "INTTYPE", "LONGTYPE", "DOUBLETYPE",
  "UNSIGNED_", "SIZEOF", "SUBMODINDEX", "PLUSPLUS", "EQ", "NE", "GT", "GE",
  "LS", "LE", "AND", "OR", "XOR", "NOT", "BIN_AND", "BIN_OR", "BIN_XOR",
  "BIN_COMPL", "SHIFT_LEFT", "SHIFT_RIGHT", "INVALID_CHAR", "'?'", "':'",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "UMIN", "';'", "','", "'{'",
  "'}'", "'='", "'['", "']'", "'.'", "'('", "')'", "$accept",
  "networkdescription", "somedefinitions", "definition", "import", "$@1",
  "filenames", "filename", "channeldefinition_old", "channelheader_old",
  "opt_channelattrblock_old", "channelattrblock_old", "endchannel_old",
  "channeldefinition", "channelheader", "opt_channelattrblock",
  "channelattrblock", "simpledefinition_old", "simpleheader_old",
  "endsimple_old", "simpledefinition", "simpleheader",
  "moduledefinition_old", "moduleheader_old", "endmodule_old",
  "moduledefinition", "moduleheader", "opt_machineblock_old",
  "machineblock_old", "$@2", "opt_machinelist_old", "machinelist_old",
  "machine_old", "opt_machineblock", "machineblock", "$@3",
  "opt_machinelist", "machinelist", "machine", "opt_displayblock_old",
  "displayblock_old", "opt_displayblock", "displayblock",
  "opt_paramblock_old", "paramblock_old", "$@4", "opt_parameters_old",
  "parameters_old", "opt_paramblock", "paramblock", "$@5",
  "opt_parameters", "parameters", "parameter_old", "parameter",
  "opt_gateblock_old", "gateblock_old", "$@6", "opt_gates_old",
  "gates_old", "gatesI_old", "gateI_old", "gatesO_old", "gateO_old",
  "opt_gateblock", "gateblock", "$@7", "opt_gates", "gates", "gatesI",
  "gateI", "gatesO", "gateO", "opt_submodblock_old", "submodblock_old",
  "$@8", "opt_submodules_old", "submodules_old", "submodule_old", "$@9",
  "$@10", "$@11", "$@12", "submodule_body_old", "opt_submodblock",
  "submodblock", "$@13", "opt_submodules", "submodules", "submodule",
  "$@14", "$@15", "$@16", "$@17", "submodule_body", "opt_on_blocks_old",
  "on_blocks_old", "on_block_old", "$@18", "$@19", "opt_on_list_old",
  "on_list_old", "on_mach_old", "opt_on_blocks", "on_blocks", "on_block",
  "$@20", "$@21", "opt_on_list", "on_list", "on_mach",
  "opt_substparamblocks_old", "substparamblocks_old",
  "substparamblock_old", "$@22", "$@23", "opt_substparameters_old",
  "substparameters_old", "substparameter_old", "opt_substparamblocks",
  "substparamblocks", "substparamblock", "$@24", "$@25",
  "opt_substparameters", "substparameters", "substparameter",
  "opt_gatesizeblocks_old", "gatesizeblock_old", "$@26", "$@27",
  "opt_gatesizes_old", "gatesizes_old", "gatesize_old",
  "opt_gatesizeblocks", "gatesizeblock", "$@28", "$@29", "opt_gatesizes",
  "gatesizes", "gatesize", "opt_submod_displayblock_old",
  "opt_submod_displayblock", "opt_connblock_old", "connblock_old", "$@30",
  "$@31", "opt_connections_old", "connections_old", "connection_old",
  "loopconnection_old", "$@32", "loopvarlist_old", "loopvar_old",
  "opt_conncondition_old", "opt_conn_displaystr_old",
  "notloopconnections_old", "notloopconnection_old", "leftgatespec_old",
  "leftmod_old", "leftgate_old", "parentleftgate_old", "rightgatespec_old",
  "rightmod_old", "rightgate_old", "parentrightgate_old",
  "channeldescr_old", "opt_connblock", "connblock", "$@33", "$@34",
  "opt_connections", "connections", "connection", "loopconnection", "$@35",
  "loopvarlist", "loopvar", "opt_conncondition", "opt_conn_displaystr",
  "notloopconnections", "notloopconnection", "leftgatespec", "leftmod",
  "leftgate", "parentleftgate", "rightgatespec", "rightmod", "rightgate",
  "parentrightgate", "channeldescr", "networkdefinition_old",
  "networkheader_old", "endnetwork_old", "networkdefinition",
  "networkheader", "vector", "expression", "inputvalue", "xmldocvalue",
  "expr", "simple_expr", "parameter_expr", "string_expr", "boolconst_expr",
  "numconst_expr", "special_expr", "stringconstant", "numconst",
  "timeconstant", "cplusplus", "struct_decl", "class_decl", "message_decl",
  "enum_decl", "enum", "$@36", "$@37", "opt_enumfields", "enumfields",
  "enumfield", "message", "$@38", "$@39", "class", "$@40", "$@41",
  "struct", "$@42", "$@43", "opt_propertiesblock", "$@44",
  "opt_properties", "properties", "property", "propertyvalue",
  "opt_fieldsblock", "$@45", "opt_fields", "fields", "field", "$@46",
  "$@47", "fieldmodifiers", "fielddatatype", "opt_fieldvector",
  "opt_fieldenum", "opt_fieldvalue", "fieldvalue", "fieldvalue_token",
  "enumvalue", "opt_semicolon", "comma_or_semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-798)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-485)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -798,    81,   187,  -798,  -798,    29,   127,   161,   170,   121,
     175,   142,   231,   265,  -798,  -798,  -798,   288,  -798,   181,
    -798,   293,  -798,   236,  -798,   293,  -798,   249,  -798,   313,
    -798,   254,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,   303,   261,   276,   284,   292,   270,   -36,   -23,   356,
     104,   133,   452,   372,   394,   396,   368,   456,  -798,   431,
     456,   431,    18,   468,   313,  -798,   449,  -798,   -45,  -798,
     461,  -798,  -798,   470,  -798,  -798,   471,  -798,  -798,   387,
     472,  -798,  -798,   473,  -798,  -798,   474,   475,   407,  -798,
    -798,  -798,   408,   409,   164,   227,   410,   411,   140,   140,
     140,   140,   270,  -798,  -798,   793,  -798,  -798,  -798,  -798,
    -798,  -798,  -798,  -798,   282,   -13,  -798,   452,   416,   418,
     476,  -798,   433,   515,  -798,   436,   518,  -798,   515,   518,
     452,  -798,    56,   499,   468,  -798,  -798,    63,   520,   449,
    -798,  -798,   303,   270,   430,   478,   432,   478,  -798,   435,
     478,   439,   495,  -798,  -798,   256,   316,   503,  -798,   500,
    -798,   505,   506,   438,  -798,  -798,  -798,   715,  -798,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     507,   510,   270,  -798,   270,   452,   270,   445,   512,  -798,
     459,   525,  -798,  -798,   463,   544,  -798,   543,   544,   466,
     521,   452,  -798,   270,  -798,  -798,   452,  -798,   134,   460,
     520,  -798,  -798,  -798,   462,  -798,   477,   501,  -798,   501,
    -798,   501,  -798,   147,   464,   495,  -798,  -798,   552,  -798,
     595,   130,  -798,  -798,   465,  -798,  -798,   342,   342,   342,
     342,   342,   342,   843,   843,   843,   278,   278,   278,   349,
     349,   504,   212,   212,   479,   479,   479,   479,  -798,  -798,
    -798,  -798,   491,  -798,   452,  -798,  -798,   220,  -798,    16,
    -798,    -6,  -798,   530,  -798,   480,   467,  -798,   481,   556,
    -798,   561,  -798,  -798,  -798,   229,  -798,   502,   565,  -798,
     513,   566,   452,  -798,   270,  -798,   478,  -798,   514,   508,
     478,   509,   478,   511,   495,  -798,   152,   270,  -798,   140,
    -798,   140,  -798,   503,  -798,  -798,   140,  -798,   516,  -798,
     512,   517,   569,  -798,   240,  -798,  -798,   328,   270,  -798,
    -798,  -798,   248,  -798,    16,  -798,   270,  -798,    -3,   594,
    -798,   523,   602,  -798,   521,  -798,   521,  -798,   531,  -798,
     259,  -798,  -798,  -798,  -798,   268,  -798,   526,   582,  -798,
     501,   583,  -798,   270,   501,   270,   501,   270,   519,  -798,
    -798,   588,   542,  -798,   631,   741,   535,   818,  -798,  -798,
     453,  -798,  -798,    16,   613,   614,  -798,   363,  -798,  -798,
     530,  -798,   554,  -798,   365,  -798,   617,   567,  -798,   568,
     628,  -798,  -798,     4,   642,  -798,  -798,  -798,   565,   452,
    -798,   565,   566,  -798,   566,  -798,   558,  -798,   582,  -798,
     560,   562,  -798,   583,  -798,   -19,  -798,   564,  -798,   575,
    -798,   270,  -798,  -798,   140,  -798,  -798,  -798,   616,   618,
    -798,  -798,  -798,  -798,  -798,   577,   289,  -798,   580,   291,
    -798,   613,   614,  -798,    16,   649,   656,  -798,   379,   603,
    -798,   617,  -798,  -798,    20,   658,    15,  -798,   660,   607,
    -798,   608,   598,  -798,  -798,  -798,  -798,  -798,  -798,   582,
     452,  -798,   270,   245,  -798,   638,   640,  -798,   106,  -798,
     267,   270,   270,  -798,   679,  -798,  -798,   612,  -798,   613,
     624,  -798,   614,   300,   306,  -798,   609,   308,  -798,   626,
     311,  -798,   649,   656,   685,  -798,    20,  -798,    98,  -798,
      20,  -798,  -798,  -798,   400,   627,  -798,   633,   270,  -798,
     643,  -798,   660,  -798,  -798,   178,   697,   270,  -798,   637,
    -798,   474,   475,  -798,  -798,  -798,   282,   639,  -798,  -798,
    -798,    22,  -798,  -798,  -798,  -798,  -798,  -798,  -798,   247,
     700,  -798,  -798,   140,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,   635,  -798,   649,   636,  -798,   656,   322,   332,    45,
    -798,   705,  -798,   452,   641,  -798,   190,   190,   706,  -798,
    -798,   707,  -798,   178,  -798,   114,  -798,   178,  -798,  -798,
    -798,   428,   645,  -798,   646,  -798,  -798,  -798,  -798,   644,
    -798,  -798,  -798,  -798,  -798,   767,  -798,  -798,  -798,  -798,
    -798,  -798,   709,     0,  -798,   665,   726,   661,   672,   452,
       1,   746,   674,  -798,    48,   746,   201,   -33,  -798,    52,
    -798,   744,  -798,   676,  -798,   747,   747,   748,  -798,    -5,
     721,   644,  -798,   270,   749,  -798,   313,   452,   760,   705,
    -798,  -798,  -798,   677,   452,   786,   769,   776,   452,   786,
     776,  -798,  -798,   790,     2,  -798,   701,   807,   732,   217,
     815,   750,  -798,    19,   815,    31,   -32,  -798,   753,   774,
    -798,   758,   779,   721,  -798,   270,   313,   468,   831,   235,
    -798,  -798,  -798,   846,   376,   110,  -798,   145,   746,  -798,
     376,   746,   270,   863,  -798,   449,   452,   867,   744,  -798,
     801,  -798,   452,   893,   872,   885,   452,   893,   885,  -798,
    -798,  -798,  -798,   886,   377,   825,   779,   313,  -798,   468,
    -798,  -798,   452,   270,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,   786,  -798,   786,  -798,   270,   449,   520,   903,   253,
    -798,  -798,  -798,   891,   833,   119,  -798,   159,   815,  -798,
     834,   815,   826,   474,   475,  -798,  -798,  -798,  -798,  -798,
    -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,  -798,  -798,  -798,  -798,  -798,   282,  -798,   377,  -798,
     842,   468,   313,  -798,   258,  -798,  -798,   376,   376,   449,
    -798,   520,  -798,  -798,   452,   844,  -798,  -798,  -798,  -798,
    -798,   893,  -798,   893,  -798,  -798,  -798,  -798,   468,   137,
     851,  -798,  -798,  -798,  -798,   520,   449,  -798,   283,  -798,
    -798,   845,   847,  -798,   452,  -798,   907,  -798,   520,   138,
     856,  -798,  -798,  -798,  -798,   857,   912,   852,  -798,   452,
    -798,   914,  -798,   849,  -798,   381,  -798,  -798,   859,   917,
     858,   912,  -798,  -798,   912,  -798,   849,  -798,   406,  -798,
    -798,  -798,  -798,   917,  -798,  -798,   917,  -798,  -798
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       4,     0,     2,     1,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     3,     5,    10,    30,     6,     0,
      11,    55,     7,     0,    12,    55,     8,     0,    13,   176,
       9,     0,    14,    15,    16,    17,    18,    19,    20,    21,
      22,     0,    43,    29,    49,     0,   545,     0,     0,     0,
       0,     0,     0,     0,    31,    39,     0,    79,    54,    64,
      79,    64,     0,   202,   175,   178,   189,    27,     0,    26,
       0,   544,   426,     0,   430,   442,     0,   428,   446,     0,
       0,   427,   450,     0,   431,   432,   419,   420,   406,   418,
     412,   413,   367,     0,     0,     0,     0,   415,     0,     0,
       0,     0,   545,   362,   363,   361,   370,   401,   402,   403,
     404,   405,   411,   414,   421,   545,    28,     0,     0,     0,
      38,    56,     0,   106,    78,     0,    87,    63,   106,    87,
       0,   179,     0,     0,   201,   204,   177,     0,   215,   188,
     191,    24,     0,   545,     0,   456,     0,   456,   429,     0,
     456,     0,   437,   424,   425,     0,     0,     0,   407,     0,
     410,     0,     0,     0,   388,   392,   378,     0,    33,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   545,    35,   545,     0,   545,     0,    59,    80,
       0,     0,   105,    65,     0,   124,    86,   142,   124,     0,
     184,     0,   205,   545,   355,   203,     0,   192,     0,     0,
     214,   217,   190,    25,   356,   444,     0,   470,   448,   470,
     452,   470,   434,     0,     0,   436,   439,   396,     0,   366,
       0,     0,   408,   409,     0,   416,   371,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   389,   390,   391,   393,
     394,     0,   372,   373,   374,   375,   376,   377,   422,   423,
      34,    32,     0,    36,     0,    62,    57,     0,    61,    83,
     107,   545,    42,    68,    88,     0,     0,   123,     0,   256,
     141,   159,   181,   187,   180,     0,   186,     0,   210,   357,
       0,   197,     0,   218,   545,   216,   456,   454,     0,     0,
     456,     0,   456,     0,   437,   440,     0,   545,   438,     0,
     397,     0,   365,     0,   369,   417,     0,    41,     0,    58,
       0,    95,     0,    81,     0,    85,    94,   110,   545,    45,
      71,    66,     0,    70,    91,   125,   545,   143,     0,    73,
     255,     0,   306,   158,   184,   183,     0,   207,     0,   206,
       0,   212,   194,   200,   193,     0,   199,     0,   223,   358,
     470,   458,   468,   545,   470,   545,   470,   545,     0,   541,
     543,     0,     0,   433,     0,     0,     0,   395,    40,    60,
       0,    97,    82,     0,     0,     0,   108,   109,    44,    67,
       0,    89,     0,    93,   128,    46,   146,     0,   259,     0,
       0,    72,   160,     0,    76,   305,   182,   185,   210,     0,
     209,     0,   197,   196,     0,   220,     0,   219,   222,   225,
       0,     0,   455,   457,   460,   483,   443,     0,   447,     0,
     451,   545,   542,   441,     0,   398,   364,   368,    98,    96,
     101,   102,   103,   104,    84,   118,     0,   116,   122,     0,
     120,     0,     0,    69,    90,     0,     0,   126,   127,     0,
     144,   145,   148,   257,   262,     0,   545,    48,   163,     0,
     309,     0,     0,    75,   208,   213,   211,   195,   198,   223,
       0,   224,   545,     0,   459,   479,   480,   469,   483,   474,
       0,   545,   545,   435,     0,    99,   100,     0,   112,     0,
       0,   114,     0,     0,     0,    92,   136,     0,   134,   140,
       0,   138,     0,     0,     0,   147,   262,   267,   290,   260,
     261,   264,   265,   266,     0,     0,   283,     0,   545,    51,
       0,   161,   162,   165,   307,   312,     0,   545,   221,     0,
     445,   463,   464,   462,   466,   467,   465,     0,   481,   482,
     473,   477,   495,   496,   486,   487,   488,   489,   494,     0,
       0,   449,   453,     0,   399,   117,   115,   121,   119,   111,
     113,     0,   130,     0,     0,   132,     0,     0,     0,   545,
     258,     0,   291,     0,   289,   263,     0,     0,     0,    74,
      50,     0,   164,   312,   317,   340,   310,   311,   314,   315,
     316,     0,     0,   333,     0,    52,   226,   461,   485,   500,
     490,   491,   492,   493,   475,     0,   135,   133,   139,   137,
     129,   131,     0,   545,   149,     0,     0,   270,     0,     0,
     300,   273,     0,   293,     0,   273,     0,   287,   282,   545,
     308,     0,   341,   339,   313,     0,     0,     0,    77,     0,
     502,   500,   400,   545,     0,   151,   176,     0,     0,     0,
     360,   303,   301,   299,     0,   275,     0,     0,     0,   275,
       0,   288,   286,     0,   545,   166,     0,     0,   320,   350,
     323,     0,   343,     0,   323,     0,   337,   332,     0,     0,
     499,     0,   504,   502,   153,   545,   176,   202,     0,     0,
     277,   269,   272,     0,     0,   297,   292,   300,   273,   304,
       0,   273,   545,     0,   168,   189,     0,     0,     0,   351,
     349,   353,     0,   325,     0,     0,     0,   325,     0,   338,
     336,   497,   498,     0,     0,     0,   504,   176,   155,   202,
     150,   228,     0,   545,   276,   274,   547,   546,   280,   298,
     296,   275,   278,   275,   170,   545,   189,   215,     0,     0,
     327,   319,   322,     0,     0,   347,   342,   350,   323,   354,
       0,   323,     0,   509,   510,   514,   507,   508,   512,   513,
     524,   525,   526,   527,   528,   529,   521,   522,   523,   530,
     531,   532,   533,   534,   519,   520,   535,   536,   537,   538,
     539,   540,   518,   517,   515,   516,   511,   503,   506,   478,
       0,   202,   176,   152,   252,   271,   268,     0,     0,   189,
     172,   215,   167,   240,     0,     0,   326,   324,   330,   348,
     346,   325,   328,   325,   501,   505,   476,   154,   202,     0,
       0,   227,   157,   281,   279,   215,   189,   169,   254,   321,
     318,     0,     0,   156,     0,   229,     0,   171,   215,     0,
       0,   239,   174,   331,   329,     0,   234,     0,   173,     0,
     241,     0,   231,   238,   230,     0,   236,   251,     0,   246,
       0,   234,   237,   233,     0,   243,   250,   242,     0,   248,
     253,   232,   235,   246,   249,   245,     0,   244,   247
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -798,  -798,  -798,  -798,  -798,  -798,  -798,   810,  -798,  -798,
    -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,
    -798,  -798,  -798,  -798,  -798,  -798,  -798,   928,  -798,  -798,
    -798,  -798,   625,   895,  -798,  -798,  -798,  -798,   557,  -798,
    -798,  -798,  -798,   898,  -798,  -798,  -798,  -798,   830,  -798,
    -798,  -798,  -798,   570,  -328,   832,  -798,  -798,  -798,  -798,
     522,   455,   524,   450,   757,  -798,  -798,  -798,  -798,   444,
     378,   446,   382,  -798,  -798,  -798,  -798,  -798,   496,  -798,
    -798,  -798,  -798,  -739,  -798,  -798,  -798,  -798,  -798,   429,
    -798,  -798,  -798,  -798,  -797,  -659,  -798,   906,  -798,  -798,
     619,  -798,   620,  -694,  -798,   835,  -798,  -798,   550,  -798,
     551,   915,  -798,   848,  -798,  -798,   559,  -798,   563,   841,
    -798,   761,  -798,  -798,   498,  -798,   571,  -798,  -798,  -798,
    -798,    89,  -798,    91,  -798,  -798,  -798,  -798,    85,  -798,
      83,  -798,  -798,  -798,  -798,  -798,  -798,   469,  -798,   482,
    -798,  -798,   321,  -798,  -569,  -656,  -798,  -643,  -798,  -798,
    -798,  -798,  -591,  -798,  -798,  -798,   395,  -798,  -798,  -798,
    -798,   388,  -798,   386,  -798,  -798,   266,  -798,  -639,  -733,
    -798,  -712,  -798,  -798,  -798,  -798,  -595,  -798,  -798,  -798,
     340,  -798,  -798,  -798,  -798,  -798,  -502,  -117,  -798,  -798,
     -55,  -798,  -798,  -798,  -798,  -798,  -798,  -154,  -798,  -491,
    -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,   683,  -798,
     763,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,  -798,
    -142,  -798,  -798,  -798,   572,  -798,  -220,  -798,  -798,  -798,
     527,  -798,  -798,  -798,  -798,   339,   298,   257,   184,  -798,
    -798,  -101,  -690
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    14,    15,    41,    68,    69,    16,    17,
      53,    54,   116,    18,    19,   119,   120,    20,    21,   282,
      22,    23,    24,    25,   477,    26,    27,    57,    58,   198,
     276,   277,   278,   126,   127,   283,   341,   342,   343,   410,
     411,   482,   483,   123,   124,   279,   333,   334,   205,   206,
     344,   401,   402,   335,   336,   201,   202,   337,   396,   397,
     456,   457,   459,   460,   286,   287,   404,   467,   468,   517,
     518,   520,   521,   289,   290,   406,   470,   471,   472,   666,
     706,   747,   822,   750,   352,   353,   478,   541,   542,   543,
     725,   766,   829,   856,   832,    63,    64,    65,   210,   354,
     294,   295,   296,   138,   139,   140,   301,   422,   364,   365,
     366,   751,   134,   135,   298,   418,   359,   360,   361,   833,
     220,   221,   368,   489,   427,   428,   429,   824,   851,   876,
     891,   884,   885,   886,   858,   871,   889,   903,   897,   898,
     899,   852,   872,   349,   350,   526,   474,   529,   530,   531,
     532,   591,   636,   637,   675,   714,   709,   533,   534,   535,
     648,   536,   641,   642,   716,   643,   644,   414,   415,   603,
     545,   606,   607,   608,   609,   651,   687,   688,   733,   774,
     769,   610,   611,   612,   697,   613,   690,   691,   776,   692,
     693,    28,    29,   214,    30,    31,   673,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
      32,    33,    34,    35,    36,    37,   152,   314,   234,   235,
     236,    38,   145,   306,    39,   147,   310,    40,   150,   312,
     227,   371,   432,   433,   434,   557,   309,   435,   497,   498,
     499,   661,   619,   500,   570,   660,   702,   745,   817,   818,
     382,    72,   758
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     194,   168,   556,   241,   780,   229,   645,   707,   231,   311,
     823,   313,   407,   209,   193,   770,   403,  -302,  -302,   479,
      73,   192,   664,   720,   723,   710,   594,   698,   338,   699,
     762,   767,  -302,    76,   857,   735,   681,   739,   527,   130,
     495,   496,   224,   164,   165,   166,   167,   749,   738,   538,
     331,   141,   142,   736,   528,   737,  -484,   836,   867,   332,
      74,   694,    75,    42,   677,   736,   754,   632,   593,   593,
     672,   878,   831,    77,   683,    78,   679,   211,   272,   678,
    -472,     3,   847,    71,   216,   408,   718,   633,   821,   721,
      71,   270,   480,   271,   297,   273,    71,   700,    71,   300,
     238,   240,   593,   653,  -295,   827,   131,   828,   861,   863,
     862,    71,   299,   618,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   855,   515,   853,   854,   841,
     778,    71,   843,   781,   212,   682,   593,   684,    71,   761,
     430,   217,   763,   593,   437,   302,   439,   328,   864,   879,
      80,    43,   868,   848,   370,   495,   496,   592,   374,   386,
     376,    46,    86,    87,    88,    89,    48,    90,    91,   759,
     339,    94,    95,   652,   379,   367,   380,   730,   839,    83,
       4,     5,     6,     7,   740,    44,   604,    49,   158,   593,
      81,  -285,    82,   369,    45,  -471,   159,    96,    97,    47,
       8,   593,   605,   760,   672,   593,   383,  -335,   680,    98,
     593,   639,   303,    99,   640,   865,   880,   323,   729,    84,
     100,    85,   678,  -352,  -352,   324,     9,   398,    10,    11,
      12,    13,   381,   315,   101,   405,   593,   316,  -295,    86,
      87,    88,    89,   816,    90,    91,    92,    93,    94,    95,
     593,   160,  -345,   753,   384,    50,   385,   849,   161,   528,
     850,   387,   436,   840,   438,   730,   440,   551,   552,    55,
     553,   835,   554,   555,    96,    97,   729,   605,    86,    87,
      88,    89,   869,    90,    91,   870,    98,    94,    95,    51,
      99,   561,   485,   186,   187,   188,   189,   100,   620,   621,
     622,   623,   562,   563,   190,   191,   329,   330,   593,    52,
    -345,   101,    56,    96,    97,   355,   356,   816,   564,   565,
     566,   567,   568,   569,    59,    98,   392,   393,    67,    99,
     503,   394,   395,    62,   399,   400,   100,    61,    86,    87,
      88,    89,    66,    90,    91,   420,   421,    94,    95,   -47,
     101,   237,   181,   182,   423,   424,    71,   184,   185,   186,
     187,   188,   189,   549,   -37,   539,   461,   462,   465,   466,
      70,   892,   -53,    96,    97,   508,   509,   511,   512,   504,
      79,   550,   522,   523,   904,    98,   579,   509,   115,    99,
     571,   572,   580,   512,   582,   583,   100,   585,   586,   783,
     784,   785,   786,   787,   788,   789,   596,   597,   630,   583,
     101,   239,   178,   179,   180,   117,   181,   182,   631,   586,
     118,   184,   185,   186,   187,   188,   189,   600,   184,   185,
     186,   187,   188,   189,   655,   656,   615,   790,   791,   792,
     793,   794,   795,   796,   797,   798,   121,   799,   800,   801,
     125,   802,   803,   122,   804,   805,   806,   807,   808,   809,
     810,   811,   756,   757,   812,   132,   638,   893,   894,   137,
     813,   814,   815,   148,    86,    87,    88,    89,   634,    90,
      91,    92,    93,    94,    95,   143,   448,   449,   450,   451,
     452,   453,   905,   906,   144,   146,   149,   151,   153,   154,
     197,   155,   156,   157,   162,   163,   195,   196,   625,    96,
      97,   199,   671,   200,   203,   204,   213,   218,   225,   233,
     228,    98,   665,   230,   242,    99,   226,   232,    89,   243,
     244,   268,   100,   245,   269,   274,   275,   280,   685,   281,
     708,   284,   285,   288,   292,   293,   101,   712,   308,   304,
    -359,   719,   704,   317,   340,   307,   346,   348,   345,   347,
     325,   351,   731,   189,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   724,   178,   179,   180,   327,   181,   182,
     357,   183,   326,   184,   185,   186,   187,   188,   189,   358,
     363,   362,   372,   391,   748,   390,   409,   373,   375,   768,
     377,   412,   388,   413,   425,   772,   426,   431,   441,   779,
     442,   764,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   419,   178,   179,   180,   825,   181,   182,   443,   183,
     447,   184,   185,   186,   187,   188,   189,   455,   458,   319,
     464,   469,   826,   476,   481,   473,   475,   320,   490,   492,
     505,   506,   493,   501,   830,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   502,   178,   179,   180,   507,   181,
     182,   510,   183,   516,   184,   185,   186,   187,   188,   189,
     519,   524,   321,   537,   540,   544,   546,   547,   558,   559,
     322,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     581,   178,   179,   180,   575,   181,   182,   859,   183,   589,
     184,   185,   186,   187,   188,   189,   577,   584,   444,   599,
     598,   601,   614,   616,   624,   617,   445,   626,   628,   635,
     647,   649,   658,   663,  -284,   659,   668,   875,   657,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   669,   178,
     179,   180,   888,   181,   182,   667,   183,   674,   184,   185,
     186,   187,   188,   189,   670,   701,   573,   676,   686,  -334,
    -294,   689,   696,   705,   574,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   528,   178,   179,   180,   713,   181,
     182,   726,   183,   715,   184,   185,   186,   187,   188,   189,
     717,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     246,   178,   179,   180,   722,   181,   182,   727,   183,   728,
     184,   185,   186,   187,   188,   189,   732,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   446,   178,   179,   180,
     752,   181,   182,   734,   183,   741,   184,   185,   186,   187,
     188,   189,   743,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   662,   178,   179,   180,   742,   181,   182,   744,
     183,   755,   184,   185,   186,   187,   188,   189,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   765,   178,   179,
     180,   605,   181,   182,  -344,   773,   775,   184,   185,   186,
     187,   188,   189,   169,   170,   171,   172,   173,   174,   777,
     782,   819,   834,   178,   179,   180,   837,   181,   182,   838,
     842,   844,   184,   185,   186,   187,   188,   189,   846,   866,
     860,   873,   877,   874,   881,   882,   883,   895,   887,   890,
     593,   896,   223,    60,   900,   389,   129,   463,   128,   208,
     207,   627,   578,   454,   576,   291,   587,   525,   629,   588,
     136,   602,   487,   416,   222,   488,   417,   484,   133,   219,
     901,   305,   215,   513,   486,   902,   514,   548,   907,   908,
     711,   650,   646,   654,   771,   590,   695,   378,   318,   491,
     703,   746,   845,   820,     0,   494,     0,     0,     0,     0,
       0,     0,   595,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   560
};

static const yytype_int16 yycheck[] =
{
     117,   102,   493,   157,   737,   147,   597,   666,   150,   229,
     749,   231,    15,   130,   115,   727,   344,    16,    17,    15,
      56,    34,    22,   679,    22,   668,   528,    32,    34,    34,
     720,   725,    31,    56,   831,    16,    69,    69,    18,    21,
      59,    60,   143,    98,    99,   100,   101,   706,    17,    34,
      34,    96,    97,    34,    34,   694,    34,   769,   855,    43,
      96,   656,    98,    34,    16,    34,   709,    22,   101,   101,
      69,   868,   766,    96,    22,    98,   645,    21,   195,    31,
      99,     0,   821,    96,    21,    88,   677,   589,   747,   680,
      96,   192,    88,   194,   211,   196,    96,   102,    96,   216,
     155,   156,   101,   605,   103,   761,    88,   763,   841,   848,
     843,    96,   213,    91,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   829,   464,   827,   828,   778,
     735,    96,   781,   738,    88,   647,   101,   649,    96,   718,
     370,    88,   721,   101,   374,    21,   376,   274,    21,    21,
      56,    34,   856,   822,   306,    59,    60,    69,   310,   323,
     312,    50,    32,    33,    34,    35,    34,    37,    38,    69,
     281,    41,    42,    69,    32,   302,    34,   689,    69,    56,
       3,     4,     5,     6,   696,    34,    18,    55,    34,   101,
      96,   103,    98,   304,    34,    99,    42,    67,    68,    34,
      23,   101,    34,   715,    69,   101,   317,   103,    17,    79,
     101,    31,    88,    83,    34,    88,    88,    97,    69,    96,
      90,    98,    31,    16,    17,   105,    49,   338,    51,    52,
      53,    54,    90,    96,   104,   346,   101,   100,   103,    32,
      33,    34,    35,   744,    37,    38,    39,    40,    41,    42,
     101,    34,   103,    28,   319,    34,   321,     9,    41,    34,
      12,   326,   373,   775,   375,   777,   377,    32,    33,    98,
      35,    28,    37,    38,    67,    68,    69,    34,    32,    33,
      34,    35,     9,    37,    38,    12,    79,    41,    42,    34,
      83,    34,   419,    91,    92,    93,    94,    90,    61,    62,
      63,    64,    45,    46,    32,    33,    96,    97,   101,    31,
     103,   104,    29,    67,    68,    96,    97,   818,    61,    62,
      63,    64,    65,    66,    98,    79,    96,    97,    35,    83,
     441,    13,    14,    30,    96,    97,    90,    98,    32,    33,
      34,    35,    98,    37,    38,    96,    97,    41,    42,    98,
     104,   105,    84,    85,    96,    97,    96,    89,    90,    91,
      92,    93,    94,   490,    98,   476,    13,    14,    13,    14,
      88,   883,    98,    67,    68,    96,    97,    96,    97,   444,
      34,   492,    13,    14,   896,    79,    96,    97,    26,    83,
     501,   502,    96,    97,    96,    97,    90,    96,    97,    32,
      33,    34,    35,    36,    37,    38,    16,    17,    96,    97,
     104,   105,    80,    81,    82,    31,    84,    85,    96,    97,
      34,    89,    90,    91,    92,    93,    94,   538,    89,    90,
      91,    92,    93,    94,    16,    17,   547,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    88,    80,    81,    82,
      29,    84,    85,     7,    87,    88,    89,    90,    91,    92,
      93,    94,    96,    97,    97,     7,   593,    96,    97,    30,
     103,   104,   105,    96,    32,    33,    34,    35,   589,    37,
      38,    39,    40,    41,    42,    34,    43,    44,    45,    46,
      47,    48,    96,    97,    34,    34,    34,    34,    34,    34,
      34,   104,   104,   104,   104,   104,   100,    99,   573,    67,
      68,    88,   639,     8,    88,     7,    27,     7,    98,    34,
      98,    79,   633,    98,    34,    83,    58,    98,    35,    34,
      34,    34,    90,   105,    34,   100,    34,    88,   649,    24,
     667,    88,     8,    10,    88,    34,   104,   674,    57,    99,
      98,   678,   663,    99,    34,    88,    99,    11,    88,    88,
     105,    10,   689,    94,    70,    71,    72,    73,    74,    75,
      76,    77,    78,   684,    80,    81,    82,    96,    84,    85,
      88,    87,    88,    89,    90,    91,    92,    93,    94,    34,
      34,    88,    88,    34,   705,    88,    12,    99,    99,   726,
      99,    88,    96,    11,    88,   732,    34,    34,    99,   736,
      32,   722,    70,    71,    72,    73,    74,    75,    76,    77,
      78,   100,    80,    81,    82,   752,    84,    85,    96,    87,
     105,    89,    90,    91,    92,    93,    94,    34,    34,    97,
      96,    34,   753,    25,    12,    88,    88,   105,   100,    99,
      44,    43,   100,    99,   765,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    99,    80,    81,    82,   101,    84,
      85,   101,    87,    34,    89,    90,    91,    92,    93,    94,
      34,    88,    97,    35,    34,    88,    88,    99,    60,    59,
     105,    70,    71,    72,    73,    74,    75,    76,    77,    78,
     101,    80,    81,    82,   102,    84,    85,   834,    87,    34,
      89,    90,    91,    92,    93,    94,   102,   101,    97,    96,
     103,    88,    35,    96,    34,    96,   105,   102,   102,    34,
      34,    34,    96,    34,   103,   101,    20,   864,   103,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    97,    80,
      81,    82,   879,    84,    85,   100,    87,    21,    89,    90,
      91,    92,    93,    94,   102,    54,    97,   103,    34,   103,
     103,    34,    34,    34,   105,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    34,    80,    81,    82,    12,    84,
      85,   100,    87,    34,    89,    90,    91,    92,    93,    94,
      34,    70,    71,    72,    73,    74,    75,    76,    77,    78,
     105,    80,    81,    82,    34,    84,    85,    20,    87,    97,
      89,    90,    91,    92,    93,    94,    21,    70,    71,    72,
      73,    74,    75,    76,    77,    78,   105,    80,    81,    82,
      19,    84,    85,   103,    87,   102,    89,    90,    91,    92,
      93,    94,   104,    70,    71,    72,    73,    74,    75,    76,
      77,    78,   105,    80,    81,    82,   102,    84,    85,   100,
      87,    35,    89,    90,    91,    92,    93,    94,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    34,    80,    81,
      82,    34,    84,    85,   103,    12,    34,    89,    90,    91,
      92,    93,    94,    70,    71,    72,    73,    74,    75,    34,
      34,    96,    19,    80,    81,    82,    35,    84,    85,    96,
      96,   105,    89,    90,    91,    92,    93,    94,    96,    88,
      96,    96,    35,    96,    88,    88,    34,    88,    96,    35,
     101,    34,   142,    25,    96,   330,    61,   400,    60,   129,
     128,   583,   512,   393,   509,   208,   522,   471,   586,   523,
      64,   542,   422,   354,   139,   424,   356,   418,    63,   138,
     891,   220,   134,   461,   421,   894,   462,   489,   903,   906,
     669,   603,   597,   607,   728,   526,   656,   314,   235,   428,
     661,   703,   818,   746,    -1,   433,    -1,    -1,    -1,    -1,
      -1,    -1,   530,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   498
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   107,   108,     0,     3,     4,     5,     6,    23,    49,
      51,    52,    53,    54,   109,   110,   114,   115,   119,   120,
     123,   124,   126,   127,   128,   129,   131,   132,   297,   298,
     300,   301,   316,   317,   318,   319,   320,   321,   327,   330,
     333,   111,    34,    34,    34,    34,    50,    34,    34,    55,
      34,    34,    31,   116,   117,    98,    29,   133,   134,    98,
     133,    98,    30,   201,   202,   203,    98,    35,   112,   113,
      88,    96,   357,    56,    96,    98,    56,    96,    98,    34,
      56,    96,    98,    56,    96,    98,    32,    33,    34,    35,
      37,    38,    39,    40,    41,    42,    67,    68,    79,    83,
      90,   104,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,    26,   118,    31,    34,   121,
     122,    88,     7,   149,   150,    29,   139,   140,   149,   139,
      21,    88,     7,   217,   218,   219,   203,    30,   209,   210,
     211,    96,    97,    34,    34,   328,    34,   331,    96,    34,
     334,    34,   322,    34,    34,   104,   104,   104,    34,    42,
      34,    41,   104,   104,   306,   306,   306,   306,   357,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    80,    81,
      82,    84,    85,    87,    89,    90,    91,    92,    93,    94,
      32,    33,    34,   357,   303,   100,    99,    34,   135,    88,
       8,   161,   162,    88,     7,   154,   155,   161,   154,   303,
     204,    21,    88,    27,   299,   219,    21,    88,     7,   225,
     226,   227,   211,   113,   357,    98,    58,   336,    98,   336,
      98,   336,    98,    34,   324,   325,   326,   105,   306,   105,
     306,   313,    34,    34,    34,   105,   105,   306,   306,   306,
     306,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   306,   306,   306,   306,   306,    34,    34,
     357,   357,   303,   357,   100,    34,   136,   137,   138,   151,
      88,    24,   125,   141,    88,     8,   170,   171,    10,   179,
     180,   170,    88,    34,   206,   207,   208,   303,   220,   357,
     303,   212,    21,    88,    99,   227,   329,    88,    57,   342,
     332,   342,   335,   342,   323,    96,   100,    99,   326,    97,
     105,    97,   105,    97,   105,   105,    88,    96,   303,    96,
      97,    34,    43,   152,   153,   159,   160,   163,    34,   357,
      34,   142,   143,   144,   156,    88,    99,    88,    11,   249,
     250,    10,   190,   191,   205,    96,    97,    88,    34,   222,
     223,   224,    88,    34,   214,   215,   216,   303,   228,   357,
     336,   337,    88,    99,   336,    99,   336,    99,   324,    32,
      34,    90,   356,   357,   306,   306,   313,   306,    96,   138,
      88,    34,    96,    97,    13,    14,   164,   165,   357,    96,
      97,   157,   158,   160,   172,   357,   181,    15,    88,    12,
     145,   146,    88,    11,   273,   274,   206,   208,   221,   100,
      96,    97,   213,    96,    97,    88,    34,   230,   231,   232,
     342,    34,   338,   339,   340,   343,   357,   342,   357,   342,
     357,    99,    32,    96,    97,   105,   105,   105,    43,    44,
      45,    46,    47,    48,   159,    34,   166,   167,    34,   168,
     169,    13,    14,   144,    96,    13,    14,   173,   174,    34,
     182,   183,   184,    88,   252,    88,    25,   130,   192,    15,
      88,    12,   147,   148,   222,   303,   224,   214,   216,   229,
     100,   232,    99,   100,   340,    59,    60,   344,   345,   346,
     349,    99,    99,   357,   306,    44,    43,   101,    96,    97,
     101,    96,    97,   166,   168,   160,    34,   175,   176,    34,
     177,   178,    13,    14,    88,   184,   251,    18,    34,   253,
     254,   255,   256,   263,   264,   265,   267,    35,    34,   357,
      34,   193,   194,   195,    88,   276,    88,    99,   230,   303,
     357,    32,    33,    35,    37,    38,   315,   341,    60,    59,
     346,    34,    45,    46,    61,    62,    63,    64,    65,    66,
     350,   357,   357,    97,   105,   102,   167,   102,   169,    96,
      96,   101,    96,    97,   101,    96,    97,   175,   177,    34,
     253,   257,    69,   101,   302,   255,    16,    17,   103,    96,
     357,    88,   195,   275,    18,    34,   277,   278,   279,   280,
     287,   288,   289,   291,    35,   357,    96,    96,    91,   348,
      61,    62,    63,    64,    34,   306,   102,   176,   102,   178,
      96,    96,    22,   302,   357,    34,   258,   259,   303,    31,
      34,   268,   269,   271,   272,   268,   272,    34,   266,    34,
     277,   281,    69,   302,   279,    16,    17,   103,    96,   101,
     351,   347,   105,    34,    22,   357,   185,   100,    20,    97,
     102,   303,    69,   302,    21,   260,   103,    16,    31,   260,
      17,    69,   302,    22,   302,   357,    34,   282,   283,    34,
     292,   293,   295,   296,   292,   296,    34,   290,    32,    34,
     102,    54,   352,   351,   357,    34,   186,   201,   303,   262,
     263,   258,   303,    12,   261,    34,   270,    34,   268,   303,
     261,   268,    34,    22,   357,   196,   100,    20,    97,    69,
     302,   303,    21,   284,   103,    16,    34,   284,    17,    69,
     302,   102,   102,   104,   100,   353,   352,   187,   357,   201,
     189,   217,    19,    28,   263,    35,    96,    97,   358,    69,
     302,   260,   358,   260,   357,    34,   197,   209,   303,   286,
     287,   282,   303,    12,   285,    34,   294,    34,   292,   303,
     285,   292,    34,    32,    33,    34,    35,    36,    37,    38,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    80,
      81,    82,    84,    85,    87,    88,    89,    90,    91,    92,
      93,    94,    97,   103,   104,   105,   315,   354,   355,    96,
     353,   201,   188,   189,   233,   303,   357,   261,   261,   198,
     357,   209,   200,   225,    19,    28,   287,    35,    96,    69,
     302,   284,    96,   284,   105,   354,    96,   189,   201,     9,
      12,   234,   247,   358,   358,   209,   199,   200,   240,   303,
      96,   285,   285,   189,    21,    88,    88,   200,   209,     9,
      12,   241,   248,    96,    96,   303,   235,    35,   200,    21,
      88,    88,    88,    34,   237,   238,   239,    96,   303,   242,
      35,   236,   302,    96,    97,    88,    34,   244,   245,   246,
      96,   237,   239,   243,   302,    96,    97,   244,   246
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   106,   107,   108,   108,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   111,   110,   112,   112,   113,   114,   115,
     116,   116,   117,   117,   118,   118,   119,   120,   121,   121,
     122,   122,   123,   124,   125,   125,   126,   127,   128,   129,
     130,   130,   131,   132,   133,   133,   135,   134,   136,   136,
     137,   137,   138,   139,   139,   141,   140,   142,   142,   143,
     143,   144,   145,   145,   146,   147,   147,   148,   149,   149,
     151,   150,   152,   152,   153,   153,   154,   154,   156,   155,
     157,   157,   158,   158,   159,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   161,   161,   163,   162,   164,
     164,   165,   165,   165,   165,   166,   166,   167,   167,   168,
     168,   169,   169,   170,   170,   172,   171,   173,   173,   174,
     174,   174,   174,   175,   175,   176,   176,   177,   177,   178,
     178,   179,   179,   181,   180,   182,   182,   183,   183,   185,
     184,   186,   184,   187,   184,   188,   184,   189,   190,   190,
     192,   191,   193,   193,   194,   194,   196,   195,   197,   195,
     198,   195,   199,   195,   200,   201,   201,   202,   202,   204,
     203,   205,   203,   206,   206,   207,   207,   208,   209,   209,
     210,   210,   212,   211,   213,   211,   214,   214,   215,   215,
     216,   217,   217,   218,   218,   220,   219,   221,   219,   222,
     222,   223,   223,   224,   225,   225,   226,   226,   228,   227,
     229,   227,   230,   230,   231,   231,   232,   233,   233,   235,
     234,   236,   234,   237,   237,   238,   238,   239,   239,   240,
     240,   242,   241,   243,   241,   244,   244,   245,   245,   246,
     246,   247,   247,   248,   248,   249,   249,   251,   250,   252,
     250,   253,   253,   254,   254,   255,   255,   257,   256,   258,
     258,   259,   260,   260,   261,   261,   262,   262,   263,   263,
     263,   263,   264,   264,   265,   265,   266,   266,   266,   267,
     267,   267,   268,   268,   269,   269,   270,   270,   270,   271,
     271,   271,   272,   272,   272,   273,   273,   275,   274,   276,
     274,   277,   277,   278,   278,   279,   279,   281,   280,   282,
     282,   283,   284,   284,   285,   285,   286,   286,   287,   287,
     287,   287,   288,   288,   289,   289,   290,   290,   290,   291,
     291,   291,   292,   292,   293,   293,   294,   294,   294,   295,
     295,   295,   296,   296,   296,   297,   298,   299,   300,   301,
     302,   303,   303,   303,   304,   304,   304,   304,   305,   305,
     306,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   307,   307,   307,   307,   307,   308,   308,   308,   308,
     308,   309,   310,   310,   311,   312,   312,   312,   313,   314,
     314,   314,   315,   315,   315,   315,   316,   317,   318,   318,
     319,   320,   322,   321,   323,   321,   324,   324,   325,   325,
     326,   326,   328,   327,   329,   327,   331,   330,   332,   330,
     334,   333,   335,   333,   337,   336,   336,   338,   338,   339,
     339,   340,   341,   341,   341,   341,   341,   341,   343,   342,
     342,   344,   344,   345,   345,   347,   346,   348,   346,   349,
     349,   349,   349,   349,   350,   350,   350,   350,   350,   350,
     350,   350,   350,   350,   350,   350,   350,   351,   351,   351,
     351,   352,   352,   353,   353,   354,   354,   355,   355,   355,
     355,   355,   355,   355,   355,   355,   355,   355,   355,   355,
     355,   355,   355,   355,   355,   355,   355,   355,   355,   355,
     355,   355,   355,   355,   355,   355,   355,   355,   355,   355,
     355,   356,   356,   356,   357,   357,   358,   358
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     4,     3,     1,     1,     3,     2,
       0,     1,     4,     3,     3,     2,     5,     2,     1,     0,
       5,     4,     5,     2,     3,     2,     7,     2,     8,     2,
       3,     2,    10,     2,     1,     0,     0,     4,     2,     0,
       3,     1,     1,     1,     0,     0,     4,     2,     0,     3,
       1,     1,     1,     0,     4,     1,     0,     4,     1,     0,
       0,     4,     2,     0,     3,     1,     1,     0,     0,     4,
       2,     0,     3,     1,     1,     1,     3,     2,     3,     4,
       4,     3,     3,     3,     3,     1,     0,     0,     4,     1,
       0,     4,     3,     4,     3,     3,     1,     3,     1,     3,
       1,     3,     1,     1,     0,     0,     4,     1,     0,     4,
       3,     4,     3,     3,     1,     3,     1,     3,     1,     3,
       1,     1,     0,     0,     4,     1,     0,     2,     1,     0,
       7,     0,     8,     0,     9,     0,    10,     3,     1,     0,
       0,     4,     1,     0,     2,     1,     0,     7,     0,     8,
       0,     9,     0,    10,     3,     1,     0,     2,     1,     0,
       4,     0,     6,     2,     0,     3,     1,     1,     1,     0,
       2,     1,     0,     4,     0,     6,     2,     0,     3,     1,
       1,     1,     0,     2,     1,     0,     4,     0,     6,     2,
       0,     3,     1,     3,     1,     0,     2,     1,     0,     4,
       0,     6,     1,     0,     2,     1,     4,     2,     0,     0,
       4,     0,     6,     2,     0,     3,     1,     2,     1,     2,
       0,     0,     4,     0,     6,     2,     0,     3,     1,     2,
       1,     4,     0,     4,     0,     1,     0,     0,     5,     0,
       4,     1,     0,     2,     1,     1,     1,     0,     7,     3,
       1,     5,     2,     0,     2,     0,     2,     1,     6,     8,
       6,     8,     3,     1,     2,     1,     2,     1,     2,     2,
       1,     2,     3,     1,     2,     1,     2,     1,     2,     2,
       1,     2,     1,     2,     3,     1,     0,     0,     5,     0,
       4,     1,     0,     2,     1,     1,     1,     0,     7,     3,
       1,     5,     2,     0,     2,     0,     2,     1,     6,     8,
       6,     8,     3,     1,     2,     1,     2,     1,     2,     2,
       1,     2,     3,     1,     2,     1,     2,     1,     2,     2,
       1,     2,     1,     2,     3,     4,     5,     2,     6,     5,
       3,     1,     1,     1,     6,     4,     3,     1,     6,     4,
       1,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       3,     3,     2,     3,     3,     5,     3,     4,     6,     8,
      10,     1,     1,     1,     1,     1,     1,     2,     3,     3,
       2,     1,     1,     1,     1,     1,     3,     4,     1,     1,
       1,     1,     3,     3,     2,     2,     3,     3,     3,     4,
       3,     3,     0,     7,     0,     9,     1,     0,     2,     1,
       2,     4,     0,     8,     0,    10,     0,     8,     0,    10,
       0,     8,     0,    10,     0,     4,     0,     1,     0,     2,
       1,     4,     1,     1,     1,     1,     1,     1,     0,     4,
       0,     1,     0,     2,     1,     0,     8,     0,     7,     1,
       1,     2,     2,     0,     1,     2,     1,     1,     1,     1,
       2,     2,     2,     2,     1,     1,     1,     3,     3,     2,
       0,     4,     0,     2,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     0,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 6: /* definition: channeldefinition  */
#line 255 "ned.y"
                { if (ps.storeSourceCode) ps.channel->setSourceCode(toString((yylsp[0]))); }
#line 2459 "ned.tab.c"
    break;

  case 7: /* definition: simpledefinition  */
#line 257 "ned.y"
                { if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString((yylsp[0]))); }
#line 2465 "ned.tab.c"
    break;

  case 8: /* definition: moduledefinition  */
#line 259 "ned.y"
                { if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString((yylsp[0]))); }
#line 2471 "ned.tab.c"
    break;

  case 9: /* definition: networkdefinition  */
#line 261 "ned.y"
                { if (ps.storeSourceCode) ps.network->setSourceCode(toString((yylsp[0]))); }
#line 2477 "ned.tab.c"
    break;

  case 10: /* definition: channeldefinition_old  */
#line 264 "ned.y"
                { if (ps.storeSourceCode) ps.channel->setSourceCode(toString((yylsp[0]))); }
#line 2483 "ned.tab.c"
    break;

  case 11: /* definition: simpledefinition_old  */
#line 266 "ned.y"
                { if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString((yylsp[0]))); }
#line 2489 "ned.tab.c"
    break;

  case 12: /* definition: moduledefinition_old  */
#line 268 "ned.y"
                { if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString((yylsp[0]))); }
#line 2495 "ned.tab.c"
    break;

  case 13: /* definition: networkdefinition_old  */
#line 270 "ned.y"
                { if (ps.storeSourceCode) ps.network->setSourceCode(toString((yylsp[0]))); }
#line 2501 "ned.tab.c"
    break;

  case 19: /* definition: enum  */
#line 279 "ned.y"
                { if (ps.storeSourceCode) ps.enump->setSourceCode(toString((yylsp[0]))); }
#line 2507 "ned.tab.c"
    break;

  case 20: /* definition: message  */
#line 281 "ned.y"
                { if (ps.storeSourceCode) ps.messagep->setSourceCode(toString((yylsp[0]))); }
#line 2513 "ned.tab.c"
    break;

  case 21: /* definition: class  */
#line 283 "ned.y"
                { if (ps.storeSourceCode) ps.classp->setSourceCode(toString((yylsp[0]))); }
#line 2519 "ned.tab.c"
    break;

  case 22: /* definition: struct  */
#line 285 "ned.y"
                { if (ps.storeSourceCode) ps.structp->setSourceCode(toString((yylsp[0]))); }
#line 2525 "ned.tab.c"
    break;

  case 23: /* $@1: %empty  */
#line 293 "ned.y"
                {
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,(yylsp[0]));
                }
#line 2534 "ned.tab.c"
    break;

  case 24: /* import: INCLUDE $@1 filenames ';'  */
#line 298 "ned.y"
                {
                  // NOTE: no setTrailingComment(ps.imports,@3) -- comment already stored with last filename */
                }
#line 2542 "ned.tab.c"
    break;

  case 27: /* filename: STRINGCONSTANT  */
#line 310 "ned.y"
                {
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes((yylsp[0]))));
                  setComments(ps.import,(yylsp[0]));
                }
#line 2552 "ned.tab.c"
    break;

  case 29: /* channelheader_old: CHANNEL NAME  */
#line 326 "ned.y"
                {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString((yylsp[0])));
                  setComments(ps.channel,(yylsp[-1]),(yylsp[0]));
                }
#line 2562 "ned.tab.c"
    break;

  case 32: /* channelattrblock_old: channelattrblock_old CHANATTRNAME expression opt_semicolon  */
#line 340 "ned.y"
                {
                  ps.chanattr = addChanAttr(ps.channel,toString((yylsp[-2])));
                  addExpression(ps.chanattr, "value",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.channel,(yylsp[-2]),(yylsp[-1]));
                }
#line 2572 "ned.tab.c"
    break;

  case 33: /* channelattrblock_old: CHANATTRNAME expression opt_semicolon  */
#line 346 "ned.y"
                {
                  ps.chanattr = addChanAttr(ps.channel,toString((yylsp[-2])));
                  addExpression(ps.chanattr, "value",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.channel,(yylsp[-2]),(yylsp[-1]));
                }
#line 2582 "ned.tab.c"
    break;

  case 34: /* endchannel_old: ENDCHANNEL NAME opt_semicolon  */
#line 355 "ned.y"
                {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                }
#line 2590 "ned.tab.c"
    break;

  case 35: /* endchannel_old: ENDCHANNEL opt_semicolon  */
#line 359 "ned.y"
                {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                }
#line 2598 "ned.tab.c"
    break;

  case 36: /* channeldefinition: channelheader '{' opt_channelattrblock '}' opt_semicolon  */
#line 371 "ned.y"
                {
                  setTrailingComment(ps.channel,(yylsp[-1]));
                }
#line 2606 "ned.tab.c"
    break;

  case 37: /* channelheader: CHANNEL NAME  */
#line 378 "ned.y"
                {
                  use_chanattrname_token = 0;
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString((yylsp[0])));
                  setComments(ps.channel,(yylsp[-1]),(yylsp[0]));
                }
#line 2617 "ned.tab.c"
    break;

  case 40: /* channelattrblock: channelattrblock NAME '=' expression ';'  */
#line 393 "ned.y"
                {
                  ps.chanattr = addChanAttr(ps.channel,toString((yylsp[-3])));
                  addExpression(ps.chanattr, "value",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.channel,(yylsp[-3]),(yylsp[-1]));
                }
#line 2627 "ned.tab.c"
    break;

  case 41: /* channelattrblock: NAME '=' expression ';'  */
#line 399 "ned.y"
                {
                  ps.chanattr = addChanAttr(ps.channel,toString((yylsp[-3])));
                  addExpression(ps.chanattr, "value",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.channel,(yylsp[-3]),(yylsp[-1]));
                }
#line 2637 "ned.tab.c"
    break;

  case 43: /* simpleheader_old: SIMPLE NAME  */
#line 419 "ned.y"
                {
                  use_chanattrname_token = 1;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                }
#line 2648 "ned.tab.c"
    break;

  case 44: /* endsimple_old: ENDSIMPLE NAME opt_semicolon  */
#line 429 "ned.y"
                {
                  setTrailingComment(ps.module,(yylsp[-1]));
                }
#line 2656 "ned.tab.c"
    break;

  case 45: /* endsimple_old: ENDSIMPLE opt_semicolon  */
#line 433 "ned.y"
                {
                  setTrailingComment(ps.module,(yylsp[-1]));
                }
#line 2664 "ned.tab.c"
    break;

  case 46: /* simpledefinition: simpleheader '{' opt_machineblock opt_paramblock opt_gateblock '}' opt_semicolon  */
#line 447 "ned.y"
                {
                  setTrailingComment(ps.module,(yylsp[-1]));
                }
#line 2672 "ned.tab.c"
    break;

  case 47: /* simpleheader: SIMPLE NAME  */
#line 454 "ned.y"
                {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                }
#line 2682 "ned.tab.c"
    break;

  case 49: /* moduleheader_old: MODULE NAME  */
#line 478 "ned.y"
                {
                  use_chanattrname_token = 1;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                }
#line 2693 "ned.tab.c"
    break;

  case 50: /* endmodule_old: ENDMODULE NAME opt_semicolon  */
#line 488 "ned.y"
                {
                  setTrailingComment(ps.module,(yylsp[-1]));
                }
#line 2701 "ned.tab.c"
    break;

  case 51: /* endmodule_old: ENDMODULE opt_semicolon  */
#line 492 "ned.y"
                {
                  setTrailingComment(ps.module,(yylsp[-1]));
                }
#line 2709 "ned.tab.c"
    break;

  case 52: /* moduledefinition: moduleheader '{' opt_machineblock opt_paramblock opt_gateblock opt_submodblock opt_connblock opt_displayblock '}' opt_semicolon  */
#line 509 "ned.y"
                {
                  setTrailingComment(ps.module,(yylsp[-1]));
                }
#line 2717 "ned.tab.c"
    break;

  case 53: /* moduleheader: MODULE NAME  */
#line 516 "ned.y"
                {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString((yylsp[0])));
                  setComments(ps.module,(yylsp[-1]),(yylsp[0]));
                }
#line 2727 "ned.tab.c"
    break;

  case 56: /* $@2: %empty  */
#line 534 "ned.y"
                {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,(yylsp[-1]),(yylsp[0]));
                }
#line 2736 "ned.tab.c"
    break;

  case 57: /* machineblock_old: MACHINES ':' $@2 opt_machinelist_old  */
#line 539 "ned.y"
                {
                }
#line 2743 "ned.tab.c"
    break;

  case 62: /* machine_old: NAME  */
#line 555 "ned.y"
                {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString((yylsp[0])));
                  setComments(ps.machine,(yylsp[0]),(yylsp[0]));
                }
#line 2753 "ned.tab.c"
    break;

  case 65: /* $@3: %empty  */
#line 572 "ned.y"
                {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,(yylsp[-1]),(yylsp[0]));
                }
#line 2762 "ned.tab.c"
    break;

  case 66: /* machineblock: MACHINES ':' $@3 opt_machinelist  */
#line 577 "ned.y"
                {
                }
#line 2769 "ned.tab.c"
    break;

  case 71: /* machine: NAME  */
#line 593 "ned.y"
                {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString((yylsp[0])));
                  setComments(ps.machine,(yylsp[0]),(yylsp[0]));
                }
#line 2779 "ned.tab.c"
    break;

  case 74: /* displayblock_old: DISPLAY ':' STRINGCONSTANT ';'  */
#line 610 "ned.y"
                {
                  addDisplayString(ps.module,(yylsp[-1]));
                }
#line 2787 "ned.tab.c"
    break;

  case 77: /* displayblock: DISPLAY ':' STRINGCONSTANT ';'  */
#line 625 "ned.y"
                {
                  addDisplayString(ps.module,(yylsp[-1]));
                }
#line 2795 "ned.tab.c"
    break;

  case 80: /* $@4: %empty  */
#line 640 "ned.y"
                {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,(yylsp[-1]),(yylsp[0]));
                }
#line 2804 "ned.tab.c"
    break;

  case 81: /* paramblock_old: PARAMETERS ':' $@4 opt_parameters_old  */
#line 645 "ned.y"
                {
                }
#line 2811 "ned.tab.c"
    break;

  case 84: /* parameters_old: parameters_old ',' parameter_old  */
#line 656 "ned.y"
                {
                  setComments(ps.param,(yylsp[0]));
                }
#line 2819 "ned.tab.c"
    break;

  case 85: /* parameters_old: parameter_old  */
#line 660 "ned.y"
                {
                  setComments(ps.param,(yylsp[0]));
                }
#line 2827 "ned.tab.c"
    break;

  case 88: /* $@5: %empty  */
#line 675 "ned.y"
                {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,(yylsp[-1]),(yylsp[0]));
                }
#line 2836 "ned.tab.c"
    break;

  case 89: /* paramblock: PARAMETERS ':' $@5 opt_parameters  */
#line 680 "ned.y"
                {
                }
#line 2843 "ned.tab.c"
    break;

  case 92: /* parameters: parameters ';' parameter  */
#line 691 "ned.y"
                {
                  setComments(ps.param,(yylsp[0]));
                }
#line 2851 "ned.tab.c"
    break;

  case 93: /* parameters: parameter  */
#line 695 "ned.y"
                {
                  setComments(ps.param,(yylsp[0]));
                }
#line 2859 "ned.tab.c"
    break;

  case 95: /* parameter: NAME  */
#line 710 "ned.y"
                {
                  ps.param = addParameter(ps.params,(yylsp[0]),TYPE_NUMERIC);
                }
#line 2867 "ned.tab.c"
    break;

  case 96: /* parameter: NAME ':' NUMERICTYPE  */
#line 714 "ned.y"
                {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_NUMERIC);
                }
#line 2875 "ned.tab.c"
    break;

  case 97: /* parameter: CONSTDECL NAME  */
#line 718 "ned.y"
                {
                  ps.param = addParameter(ps.params,(yylsp[0]),TYPE_CONST_NUM);
                }
#line 2883 "ned.tab.c"
    break;

  case 98: /* parameter: NAME ':' CONSTDECL  */
#line 722 "ned.y"
                {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_CONST_NUM);
                }
#line 2891 "ned.tab.c"
    break;

  case 99: /* parameter: NAME ':' CONSTDECL NUMERICTYPE  */
#line 726 "ned.y"
                {
                  ps.param = addParameter(ps.params,(yylsp[-3]),TYPE_CONST_NUM);
                }
#line 2899 "ned.tab.c"
    break;

  case 100: /* parameter: NAME ':' NUMERICTYPE CONSTDECL  */
#line 730 "ned.y"
                {
                  ps.param = addParameter(ps.params,(yylsp[-3]),TYPE_CONST_NUM);
                }
#line 2907 "ned.tab.c"
    break;

  case 101: /* parameter: NAME ':' STRINGTYPE  */
#line 734 "ned.y"
                {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_STRING);
                }
#line 2915 "ned.tab.c"
    break;

  case 102: /* parameter: NAME ':' BOOLTYPE  */
#line 738 "ned.y"
                {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_BOOL);
                }
#line 2923 "ned.tab.c"
    break;

  case 103: /* parameter: NAME ':' XMLTYPE  */
#line 742 "ned.y"
                {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_XML);
                }
#line 2931 "ned.tab.c"
    break;

  case 104: /* parameter: NAME ':' ANYTYPE  */
#line 746 "ned.y"
                {
                  ps.param = addParameter(ps.params,(yylsp[-2]),TYPE_ANYTYPE);
                }
#line 2939 "ned.tab.c"
    break;

  case 107: /* $@6: %empty  */
#line 761 "ned.y"
                {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,(yylsp[-1]),(yylsp[0]));
                }
#line 2948 "ned.tab.c"
    break;

  case 108: /* gateblock_old: GATES ':' $@6 opt_gates_old  */
#line 766 "ned.y"
                {
                }
#line 2955 "ned.tab.c"
    break;

  case 117: /* gateI_old: NAME '[' ']'  */
#line 789 "ned.y"
                {
                  ps.gate = addGate(ps.gates, (yylsp[-2]), 1, 1 );
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                }
#line 2964 "ned.tab.c"
    break;

  case 118: /* gateI_old: NAME  */
#line 794 "ned.y"
                {
                  ps.gate = addGate(ps.gates, (yylsp[0]), 1, 0 );
                  setComments(ps.gate,(yylsp[0]));
                }
#line 2973 "ned.tab.c"
    break;

  case 121: /* gateO_old: NAME '[' ']'  */
#line 807 "ned.y"
                {
                  ps.gate = addGate(ps.gates, (yylsp[-2]), 0, 1 );
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                }
#line 2982 "ned.tab.c"
    break;

  case 122: /* gateO_old: NAME  */
#line 812 "ned.y"
                {
                  ps.gate = addGate(ps.gates, (yylsp[0]), 0, 0 );
                  setComments(ps.gate,(yylsp[0]));
                }
#line 2991 "ned.tab.c"
    break;

  case 125: /* $@7: %empty  */
#line 828 "ned.y"
                {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,(yylsp[-1]),(yylsp[0]));
                }
#line 3000 "ned.tab.c"
    break;

  case 126: /* gateblock: GATES ':' $@7 opt_gates  */
#line 833 "ned.y"
                {
                }
#line 3007 "ned.tab.c"
    break;

  case 135: /* gateI: NAME '[' ']'  */
#line 856 "ned.y"
                {
                  ps.gate = addGate(ps.gates, (yylsp[-2]), 1, 1 );
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                }
#line 3016 "ned.tab.c"
    break;

  case 136: /* gateI: NAME  */
#line 861 "ned.y"
                {
                  ps.gate = addGate(ps.gates, (yylsp[0]), 1, 0 );
                  setComments(ps.gate,(yylsp[0]));
                }
#line 3025 "ned.tab.c"
    break;

  case 139: /* gateO: NAME '[' ']'  */
#line 874 "ned.y"
                {
                  ps.gate = addGate(ps.gates, (yylsp[-2]), 0, 1 );
                  setComments(ps.gate,(yylsp[-2]),(yylsp[0]));
                }
#line 3034 "ned.tab.c"
    break;

  case 140: /* gateO: NAME  */
#line 879 "ned.y"
                {
                  ps.gate = addGate(ps.gates, (yylsp[0]), 0, 0 );
                  setComments(ps.gate,(yylsp[0]));
                }
#line 3043 "ned.tab.c"
    break;

  case 143: /* $@8: %empty  */
#line 895 "ned.y"
                {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,(yylsp[-1]),(yylsp[0]));
                }
#line 3052 "ned.tab.c"
    break;

  case 144: /* submodblock_old: SUBMODULES ':' $@8 opt_submodules_old  */
#line 900 "ned.y"
                {
                }
#line 3059 "ned.tab.c"
    break;

  case 149: /* $@9: %empty  */
#line 916 "ned.y"
                {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-3]), (yylsp[-1]), NULLPOS);
                  setComments(ps.submod,(yylsp[-3]),(yylsp[0]));
                }
#line 3068 "ned.tab.c"
    break;

  case 150: /* submodule_old: NAME ':' NAME opt_semicolon $@9 opt_on_blocks_old submodule_body_old  */
#line 922 "ned.y"
                {
                }
#line 3075 "ned.tab.c"
    break;

  case 151: /* $@10: %empty  */
#line 925 "ned.y"
                {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-4]), (yylsp[-2]), NULLPOS);
                  addVector(ps.submod, "vector-size",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.submod,(yylsp[-4]),(yylsp[0]));
                }
#line 3085 "ned.tab.c"
    break;

  case 152: /* submodule_old: NAME ':' NAME vector opt_semicolon $@10 opt_on_blocks_old submodule_body_old  */
#line 932 "ned.y"
                {
                }
#line 3092 "ned.tab.c"
    break;

  case 153: /* $@11: %empty  */
#line 935 "ned.y"
                {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-5]), (yylsp[-1]), (yylsp[-3]));
                  setComments(ps.submod,(yylsp[-5]),(yylsp[0]));
                }
#line 3101 "ned.tab.c"
    break;

  case 154: /* submodule_old: NAME ':' NAME LIKE NAME opt_semicolon $@11 opt_on_blocks_old submodule_body_old  */
#line 941 "ned.y"
                {
                }
#line 3108 "ned.tab.c"
    break;

  case 155: /* $@12: %empty  */
#line 944 "ned.y"
                {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-6]), (yylsp[-1]), (yylsp[-4]));
                  addVector(ps.submod, "vector-size",(yylsp[-3]),yyvsp[-3]);
                  setComments(ps.submod,(yylsp[-6]),(yylsp[0]));
                }
#line 3118 "ned.tab.c"
    break;

  case 156: /* submodule_old: NAME ':' NAME vector LIKE NAME opt_semicolon $@12 opt_on_blocks_old submodule_body_old  */
#line 951 "ned.y"
                {
                }
#line 3125 "ned.tab.c"
    break;

  case 160: /* $@13: %empty  */
#line 971 "ned.y"
                {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,(yylsp[-1]),(yylsp[0]));
                }
#line 3134 "ned.tab.c"
    break;

  case 161: /* submodblock: SUBMODULES ':' $@13 opt_submodules  */
#line 976 "ned.y"
                {
                }
#line 3141 "ned.tab.c"
    break;

  case 166: /* $@14: %empty  */
#line 992 "ned.y"
                {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-3]), (yylsp[-1]), NULLPOS);
                  setComments(ps.submod,(yylsp[-3]),(yylsp[0]));
                }
#line 3150 "ned.tab.c"
    break;

  case 167: /* submodule: NAME ':' NAME opt_semicolon $@14 opt_on_blocks submodule_body  */
#line 998 "ned.y"
                {
                }
#line 3157 "ned.tab.c"
    break;

  case 168: /* $@15: %empty  */
#line 1001 "ned.y"
                {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-4]), (yylsp[-2]), NULLPOS);
                  addVector(ps.submod, "vector-size",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.submod,(yylsp[-4]),(yylsp[0]));
                }
#line 3167 "ned.tab.c"
    break;

  case 169: /* submodule: NAME ':' NAME vector opt_semicolon $@15 opt_on_blocks submodule_body  */
#line 1008 "ned.y"
                {
                }
#line 3174 "ned.tab.c"
    break;

  case 170: /* $@16: %empty  */
#line 1011 "ned.y"
                {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-5]), (yylsp[-1]), (yylsp[-3]));
                  setComments(ps.submod,(yylsp[-5]),(yylsp[0]));
                }
#line 3183 "ned.tab.c"
    break;

  case 171: /* submodule: NAME ':' NAME LIKE NAME opt_semicolon $@16 opt_on_blocks submodule_body  */
#line 1017 "ned.y"
                {
                }
#line 3190 "ned.tab.c"
    break;

  case 172: /* $@17: %empty  */
#line 1020 "ned.y"
                {
                  ps.submod = addSubmodule(ps.submods, (yylsp[-6]), (yylsp[-1]), (yylsp[-4]));
                  addVector(ps.submod, "vector-size",(yylsp[-3]),yyvsp[-3]);
                  setComments(ps.submod,(yylsp[-6]),(yylsp[0]));
                }
#line 3200 "ned.tab.c"
    break;

  case 173: /* submodule: NAME ':' NAME vector LIKE NAME opt_semicolon $@17 opt_on_blocks submodule_body  */
#line 1027 "ned.y"
                {
                }
#line 3207 "ned.tab.c"
    break;

  case 179: /* $@18: %empty  */
#line 1052 "ned.y"
                {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,(yylsp[-1]),(yylsp[0]));
                }
#line 3216 "ned.tab.c"
    break;

  case 180: /* on_block_old: ON ':' $@18 opt_on_list_old  */
#line 1057 "ned.y"
                {
                }
#line 3223 "ned.tab.c"
    break;

  case 181: /* $@19: %empty  */
#line 1060 "ned.y"
                {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.substmachines,(yylsp[-3]),(yylsp[0]));
                }
#line 3233 "ned.tab.c"
    break;

  case 182: /* on_block_old: ON IF expression ':' $@19 opt_on_list_old  */
#line 1066 "ned.y"
                {
                }
#line 3240 "ned.tab.c"
    break;

  case 187: /* on_mach_old: NAME  */
#line 1082 "ned.y"
                {
                  ps.substmachine = addSubstmachine(ps.substmachines,(yylsp[0]));
                  setComments(ps.substmachine,(yylsp[0]));
                }
#line 3249 "ned.tab.c"
    break;

  case 192: /* $@20: %empty  */
#line 1103 "ned.y"
                {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,(yylsp[-1]),(yylsp[0]));
                }
#line 3258 "ned.tab.c"
    break;

  case 193: /* on_block: ON ':' $@20 opt_on_list  */
#line 1108 "ned.y"
                {
                }
#line 3265 "ned.tab.c"
    break;

  case 194: /* $@21: %empty  */
#line 1111 "ned.y"
                {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.substmachines,(yylsp[-3]),(yylsp[0]));
                }
#line 3275 "ned.tab.c"
    break;

  case 195: /* on_block: ON IF expression ':' $@21 opt_on_list  */
#line 1117 "ned.y"
                {
                }
#line 3282 "ned.tab.c"
    break;

  case 200: /* on_mach: NAME  */
#line 1133 "ned.y"
                {
                  ps.substmachine = addSubstmachine(ps.substmachines,(yylsp[0]));
                  setComments(ps.substmachine,(yylsp[0]));
                }
#line 3291 "ned.tab.c"
    break;

  case 205: /* $@22: %empty  */
#line 1154 "ned.y"
                {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,(yylsp[-1]),(yylsp[0]));
                }
#line 3300 "ned.tab.c"
    break;

  case 206: /* substparamblock_old: PARAMETERS ':' $@22 opt_substparameters_old  */
#line 1159 "ned.y"
                {
                }
#line 3307 "ned.tab.c"
    break;

  case 207: /* $@23: %empty  */
#line 1162 "ned.y"
                {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.substparams,(yylsp[-3]),(yylsp[0]));
                }
#line 3317 "ned.tab.c"
    break;

  case 208: /* substparamblock_old: PARAMETERS IF expression ':' $@23 opt_substparameters_old  */
#line 1168 "ned.y"
                {
                }
#line 3324 "ned.tab.c"
    break;

  case 213: /* substparameter_old: NAME '=' expression  */
#line 1185 "ned.y"
                {
                  ps.substparam = addSubstparam(ps.substparams,(yylsp[-2]));
                  addExpression(ps.substparam, "value",(yylsp[0]),yyvsp[0]);
                  setComments(ps.substparam,(yylsp[-2]),(yylsp[0]));
                }
#line 3334 "ned.tab.c"
    break;

  case 218: /* $@24: %empty  */
#line 1207 "ned.y"
                {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,(yylsp[-1]),(yylsp[0]));
                }
#line 3343 "ned.tab.c"
    break;

  case 219: /* substparamblock: PARAMETERS ':' $@24 opt_substparameters  */
#line 1212 "ned.y"
                {
                }
#line 3350 "ned.tab.c"
    break;

  case 220: /* $@25: %empty  */
#line 1215 "ned.y"
                {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.substparams,(yylsp[-3]),(yylsp[0]));
                }
#line 3360 "ned.tab.c"
    break;

  case 221: /* substparamblock: PARAMETERS IF expression ':' $@25 opt_substparameters  */
#line 1221 "ned.y"
                {
                }
#line 3367 "ned.tab.c"
    break;

  case 226: /* substparameter: NAME '=' expression ';'  */
#line 1238 "ned.y"
                {
                  ps.substparam = addSubstparam(ps.substparams,(yylsp[-3]));
                  addExpression(ps.substparam, "value",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.substparam,(yylsp[-3]),(yylsp[-1]));
                }
#line 3377 "ned.tab.c"
    break;

  case 229: /* $@26: %empty  */
#line 1255 "ned.y"
                {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,(yylsp[-1]),(yylsp[0]));
                }
#line 3386 "ned.tab.c"
    break;

  case 230: /* gatesizeblock_old: GATESIZES ':' $@26 opt_gatesizes_old  */
#line 1260 "ned.y"
                {
                }
#line 3393 "ned.tab.c"
    break;

  case 231: /* $@27: %empty  */
#line 1263 "ned.y"
                {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.gatesizes,(yylsp[-3]),(yylsp[0]));
                }
#line 3403 "ned.tab.c"
    break;

  case 232: /* gatesizeblock_old: GATESIZES IF expression ':' $@27 opt_gatesizes_old  */
#line 1269 "ned.y"
                {
                }
#line 3410 "ned.tab.c"
    break;

  case 237: /* gatesize_old: NAME vector  */
#line 1285 "ned.y"
                {
                  ps.gatesize = addGateSize(ps.gatesizes,(yylsp[-1]));
                  addVector(ps.gatesize, "vector-size",(yylsp[0]),yyvsp[0]);

                  setComments(ps.gatesize,(yylsp[-1]),(yylsp[0]));
                }
#line 3421 "ned.tab.c"
    break;

  case 238: /* gatesize_old: NAME  */
#line 1292 "ned.y"
                {
                  ps.gatesize = addGateSize(ps.gatesizes,(yylsp[0]));
                  setComments(ps.gatesize,(yylsp[0]));
                }
#line 3430 "ned.tab.c"
    break;

  case 241: /* $@28: %empty  */
#line 1308 "ned.y"
                {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,(yylsp[-1]),(yylsp[0]));
                }
#line 3439 "ned.tab.c"
    break;

  case 242: /* gatesizeblock: GATESIZES ':' $@28 opt_gatesizes  */
#line 1313 "ned.y"
                {
                }
#line 3446 "ned.tab.c"
    break;

  case 243: /* $@29: %empty  */
#line 1316 "ned.y"
                {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",(yylsp[-1]),yyvsp[-1]);
                  setComments(ps.gatesizes,(yylsp[-3]),(yylsp[0]));
                }
#line 3456 "ned.tab.c"
    break;

  case 244: /* gatesizeblock: GATESIZES IF expression ':' $@29 opt_gatesizes  */
#line 1322 "ned.y"
                {
                }
#line 3463 "ned.tab.c"
    break;

  case 249: /* gatesize: NAME vector  */
#line 1338 "ned.y"
                {
                  ps.gatesize = addGateSize(ps.gatesizes,(yylsp[-1]));
                  addVector(ps.gatesize, "vector-size",(yylsp[0]),yyvsp[0]);

                  setComments(ps.gatesize,(yylsp[-1]),(yylsp[0]));
                }
#line 3474 "ned.tab.c"
    break;

  case 250: /* gatesize: NAME  */
#line 1345 "ned.y"
                {
                  ps.gatesize = addGateSize(ps.gatesizes,(yylsp[0]));
                  setComments(ps.gatesize,(yylsp[0]));
                }
#line 3483 "ned.tab.c"
    break;

  case 251: /* opt_submod_displayblock_old: DISPLAY ':' STRINGCONSTANT ';'  */
#line 1356 "ned.y"
                {
                  addDisplayString(ps.submod,(yylsp[-1]));
                }
#line 3491 "ned.tab.c"
    break;

  case 253: /* opt_submod_displayblock: DISPLAY ':' STRINGCONSTANT ';'  */
#line 1367 "ned.y"
                {
                  addDisplayString(ps.submod,(yylsp[-1]));
                }
#line 3499 "ned.tab.c"
    break;

  case 257: /* $@30: %empty  */
#line 1383 "ned.y"
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,(yylsp[-2]),(yylsp[0]));
                }
#line 3509 "ned.tab.c"
    break;

  case 258: /* connblock_old: CONNECTIONS NOCHECK ':' $@30 opt_connections_old  */
#line 1389 "ned.y"
                {
                }
#line 3516 "ned.tab.c"
    break;

  case 259: /* $@31: %empty  */
#line 1392 "ned.y"
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,(yylsp[-1]),(yylsp[0]));
                }
#line 3526 "ned.tab.c"
    break;

  case 260: /* connblock_old: CONNECTIONS ':' $@31 opt_connections_old  */
#line 1398 "ned.y"
                {
                }
#line 3533 "ned.tab.c"
    break;

  case 267: /* $@32: %empty  */
#line 1419 "ned.y"
                {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                }
#line 3542 "ned.tab.c"
    break;

  case 268: /* loopconnection_old: FOR $@32 loopvarlist_old DO notloopconnections_old ENDFOR opt_semicolon  */
#line 1424 "ned.y"
                {
                  ps.inLoop=0;
                  setComments(ps.forloop,(yylsp[-6]),(yylsp[-3]));
                  setTrailingComment(ps.forloop,(yylsp[-1]));
                }
#line 3552 "ned.tab.c"
    break;

  case 271: /* loopvar_old: NAME '=' expression TO expression  */
#line 1438 "ned.y"
                {
                  ps.loopvar = addLoopVar(ps.forloop,(yylsp[-4]));
                  addExpression(ps.loopvar, "from-value",(yylsp[-2]),yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",(yylsp[0]),yyvsp[0]);
                  setComments(ps.loopvar,(yylsp[-4]),(yylsp[0]));
                }
#line 3563 "ned.tab.c"
    break;

  case 272: /* opt_conncondition_old: IF expression  */
#line 1448 "ned.y"
                {
                  addExpression(ps.conn, "condition",(yylsp[0]),yyvsp[0]);
                }
#line 3571 "ned.tab.c"
    break;

  case 274: /* opt_conn_displaystr_old: DISPLAY STRINGCONSTANT  */
#line 1456 "ned.y"
                {
                  addDisplayString(ps.conn,(yylsp[0]));
                }
#line 3579 "ned.tab.c"
    break;

  case 278: /* notloopconnection_old: leftgatespec_old RIGHT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon  */
#line 1469 "ned.y"
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                }
#line 3588 "ned.tab.c"
    break;

  case 279: /* notloopconnection_old: leftgatespec_old RIGHT_ARROW channeldescr_old RIGHT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon  */
#line 1474 "ned.y"
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                }
#line 3597 "ned.tab.c"
    break;

  case 280: /* notloopconnection_old: leftgatespec_old LEFT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon  */
#line 1479 "ned.y"
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                }
#line 3607 "ned.tab.c"
    break;

  case 281: /* notloopconnection_old: leftgatespec_old LEFT_ARROW channeldescr_old LEFT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon  */
#line 1485 "ned.y"
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                }
#line 3617 "ned.tab.c"
    break;

  case 284: /* leftmod_old: NAME vector  */
#line 1499 "ned.y"
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "src-module-index",(yylsp[0]),yyvsp[0]);
                }
#line 3627 "ned.tab.c"
    break;

  case 285: /* leftmod_old: NAME  */
#line 1505 "ned.y"
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[0])) );
                }
#line 3636 "ned.tab.c"
    break;

  case 286: /* leftgate_old: NAME vector  */
#line 1513 "ned.y"
                {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),yyvsp[0]);
                }
#line 3645 "ned.tab.c"
    break;

  case 287: /* leftgate_old: NAME  */
#line 1518 "ned.y"
                {
                  ps.conn->setSrcGate( toString( (yylsp[0])) );
                }
#line 3653 "ned.tab.c"
    break;

  case 288: /* leftgate_old: NAME PLUSPLUS  */
#line 1522 "ned.y"
                {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  ps.conn->setSrcGatePlusplus(true);
                }
#line 3662 "ned.tab.c"
    break;

  case 289: /* parentleftgate_old: NAME vector  */
#line 1530 "ned.y"
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),yyvsp[0]);
                }
#line 3673 "ned.tab.c"
    break;

  case 290: /* parentleftgate_old: NAME  */
#line 1537 "ned.y"
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[0])));
                }
#line 3683 "ned.tab.c"
    break;

  case 291: /* parentleftgate_old: NAME PLUSPLUS  */
#line 1543 "ned.y"
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  ps.conn->setSrcGatePlusplus(true);
                }
#line 3694 "ned.tab.c"
    break;

  case 294: /* rightmod_old: NAME vector  */
#line 1558 "ned.y"
                {
                  ps.conn->setDestModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "dest-module-index",(yylsp[0]),yyvsp[0]);
                }
#line 3703 "ned.tab.c"
    break;

  case 295: /* rightmod_old: NAME  */
#line 1563 "ned.y"
                {
                  ps.conn->setDestModule( toString((yylsp[0])) );
                }
#line 3711 "ned.tab.c"
    break;

  case 296: /* rightgate_old: NAME vector  */
#line 1570 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),yyvsp[0]);
                }
#line 3720 "ned.tab.c"
    break;

  case 297: /* rightgate_old: NAME  */
#line 1575 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                }
#line 3728 "ned.tab.c"
    break;

  case 298: /* rightgate_old: NAME PLUSPLUS  */
#line 1579 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                }
#line 3737 "ned.tab.c"
    break;

  case 299: /* parentrightgate_old: NAME vector  */
#line 1587 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),yyvsp[0]);
                }
#line 3746 "ned.tab.c"
    break;

  case 300: /* parentrightgate_old: NAME  */
#line 1592 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                }
#line 3754 "ned.tab.c"
    break;

  case 301: /* parentrightgate_old: NAME PLUSPLUS  */
#line 1596 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                }
#line 3763 "ned.tab.c"
    break;

  case 302: /* channeldescr_old: NAME  */
#line 1605 "ned.y"
                {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",(yylsp[0]),createExpression(createConst(NED_CONST_STRING, toString((yylsp[0])))));
                }
#line 3772 "ned.tab.c"
    break;

  case 303: /* channeldescr_old: CHANATTRNAME expression  */
#line 1610 "ned.y"
                {
                  ps.connattr = addConnAttr(ps.conn,toString((yylsp[-1])));
                  addExpression(ps.connattr, "value",(yylsp[0]),yyvsp[0]);
                }
#line 3781 "ned.tab.c"
    break;

  case 304: /* channeldescr_old: channeldescr_old CHANATTRNAME expression  */
#line 1615 "ned.y"
                {
                  ps.connattr = addConnAttr(ps.conn,toString((yylsp[-1])));
                  addExpression(ps.connattr, "value",(yylsp[0]),yyvsp[0]);
                }
#line 3790 "ned.tab.c"
    break;

  case 307: /* $@33: %empty  */
#line 1631 "ned.y"
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,(yylsp[-2]),(yylsp[0]));
                }
#line 3800 "ned.tab.c"
    break;

  case 308: /* connblock: CONNECTIONS NOCHECK ':' $@33 opt_connections  */
#line 1637 "ned.y"
                {
                }
#line 3807 "ned.tab.c"
    break;

  case 309: /* $@34: %empty  */
#line 1640 "ned.y"
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,(yylsp[-1]),(yylsp[0]));
                }
#line 3817 "ned.tab.c"
    break;

  case 310: /* connblock: CONNECTIONS ':' $@34 opt_connections  */
#line 1646 "ned.y"
                {
                }
#line 3824 "ned.tab.c"
    break;

  case 317: /* $@35: %empty  */
#line 1667 "ned.y"
                {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                }
#line 3833 "ned.tab.c"
    break;

  case 318: /* loopconnection: FOR $@35 loopvarlist DO notloopconnections ENDFOR ';'  */
#line 1672 "ned.y"
                {
                  ps.inLoop=0;
                  setComments(ps.forloop,(yylsp[-6]),(yylsp[-3]));
                  setTrailingComment(ps.forloop,(yylsp[-1]));
                }
#line 3843 "ned.tab.c"
    break;

  case 321: /* loopvar: NAME '=' expression TO expression  */
#line 1686 "ned.y"
                {
                  ps.loopvar = addLoopVar(ps.forloop,(yylsp[-4]));
                  addExpression(ps.loopvar, "from-value",(yylsp[-2]),yyvsp[-2]);
                  addExpression(ps.loopvar, "to-value",(yylsp[0]),yyvsp[0]);
                  setComments(ps.loopvar,(yylsp[-4]),(yylsp[0]));
                }
#line 3854 "ned.tab.c"
    break;

  case 322: /* opt_conncondition: IF expression  */
#line 1696 "ned.y"
                {
                  addExpression(ps.conn, "condition",(yylsp[0]),yyvsp[0]);
                }
#line 3862 "ned.tab.c"
    break;

  case 324: /* opt_conn_displaystr: DISPLAY STRINGCONSTANT  */
#line 1704 "ned.y"
                {
                  addDisplayString(ps.conn,(yylsp[0]));
                }
#line 3870 "ned.tab.c"
    break;

  case 328: /* notloopconnection: leftgatespec RIGHT_ARROW rightgatespec opt_conncondition opt_conn_displaystr ';'  */
#line 1717 "ned.y"
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                }
#line 3879 "ned.tab.c"
    break;

  case 329: /* notloopconnection: leftgatespec RIGHT_ARROW channeldescr RIGHT_ARROW rightgatespec opt_conncondition opt_conn_displaystr ';'  */
#line 1722 "ned.y"
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                }
#line 3888 "ned.tab.c"
    break;

  case 330: /* notloopconnection: leftgatespec LEFT_ARROW rightgatespec opt_conncondition opt_conn_displaystr ';'  */
#line 1727 "ned.y"
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,(yylsp[-5]),(yylsp[-1]));
                }
#line 3898 "ned.tab.c"
    break;

  case 331: /* notloopconnection: leftgatespec LEFT_ARROW channeldescr LEFT_ARROW rightgatespec opt_conncondition opt_conn_displaystr ';'  */
#line 1733 "ned.y"
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,(yylsp[-7]),(yylsp[-1]));
                }
#line 3908 "ned.tab.c"
    break;

  case 334: /* leftmod: NAME vector  */
#line 1747 "ned.y"
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "src-module-index",(yylsp[0]),yyvsp[0]);
                }
#line 3918 "ned.tab.c"
    break;

  case 335: /* leftmod: NAME  */
#line 1753 "ned.y"
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString((yylsp[0])) );
                }
#line 3927 "ned.tab.c"
    break;

  case 336: /* leftgate: NAME vector  */
#line 1761 "ned.y"
                {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),yyvsp[0]);
                }
#line 3936 "ned.tab.c"
    break;

  case 337: /* leftgate: NAME  */
#line 1766 "ned.y"
                {
                  ps.conn->setSrcGate( toString( (yylsp[0])) );
                }
#line 3944 "ned.tab.c"
    break;

  case 338: /* leftgate: NAME PLUSPLUS  */
#line 1770 "ned.y"
                {
                  ps.conn->setSrcGate( toString( (yylsp[-1])) );
                  ps.conn->setSrcGatePlusplus(true);
                }
#line 3953 "ned.tab.c"
    break;

  case 339: /* parentleftgate: NAME vector  */
#line 1778 "ned.y"
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  addVector(ps.conn, "src-gate-index",(yylsp[0]),yyvsp[0]);
                }
#line 3964 "ned.tab.c"
    break;

  case 340: /* parentleftgate: NAME  */
#line 1785 "ned.y"
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[0])));
                }
#line 3974 "ned.tab.c"
    break;

  case 341: /* parentleftgate: NAME PLUSPLUS  */
#line 1791 "ned.y"
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString((yylsp[-1])));
                  ps.conn->setSrcGatePlusplus(true);
                }
#line 3985 "ned.tab.c"
    break;

  case 344: /* rightmod: NAME vector  */
#line 1806 "ned.y"
                {
                  ps.conn->setDestModule( toString((yylsp[-1])) );
                  addVector(ps.conn, "dest-module-index",(yylsp[0]),yyvsp[0]);
                }
#line 3994 "ned.tab.c"
    break;

  case 345: /* rightmod: NAME  */
#line 1811 "ned.y"
                {
                  ps.conn->setDestModule( toString((yylsp[0])) );
                }
#line 4002 "ned.tab.c"
    break;

  case 346: /* rightgate: NAME vector  */
#line 1818 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),yyvsp[0]);
                }
#line 4011 "ned.tab.c"
    break;

  case 347: /* rightgate: NAME  */
#line 1823 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                }
#line 4019 "ned.tab.c"
    break;

  case 348: /* rightgate: NAME PLUSPLUS  */
#line 1827 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                }
#line 4028 "ned.tab.c"
    break;

  case 349: /* parentrightgate: NAME vector  */
#line 1835 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  addVector(ps.conn, "dest-gate-index",(yylsp[0]),yyvsp[0]);
                }
#line 4037 "ned.tab.c"
    break;

  case 350: /* parentrightgate: NAME  */
#line 1840 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[0])) );
                }
#line 4045 "ned.tab.c"
    break;

  case 351: /* parentrightgate: NAME PLUSPLUS  */
#line 1844 "ned.y"
                {
                  ps.conn->setDestGate( toString( (yylsp[-1])) );
                  ps.conn->setDestGatePlusplus(true);
                }
#line 4054 "ned.tab.c"
    break;

  case 352: /* channeldescr: NAME  */
#line 1853 "ned.y"
                {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",(yylsp[0]),createExpression(createConst(NED_CONST_STRING, toString((yylsp[0])))));
                }
#line 4063 "ned.tab.c"
    break;

  case 353: /* channeldescr: NAME expression  */
#line 1858 "ned.y"
                {
                  ps.connattr = addConnAttr(ps.conn,toString((yylsp[-1])));
                  addExpression(ps.connattr, "value",(yylsp[0]),yyvsp[0]);
                }
#line 4072 "ned.tab.c"
    break;

  case 354: /* channeldescr: channeldescr NAME expression  */
#line 1863 "ned.y"
                {
                  ps.connattr = addConnAttr(ps.conn,toString((yylsp[-1])));
                  addExpression(ps.connattr, "value",(yylsp[0]),yyvsp[0]);
                }
#line 4081 "ned.tab.c"
    break;

  case 356: /* networkheader_old: NETWORK NAME ':' NAME opt_semicolon  */
#line 1881 "ned.y"
                {
                  ps.network = addNetwork(ps.nedfile,(yylsp[-3]),(yylsp[-1]));
                  setComments(ps.network,(yylsp[-4]),(yylsp[0]));
                  ps.inNetwork=1;
                }
#line 4091 "ned.tab.c"
    break;

  case 357: /* endnetwork_old: ENDNETWORK opt_semicolon  */
#line 1890 "ned.y"
                {
                  setTrailingComment(ps.network,(yylsp[-1]));
                  ps.inNetwork=0;
                }
#line 4100 "ned.tab.c"
    break;

  case 358: /* networkdefinition: networkheader '{' opt_on_blocks opt_substparamblocks '}' opt_semicolon  */
#line 1904 "ned.y"
                {
                  setTrailingComment(ps.network,(yylsp[-1]));
                  ps.inNetwork=0;
                }
#line 4109 "ned.tab.c"
    break;

  case 359: /* networkheader: NETWORK NAME ':' NAME opt_semicolon  */
#line 1912 "ned.y"
                {
                  ps.network = addNetwork(ps.nedfile,(yylsp[-3]),(yylsp[-1]));
                  setComments(ps.network,(yylsp[-4]),(yylsp[0]));
                  ps.inNetwork=1;
                }
#line 4119 "ned.tab.c"
    break;

  case 360: /* vector: '[' expression ']'  */
#line 1925 "ned.y"
                { yyval = yyvsp[-1]; }
#line 4125 "ned.tab.c"
    break;

  case 361: /* expression: expr  */
#line 1931 "ned.y"
                {
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
#line 4133 "ned.tab.c"
    break;

  case 362: /* expression: inputvalue  */
#line 1935 "ned.y"
                {
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
#line 4141 "ned.tab.c"
    break;

  case 363: /* expression: xmldocvalue  */
#line 1939 "ned.y"
                {
                  if (ps.parseExpressions) yyval = createExpression(yyvsp[0]);
                }
#line 4149 "ned.tab.c"
    break;

  case 364: /* inputvalue: INPUT_ '(' expr ',' expr ')'  */
#line 1950 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-3], yyvsp[-1]); }
#line 4155 "ned.tab.c"
    break;

  case 365: /* inputvalue: INPUT_ '(' expr ')'  */
#line 1952 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction("input", yyvsp[-1]); }
#line 4161 "ned.tab.c"
    break;

  case 366: /* inputvalue: INPUT_ '(' ')'  */
#line 1954 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction("input"); }
#line 4167 "ned.tab.c"
    break;

  case 367: /* inputvalue: INPUT_  */
#line 1956 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction("input"); }
#line 4173 "ned.tab.c"
    break;

  case 368: /* xmldocvalue: XMLDOC '(' stringconstant ',' stringconstant ')'  */
#line 1961 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-3], yyvsp[-1]); }
#line 4179 "ned.tab.c"
    break;

  case 369: /* xmldocvalue: XMLDOC '(' stringconstant ')'  */
#line 1963 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction("xmldoc", yyvsp[-1]); }
#line 4185 "ned.tab.c"
    break;

  case 371: /* expr: '(' expr ')'  */
#line 1969 "ned.y"
                { yyval = yyvsp[-1]; }
#line 4191 "ned.tab.c"
    break;

  case 372: /* expr: expr '+' expr  */
#line 1972 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("+", yyvsp[-2], yyvsp[0]); }
#line 4197 "ned.tab.c"
    break;

  case 373: /* expr: expr '-' expr  */
#line 1974 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("-", yyvsp[-2], yyvsp[0]); }
#line 4203 "ned.tab.c"
    break;

  case 374: /* expr: expr '*' expr  */
#line 1976 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("*", yyvsp[-2], yyvsp[0]); }
#line 4209 "ned.tab.c"
    break;

  case 375: /* expr: expr '/' expr  */
#line 1978 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("/", yyvsp[-2], yyvsp[0]); }
#line 4215 "ned.tab.c"
    break;

  case 376: /* expr: expr '%' expr  */
#line 1980 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("%", yyvsp[-2], yyvsp[0]); }
#line 4221 "ned.tab.c"
    break;

  case 377: /* expr: expr '^' expr  */
#line 1982 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("^", yyvsp[-2], yyvsp[0]); }
#line 4227 "ned.tab.c"
    break;

  case 378: /* expr: '-' expr  */
#line 1986 "ned.y"
                { if (ps.parseExpressions) yyval = unaryMinus(yyvsp[0]); }
#line 4233 "ned.tab.c"
    break;

  case 379: /* expr: expr EQ expr  */
#line 1989 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("==", yyvsp[-2], yyvsp[0]); }
#line 4239 "ned.tab.c"
    break;

  case 380: /* expr: expr NE expr  */
#line 1991 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("!=", yyvsp[-2], yyvsp[0]); }
#line 4245 "ned.tab.c"
    break;

  case 381: /* expr: expr GT expr  */
#line 1993 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator(">", yyvsp[-2], yyvsp[0]); }
#line 4251 "ned.tab.c"
    break;

  case 382: /* expr: expr GE expr  */
#line 1995 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator(">=", yyvsp[-2], yyvsp[0]); }
#line 4257 "ned.tab.c"
    break;

  case 383: /* expr: expr LS expr  */
#line 1997 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("<", yyvsp[-2], yyvsp[0]); }
#line 4263 "ned.tab.c"
    break;

  case 384: /* expr: expr LE expr  */
#line 1999 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("<=", yyvsp[-2], yyvsp[0]); }
#line 4269 "ned.tab.c"
    break;

  case 385: /* expr: expr AND expr  */
#line 2002 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("&&", yyvsp[-2], yyvsp[0]); }
#line 4275 "ned.tab.c"
    break;

  case 386: /* expr: expr OR expr  */
#line 2004 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("||", yyvsp[-2], yyvsp[0]); }
#line 4281 "ned.tab.c"
    break;

  case 387: /* expr: expr XOR expr  */
#line 2006 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("##", yyvsp[-2], yyvsp[0]); }
#line 4287 "ned.tab.c"
    break;

  case 388: /* expr: NOT expr  */
#line 2010 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("!", yyvsp[0]); }
#line 4293 "ned.tab.c"
    break;

  case 389: /* expr: expr BIN_AND expr  */
#line 2013 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("&", yyvsp[-2], yyvsp[0]); }
#line 4299 "ned.tab.c"
    break;

  case 390: /* expr: expr BIN_OR expr  */
#line 2015 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("|", yyvsp[-2], yyvsp[0]); }
#line 4305 "ned.tab.c"
    break;

  case 391: /* expr: expr BIN_XOR expr  */
#line 2017 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("#", yyvsp[-2], yyvsp[0]); }
#line 4311 "ned.tab.c"
    break;

  case 392: /* expr: BIN_COMPL expr  */
#line 2021 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("~", yyvsp[0]); }
#line 4317 "ned.tab.c"
    break;

  case 393: /* expr: expr SHIFT_LEFT expr  */
#line 2023 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("<<", yyvsp[-2], yyvsp[0]); }
#line 4323 "ned.tab.c"
    break;

  case 394: /* expr: expr SHIFT_RIGHT expr  */
#line 2025 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator(">>", yyvsp[-2], yyvsp[0]); }
#line 4329 "ned.tab.c"
    break;

  case 395: /* expr: expr '?' expr ':' expr  */
#line 2027 "ned.y"
                { if (ps.parseExpressions) yyval = createOperator("?:", yyvsp[-4], yyvsp[-2], yyvsp[0]); }
#line 4335 "ned.tab.c"
    break;

  case 396: /* expr: NAME '(' ')'  */
#line 2030 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction(toString((yylsp[-2]))); }
#line 4341 "ned.tab.c"
    break;

  case 397: /* expr: NAME '(' expr ')'  */
#line 2032 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction(toString((yylsp[-3])), yyvsp[-1]); }
#line 4347 "ned.tab.c"
    break;

  case 398: /* expr: NAME '(' expr ',' expr ')'  */
#line 2034 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction(toString((yylsp[-5])), yyvsp[-3], yyvsp[-1]); }
#line 4353 "ned.tab.c"
    break;

  case 399: /* expr: NAME '(' expr ',' expr ',' expr ')'  */
#line 2036 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction(toString((yylsp[-7])), yyvsp[-5], yyvsp[-3], yyvsp[-1]); }
#line 4359 "ned.tab.c"
    break;

  case 400: /* expr: NAME '(' expr ',' expr ',' expr ',' expr ')'  */
#line 2038 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction(toString((yylsp[-9])), yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); }
#line 4365 "ned.tab.c"
    break;

  case 406: /* parameter_expr: NAME  */
#line 2051 "ned.y"
                {
                  // if there's no modifier, might be a loop variable too
                  yyval = createParamRefOrIdent(toString((yylsp[0])));
                }
#line 4374 "ned.tab.c"
    break;

  case 407: /* parameter_expr: REF NAME  */
#line 2056 "ned.y"
                {
                  yyval = createParamRef(toString((yylsp[0])));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                }
#line 4383 "ned.tab.c"
    break;

  case 408: /* parameter_expr: REF ANCESTOR NAME  */
#line 2061 "ned.y"
                {
                  yyval = createParamRef(toString((yylsp[0])));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                }
#line 4393 "ned.tab.c"
    break;

  case 409: /* parameter_expr: ANCESTOR REF NAME  */
#line 2067 "ned.y"
                {
                  yyval = createParamRef(toString((yylsp[0])));
                  ((ParamRefNode *)yyval)->setIsRef(true);
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                }
#line 4403 "ned.tab.c"
    break;

  case 410: /* parameter_expr: ANCESTOR NAME  */
#line 2073 "ned.y"
                {
                  yyval = createParamRef(toString((yylsp[0])));
                  ((ParamRefNode *)yyval)->setIsAncestor(true);
                }
#line 4412 "ned.tab.c"
    break;

  case 412: /* boolconst_expr: TRUE_  */
#line 2085 "ned.y"
                { yyval = createConst(NED_CONST_BOOL, "true"); }
#line 4418 "ned.tab.c"
    break;

  case 413: /* boolconst_expr: FALSE_  */
#line 2087 "ned.y"
                { yyval = createConst(NED_CONST_BOOL, "false"); }
#line 4424 "ned.tab.c"
    break;

  case 415: /* special_expr: SUBMODINDEX  */
#line 2096 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction("index"); }
#line 4430 "ned.tab.c"
    break;

  case 416: /* special_expr: SUBMODINDEX '(' ')'  */
#line 2098 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction("index"); }
#line 4436 "ned.tab.c"
    break;

  case 417: /* special_expr: SIZEOF '(' NAME ')'  */
#line 2100 "ned.y"
                { if (ps.parseExpressions) yyval = createFunction("sizeof", createIdent(toString((yylsp[-1])))); }
#line 4442 "ned.tab.c"
    break;

  case 418: /* stringconstant: STRINGCONSTANT  */
#line 2105 "ned.y"
                { yyval = createConst(NED_CONST_STRING, toString(trimQuotes((yylsp[0])))); }
#line 4448 "ned.tab.c"
    break;

  case 419: /* numconst: INTCONSTANT  */
#line 2110 "ned.y"
                { yyval = createConst(NED_CONST_INT, toString((yylsp[0]))); }
#line 4454 "ned.tab.c"
    break;

  case 420: /* numconst: REALCONSTANT  */
#line 2112 "ned.y"
                { yyval = createConst(NED_CONST_REAL, toString((yylsp[0]))); }
#line 4460 "ned.tab.c"
    break;

  case 421: /* numconst: timeconstant  */
#line 2114 "ned.y"
                { yyval = createTimeConst(toString((yylsp[0]))); }
#line 4466 "ned.tab.c"
    break;

  case 426: /* cplusplus: CPLUSPLUS CPLUSPLUSBODY opt_semicolon  */
#line 2131 "ned.y"
                {
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces((yylsp[-1]))));
                  setComments(ps.cplusplus,(yylsp[-2]),(yylsp[-1]));
                }
#line 4476 "ned.tab.c"
    break;

  case 427: /* struct_decl: STRUCT NAME ';'  */
#line 2140 "ned.y"
                {
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString((yylsp[-1])));
                  setComments(ps.structdecl,(yylsp[-2]),(yylsp[-1]));
                }
#line 4486 "ned.tab.c"
    break;

  case 428: /* class_decl: CLASS NAME ';'  */
#line 2149 "ned.y"
                {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString((yylsp[-1])));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,(yylsp[-2]),(yylsp[-1]));
                }
#line 4497 "ned.tab.c"
    break;

  case 429: /* class_decl: CLASS NONCOBJECT NAME ';'  */
#line 2156 "ned.y"
                {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString((yylsp[-1])));
                  setComments(ps.classdecl,(yylsp[-3]),(yylsp[-2]));
                }
#line 4508 "ned.tab.c"
    break;

  case 430: /* message_decl: MESSAGE NAME ';'  */
#line 2166 "ned.y"
                {
                  ps.messagedecl = (MessageDeclNode *)createNodeWithTag(NED_MESSAGE_DECL, ps.nedfile );
                  ps.messagedecl->setName(toString((yylsp[-1])));
                  setComments(ps.messagedecl,(yylsp[-2]),(yylsp[-1]));
                }
#line 4518 "ned.tab.c"
    break;

  case 431: /* enum_decl: ENUM NAME ';'  */
#line 2175 "ned.y"
                {
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString((yylsp[-1])));
                  setComments(ps.enumdecl,(yylsp[-2]),(yylsp[-1]));
                }
#line 4528 "ned.tab.c"
    break;

  case 432: /* $@36: %empty  */
#line 2184 "ned.y"
                {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString((yylsp[-1])));
                  setComments(ps.enump,(yylsp[-2]),(yylsp[-1]));
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
#line 4539 "ned.tab.c"
    break;

  case 433: /* enum: ENUM NAME '{' $@36 opt_enumfields '}' opt_semicolon  */
#line 2191 "ned.y"
                {
                  setTrailingComment(ps.enump,(yylsp[-1]));
                }
#line 4547 "ned.tab.c"
    break;

  case 434: /* $@37: %empty  */
#line 2195 "ned.y"
                {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString((yylsp[-3])));
                  ps.enump->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.enump,(yylsp[-4]),(yylsp[-1]));
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
#line 4559 "ned.tab.c"
    break;

  case 435: /* enum: ENUM NAME EXTENDS NAME '{' $@37 opt_enumfields '}' opt_semicolon  */
#line 2203 "ned.y"
                {
                  setTrailingComment(ps.enump,(yylsp[-1]));
                }
#line 4567 "ned.tab.c"
    break;

  case 440: /* enumfield: NAME ';'  */
#line 2220 "ned.y"
                {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString((yylsp[-1])));
                  setComments(ps.enumfield,(yylsp[-1]),(yylsp[-1]));
                }
#line 4577 "ned.tab.c"
    break;

  case 441: /* enumfield: NAME '=' enumvalue ';'  */
#line 2226 "ned.y"
                {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString((yylsp[-3])));
                  ps.enumfield->setValue(toString((yylsp[-1])));
                  setComments(ps.enumfield,(yylsp[-3]),(yylsp[-1]));
                }
#line 4588 "ned.tab.c"
    break;

  case 442: /* $@38: %empty  */
#line 2236 "ned.y"
                {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString((yylsp[-1])));
                  setComments(ps.messagep,(yylsp[-2]),(yylsp[-1]));
                }
#line 4598 "ned.tab.c"
    break;

  case 443: /* message: MESSAGE NAME '{' $@38 opt_propertiesblock opt_fieldsblock '}' opt_semicolon  */
#line 2242 "ned.y"
                {
                  setTrailingComment(ps.messagep,(yylsp[-1]));
                }
#line 4606 "ned.tab.c"
    break;

  case 444: /* $@39: %empty  */
#line 2246 "ned.y"
                {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString((yylsp[-3])));
                  ps.messagep->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.messagep,(yylsp[-4]),(yylsp[-1]));
                }
#line 4617 "ned.tab.c"
    break;

  case 445: /* message: MESSAGE NAME EXTENDS NAME '{' $@39 opt_propertiesblock opt_fieldsblock '}' opt_semicolon  */
#line 2253 "ned.y"
                {
                  setTrailingComment(ps.messagep,(yylsp[-1]));
                }
#line 4625 "ned.tab.c"
    break;

  case 446: /* $@40: %empty  */
#line 2260 "ned.y"
                {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString((yylsp[-1])));
                  setComments(ps.classp,(yylsp[-2]),(yylsp[-1]));
                }
#line 4635 "ned.tab.c"
    break;

  case 447: /* class: CLASS NAME '{' $@40 opt_propertiesblock opt_fieldsblock '}' opt_semicolon  */
#line 2266 "ned.y"
                {
                  setTrailingComment(ps.classp,(yylsp[-1]));
                }
#line 4643 "ned.tab.c"
    break;

  case 448: /* $@41: %empty  */
#line 2270 "ned.y"
                {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString((yylsp[-3])));
                  ps.classp->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.classp,(yylsp[-4]),(yylsp[-1]));
                }
#line 4654 "ned.tab.c"
    break;

  case 449: /* class: CLASS NAME EXTENDS NAME '{' $@41 opt_propertiesblock opt_fieldsblock '}' opt_semicolon  */
#line 2277 "ned.y"
                {
                  setTrailingComment(ps.classp,(yylsp[-1]));
                }
#line 4662 "ned.tab.c"
    break;

  case 450: /* $@42: %empty  */
#line 2284 "ned.y"
                {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString((yylsp[-1])));
                  setComments(ps.structp,(yylsp[-2]),(yylsp[-1]));
                }
#line 4672 "ned.tab.c"
    break;

  case 451: /* struct: STRUCT NAME '{' $@42 opt_propertiesblock opt_fieldsblock '}' opt_semicolon  */
#line 2290 "ned.y"
                {
                  setTrailingComment(ps.structp,(yylsp[-1]));
                }
#line 4680 "ned.tab.c"
    break;

  case 452: /* $@43: %empty  */
#line 2294 "ned.y"
                {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString((yylsp[-3])));
                  ps.structp->setExtendsName(toString((yylsp[-1])));
                  setComments(ps.structp,(yylsp[-4]),(yylsp[-1]));
                }
#line 4691 "ned.tab.c"
    break;

  case 453: /* struct: STRUCT NAME EXTENDS NAME '{' $@43 opt_propertiesblock opt_fieldsblock '}' opt_semicolon  */
#line 2301 "ned.y"
                {
                  setTrailingComment(ps.structp,(yylsp[-1]));
                }
#line 4699 "ned.tab.c"
    break;

  case 454: /* $@44: %empty  */
#line 2308 "ned.y"
                {
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,(yylsp[-1]));
                }
#line 4708 "ned.tab.c"
    break;

  case 461: /* property: NAME '=' propertyvalue ';'  */
#line 2328 "ned.y"
                {
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString((yylsp[-3])));
                  ps.property->setValue(toString((yylsp[-1])));
                  setComments(ps.property,(yylsp[-3]),(yylsp[-1]));
                }
#line 4719 "ned.tab.c"
    break;

  case 468: /* $@45: %empty  */
#line 2347 "ned.y"
                {
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,(yylsp[-1]));
                }
#line 4728 "ned.tab.c"
    break;

  case 475: /* $@46: %empty  */
#line 2367 "ned.y"
                {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString((yylsp[0])));
                  ps.field->setDataType(toString((yylsp[-1])));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                }
#line 4740 "ned.tab.c"
    break;

  case 476: /* field: fieldmodifiers fielddatatype NAME $@46 opt_fieldvector opt_fieldenum opt_fieldvalue ';'  */
#line 2375 "ned.y"
                {
                  setComments(ps.field,(yylsp[-7]),(yylsp[-1]));
                }
#line 4748 "ned.tab.c"
    break;

  case 477: /* $@47: %empty  */
#line 2379 "ned.y"
                {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString((yylsp[0])));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                }
#line 4759 "ned.tab.c"
    break;

  case 478: /* field: fieldmodifiers NAME $@47 opt_fieldvector opt_fieldenum opt_fieldvalue ';'  */
#line 2386 "ned.y"
                {
                  setComments(ps.field,(yylsp[-6]),(yylsp[-1]));
                }
#line 4767 "ned.tab.c"
    break;

  case 479: /* fieldmodifiers: ABSTRACT  */
#line 2393 "ned.y"
                { ps.isAbstract = true; ps.isReadonly = false; }
#line 4773 "ned.tab.c"
    break;

  case 480: /* fieldmodifiers: READONLY  */
#line 2395 "ned.y"
                { ps.isAbstract = false; ps.isReadonly = true; }
#line 4779 "ned.tab.c"
    break;

  case 481: /* fieldmodifiers: ABSTRACT READONLY  */
#line 2397 "ned.y"
                { ps.isAbstract = true; ps.isReadonly = true; }
#line 4785 "ned.tab.c"
    break;

  case 482: /* fieldmodifiers: READONLY ABSTRACT  */
#line 2399 "ned.y"
                { ps.isAbstract = true; ps.isReadonly = true; }
#line 4791 "ned.tab.c"
    break;

  case 483: /* fieldmodifiers: %empty  */
#line 2401 "ned.y"
                { ps.isAbstract = false; ps.isReadonly = false; }
#line 4797 "ned.tab.c"
    break;

  case 497: /* opt_fieldvector: '[' INTCONSTANT ']'  */
#line 2426 "ned.y"
                {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString((yylsp[-1])));
                }
#line 4806 "ned.tab.c"
    break;

  case 498: /* opt_fieldvector: '[' NAME ']'  */
#line 2431 "ned.y"
                {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString((yylsp[-1])));
                }
#line 4815 "ned.tab.c"
    break;

  case 499: /* opt_fieldvector: '[' ']'  */
#line 2436 "ned.y"
                {
                  ps.field->setIsVector(true);
                }
#line 4823 "ned.tab.c"
    break;

  case 501: /* opt_fieldenum: ENUM '(' NAME ')'  */
#line 2444 "ned.y"
                {
                  ps.field->setEnumName(toString((yylsp[-1])));
                }
#line 4831 "ned.tab.c"
    break;

  case 503: /* opt_fieldvalue: '=' fieldvalue  */
#line 2452 "ned.y"
                {
                  ps.field->setDefaultValue(toString((yylsp[0])));
                }
#line 4839 "ned.tab.c"
    break;


#line 4843 "ned.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 2505 "ned.y"


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
