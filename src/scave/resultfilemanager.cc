//=========================================================================
//  RESULTFILEMANAGER.CC - part of
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
#include "fields.h" // for name constants
#include "scaveutils.h"
#include "scaveexception.h"
#include "sqliteresultfileutils.h"
#include "resultfilemanager.h"
#include "omnetppresultfileloader.h"
#include "sqliteresultfileloader.h"
#include "vectorfileindex.h"
#include "interruptedflag.h"


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

size_t StringMapPtrHash::operator() (const StringMap *map) const
{
    size_t seed = 0;
    for (auto pair : *map) {
        hash_combine(seed, pair.first);
        hash_combine(seed, pair.second);
    }
    return seed;
}

bool StringMapPtrEq::operator () (const StringMap *lhs, const StringMap *rhs) const
{
    return *lhs == *rhs;
}

ResultFileManager::~ResultFileManager()
{
    clear();
}

void ResultFileManager::clear()
{
    WRITER_MUTEX

    serial++;

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

    for (const StringMap *attrs : attrsPool)
        delete attrs;
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

const ResultItem *ResultFileManager::getItem(ID id, ScalarResult& buffer) const
{
    if (_fieldid(id) == 0)
        return getNonfieldItem(id);
    else {
        buffer = getFieldScalar(id);
        return &buffer;
    }
}

const ResultItem *ResultFileManager::getNonfieldItem(ID id) const
{
    if (_fieldid(id) != 0 || _hosttype(id) != 0)
        throw opp_runtime_error("ResultFileManager::getItem(id): use getFieldScalar() for scalars which are a field of a statistic/histogram/vector");

    READER_MUTEX

    try {
        switch (_type(id)) {
            case SCALAR: return &getFileRunForID(id)->scalarResults.at(_pos(id));
            case PARAMETER: return &getFileRunForID(id)->parameterResults.at(_pos(id));
            case VECTOR: return &getFileRunForID(id)->vectorResults.at(_pos(id));
            case STATISTICS: return &getFileRunForID(id)->statisticsResults.at(_pos(id));
            case HISTOGRAM: return &getFileRunForID(id)->histogramResults.at(_pos(id));
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
    ScalarResult buffer;
    const std::string *lastModuleName = nullptr;
    for (ID id : ids) {
        const std::string *moduleName = &getItem(id, buffer)->getModuleName();
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
    ScalarResult buffer;
    for (ID id : ids)
        set.insert(&getItem(id, buffer)->getName());

    StringSet result;
    for (const std::string *e : set)
        result.insert(*e);
    return result;
}

StringSet ResultFileManager::getUniqueModuleAndResultNamePairs(const IDList& ids) const
{
    READER_MUTEX
    StringSet set;
    ScalarResult buffer;
    for (ID id : ids) {
        const ResultItem *result = getItem(id, buffer);
        set.insert(result->getModuleName() + "." + result->getName());
    }
    return set;
}

StringSet ResultFileManager::getUniqueResultAttributeNames(const IDList& ids) const
{
    READER_MUTEX
    StringSet set;
    ScalarResult buffer;
    for (ID id : ids) {
        const StringMap& attributes = getItem(id, buffer)->getAttributes(); //TODO too much copying!!!!!!!!!!!!!!!!!!!
        for (StringMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
            set.insert(it->first);
    }
    return set;
}

StringSet ResultFileManager::getUniqueResultAttributeValues(const IDList& ids, const char *attrName) const
{
    READER_MUTEX
    StringSet values;
    ScalarResult buffer;
    for (ID id : ids) {
        const string& value = getItem(id, buffer)->getAttribute(attrName); //TODO too much copying!!!!!!!!!!!!!!!!!!!
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

StringSet ResultFileManager::getUniqueParamAssignmentConfigKeys(const RunList& runList) const
{
    READER_MUTEX
    StringSet set;
    for (const Run *run : runList)
        for (auto& p : run->getConfigEntries())
            if (Run::isParamAssignmentConfigKey(p.first))
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
            Run *run = fileRunList[fileRunId]->runRef;
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
            ResultFile *file = fileRunList[fileRunId]->fileRef;
            currentFileIDList = &map[file];
            lastFileRunId = fileRunId;
        }
        currentFileIDList->append(id);
    }
    return IDListsByFile(map);
}

const ScalarResult *ResultFileManager::getNonfieldScalar(ID id) const
{
    READER_MUTEX
    if (_type(id) != SCALAR)
        throw opp_runtime_error("ResultFileManager::getScalar(id): This item is not a scalar");
    if (_fieldid(id) != 0 || _hosttype(id) != 0)
        throw opp_runtime_error("ResultFileManager::getScalar(id): use getFieldScalar() for scalars which are a field of a statistic/histogram/vector");
    return &getFileRunForID(id)->scalarResults.at(_pos(id));
}

const char *ResultFileManager::getNameSuffixForFieldScalar(FieldNum fieldId)
{
    switch (fieldId) {
    case FieldNum::COUNT: return Scave::COUNT;
    case FieldNum::SUM: return Scave::SUM;
    case FieldNum::SUMWEIGHTS: return Scave::SUMWEIGHTS;
    case FieldNum::MEAN: return Scave::MEAN;
    case FieldNum::STDDEV: return Scave::STDDEV;
    case FieldNum::MIN: return Scave::MIN;
    case FieldNum::MAX: return Scave::MAX;
    case FieldNum::NUMBINS: return Scave::NUMBINS;
    case FieldNum::RANGEMIN: return Scave::RANGEMIN;
    case FieldNum::RANGEMAX: return Scave::RANGEMAX;
    case FieldNum::UNDERFLOWS: return Scave::UNDERFLOWS;
    case FieldNum::OVERFLOWS: return Scave::OVERFLOWS;
    case FieldNum::STARTTIME: return Scave::STARTTIME;
    case FieldNum::ENDTIME: return Scave::ENDTIME;
    default: throw opp_runtime_error("Invalid fieldId");
    }
}

ScalarResult ResultFileManager::getFieldScalar(ID id) const
{
    ScalarResult result;
    fillFieldScalar(result, id);
    return result;
}

void ResultFileManager::fillFieldScalar(ScalarResult& scalar, ID id) const
{
    READER_MUTEX
    const ResultItem *container = getContainingItem(id);
    FieldNum fieldId = (FieldNum) _fieldid(id);
    scalar.fileRunRef = container->fileRunRef;
    scalar.moduleNameRef = container->moduleNameRef;
    scalar.nameRef = getPooledNameWithSuffix(container->nameRef,fieldId);
    scalar.attributes = container->attributes;
    scalar.value = container->getScalarField(fieldId);
    scalar.ownID = id;
}

const std::string *ResultFileManager::getPooledNameWithSuffix(const std::string *name, FieldNum fieldId) const
{
    auto key = std::make_pair(name, fieldId);
    auto it = namesWithSuffixCache.find(key);
    if (it != namesWithSuffixCache.end())
        return it->second;
    else {
        const std::string *value = const_cast<ResultFileManager*>(this)->names.insert(*name+":"+getNameSuffixForFieldScalar(fieldId));
        namesWithSuffixCache[key] = value;
        return value;
    }
}

const ScalarResult *ResultFileManager::getScalar(ID id, ScalarResult& buffer) const
{
    if (_fieldid(id) == 0)
        return getNonfieldScalar(id);
    else {
        fillFieldScalar(buffer, id);
        return &buffer;
    }
}

const ParameterResult *ResultFileManager::getParameter(ID id) const
{
    READER_MUTEX
    if (_type(id) != PARAMETER)
        throw opp_runtime_error("ResultFileManager::getParameter(id): This item is not a parameter");
    return &getFileRunForID(id)->parameterResults.at(_pos(id));
}

const VectorResult *ResultFileManager::getVector(ID id) const
{
    READER_MUTEX
    if (_type(id) != VECTOR)
        throw opp_runtime_error("ResultFileManager::getVector(id): This item is not a vector");
    return &getFileRunForID(id)->vectorResults.at(_pos(id));
}

const StatisticsResult *ResultFileManager::getStatistics(ID id) const
{
    READER_MUTEX
    if (_type(id) == STATISTICS)
        return &getFileRunForID(id)->statisticsResults.at(_pos(id));
    else if (_type(id) == HISTOGRAM)
        return &getFileRunForID(id)->histogramResults.at(_pos(id));
    else
        throw opp_runtime_error("ResultFileManager::getStatistics(id): This item is not a summary statistics or a histogram");
}

const HistogramResult *ResultFileManager::getHistogram(ID id) const
{
    READER_MUTEX
    if (_type(id) != HISTOGRAM)
        throw opp_runtime_error("ResultFileManager::getHistogram(id): This item is not a histogram");
    return &getFileRunForID(id)->histogramResults.at(_pos(id));
}

void ResultFileManager::makeIDs(std::vector<ID>& out, FileRun *fileRun, int numItems, int type) const
{
    // internal method, READER_MUTEX not needed
    int fileRunId = fileRun->id;
    for (int pos = 0; pos < numItems; pos++)
        out.push_back(_mkID(type, fileRunId, pos));
}

void ResultFileManager::makeFieldScalarIDs(std::vector<ID>& out, FileRun *fileRun, int numItems, HostType hosttype, FieldNum *fieldIds) const
{
    // internal method, READER_MUTEX not needed
    int fileRunId = fileRun->id;
    for (int pos = 0; pos < numItems; pos++)
        for (int f = 0; fieldIds[f] != FieldNum::NONE; f++)
            out.push_back(_mkID(hosttype, fileRunId, pos, (int)fieldIds[f]));
}

const char *ResultFileManager::getItemProperty(ID id, const char *propertyName) const
{
    switch (propertyName[0]) {
        case Scave::TYPE[0]: {
            if (strcmp(propertyName, Scave::TYPE) == 0)
                return ResultItem::itemTypeToString(_type(id));
            break;
        }
        case Scave::MODULE[0]: {
            if (strcmp(propertyName, Scave::MODULE) == 0) {
                if (isField(id))
                    return getContainingItem(id)->getModuleName().c_str(); // field scalar has the same module as its containing result item
                else
                    return getNonfieldItem(id)->getModuleName().c_str();
            }
            break;
        }
        case Scave::NAME[0]: {
            if (strcmp(propertyName, Scave::NAME) == 0) {
                if (isField(id))
                    return getPooledNameWithSuffix(&getContainingItem(id)->getName(), (FieldNum)_fieldid(id))->c_str();
                else
                    return getNonfieldItem(id)->getName().c_str();
            }
            break;
        }
        case Scave::FILE[0]: {
            if (strcmp(propertyName, Scave::FILE) == 0)
                return getFileRun(id)->getFile()->getFileName().c_str();
            break;
        }
        case Scave::RUN[0]: { // and RUNATTR_PREFIX
            static_assert(Scave::RUN[0] == Scave::RUNATTR_PREFIX[0], "initial letter mismatch");
            if (strcmp(propertyName, Scave::RUN) == 0)
                return getFileRun(id)->getRun()->getRunName().c_str();
            if (strncmp(propertyName, Scave::RUNATTR_PREFIX, strlen(Scave::RUNATTR_PREFIX)) == 0)
                return getFileRun(id)->getRun()->getAttribute(propertyName + strlen(Scave::RUNATTR_PREFIX)).c_str();
            break;
        }
        case Scave::ISFIELD[0]: { // and ITERVAR_PREFIX
            static_assert(Scave::ISFIELD[0] == Scave::ITERVAR_PREFIX[0], "initial letter mismatch");
            if (strcmp(propertyName, Scave::ISFIELD) == 0)
                return _fieldid(id) != 0 ? Scave::TRUE : Scave::FALSE;
            if (strncmp(propertyName, Scave::ITERVAR_PREFIX, strlen(Scave::ITERVAR_PREFIX)) == 0)
                return getFileRun(id)->getRun()->getIterationVariable(propertyName + strlen(Scave::ITERVAR_PREFIX)).c_str();
            break;
        }
        case Scave::CONFIG_PREFIX[0]: {
            if (strncmp(propertyName, Scave::CONFIG_PREFIX, strlen(Scave::CONFIG_PREFIX)) == 0)
                return getFileRun(id)->getRun()->getConfigValue(propertyName + strlen(Scave::CONFIG_PREFIX)).c_str();
            break;
        }
        case Scave::ATTR_PREFIX[0]: {
            if (strncmp(propertyName, Scave::ATTR_PREFIX, strlen(Scave::ATTR_PREFIX)) == 0) {
                ScalarResult buffer;
                return getItem(id, buffer)->getAttribute(propertyName + strlen(Scave::ATTR_PREFIX)).c_str();
            }
            break;
        }
    }
    throw opp_runtime_error("ResultFileManager::getItemProperty(): unrecognized property name '%s'", propertyName);
}

IDList ResultFileManager::getItems(const FileRunList& fileRuns, int types, bool includeFields) const
{
    READER_MUTEX

    std::vector<ID> out;
    for (FileRun *fileRun : fileRuns) { // make fileRun the outer loop so that getUnique/getPartition methods are faster
        if (fileRun != nullptr) {
            if (types & PARAMETER)
                makeIDs(out, fileRun, fileRun->parameterResults.size(), PARAMETER);
            if (types & SCALAR) {
                makeIDs(out, fileRun, fileRun->scalarResults.size(), SCALAR);
                if (includeFields) {
                    makeFieldScalarIDs(out, fileRun, fileRun->statisticsResults.size(), HOSTTYPE_STATISTICS, StatisticsResult::getAvailableFields());
                    makeFieldScalarIDs(out, fileRun, fileRun->histogramResults.size(), HOSTTYPE_HISTOGRAM, HistogramResult::getAvailableFields());
                    makeFieldScalarIDs(out, fileRun, fileRun->vectorResults.size(), HOSTTYPE_VECTOR, VectorResult::getAvailableFields());
                }
            }
            if (types & STATISTICS)
                makeIDs(out, fileRun, fileRun->statisticsResults.size(), STATISTICS);
            if (types & HISTOGRAM)
                makeIDs(out, fileRun, fileRun->histogramResults.size(), HISTOGRAM);
            if (types & VECTOR)
                makeIDs(out, fileRun, fileRun->vectorResults.size(), VECTOR);
        }
    }
    return IDList(std::move(out));
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
    std::vector<ID> out;
    FileRun *lastFileRunRef = nullptr;
    bool lastFileRunMatched = false;
    ScalarResult buffer;
    for (ID id : idlist) {
        if (fileRunFilter) {
            FileRun *fileRun = getFileRun(id);
            if (lastFileRunRef != fileRun) {
                lastFileRunRef = fileRun;
                lastFileRunMatched = std::find(fileRunFilter->begin(), fileRunFilter->end(), fileRun) != fileRunFilter->end();
            }
            if (!lastFileRunMatched)
                continue;
        }

        const ResultItem *item = getItem(id, buffer);

        if (moduleFilter && moduleFilter[0] &&
            (patMatchModule ? !modulePattern->matches(item->getModuleName().c_str())
             : strcmp(item->getModuleName().c_str(), moduleFilter))
            )
            continue;  // no match

        if (nameFilter && nameFilter[0] &&
            (patMatchName ? !namePattern->matches(item->getName().c_str())
             : strcmp(item->getName().c_str(), nameFilter))
            )
            continue;  // no match

        // everything matched, insert it.
        // (note: append() is fine: if input IDList didn't contain duplicates,
        // the result won't either)
        out.push_back(id);
    }
    return IDList(std::move(out));
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

    std::vector<ID> result;
    ScalarResult buffer;
    for (ID id : idlist) {
        if (run && getFileRun(id)->runRef != run)
            continue;

        const ResultItem *item = getItem(id, buffer);

        if (moduleName && item->getModuleName() != moduleName)
            continue;

        if (name && item->getName() != name)
            continue;

        // everything matched, insert it.
        result.push_back(id);
    }
    return IDList(std::move(result));
}

class MatchableResultItem : public MatchExpression::Matchable
{
    private:
        const ResultFileManager *manager;
        ID id;
    public:
        MatchableResultItem(const ResultFileManager *manager, ID id) : manager(manager), id(id) {}
        virtual const char *getAsString() const override { return manager->getItemProperty(id, "name"); }
        virtual const char *getAsString(const char *attribute) const override { return manager->getItemProperty(id, attribute); }
};

class MatchableRun : public MatchExpression::Matchable
{
    private:
        const Run *run;
    public:
        MatchableRun(const Run *run) : run(run) {}
        virtual const char *getAsString() const override { return run->getRunName().c_str(); }
        virtual const char *getAsString(const char *attribute) const override { return run->getProperty(attribute); }
};

class MatchableItervar : public MatchExpression::Matchable
{
    private:
        const Run *run;
        const std::string& itervar;
    public:
        MatchableItervar(const Run *run, const std::string &itervar) : run(run), itervar(itervar) {}
        virtual const char *getAsString() const override { return itervar.c_str(); }
        virtual const char *getAsString(const char *attribute) const override;
};

const char *MatchableItervar::getAsString(const char *attribute) const
{
    if (strcmp(Scave::NAME, attribute) == 0)
        return itervar.c_str();
    else
        return run->getProperty(attribute);
}

class MatchableRunattr : public MatchExpression::Matchable
{
    private:
        const Run *run;
        const std::string& runattr;
    public:
        MatchableRunattr(const Run *run, const std::string &runattr) : run(run), runattr(runattr) {}
        virtual const char *getAsString() const override { return runattr.c_str(); }
        virtual const char *getAsString(const char *attribute) const override;
};

const char *MatchableRunattr::getAsString(const char *attribute) const
{
    if (strcmp(Scave::NAME, attribute) == 0)
        return runattr.c_str();
    else
        return run->getProperty(attribute);
}

class MatchableConfigEntry : public MatchExpression::Matchable
{
    private:
        const Run *run;
        const std::string& key;
    public:
        MatchableConfigEntry(const Run *run, const std::string &key) : run(run), key(key) {}
        virtual const char *getAsString() const override { return key.c_str(); }
        virtual const char *getAsString(const char *attribute) const override;
};

const char *MatchableConfigEntry::getAsString(const char *attribute) const
{
    if (strcmp(Scave::NAME, attribute) == 0)
        return key.c_str();
    else
        return run->getProperty(attribute);
}

RunAndValueList ResultFileManager::getMatchingItervars(const RunList& runs, const char *pattern) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    RunAndValueList out;
    for (Run *run : runs) {
        for (auto &iv : run->getIterationVariables()) {
            MatchableItervar matchable(run, iv.first);
            if (matchExpr.matches(&matchable))
                out.push_back({run, iv.first});
        }
    }
    return out;
}

RunAndValueList ResultFileManager::getMatchingRunattrs(const RunList& runs, const char *pattern) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    RunAndValueList out;
    for (Run *run : runs) {
        for (auto &ra : run->getAttributes()) {
            MatchableRunattr matchable(run, ra.first);
            if (matchExpr.matches(&matchable))
                out.push_back({run, ra.first});
        }
    }
    return out;
}

RunAndValueList ResultFileManager::getMatchingConfigEntries(const RunList& runs, const char *pattern) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    RunAndValueList out;
    for (Run *run : runs) {
        for (auto &entry : run->getConfigEntries()) {
            MatchableConfigEntry matchable(run, entry.first);
            if (matchExpr.matches(&matchable))
                out.push_back({run, entry.first});
        }
    }
    return out;
}

RunAndValueList ResultFileManager::getMatchingParamAssignmentConfigEntries(const RunList& runs, const char *pattern) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    RunAndValueList out;
    for (Run *run : runs) {
        for (auto &param : run->getParamAssignmentConfigEntries()) {
            MatchableConfigEntry matchable(run, param.first);
            if (matchExpr.matches(&matchable))
                out.push_back({run, param.first});
        }
    }
    return out;
}

RunAndValueList ResultFileManager::getMatchingNonParamAssignmentConfigEntries(const RunList& runs, const char *pattern) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    RunAndValueList out;
    for (Run *run : runs) {
        for (auto &entry : run->getNonParamAssignmentConfigEntries()) {
            MatchableConfigEntry matchable(run, entry.first);
            if (matchExpr.matches(&matchable))
                out.push_back({run, entry.first});
        }
    }
    return out;
}

IDList ResultFileManager::filterIDList(const IDList& idlist, const char *pattern, int limit, InterruptedFlag *interrupted) const
{
    if (opp_isblank(pattern))  // no filter
        throw opp_runtime_error("Empty filter expression is not allowed");

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    InterruptedFlag dummy;
    if (interrupted == nullptr)
        interrupted = &dummy;

    READER_MUTEX
    std::vector<ID> out;
    int count = 0;
    for (ID id : idlist) {
        if (interrupted->flag)
            throw InterruptedException("Result filtering interrupted");
        MatchableResultItem matchable(this, id);
        if (matchExpr.matches(&matchable)) {
            out.push_back(id);
            count++;
            if (limit > 0 && count == limit)
                break;
        }
    }
    return IDList(std::move(out));
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
    return statisticsResults.size() - 1;
}

int ResultFileManager::addHistogram(FileRun *fileRunRef, const char *moduleName, const char *histogramName,
        const Statistics& stat, const Histogram& bins, const StringMap& attrs)
{
    HistogramResult histogram(fileRunRef, moduleName, histogramName, attrs, stat, bins);
    HistogramResults& histograms = fileRunRef->histogramResults;
    histograms.push_back(histogram);
    return histograms.size() - 1;
}

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
                    break;
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
        serial++;
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

    serial++;

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
    return opp_strdictcmp(first.c_str(), second.c_str()) < 0;
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
        std::string::iterator end = std::find_if_not(start, str.end(), std::ref(opp_isdigit));
        str.replace(start, end, 1, '*');
    }
}

static bool replaceIndexWithWildcard(std::string& str)
{
    bool changed = false;
    std::string::iterator start = str.begin();
    while ((start = std::find_if(start, str.end(), opp_isdigit)) != str.end()) {
        std::string::iterator end = std::find_if_not(start, str.end(), std::ref(opp_isdigit));
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
    StringSet attrValues = getUniqueResultAttributeValues(idlist, attrName);
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
    if (!Run::isParamAssignmentConfigKey(key))
        return StringVector();
    return getConfigEntryFilterHints(runList, key);
}


bool ResultFileManager::hasStaleID(const IDList& ids) const
{
    READER_MUTEX
    for (ID id : ids)
        if (isStaleID(id))
            return true;
    return false;
}

const char *ResultFileManager::getRunAttribute(ID id, const char *attribute) const
{
    return getFileRun(id)->getRun()->getAttribute(attribute).c_str();
}

}  // namespace scave
}  // namespace omnetpp

