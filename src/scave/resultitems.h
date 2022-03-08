//=========================================================================
//  RESULTITEMS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_RESULTITEMS_H
#define __OMNETPP_SCAVE_RESULTITEMS_H

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
#include "common/statistics.h"
#include "common/histogram.h"
#include "common/stlutil.h" // keys() etc
#include "idlist.h"
#include "enumtype.h"
#include "scaveutils.h"
#include "enums.h"
#include "filefingerprint.h"

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
class OmnetppResultFileLoader;
class SqliteResultFileLoader;

typedef std::vector<std::string> StringVector;
typedef std::set<std::string> StringSet;
typedef std::map<std::string, std::string> StringMap;
typedef std::vector< std::pair<std::string, std::string> > OrderedKeyValueList;
typedef std::vector< std::pair<Run *, std::string> > RunAndValueList;

using omnetpp::common::Statistics;
using omnetpp::common::Histogram;

/**
 * Item in an output scalar or output vector file. Represents common properties
 * of an output vector or output scalar.
 */
class SCAVE_API ResultItem
{
    friend class ResultFileManager;
    friend class SqliteResultFileLoader;

  public:
    enum DataType { TYPE_INT, TYPE_DOUBLE, TYPE_ENUM };
    enum class FieldNum { NONE=0, COUNT, SUM, SUMWEIGHTS, MEAN, STDDEV, MIN, MAX, NUMBINS, RANGEMIN, RANGEMAX, UNDERFLOWS, OVERFLOWS, STARTTIME, ENDTIME };

  protected:
    FileRun *fileRunRef = nullptr;               // backref to containing FileRun
    const std::string *moduleNameRef = nullptr;  // points into ResultFileManager's StringSet
    const std::string *nameRef = nullptr;        // scalarname or vectorname; points into ResultFileManager's StringSet
    const StringMap *attributes = nullptr;       // metadata in key/value form; pooled (points into ResultFileManager)

  protected:
    ResultItem() {} // for ScalarResult default ctor
    ResultItem(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs);
    void setAttributes(const StringMap& attrs);
    void setAttribute(const std::string& attrName, const std::string& value);

  public:
    ResultItem(const ResultItem& o) = default;
    ResultItem& operator=(const ResultItem& rhs);
    virtual ~ResultItem() {}

    ResultFileManager *getResultFileManager() const;
    virtual int getItemType() const = 0;
    const char *getItemTypeString() const {return itemTypeToString(getItemType());}
    static const char *itemTypeToString(int type);
    //TODO virtual ID getID() const = 0;

    const std::string& getName() const {return *nameRef;}
    const std::string& getModuleName() const {return *moduleNameRef;}

    FileRun *getFileRun() const {return fileRunRef;}
    inline ResultFile *getFile() const;
    inline Run *getRun() const;

    const StringMap& getAttributes() const {return *attributes;}

    const std::string& getAttribute(const std::string& attrName) const {
        StringMap::const_iterator it = attributes->find(attrName);
        return it==attributes->end() ? NULLSTRING : it->second;
    }

    const char *getProperty(const char *propertyName) const; // propertyName: "name", "module", "itervar:numHosts", etc

    /**
     * Returns the data type of this result item (INT,DOUBLE,ENUM).
     * If neither "type" nor "enum" attribute is given it returns DOUBLE.
     */
    DataType getDataType() const;

    /**
     * Returns a pointer to the enum type described by the "enum" attribute
     * or nullptr if no "enum" attribute.
     */
    EnumType *getEnum() const;

    virtual double getScalarField(FieldNum fieldId) const = 0;
};

/**
 * Represents an output scalar
 */
class SCAVE_API ScalarResult : public ResultItem
{
    friend class ResultFileManager;
  private:
    double value;
    ID ownID; // indicates whether this scalar is a field of a histogram/vector/etc; and if so, which field of which item
  protected:
    ScalarResult(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs, double value, ID ownID) :
        ResultItem(fileRun, moduleName, name, attrs), value(value), ownID(ownID) {}
  public:
    ScalarResult() : ResultItem() {} // to be able to create a buffer for ResultFileManager::getScalar()
    virtual int getItemType() const;
    virtual ID getID() const {return ownID;}
    double getValue() const {return value;}
    bool isField() const;
    const ResultItem *getContainingItem() const; // only if this is a field
    virtual double getScalarField(FieldNum fieldId) const;
};

/**
 * Represents a recorded module or channel parameter
 */
class SCAVE_API ParameterResult : public ResultItem
{
    friend class ResultFileManager;
  private:
    std::string value; //TODO stringpool
  protected:
    ParameterResult(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs, const std::string& value) :
        ResultItem(fileRun, moduleName, name, attrs), value(value) {}
  public:
    virtual int getItemType() const;
    const std::string& getValue() const {return value;}
    virtual double getScalarField(FieldNum fieldId) const;
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
    virtual int getItemType() const;
    int getVectorId() const {return vectorId;}
    const std::string& getColumns() const {return columns;}
    const Statistics& getStatistics() const {return stat;}
    eventnumber_t getStartEventNum() const {return startEventNum;}
    eventnumber_t getEndEventNum() const {return endEventNum;}
    simultime_t getStartTime() const {return startTime;}
    simultime_t getEndTime() const {return endTime;}
    InterpolationMode getInterpolationMode() const; // defaults to UNSPECIFIED
    virtual double getScalarField(FieldNum fieldId) const;
    static FieldNum *getAvailableFields(); // zero-terminated array
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
    Statistics stat;
  protected:
    StatisticsResult(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs, const Statistics& stat) :
        ResultItem(fileRun, moduleName, name, attrs), stat(stat) {}
  public:
    virtual int getItemType() const;
    const Statistics& getStatistics() const {return stat;}
    virtual double getScalarField(FieldNum fieldId) const;
    static FieldNum *getAvailableFields(); // zero-terminated array
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
  public:
    virtual int getItemType() const;
    const Histogram& getHistogram() const {return bins;}
    virtual double getScalarField(FieldNum fieldId) const;
    static FieldNum *getAvailableFields(); // zero-terminated array
};

typedef std::vector<ScalarResult> ScalarResults;
typedef std::vector<ParameterResult> ParameterResults;
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

  public:
    enum FileType { FILETYPE_OMNETPP, FILETYPE_SQLITE };

  private:
    ResultFileManager *resultFileManager; // backref to containing ResultFileManager
    FileRunList fileRuns; // associated fileRuns
    std::string displayNameFolderPart; // folder (project) part of displayName
    std::string displayNameFilePart; // file name part of displayName
    std::string displayName; // practically: path in the Eclipse workspace (IFile.getFullPath())
    std::string fileSystemFilePath; // path to underlying file (for fopen())
    std::string inputName; // pattern by which it was loaded in the IDE, e.g. "results/**/*.vec"
    FileFingerprint fingerprint; // read-time file size and date/time
    FileType fileType;

  public:
    ResultFileManager *getResultFileManager() const {return resultFileManager;}
    const std::string& getDirectory() const {return displayNameFolderPart;}
    const std::string& getFileName() const {return displayNameFilePart;}
    const std::string& getFilePath() const {return displayName;}
    const std::string& getFileSystemFilePath() const {return fileSystemFilePath;}
    const std::string& getInputName() const {return inputName;}
    const FileFingerprint& getFingerprint() const {return fingerprint;}
    const int64_t getFileSize() const {return fingerprint.fileSize;}
    const int64_t getModificationTime() const {return fingerprint.lastModified;}
    FileType getFileType() const {return fileType;}
    const FileRunList& getFileRuns() const {return fileRuns;}
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
    FileRunList fileRuns; // associated fileRuns
    ResultFileManager *resultFileManager; // backref to containing ResultFileManager
    StringMap attributes;  // run attributes, such as configname, runnumber, network, datetime, processid, etc.
    StringMap itervars;  // iteration variables (${} notation in omnetpp.ini)
    OrderedKeyValueList configEntries; // configuration entries from the ini file and command line

  private:
    Run(const std::string& runName, ResultFileManager *manager) : runName(runName), resultFileManager(manager) {}
    void setAttribute(const std::string& attrName, const std::string& value) {attributes[attrName] = value;}
    void addParamAssignmentEntry(const std::string& key, const std::string& value) { addConfigEntry(key, value); /* TODO remove */ }
    void addConfigEntry(const std::string& key, const std::string& value) {configEntries.push_back(std::make_pair(key,value));}

  public:
    ResultFileManager *getResultFileManager() const {return resultFileManager;}
    const std::string& getRunName() const {return runName;}
    const FileRunList& getFileRuns() const {return fileRuns;}

    const StringMap& getAttributes() const {return attributes;}
    const std::string& getAttribute(const std::string& attrName) const;

    const StringMap& getIterationVariables() const {return itervars;}
    const std::string& getIterationVariable(const std::string& name) const;

    const OrderedKeyValueList& getConfigEntries() const {return configEntries;}
    const std::string& getConfigValue(const std::string& configKey) const;
    static bool isParamAssignmentConfigKey(const std::string& key);
    static bool isGlobalOptionConfigKey(const std::string& key);
    const OrderedKeyValueList getParamAssignmentConfigEntries() const; // a subset of getConfigEntries()
    const OrderedKeyValueList getNonParamAssignmentConfigEntries() const; // a subset of getConfigEntries()

    const char *getProperty(const char *propertyName) const; // propertyName: "name", "module", "itervar:numHosts", etc
};

/**
 * Represents a run in a result file. Such item is needed because
 * result files and runs are in many-to-many relationship: a result file
 * may contain more than one runs (.sca file), and during a simulation run
 * (represented by class Run) more than one result files are written into
 * (namely, a .vec and a .sca file, or sometimes many of them).
 */
class SCAVE_API FileRun
{
    friend class IDList;
    friend class ResultItem;
    friend class ResultFileManager;
    friend class OmnetppResultFileLoader;
    friend class SqliteResultFileLoader;

  private:
    int id;  // position in fileRunList
    ResultFile *fileRef;
    Run *runRef;

    ScalarResults scalarResults;
    ParameterResults parameterResults;
    VectorResults vectorResults;
    StatisticsResults statisticsResults;
    HistogramResults histogramResults;
  public:
    ResultFile *getFile() const {return fileRef;}
    Run *getRun() const {return runRef;}
};

inline ResultFile *ResultItem::getFile() const {return fileRunRef->fileRef;}
inline Run *ResultItem::getRun() const {return fileRunRef->runRef;}

}  // namespace scave
}  // namespace omnetpp


#endif
