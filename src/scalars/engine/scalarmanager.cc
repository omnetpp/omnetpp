//=========================================================================
//  SCALARMANAGER.CC - part of
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
#include "patmatch.h"
#include "scalarmanager.h"
#include "filetokenizer.h"


ScalarManager::ScalarManager()
{
    lastInsertedModuleRef = moduleNames.end();
}

ScalarManager::~ScalarManager()
{
}

ScalarManager::StringRef ScalarManager::stringMapFindOrInsert(StringSet& set, const std::string& str)
{
    StringRef m = set.find(str);
    if (m==set.end())
    {
        std::pair<StringSet::iterator,bool> p = set.insert(str);
        m = p.first;
    }
    return m;
}

void ScalarManager::addValue(RunRef runRef, const char *moduleName,
                             const char *scalarName, double value)
{
    Datum d;
    d.runRef = runRef;

    // lines in omnetpp.sca are usually grouped by module, we can exploit this for efficiency
    if (lastInsertedModuleRef!=moduleNames.end() && *lastInsertedModuleRef==moduleName)
    {
        d.moduleNameRef = lastInsertedModuleRef;
    }
    else
    {
        StringRef m = stringMapFindOrInsert(moduleNames, std::string(moduleName));
        d.moduleNameRef = lastInsertedModuleRef = m;
    }

    d.scalarNameRef = stringMapFindOrInsert(scalarNames, std::string(scalarName));

    d.value = value;

    scalarValues.push_back(d);
}


void ScalarManager::dump(FileRef fileRef, std::ostream& out) const
{
    RunRef prevRunRef = runList.end();
    for (Values::const_iterator i = scalarValues.begin(); i!=scalarValues.end(); i++)
    {
        const Datum& d = *i;
        if (d.runRef->fileRef==fileRef)
        {
            if (d.runRef!=prevRunRef)
            {
                out << "run " << d.runRef->runNumber << " " << d.runRef->networkName
                    << " \"" << d.runRef->date << "\"\n";
                prevRunRef = d.runRef;
            }
            out << "scalar \"" << *d.moduleNameRef << "\"\t\""
                << *d.scalarNameRef << "\"\t" << d.value << "\n";
        }
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

void ScalarManager::processLine(char **vec, int numtokens, RunRef& runRef, FileRef fileRef, int lineNum)
{
    if (numtokens>=1 && !strcmp(vec[0],"run"))
    {
        if (numtokens<2)
            throw new TException("invalid scalar file: no run number on `run' line, line %d", lineNum);

        // add a new Run, and fill in its members
        runList.push_back(Run());
        runRef = --(runList.end());

        Run& run = runList.back();
        run.fileRef = fileRef;
        run.runNumber = atoi(vec[1]);
        run.networkName = numtokens<3 ? "" : vec[2];
        run.date = numtokens<4 ? "" : vec[3];
        run.lineNum = lineNum;

        std::stringstream os;
        os << run.runNumber << " (at line " << lineNum << ")";
        run.runName = os.str();
        //if (!date.empty())
        //    run.runName += " (" + date + ")";
        run.fileAndRunName = fileRef->filePath + "#" + runRef->runName;
    }
    else if (numtokens>=1 && !strcmp(vec[0],"scalar"))
    {
        if (runRef==runList.end())
            throw new TException("invalid scalar file: no `run' line before first `scalar' line, line %d", lineNum);

        // syntax: "scalar <module> <scalarname> <value>"
        if (numtokens<4)
            throw new TException("invalid scalar file: too few items on `scalar' line, line %d", lineNum);

        double value;
        if (!parseDouble(vec[3],value))
            throw new TException("invalid scalar file syntax: invalid value column, line %d", lineNum);

        addValue(runRef, vec[1], vec[2], value);
    }
    else
    {
        // some other line, ignore
    }
}


ScalarManager::FileRef ScalarManager::loadFile(const char *filename)
{
    // check
    for (FileRef i = fileList.begin(); i!=fileList.end(); i++)
        if (i->filePath==filename)
            throw new TException("file already loaded");

    // try if file can be opened, before we add it to our database
    FILE *f = fopen(filename, "r");
    if (!f)
        throw new TException("cannot open `%s' for read", filename);
    fclose(f);

    // add to fileList
    fileList.push_back(File());
    FileRef fileRef = --(fileList.end());
    File& file = fileList.back();
    file.filePath = filename;
    splitFileName(filename, file.directory, file.fileName);

    RunRef runRef = runList.end();

    FileTokenizer ftok(filename);
    while (ftok.readLine())
    {
        int numtokens = ftok.numTokens();
        char **vec = ftok.tokens();
        processLine(vec, numtokens, runRef, fileRef, ftok.lineNum());
    }
    if (!ftok.eof())
        throw new TException(ftok.errorMsg().c_str());

    return fileRef;
}


