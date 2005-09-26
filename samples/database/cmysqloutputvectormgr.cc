//==========================================================================
//  MYSQLOUTPUTVECTOR.CC - part of
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
#include "omnetapp.h" // getOutVectorConfig() -- TODO eliminate dependency
#include "cmysqloutputvectormgr.h"
#include "oppmysqlutils.h"


//
// NOTE: INSERT DELAYED is not supported on all engines, notably InnoDB.
// It is best to create the tables with the ENGINE = MYISAM option.
//
// IMPORTANT: "INSERT INTO vector" cannot be DELAYED, otherwise we don't get back the vectorid.
//
#define SQL_INSERT_VRUN     "INSERT INTO vrun(runnumber,network) VALUES(@runnumber@,\"@network@\")"
#define SQL_INSERT_VECTOR   "INSERT INTO vector(runid,module,name) VALUES(?,?,?)"
#define SQL_INSERT_VECDATA  "INSERT DELAYED INTO vecdata(vectorid,time,value) VALUES(?,?,?)"


Register_Class(cMySQLOutputVectorManager);

cMySQLOutputVectorManager::cMySQLOutputVectorManager()
{
    mysql = NULL;
    insertVectorStmt = insertVecdataStmt = NULL;
}

cMySQLOutputVectorManager::~cMySQLOutputVectorManager()
{
    if (mysql)
        closeDB();
}

void cMySQLOutputVectorManager::openDB()
{
    // connect
    const char *prefix = ev.config()->getAsString("General", "mysqloutputvectormanager-connectprefix", NULL);
    ev << className() << " connecting to MySQL database";
    if (prefix && prefix[0]) ev << " using " << prefix << "-* config entries";
    ev << "...";
    mysql = mysql_init(NULL);
    opp_mysql_connectToDB(mysql, ev.config(), prefix);
    ev << " OK\n";

    commitFreq = ev.config()->getAsInt("General", "mysqloutputvectormanager-commit-freq", 50);
    insertCount = 0;

    // prepare and bind INSERT INTO VECTOR...
    insertVectorStmt = mysql_stmt_init(mysql);
    if (!insertVectorStmt)
        throw new cRuntimeError("MySQL error: out of memory");
    if (mysql_stmt_prepare(insertVectorStmt, SQL_INSERT_VECTOR, strlen(SQL_INSERT_VECTOR)))
        throw new cRuntimeError("MySQL error: prepare statement failed: %s", mysql_error(mysql));
    ASSERT(mysql_stmt_param_count(insertVectorStmt)==sizeof(insVectorBind)/sizeof(MYSQL_BIND));

    opp_mysql_bindLONG(insVectorBind[0], runidBuf); // vector.runid
    opp_mysql_bindSTRING(insVectorBind[1], moduleBuf, sizeof(moduleBuf), moduleLength); // vector.module
    opp_mysql_bindSTRING(insVectorBind[2], nameBuf, sizeof(nameBuf), nameLength); // vector.name

    if (mysql_stmt_bind_param(insertVectorStmt, insVectorBind))
        throw new cRuntimeError("MySQL error: bind failed: %s", mysql_error(mysql));

    // prepare and bind INSERT INTO VECDATA...
    insertVecdataStmt = mysql_stmt_init(mysql);
    if (!insertVecdataStmt)
        throw new cRuntimeError("MySQL error: out of memory");
    if (mysql_stmt_prepare(insertVecdataStmt, SQL_INSERT_VECDATA, strlen(SQL_INSERT_VECDATA)))
        throw new cRuntimeError("MySQL error: prepare statement failed: %s", mysql_error(mysql));
    ASSERT(mysql_stmt_param_count(insertVecdataStmt)==sizeof(insVecdataBind)/sizeof(MYSQL_BIND));

    opp_mysql_bindLONG(insVecdataBind[0], vectoridBuf); // vecdata.vectorid
    opp_mysql_bindDOUBLE(insVecdataBind[1], timeBuf); // vecdata.time
    opp_mysql_bindDOUBLE(insVecdataBind[2], valueBuf); // vecdata.value

    if (mysql_stmt_bind_param(insertVecdataStmt, insVecdataBind))
        throw new cRuntimeError("MySQL error: bind failed: %s", mysql_error(mysql));
}

void cMySQLOutputVectorManager::closeDB()
{
    if (insertVectorStmt) mysql_stmt_close(insertVectorStmt);
    if (insertVecdataStmt) mysql_stmt_close(insertVecdataStmt);
    insertVectorStmt = insertVecdataStmt = NULL;

    if (mysql) mysql_close(mysql);
    mysql = NULL;
}

void cMySQLOutputVectorManager::commitDB()
{
    if (mysql_commit(mysql))
        throw new cRuntimeError("MySQL error: COMMIT failed: %s", mysql_error(mysql));
}

void cMySQLOutputVectorManager::initVector(sVectorData *vp)
{
    if (!initialized)
        insertRunIntoDB();

    // fill in prepared statement parameters, and fire off the statement
    runidBuf = runId;
    opp_mysql_assignSTRING(insVectorBind[1], moduleBuf, vp->modulename.c_str());
    opp_mysql_assignSTRING(insVectorBind[2], nameBuf, vp->vectorname.c_str());
    if (mysql_stmt_execute(insertVectorStmt))
        throw new cRuntimeError("MySQL error: INSERT failed: %s", mysql_error(mysql));

    // query the automatic vectorid from the newly inserted row
    // Note: this INSERT must not be DELAYED, otherwise we get zero here.
    vp->id = (long) mysql_stmt_insert_id(insertVectorStmt);

    vp->initialised = true;
}

void cMySQLOutputVectorManager::startRun()
{
    // clean up file from previous runs
    if (mysql)
        closeDB();
    openDB();
    initialized = false;
}

void cMySQLOutputVectorManager::endRun()
{
    if (mysql)
    {
        commitDB();
        closeDB();
    }
}

void cMySQLOutputVectorManager::insertRunIntoDB()
{
    if (!initialized)
    {
        // insert run into the database
        std::string insertRunStmt = SQL_INSERT_VRUN;
        opp_mysql_substitute(insertRunStmt, "@runnumber@", simulation.runNumber(), mysql);
        opp_mysql_substitute(insertRunStmt, "@network@", simulation.networkType()->name(), mysql);
        if (mysql_query(mysql, insertRunStmt.c_str()))
            throw new cRuntimeError("MySQL error: INSERT failed: %s", mysql_error(mysql));

        // query the automatic runid from the newly inserted row
        runId = (long) mysql_insert_id(mysql);

        initialized = true;
    }
}

void *cMySQLOutputVectorManager::registerVector(const char *modulename, const char *vectorname, int tuple)
{
    // only create the data structure here -- we'll insert the entry into the
    // DB lazily, when first data gets written
    sVectorData *vp = createVectorData();
    vp->tuple = tuple;
    vp->id = -1; // we'll get it from the database
    vp->initialised = false;
    vp->modulename = modulename;
    vp->vectorname = vectorname;

    ev.app->getOutVectorConfig(simulation.runNumber(), modulename, vectorname,
                               vp->enabled, vp->starttime, vp->stoptime);
    return vp;
}

cMySQLOutputVectorManager::sVectorData *cMySQLOutputVectorManager::createVectorData()
{
    return new sVectorData;
}

void cMySQLOutputVectorManager::deregisterVector(void *vectorhandle)
{
    sVectorData *vp = (sVectorData *)vectorhandle;
    delete vp;
}

bool cMySQLOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    sVectorData *vp = (sVectorData *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (t>=vp->starttime && (vp->stoptime==0.0 || t<=vp->stoptime))
    {
        if (!vp->initialised)
            initVector(vp);

        // fill in prepared statement parameters, and fire off the statement
        vectoridBuf = vp->id;
        timeBuf = t;
        valueBuf = value;
        if (mysql_stmt_execute(insertVecdataStmt))
            throw new cRuntimeError("MySQL error: INSERT failed: %s", mysql_error(mysql));

        // commit every once in a while
        if (++insertCount == commitFreq)
        {
            insertCount = 0;
            commitDB();
        }

        return true;
    }
    return false;
}

bool cMySQLOutputVectorManager::record(void *vectorhandle, simtime_t t, double value1, double value2)
{
    throw new cRuntimeError("cMySQLOutputVectorManager: tuple=2 not supported");
}

void cMySQLOutputVectorManager::flush()
{
    commitDB();
}

