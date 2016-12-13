//=========================================================================
//  RESULTFILEMANAGER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

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
#include "omnetpp/platdep/platmisc.h"
#include "indexfile.h"
#include "scaveutils.h"
#include "scaveexception.h"
#include "sqliteresultfileutils.h"
#include "resultfilemanager.h"
#include "omnetppresultfileloader.h"
#include "sqliteresultfileloader.h"


#ifdef THREADED
#define READER_MUTEX    Mutex __reader_mutex_(getReadLock());
#define WRITER_MUTEX    Mutex __writer_mutex_(getWriteLock());
#else
#define READER_MUTEX
#define WRITER_MUTEX
#endif

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {


ResultItem& ResultItem::operator=(const ResultItem& rhs)
{
    if (this == &rhs)
        return *this;

    fileRunRef = rhs.fileRunRef;
    moduleNameRef = rhs.moduleNameRef;
    nameRef = rhs.nameRef;
    attributes = rhs.attributes;
    if (computation)
        delete computation;
    computation = rhs.computation ? rhs.computation->dup() : nullptr;
    return *this;
}

ResultItem::Type ResultItem::getType() const
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

void HistogramResult::addBin(double lower_bound, double value)
{
    if (!bins.empty() && bins.back() >= lower_bound)
        throw opp_runtime_error("Histogram bin bounds must be specified in increasing order");
    bins.push_back(lower_bound);
    values.push_back(value);
}

ResultFileManager::ResultFileManager()
{
    computedScalarFile = nullptr;
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
        if (fileList[i] && !fileList[i]->computed)
            out.push_back(fileList[i]);
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
        if (fileRunList[i]->runRef == run && !fileRunList[i]->fileRef->computed)
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
        set.insert(getItem(ids.get(i)).fileRunRef->fileRef);

    // convert to list for easier handling at recipient
    return new ResultFileList(set.begin(), set.end());
}

RunList *ResultFileManager::getUniqueRuns(const IDList& ids) const
{
    READER_MUTEX
    // collect unique runs in this dataset
    std::set<Run *> set;
    for (int i = 0; i < ids.size(); i++)
        set.insert(getItem(ids.get(i)).fileRunRef->runRef);

    // convert to list for easier handling at recipient
    return new RunList(set.begin(), set.end());
}

FileRunList *ResultFileManager::getUniqueFileRuns(const IDList& ids) const
{
    READER_MUTEX
    // collect unique FileRuns in this dataset
    std::set<FileRun *> set;
    for (int i = 0; i < ids.size(); i++)
        set.insert(getItem(ids.get(i)).fileRunRef);

    // convert to list for easier handling at recipient
    return new FileRunList(set.begin(), set.end());
}

StringSet *ResultFileManager::getUniqueModuleNames(const IDList& ids) const
{
    READER_MUTEX
    // collect unique module names in this dataset
    StringSet *set = new StringSet();
    for (int i = 0; i < ids.size(); i++)
        set->insert(*getItem(ids.get(i)).moduleNameRef);
    return set;
}

StringSet *ResultFileManager::getUniqueNames(const IDList& ids) const
{
    READER_MUTEX
    // collect unique scalar/vector names in this dataset
    StringSet *set = new StringSet();
    for (int i = 0; i < ids.size(); i++)
        set->insert(*getItem(ids.get(i)).nameRef);
    return set;
}

StringSet *ResultFileManager::getUniqueModuleAndResultNamePairs(const IDList& ids) const
{
    READER_MUTEX
    // collect unique scalar/vector names in this dataset
    StringSet *set = new StringSet();
    for (int i = 0; i < ids.size(); i++) {
        const ResultItem& result = getItem(ids.get(i));
        set->insert(*result.moduleNameRef + "." + *result.nameRef);
    }
    return set;
}

StringSet *ResultFileManager::getUniqueAttributeNames(const IDList& ids) const
{
    READER_MUTEX
    StringSet *set = new StringSet;
    for (int i = 0; i < ids.size(); i++) {
        const StringMap& attributes = getItem(ids.get(i)).attributes;
        for (StringMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
            set->insert(it->first);
    }
    return set;
}

StringSet *ResultFileManager::getUniqueRunAttributeNames(const RunList *runList) const
{
    READER_MUTEX
    StringSet *set = new StringSet;
    for (RunList::const_iterator runRef = runList->begin(); runRef != runList->end(); ++runRef) {
        const StringMap& attributes = (*runRef)->attributes;
        for (StringMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
            set->insert(it->first);
    }
    return set;
}

StringSet *ResultFileManager::getUniqueModuleParamNames(const RunList *runList) const
{
    READER_MUTEX
    StringSet *set = new StringSet;
    for (RunList::const_iterator runRef = runList->begin(); runRef != runList->end(); ++runRef) {
        const StringMap& params = (*runRef)->moduleParams;
        for (StringMap::const_iterator it = params.begin(); it != params.end(); ++it)
            set->insert(it->first);
    }
    return set;
}

StringSet *ResultFileManager::getUniqueAttributeValues(const IDList& ids, const char *attrName) const
{
    READER_MUTEX
    StringSet *values = new StringSet;
    for (int i = 0; i < ids.size(); ++i) {
        const char *value = getItem(ids.get(i)).getAttribute(attrName);
        if (value != nullptr)
            values->insert(value);
    }
    return values;
}

StringSet *ResultFileManager::getUniqueRunAttributeValues(const RunList& runList, const char *attrName) const
{
    READER_MUTEX
    StringSet *values = new StringSet;
    for (RunList::const_iterator runRef = runList.begin(); runRef != runList.end(); ++runRef) {
        const char *value = (*runRef)->getAttribute(attrName);
        if (value != nullptr)
            values->insert(value);
    }

    return values;
}

StringSet *ResultFileManager::getUniqueModuleParamValues(const RunList& runList, const char *paramName) const
{
    READER_MUTEX
    StringSet *values = new StringSet;
    for (RunList::const_iterator runRef = runList.begin(); runRef != runList.end(); ++runRef) {
        const char *value = (*runRef)->getModuleParam(paramName);
        if (value != nullptr)
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

const HistogramResult& ResultFileManager::getHistogram(ID id) const
{
    READER_MUTEX
    if (_type(id) != HISTOGRAM)
        throw opp_runtime_error("ResultFileManager::getHistogram(id): This item is not a histogram");
    return getFileForID(id)->histogramResults.at(_pos(id));
}

template<class T>
void ResultFileManager::collectIDs(IDList& out, std::vector<T> ResultFile::*vec, int type, bool includeComputed, bool includeFields) const
{
    for (int k = 0; k < (int)fileList.size(); k++) {
        if (fileList[k] != nullptr) {
            std::vector<T>& v = fileList[k]->*vec;
            for (int i = 0; i < (int)v.size(); i++) {
                bool isComputed = v[i].isComputed();
                bool isField = type == SCALAR ? ((ScalarResult&)v[i]).isField : false;

                if ((!isField || includeFields) && (!isComputed || includeComputed))
                    out.uncheckedAdd(_mkID(isComputed, isField, type, k, i));
            }
        }
    }
}

IDList ResultFileManager::getAllItems(bool includeComputed, bool includeFields) const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::scalarResults, SCALAR, includeComputed, includeFields);
    collectIDs(out, &ResultFile::vectorResults, VECTOR, includeComputed, includeFields);
    collectIDs(out, &ResultFile::histogramResults, HISTOGRAM, includeComputed, includeFields);
    return out;
}

IDList ResultFileManager::getAllScalars(bool includeComputed, bool includeFields) const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::scalarResults, SCALAR, includeComputed, includeFields);
    return out;
}

IDList ResultFileManager::getAllVectors(bool includeComputed) const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::vectorResults, VECTOR, includeComputed);
    return out;
}

IDList ResultFileManager::getAllHistograms(bool includeComputed) const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::histogramResults, HISTOGRAM, includeComputed);
    return out;
}

IDList ResultFileManager::getScalarsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX
    IDList out;
    int fileId = fileRun->fileRef->id;
    ScalarResults& v = fileRun->fileRef->scalarResults;
    for (int i = 0; i < (int)v.size(); i++)
        if (v[i].fileRunRef == fileRun && !v[i].isComputed())
            out.uncheckedAdd(_mkID(false, v[i].isField, SCALAR, fileId, i));

    return out;
}

IDList ResultFileManager::getVectorsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX
    IDList out;
    int fileId = fileRun->fileRef->id;
    VectorResults& v = fileRun->fileRef->vectorResults;
    for (int i = 0; i < (int)v.size(); i++)
        if (v[i].fileRunRef == fileRun && !v[i].isComputed())
            out.uncheckedAdd(_mkID(false, false, VECTOR, fileId, i));

    return out;
}

IDList ResultFileManager::getHistogramsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX
    IDList out;
    int fileId = fileRun->fileRef->id;
    HistogramResults& v = fileRun->fileRef->histogramResults;
    for (int i = 0; i < (int)v.size(); i++)
        if (v[i].fileRunRef == fileRun && !v[i].isComputed())
            out.uncheckedAdd(_mkID(false, false, HISTOGRAM, fileId, i));

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
        if (fileList[i] != nullptr && fileList[i]->filePath == fileName)
            return fileList[i];

    return nullptr;
}

Run *ResultFileManager::getRunByName(const char *runName) const
{
    if (!runName)
        return nullptr;

    READER_MUTEX
    for (int i = 0; i < (int)runList.size(); i++)
        if (runList[i]->runName == runName)
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

    const std::string *moduleNameRef = moduleNames.find(module);
    if (!moduleNameRef)
        return 0;

    const std::string *nameRef = names.find(name);
    if (!nameRef)
        return 0;

    ScalarResults& scalarResults = fileRunRef->fileRef->scalarResults;
    for (int i = 0; i < (int)scalarResults.size(); i++) {
        const ResultItem& d = scalarResults[i];
        if (d.moduleNameRef == moduleNameRef && d.nameRef == nameRef && d.fileRunRef == fileRunRef)
            return _mkID(d.isComputed(), scalarResults[i].isField, SCALAR, fileRunRef->fileRef->id, i);
    }

    VectorResults& vectorResults = fileRunRef->fileRef->vectorResults;
    for (int i = 0; i < (int)vectorResults.size(); i++) {
        const ResultItem& d = vectorResults[i];
        if (d.moduleNameRef == moduleNameRef && d.nameRef == nameRef && d.fileRunRef == fileRunRef)
            return _mkID(d.isComputed(), false, VECTOR, fileRunRef->fileRef->id, i);
    }

    HistogramResults& histogramResults = fileRunRef->fileRef->histogramResults;
    for (int i = 0; i < (int)histogramResults.size(); i++) {
        const ResultItem& d = histogramResults[i];
        if (d.moduleNameRef == moduleNameRef && d.nameRef == nameRef && d.fileRunRef == fileRunRef)
            return _mkID(d.isComputed(), false, HISTOGRAM, fileRunRef->fileRef->id, i);
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
        if (!runNamePattern.matches(run->runName.c_str()))
            continue;
        bool matches = true;
        for (int j = 0; j < (int)attrNames.size() && matches; j++) {
            const char *attrValue = run->getAttribute(attrNames[j].c_str());
            if (attrValue == nullptr)
                attrValue = "";

            if (!attrValues[j].matches(attrValue)) {
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
        if (!filePathPattern.matches(file->filePath.c_str()))
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
            if (lastFileRunRef != d.fileRunRef) {
                lastFileRunRef = d.fileRunRef;
                lastFileRunMatched = std::find(fileRunFilter->begin(), fileRunFilter->end(), d.fileRunRef) != fileRunFilter->end();
            }
            if (!lastFileRunMatched)
                continue;
        }

        if (moduleFilter && moduleFilter[0] &&
            (patMatchModule ? !modulePattern->matches(d.moduleNameRef->c_str())
             : strcmp(d.moduleNameRef->c_str(), moduleFilter))
            )
            continue;  // no match

        if (nameFilter && nameFilter[0] &&
            (patMatchName ? !namePattern->matches(d.nameRef->c_str())
             : strcmp(d.nameRef->c_str(), nameFilter))
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

        if (run && d.fileRunRef->runRef != run)
            continue;

        if (moduleName && (*d.moduleNameRef) != moduleName)
            continue;

        if (name && (*d.nameRef != name))
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
        virtual const char *getAsString() const override;
        virtual const char *getAsString(const char *attribute) const override;
    private:
        const char *getName() const { return item.nameRef->c_str(); }
        const char *getModuleName() const { return item.moduleNameRef->c_str(); }
        const char *getFileName() const { return item.fileRunRef->fileRef->filePath.c_str(); }
        const char *getRunName() const { return item.fileRunRef->runRef->runName.c_str(); }
        const char *getResultItemAttribute(const char *attrName) const { return item.getAttribute(attrName); }
        const char *getRunAttribute(const char *attrName) const { return item.fileRunRef->runRef->getAttribute(attrName); }
        const char *getModuleParam(const char *paramName) const { return item.fileRunRef->runRef->getModuleParam(paramName); }
};

const char *MatchableResultItem::getAsString() const
{
    return getName();
}

const char *MatchableResultItem::getAsString(const char *attribute) const
{
    if (strcasecmp("name", attribute) == 0)
        return getName();
    else if (strcasecmp("module", attribute) == 0)
        return getModuleName();
    else if (strcasecmp("file", attribute) == 0)
        return getFileName();
    else if (strcasecmp("run", attribute) == 0)
        return getRunName();
    else if (strncasecmp("attr:", attribute, 5) == 0)
        return getRunAttribute(attribute+5);
    else if (strncasecmp("param:", attribute, 6) == 0)
        return getModuleParam(attribute+6);
    else
        return getResultItemAttribute(attribute);
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

void ResultFileManager::checkPattern(const char *pattern)
{
    if (pattern == nullptr || pattern[0] == '\0')  // no filter
        return;

    // parse it
    // XXX after successful parsing, we could also check that attribute names in it are valid
    MatchExpression matchExpr(pattern, false  /*dottedpath*/, true  /*fullstring*/, true  /*casesensitive*/);
}

ResultFile *ResultFileManager::addFile(const char *fileName, const char *fileSystemFileName, ResultFile::FileType fileType, bool computed)
{
    ResultFile *file = new ResultFile();
    file->id = fileList.size();
    fileList.push_back(file);
    file->resultFileManager = this;
    file->fileSystemFilePath = fileSystemFileName;
    file->filePath = fileNameToSlash(fileName);
    splitFileName(file->filePath.c_str(), file->directory, file->fileName);
    file->fileType = fileType;
    file->computed = computed;
    file->numLines = 0;
    file->numUnrecognizedLines = 0;
    return file;
}

Run *ResultFileManager::addRun(bool computed)
{
    Run *run = new Run(computed, this);
    run->runNumber = 0;
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

int ResultFileManager::addScalar(FileRun *fileRunRef, const char *moduleName,
        const char *scalarName, double value, bool isField)
{
    ScalarResult scalar;
    scalar.fileRunRef = fileRunRef;

    scalar.moduleNameRef = moduleNames.insert(moduleName);
    scalar.nameRef = names.insert(scalarName);
    scalar.value = value;
    scalar.isField = isField;

    ScalarResults& scalars = fileRunRef->fileRef->scalarResults;
    scalars.push_back(scalar);
    return scalars.size() - 1;
}

int ResultFileManager::addVector(FileRun *fileRunRef, int vectorId, const char *moduleName, const char *vectorName, const char *columns)
{
    VectorResult vector;
    vector.fileRunRef = fileRunRef;
    vector.vectorId = vectorId;
    vector.moduleNameRef = moduleNames.insert(moduleName);
    vector.nameRef = names.insert(vectorName);
    vector.columns = columns;
    vector.stat = Statistics(-1, NaN, NaN, NaN, NaN);
    VectorResults& vectors = fileRunRef->fileRef->vectorResults;
    vectors.push_back(vector);
    return vectors.size() - 1;
}

int ResultFileManager::addHistogram(FileRun *fileRunRef, const char *moduleName, const char *histogramName,
        Statistics stat, const StringMap& attrs)
{
    HistogramResult histogram;
    histogram.attributes = attrs;
    histogram.fileRunRef = fileRunRef;
    histogram.moduleNameRef = moduleNames.insert(moduleName);
    histogram.nameRef = names.insert(histogramName);
    histogram.stat = stat;
    HistogramResults& histograms = fileRunRef->fileRef->histogramResults;
    histograms.push_back(histogram);
    return histograms.size() - 1;
}

// create a file for each dataset?
ID ResultFileManager::addComputedVector(int vectorId, const char *name, const char *file,
        const StringMap& attributes, ComputationID computationID, ID input, IComputation *computation)
{
    WRITER_MUTEX

    assert(getTypeOf(input) == VECTOR);

    const VectorResult& vector = getVector(input);

    ResultFile *fileRef = getFile(file);
    if (!fileRef)
        fileRef = addFile(file, file, ResultFile::FILETYPE_TEXT, true);  // XXX
    Run *runRef = vector.fileRunRef->runRef;
    FileRun *fileRunRef = getFileRun(fileRef, runRef);
    if (!fileRunRef)
        fileRunRef = addFileRun(fileRef, runRef);

    VectorResult newVector = VectorResult();
    newVector.vectorId = vectorId;
    newVector.computation = computation;
    newVector.columns = vector.columns;
    newVector.moduleNameRef = vector.moduleNameRef;
    newVector.nameRef = names.insert(name);
    newVector.fileRunRef = fileRunRef;
    newVector.attributes = attributes;
    newVector.stat = Statistics(-1, NaN, NaN, NaN, NaN);
    fileRef->vectorResults.push_back(newVector);
    ID id = _mkID(true, false, VECTOR, fileRef->id, fileRef->vectorResults.size()-1);
    std::pair<ComputationID, ID> key = std::make_pair(computationID, input);
    computedIDCache[key] = id;
    return id;
}

ID ResultFileManager::getComputedID(ComputationID computationID, ID input) const
{
    READER_MUTEX
    std::pair<ComputationID, ID> key = std::make_pair(computationID, input);
    ComputedIDCache::const_iterator it = computedIDCache.find(key);
    if (it != computedIDCache.end())
        return it->second;
    else
        return -1;
}

//---------------------------------------------------------------------------------------------------------------------
// Computed Scalars
//---------------------------------------------------------------------------------------------------------------------

ID ResultFileManager::addComputedScalar(const char *name, const char *module, const char *runName, double value, const StringMap& attributes, IComputation *node)
{
    WRITER_MUTEX

    if (!computedScalarFile)
        computedScalarFile = addFile("", "", ResultFile::FILETYPE_TEXT, true);
    Run *run = getRunByName(runName);
    if (!run) {
        run = addRun(true);
        run->runName = runName;
        run->attributes = attributes;
    }
    FileRun *fileRun = getFileRun(computedScalarFile, run);
    if (!fileRun)
        fileRun = addFileRun(computedScalarFile, run);

    ScalarResult scalar;
    scalar.nameRef = computedScalarNames.insert(name);
    scalar.moduleNameRef = computedModuleNames.insert(module);
    scalar.fileRunRef = fileRun;
    scalar.computation = node;
    scalar.isField = false;
    scalar.value = value;

    int id = computedScalarFile->scalarResults.size();
    computedScalarFile->scalarResults.push_back(scalar);
    return _mkID(true, false, SCALAR, computedScalarFile->id, id);
}

IDList ResultFileManager::getComputedScalarIDs(const IComputation *node) const
{
    READER_MUTEX

    IDList result;
    if (computedScalarFile && node) {
        const ScalarResults& scalars = computedScalarFile->scalarResults;
        for (int i = 0; i < (int)scalars.size(); ++i) {
            const ScalarResult& scalar = scalars[i];
            if ((*scalar.computation) == (*node))
                result.add(_mkID(true, false, SCALAR, computedScalarFile->id, i));
        }
    }
    return result;
}

void ResultFileManager::clearComputedScalars()
{
    WRITER_MUTEX

    if (computedScalarFile)
        unloadFile(computedScalarFile);
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

    if (SqliteResultFileUtils::isSqliteFile(fileSystemFileName)) {
        return SqliteResultFileLoader(this).loadFile(fileName, fileSystemFileName, reload);
    }
    else {
        return OmnetppResultFileLoader(this).loadFile(fileName, fileSystemFileName, reload);
    }
}

void ResultFileManager::unloadFile(ResultFile *file)
{
    WRITER_MUTEX

    // remove computed vector IDs
    for (ComputedIDCache::iterator it = computedIDCache.begin(); it != computedIDCache.end(); ) {
        ID id = it->first.second;
        if (_fileid(id) == file->id) {
            ComputedIDCache::iterator oldIt = it;
            ++it;
            computedIDCache.erase(oldIt);
        }
        else
            ++it;
    }

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

    // Computed names only refered from computedScalarFile, so clear them now.
    if (file == computedScalarFile) {
        computedScalarFile = nullptr;
        computedScalarNames.clear();
        computedModuleNames.clear();
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
    bool operator()(const std::string& first, const std::string& second)
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

StringVector *ResultFileManager::getFileAndRunNumberFilterHints(const IDList& idlist) const
{
    READER_MUTEX
    FileRunList *fileRuns = getUniqueFileRuns(idlist);

    StringVector vec;
    DuplicateStringCollector coll;

    for (int i = 0; i < (int)fileRuns->size(); i++) {
        FileRun *fileRun = (*fileRuns)[i];
        if (fileRun->runRef->runNumber == 0) {
            vec.push_back(fileRun->fileRef->filePath);
        }
        else {
            char runNumberStr[32];
            sprintf(runNumberStr, "%d", fileRun->runRef->runNumber);
            vec.push_back(fileRun->fileRef->filePath+"#"+runNumberStr);
            coll.add(fileRun->fileRef->filePath+"#*");
        }
    }
    delete fileRuns;

    // sort and concatenate them, and return the result
    StringVector *wildvec = new StringVector(coll.get());
    std::sort(vec.begin(), vec.end(), strdictLess);
    std::sort(wildvec->begin(), wildvec->end(), strdictLess);
    wildvec->insert(wildvec->end(), vec.begin(), vec.end());
    return wildvec;
}

StringVector *ResultFileManager::getFilePathFilterHints(const ResultFileList& fileList) const
{
    READER_MUTEX
    StringVector *filterHints = new StringVector;
    for (ResultFileList::const_iterator it = fileList.begin(); it != fileList.end(); ++it)
        filterHints->push_back((*it)->filePath);
    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    return filterHints;
}

StringVector *ResultFileManager::getRunNameFilterHints(const RunList& runList) const
{
    READER_MUTEX
    StringVector *filterHints = new StringVector;
    for (RunList::const_iterator it = runList.begin(); it != runList.end(); ++it)
        if ((*it)->runName.size() > 0)
            filterHints->push_back((*it)->runName);

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

StringVector *ResultFileManager::getModuleParamFilterHints(const RunList& runList, const char *paramName) const
{
    READER_MUTEX
    StringSet *paramValues = getUniqueModuleParamValues(runList, paramName);
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
    return getItem(id).fileRunRef->runRef->getAttribute(attribute);
}

}  // namespace scave
}  // namespace omnetpp

