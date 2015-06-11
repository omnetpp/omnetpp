//==========================================================================
//  MYSQLOUTPUTVECTOR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2011 Zoltan Bojthe

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>

#include "cmysqloutputvectormgr.h"

#include "fileoutvectormgr.h"
#include "oppmysqlutils.h"


//
// NOTE: INSERT DELAYED is not supported on all engines, notably InnoDB.
// It is best to create the tables with the ENGINE = MYISAM option.
//
// IMPORTANT: "INSERT INTO vector" cannot be DELAYED, otherwise we don't get back the vectorid.
//
#define SQL_INSERT_VRUN     "INSERT INTO vrun(runnumber,network) VALUES(@runnumber@,'@network@')"
#define SQL_INSERT_VECTOR   "INSERT INTO vector(runid,module,name) VALUES(?,?,?)"
#define SQL_INSERT_VECATTR  "INSERT INTO vecattr(vectorid,name,value) VALUES(?,?,?)"
#define SQL_INSERT_VECDATA  "INSERT DELAYED INTO vecdata(vectorid,time,value) VALUES(?,?,?)"


Register_Class(cMySQLOutputVectorManager);

//#define Register_GlobalConfigOption(ID, NAME, TYPE, DEFAULTVALUE, DESCRIPTION)
Register_GlobalConfigOption(CFGID_MYSQLOUTVECTORMGR_CONNECTIONNAME, "mysqloutputvectormanager-connectionname", CFG_STRING, "\"mysql\"", "Object name of database connection parameters, default='mysql'");
Register_GlobalConfigOption(CFGID_MYSQLOUTVECTORMGR_COMMIT_FREQ, "mysqloutputvectormanager-commit-freq", CFG_INT, "50", "COMMIT every n INSERTs, default=50");


cMySQLOutputVectorManager::cMySQLOutputVectorManager()
{
    mysql = nullptr;
    insertVectorStmt = insertVecdataStmt = nullptr;
}

cMySQLOutputVectorManager::~cMySQLOutputVectorManager()
{
    if (mysql)
        closeDB();
}

void cMySQLOutputVectorManager::openDB()
{
    EV << getClassName() << " connecting to MySQL database...";

    // connect
    std::string cfgobj = getEnvir()->getConfig()->getAsString(CFGID_MYSQLOUTVECTORMGR_CONNECTIONNAME);
    if (cfgobj.empty())
        cfgobj = "mysql";
    mysql = mysql_init(nullptr);
    opp_mysql_connectToDB(mysql, getEnvir()->getConfig(), cfgobj.c_str());
    EV << " OK\n";

    commitFreq = getEnvir()->getConfig()->getAsInt(CFGID_MYSQLOUTVECTORMGR_COMMIT_FREQ);
    insertCount = 0;

    // prepare and bind INSERT INTO VECTOR...
    insertVectorStmt = mysql_stmt_init(mysql);
    if (!insertVectorStmt)
        throw cRuntimeError("MySQL error: out of memory");
    if (mysql_stmt_prepare(insertVectorStmt, SQL_INSERT_VECTOR, strlen(SQL_INSERT_VECTOR)))
        throw cRuntimeError("MySQL error: prepare statement failed: %s", mysql_error(mysql));
    ASSERT(mysql_stmt_param_count(insertVectorStmt) == sizeof(insVectorBind)/sizeof(MYSQL_BIND));

    opp_mysql_bindLONG(insVectorBind[0], runidBuf);  // vector.runid
    opp_mysql_bindSTRING(insVectorBind[1], moduleBuf, sizeof(moduleBuf), moduleLength);  // vector.module
    opp_mysql_bindSTRING(insVectorBind[2], nameBuf, sizeof(nameBuf), nameLength);  // vector.name

    if (mysql_stmt_bind_param(insertVectorStmt, insVectorBind))
        throw cRuntimeError("MySQL error: bind failed: %s", mysql_error(mysql));

    // prepare and bind INSERT INTO VECATTR...
    insertVecAttrStmt = mysql_stmt_init(mysql);
    if (!insertVecAttrStmt)
        throw cRuntimeError("MySQL error: out of memory");
    if (mysql_stmt_prepare(insertVecAttrStmt, SQL_INSERT_VECATTR, strlen(SQL_INSERT_VECATTR)))
        throw cRuntimeError("MySQL error: prepare statement failed: %s", mysql_error(mysql));
    ASSERT(mysql_stmt_param_count(insertVecAttrStmt) == sizeof(insVecAttrBind)/sizeof(MYSQL_BIND));

    opp_mysql_bindLONG(insVecAttrBind[0], vectoridBuf);  // vecattr.vectorid
    opp_mysql_bindSTRING(insVecAttrBind[1], vecAttrNameBuf, sizeof(vecAttrNameBuf), vecAttrNameLength);  // vecattr.name
    opp_mysql_bindSTRING(insVecAttrBind[2], vecAttrValueBuf, sizeof(vecAttrValueBuf), vecAttrValueLength);  // vecattr.value

    if (mysql_stmt_bind_param(insertVecAttrStmt, insVecAttrBind))
        throw cRuntimeError("MySQL error: bind failed: %s", mysql_error(mysql));

    // prepare and bind INSERT INTO VECDATA...
    insertVecdataStmt = mysql_stmt_init(mysql);
    if (!insertVecdataStmt)
        throw cRuntimeError("MySQL error: out of memory");
    if (mysql_stmt_prepare(insertVecdataStmt, SQL_INSERT_VECDATA, strlen(SQL_INSERT_VECDATA)))
        throw cRuntimeError("MySQL error: prepare statement failed: %s", mysql_error(mysql));
    ASSERT(mysql_stmt_param_count(insertVecdataStmt) == sizeof(insVecdataBind)/sizeof(MYSQL_BIND));

    opp_mysql_bindLONG(insVecdataBind[0], vectoridBuf);  // vecdata.vectorid
    opp_mysql_bindDOUBLE(insVecdataBind[1], timeBuf);  // vecdata.time
    opp_mysql_bindDOUBLE(insVecdataBind[2], valueBuf);  // vecdata.value

    if (mysql_stmt_bind_param(insertVecdataStmt, insVecdataBind))
        throw cRuntimeError("MySQL error: bind failed: %s", mysql_error(mysql));
}

void cMySQLOutputVectorManager::closeDB()
{
    if (insertVectorStmt)
        mysql_stmt_close(insertVectorStmt);
    if (insertVecAttrStmt)
        mysql_stmt_close(insertVecAttrStmt);
    if (insertVecdataStmt)
        mysql_stmt_close(insertVecdataStmt);
    insertVectorStmt = insertVecdataStmt = insertVecAttrStmt = nullptr;

    if (mysql)
        mysql_close(mysql);
    mysql = nullptr;
}

void cMySQLOutputVectorManager::commitDB()
{
    if (mysql_commit(mysql))
        throw cRuntimeError("MySQL error: COMMIT failed: %s", mysql_error(mysql));
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
        throw cRuntimeError("MySQL error: INSERT failed: %s", mysql_error(mysql));

    // query the automatic vectorid from the newly inserted row
    // Note: this INSERT must not be DELAYED, otherwise we get zero here.
    vp->id = (long)mysql_stmt_insert_id(insertVectorStmt);

    // write attributes:
    vectoridBuf = vp->id;
    for (opp_string_map::iterator it = vp->attributes.begin(); it != vp->attributes.end(); ++it) {
        opp_mysql_assignSTRING(insVecAttrBind[1], vecAttrNameBuf, it->first.c_str());
        opp_mysql_assignSTRING(insVecAttrBind[2], vecAttrValueBuf, it->second.c_str());
        if (mysql_stmt_execute(insertVecAttrStmt))
            throw cRuntimeError("MySQL error: INSERT failed: %s", mysql_error(mysql));
    }

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
    if (mysql) {
        commitDB();
        closeDB();
    }
}

void cMySQLOutputVectorManager::insertRunIntoDB()
{
    if (!initialized) {
        // insert run into the database
        std::string insertRunStmt = SQL_INSERT_VRUN;
        int runNumber = getEnvir()->getConfigEx()->getActiveRunNumber();
        opp_mysql_substitute(insertRunStmt, "@runnumber@", runNumber, mysql);
        opp_mysql_substitute(insertRunStmt, "@network@", getSimulation()->getNetworkType()->getName(), mysql);
        if (mysql_query(mysql, insertRunStmt.c_str()))
            throw cRuntimeError("MySQL error: INSERT failed: %s", mysql_error(mysql));

        // query the automatic runid from the newly inserted row
        runId = (long)mysql_insert_id(mysql);

        initialized = true;
    }
}

void *cMySQLOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    // only create the data structure here -- we'll insert the entry into the
    // DB lazily, when first data gets written
    sVectorData *vp = createVectorData();
    vp->id = -1;  // we'll get it from the database
    vp->initialised = false;
    vp->modulename = modulename;
    vp->vectorname = vectorname;

    cFileOutputVectorManager::getOutVectorConfig(modulename, vectorname,
            vp->enabled, vp->recordEventNumbers, vp->intervals);  // FIXME...
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

void cMySQLOutputVectorManager::setVectorAttribute(void *vectorhandle, const char *name, const char *value)
{
    ASSERT(vectorhandle != nullptr);
    sVectorData *vp = (sVectorData *)vectorhandle;
    vp->attributes[name] = value;
}

bool cMySQLOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    sVectorData *vp = (sVectorData *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (vp->intervals.contains(t)) {
        if (!vp->initialised)
            initVector(vp);

        // fill in prepared statement parameters, and fire off the statement
        vectoridBuf = vp->id;
        timeBuf = SIMTIME_DBL(t);
        valueBuf = value;
        if (mysql_stmt_execute(insertVecdataStmt))
            throw cRuntimeError("MySQL error: INSERT failed: %s", mysql_error(mysql));

        // commit every once in a while
        if (++insertCount == commitFreq) {
            insertCount = 0;
            commitDB();
        }

        return true;
    }
    return false;
}

void cMySQLOutputVectorManager::flush()
{
    commitDB();
}

