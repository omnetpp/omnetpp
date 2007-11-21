//=========================================================================
//  RESULTFILEMANAGER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <utility>
#include "opp_ctype.h"
#include "platmisc.h"
#include "matchexpression.h"
#include "patternmatcher.h"
#include "filereader.h"
#include "linetokenizer.h"
#include "stringtokenizer.h"
#include "filereader.h"
#include "indexfile.h"
#include "scaveutils.h"
#include "resultfilemanager.h"
#include "commonutil.h"


static double zero = 0.0;
static double NaN = zero / zero;

static class SetPosixLocale
{
  public:
    SetPosixLocale() {
        setPosixLocale();
    }
} instance;


ResultItem::Type ResultItem::getType() const
{
    StringMap::const_iterator it = attributes.find("type");
    if (it == attributes.end())
    {
        if (attributes.find("enum") != attributes.end())
            return TYPE_ENUM;
        else
            return TYPE_DOUBLE;
    }
    else
    {
        const std::string &type = it->second;
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

EnumType* ResultItem::getEnum() const
{
    StringMap::const_iterator it = attributes.find("enum");
    if (it != attributes.end())
    {
        EnumType *enumPtr = new EnumType();
        enumPtr->parseFromString(it->second.c_str());
        return enumPtr;
    }
    else
    {
        return NULL;
    }
}

InterpolationMode VectorResult::getInterpolationMode() const
{
    StringMap::const_iterator it = attributes.find("interpolationmode");
    if (it != attributes.end())
    {
        const std::string &mode = it->second;
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
    else
    {
        return UNSPECIFIED;
    }
}

ResultFileManager::ResultFileManager()
{
}

ResultFileManager::~ResultFileManager()
{
    for (int i=0; i<(int)fileRunList.size(); i++)
        delete fileRunList[i];

    for (int i=0; i<(int)runList.size(); i++)
        delete runList[i];

    for (int i=0; i<(int)fileList.size(); i++)
        delete fileList[i];

    fileRunList.clear();
    runList.clear();
    fileList.clear();

    moduleNames.clear();
    names.clear();
    classNames.clear();
}

std::string *ResultFileManager::stringSetFindOrInsert(StringSet& set, const std::string& str)
{
    StringSet::iterator m = set.find(str);
    if (m==set.end())
    {
        std::pair<StringSet::iterator,bool> p = set.insert(str);
        m = p.first;
    }
    return &const_cast<std::string&>(*m);
}

ResultFileList ResultFileManager::getFiles() const
{
    ResultFileList out;
    for (int i=0; i<(int)fileList.size(); i++)
        if (fileList[i])
            out.push_back(fileList[i]);
    return out;
}

RunList ResultFileManager::getRunsInFile(ResultFile *file) const
{
    RunList out;
    for (int i=0; i<(int)fileRunList.size(); i++)
        if (fileRunList[i]->fileRef == file)
            out.push_back(fileRunList[i]->runRef);
    return out;
}

ResultFileList ResultFileManager::getFilesForRun(Run *run) const
{
    ResultFileList out;
    for (int i=0; i<(int)fileRunList.size(); i++)
        if (fileRunList[i]->runRef == run)
            out.push_back(fileRunList[i]->fileRef);
    return out;
}

const ResultItem& ResultFileManager::getItem(ID id) const
{
    try
    {
        switch (_type(id))
        {
            case SCALAR: return getFileForID(id)->scalarResults.at(_pos(id));
            case VECTOR: return getFileForID(id)->vectorResults.at(_pos(id));
            case HISTOGRAM: return getFileForID(id)->histogramResults.at(_pos(id));
            default: throw opp_runtime_error("ResultFileManager: invalid ID: wrong type");
        }
    }
    catch (std::out_of_range e)
    {
        throw opp_runtime_error("ResultFileManager::getItem(id): invalid ID");
    }
}

ResultFileList *ResultFileManager::getUniqueFiles(const IDList& ids) const
{
    // collect unique runs in this dataset
    std::set<ResultFile*> set;
    for (int i=0; i<(int)ids.size(); i++)
        set.insert(getItem(ids.get(i)).fileRunRef->fileRef);

    // convert to list for easier handling at recipient
    ResultFileList *list = new ResultFileList();
    for (std::set<ResultFile*>::iterator i = set.begin(); i!=set.end(); i++)
        list->push_back(*i);
    return list;
}

RunList *ResultFileManager::getUniqueRuns(const IDList& ids) const
{
    // collect unique runs in this dataset
    std::set<Run*> set;
    for (int i=0; i<(int)ids.size(); i++)
        set.insert(getItem(ids.get(i)).fileRunRef->runRef);

    // convert to list for easier handling at recipient
    RunList *list = new RunList();
    for (std::set<Run*>::iterator i = set.begin(); i!=set.end(); i++)
        list->push_back(*i);
    return list;
}

FileRunList *ResultFileManager::getUniqueFileRuns(const IDList& ids) const
{
    // collect unique FileRuns in this dataset
    std::set<FileRun*> set;
    for (int i=0; i<(int)ids.size(); i++)
        set.insert(getItem(ids.get(i)).fileRunRef);

    // convert to list for easier handling at recipient
    FileRunList *list = new FileRunList();
    for (std::set<FileRun*>::iterator i = set.begin(); i!=set.end(); i++)
        list->push_back(*i);
    return list;
}

StringSet *ResultFileManager::getUniqueModuleNames(const IDList& ids) const
{
    // collect unique module names in this dataset
    StringSet *set = new StringSet();
    for (int i=0; i<(int)ids.size(); i++)
        set->insert(*getItem(ids.get(i)).moduleNameRef);
    return set;
}

StringSet *ResultFileManager::getUniqueNames(const IDList& ids) const
{
    // collect unique scalar/vector names in this dataset
    StringSet *set = new StringSet();
    for (int i=0; i<(int)ids.size(); i++)
        set->insert(*getItem(ids.get(i)).nameRef);
    return set;
}

const ScalarResult& ResultFileManager::getScalar(ID id) const
{
    if (_type(id)!=SCALAR)
        throw opp_runtime_error("ResultFileManager::getScalar(id): this item is not a scalar");
    return getFileForID(id)->scalarResults.at(_pos(id));
}

const VectorResult& ResultFileManager::getVector(ID id) const
{
    if (_type(id)!=VECTOR)
        throw opp_runtime_error("ResultFileManager::getVector(id): this item is not a vector");
    return getFileForID(id)->vectorResults.at(_pos(id));
}

const HistogramResult& ResultFileManager::getHistogram(ID id) const
{
    if (_type(id)!=HISTOGRAM)
        throw opp_runtime_error("ResultFileManager::getHistogram(id): this item is not a histogram");
    return getFileForID(id)->histogramResults.at(_pos(id));
}

IDList ResultFileManager::getAllScalars() const
{
    IDList out;
    for (int k=0; k<(int)fileList.size(); k++)
    {
        if (fileList[k]!=NULL)
        {
            ScalarResults& v = fileList[k]->scalarResults;
            for (int i=0; i<(int)v.size(); i++)
                if (!v[i].isComputed())
                    out.uncheckedAdd(_mkID(false,SCALAR,k,i));
        }
    }
    return out;
}

IDList ResultFileManager::getAllVectors() const
{
    IDList out;
    for (int k=0; k<(int)fileList.size(); k++)
    {
        if (fileList[k]!=NULL)
        {
            VectorResults& v = fileList[k]->vectorResults;
            for (int i=0; i<(int)v.size(); i++)
                if (!v[i].isComputed())
                    out.uncheckedAdd(_mkID(false,VECTOR,k,i));
        }
    }
    return out;
}

IDList ResultFileManager::getAllHistograms() const
{
    IDList out;
    for (int k=0; k<(int)fileList.size(); k++)
    {
        if (fileList[k]!=NULL)
        {
            HistogramResults& v = fileList[k]->histogramResults;
            for (int i=0; i<(int)v.size(); i++)
                if (!v[i].isComputed())
                    out.uncheckedAdd(_mkID(false,HISTOGRAM,k,i));
        }
    }
    return out;
}

IDList ResultFileManager::getScalarsInFileRun(FileRun *fileRun) const
{
    IDList out;
    int fileId = fileRun->fileRef->id;
    ScalarResults& v = fileRun->fileRef->scalarResults;
    for (int i=0; i<(int)v.size(); i++)
        if (v[i].fileRunRef==fileRun && !v[i].isComputed())
            out.uncheckedAdd(_mkID(false,SCALAR,fileId,i));
    return out;
}

IDList ResultFileManager::getVectorsInFileRun(FileRun *fileRun) const
{
    IDList out;
    int fileId = fileRun->fileRef->id;
    VectorResults& v = fileRun->fileRef->vectorResults;
    for (int i=0; i<(int)v.size(); i++)
        if (v[i].fileRunRef==fileRun && !v[i].isComputed())
            out.uncheckedAdd(_mkID(false,VECTOR,fileId,i));
    return out;
}

IDList ResultFileManager::getHistogramsInFileRun(FileRun *fileRun) const
{
    IDList out;
    int fileId = fileRun->fileRef->id;
    HistogramResults& v = fileRun->fileRef->histogramResults;
    for (int i=0; i<(int)v.size(); i++)
        if (v[i].fileRunRef==fileRun && !v[i].isComputed())
            out.uncheckedAdd(_mkID(false,HISTOGRAM,fileId,i));
    return out;
}

bool ResultFileManager::isFileLoaded(const char *fileName) const
{
    return getFile(fileName)!=NULL;
}

ResultFile *ResultFileManager::getFile(const char *fileName) const
{
    if (!fileName)
        return NULL;
    for (int i=0; i<(int)fileList.size(); i++)
        if (fileList[i]!=NULL && fileList[i]->filePath==fileName)
            return fileList[i];
    return NULL;
}

Run *ResultFileManager::getRunByName(const char *runName) const
{
    if (!runName)
        return NULL;
    for (int i=0; i<(int)runList.size(); i++)
        if (runList[i]->runName==runName)
            return runList[i];
    return NULL;
}

FileRun *ResultFileManager::getFileRun(ResultFile *file, Run *run) const
{
    for (int i=0; i<(int)fileRunList.size(); i++)
        if (fileRunList[i]->fileRef==file && fileRunList[i]->runRef==run)
            return fileRunList[i];
    return NULL;
}

// currently unused
ID ResultFileManager::getItemByName(FileRun *fileRunRef, const char *module, const char *name) const
{
    if (!fileRunRef || !module || !name)
        return 0;

    StringSet::const_iterator m = moduleNames.find(module);
    if (m==moduleNames.end())
        return 0;
    const std::string *moduleNameRef = &(*m);

    StringSet::const_iterator n = names.find(name);
    if (n==names.end())
        return 0;
    const std::string *nameRef = &(*n);

    ScalarResults& scalarResults = fileRunRef->fileRef->scalarResults;
    for (int i=0; i<(int)scalarResults.size(); i++)
    {
        const ResultItem& d = scalarResults[i];
        if (d.moduleNameRef==moduleNameRef && d.nameRef==nameRef && d.fileRunRef==fileRunRef)
            return _mkID(d.isComputed(), SCALAR, fileRunRef->fileRef->id, i);
    }

    VectorResults& vectorResults = fileRunRef->fileRef->vectorResults;
    for (int i=0; i<(int)vectorResults.size(); i++)
    {
        const ResultItem& d = vectorResults[i];
        if (d.moduleNameRef==moduleNameRef && d.nameRef==nameRef && d.fileRunRef==fileRunRef)
            return _mkID(d.isComputed(), VECTOR, fileRunRef->fileRef->id, i);
    }

    HistogramResults& histogramResults = fileRunRef->fileRef->histogramResults;
    for (int i=0; i<(int)histogramResults.size(); i++)
    {
        const ResultItem& d = histogramResults[i];
        if (d.moduleNameRef==moduleNameRef && d.nameRef==nameRef && d.fileRunRef==fileRunRef)
            return _mkID(d.isComputed(), HISTOGRAM, fileRunRef->fileRef->id, i);
    }
    return 0;
}

StringVector ResultFileManager::getUniqueAttributeValues(const RunList& runList, const char *attrName) const
{
    StringSet values;
    for (int i=0; i<(int)runList.size(); i++)
    {
        const char *value = runList[i]->getAttribute(attrName);
        if (value!=NULL)
            values.insert(value);
    }

    // copy into a vector
    StringVector vec;
    for (StringSet::iterator it=values.begin(); it!=values.end(); it++)
        vec.push_back(*it);

    // sort it and return the result
    std::sort(vec.begin(), vec.end());
    return vec;
}

RunList ResultFileManager::filterRunList(const RunList& runList,
                                         const char *runNameFilter,
                                         const StringMap& attrFilter) const
{
    RunList out;

    // runName matcher
    PatternMatcher runNamePattern(runNameFilter, false, true, true);

    // copy attributes to vectors for performance (std::map iterator is
    // infamously slow, and we need PatternMatchers as well)
    StringVector attrNames;
    std::vector<PatternMatcher> attrValues;
    for (StringMap::const_iterator it = attrFilter.begin(); it!=attrFilter.end(); ++it)
    {
        attrNames.push_back(it->first);
        attrValues.push_back(PatternMatcher(it->second.c_str(), false, true, true));
    }

    // do it
    for (int i=0; i<(int)runList.size(); i++)
    {
        Run *run = runList[i];
        if (!runNamePattern.matches(run->runName.c_str()))
            continue;
        bool matches = true;
        for (int j=0; j<(int)attrNames.size() && matches; j++)
        {
            const char *attrValue = run->getAttribute(attrNames[j].c_str());
            if (attrValue == NULL)
                attrValue = "";

            if (!attrValues[j].matches(attrValue))
            {
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

    for (int i=0; i<(int)fileList.size(); i++)
    {
        ResultFile *file = fileList[i];
        if (!filePathPattern.matches(file->filePath.c_str()))
            continue;
        out.push_back(file);
    }
    return out;
}

FileRunList ResultFileManager::getFileRuns(const ResultFileList *fileList, const RunList *runList) const
{
    FileRunList out;
    for (int i=0; i<(int)fileRunList.size(); i++)
    {
        FileRun *fileRun = fileRunList[i];
        if (fileList && std::find(fileList->begin(), fileList->end(), fileRun->fileRef)==fileList->end())
            continue;
        if (runList && std::find(runList->begin(), runList->end(), fileRun->runRef)==runList->end())
            continue;
        out.push_back(fileRun);
    }
    return out;
}

IDList ResultFileManager::filterIDList(const IDList& idlist,
                                       const FileRunList *fileRunFilter,
                                       const char *moduleFilter,
                                       const char *nameFilter)
{
    // "*" means no filtering, so ignore it
    if (!strcmp(moduleFilter,"*")) moduleFilter = "";
    if (!strcmp(nameFilter,"*")) nameFilter = "";

    // prepare for wildcard matches
    bool patMatchModule = PatternMatcher::containsWildcards(moduleFilter);
    bool patMatchName = PatternMatcher::containsWildcards(nameFilter);

    PatternMatcher *modulePattern = NULL;
    if (patMatchModule)
    {
        modulePattern = new PatternMatcher(moduleFilter, false, true, true); // case-sensitive full-string match
    }
    PatternMatcher *namePattern = NULL;
    if (patMatchName)
    {
        namePattern = new PatternMatcher(nameFilter, false, true, true); // case-sensitive full-string match
    }

    // TODO: if there's no wildcard, find string pointers in the stringsets
    // in advance, then we don't have to do strcmp().

    // iterate over all values and add matching ones to "out".
    // we can exploit the fact that ResultFileManager contains the data in the order
    // they were read from file, i.e. grouped by runs
    IDList out;
    FileRun *lastFileRunRef = NULL;
    bool lastFileRunMatched = false;
    int sz = idlist.size();
    for (int i=0; i<sz; i++)
    {
        ID id = idlist.get(i);
        const ResultItem& d = getItem(id);

        if (fileRunFilter)
        {
            if (lastFileRunRef!=d.fileRunRef)
            {
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
            continue; // no match

        if (nameFilter && nameFilter[0] &&
            (patMatchName ? !namePattern->matches(d.nameRef->c_str())
                          : strcmp(d.nameRef->c_str(), nameFilter))
           )
            continue; // no match

        // everything matched, insert it.
        // (note: uncheckedAdd() is fine: if input IDList didn't contain duplicates,
        // the result won't either)
        out.uncheckedAdd(id);
    }
    return out;
}

class MatchableResultItem : public MatchExpression::Matchable
{
    const ResultItem& item;

    public:
        MatchableResultItem(const ResultItem& item) : item(item) {}
        virtual const char *getDefaultAttribute() const;
        virtual const char *getAttribute(const char *name) const;
    private:
        const char *getName() const { return item.nameRef->c_str(); }
        const char *getModuleName() const { return item.moduleNameRef->c_str(); }
        const char *getFileName() const { return item.fileRunRef->fileRef->filePath.c_str(); }
        const char *getRunName() const { return item.fileRunRef->runRef->runName.c_str(); }
        const char *getRunAttribute(const char *attrName) const { return item.fileRunRef->runRef->getAttribute(attrName); }
};

const char *MatchableResultItem::getDefaultAttribute() const
{
    return getName();
}

const char *MatchableResultItem::getAttribute(const char *name) const
{
    if (strcasecmp("name", name) == 0)
        return getName();
    else if (strcasecmp("module", name) == 0)
        return getModuleName();
    else if (strcasecmp("file", name) == 0)
        return getFileName();
    else if (strcasecmp("run", name) == 0)
        return getRunName();
    else
        return getRunAttribute(name);
}

IDList ResultFileManager::filterIDList(const IDList &idlist, const char *pattern) const
{
    if (pattern == NULL || pattern[0] == '\0') // no filter
        pattern = "*";

    MatchExpression matchExpr(pattern, false /*dottedpath*/, true /*fullstring*/, true /*casesensitive*/);
    IDList out;
    int sz = idlist.size();
    for (int i=0; i<sz; ++i)
    {
        ID id = idlist.get(i);
        const ResultItem &item = getItem(id);
        MatchableResultItem matchable(item);
        if (matchExpr.matches(&matchable))
            out.uncheckedAdd(id);
    }
    return out;
}

void ResultFileManager::checkPattern(const char *pattern)
{
    if (pattern==NULL || pattern[0] == '\0') // no filter
        return;

    // parse it
    //XXX after successful parsing, we could also check that attribute names in it are valid
    MatchExpression matchExpr(pattern, false /*dottedpath*/, true /*fullstring*/, true /*casesensitive*/);
}

static void splitFileName(const char *pathname, std::string& dir, std::string& fnameonly)
{
    if (!pathname || !*pathname)
    {
         dir = "";
         fnameonly = "";
         return;
    }

    dir = pathname;

    // find last "/" or "\"
    const char *s = pathname + strlen(pathname) - 1;
    while (s>=pathname && *s!='\\' && *s!='/') s--;

    // split along that
    if (s<pathname)
    {
        fnameonly = pathname;
        dir = ".";
    }
    else
    {
        fnameonly = s+1;
        dir = "";
        dir.append(pathname, s-pathname+1);
    }
}

static std::string fileNameToSlash(const char *fileName)
{
    std::string res;
    res.reserve(strlen(fileName));
    for (; *fileName; fileName++)
        res.append(1, *fileName=='\\' ? '/' : *fileName);
    return res;
}

ResultFile *ResultFileManager::addFile(const char *fileName, const char *fileSystemFileName)
{
    ResultFile *file = new ResultFile();
    file->id = fileList.size();
    fileList.push_back(file);
    file->resultFileManager = this;
    file->fileSystemFilePath = fileSystemFileName;
    file->filePath = fileNameToSlash(fileName);
    splitFileName(file->filePath.c_str(), file->directory, file->fileName);
    file->numLines = 0;
    file->numUnrecognizedLines = 0;
    return file;
}

Run *ResultFileManager::addRun()
{
    Run *run = new Run();
    run->resultFileManager = this;
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

void ResultFileManager::addScalar(FileRun *fileRunRef, const char *moduleName,
                                  const char *scalarName, double value)
{
    static std::string *lastInsertedModuleRef = NULL;

    ScalarResult d;
    d.fileRunRef = fileRunRef;

    // lines in omnetpp.sca are usually grouped by module, we can exploit this for efficiency
// broken concept: load scalars, load vectors with new module names, load scalars -> dangling pointer
// TODO: create string pool for module names, and add this improvement to it
//    if (lastInsertedModuleRef && *lastInsertedModuleRef==moduleName)
//    {
//        d.moduleNameRef = lastInsertedModuleRef;
//    }
//    else
//    {
        std::string *m = stringSetFindOrInsert(moduleNames, std::string(moduleName));
        d.moduleNameRef = lastInsertedModuleRef = m;
//    }

    d.nameRef = stringSetFindOrInsert(names, std::string(scalarName));

    d.value = value;

    fileRunRef->fileRef->scalarResults.push_back(d);
}


// create a file for each dataset?
ID ResultFileManager::addComputedVector(const char *name, const char *file, ComputationID computationID, ID input, ComputationNode computation)
{
    assert(getTypeOf(input) == VECTOR);

    const VectorResult& vector = getVector(input);

    ResultFile *fileRef = getFile(file);
    if (!fileRef)
    	fileRef = addFile(file, file); // XXX
    Run *runRef = vector.fileRunRef->runRef;
    FileRun *fileRunRef = getFileRun(fileRef, runRef);
    if (!fileRunRef)
    	fileRunRef = addFileRun(fileRef, runRef);
    
    VectorResult newVector = VectorResult(vector);
    newVector.computation = computation;
    newVector.nameRef = stringSetFindOrInsert(names, name);
    newVector.fileRunRef = fileRunRef;
    newVector.stat = Statistics(-1, dblNaN, dblNaN, dblNaN, dblNaN);
    fileRef->vectorResults.push_back(newVector);
    ID id = _mkID(true, VECTOR, fileRef->id, fileRef->vectorResults.size()-1);
    std::pair<ComputationID, ID> key = std::make_pair(computationID, input);
    computedIDCache[key] = id;
    return id;
}

ID ResultFileManager::getComputedVector(ComputationID computationID, ID input)
{
    std::pair<ComputationID, ID> key = std::make_pair(computationID, input);
    ComputedIDCache::iterator it = computedIDCache.find(key);
    if (it != computedIDCache.end())
      return it->second;
    else
        return -1;
}

/*
void ResultFileManager::dump(ResultFile *fileRef, std::ostream& out) const
{
    Run *prevRunRef = NULL;
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

/* XXX unused function
static void parseString(char *&s, std::string& dest, int lineNum)
{
    while (*s==' ' || *s=='\t') s++;
    if (*s=='"')
    {
        // parse quoted string    //FIXME use opp_parsequotedstr() instead!
        char *start = s+1;
        s++;
        while (*s && (*s!='"' || *(s-1)=='\\') && *s!='\r' && *s!='\n') s++;
        if (*s!='"')
            throw opp_runtime_error("invalid syntax: missing close quote, line %d", lineNum);
        dest.assign(start, s-start);
        s++;
    }
    else
    {
        // parse unquoted string
        char *start = s;
        while (*s && *s!=' ' && *s!='\t' && *s!='\r' && *s!='\n') s++;
        dest.assign(start, s-start); // can be empty as well
    }
}
*/

void ResultFileManager::processLine(char **vec, int numTokens, FileRun *&fileRunRef, ResultItem *&resultItemRef,
                                    ResultFile *fileRef, int lineNum)
{
    // ignore empty lines
    if (numTokens==0 || vec[0][0]=='#')
        return;

    // process "run" lines
    if (vec[0][0]=='r' && !strcmp(vec[0],"run"))
    {
        if (numTokens<2)
            throw opp_runtime_error("invalid result file: run Id missing from `run' line, line %d", lineNum);

        if (atoi(vec[1])>0 && strlen(vec[1])<=10)
        {
            // old-style "run" line, format: run <runNumber> [<networkName>] [<dateTime>]
            // and runs in different files cannot be related, so we must create a new Run entry.
            Run *runRef = addRun();
            fileRunRef = addFileRun(fileRef, runRef);

            runRef->runNumber = atoi(vec[1]);
            runRef->attributes["run-number"] = vec[1];
            if (numTokens>=3)
                runRef->attributes["network"] = vec[2];
            if (numTokens>=4)
                runRef->attributes["dateTime"] = vec[3];

            // assemble a probably-unique runName
            std::stringstream os;
            os << fileRef->fileName << ":" << lineNum << "-#" << vec[1];
            if (numTokens>=3)
                os << "-" << vec[2];
            if (numTokens>=4)
                os << "-" << vec[3];
            runRef->runName = os.str();
        }
        else
        {
            // new-style "run" line, format: run <runName>
            // find out of we have that run already
            Run *runRef = getRunByName(vec[1]);
            if (!runRef)
            {
                // not yet: add it
                runRef = addRun();
                runRef->runName = vec[1];
            }
            // associate Run with this file
            if (getFileRun(fileRef, runRef)!=NULL)
                throw opp_runtime_error("invalid result file: run Id repeats in the file, line %d", lineNum);
            fileRunRef = addFileRun(fileRef, runRef);
        }
        return;
    }

    // if we haven't seen a "run" line yet (as with old vector files), add a default run
    if (fileRunRef==NULL)
    {
        // fake a new Run
        Run *runRef = addRun();
        fileRunRef = addFileRun(fileRef, runRef);
        runRef->runNumber = 0;

        /*
        // make up a unique runName
        std::stringstream os;
        static int counter=1000;
        os << fileRef->fileName << ":" << lineNum << "-#n/a-" <<++counter;
        runRef->runName = os.str();
        */
    }

    if (vec[0][0]=='a' && !strcmp(vec[0],"attr"))
    {
        if (numTokens<3)
            throw opp_runtime_error("invalid result file: 'attr <name> <value>' expected, line %d", lineNum);

        if (resultItemRef == NULL)
        {
            // store attribute
            fileRunRef->runRef->attributes[vec[1]] = vec[2];

            // the "runNumber" attribute is also stored separately
            if (!strcmp(vec[1], "runNumber"))
                fileRunRef->runRef->runNumber = atoi(vec[2]);
        }
        else
        {
            resultItemRef->attributes[vec[1]] = vec[2];
        }
    }
    else if (vec[0][0]=='p' && !strcmp(vec[0],"param"))
    {
        if (numTokens<3)
            throw opp_runtime_error("invalid result file: 'param <namePattern> <value>' expected, line %d", lineNum);

        // store attribute
        fileRunRef->runRef->moduleParams[vec[1]] = vec[2];
    }
    else if (vec[0][0]=='s' && !strcmp(vec[0],"scalar"))
    {
        // syntax: "scalar <module> <scalarname> <value>"
        if (numTokens<4)
            throw opp_runtime_error("invalid scalar file: too few items on `scalar' line, line %d", lineNum);

        double value;
        if (!parseDouble(vec[3],value))
            throw opp_runtime_error("invalid scalar file syntax: invalid value column, line %d", lineNum);

        addScalar(fileRunRef, vec[1], vec[2], value);
        resultItemRef = &fileRef->scalarResults.back();
        return;
    }
    else if (vec[0][0]=='v' && !strcmp(vec[0],"vector"))
    {
        // vector line
        if (numTokens<4)
            throw opp_runtime_error("invalid vector file syntax: too few items on 'vector' line, line %d", lineNum);

        VectorResult vecdata;
        vecdata.fileRunRef = fileRunRef;

        // vectorId
        char *e;
        vecdata.vectorId = (int) strtol(vec[1],&e,10);
        if (*e)
            throw opp_runtime_error("invalid vector file syntax: invalid vector id in vector definition, line %d", lineNum);
        // module name, vector name
        const char *moduleName = vec[2];
        const char *vectorName = vec[3];
        const char *columns = (numTokens < 5 || opp_isdigit(vec[4][0]) ? "TV" : vec[4]);
        //printf("columns: %s\n", vecdata.columns.c_str());
        vecdata.moduleNameRef = stringSetFindOrInsert(moduleNames, std::string(moduleName));
        vecdata.nameRef = stringSetFindOrInsert(names, std::string(vectorName));
        vecdata.columns = columns;
        vecdata.stat = Statistics(-1, NaN, NaN, NaN, NaN);
        fileRef->vectorResults.push_back(vecdata);
        resultItemRef = &fileRef->vectorResults.back();
    }
    else if (opp_isdigit(vec[0][0]) && numTokens>=3)
    {
        // this looks like a vector data line, skip it this time
    }
    else
    {
        // ignore unknown lines and vector data lines
        fileRef->numUnrecognizedLines++;
    }
}

static bool isFileReadable(const char *fileName)
{
    FILE *f = fopen(fileName, "r");
    if (f!=NULL)
    {
        fclose(f);
        return true;
    }
    else
        return false;
}

ResultFile *ResultFileManager::loadFile(const char *fileName, const char *fileSystemFileName)
{
    // tricky part: we store "fileName" which is the Eclipse pathname of the file
    // (or some other "display name" of the file), but we actually load from
    // fileSystemFileName which is the fileName suitable for fopen()

    // check
    if (isFileLoaded(fileName))
        return getFile(fileName);

    // try if file can be opened, before we add it to our database
    if (fileSystemFileName==NULL)
        fileSystemFileName = fileName;
    if (!isFileReadable(fileSystemFileName))
        throw opp_runtime_error("cannot open `%s' for read", fileSystemFileName);

    // add to fileList
    ResultFile *fileRef = addFile(fileName, fileSystemFileName);

    try
    {
        // if vector file and has index, load vectors from the index file
        if (IndexFile::isVectorFile(fileSystemFileName) && IndexFile::isIndexFileUpToDate(fileSystemFileName))
        {
            std::string indexFileName = IndexFile::getIndexFileName(fileSystemFileName);
            loadVectorsFromIndex(indexFileName.c_str(), fileRef);
        }
        else
        {
            // process lines in file
            FileReader freader(fileSystemFileName);
            char *line;
            LineTokenizer tokenizer;
            FileRun *fileRunRef = NULL;
            ResultItem *resultItemRef = NULL;
            while ((line=freader.getNextLineBufferPointer())!=NULL)
            {
                int len = freader.getLastLineLength();
                int numTokens = tokenizer.tokenize(line, len);
                char **tokens = tokenizer.tokens();
                processLine(tokens, numTokens, fileRunRef, resultItemRef, fileRef, freader.getNumReadLines());
            }

            fileRef->numLines = freader.getNumReadLines();
        }
    }
    catch (std::exception&)
    {
        try
        {
            unloadFile(fileRef);
        }
        catch (...) {}

        throw;
    }

    return fileRef;
}

void ResultFileManager::loadVectorsFromIndex(const char *filename, ResultFile *fileRef)
{
    VectorFileIndex *index = IndexFileReader(filename).readAll();
    int numOfVectors = index->getNumberOfVectors();

    if (numOfVectors == 0) {
        delete index;
        return;
    }

    Run *runRef = getRunByName(index->run.runName.c_str());
    if (!runRef)
    {
        runRef = addRun();
        runRef->runName = index->run.runName;
    }
    runRef->runNumber = index->run.runNumber;
    runRef->attributes = index->run.attributes;
    runRef->moduleParams = index->run.moduleParams;
    FileRun *fileRunRef = addFileRun(fileRef, runRef);

    for (int i = 0; i < numOfVectors; ++i)
    {
        const VectorData *vectorRef = index->getVectorAt(i);
        assert(vectorRef);

        VectorResult vectorResult;
        vectorResult.fileRunRef = fileRunRef;
        vectorResult.attributes = StringMap(vectorRef->attributes);
        vectorResult.vectorId = vectorRef->vectorId;
        vectorResult.moduleNameRef = stringSetFindOrInsert(moduleNames, vectorRef->moduleName);
        vectorResult.nameRef = stringSetFindOrInsert(names, vectorRef->name);
        vectorResult.columns = vectorRef->columns;
        vectorResult.stat = vectorRef->stat;
        fileRef->vectorResults.push_back(vectorResult);
    }
    delete index;
}

void ResultFileManager::unloadFile(ResultFile *file)
{
    // remove computed vector IDs
    for (ComputedIDCache::iterator it = computedIDCache.begin(); it != computedIDCache.end();)
    {
        ID id = it->first.second;
        if (_fileid(id) == file->id)
        {
            ComputedIDCache::iterator oldIt = it;
            it++;
            computedIDCache.erase(oldIt);
        }
        else
            ++it;
    }

    // remove FileRun entries
    RunList runsPotentiallyToBeDeleted;
    for (int i=0; i<(int)fileRunList.size(); i++)
    {
        if (fileRunList[i]->fileRef==file)
        {
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
    fileList[file->id] = NULL;
    delete file;

    // remove Runs that don't appear in other loaded files
    for (int i=0; i<(int)runsPotentiallyToBeDeleted.size(); i++)
    {
        Run *runRef = runsPotentiallyToBeDeleted[i];
        if (getFilesForRun(runRef).empty())
        {
            // delete it.
            RunList::iterator it = std::find(runList.begin(), runList.end(), runRef);
            assert(it != runList.end());  // runs may occur only once in runsPotentiallyToBeDeleted, because runNames are not allowed to repeat in files
            runList.erase(it);
        }
    }
}

/**
 * Utility class: collects strings that occur at least twice.
 */
class DuplicateStringCollector
{
  private:
    typedef std::map<std::string,int> StringCountMap;
    StringCountMap map;
    StringVector vec;
  public:
    void add(const std::string& str) {
        StringCountMap::iterator mi = map.find(str);
        if (mi==map.end())
            map[str] = 1;  // 1st occurrence
        else if (++(mi->second)==2)  // 2nd occurrence
            vec.push_back(str);
    }
    StringVector& get() {
        return vec;
    }
};

StringVector ResultFileManager::getFileAndRunNumberFilterHints(const IDList& idlist)
{
    FileRunList *fileRuns = getUniqueFileRuns(idlist);

    StringVector vec;
    DuplicateStringCollector coll;

    for (int i=0; i<(int)fileRuns->size(); i++)
    {
        FileRun *fileRun = (*fileRuns)[i];
        if (fileRun->runRef->runNumber==0)
        {
            vec.push_back(fileRun->fileRef->filePath);
        }
        else
        {
            char runNumberStr[32];
            sprintf(runNumberStr, "%d", fileRun->runRef->runNumber);
            vec.push_back(fileRun->fileRef->filePath+"#"+runNumberStr);
            coll.add(fileRun->fileRef->filePath+"#*");
        }
    }
    delete fileRuns;

    // sort and concatenate them, and return the result
    StringVector wildvec = coll.get();
    std::sort(vec.begin(), vec.end());
    std::sort(wildvec.begin(), wildvec.end());
    //vec.insert(vec.end(), wildvec.begin(), wildvec.end());
    wildvec.insert(wildvec.end(), vec.begin(), vec.end());
    return wildvec;
}

StringVector ResultFileManager::getModuleFilterHints(const IDList& idlist)
{
    StringSet& names = *getUniqueModuleNames(idlist);

    StringVector vec;
    DuplicateStringCollector coll;

    for (StringSet::iterator i=names.begin(); i!=names.end(); i++)
    {
        std::string a = (*i);
        vec.push_back(a);

        // replace embedded numbers with "*"
        //FIXME TODO!!!!!!!!!!!!!!

        // break it up along dots, and...
        StringTokenizer tokenizer(a.c_str(), ".");
        const char *prefix = "";
        const char *suffix = ".*";
        while (tokenizer.hasMoreTokens()) {
            const char *token = tokenizer.nextToken();
            if (!tokenizer.hasMoreTokens()) suffix = "";
            coll.add(std::string(prefix)+token+suffix);
            prefix = "*.";
        }
    }
    delete &names;

    // sort and concatenate them, and return the result
    StringVector wildvec = coll.get();
    wildvec.push_back(std::string("*"));
    std::sort(vec.begin(), vec.end());
    std::sort(wildvec.begin(), wildvec.end());
    //vec.insert(vec.end(), wildvec.begin(), wildvec.end());
    wildvec.insert(wildvec.end(), vec.begin(), vec.end());
    return wildvec;
}

StringVector ResultFileManager::getNameFilterHints(const IDList& idlist)
{
    StringSet& names = *getUniqueNames(idlist);

    StringVector vec;
    DuplicateStringCollector coll;

    for (StringSet::iterator i=names.begin(); i!=names.end(); i++)
    {
        std::string a = (*i);
        vec.push_back(a);

        // break it up along spaces, and...
        StringTokenizer tokenizer(a.c_str());
        const char *prefix = "";
        const char *suffix = " *";
        while (tokenizer.hasMoreTokens()) {
            const char *token = tokenizer.nextToken();
            if (!tokenizer.hasMoreTokens()) suffix = "";
            coll.add(std::string(prefix)+token+suffix);
            prefix = "* ";
        }
    }
    delete &names;

    // sort and concatenate them, and return the result
    StringVector wildvec = coll.get();
    wildvec.push_back(std::string("*"));
    std::sort(vec.begin(), vec.end());
    std::sort(wildvec.begin(), wildvec.end());
    //vec.insert(vec.end(), wildvec.begin(), wildvec.end());
    wildvec.insert(wildvec.end(), vec.begin(), vec.end());
    return wildvec;
}


