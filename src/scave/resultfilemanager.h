//=========================================================================
//  RESULTFILEMANAGER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _RESULTFILEMANAGER_H_
#define _RESULTFILEMANAGER_H_

#include <assert.h>
#include <math.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>

#include "idlist.h"
#include "enumtype.h"
#include "exception.h"
#include "commonutil.h"
#include "statistics.h"

class Run;
class ResultFile;
class FileRun;
class ResultFileManager;

typedef std::map<std::string, std::string> StringMap;

/**
 * Item in an output scalar or output vector file. Represents common properties
 * of an output vector or output scalar.
 */
struct SCAVE_API ResultItem
{
    enum Type { TYPE_INT, TYPE_DOUBLE, TYPE_ENUM };

    FileRun *fileRunRef; // backref to containing FileRun
    std::string *moduleNameRef; // points into ResultFileManager's StringSet
    std::string *nameRef; // scalarname or vectorname; points into ResultFileManager's StringSet
    StringMap attributes; // metadata in key/value form

    /**
     * Returns the type of this result item (INT,DOUBLE,ENUM).
     * If neither "type" nor "enum" attribute is given it returns DOUBLE.
     */
    Type getType() const;
    /**
     * Returns a pointer to the enum type described by the "enum" attribute
     * or NULL if no "enum" attribute.
     */
    EnumType* getEnum() const;
};

/**
 * Represents an output scalar
 */
struct SCAVE_API ScalarResult : public ResultItem
{
    double value;
};

/**
 * Represents an output vector. This is only the declaration,
 * actual vector data are not kept in memory.
 */
struct SCAVE_API VectorResult : public ResultItem
{
    enum InterpolationMode { UNSPECIFIED, NONE, SAMPLE_HOLD, BACKWARD_SAMPLE_HOLD, LINEAR };

    int vectorId;
    std::string columns;
    Statistics stat;

    long count()      const { return stat.count(); }
    double min()      const { return stat.min(); }
    double max()      const { return stat.max(); }
    double mean()     const { return stat.mean(); }
    double variance() const { return stat.variance(); } 
    double stddev()   const { return stat.stddev(); }

    /**
     * Returns the value of the "interpolation-mode" attribute as an InterpolationMode,
     * defaults to UNSPECIFIED.
     */
    InterpolationMode getInterpolationMode() const;
};

/**
 * Represents a histogram.
 */
struct SCAVE_API HistogramResult : public ResultItem
{
    //TODO add data members; also TODO implement loading histograms
};

typedef std::vector<ScalarResult> ScalarResults;
typedef std::vector<VectorResult> VectorResults;
typedef std::vector<HistogramResult> HistogramResults;

typedef std::vector<Run*> RunList;
typedef std::vector<ResultFile*> ResultFileList;
typedef std::vector<FileRun *> FileRunList;

/**
 * Represents a loaded scalar or vector file.
 */
struct SCAVE_API ResultFile
{
    int id;  // position in fileList
    ResultFileManager *resultFileManager; // backref to containing ResultFileManager
    std::string fileSystemFilePath; // directory+fileName of underlying file (for fopen())
    std::string directory; // directory's location in the Eclipse workspace
    std::string fileName; // file name
    std::string filePath; // workspace directory + fileName
    ScalarResults scalarResults;
    VectorResults vectorResults;
    HistogramResults histogramResults;
    int numLines;
    int numUnrecognizedLines;
};

/**
 * Represents a run. If several scalar or vector files contain
 * the same run (i.e. runName is the same), they will share
 * the same Run object.
 */
struct SCAVE_API Run
{
    std::string runName; // unique identifier for the run, "runId"
    ResultFileManager *resultFileManager; // backref to containing ResultFileManager

    // various attributes of the run are stored in a string map.
    // keys include: runNumber, networkName, datetime, experiment, measurement, replication
    StringMap attributes;
    int runNumber; // this is stored separately as well, for convenience

    // module parameters: maps wildcard pattern to value
    StringMap moduleParams;

    // utility methods to non-disruptive access to the maps (note that evaluating
    // attributes["nonexisting-key"] would create a blank entry with that key!)
    const char *getAttribute(const char *attrName) const {
        StringMap::const_iterator it = attributes.find(attrName);
        return it==attributes.end() ? NULL : it->second.c_str();
    }
    const char *getModuleParam(const char *namePattern) const {
        StringMap::const_iterator it = moduleParams.find(namePattern);
        return it==attributes.end() ? NULL : it->second.c_str();
    }
};


/**
 * Represents a run in a result file. Such item is needed because
 * result files and runs are in many-to-many relationship: a result file
 * may contain more than one runs (.sca file), and during a simulation run
 * (represented by struct Run) more than one result files are written into
 * (namely, a .vec and a .sca file, or sometimes many of them).
 * And ResultItems refer to a FileRun instead of a ResultFile and a Run,
 * to conserve memory.
 */
struct SCAVE_API FileRun
{
    ResultFile *fileRef;
    Run *runRef;
};

typedef std::set<std::string> StringSet;
typedef std::vector<std::string> StringVector;
//typedef std::map<ID,ID> IDMap;
typedef std::map<std::pair<long, ID> , ID> ComputedIDCache;

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
    friend class IDList;  // _type()
  private:
    // List of files loaded. This vector can have holes (NULLs) in it due to
    // unloaded files. The "id" field of ResultFile is the index into this vector.
    // It is not allowed to move elements, because IDs contain the file's index in them.
    ResultFileList fileList;

    // List of unique runs in the files. If several files contain the same runName,
    // it will generate only one Run entry here.
    RunList runList;

    // ResultFiles and Runs have many-to-many relationship. This is where we store
    // their relations (instead of having a collection in both). ResultItems also
    // contain a FileRun pointer instead of separate ResultFile and Run pointers.
    FileRunList fileRunList;

    // module names and variable names are stringpooled to conserve space
    StringSet moduleNames;
    StringSet names;
    StringSet classNames; // currently not used

    ComputedIDCache computedIDCache;

  public:
    enum {SCALAR=1, VECTOR=2, HISTOGRAM=4}; // must be 1,2,4,8 etc, because of IDList::itemTypes()

  private:
    static std::string *stringSetFindOrInsert(StringSet& set, const std::string& str);

    // ID: 8 bit type, 24 bit fileid, 32 bit pos
    static int _type(ID id)   {return id >> 56;}
    static int _fileid(ID id) {return (id >> 32) & 0x00fffffful;}
    static int _pos(ID id)    {return id & 0xffffffffUL;}
    static ID _mkID(int type, int fileid, int pos) {
        assert((type>>8)==0 && (fileid>>24)==0 && (pos>>31)<=1);
        return ((ID)type << 56) | ((ID)fileid << 32) | (ID)pos;
    }

    // utility functions called while loading a result file
    ResultFile *addFile();
    Run *addRun();
    FileRun *addFileRun(ResultFile *file, Run *run);  // associates a ResultFile with a Run

    void processLine(char **vec, int numTokens, FileRun *&fileRunRef, ResultItem *&resultItemRef, ResultFile *fileRef, int lineNum);
    void addScalar(FileRun *fileRunRef, const char *moduleName, const char *scalarName, double value);

    ResultFile *getFileForID(ID id) const; // checks for NULL
    void loadVectorsFromIndex(const char *filename, ResultFile *fileRef);

  public:
    ResultFileManager();
    ~ResultFileManager();

    // navigation
    ResultFileList getFiles() const; // filters out NULLs
    const RunList& getRuns() const {return runList;}
    RunList getRunsInFile(ResultFile *file) const;
    ResultFileList getFilesForRun(Run *run) const;

    const ResultItem& getItem(ID id) const;
    const ScalarResult& getScalar(ID id) const;
    const VectorResult& getVector(ID id) const;
    const HistogramResult& getHistogram(ID id) const;
    int getTypeOf(ID id) const {return _type(id);} // SCALAR/VECTOR/HISTOGRAM

    // the following are needed for filter combos
    ResultFileList *getUniqueFiles(const IDList& ids) const; //XXX why returns pointer?
    RunList *getUniqueRuns(const IDList& ids) const;
    FileRunList *getUniqueFileRuns(const IDList& ids) const;
    StringSet *getUniqueModuleNames(const IDList& ids) const;
    StringSet *getUniqueNames(const IDList& ids) const;

    // getting lists of data items
    IDList getAllScalars() const;
    IDList getAllVectors() const;
    IDList getAllHistograms() const;
    IDList getScalarsInFileRun(FileRun *fileRun) const;
    IDList getVectorsInFileRun(FileRun *fileRun) const;
    IDList getHistogramsInFileRun(FileRun *fileRun) const;

    // unchecked getters are only for internal use by IDList
    const ResultItem& uncheckedGetItem(ID id) const;
    const ScalarResult& uncheckedGetScalar(ID id) const;
    const VectorResult& uncheckedGetVector(ID id) const;
    const HistogramResult& uncheckedGetHistogram(ID id) const;

    /**
     * Get a filtered subset of the input set (of scalars or vectors).
     * All three filter parameters may be null, or (the textual ones)
     * may contain wildcards (*,?).
     * Uses full string match (substrings need asterisks (*) both ends).
     */
    IDList filterIDList(const IDList& idlist,
                        const FileRunList *fileRunFilter,
                        const char *moduleFilter,
                        const char *nameFilter);

    IDList filterIDList(const IDList &idlist, const char *pattern) const;

    /**
     * Checks that the given pattern is syntactically correct.
     * If not, an exception is thrown, with a (more-or-less useful)
     * message.
     */
    static void checkPattern(const char *pattern);

    // computed data 
    ID addComputedVector(const char *name, long nodeID, ID inputID);
    ID getComputedVector(long nodeID, ID inputID);

    /**
     * loading files. fileName is the file path in the Eclipse workspace;
     * the file is actually read from fileSystemFileName
     */
    ResultFile *loadFile(const char *fileName, const char *fileSystemFileName=NULL);
    void unloadFile(ResultFile *file);

    bool isFileLoaded(const char *fileName) const;
    ResultFile *getFile(const char *fileName) const;
    Run *getRunByName(const char *runName) const;
    FileRun *getFileRun(ResultFile *file, Run *run) const;
    ID getItemByName(FileRun *fileRun, const char *module, const char *name) const;

    // get unique values of an attribute ("experiment",etc) in a set of Runs
    StringVector getUniqueAttributeValues(const RunList& runList, const char *attrName) const;

    // filtering files and runs
    RunList filterRunList(const RunList& runList, const char *runNameFilter,
                                                  const StringMap& attrFilter) const;
    ResultFileList filterFileList(const ResultFileList& fileList, const char *filePathPattern) const;

    // select FileRuns which are both in file list and run list (both can be NULL, meaning '*')
    FileRunList getFileRuns(const ResultFileList *fileList, const RunList *runList) const;

    // utility
    //void dump(ResultFile *fileRef, std::ostream& out) const;

    StringVector getFileAndRunNumberFilterHints(const IDList& idlist);
    StringVector getModuleFilterHints(const IDList& idlist);
    StringVector getNameFilterHints(const IDList& idlist);
};

inline const ResultItem& ResultFileManager::uncheckedGetItem(ID id) const
{
    switch (_type(id))
    {
        case SCALAR: return fileList[_fileid(id)]->scalarResults[_pos(id)];
        case VECTOR: return fileList[_fileid(id)]->vectorResults[_pos(id)];
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

inline const HistogramResult& ResultFileManager::uncheckedGetHistogram(ID id) const
{
    return fileList[_fileid(id)]->histogramResults[_pos(id)];
}

inline ResultFile *ResultFileManager::getFileForID(ID id) const
{
    ResultFile *fileRef = fileList.at(_fileid(id));
    if (fileRef==NULL)
        throw opp_runtime_error("ResultFileManager: stale ID: its file has already been unloaded");
    return fileRef;
}


#endif


