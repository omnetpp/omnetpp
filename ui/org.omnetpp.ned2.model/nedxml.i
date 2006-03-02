%module nedxml

%{
#include "nedelement.h"
#include "nedparser.h"
#include "nedgenerator.h"
#include "nedelements.h"

#include "neddtdvalidator.h"
// #include "nedbasicvalidator.h"

// #include "nedcompiler.h"
#include "nedfilebuffer.h"
// #include "nedsemanticvalidator.h"
// #include "cppgenerator.h"
%}

%include "std_string.i"

%include "enums.swg"
%javaconst(1);


%pragma(java) jniclasscode=%{
  static {
    try {
      System.loadLibrary("nedxml_engine");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. \n" + e);
      System.exit(1);
    }
  }
%}

// These are only public for technical reasons, shouldn't be wrapped
%ignore NEDParser::nedsource;
%ignore NEDParser::error(const char *, int);
%ignore np;

%ignore NEDElementUserData;
%ignore NEDElement::setUserData(NEDElementUserData *);
%ignore NEDElement::getUserData() const;

%ignore generateNed(ostream&, NEDElement *);
%ignore NEDGenerator::generate(ostream&, NEDElement *, const char *);

%ignore ltstr;

//
// The following code is from "19.9.7 Adding Java downcasts to polymorphic return types"
// in the SWIG Manual
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
%ignore  WhitespaceNode;
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
%ignore  WhereNode;
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
%include "nedelement.h"
%include "nedparser.h"
%include "nedgenerator.h"
%include "nedelements.h"

%include "nedvalidator.h"
%include "neddtdvalidatorbase.h"
%include "neddtdvalidator.h"
//%include "nedbasicvalidator.h"

//%include "nedcompiler.h"
//%include "nedfilebuffer.h"
//%include "nedsemanticvalidator.h"
//%include "cppgenerator.h"

