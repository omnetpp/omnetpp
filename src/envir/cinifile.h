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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CINIFILE_H
#define __CINIFILE_H


#include <stdlib.h> // atol, atof
#include <string.h> // strncpy
#include "defs.h"
#include "envdefs.h"


class cIniFile;
class cIniFileIterator;

//=====================================================================
//=== cIniFile

class ENVIR_API cIniFile
{
  friend class cIniFileIterator;
  friend class cIniFileSectionIterator;

  private:
    char *fname;                // file name

    struct sEntry {             // one entry contains:
        int section_id;         //  section it belongs to
        char *key;              //  the parameter
        char *value;            //  its value
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

    int _error;                 // OK=0, ERROR=1

    const char *_getValue(const char *section, const char *key,int raw);
    void _readFile(const char *fname, int section_id);

  public:
    cIniFile(const char *fname);
    ~cIniFile();

    void readFile(const char *fname);
    void clearContents();

    const char *filename() {return fname;}

    int error();    // true if there was an error
    bool warnings;  // enable/disable warnings

    // get an entry from [section]
    bool exists(const char *section, const char *key);
    const char *getRaw(const char *section, const char *key, const char *defaultval=NULL); // with quotes (if any)
    bool getAsBool(const char *section, const char *key, bool defaultval=false);
    long getAsInt(const char *section, const char *key, long defaultval=0);
    double getAsDouble(const char *section, const char *key, double defaultval=0.0);
    const char *getAsString(const char *section, const char *key, const char *defaultval=""); // quotes stripped (if any)
    double getAsTime(const char *sect, const char *key, double defaultval=0.0);

    // get an entry from [sect1] or if it isn't there, from [sect2]
    bool exists2(const char *sect1, const char *sect2, const char *key);
    const char *getRaw2(const char *sect1, const char *sect2, const char *key, const char *defaultval=NULL);
    bool getAsBool2(const char *sect1, const char *sect2, const char *key, bool defaultval=false);
    long getAsInt2(const char *sect1, const char *sect2, const char *key, long defaultval=0);
    double getAsDouble2(const char *sect1, const char *sect2, const char *key, double defaultval=0.0);
    const char *getAsString2(const char *sect1, const char *sect2, const char *key, const char *defaultval="");
    double getAsTime2(const char *sect1, const char *sect2, const char *key, double defaultval=0.0);

};

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

class ENVIR_API cIniFileSectionIterator
{
   private:
      cIniFile *ini;
      int idx;
   public:
      cIniFileSectionIterator(cIniFile *i) {ini=i; idx=0;}
      void reset()            {idx=0;}
      bool end()              {return (bool)(idx>=ini->num_sections);}
      void operator++(int)    {if (idx<ini->num_sections) idx++;}
      const char *section()   {return ini->sections[idx];}
};

#endif

