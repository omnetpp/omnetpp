//==========================================================================
//   CINIFILE.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cInifile
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


class cIniFile;
class cIniFileIterator;

//=====================================================================
//=== cIniFile

#define MAX_INI_SECTIONS 110
#define MAX_INI_ENTRIES  1000   // increase if necessary

class cIniFile
{
  friend class cIniFileIterator;
  friend class cIniFileSectionIterator;

  private:
    struct sEntry {             //one entry contains:
        char *section;          //  name of section it belongs to
        char *key;              //  the parameter
        char *value;            //  its value
        char *rawvalue;         //  if original was quoted, this is the
                                //        strdupped version of it, otherwise 0
        int accessed;           //  has it been accessed?
    };
    int num_sections;           //total number of sections
    char *section[MAX_INI_SECTIONS]; //array of sectionnames
    int num_entries;                 //total number of entries
    sEntry entry[MAX_INI_ENTRIES];   //array of entries
    int _error;   //OK=0, ERROR=1

    char *_getValue(char *section, char *key,int raw);
    void _readFile(char *fname, char *sectionptr);
  public:

    cIniFile(char *fname);
    ~cIniFile();

    void readFile(char *fname);
    void clearContents();

    int error();    // TRUE if there was an error
    bool warnings;  // enable/disable warnings

    // get an entry from [section]
    char *getRaw(char *section, char *key, char *defaultval=NULL); // with quotes (if any)
    bool getAsBool(char *section, char *key, bool defaultval=FALSE);
    long getAsInt(char *section, char *key, long defaultval=0);
    double getAsDouble(char *section, char *key, double defaultval=0.0);
    char *getAsString(char *section, char *key, char *defaultval=""); // quotes stripped (if any)
    double getAsTime(char *sect, char *key, double defaultval=0.0);

    // get an entry from [sect1] or if it isn't there, from [sect2]
    char *getRaw2(char *sect1, char *sect2, char *key, char *defaultval=NULL);
    bool getAsBool2(char *sect1, char *sect2, char *key, bool defaultval=FALSE);
    long getAsInt2(char *sect1, char *sect2, char *key, long defaultval=0);
    double getAsDouble2(char *sect1, char *sect2, char *key, double defaultval=0.0);
    char *getAsString2(char *sect1, char *sect2, char *key, char *defaultval="");
    double getAsTime2(char *sect1, char *sect2, char *key, double defaultval=0.0);

};

class cIniFileIterator
{
   private:
      cIniFile *ini;
      int idx;
   public:
      cIniFileIterator(cIniFile *i)  {ini=i; idx=0;}
      void reset()                   {idx=0;}
      bool end()                     {return (bool)(idx>=ini->num_entries);}
      void operator++(int)           {if (idx<ini->num_entries) idx++;}
      char *section()                {return ini->entry[idx].section;}
      char *entry()                  {return ini->entry[idx].key;}
      char *value()                  {return ini->entry[idx].value;}
      bool accessed()                {return ini->entry[idx].accessed;}
};

class cIniFileSectionIterator
{
   private:
      cIniFile *ini;
      int idx;
   public:
      cIniFileSectionIterator(cIniFile *i)  {ini=i; idx=0;}
      void reset()                   {idx=0;}
      bool end()                     {return (bool)(idx>=ini->num_sections);}
      void operator++(int)           {if (idx<ini->num_sections) idx++;}
      char *section()                {return ini->section[idx];}
};

#endif
