%module CommonEngine

%include "enumtypeunsafe.swg"
%javaconst(1);

%pragma(java) jniclasscode=%{
  static {
    try {
      System.load("C:\\Workspace\\Repository\\trunk\\omnetpp\\ui\\org.omnetpp.common.engine\\common_engine.dll");
      //System.loadLibrary("common_engine");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. \n" + e);
      System.exit(1);
    }
  }
%}

%{
#include "geometry.h"
#include "forcedirectedparametersbase.h"
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

%typemap(javacode) Variable %{
    public boolean equals(Object obj) {
        return (obj instanceof Variable) && getCPtr(this)==getCPtr((Variable)obj);
    }
    public int hashCode() {
        return (int)getCPtr(this);
    }
%}

%typemap(javacode) ForceDirectedEmbedding %{
    private java.util.ArrayList<Object> members = new java.util.ArrayList<Object>();

	public void addForceProvider(IForceProvider forceProvider) {
	    members.add(forceProvider);
		addForceProvider0(forceProvider);
	}

	public void addBody(IBody body) {
	    members.add(body);
	    members.add(body.getVariable());
		addBody0(body);
    }
%}

%typemap(javacode) Body %{
    private Variable variable;
%}

%rename ForceDirectedEmbedding::addBody addBody0;
%rename ForceDirectedEmbedding::addForceProvider addForceProvider0;

%ignore zero;
%ignore NaN;
%ignore isNaN;

%include "geometry.h"
%include "forcedirectedparametersbase.h"
%include "forcedirectedparameters.h"
%include "forcedirectedembedding.h"
