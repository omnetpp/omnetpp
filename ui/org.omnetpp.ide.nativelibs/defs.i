//
// Definitions shared by all modules.
//

%{
#include "jprogressmonitor.h"
%}

// Swig doesn't understand C++11 syntax
#define override
%ignore nullptr;
#define nullptr 0

%exception {
    try {
        $action
    } catch (std::exception& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, const_cast<char*>(e.what()));
        return $null;
    }
}

/*
 * Wrapping for [u]int(8|16|32)_t; note that int64_t needs to be handled separately, because 
 * its definition is platform-dependent.
 */
typedef signed char         int8_t;
typedef short               int16_t;
typedef int                 int32_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;

/*
 * Wrap int64_t and const int64_t into Java long.
 * Custom typemaps needed because Linux defines int64_t as long, and swig wraps long as Java int (which is 32 bits!!!)
 * Note: NO TYPEDEF! (it will cause problems in scave's IDVectorVector, due to int64_t being defined differently on different platforms)
 */
%typemap(in) int64_t = long;
%typemap(out) int64_t %{ $result = $1; %}
%typemap(javaout) int64_t = long;
%typemap(jni) int64_t  "jlong"
%typemap(jtype) int64_t  "long"
%typemap(jstype) int64_t  "long"
%typemap(javain) int64_t  "$javainput"
%typemap(freearg) int64_t  ""
%typemap(typecheck) int64_t  = long;

%typemap(in) const int64_t & = const long &;
%typemap(out) const int64_t & %{ $result = *$1; %}
%typemap(javaout) const int64_t & = long;
%typemap(jni) const int64_t &  "jlong"
%typemap(jtype) const int64_t &  "long"
%typemap(jstype) const int64_t &  "long"
%typemap(javain) const int64_t &  "$javainput"
%typemap(freearg) const int64_t &  ""
%typemap(typecheck) const int64_t &  = long;


/*
 * IProgressMonitor
 */
namespace omnetpp { namespace common {
class IProgressMonitor;
} }

%typemap(jni)    omnetpp::common::IProgressMonitor * "jobject"
%typemap(jtype)  omnetpp::common::IProgressMonitor * "org.eclipse.core.runtime.IProgressMonitor"
%typemap(jstype) omnetpp::common::IProgressMonitor * "org.eclipse.core.runtime.IProgressMonitor"
%typemap(javain) omnetpp::common::IProgressMonitor * "$javainput"
%typemap(in)     omnetpp::common::IProgressMonitor * (omnetpp::JniProgressMonitor jProgressMonitor) {
   if ($input)
   {
      jProgressMonitor = omnetpp::JniProgressMonitor($input, jenv);
      $1 = &jProgressMonitor;
   }
   else
   {
      $1 = NULL;
   }
}

/*
 * Macro to add equals() and hashCode() to generated Java classes
 */
%define ADD_CPTR_EQUALS_AND_HASHCODE(CLASS)
%typemap(javacode) CLASS %{
  public boolean equals(Object obj) {
    if (this == obj)
      return true;
    if (obj == null || this.getClass() != obj.getClass())
      return false;
    return getCPtr(this) == getCPtr((CLASS)obj);
  }

  public int hashCode() {
    return (int)getCPtr(this);
  }
%}
%enddef

/*
 * ILock
 */
%define USE_COMMON_ENGINE_ILOCK()
namespace omnetpp { namespace common {
class ILock;
%typemap(jstype) ILock "org.omnetpp.common.engine.ILock";
%typemap(javaout) ILock {
    long cPtr = $jnicall;
    return (cPtr == 0) ? null : new org.omnetpp.common.engine.ILock(cPtr, $owner);
}

%typemap(jstype) ILock* "org.omnetpp.common.engine.ILock";
%typemap(javaout) ILock* {
    long cPtr = $jnicall;
    return (cPtr == 0) ? null : new org.omnetpp.common.engine.ILock(cPtr, $owner);
}

%typemap(jstype) ILock& "org.omnetpp.common.engine.ILock";
%typemap(javaout) ILock& {
    long cPtr = $jnicall;
    return (cPtr == 0) ? null : new org.omnetpp.common.engine.ILock(cPtr, $owner);
}
} } // namespaces
%enddef
