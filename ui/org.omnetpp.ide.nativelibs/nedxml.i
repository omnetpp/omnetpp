%module nedxml

// covariant return type warning disabled
#pragma SWIG nowarn=822

%include "loadlib.i"
%include "std_string.i"
%include "enums.swg"
%include "defs.i"

%javaconst(1);

%{
#include "nedxml/astnode.h"
#include "nedxml/errorstore.h"
#include "nedxml/nedparser.h"
#include "nedxml/msgparser.h"
#include "nedxml/nedgenerator.h"
#include "nedxml/msggenerator.h"
#include "nedxml/xmlgenerator.h"
#include "nedxml/nedelements.h"
#include "nedxml/msgelements.h"
#include "nedxml/neddtdvalidator.h"
#include "nedxml/msgdtdvalidator.h"
#include "nedxml/nedtools.h"
#include "nedxml/nedsyntaxvalidator.h"
#include "nedxml/sourcedocument.h"

using namespace omnetpp::nedxml;
%}

// hide export/import macros from swig
#define COMMON_API
#define NEDXML_API
#define OPP_DLLEXPORT
#define OPP_DLLIMPORT

namespace std {
//class ostream;
};

namespace omnetpp { namespace nedxml {

// parse functions return new objects, supposed to be deleted from Java
%newobject NedParser::parseNedText(const char *);
%newobject NedParser::parseNedFile(const char *);
%newobject NedParser::parseNedExpression(const char *);
%newobject createElementWithTag(int tagcode, ASTNode *parent);

// These are only public for technical reasons, shouldn't be wrapped
%ignore NedParser::getSource();
%ignore NedParser::getErrors();
%ignore NedParser::error(const char *, int);

%ignore np;

%ignore UserData;
%ignore ASTNode::setUserData(UserData *);
%ignore ASTNode::getUserData() const;

%ignore generateNed(std::ostream&, ASTNode *);
%ignore generateXML(std::ostream&, ASTNode *, bool, int);
%ignore NedInternalError;

%ignore NedTools::splitToFiles;

%ignore NamespaceElement::getFirstCommentChild;
%ignore PackageElement::getFirstCommentChild;
%ignore PacketDeclElement::getFirstCommentChild;
%ignore PacketElement::getFirstCommentChild;
%ignore PacketElement::getFirstPropertyChild;
%ignore PacketElement::getFirstFieldChild;


// XXX for some reason, SWIG doesn't give a s&%$# about the following ignores:
%ignore NedElementStore::add(ASTNode *, const char *, ...);
%ignore NedElementStore::add(ASTNode *, int, const char *, ...);
%ignore NedGenerator::generate(std::ostream&, ASTNode *, const char *);
%ignore MsgGenerator::generate(std::ostream&, ASTNode *, const char *);
%ignore XMLGenerator::generate(std::ostream&, ASTNode *);

%ignore ltstr;

%ignore  FilesElement;
%ignore  NedFileElement;
%ignore  CommentElement;
%ignore  PackageElement;
%ignore  ImportElement;
%ignore  PropertyDeclElement;
%ignore  ExtendsElement;
%ignore  InterfaceNameElement;
%ignore  SimpleModuleElement;
%ignore  ModuleInterfaceElement;
%ignore  CompoundModuleElement;
%ignore  ChannelInterfaceElement;
%ignore  ChannelElement;
%ignore  ParametersElement;
%ignore  ParamGroupElement;
%ignore  ParamElement;
%ignore  PatternElement;
%ignore  PropertyElement;
%ignore  PropertyKeyElement;
%ignore  GatesElement;
%ignore  GateGroupElement;
%ignore  GateElement;
%ignore  TypesElement;
%ignore  SubmodulesElement;
%ignore  SubmoduleElement;
%ignore  ConnectionsElement;
%ignore  ConnectionElement;
%ignore  ChannelSpecElement;
%ignore  ConnectionGroupElement;
%ignore  LoopElement;
%ignore  ConditionElement;
%ignore  ExpressionElement;
%ignore  OperatorElement;
%ignore  FunctionElement;
%ignore  IdentElement;
%ignore  LiteralElement;
%ignore  MsgFileElement;
%ignore  NamespaceElement;
%ignore  CplusplusElement;
%ignore  StructDeclElement;
%ignore  ClassDeclElement;
%ignore  MessageDeclElement;
%ignore  PacketDeclElement;
%ignore  EnumDeclElement;
%ignore  EnumElement;
%ignore  EnumFieldsElement;
%ignore  EnumFieldElement;
%ignore  MessageElement;
%ignore  PacketElement;
%ignore  ClassElement;
%ignore  StructElement;
%ignore  FieldsElement;
%ignore  FieldElement;
%ignore  PropertiesElement;
%ignore  MsgpropertyElement;
%ignore  UnknownElement;

} } // namespaces

/* Let's just grab the original header file here */
%include "nedxml/nedxmldefs.h"
%include "nedxml/astnode.h"
%include "nedxml/errorstore.h"
%include "nedxml/nedparser.h"
%include "nedxml/msgparser.h"
%include "nedxml/nedgenerator.h"
%include "nedxml/msggenerator.h"
%include "nedxml/xmlgenerator.h"
%include "nedxml/nedelements.h"
%include "nedxml/msgelements.h"

%include "nedxml/nedvalidator.h"
%include "nedxml/msgvalidator.h"
%include "nedxml/neddtdvalidator.h"
%include "nedxml/msgdtdvalidator.h"
%include "nedxml/nedtools.h"
%include "nedxml/nedsyntaxvalidator.h"

