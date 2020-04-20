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
#include <unordered_set>

#include "common/exception.h"
#include "common/commonutil.h"
#include "common/stlutil.h" // keys() etc
#include "resultitems.h"
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

class SCAVE_API InterruptedFlag {
  public:
    bool flag = false;
};

class SCAVE_API InterruptedException : public opp_runtime_error {
  public:
    InterruptedException(const char *msg="Interrupted") : opp_runtime_error(msg) {}
};

class SCAVE_API IDListsByRun
{
  private:
    std::map<Run*,IDList> map;
  public:
    IDListsByRun() {} // required by swig
    IDListsByRun(std::map<Run*,IDList>& map) {this->map = std::move(map);}
    RunList getRuns() {return omnetpp::common::keys(map);}
    IDList& getIDList(Run *run) {return map.at(run);}
};

class SCAVE_API IDListsByFile
{
  private:
    std::map<ResultFile*,IDList> map;
  public:
    IDListsByFile() {} // required by swig
    IDListsByFile(std::map<ResultFile*,IDList>& map) {this->map = std::move(map);}
    ResultFileList getFiles() {return omnetpp::common::keys(map);}
    IDList& getIDList(ResultFile *file) {return map.at(file);}
};

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
  public:
    /**
     * Options for loadFile().
     */
    enum LoadFlags {
        // What to do if file is already loaded:
        RELOAD = (1<<0), // reload if already loaded
        RELOAD_IF_CHANGED = (1<<1), // reload only if file changed since being loaded (this is the default)
        NEVER_RELOAD = (1<<2), // don't reload if already loaded

        // What to do if index file is missing or out of date:
        ALLOW_INDEXING = (1<<3), // create index (this is the default)
        SKIP_IF_NO_INDEX = (1<<4), // don't load
        ALLOW_LOADING_WITHOUT_INDEX = (1<<5), // load without creating an index (i.e. scan .vec file instead of .vci file)

        // What to do if there is a lock file (indicating that the file is being written to)
        SKIP_IF_LOCKED = (1<<6), // don't load (this is the default)
        IGNORE_LOCK_FILE = (1<<7), // pretend lock file doesn't exist

        VERBOSE = (1<<8), // print on stdout what it's doing

        LOADFLAGS_DEFAULTS = RELOAD_IF_CHANGED | ALLOW_INDEXING | SKIP_IF_LOCKED
    };


  public:
    friend class ResultItem; // moduleNames, names
    friend class IDList;  // _type()
    friend class Run;  // _pos()
    friend class CmpBase; // uncheckedGet...()
    friend class OmnetppResultFileLoader;
    friend class SqliteResultFileLoader;
  private:
    std::unordered_set<ResultFile*> fileList;
    std::unordered_set<Run*> runList;

    FileRunList fileRunList; // contains nullptr for unloaded entries

    std::map<std::string, ResultFile *> filesByDisplayName;
    std::map<std::string, Run *> runsByName;

    // module names and variable names are stringpooled to conserve space
    ScaveStringPool moduleNames;
    ScaveStringPool names;
    ScaveStringPool classNames; // currently not used

#ifdef THREADED
    omnetpp::common::ReentrantReadWriteLock lock;
#endif

  public:
    enum {PARAMETER = 1<<0, SCALAR = 1<<1, STATISTICS = 1<<2, HISTOGRAM = 1<<3, VECTOR = 1<<4}; // must be 1,2,4,8 etc, because of IDList::getItemTypes()

  private:
    // ID: 8 bit type, 24 bit filerunid, 32 bit pos
    static int _type(ID id)   {return (id >> 56) & 0xffUL;}
    static int _filerunid(ID id) {return (id >> 32) & 0x00fffffful;}
    static int _pos(ID id)    {return id & 0xffffffffUL;}
    static ID _mkID(int type, int fileid, int pos) {
        assert((type>>8)==0 && (fileid>>24)==0 && ((pos>>31)==0 || (pos>>31)==-1)); // can't shift by 32, and int is 32 bits
        return ((ID)type << 56) | ((ID)fileid << 32) | (ID)pos;
    }

    // utility functions called while loading a result file
    ResultFile *addFile(const char *displayName, const char *fileSystemFileName, ResultFile::FileType fileType);
    Run *addRun(const std::string& runName);
    FileRun *addFileRun(ResultFile *file, Run *run);
    Run *getOrAddRun(const std::string& runName);
    FileRun *getOrAddFileRun(ResultFile *file, Run *run);

    int addScalar(FileRun *fileRunRef, const char *moduleName, const char *scalarName, const StringMap& attrs, double value, bool isField);
    int addParameter(FileRun *fileRunRef, const char *moduleName, const char *paramName, const StringMap& attrs, const std::string& value);
    int addVector(FileRun *fileRunRef, int vectorId, const char *moduleName, const char *vectorName, const StringMap& attrs, const char *columns);
    int addStatistics(FileRun *fileRunRef, const char *moduleName, const char *statisticsName, const Statistics& stat, const StringMap& attrs);
    int addHistogram(FileRun *fileRunRef, const char *moduleName, const char *histogramName, const Statistics& stat, const Histogram& bins, const StringMap& attrs);
    void addStatisticsFieldsAsScalars(FileRun *fileRunRef, const char *moduleName, const char *statisticsName, const Statistics& stat);

    FileRun *getFileRunForID(ID id) const; // checks for nullptr

    template <class T>
    void collectIDs(IDList& result, FileRun *fileRun, std::vector<T> FileRun::* vec, int type, bool includeFields) const;

    // unchecked getters are only for internal use by CmpBase in idlist.cc
    const ResultItem& uncheckedGetItem(ID id) const;
    const ScalarResult& uncheckedGetScalar(ID id) const;
    const ParameterResult& uncheckedGetParameter(ID id) const;
    const VectorResult& uncheckedGetVector(ID id) const;
    const StatisticsResult& uncheckedGetStatistics(ID id) const;
    const HistogramResult& uncheckedGetHistogram(ID id) const;

  public:
    ResultFileManager();
    ~ResultFileManager();
    void clear();

#ifdef THREADED
    typedef omnetpp::common::ILock ILock;
    ILock& getReadLock() { return lock.readLock(); }
    ILock& getWriteLock() { return lock.writeLock(); }
    ILock& getReadLock() const { return const_cast<ResultFileManager*>(this)->lock.readLock(); }
    ILock& getWriteLock() const { return const_cast<ResultFileManager*>(this)->lock.writeLock(); }
#endif


    // navigation
    ResultFileList getFiles() const;
    RunList getRuns() const;
    const FileRunList& getFileRunsInFile(ResultFile *file) const {return file->fileRuns;}
    const FileRunList& getFileRunsForRun(Run *run) const {return run->fileRuns;}
    RunList getRunsInFile(ResultFile *file) const;
    int getNumRunsInFile(ResultFile *file) const {return getRunsInFile(file).size();} // for InputsPage
    ResultFileList getFilesForRun(Run *run) const;
    ResultFileList getFilesForInput(const char *inputName) const;

    const ResultItem& getItem(ID id) const;
    const ScalarResult& getScalar(ID id) const;
    const ParameterResult& getParameter(ID id) const;
    const VectorResult& getVector(ID id) const;
    const StatisticsResult& getStatistics(ID id) const;
    const HistogramResult& getHistogram(ID id) const;
    static int getTypeOf(ID id) {return _type(id);} // SCALAR/VECTOR/STATISTICS/HISTOGRAM

    bool isStaleID(ID id) const;
    bool hasStaleID(const IDList& ids) const;

    // the following are needed for filter combos
    // Note: their return value is allocated with new and callers should delete them
    FileRunList getUniqueFileRuns(const IDList& ids) const;
    ResultFileList getUniqueFiles(const IDList& ids) const;
    RunList getUniqueRuns(const IDList& ids) const;
    ResultFileList getUniqueFiles(const FileRunList& fileRunList) const; // helper
    RunList getUniqueRuns(const FileRunList& fileRunList) const; // helper

    StringSet getUniqueModuleNames(const IDList& ids) const;
    StringSet getUniqueResultNames(const IDList& ids) const;
    StringSet getUniqueModuleAndResultNamePairs(const IDList& ids) const;
    StringSet getUniqueResultAttributeNames(const IDList& ids) const;
    StringSet getUniqueResultAttributeValues(const IDList& ids, const char *attrName) const;
    StringSet getUniqueRunAttributeNames(const RunList& runList) const;
    StringSet getUniqueRunAttributeValues(const RunList& runList, const char *attrName) const;
    StringSet getUniqueIterationVariableNames(const RunList& runList) const;
    StringSet getUniqueIterationVariableValues(const RunList& runList, const char *itervarName) const;
    StringSet getUniqueConfigKeys(const RunList& runList) const;
    StringSet getUniqueConfigValues(const RunList& runList, const char *key) const;
    StringSet getUniqueParamAssignmentKeys(const RunList& runList) const;
    IDListsByRun getPartitionByRun(const IDList& ids) const;
    IDListsByFile getPartitionByFile(const IDList& ids) const;

    // getting lists of data items
    IDList getAllScalars(bool includeFields = true) const;
    IDList getAllParameters() const;
    IDList getAllVectors() const;
    IDList getAllStatistics() const;
    IDList getAllHistograms() const;
    IDList getAllItems(bool includeFields = true) const;
    IDList getScalarsInFileRun(FileRun *fileRun) const;
    IDList getParametersInFileRun(FileRun *fileRun) const;
    IDList getVectorsInFileRun(FileRun *fileRun) const;
    IDList getStatisticsInFileRun(FileRun *fileRun) const;
    IDList getHistogramsInFileRun(FileRun *fileRun) const;

    // these ones are called from InputsTree; TODO would be nice to make them constant-cost
    int getNumScalarsInFileRun(FileRun *fileRun) const {return getScalarsInFileRun(fileRun).size();}
    int getNumParametersInFileRun(FileRun *fileRun) const {return getParametersInFileRun(fileRun).size();}
    int getNumVectorsInFileRun(FileRun *fileRun) const {return getVectorsInFileRun(fileRun).size();}
    int getNumStatisticsInFileRun(FileRun *fileRun) const {return getStatisticsInFileRun(fileRun).size();}
    int getNumHistogramsInFileRun(FileRun *fileRun) const {return getHistogramsInFileRun(fileRun).size();}

    // These are the ones that are called from Python. They return (runID, name) pairs (the values are queried later).

    // these are the ones that are called from opp_scavetool
    RunAndValueList getMatchingItervars(const RunList& runs, const char *pattern) const;
    RunAndValueList getMatchingRunattrs(const RunList& runs, const char *pattern) const;
    RunAndValueList getMatchingConfigEntries(const RunList& runs, const char *pattern) const;
    RunAndValueList getMatchingParamAssignments(const RunList& runs, const char *pattern) const;
    RunAndValueList getMatchingNonParamAssignmentConfigEntries(const RunList& runs, const char *pattern) const;

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

    IDList filterIDList(const IDList& idlist, const char *pattern, int limit=-1, const InterruptedFlag& interrupted=InterruptedFlag()) const;

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

    RunList filterRunList(const RunList& runlist, const char *pattern) const;

    /**
     * Checks that the given pattern is syntactically correct.
     * If not, an exception is thrown, with a (more-or-less useful)
     * message.
     */
    static void checkPattern(const char *pattern);

    /**
     * Loading files. displayName is the file path in the Eclipse workspace;
     * the file is actually read from fileSystemFileName.
     */
    ResultFile *loadFile(const char *displayName, const char *fileSystemFileName, int flags, InterruptedFlag *interrupted);
    void setFileInput(ResultFile *file, const char *inputName); // for the "Inputs" page in the IDE
    void unloadFile(ResultFile *file);
    void unloadFile(const char *displayName);

    bool isFileLoaded(const char *displayName) const;
    ResultFile *getFile(const char *displayName) const;
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

    StringVector getFilePathFilterHints(const ResultFileList& fileList) const;
    StringVector getRunNameFilterHints(const RunList& runList) const;
    StringVector getModuleFilterHints(const IDList& idlist) const;
    StringVector getResultNameFilterHints(const IDList& idlist)const;
    StringVector getResultItemAttributeFilterHints(const IDList& idlist, const char *attrName) const;
    StringVector getRunAttributeFilterHints(const RunList& runList, const char *attrName) const;
    StringVector getIterationVariableFilterHints(const RunList& runList, const char *itervarName) const;
    StringVector getConfigEntryFilterHints(const RunList& runList, const char *key) const;
    StringVector getParamAssignmentFilterHints(const RunList& runList, const char *key) const;

    const char *getRunAttribute(ID id, const char *attribute) const;
};

inline const ResultItem& ResultFileManager::uncheckedGetItem(ID id) const
{
    switch (_type(id))
    {
        case SCALAR: return fileRunList[_filerunid(id)]->scalarResults[_pos(id)];
        case PARAMETER: return fileRunList[_filerunid(id)]->parameterResults[_pos(id)];
        case VECTOR: return fileRunList[_filerunid(id)]->vectorResults[_pos(id)];
        case STATISTICS: return fileRunList[_filerunid(id)]->statisticsResults[_pos(id)];
        case HISTOGRAM: return fileRunList[_filerunid(id)]->histogramResults[_pos(id)];
        default: throw opp_runtime_error("ResultFileManager: invalid ID: wrong type");
    }
}

inline const ScalarResult& ResultFileManager::uncheckedGetScalar(ID id) const
{
    return fileRunList[_filerunid(id)]->scalarResults[_pos(id)];
}

inline const ParameterResult& ResultFileManager::uncheckedGetParameter(ID id) const
{
    return fileRunList[_filerunid(id)]->parameterResults[_pos(id)];
}

inline const VectorResult& ResultFileManager::uncheckedGetVector(ID id) const
{
    return fileRunList[_filerunid(id)]->vectorResults[_pos(id)];
}

inline const StatisticsResult& ResultFileManager::uncheckedGetStatistics(ID id) const
{
    return fileRunList[_filerunid(id)]->statisticsResults[_pos(id)];
}

inline const HistogramResult& ResultFileManager::uncheckedGetHistogram(ID id) const
{
    return fileRunList[_filerunid(id)]->histogramResults[_pos(id)];
}

inline FileRun *ResultFileManager::getFileRunForID(ID id) const
{
    FileRun *fileRun = fileRunList.at(_filerunid(id));
    if (fileRun == nullptr)
        throw opp_runtime_error("ResultFileManager: stale ID: its file has already been unloaded");
    return fileRun;
}

inline bool ResultFileManager::isStaleID(ID id) const
{
    return fileRunList.at(_filerunid(id)) == nullptr;
}

class SCAVE_API IResultFileLoader
{
  protected:
    ResultFileManager *resultFileManager;
  public:
    IResultFileLoader(ResultFileManager *resultFileManagerPar) : resultFileManager(resultFileManagerPar) {}
    virtual ~IResultFileLoader() {}
    virtual ResultFile *loadFile(const char *displayName, const char *fileSystemFileName) = 0;
};

} // namespace scave
}  // namespace omnetpp


#endif
