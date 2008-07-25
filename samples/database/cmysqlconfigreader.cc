//==========================================================================
//  CMYSQLCONFIGURATION.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cmysqlconfiguration.h"
#include "fileutil.h"  // directoryOf, tidyFilename
#include "patternmatcher.h" // TODO eliminate dependency
#include "cinifile.h" // for dumpConfig(); TODO eliminate dependency
#include "oppmysqlutils.h"


Register_Class(cMySQLConfigReader);

Register_GlobalConfigEntry(CFGID_MYSQLCONFIGURATION_DUMPBOOTCFG, "mysqlconfiguration-dumpbootcfg", "General", CFG_BOOL,  false, "FIXME add some description here");
Register_GlobalConfigEntry(CFGID_MYSQLCONFIGURATION_CONNECTPREFIX, "mysqlconfiguration-connectprefix", "General", CFG_STRING, "", "FIXME add some description here");
Register_GlobalConfigEntry(CFGID_MYSQLCONFIGURATION_CONFIGNAME, "mysqlconfiguration-configname", "General", CFG_STRING, "", "FIXME add some description here");


cMySQLConfigReader::cMySQLConfigReader()
{
}

cMySQLConfigReader::~cMySQLConfigReader()
{
    clearContents();
}

void cMySQLConfigReader::initializeFrom(cConfiguration *cfg)
{
    // utility: dump existing configuration as SQL INSERT statements
    if (cfg->getAsBool(CFGID_MYSQLCONFIGURATION_DUMPBOOTCFG))
        dumpConfig(cfg);

    // connect
    const char *prefix = cfg->getAsString(CFGID_MYSQLCONFIGURATION_CONNECTPREFIX);
    EV << getClassName() << " connecting to MySQL database";
    if (prefix && prefix[0]) EV << " using " << prefix << "-* config entries";
    EV << "...";
    MYSQL *mysql = mysql_init(NULL);
    opp_mysql_connectToDB(mysql, cfg, prefix);
    EV << " OK\n";

    // read the contents, then close the database
    copyExistingConfig(cfg);
    const char *configName = cfg->getAsString(CFGID_MYSQLCONFIGURATION_CONFIGNAME);
    if (!configName || !configName[0])
        throw cRuntimeError("cMySQLConfigReader: config entry mysqlconfiguration-configname= must be given");
    readDB(mysql, configName);
    mysql_close(mysql);
}

void cMySQLConfigReader::readDB(MYSQL *mysql, const char *configName)
{
    // query sections from the database
    if (strchr(configName,'"'))
        throw cRuntimeError("cMySQLConfigReader: configname cannot contain quotes");
    std::string selectSectionsStmt = "SELECT s.id, s.name FROM configsection s, config c "
                                     "WHERE s.configid=c.id AND c.name=\"@configname@\"";
    opp_mysql_substitute(selectSectionsStmt, "@configname@",  configName, mysql);
    if (mysql_query(mysql, selectSectionsStmt.c_str()))
        throw cRuntimeError("MySQL error: SELECT failed: %s", mysql_error(mysql));

    // and add them to the config
    std::map<long,int> sectionId2index;
    MYSQL_RES *res = mysql_store_result(mysql);
    if (res==NULL)
        throw cRuntimeError("MySQL error: mysql_store_result() failed: %s", mysql_error(mysql));
    if (mysql_num_rows(res)==0)
        throw cRuntimeError("cMySQLConfigReader: configname='%s': no such config in the database, or it is empty", configName);
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
        throw cRuntimeError("MySQL error: SELECT failed: %s", mysql_error(mysql));

    // and add them to the config
    res = mysql_store_result(mysql);
    if (res==NULL)
        throw cRuntimeError("MySQL error: mysql_store_result() failed: %s", mysql_error(mysql));
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

const char *cMySQLConfigReader::getFileName() const
{
}

const char *cMySQLConfigReader::getDefaultBaseDirectory() const
{
}

int cMySQLConfigReader::getNumSections() const
{
    return sections.size();
}

const char *cMySQLConfigReader::getSectionName(int sectionId) const
{
    if (sectionId<0 || sectionId>=(int)sections.size())
        return NULL;
    return sections[sectionId];
}

int cMySQLConfigReader::getNumEntries(int sectionId) const
{
}

const KeyValue& cMySQLConfigReader::getEntry(int sectionId, int entryId) const
{
}

void cMySQLConfigReader::dump() const
{
}


void cMySQLConfigReader::dumpConfig(cConfiguration *cfg)
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
        throw cRuntimeError("cMySQLConfigReader: boot-time config is not a cIniFile");

    int k = 0;
    for (cIniFileIterator it(ini); !it.end(); it++)
    {
        int secnum = getSectionId(cfg, it.section());
        ev.printf("INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (%d,\"%s\",\"%s\",%d);\n",
                  base+secnum, it.entry(), it.value(), ++k);
    }
}


void cMySQLConfigReader::copyExistingConfig(cConfiguration *cfg)
{
    std::map<int,int> sectionold2new;
    for (int i=0; i<cfg->getNumSections(); i++)
    {
        sections.push_back(opp_strdup(cfg->getSectionName(i)));
        sectionold2new[i] = sections.size()-1;
    }

    cIniFile *ini = dynamic_cast<cIniFile *>(cfg);
    if (!ini)
        throw cRuntimeError("cMySQLConfigReader: boot-time config is not a cIniFile");

    for (cIniFileIterator it(ini); !it.end(); it++)
    {
        int secnum = getSectionId(cfg, it.section());
        ASSERT(secnum!=-1 && sectionold2new.find(secnum)!=sectionold2new.end());

        int fileId = getFileId(it.getFileName());
        if (fileId==-1) {
            files.push_back(sFile());
            sFile& f = files.back();
            f.fname = opp_strdup(it.getFileName());
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


void cMySQLConfigReader::clearContents()
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


