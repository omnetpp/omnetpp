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
#include <set>
#include <vector>
#include <iostream>
#include <functional>
#include "omnetpp/cconfigreader.h"
#include "envirdefs.h"
#include "common/pooledstring.h"

namespace omnetpp {
namespace envir {

using common::opp_staticpooledstring;

/**
 * Low-level inifile reading, including the resolution of includes.
 * Inifile contents is presented as key-value pairs grouped into sections.
 * This class does not try to make sense of section/key names.
 */
class ENVIR_API InifileReader : public cConfigurationReader
{
  protected:
    class IniKeyValue : public KeyValue {
      private:
        opp_staticpooledstring baseDir;
        FileLine loc;
        std::string key;
        std::string value;

      public:
        IniKeyValue(const char *baseDir, const char *fileName, int line, const char *key, const char *value) :
            baseDir(baseDir), loc(fileName,line), key(key), value(value) {}

        // virtual functions implementing the KeyValue interface
        virtual const char *getKey() const override   {return key.c_str();}
        virtual const char *getValue() const override {return value.c_str();}
        virtual const char *getBaseDirectory() const override  {return baseDir.c_str();}
        virtual FileLine getSourceLocation() const override {return loc;}
    };

    std::string rootFilename;  // name of "root" ini file read
    std::string defaultBasedir; // the default base directory

    struct Section {
        std::string name;
        std::vector<IniKeyValue> entries;
    };
    std::vector<Section> sections;

  protected:
    void doReadFile(const char *filename, int currentSectionIndex, std::vector<std::string> &includedFiles);
    void forEachJoinedLine(std::istream& in, const std::function<void(std::string&,int,int)>& processLine);
    const Section& getSection(int sectionId) const;
    int getOrCreateSection(const char *sectionName); // returns index into sections[]
    static const char *findEndContent(const char *line, const char *filename, int lineNumber);
    static std::string trim(const char *start, const char *end);
    static void rtrim(std::string& str);

  public:
    InifileReader() {}
    virtual ~InifileReader() {}
    virtual void readFile(const char *filename);

    /** @name Methods implementing cConfigurationReader */
    //@{
    virtual void initializeFrom(cConfiguration *bootConfig) override;
    virtual const char *getFileName() const override;
    virtual const char *getDefaultBaseDirectory() const override;
    virtual int getNumSections() const override;
    virtual const char *getSectionName(int sectionId) const override;
    virtual int getNumEntries(int sectionId) const override;
    virtual const KeyValue& getEntry(int sectionId, int entryId) const override;
    virtual void dump() const override;
    //@}
};

}  // namespace envir
}  // namespace omnetpp


#endif


