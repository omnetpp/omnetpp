//=========================================================================
//  RESULTFILEMANAGER.CC - part of
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

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <utility>
#include <functional>
#include "common/opp_ctype.h"
#include "common/matchexpression.h"
#include "common/patternmatcher.h"
#include "common/filereader.h"
#include "common/linetokenizer.h"
#include "common/stringtokenizer.h"
#include "common/filereader.h"
#include "common/fileutil.h"
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "common/unitconversion.h"
#include "omnetpp/platdep/platmisc.h"
#include "scaveutils.h"
#include "scaveexception.h"
#include "sqliteresultfileutils.h"
#include "resultfilemanager.h"
#include "omnetppresultfileloader.h"
#include "sqliteresultfileloader.h"
#include "vectorfileindex.h"


#ifdef THREADED
#define READER_MUTEX    Mutex __reader_mutex_(getReadLock());
#define WRITER_MUTEX    Mutex __writer_mutex_(getWriteLock());
#else
#define READER_MUTEX
#define WRITER_MUTEX
#endif

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

const std::string NULLSTRING = "";

ResultItem::ResultItem(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs) :
        fileRunRef(fileRun), moduleNameRef(nullptr), nameRef(nullptr), attributes(attrs)
{
    ResultFileManager *resultFileManager = fileRun->fileRef->getResultFileManager();
    moduleNameRef = resultFileManager->moduleNames.insert(moduleName);
    nameRef = resultFileManager->names.insert(name);
}

ResultItem& ResultItem::operator=(const ResultItem& rhs)
{
    if (this == &rhs)
        return *this;

    fileRunRef = rhs.fileRunRef;
    moduleNameRef = rhs.moduleNameRef;
    nameRef = rhs.nameRef;
    attributes = rhs.attributes;

    return *this;
}

ResultItem::DataType ResultItem::getDataType() const
{
    StringMap::const_iterator it = attributes.find("type");
    if (it == attributes.end()) {
        if (attributes.find("enum") != attributes.end())
            return TYPE_ENUM;
        else
            return TYPE_DOUBLE;
    }
    else {
        const std::string& type = it->second;
        if (type == "int")
            return TYPE_INT;
        else if (type == "double")
            return TYPE_DOUBLE;
        else if (type == "enum")
            return TYPE_ENUM;
        else
            throw opp_runtime_error("Unknown type: %s", type.c_str());
    }
}

EnumType *ResultItem::getEnum() const
{
    StringMap::const_iterator it = attributes.find("enum");
    if (it != attributes.end()) {
        EnumType *enumPtr = new EnumType();
        enumPtr->parseFromString(it->second.c_str());
        return enumPtr;
    }
    else {
        return nullptr;
    }
}

int ScalarResult::getItemType() const
{
    return ResultFileManager::SCALAR;
}

int ParameterResult::getItemType() const
{
    return ResultFileManager::PARAMETER;
}

int VectorResult::getItemType() const
{
    return ResultFileManager::VECTOR;
}

InterpolationMode VectorResult::getInterpolationMode() const
{
    StringMap::const_iterator it = attributes.find("interpolationmode");
    if (it != attributes.end()) {
        const std::string& mode = it->second;
        if (mode == "none")
            return NONE;
        else if (mode == "sample-hold")
            return SAMPLE_HOLD;
        else if (mode == "backward-sample-hold")
            return BACKWARD_SAMPLE_HOLD;
        else if (mode == "linear")
            return LINEAR;
        else
            throw opp_runtime_error("Unknown interpolation mode: %s", mode.c_str());
    }
    else {
        return UNSPECIFIED;
    }
}

int StatisticsResult::getItemType() const
{
    return ResultFileManager::STATISTICS;
}

int HistogramResult::getItemType() const
{
    return ResultFileManager::HISTOGRAM;
}

const std::string& Run::getAttribute(const std::string& attrName) const
{
    auto it = attributes.find(attrName);
    return it==attributes.end() ? NULLSTRING : it->second;
}

const std::string& Run::getIterationVariable(const std::string& name) const
{
    auto it = itervars.find(name);
    return it==itervars.end() ? NULLSTRING : it->second;
}

const std::string& Run::getConfigValue(const std::string& key) const
{
    for (auto& p : configEntries)  // TODO some kind of ordered map would be better (e.g. std::map plus an std::vector<string> to store the order)
        if (p.first == key)
            return p.second;
    return NULLSTRING;
}

static bool isKeyParamAssignment(const std::string& key) {
    size_t dotIndex = key.rfind(".");
    if (dotIndex == std::string::npos)
        return false;
    std::string afterDot = key.substr(dotIndex+1);
    return afterDot != "typename" && afterDot.find("-") == std::string::npos;
}

static bool isKeyGlobalConfigOption(const std::string& key) {
    return key.find(".") == std::string::npos;
}

const OrderedKeyValueList Run::getParamAssignments() const
{
    OrderedKeyValueList result;
    for (auto& p : configEntries)
        if (isKeyParamAssignment(p.first))
            result.push_back(p);
    return result;
}

const std::string& Run::getParamAssignment(const std::string& key) const
{
    return isKeyParamAssignment(key) ? getConfigValue(key) : NULLSTRING;
}

const OrderedKeyValueList Run::getNonParamAssignmentConfigEntries() const
{
    OrderedKeyValueList result;
    for (auto& p : configEntries)
        if (!isKeyParamAssignment(p.first))
            result.push_back(p);
    return result;
}

const std::string& Run::getNonParamAssignmentConfigEntry(const std::string& key) const
{
    return !isKeyParamAssignment(key) ? getConfigValue(key) : NULLSTRING;
}


//------

ResultFileManager::ResultFileManager()
{

}

ResultFileManager::~ResultFileManager()
{
    clear();
}

void ResultFileManager::clear()
{
    WRITER_MUTEX

    for (FileRun *fileRun : fileRunList)
        delete fileRun;

    for (Run *run : runList)
        delete run;

    for (ResultFile *file : fileList)
        delete file;

    fileRunList.clear();
    runList.clear();
    runsByName.clear();
    fileList.clear();
    filesByDisplayName.clear();

    moduleNames.clear();
    names.clear();
    classNames.clear();
}

ResultFileList ResultFileManager::getFiles() const
{
    READER_MUTEX
    return ResultFileList(fileList.begin(), fileList.end());
}

RunList ResultFileManager::getRuns() const
{
    READER_MUTEX
    return RunList(runList.begin(), runList.end());
}

RunList ResultFileManager::getRunsInFile(ResultFile *file) const
{
    READER_MUTEX
    RunList out;
    for (FileRun *fileRun : file->fileRuns)
        out.push_back(fileRun->runRef);
    return out;
}

ResultFileList ResultFileManager::getFilesForRun(Run *run) const
{
    READER_MUTEX
    ResultFileList out;
    for (FileRun *fileRun : run->fileRuns)
        out.push_back(fileRun->fileRef);
    return out;
}

ResultFileList ResultFileManager::getFilesForInput(const char *inputName) const
{
    READER_MUTEX
    ResultFileList out;
    if (inputName == nullptr)
        return out;
    std::string inputNameStr = inputName; // avoid conversion per loop iteration
    for (ResultFile *file : fileList)
        if (file && file->getInputName() == inputNameStr)
            out.push_back(file);
    return out;
}

const ResultItem& ResultFileManager::getItem(ID id) const
{
    READER_MUTEX
    try
    {
        switch (_type(id)) {
            case SCALAR: return getFileRunForID(id)->scalarResults.at(_pos(id));
            case PARAMETER: return getFileRunForID(id)->parameterResults.at(_pos(id));
            case VECTOR: return getFileRunForID(id)->vectorResults.at(_pos(id));
            case STATISTICS: return getFileRunForID(id)->statisticsResults.at(_pos(id));
            case HISTOGRAM: return getFileRunForID(id)->histogramResults.at(_pos(id));
            default: throw opp_runtime_error("ResultFileManager: Invalid ID: Wrong type");
        }
    }
    catch (std::out_of_range& e) {
        throw opp_runtime_error("ResultFileManager::getItem(id): Invalid ID");
    }
}

FileRunList ResultFileManager::getUniqueFileRuns(const IDList& ids) const
{
    READER_MUTEX

    // collect just unique fileIds first, to avoid dereferences at each ID
    std::set<int> fileRunIds;
    int lastFileRunId = -1;
    for (ID id : ids) {
        int fileRunId = _filerunid(id);
        if (fileRunId != lastFileRunId) {
            fileRunIds.insert(fileRunId);
            lastFileRunId = fileRunId;
        }
    }

    // convert set to FileRunList
    FileRunList result;
    result.reserve(fileRunIds.size());
    for (int fileId : fileRunIds) {
        FileRun *fileRun = fileRunList.at(fileId);
        if (!fileRun)
            throw opp_runtime_error("ResultFileManager::getUniqueFileRuns(): stale ID in input IDList");
        result.push_back(fileRun);
    }
    return result;
}

ResultFileList ResultFileManager::getUniqueFiles(const IDList& ids) const
{
    READER_MUTEX

    FileRunList fileRunList = getUniqueFileRuns(ids);
    return getUniqueFiles(fileRunList);
}

RunList ResultFileManager::getUniqueRuns(const IDList& ids) const
{
    READER_MUTEX

    FileRunList fileRunList = getUniqueFileRuns(ids);
    return getUniqueRuns(fileRunList);
}

ResultFileList ResultFileManager::getUniqueFiles(const FileRunList& fileRunList) const
{
    std::set<ResultFile*> set;
    for (FileRun *fileRun : fileRunList)
        set.insert(fileRun->fileRef);
    return ResultFileList(set.begin(), set.end());
}

RunList ResultFileManager::getUniqueRuns(const FileRunList& fileRunList) const
{
    std::set<Run*> set;
    for (FileRun *fileRun : fileRunList)
        set.insert(fileRun->runRef);
    return RunList(set.begin(), set.end());
}

StringSet ResultFileManager::getUniqueModuleNames(const IDList& ids) const
{
    READER_MUTEX
    std::set<const std::string*> set;  // all strings are stringpooled, so we can collect unique *pointers* instead of unique strings
    const std::string *lastModuleName = nullptr;
    for (ID id : ids) {
        const std::string *moduleName = &getItem(id).getModuleName();
        if (moduleName != lastModuleName) {
            set.insert(moduleName);
            lastModuleName = moduleName;
        }
    }
    StringSet result;
    for (const std::string *e : set)
        result.insert(*e);
    return result;
}

StringSet ResultFileManager::getUniqueResultNames(const IDList& ids) const
{
    READER_MUTEX
    std::set<const std::string*> set;  // all strings are stringpooled, so we can collect unique *pointers* instead of unique strings
    for (ID id : ids)
        set.insert(&getItem(id).getName());

    StringSet result;
    for (const std::string *e : set)
        result.insert(*e);
    return result;
}

StringSet ResultFileManager::getUniqueModuleAndResultNamePairs(const IDList& ids) const
{
    READER_MUTEX
    StringSet set;
    for (ID id : ids) {
        const ResultItem& result = getItem(id);
        set.insert(result.getModuleName() + "." + result.getName());
    }
    return set;
}

StringSet ResultFileManager::getUniqueAttributeNames(const IDList& ids) const
{
    READER_MUTEX
    StringSet set;
    for (ID id : ids) {
        const StringMap& attributes = getItem(id).getAttributes();
        for (StringMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
            set.insert(it->first);
    }
    return set;
}

StringSet ResultFileManager::getUniqueAttributeValues(const IDList& ids, const char *attrName) const
{
    READER_MUTEX
    StringSet values;
    for (int i = 0; i < ids.size(); ++i) {
        const string& value = getItem(ids.get(i)).getAttribute(attrName);
        if (&value != &NULLSTRING)
            values.insert(value);
    }
    return values;
}

StringSet ResultFileManager::getUniqueRunAttributeNames(const RunList& runList) const
{
    READER_MUTEX
    StringSet set;
    for (const Run *run : runList) {
        const StringMap& attributes = run->getAttributes();
        for (StringMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
            set.insert(it->first);
    }
    return set;
}

StringSet ResultFileManager::getUniqueRunAttributeValues(const RunList& runList, const char *attrName) const
{
    READER_MUTEX
    StringSet values;
    for (const Run *run : runList) {
        const string& value = run->getAttribute(attrName);
        if (&value != &NULLSTRING)
            values.insert(value);
    }

    return values;
}

StringSet ResultFileManager::getUniqueIterationVariableNames(const RunList& runList) const
{
    READER_MUTEX
    StringSet set;
    for (const Run *run : runList) {
        const StringMap& itervars = run->getIterationVariables();
        for (StringMap::const_iterator it = itervars.begin(); it != itervars.end(); ++it)
            set.insert(it->first);
    }
    return set;
}

StringSet ResultFileManager::getUniqueIterationVariableValues(const RunList& runList, const char *itervarName) const
{
    READER_MUTEX
    StringSet values;
    for (const Run *run : runList) {
        const string& value = run->getIterationVariable(itervarName);
        if (&value != &NULLSTRING)
            values.insert(value);
    }

    return values;
}

StringSet ResultFileManager::getUniqueConfigKeys(const RunList& runList) const
{
    READER_MUTEX
    StringSet set;
    for (const Run *run : runList)
        for (auto& p : run->getConfigEntries())
            set.insert(p.first);
    return set;
}

StringSet ResultFileManager::getUniqueConfigValues(const RunList& runList, const char *key) const
{
    READER_MUTEX
    StringSet values;
    for (const Run *run : runList) {
        const string& value = run->getConfigValue(key);
        if (&value != &NULLSTRING)
            values.insert(value);
    }

    return values;
}

StringSet ResultFileManager::getUniqueParamAssignmentKeys(const RunList& runList) const
{
    READER_MUTEX
    StringSet set;
    for (const Run *run : runList)
        for (auto& p : run->getConfigEntries())
            if (isKeyParamAssignment(p.first))
                set.insert(p.first);
    return set;
}

IDListsByRun ResultFileManager::getPartitionByRun(const IDList& ids) const
{
    READER_MUTEX

    std::map<Run*,IDList> map;
    int lastFileRunId = -1;
    IDList *currentRunIDList = nullptr;
    for (ID id : ids) {
        int fileRunId = _filerunid(id);
        if (fileRunId != lastFileRunId) {
            Run *run = getItem(id).getRun();
            currentRunIDList = &map[run];
            lastFileRunId = fileRunId;
        }
        currentRunIDList->append(id);
    }
    return IDListsByRun(map);
}

IDListsByFile ResultFileManager::getPartitionByFile(const IDList& ids) const
{
    READER_MUTEX

    std::map<ResultFile*,IDList> map;
    int lastFileRunId = -1;
    IDList *currentFileIDList = nullptr;
    for (ID id : ids) {
        int fileRunId = _filerunid(id);
        if (fileRunId != lastFileRunId) {
            ResultFile *file = getItem(id).getFile();
            currentFileIDList = &map[file];
            lastFileRunId = fileRunId;
        }
        currentFileIDList->append(id);
    }
    return IDListsByFile(map);
}

const ScalarResult& ResultFileManager::getScalar(ID id) const
{
    READER_MUTEX
    if (_type(id) != SCALAR)
        throw opp_runtime_error("ResultFileManager::getScalar(id): This item is not a scalar");
    return getFileRunForID(id)->scalarResults.at(_pos(id));
}

const ParameterResult& ResultFileManager::getParameter(ID id) const
{
    READER_MUTEX
    if (_type(id) != PARAMETER)
        throw opp_runtime_error("ResultFileManager::getParameter(id): This item is not a parameter");
    return getFileRunForID(id)->parameterResults.at(_pos(id));
}

const VectorResult& ResultFileManager::getVector(ID id) const
{
    READER_MUTEX
    if (_type(id) != VECTOR)
        throw opp_runtime_error("ResultFileManager::getVector(id): This item is not a vector");
    return getFileRunForID(id)->vectorResults.at(_pos(id));
}

const StatisticsResult& ResultFileManager::getStatistics(ID id) const
{
    READER_MUTEX
    if (_type(id) == STATISTICS)
        return getFileRunForID(id)->statisticsResults.at(_pos(id));
    else if (_type(id) == HISTOGRAM)
        return getFileRunForID(id)->histogramResults.at(_pos(id));
    else
        throw opp_runtime_error("ResultFileManager::getStatistics(id): This item is not a summary statistics or a histogram");
}

const HistogramResult& ResultFileManager::getHistogram(ID id) const
{
    READER_MUTEX
    if (_type(id) != HISTOGRAM)
        throw opp_runtime_error("ResultFileManager::getHistogram(id): This item is not a histogram");
    return getFileRunForID(id)->histogramResults.at(_pos(id));
}

template<class T>
void ResultFileManager::collectIDs(IDList& out, FileRun *fileRun, std::vector<T> FileRun::*vec, int type, bool includeFields) const
{
    // internal method, READER_MUTEX not needed
    std::vector<T>& v = fileRun->*vec;
    int n = v.size();
    int fileRunId = fileRun->id;
    for (int i = 0; i < n; i++) {
        bool isField = type == SCALAR ? ((ScalarResult&)v[i]).isField() : false;
        if (!isField || includeFields)
            out.append(_mkID(type, fileRunId, i));
    }
}

IDList ResultFileManager::getAllItems(bool includeFields) const
{
    READER_MUTEX

    IDList out;
    for (FileRun *fileRun : fileRunList) { // make fileRun the outer loop so that getUnique/getPartition methods are faster
        if (fileRun != nullptr) {
            collectIDs(out, fileRun, &FileRun::parameterResults, PARAMETER, includeFields);
            collectIDs(out, fileRun, &FileRun::scalarResults, SCALAR, includeFields);
            collectIDs(out, fileRun, &FileRun::statisticsResults, STATISTICS, includeFields);
            collectIDs(out, fileRun, &FileRun::histogramResults, HISTOGRAM, includeFields);
            collectIDs(out, fileRun, &FileRun::vectorResults, VECTOR, includeFields);
        }
    }
    return out;
}

IDList ResultFileManager::getAllScalars(bool includeFields) const
{
    READER_MUTEX
    IDList out;
    for (FileRun *fileRun : fileRunList)
        if (fileRun != nullptr)
            collectIDs(out, fileRun, &FileRun::scalarResults, SCALAR, includeFields);
    return out;
}

IDList ResultFileManager::getAllParameters() const
{
    READER_MUTEX
    IDList out;
    for (FileRun *fileRun : fileRunList)
        if (fileRun != nullptr)
            collectIDs(out, fileRun, &FileRun::parameterResults, PARAMETER, true);
    return out;
}

IDList ResultFileManager::getAllVectors() const
{
    READER_MUTEX
    IDList out;
    for (FileRun *fileRun : fileRunList)
        if (fileRun != nullptr)
            collectIDs(out, fileRun, &FileRun::vectorResults, VECTOR, true);
    return out;
}

IDList ResultFileManager::getAllStatistics() const
{
    READER_MUTEX
    IDList out;
    for (FileRun *fileRun : fileRunList)
        if (fileRun != nullptr)
            collectIDs(out, fileRun, &FileRun::statisticsResults, STATISTICS, true);
    return out;
}

IDList ResultFileManager::getAllHistograms() const
{
    READER_MUTEX
    IDList out;
    for (FileRun *fileRun : fileRunList)
        if (fileRun != nullptr)
            collectIDs(out, fileRun, &FileRun::histogramResults, HISTOGRAM, true);
    return out;
}

IDList ResultFileManager::getScalarsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX

    IDList out;
    collectIDs(out, fileRun, &FileRun::scalarResults, SCALAR, true);
    return out;
}

IDList ResultFileManager::getParametersInFileRun(FileRun *fileRun) const
{
    READER_MUTEX

    IDList out;
    collectIDs(out, fileRun, &FileRun::parameterResults, PARAMETER, true);
    return out;
}

IDList ResultFileManager::getVectorsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX

    IDList out;
    collectIDs(out, fileRun, &FileRun::vectorResults, VECTOR, true);
    return out;
}

IDList ResultFileManager::getStatisticsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX

    IDList out;
    collectIDs(out, fileRun, &FileRun::statisticsResults, STATISTICS, true);
    return out;
}

IDList ResultFileManager::getHistogramsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX

    IDList out;
    collectIDs(out, fileRun, &FileRun::histogramResults, HISTOGRAM, true);
    return out;
}

bool ResultFileManager::isFileLoaded(const char *displayName) const
{
    return getFile(displayName) != nullptr;
}

ResultFile *ResultFileManager::getFile(const char *displayName) const
{
    if (!displayName)
        return nullptr;

    READER_MUTEX
    auto it = filesByDisplayName.find(displayName);
    if (it != filesByDisplayName.end())
        return it->second;

    return nullptr;
}

Run *ResultFileManager::getRunByName(const char *runName) const
{
    if (!runName)
        return nullptr;

    READER_MUTEX
    auto it = runsByName.find(runName);
    if (it != runsByName.end())
        return it->second;

    return nullptr;
}

FileRun *ResultFileManager::getFileRun(ResultFile *file, Run *run) const
{
    READER_MUTEX
    for (FileRun *fileRun : run->fileRuns)  // max two items (sca and/or vec file)
        if (fileRun->fileRef == file)
            return fileRun;
    return nullptr;
}

// currently unused
ID ResultFileManager::getItemByName(FileRun *fileRunRef, const char *module, const char *name) const
{
    if (!fileRunRef || !module || !name)
        return 0;

    READER_MUTEX

    const std::string *moduleNameRef = moduleNames.find(module);
    if (!moduleNameRef)
        return 0;

    const std::string *nameRef = names.find(name);
    if (!nameRef)
        return 0;

    //TODO could use pointer comparisons (const char* pointing into a stringpool) instead of string comparisons
    ScalarResults& scalarResults = fileRunRef->scalarResults;
    for (int i = 0; i < (int)scalarResults.size(); i++) {
        const ResultItem& d = scalarResults[i];
        if (d.getModuleName() == *moduleNameRef && d.getName() == *nameRef)
            return _mkID(SCALAR, fileRunRef->id, i);
    }

    ParameterResults& parameterResults = fileRunRef->parameterResults;
    for (int i = 0; i < (int)parameterResults.size(); i++) {
        const ResultItem& d = parameterResults[i];
        if (d.getModuleName() == *moduleNameRef && d.getName() == *nameRef)
            return _mkID(PARAMETER, fileRunRef->id, i);
    }

    VectorResults& vectorResults = fileRunRef->vectorResults;
    for (int i = 0; i < (int)vectorResults.size(); i++) {
        const ResultItem& d = vectorResults[i];
        if (d.getModuleName() == *moduleNameRef && d.getName() == *nameRef)
            return _mkID(VECTOR, fileRunRef->id, i);
    }

    StatisticsResults& statisticsResults = fileRunRef->statisticsResults;
    for (int i = 0; i < (int)statisticsResults.size(); i++) {
        const ResultItem& d = statisticsResults[i];
        if (d.getModuleName() == *moduleNameRef && d.getName() == *nameRef)
            return _mkID(STATISTICS, fileRunRef->id, i);
    }

    HistogramResults& histogramResults = fileRunRef->histogramResults;
    for (int i = 0; i < (int)histogramResults.size(); i++) {
        const ResultItem& d = histogramResults[i];
        if (d.getModuleName() == *moduleNameRef && d.getName() == *nameRef)
            return _mkID(HISTOGRAM, fileRunRef->id, i);
    }
    return 0;
}

RunList ResultFileManager::filterRunList(const RunList& runList,
        const char *runNameFilter,
        const StringMap& attrFilter) const
{
    READER_MUTEX

    RunList out;

    // runName matcher
    PatternMatcher runNamePattern(runNameFilter, false, true, true);

    // copy attributes to vectors for performance (std::map iterator is
    // infamously slow, and we need PatternMatchers as well)
    StringVector attrNames;
    std::vector<PatternMatcher> attrValues;
    for (StringMap::const_iterator it = attrFilter.begin(); it != attrFilter.end(); ++it) {
        attrNames.push_back(it->first);
        attrValues.push_back(PatternMatcher(it->second.c_str(), false, true, true));
    }

    // do it
    for (int i = 0; i < (int)runList.size(); i++) {
        Run *run = runList[i];
        if (!runNamePattern.matches(run->getRunName().c_str()))
            continue;
        bool matches = true;
        for (int j = 0; j < (int)attrNames.size() && matches; j++) {
            const string& attrValue = run->getAttribute(attrNames[j]);
            if (!attrValues[j].matches(attrValue.c_str())) {
                matches = false;
                break;
            }
        }
        if (matches)
            out.push_back(run);
    }
    return out;
}

ResultFileList ResultFileManager::filterFileList(const ResultFileList& fileList, const char *filePathFilter) const
{
    ResultFileList out;

    // filePath matcher
    PatternMatcher filePathPattern(filePathFilter, false, true, true);

    READER_MUTEX
    for (ResultFile *file : fileList)
        if (filePathPattern.matches(file->getFilePath().c_str()))
            out.push_back(file);
    return out;
}

FileRunList ResultFileManager::getFileRuns(const ResultFileList *fileList, const RunList *runList) const
{
    READER_MUTEX
    FileRunList out;
    for (FileRun *fileRun : fileRunList) {
        //TODO use files and runs maps in fileRun
        if (fileList && std::find(fileList->begin(), fileList->end(), fileRun->fileRef) == fileList->end())
            continue;
        if (runList && std::find(runList->begin(), runList->end(), fileRun->runRef) == runList->end())
            continue;
        out.push_back(fileRun);
    }
    return out;
}

IDList ResultFileManager::filterIDList(const IDList& idlist,
        const FileRunList *fileRunFilter,
        const char *moduleFilter,
        const char *nameFilter) const
{
    READER_MUTEX

    // "*" means no filtering, so ignore it
    if (!strcmp(moduleFilter, "*"))
        moduleFilter = "";
    if (!strcmp(nameFilter, "*"))
        nameFilter = "";

    // prepare for wildcard matches
    bool patMatchModule = PatternMatcher::containsWildcards(moduleFilter);
    bool patMatchName = PatternMatcher::containsWildcards(nameFilter);

    PatternMatcher *modulePattern = nullptr;
    if (patMatchModule) {
        modulePattern = new PatternMatcher(moduleFilter, false, true, true);  // case-sensitive full-string match
    }
    PatternMatcher *namePattern = nullptr;
    if (patMatchName) {
        namePattern = new PatternMatcher(nameFilter, false, true, true);  // case-sensitive full-string match
    }

    // TODO: if there's no wildcard, find string pointers in the stringsets
    // in advance, then we don't have to do strcmp().

    // iterate over all values and add matching ones to "out".
    // we can exploit the fact that ResultFileManager contains the data in the order
    // they were read from file, i.e. grouped by runs
    IDList out;
    FileRun *lastFileRunRef = nullptr;
    bool lastFileRunMatched = false;
    int sz = idlist.size();
    for (int i = 0; i < sz; i++) {
        ID id = idlist.get(i);
        const ResultItem& d = getItem(id);

        if (fileRunFilter) {
            if (lastFileRunRef != d.getFileRun()) {
                lastFileRunRef = d.getFileRun();
                lastFileRunMatched = std::find(fileRunFilter->begin(), fileRunFilter->end(), d.getFileRun()) != fileRunFilter->end();
            }
            if (!lastFileRunMatched)
                continue;
        }

        if (moduleFilter && moduleFilter[0] &&
            (patMatchModule ? !modulePattern->matches(d.getModuleName().c_str())
             : strcmp(d.getModuleName().c_str(), moduleFilter))
            )
            continue;  // no match

        if (nameFilter && nameFilter[0] &&
            (patMatchName ? !namePattern->matches(d.getName().c_str())
             : strcmp(d.getName().c_str(), nameFilter))
            )
            continue;  // no match

        // everything matched, insert it.
        // (note: append() is fine: if input IDList didn't contain duplicates,
        // the result won't either)
        out.append(id);
    }
    return out;
}

IDList ResultFileManager::filterIDList(const IDList& idlist, const char *runName, const char *moduleName, const char *name) const
{
    READER_MUTEX
    Run *run = getRunByName(runName);
    return filterIDList(idlist, run, moduleName, name);
}

IDList ResultFileManager::filterIDList(const IDList& idlist, const Run *run, const char *moduleName, const char *name) const
{
    READER_MUTEX

    IDList result;
    int sz = idlist.size();
    for (int i = 0; i < sz; i++) {
        ID id = idlist.get(i);
        const ResultItem& d = getItem(id);

        if (run && d.getRun() != run)
            continue;

        if (moduleName && d.getModuleName() != moduleName)
            continue;

        if (name && d.getName() != name)
            continue;

        // everything matched, insert it.
        // (note: append() is fine: if input IDList didn't contain duplicates,
        // the result won't either)
        result.append(id);
    }
    return result;
}

class MatchableResultItem : public MatchExpression::Matchable
{
    const ResultItem& item;

    public:
        MatchableResultItem(const ResultItem& item) : item(item) {}
        virtual const char *getAsString() const override { return getName(); }
        virtual const char *getAsString(const char *attribute) const override;
    private:
        const char *getItemType() const;
        const char *getName() const { return item.getName().c_str(); }
        const char *getModuleName() const { return item.getModuleName().c_str(); }
        const char *getFileName() const { return item.getFile()->getFilePath().c_str(); }
        const char *getRunName() const { return item.getRun()->getRunName().c_str(); }
        const char *getResultItemAttribute(const char *attrName) const { return item.getAttribute(attrName).c_str(); }
        const char *getRunAttribute(const char *attrName) const { return item.getRun()->getAttribute(attrName).c_str(); }
        const char *getIterationVariable(const char *name) const { return item.getRun()->getIterationVariable(name).c_str(); }
        const char *getConfigValue(const char *key) const { return item.getRun()->getConfigValue(key).c_str(); }
};

const char *MatchableResultItem::getAsString(const char *attribute) const
{
    if (strcasecmp("name", attribute) == 0)
        return getName();
    else if (strcasecmp("module", attribute) == 0)
        return getModuleName();
    else if (strcasecmp("type", attribute) == 0)
        return getItemType();
    else if (strcasecmp("run", attribute) == 0)
        return getRunName();
    else if (strcasecmp("file", attribute) == 0)
        return getFileName();
    else if (strncasecmp("attr:", attribute, 5) == 0)
        return getResultItemAttribute(attribute+5);
    // TODO: add back param: but as par: -module parameter value, not parameter assignment
    else if (strncasecmp("runattr:", attribute, 8) == 0)
        return getRunAttribute(attribute+8);
    else if (strncasecmp("itervar:", attribute, 8) == 0)
        return getIterationVariable(attribute+8);
    else if (strncasecmp("config:", attribute, 7) == 0)
        return getConfigValue(attribute+7);
    else
        return getResultItemAttribute(attribute);
}

const char *MatchableResultItem::getItemType() const
{
    switch (item.getItemType()) {
        case ResultFileManager::PARAMETER: return "parameter";
        case ResultFileManager::SCALAR: return "scalar";
        case ResultFileManager::VECTOR: return "vector";
        case ResultFileManager::STATISTICS: return "statistics";
        case ResultFileManager::HISTOGRAM: return "histogram";
        default: return "?";
    }
}


class MatchableRun : public MatchExpression::Matchable
{
    const Run *run;

    public:
        MatchableRun(const Run *run) : run(run) {}
        virtual const char *getAsString() const override { return getName(); }
        virtual const char *getAsString(const char *attribute) const override;
    private:
        const char *getName() const { return run->getRunName().c_str(); }
        const char *getAttribute(const char *attrName) const { return run->getAttribute(attrName).c_str(); }
        const char *getIterationVariable(const char *name) const { return run->getIterationVariable(name).c_str(); }
        const char *getConfigValue(const char *key) const { return run->getConfigValue(key).c_str(); }
};

const char *MatchableRun::getAsString(const char *attribute) const
{
    if (strcasecmp("name", attribute) == 0)
        return getName();
    else if (strncasecmp("attr:", attribute, 5) == 0) // TODO: also add with runattr: prefix?
        return getAttribute(attribute+5);
    else if (strncasecmp("itervar:", attribute, 8) == 0)
        return getIterationVariable(attribute+8);
    else if (strncasecmp("config:", attribute, 7) == 0)
        return getConfigValue(attribute+7); // TODO: add param: as well? (also update the docs, if/when done)
    else
        return getAttribute(attribute);
}


class MatchableItervar : public MatchExpression::Matchable
{
    const Run *run;
    std::string itervar;

    public:
        MatchableItervar(const Run *run, const std::string &itervar) : run(run), itervar(itervar) {}
        virtual const char *getAsString() const override { return getName(); }
        virtual const char *getAsString(const char *attribute) const override;
    private:
        const char *getName() const { return itervar.c_str(); }
        const char *getRunName() const { return run->getRunName().c_str(); }
        const char *getRunAttribute(const char *attrName) const { return run->getAttribute(attrName).c_str(); }
        const char *getIterationVariable(const char *name) const { return run->getIterationVariable(name).c_str(); }
        const char *getConfigValue(const char *key) const { return run->getConfigValue(key).c_str(); }
};

const char *MatchableItervar::getAsString(const char *attribute) const
{
    if (strcasecmp("name", attribute) == 0)
        return getName();
    else if (strcasecmp("run", attribute) == 0)
        return getRunName();
    else if (strncasecmp("runattr:", attribute, 8) == 0)
        return getRunAttribute(attribute+8);
    else if (strncasecmp("itervar:", attribute, 8) == 0)
        return getIterationVariable(attribute+8);
    else if (strncasecmp("config:", attribute, 7) == 0)
        return getConfigValue(attribute+7);
    else
        return getName();
}



class MatchableRunattr : public MatchExpression::Matchable
{
    const Run *run;
    std::string runattr;

    public:
        MatchableRunattr(const Run *run, const std::string &runattr) : run(run), runattr(runattr) {}
        virtual const char *getAsString() const override { return getName(); }
        virtual const char *getAsString(const char *attribute) const override;
    private:
        const char *getName() const { return runattr.c_str(); }
        const char *getRunName() const { return run->getRunName().c_str(); }
        const char *getRunAttribute(const char *attrName) const { return run->getAttribute(attrName).c_str(); }
        const char *getIterationVariable(const char *name) const { return run->getIterationVariable(name).c_str(); }
        const char *getConfigValue(const char *key) const { return run->getConfigValue(key).c_str(); }
};

const char *MatchableRunattr::getAsString(const char *attribute) const
{
    if (strcasecmp("name", attribute) == 0)
        return getName();
    else if (strcasecmp("run", attribute) == 0)
        return getRunName();
    else if (strncasecmp("runattr:", attribute, 8) == 0)
        return getRunAttribute(attribute+8);
    else if (strncasecmp("itervar:", attribute, 8) == 0)
        return getIterationVariable(attribute+8);
    else if (strncasecmp("config:", attribute, 7) == 0)
        return getConfigValue(attribute+7);
    else
        return getName();
}



class MatchableConfigEntry : public MatchExpression::Matchable
{
    const Run *run;
    std::string key;

    public:
        MatchableConfigEntry(const Run *run, const std::string &key) : run(run), key(key) {}
        virtual const char *getAsString() const override { return getName(); }
        virtual const char *getAsString(const char *attribute) const override;
    private:
        const char *getName() const { return key.c_str(); }
        const char *getRunName() const { return run->getRunName().c_str(); }
        const char *getRunAttribute(const char *attrName) const { return run->getAttribute(attrName).c_str(); }
        const char *getIterationVariable(const char *name) const { return run->getIterationVariable(name).c_str(); }
        const char *getParamAssignment(const char *key) const { return run->getParamAssignment(key).c_str(); }
};

const char *MatchableConfigEntry::getAsString(const char *attribute) const
{
    if (strcasecmp("name", attribute) == 0)
        return getName();
    else if (strcasecmp("run", attribute) == 0)
        return getRunName();
    else if (strcasecmp("type", attribute) == 0)
        return isKeyParamAssignment(key) ? "param-assignment"
            : isKeyGlobalConfigOption(key) ? "global-config" : "per-object-config";
    else if (strncasecmp("runattr:", attribute, 8) == 0)
        return getRunAttribute(attribute+8);
    else if (strncasecmp("itervar:", attribute, 8) == 0)
        return getIterationVariable(attribute+8);
    else if (strncasecmp("param:", attribute, 6) == 0) // or config?
        return getParamAssignment(attribute+6);
    else
        return getName();
}

std::vector<std::pair<std::string, std::string>> ResultFileManager::getMatchingItervars(const char *pattern) const
{
    std::vector<std::pair<std::string, std::string>> out;
    for (auto i : getMatchingItervarsPtr(pattern))
        out.push_back({i.first->getRunName(), i.second});
    return out;
}

std::vector< std::pair<Run *, std::string> > ResultFileManager::getMatchingItervarsPtr(const char *pattern) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    std::vector< std::pair<Run *, std::string> > out;
    for (Run *run : runList) {
        for (auto &iv : run->getIterationVariables()) {
            MatchableItervar matchable(run, iv.first);
            if (matchExpr.matches(&matchable))
                out.push_back({run, iv.first});
        }
    }
    return out;
}

std::vector<std::pair<std::string, std::string>> ResultFileManager::getMatchingRunattrs(const char *pattern) const
{
    std::vector<std::pair<std::string, std::string>> out;
    for (auto i : getMatchingRunattrsPtr(pattern))
        out.push_back({i.first->getRunName(), i.second});
    return out;
}

std::vector< std::pair<Run *, std::string> > ResultFileManager::getMatchingRunattrsPtr(const char *pattern) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    std::vector< std::pair<Run *, std::string> > out;
    for (Run *run : runList) {
        for (auto &ra : run->getAttributes()) {
            MatchableRunattr matchable(run, ra.first);
            if (matchExpr.matches(&matchable))
                out.push_back({run, ra.first});
        }
    }
    return out;
}

std::vector< std::pair<std::string, std::string>> ResultFileManager::getMatchingConfigEntries(const char *pattern) const
{
    std::vector<std::pair<std::string, std::string>> out;
    for (auto i : getMatchingConfigEntriesPtr(pattern))
        out.push_back({i.first->getRunName(), i.second});
    return out;
}

std::vector< std::pair<Run *, std::string> > ResultFileManager::getMatchingConfigEntriesPtr(const char *pattern) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    std::vector< std::pair<Run *, std::string> > out;
    for (Run *run : runList) {
        for (auto &entry : run->getConfigEntries()) {
            MatchableConfigEntry matchable(run, entry.first);
            if (matchExpr.matches(&matchable))
                out.push_back({run, entry.first});
        }
    }
    return out;
}

// TODO use getMatchingConfigEntriesPtr then filter
std::vector<std::pair<std::string, std::string>> ResultFileManager::getMatchingParamAssignments(const char *pattern) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    std::vector<std::pair<std::string, std::string>> out;
    for (Run *run : runList) {
        for (auto &param : run->getParamAssignments()) {
            MatchableConfigEntry matchable(run, param.first);
            if (matchExpr.matches(&matchable))
                out.push_back({run->getRunName(), param.first});
        }
    }
    return out;
}

// TODO use getMatchingConfigEntriesPtr then filter
std::vector<std::pair<std::string, std::string>> ResultFileManager::getMatchingNonParamAssignmentConfigEntries(const char *pattern) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    std::vector<std::pair<std::string, std::string>> out;
    for (Run *run : runList) {
        for (auto &entry : run->getNonParamAssignmentConfigEntries()) {
            MatchableConfigEntry matchable(run, entry.first);
            if (matchExpr.matches(&matchable))
                out.push_back({run->getRunName(), entry.first});
        }
    }
    return out;
}

IDList ResultFileManager::filterIDList(const IDList& idlist, const char *pattern, int limit, const InterruptedFlag& interrupted) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    IDList out;
    int sz = idlist.size();
    int count = 0;
    for (int i = 0; i < sz; ++i) {
        if (interrupted.flag)
            throw InterruptedException("Result filtering interrupted");
        ID id = idlist.get(i);
        const ResultItem& item = getItem(id);
        MatchableResultItem matchable(item);
        if (matchExpr.matches(&matchable)) {
            out.append(id);
            count++;
            if (limit > 0 && count == limit)
                break;
        }
    }
    return out;
}

RunList ResultFileManager::filterRunList(const RunList& runlist, const char *pattern) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    RunList out;
    for (Run *run : runlist) {
        MatchableRun matchable(run);
        if (matchExpr.matches(&matchable))
            out.push_back(run);
    }
    return out;
}

void ResultFileManager::checkPattern(const char *pattern)
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    // parse it
    // XXX after successful parsing, we could also check that attribute names in it are valid
    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);
}

ResultFile *ResultFileManager::addFile(const char *displayName, const char *fileSystemFileName, ResultFile::FileType fileType)
{
    auto displayNameSlash = fileNameToSlash(displayName);
    ResultFile *file = new ResultFile();
    file->resultFileManager = this;
    file->fileSystemFilePath = fileSystemFileName;
    file->displayName = displayNameSlash;
    splitFileName(file->displayName.c_str(), file->displayNameFolderPart, file->displayNameFilePart);
    file->fingerprint = readFileFingerprint(fileSystemFileName);
    file->fileType = fileType;
    fileList.insert(file);
    filesByDisplayName[displayNameSlash] = file;
    return file;
}

Run *ResultFileManager::addRun(const std::string& runName)
{
    Run *run = new Run(runName, this);
    runList.insert(run);
    runsByName[runName] = run;
    return run;
}

FileRun *ResultFileManager::addFileRun(ResultFile *file, Run *run)
{
    FileRun *fileRun = new FileRun();
    fileRun->id = fileRunList.size();
    fileRunList.push_back(fileRun);
    fileRun->fileRef = file;
    fileRun->runRef = run;
    file->fileRuns.push_back(fileRun);
    run->fileRuns.push_back(fileRun);
    return fileRun;
}

Run *ResultFileManager::getOrAddRun(const std::string& runName)
{
    Run *run = getRunByName(runName.c_str());
    if (!run)
        run = addRun(runName);
    return run;
}

FileRun *ResultFileManager::getOrAddFileRun(ResultFile *file, Run *run)
{
    FileRun *fileRun = getFileRun(file, run);
    if (!fileRun)
        fileRun = addFileRun(file, run);
    return fileRun;
}

int ResultFileManager::addScalar(FileRun *fileRunRef, const char *moduleName, const char *scalarName,
        const StringMap& attrs, double value, bool isField)
{
    ScalarResult scalar(fileRunRef, moduleName, scalarName, attrs, value, isField);
    ScalarResults& scalars = fileRunRef->scalarResults;
    scalars.push_back(scalar);
    return scalars.size() - 1;
}

int ResultFileManager::addParameter(FileRun *fileRunRef, const char *moduleName, const char *paramName, const StringMap& attrs, const std::string& value)
{
    ParameterResult param(fileRunRef, moduleName, paramName, attrs, value);
    ParameterResults& params = fileRunRef->parameterResults;
    params.push_back(param);
    return params.size() - 1;
}

int ResultFileManager::addVector(FileRun *fileRunRef, int vectorId, const char *moduleName, const char *vectorName, const StringMap& attrs, const char *columns)
{
    VectorResult vector(fileRunRef, moduleName, vectorName, attrs, vectorId, columns);
    vector.stat = Statistics::makeInvalid(false);
    VectorResults& vectors = fileRunRef->vectorResults;
    vectors.push_back(vector);
    return vectors.size() - 1;
}

int ResultFileManager::addStatistics(FileRun *fileRunRef, const char *moduleName, const char *statisticsName,
        const Statistics& stat, const StringMap& attrs)
{
    StatisticsResult statistics(fileRunRef, moduleName, statisticsName, attrs, stat);
    StatisticsResults& statisticsResults = fileRunRef->statisticsResults;
    statisticsResults.push_back(statistics);
    if (true) //TODO flag
        addStatisticsFieldsAsScalars(fileRunRef, moduleName, statisticsName, stat);
    return statisticsResults.size() - 1;
}

int ResultFileManager::addHistogram(FileRun *fileRunRef, const char *moduleName, const char *histogramName,
        const Statistics& stat, const Histogram& bins, const StringMap& attrs)
{
    HistogramResult histogram(fileRunRef, moduleName, histogramName, attrs, stat, bins);
    HistogramResults& histograms = fileRunRef->histogramResults;
    histograms.push_back(histogram);
    if (true) //TODO flag
        addStatisticsFieldsAsScalars(fileRunRef, moduleName, histogramName, stat);
    return histograms.size() - 1;
}

void ResultFileManager::addStatisticsFieldsAsScalars(FileRun *fileRunRef, const char *moduleName, const char *statisticsName, const Statistics& stat)
{
    std::string name = statisticsName;
    StringMap emptyAttrs;
    addScalar(fileRunRef, moduleName, (name+":count").c_str(), emptyAttrs, stat.getCount(), true);
    addScalar(fileRunRef, moduleName, (name+":mean").c_str(), emptyAttrs, stat.getMean(), true);
    addScalar(fileRunRef, moduleName, (name+":stddev").c_str(), emptyAttrs, stat.getStddev(), true);
    addScalar(fileRunRef, moduleName, (name+":min").c_str(), emptyAttrs, stat.getMin(), true);
    addScalar(fileRunRef, moduleName, (name+":max").c_str(), emptyAttrs, stat.getMax(), true);
    if (!stat.isWeighted())
        addScalar(fileRunRef, moduleName, (name+":sum").c_str(), emptyAttrs, stat.getSum(), true);  // this one might be useful
    //TODO sumWeights? isWeighted?
}

/*
void ResultFileManager::dump(ResultFile *fileRef, std::ostream& out) const
{
    Run *prevRunRef = nullptr;
    for (ScalarResults::const_iterator i = scalars.begin(); i!=scalars.end(); i++)
    {
        const ScalarResult& d = *i;
        if (d.runRef->fileRef==fileRef)
        {
            if (d.runRef!=prevRunRef)
            {
                out << "run " << d.runRef->runNumber << " " << d.runRef->networkName
                    << " \"" << d.runRef->date << "\"\n";
                prevRunRef = d.runRef;
            }
            out << "scalar \"" << *d.moduleNameRef << "\"\t\""
                << *d.nameRef << "\"\t" << d.value << "\n";   //FIXME use opp_quotestr() here
        }
    }
}
*/

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond,msg) if (!(cond)) throw ResultFileFormatException(msg, ctx.fileName, ctx.lineNo);

static bool isFileReadable(const char *fileName)
{
    FILE *f = fopen(fileName, "r");
    if (f != nullptr) {
        fclose(f);
        return true;
    }
    else
        return false;
}

#define LOG !verbose ? std::cout : std::cout

ResultFile *ResultFileManager::loadFile(const char *displayName, const char *fileSystemFileName, int flags, InterruptedFlag *interrupted)
{
    WRITER_MUTEX

    // extract and validate flags
    int reloadOption = flags & (RELOAD|RELOAD_IF_CHANGED|NEVER_RELOAD);
    int indexingOption = flags & (ALLOW_INDEXING|SKIP_IF_NO_INDEX|ALLOW_LOADING_WITHOUT_INDEX);
    int lockfileOption = flags & (SKIP_IF_LOCKED|IGNORE_LOCK_FILE);
    bool verbose = (flags & VERBOSE) != 0;

    if (reloadOption != RELOAD && reloadOption != RELOAD_IF_CHANGED && reloadOption != NEVER_RELOAD)
        throw opp_runtime_error("invalid reload flags %d, must be one of: RELOAD, RELOAD_IF_CHANGED, NEVER_RELOAD", reloadOption);
    if (indexingOption != ALLOW_INDEXING && indexingOption != SKIP_IF_NO_INDEX && indexingOption != ALLOW_LOADING_WITHOUT_INDEX)
        throw opp_runtime_error("invalid indexing flags %d, must be one of: ALLOW_INDEXING, SKIP_IF_NO_INDEX, ALLOW_LOADING_WITHOUT_INDEX", indexingOption);
    if (lockfileOption != SKIP_IF_LOCKED && lockfileOption != IGNORE_LOCK_FILE)
        throw opp_runtime_error("invalid lockfile handling flags %d, must be one of: SKIP_IF_LOCKED, IGNORE_LOCK_FILE", lockfileOption);

    if (interrupted == nullptr) {
        static InterruptedFlag neverInterrupted;
        interrupted = &neverInterrupted; // eliminate need for nullptr checks
    }

    // check if loaded
    ResultFile *fileRef = getFile(displayName);
    if (fileRef) {
        FileFingerprint fingerprint = readFileFingerprint(fileSystemFileName);
        switch (reloadOption) {
            case RELOAD: {
                LOG << "already loaded, unloading previous content: " << displayName << std::endl;
                unloadFile(fileRef);
                break;
            }
            case RELOAD_IF_CHANGED: {
                bool isUpToDate = (fingerprint == fileRef->fingerprint);
                if (isUpToDate) {
                    LOG << "already loaded and unchanged since, skipping: " << displayName << std::endl;
                    return fileRef;
                }
                else {
                    LOG << "already loaded but changed since, unloading previous content: " << displayName << std::endl;
                    unloadFile(fileRef);
                }
            }
            case NEVER_RELOAD: {
                LOG << "already loaded, skipping: " << displayName << std::endl;
                return fileRef;
            }
        }
    }

    // try if file can be opened, before we add it to our database
    if (fileSystemFileName == nullptr)
        fileSystemFileName = displayName;
    if (!isFileReadable(fileSystemFileName))
        throw opp_runtime_error("Cannot open '%s' for read", fileSystemFileName);

    try {
        ResultFile *file = SqliteResultFileUtils::isSqliteFile(fileSystemFileName) ?
                SqliteResultFileLoader(this, flags, interrupted).loadFile(displayName, fileSystemFileName) :
                OmnetppResultFileLoader(this, flags, interrupted).loadFile(displayName, fileSystemFileName);
        return file; // note: nullptr if file was skipped (e.g. due to missing index)
    }
    catch (InterruptedException& e) {
        return nullptr;
    }
}

#undef LOG

void ResultFileManager::setFileInput(ResultFile *file, const char *inputName)
{
    WRITER_MUTEX

    // Note: DO NOT MERGE this method into loadFile(), because it doesn't/shouldn't know
    // whether already loaded files will need their inputName to be updated or not
    file->inputName = inputName;
}

void ResultFileManager::unloadFile(const char *displayName)
{
    WRITER_MUTEX

    ResultFile *file = getFile(displayName);
    if (file)
        unloadFile(file);
}

void ResultFileManager::unloadFile(ResultFile *file)
{
    WRITER_MUTEX

    // delete FileRuns
    set<Run *> affectedRuns;
    for (FileRun *fileRun : file->fileRuns) {
        Run *run = fileRun->runRef;
        fileRunList[fileRun->id] = nullptr;  // do not erase, because existing IDs would change their meaning
        delete fileRun;

        // remove it from the corresponding Run, and if it was the last one, remove Run too
        FileRunList& runFileRuns = run->fileRuns;
        if (runFileRuns.size() == 1) {
            assert(runFileRuns[0] == fileRun);
            runsByName.erase(runsByName.find(run->getRunName()));
            runList.erase(run);
            delete run;
        }
        else {
            auto it = find(runFileRuns, fileRun);
            assert(it != runFileRuns.end());
            runFileRuns.erase(it);
        }
    }

    // delete ResultFile
    filesByDisplayName.erase(filesByDisplayName.find(file->getFilePath()));
    fileList.erase(file);
    delete file;
}

/*--------------------------------------------------------------------------
 *                        compute filter hints
 *--------------------------------------------------------------------------*/

/**
 * Utility class: collects strings that occur at least twice.
 */
class DuplicateStringCollector
{
  private:
    typedef std::map<std::string, int> StringCountMap;
    StringCountMap map;
    StringVector vec;

  public:
    void add(const std::string& str)
    {
        StringCountMap::iterator mi = map.find(str);
        if (mi == map.end())
            map[str] = 1;  // 1st occurrence
        else if (++(mi->second) == 2)  // 2nd occurrence
            vec.push_back(str);
    }

    StringVector& get()
    {
        return vec;
    }
};

inline bool strdictLess(const std::string& first, const std::string& second)
{
    return strdictcmp(first.c_str(), second.c_str()) < 0;
}

struct StrDictCompare
{
    bool operator()(const std::string& first, const std::string& second) const
    {
        return strdictLess(first, second);
    }
};

typedef std::set<std::string, StrDictCompare> SortedStringSet;

static void replaceDigitsWithWildcard(std::string& str)
{
    std::string::iterator start;
    while ((start = std::find_if(str.begin(), str.end(), opp_isdigit)) != str.end()) {
        std::string::iterator end = std::find_if(start, str.end(), std::not1(std::ptr_fun(opp_isdigit)));
        str.replace(start, end, 1, '*');
    }
}

static bool replaceIndexWithWildcard(std::string& str)
{
    bool changed = false;
    std::string::iterator start = str.begin();
    while ((start = std::find_if(start, str.end(), opp_isdigit)) != str.end()) {
        std::string::iterator end = std::find_if(start, str.end(), std::not1(std::ptr_fun(opp_isdigit)));
        if (start != str.begin() && end != str.end() && *(start-1) == '[' && *end == ']') {
            str.replace(start, end, 1, '*');
            changed = true;
        }
        start = end;
    }
    return changed;
}

StringVector ResultFileManager::getFilePathFilterHints(const ResultFileList& fileList) const
{
    READER_MUTEX
    StringVector filterHints;
    for (const ResultFile *file : fileList)
        filterHints.push_back(file->getFilePath());
    std::sort(filterHints.begin(), filterHints.end(), strdictLess);
    filterHints.insert(filterHints.begin(), "*");
    return filterHints;
}

StringVector ResultFileManager::getRunNameFilterHints(const RunList& runList) const
{
    READER_MUTEX
    StringVector filterHints;
    for (const Run *run : runList)
        if (run->getRunName().size() > 0)
            filterHints.push_back(run->getRunName());

    std::sort(filterHints.begin(), filterHints.end(), strdictLess);
    filterHints.insert(filterHints.begin(), "*");
    return filterHints;
}

StringVector ResultFileManager::getModuleFilterHints(const IDList& idlist) const
{
    READER_MUTEX
    StringSet names = getUniqueModuleNames(idlist);

    SortedStringSet nameHints;
    DuplicateStringCollector coll;

    for (const std::string& a0 : names) {
        // replace embedded numbers with "*"
        std::string a = a0;
        if (names.size() > 100)
            replaceDigitsWithWildcard(a);
        nameHints.insert(a);

        // break it up along dots, and...
        StringTokenizer tokenizer(a.c_str(), ".");
        const char *prefix = "";
        const char *suffix = ".*";
        while (tokenizer.hasMoreTokens()) {
            std::string token = tokenizer.nextToken();
            if (!tokenizer.hasMoreTokens())
                suffix = "";
            coll.add(std::string(prefix)+token+suffix);
            if (replaceIndexWithWildcard(token))  // add [*] version too
                coll.add(std::string(prefix)+token+suffix);
            prefix = "*.";
        }
    }

    // sort and concatenate them, and return the result
    StringVector wildvec(coll.get());
    wildvec.push_back(std::string("*"));
    std::sort(wildvec.begin(), wildvec.end(), strdictLess);
    wildvec.insert(wildvec.end(), nameHints.begin(), nameHints.end());
    return wildvec;
}

StringVector ResultFileManager::getResultNameFilterHints(const IDList& idlist) const
{
    READER_MUTEX
    StringSet names = getUniqueResultNames(idlist);

    StringVector vec;
    DuplicateStringCollector coll;

    for (const std::string& a : names) {
        vec.push_back(a);

        // break it up along spaces, and...
        StringTokenizer tokenizer(a.c_str());
        const char *prefix = "";
        const char *suffix = " *";
        while (tokenizer.hasMoreTokens()) {
            const char *token = tokenizer.nextToken();
            if (!tokenizer.hasMoreTokens())
                suffix = "";
            coll.add(std::string(prefix)+token+suffix);
            prefix = "* ";
        }
    }

    // sort and concatenate them, and return the result
    StringVector wildvec(coll.get());
    wildvec.push_back(std::string("*"));
    std::sort(vec.begin(), vec.end(), strdictLess);
    std::sort(wildvec.begin(), wildvec.end(), strdictLess);
    wildvec.insert(wildvec.end(), vec.begin(), vec.end());
    return wildvec;
}

StringVector ResultFileManager::getResultItemAttributeFilterHints(const IDList& idlist, const char *attrName) const
{
    READER_MUTEX
    StringSet attrValues = getUniqueAttributeValues(idlist, attrName);
    StringVector filterHints(attrValues.begin(), attrValues.end());
    std::sort(filterHints.begin(), filterHints.end(), strdictLess);
    filterHints.insert(filterHints.begin(), "*");
    return filterHints;
}

StringVector ResultFileManager::getRunAttributeFilterHints(const RunList& runList, const char *attrName) const
{
    READER_MUTEX
    StringSet attrValues = getUniqueRunAttributeValues(runList, attrName);
    StringVector filterHints(attrValues.begin(), attrValues.end());
    std::sort(filterHints.begin(), filterHints.end(), strdictLess);
    filterHints.insert(filterHints.begin(), "*");
    return filterHints;
}

StringVector ResultFileManager::getIterationVariableFilterHints(const RunList& runList, const char *itervarName) const
{
    READER_MUTEX
    StringSet values = getUniqueIterationVariableValues(runList, itervarName);
    StringVector filterHints(values.begin(), values.end());
    std::sort(filterHints.begin(), filterHints.end(), strdictLess);
    filterHints.insert(filterHints.begin(), "*");
    return filterHints;
}

StringVector ResultFileManager::getConfigEntryFilterHints(const RunList& runList, const char *key) const
{
    READER_MUTEX
    StringSet paramValues = getUniqueConfigValues(runList, key);
    StringVector filterHints(paramValues.begin(), paramValues.end());
    std::sort(filterHints.begin(), filterHints.end(), strdictLess);
    filterHints.insert(filterHints.begin(), "*");
    return filterHints;
}

StringVector ResultFileManager::getParamAssignmentFilterHints(const RunList& runList, const char *key) const
{
    if (!isKeyParamAssignment(key))
        return StringVector();
    return getConfigEntryFilterHints(runList, key);
}


bool ResultFileManager::hasStaleID(const IDList& ids) const
{
    READER_MUTEX
    for (int i = 0; i < ids.size(); ++i) {
        ID id = ids.get(i);
        if (isStaleID(id))
            return true;
    }
    return false;
}

const char *ResultFileManager::getRunAttribute(ID id, const char *attribute) const
{
    return getItem(id).getRun()->getAttribute(attribute).c_str();
}

}  // namespace scave
}  // namespace omnetpp

