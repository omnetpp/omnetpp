//==========================================================================
//  INIFILEREADER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
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
#include "omnetpp/cconfigreader.h"
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
    class KeyValue1 : public KeyValue {
      private:
        const std::string *basedir;  // points into basedirs[]
        const std::string *filename; // points into filenames[]
        int lineNumber;
        std::string key;
        std::string value; //XXX stringpool it?

      public:
        KeyValue1(const std::string *bdir, const std::string *fname, int li, const char *k, const char *v) {
            basedir = bdir; filename = fname; lineNumber = li; key = k; value = v;
        }

        // virtual functions implementing the KeyValue interface
        virtual const char *getKey() const override   {return key.c_str();}
        virtual const char *getValue() const override {return value.c_str();}
        virtual const char *getBaseDirectory() const override  {return basedir->c_str();}
        virtual const char *getFileName() const override  {return filename->c_str();}
        virtual int getLineNumber() const override  {return lineNumber;}
    };

    std::string rootFilename;  // name of "root" ini file read
    std::string defaultBasedir; // the default base directory

    typedef std::set<std::string> StringSet;
    StringSet filenames; // stores ini file names (absolute paths)
    StringSet basedirs;  // stores base directory names (absolute paths)

    struct Section {
        std::string name;
        std::vector<KeyValue1> entries;
    };
    std::vector<Section> sections;

  protected:
    void internalReadFile(const char *filename, int currentSectionIndex, std::vector<std::string> &includedFiles);
    const Section& getSection(int sectionId) const;
    int getOrCreateSection(const char *sectionName); // returns index into sections[]
    static const char *findEndContent(const char *line, const char *filename, int lineNumber);
    static std::string trim(const char *start, const char *end);
    static void rtrim(std::string& str);

  public:
    InifileReader();
    virtual ~InifileReader();
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

} // namespace envir
}  // namespace omnetpp


#endif


