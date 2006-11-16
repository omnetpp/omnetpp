%module ScaveEngine

%include "enumtypeunsafe.swg"
%javaconst(1);

%pragma(java) jniclasscode=%{
  static {
    try {
      System.loadLibrary("eventlog_engine");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. \n" + e);
      System.exit(1);
    }
  }
%}

%{
#include "ievent.h"
#include "ieventlog.h"
#include "event.h"
#include "filteredevent.h"
#include "messagedependency.h"
#include "eventlogentry.h"
#include "eventlogentries.h"
#include "eventlogindex.h"
#include "eventlog.h"
#include "eventlogfacade.h"
#include "eventlogtablefacade.h"
#include "sequencechartfacade.h"
#include "filteredeventlog.h"
#include "filereader.h"
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

%include "eventlogdefs.h"

%typemap(jni)    int64 "jlong"
%typemap(jtype)  int64 "long"
%typemap(jstype) int64 "long"
%typemap(javain) int64 "$javainput"

%include "std_common.i"
%include "std_string.i"
%include "std_set.i"     // our custom version
%include "std_list.i"    // our custom version
%include "std_vector.i"
%include "std_map.i"

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

   %typemap(javacode) vector<Run*> %{
        public Run[] toArray() {
            int sz = (int)size();
            Run[] array = new Run[sz];
            for (int i=0; i<sz; i++)
                array[i] = get(i);
            return array;
        }
   %}

   %typemap(javacode) vector<ResultFile*> %{
        public ResultFile[] toArray() {
            int sz = (int)size();
            ResultFile[] array = new ResultFile[sz];
            for (int i=0; i<sz; i++)
                array[i] = get(i);
            return array;
        }
   %}

   %typemap(javacode) vector<FileRun*> %{
        public FileRun[] toArray() {
            int sz = (int)size();
            FileRun[] array = new FileRun[sz];
            for (int i=0; i<sz; i++)
                array[i] = get(i);
            return array;
        }
   %}

   %typemap(javacode) vector<ID> %{
        public Long[] toArray() {
            int sz = (int)size();
            Long[] array = new Long[sz];
            for (int i=0; i<sz; i++)
                array[i] = Long.valueOf(get(i));
            return array;
        }
   %}

   %typemap(javacode) set<int> %{
       public IntSet(boolean cMemoryOwn) {
           this(ScaveEngineJNI.new_IntSet__SWIG_0(), cMemoryOwn);
       }
   %}

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

   %template(StringSet) set<string>;
   %template(StringVector) vector<string>;

   //specialize_std_map_on_both(string,,,,string,,,);

   //%template(StringMap) map<string,string>;

   specialize_std_vector(MessageDependency*);
   %template(MessageDependencyList) vector<MessageDependency*>;

   specialize_std_vector(FilteredMessageDependency*);
   %template(FilteredMessageDependencyList) vector<FilteredMessageDependency*>;

   specialize_std_vector(const char *);

   %template(PStringVector) vector<const char *>;

   %template(IntSet) set<int>;

   %template(LongSet) set<int64>;

   specialize_std_map_on_both(int,,,,int,,,);

   %template(IntIntMap) map<int,int>;

   %template(IntVector) vector<int>;
   %template(LongVector) vector<long>;

   specialize_std_vector(int64);
   %template(Int64Vector) vector<int64>;
};

/*
%ignore EventLog::writeTrace;

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
*/

%typemap(javaout) EventLogEntry * {
   return EventLogEntry.newEventLogEntry($jnicall, $owner);
}

%typemap(javaimports) EventLogEntry %{
import java.lang.reflect.Constructor;
%}

%typemap(javacode) EventLogEntry %{
   private static Constructor[] eventLogConstructors = new Constructor[100];

   public static EventLogEntry newEventLogEntry(long cPtr, boolean isOwner) {
	  try {
         if (cPtr == 0)
            return null;

         int index = ScaveEngineJNI.EventLogEntry_getClassIndex(cPtr);
         Constructor constructor = eventLogConstructors[index];
      
         if (constructor == null)
         {
            String name = "org.omnetpp.eventlog.engine." + ScaveEngineJNI.EventLogEntry_getClassName(cPtr);
            Class clazz = Class.forName(name);
            constructor = clazz.getDeclaredConstructor(long.class, boolean.class);
            eventLogConstructors[index] = constructor;
         }
	  
         return (EventLogEntry)constructor.newInstance(cPtr, isOwner);
      }
      catch (Exception e) {
         throw new RuntimeException(e);
      }
   }
%}

%typemap(javacode) FileReader %{
    public FileReader(String fileName, boolean cMemoryOwn) {
	    this(fileName);
	    this.swigCMemOwn = cMemoryOwn;
    }
%}

%typemap(javacode) IEvent %{
    public long getCPtr() {
        return swigCPtr;
    }
%}

%typemap(javacode) MessageDependency %{
    public long getCPtr() {
        return swigCPtr;
    }
%}

%newobject SequenceChartFacade::getIntersectingMessageDependencies;

typedef double simtime_t;

%ignore eventLogStringPool;
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

%include "ievent.h"
%include "ieventlog.h"
%include "event.h"
%include "filteredevent.h"
%include "messagedependency.h"
%include "eventlogentry.h"
%include "eventlogentries.h"
%include "eventlogindex.h"
%include "eventlog.h"
%include "eventlogfacade.h"
%include "eventlogtablefacade.h"
%include "sequencechartfacade.h"
%include "filteredeventlog.h"
%include "filereader.h"
