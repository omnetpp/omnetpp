%module ScaveEngine

%pragma(java) jniclasscode=%{
  static {
    try {
      System.loadLibrary("scave_engine");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. \n" + e);
      System.exit(1);
    }
  }
%}

%{
#include "idlist.h"
#include "resultfilemanager.h"
#include "datasorter.h"
#include "util.h"   // strdictcmp
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

%include "scavedefs.h"

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

   %typemap(javacode) vector<File*> %{
        public File[] toArray() {
            int sz = (int)size();
            File[] array = new File[sz];
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


   %template(StringSet) set<string>;
   %template(StringVector) vector<string>;

   specialize_std_vector(ID);

   %template(IDVector) vector<ID>;
   %template(IDVectorVector) vector<vector<ID> >;

   specialize_std_vector(Run*);
   specialize_std_vector(File*);

   %template(RunList) vector<Run*>;
   %template(FileList) vector<File*>;

   specialize_std_vector(MessageEntry*);

   %template(MessageEntries) vector<MessageEntry*>;

   specialize_std_vector(const char *);

   %template(PStringVector) vector<const char *>;

   %template(IntSet) set<int>;

   specialize_std_map_on_both(int,,,,int,,,);

   %template(IntIntMap) map<int,int>;

   %template(IntVector) vector<int>;

};

%typemap(javacode) IDList %{
    public void swigDisown() {
        swigCMemOwn = false;
    }
    public Long[] toArray() {
        int sz = (int) size();
        Long[] array = new Long[sz];
        for (int i=0; i<sz; i++)
            array[i] = Long.valueOf(get(i));
        return array;
    }
    public static IDList fromArray(Long[] array) {
        IDList list = new IDList();
        for (int i=0; i<array.length; i++)
            list.add(array[i].longValue());
        return list;
    }
%}

//
// The following code is for IDList::getSubsetByIndices():
//
%typemap(in) (int *array, int n) {
    $2 = jenv->GetArrayLength($input);
    jint *a = jenv->GetIntArrayElements($input, 0);
    $1 = new int[$2];
    for (int i=0; i<$2; i++)  $1[i] = a[i];
    jenv->ReleaseIntArrayElements($input, a, 0);
}

%typemap(freearg) (int *array, int n) {
    delete [] $1;
}

%typemap(jni)    (int *array, int n) "jintArray"
%typemap(jtype)  (int *array, int n) "int[]"
%typemap(jstype) (int *array, int n) "int[]"
%typemap(javain) (int *array, int n) "$javainput"

//
// The following code is for IDList::toByteArray()/fromByteArray()
//
%typemap(in) (char *array, int n) {
    $2 = jenv->GetArrayLength($input);
    $1 = (char *) jenv->GetByteArrayElements($input, 0);
}

%typemap(freearg) (char *array, int n) {
    jenv->ReleaseByteArrayElements($input, (jbyte *)$1, 0);
}

%typemap(jni)    (char *array, int n) "jbyteArray"
%typemap(jtype)  (char *array, int n) "byte[]"
%typemap(jstype) (char *array, int n) "byte[]"
%typemap(javain) (char *array, int n) "$javainput"

// FIXME add %newobject where needed! (e.g. EventLog::buildMessageCountGraph)

%ignore ResultFileManager::dump;

//
// By default, string members get wrapped with get/set using *pointers* to strings.
// This is not too nice, so override it by defining proper getter/setter methods,
// and hiding (ignoring) the class member
//
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

FIX_STRING_MEMBER(File, filePath, FilePath);
FIX_STRING_MEMBER(File, directory, Directory);
FIX_STRING_MEMBER(File, fileName, FileName);

FIX_STRING_MEMBER(Run, networkName, NetworkName);
FIX_STRING_MEMBER(Run, date, Date);
FIX_STRING_MEMBER(Run, runName, RunName);
FIX_STRING_MEMBER(Run, fileAndRunName, FileAndRunName);
FIX_STRING_MEMBER(Run, experimentName, ExperimentName);
FIX_STRING_MEMBER(Run, measurementName, MeasurementName);
FIX_STRING_MEMBER(Run, replicationName, ReplicationName);

%rename Run::fileRef file;
%rename ResultItem::runRef run;

%ignore ResultItem::moduleNameRef;
%ignore ResultItem::nameRef;
%extend ResultItem {
   std::string getModuleName() {return *self->moduleNameRef;}
   std::string getName() {return *self->nameRef;}
}

%ignore File::id;
%ignore File::runs;
%ignore File::scalarResults;
%ignore File::vectorResults;

%typemap(javacode) File %{
    public boolean equals(Object obj) {
        return (obj instanceof File) && getCPtr(this)==getCPtr((File)obj);
    }
    public int hashCode() {
        return (int)getCPtr(this);
    }
%}

%typemap(javacode) Run %{
    public boolean equals(Object obj) {
        return (obj instanceof Run) && getCPtr(this)==getCPtr((Run)obj);
    }
    public int hashCode() {
        return (int)getCPtr(this);
    }
%}

// Java doesn't appear to have dictionary sort, export it
int strdictcmp(const char *s1, const char *s2);

/* Let's just grab the original header file here */
%include "idlist.h"
%include "resultfilemanager.h"
%include "datasorter.h"

// wrap the data-flow engine as well
%include plove.i

// wrap eventlog.h
%ignore EventLog::writeTrace;

FIX_CHARPTR_MEMBER(MessageEntry, messageName, MessageName);
FIX_CHARPTR_MEMBER(MessageEntry, messageClassName, MessageClassName);
FIX_CHARPTR_MEMBER(ModuleEntry, moduleClassName, ModuleClassName);
FIX_STRING_MEMBER(ModuleEntry, moduleFullPath, ModuleFullPath);

%include "eventlog.h"


