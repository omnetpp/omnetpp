//==========================================================================
//  INIFILEREADER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef INIFILEREADER_H_
#define INIFILEREADER_H_

#include <string>
#include <vector>
#include "envdefs.h"
#include "cconfigreader.h"

NAMESPACE_BEGIN

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
        virtual const char *getKey() const   {return key.c_str();}
        virtual const char *getValue() const {return value.c_str();}
        virtual const char *getBaseDirectory() const  {return basedir->c_str();}
        virtual const char *getFileName() const  {return filename->c_str();}
        virtual int getLineNumber() const  {return lineNumber;}
    };

    std::string rootfilename;  // name of "root" ini file read
    std::string defaultbasedir; // the default base directory

    typedef std::set<std::string> StringSet;
    StringSet filenames; // stores ini file names (absolute paths)
    StringSet basedirs;  // stores base directory names (absolute paths)

    struct Section {
        std::string name;
        std::vector<KeyValue1> entries;
    };
    std::vector<Section> sections;

  protected:
    void internalReadFile(const char *filename);
    const Section& getSection(int sectionId) const;
    static const char *findEndContent(const char *line, const char *filename, int lineNumber);
    static bool readLineInto(std::string& line, FILE *file);
    static std::string trim(const char *start, const char *end);

  public:
    InifileReader();
    virtual ~InifileReader();
    virtual void readFile(const char *filename);

    /** @name Methods implementing cConfigurationReader */
    //@{
    virtual const char *getFileName() const;
    virtual const char *getDefaultBaseDirectory() const;
    virtual int getNumSections() const;
    virtual const char *getSectionName(int sectionId) const;
    virtual int getNumEntries(int sectionId) const;
    virtual const KeyValue& getEntry(int sectionId, int entryId) const;
    virtual void dump() const;
    //@}
};

NAMESPACE_END


#endif


