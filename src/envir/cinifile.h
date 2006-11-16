//==========================================================================
//  CINIFILE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cIniFile
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CINIFILE_H
#define __CINIFILE_H


#include <stdlib.h> // atol, atof
#include <string.h> // strncpy
#include <string>
#include "defs.h"
#include "envdefs.h"
#include "cconfig.h"
#include "patternmatcher.h"


class cIniFile;
class cIniFileIterator;

/**
 * Ini file reader class.
 */
class ENVIR_API cIniFile : public cConfiguration
{
    friend class cIniFileIterator;
    friend class cIniFileSectionIterator;

  private:
    char *fname;                // file name

    struct sFile {
        char *fname;            // name of the file (absolute path)
        char *directory;        // directory (substring of fname)
    };

    struct sEntry {             // one entry contains:
        int section_id;         //  section it belongs to
        char *key;              //  key
        PatternMatcher *keypattern; // key as pattern
        char *value;            //  its value (without quotes)
        char *rawvalue;         //  original value with quotes, or NULL if it had no quotes
        int file_id;            //  file it was read from
        int lineno;             //  line number in file
        bool accessed;          //  has it been accessed?
    };

    std::vector<sFile> files;      // table of ini files
    std::vector<char *> sections;  // table of section names
    std::vector<sEntry> entries;   // table of entries

    sEntry *_findEntry(const char *section, const char *key);
    const char *_getValue(const char *section, const char *key, bool raw);
    void _readFile(const char *fname, int section_id);

    bool warnings;
    bool notfound;

  public:
    cIniFile();
    virtual ~cIniFile();

    /**
     * Redefined method from cConfiguration.
     */
    virtual void initializeFrom(cConfiguration *conf);

    /** @name Inifile-specific methods. Used only from "boot-time" code. */
    //@{
    void readFile(const char *fname);
    void setWarnings(bool warn) {warnings = warn;}
    void clearContents();
    //@}

    /** @name Redefined section query methods from cConfiguration */
    //@{
    virtual int getNumSections();
    virtual const char *getSectionName(int k);
    //@}

    /** @name Redefined getter methods from cConfiguration */
    //@{
    virtual bool exists(const char *section, const char *key);
    virtual bool getAsBool(const char *section, const char *key, bool defaultvalue=false);
    virtual long getAsInt(const char *section, const char *key, long defaultvalue=0);
    virtual double getAsDouble(const char *section, const char *key, double defaultvalue=0.0);
    virtual double getAsTime(const char *section, const char *key, double defaultvalue=0.0);
    virtual const char *getAsString(const char *section, const char *key, const char *defaultvalue=""); // quotes stripped (if any)
    virtual std::string getAsFilename(const char *section, const char *key, const char *defaultvalue="");
    virtual std::string getAsFilenames(const char *section, const char *key, const char *defaultvalue="");
    virtual const char *getAsCustom(const char *section, const char *key, const char *defaultvalue=NULL); // with quotes (if any)
    virtual const char *getBaseDirectoryFor(const char *section, const char *key);
    virtual std::string getLocation(const char *section, const char *key);
    virtual bool notFound();
    virtual std::vector<opp_string> getEntriesWithPrefix(const char *section, const char *keypart1, const char *keypart2);
    //@}

    /** @name Redefined two-section getter methods from cConfiguration */
    //@{
    virtual bool exists2(const char *section1, const char *section2, const char *key);
    virtual bool getAsBool2(const char *section1, const char *section2, const char *key, bool defaultvalue=false);
    virtual long getAsInt2(const char *section1, const char *section2, const char *key, long defaultvalue=0);
    virtual double getAsDouble2(const char *section1, const char *section2, const char *key, double defaultvalue=0.0);
    virtual double getAsTime2(const char *section1, const char *section2, const char *key, double defaultvalue=0.0);
    virtual const char *getAsString2(const char *section1, const char *section2, const char *key, const char *defaultvalue="");
    virtual std::string getAsFilename2(const char *section1, const char *section2, const char *key, const char *defaultvalue="");
    virtual std::string getAsFilenames2(const char *section1, const char *section2, const char *key, const char *defaultvalue="");
    virtual const char *getAsCustom2(const char *section1, const char *section2, const char *key, const char *defaultvalue="");
    virtual const char *getBaseDirectoryFor(const char *section1, const char *section2, const char *key);
    virtual std::string getLocation(const char *section1, const char *section2, const char *key);
    virtual std::vector<opp_string> getEntriesWithPrefix(const char *section1, const char *section2, const char *keypart1, const char *keypart2);
    //@}

    /**
     * Redefined method from cConfiguration.
     */
    virtual const char *fileName() const;
};

/**
 * Helper class, currently not used.
 */
class ENVIR_API cIniFileIterator
{
   private:
      cIniFile *ini;
      unsigned int idx;
   public:
      cIniFileIterator(cIniFile *i)  {ini=i; idx=0;}
      void reset()            {idx=0;}
      bool end()              {return (bool)(idx>=ini->entries.size());}
      void operator++(int)    {if (idx<ini->entries.size()) idx++;}
      const char *section()   {return ini->sections[ini->entries[idx].section_id];}
      const char *entry()     {return ini->entries[idx].key;}
      const char *value()     {return ini->entries[idx].value;}
      const char *baseDir()   {return ini->files[ini->entries[idx].file_id].directory;}
      const char *fileName()  {return ini->files[ini->entries[idx].file_id].fname;}
      int lineNumber()        {return ini->entries[idx].lineno;}
      bool accessed()         {return ini->entries[idx].accessed;}
};

#endif

