//==========================================================================
//  INIFILEREADER.H - part of
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

#ifndef __OMNETPP_ENVIR_INIFILEREADER_H
#define __OMNETPP_ENVIR_INIFILEREADER_H

#include <string>
#include <iostream>
#include <functional>
#include "omnetpp/cconfigurationreader.h"
#include "envirdefs.h"

namespace omnetpp {
namespace envir {

/**
 * Low-level inifile reading, including the resolution of includes.
 * Inifile contents is presented as key-value pairs grouped into sections.
 * This class does not try to make sense of section/key names.
 */
class ENVIR_API InifileReader : public cConfigurationReader
{
  protected:
    void doReadFile(const char *filename, int currentSectionIndex, std::vector<std::string>& includedFiles);
    void doReadFromStream(std::istream& in, const char *filename, int currentSectionIndex, std::vector<std::string>& includedFiles, const std::string& absoluteFilename, const std::string& baseDir);
    void forEachJoinedLine(std::istream& in, const std::function<void(std::string&,int,int)>& processLine);
    static const char *findEndContent(const char *line, const char *filename, int lineNumber);
    static std::string trim(const char *start, const char *end);
    static void rtrim(std::string& str);

  public:
    InifileReader() {}
    InifileReader(Callback *callback) : cConfigurationReader(callback) {}
    virtual void read(cConfiguration *spec);
    virtual void readFile(const char *filename);
    virtual void readText(const char *text, const char *filename, const char *baseDir);
    virtual void readStream(std::istream& in, const char *filename, const char *baseDir);
};

}  // namespace envir
}  // namespace omnetpp

#endif


