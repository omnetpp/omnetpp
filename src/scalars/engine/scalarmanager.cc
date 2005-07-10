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
    RunRef prevRunRef = NULL;
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
            throw new TException("invalid vector file syntax: missing close quote, line %d", lineNum);
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

void ScalarManager::processLine(char *&s, RunRef& runRef, FileRef fileRef, int lineNum)
{
    if (!strncmp(s,"run ",4))
    {
        // parse and store run number
        s+=4;
        std::string runNumber, networkName, date;
        parseString(s, runNumber, lineNum);
        parseString(s, networkName, lineNum);
        parseString(s, date, lineNum);
        while (*s && *s!='\r' && *s!='\n') s++;
        if (runNumber.empty())
            throw new TException("invalid scalar file: no run number on `run' line, line %d", lineNum);

        // add a new Run, and fill in its members
        runList.push_back(Run());
        runRef = --(runList.end());

        Run& run = runList.back();
        run.fileRef = fileRef;
        run.runNumber = atoi(runNumber.c_str());
        run.networkName = networkName;
        run.date = date;
        run.lineNum = lineNum;

        std::stringstream os;
        os << runNumber << " (at line " << lineNum << ")";
        run.runName = os.str();
        //if (!date.empty())
        //    run.runName += " (" + date + ")";
        run.fileAndRunName = fileRef->filePath + "#" + runRef->runName;
    }
    else if (!strncmp(s,"scalar ",7))
    {
        if (runRef==NULL)
            throw new TException("invalid scalar file: no `run' line before first `scalar' line, line %d", lineNum);

        s+=7;

        std::string moduleName;
        std::string scalarName;
        parseString(s, moduleName, lineNum);
        parseString(s, scalarName, lineNum);

        char *e;
        double value = strtod(s,&e);
        if (s==e)
            throw new TException("invalid scalar file syntax: invalid value column, line %d", lineNum);
        s = e;
        while (*s==' ' || *s=='\t') s++;

        addValue(runRef, moduleName.c_str(), scalarName.c_str(), value);
    }
}


ScalarManager::FileRef ScalarManager::loadFile(const char *filename)
{
    // check
    for (FileRef i = fileList.begin(); i!=fileList.end(); i++)
        if (i->filePath==filename)
            throw new TException("file already loaded");

    // open file
    FILE *f = fopen(filename, "r");
    if (!f)
        throw new TException("cannot open `%s' for read", filename);

    // add to fileList
    fileList.push_back(File());
    FileRef fileRef = --(fileList.end());
    File& file = fileList.back();
    file.filePath = filename;
    splitFileName(filename, file.directory, file.fileName);

    int lineNum = 0;
    RunRef runRef = NULL;

    // process file, reading it in large chunks
    const int buffersize = 4*1024*1024;  // 4MB
    char *buffer = new char[buffersize+1];
    std::auto_ptr<char> bufferDeleter(buffer);
    int bufferused = 0;
    bool eofreached = false;

    while (!eofreached)
    {
        // read enough bytes to fill the buffer
        int bytesread = fread(buffer+bufferused, 1, buffersize-bufferused, f);
        if (feof(f))
            eofreached = true;
        if (ferror(f))
            throw new TException("error reading from `%s'", filename);

        buffer[bufferused+bytesread] = '\0';
        const char *endbuffer = buffer+bufferused+bytesread;

        char *line = buffer;

        // process all full lines
        while (1)
        {
            // do we have a full line?
            char *s = line;
            while (*s && *s!='\r' && *s!='\n') s++;
            char *endline = s;
            if (!*s && !eofreached)  // if at eof, we have to process unterminated last line, too
                break;

            lineNum++;
            //printf("%d\r",lineNum);

            s = line;
            if (!*s && eofreached)
            {
                // end of file, no more work
                break;
            }
            else if (!*s || *s=='\r' || *s=='\n')
            {
                // blank line, ignore
            }
            else if (*s=='#')
            {
                // ignore ("#...") lines
                while (*s && *s!='\r' && *s!='\n') s++;
            }
            else
            {
                // parse line
                char old = *endline;
                *endline = 0;

                processLine(s, runRef, fileRef, lineNum);

                *endline = old;
            }

            // skip line termination
            if (*s && *s!='\r' && *s!='\n')
                throw new TException("invalid file syntax: garbage at end of line: `%s'",s);
            while (*s=='\r' || *s=='\n') s++;
            line = s;
        }

        // copy the last (partial) line to beginning of buffer
        bufferused = endbuffer-line;
        memcpy(buffer, line, bufferused);
    }
    printf("DBG: read %d lines\n",lineNum);

    // dump(fileRef, std::cout);

    return fileRef;
}


