//==========================================================================
//   CINIFILE.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cIniFile
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CINIFILE_H
#define __CINIFILE_H


#include <stdlib.h> // atol, atof
#include <string.h> // strncpy
#include "defs.h"
#include "envdefs.h"
#include "cconfig.h"
#include "patmatch.h"


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

    struct sEntry {             // one entry contains:
        int section_id;         //  section it belongs to
        char *key;              //  key
        cPatternMatcher *keypattern; // key as pattern
        char *value;            //  its value (without quotes)
        char *rawvalue;         //  if original was quoted, this is the
                                //        strdupped version of it, otherwise NULL
        bool accessed;          //  has it been accessed?
    };

    char **sections;            // table of section names
    int sectiontable_size;      // size of section table allocated
    int num_sections;           // number of sections used

    sEntry *entries;            // table of entries
    int entrytable_size;        // size of entry table allocated
    int num_entries;            // number of entries used

    const char *_getValue(const char *section, const char *key, bool raw);
    void _readFile(const char *fname, int section_id);
    void clearContents();

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
    virtual const char *getAsCustom(const char *section, const char *key, const char *defaultvalue=NULL); // with quotes (if any)
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
    virtual const char *getAsCustom2(const char *section1, const char *section2, const char *key, const char *defaultvalue="");
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
      int idx;
   public:
      cIniFileIterator(cIniFile *i)  {ini=i; idx=0;}
      void reset()            {idx=0;}
      bool end()              {return (bool)(idx>=ini->num_entries);}
      void operator++(int)    {if (idx<ini->num_entries) idx++;}
      const char *section()   {return ini->sections[ini->entries[idx].section_id];}
      const char *entry()     {return ini->entries[idx].key;}
      const char *value()     {return ini->entries[idx].value;}
      bool accessed()         {return ini->entries[idx].accessed;}
};

#endif

