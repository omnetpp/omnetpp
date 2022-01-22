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

/* number of expected shift-reduce conflicts */
%expect 0

/* Reserved words. Trailing '_' in some token names are there to avoid
 * (potential) conflict with macros defined in system headers.
 */
%token NAMESPACE CPLUSPLUS CPLUSPLUSBODY
%token MESSAGE PACKET CLASS STRUCT ENUM NONCOBJECT
%token EXTENDS ABSTRACT
%token IMPORT USING

%token NAME PROPNAME DOUBLECOLON
%token INTCONSTANT REALCONSTANT STRINGCONSTANT CHARCONSTANT
%token TRUE_ FALSE_
%token BOOL_ CHAR_ SHORT_ INT_ LONG_ DOUBLE_ UNSIGNED_ CONST_ STRING_

%token EQ NE GE LE
%token AND OR XOR
%token SHIFT_LEFT SHIFT_RIGHT

%token COMMONCHAR
%token INVALID_CHAR   /* just to generate parse error */

/* Operator precedences (low to high) and associativity */
%left '?' ':'
%left AND OR XOR
%left EQ NE '>' GE '<' LE
%left '&' '|' '#'
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-'
%left '*' '/' '%'
%right '^'
%left UMIN '!' '~'

%start msgfile

%parse-param {omnetpp::nedxml::ParseContext *np}

/* requires at least bison 1.50 (tested with bison 2.1); otherwise won't parse "class B extends A;" syntax */
%glr-parser

%{

#include <cstdio>
#include <cstdlib>
#include "yydefs.h"
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "errorstore.h"
#include "sourcedocument.h"
#include "exception.h"
#include "msgelements.h"

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <cstring>         /* YYVERBOSE needs it */
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
void yyerror (omnetpp::nedxml::ParseContext *np, const char *s);

#include "msgyyutil.h"

using namespace omnetpp;

using namespace omnetpp::common;
using namespace omnetpp::nedxml;
using namespace omnetpp::nedxml::msgyyutil;

static struct MSG2ParserState
{
    /* tmp flags, used with msg fields */
    bool isAbstract;
    bool isConst;
    bool isPointer;
    std::string dataType;

    std::vector<ASTNode *> propvals; // temporarily collects property values

    /* MSG-II: message subclassing */
    MsgFileElement *msgfile;
    NamespaceElement *namespacedecl;
    CplusplusElement *cplusplus;
    ImportElement *import;
    StructDeclElement *structdecl;
    ClassDeclElement *classdecl;
    MessageDeclElement *messagedecl;
    PacketDeclElement *packetdecl;
    EnumDeclElement *enumdecl;
    EnumElement *enump;
    MessageElement *messagep;
    PacketElement *packetp;
    ClassElement *classp;
    StructElement *structp;
    ASTNode *msgclassorstruct;
    EnumFieldElement *enumfield;
    FieldElement *field;
    PropertyElement *property;
    PropertyKeyElement *propkey;
} ps;

static void resetParserState()
{
    static MSG2ParserState cleanps;
    ps = cleanps;
}

%}

%%

/*
 * Top-level components
 */
msgfile
        : definitions
        ;

definitions
        : definitions definition
        | %empty
        ;

definition
        : namespace_decl
        | fileproperty
        | cplusplus
        | import
        | struct_decl
        | class_decl
        | message_decl
        | packet_decl
        | enum_decl

        | enum
                { if (np->getStoreSourceFlag()) ps.enump->setSourceCode(toString(np, @1)); }
        | message
                { if (np->getStoreSourceFlag()) ps.messagep->setSourceCode(toString(np, @1)); }
        | packet
                { if (np->getStoreSourceFlag()) ps.packetp->setSourceCode(toString(np, @1)); }
        | class
                { if (np->getStoreSourceFlag()) ps.classp->setSourceCode(toString(np, @1)); }
        | struct
                { if (np->getStoreSourceFlag()) ps.structp->setSourceCode(toString(np, @1)); }
        ;

/*
 * namespace declaration
 */
namespace_decl
        : NAMESPACE qname ';'
                {
                  ps.namespacedecl = (NamespaceElement *)createMsgElementWithTag(np, MSG_NAMESPACE, ps.msgfile );
                  ps.namespacedecl->setName(removeSpaces(np, @2).c_str());
                  storeBannerAndRightComments(np, ps.namespacedecl,@1,@2);
                }
        | NAMESPACE ';'
                {
                  ps.namespacedecl = (NamespaceElement *)createMsgElementWithTag(np, MSG_NAMESPACE, ps.msgfile );
                  storeBannerAndRightComments(np, ps.namespacedecl,@1,@1);
                }

qname
        : DOUBLECOLON qname1
        | qname1
        ;

qname1
        : qname1 DOUBLECOLON NAME
        | NAME
        ;

/*
 * File Property
 */
fileproperty
        : property_namevalue ';'
                {
                  storePos(np,  ps.property, @$);
                  storeBannerAndRightComments(np, ps.property,@$);
                }
        ;

/*
 * C++ block
 */
cplusplus
        : CPLUSPLUS CPLUSPLUSBODY opt_semicolon
                {
                  ps.cplusplus = (CplusplusElement *)createMsgElementWithTag(np, MSG_CPLUSPLUS, ps.msgfile );
                  ps.cplusplus->setBody(toString(np, trimDoubleBraces(@2)));
                  storeBannerAndRightComments(np, ps.cplusplus,@1,@2);
                }
        | CPLUSPLUS '(' targetspec ')' CPLUSPLUSBODY opt_semicolon
                {
                  ps.cplusplus = (CplusplusElement *)createMsgElementWithTag(np, MSG_CPLUSPLUS, ps.msgfile );
                  ps.cplusplus->setTarget(toString(np, @3));
                  ps.cplusplus->setBody(toString(np, trimDoubleBraces(@5)));
                  storeBannerAndRightComments(np, ps.cplusplus,@1,@5);
                }
        ;
        ;

targetspec
        : targetspec targetitem
        | targetitem
        ;

targetitem
        : NAME | DOUBLECOLON | INTCONSTANT | ':' | '.' | ',' | '~' | '=' | '&'
        ;

/*
 * Import
 */
import
        : IMPORT importspec ';'
                {
                  ps.import = (ImportElement *)createMsgElementWithTag(np, MSG_IMPORT, ps.msgfile);
                  ps.import->setImportSpec(removeSpaces(np, @2).c_str());
                  storePos(np,  ps.import,@$);
                  storeBannerAndRightComments(np, ps.import,@$);
                }
        ; /* no error recovery rule -- see discussion at top */

importspec
        : importspec '.' importname
        | importname
        ;

importname
        : NAME
        | MESSAGE | PACKET | CLASS | STRUCT | ENUM | ABSTRACT  /* for convenience, we also allow some keywords to be used as folder names */
        ;

/*
 * Forward declarations
 */
struct_decl
        : STRUCT qname ';'
                {
                  ps.structdecl = (StructDeclElement *)createMsgElementWithTag(np, MSG_STRUCT_DECL, ps.msgfile );
                  ps.structdecl->setName(removeSpaces(np, @2).c_str());
                  storeBannerAndRightComments(np, ps.structdecl,@1,@2);
                }
        ;

class_decl
        : CLASS qname ';'
                {
                  ps.classdecl = (ClassDeclElement *)createMsgElementWithTag(np, MSG_CLASS_DECL, ps.msgfile );
                  ps.classdecl->setName(removeSpaces(np, @2).c_str());
                  ps.classdecl->setIsCobject(true);
                  storeBannerAndRightComments(np, ps.classdecl,@1,@2);
                }
        | CLASS NONCOBJECT qname ';'
                {
                  ps.classdecl = (ClassDeclElement *)createMsgElementWithTag(np, MSG_CLASS_DECL, ps.msgfile );
                  ps.classdecl->setIsCobject(false);
                  ps.classdecl->setName(toString(np, @3));
                  storeBannerAndRightComments(np, ps.classdecl,@1,@3);
                }
        | CLASS qname EXTENDS qname ';'
                {
                  ps.classdecl = (ClassDeclElement *)createMsgElementWithTag(np, MSG_CLASS_DECL, ps.msgfile );
                  ps.classdecl->setIsCobject(true);
                  ps.classdecl->setName(removeSpaces(np, @2).c_str());
                  ps.classdecl->setExtendsName(toString(np, @4));
                  storeBannerAndRightComments(np, ps.classdecl,@1,@4);
                }
        ;

message_decl
        : MESSAGE qname ';'
                {
                  ps.messagedecl = (MessageDeclElement *)createMsgElementWithTag(np, MSG_MESSAGE_DECL, ps.msgfile );
                  ps.messagedecl->setName(removeSpaces(np, @2).c_str());
                  storeBannerAndRightComments(np, ps.messagedecl,@1,@2);
                }
        ;

packet_decl
        : PACKET qname ';'
                {
                  ps.packetdecl = (PacketDeclElement *)createMsgElementWithTag(np, MSG_PACKET_DECL, ps.msgfile );
                  ps.packetdecl->setName(removeSpaces(np, @2).c_str());
                  storeBannerAndRightComments(np, ps.packetdecl,@1,@2);
                }
        ;

enum_decl
        : ENUM qname ';'
                {
                  ps.enumdecl = (EnumDeclElement *)createMsgElementWithTag(np, MSG_ENUM_DECL, ps.msgfile );
                  ps.enumdecl->setName(removeSpaces(np, @2).c_str());
                  storeBannerAndRightComments(np, ps.enumdecl,@1,@2);
                }
        ;

/*
 * Enum definition
 */
enum
        : ENUM qname '{'
                {
                  ps.enump = (EnumElement *)createMsgElementWithTag(np, MSG_ENUM, ps.msgfile );
                  ps.enump->setName(removeSpaces(np, @2).c_str());
                  storeBannerAndRightComments(np, ps.enump,@1,@2);
                }
          opt_enumfields '}' opt_semicolon
                { storeTrailingComment(np, ps.enump,@$); }
        ;

opt_enumfields
        : enumfields
        | %empty
        ;

enumfields
        : enumfields enumfield
        | enumfield
        ;

enumfield
        : NAME ';'
                {
                  ps.enumfield = (EnumFieldElement *)createMsgElementWithTag(np, MSG_ENUM_FIELD, ps.enump);
                  ps.enumfield->setName(toString(np, @1));
                  storeBannerAndRightComments(np, ps.enumfield,@1,@1);
                }
        | NAME '=' enumvalue ';'
                {
                  ps.enumfield = (EnumFieldElement *)createMsgElementWithTag(np, MSG_ENUM_FIELD, ps.enump);
                  ps.enumfield->setName(toString(np, @1));
                  ps.enumfield->setValue(toString(np, @3));
                  storeBannerAndRightComments(np, ps.enumfield,@1,@3);
                }
        ;

/*
 * Message, class and struct definitions
 */
message
        : message_header body
                { storeTrailingComment(np, ps.messagep,@$); }
        ;

packet
        : packet_header body
                { storeTrailingComment(np, ps.packetp,@$); }
        ;

class
        : class_header body
                { storeTrailingComment(np, ps.classp,@$); }
        ;

struct
        : struct_header body
                { storeTrailingComment(np, ps.structp,@$); }
        ;

message_header
        : MESSAGE qname '{'
                {
                  ps.msgclassorstruct = ps.messagep = (MessageElement *)createMsgElementWithTag(np, MSG_MESSAGE, ps.msgfile );
                  ps.messagep->setName(removeSpaces(np, @2).c_str());
                  storeBannerAndRightComments(np, ps.messagep,@1,@2);
                }
        | MESSAGE qname EXTENDS qname '{'
                {
                  ps.msgclassorstruct = ps.messagep = (MessageElement *)createMsgElementWithTag(np, MSG_MESSAGE, ps.msgfile );
                  ps.messagep->setName(removeSpaces(np, @2).c_str());
                  ps.messagep->setExtendsName(removeSpaces(np, @4).c_str());
                  storeBannerAndRightComments(np, ps.messagep,@1,@4);
                }
        ;

packet_header
        : PACKET qname '{'
                {
                  ps.msgclassorstruct = ps.packetp = (PacketElement *)createMsgElementWithTag(np, MSG_PACKET, ps.msgfile );
                  ps.packetp->setName(removeSpaces(np, @2).c_str());
                  storeBannerAndRightComments(np, ps.packetp,@1,@2);
                }
        | PACKET qname EXTENDS qname '{'
                {
                  ps.msgclassorstruct = ps.packetp = (PacketElement *)createMsgElementWithTag(np, MSG_PACKET, ps.msgfile );
                  ps.packetp->setName(removeSpaces(np, @2).c_str());
                  ps.packetp->setExtendsName(removeSpaces(np, @4).c_str());
                  storeBannerAndRightComments(np, ps.packetp,@1,@4);
                }
        ;

class_header
        : CLASS qname '{'
                {
                  ps.msgclassorstruct = ps.classp = (ClassElement *)createMsgElementWithTag(np, MSG_CLASS, ps.msgfile );
                  ps.classp->setName(removeSpaces(np, @2).c_str());
                  storeBannerAndRightComments(np, ps.classp,@1,@2);
                }
        | CLASS qname EXTENDS qname '{'
                {
                  ps.msgclassorstruct = ps.classp = (ClassElement *)createMsgElementWithTag(np, MSG_CLASS, ps.msgfile );
                  ps.classp->setName(removeSpaces(np, @2).c_str());
                  ps.classp->setExtendsName(removeSpaces(np, @4).c_str());
                  storeBannerAndRightComments(np, ps.classp,@1,@4);
                }
        ;

struct_header
        : STRUCT qname '{'
                {
                  ps.msgclassorstruct = ps.structp = (StructElement *)createMsgElementWithTag(np, MSG_STRUCT, ps.msgfile );
                  ps.structp->setName(removeSpaces(np, @2).c_str());
                  storeBannerAndRightComments(np, ps.structp,@1,@2);
                }
        | STRUCT qname EXTENDS qname '{'
                {
                  ps.msgclassorstruct = ps.structp = (StructElement *)createMsgElementWithTag(np, MSG_STRUCT, ps.msgfile );
                  ps.structp->setName(removeSpaces(np, @2).c_str());
                  ps.structp->setExtendsName(removeSpaces(np, @4).c_str());
                  storeBannerAndRightComments(np, ps.structp,@1,@4);
                }
        ;

body
        : opt_fields_and_properties
          '}' opt_semicolon
                { ps.msgclassorstruct = nullptr; }
        ;

opt_fields_and_properties
        : fields_and_properties
        | %empty
        ;

fields_and_properties
        : fields_and_properties field
        | fields_and_properties property
        | field
        | property
        ;

/*
 * Field
 */
field
        :  fieldtypename opt_fieldvector opt_inline_properties ';'
                {
                  storeBannerAndRightComments(np, ps.field,@1,@4);
                  ps.field = nullptr; // important! see addProperty() calls
                }
        |  fieldtypename opt_fieldvector opt_inline_properties '=' fieldvalue opt_inline_properties ';'
                {
                  ps.field->setDefaultValue(toString(np, @5));
                  storeBannerAndRightComments(np, ps.field,@1,@7);
                  ps.field = nullptr; // important! see addProperty() calls
                }
        ;

fieldtypename
        : fieldmodifiers fielddatatype NAME
                {
                  ps.field = (FieldElement *)createMsgElementWithTag(np, MSG_FIELD, ps.msgclassorstruct);
                  ps.field->setName(toString(np, @3));
                  ps.field->setDataType(ps.dataType.c_str());
                  ps.field->setIsAbstract(ps.isAbstract);
                  ps.field->setIsConst(ps.isConst);
                  ps.field->setIsPointer(ps.isPointer);
                }
        | fieldmodifiers NAME
                {
                  ps.field = (FieldElement *)createMsgElementWithTag(np, MSG_FIELD, ps.msgclassorstruct);
                  ps.field->setName(toString(np, @2));
                  ps.field->setIsAbstract(ps.isAbstract);
                }
        ;

fieldmodifiers
        : ABSTRACT
                { ps.isAbstract = true; }
        | %empty
                { ps.isAbstract = false; }
        ;

fielddatatype
        : fieldsimpledatatype
                { ps.isConst = false; ps.isPointer = false; ps.dataType = toString(np, @1); }
        | fieldsimpledatatype '*'
                { ps.isConst = false; ps.isPointer = true; ps.dataType = toString(np, @1); }
        | CONST_ fieldsimpledatatype
                { ps.isConst = true; ps.isPointer = false; ps.dataType = toString(np, @2);}
        | CONST_ fieldsimpledatatype '*'
                { ps.isConst = true; ps.isPointer = true; ps.dataType = toString(np, @2);}
        ;

fieldsimpledatatype
        : qname

        | CHAR_
        | SHORT_
        | INT_
        | LONG_

        | UNSIGNED_ CHAR_
        | UNSIGNED_ SHORT_
        | UNSIGNED_ INT_
        | UNSIGNED_ LONG_

        | DOUBLE_
        | STRING_
        | BOOL_
        ;


opt_fieldvector
        : '[' INTCONSTANT ']'
                {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(np, @2));
                }
        | '[' qname ']'
                {
                  ps.field->setIsVector(true);
                  ps.field->setVectorSize(toString(np, @2));
                }
        | '[' ']'
                {
                  ps.field->setIsVector(true);
                }
        | %empty
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
        | TRUE_
        | FALSE_
        | NAME
        | DOUBLECOLON
        | '?' | ':' | AND | OR | XOR | EQ | NE | '>' | GE | '<' | LE
        | '&' | '|' | '#' | SHIFT_LEFT | SHIFT_RIGHT
        | '+' | '-' | '*' | '/' | '%' | '^' | UMIN | '!' | '~'
        | '.' | ',' | '(' | ')' | '[' | ']'
        ;

enumvalue
        : INTCONSTANT
        | '-' INTCONSTANT
        | NAME
        ;

opt_inline_properties
        : inline_properties
        | %empty
        ;

inline_properties
        : inline_properties property_namevalue
        | property_namevalue
        ;

/*
 * Property (code nearly same as in ned2.y)
 */
property
        : property_namevalue ';'
                {
                  storePos(np,  ps.property, @$);
                  storeBannerAndRightComments(np, ps.property,@$);
                }
        ;

property_namevalue
        : property_name
        | property_name '(' opt_property_keys ')'
        | ENUM '(' NAME ')' /* legacy syntax */
                {
                  ASTNode *propertyscope = ps.field ? ps.field : ps.msgclassorstruct ? ps.msgclassorstruct : ps.msgfile;
                  ps.property = addProperty(np, propertyscope, toString(np, @1));
                  ps.propkey = (PropertyKeyElement *)createMsgElementWithTag(np, MSG_PROPERTY_KEY, ps.property);
                  ps.propkey->appendChild(createPropertyValue(np, @3));
                  storePos(np,  ps.propkey, @3);
                }
        ;

property_name
        : '@' PROPNAME
                {
                  ASTNode *propertyscope = ps.field ? ps.field : ps.msgclassorstruct ? ps.msgclassorstruct : ps.msgfile;
                  ps.property = addProperty(np, propertyscope, toString(np, @2));
                  ps.propvals.clear(); // just to be safe
                }
        | '@' PROPNAME '[' PROPNAME ']'
                {
                  ASTNode *propertyscope = ps.field ? ps.field : ps.msgclassorstruct ? ps.msgclassorstruct : ps.msgfile;
                  ps.property = addProperty(np, propertyscope, toString(np, @2));
                  ps.property->setIndex(toString(np, @4));
                  ps.propvals.clear(); // just to be safe
                }
        ;

opt_property_keys
        : property_keys  /* can't allow epsilon rule here, because @foo() would result in "ambiguous syntax" :( */
        ;

property_keys
        : property_keys ';' property_key
        | property_key
        ;

property_key
        : property_literal '=' property_values
                {
                  ps.propkey = (PropertyKeyElement *)createMsgElementWithTag(np, MSG_PROPERTY_KEY, ps.property);
                  ps.propkey->setName(opp_trim(toString(np, @1)).c_str());
                  for (int i=0; i<(int)ps.propvals.size(); i++)
                      ps.propkey->appendChild(ps.propvals[i]);
                  ps.propvals.clear();
                  storePos(np,  ps.propkey, @$);
                }
        | property_values
                {
                  ps.propkey = (PropertyKeyElement *)createMsgElementWithTag(np, MSG_PROPERTY_KEY, ps.property);
                  ps.propkey->appendChild($1);
                  for (int i=0; i<(int)ps.propvals.size(); i++)
                      ps.propkey->appendChild(ps.propvals[i]);
                  ps.propvals.clear();
                  storePos(np,  ps.propkey, @$);
                }
        ;

property_values
        : property_values ',' property_value
                { ps.propvals.push_back($3); }
        | property_value
                { ps.propvals.push_back($1); }
        ;

property_value
        : property_literal
                {
                  $$ = createPropertyValue(np, @$);
                }
        | %empty
                {
                  LiteralElement *node = (LiteralElement *)createMsgElementWithTag(np, MSG_LITERAL);
                  node->setType(LIT_SPEC); // and leave both value and text at ""
                  $$ = node;
                }
        ;

property_literal
        : property_literal COMMONCHAR
        | property_literal STRINGCONSTANT
        | COMMONCHAR
        | STRINGCONSTANT
        ;

opt_semicolon
        : ';'
        | %empty
        ;

%%

//----------------------------------------------------------------------
// general bison/flex stuff:
//
int msg2yylex_destroy();  // from lex.XXX.cc file

ASTNode *doParseMsg(ParseContext *np, const char *msgtext)
{
#if YYDEBUG != 0      /* #if added --VA */
    yydebug = YYDEBUGGING_ON;
#endif

    msg2yylex_destroy();

    DETECT_PARSER_REENTRY();

    // reset the lexer
    pos.co = 0;
    pos.li = 1;
    prevpos = pos;

    yyin = nullptr;
    yyout = stderr; // not used anyway

    // alloc buffer
    struct yy_buffer_state *handle = yy_scan_string(msgtext);
    if (!handle)
        {np->getErrors()->addError("", "unable to allocate work memory"); return nullptr;}

    // create parser state and NedFileElement
    resetParserState();
    ps.msgfile = new MsgFileElement();

    // store file name with slashes always, even on Windows -- neddoc relies on that
    ps.msgfile->setFilename(slashifyFilename(np->getFileName()).c_str());

    // store file comment
    storeFileComment(np, ps.msgfile);

    if (np->getStoreSourceFlag())
        storeSourceCode(np, ps.msgfile, np->getSource()->getFullTextPos());

    // parse
    try
    {
        yyparse(np);
    }
    catch (NedException& e)
    {
        yyerror(np, (std::string("error during parsing: ")+e.what()).c_str());
        yy_delete_buffer(handle);
        return 0;
    }

    yy_delete_buffer(handle);

    //FIXME TODO: fill in @documentation properties from comments
    return ps.msgfile;
}

void yyerror(ParseContext *np, const char *s)
{
    // chop newline
    char buf[250];
    strcpy(buf, s);
    if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

    np->error(buf, pos.li);
}
