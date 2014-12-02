//==========================================================================
//  INIFILEREADER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <set>
#include <vector>
#include <algorithm>
#include "opp_ctype.h"
#include "fileutil.h"  // directoryOf
#include "inifilereader.h"
#include "fsutils.h"
#include "cexception.h"

NAMESPACE_BEGIN


#define ERRPREFIX  "Error at `%s' line %d: "


InifileReader::InifileReader()
{
}

InifileReader::~InifileReader()
{
}

void InifileReader::initializeFrom(cConfiguration *bootConfig)
{
    throw cRuntimeError("InifileReader: initializeFrom() method not supported");
}

const char *InifileReader::getFileName() const
{
    return rootfilename.c_str();
}

const char *InifileReader::getDefaultBaseDirectory() const
{
    return defaultbasedir.c_str();
}

int InifileReader::getNumSections() const
{
    return sections.size();
}

const char *InifileReader::getSectionName(int sectionId) const
{
    const Section& section = getSection(sectionId);
    return section.name.c_str();
}

int InifileReader::getNumEntries(int sectionId) const
{
    const Section& section = getSection(sectionId);
    return section.entries.size();
}

const InifileReader::KeyValue& InifileReader::getEntry(int sectionId, int entryId) const
{
    const Section& section = getSection(sectionId);
    if (entryId<0 || entryId>=(int)section.entries.size())
        throw cRuntimeError("InifileReader: entry index %d out of bounds", entryId);
    return section.entries[entryId];
}

const InifileReader::Section& InifileReader::getSection(int sectionId) const
{
    if (sectionId<0 || sectionId>=(int)sections.size())
        throw cRuntimeError("InifileReader: section index %d out of bounds", sectionId);
    return sections[sectionId];
}

int InifileReader::getOrCreateSection(const char *sectionName)
{
    for (int i=0; i<(int)sections.size(); i++)
        if (!strcmp(sections[i].name.c_str(), sectionName))
            return i;
    Section section;
    section.name = sectionName;
    sections.push_back(section);
    return sections.size()-1;
}

void InifileReader::readFile(const char *filename)
{
    rootfilename = filename;

    // use the first ini file's location as default base dir
    if (defaultbasedir.empty())
        defaultbasedir = directoryOf(rootfilename.c_str());

    std::vector<std::string> includedFiles;
    internalReadFile(filename, -1, includedFiles);
}

void InifileReader::internalReadFile(const char *filename, int currentSectionIndex, std::vector<std::string> &includedFiles)
{
    // create an entry for this file, checking against circular inclusion
    std::string tmpfname = tidyFilename(toAbsolutePath(filename).c_str(),true);
    std::string tmpdir = directoryOf(tmpfname.c_str());
    if (find(includedFiles.begin(), includedFiles.end(), tmpfname)!=includedFiles.end())
        throw cRuntimeError("Ini file `%s' includes itself, directly or indirectly", filename);
    filenames.insert(tmpfname);
    if (basedirs.find(tmpdir)==basedirs.end())
        basedirs.insert(tmpdir);

    // get a reference to the string instance in filenames[], we'll refer to it in KeyValue
    const std::string *filenameRef = &(*filenames.find(tmpfname));
    const std::string *basedirRef = &(*basedirs.find(tmpdir));

    // open and read this file
    FILE *file = fopen(filename,"r");
    if (!file)
        throw cRuntimeError("Cannot open ini file `%s'", filename);

    int lineNumber = 0;
    std::set<std::string> sectionsInFile; // we'll check for uniqueness

    std::string rawLine;
    while (readLineInto(rawLine, file))
    {
        ASSERT(rawLine.empty() || (*(rawLine.end()-1)!='\r' && *(rawLine.end()-1)!='\n'));

        // join continued lines
        lineNumber++;
        std::string lineBuf = rawLine;
        if (!rawLine.empty() && *(rawLine.end()-1) == '\\')
        {
            while (true)
            {
                readLineInto(rawLine, file);
                lineNumber++;
                lineBuf.resize(lineBuf.size()-1); // cut off backslash from previous line
                lineBuf += rawLine;
                if (rawLine.empty() || *(rawLine.end()-1) != '\\')
                    break;
            }
        }

        // process the line
        const char *line = lineBuf.c_str();
        while (opp_isspace(*line)) line++;

        if (!strncmp(line, "#% old-wildcards", 16))
        {
            // in old ini files, "*" matched dots as well, like "**" does now;
            // we don't support this backward compatibility feature any longer
            throw cRuntimeError(ERRPREFIX "old-wildcards mode (#%% old-wildcards syntax) no longer supported", filename, lineNumber);
        }
        else if (!*line || *line=='#')
        {
            // blank or comment line, ignore
        }
        else if (*line==';')
        {
            // obsolete comment line
            throw cRuntimeError(ERRPREFIX "semicolon is no longer a comment start character, please use hashmark ('#')", filename, lineNumber);
        }
        else if (*line=='i' && strncmp(line, "include", 7)==0 && opp_isspace(line[7]))
        {
            // include directive
            const char *rest = line+7;
            const char *endPos = findEndContent(rest, filename, lineNumber);
            std::string includeFilename = trim(rest, endPos);

            // filenames should be relative to the current ini file we're processing,
            // so cd into its directory before opening included file
            PushDir d(directoryOf(filename).c_str());

            // process included inifile
            includedFiles.push_back(tmpfname);
            internalReadFile(includeFilename.c_str(), currentSectionIndex, includedFiles);
            includedFiles.pop_back();
        }
        else if (*line=='[')
        {
            // section heading
            const char *endPos = findEndContent(line, filename, lineNumber);
            if (*(endPos-1) != ']')
                throw cRuntimeError(ERRPREFIX "syntax error in section heading", filename, lineNumber);
            std::string sectionName = trim(line+1, endPos-1);
            if (sectionName.empty())
                throw cRuntimeError(ERRPREFIX "section name cannot be empty", filename, lineNumber);

            // section name must be unique within file (to reduce risk of copy/paste errors)
            if (sectionsInFile.find(sectionName) != sectionsInFile.end())
                throw cRuntimeError(ERRPREFIX "section name not unique within file", filename, lineNumber);
            sectionsInFile.insert(sectionName);

            // add section of not yet seen (in another file)
            currentSectionIndex = getOrCreateSection(sectionName.c_str());
        }
        else
        {
            // key = value
            if (currentSectionIndex == -1)
                currentSectionIndex = getOrCreateSection("General");
            const char *endPos = findEndContent(line, filename, lineNumber);
            const char *equalSignPos = strchr(line, '=');
            if (equalSignPos==NULL || equalSignPos > endPos)
                throw cRuntimeError(ERRPREFIX "line must be in the form key=value", filename, lineNumber);
            std::string key = trim(line, equalSignPos);
            std::string value = trim(equalSignPos+1, endPos);
            if (key.empty())
                throw cRuntimeError(ERRPREFIX "line must be in the form key=value", filename, lineNumber);

            sections[currentSectionIndex].entries.push_back(KeyValue1(basedirRef, filenameRef, lineNumber, key.c_str(), value.c_str()));
        }
    }

    fclose(file);
}

bool InifileReader::readLineInto(std::string& line, FILE *file)
{
    line = "";
    if (feof(file))
        return false;

    char buffer[512+1];
    while (fgets(buffer, 512, file)!=NULL)
    {
        const char *endBuffer = buffer + strlen(buffer);
        if (buffer==endBuffer) break; // should not happen
        bool eolReached = *(endBuffer-1)=='\n' || *(endBuffer-1)=='\r';
        if (eolReached)
            while (endBuffer>buffer && opp_isspace(*(endBuffer-1))) endBuffer--;
        line.append(buffer, endBuffer - buffer);
        if (eolReached)
            break;
    }
    if (ferror(file))
        throw cRuntimeError("Cannot read ini file: %s", strerror(errno));
    return true;
}

/**
 * Returns the position of the comment on the given line (i.e. the position of the
 * # character), or line.length() if no comment is found. string literals
 * are recognized and skipped properly.
 */
const char *InifileReader::findEndContent(const char *line, const char *filename, int lineNumber)
{
    const char *s = line;
    while (*s)
    {
        switch (*s) {
        case '"':
            // string literal: skip it
            s++;
            while (*s && *s!='"') {
                if (*s=='\\')  // skip \", \\, etc.
                    s++;
                s++;
            }
            if (!*s)
                throw cRuntimeError(ERRPREFIX "unterminated string constant", filename, lineNumber);
            s++;
            break;
        case '#':
            // comment; seek back to last non-space character
            while ((s-1)>line && opp_isspace(*(s-1)))
                s--;
            return s;
        default:
            s++;
        }
    }
    return s;
}

std::string InifileReader::trim(const char *start, const char *end)
{
    while (start<end && opp_isspace(*start)) start++;
    while ((end-1)>start && opp_isspace(*(end-1))) end--;
    return std::string(start, end-start);
}

void InifileReader::dump() const
{
    for (int i=0; i<getNumSections(); i++)
    {
        printf("\n[%s]\n", getSectionName(i));
        for (int j=0; j<getNumEntries(i); j++)
        {
            const KeyValue& e = getEntry(i, j);
            printf("%s = %s\t[basedir: %s]\n", e.getKey(), e.getValue(), e.getBaseDirectory());
        }
    }
}

NAMESPACE_END

