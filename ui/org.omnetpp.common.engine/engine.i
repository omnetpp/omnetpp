%module ScaveEngine

%include "enumtypeunsafe.swg"
%javaconst(1);

%pragma(java) jniclasscode=%{
  static {
    try {
      System.loadLibrary("common_engine");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. \n" + e);
      System.exit(1);
    }
  }
%}

%{
#include "geometry.h"
#include "forcedirectedparameters.h"
#include "forcedirectedembedding.h"
%}

%exception {
    try {
        $action
    } catch (Exception *e) {
        SWIG_exception(SWIG_RuntimeError,const_cast<char*>(e->message()));
    } catch (std::exception e) {
        SWIG_exception(SWIG_RuntimeError,const_cast<char*>(e.what()));
    }
}

%typemap(jni)    int64 "jlong"
%typemap(jtype)  int64 "long"
%typemap(jstype) int64 "long"
%typemap(javain) int64 "$javainput"

%include "std_common.i"
%include "std_string.i"
%include "std_vector.i"

namespace std {
   specialize_std_vector(Cc);
   %template(CcList) vector<Cc>;

   specialize_std_vector(IBody *);
   %template(IBodyList) vector<IBody *>;

   specialize_std_vector(IForceProvider *);
   %template(IForceProviderList) vector<IForceProvider *>;

   specialize_std_vector(Pt);
   %template(PtList) vector<Pt>;

   specialize_std_vector(Variable *);
   %template(VariableList) vector<Variable *>;
};

%ignore zero;
%ignore NaN;
%ignore isNaN;

%include "geometry.h"
%include "forcedirectedparameters.h"
%include "forcedirectedembedding.h"
