%module ScaveEngine

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
#include "eventlog.h"
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

%typemap(jni) ID "jlong";

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

   specialize_std_vector(MessageEntry*);

   %template(MessageEntries) vector<MessageEntry*>;

   specialize_std_vector(const char *);

   %template(PStringVector) vector<const char *>;

   %template(IntSet) set<int>;

   specialize_std_map_on_both(int,,,,int,,,);

   %template(IntIntMap) map<int,int>;

   %template(IntVector) vector<int>;

};

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

FIX_CHARPTR_MEMBER(MessageEntry, messageName, MessageName);
FIX_CHARPTR_MEMBER(MessageEntry, messageClassName, MessageClassName);
FIX_CHARPTR_MEMBER(ModuleEntry, moduleClassName, ModuleClassName);
FIX_STRING_MEMBER(ModuleEntry, moduleFullPath, ModuleFullPath);

%include "eventlog.h"


