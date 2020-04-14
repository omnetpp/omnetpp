%module ScaveEngine

// covariant return type warning disabled
#pragma SWIG nowarn=822

%include "loadlib.i"
#%include "enums.swg"
#%javaconst(1);

%{
#include "common/statistics.h"
#include "common/histogram.h"
#include "common/stringutil.h"   // strdictcmp
#include "scave/scavedefs.h"
#include "scave/enumtype.h"
#include "scave/idlist.h"
#include "scave/resultitems.h"
#include "scave/resultfilemanager.h"
#include "scave/fields.h"
#include "scave/exportutils.h"
#include "scave/vectorutils.h"
#include "scave/memoryutils.h"
#include "scave/vectorfileindex.h"
#include "scave/indexfileutils.h"
#include "scave/ivectordatareader.h"
#include "scave/indexedvectorfilereader.h"
#include "scave/sqlitevectordatareader.h"
#include "scave/vectorfileindexer.h"
#include "scave/scaveexception.h"
#include "scave/exporter.h"
#include "scave/sqliteresultfileutils.h"
#include "scave/xyarray.h"

#include "resultspickler.h"

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
class IDList;
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
%include "std_pair.i"

%include "map_oldapi.i" // needed for SWIG >=4.0 so we still have the 3.x functions

// If there're wrappers (SwigValueWrapper) in the generated code, the following will help: 
//%feature("novaluewrapper") omnetpp::scave::IDListsByRun;

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

   %extend vector<omnetpp::scave::ID> {
       void append(const vector<omnetpp::scave::ID>& v) {
           self->insert(self->end(), v.begin(), v.end());
       }
       void append(const omnetpp::scave::IDList& ids) {
           self->insert(self->end(), ids.begin(), ids.end());
       }
       omnetpp::scave::IDList toIDList() {
           return IDList(std::move(*self));
       }
   }

   %template(StringSet) set<string>;
   %template(StringVector) vector<string>;
   //specialize_std_map_on_both(string,,,,string,,,);
   %template(StringMap) map<string,string>;

   //TODO take the following stuff out of namepace std{}!!!

   %template(IDListBuffer) vector<omnetpp::scave::ID>;
   %template(RunList) vector<omnetpp::scave::Run*>;
   %template(ResultFileList) vector<omnetpp::scave::ResultFile*>;
   %template(FileRunList) vector<omnetpp::scave::FileRun*>;
   %template(DoubleVector) vector<double>;
   %template(StringPair) pair<string,string>;
   %template(OrderedKeyValueList) vector< pair<string,string> >;
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
namespace omnetpp { namespace common {
%ignore Statistics::operator=;
} } // namespaces

%include "common/statistics.h"

/* ------------- histogram.h  ----------------- */
namespace omnetpp { namespace common {
%ignore Histogram::operator=;
} } // namespaces

%include "common/histogram.h"

/* ------------- idlist.h  ----------------- */
%include "idlist.i"

/* ------------- resultfilemanager.h  ----------------- */
namespace omnetpp { namespace scave {

%newobject ResultItem::getEnum() const;

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
      else if (type==PARAMETER)
          return getParameter(id);
      else if (type==VECTOR)
          return getVector(id);
      else if (type==STATISTICS)
          return getStatistics(id);
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

  public static void runWithReadLock(ResultFileManager manager, java.lang.Runnable runnable) {
      runWithLock(manager != null ? manager.getReadLock() : null, runnable);
  }

  public static void runWithWriteLock(ResultFileManager manager, java.lang.Runnable runnable) {
      runWithLock(manager != null ? manager.getWriteLock() : null, runnable);
  }

  private static void runWithLock(org.omnetpp.common.engine.ILock lock, java.lang.Runnable runnable) {
      if (lock != null)
          lock.lock();
      try {
          runnable.run();
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

ADD_CPTR_EQUALS_AND_HASHCODE(ResultFile);
ADD_CPTR_EQUALS_AND_HASHCODE(Run);
ADD_CPTR_EQUALS_AND_HASHCODE(FileRun);
ADD_CPTR_EQUALS_AND_HASHCODE(ResultItem);
CHECK_RESULTFILE_FORMAT_EXCEPTION(ResultFileManager::loadFile)

} } // namespaces

%include "scave/resultitems.h"
%include "scave/resultfilemanager.h"

/* ------------- indexfileutils.h  ----------------- */
%include "scave/indexfileutils.h"

/* ------------- vectorfileindexer.h  ----------------- */
namespace omnetpp { namespace scave {
CHECK_RESULTFILE_FORMAT_EXCEPTION(VectorFileIndexer::generateIndex)
} } // namespaces

%include "scave/vectorfileindexer.h"

/* ------------- ivectordatareader.h  ----------------- */
%typemap(javainterfaces) omnetpp::scave::VectorDatum "Comparable<VectorDatum>"

%typemap(javacode) omnetpp::scave::VectorDatum %{
   public boolean equals(Object obj) {
      return (obj instanceof VectorDatum) && getSerial() == ((VectorDatum)obj).getSerial();
   }

   public int hashCode() {
      return getSerial();
   }

   public int compareTo(VectorDatum o) {
      return getSerial() - o.getSerial();
   }
%}

namespace omnetpp { namespace scave {

%template(EntryVector) ::std::vector<omnetpp::scave::VectorDatum>;

} } // namespaces

%include "scave/ivectordatareader.h"

/* ------------- indexedvectorfilereader.h  ----------------- */

%include "scave/indexedvectorfilereader.h"

/* ------------- sqlitevectordatareader.h  ----------------- */

%include "scave/sqlitevectordatareader.h"

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

} } // namespaces

%include "scave/fields.h"

/* ------------------ exporter.h ----------------------- */
namespace omnetpp { namespace scave {
%newobject ExporterFactory::createExporter;
} } // namespaces

%include "scave/exporter.h"

/* ------------------ exportutils.h ----------------------- */
%include "scave/exportutils.h"

/* ------------------ sqliteresultfileutils.h ----------------------- */
%include "scave/sqliteresultfileutils.h"

/* ------------------ xyarray.h ----------------------- */
%include "scave/xyarray.h"

/* ------------------ vectorutils.h ----------------------- */

namespace omnetpp { namespace scave {
%ignore readVectorsIntoArrays;
%newobject readVectorsIntoArrays2;

} } // namespaces

%include "scave/vectorutils.h"

/* ------------------ memoryutils.h ----------------------- */
%include "scave/memoryutils.h"

/* ------------------ resultspickler.h ----------------------- */
%include "resultspickler.h"

/* ------------------ sharedmemory.cc ----------------------- */

%native(createSharedMemory) void createSharedMemory(jstring name, jlong size);

%typemap(jstype) ByteBuffer mapSharedMemory "java.nio.ByteBuffer"
%typemap(jtype) ByteBuffer mapSharedMemory "java.nio.ByteBuffer"
%typemap(javaout) ByteBuffer mapSharedMemory { return $jnicall; }
%native(mapSharedMemory) ByteBuffer mapSharedMemory(jstring name, jlong size);

// TODO figure out how to replace jobject with java.nio.ByteBuffer here as well
%native(unmapSharedMemory) void unmapSharedMemory(jobject buf);

%native(removeSharedMemory) void removeSharedMemory(jstring name);
