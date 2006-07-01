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
#include "patmatch.h"
#include "resultfilemanager.h"
#include "filetokenizer.h"
#include "cstrtokenizer.h"


ResultFileManager::ResultFileManager()
{
}

ResultFileManager::~ResultFileManager()
{
    for (int i=0; i<fileRunList.size(); i++)
        delete fileRunList[i];

    for (int i=0; i<runList.size(); i++)
        delete runList[i];

    for (int i=0; i<fileList.size(); i++)
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

RunList ResultFileManager::getRunsInFile(ResultFile *file) const
{
    RunList out;
    for (int i=0; i<fileRunList.size(); i++)
        if (fileRunList[i]->fileRef == file)
            out.push_back(fileRunList[i]->runRef);
    return out;
}

ResultFileList ResultFileManager::getFilesForRun(Run *run) const
{
    ResultFileList out;
    for (int i=0; i<fileRunList.size(); i++)
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
            default: throw new Exception("ResultFileManager: invalid ID: bad type");
        }
    }
    catch (std::out_of_range e)
    {
        throw new Exception("ResultFileManager::getItem(id): invalid ID");
    }
}

ResultFileList *ResultFileManager::getUniqueFiles(const IDList& ids) const
{
    // collect unique runs in this dataset
    std::set<ResultFile*> set;
    for (int i=0; i<ids.size(); i++)
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
    for (int i=0; i<ids.size(); i++)
        set.insert(getItem(ids.get(i)).fileRunRef->runRef);

    // convert to list for easier handling at recipient
    RunList *list = new RunList();
    for (std::set<Run*>::iterator i = set.begin(); i!=set.end(); i++)
        list->push_back(*i);
    return list;
}

StringSet *ResultFileManager::getUniqueModuleNames(const IDList& ids) const
{
    // collect unique module names in this dataset
    StringSet *set = new StringSet();
    for (int i=0; i<ids.size(); i++)
        set->insert(*getItem(ids.get(i)).moduleNameRef);
    return set;
}

StringSet *ResultFileManager::getUniqueNames(const IDList& ids) const
{
    // collect unique scalar/vector names in this dataset
    StringSet *set = new StringSet();
    for (int i=0; i<ids.size(); i++)
        set->insert(*getItem(ids.get(i)).nameRef);
    return set;
}

const ScalarResult& ResultFileManager::getScalar(ID id) const
{
    if (_type(id)!=SCALAR)
        throw new Exception("ResultFileManager::getScalar(id): this item is not a scalar");
    return getFileForID(id)->scalarResults.at(_pos(id));
}

const VectorResult& ResultFileManager::getVector(ID id) const
{
    if (_type(id)!=VECTOR)
        throw new Exception("ResultFileManager::getVector(id): this item is not a vector");
    return getFileForID(id)->vectorResults.at(_pos(id));
}

IDList ResultFileManager::getAllScalars() const
{
    IDList out;
    for (int k=0; k<fileList.size(); k++)
    {
        if (fileList[k]!=NULL)
        {
            ScalarResults& v = fileList[k]->scalarResults;
            for (int i=0; i<v.size(); i++)
                out.uncheckedAdd(_mkID(SCALAR,k,i));
        }
    }
    return out;
}

IDList ResultFileManager::getAllVectors() const
{
    IDList out;
    for (int k=0; k<fileList.size(); k++)
    {
        if (fileList[k]!=NULL)
        {
            VectorResults& v = fileList[k]->vectorResults;
            for (int i=0; i<v.size(); i++)
                out.uncheckedAdd(_mkID(VECTOR,k,i));
        }
    }
    return out;
}

IDList ResultFileManager::getScalarsInFileRun(FileRun *fileRun) const
{
    IDList out;
    int fileId = fileRun->fileRef->id;
    ScalarResults& v = fileRun->fileRef->scalarResults;
    for (int i=0; i<v.size(); i++)
        if (v[i].fileRunRef==fileRun)
            out.uncheckedAdd(_mkID(SCALAR,fileId,i));
    return out;
}

IDList ResultFileManager::getVectorsInFileRun(FileRun *fileRun) const
{
    IDList out;
    int fileId = fileRun->fileRef->id;
    VectorResults& v = fileRun->fileRef->vectorResults;
    for (int i=0; i<v.size(); i++)
        if (v[i].fileRunRef==fileRun)
            out.uncheckedAdd(_mkID(VECTOR,fileId,i));
    return out;
}

bool ResultFileManager::isFileLoaded(const char *filename) const
{
    return getFile(filename)!=NULL;
}

ResultFile *ResultFileManager::getFile(const char *filename) const
{
    if (!filename)
        return NULL;
    for (int i=0; i<fileList.size(); i++)
        if (fileList[i]!=NULL && fileList[i]->filePath==filename)
            return fileList[i];
    return NULL;
}

Run *ResultFileManager::getRunByName(const char *runName) const
{
    if (!runName)
        return NULL;
    for (int i=0; i<runList.size(); i++)
        if (runList[i]->runName==runName)
            return runList[i];
    return NULL;
}

FileRun *ResultFileManager::getFileRun(ResultFile *file, Run *run) const
{
    for (int i=0; i<fileRunList.size(); i++)
        if (fileRunList[i]->fileRef==file && fileRunList[i]->runRef==run)
            return fileRunList[i];
    return NULL;
}

// currently unused
ID ResultFileManager::getItemByName(ResultFile *fileRef, const char *module, const char *name) const
{
    if (!fileRef || !module || !name)
        return 0;

    StringSet::const_iterator m = moduleNames.find(module);
    if (m==moduleNames.end())
        return 0;
    const std::string *moduleNameRef = &(*m);

    StringSet::const_iterator n = names.find(name);
    if (n==names.end())
        return 0;
    const std::string *nameRef = &(*n);

    ScalarResults& scalarResults = fileRef->scalarResults;
    for (int i=0; i<scalarResults.size(); i++)
    {
        const ResultItem& d = scalarResults[i];
        if (d.moduleNameRef==moduleNameRef && d.nameRef==nameRef)
            return _mkID(SCALAR, fileRef->id, i);
    }

    VectorResults& vectorResults = fileRef->vectorResults;
    for (int i=0; i<vectorResults.size(); i++)
    {
        const ResultItem& d = vectorResults[i];
        if (d.moduleNameRef==moduleNameRef && d.nameRef==nameRef)
            return _mkID(VECTOR, fileRef->id, i);
    }
    return 0;
}

IDList ResultFileManager::getFilteredList(const IDList& idlist,
                                          const FileRunList *fileRunFilter,
                                          const char *moduleFilter,
                                          const char *nameFilter)
{
    // "*" means no filtering, so ignore it
    if (!strcmp(moduleFilter,"*")) moduleFilter = "";
    if (!strcmp(nameFilter,"*")) nameFilter = "";

    // prepare for wildcard matches
    bool patMatchModule = contains_wildcards(moduleFilter);
    bool patMatchName = contains_wildcards(nameFilter);

    short *modulePattern = NULL;
    if (patMatchModule)
    {
        modulePattern = new short[512]; // FIXME!
        transform_pattern(moduleFilter, modulePattern);
    }
    short *namePattern = NULL;
    if (patMatchName)
    {
        namePattern = new short[512]; // FIXME!
        transform_pattern(nameFilter, namePattern);
    }

    // FIXME TODO: if there's no wildcard, find string pointers in the stringsets
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
            (patMatchModule ? !stringmatch(modulePattern, d.moduleNameRef->c_str())
                            : strcmp(d.moduleNameRef->c_str(), moduleFilter))
           )
            continue; // no match

        if (nameFilter && nameFilter[0] &&
            (patMatchName ? !stringmatch(namePattern, d.nameRef->c_str())
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

ResultFile *ResultFileManager::addFile()
{
    ResultFile *file = new ResultFile();
    file->id = fileList.size();
    fileList.push_back(file);
    file->resultFileManager = this;
    file->numLines = 0;
    file->numUnrecognizedLines = 0;
    return file;
}

Run *ResultFileManager::addRun()
{
    Run *run = new Run();
    run->resultFileManager = this;
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
    if (lastInsertedModuleRef && *lastInsertedModuleRef==moduleName)
    {
        d.moduleNameRef = lastInsertedModuleRef;
    }
    else
    {
        std::string *m = stringSetFindOrInsert(moduleNames, std::string(moduleName));
        d.moduleNameRef = lastInsertedModuleRef = m;
    }

    d.nameRef = stringSetFindOrInsert(names, std::string(scalarName));

    d.value = value;

    fileRunRef->fileRef->scalarResults.push_back(d);
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
                << *d.nameRef << "\"\t" << d.value << "\n";
        }
    }
}
*/

// XXX unused function
static void parseString(char *&s, std::string& dest, int lineNum)
{
    while (*s==' ' || *s=='\t') s++;
    if (*s=='"')
    {
        // parse quoted string
        char *start = s+1;
        s++;
        while (*s && (*s!='"' || *(s-1)=='\\') && *s!='\r' && *s!='\n') s++;
        if (*s!='"')
            throw new Exception("invalid syntax: missing close quote, line %d", lineNum);
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

static double zero =0;

static bool parseDouble(char *s, double& dest)
{
    char *e;
    dest = strtod(s,&e);
    if (!*e)
    {
        return true;
    }
    if (strstr(s,"INF") || strstr(s, "inf"))
    {
        dest = 1/zero;  // +INF or -INF
        if (*s=='-') dest = -dest;
        return true;
    }
    return false;
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

static std::string filenameToSlash(const char *filename)
{
    std::string res;
    res.reserve(strlen(filename));
    for (; *filename; filename++)
        res.append(1, *filename=='\\' ? '/' : *filename);
    return res;
}

void ResultFileManager::processLine(char **vec, int numTokens, FileRun *&fileRunRef, ResultFile *fileRef, int lineNum)
{
    // ignore empty lines
    if (numTokens==0 || vec[0][0]=='#')
        return;

    // process "run" lines
    if (vec[0][0]=='r' && !strcmp(vec[0],"run"))
    {
        if (numTokens<2)
            throw new Exception("invalid result file: run Id missing from `run' line, line %d", lineNum);

        if (atoi(vec[1])>0 && strlen(vec[1])<=10)
        {
            // old-style "run" line, format: run <runNumber> [<networkName>] [<dateTime>]
            // and runs in different files cannot be related, so we must create a new Run entry.
            Run *runRef = addRun();
            fileRunRef = addFileRun(fileRef, runRef);

            runRef->runNumber = atoi(vec[1]);
            runRef->attributes["runNumber"] = vec[1];
            if (numTokens>=3)
                runRef->attributes["networkName"] = vec[2];
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
                throw new Exception("invalid result file: run Id repeats in the file, line %d", lineNum);
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

        // make up a unique runName
        std::stringstream os;
        static int counter=1000;
        os << fileRef->fileName << ":" << lineNum << "-#n/a-" <<++counter;
        runRef->runName = os.str();
    }

    if (vec[0][0]=='a' && !strcmp(vec[0],"attr"))
    {
        if (numTokens<3)
            throw new Exception("invalid result file: 'attr <name> <value>' expected, line %d", lineNum);

        // store attribute
        fileRunRef->runRef->attributes[vec[1]] = vec[2];
    }
    else if (vec[0][0]=='p' && !strcmp(vec[0],"param"))
    {
        if (numTokens<3)
            throw new Exception("invalid result file: 'param <namePattern> <value>' expected, line %d", lineNum);

        // store attribute
        fileRunRef->runRef->moduleParams[vec[1]] = vec[2];
    }
    else if (vec[0][0]=='s' && !strcmp(vec[0],"scalar"))
    {
        // syntax: "scalar <module> <scalarname> <value>"
        if (numTokens<4)
            throw new Exception("invalid scalar file: too few items on `scalar' line, line %d", lineNum);

        double value;
        if (!parseDouble(vec[3],value))
            throw new Exception("invalid scalar file syntax: invalid value column, line %d", lineNum);

        addScalar(fileRunRef, vec[1], vec[2], value);
        return;
    }
    else if (vec[0][0]=='v' && !strcmp(vec[0],"vector"))
    {
        // vector line
        if (numTokens<4)
            throw new Exception("invalid vector file syntax: too few items on 'vector' line, line %d", lineNum);

        VectorResult vecdata;
        vecdata.fileRunRef = fileRunRef;

        // vectorId
        char *e;
        vecdata.vectorId = (int) strtol(vec[1],&e,10);
        if (*e)
            throw new Exception("invalid vector file syntax: invalid vector id in vector definition, line %d", lineNum);

        // module name, vector name
        const char *moduleName = vec[2];
        const char *vectorName = vec[3];
        vecdata.moduleNameRef = stringSetFindOrInsert(moduleNames, std::string(moduleName));
        vecdata.nameRef = stringSetFindOrInsert(names, std::string(vectorName));

        fileRef->vectorResults.push_back(vecdata);
    }
    else if (isdigit(vec[0][0]) && numTokens==3)
    {
        // this looks like a vector data line, skip it this time
    }
    else
    {
        // ignore unknown lines and vector data lines
        fileRef->numUnrecognizedLines++;
    }
}


ResultFile *ResultFileManager::loadFile(const char *filename)
{
    // check
    if (isFileLoaded(filename))
        return getFile(filename);

    // try if file can be opened, before we add it to our database
    FILE *f = fopen(filename, "r");
    if (!f)
        throw new Exception("cannot open `%s' for read", filename);
    fclose(f);

    // add to fileList
    ResultFile *fileRef = addFile();
    fileRef->filePath = filenameToSlash(filename);
    splitFileName(fileRef->filePath.c_str(), fileRef->directory, fileRef->fileName);

    FileRun *fileRunRef = NULL;

    FileTokenizer ftok(filename);
    while (ftok.readLine())
    {
        int numTokens = ftok.numTokens();
        char **vec = ftok.tokens();
        processLine(vec, numTokens, fileRunRef, fileRef, ftok.lineNum());
    }

    // ignore "incomplete last line" error, because we might be reading
    // from a vec file currently being written by a simulation
    if (!ftok.eof() && ftok.errorCode()!=FileTokenizer::INCOMPLETELINE)
        throw new Exception(ftok.errorMsg().c_str());

    fileRef->numLines = ftok.lineNum();

    return fileRef;
}

void ResultFileManager::unloadFile(ResultFile *file)
{
    // remove FileRun entries
    RunList runsPotentiallyToBeDeleted;
    for (int i=0; i<fileRunList.size(); i++)
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
    for (int i=0; i<runsPotentiallyToBeDeleted.size(); i++)
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

StringVector ResultFileManager::getRunNameFilterHints(const IDList& idlist)
{
    RunList& runs = *getUniqueRuns(idlist);

    StringVector vec;
    DuplicateStringCollector coll;

    for (int i=0; i<runs.size(); i++)
    {
        std::string a = runs[i]->runName;
        vec.push_back(a);

        int k = a.find('#');  //XXX this code is out of date now...
        if (k!=a.npos)
        {
            a.replace(k, a.length()-k, "#*");
            coll.add(a);
        }
    }
    delete &runs;

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
        cStringTokenizer tokenizer(a.c_str(), ".");
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
        cStringTokenizer tokenizer(a.c_str());
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
    std::sort(vec.begin(), vec.end());
    std::sort(wildvec.begin(), wildvec.end());
    //vec.insert(vec.end(), wildvec.begin(), wildvec.end());
    wildvec.insert(wildvec.end(), vec.begin(), vec.end());
    return wildvec;
}


