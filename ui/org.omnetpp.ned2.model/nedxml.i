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
%ignore NEDParser::tree;
%ignore NEDParser::nedsource;
%ignore np;

%ignore NEDElementUserData;
%ignore NEDElement::setUserData(NEDElementUserData *);
%ignore NEDElement::getUserData() const;

%ignore generateNed(ostream&, NEDElement *, bool);
%ignore NEDGenerator::generate(ostream&, NEDElement *, const char *);

%ignore ltstr;

//%ignore NEDDTDValidator::validateElement;
//%ignore NEDBasicValidator::validateElement;
//%ignore NEDSemanticValidator::validateElement;

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
%ignore ChannelAttrNode;
%ignore ChannelNode;
%ignore ClassDeclNode;
%ignore ClassNode;
%ignore CompoundModuleNode;
%ignore ConnAttrNode;
%ignore ConnectionNode;
%ignore ConnectionsNode;
%ignore ConstNode;
%ignore CplusplusNode;
%ignore DisplayStringNode;
%ignore EnumDeclNode;
%ignore EnumFieldNode;
%ignore EnumFieldsNode;
%ignore EnumNode;
%ignore ExpressionNode;
%ignore FieldNode;
%ignore FieldsNode;
%ignore ForLoopNode;
%ignore FunctionNode;
%ignore GateNode;
%ignore GatesizeNode;
%ignore GatesizesNode;
%ignore GatesNode;
%ignore IdentNode;
%ignore ImportedFileNode;
%ignore ImportNode;
%ignore LoopVarNode;
%ignore MachineNode;
%ignore MachinesNode;
%ignore MessageDeclNode;
%ignore MessageNode;
%ignore NedFileNode;
%ignore NedFilesNode;
%ignore NetworkNode;
%ignore OperatorNode;
%ignore ParamNode;
%ignore ParamRefNode;
%ignore ParamsNode;
%ignore PropertiesNode;
%ignore PropertyNode;
%ignore SimpleModuleNode;
%ignore StructDeclNode;
%ignore StructNode;
%ignore SubmoduleNode;
%ignore SubmodulesNode;
%ignore SubstmachineNode;
%ignore SubstmachinesNode;
%ignore SubstparamNode;
%ignore SubstparamsNode;
%ignore UnknownNode;


/* Let's just grab the original header file here */
%include "nedelement.h"
%include "nedparser.h"
%include "nedgenerator.h"
%include "nedelements.h"

%include "nedvalidator.h"
%include "neddtdvalidator.h"
%include "nedbasicvalidator.h"

%include "nedcompiler.h"
// %include "nedfilebuffer.h"
%include "nedsemanticvalidator.h"
//%include "cppgenerator.h"

