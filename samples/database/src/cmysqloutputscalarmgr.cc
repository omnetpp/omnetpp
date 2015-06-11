//==========================================================================
//  MYSQLOUTPUTSCALARMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>

#include "cmysqloutputscalarmgr.h"

#include "oppmysqlutils.h"

//
// NOTE: INSERT DELAYED is not supported on all engines, notably InnoDB.
// It is best to create the tables with the ENGINE = MYISAM option.
//
#define SQL_INSERT_RUN       "INSERT INTO run(runnumber,network) VALUES(@runnumber@,'@network@')"
#define SQL_INSERT_SCALAR    "INSERT DELAYED INTO scalar(runid,module,name,value) VALUES(?,?,?,?)"

Register_Class(cMySQLOutputScalarManager);

Register_GlobalConfigOption(CFGID_MYSQLOUTSCALARMGR_CONNECTIONNAME, "mysqloutputscalarmanager-connectionname", CFG_STRING, "\"mysql\"", "Object name of database connection parameters");
Register_GlobalConfigOption(CFGID_MYSQLOUTSCALARMGR_COMMIT_FREQ, "mysqloutputscalarmanager-commit-freq", CFG_INT, "10", "COMMIT every n INSERTs, default=10");

cMySQLOutputScalarManager::cMySQLOutputScalarManager()
{
    mysql = nullptr;
    insertScalarStmt = nullptr;
}

cMySQLOutputScalarManager::~cMySQLOutputScalarManager()
{
    if (mysql)
        closeDB();
}

void cMySQLOutputScalarManager::openDB()
{
    // connect
    EV << getClassName() << " connecting to MySQL database...";
    std::string cfgobj = getEnvir()->getConfig()->getAsString(CFGID_MYSQLOUTSCALARMGR_CONNECTIONNAME);
    if (cfgobj.empty())
        cfgobj = "mysql";
    mysql = mysql_init(nullptr);
    opp_mysql_connectToDB(mysql, getEnvir()->getConfig(), cfgobj.c_str());
    EV << " OK\n";

    commitFreq = getEnvir()->getConfig()->getAsInt(CFGID_MYSQLOUTSCALARMGR_COMMIT_FREQ);
    insertCount = 0;

    // prepare and bind INSERT INTO SCALAR...
    insertScalarStmt = mysql_stmt_init(mysql);
    if (!insertScalarStmt)
        throw cRuntimeError("MySQL error: out of memory");
    if (mysql_stmt_prepare(insertScalarStmt, SQL_INSERT_SCALAR, strlen(SQL_INSERT_SCALAR)))
        throw cRuntimeError("MySQL error: prepare statement failed: %s", mysql_error(mysql));
    ASSERT(mysql_stmt_param_count(insertScalarStmt) == sizeof(insScalarBind)/sizeof(MYSQL_BIND));

    opp_mysql_bindLONG(insScalarBind[0], runidBuf);  // scalar.runid
    opp_mysql_bindSTRING(insScalarBind[1], moduleBuf, sizeof(moduleBuf), moduleLength);  // scalar.module
    opp_mysql_bindSTRING(insScalarBind[2], nameBuf, sizeof(nameBuf), nameLength);  // scalar.name
    opp_mysql_bindDOUBLE(insScalarBind[3], valueBuf);  // scalar.value

    if (mysql_stmt_bind_param(insertScalarStmt, insScalarBind))
        throw cRuntimeError("MySQL error: bind failed: %s", mysql_error(mysql));
}

void cMySQLOutputScalarManager::closeDB()
{
    if (insertScalarStmt)
        mysql_stmt_close(insertScalarStmt);
    insertScalarStmt = nullptr;

    if (mysql)
        mysql_close(mysql);
    mysql = nullptr;
}

void cMySQLOutputScalarManager::commitDB()
{
    if (mysql_commit(mysql))
        throw cRuntimeError("MySQL error: COMMIT failed: %s", mysql_error(mysql));
}

void cMySQLOutputScalarManager::startRun()
{
    // clean up file from previous runs
    if (mysql)
        closeDB();
    openDB();
    initialized = false;
    runId = -1;
}

void cMySQLOutputScalarManager::endRun()
{
    if (mysql) {
        commitDB();
        closeDB();
    }
}

void cMySQLOutputScalarManager::insertRunIntoDB()
{
    if (!initialized) {
        // insert run into the database
        std::string insertRunStmt = SQL_INSERT_RUN;
        opp_mysql_substitute(insertRunStmt, "@runnumber@", getEnvir()->getConfigEx()->getActiveRunNumber(), mysql);
        opp_mysql_substitute(insertRunStmt, "@network@", getSimulation()->getNetworkType()->getName(), mysql);
        if (mysql_query(mysql, insertRunStmt.c_str()))
            throw cRuntimeError("MySQL error: INSERT failed: %s", mysql_error(mysql));

        // query the automatic runid from the newly inserted row
        runId = (long)mysql_insert_id(mysql);

        initialized = true;
    }
}

void cMySQLOutputScalarManager::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    if (!initialized)
        insertRunIntoDB();

    // fill in prepared statement parameters, and fire off the statement
    runidBuf = runId;
    opp_mysql_assignSTRING(insScalarBind[1], moduleBuf, component->getFullPath().c_str());
    opp_mysql_assignSTRING(insScalarBind[2], nameBuf, name);
    valueBuf = value;

    if (mysql_stmt_execute(insertScalarStmt))
        throw cRuntimeError("MySQL error: INSERT failed: %s", mysql_error(mysql));

    // commit every once in a while
    if (++insertCount == commitFreq) {
        insertCount = 0;
        commitDB();
    }
}

void cMySQLOutputScalarManager::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    throw cRuntimeError("cMySQLOutputScalarManager: recording cStatistics objects not supported yet");
}

void cMySQLOutputScalarManager::flush()
{
    commitDB();
}

