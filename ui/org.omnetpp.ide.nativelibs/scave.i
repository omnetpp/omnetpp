%module ScaveEngine

// covariant return type warning disabled
#pragma SWIG nowarn=822

%include "loadlib.i"
#%include "enums.swg"
#%javaconst(1);

%{
#include "scave/scavedefs.h"
#include "scave/enumtype.h"
#include "scave/statistics.h"
#include "scave/idlist.h"
#include "scave/resultfilemanager.h"
#include "scave/fields.h"
#include "scave/datasorter.h"
#include "common/stringutil.h"   // strdictcmp
#include "scave/indexfile.h"
#include "scave/commonnodes.h"
#include "scave/indexedvectorfile.h"
#include "scave/vectorfileindexer.h"
#include "scave/vectorfilereader.h"
#include "scave/indexedvectorfilereader.h"
#include "scave/sqlitevectorreader.h"
#include "scave/scaveexception.h"
#include "scave/export.h"

using namespace omnetpp::scave;
%}

#define THREADED

%include "defs.i"

// hide export/import macros from swig
#define SCAVE_API
#define COMMON_API

namespace omnetpp { namespace scave {

typedef omnetpp::common::BigDecimal BigDecimal;
typedef omnetpp::common::opp_runtime_error opp_runtime_error;

typedef BigDecimal simultime_t;
#define SIMTIME_MIN BigDecimal::NegativeInfinity
#define SIMTIME_MAX BigDecimal::PositiveInfinity
typedef int64_t ID;
typedef int64_t eventnumber_t;

} } // namespaces

%include "bigdecimal.i"

namespace omnetpp { namespace scave {
class ResultItemField;
class ResultItemFields;
class DataflowManager;
class Node;
} } // namespaces

USE_COMMON_ENGINE_ILOCK();

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

   %typemap(javacode) vector<double> %{
       public double[] toArray() {
           int sz = (int) size();
           double[] array = new double[sz];
           for (int i=0; i<sz; i++)
               array[i] = get(i);
           return array;
       }

   %}

   %typemap(javacode) vector<omnetpp::scave::Run*> %{
        public Run[] toArray() {
            int sz = (int)size();
            Run[] array = new Run[sz];
            for (int i=0; i<sz; i++)
                array[i] = get(i);
            return array;
        }
   %}

   %typemap(javacode) vector<omnetpp::scave::ResultFile*> %{
        public ResultFile[] toArray() {
            int sz = (int)size();
            ResultFile[] array = new ResultFile[sz];
            for (int i=0; i<sz; i++)
                array[i] = get(i);
            return array;
        }
   %}

   %typemap(javacode) vector<omnetpp::scave::FileRun*> %{
        public FileRun[] toArray() {
            int sz = (int)size();
            FileRun[] array = new FileRun[sz];
            for (int i=0; i<sz; i++)
                array[i] = get(i);
            return array;
        }
   %}

   %typemap(javacode) vector<omnetpp::scave::ID> %{
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

   %template(IDVector) vector<omnetpp::scave::ID>;
   %template(IDVectorVector) vector<vector<omnetpp::scave::ID> >;
   %template(RunList) vector<omnetpp::scave::Run*>;
   %template(ResultFileList) vector<omnetpp::scave::ResultFile*>;
   %template(FileRunList) vector<omnetpp::scave::FileRun*>;
   %template(DoubleVector) vector<double>;
   %template(XYDatasetVector) vector<omnetpp::scave::XYDataset>;
};

//
// enumtype.h
//
namespace omnetpp { namespace scave {

%rename(toString) EnumType::str;
%ignore EnumType::insert;
%ignore EnumType::parseFromString;
%ignore EnumType::operator=;

} } // namespaces

%include "scave/enumtype.h"

%include "scave/enums.h"

/*---------------------------------------------------------------------------
 *                    ResultFileManager
 *---------------------------------------------------------------------------*/


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


// FIXME add %newobject where needed!

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


//
// addComputedVector
//

%typemap(jni)     omnetpp::scave::IComputation* "jobject"
%typemap(jtype)   omnetpp::scave::IComputation* "Object"
%typemap(jstype)  omnetpp::scave::IComputation* "Object"
%typemap(javain)  omnetpp::scave::IComputation* "$javainput"
%typemap(javaout) omnetpp::scave::IComputation* {
   return $jnicall;
}

%{
class Computation : public omnetpp::scave::IComputation {
  JavaVM *jvm;
  jobject ref;
  private:
    Computation(const Computation &other); // unimplemented
    Computation &operator=(const Computation &other); // unimplemented
    JNIEnv *getJNIEnv() const
    {
      JNIEnv *env;
      jint error = jvm->AttachCurrentThread((void**)&env, NULL);
      if (error)
        throw opp_runtime_error("Can not access JNIEnv from the current thread.");
      return env;
    }
  public:
    Computation(JNIEnv *jenv, jobject computation)
    {
        jint error = jenv->GetJavaVM(&jvm);
        if (error)
          throw opp_runtime_error("Can not access JVM.");
        ref = jenv->NewGlobalRef(computation);
    }

    virtual IComputation *dup() { return new Computation(getJNIEnv(), ref); }
    virtual ~Computation() { getJNIEnv()->DeleteGlobalRef(ref); }

    virtual bool operator==(const IComputation &other) const
    {
      Computation &o = dynamic_cast<Computation&>(const_cast<IComputation&>(other));
      return getJNIEnv()->IsSameObject(this->ref, o.ref);
    }

    jobject getJavaObject() const { return getJNIEnv()->NewLocalRef(ref);; }
};
%}

%typemap(in) omnetpp::scave::IComputation* {
  $1 = (IComputation*)new Computation(jenv, $input);
}

%typemap(out) omnetpp::scave::IComputation* {
  if (dynamic_cast<Computation*>($1))
    $result = dynamic_cast<Computation*>($1)->getJavaObject();
  else
    $result = $null;
}


/*--------------------------------------------------------------------------
 *                     check ResultFileFormatException
 *--------------------------------------------------------------------------*/
%define CHECK_RESULTFILE_FORMAT_EXCEPTION(METHOD)
%exception METHOD {
    try {
        $action
    } catch (ResultFileFormatException& e) {
    jclass clazz = jenv->FindClass("org/omnetpp/scave/engineext/ResultFileFormatException");
    jmethodID methodId = jenv->GetMethodID(clazz, "<init>", "(Ljava/lang/String;Ljava/lang/String;I)V");
    jthrowable exception = (jthrowable)(jenv->NewObject(clazz, methodId, jenv->NewStringUTF(e.what()), jenv->NewStringUTF(e.getFileName()), e.getLine()));
        jenv->Throw(exception);
        return $null;
    } catch (std::exception& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, const_cast<char*>(e.what()));
        return $null;
    }
}
%enddef


// Java doesn't appear to have dictionary sort, export it
namespace omnetpp { namespace common {
int strdictcmp(const char *s1, const char *s2);
} } // namespaces

/* ------------- statistics.h  ----------------- */
namespace omnetpp { namespace scave {
%ignore Statistics::operator=;
} } // namespaces

%include "scave/statistics.h"

/* ------------- idlist.h  ----------------- */
%include "idlist.i"

/* ------------- resultfilemanager.h  ----------------- */
namespace omnetpp { namespace scave {
%ignore IComputation;
%ignore ResultFileManager::dump;
%ignore VectorResult::stat;
%ignore HistogramResult::stat;
%ignore ResultFile::id;
%ignore ResultFile::scalarResults;
%ignore ResultFile::vectorResults;
%ignore ResultFile::histogramResults;

%rename FileRun::fileRef file;
%rename FileRun::runRef run;
%rename ResultItem::fileRunRef fileRun;

%ignore ResultItem::moduleNameRef;
%ignore ResultItem::nameRef;

// do not allow direct access to unwrapped ResultFileManager
%ignore ResultFile::resultFileManager;
%ignore Run::resultFileManager;

// unused methods
%ignore ResultFileManager::filterRunList;
%ignore ResultFileManager::filterFileList;
%ignore ResultFileManager::getFileRuns;
%ignore ResultFileManager::getUniqueFileRuns;
%ignore ResultFileManager::getScalarsInFileRun;
%ignore ResultFileManager::getHistogramsInFileRun;
%ignore ResultFileManager::getUniqueRunAttributeValues;
%ignore ResultFileManager::getUniqueModuleParamValues;
//%ignore ResultFileManager::getUniqueModuleNames;
//%ignore ResultFileManager::getUniqueNames;
//%ignore ResultFileManager::getUniqueAttributeValues;
//%ignore ResultFileManager::getFileAndRunNumberFilterHints;

%extend ResultFileManager {
    IDList getComputedScalarIDs(const IComputation *computation) const
    {
        IDList result = $self->getComputedScalarIDs(computation);
        // KLUDGE: computation is allocated in the SWIG generated JNI code
        // unfortunately it isn't easy to delete this there without affecting other generated code
        delete computation;
        return result;
    }
}

%newobject ResultItem::getEnum() const;

%extend ResultItem {
   std::string getModuleName() {return *self->moduleNameRef;}
   std::string getName() {return *self->nameRef;}
}

//
// Add polymorphic return type to ResultFileManager::getItem(),
// because plain ResultItem does not contain the type (VECTOR, SCALAR, etc).
//
%ignore ResultFileManager::getItem;
%typemap(javacode) ResultFileManager %{

  public ResultItem getItem(long id) {
      int type = getTypeOf(id);
      if (type==SCALAR)
          return getScalar(id);
      else if (type==VECTOR)
          return getVector(id);
      else if (type==HISTOGRAM)
          return getHistogram(id);
      else
          throw new RuntimeException("unknown ID type");
  }

  public static <T> T callWithReadLock(ResultFileManager manager, java.util.concurrent.Callable<T> callable) {
    return callWithLock(manager != null ? manager.getReadLock() : null, callable);
  }

  public static <T> T callWithWriteLock(ResultFileManager manager, java.util.concurrent.Callable<T> callable) {
    return callWithLock(manager != null ? manager.getWriteLock() : null, callable);
  }

  private static <T> T callWithLock(org.omnetpp.common.engine.ILock lock, java.util.concurrent.Callable<T> callable) {
    if (lock != null)
      lock.lock();
    try {
      return callable.call();
    }
    catch (RuntimeException e) {
      throw e;
    }
    catch (Exception e) {
      throw new RuntimeException(e);
    }
    finally {
      if (lock != null)
        lock.unlock();
    }
  }


  public void checkReadLock() {
    org.eclipse.core.runtime.Assert.isTrue(getReadLock().hasLock(), "Missing read lock.");
  }

  public void checkWriteLock() {
    org.eclipse.core.runtime.Assert.isTrue(getWriteLock().hasLock(), "Missing write lock.");
  }

  public boolean equals(Object obj) {
    if (this == obj)
      return true;
    if (obj == null || this.getClass() != obj.getClass())
      return false;
    return getCPtr(this) == getCPtr((ResultFileManager)obj);
  }

  public int hashCode() {
    return (int)getCPtr(this);
  }
%}

FIX_STRING_MEMBER(ResultFile, filePath, FilePath);
FIX_STRING_MEMBER(ResultFile, directory, Directory);
FIX_STRING_MEMBER(ResultFile, fileName, FileName);
FIX_STRING_MEMBER(ResultFile, fileSystemFilePath, FileSystemFilePath);
//FIX_STRING_MEMBER(Run, networkName, NetworkName);
//FIX_STRING_MEMBER(Run, date, Date);
FIX_STRING_MEMBER(Run, runName, RunName);
//FIX_STRING_MEMBER(Run, fileAndRunName, FileAndRunName);
//FIX_STRING_MEMBER(Run, experimentName, ExperimentName);
//FIX_STRING_MEMBER(Run, measurementName, MeasurementName);
//FIX_STRING_MEMBER(Run, replicationName, ReplicationName);
FIX_STRING_MEMBER(VectorResult, columns, Columns);

ADD_CPTR_EQUALS_AND_HASHCODE(ResultFile);
ADD_CPTR_EQUALS_AND_HASHCODE(Run);
ADD_CPTR_EQUALS_AND_HASHCODE(FileRun);
ADD_CPTR_EQUALS_AND_HASHCODE(ResultItem);
CHECK_RESULTFILE_FORMAT_EXCEPTION(ResultFileManager::loadFile)

%newobject ResultFileManager::getUniqueFiles(const IDList&) const;
%newobject ResultFileManager::getUniqueRuns(const IDList&) const;
%newobject ResultFileManager::getUniqueFileRuns(const IDList&) const;
%newobject ResultFileManager::getUniqueModuleNames(const IDList&) const;
%newobject ResultFileManager::getUniqueNames(const IDList&) const;
%newobject ResultFileManager::getUniqueAttributeNames(const IDList&) const;
%newobject ResultFileManager::getUniqueRunAttributeNames(const RunList *) const;
%newobject ResultFileManager::getUniqueModuleParamNames(const RunList *) const;
%newobject ResultFileManager::getUniqueAttributeValues(const IDList &, const char *) const;
%newobject ResultFileManager::getUniqueRunAttributeValues(const RunList&, const char *) const;
%newobject ResultFileManager::getUniqueModuleParamValues(const RunList&, const char *) const;

%newobject ResultFileManager::getFileAndRunNumberFilterHints(const IDList&) const;
%newobject ResultFileManager::getFilePathFilterHints(const ResultFileList&) const;
%newobject ResultFileManager::getRunNameFilterHints(const RunList&) const;
%newobject ResultFileManager::getModuleFilterHints(const IDList&) const;
%newobject ResultFileManager::getNameFilterHints(const IDList&) const;
%newobject ResultFileManager::getResultItemAttributeFilterHints(const IDList&, const char*) const;
%newobject ResultFileManager::getRunAttributeFilterHints(const RunList&, const char*) const;
%newobject ResultFileManager::getModuleParamFilterHints(const RunList&, const char*) const;
} } // namespaces

%include "scave/resultfilemanager.h"

/* ------------- datasorter.h  ----------------- */
%include "scave/datasorter.h"


/* ------------- dataflownetwork.h  ----------------- */
// wrap the data-flow engine as well
namespace omnetpp { namespace scave {
CHECK_RESULTFILE_FORMAT_EXCEPTION(DataflowManager::execute)
} } // namespaces
%include scave-plove.i

/* ------------- indexfile.h  ----------------- */
// %include "scave/indexfile.h"

namespace omnetpp { namespace scave {

%javamethodmodifiers IndexFile::isIndexFileUpToDate "protected";

class IndexFile
{
    public:
        static bool isIndexFile(const char *indexFileName);
        static bool isExistingVectorFile(const char *vectorFileName);
        static std::string getIndexFileName(const char *vectorFileName);
        static std::string getVectorFileName(const char *indexFileName);
        static bool isIndexFileUpToDate(const char *fileName);
};

} } // namespaces

/* ------------- vectorfileindexer.h  ----------------- */
namespace omnetpp { namespace scave {
CHECK_RESULTFILE_FORMAT_EXCEPTION(VectorFileIndexer::generateIndex)
} } // namespaces

%include "scave/vectorfileindexer.h"

/* ------------- indexedvectorfile.h  ----------------- */
%typemap(javainterfaces) omnetpp::scave::OutputVectorEntry "Comparable<OutputVectorEntry>"

%typemap(javacode) omnetpp::scave::OutputVectorEntry %{
   public boolean equals(Object obj) {
      return (obj instanceof OutputVectorEntry) && getSerial() == ((OutputVectorEntry)obj).getSerial();
   }

   public int hashCode() {
      return getSerial();
   }

   public int compareTo(OutputVectorEntry o) {
      return getSerial() - o.getSerial();
   }
%}

namespace omnetpp { namespace scave {

%template(EntryVector) ::std::vector<omnetpp::scave::OutputVectorEntry>;

%ignore IndexedVectorFileWriterNode;
%ignore IndexedVectorFileWriterNodeType;
} } // namespaces

%include "scave/indexedvectorfile.h"

/* ------------- vectorfilereader.h  ----------------- */
namespace omnetpp { namespace scave {
%ignore SingleSourceNode;
%ignore SingleSinkNode;
%ignore FilterNode;
%ignore ReaderNode;
%ignore FileReaderNode;
%ignore SingleSourceNodeType;
%ignore SingleSinkNodeType;
%ignore FilterNodeType;
%ignore ReaderNodeType;
} } // namespaces

%include "scave/commonnodes.h"

namespace omnetpp { namespace scave {

%extend VectorFileReaderNode {
    static VectorFileReaderNode *cast(Node* node) { return dynamic_cast<VectorFileReaderNode*>(node); }
};

%ignore VectorFileReaderNodeType;
%ignore parseColumns;

} } // namespaces

%include "scave/vectorfilereader.h"

/* ------------- indexedvectorfilereader.h  ----------------- */
%include "scave/indexedvectorfilereader.h"

namespace omnetpp { namespace scave {
%extend IndexedVectorFileReaderNode {
    static IndexedVectorFileReaderNode *cast(Node* node) { return dynamic_cast<IndexedVectorFileReaderNode*>(node); }
};
} } // namespaces

/* ------------- sqlitevectorreader.h  ----------------- */
%include "scave/sqlitevectorreader.h"

namespace omnetpp { namespace scave {
%extend SqliteVectorReaderNode {
    static SqliteVectorReaderNode *cast(Node* node) { return dynamic_cast<SqliteVectorReaderNode*>(node); }
};
} } // namespaces

/* ------------------ fields.h --------------------- */
namespace omnetpp { namespace scave {
%ignore ResultItemFieldsEqual;
%ignore ResultItemFieldsLess;
%ignore IDFieldsEqual;
%ignore IDFieldsLess;
%ignore getAttribute;
%ignore ResultItemFields::begin;
%ignore ResultItemFields::end;

%typemap(javacode) ResultItemField %{

  public static final String FILE   = getFILE();
  public static final String RUN    = getRUN();
  public static final String MODULE = getMODULE();
  public static final String NAME   = getNAME();

  public boolean equals(Object other) {
    if (this == other)
      return true;
    if (other == null || this.getClass() != other.getClass())
      return false;
    ResultItemField otherField = (ResultItemField)other;
    return this.getID() == otherField.getID() && this.getName().equals(otherField.getName());
  }

  public int hashCode() {
    return 37 * getID() + getName().hashCode();
  }
%}

%typemap(javacode) RunAttribute %{

  public static final String INIFILE           = getINIFILE();
  public static final String CONFIGNAME        = getCONFIGNAME();
  public static final String RUNNUMBER         = getRUNNUMBER();
  public static final String NETWORK           = getNETWORK();
  public static final String EXPERIMENT        = getEXPERIMENT();
  public static final String MEASUREMENT       = getMEASUREMENT();
  public static final String REPLICATION       = getREPLICATION();
  public static final String PROCESSID         = getPROCESSID();
  public static final String DATETIME          = getDATETIME();
  public static final String RESULTDIR         = getRESULTDIR();
  public static final String REPETITION        = getREPETITION();
  public static final String SEEDSET           = getSEEDSET();
  public static final String ITERATIONVARS     = getITERATIONVARS();
  public static final String ITERATIONVARS2    = getITERATIONVARS2();

%}

%typemap(javacode) ResultItemFields %{

  public ResultItemFields(String... fieldNames) {
    this(StringVector.fromArray(fieldNames));
  }
%}

} } // namespaces

%include "scave/fields.h"

/* ------------------ datasorter.h --------------------- */
%include "scave/datasorter.h"

/* ------------------ export.h ----------------------- */
namespace omnetpp { namespace scave {
%ignore Column;
%ignore DataTable;
%ignore XYDataTable;
%ignore ScalarDataTable;
%ignore ScatterDataTable;
%ignore JoinedDataTable;
%ignore MatlabStructExport;
%ignore MatlabScriptExport;
%ignore OctaveTextExport;
%ignore ScaveExport::saveVectors;
%rename(EOL)    CsvExport::eol;
%newobject ExporterFactory::createExporter;
} } // namespaces

%include "scave/export.h"

