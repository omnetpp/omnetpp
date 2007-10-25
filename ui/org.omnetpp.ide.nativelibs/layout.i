%module LayoutEngine

%include "loadlib.i"
%include "enumtypeunsafe.swg"
%javaconst(1);

%{
#include "graphlayouter.h"
#include "basicspringembedderlayout.h"
#include "forcedirectedgraphlayouter.h"
#include "geometry.h"
#include "forcedirectedparametersbase.h"
#include "forcedirectedparameters.h"
#include "forcedirectedembedding.h"
%}

%exception {
    try {
        $action
    } catch (std::exception& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, const_cast<char*>(e.what()));
        return $null;
    }
}

// hide export/import macros from swig
#define COMMON_API
#define LAYOUT_API
#define OPP_DLLEXPORT
#define OPP_DLLIMPORT

%typemap(jni)    int32 "jint"
%typemap(jtype)  int32 "int"
%typemap(jstype) int32 "int"
%typemap(javain) int32 "$javainput"

%typemap(jni)    int64 "jlong"
%typemap(jtype)  int64 "long"
%typemap(jstype) int64 "long"
%typemap(javain) int64 "$javainput"

%typemap(jni)    cModule* "jobject"
%typemap(jtype)  cModule* "Object"
%typemap(jstype) cModule* "Object"
%typemap(javain) cModule* "$javainput"

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

   @SuppressWarnings("unchecked")
   private static java.util.ArrayList<Constructor> bodyConstructors = new java.util.ArrayList<Constructor>();

   @SuppressWarnings("unchecked")
   public static IBody newIBody(long cPtr, boolean isOwner) {
      try {
         if (cPtr == 0)
            return null;

         String className = LayoutEngineJNI.IBody_getClassName(cPtr, null);
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

   @SuppressWarnings("unchecked")
   private static java.util.ArrayList<Constructor> forceProviderConstructors = new java.util.ArrayList<Constructor>();

   @SuppressWarnings("unchecked")
   public static IForceProvider newIForceProvider(long cPtr, boolean isOwner) {
      try {
         if (cPtr == 0)
            return null;

         // we implement polymorphic return types:
         String className = LayoutEngineJNI.IForceProvider_getClassName(cPtr, null);
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
   %template(CcList) vector<Cc>;
   %template(IBodyList) vector<IBody *>;
   %template(IForceProviderList) vector<IForceProvider *>;
   %template(PtList) vector<Pt>;
   %template(VariableList) vector<Variable *>;
};

%define FIXUP_GETNODEPOSITION(CLASS)
%ignore CLASS::getNodePosition;
%extend CLASS {
   int getNodePositionX(cModule *mod) {int x,y; self->getNodePosition(mod, x, y); return x;}
   int getNodePositionY(cModule *mod) {int x,y; self->getNodePosition(mod, x, y); return y;}
}
%enddef

FIXUP_GETNODEPOSITION(GraphLayouter);
FIXUP_GETNODEPOSITION(BasicSpringEmbedderLayout);
FIXUP_GETNODEPOSITION(ForceDirectedGraphLayouter);

/*
XXX to ignore:
  getDistanceAndVector()
  getStandardVerticalDistanceAndVector()
  getSlipperyDistanceAndVector()
*/

%ignore zero;
%ignore NaN;
%ignore POSITIVE_INFINITY;
%ignore isNaN;
%ignore ForceDirectedEmbedding::parameters;
%ignore ForceDirectedEmbedding::writeDebugInformation;
%ignore LeastExpandedSpring::LeastExpandedSpring;   //XXX takes std::vector<AbstractSpring*>

%include "graphlayouter.h"
%include "basicspringembedderlayout.h"
%include "forcedirectedgraphlayouter.h"
%include "geometry.h"
%include "forcedirectedparametersbase.h"
%include "forcedirectedparameters.h"
%include "forcedirectedembedding.h"

