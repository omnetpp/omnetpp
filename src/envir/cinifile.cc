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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

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
cIniFile::cIniFile(char *fname)
{
    warnings=FALSE;
    num_sections = num_entries = 0;
    _error = FALSE;

    readFile( fname );
}

cIniFile::~cIniFile()
{
    clearContents();
}

#define SYNTAX_ERROR(txt) \
          { _error=TRUE;\
            ev.printfmsg("Error reading `%s', line %d: %s",fname,line,txt);\
            return; }

void cIniFile::readFile(char *fname)
{
    _readFile(fname,0);
}

void cIniFile::_readFile(char *fname, char *sect)
{
    FILE *file;
    char buf[MAX_LINE];
    int line=0;
    int i;

    _error = FALSE;

    file = fopen(fname,"r");
    if (file==NULL)
    {
        _error=TRUE;
        ev.printfmsg("Cannot open `%s'.",fname);
        return;
    }

    while (!feof(file))
    {
        // join lines that end with backslash
        int len=0;
        buf[0]=0; // fgets doesn't do this on eof!
        for(;;)
        {
            line++;
            fgets(buf+len,MAX_LINE-len,file);
            buf[MAX_LINE-1]=0;
            len+=strlen(buf+len);

            if (len>0 && buf[len-1]=='\n')
                buf[--len]='\0'; // chop LF
            if (len>0 && buf[len-1]=='\r')
                buf[--len]='\0'; // chop CR

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
            while (*e!=' ' && *e!='\t' && e!='\0') e++;
            *e = '\0';

            _readFile( s, sect );    // process included inifile
        }
        // process section heading '[new section]'
        else if (*s=='[')
        {
           // section header text into s:
           char *e=++s;
           while (*e!=']' && e!=0) e++;      // find closing brace
           if (*e!=']') SYNTAX_ERROR("no ']'");
           *e=0;

           // maybe already exists
           for (i=0; i<num_sections; i++)
               if (strcmp(section[i],s)==0)
                   break;
           if (i==num_sections)
               section[num_sections++] = sect = opp_strdup(s);
           else
               sect = section[i];
        }
        // process 'key = value' line
        else
        {
           if (!sect) SYNTAX_ERROR("no section header (like [Foo]) seen yet");

           // s --> key, e --> value, e1 --> last char of value

           // find '=' sign
           char *e, *e1;
           e = strchr(s,'=');
           if (!e) SYNTAX_ERROR("no '=' sign");

           // chop trailing spaces from key
           char *s2 = e-1;
           while (s2>=s && (*s2==' ' || *s2=='\t')) s2--;
           *(s2+1) = 0;
           if (s2<s) SYNTAX_ERROR("line begins with '='");

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
           entry[num_entries].section = sect;
           entry[num_entries].key = opp_strdup(s);
           if (*e=='"') {
              entry[num_entries].rawvalue = opp_strdup(e);
              *e1=0;
              entry[num_entries].value = opp_strdup(e+1);
           } else {
              entry[num_entries].rawvalue = NULL;
              entry[num_entries].value = opp_strdup(e);
           }
           entry[num_entries++].accessed = 0;

           // // maybe already exists
           // for (i=0; i<num_entries; i++)
           //   if (entry[i].section==sect && strcmp(entry[i].key,s)==0)
           //      break;
           // if (i==num_entries)
           // {
           //    entry[num_entries].section = sect;   //fill in the entry
           //    entry[num_entries].key = opp_strdup(s);
           //    entry[num_entries].value = opp_strdup(e);
           //    entry[num_entries++].accessed = 0;
           // }
           // else
           // {
           //    ev.printfmsg("Warning: `%s', line %d: duplicate entry %s=",fname,line,s);
           //    delete entry[i].value;
           //    entry[i].value = opp_strdup(e);
           // }
        }
        if (num_entries>=MAX_INI_ENTRIES || num_sections>=MAX_INI_SECTIONS)
        {
           _error=TRUE;               // file is too big
           ev.printfmsg("Ini file reader: Table full, change src/envir/cinifile.h");
           break;
        }
    }
    fclose(file);
}
#undef SYNTAX_ERROR

void cIniFile::clearContents()
{
    int i;
    for (i=0; i<num_sections; i++)
       delete section[i];
    for (i=0; i<num_entries; i++)
    {
       delete entry[i].key;
       delete entry[i].value;
       delete entry[i].rawvalue;
    }
    num_sections = num_entries = 0;
}

char *cIniFile::_getValue(char *sect, char *ent, int raw)
{
    _error=FALSE;  // clear error flag
    int i;

    // search for section
    char *sect_p = NULL;
    if (sect)
       for(i=0; i<num_sections && !sect_p; i++)
          if (strcmp(sect,section[i])==0)
              sect_p = section[i];

    // search for entry
    for(i=0; i<num_entries; i++)
    {
       if( !sect_p || entry[i].section==sect_p)
       {
          if (strcmp(ent,entry[i].key)==0)
          {
              entry[i].accessed=1;
              return (raw && entry[i].rawvalue) ? entry[i].rawvalue : entry[i].value;
          }

          if (strchr(entry[i].key,'*') ||
              strchr(entry[i].key,'?') ||
              strchr(entry[i].key,'{'))
          {
             // try pattern matching
             short pat[256];
             if (transform_pattern(entry[i].key,pat) && stringmatch(pat,ent))
             {
                 entry[i].accessed=1;
                 return (raw && entry[i].rawvalue) ? entry[i].rawvalue : entry[i].value;
             }
          }
       }
    }
    // not found
    _error=TRUE;
    return NULL;
}

int cIniFile::error()
{
    int e = _error;
    _error = FALSE;
    return e;
}

//-----------

char *cIniFile::getRaw(char *sect, char *ent, char *defaultval)
{
    char *s = _getValue(sect, ent, 1);
    if (s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, \"%s\" used as default\n",
                     sect,ent,defaultval?defaultval:"");
       return defaultval;
    }

    return s;
}

bool cIniFile::getAsBool(char *sect, char *ent, bool defaultval)
{
    char *s = _getValue(sect, ent, 1);
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

long cIniFile::getAsInt(char *sect, char *ent, long defaultval)
{
    char *s = _getValue(sect, ent, 1);
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

double cIniFile::getAsDouble(char *sect, char *ent, double defaultval)
{
    char *s = _getValue(sect, ent, 1);
    if (s==0 || *s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, %lg used as default\n",
                     sect,ent,defaultval);
       return defaultval;
    }

    double val;
    sscanf(s,"%lf",&val);
    return val;
}

char *cIniFile::getAsString(char *sect, char *ent, char *defaultval)
{
    char *s = _getValue(sect, ent, 0);
    if (s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, \"%s\" used as default\n",
                     sect,ent,defaultval?defaultval:"");
       return defaultval;
    }

    return s;
}

double cIniFile::getAsTime(char *sect, char *ent, double defaultval)
{
    char *s = _getValue(sect, ent, 1);
    if (s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, %lg used as default\n",
                     sect,ent,defaultval);
       return defaultval;
    }

    return strToSimtime(s);
}

//-----------

char *cIniFile::getRaw2(char *sect1, char *sect2, char *key, char *defaultval)
{
    bool w = warnings; warnings = FALSE;
    char *a = getRaw(sect1,key,defaultval);
    if (_error)
         a = getRaw(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

bool cIniFile::getAsBool2(char *sect1, char *sect2, char *key, bool defaultval)
{
    bool w = warnings; warnings = FALSE;
    bool a = getAsBool(sect1,key,defaultval);
    if (_error)
         a = getAsBool(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %s used as default\n",
                   key,sect1,sect2,defaultval?"true":"false");
    return a;
}

long cIniFile::getAsInt2(char *sect1, char *sect2, char *key, long defaultval)
{
    bool w = warnings; warnings = FALSE;
    long a = getAsInt(sect1,key,defaultval);
    if (_error)
         a = getAsInt(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %ld used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

double cIniFile::getAsDouble2(char *sect1, char *sect2, char *key, double defaultval)
{
    bool w = warnings; warnings = FALSE;
    double a = getAsDouble(sect1,key,defaultval);
    if (_error)
         a = getAsDouble(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %lg used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

char *cIniFile::getAsString2(char *sect1, char *sect2, char *key, char *defaultval)
{
    bool w = warnings; warnings = FALSE;
    char *a = getAsString(sect1,key,defaultval);
    if (_error)
         a = getAsString(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

double cIniFile::getAsTime2(char *sect1, char *sect2, char *key, double defaultval)
{
    bool w = warnings; warnings = FALSE;
    double a = getAsTime(sect1,key,defaultval);
    if (_error)
         a = getAsTime(sect2,key,defaultval);
    warnings = w;
    if (_error && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %lg used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

