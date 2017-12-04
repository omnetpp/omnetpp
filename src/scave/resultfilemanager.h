//=========================================================================
//  RESULTFILEMANAGER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_RESULTFILEMANAGER_H
#define __OMNETPP_SCAVE_RESULTFILEMANAGER_H

#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>

#include "common/exception.h"
#include "common/commonutil.h"
#include "common/statistics.h"
#include "common/histogram.h"
#include "idlist.h"
#include "enumtype.h"
#include "scaveutils.h"
#include "enums.h"

#ifdef THREADED
#include "common/rwlock.h"
#endif

namespace omnetpp {
namespace scave {

/**
 * Returned by reference for missing values. It has value "".
 */
extern const std::string NULLSTRING;

class Run;
class ResultFile;
class FileRun;
class ResultFileManager;
class CmpBase;
class OmnetppResultFileLoader;
class SqliteResultFileLoader;

typedef std::vector<std::string> StringVector;
typedef std::set<std::string> StringSet;
typedef std::map<std::string, std::string> StringMap;
typedef std::vector< std::pair<std::string, std::string> > OrderedKeyValueList;

using omnetpp::common::Statistics;
using omnetpp::common::Histogram;

typedef int64_t ComputationID;

struct SCAVE_API IComputation {
    IComputation() {}
    virtual ~IComputation() {}
    virtual IComputation *dup() = 0;
    virtual bool operator==(const IComputation& other) const = 0;
    virtual bool operator!=(const IComputation& other) const { return !(*this == other);}
private:
    IComputation(const IComputation &); // unimplemented
    IComputation& operator=(const IComputation &); // unimplemented
};

/**
 * Represents a run in a result file. Such item is needed because
 * result files and runs are in many-to-many relationship: a result file
 * may contain more than one runs (.sca file), and during a simulation run
 * (represented by class Run) more than one result files are written into
 * (namely, a .vec and a .sca file, or sometimes many of them).
 * And ResultItems refer to a FileRun instead of a ResultFile and a Run,
 * to conserve memory.
 */
class SCAVE_API FileRun
{
public: //TODO private
    ResultFile *fileRef;
    Run *runRef;
};

/**
 * Item in an output scalar or output vector file. Represents common properties
 * of an output vector or output scalar.
 */
class SCAVE_API ResultItem
{
    friend class ResultFileManager;
    friend class SqliteResultFileLoader;

  public:
    enum Type { TYPE_INT, TYPE_DOUBLE, TYPE_ENUM };

  protected:
    FileRun *fileRunRef; // backref to containing FileRun
    const std::string *moduleNameRef; // points into ResultFileManager'strptr StringSet
    const std::string *nameRef; // scalarname or vectorname; points into ResultFileManager'strptr StringSet
    StringMap attributes; // metadata in key/value form
    IComputation *computation;

  protected:
    ResultItem(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs);
    void setAttributes(const StringMap& attrs) {attributes = attrs;}
    void setAttribute(const std::string& attrName, const std::string& value) {attributes[attrName] = value;}

  public:
    ResultItem(const ResultItem& o)
        : fileRunRef(o.fileRunRef), moduleNameRef(o.moduleNameRef), nameRef(o.nameRef),
          attributes(o.attributes), computation(o.computation ? o.computation->dup() : nullptr) {}
    ResultItem& operator=(const ResultItem& rhs);
    virtual ~ResultItem() { delete computation; }

    const std::string& getName() const {return *nameRef;}
    const std::string& getModuleName() const {return *moduleNameRef;}

    FileRun *getFileRun() const {return fileRunRef;} //TODO return const
    ResultFile *getFile() const {return fileRunRef->fileRef;} //TODO return const
    Run *getRun() const {return fileRunRef->runRef;}  //TODO return const

    const StringMap& getAttributes() const {return attributes;}
    IComputation *getComputation() const {return computation;}

    const std::string& getAttribute(const std::string& attrName) const {
        StringMap::const_iterator it = attributes.find(attrName);
        return it==attributes.end() ? NULLSTRING : it->second;
    }

    /**
     * Returns the type of this result item (INT,DOUBLE,ENUM).
     * If neither "type" nor "enum" attribute is given it returns DOUBLE.
     */
    Type getType() const;

    /**
     * Returns a pointer to the enum type described by the "enum" attribute
     * or nullptr if no "enum" attribute.
     */
    EnumType* getEnum() const;

    bool isComputed() const { return computation != nullptr; }
};

/**
 * Represents an output scalar
 */
class SCAVE_API ScalarResult : public ResultItem
{
    friend class ResultFileManager;
  private:
    double value;
    bool isField_; // whether this scalar was created by exploding a "statistic" to its fields
    bool isItervar_; // whether this scalar was create by converting itervars to scalars
  protected:
    ScalarResult(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs, double value, bool isField, bool isItervar) :
        ResultItem(fileRun, moduleName, name, attrs), value(value), isField_(isField), isItervar_(isItervar) {}
  public:
    double getValue() const {return value;}
    bool isField() const {return isField_;}
    bool isItervar() const {return isItervar_;}
};

/**
 * Represents an output vector. This is only the declaration,
 * actual vector data are not kept in memory.
 */
class SCAVE_API VectorResult : public ResultItem
{
    friend class ResultFileManager;
    friend class OmnetppResultFileLoader;
    friend class SqliteResultFileLoader;
  private:
    int vectorId;
    std::string columns;
    eventnumber_t startEventNum, endEventNum;
    simultime_t startTime, endTime;
    Statistics stat;
  protected:
    VectorResult(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs, int vectorId, const std::string& columns) :
        ResultItem(fileRun, moduleName, name, attrs), vectorId(vectorId), columns(columns), startEventNum(-1), endEventNum(-1), startTime(0.0), endTime(0.0) {}
  public:
    int getVectorId() const {return vectorId;}
    const std::string& getColumns() const {return columns;}
    const Statistics& getStatistics() const {return stat;}
    eventnumber_t getStartEventNum() const {return startEventNum;}
    eventnumber_t getEndEventNum() const {return endEventNum;}
    simultime_t getStartTime() const {return startTime;}
    simultime_t getEndTime() const {return endTime;}

    /**
     * Returns the value of the "interpolation-mode" attribute as an InterpolationMode,
     * defaults to UNSPECIFIED.
     */
    InterpolationMode getInterpolationMode() const;
};

/**
 * Represents summary statistics of variable
 */
// TODO: there's a missing superclass between vectors and histograms that provides statistics, see the various sort<foo>By<bar> methods we need to make it work
class SCAVE_API StatisticsResult : public ResultItem
{
    friend class ResultFileManager;
    friend class OmnetppResultFileLoader;
    friend class SqliteResultFileLoader;
  private:
    Statistics stat; //TODO weighted
  protected:
    StatisticsResult(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs, const Statistics& stat) :
        ResultItem(fileRun, moduleName, name, attrs), stat(stat) {}
  public:
    const Statistics& getStatistics() const {return stat;}
};

/**
 * Represents a histogram.
 */
class SCAVE_API HistogramResult : public StatisticsResult
{
    friend class ResultFileManager;
    friend class OmnetppResultFileLoader;
    friend class SqliteResultFileLoader;
  private:
    Histogram bins;
  protected:
    HistogramResult(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs, const Statistics& stat, const Histogram& bins) :
        StatisticsResult(fileRun, moduleName, name, attrs, stat), bins(bins) {}
    void addBin(double lowerBound, double count);
  public:
    const Histogram& getHistogram() const {return bins;}
};

typedef std::vector<ScalarResult> ScalarResults;
typedef std::vector<VectorResult> VectorResults;
typedef std::vector<StatisticsResult> StatisticsResults;
typedef std::vector<HistogramResult> HistogramResults;

typedef std::vector<Run*> RunList;
typedef std::vector<ResultFile*> ResultFileList;
typedef std::vector<FileRun *> FileRunList;

/**
 * Represents a loaded scalar or vector file.
 */
class SCAVE_API ResultFile
{
    friend class OmnetppResultFileLoader;
    friend class SqliteResultFileLoader;
    friend class ResultFileManager;
    friend class DataSorter; // due to ScalarResults[] etc

  public:
    enum FileType { FILETYPE_OMNETPP, FILETYPE_SQLITE };

  private:
    int id;  // position in fileList
    ResultFileManager *resultFileManager; // backref to containing ResultFileManager
    std::string directory; // directory's location in the Eclipse workspace
    std::string fileName; // file name
    std::string filePath; // workspace directory + fileName
    std::string fileSystemFilePath; // directory+fileName of underlying file (for fopen())
    FileType fileType;
    bool computed;
    ScalarResults scalarResults;
    VectorResults vectorResults;
    StatisticsResults statisticsResults;
    HistogramResults histogramResults;

  public:
    ResultFileManager *getResultFileManager() const {return resultFileManager;}
    const std::string& getDirectory() const {return directory;}
    const std::string& getFileName() const {return fileName;}
    const std::string& getFilePath() const {return filePath;}
    const std::string& getFileSystemFilePath() const {return fileSystemFilePath;}
    FileType getFileType() const {return fileType;}
    bool isComputed() const {return computed;}
};

/**
 * Represents a run. If several scalar or vector files contain
 * the same run (i.e. runName is the same), they will share
 * the same Run object.
 */
class SCAVE_API Run
{
    friend class ResultFileManager;
    friend class OmnetppResultFileLoader;
    friend class SqliteResultFileLoader;

  private:
    std::string runName; // unique identifier for the run, "runId"
    ResultFileManager *resultFileManager; // backref to containing ResultFileManager
    StringMap attributes;  // run attributes, such as configname, runnumber, network, datetime, processid, etc.
    StringMap itervars;  // iteration variables (${} notation in omnetpp.ini)
    OrderedKeyValueList paramAssignments; // module parameter assignments from the ini file and command line
    //TODO: OrderedKeyValueList configEntries; // configuration entries from the ini file and command line
    bool computed;

  private:
    Run(const std::string& runName, bool computed, ResultFileManager *manager) : runName(runName), resultFileManager(manager), computed(computed) {}
    void setAttribute(const std::string& attrName, const std::string& value) {attributes[attrName] = value;}
    void addParamAssignmentEntry(const std::string& key, const std::string& value) {paramAssignments.push_back(std::make_pair(key,value));}
    //TODO void addConfigEntry(const std::string& key, const std::string& value) {configEntries.push_back(std::make_pair(key,value));}

  public:
    ResultFileManager *getResultFileManager() const {return resultFileManager;}
    const std::string& getRunName() const {return runName;}
    const StringMap& getAttributes() const {return attributes;}
    const std::string& getAttribute(const std::string& attrName) const;
    const StringMap& getIterationVariables() const {return itervars;}
    const std::string& getIterationVariable(const std::string& name) const;
    const OrderedKeyValueList& getParamAssignments() const {return paramAssignments;}
    const std::string& getParamAssignment(const std::string& key) const;
    //TODO const OrderedKeyValueList& getConfigEntries() const {return configEntries;}
    //TODO const std::string& getConfigOption(const std::string& key) const;
    bool isComputed() const {return computed;}
};


typedef std::map<std::pair<ComputationID, ID> , ID> ComputedIDCache;

/**
 * Loads and efficiently stores OMNeT++ output scalar files and output
 * vector files. (Actual vector contents in vector files are not read
 * into memory though, only the list of vectors.)
 *
 * Gives access to the list of module names and scalar data labels used in
 * the file as well as the list of all data.
 *
 * Data can be extracted, filtered, etc by external functions.
 */
class SCAVE_API ResultFileManager
{
    friend class ResultItem; // moduleNames, names
    friend class IDList;  // _type()
    friend class CmpBase; // uncheckedGet...()
    friend class OmnetppResultFileLoader;
    friend class SqliteResultFileLoader;
  private:
    // List of files loaded. This vector can have holes (NULLs) in it due to
    // unloaded files. The "id" field of ResultFile is the index into this vector.
    // It is not allowed to move elements, because IDs contain the file'strptr index in them.
    ResultFileList fileList;

    // List of unique runs in the files. If several files contain the same runName,
    // it will generate only one Run entry here.
    RunList runList;

    // ResultFiles and Runs have many-to-many relationship. This is where we store
    // their relations (instead of having a collection in both). ResultItems also
    // contain a FileRun pointer instead of separate ResultFile and Run pointers.
    FileRunList fileRunList;

    // module names and variable names are stringpooled to conserve space
    ScaveStringPool moduleNames;
    ScaveStringPool names;
    ScaveStringPool classNames; // currently not used

    ResultFile *computedScalarFile; // this computed file contains all computed scalars
    ScaveStringPool computedModuleNames; // computed modules, cleared when the computed scalar file is unloaded
    ScaveStringPool computedScalarNames; // computed scalar names, cleared when the computed scalar file is unloaded

    ComputedIDCache computedIDCache;
#ifdef THREADED
    omnetpp::common::ReentrantReadWriteLock lock;
#endif

  public:
    enum {SCALAR=1, VECTOR=2, STATISTICS=4, HISTOGRAM=8}; // must be 1,2,4,8 etc, because of IDList::getItemTypes()

  private:
    // ID: 1 bit computed**, 1 bit field**, 6 bit type, 24 bit fileid, 32 bit pos (** apparently unused)
    static int _type(ID id)   {return (id >> 56) & 0x3fUL;}
    static int _fileid(ID id) {return (id >> 32) & 0x00fffffful;}
    static int _pos(ID id)    {return id & 0xffffffffUL;}
    static ID _mkID(bool computed, bool field, int type, int fileid, int pos) {
        assert((type>>7)==0 && (fileid>>24)==0 && (pos>>31)<=1);
        return ((computed ? (ID)1 << 63 : (ID)0) | (field ? (ID)1 << 62 : (ID)0) | (ID)type << 56) | ((ID)fileid << 32) | (ID)pos;
    }

    // utility functions called while loading a result file
    ResultFile *addFile(const char *fileName, const char *fileSystemFileName, ResultFile::FileType fileType, bool computed);
    Run *addRun(const std::string& runName, bool computed=false);
    FileRun *addFileRun(ResultFile *file, Run *run);
    Run *getOrAddRun(const std::string& runName);
    FileRun *getOrAddFileRun(ResultFile *file, Run *run);

    int addScalar(FileRun *fileRunRef, const char *moduleName, const char *scalarName, const StringMap& attrs, double value, bool isField, bool isItervar);
    int addVector(FileRun *fileRunRef, int vectorId, const char *moduleName, const char *vectorName, const StringMap& attrs, const char *columns);
    int addStatistics(FileRun *fileRunRef, const char *moduleName, const char *statisticsName, const Statistics& stat, const StringMap& attrs);
    int addHistogram(FileRun *fileRunRef, const char *moduleName, const char *histogramName, const Statistics& stat, const Histogram& bins, const StringMap& attrs);
    void addStatisticsFieldsAsScalars(FileRun *fileRunRef, const char *moduleName, const char *statisticsName, const Statistics& stat);

    ResultFile *getFileForID(ID id) const; // checks for nullptr

    void addNumericIterationVariableAsScalar(FileRun *fileRunRef, const char *name, const char *value);

    template <class T>
    void collectIDs(IDList& result, std::vector<T> ResultFile::* vec, int type, bool includeComputed, bool includeFields, bool includeItervars) const;

    // unchecked getters are only for internal use by CmpBase in idlist.cc
    const ResultItem& uncheckedGetItem(ID id) const;
    const ScalarResult& uncheckedGetScalar(ID id) const;
    const VectorResult& uncheckedGetVector(ID id) const;
    const StatisticsResult& uncheckedGetStatistics(ID id) const;
    const HistogramResult& uncheckedGetHistogram(ID id) const;

  public:
    ResultFileManager();
    ~ResultFileManager();

#ifdef THREADED
    typedef omnetpp::common::ILock ILock;
    ILock& getReadLock() { return lock.readLock(); }
    ILock& getWriteLock() { return lock.writeLock(); }
    ILock& getReadLock() const { return const_cast<ResultFileManager*>(this)->lock.readLock(); }
    ILock& getWriteLock() const { return const_cast<ResultFileManager*>(this)->lock.writeLock(); }
#endif


    // navigation
    ResultFileList getFiles() const; // filters out NULLs
    const RunList& getRuns() const {return runList;}
    FileRunList getFileRunsInFile(ResultFile *file) const;
    RunList getRunsInFile(ResultFile *file) const;
    ResultFileList getFilesForRun(Run *run, bool includeComputed = false) const;

    const ResultItem& getItem(ID id) const;
    const ScalarResult& getScalar(ID id) const;
    const VectorResult& getVector(ID id) const;
    const StatisticsResult& getStatistics(ID id) const;
    const HistogramResult& getHistogram(ID id) const;
    static int getTypeOf(ID id) {return _type(id);} // SCALAR/VECTOR/STATISTICS/HISTOGRAM

    bool isStaleID(ID id) const;
    bool hasStaleID(const IDList& ids) const;

    // the following are needed for filter combos
    // Note: their return value is allocated with new and callers should delete them
    //FIXME why these functions return bloody pointers???
    //TODO move these group down, next to the getXXXFilterHints() methods
    ResultFileList *getUniqueFiles(const IDList& ids) const;
    RunList *getUniqueRuns(const IDList& ids) const;
    FileRunList *getUniqueFileRuns(const IDList& ids) const;
    StringSet *getUniqueModuleNames(const IDList& ids) const;
    StringSet *getUniqueNames(const IDList& ids) const;
    StringSet *getUniqueModuleAndResultNamePairs(const IDList& ids) const;
    StringSet *getUniqueAttributeNames(const IDList& ids) const;
    StringSet *getUniqueAttributeValues(const IDList& ids, const char *attrName) const;
    StringSet *getUniqueRunAttributeNames(const RunList *runList) const;
    StringSet *getUniqueRunAttributeValues(const RunList& runList, const char *attrName) const;
    StringSet *getUniqueIterationVariableNames(const RunList *runList) const;
    StringSet *getUniqueIterationVariableValues(const RunList& runList, const char *itervarName) const;
    StringSet *getUniqueParamAssignmentKeys(const RunList *runList) const;
    StringSet *getUniqueParamAssignmentValues(const RunList& runList, const char *key) const;

    // getting lists of data items
    IDList getAllScalars(bool includeComputed = false, bool includeFields = true, bool includeItervars = true) const;
    IDList getAllVectors(bool includeComputed = false) const;
    IDList getAllStatistics(bool includeComputed = false) const;
    IDList getAllHistograms(bool includeComputed = false) const;
    IDList getAllItems(bool includeComputed = false, bool includeFields = true, bool includeItervars = true) const;
    IDList getScalarsInFileRun(FileRun *fileRun) const;
    IDList getVectorsInFileRun(FileRun *fileRun) const;
    IDList getStatisticsInFileRun(FileRun *fileRun) const;
    IDList getHistogramsInFileRun(FileRun *fileRun) const;

    /**
     * Get a filtered subset of the input set (of scalars or vectors).
     * All three filter parameters may be null, or (the textual ones)
     * may contain wildcards (*,?).
     * Uses full string match (substrings need asterisks (*) both ends).
     */
    IDList filterIDList(const IDList& idlist,
                        const FileRunList *fileRunFilter,
                        const char *moduleFilter,
                        const char *nameFilter) const;

    IDList filterIDList(const IDList& idlist, const char *pattern) const;

    /**
     * Get a filtered subset of the input set.
     * All three filter parameters may be null, if given they are
     * matched exactly.
     */
    IDList filterIDList(const IDList& idlist, const Run *run, const char *moduleName, const char *name) const;

    /**
     * Get a filtered subset of the input set.
     * All three filter parameters may be null, if given they are
     * matched exactly.
     */
    IDList filterIDList(const IDList& idlist, const char *runName, const char *moduleName, const char *name) const;

    /**
     * Checks that the given pattern is syntactically correct.
     * If not, an exception is thrown, with a (more-or-less useful)
     * message.
     */
    static void checkPattern(const char *pattern);

    // computed vectors
    ID getComputedID(ComputationID computationID, ID inputID) const;
    ID addComputedVector(int vectorId, const char *name, const char *file, const StringMap& attributes, ComputationID computationID, ID inputID, IComputation *node);
    // computedScalars
    ResultFile *getComputedScalarFile() const { return computedScalarFile; }
    IDList getComputedScalarIDs(const IComputation *node) const;
    ID addComputedScalar(const char *name, const char *module, const char *runName, double value, const StringMap& runAttributes, IComputation *node);
    void clearComputedScalars();

    /**
     * loading files. fileName is the file path in the Eclipse workspace;
     * the file is actually read from fileSystemFileName
     */
    ResultFile *loadFile(const char *fileName, const char *fileSystemFileName=nullptr, bool reload=false);
    void unloadFile(ResultFile *file);


    bool isFileLoaded(const char *fileName) const;
    ResultFile *getFile(const char *fileName) const;
    Run *getRunByName(const char *runName) const;
    FileRun *getFileRun(ResultFile *file, Run *run) const;
    ID getItemByName(FileRun *fileRun, const char *module, const char *name) const;

    // filtering files and runs
    RunList filterRunList(const RunList& runList, const char *runNameFilter,
                                                  const StringMap& attrFilter) const;
    ResultFileList filterFileList(const ResultFileList& fileList, const char *filePathPattern) const;

    // select FileRuns which are both in file list and run list (both can be nullptr, meaning '*')
    FileRunList getFileRuns(const ResultFileList *fileList, const RunList *runList) const;

    // utility
    //void dump(ResultFile *fileRef, std::ostream& out) const;

    StringVector *getFilePathFilterHints(const ResultFileList& fileList) const;
    StringVector *getRunNameFilterHints(const RunList& runList) const;
    StringVector *getModuleFilterHints(const IDList& idlist) const;
    StringVector *getNameFilterHints(const IDList& idlist)const;
    StringVector *getResultItemAttributeFilterHints(const IDList& idlist, const char *attrName) const;
    StringVector *getRunAttributeFilterHints(const RunList& runList, const char *attrName) const;
    StringVector *getIterationVariableFilterHints(const RunList& runList, const char *itervarName) const;
    StringVector *getParamAssignmentFilterHints(const RunList& runList, const char *key) const;

    const char *getRunAttribute(ID id, const char *attribute) const;
};

inline const ResultItem& ResultFileManager::uncheckedGetItem(ID id) const
{
    switch (_type(id))
    {
        case SCALAR: return fileList[_fileid(id)]->scalarResults[_pos(id)];
        case VECTOR: return fileList[_fileid(id)]->vectorResults[_pos(id)];
        case STATISTICS: return fileList[_fileid(id)]->statisticsResults[_pos(id)];
        case HISTOGRAM: return fileList[_fileid(id)]->histogramResults[_pos(id)];
        default: throw opp_runtime_error("ResultFileManager: invalid ID: wrong type");
    }
}

inline const ScalarResult& ResultFileManager::uncheckedGetScalar(ID id) const
{
    return fileList[_fileid(id)]->scalarResults[_pos(id)];
}

inline const VectorResult& ResultFileManager::uncheckedGetVector(ID id) const
{
    return fileList[_fileid(id)]->vectorResults[_pos(id)];
}

inline const StatisticsResult& ResultFileManager::uncheckedGetStatistics(ID id) const
{
    return fileList[_fileid(id)]->statisticsResults[_pos(id)];
}

inline const HistogramResult& ResultFileManager::uncheckedGetHistogram(ID id) const
{
    return fileList[_fileid(id)]->histogramResults[_pos(id)];
}

inline ResultFile *ResultFileManager::getFileForID(ID id) const
{
    ResultFile *fileRef = fileList.at(_fileid(id));
    if (fileRef==nullptr)
        throw opp_runtime_error("ResultFileManager: stale ID: its file has already been unloaded");
    return fileRef;
}

inline bool ResultFileManager::isStaleID(ID id) const
{
    return fileList.at(_fileid(id)) == nullptr;
}

class SCAVE_API IResultFileLoader
{
  protected:
    ResultFileManager *resultFileManager;
  public:
    IResultFileLoader(ResultFileManager *resultFileManagerPar) : resultFileManager(resultFileManagerPar) {}
    virtual ~IResultFileLoader() {}
    virtual ResultFile *loadFile(const char *fileName, const char *fileSystemFileName=nullptr, bool reload=false) = 0;
};

} // namespace scave
}  // namespace omnetpp


#endif


