//==========================================================================
//  CINIFILE.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Implementation of
//    cIniFile     : ini file reader
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cenvir.h"
#include "util.h"  //strToSimtime
#include "cinifile.h"
#include "patternmatcher.h"
#include "cexception.h"
#include "fsutils.h"
#include "platdep/fileutil.h"  // directoryOf
#include "cstrtokenizer.h"


#define MAX_LINE   1024


cIniFile::cIniFile()
{
    warnings=false;
    notfound = false;

    fname = NULL;
}

cIniFile::~cIniFile()
{
    clearContents();
}

void cIniFile::initializeFrom(cConfiguration *)
{
}

#define SYNTAX_ERROR(txt) throw new cRuntimeError("Error reading `%s' line %d: %s",fname,lineno,txt);

void cIniFile::readFile(const char *filename)
{
    delete [] fname;
    fname = opp_strdup(filename);
    _readFile(fname,-1);
}

void cIniFile::_readFile(const char *fname, int section_id)
{
    // add this file to the 'files' vector
    files.push_back(sFile());
    int file_id = files.size()-1;
    files[file_id].fname = opp_strdup(tidyFilename(absolutePath(fname).c_str()).c_str());
    files[file_id].directory = opp_strdup(directoryOf(files[file_id].fname).c_str());

    // then open and read this file
    FILE *file = fopen(fname,"r");
    if (file==NULL)
        throw new cRuntimeError("Cannot open ini file `%s'", fname);

    int lineno = 0;

    const int bufsize = MAX_LINE+2; // +2 for CR (or LF) + EOS
    char buf[bufsize];
    buf[bufsize-1] = '\0'; // 'line too long' guard

    bool oldwildcardsmode = false; // if true, '*' matches anything (also a ".")
    while (!feof(file))
    {
        // start read a line
        int len=0;
        buf[0]=0; // fgets doesn't do this on eof!

        // join lines that end with backslash
        for(;;)
        {
            lineno++;
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

        // compatibility mode
        if (!strncmp(s,"#% old-wildcards",16))
        {
            ev.printf("\n*** Turned on old-wildcards mode for `%s'.\n\n", fname);
            oldwildcardsmode = true;
        }

        // skip empty and comment lines
        if (*s==0 || *s==';' || *s=='#')  continue;

        // process 'include' stuff
        if (memcmp(s,"include",7)==0 && (s[7]==' ' || s[7]=='\t'))
        {
            s+=7;
            while (*s==' ' || *s=='\t') s++;  // skip spaces after 'include'
            char *e=s;                        // find end of fname
            while (*e!=' ' && *e!='\t' && *e!='\0') e++;
            *e = '\0';

            // filenames should be relative to the current ini file we're processing,
            // so cd into its directory before opening included file
            PushDir d(directoryOf(fname).c_str());

            // process included inifile
            _readFile(s, section_id);
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
           unsigned int i;
           for (i=0; i<sections.size(); i++)
               if (strcmp(sections[i],s)==0)
                   break;
           if (i==sections.size()) {
               section_id = sections.size();
               sections.push_back(opp_strdup(s));
           } else {
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
           entries.push_back(sEntry());
           sEntry& entry = entries.back();
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
           entry.accessed = false;
           entry.keypattern = new PatternMatcher();
           entry.keypattern->setPattern(entry.key, !oldwildcardsmode, true, true);
           entry.file_id = file_id;
           entry.lineno = lineno;

           // // maybe already exists
           // for (i=0; i<entries.size(); i++)
           //   if (entries[i].section==sect && strcmp(entries[i].key,s)==0)
           //      break;
           // if (i==entries.size())
           // {
           //    entries[entries.size()].section = sect;   //fill in the entry
           //    entries[entries.size()].key = opp_strdup(s);
           //    entries[entries.size()].value = opp_strdup(e);
           //    entries[entries.size()++].accessed = 0;
           // }
           // else
           // {
           //    ev.printfmsg("Warning: `%s', line %d: duplicate entry %s=",fname,lineno,s);
           //    delete entries[i].value;
           //    entries[i].value = opp_strdup(e);
           // }
        }
    }
    fclose(file);
}
#undef SYNTAX_ERROR

void cIniFile::clearContents()
{
    unsigned int i;
    for (i=0; i<sections.size(); i++)
    {
       delete [] sections[i];
    }

    for (i=0; i<entries.size(); i++)
    {
       delete [] entries[i].key;
       delete entries[i].keypattern;
       delete [] entries[i].value;
       delete [] entries[i].rawvalue;
    }

    for (i=0; i<files.size(); i++)
    {
       delete [] files[i].fname;
       delete [] files[i].directory;
    }

    sections.clear();
    entries.clear();
    files.clear();

    delete [] fname;
    fname = NULL;
}

cIniFile::sEntry *cIniFile::_findEntry(const char *sect, const char *key)
{
    notfound=false;  // clear error flag
    unsigned int i;

    // search for section
    int section_id = -1;
    if (sect)
       for (i=0; i<sections.size() && section_id<0; i++)
          if (strcmp(sect,sections[i])==0)
              section_id = i;

    // search for entry
    for (i=0; i<entries.size(); i++)
    {
       if (!sect || entries[i].section_id==section_id)
       {
          if (strcmp(key,entries[i].key)==0)
          {
              entries[i].accessed = true;
              return &entries[i];
          }

          if (entries[i].keypattern->matches(key))
          {
              entries[i].accessed = true;
              return &entries[i];
          }
       }
    }

    // not found
    notfound=true;
    return NULL;
}

const char *cIniFile::_getValue(const char *sect, const char *key, bool raw)
{
    sEntry *entry = _findEntry(sect, key);
    if (!entry)
        return NULL;
    return (raw && entry->rawvalue) ? entry->rawvalue : entry->value;
}

int cIniFile::getNumSections()
{
    return sections.size();
}

const char *cIniFile::getSectionName(int k)
{
    if (k<0 || k>=(int)sections.size())
        return NULL;
    return sections[k];
}

bool cIniFile::exists(const char *sect, const char *key)
{
    return _getValue(sect, key, true)!=NULL;
}

bool cIniFile::getAsBool(const char *sect, const char *key, bool defaultval)
{
    const char *s = _getValue(sect, key, true);
    if (s==0 || *s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, %s used as default\n",
                     sect,key,defaultval?"true":"false");
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
                 sect,key,s);
       val = defaultval;
    }
    return val;
}

long cIniFile::getAsInt(const char *sect, const char *key, long defaultval)
{
    const char *s = _getValue(sect, key, true);
    if (s==0 || *s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, %ld used as default\n",
                     sect,key,defaultval);
       return defaultval;
    }

    long val;
    if (s[0]=='0' && s[1]=='x')
          sscanf(s+2,"%lx",&val);
    else
          sscanf(s,"%ld",&val);
    return val;
}

double cIniFile::getAsDouble(const char *sect, const char *key, double defaultval)
{
    const char *s = _getValue(sect, key, true);
    if (s==0 || *s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, %g used as default\n",
                     sect,key,defaultval);
       return defaultval;
    }

    double val;
    sscanf(s,"%lf",&val);
    return val;
}

const char *cIniFile::getAsString(const char *sect, const char *key, const char *defaultval)
{
    const char *s = _getValue(sect, key, false);
    if (s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, \"%s\" used as default\n",
                     sect,key,defaultval?defaultval:"");
       return defaultval;
    }

    return s;
}

double cIniFile::getAsTime(const char *sect, const char *key, double defaultval)
{
    const char *s = _getValue(sect, key, true);
    if (s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, %g used as default\n",
                     sect,key,defaultval);
       return defaultval;
    }

    return strToSimtime(s);
}

std::string cIniFile::getAsFilename(const char *sect, const char *key, const char *defaultval)
{
    sEntry *entry = _findEntry(sect, key);
    if (!entry)
    {
       if (!defaultval)
          defaultval = "";
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, \"%s\" used as default\n",
                     sect,key,defaultval);
       return defaultval;
    }

    if (!entry->value || !*entry->value)
       return "";

    const char *baseDir = files[entry->file_id].directory;
    return tidyFilename(concatDirAndFile(baseDir, entry->value).c_str());
}

std::string cIniFile::getAsFilenames(const char *sect, const char *key, const char *defaultval)
{
    sEntry *entry = _findEntry(sect, key);
    if (!entry)
    {
       if (!defaultval)
          defaultval = "";
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, \"%s\" used as default\n",
                     sect,key,defaultval);
       return defaultval;
    }

    if (!entry->value || !*entry->value)
       return "";

    const char *baseDir = files[entry->file_id].directory;

    // tokenize the string, and prepend each item with baseDir.
    // recognize and treat list files (beginning with '@' or '@@') specially.
    // if baseDir contains spaces, the filename will be surrounded with quotation marks.
    std::string result;
    cStringTokenizer tokenizer(entry->value);
    std::string quot = strchr(baseDir, ' ')!=NULL ? "\"" : "";
    const char *token;
    while ((token = tokenizer.nextToken())!=NULL)
    {
        if (token[0]=='@' && token[1]=='@')
        {
            result += quot + "@@" + tidyFilename(concatDirAndFile(baseDir, token+2).c_str()) + quot + " ";
        }
        else if (token[0]=='@')
        {
            result += quot + "@" + tidyFilename(concatDirAndFile(baseDir, token+1).c_str()) + quot + " ";
        }
        else
        {
            result += quot + tidyFilename(concatDirAndFile(baseDir, token).c_str()) + quot + " ";
        }
    }
    if (result.size()>0)
        result.erase(result.end()-1); // chop trailing space
    return result;
}

const char *cIniFile::getAsCustom(const char *sect, const char *key, const char *defaultval)
{
    const char *s = _getValue(sect, key, true);
    if (s==0)
    {
       if (warnings)
          ev.printf("Entry [%s]/%s= not in ini file, \"%s\" used as default\n",
                     sect,key,defaultval?defaultval:"");
       return defaultval;
    }

    return s;
}

const char *cIniFile::getBaseDirectoryFor(const char *section, const char *key)
{
    sEntry *entry = _findEntry(section, key);
    if (!entry)
        return NULL;
    return files[entry->file_id].directory;
}

std::string cIniFile::getLocation(const char *section, const char *key)
{
    sEntry *entry = _findEntry(section, key);
    if (!entry)
        return "";
    char buf[16];
    sprintf(buf, "%d", entry->lineno);
    return std::string(files[entry->file_id].fname) + ":" + buf;
}

bool cIniFile::notFound()
{
    bool e = notfound;
    notfound = false;
    return e;
}

//-----------

bool cIniFile::exists2(const char *sect1, const char *sect2, const char *key)
{
    return exists(sect1, key) || exists(sect2, key);
}

bool cIniFile::getAsBool2(const char *sect1, const char *sect2, const char *key, bool defaultval)
{
    bool w = warnings; warnings = false;
    bool a = getAsBool(sect1,key,defaultval);
    if (notfound)
         a = getAsBool(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %s used as default\n",
                   key,sect1,sect2,defaultval?"true":"false");
    return a;
}

long cIniFile::getAsInt2(const char *sect1, const char *sect2, const char *key, long defaultval)
{
    bool w = warnings; warnings = false;
    long a = getAsInt(sect1,key,defaultval);
    if (notfound)
         a = getAsInt(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %ld used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

double cIniFile::getAsDouble2(const char *sect1, const char *sect2, const char *key, double defaultval)
{
    bool w = warnings; warnings = false;
    double a = getAsDouble(sect1,key,defaultval);
    if (notfound)
         a = getAsDouble(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %g used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

const char *cIniFile::getAsString2(const char *sect1, const char *sect2, const char *key, const char *defaultval)
{
    bool w = warnings; warnings = false;
    const char *a = getAsString(sect1,key,defaultval);
    if (notfound)
         a = getAsString(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

std::string cIniFile::getAsFilename2(const char *sect1, const char *sect2, const char *key, const char *defaultval)
{
    bool w = warnings; warnings = false;
    std::string a = getAsFilename(sect1,key,defaultval);
    if (notfound)
         a = getAsFilename(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

std::string cIniFile::getAsFilenames2(const char *sect1, const char *sect2, const char *key, const char *defaultval)
{
    bool w = warnings; warnings = false;
    std::string a = getAsFilenames(sect1,key,defaultval);
    if (notfound)
         a = getAsFilenames(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

double cIniFile::getAsTime2(const char *sect1, const char *sect2, const char *key, double defaultval)
{
    bool w = warnings; warnings = false;
    double a = getAsTime(sect1,key,defaultval);
    if (notfound)
         a = getAsTime(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], %g used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

const char *cIniFile::getAsCustom2(const char *sect1, const char *sect2, const char *key, const char *defaultval)
{
    bool w = warnings; warnings = false;
    const char *a = getAsCustom(sect1,key,defaultval);
    if (notfound)
         a = getAsCustom(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Ini file entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

const char *cIniFile::getBaseDirectoryFor(const char *sect1, const char *sect2, const char *key)
{
    bool w = warnings; warnings = false;
    const char *a = getBaseDirectoryFor(sect1,key);
    if (notfound)
         a = getBaseDirectoryFor(sect2,key);
    warnings = w;
    return a;
}

std::string cIniFile::getLocation(const char *sect1, const char *sect2, const char *key)
{
    bool w = warnings; warnings = false;
    std::string a = getLocation(sect1,key);
    if (notfound)
         a = getLocation(sect2,key);
    warnings = w;
    return a;
}

const char *cIniFile::fileName() const
{
    return fname;
}

//-----------

std::vector<opp_string> cIniFile::getEntriesWithPrefix(const char *section, const char *keypart1, const char *keypart2)
{
    // For getting RNG mapping, this is called with:
    //    keypart1: concrete module fullPath, e.g. "net.rngCalc[0]"
    //    keypart2: ".rng-"
    // and this should match entries like:
    //    *.rngCalc[0].rng-0 = 1
    //    *.rngCalc[*].rng-0 = 2
    //    *.rngCalc[*].rng-1 = 3

    unsigned int i;

    // search for section
    int section_id = -1;
    if (section)
       for (i=0; i<sections.size() && section_id<0; i++)
          if (strcmp(section,sections[i])==0)
              section_id = i;

    // search through entries in that section
    std::string keyprefix = keypart1;
    keyprefix += keypart2;
    int keypart1len = strlen(keypart1);
    int prefixlen = strlen(keyprefix.c_str());

    std::vector<opp_string> result;
    for (i=0; i<entries.size(); i++)
    {
       if (!section || entries[i].section_id==section_id)
       {
          const char *rest = NULL;
          if (!strncmp(keyprefix.c_str(), entries[i].key, prefixlen))
              rest = entries[i].key+prefixlen;
          else
              rest = entries[i].keypattern->patternPrefixMatches(keyprefix.c_str(), keypart1len);
          if (rest)
          {
              // add to result if not already there
              bool found = false;
              for (int j=0; j<(int)result.size(); j+=2)
              {
                  if (!strcmp(rest, result[j].c_str()))
                  {
                      found = true;
                      break;
                  }
              }
              if (!found)
              {
                  result.push_back(opp_string(rest));
                  result.push_back(opp_string(entries[i].value));
              }
          }

       }
    }
    return result;
}

std::vector<opp_string> cIniFile::getEntriesWithPrefix(const char *section1, const char *section2, const char *keypart1, const char *keypart2)
{
    // get both sets
    std::vector<opp_string> res1 = getEntriesWithPrefix(section1, keypart1, keypart2);
    std::vector<opp_string> res2 = getEntriesWithPrefix(section2, keypart1, keypart2);

    // merge them: add those from res2 into res1 which are not already present
    for (unsigned int i=0; i<res2.size(); i+=2)
    {
        opp_string& key = res2[i];
        bool found = false;
        for (unsigned int j=0; j<res1.size(); j+=2)
        {
            if (!strcmp(key.c_str(), res1[j].c_str()))
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            res1.push_back(res2[i]);
            res1.push_back(res2[i+1]);
        }
    }
    return res1;
}

