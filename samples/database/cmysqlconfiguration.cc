//==========================================================================
//  CMYSQLCONFIG.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
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
#include "cmysqlconfiguration.h"
#include "platdep/fileutil.h"  // directoryOf, tidyFilename
#include "patternmatcher.h" // TODO eliminate dependency
#include "cinifile.h" // for dumpConfig(); TODO eliminate dependency
#include "oppmysqlutils.h"


Register_Class(cMySQLConfiguration);


cMySQLConfiguration::cMySQLConfiguration()
{
    warnings = false;
    notfound = false;
}

cMySQLConfiguration::~cMySQLConfiguration()
{
    clearContents();
}

void cMySQLConfiguration::initializeFrom(cConfiguration *cfg)
{
    // utility: dump existing configuration as SQL INSERT statements
    if (cfg->getAsBool("General", "mysqlconfiguration-dumpbootcfg", false))
        dumpConfig(cfg);

    // connect
    const char *prefix = cfg->getAsString("General", "mysqlconfiguration-connectprefix", NULL);
    ev << className() << " connecting to MySQL database";
    if (prefix && prefix[0]) ev << " using " << prefix << "-* config entries";
    ev << "...";
    MYSQL *mysql = mysql_init(NULL);
    opp_mysql_connectToDB(mysql, cfg, prefix);
    ev << " OK\n";

    // read the contents, then close the database
    copyExistingConfig(cfg);
    const char *configName = cfg->getAsString("General", "mysqlconfiguration-configname", NULL);
    if (!configName || !configName[0])
        throw new cRuntimeError("cMySQLConfiguration: config entry mysqlconfiguration-configname= must be given");
    readDB(mysql, configName);
    mysql_close(mysql);
}

void cMySQLConfiguration::dumpConfig(cConfiguration *cfg)
{
    ev.printf("INSERT INTO config (id,name) VALUES (10,\"someconfig\");\n");

    const int base = 1000;
    for (int i=0; i<cfg->getNumSections(); i++)
    {
        ev.printf("INSERT INTO configsection (configid,id,name) VALUES (10,%d,\"%s\");\n",
                  base+i, cfg->getSectionName(i));
    }

    cIniFile *ini = dynamic_cast<cIniFile *>(cfg);
    if (!ini)
        throw new cRuntimeError("cMySQLConfiguration: boot-time config is not a cIniFile");

    int k = 0;
    for (cIniFileIterator it(ini); !it.end(); it++)
    {
        int secnum = getSectionId(cfg, it.section());
        ev.printf("INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (%d,\"%s\",\"%s\",%d);\n",
                  base+secnum, it.entry(), it.value(), ++k);
    }
}

int cMySQLConfiguration::getSectionId(cConfiguration *cfg, const char *sectionName)
{
    for (int i=0; i<cfg->getNumSections(); i++)
        if (strcmp(sectionName, cfg->getSectionName(i))==0)
            return i;
    return -1;
}

int cMySQLConfiguration::getFileId(const char *fileName)
{
    for (int i=0; i<(int)files.size(); i++)
        if (strcmp(fileName, files[i].fname)==0)
            return i;
    return -1;
}

void cMySQLConfiguration::copyExistingConfig(cConfiguration *cfg)
{
    std::map<int,int> sectionold2new;
    for (int i=0; i<cfg->getNumSections(); i++)
    {
        sections.push_back(opp_strdup(cfg->getSectionName(i)));
        sectionold2new[i] = sections.size()-1;
    }

    cIniFile *ini = dynamic_cast<cIniFile *>(cfg);
    if (!ini)
        throw new cRuntimeError("cMySQLConfiguration: boot-time config is not a cIniFile");

    for (cIniFileIterator it(ini); !it.end(); it++)
    {
        int secnum = getSectionId(cfg, it.section());
        ASSERT(secnum!=-1 && sectionold2new.find(secnum)!=sectionold2new.end());

        int fileId = getFileId(it.fileName());
        if (fileId==-1) {
            files.push_back(sFile());
            sFile& f = files.back();
            f.fname = opp_strdup(it.fileName());
            f.directory = opp_strdup(it.baseDir());
            fileId = files.size()-1;
        }

        entries.push_back(sEntry());
        sEntry& e = entries.back();
        e.section_id = sectionold2new[secnum];
        e.key = opp_strdup(it.entry());
        e.keypattern = new PatternMatcher();
        e.keypattern->setPattern(e.key, true, true, true);
        e.value = opp_strdup(it.value());
        e.rawvalue = NULL;
        e.file_id = fileId;
        e.lineno = it.lineNumber();
        e.accessed = it.accessed();
    }
}

void cMySQLConfiguration::readDB(MYSQL *mysql, const char *configName)
{
    // query sections from the database
    if (strchr(configName,'"'))
        throw new cRuntimeError("cMySQLConfiguration: configname cannot contain quotes");
    std::string selectSectionsStmt = "SELECT s.id, s.name FROM configsection s, config c "
                                     "WHERE s.configid=c.id AND c.name=\"@configname@\"";
    opp_mysql_substitute(selectSectionsStmt, "@configname@",  configName, mysql);
    if (mysql_query(mysql, selectSectionsStmt.c_str()))
        throw new cRuntimeError("MySQL error: SELECT failed: %s", mysql_error(mysql));

    // and add them to the config
    std::map<long,int> sectionId2index;
    MYSQL_RES *res = mysql_store_result(mysql);
    if (res==NULL)
        throw new cRuntimeError("MySQL error: mysql_store_result() failed: %s", mysql_error(mysql));
    if (mysql_num_rows(res)==0)
        throw new cRuntimeError("cMySQLConfiguration: configname='%s': no such config in the database, or it is empty", configName);
    ASSERT(mysql_num_fields(res)==2);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))!=NULL)
    {
        long dbSectionId = atol(row[0]);
        const char *sectionName = row[1];
        int sectionnum = getSectionId(this, sectionName);
        if (sectionnum==-1) {
            sections.push_back(opp_strdup(sectionName));
            sectionId2index[dbSectionId] = sections.size()-1;
        } else {
            sectionId2index[dbSectionId] = sectionnum;
        }
    }
    mysql_free_result(res);

    // add a "file" entry (should we rename it to location?)
    files.push_back(sFile());
    sFile& f = files.back();
    std::string loc = std::string("MySQL/configname=")+configName;
    f.fname = opp_strdup(loc.c_str());
    f.directory = opp_strdup(files[0].directory); // use first inifile's dir
    int fileId = files.size()-1;

    // query entries from the database
    std::string selectEntriesStmt = "SELECT sectionid, e.name, value "
                                    "FROM configentry e, configsection s, config c "
                                    "WHERE e.sectionid=s.id AND s.configid=c.id AND c.name=\"@configname@\" "
                                    "ORDER BY entryorder";
    opp_mysql_substitute(selectEntriesStmt, "@configname@",  configName, mysql);
    if (mysql_query(mysql, selectEntriesStmt.c_str()))
        throw new cRuntimeError("MySQL error: SELECT failed: %s", mysql_error(mysql));

    // and add them to the config
    res = mysql_store_result(mysql);
    if (res==NULL)
        throw new cRuntimeError("MySQL error: mysql_store_result() failed: %s", mysql_error(mysql));
    ASSERT(mysql_num_fields(res)==3);
    while ((row = mysql_fetch_row(res))!=NULL)
    {
        long dbSectionId = atol(row[0]);
        const char *entryName = row[1];
        const char *value = row[2];
        ASSERT(sectionId2index.find(dbSectionId)!=sectionId2index.end());

        entries.push_back(sEntry());
        sEntry& e = entries.back();
        e.section_id = sectionId2index[dbSectionId];
        e.key = opp_strdup(entryName);
        e.keypattern = new PatternMatcher();
        e.keypattern->setPattern(e.key, true, true, true);
        e.value = opp_strdup(value);
        e.rawvalue = NULL;
        e.file_id = fileId;
        e.lineno = 0;  // no line number
        e.accessed = false;
    }
    mysql_free_result(res);
}

void cMySQLConfiguration::clearContents()
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
}

cMySQLConfiguration::sEntry *cMySQLConfiguration::_findEntry(const char *sect, const char *key)
{
    notfound = false;  // clear error flag
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

const char *cMySQLConfiguration::_getValue(const char *sect, const char *key, bool raw)
{
    sEntry *entry = _findEntry(sect, key);
    if (!entry)
        return NULL;
    return (raw && entry->rawvalue) ? entry->rawvalue : entry->value;
}

int cMySQLConfiguration::getNumSections()
{
    return sections.size();
}

const char *cMySQLConfiguration::getSectionName(int k)
{
    if (k<0 || k>=(int)sections.size())
        return NULL;
    return sections[k];
}

bool cMySQLConfiguration::exists(const char *sect, const char *key)
{
    return _getValue(sect, key, true)!=NULL;
}

bool cMySQLConfiguration::getAsBool(const char *sect, const char *key, bool defaultval)
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

long cMySQLConfiguration::getAsInt(const char *sect, const char *key, long defaultval)
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

double cMySQLConfiguration::getAsDouble(const char *sect, const char *key, double defaultval)
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

const char *cMySQLConfiguration::getAsString(const char *sect, const char *key, const char *defaultval)
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

double cMySQLConfiguration::getAsTime(const char *sect, const char *key, double defaultval)
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

std::string cMySQLConfiguration::getAsFilenames(const char *sect, const char *key, const char *defaultval)
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
    std::string result;
    cStringTokenizer tokenizer(entry->value);
    const char *token;
    while ((token = tokenizer.nextToken())!=NULL)
    {
        if (token[0]=='@' && token[1]=='@')
        {
            result += "@@" + tidyFilename(concatDirAndFile(baseDir, token+2).c_str()) + " ";
        }
        else if (token[0]=='@')
        {
            result += "@" + tidyFilename(concatDirAndFile(baseDir, token+1).c_str()) + " ";
        }
        else
        {
            result += tidyFilename(concatDirAndFile(baseDir, token).c_str()) + " ";
        }
    }
    if (result.size()>0)
        result.erase(result.end()-1); // chop trailing space
    return result;
}

const char *cMySQLConfiguration::getAsCustom(const char *sect, const char *key, const char *defaultval)
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

const char *cMySQLConfiguration::getBaseDirectoryFor(const char *section, const char *key)
{
    sEntry *entry = _findEntry(section, key);
    if (!entry)
        return NULL;
    return files[entry->file_id].directory;
}

std::string cMySQLConfiguration::getLocation(const char *section, const char *key)
{
    sEntry *entry = _findEntry(section, key);
    if (!entry)
        return "";
    char buf[16];
    sprintf(buf, "%d", entry->lineno);
    return std::string(files[entry->file_id].fname) + ":" + buf;
}

bool cMySQLConfiguration::notFound()
{
    bool e = notfound;
    notfound = false;
    return e;
}

//-----------

bool cMySQLConfiguration::exists2(const char *sect1, const char *sect2, const char *key)
{
    return exists(sect1, key) || exists(sect2, key);
}

bool cMySQLConfiguration::getAsBool2(const char *sect1, const char *sect2, const char *key, bool defaultval)
{
    bool w = warnings; warnings = false;
    bool a = getAsBool(sect1,key,defaultval);
    if (notfound)
         a = getAsBool(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Configuration entry %s= not in [%s] or [%s], %s used as default\n",
                   key,sect1,sect2,defaultval?"true":"false");
    return a;
}

long cMySQLConfiguration::getAsInt2(const char *sect1, const char *sect2, const char *key, long defaultval)
{
    bool w = warnings; warnings = false;
    long a = getAsInt(sect1,key,defaultval);
    if (notfound)
         a = getAsInt(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Configuration entry %s= not in [%s] or [%s], %ld used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

double cMySQLConfiguration::getAsDouble2(const char *sect1, const char *sect2, const char *key, double defaultval)
{
    bool w = warnings; warnings = false;
    double a = getAsDouble(sect1,key,defaultval);
    if (notfound)
         a = getAsDouble(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Configuration entry %s= not in [%s] or [%s], %g used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

const char *cMySQLConfiguration::getAsString2(const char *sect1, const char *sect2, const char *key, const char *defaultval)
{
    bool w = warnings; warnings = false;
    const char *a = getAsString(sect1,key,defaultval);
    if (notfound)
         a = getAsString(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Configuration entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

std::string cMySQLConfiguration::getAsFilename2(const char *sect1, const char *sect2, const char *key, const char *defaultval)
{
    bool w = warnings; warnings = false;
    std::string a = getAsFilename(sect1,key,defaultval);
    if (notfound)
         a = getAsFilename(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Configuration entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

std::string cMySQLConfiguration::getAsFilenames2(const char *sect1, const char *sect2, const char *key, const char *defaultval)
{
    bool w = warnings; warnings = false;
    std::string a = getAsFilenames(sect1,key,defaultval);
    if (notfound)
         a = getAsFilenames(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Configuration entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

double cMySQLConfiguration::getAsTime2(const char *sect1, const char *sect2, const char *key, double defaultval)
{
    bool w = warnings; warnings = false;
    double a = getAsTime(sect1,key,defaultval);
    if (notfound)
         a = getAsTime(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Configuration entry %s= not in [%s] or [%s], %g used as default\n",
                   key,sect1,sect2,defaultval);
    return a;
}

const char *cMySQLConfiguration::getAsCustom2(const char *sect1, const char *sect2, const char *key, const char *defaultval)
{
    bool w = warnings; warnings = false;
    const char *a = getAsCustom(sect1,key,defaultval);
    if (notfound)
         a = getAsCustom(sect2,key,defaultval);
    warnings = w;
    if (notfound && warnings)
         ev.printf("Configuration entry %s= not in [%s] or [%s], \"%s\" used as default\n",
                   key,sect1,sect2,defaultval?defaultval:"");
    return a;
}

const char *cMySQLConfiguration::getBaseDirectoryFor(const char *sect1, const char *sect2, const char *key)
{
    bool w = warnings; warnings = false;
    const char *a = getBaseDirectoryFor(sect1,key);
    if (notfound)
         a = getBaseDirectoryFor(sect2,key);
    warnings = w;
    return a;
}

std::string cMySQLConfiguration::getLocation(const char *sect1, const char *sect2, const char *key)
{
    bool w = warnings; warnings = false;
    std::string a = getLocation(sect1,key);
    if (notfound)
         a = getLocation(sect2,key);
    warnings = w;
    return a;
}

const char *cMySQLConfiguration::fileName() const
{
    return NULL;
}

//-----------

std::vector<opp_string> cMySQLConfiguration::getEntriesWithPrefix(const char *section, const char *keypart1, const char *keypart2)
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

std::vector<opp_string> cMySQLConfiguration::getEntriesWithPrefix(const char *section1, const char *section2, const char *keypart1, const char *keypart2)
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

