%module EventLogEngine

%include "loadlib.i"
%include "enumtypeunsafe.swg"
%include "commondefs.i"
%javaconst(1);

COMMON_ENGINE_BIGDECIMAL();

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
    } catch (std::exception& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, const_cast<char*>(e.what()));
        return $null;
    }
}

// hide export/import macros from swig
#define COMMON_API
#define OPP_DLLEXPORT
#define OPP_DLLIMPORT

%include "inttypes.h"
%include "eventlogdefs.h"

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
           this(EventLogEngineJNI.new_IntSet__SWIG_0(), cMemoryOwn);
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

   specialize_std_vector(IMessageDependency*);
   %template(IMessageDependencyList) vector<IMessageDependency*>;

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
   private static Constructor[] eventLogEntryConstructors = new Constructor[100];

   public boolean equals(Object obj) {
      return (obj instanceof EventLogEntry) && getCPtr(this)==getCPtr((EventLogEntry)obj);
   }

   public int hashCode() {
      return (int)getCPtr(this);
   }

   public static EventLogEntry newEventLogEntry(long cPtr, boolean isOwner) {
      try {
         if (cPtr == 0)
            return null;

         int index = EventLogEngineJNI.EventLogEntry_getClassIndex(cPtr);
         Constructor constructor = eventLogEntryConstructors[index];

         if (constructor == null)
         {
            String name = "org.omnetpp.eventlog.engine." + EventLogEngineJNI.EventLogEntry_getClassName(cPtr);
            Class clazz = Class.forName(name);
            constructor = clazz.getDeclaredConstructor(long.class, boolean.class);
            eventLogEntryConstructors[index] = constructor;
         }

         return (EventLogEntry)constructor.newInstance(cPtr, isOwner);
      }
      catch (Exception e) {
         throw new RuntimeException(e);
      }
   }
%}

%typemap(javaout) IMessageDependency * {
   return IMessageDependency.newIMessageDependency($jnicall, $owner);
}

%typemap(javaimports) IMessageDependency %{
import java.lang.reflect.Constructor;
%}

%typemap(javacode) IMessageDependency %{
   private static Constructor[] messageDependencyConstructors = new Constructor[100];

   public long getCPtr() {
       return swigCPtr;
   }

   public boolean equals(Object obj) {
      return (obj instanceof IMessageDependency) && getCPtr(this)==getCPtr((IMessageDependency)obj);
   }

   public int hashCode() {
      return (int)getCPtr(this);
   }

   public static IMessageDependency newIMessageDependency(long cPtr, boolean isOwner) {
      try {
         if (cPtr == 0)
            return null;

         int index = EventLogEngineJNI.IMessageDependency_getClassIndex(cPtr);
         Constructor constructor = messageDependencyConstructors[index];

         if (constructor == null)
         {
            String name = "org.omnetpp.eventlog.engine." + EventLogEngineJNI.IMessageDependency_getClassName(cPtr);
            Class clazz = Class.forName(name);
            constructor = clazz.getDeclaredConstructor(long.class, boolean.class);
            messageDependencyConstructors[index] = constructor;
         }

         return (IMessageDependency)constructor.newInstance(cPtr, isOwner);
      }
      catch (Exception e) {
         throw new RuntimeException(e);
      }
   }
%}

%typemap(javacode) IEventLog %{
    public IEventLog own() {
        swigCMemOwn = true;
        return this;
    }

    public IEventLog disown() {
        swigCMemOwn = false;
        return this;
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
