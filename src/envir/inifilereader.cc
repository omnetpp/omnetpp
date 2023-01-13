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
#include "omnetpp/cexception.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/globals.h"
#include "inifilereader.h"
#include "fsutils.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

Register_GlobalConfigOption(CFGID_INIFILEREADER_FILENAME, "inifilereader-filename", CFG_FILENAME, nullptr, "If `configuration-reader-class=InifileReader`, it specifies the name of the ini file to be read.");

void InifileReader::read(cConfiguration *spec)
{
    std::string filename = spec->getAsFilename(CFGID_INIFILEREADER_FILENAME);
    readFile(filename.c_str());
}

void InifileReader::readFile(const char *filename)
{
    std::vector<std::string> includeFileStack;
    doReadFile(filename, includeFileStack);
}

void InifileReader::readText(const char *text, const char *filename, const char *baseDir)
{
    std::stringstream in(text);
    readStream(in, filename, baseDir);
}

void InifileReader::readStream(std::istream& in, const char *filename, const char *baseDir)
{
    std::string baseDirStr = baseDir ? std::string(baseDir) : getWorkingDir();

    std::vector<std::string> includeFileStack;
    doReadFromStream(in, filename, includeFileStack, std::string(filename), baseDirStr.c_str());
}

void InifileReader::doReadFile(const char *filename, std::vector<std::string>& includeFileStack)
{
    // create an entry for this file, checking against circular inclusion
    std::string absoluteFilename = tidyFilename(toAbsolutePath(filename).c_str(), true);
    std::string baseDir = directoryOf(absoluteFilename.c_str());

    if (contains(includeFileStack, absoluteFilename))
        throw cRuntimeError("Ini file '%s' includes itself, directly or indirectly", filename);

    // open and read this file
    std::ifstream in(filename, std::ios::in);
    if (!in.is_open())
        throw cRuntimeError("Cannot open ini file '%s'", filename);

    doReadFromStream(in, filename, includeFileStack, absoluteFilename, baseDir);
}

inline char firstNonwhitespaceChar(const char *s)
{
    for (; *s == ' ' || *s == '\t'; s++)
        /**/;
    return *s;
}

void InifileReader::doReadFromStream(std::istream& in, const char *filename, std::vector<std::string>& includeFileStack, const std::string& absoluteFilename, const std::string& baseDir)
{
    if (callback == nullptr)
        throw cRuntimeError("InifileReader: Callback not set");

    std::set<std::string> sectionsInFile;  // we'll check for uniqueness

    opp_staticpooledstring locationFilename = filename;
    opp_staticpooledstring locationNote;

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
            includeFileStack.push_back(absoluteFilename);
            doReadFile(includeFilename.c_str(), includeFileStack);
            includeFileStack.pop_back();
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
            if (contains(sectionsInFile, sectionName))
                throw cRuntimeError("Duplicate section '%s' within file, '%s' line %d", sectionName.c_str(), filename, lineNumber);
            sectionsInFile.insert(sectionName);

            locationNote = "section [" + sectionName + "]";

            callback->sectionHeader(sectionName.c_str(), FileLine(locationFilename, lineNumber, locationNote));
        }
        else {
            // key = value
            const char *endPos = findEndContent(line, filename, lineNumber);
            const char *equalSignPos = strchr(line, '=');
            if (equalSignPos == nullptr || equalSignPos > endPos)
                throw cRuntimeError("Line must be in the form key=value, '%s' line %d", filename, lineNumber);
            std::string key = trim(line, equalSignPos);
            std::string value = trim(equalSignPos+1, endPos);
            if (key.empty())
                throw cRuntimeError("Line must be in the form key=value, '%s' line %d", filename, lineNumber);

            callback->keyValue(key.c_str(), value.c_str(), baseDir.c_str(), FileLine(locationFilename, lineNumber, locationNote));
        }
    });

    if (in.bad())
        throw cRuntimeError("Cannot read ini file '%s'", filename);
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

}  // namespace envir
}  // namespace omnetpp

