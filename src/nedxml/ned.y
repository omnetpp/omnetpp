/*===============================================================
 * File: ned.y
 *
 *  Grammar for OMNeT++ NED.
 *
 *  Author: Andras Varga
 *
 *  Based on code from nedc.
 *
 *  nedc credits:
 *     original code:
 *       Jan Heijmans, Alex Paalvast, Robert van der Leij, 1996
 *       (nedc was originally named jar, for Jan+Alex+Robert)
 *     modifications
 *       Gabor Lencse 1998
 *     restructuring, maintenance, new features, etc:
 *       Andras Varga 1996-2001
 *
 *=============================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 1992,2002 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


%token INCLUDE SIMPLE
%token CHANNEL /*DELAY ERROR DATARATE are no longer tokens*/
%token MODULE PARAMETERS GATES GATESIZES SUBMODULES CONNECTIONS DISPLAY
%token IN OUT
%token NOCHECK LEFT_ARROW RIGHT_ARROW
%token FOR TO DO IF LIKE
%token NETWORK
%token ENDSIMPLE ENDMODULE ENDCHANNEL
%token ENDNETWORK ENDFOR
%token MACHINES ON
%token CHANATTRNAME   /* only needed to parse old-syntax NED */

%token INTCONSTANT REALCONSTANT NAME STRINGCONSTANT CHARCONSTANT
%token TRUE_ FALSE_
%token INPUT_ XMLDOC
%token REF ANCESTOR
%token CONSTDECL NUMERICTYPE STRINGTYPE BOOLTYPE XMLTYPE ANYTYPE

%token CPLUSPLUS CPLUSPLUSBODY
%token MESSAGE CLASS STRUCT ENUM NONCOBJECT
%token EXTENDS FIELDS PROPERTIES ABSTRACT READONLY
%token CHARTYPE SHORTTYPE INTTYPE LONGTYPE DOUBLETYPE UNSIGNED_

%token SIZEOF SUBMODINDEX PLUSPLUS
%token EQ NE GT GE LS LE
%token AND OR XOR NOT
%token BIN_AND BIN_OR BIN_XOR BIN_COMPL
%token SHIFT_LEFT SHIFT_RIGHT

%token INVALID_CHAR   /* just to generate parse error --VA */

/* Operator precedences (low to high) and associativity */
%left '?' ':'
%left AND OR XOR
%left EQ NE GT GE LS LE
%left BIN_AND BIN_OR BIN_XOR
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-'
%left '*' '/' '%'
%right '^'
%left UMIN NOT BIN_COMPL

%start networkdescription


%{

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

%}

%%

/*
 * Top-level components (no shift-reduce conflict here)
 */
networkdescription
        : somedefinitions
        ;

somedefinitions
        : somedefinitions definition
        |
        ;

definition
        : import

        | channeldefinition
                { if (ps.storeSourceCode) ps.channel->setSourceCode(toString(@1)); }
        | simpledefinition
                { if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(@1)); }
        | moduledefinition
                { if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(@1)); }
        | networkdefinition
                { if (ps.storeSourceCode) ps.network->setSourceCode(toString(@1)); }

        | channeldefinition_old
                { if (ps.storeSourceCode) ps.channel->setSourceCode(toString(@1)); }
        | simpledefinition_old
                { if (ps.storeSourceCode) ((SimpleModuleNode *)ps.module)->setSourceCode(toString(@1)); }
        | moduledefinition_old
                { if (ps.storeSourceCode) ((CompoundModuleNode *)ps.module)->setSourceCode(toString(@1)); }
        | networkdefinition_old
                { if (ps.storeSourceCode) ps.network->setSourceCode(toString(@1)); }

        | cplusplus
        | struct_decl
        | class_decl
        | message_decl
        | enum_decl

        | enum
                { if (ps.storeSourceCode) ps.enump->setSourceCode(toString(@1)); }
        | message
                { if (ps.storeSourceCode) ps.messagep->setSourceCode(toString(@1)); }
        | class
                { if (ps.storeSourceCode) ps.classp->setSourceCode(toString(@1)); }
        | struct
                { if (ps.storeSourceCode) ps.structp->setSourceCode(toString(@1)); }
        ;

/*
 * Imports (no shift-reduce conflict here)
 */
import
        : INCLUDE
                {
                  ps.imports = (ImportNode *)createNodeWithTag(NED_IMPORT, ps.nedfile );
                  setComments(ps.imports,@1);
                }
          filenames ';'
                {
                  // NOTE: no setTrailingComment(ps.imports,@3) -- comment already stored with last filename */
                }
        ;

filenames
        : filenames ',' filename
        | filename
        ;

filename
        : STRINGCONSTANT
                {
                  ps.import = (ImportedFileNode *)createNodeWithTag(NED_IMPORTED_FILE, ps.imports );
                  ps.import->setFilename(toString(trimQuotes(@1)));
                  setComments(ps.import,@1);
                }
        ;

/*
 * Channel - old syntax
 */
channeldefinition_old
        : channelheader_old opt_channelattrblock_old endchannel_old
        ;

channelheader_old
        : CHANNEL NAME
                {
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(@2));
                  setComments(ps.channel,@1,@2);
                }
        ;

opt_channelattrblock_old
        :
        | channelattrblock_old
        ;

channelattrblock_old
        : channelattrblock_old CHANATTRNAME expression opt_semicolon
                {
                  ps.chanattr = addChanAttr(ps.channel,toString(@2));
                  addExpression(ps.chanattr, "value",@3,$3);
                  setComments(ps.channel,@2,@3);
                }
        | CHANATTRNAME expression opt_semicolon
                {
                  ps.chanattr = addChanAttr(ps.channel,toString(@1));
                  addExpression(ps.chanattr, "value",@2,$2);
                  setComments(ps.channel,@1,@2);
                }
        ;

endchannel_old
        : ENDCHANNEL NAME opt_semicolon
                {
                  setTrailingComment(ps.channel,@2);
                }
        | ENDCHANNEL opt_semicolon
                {
                  setTrailingComment(ps.channel,@1);
                }
        ;

/*
 * Channel - new syntax
 */
channeldefinition
        : channelheader '{'
            opt_channelattrblock
          '}' opt_semicolon
                {
                  setTrailingComment(ps.channel,@4);
                }
        ;

channelheader
        : CHANNEL NAME
                {
                  use_chanattrname_token = 0;
                  ps.channel = (ChannelNode *)createNodeWithTag(NED_CHANNEL, ps.nedfile );
                  ps.channel->setName(toString(@2));
                  setComments(ps.channel,@1,@2);
                }
        ;

opt_channelattrblock
        : channelattrblock
        |
        ;

channelattrblock
        : channelattrblock NAME '=' expression ';'
                {
                  ps.chanattr = addChanAttr(ps.channel,toString(@2));
                  addExpression(ps.chanattr, "value",@4,$4);
                  setComments(ps.channel,@2,@4);
                }
        | NAME '=' expression ';'
                {
                  ps.chanattr = addChanAttr(ps.channel,toString(@1));
                  addExpression(ps.chanattr, "value",@3,$3);
                  setComments(ps.channel,@1,@3);
                }
        ;

/*
 * Simple module - old syntax
 */
simpledefinition_old
        : simpleheader_old
            opt_machineblock_old
            opt_paramblock_old
            opt_gateblock_old
          endsimple_old
        ;

simpleheader_old
        : SIMPLE NAME
                {
                  use_chanattrname_token = 1;
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(@2));
                  setComments(ps.module,@1,@2);
                }
        ;

endsimple_old
        : ENDSIMPLE NAME opt_semicolon
                {
                  setTrailingComment(ps.module,@2);
                }
        | ENDSIMPLE opt_semicolon
                {
                  setTrailingComment(ps.module,@1);
                }
        ;

/*
 * Simple module - new syntax
 */
simpledefinition
        : simpleheader '{'
            opt_machineblock
            opt_paramblock
            opt_gateblock
          '}' opt_semicolon
                {
                  setTrailingComment(ps.module,@6);
                }
        ;

simpleheader
        : SIMPLE NAME
                {
                  ps.module = (SimpleModuleNode *)createNodeWithTag(NED_SIMPLE_MODULE, ps.nedfile );
                  ((SimpleModuleNode *)ps.module)->setName(toString(@2));
                  setComments(ps.module,@1,@2);
                }
        ;


/*
 * Module - old syntax
 */
moduledefinition_old
        : moduleheader_old
            opt_machineblock_old
            opt_paramblock_old
            opt_gateblock_old
            opt_submodblock_old
            opt_connblock_old
            opt_displayblock_old
          endmodule_old
        ;

moduleheader_old
        : MODULE NAME
                {
                  use_chanattrname_token = 1;
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(@2));
                  setComments(ps.module,@1,@2);
                }
        ;

endmodule_old
        : ENDMODULE NAME opt_semicolon
                {
                  setTrailingComment(ps.module,@2);
                }
        | ENDMODULE opt_semicolon
                {
                  setTrailingComment(ps.module,@1);
                }
        ;

/*
 * Module - new syntax
 */
moduledefinition
        : moduleheader '{'
            opt_machineblock
            opt_paramblock
            opt_gateblock
            opt_submodblock
            opt_connblock
            opt_displayblock
          '}' opt_semicolon
                {
                  setTrailingComment(ps.module,@9);
                }
        ;

moduleheader
        : MODULE NAME
                {
                  ps.module = (CompoundModuleNode *)createNodeWithTag(NED_COMPOUND_MODULE, ps.nedfile );
                  ((CompoundModuleNode *)ps.module)->setName(toString(@2));
                  setComments(ps.module,@1,@2);
                }
        ;


/*
 * Machine block - old syntax
 */
opt_machineblock_old
        : machineblock_old
        |
        ;

machineblock_old
        : MACHINES ':'
                {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,@1,@2);
                }
          opt_machinelist_old
                {
                }
        ;

opt_machinelist_old
        : machinelist_old ';'
        |
        ;

machinelist_old
        : machinelist_old ',' machine_old
        | machine_old
        ;

machine_old
        : NAME
                {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(@1));
                  setComments(ps.machine,@1,@1);
                }
        ;

/*
 * Machine block - new syntax
 */
opt_machineblock
        : machineblock
        |
        ;

machineblock
        : MACHINES ':'
                {
                  ps.machines = (MachinesNode *)createNodeWithTag(NED_MACHINES, ps.module );
                  setComments(ps.machines,@1,@2);
                }
          opt_machinelist
                {
                }
        ;

opt_machinelist
        : machinelist ';'
        |
        ;

machinelist
        : machinelist ',' machine
        | machine
        ;

machine
        : NAME
                {
                  ps.machine = (MachineNode *)createNodeWithTag(NED_MACHINE, ps.machines );
                  ps.machine->setName(toString(@1));
                  setComments(ps.machine,@1,@1);
                }
        ;

/*
 * Display block - old syntax
 */
opt_displayblock_old
        : displayblock_old
        |
        ;

displayblock_old
        : DISPLAY ':' STRINGCONSTANT ';'
                {
                  addDisplayString(ps.module,@3);
                }
        ;

/*
 * Display block - new syntax
 */
opt_displayblock
        : displayblock
        |
        ;

displayblock
        : DISPLAY ':' STRINGCONSTANT ';'
                {
                  addDisplayString(ps.module,@3);
                }
        ;

/*
 * Parameters - old syntax
 */
opt_paramblock_old
        : paramblock_old
        |
        ;

paramblock_old
        : PARAMETERS ':'
                {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,@1,@2);
                }
          opt_parameters_old
                {
                }
        ;

opt_parameters_old
        : parameters_old ';'
        |
        ;

parameters_old
        : parameters_old ',' parameter_old  /* comma as separator */
                {
                  setComments(ps.param,@3);
                }
        | parameter_old
                {
                  setComments(ps.param,@1);
                }
        ;

/*
 * Parameters - new syntax
 */
opt_paramblock
        : paramblock
        |
        ;

paramblock
        : PARAMETERS ':'
                {
                  ps.params = (ParamsNode *)createNodeWithTag(NED_PARAMS, ps.module );
                  setComments(ps.params,@1,@2);
                }
          opt_parameters
                {
                }
        ;

opt_parameters
        : parameters ';'
        |
        ;

parameters
        : parameters ';' parameter   /* semicolon as separator */
                {
                  setComments(ps.param,@3);
                }
        | parameter
                {
                  setComments(ps.param,@1);
                }
        ;


/*
 * Parameter - identical old and new formats
 */
parameter_old
        : parameter
        ;

parameter
        : NAME
                {
                  ps.param = addParameter(ps.params,@1,TYPE_NUMERIC);
                }
        | NAME ':' NUMERICTYPE
                {
                  ps.param = addParameter(ps.params,@1,TYPE_NUMERIC);
                }
        | CONSTDECL NAME /* for compatibility */
                {
                  ps.param = addParameter(ps.params,@2,TYPE_CONST_NUM);
                }
        | NAME ':' CONSTDECL
                {
                  ps.param = addParameter(ps.params,@1,TYPE_CONST_NUM);
                }
        | NAME ':' CONSTDECL NUMERICTYPE
                {
                  ps.param = addParameter(ps.params,@1,TYPE_CONST_NUM);
                }
        | NAME ':' NUMERICTYPE CONSTDECL
                {
                  ps.param = addParameter(ps.params,@1,TYPE_CONST_NUM);
                }
        | NAME ':' STRINGTYPE
                {
                  ps.param = addParameter(ps.params,@1,TYPE_STRING);
                }
        | NAME ':' BOOLTYPE
                {
                  ps.param = addParameter(ps.params,@1,TYPE_BOOL);
                }
        | NAME ':' XMLTYPE
                {
                  ps.param = addParameter(ps.params,@1,TYPE_XML);
                }
        | NAME ':' ANYTYPE
                {
                  ps.param = addParameter(ps.params,@1,TYPE_ANYTYPE);
                }
        ;

/*
 * Gates - old syntax
 */
opt_gateblock_old
        : gateblock_old
        |
        ;

gateblock_old
        : GATES ':'
                {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,@1,@2);
                }
          opt_gates_old
                {
                }
        ;

opt_gates_old
        : gates_old
        |
        ;

gates_old
        : gates_old IN gatesI_old ';'
        | IN  gatesI_old ';'
        | gates_old OUT gatesO_old ';'
        | OUT gatesO_old ';'
        ;

gatesI_old
        : gatesI_old ',' gateI_old
        | gateI_old
        ;

gateI_old
        : NAME '[' ']'
                {
                  ps.gate = addGate(ps.gates, @1, 1, 1 );
                  setComments(ps.gate,@1,@3);
                }
        | NAME
                {
                  ps.gate = addGate(ps.gates, @1, 1, 0 );
                  setComments(ps.gate,@1);
                }
        ;

gatesO_old
        : gatesO_old ',' gateO_old
        | gateO_old
        ;

gateO_old
        : NAME '[' ']'
                {
                  ps.gate = addGate(ps.gates, @1, 0, 1 );
                  setComments(ps.gate,@1,@3);
                }
        | NAME
                {
                  ps.gate = addGate(ps.gates, @1, 0, 0 );
                  setComments(ps.gate,@1);
                }
        ;

/*
 * Gates - new syntax
 */
opt_gateblock
        : gateblock
        |
        ;

gateblock
        : GATES ':'
                {
                  ps.gates = (GatesNode *)createNodeWithTag(NED_GATES, ps.module );
                  setComments(ps.gates,@1,@2);
                }
          opt_gates
                {
                }
        ;

opt_gates
        : gates
        |
        ;

gates
        : gates IN gatesI ';'
        | IN  gatesI ';'
        | gates OUT gatesO ';'
        | OUT gatesO ';'
        ;

gatesI
        : gatesI ',' gateI
        | gateI
        ;

gateI
        : NAME '[' ']'
                {
                  ps.gate = addGate(ps.gates, @1, 1, 1 );
                  setComments(ps.gate,@1,@3);
                }
        | NAME
                {
                  ps.gate = addGate(ps.gates, @1, 1, 0 );
                  setComments(ps.gate,@1);
                }
        ;

gatesO
        : gatesO ',' gateO
        | gateO
        ;

gateO
        : NAME '[' ']'
                {
                  ps.gate = addGate(ps.gates, @1, 0, 1 );
                  setComments(ps.gate,@1,@3);
                }
        | NAME
                {
                  ps.gate = addGate(ps.gates, @1, 0, 0 );
                  setComments(ps.gate,@1);
                }
        ;

/*
 * Submodules - old syntax
 */
opt_submodblock_old
        : submodblock_old
        |
        ;

submodblock_old
        : SUBMODULES ':'
                {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,@1,@2);
                }
          opt_submodules_old
                {
                }
        ;

opt_submodules_old
        : submodules_old
        |
        ;

submodules_old
        : submodules_old submodule_old
        | submodule_old
        ;

submodule_old
        : NAME ':' NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, NULLPOS);
                  setComments(ps.submod,@1,@4);
                }
          opt_on_blocks_old
          submodule_body_old
                {
                }
        | NAME ':' NAME vector opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, NULLPOS);
                  addVector(ps.submod, "vector-size",@4,$4);
                  setComments(ps.submod,@1,@5);
                }
          opt_on_blocks_old
          submodule_body_old
                {
                }
        | NAME ':' NAME LIKE NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @5, @3);
                  setComments(ps.submod,@1,@6);
                }
          opt_on_blocks_old
          submodule_body_old
                {
                }
        | NAME ':' NAME vector LIKE NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @6, @3);
                  addVector(ps.submod, "vector-size",@4,$4);
                  setComments(ps.submod,@1,@7);
                }
          opt_on_blocks_old
          submodule_body_old
                {
                }
        ;

submodule_body_old
        : opt_substparamblocks_old
          opt_gatesizeblocks_old
          opt_submod_displayblock_old
        ;

/*
 * Submodules - new syntax
 */
opt_submodblock
        : submodblock
        |
        ;

submodblock
        : SUBMODULES ':'
                {
                  ps.submods = (SubmodulesNode *)createNodeWithTag(NED_SUBMODULES, ps.module );
                  setComments(ps.submods,@1,@2);
                }
          opt_submodules
                {
                }
        ;

opt_submodules
        : submodules
        |
        ;

submodules
        : submodules submodule
        | submodule
        ;

submodule
        : NAME ':' NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, NULLPOS);
                  setComments(ps.submod,@1,@4);
                }
          opt_on_blocks
          submodule_body
                {
                }
        | NAME ':' NAME vector opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @3, NULLPOS);
                  addVector(ps.submod, "vector-size",@4,$4);
                  setComments(ps.submod,@1,@5);
                }
          opt_on_blocks
          submodule_body
                {
                }
        | NAME ':' NAME LIKE NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @5, @3);
                  setComments(ps.submod,@1,@6);
                }
          opt_on_blocks
          submodule_body
                {
                }
        | NAME ':' NAME vector LIKE NAME opt_semicolon
                {
                  ps.submod = addSubmodule(ps.submods, @1, @6, @3);
                  addVector(ps.submod, "vector-size",@4,$4);
                  setComments(ps.submod,@1,@7);
                }
          opt_on_blocks
          submodule_body
                {
                }
        ;

submodule_body
        : opt_substparamblocks
          opt_gatesizeblocks
          opt_submod_displayblock
        ;

/*
 * On-block - old syntax
 */
opt_on_blocks_old
        : on_blocks_old
        |
        ;

on_blocks_old
        : on_blocks_old on_block_old
        | on_block_old
        ;

on_block_old
        : ON ':'
                {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,@1,@2);
                }
          opt_on_list_old
                {
                }
        | ON IF expression ':'
                {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",@3,$3);
                  setComments(ps.substmachines,@1,@4);
                }
          opt_on_list_old
                {
                }
        ;

opt_on_list_old
        : on_list_old ';'
        |
        ;

on_list_old
        : on_list_old ',' on_mach_old
        | on_mach_old
        ;

on_mach_old
        : NAME
                {
                  ps.substmachine = addSubstmachine(ps.substmachines,@1);
                  setComments(ps.substmachine,@1);
                }
        ;

/*
 * On-block - new syntax
 */
opt_on_blocks
        : on_blocks
        |
        ;

on_blocks
        : on_blocks on_block
        | on_block
        ;

on_block
        : ON ':'
                {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substmachines,@1,@2);
                }
          opt_on_list
                {
                }
        | ON IF expression ':'
                {
                  ps.substmachines = (SubstmachinesNode *)createNodeWithTag(NED_SUBSTMACHINES, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substmachines, "condition",@3,$3);
                  setComments(ps.substmachines,@1,@4);
                }
          opt_on_list
                {
                }
        ;

opt_on_list
        : on_list ';'
        |
        ;

on_list
        : on_list ',' on_mach
        | on_mach
        ;

on_mach
        : NAME
                {
                  ps.substmachine = addSubstmachine(ps.substmachines,@1);
                  setComments(ps.substmachine,@1);
                }
        ;

/*
 * Substparameters - old syntax
 */
opt_substparamblocks_old
        : substparamblocks_old
        |
        ;

substparamblocks_old
        : substparamblocks_old substparamblock_old
        | substparamblock_old
        ;

substparamblock_old
        : PARAMETERS ':'
                {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,@1,@2);
                }
          opt_substparameters_old
                {
                }
        | PARAMETERS IF expression ':'
                {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",@3,$3);
                  setComments(ps.substparams,@1,@4);
                }
          opt_substparameters_old
                {
                }

        ;

opt_substparameters_old
        : substparameters_old ';'
        |
        ;

substparameters_old
        : substparameters_old ',' substparameter_old   /* comma as separator */
        | substparameter_old
        ;

substparameter_old
        : NAME '=' expression
                {
                  ps.substparam = addSubstparam(ps.substparams,@1);
                  addExpression(ps.substparam, "value",@3,$3);
                  setComments(ps.substparam,@1,@3);
                }
        ;

/*
 * Substparameters - new syntax
 */
opt_substparamblocks
        : substparamblocks
        |
        ;

substparamblocks
        : substparamblocks substparamblock
        | substparamblock
        ;

substparamblock
        : PARAMETERS ':'
                {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  setComments(ps.substparams,@1,@2);
                }
          opt_substparameters
                {
                }
        | PARAMETERS IF expression ':'
                {
                  ps.substparams = (SubstparamsNode *)createNodeWithTag(NED_SUBSTPARAMS, ps.inNetwork ? (NEDElement *)ps.network : (NEDElement *)ps.submod );
                  addExpression(ps.substparams, "condition",@3,$3);
                  setComments(ps.substparams,@1,@4);
                }
          opt_substparameters
                {
                }

        ;

opt_substparameters
        : substparameters
        |
        ;

substparameters
        : substparameters substparameter
        | substparameter
        ;

substparameter
        : NAME '=' expression ';'   /* semicolon as separator */
                {
                  ps.substparam = addSubstparam(ps.substparams,@1);
                  addExpression(ps.substparam, "value",@3,$3);
                  setComments(ps.substparam,@1,@3);
                }
        ;

/*
 * Gatesizes - old syntax
 */
opt_gatesizeblocks_old
        : opt_gatesizeblocks_old gatesizeblock_old
        |
        ;

gatesizeblock_old
        : GATESIZES ':'
                {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,@1,@2);
                }
          opt_gatesizes_old
                {
                }
        | GATESIZES IF expression ':'
                {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",@3,$3);
                  setComments(ps.gatesizes,@1,@4);
                }
          opt_gatesizes_old
                {
                }
        ;

opt_gatesizes_old
        : gatesizes_old ';'
        |
        ;

gatesizes_old
        : gatesizes_old ',' gatesize_old
        | gatesize_old
        ;

gatesize_old
        : NAME vector
                {
                  ps.gatesize = addGateSize(ps.gatesizes,@1);
                  addVector(ps.gatesize, "vector-size",@2,$2);

                  setComments(ps.gatesize,@1,@2);
                }
        | NAME
                {
                  ps.gatesize = addGateSize(ps.gatesizes,@1);
                  setComments(ps.gatesize,@1);
                }
        ;

/*
 * Gatesizes - new syntax
 */
opt_gatesizeblocks
        : opt_gatesizeblocks gatesizeblock
        |
        ;

gatesizeblock
        : GATESIZES ':'
                {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  setComments(ps.gatesizes,@1,@2);
                }
          opt_gatesizes
                {
                }
        | GATESIZES IF expression ':'
                {
                  ps.gatesizes = (GatesizesNode *)createNodeWithTag(NED_GATESIZES, ps.submod );
                  addExpression(ps.gatesizes, "condition",@3,$3);
                  setComments(ps.gatesizes,@1,@4);
                }
          opt_gatesizes
                {
                }
        ;

opt_gatesizes
        : gatesizes ';'
        |
        ;

gatesizes
        : gatesizes ',' gatesize
        | gatesize
        ;

gatesize
        : NAME vector
                {
                  ps.gatesize = addGateSize(ps.gatesizes,@1);
                  addVector(ps.gatesize, "vector-size",@2,$2);

                  setComments(ps.gatesize,@1,@2);
                }
        | NAME
                {
                  ps.gatesize = addGateSize(ps.gatesizes,@1);
                  setComments(ps.gatesize,@1);
                }
        ;

/*
 * Submodule-displayblock - old syntax
 */
opt_submod_displayblock_old
        : DISPLAY ':' STRINGCONSTANT ';'
                {
                  addDisplayString(ps.submod,@3);
                }
        |
        ;

/*
 * Submodule-displayblock - new syntax
 */
opt_submod_displayblock
        : DISPLAY ':' STRINGCONSTANT ';'
                {
                  addDisplayString(ps.submod,@3);
                }
        |
        ;

/*
 * Connections - old syntax  (about 7 shift/reduce)
 */
opt_connblock_old
        : connblock_old
        |
        ;

connblock_old
        : CONNECTIONS NOCHECK ':'
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,@1,@3);
                }
          opt_connections_old
                {
                }
        | CONNECTIONS ':'
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,@1,@2);
                }
          opt_connections_old
                {
                }
        ;

opt_connections_old
        : connections_old
        |
        ;

connections_old
        : connections_old connection_old
        | connection_old
        ;

connection_old
        : loopconnection_old
        | notloopconnection_old
        ;

loopconnection_old
        : FOR
                {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                }
          loopvarlist_old DO notloopconnections_old ENDFOR opt_semicolon
                {
                  ps.inLoop=0;
                  setComments(ps.forloop,@1,@4);
                  setTrailingComment(ps.forloop,@6);
                }
        ;

loopvarlist_old
        : loopvar_old ',' loopvarlist_old
        | loopvar_old
        ;

loopvar_old
        : NAME '=' expression TO expression
                {
                  ps.loopvar = addLoopVar(ps.forloop,@1);
                  addExpression(ps.loopvar, "from-value",@3,$3);
                  addExpression(ps.loopvar, "to-value",@5,$5);
                  setComments(ps.loopvar,@1,@5);
                }
        ;

opt_conncondition_old
        : IF expression
                {
                  addExpression(ps.conn, "condition",@2,$2);
                }
        |
        ;

opt_conn_displaystr_old
        : DISPLAY STRINGCONSTANT
                {
                  addDisplayString(ps.conn,@2);
                }
        |
        ;

notloopconnections_old
        : notloopconnections_old notloopconnection_old
        | notloopconnection_old
        ;

notloopconnection_old
        : leftgatespec_old RIGHT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,@1,@5);
                }
        | leftgatespec_old RIGHT_ARROW channeldescr_old RIGHT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,@1,@7);
                }
        | leftgatespec_old LEFT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,@1,@5);
                }
        | leftgatespec_old LEFT_ARROW channeldescr_old LEFT_ARROW rightgatespec_old opt_conncondition_old opt_conn_displaystr_old comma_or_semicolon
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,@1,@7);
                }
        ;

leftgatespec_old
        : leftmod_old '.' leftgate_old
        | parentleftgate_old
        ;

leftmod_old
        : NAME vector
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                  addVector(ps.conn, "src-module-index",@2,$2);
                }
        | NAME
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                }
        ;

leftgate_old
        : NAME vector
                {
                  ps.conn->setSrcGate( toString( @1) );
                  addVector(ps.conn, "src-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setSrcGate( toString( @1) );
                }
        | NAME PLUSPLUS
                {
                  ps.conn->setSrcGate( toString( @1) );
                  ps.conn->setSrcGatePlusplus(true);
                }
        ;

parentleftgate_old
        : NAME vector
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                  addVector(ps.conn, "src-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                }
        | NAME PLUSPLUS
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                  ps.conn->setSrcGatePlusplus(true);
                }
        ;

rightgatespec_old
        : rightmod_old '.' rightgate_old
        | parentrightgate_old
        ;

rightmod_old
        : NAME vector
                {
                  ps.conn->setDestModule( toString(@1) );
                  addVector(ps.conn, "dest-module-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setDestModule( toString(@1) );
                }
        ;

rightgate_old
        : NAME vector
                {
                  ps.conn->setDestGate( toString( @1) );
                  addVector(ps.conn, "dest-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setDestGate( toString( @1) );
                }
        | NAME PLUSPLUS
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGatePlusplus(true);
                }
        ;

parentrightgate_old
        : NAME vector
                {
                  ps.conn->setDestGate( toString( @1) );
                  addVector(ps.conn, "dest-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setDestGate( toString( @1) );
                }
        | NAME PLUSPLUS
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGatePlusplus(true);
                }
        ;


channeldescr_old
        : NAME
                {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",@1,createExpression(createConst(NED_CONST_STRING, toString(@1))));
                }
        | CHANATTRNAME expression
                {
                  ps.connattr = addConnAttr(ps.conn,toString(@1));
                  addExpression(ps.connattr, "value",@2,$2);
                }
        | channeldescr_old CHANATTRNAME expression
                {
                  ps.connattr = addConnAttr(ps.conn,toString(@2));
                  addExpression(ps.connattr, "value",@3,$3);
                }
        ;

/*
 * Connections - new syntax  (about 7 shift/reduce)
 */
opt_connblock
        : connblock
        |
        ;

connblock
        : CONNECTIONS NOCHECK ':'
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(false);
                  setComments(ps.conns,@1,@3);
                }
          opt_connections
                {
                }
        | CONNECTIONS ':'
                {
                  ps.conns = (ConnectionsNode *)createNodeWithTag(NED_CONNECTIONS, ps.module );
                  ps.conns->setCheckUnconnected(true);
                  setComments(ps.conns,@1,@2);
                }
          opt_connections
                {
                }
        ;

opt_connections
        : connections
        |
        ;

connections
        : connections connection
        | connection
        ;

connection
        : loopconnection
        | notloopconnection
        ;

loopconnection
        : FOR
                {
                  ps.forloop = (ForLoopNode *)createNodeWithTag(NED_FOR_LOOP, ps.conns );
                  ps.inLoop=1;
                }
          loopvarlist DO notloopconnections ENDFOR ';'
                {
                  ps.inLoop=0;
                  setComments(ps.forloop,@1,@4);
                  setTrailingComment(ps.forloop,@6);
                }
        ;

loopvarlist
        : loopvar ',' loopvarlist
        | loopvar
        ;

loopvar
        : NAME '=' expression TO expression
                {
                  ps.loopvar = addLoopVar(ps.forloop,@1);
                  addExpression(ps.loopvar, "from-value",@3,$3);
                  addExpression(ps.loopvar, "to-value",@5,$5);
                  setComments(ps.loopvar,@1,@5);
                }
        ;

opt_conncondition
        : IF expression
                {
                  addExpression(ps.conn, "condition",@2,$2);
                }
        |
        ;

opt_conn_displaystr
        : DISPLAY STRINGCONSTANT
                {
                  addDisplayString(ps.conn,@2);
                }
        |
        ;

notloopconnections
        : notloopconnections notloopconnection
        | notloopconnection
        ;

notloopconnection
        : leftgatespec RIGHT_ARROW rightgatespec opt_conncondition opt_conn_displaystr ';'
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,@1,@5);
                }
        | leftgatespec RIGHT_ARROW channeldescr RIGHT_ARROW rightgatespec opt_conncondition opt_conn_displaystr ';'
                {
                  ps.conn->setArrowDirection(NED_ARROWDIR_RIGHT);
                  setComments(ps.conn,@1,@7);
                }
        | leftgatespec LEFT_ARROW rightgatespec opt_conncondition opt_conn_displaystr ';'
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,@1,@5);
                }
        | leftgatespec LEFT_ARROW channeldescr LEFT_ARROW rightgatespec opt_conncondition opt_conn_displaystr ';'
                {
                  swapConnection(ps.conn);
                  ps.conn->setArrowDirection(NED_ARROWDIR_LEFT);
                  setComments(ps.conn,@1,@7);
                }
        ;

leftgatespec
        : leftmod '.' leftgate
        | parentleftgate
        ;

leftmod
        : NAME vector
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                  addVector(ps.conn, "src-module-index",@2,$2);
                }
        | NAME
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule( toString(@1) );
                }
        ;

leftgate
        : NAME vector
                {
                  ps.conn->setSrcGate( toString( @1) );
                  addVector(ps.conn, "src-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setSrcGate( toString( @1) );
                }
        | NAME PLUSPLUS
                {
                  ps.conn->setSrcGate( toString( @1) );
                  ps.conn->setSrcGatePlusplus(true);
                }
        ;

parentleftgate
        : NAME vector
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                  addVector(ps.conn, "src-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                }
        | NAME PLUSPLUS
                {
                  ps.conn = (ConnectionNode *)createNodeWithTag(NED_CONNECTION, ps.inLoop ? (NEDElement*)ps.forloop : (NEDElement*)ps.conns );
                  ps.conn->setSrcModule("");
                  ps.conn->setSrcGate(toString(@1));
                  ps.conn->setSrcGatePlusplus(true);
                }
        ;

rightgatespec
        : rightmod '.' rightgate
        | parentrightgate
        ;

rightmod
        : NAME vector
                {
                  ps.conn->setDestModule( toString(@1) );
                  addVector(ps.conn, "dest-module-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setDestModule( toString(@1) );
                }
        ;

rightgate
        : NAME vector
                {
                  ps.conn->setDestGate( toString( @1) );
                  addVector(ps.conn, "dest-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setDestGate( toString( @1) );
                }
        | NAME PLUSPLUS
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGatePlusplus(true);
                }
        ;

parentrightgate
        : NAME vector
                {
                  ps.conn->setDestGate( toString( @1) );
                  addVector(ps.conn, "dest-gate-index",@2,$2);
                }
        | NAME
                {
                  ps.conn->setDestGate( toString( @1) );
                }
        | NAME PLUSPLUS
                {
                  ps.conn->setDestGate( toString( @1) );
                  ps.conn->setDestGatePlusplus(true);
                }
        ;


channeldescr
        : NAME
                {
                  ps.connattr = addConnAttr(ps.conn,"channel");
                  addExpression(ps.connattr, "value",@1,createExpression(createConst(NED_CONST_STRING, toString(@1))));
                }
        | NAME expression
                {
                  ps.connattr = addConnAttr(ps.conn,toString(@1));
                  addExpression(ps.connattr, "value",@2,$2);
                }
        | channeldescr NAME expression
                {
                  ps.connattr = addConnAttr(ps.conn,toString(@2));
                  addExpression(ps.connattr, "value",@3,$3);
                }
        ;

/*
 * Network - old syntax
 */
networkdefinition_old
        : networkheader_old
            opt_on_blocks_old
            opt_substparamblocks_old
          endnetwork_old
        ;

networkheader_old
        : NETWORK NAME ':' NAME opt_semicolon
                {
                  ps.network = addNetwork(ps.nedfile,@2,@4);
                  setComments(ps.network,@1,@5);
                  ps.inNetwork=1;
                }
        ;

endnetwork_old
        : ENDNETWORK opt_semicolon
                {
                  setTrailingComment(ps.network,@1);
                  ps.inNetwork=0;
                }
        ;

/*
 * Network - new syntax
 */
networkdefinition
        : networkheader '{'
            opt_on_blocks
            opt_substparamblocks
          '}' opt_semicolon
                {
                  setTrailingComment(ps.network,@5);
                  ps.inNetwork=0;
                }
        ;

networkheader
        : NETWORK NAME ':' NAME opt_semicolon
                {
                  ps.network = addNetwork(ps.nedfile,@2,@4);
                  setComments(ps.network,@1,@5);
                  ps.inNetwork=1;
                }
        ;

/*
 * Common part - old and new
 */

vector
        : '[' expression ']'
                { $$ = $2; }
        ;

expression
        :
          expr
                {
                  if (ps.parseExpressions) $$ = createExpression($1);
                }
        | inputvalue
                {
                  if (ps.parseExpressions) $$ = createExpression($1);
                }
        | xmldocvalue
                {
                  if (ps.parseExpressions) $$ = createExpression($1);
                }
        ;

/*
 * Expressions (3 shift-reduce conflicts here)
 */

inputvalue
        : INPUT_ '(' expr ',' expr ')'
                { if (ps.parseExpressions) $$ = createFunction("input", $3, $5); }
        | INPUT_ '(' expr ')'
                { if (ps.parseExpressions) $$ = createFunction("input", $3); }
        | INPUT_ '(' ')'
                { if (ps.parseExpressions) $$ = createFunction("input"); }
        | INPUT_
                { if (ps.parseExpressions) $$ = createFunction("input"); }
        ;

xmldocvalue
        : XMLDOC '(' stringconstant ',' stringconstant ')'
                { if (ps.parseExpressions) $$ = createFunction("xmldoc", $3, $5); }
        | XMLDOC '(' stringconstant ')'
                { if (ps.parseExpressions) $$ = createFunction("xmldoc", $3); }
        ;

expr
        : simple_expr
        | '(' expr ')'
                { $$ = $2; }

        | expr '+' expr
                { if (ps.parseExpressions) $$ = createOperator("+", $1, $3); }
        | expr '-' expr
                { if (ps.parseExpressions) $$ = createOperator("-", $1, $3); }
        | expr '*' expr
                { if (ps.parseExpressions) $$ = createOperator("*", $1, $3); }
        | expr '/' expr
                { if (ps.parseExpressions) $$ = createOperator("/", $1, $3); }
        | expr '%' expr
                { if (ps.parseExpressions) $$ = createOperator("%", $1, $3); }
        | expr '^' expr
                { if (ps.parseExpressions) $$ = createOperator("^", $1, $3); }

        | '-' expr
                %prec UMIN
                { if (ps.parseExpressions) $$ = unaryMinus($2); }

        | expr EQ expr
                { if (ps.parseExpressions) $$ = createOperator("==", $1, $3); }
        | expr NE expr
                { if (ps.parseExpressions) $$ = createOperator("!=", $1, $3); }
        | expr GT expr
                { if (ps.parseExpressions) $$ = createOperator(">", $1, $3); }
        | expr GE expr
                { if (ps.parseExpressions) $$ = createOperator(">=", $1, $3); }
        | expr LS expr
                { if (ps.parseExpressions) $$ = createOperator("<", $1, $3); }
        | expr LE expr
                { if (ps.parseExpressions) $$ = createOperator("<=", $1, $3); }

        | expr AND expr
                { if (ps.parseExpressions) $$ = createOperator("&&", $1, $3); }
        | expr OR expr
                { if (ps.parseExpressions) $$ = createOperator("||", $1, $3); }
        | expr XOR expr
                { if (ps.parseExpressions) $$ = createOperator("##", $1, $3); }

        | NOT expr
                %prec UMIN
                { if (ps.parseExpressions) $$ = createOperator("!", $2); }

        | expr BIN_AND expr
                { if (ps.parseExpressions) $$ = createOperator("&", $1, $3); }
        | expr BIN_OR expr
                { if (ps.parseExpressions) $$ = createOperator("|", $1, $3); }
        | expr BIN_XOR expr
                { if (ps.parseExpressions) $$ = createOperator("#", $1, $3); }

        | BIN_COMPL expr
                %prec UMIN
                { if (ps.parseExpressions) $$ = createOperator("~", $2); }
        | expr SHIFT_LEFT expr
                { if (ps.parseExpressions) $$ = createOperator("<<", $1, $3); }
        | expr SHIFT_RIGHT expr
                { if (ps.parseExpressions) $$ = createOperator(">>", $1, $3); }
        | expr '?' expr ':' expr
                { if (ps.parseExpressions) $$ = createOperator("?:", $1, $3, $5); }

        | NAME '(' ')'
                { if (ps.parseExpressions) $$ = createFunction(toString(@1)); }
        | NAME '(' expr ')'
                { if (ps.parseExpressions) $$ = createFunction(toString(@1), $3); }
        | NAME '(' expr ',' expr ')'
                { if (ps.parseExpressions) $$ = createFunction(toString(@1), $3, $5); }
        | NAME '(' expr ',' expr ',' expr ')'
                { if (ps.parseExpressions) $$ = createFunction(toString(@1), $3, $5, $7); }
        | NAME '(' expr ',' expr ',' expr ',' expr ')'
                { if (ps.parseExpressions) $$ = createFunction(toString(@1), $3, $5, $7, $9); }
         ;

simple_expr
        : parameter_expr
        | string_expr
        | boolconst_expr
        | numconst_expr
        | special_expr
        ;

parameter_expr
        : NAME
                {
                  // if there's no modifier, might be a loop variable too
                  $$ = createParamRefOrIdent(toString(@1));
                }
        | REF NAME
                {
                  $$ = createParamRef(toString(@2));
                  ((ParamRefNode *)$$)->setIsRef(true);
                }
        | REF ANCESTOR NAME
                {
                  $$ = createParamRef(toString(@3));
                  ((ParamRefNode *)$$)->setIsRef(true);
                  ((ParamRefNode *)$$)->setIsAncestor(true);
                }
        | ANCESTOR REF NAME
                {
                  $$ = createParamRef(toString(@3));
                  ((ParamRefNode *)$$)->setIsRef(true);
                  ((ParamRefNode *)$$)->setIsAncestor(true);
                }
        | ANCESTOR NAME
                {
                  $$ = createParamRef(toString(@2));
                  ((ParamRefNode *)$$)->setIsAncestor(true);
                }
        ;

string_expr
        : stringconstant
        ;

boolconst_expr
        : TRUE_
                { $$ = createConst(NED_CONST_BOOL, "true"); }
        | FALSE_
                { $$ = createConst(NED_CONST_BOOL, "false"); }
        ;

numconst_expr
        : numconst
        ;

special_expr
        : SUBMODINDEX
                { if (ps.parseExpressions) $$ = createFunction("index"); }
        | SUBMODINDEX '(' ')'
                { if (ps.parseExpressions) $$ = createFunction("index"); }
        | SIZEOF '(' NAME ')'
                { if (ps.parseExpressions) $$ = createFunction("sizeof", createIdent(toString(@3))); }
        ;

stringconstant
        : STRINGCONSTANT
                { $$ = createConst(NED_CONST_STRING, toString(trimQuotes(@1))); }
        ;

numconst
        : INTCONSTANT
                { $$ = createConst(NED_CONST_INT, toString(@1)); }
        | REALCONSTANT
                { $$ = createConst(NED_CONST_REAL, toString(@1)); }
        | timeconstant
                { $$ = createTimeConst(toString(@1)); }

        ;

timeconstant
        : timeconstant INTCONSTANT NAME
        | timeconstant REALCONSTANT NAME
        | INTCONSTANT NAME
        | REALCONSTANT NAME
        ;

/*
 * NED-2: Message subclassing (no shift-reduce conflict here)
 */

cplusplus
        : CPLUSPLUS CPLUSPLUSBODY opt_semicolon
                {
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.nedfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(@2)));
                  setComments(ps.cplusplus,@1,@2);
                }
        ;

struct_decl
        : STRUCT NAME ';'
                {
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.nedfile );
                  ps.structdecl->setName(toString(@2));
                  setComments(ps.structdecl,@1,@2);
                }
        ;

class_decl
        : CLASS NAME ';'
                {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setName(toString(@2));
                  ps.classdecl->setIsCobject(true);
                  setComments(ps.classdecl,@1,@2);
                }
        | CLASS NONCOBJECT NAME ';'
                {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.nedfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(@3));
                  setComments(ps.classdecl,@1,@2);
                }
        ;

message_decl
        : MESSAGE NAME ';'
                {
                  ps.messagedecl = (MessageDeclNode *)createNodeWithTag(NED_MESSAGE_DECL, ps.nedfile );
                  ps.messagedecl->setName(toString(@2));
                  setComments(ps.messagedecl,@1,@2);
                }
        ;

enum_decl
        : ENUM NAME ';'
                {
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.nedfile );
                  ps.enumdecl->setName(toString(@2));
                  setComments(ps.enumdecl,@1,@2);
                }
        ;

enum
        : ENUM NAME '{'
                {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(@2));
                  setComments(ps.enump,@1,@2);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
          opt_enumfields '}' opt_semicolon
                {
                  setTrailingComment(ps.enump,@6);
                }
        | ENUM NAME EXTENDS NAME '{'
                {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.nedfile );
                  ps.enump->setName(toString(@2));
                  ps.enump->setExtendsName(toString(@4));
                  setComments(ps.enump,@1,@4);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
          opt_enumfields '}' opt_semicolon
                {
                  setTrailingComment(ps.enump,@8);
                }
        ;

opt_enumfields
        : enumfields
        |
        ;

enumfields
        : enumfields enumfield
        | enumfield
        ;

enumfield
        : NAME ';'
                {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(@1));
                  setComments(ps.enumfield,@1,@1);
                }
        | NAME '=' enumvalue ';'
                {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(@1));
                  ps.enumfield->setValue(toString(@3));
                  setComments(ps.enumfield,@1,@3);
                }
        ;

message
        : MESSAGE NAME '{'
                {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(@2));
                  setComments(ps.messagep,@1,@2);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.messagep,@7);
                }
        | MESSAGE NAME EXTENDS NAME '{'
                {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.nedfile );
                  ps.messagep->setName(toString(@2));
                  ps.messagep->setExtendsName(toString(@4));
                  setComments(ps.messagep,@1,@4);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.messagep,@9);
                }
        ;

class
        : CLASS NAME '{'
                {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(@2));
                  setComments(ps.classp,@1,@2);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.classp,@7);
                }
        | CLASS NAME EXTENDS NAME '{'
                {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.nedfile );
                  ps.classp->setName(toString(@2));
                  ps.classp->setExtendsName(toString(@4));
                  setComments(ps.classp,@1,@4);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.classp,@9);
                }
        ;

struct
        : STRUCT NAME '{'
                {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(@2));
                  setComments(ps.structp,@1,@2);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.structp,@7);
                }
        | STRUCT NAME EXTENDS NAME '{'
                {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.nedfile );
                  ps.structp->setName(toString(@2));
                  ps.structp->setExtendsName(toString(@4));
                  setComments(ps.structp,@1,@4);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  setTrailingComment(ps.structp,@9);
                }
        ;

opt_propertiesblock
        : PROPERTIES ':'
                {
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  setComments(ps.properties,@1);
                }
          opt_properties
        |
        ;

opt_properties
        : properties
        |
        ;

properties
        : properties property
        | property
        ;

property
        : NAME '=' propertyvalue ';'
                {
                  ps.property = (PropertyNode *)createNodeWithTag(NED_PROPERTY, ps.properties);
                  ps.property->setName(toString(@1));
                  ps.property->setValue(toString(@3));
                  setComments(ps.property,@1,@3);
                }
        ;

propertyvalue
        : STRINGCONSTANT
        | INTCONSTANT
        | REALCONSTANT
        | timeconstant
        | TRUE_
        | FALSE_
        ;

opt_fieldsblock
        : FIELDS ':'
                {
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  setComments(ps.fields,@1);
                }
          opt_fields
        |
        ;

opt_fields
        : fields
        |
        ;

fields
        : fields field
        | field
        ;

field
        : fieldmodifiers fielddatatype NAME
                {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(@3));
                  ps.field->setDataType(toString(@2));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                }
           opt_fieldvector opt_fieldenum opt_fieldvalue ';'
                {
                  setComments(ps.field,@1,@7);
                }
        | fieldmodifiers NAME
                {
                  ps.field = (FieldNode *)createNodeWithTag(NED_FIELD, ps.fields);
                  ps.field->setName(toString(@2));
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsReadonly(ps.isReadonly);
                }
           opt_fieldvector opt_fieldenum opt_fieldvalue ';'
                {
                  setComments(ps.field,@1,@6);
                }
        ;

fieldmodifiers
        : ABSTRACT
                { ps.isAbstract = true; ps.isReadonly = false; }
        | READONLY
                { ps.isAbstract = false; ps.isReadonly = true; }
        | ABSTRACT READONLY
                { ps.isAbstract = true; ps.isReadonly = true; }
        | READONLY ABSTRACT
                { ps.isAbstract = true; ps.isReadonly = true; }
        |
                { ps.isAbstract = false; ps.isReadonly = false; }
        ;

fielddatatype
        : NAME
        | NAME '*'

        | CHARTYPE
        | SHORTTYPE
        | INTTYPE
        | LONGTYPE

        | UNSIGNED_ CHARTYPE
        | UNSIGNED_ SHORTTYPE
        | UNSIGNED_ INTTYPE
        | UNSIGNED_ LONGTYPE

        | DOUBLETYPE
        | STRINGTYPE
        | BOOLTYPE
        ;


opt_fieldvector
        : '[' INTCONSTANT ']'
                {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(@2));
                }
        | '[' NAME ']'
                {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(@2));
                }
        | '[' ']'
                {
                  ps.field->setIsVector(true);
                }
        |
        ;

opt_fieldenum
        : ENUM '(' NAME ')'
                {
                  ps.field->setEnumName(toString(@3));
                }
        |
        ;

opt_fieldvalue
        : '=' fieldvalue
                {
                  ps.field->setDefaultValue(toString(@2));
                }
        |
        ;

/*
fieldvalue
        : STRINGCONSTANT
        | CHARCONSTANT
        | INTCONSTANT
        | REALCONSTANT
        | timeconstant
        | TRUE_
        | FALSE_
        | NAME
        ;
*/

fieldvalue
        : fieldvalue_token fieldvalue
        | fieldvalue_token
        ;

fieldvalue_token
        : STRINGCONSTANT
        | CHARCONSTANT
        | INTCONSTANT
        | REALCONSTANT
        | timeconstant
        | TRUE_
        | FALSE_
        | NAME
        | '(' | ')' | '.' | ','
        | '?' | ':'
        | AND | OR | XOR
        | EQ | NE | GT | GE | LS | LE
        | BIN_AND | BIN_OR | BIN_XOR
        | SHIFT_LEFT | SHIFT_RIGHT
        | '+' | '-' | '*' | '/' | '%' | '^'
        ;


enumvalue
        : INTCONSTANT
        | '-' INTCONSTANT
        | NAME
        ;

opt_semicolon : ';' | ;

comma_or_semicolon : ',' | ';' ;

%%

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
