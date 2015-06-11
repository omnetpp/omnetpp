//==========================================================================
//  CMYSQLCONFIGREADER.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2011 Zoltan Bojthe

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cmysqlconfigreader.h"

#include "sectionbasedconfig.h"

#include "oppmysqlutils.h"

Register_Class(cMySQLConfigReader);

Register_GlobalConfigOption(CFGID_MYSQLCONFIG_DUMPBOOTCFG,    "mysqlconfig-dumpbootcfg", CFG_BOOL, "false", "Prints the current configuration as a series of SQL INSERT statements.");
Register_GlobalConfigOption(CFGID_MYSQLCONFIG_CONNECTIONNAME, "mysqlconfig-connectionname", CFG_STRING, "\"mysql\"", "Object name of database connection parameters, default='mysql'");
Register_GlobalConfigOption(CFGID_MYSQLCONFIG_CONFIGNAME,     "mysqlconfig-configname", CFG_STRING, "\"\"", "Name of configuration in database. The 'SELECT name FROM config;' SQL command shows the list of available configs");


cMySQLConfigReader::cMySQLConfigReader()
{
}

cMySQLConfigReader::~cMySQLConfigReader()
{
    clearContents();
}

void cMySQLConfigReader::initializeFrom(cConfiguration *cfg)
{
    SectionBasedConfiguration *scfg = check_and_cast<SectionBasedConfiguration *>(cfg);
    cConfigurationReader *reader = scfg->getConfigurationReader();

    // utility: dump existing configuration as SQL INSERT statements
    if (cfg->getAsBool(CFGID_MYSQLCONFIG_DUMPBOOTCFG))
        dumpConfig(reader);

    defaultbasedir = reader->getDefaultBaseDirectory();

    // connect
    std::string cfgobj = cfg->getAsString(CFGID_MYSQLCONFIG_CONNECTIONNAME);
    if (cfgobj.empty())
        throw cRuntimeError("cMySQLConfigReader: config entry mysqlconfig-connectionname= must not be empty");
    EV << "cMySQLConfigReader connecting to MySQL database, using " << cfgobj << ".* config entries ...";
    MYSQL *mysql = mysql_init(nullptr);
    opp_mysql_connectToDB(mysql, cfg, cfgobj.c_str());
    EV << " OK\n";

    // read the contents, then close the database
    copyExistingConfig(reader);
    std::string configName = cfg->getAsString(CFGID_MYSQLCONFIG_CONFIGNAME);
    if (configName.empty())
        throw cRuntimeError("cMySQLConfigReader: config entry mysqlconfig-configname= must be given");
    readDB(mysql, configName.c_str());
    mysql_close(mysql);
}

int cMySQLConfigReader::findSection(const char *sectionName) const
{
    for (int sectionnum = 0; sectionnum < getNumSections(); sectionnum++)
        if (!strcmp(getSectionName(sectionnum), sectionName))
            return sectionnum;
    return -1;
}

void cMySQLConfigReader::readDB(MYSQL *mysql, const char *configName)
{
    // query sections from the database
    if (strchr(configName, '"'))
        throw cRuntimeError("cMySQLConfigReader: configname cannot contain quotes");
    std::string selectSectionsStmt = "SELECT s.id, s.name FROM configsection s, config c "
                                     "WHERE s.configid=c.id AND c.name='@configname@' "
                                     "ORDER BY s.id";
    opp_mysql_substitute(selectSectionsStmt, "@configname@", configName, mysql);
    if (mysql_query(mysql, selectSectionsStmt.c_str()))
        throw cRuntimeError("MySQL error: SELECT failed: %s", mysql_error(mysql));

    // and add them to the config
    std::map<long, int> dbSectionId2index;
    MYSQL_RES *res = mysql_store_result(mysql);
    if (res == nullptr)
        throw cRuntimeError("MySQL error: mysql_store_result() failed: %s", mysql_error(mysql));
    if (mysql_num_rows(res) == 0)
        throw cRuntimeError("cMySQLConfigReader: configname='%s': no such config in the database, or it is empty", configName);
    ASSERT(mysql_num_fields(res) == 2);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr) {
        long dbSectionId = atol(row[0]);
        const char *sectionName = row[1];
        int sectionnum = findSection(sectionName);
        if (sectionnum == -1) {
            Section section;
            section.name = sectionName;
            sections.push_back(section);
            sectionnum = sections.size()-1;
        }
        dbSectionId2index[dbSectionId] = sectionnum;
    }
    mysql_free_result(res);

    // add a "file" entry (should we rename it to location?)
    std::string loc = std::string("MySQL/configname=")+configName;

    // query entries from the database
    std::string selectEntriesStmt = "SELECT sectionid, e.name, value, entryorder "
                                    "FROM configentry e, configsection s, config c "
                                    "WHERE e.sectionid=s.id AND s.configid=c.id AND c.name='@configname@' "
                                    "ORDER BY s.id,entryorder";
    opp_mysql_substitute(selectEntriesStmt, "@configname@", configName, mysql);
    if (mysql_query(mysql, selectEntriesStmt.c_str()))
        throw cRuntimeError("MySQL error: SELECT failed: %s", mysql_error(mysql));

    // and add them to the config
    res = mysql_store_result(mysql);
    if (res == nullptr)
        throw cRuntimeError("MySQL error: mysql_store_result() failed: %s", mysql_error(mysql));
    ASSERT(mysql_num_fields(res) == 4);
    while ((row = mysql_fetch_row(res)) != nullptr) {
        long dbSectionId = atol(row[0]);
        const char *entryName = row[1];
        const char *value = row[2];
        long linenumber = atol(row[3]);
        ASSERT(dbSectionId2index.find(dbSectionId) != dbSectionId2index.end());
        int sectionIndex = dbSectionId2index[dbSectionId];

        KeyValue1 e(&defaultbasedir, &rootfilename, linenumber, entryName, value);
        sections[sectionIndex].entries.push_back(e);
    }
    mysql_free_result(res);
}

const char *cMySQLConfigReader::getFileName() const
{
    return nullptr;
}

const char *cMySQLConfigReader::getDefaultBaseDirectory() const
{
    return defaultbasedir.c_str();
}

int cMySQLConfigReader::getNumSections() const
{
    return sections.size();
}

const char *cMySQLConfigReader::getSectionName(int sectionId) const
{
    if (sectionId < 0 || sectionId >= (int)sections.size())
        throw cRuntimeError("Invalid sectionId (%d)", sectionId);
    return sections[sectionId].name.c_str();
}

int cMySQLConfigReader::getNumEntries(int sectionId) const
{
    if (sectionId < 0 || sectionId >= (int)sections.size())
        throw cRuntimeError("Invalid sectionId (%d)", sectionId);
    return sections[sectionId].entries.size();
}

const cConfigurationReader::KeyValue& cMySQLConfigReader::getEntry(int sectionId, int entryId) const
{
    if (sectionId < 0 || sectionId >= (int)sections.size())
        throw cRuntimeError("Invalid sectionId (%d)", sectionId);
    const Section& curSect = sections[sectionId];
    if (entryId < 0 || entryId >= (int)curSect.entries.size())
        throw cRuntimeError("Invalid entryId (%d)", entryId);
    return curSect.entries[entryId];
}

void cMySQLConfigReader::dump() const
{
    ::printf("INSERT INTO config (id,name) VALUES (10,'someconfig');\n");

    const int base = 1000;
    int k = 0;

    for (int i = 0; i < getNumSections(); i++) {
        ::printf("INSERT INTO configsection (configid,id,name) VALUES (10,%d,'%s');\n",
                base+i, escape(getSectionName(i)).c_str());
        for (int j = 0; j < getNumEntries(i); j++) {
            const KeyValue& entry = getEntry(i, j);
            ::printf("INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (%d,'%s','%s',%d);\n",
                    base+i, escape(entry.getKey()).c_str(), escape(entry.getValue()).c_str(), k++);
        }
    }
}

void cMySQLConfigReader::dumpConfig(cConfigurationReader *cfg) const
{
    ::printf("INSERT INTO config (id,name) VALUES (10,'someconfig');\n");

    const int base = 1000;
    int k = 0;

    for (int i = 0; i < cfg->getNumSections(); i++) {
        ::printf("INSERT INTO configsection (configid,id,name) VALUES (10,%d,'%s');\n",
                base+i, escape(cfg->getSectionName(i)).c_str());
        for (int j = 0; j < cfg->getNumEntries(i); j++) {
            const KeyValue& entry = cfg->getEntry(i, j);

            ::printf("INSERT INTO configentry (sectionid,name,value,entryorder) VALUES (%d,'%s','%s',%d);\n",
                    base+i, escape(entry.getKey()).c_str(), escape(entry.getValue()).c_str(), k++);
        }
    }
}

void cMySQLConfigReader::copyExistingConfig(cConfigurationReader *cfg)
{
    ASSERT(sections.empty());

    std::map<int, int> sectionold2new;

    for (int i = 0; i < cfg->getNumSections(); i++) {
        Section section;
        section.name = cfg->getSectionName(i);
        sections.push_back(section);
        sectionold2new[i] = sections.size()-1;
        for (int j = 0; j < cfg->getNumEntries(i); j++) {
            const KeyValue& e = cfg->getEntry(i, j);
            // get a reference to the string instance in filenames[], we'll refer to it in KeyValue
            StringSet::iterator basedirIt, filenameIt;
            basedirIt = basedirs.insert(e.getBaseDirectory()).first;
            filenameIt = filenames.insert(e.getFileName()).first;

            KeyValue1 newEntry(&(*basedirIt), &(*filenameIt), e.getLineNumber(), e.getKey(), e.getValue());
        }
    }
}

void cMySQLConfigReader::clearContents()
{
    sections.clear();
}

std::string cMySQLConfigReader::escape(const char *str)
{
    size_t len = strlen(str);
    char buf[2*len+1];
    buf[0] = '\0';
    mysql_escape_string(buf, str, len);
    return std::string(buf);
}

