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

void HistogramResult::addBin(double lowerBound, double count)
{
    bins.addBin(lowerBound, count);
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

const std::string& Run::getParamAssignment(const std::string& key) const
{
    for (auto& p : paramAssignments)  // TODO some kind of ordered map would be better (e.g. std::map plus an std::vector<string> to store the order)
        if (p.first == key)
            return p.second;
    return NULLSTRING;
}

//------

ResultFileManager::ResultFileManager()
{

}

ResultFileManager::~ResultFileManager()
{
    for (int i = 0; i < (int)fileRunList.size(); i++)
        delete fileRunList[i];

    for (int i = 0; i < (int)runList.size(); i++)
        delete runList[i];

    for (int i = 0; i < (int)fileList.size(); i++)
        delete fileList[i];

    fileRunList.clear();
    runList.clear();
    fileList.clear();

    moduleNames.clear();
    names.clear();
    classNames.clear();
}

ResultFileList ResultFileManager::getFiles() const
{
    READER_MUTEX
    ResultFileList out;
    for (int i = 0; i < (int)fileList.size(); i++)
        if (fileList[i])
            out.push_back(fileList[i]);
    return out;
}

FileRunList ResultFileManager::getFileRunsInFile(ResultFile *file) const
{
    READER_MUTEX
    FileRunList out;
    for (int i = 0; i < (int)fileRunList.size(); i++)
        if (fileRunList[i]->fileRef == file)
            out.push_back(fileRunList[i]);
    return out;
}

RunList ResultFileManager::getRunsInFile(ResultFile *file) const
{
    READER_MUTEX
    RunList out;
    for (int i = 0; i < (int)fileRunList.size(); i++)
        if (fileRunList[i]->fileRef == file)
            out.push_back(fileRunList[i]->runRef);
    return out;
}

ResultFileList ResultFileManager::getFilesForRun(Run *run) const
{
    READER_MUTEX
    ResultFileList out;
    for (int i = 0; i < (int)fileRunList.size(); i++)
        if (fileRunList[i]->runRef == run)
            out.push_back(fileRunList[i]->fileRef);
    return out;
}

const ResultItem& ResultFileManager::getItem(ID id) const
{
    READER_MUTEX
    try
    {
        switch (_type(id)) {
            case SCALAR: return getFileForID(id)->scalarResults.at(_pos(id));
            case VECTOR: return getFileForID(id)->vectorResults.at(_pos(id));
            case STATISTICS: return getFileForID(id)->statisticsResults.at(_pos(id));
            case HISTOGRAM: return getFileForID(id)->histogramResults.at(_pos(id));
            default: throw opp_runtime_error("ResultFileManager: Invalid ID: Wrong type");
        }
    }
    catch (std::out_of_range& e) {
        throw opp_runtime_error("ResultFileManager::getItem(id): Invalid ID");
    }
}

ResultFileList *ResultFileManager::getUniqueFiles(const IDList& ids) const
{
    READER_MUTEX
    // collect unique runs in this dataset
    std::set<ResultFile *> set;
    for (int i = 0; i < ids.size(); i++)
        set.insert(getItem(ids.get(i)).getFile());

    // convert to list for easier handling at recipient
    return new ResultFileList(set.begin(), set.end());
}

RunList *ResultFileManager::getUniqueRuns(const IDList& ids) const
{
    READER_MUTEX
    // collect unique runs in this dataset
    std::set<Run *> set;
    for (int i = 0; i < ids.size(); i++)
        set.insert(getItem(ids.get(i)).getRun());

    // convert to list for easier handling at recipient
    return new RunList(set.begin(), set.end());
}

FileRunList *ResultFileManager::getUniqueFileRuns(const IDList& ids) const
{
    READER_MUTEX
    // collect unique FileRuns in this dataset
    std::set<FileRun *> set;
    for (int i = 0; i < ids.size(); i++)
        set.insert(getItem(ids.get(i)).getFileRun());

    // convert to list for easier handling at recipient
    return new FileRunList(set.begin(), set.end());
}

StringSet *ResultFileManager::getUniqueModuleNames(const IDList& ids) const
{
    READER_MUTEX
    // collect unique module names in this dataset
    StringSet *set = new StringSet();
    for (int i = 0; i < ids.size(); i++)
        set->insert(getItem(ids.get(i)).getModuleName());
    return set;
}

StringSet *ResultFileManager::getUniqueNames(const IDList& ids) const
{
    READER_MUTEX
    // collect unique scalar/vector names in this dataset
    StringSet *set = new StringSet();
    for (int i = 0; i < ids.size(); i++)
        set->insert(getItem(ids.get(i)).getName());
    return set;
}

StringSet *ResultFileManager::getUniqueModuleAndResultNamePairs(const IDList& ids) const
{
    READER_MUTEX
    // collect unique scalar/vector names in this dataset
    StringSet *set = new StringSet();
    for (int i = 0; i < ids.size(); i++) {
        const ResultItem& result = getItem(ids.get(i));
        set->insert(result.getModuleName() + "." + result.getName());
    }
    return set;
}

StringSet *ResultFileManager::getUniqueAttributeNames(const IDList& ids) const
{
    READER_MUTEX
    StringSet *set = new StringSet;
    for (int i = 0; i < ids.size(); i++) {
        const StringMap& attributes = getItem(ids.get(i)).getAttributes();
        for (StringMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
            set->insert(it->first);
    }
    return set;
}

StringSet *ResultFileManager::getUniqueAttributeValues(const IDList& ids, const char *attrName) const
{
    READER_MUTEX
    StringSet *values = new StringSet;
    for (int i = 0; i < ids.size(); ++i) {
        const string& value = getItem(ids.get(i)).getAttribute(attrName);
        if (&value != &NULLSTRING)
            values->insert(value);
    }
    return values;
}

StringSet *ResultFileManager::getUniqueRunAttributeNames(const RunList *runList) const
{
    READER_MUTEX
    StringSet *set = new StringSet;
    for (RunList::const_iterator runRef = runList->begin(); runRef != runList->end(); ++runRef) {
        const StringMap& attributes = (*runRef)->getAttributes();
        for (StringMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
            set->insert(it->first);
    }
    return set;
}

StringSet *ResultFileManager::getUniqueRunAttributeValues(const RunList& runList, const char *attrName) const
{
    READER_MUTEX
    StringSet *values = new StringSet;
    for (RunList::const_iterator runRef = runList.begin(); runRef != runList.end(); ++runRef) {
        const string& value = (*runRef)->getAttribute(attrName);
        if (&value != &NULLSTRING)
            values->insert(value);
    }

    return values;
}

StringSet *ResultFileManager::getUniqueIterationVariableNames(const RunList *runList) const
{
    READER_MUTEX
    StringSet *set = new StringSet;
    for (RunList::const_iterator runRef = runList->begin(); runRef != runList->end(); ++runRef) {
        const StringMap& itervars = (*runRef)->getIterationVariables();
        for (StringMap::const_iterator it = itervars.begin(); it != itervars.end(); ++it)
            set->insert(it->first);
    }
    return set;
}

StringSet *ResultFileManager::getUniqueIterationVariableValues(const RunList& runList, const char *itervarName) const
{
    READER_MUTEX
    StringSet *values = new StringSet;
    for (RunList::const_iterator runRef = runList.begin(); runRef != runList.end(); ++runRef) {
        const string& value = (*runRef)->getIterationVariable(itervarName);
        if (&value != &NULLSTRING)
            values->insert(value);
    }

    return values;
}


StringSet *ResultFileManager::getUniqueParamAssignmentKeys(const RunList *runList) const
{
    READER_MUTEX
    StringSet *set = new StringSet;
    for (auto runRef : *runList)
        for (auto& p : runRef->getParamAssignments())
            set->insert(p.first);
    return set;
}


StringSet *ResultFileManager::getUniqueParamAssignmentValues(const RunList& runList, const char *key) const
{
    READER_MUTEX
    StringSet *values = new StringSet;
    for (auto runRef : runList) {
        const string& value = runRef->getParamAssignment(key);
        if (&value != &NULLSTRING)
            values->insert(value);
    }

    return values;
}

const ScalarResult& ResultFileManager::getScalar(ID id) const
{
    READER_MUTEX
    if (_type(id) != SCALAR)
        throw opp_runtime_error("ResultFileManager::getScalar(id): This item is not a scalar");
    return getFileForID(id)->scalarResults.at(_pos(id));
}

const VectorResult& ResultFileManager::getVector(ID id) const
{
    READER_MUTEX
    if (_type(id) != VECTOR)
        throw opp_runtime_error("ResultFileManager::getVector(id): This item is not a vector");
    return getFileForID(id)->vectorResults.at(_pos(id));
}

const StatisticsResult& ResultFileManager::getStatistics(ID id) const
{
    READER_MUTEX
    if (_type(id) == STATISTICS)
        return getFileForID(id)->statisticsResults.at(_pos(id));
    else if (_type(id) == HISTOGRAM)
        return getFileForID(id)->histogramResults.at(_pos(id));
    else
        throw opp_runtime_error("ResultFileManager::getStatistics(id): This item is not a summary statistics or a histogram");
}

const HistogramResult& ResultFileManager::getHistogram(ID id) const
{
    READER_MUTEX
    if (_type(id) != HISTOGRAM)
        throw opp_runtime_error("ResultFileManager::getHistogram(id): This item is not a histogram");
    return getFileForID(id)->histogramResults.at(_pos(id));
}

template<class T>
void ResultFileManager::collectIDs(IDList& out, std::vector<T> ResultFile::*vec, int type, bool includeFields, bool includeItervars) const
{
    for (int k = 0; k < (int)fileList.size(); k++) {
        if (fileList[k] != nullptr) {
            std::vector<T>& v = fileList[k]->*vec;
            for (int i = 0; i < (int)v.size(); i++) {
                bool isField = type == SCALAR ? ((ScalarResult&)v[i]).isField() : false;

                if (!isField || includeFields)
                    out.uncheckedAdd(_mkID(type, k, i));
            }
        }
    }
}

IDList ResultFileManager::getAllItems(bool includeFields, bool includeItervars) const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::scalarResults, SCALAR, includeFields, includeItervars);
    collectIDs(out, &ResultFile::vectorResults, VECTOR, includeFields, includeItervars);
    collectIDs(out, &ResultFile::statisticsResults, STATISTICS, includeFields, includeItervars);
    collectIDs(out, &ResultFile::histogramResults, HISTOGRAM, includeFields, includeItervars);
    return out;
}

IDList ResultFileManager::getAllScalars(bool includeFields, bool includeItervars) const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::scalarResults, SCALAR, includeFields, includeItervars);
    return out;
}

IDList ResultFileManager::getAllVectors() const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::vectorResults, VECTOR, true, true);
    return out;
}

IDList ResultFileManager::getAllStatistics() const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::statisticsResults, STATISTICS, true, true);
    return out;
}

IDList ResultFileManager::getAllHistograms() const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::histogramResults, HISTOGRAM, true, true);
    return out;
}

IDList ResultFileManager::getScalarsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX
    IDList out;
    int fileId = fileRun->fileRef->id;
    ScalarResults& v = fileRun->fileRef->scalarResults;
    for (int i = 0; i < (int)v.size(); i++)
        if (v[i].getFileRun() == fileRun)
            out.uncheckedAdd(_mkID(SCALAR, fileId, i));

    return out;
}

IDList ResultFileManager::getVectorsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX
    IDList out;
    int fileId = fileRun->fileRef->id;
    VectorResults& v = fileRun->fileRef->vectorResults;
    for (int i = 0; i < (int)v.size(); i++)
        if (v[i].getFileRun() == fileRun)
            out.uncheckedAdd(_mkID(VECTOR, fileId, i));

    return out;
}

IDList ResultFileManager::getStatisticsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX
    IDList out;
    int fileId = fileRun->fileRef->id;
    StatisticsResults& v = fileRun->fileRef->statisticsResults;
    for (int i = 0; i < (int)v.size(); i++)
        if (v[i].getFileRun() == fileRun)
            out.uncheckedAdd(_mkID(STATISTICS, fileId, i));

    return out;
}

IDList ResultFileManager::getHistogramsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX
    IDList out;
    int fileId = fileRun->fileRef->id;
    HistogramResults& v = fileRun->fileRef->histogramResults;
    for (int i = 0; i < (int)v.size(); i++)
        if (v[i].getFileRun() == fileRun)
            out.uncheckedAdd(_mkID(HISTOGRAM, fileId, i));

    return out;
}

bool ResultFileManager::isFileLoaded(const char *fileName) const
{
    return getFile(fileName) != nullptr;
}

ResultFile *ResultFileManager::getFile(const char *fileName) const
{
    if (!fileName)
        return nullptr;

    READER_MUTEX
    for (int i = 0; i < (int)fileList.size(); i++)
        if (fileList[i] != nullptr && fileList[i]->getFilePath() == fileName)
            return fileList[i];

    return nullptr;
}

Run *ResultFileManager::getRunByName(const char *runName) const
{
    if (!runName)
        return nullptr;

    READER_MUTEX
    for (int i = 0; i < (int)runList.size(); i++)
        if (runList[i]->getRunName() == runName)
            return runList[i];

    return nullptr;
}

FileRun *ResultFileManager::getFileRun(ResultFile *file, Run *run) const
{
    READER_MUTEX
    for (int i = 0; i < (int)fileRunList.size(); i++)
        if (fileRunList[i]->fileRef == file && fileRunList[i]->runRef == run)
            return fileRunList[i];

    return nullptr;
}

// currently unused
ID ResultFileManager::getItemByName(FileRun *fileRunRef, const char *module, const char *name) const
{
    if (!fileRunRef || !module || !name)
        return 0;

    READER_MUTEX

    const std::string *moduleNameRef = moduleNames.find(module); //TODO not needed
    if (!moduleNameRef)
        return 0;

    const std::string *nameRef = names.find(name);
    if (!nameRef)
        return 0;

    ScalarResults& scalarResults = fileRunRef->fileRef->scalarResults;
    for (int i = 0; i < (int)scalarResults.size(); i++) {
        const ResultItem& d = scalarResults[i];
        if (d.getModuleName() == *moduleNameRef && d.getName() == *nameRef && d.getFileRun() == fileRunRef)
            return _mkID(SCALAR, fileRunRef->fileRef->id, i);
    }

    VectorResults& vectorResults = fileRunRef->fileRef->vectorResults;
    for (int i = 0; i < (int)vectorResults.size(); i++) {
        const ResultItem& d = vectorResults[i];
        if (d.getModuleName() == *moduleNameRef && d.getName() == *nameRef && d.getFileRun() == fileRunRef)
            return _mkID(VECTOR, fileRunRef->fileRef->id, i);
    }

    StatisticsResults& statisticsResults = fileRunRef->fileRef->statisticsResults;
    for (int i = 0; i < (int)statisticsResults.size(); i++) {
        const ResultItem& d = statisticsResults[i];
        if (d.getModuleName() == *moduleNameRef && d.getName() == *nameRef && d.getFileRun() == fileRunRef)
            return _mkID(STATISTICS, fileRunRef->fileRef->id, i);
    }

    HistogramResults& histogramResults = fileRunRef->fileRef->histogramResults;
    for (int i = 0; i < (int)histogramResults.size(); i++) {
        const ResultItem& d = histogramResults[i];
        if (d.getModuleName() == *moduleNameRef && d.getName() == *nameRef && d.getFileRun() == fileRunRef)
            return _mkID(HISTOGRAM, fileRunRef->fileRef->id, i);
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

ResultFileList ResultFileManager::filterFileList(const ResultFileList& fileList,
        const char *filePathFilter) const
{
    ResultFileList out;

    // filePath matcher
    PatternMatcher filePathPattern(filePathFilter, false, true, true);

    READER_MUTEX
    for (int i = 0; i < (int)fileList.size(); i++) {
        ResultFile *file = fileList[i];
        if (!filePathPattern.matches(file->getFilePath().c_str()))
            continue;
        out.push_back(file);
    }
    return out;
}

FileRunList ResultFileManager::getFileRuns(const ResultFileList *fileList, const RunList *runList) const
{
    READER_MUTEX
    FileRunList out;
    for (int i = 0; i < (int)fileRunList.size(); i++) {
        FileRun *fileRun = fileRunList[i];
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
        // (note: uncheckedAdd() is fine: if input IDList didn't contain duplicates,
        // the result won't either)
        out.uncheckedAdd(id);
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
        // (note: uncheckedAdd() is fine: if input IDList didn't contain duplicates,
        // the result won't either)
        result.uncheckedAdd(id);
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
        const char *getParamAssignment(const char *key) const { return item.getRun()->getParamAssignment(key).c_str(); }
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
    else if (strncasecmp("runattr:", attribute, 8) == 0)
        return getRunAttribute(attribute+8);
    else if (strncasecmp("itervar:", attribute, 8) == 0)
        return getIterationVariable(attribute+8);
    else if (strncasecmp("param:", attribute, 6) == 0)
        return getParamAssignment(attribute+6);
    else
        return getResultItemAttribute(attribute);
}

const char *MatchableResultItem::getItemType() const
{
    switch (item.getItemType()) {
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
        const char *getParamAssignment(const char *key) const { return run->getParamAssignment(key).c_str(); }
};

const char *MatchableRun::getAsString(const char *attribute) const
{
    if (strcasecmp("name", attribute) == 0)
        return getName();
    else if (strncasecmp("attr:", attribute, 5) == 0) // TODO: also add with runattr: prefix?
        return getAttribute(attribute+5);
    else if (strncasecmp("itervar:", attribute, 8) == 0)
        return getIterationVariable(attribute+8);
    else if (strncasecmp("param:", attribute, 6) == 0)
        return getParamAssignment(attribute+6);
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
        const char *getParamAssignment(const char *key) const { return run->getParamAssignment(key).c_str(); }
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
    else if (strncasecmp("param:", attribute, 6) == 0)
        return getParamAssignment(attribute+6);
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
        const char *getParamAssignment(const char *key) const { return run->getParamAssignment(key).c_str(); }
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
    else if (strncasecmp("param:", attribute, 6) == 0)
        return getParamAssignment(attribute+6);
    else
        return getName();
}



std::vector<std::pair<std::string, std::string>> ResultFileManager::getMatchingItervars(const char *pattern) const
{
    if (pattern == nullptr || pattern[0] == '\0')  // no filter
        pattern = "*";

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    std::vector<std::pair<std::string, std::string>> out;
    for (int k = 0; k < (int)runList.size(); k++) {
        if (runList[k] != nullptr) {
            Run *run = runList[k];
            int i = 0;
            for (auto &iv : run->getIterationVariables()) {

                MatchableItervar matchable(run, iv.first);

                if (matchExpr.matches(&matchable))
                    out.push_back({run->getRunName(), iv.first});

                ++i;
            }
        }
    }
    return out;
}

std::vector<std::pair<std::string, std::string>> ResultFileManager::getMatchingRunattrs(const char *pattern) const
{
    if (pattern == nullptr || pattern[0] == '\0')  // no filter
        pattern = "*";

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    std::vector<std::pair<std::string, std::string>> out;
    for (int k = 0; k < (int)runList.size(); k++) {
        if (runList[k] != nullptr) {
            Run *run = runList[k];
            int i = 0;
            for (auto &ra : run->getAttributes()) {

                MatchableRunattr matchable(run, ra.first);

                if (matchExpr.matches(&matchable))
                    out.push_back({run->getRunName(), ra.first});

                ++i;
            }
        }
    }
    return out;
}

IDList ResultFileManager::filterIDList(const IDList& idlist, const char *pattern) const
{
    if (pattern == nullptr || pattern[0] == '\0')  // no filter
        pattern = "*";

    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);

    READER_MUTEX
    IDList out;
    int sz = idlist.size();
    for (int i = 0; i < sz; ++i) {
        ID id = idlist.get(i);
        const ResultItem& item = getItem(id);
        MatchableResultItem matchable(item);
        if (matchExpr.matches(&matchable))
            out.uncheckedAdd(id);
    }
    return out;
}

RunList ResultFileManager::filterRunList(const RunList& runlist, const char *pattern) const
{
    if (pattern == nullptr || pattern[0] == '\0')  // no filter
        pattern = "*";

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
    if (pattern == nullptr || pattern[0] == '\0')  // no filter
        return;

    // parse it
    // XXX after successful parsing, we could also check that attribute names in it are valid
    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);
}

ResultFile *ResultFileManager::addFile(const char *fileName, const char *fileSystemFileName, ResultFile::FileType fileType)
{
    ResultFile *file = new ResultFile();
    file->id = fileList.size();
    fileList.push_back(file);
    file->resultFileManager = this;
    file->fileSystemFilePath = fileSystemFileName;
    file->filePath = fileNameToSlash(fileName);
    splitFileName(file->filePath.c_str(), file->directory, file->fileName);
    file->fileType = fileType;
    return file;
}

Run *ResultFileManager::addRun(const std::string& runName)
{
    Run *run = new Run(runName, this);
    runList.push_back(run);
    return run;
}


FileRun *ResultFileManager::addFileRun(ResultFile *file, Run *run)
{
    FileRun *fileRun = new FileRun();
    fileRunList.push_back(fileRun);
    fileRun->fileRef = file;
    fileRun->runRef = run;
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
    ScalarResults& scalars = fileRunRef->fileRef->scalarResults;
    scalars.push_back(scalar);
    return scalars.size() - 1;
}

int ResultFileManager::addVector(FileRun *fileRunRef, int vectorId, const char *moduleName, const char *vectorName, const StringMap& attrs, const char *columns)
{
    VectorResult vector(fileRunRef, moduleName, vectorName, attrs, vectorId, columns);
    vector.stat = Statistics::makeInvalid(false);
    VectorResults& vectors = fileRunRef->fileRef->vectorResults;
    vectors.push_back(vector);
    return vectors.size() - 1;
}

int ResultFileManager::addStatistics(FileRun *fileRunRef, const char *moduleName, const char *statisticsName,
        const Statistics& stat, const StringMap& attrs)
{
    StatisticsResult statistics(fileRunRef, moduleName, statisticsName, attrs, stat);
    StatisticsResults& statisticsResults = fileRunRef->fileRef->statisticsResults;
    statisticsResults.push_back(statistics);
    if (true) //TODO flag
        addStatisticsFieldsAsScalars(fileRunRef, moduleName, statisticsName, stat);
    return statisticsResults.size() - 1;
}

int ResultFileManager::addHistogram(FileRun *fileRunRef, const char *moduleName, const char *histogramName,
        const Statistics& stat, const Histogram& bins, const StringMap& attrs)
{
    HistogramResult histogram(fileRunRef, moduleName, histogramName, attrs, stat, bins);
    HistogramResults& histograms = fileRunRef->fileRef->histogramResults;
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

ResultFile *ResultFileManager::loadFile(const char *fileName, const char *fileSystemFileName, bool reload)
{
    WRITER_MUTEX

    // tricky part: we store "fileName" which is the Eclipse pathname of the file
    // (or some other "display name" of the file), but we actually load from
    // fileSystemFileName which is the fileName suitable for fopen()

    // check if loaded
    ResultFile *fileRef = getFile(fileName);
    if (fileRef) {
        if (reload)
            unloadFile(fileRef);
        else
            return fileRef;
    }

    // try if file can be opened, before we add it to our database
    if (fileSystemFileName == nullptr)
        fileSystemFileName = fileName;
    if (!isFileReadable(fileSystemFileName))
        throw opp_runtime_error("Cannot open '%s' for read", fileSystemFileName);

    ResultFile *file = SqliteResultFileUtils::isSqliteFile(fileSystemFileName) ?
        SqliteResultFileLoader(this).loadFile(fileName, fileSystemFileName, reload) :
        OmnetppResultFileLoader(this).loadFile(fileName, fileSystemFileName, reload);

    Assert(file != nullptr);
    return file;
}

void ResultFileManager::unloadFile(ResultFile *file)
{
    WRITER_MUTEX

    // remove FileRun entries
    RunList runsPotentiallyToBeDeleted;
    for (int i = 0; i < (int)fileRunList.size(); i++) {
        if (fileRunList[i]->fileRef == file) {
            // remember Run; if this was the last reference, we need to delete it
            runsPotentiallyToBeDeleted.push_back(fileRunList[i]->runRef);

            // delete fileRun
            delete fileRunList[i];
            fileRunList.erase(fileRunList.begin()+i);
            i--;
        }
    }

    // delete ResultFile entry. Note that the fileList array will have a hole.
    // It is not allowed to move another ResultFile into the hole, because
    // that would change its "id", and invalidate existing IDs (IDLists)
    // that use that file.
    fileList[file->id] = nullptr;
    delete file;

    // remove Runs that don't appear in other loaded files
    for (int i = 0; i < (int)runsPotentiallyToBeDeleted.size(); i++) {
        Run *runRef = runsPotentiallyToBeDeleted[i];
        if (getFilesForRun(runRef).empty()) {
            // delete it.
            RunList::iterator it = std::find(runList.begin(), runList.end(), runRef);
            assert(it != runList.end());  // runs may occur only once in runsPotentiallyToBeDeleted, because runNames are not allowed to repeat in files

            delete *it;
            runList.erase(it);
        }
    }
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

StringVector *ResultFileManager::getFilePathFilterHints(const ResultFileList& fileList) const
{
    READER_MUTEX
    StringVector *filterHints = new StringVector;
    for (ResultFileList::const_iterator it = fileList.begin(); it != fileList.end(); ++it)
        filterHints->push_back((*it)->getFilePath());
    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    return filterHints;
}

StringVector *ResultFileManager::getRunNameFilterHints(const RunList& runList) const
{
    READER_MUTEX
    StringVector *filterHints = new StringVector;
    for (RunList::const_iterator it = runList.begin(); it != runList.end(); ++it)
        if ((*it)->getRunName().size() > 0)
            filterHints->push_back((*it)->getRunName());

    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    return filterHints;
}

StringVector *ResultFileManager::getModuleFilterHints(const IDList& idlist) const
{
    READER_MUTEX
    StringSet& names = *getUniqueModuleNames(idlist);

    SortedStringSet nameHints;
    DuplicateStringCollector coll;

    for (StringSet::iterator i = names.begin(); i != names.end(); i++) {
        std::string a = (*i);

        // replace embedded numbers with "*"
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
    delete &names;

    // sort and concatenate them, and return the result
    StringVector *wildvec = new StringVector(coll.get());
    wildvec->push_back(std::string("*"));
    std::sort(wildvec->begin(), wildvec->end(), strdictLess);
    wildvec->insert(wildvec->end(), nameHints.begin(), nameHints.end());
    return wildvec;
}

StringVector *ResultFileManager::getNameFilterHints(const IDList& idlist) const
{
    READER_MUTEX
    StringSet& names = *getUniqueNames(idlist);

    StringVector vec;
    DuplicateStringCollector coll;

    for (StringSet::iterator i = names.begin(); i != names.end(); i++) {
        std::string a = (*i);
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
    delete &names;

    // sort and concatenate them, and return the result
    StringVector *wildvec = new StringVector(coll.get());
    wildvec->push_back(std::string("*"));
    std::sort(vec.begin(), vec.end(), strdictLess);
    std::sort(wildvec->begin(), wildvec->end(), strdictLess);
    wildvec->insert(wildvec->end(), vec.begin(), vec.end());
    return wildvec;
}

StringVector *ResultFileManager::getResultItemAttributeFilterHints(const IDList& idlist, const char *attrName) const
{
    READER_MUTEX
    StringSet *attrValues = getUniqueAttributeValues(idlist, attrName);
    StringVector *filterHints = new StringVector(attrValues->begin(), attrValues->end());
    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    delete attrValues;
    return filterHints;
}

StringVector *ResultFileManager::getRunAttributeFilterHints(const RunList& runList, const char *attrName) const
{
    READER_MUTEX
    StringSet *attrValues = getUniqueRunAttributeValues(runList, attrName);
    StringVector *filterHints = new StringVector(attrValues->begin(), attrValues->end());
    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    delete attrValues;
    return filterHints;
}

StringVector *ResultFileManager::getIterationVariableFilterHints(const RunList& runList, const char *itervarName) const
{
    READER_MUTEX
    StringSet *values = getUniqueIterationVariableValues(runList, itervarName);
    StringVector *filterHints = new StringVector(values->begin(), values->end());
    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    delete values;
    return filterHints;
}

StringVector *ResultFileManager::getParamAssignmentFilterHints(const RunList& runList, const char *key) const
{
    READER_MUTEX
    StringSet *paramValues = getUniqueParamAssignmentValues(runList, key);
    StringVector *filterHints = new StringVector(paramValues->begin(), paramValues->end());
    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    delete paramValues;
    return filterHints;
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

