%module LayoutEngine

%include "enumtypeunsafe.swg"
%javaconst(1);

%{
#include "geometry.h"
#include "forcedirectedparametersbase.h"
#include "forcedirectedparameters.h"
#include "forcedirectedembedding.h"
%}

%exception {
    try {
        $action
    } catch (std::exception& e) {
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

%typemap(javacode) Variable %{
    protected Variable disown() {
		swigCMemOwn = false;
	    return this;
    }

    public boolean equals(Object obj) {
        return (obj instanceof Variable) && getCPtr(this)==getCPtr((Variable)obj);
    }
    public int hashCode() {
        return (int)getCPtr(this);
    }
%}

%typemap(javain) Variable * "Variable.getCPtr($javainput.disown())";

%typemap(javain) IBody * "IBody.getCPtr($javainput.disown())";

%typemap(javaimports) IBody %{
import java.lang.reflect.Constructor;
%}

%typemap(javaout) IBody * {
   return IBody.newIBody($jnicall, $owner);
}

%typemap(javacode) IBody %{
   protected IBody disown() {
      swigCMemOwn = false;
      return this;
   }

   private static java.util.ArrayList<Constructor> bodyConstructors = new java.util.ArrayList<Constructor>();

   public static IBody newIBody(long cPtr, boolean isOwner) {
	  try {
         if (cPtr == 0)
            return null;

         String className = CommonEngineJNI.IBody_getClassName(cPtr);
         Constructor constructor = null;
         for (int i = 0; i < bodyConstructors.size(); i++)
            if (bodyConstructors.get(i).getName().equals(className))
               constructor = bodyConstructors.get(i);

         if (constructor == null)
         {
            String name = "org.omnetpp.common.engine." + className;
            Class clazz = Class.forName(name);
            constructor = clazz.getDeclaredConstructor(long.class, boolean.class);
            bodyConstructors.add(constructor);
         }

         return (IBody)constructor.newInstance(cPtr, isOwner);
      }
      catch (Exception e) {
         throw new RuntimeException(e);
      }
   }
%}

%typemap(javain) IForceProvider * "IForceProvider.getCPtr($javainput.disown())";

%typemap(javaimports) IForceProvider %{
import java.lang.reflect.Constructor;
%}

%typemap(javaout) IForceProvider * {
   return IForceProvider.newIForceProvider($jnicall, $owner);
}

%typemap(javacode) IForceProvider %{
   protected IForceProvider disown() {
      swigCMemOwn = false;
      return this;
   }

   private static java.util.ArrayList<Constructor> forceProviderConstructors = new java.util.ArrayList<Constructor>();

   public static IForceProvider newIForceProvider(long cPtr, boolean isOwner) {
	  try {
         if (cPtr == 0)
            return null;

         String className = CommonEngineJNI.IForceProvider_getClassName(cPtr);
         Constructor constructor = null;
         for (int i = 0; i < forceProviderConstructors.size(); i++)
            if (forceProviderConstructors.get(i).getName().equals(className))
               constructor = forceProviderConstructors.get(i);

         if (constructor == null)
         {
            String name = "org.omnetpp.common.engine." + className;
            Class clazz = Class.forName(name);
            constructor = clazz.getDeclaredConstructor(long.class, boolean.class);
            forceProviderConstructors.add(constructor);
         }

         return (IForceProvider)constructor.newInstance(cPtr, isOwner);
      }
      catch (Exception e) {
         throw new RuntimeException(e);
      }
   }
%}

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
%ignore ForceDirectedEmbedding::parameters;

%include "geometry.h"
%include "forcedirectedparametersbase.h"
%include "forcedirectedparameters.h"
%include "forcedirectedembedding.h"
