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
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>

#include "idlist.h"
#include "exception.h"
#include "util.h"


class Run;
class File;  //XXX rename to: ResultFile
class FileRun;
class ResultFileManager;

/**
 * Item in an output scalar or output vector file. Represents common properties
 * of an output vector or output scalar.
 */
struct ResultItem
{
    FileRun *fileRunRef;
    std::string *moduleNameRef;
    std::string *nameRef; // scalarname or vectorname
};

/**
 * An output scalar
 */
struct ScalarResult : public ResultItem
{
    double value;
};

/**
 * An output vector
 */
struct VectorResult : public ResultItem
{
    int vectorId;
};

typedef std::vector<ScalarResult> ScalarResults;
typedef std::vector<VectorResult> VectorResults;

typedef std::vector<Run*> RunList;
typedef std::vector<File*> FileList;
typedef std::vector<FileRun *> FileRunList;

/**
 * Represents a loaded scalar or vector file.
 */
struct File
{
    int id;  // position in fileList
    enum {VECTOR_FILE, SCALAR_FILE} fileType;
    ResultFileManager *resultFileManager;
    std::string filePath; // directory + fileName
    std::string directory;
    std::string fileName;
    //XXX RunList runs;
    ScalarResults *scalarResults;
    VectorResults *vectorResults;
};


/**
 * Represents a run in a scalar or vector file.
 * (Presently, vector files may contain only one run.)
 */
struct Run
{
    ResultFileManager *resultFileManager;
    //XXX File *fileRef;
    int runNumber;
    std::string networkName;
    std::string date;
    int lineNum;

    std::string runName; //XXX unique identifier for the run
    std::string fileAndRunName;

    //XXX replace them with string HashMap
    // run attributes
    std::string experimentName;
    std::string measurementName;
    std::string replicationName;
    // plus we need custom labels
};

/**
 * Represents a run in a result file. Such item is needed because
 * result files and runs are in many-to-many relationship: a result file
 * may contain more than one runs (.sca file), and during a simulation run
 * (represented by struct Run) more than one result files are written into
 * (namely, a .vec and a .sca file). And ResultItems refer to a FileRun
 * instead of a File and a Run, to conserve memory.
 */
struct FileRun
{
    File *fileRef;
    Run *runRef;
};

typedef std::set<std::string> StringSet;

typedef std::vector<std::string> StringVector;


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
class ResultFileManager
{
    friend class IDList;  // _type()
  private:
    FileList fileList;
    RunList runList;
    FileRunList fileRunList; // stores the connection between files and runs (many-to-many relationship)

    StringSet moduleNames;
    StringSet names;
    StringSet classNames; // currently not used

    enum {SCALAR=1, VECTOR=2}; // must be 1,2,4,8 etc, because of IDList::itemTypes() XXX add histogram

  private:
    // utility, called during processing one line
    void processLine(char **vec, int numtokens, FileRun *&fileRunRef, File *fileRef, int lineNum);

    static std::string *stringSetFindOrInsert(StringSet& set, const std::string& str);

    // ID: 8 bit type, 24 bit fileid, 32 bit pos
    static int _type(ID id)   {return id >> 56;}
    static int _fileid(ID id) {return (id >> 32) & 0x00fffffful;}
    static int _pos(ID id)    {return id & 0xffffffffUL;}
    static ID _mkID(int type, int fileid, int pos) {
        assert((type>>8)==0 && (fileid>>24)==0 && (pos>>31)<=1);
        return ((ID)type << 56) | ((ID)fileid << 32) | (ID)pos;
    }

    void addScalar(FileRun *fileRunRef, const char *moduleName, const char *scalarName, double value);
    //void addVector(FileRun *fileRunRef, const char *moduleName, const char *vectorName, int vectorId);

    File *addFile();
    Run *addRun();
    FileRun *addFileRun(File *file, Run *run);  // associate a File with a Run

  public:
    ResultFileManager();
    ~ResultFileManager();

    // navigation
    const FileList& getFiles() const  {return fileList;}
    const RunList& getRuns() const {return runList;}
    RunList getRunsInFile(File *file) const;
    FileList getFilesForRun(Run *run) const;

    //IDList getDataInFile(File *file) const;
    //IDList getDataInRun(Run *run) const;
    const ResultItem& getItem(ID id) const;
    bool isVector(ID id) const {return _type(id)==VECTOR;}
    bool isScalar(ID id) const {return _type(id)==SCALAR;}

    // the following are needed for filter combos
    FileList *getUniqueFiles(const IDList& ids) const; //XXX why returns pointer?
    RunList *getUniqueRuns(const IDList& ids) const;
    StringSet *getUniqueModuleNames(const IDList& ids) const;
    StringSet *getUniqueNames(const IDList& ids) const;

    // scalars access
    IDList getAllScalars() const;
    const ScalarResult& getScalar(ID id) const;

    // vectors access
    IDList getAllVectors() const;
    const VectorResult& getVector(ID id) const;

    // unchecked getters are only for internal use by IDList
    const ResultItem& uncheckedGetItem(ID id) const;
    const ScalarResult& uncheckedGetScalar(ID id) const;
    const VectorResult& uncheckedGetVector(ID id) const;

    /**
     * Get a filtered subset of the input set (of scalars or vectors).
     * All three filter parameters may be null, or (the textual ones)
     * may contain wildcards (*,?).
     * Uses full string match (substrings need asterisks (*) both ends).
     */
    IDList getFilteredList(const IDList& idlist,
                           const FileRunList *fileRunFilter,
                           const char *moduleFilter,
                           const char *nameFilter);

    // loading files
    File *loadFile(const char *filename);
    //File *loadVectorFile(const char *filename);
    void unloadFile(File *file);

    bool isFileLoaded(const char *filename) const;
    File *getFile(const char *filename) const;
    Run *getRunByName(const char *runName) const;
    //ID getItemByName(Run *run, const char *module, const char *name) const;

    // utility
    //void dump(File *fileRef, std::ostream& out) const;

    StringVector getRunNameFilterHints(const IDList& idlist);
    StringVector getModuleFilterHints(const IDList& idlist);
    StringVector getNameFilterHints(const IDList& idlist);
};

inline const ResultItem& ResultFileManager::uncheckedGetItem(ID id) const
{
    switch (_type(id))
    {
        case SCALAR: return (*fileList[_fileid(id)]->scalarResults)[_pos(id)];
        case VECTOR: return (*fileList[_fileid(id)]->vectorResults)[_pos(id)];
        default: throw new Exception("");
    }
}

inline const ScalarResult& ResultFileManager::uncheckedGetScalar(ID id) const
{
    return (*fileList[_fileid(id)]->scalarResults)[_pos(id)];
}

inline const VectorResult& ResultFileManager::uncheckedGetVector(ID id) const
{
    return (*fileList[_fileid(id)]->vectorResults)[_pos(id)];
}

#endif


