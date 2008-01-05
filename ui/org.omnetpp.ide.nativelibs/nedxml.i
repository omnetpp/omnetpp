%module nedxml

%include "loadlib.i"
%include "std_string.i"
%include "enums.swg"

%javaconst(1);

%{
#include "nedelement.h"
#include "nederror.h"
#include "nedparser.h"
#include "ned2generator.h"
#include "ned1generator.h"
#include "nedelements.h"

#include "neddtdvalidator.h"
#include "nedtools.h"
#include "nedsyntaxvalidator.h"

// #include "nedcompiler.h"
#include "nedfilebuffer.h"
// #include "nedsemanticvalidator.h"
// #include "cppgenerator.h"
%}

// hide export/import macros from swig
#define COMMON_API
#define NEDXML_API
#define OPP_DLLEXPORT
#define OPP_DLLIMPORT

namespace std {
//class ostream;
};

//XXX need %exception?


// parse functions return new objects, supposed to be deleted from Java
%newobject NEDParser::parseNEDText(const char *);
%newobject NEDParser::parseNEDFile(const char *);
%newobject NEDParser::parseNEDExpression(const char *);

// These are only public for technical reasons, shouldn't be wrapped
%ignore NEDParser::getSource();
%ignore NEDParser::getErrors();
%ignore NEDParser::error(const char *, int);

%ignore np;

%ignore NEDElementUserData;
%ignore NEDElement::setUserData(NEDElementUserData *);
%ignore NEDElement::getUserData() const;

%ignore generateNED1(std::ostream&, NEDElement *, NEDErrorStore *);
%ignore generateNED2(std::ostream&, NEDElement *, NEDErrorStore *);
%ignore NEDInternalError;

// XXX for some reason, SWIG doesn't give a s&%$# about the following ignores:
%ignore NEDElementStore::add(NEDElement *, const char *, ...);
%ignore NEDElementStore::add(NEDElement *, int, const char *, ...);
%ignore NEDGenerator1::generate(std::ostream&, NEDElement *, const char *);
%ignore NEDGenerator2::generate(std::ostream&, NEDElement *, const char *);

%ignore ltstr;

//
// The following code is from "19.9.7 Adding Java downcasts to polymorphic return types"
// in the SWIG Manual
// XXX probably not needed
//
%typemap(jni) NEDElement *NEDElement::getParent "jobject"
%typemap(jtype) NEDElement *NEDElement::getParent "NEDElement"
%typemap(jstype) NEDElement *NEDElement::getParent "NEDElement"
%typemap(javaout) NEDElement *NEDElement::getParent {
    return $jnicall;
}

/*
%typemap(out) NEDElement * {
        // manually find the Java class of the same name as the C++ class, and
        // call its (long cPtr, boolean cMemoryOwn) constructor
        jclass clazz = jenv->FindClass($1->className());
        if (clazz) {
            jmethodID mid = jenv->GetMethodID(clazz, "<init>", "(JZ)V");
            if (mid) {
                jlong cptr = 0;
                *(NEDElement **)&cptr = $1; // this assumes that dynamic_cast<> doesn't change the pointer value!
                $result = jenv->NewObject(clazz, mid, cptr, false);
            }
        }
        else {
            fprintf(stdout, "Failed to create java proxy object: no such class: %s\n", $1->className());
        }
}
*/

/* FIXME find a better way... */
%ignore  FilesNode;
%ignore  NedFileNode;
%ignore  CommentNode;
%ignore  ImportNode;
%ignore  PropertyDeclNode;
%ignore  ExtendsNode;
%ignore  InterfaceNameNode;
%ignore  SimpleModuleNode;
%ignore  ModuleInterfaceNode;
%ignore  CompoundModuleNode;
%ignore  ChannelInterfaceNode;
%ignore  ChannelNode;
%ignore  ParametersNode;
%ignore  ParamGroupNode;
%ignore  ParamNode;
%ignore  PatternNode;
%ignore  PropertyNode;
%ignore  PropertyKeyNode;
%ignore  GatesNode;
%ignore  GateGroupNode;
%ignore  GateNode;
%ignore  TypesNode;
%ignore  SubmodulesNode;
%ignore  SubmoduleNode;
%ignore  ConnectionsNode;
%ignore  ConnectionNode;
%ignore  ChannelSpecNode;
%ignore  ConnectionGroupNode;
%ignore  LoopNode;
%ignore  ConditionNode;
%ignore  ExpressionNode;
%ignore  OperatorNode;
%ignore  FunctionNode;
%ignore  IdentNode;
%ignore  LiteralNode;
%ignore  MsgFileNode;
%ignore  CplusplusNode;
%ignore  StructDeclNode;
%ignore  ClassDeclNode;
%ignore  MessageDeclNode;
%ignore  EnumDeclNode;
%ignore  EnumNode;
%ignore  EnumFieldsNode;
%ignore  EnumFieldNode;
%ignore  MessageNode;
%ignore  ClassNode;
%ignore  StructNode;
%ignore  FieldsNode;
%ignore  FieldNode;
%ignore  PropertiesNode;
%ignore  MsgpropertyNode;
%ignore  UnknownNode;


/* Let's just grab the original header file here */
%include "nedxmldefs.h"
%include "nedelement.h"
%include "nederror.h"
%include "nedparser.h"
%include "ned2generator.h"
%include "ned1generator.h"
%include "nedelements.h"

%include "nedvalidator.h"
%include "neddtdvalidatorbase.h"
%include "neddtdvalidator.h"
%include "nedtools.h"
%include "nedsyntaxvalidator.h"

//%include "nedcompiler.h"
//%include "nedfilebuffer.h"
//%include "nedsemanticvalidator.h"
//%include "cppgenerator.h"

