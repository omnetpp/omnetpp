//==========================================================================
//  MYSQLOUTPUTSCALARMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include <omnetpp.h>
#include "cmysqloutputscalarmgr.h"
#include "oppmysqlutils.h"


//
// NOTE: INSERT DELAYED is not supported on all engines, notably InnoDB.
// It is best to create the tables with the ENGINE = MYISAM option.
//
#define SQL_INSERT_RUN     "INSERT INTO run(runnumber,network) VALUES(@runnumber@,\"@network@\")"
#define SQL_INSERT_SCALAR  "INSERT DELAYED INTO scalar(runid,module,name,value) VALUES(?,?,?,?)"


Register_Class(cMySQLOutputScalarManager);


cMySQLOutputScalarManager::cMySQLOutputScalarManager()
{
    mysql = NULL;
    insertScalarStmt = NULL;
}

cMySQLOutputScalarManager::~cMySQLOutputScalarManager()
{
    if (mysql)
        closeDB();
}

void cMySQLOutputScalarManager::openDB()
{
    // connect
    const char *prefix = ev.config()->getAsString("General", "mysqloutputscalarmanager-connectprefix", NULL);
    ev << className() << " connecting to MySQL database";
    if (prefix && prefix[0]) ev << " using " << prefix << "-* config entries";
    ev << "...";
    mysql = mysql_init(NULL);
    opp_mysql_connectToDB(mysql, ev.config(), prefix);
    ev << " OK\n";

    commitFreq = ev.config()->getAsInt("General", "mysqloutputscalarmanager-commit-freq", 10);
    insertCount = 0;

    // prepare and bind INSERT INTO SCALAR...
    insertScalarStmt = mysql_stmt_init(mysql);
    if (!insertScalarStmt)
        throw new cRuntimeError("MySQL error: out of memory");
    if (mysql_stmt_prepare(insertScalarStmt, SQL_INSERT_SCALAR, strlen(SQL_INSERT_SCALAR)))
        throw new cRuntimeError("MySQL error: prepare statement failed: %s", mysql_error(mysql));
    ASSERT(mysql_stmt_param_count(insertScalarStmt)==sizeof(insScalarBind)/sizeof(MYSQL_BIND));

    opp_mysql_bindLONG(  insScalarBind[0], runidBuf); // scalar.runid
    opp_mysql_bindSTRING(insScalarBind[1], moduleBuf, sizeof(moduleBuf), moduleLength); // scalar.module
    opp_mysql_bindSTRING(insScalarBind[2], nameBuf, sizeof(nameBuf), nameLength); // scalar.name
    opp_mysql_bindDOUBLE(insScalarBind[3], valueBuf); // scalar.value

    if (mysql_stmt_bind_param(insertScalarStmt, insScalarBind))
        throw new cRuntimeError("MySQL error: bind failed: %s", mysql_error(mysql));
}

void cMySQLOutputScalarManager::closeDB()
{
    if (insertScalarStmt) mysql_stmt_close(insertScalarStmt);
    insertScalarStmt = NULL;

    if (mysql) mysql_close(mysql);
    mysql = NULL;
}

void cMySQLOutputScalarManager::commitDB()
{
    if (mysql_commit(mysql))
        throw new cRuntimeError("MySQL error: COMMIT failed: %s", mysql_error(mysql));
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
    if (mysql)
    {
        commitDB();
        closeDB();
    }
}

void cMySQLOutputScalarManager::insertRunIntoDB()
{
    if (!initialized)
    {
        // insert run into the database
        std::string insertRunStmt = SQL_INSERT_RUN;
        opp_mysql_substitute(insertRunStmt, "@runnumber@", simulation.runNumber(), mysql);
        opp_mysql_substitute(insertRunStmt, "@network@", simulation.networkType()->name(), mysql);
        if (mysql_query(mysql, insertRunStmt.c_str()))
            throw new cRuntimeError("MySQL error: INSERT failed: %s", mysql_error(mysql));

        // query the automatic runid from the newly inserted row
        runId = (long) mysql_insert_id(mysql);

        initialized = true;
    }
}

void cMySQLOutputScalarManager::recordScalar(cModule *module, const char *name, double value)
{
    if (!initialized)
        insertRunIntoDB();

    // fill in prepared statement parameters, and fire off the statement
    runidBuf = runId;
    opp_mysql_assignSTRING(insScalarBind[1], moduleBuf, module->fullPath().c_str());
    opp_mysql_assignSTRING(insScalarBind[2], nameBuf, name);
    valueBuf = value;

    if (mysql_stmt_execute(insertScalarStmt))
        throw new cRuntimeError("MySQL error: INSERT failed: %s", mysql_error(mysql));

    // commit every once in a while
    if (++insertCount == commitFreq)
    {
        insertCount = 0;
        commitDB();
    }
}

void cMySQLOutputScalarManager::flush()
{
    commitDB();
}


