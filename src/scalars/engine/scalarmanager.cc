//=========================================================================
//  SCALARMANAGER.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <stdlib.h>
#include <fstream>
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

void ScalarManager::addValue(FileRef fileRef, int runNumber, const char *moduleName,
                             const char *scalarName, double value)
{
    Datum d;
    d.fileRef = fileRef;
    d.runNumber = runNumber;

    // lines in omnetpp.sca are usually grouped by module, we can exploit this for efficiency
    if (lastInsertedModuleRef!=moduleNames.end() && *lastInsertedModuleRef==moduleName)
    {
        d.moduleNameRef = lastInsertedModuleRef;
    }
    else
    {
        StringRef m = moduleNames.find(moduleName);
        if (m==moduleNames.end())
        {
            std::pair<StringSet::iterator,bool> p = moduleNames.insert(std::string(moduleName));
            m = p.first;
        }
        d.moduleNameRef = lastInsertedModuleRef = m;
    }

    StringRef s = scalarNames.find(scalarName);
    if (s==scalarNames.end())
    {
        std::pair<StringSet::iterator,bool> p = scalarNames.insert(std::string(scalarName));
        s = p.first;
    }
    d.scalarNameRef = s;

    d.value = value;

    scalarValues.push_back(d);
}


void ScalarManager::dump(FileRef fileRef, int runNumber, std::ostream out) const
{
    for (Values::const_iterator i = scalarValues.begin(); i!=scalarValues.end(); i++)
    {
        const Datum& d = *i;
        if (d.fileRef==fileRef && d.runNumber==runNumber)
        {
            out << "scalar \"" << (*d.moduleNameRef) << "\"\t\""
                << (*d.scalarNameRef) << "\"\t" << d.value << "\n";
        }
    }
}


static void parseAndAppendQuotedString(char *&s, std::string& dest)
{
    while (*s==' ' || *s=='\t') s++;
    if (*s!='"')
        throw new TException("invalid vector file syntax: invalid vector definition: missing opening quote");
    char *start = s+1;
    s++;
    while (*s && (*s!='"' || *(s-1)=='\\') && *s!='\r' && *s!='\n') s++;
    if (*s!='"')
        throw new TException("invalid vector file syntax: invalid vector definition: missing closing quote");
    *s = '\0';
    dest += start;
    *s = '"';
    s++;
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

void ScalarManager::processLine(char *&s, FileMap::iterator fileRef, int& runNumber)
{
    if (!strncmp(s,"run ",4))
    {
        // parse and store run number
        s+=4;
        runNumber = atoi(s);
        fileRef->second.runNumbers.push_back(runNumber);  // FIXME if not already there!
        while (*s && *s!='\r' && *s!='\n') s++;
    }
    else if (!strncmp(s,"scalar ",7))
    {
        if (runNumber==0)
            throw new TException("invalid scalar file: no `run' line before first `scalar' line");

        s+=7;

        std::string moduleName;
        std::string scalarName;
        parseAndAppendQuotedString(s, moduleName);
        parseAndAppendQuotedString(s, scalarName);

        char *e;
        double value = strtod(s,&e);
        if (s==e)
            throw new TException("invalid scalar file syntax: invalid value column");
        s = e;
        while (*s==' ' || *s=='\t') s++;

        addValue(fileRef, runNumber, moduleName.c_str(), scalarName.c_str(), value);
    }
}


ScalarManager::FileRef ScalarManager::loadFile(const char *filename)
{
    FileMap::iterator fileRef = scalarFiles.find(filename);
    if (fileRef!=scalarFiles.end())
        throw new TException("file already loaded");

    // open file
    FILE *f = fopen(filename, "r");
    if (!f)
        throw new TException("cannot open `%s' for read", filename);

    scalarFiles[filename]; // force adding it to the map
    fileRef = scalarFiles.find(filename);

    fileRef->second.filePath = filename;
    splitFileName(filename, fileRef->second.directory, fileRef->second.fileName);

    int lineNum = 0;
    int runNumber = 0;

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

                processLine(s, fileRef, runNumber);

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

    return fileRef;
}



/*
int main(int argc, char **argv)
{
    if (argc<2)
        return 1;

    ScalarManager scamgr;
    ScalarManager::FileRef ref = scamgr.loadFile(argv[1]);
    scamgr.dump(ref,1,std::cout);
    return 0;
}
*/
