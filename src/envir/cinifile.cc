//==========================================================================
//  INIFILE.CPP - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//  Implementation of
//    cIniFile     : ini file reader
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cenvir.h"
#include "util.h"  //strToSimtime
#include "cinifile.h"
#include "patmatch.h"


#define MAX_LINE   1024

//==========================================================================
//=== cIniFile - member functions
//
cIniFile::cIniFile(const char *filename)
{
    sectiontable_size = 32;
    sections = new char*[sectiontable_size];
    num_sections = 0;

    entrytable_size = 128;
    entries = new sEntry[entrytable_size];
    num_entries = 0;

    warnings=false;
    _error = false;
    fname = opp_strdup(filename);

    readFile( filename );
}

cIniFile::~cIniFile()
{
    clearContents();
}

#define SYNTAX_ERROR(txt) \
          { _error=true;\
            ev.printfmsg("Error reading `%s', line %d: %s",fname,line,txt);\
            return; }

void cIniFile::readFile(const char *fname)
{
    _readFile(fname,-1);
}

void cIniFile::_readFile(const char *fname, int section_id)
{
    const int bufsize = MAX_LINE+2; // +2 for CR (or LF) + EOS
    char buf[bufsize];

    FILE *file;
    int line=0;
    int i;

    _error = false;
    buf[bufsize-1] = '\0'; // 'line too long' guard

    file = fopen(fname,"r");
    if (file==NULL)
    {
        _error=true;
        ev.printfmsg("Cannot open `%s'.",fname);
        return;
    }

    while (!feof(file))
    {
        // start read a line
        int len=0;
        buf[0]=0; // fgets doesn't do this on eof!

        // join lines that end with backslash
        for(;;)
        {
            line++;
            fgets(buf+len,bufsize-len,file);
            len+=strlen(buf+len);

            // if CR/LF is missing from line end: line too long or unterminated last line
            if (len>0 && buf[len-1]!='\n' && buf[len-1]!='\r' && len>=MAX_LINE)
                SYNTAX_ERROR("line too long, sorry");

            // chop CR/LF
            while (len>0 && (buf[len-1]=='\n' || buf[len-1]=='\r'))
                buf[--len]='\0';

            if (len>0 && buf[len-1]=='\\')
                buf[--len]='\0'; // chop trailing backslash
            else
                break; // exit loop if line does not end in backslash
        }

        // skip leading spaces, empty/comment lines
        char *s=buf;
        while (*s==' ' || *s=='\t') s++;
        if (*s==0 || *s==';' || *s=='#')  continue; // empty or comment line

        // process 'include' stuff
        if (memcmp(s,"include",7)==0 && (s[7]==' ' || s[7]=='\t'))
        {
            s+=7;
            while (*s==' ' || *s=='\t') s++;  // skip spaces after 'include'
            char *e=s;                        // find end of fname
            while (*e!=' ' && *e!='\t' && *e!='\0') e++;
            *e = '\0';

            _readFile( s, section_id );    // process included inifile
        }

        // process section heading '[new section]'
        else if (*s=='[')
        {
           // section header text into s:
           char *e=++s;
           while (*e!=']' && *e!=0) e++;      // find closing brace
           if (*e!=']') SYNTAX_ERROR("missing ']'");
           *e=0;
           if (!*s) SYNTAX_ERROR("section name should not be empty");

           // maybe already exists
           for (i=0; i<num_sections; i++)
               if (strcmp(sections[i],s)==0)
                   break;
           if (i==num_sections) {
               sections[section_id=num_sections++] = opp_strdup(s);
           }
           else {
               section_id = i;
           }
        }

        // process 'key = value' line
        else
        {
           if (section_id<0) SYNTAX_ERROR("no section header (e.g. [General]) seen yet");

           // s --> key, e --> value, e1 --> last char of value

           // find '=' sign
           char *e, *e1;
           e = strchr(s,'=');
           if (!e) SYNTAX_ERROR("line must have the form key=value");

           // chop trailing spaces from key
           char *s2 = e-1;
           while (s2>=s && (*s2==' ' || *s2=='\t')) s2--;
           *(s2+1) = 0;
           if (s2<s) SYNTAX_ERROR("line must have the form key=value");

           // skip '=' and spaces after it
           e++;
           while (*e==' ' || *e=='\t') e++;

           // do cosmetics on the value string
           if (*e=='"') {                // "string": keep quotes but chop off
             e1 = e+1;                   //           rest of line after it
             while (*e1 && *e1!='"') e1++;
             if (!*e1) SYNTAX_ERROR("no closing quote");
             *(e1+1)=0;    // keep closing quote
           }
           else {                               // chop off comment and
             e1 = e;                            //   trailing whitespace
             while (*e1 && *e1!=';' && *e1!='#') e1++;  // find EOL/comment start
             e1--;
             while (e<=e1 && (*e1==' '||*e1=='\t')) e1--; // go back along trailing whitespaces
             *(e1+1)=0;
           }

           // fill in the entry
           sEntry& entry = entries[num_entries++];
           entry.section_id = section_id;
           entry.key = opp_strdup(s);
           if (*e=='"') {
              entry.rawvalue = opp_strdup(e);
              *e1=0;
              entry.value = opp_strdup(e+1);
           } else {
              entry.rawvalue = NULL;
              entry.value = opp_strdup(e);
           }
           entry.accessed = 0;
           entry.haswildcard = strchr(entry.key,'*') || strchr(entry.key,'?') || strchr(entry.key,'{');

           // // maybe already exists
           // for (i=0; i<num_entries; i++)
           //   if (entries[i].section==sect && strcmp(entries[i].key,s)==0)
           //      break;
           // if (i==num_entries)
           // {
           //    entries[num_entries].section = sect;   //fill in the entry
           //    entries[num_entries].key = opp_strdup(s);
           //    entries[num_entries].value = opp_strdup(e);
           //    entries[num_entries++].accessed = 0;
           // }
           // else
           // {
           //    ev.printfmsg("Warning: `%s', line %d: duplicate entry %s=",fname,line,s);
           //    delete entries[i].value;
           //    entries[i].value = opp_strdup(e);
           // }
        }

        if (num_entries>=entrytable_size) {
           // reallocate entries[] array
           int newsize = 2*entrytable_size;
           sEntry *tmp = new sEntry[newsize];
           memcpy(tmp,entries,num_entries*sizeof(sEntry));
           delete [] entries;
           entries = tmp;
           entrytable_size = newsize;
        }

        if (num_sections>=sectiontable_size) {
           // reallocate sections[] array
           int newsize = 2*sectiontable_size;
           char **tmp = new char*[newsize];
           memcpy(tmp,sections,num_sections*sizeof(char*));
           delete [] sections;
           sections = tmp;
           sectiontable_size = newsize;
        }
    }
    fclose(file);
}
#undef SYNTAX_ERROR

void cIniFile::clearContents()
{
    int i;
    for (i=0; i<num_sections; i++)
       delete [] sections[i];
    for (i=0; i<num_entries; i++)
    {
       delete [] entries[i].key;
       delete [] entries[i].value;
       delete [] entries[i].rawvalue;
    }
    delete [] sections;
    delete [] entries;
    num_sections = num_entries = 0;

    delete fname;
    fname = NULL;
}

const char *cIniFile::_getValue(const char *sect, const char *ent, int raw)
{
    _error=false;  // clear error flag
    int i;

    // search for section
    int section_id = -1;
    if (sect)
       for(i=0; i<num_sections && section_id<0; i++)
          if (strcmp(sect,sections[i])==0)
              section_id = i;

    // search for entry
    for(i=0; i<num_entries; i++)
    {
       if (!sect || entries[i].section_id==section_id)
       {
          if (strcmp(ent,entries[i].key)==0)
          {
              entries[i].accessed=1;
              return (raw && entries[i].rawvalue) ? entries[i].rawvalue : entries[i].value;
          }

          if (entries[i].haswildcard)
          {
             // try pattern matching
             short pat[256];
             if (transform_pattern(entries[i].key,pat) && stringmatch(pat,ent))
             {
                 entries[i].accessed=1;
                 return (raw && entries[i].rawvalue) ? entries[i].rawvalue : entries[i].value;
             }
          }
       }
    }
    // not found
    _error=true;
    return NULL;
}

int cIniFile::error()
{
    int e = _error;
    _error = false;
    return e;
}

//-----------

int cIniFile::getNumSections()
{
    return num_sections;
}

const char *cIniFile::getSectionName(int k)
{
    if (k<0 || k>=num_sections)
        return NULL;
    return sections[k];
}

bool cIniFile::exists(const char *sect, const char *ent)
{
    return _getValue(sect, ent, 1)!=NULL;
}

const char *cIniFile::getRaw(const char *sect, const char *ent, const char *defaultval)
{
    const char *s = _getValue(sect, ent, 1);
    if (s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, \"%s\" used as default\n",
                     sect,ent,defaultval?defaultval:"");
       return defaultval;
    }

    return s;
}

bool cIniFile::getAsBool(const char *sect, const char *ent, bool defaultval)
{
    const char *s = _getValue(sect, ent, 1);
    if (s==0 || *s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, %s used as default\n",
                     sect,ent,defaultval?"true":"false");
       return defaultval;
    }

    char b[16]; int i;
    for (i=0; i<15 && s[i] && s[i]!=' ' && s[i]!='\t'; i++) b[i]=s[i];
    b[i]=0;

    bool val;
    if (strcmp(b,"yes")==0 || strcmp(b,"true")==0 || strcmp(b,"on")==0 || strcmp(b,"1")==0)
       val = 1;
    else if (strcmp(b,"no")==0 || strcmp(b,"false")==0 || strcmp(b,"off")==0 || strcmp(b,"0")==0)
       val = 0;
    else
    {
       ev.printf("Entry [%s]/%s=: `%s' is not a valid bool value, use true/false,on/off, yes/no or 0/1\n",
                 sect,ent,s);
       val = defaultval;
    }
    return val;
}

long cIniFile::getAsInt(const char *sect, const char *ent, long defaultval)
{
    const char *s = _getValue(sect, ent, 1);
    if (s==0 || *s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, %ld used as default\n",
                     sect,ent,defaultval);
       return defaultval;
    }

    long val;
    if (s[0]=='0' && s[1]=='x')
          sscanf(s+2,"%lx",&val);
    else
          sscanf(s,"%ld",&val);
    return val;
}

double cIniFile::getAsDouble(const char *sect, const char *ent, double defaultval)
{
    const char *s = _getValue(sect, ent, 1);
    if (s==0 || *s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, %g used as default\n",
                     sect,ent,defaultval);
       return defaultval;
    }

    double val;
    sscanf(s,"%lf",&val);
    return val;
}

const char *cIniFile::getAsString(const char *sect, const char *ent, const char *defaultval)
{
    const char *s = _getValue(sect, ent, 0);
    if (s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, \"%s\" used as default\n",
                     sect,ent,defaultval?defaultval:"");
       return defaultval;
    }

    return s;
}

double cIniFile::getAsTime(const char *sect, const char *ent, double defaultval)
{
    const char *s = _getValue(sect, ent, 1);
    if (s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, %g used as default\n",
                     sect,ent,defaultval);
       return defaultval;
    }

    return strToSimtime(s);
}

//-----------

bool cIniFile::exists2(const char *sect1, const char *sect2, const char *ent)
{
    return exists(sect1, ent) || exists(sect2, ent);
}

const char *cIniFile::getRaw2(const char *sect1, const char *sect2, const char *key, const char *defaultval)
{
    bool w = warnings; warnings = false;
    const char *a = getRaw(sect1,key,defaultval);
    if (_error)
         a = getRaw(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

bool cIniFile::getAsBool2(const char *sect1, const char *sect2, const char *key, bool defaultval)
{
    bool w = warnings; warnings = false;
    bool a = getAsBool(sect1,key,defaultval);
    if (_error)
         a = getAsBool(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %s used as default\n",
                   key,sect1,sect2,defaultval?"true":"false");
    return a;
}

long cIniFile::getAsInt2(const char *sect1, const char *sect2, const char *key, long defaultval)
{
    bool w = warnings; warnings = false;
    long a = getAsInt(sect1,key,defaultval);
    if (_error)
         a = getAsInt(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %ld used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

double cIniFile::getAsDouble2(const char *sect1, const char *sect2, const char *key, double defaultval)
{
    bool w = warnings; warnings = false;
    double a = getAsDouble(sect1,key,defaultval);
    if (_error)
         a = getAsDouble(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %g used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

const char *cIniFile::getAsString2(const char *sect1, const char *sect2, const char *key, const char *defaultval)
{
    bool w = warnings; warnings = false;
    const char *a = getAsString(sect1,key,defaultval);
    if (_error)
         a = getAsString(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

double cIniFile::getAsTime2(const char *sect1, const char *sect2, const char *key, double defaultval)
{
    bool w = warnings; warnings = false;
    double a = getAsTime(sect1,key,defaultval);
    if (_error)
         a = getAsTime(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %g used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

