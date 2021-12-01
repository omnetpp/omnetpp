//==========================================================================
//  INIFILEREADER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <set>
#include <algorithm>
#include <fstream>
#include "common/opp_ctype.h"
#include "common/fileutil.h"  // directoryOf
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "common/stringtokenizer.h"
#include "omnetpp/cexception.h"
#include "inifilereader.h"
#include "fsutils.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

void InifileReader::initializeFrom(cConfiguration *bootConfig)
{
    throw cRuntimeError("InifileReader: initializeFrom() method not supported");
}

const char *InifileReader::getFileName() const
{
    return rootFilename.c_str();
}

const char *InifileReader::getDefaultBaseDirectory() const
{
    return defaultBasedir.c_str();
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
    if (entryId < 0 || entryId >= (int)section.entries.size())
        throw cRuntimeError("InifileReader: Entry index %d out of bounds", entryId);
    return section.entries[entryId];
}

const InifileReader::Section& InifileReader::getSection(int sectionId) const
{
    if (sectionId < 0 || sectionId >= (int)sections.size())
        throw cRuntimeError("InifileReader: Section index %d out of bounds", sectionId);
    return sections[sectionId];
}

int InifileReader::getOrCreateSection(const char *sectionName)
{
    for (int i = 0; i < (int)sections.size(); i++)
        if (!strcmp(sections[i].name.c_str(), sectionName))
            return i;

    Section section;
    section.name = sectionName;
    sections.push_back(section);
    return sections.size()-1;
}

void InifileReader::readFile(const char *filename)
{
    rootFilename = filename;

    // use the first ini file's location as default base dir
    if (defaultBasedir.empty())
        defaultBasedir = directoryOf(rootFilename.c_str());

    std::vector<std::string> includedFiles;
    doReadFile(filename, -1, includedFiles);
}

inline char firstNonwhitespaceChar(const char *s) {
    for (; *s == ' ' || *s == '\t'; s++)
        /**/;
    return *s;
}

void InifileReader::doReadFile(const char *filename, int currentSectionIndex, std::vector<std::string>& includedFiles)
{
    // create an entry for this file, checking against circular inclusion
    std::string absoluteFilename = tidyFilename(toAbsolutePath(filename).c_str(), true);
    std::string baseDir = directoryOf(absoluteFilename.c_str());

    if (contains(includedFiles, absoluteFilename))
        throw cRuntimeError("Ini file '%s' includes itself, directly or indirectly", filename);

    // open and read this file
    std::ifstream in(filename, std::ios::in);
    if (!in.is_open())
        throw cRuntimeError("Cannot open ini file '%s'", filename);

    std::set<std::string> sectionsInFile;  // we'll check for uniqueness

    // open and read this file
    forEachJoinedLine(in, [&](std::string& lineBuf, int lineNumber, int numLines) {
        // remove comments (and catch unterminated string constants) inside multi-line lines
        const char *line = lineBuf.c_str();
        bool isMultiline = strchr(line, '\n') != nullptr;
        Assert(!opp_isblank(line));  // reader swallows blank lines
        Assert(!isMultiline || firstNonwhitespaceChar(line) != '#'); // reader never starts appending to comment lines

        if (isMultiline) {
            // strip out comments
            std::string tmp;
            int i = 0;
            for (std::string l : opp_split(line,  "\n")) {
                const char *endContent = findEndContent(l.c_str(), filename, lineNumber+i);  // also raises error for unterminated string constant
                tmp += l.substr(0, endContent - l.c_str()) + "\n";
                i++;
            }
            tmp.resize(tmp.size()-1);  // remove last newline
            lineBuf = tmp;
            line = lineBuf.c_str();
        }

        if (*line == ' ' || *line == '\t') {
            throw cRuntimeError("Content must begin on column 1 because indentation is used for line continuation, '%s' line %d", filename, lineNumber);
        }
        else if (opp_stringbeginswith(line, "#% old-wildcards")) {
            // in old ini files, "*" matched dots as well, like "**" does now;
            // we don't support this backward compatibility feature any longer
            throw cRuntimeError("Old-wildcards mode (#%% old-wildcards syntax) no longer supported, '%s' line %d", filename, lineNumber);
        }
        else if (*line == '#') {
            // blank or comment line, ignore
        }
        else if (*line == ';') {
            // obsolete comment line
            throw cRuntimeError("Use hash mark instead of semicolon to start comments, '%s' line %d", filename, lineNumber);
        }
        else if (opp_stringbeginswith(line, "include") && opp_isspace(line[7])) {
            // include directive
            const char *rest = line+7;
            const char *endPos = findEndContent(rest, filename, lineNumber);
            std::string includeFilename = trim(rest, endPos);

            // filenames should be relative to the current ini file we're processing,
            // so cd into its directory before opening included file
            PushDir d(directoryOf(filename).c_str());

            // process included inifile
            includedFiles.push_back(absoluteFilename);
            doReadFile(includeFilename.c_str(), currentSectionIndex, includedFiles);
            includedFiles.pop_back();
        }
        else if (*line == '[') {
            // section heading
            const char *endPos = findEndContent(line, filename, lineNumber);
            if (*(endPos-1) != ']')
                throw cRuntimeError("Syntax error in section heading, '%s' line %d", filename, lineNumber);
            std::string sectionName = opp_trim(opp_removestart(trim(line+1, endPos-1).c_str(), "Config ")); // "Config " prefix is optional, starting from OMNeT++ 6.0
            if (sectionName.empty())
                throw cRuntimeError("Section name cannot be empty, '%s' line %d", filename, lineNumber);

            // section name must be unique within file (to reduce risk of copy/paste errors)
            if (sectionsInFile.find(sectionName) != sectionsInFile.end())
                throw cRuntimeError("Duplicate section '%s' within file, '%s' line %d", sectionName.c_str(), filename, lineNumber);
            sectionsInFile.insert(sectionName);

            // add section of not yet seen (in another file)
            currentSectionIndex = getOrCreateSection(sectionName.c_str());
        }
        else {
            // key = value
            if (currentSectionIndex == -1)
                currentSectionIndex = getOrCreateSection("General");
            const char *endPos = findEndContent(line, filename, lineNumber);
            const char *equalSignPos = strchr(line, '=');
            if (equalSignPos == nullptr || equalSignPos > endPos)
                throw cRuntimeError("Line must be in the form key=value, '%s' line %d", filename, lineNumber);
            std::string key = trim(line, equalSignPos);
            std::string value = trim(equalSignPos+1, endPos);
            if (key.empty())
                throw cRuntimeError("Line must be in the form key=value, '%s' line %d", filename, lineNumber);

            sections[currentSectionIndex].entries.push_back(IniKeyValue(baseDir.c_str(), absoluteFilename.c_str(), lineNumber, key.c_str(), value.c_str()));
        }
    });

    if (in.bad())
        throw cRuntimeError("Cannot read ini file '%s'", filename);

    in.close();
}

void InifileReader::forEachJoinedLine(std::istream& in, const std::function<void(std::string&,int,int)>& processLine)
{
    // join continued lines, and call processLine() for each line
    std::string concatenatedLine = "";
    int startLineNumber = -1;
    int lineNumber = 0;
    while (true) {
        lineNumber++;
        std::string rawLine;
        if (!std::getline(in, rawLine))
            break;
        if (!rawLine.empty() && opp_iscntrl(rawLine.back()))
            rawLine.resize(rawLine.size()-1);  // remove trailing CR (for CRLF files on Linux))
        if (!concatenatedLine.empty() && concatenatedLine.back() == '\\') {
            // backslash continuation: basically delete the backslash+LF sequence
            concatenatedLine.resize(concatenatedLine.size()-1);  // remove backslash
            concatenatedLine += rawLine;
        }
        else if (!opp_isblank(concatenatedLine.c_str()) && firstNonwhitespaceChar(concatenatedLine.c_str()) != '#' && // only start appending to non-blank and NON-COMMENT (!) lines
                 !opp_isblank(rawLine.c_str()) && (rawLine[0] == ' ' || rawLine[0] == '\t')) // only append non-blank, indented lines
        {
                concatenatedLine += "\n" + rawLine;
        }
        else {
            if (startLineNumber != -1 && !opp_isblank(concatenatedLine.c_str())) {
                int numLines = lineNumber - startLineNumber;
                processLine(concatenatedLine, startLineNumber, numLines);
            }
            concatenatedLine = rawLine;
            startLineNumber = lineNumber;
        }
    }

    // last line
    if (startLineNumber != -1 && !opp_isblank(concatenatedLine.c_str())) {
        if (!concatenatedLine.empty() && concatenatedLine.back() == '\\')
            concatenatedLine.resize(concatenatedLine.size()-1);  // remove final stray backslash
        int numLines = lineNumber - startLineNumber;
        processLine(concatenatedLine, startLineNumber, numLines);
    }
}

/**
 * Returns the position of the comment on the given line (i.e. the position of the
 * # character), or line.length() if no comment is found. string literals
 * are recognized and skipped properly.
 */
const char *InifileReader::findEndContent(const char *line, const char *filename, int lineNumber)
{
    const char *s = line;
    while (*s) {
        switch (*s) {
            case '"':
                // string literal: skip it
                s++;
                while (*s && *s != '"') {
                    if (*s == '\\')  // skip \", \\, etc.
                        s++;
                    s++;
                }
                if (!*s)
                    throw cRuntimeError("Unterminated string constant, '%s' line %d", filename, lineNumber);
                s++;
                break;

            case '#':
                // comment; seek back to last non-space character
                while ((s-1) > line && opp_isspace(*(s-1)))
                    s--;
                return s;

            default:
                s++;
        }
    }
    return s;
}

void InifileReader::rtrim(std::string& str)
{
    int endPos = str.length()-1;
    while (endPos > 0 && opp_isspace(str[endPos]))
        endPos--;
    if (endPos != (int)str.length()-1)
        str = str.substr(0, endPos+1);
}

std::string InifileReader::trim(const char *start, const char *end)
{
    while (start < end && opp_isspace(*start))
        start++;
    while ((end-1) > start && opp_isspace(*(end-1)))
        end--;
    return std::string(start, end-start);
}

void InifileReader::dump() const
{
    for (int i = 0; i < getNumSections(); i++) {
        printf("\n[%s]\n", getSectionName(i));
        for (int j = 0; j < getNumEntries(i); j++) {
            const KeyValue& e = getEntry(i, j);
            printf("%s = %s\t[basedir: %s]\n", e.getKey(), e.getValue(), e.getBaseDirectory());
        }
    }
}

}  // namespace envir
}  // namespace omnetpp

