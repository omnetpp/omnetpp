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
  Copyright (C) 1992,99 Andras Varga
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

#define MAX_INI_SECTIONS 110
#define MAX_INI_ENTRIES  1000   // increase if necessary

class ENVIR_API cIniFile
{
  friend class cIniFileIterator;
  friend class cIniFileSectionIterator;

  private:
    char *fname;
    struct sEntry {             //one entry contains:
        char *section;          //  name of section it belongs to
        char *key;              //  the parameter
        char *value;            //  its value
        char *rawvalue;         //  if original was quoted, this is the
                                //        strdupped version of it, otherwise 0
        bool accessed;          //  has it been accessed?
    };
    int num_sections;           //total number of sections
    char *section[MAX_INI_SECTIONS]; //array of sectionnames
    int num_entries;                 //total number of entries
    sEntry entry[MAX_INI_ENTRIES];   //array of entries
    int _error;   //OK=0, ERROR=1

    const char *_getValue(const char *section, const char *key,int raw);
    void _readFile(const char *fname, const char *sectionptr);
  public:

    cIniFile(const char *fname);
    ~cIniFile();

    void readFile(const char *fname);
    void clearContents();

    const char *filename() {return fname;}

    int error();    // TRUE if there was an error
    bool warnings;  // enable/disable warnings

    // get an entry from [section]
    const char *getRaw(const char *section, const char *key, const char *defaultval=NULL); // with quotes (if any)
    bool getAsBool(const char *section, const char *key, bool defaultval=FALSE);
    long getAsInt(const char *section, const char *key, long defaultval=0);
    double getAsDouble(const char *section, const char *key, double defaultval=0.0);
    const char *getAsString(const char *section, const char *key, const char *defaultval=""); // quotes stripped (if any)
    double getAsTime(const char *sect, const char *key, double defaultval=0.0);

    // get an entry from [sect1] or if it isn't there, from [sect2]
    const char *getRaw2(const char *sect1, const char *sect2, const char *key, const char *defaultval=NULL);
    bool getAsBool2(const char *sect1, const char *sect2, const char *key, bool defaultval=FALSE);
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
      const char *section()   {return ini->entry[idx].section;}
      const char *entry()     {return ini->entry[idx].key;}
      const char *value()     {return ini->entry[idx].value;}
      bool accessed()         {return ini->entry[idx].accessed;}
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
      const char *section()   {return ini->section[idx];}
};

#endif

