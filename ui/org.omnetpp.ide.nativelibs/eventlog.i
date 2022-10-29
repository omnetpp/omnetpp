%module EventLogEngine

// covariant return type warning disabled
#pragma SWIG nowarn=822

%include "loadlib.i"
%include "enumtypeunsafe.swg"
%include "defs.i"
%include "stacktrace.i"
%include "std_string.i"
%javaconst(1);

%include "bigdecimal.i"

%{
#include "omnetpp/platdep/platmisc.h"
#include "common/filereader.h"
#include "common/exprvalue.h"

using namespace omnetpp::common;
using namespace omnetpp::common::expression;
%}

%typemap(javacode) omnetpp::common::FileReader %{
    public FileReader(String fileName, boolean cMemoryOwn) {
        this(fileName);
        this.swigCMemOwn = cMemoryOwn;
    }

    public FileReader(String fileName, long bufferSize, boolean cMemoryOwn) {
        this(fileName, bufferSize);
        this.swigCMemOwn = cMemoryOwn;
    }
%}

%include "common/filereader.h"

namespace omnetpp { namespace eventlog {

typedef omnetpp::common::BigDecimal BigDecimal;
typedef omnetpp::common::FileReader FileReader;
typedef omnetpp::common::opp_runtime_error opp_runtime_error;
typedef omnetpp::common::BigDecimal BigDecimal;

typedef int64_t eventnumber_t;
typedef BigDecimal simtime_t;
#define simtime_nil BigDecimal::MinusOne


} } // namespaces

/*--------------------------------------------------------------------------
 * ptr_t <--> long mapping
 *--------------------------------------------------------------------------*/

%typemap(jni)    uintptr_t "jlong"
%typemap(jtype)  uintptr_t "long"
%typemap(jstype) uintptr_t "long"
%typemap(javain) uintptr_t "$javainput"
%typemap(javaout) uintptr_t { return $jnicall; }
%typemap(out) uintptr_t { $result = (jlong)$1; }
%typemap(in) uintptr_t { $1 = (uintptr_t)$input; }

%typemap(jni)    file_offset_t "jlong"
%typemap(jtype)  file_offset_t "long"
%typemap(jstype) file_offset_t "long"
%typemap(javain) file_offset_t "$javainput"
%typemap(javaout) file_offset_t { return $jnicall; }
%typemap(out) file_offset_t { $result = (jlong)$1; }
%typemap(in) file_offset_t { $1 = (file_offset_t)$input; }

%include "std_common.i"
%include "std_string.i"
%include "std_set.i"     // our custom version
%include "std_list.i"    // our custom version
%include "std_vector.i"
%include "std_map.i"

%include "map_oldapi.i" // needed for SWIG >=4.0 so we still have the 3.x functions

namespace std {

   %typemap(javacode) vector<string> %{
       public String[] toArray() {
           int sz = (int) size();
           String[] array = new String[sz];
              for (int i=0; i<sz; i++)
               array[i] = get(i);
           return array;
       }
       public static StringVector fromArray(String[] array) {
           StringVector vector = new StringVector();
           for (int i=0; i<array.length; i++)
               vector.add(array[i]);
           return vector;
       }
   %}

   %typemap(javacode) set<int> %{
       public IntSet(boolean cMemoryOwn) {
           this(EventLogEngineJNI.new_IntSet__SWIG_0(), cMemoryOwn);
       }
   %}

   %extend set<const char *> {
       std::vector<const char *> keys() {
           std::vector<const char *> vec;
           vec.reserve(self->size());
           for (std::set<const char *>::iterator it = self->begin(); it!=self->end(); it++)
               vec.push_back(*it);
           return vec;
       }
   }

   %extend set<string> {
       std::vector<std::string> keys() {
           std::vector<std::string> vec;
           vec.reserve(self->size());
           for (std::set<std::string>::iterator it = self->begin(); it!=self->end(); it++)
               vec.push_back(*it);
           return vec;
       }
   }

   %extend map<string,string> {
       std::vector<std::string> keys() {
           std::vector<std::string> vec;
           vec.reserve(self->size());
           for (std::map<std::string,std::string>::iterator it = self->begin(); it!=self->end(); it++)
               vec.push_back(it->first);
           return vec;
       }
   }

   %template(PStringSet) set<const char *>;
   %template(StringSet) set<string>;
   %template(StringVector) vector<string>;
   %template(PStringVector) vector<const char *>;

   //specialize_std_map_on_both(string,,,,string,,,);
   //%template(StringMap) map<string,string>;

   %template(IntSet) set<int>;

   specialize_std_map_on_both(int,,,,int,,,);
   %template(IntIntMap) map<int,int>;

   %template(IntVector) vector<int>;
   %template(LongVector) vector<long>;
   %template(Int64Vector) vector<int64_t>;
};

namespace omnetpp { namespace eventlog {

%define FIX_STRING_MEMBER(STRUCT,MEMBER,CAPITALIZEDMEMBER)
%ignore STRUCT::MEMBER;
%extend STRUCT {
   std::string get ## CAPITALIZEDMEMBER() {return self->MEMBER;}
   void set ## CAPITALIZEDMEMBER(std::string __a) {self->MEMBER = __a;}
}
%enddef

%define FIX_CHARPTR_MEMBER(STRUCT,MEMBER,CAPITALIZEDMEMBER)
%ignore STRUCT::MEMBER;
%extend STRUCT {
   const char * get ## CAPITALIZEDMEMBER() {return self->MEMBER;}
}
%enddef

%{
   JNIEnv *progressDelegateJenv;

   std::string fromJavaString(JNIEnv *jenv, jstring stringObject)
   {
      if (!stringObject)
        return "<null>";
      jboolean isCopy;
      const char *buf = jenv->GetStringUTFChars(stringObject, &isCopy);
      std::string str = buf ? buf : "";
      jenv->ReleaseStringUTFChars(stringObject, buf);
      return str;
   }

%}

%typemap(in) JNIEnv * {
  $1 = jenv;
}

} } // namespaces


%ignore FILE;
%ignore *::parse;
%ignore *::print(FILE *);
%ignore *::print(FILE *, bool);
%ignore *::print(FILE *, long);
%ignore *::print(FILE *, long, long);
%ignore *::print(FILE *, long, long, bool);
%ignore *::print(FILE *, long, long, bool, bool);
%ignore *::printInitializationLogEntries(FILE *);
%ignore *::printEvents(FILE *);
%ignore *::printEvents(FILE *, long);
%ignore *::printEvents(FILE *, long, long);
%ignore *::printEvents(FILE *, long, long, bool);
%ignore *::printCause(FILE *);
%ignore *::printConsequence(FILE *);
%ignore *::printMiddle(FILE *);

namespace omnetpp { namespace eventlog {

} } // namespaces

%include "omnetpp/platdep/platmisc.h"
%include "common/filereader.h"
