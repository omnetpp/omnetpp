/*===============================================================
 * File: msg2.y
 *
 *  Grammar for OMNeT++ MSG-2.
 *
 *  Author: Andras Varga
 *
 *=============================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 1992,2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

%token CPLUSPLUS CPLUSPLUSBODY
%token MESSAGE CLASS STRUCT ENUM NONCOBJECT
%token EXTENDS FIELDS PROPERTIES ABSTRACT READONLY

%token NAME
%token INTCONSTANT REALCONSTANT STRINGCONSTANT CHARCONSTANT
%token TRUE_ FALSE_
%token BOOLTYPE CHARTYPE SHORTTYPE INTTYPE LONGTYPE DOUBLETYPE UNSIGNED_ STRINGTYPE

%token EQ NE GE LE
%token AND OR XOR NOT
%token BIN_AND BIN_OR BIN_XOR BIN_COMPL
%token SHIFT_LEFT SHIFT_RIGHT

%token INVALID_CHAR   /* just to generate parse error --VA */

/* Operator precedences (low to high) and associativity */
%left '?' ':'
%left AND OR XOR
%left EQ NE '>' GE '<' LE
%left BIN_AND BIN_OR BIN_XOR
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-'
%left '*' '/' '%'
%right '^'
%left UMIN NOT BIN_COMPL

%start networkdescription


%{

#include <stdio.h>
#include <stdlib.h>
#include "nedyydefs.h"
#include "nederror.h"

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <string.h>         /* YYVERBOSE needs it */
#endif

#define yylloc msg2yylloc
#define yyin msg2yyin
#define yyout msg2yyout
#define yyrestart msg2yyrestart
#define yy_scan_string msg2yy_scan_string
#define yy_delete_buffer msg2yy_delete_buffer
extern FILE *yyin;
extern FILE *yyout;
struct yy_buffer_state;
struct yy_buffer_state *yy_scan_string(const char *str);
void yy_delete_buffer(struct yy_buffer_state *);
void yyrestart(FILE *);
int yylex();
void yyerror (const char *s);

#include "nedparser.h"
#include "nedfilebuffer.h"
#include "nedelements.h"
#include "nedutil.h"
#include "nedyylib.h"

static struct MSG2ParserState
{
    /* tmp flags, used with msg fields */
    bool isAbstract;
    bool isReadonly;

    /* MSG-II: message subclassing */
    MsgFileNode *msgfile;
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

static void resetParserState()
{
    static MSG2ParserState cleanps;
    ps = cleanps;
}

//FIXME add @-syntax properties to the grammar!

%}

%%

/*
 * Top-level components
 */
networkdescription
        : somedefinitions
        ;

somedefinitions
        : somedefinitions definition
        |
        ;

definition
        : cplusplus
        | struct_decl
        | class_decl
        | message_decl
        | enum_decl

        | enum
                { if (np->getStoreSourceFlag()) ps.enump->setSourceCode(toString(@1)); }
        | message
                { if (np->getStoreSourceFlag()) ps.messagep->setSourceCode(toString(@1)); }
        | class
                { if (np->getStoreSourceFlag()) ps.classp->setSourceCode(toString(@1)); }
        | struct
                { if (np->getStoreSourceFlag()) ps.structp->setSourceCode(toString(@1)); }
        ;

cplusplus
        : CPLUSPLUS CPLUSPLUSBODY opt_semicolon
                {
                  ps.cplusplus = (CplusplusNode *)createNodeWithTag(NED_CPLUSPLUS, ps.msgfile );
                  ps.cplusplus->setBody(toString(trimDoubleBraces(@2)));
                  storeBannerAndRightComments(ps.cplusplus,@1,@2);
                }
        ;

struct_decl
        : STRUCT NAME ';'
                {
                  ps.structdecl = (StructDeclNode *)createNodeWithTag(NED_STRUCT_DECL, ps.msgfile );
                  ps.structdecl->setName(toString(@2));
                  storeBannerAndRightComments(ps.structdecl,@1,@2);
                }
        ;

class_decl
        : CLASS NAME ';'
                {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.msgfile );
                  ps.classdecl->setName(toString(@2));
                  ps.classdecl->setIsCobject(true);
                  storeBannerAndRightComments(ps.classdecl,@1,@2);
                }
        | CLASS NONCOBJECT NAME ';'
                {
                  ps.classdecl = (ClassDeclNode *)createNodeWithTag(NED_CLASS_DECL, ps.msgfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(@3));
                  storeBannerAndRightComments(ps.classdecl,@1,@2);
                }
        ;

message_decl
        : MESSAGE NAME ';'
                {
                  ps.messagedecl = (MessageDeclNode *)createNodeWithTag(NED_MESSAGE_DECL, ps.msgfile );
                  ps.messagedecl->setName(toString(@2));
                  storeBannerAndRightComments(ps.messagedecl,@1,@2);
                }
        ;

enum_decl
        : ENUM NAME ';'
                {
                  ps.enumdecl = (EnumDeclNode *)createNodeWithTag(NED_ENUM_DECL, ps.msgfile );
                  ps.enumdecl->setName(toString(@2));
                  storeBannerAndRightComments(ps.enumdecl,@1,@2);
                }
        ;

enum
        : ENUM NAME '{'
                {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.msgfile );
                  ps.enump->setName(toString(@2));
                  storeBannerAndRightComments(ps.enump,@1,@2);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
          opt_enumfields '}' opt_semicolon
                {
                  storeTrailingComment(ps.enump,@$);
                }
        | ENUM NAME EXTENDS NAME '{'
                {
                  ps.enump = (EnumNode *)createNodeWithTag(NED_ENUM, ps.msgfile );
                  ps.enump->setName(toString(@2));
                  ps.enump->setExtendsName(toString(@4));
                  storeBannerAndRightComments(ps.enump,@1,@4);
                  ps.enumfields = (EnumFieldsNode *)createNodeWithTag(NED_ENUM_FIELDS, ps.enump);
                }
          opt_enumfields '}' opt_semicolon
                {
                  storeTrailingComment(ps.enump,@$);
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
                  storeBannerAndRightComments(ps.enumfield,@1,@1);
                }
        | NAME '=' enumvalue ';'
                {
                  ps.enumfield = (EnumFieldNode *)createNodeWithTag(NED_ENUM_FIELD, ps.enumfields);
                  ps.enumfield->setName(toString(@1));
                  ps.enumfield->setValue(toString(@3));
                  storeBannerAndRightComments(ps.enumfield,@1,@3);
                }
        ;

message
        : MESSAGE NAME '{'
                {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.msgfile );
                  ps.messagep->setName(toString(@2));
                  storeBannerAndRightComments(ps.messagep,@1,@2);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  storeTrailingComment(ps.messagep,@$);
                }
        | MESSAGE NAME EXTENDS NAME '{'
                {
                  ps.msgclassorstruct = ps.messagep = (MessageNode *)createNodeWithTag(NED_MESSAGE, ps.msgfile );
                  ps.messagep->setName(toString(@2));
                  ps.messagep->setExtendsName(toString(@4));
                  storeBannerAndRightComments(ps.messagep,@1,@4);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  storeTrailingComment(ps.messagep,@$);
                }
        ;

class
        : CLASS NAME '{'
                {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.msgfile );
                  ps.classp->setName(toString(@2));
                  storeBannerAndRightComments(ps.classp,@1,@2);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  storeTrailingComment(ps.classp,@$);
                }
        | CLASS NAME EXTENDS NAME '{'
                {
                  ps.msgclassorstruct = ps.classp = (ClassNode *)createNodeWithTag(NED_CLASS, ps.msgfile );
                  ps.classp->setName(toString(@2));
                  ps.classp->setExtendsName(toString(@4));
                  storeBannerAndRightComments(ps.classp,@1,@4);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  storeTrailingComment(ps.classp,@$);
                }
        ;

struct
        : STRUCT NAME '{'
                {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.msgfile );
                  ps.structp->setName(toString(@2));
                  storeBannerAndRightComments(ps.structp,@1,@2);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  storeTrailingComment(ps.structp,@$);
                }
        | STRUCT NAME EXTENDS NAME '{'
                {
                  ps.msgclassorstruct = ps.structp = (StructNode *)createNodeWithTag(NED_STRUCT, ps.msgfile );
                  ps.structp->setName(toString(@2));
                  ps.structp->setExtendsName(toString(@4));
                  storeBannerAndRightComments(ps.structp,@1,@4);
                }
          opt_propertiesblock opt_fieldsblock '}' opt_semicolon
                {
                  storeTrailingComment(ps.structp,@$);
                }
        ;

opt_propertiesblock
        : PROPERTIES ':'
                {
                  ps.properties = (PropertiesNode *)createNodeWithTag(NED_PROPERTIES, ps.msgclassorstruct);
                  storeBannerAndRightComments(ps.properties,@1);
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
                  ps.msgproperty = (MsgpropertyNode *)createNodeWithTag(NED_MSGPROPERTY, ps.properties);
                  ps.msgproperty->setName(toString(@1));
                  ps.msgproperty->setValue(toString(@3));
                  storeBannerAndRightComments(ps.msgproperty,@1,@3);
                }
        ;

propertyvalue
        : STRINGCONSTANT
        | INTCONSTANT
        | REALCONSTANT
        | quantity
        | TRUE_
        | FALSE_
        ;

opt_fieldsblock
        : FIELDS ':'
                {
                  ps.fields = (FieldsNode *)createNodeWithTag(NED_FIELDS, ps.msgclassorstruct);
                  storeBannerAndRightComments(ps.fields,@1);
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
                  storeBannerAndRightComments(ps.field,@1,@7);
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
                  storeBannerAndRightComments(ps.field,@1,@6);
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


fieldvalue   /* some arbitrary C++ expression - validation left to C++ compiler */
        : fieldvalue fieldvalueitem
        | fieldvalueitem
        ;

fieldvalueitem
        : STRINGCONSTANT
        | CHARCONSTANT
        | INTCONSTANT
        | REALCONSTANT
        | quantity
        | TRUE_
        | FALSE_
        | NAME
        | '?' | ':' | AND | OR | XOR | EQ | NE | '>' | GE | '<' | LE
        | BIN_AND | BIN_OR | BIN_XOR | SHIFT_LEFT | SHIFT_RIGHT
        | '+' | '-' | '*' | '/' | '%' | '^' | '&' | UMIN | NOT | BIN_COMPL
        | '.' | ',' | '(' | ')' | '[' | ']'
        ;

enumvalue
        : INTCONSTANT
        | '-' INTCONSTANT
        | NAME
        ;

quantity
        : quantity INTCONSTANT NAME
        | quantity REALCONSTANT NAME
        | INTCONSTANT NAME
        | REALCONSTANT NAME
        ;

opt_semicolon : ';' | ;

%%

//----------------------------------------------------------------------
// general bison/flex stuff:
//

NEDElement *doParseMSG2(NEDParser *p, const char *nedtext)
{
#if YYDEBUG != 0      /* #if added --VA */
    yydebug = YYDEBUGGING_ON;
#endif

    // reset the lexer
    pos.co = 0;
    pos.li = 1;
    prevpos = pos;

    yyin = NULL;
    yyout = stderr; // not used anyway

    // alloc buffer
    struct yy_buffer_state *handle = yy_scan_string(nedtext);
    if (!handle)
        {np->getErrors()->addError("", "unable to allocate work memory"); return false;}

    // create parser state and NEDFileNode
    np = p;
    resetParserState();
    ps.msgfile = new MsgFileNode();

    // store file name with slashes always, even on Windows -- neddoc relies on that
    ps.msgfile->setFilename(slashifyFilename(np->getFileName()).c_str());

    // store file comment
    storeFileComment(ps.msgfile);

    if (np->getStoreSourceFlag())
        storeSourceCode(ps.msgfile, np->getSource()->getFullTextPos());

    // parse
    int ret;
    try
    {
        ret = yyparse();
    }
    catch (NEDException& e)
    {
        yyerror((std::string("error during parsing: ")+e.what()).c_str());
        yy_delete_buffer(handle);
        return 0;
    }

    yy_delete_buffer(handle);

    //FIXME TODO: fill in @documentation properties from comments
    return ps.msgfile;
}

void yyerror(const char *s)
{
    // chop newline
    char buf[250];
    strcpy(buf, s);
    if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

    np->error(buf, pos.li);
}

