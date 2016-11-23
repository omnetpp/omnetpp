//==========================================================================
//  SQLITEOUTPUTVECTORMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Authors: Andras Varga, Zoltan Bojthe
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/simkerneldefs.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include "common/opp_ctype.h"
#include "common/fileutil.h"
#include "common/stringutil.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/platdep/timeutil.h"
#include "omnetpp/platdep/platmisc.h"
#include "envir/envirbase.h"

#include "sqliteoutvectormgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

Register_Class(cSqliteOutputVectorManager);

#define DEFAULT_COMMIT_FREQ      "100000"

Register_PerRunConfigOption(CFGID_OUTPUT_VECTOR_DB, "output-vector-db", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.sqlite.vec", "Name for the output vector database.");
Register_PerRunConfigOption(CFGID_OUTPUT_VECTOR_DB_APPEND, "output-vector-db-append", CFG_BOOL, "false", "What to do when the output vector db already exists: append to it, or delete it and begin a new file (default).");
Register_GlobalConfigOption(CFGID_OUTPUT_VECTOR_DB_COMMIT_FREQ, "vector-db-commit-freq", CFG_INT, DEFAULT_COMMIT_FREQ, "COMMIT every n INSERTs, default=" DEFAULT_COMMIT_FREQ);

//TODO registered in fileoutvectormgr:
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING;
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING_INTERVALS;

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw cRuntimeError("Cannot write output vector file `%s'", fname.c_str())

//FIXME hack
#define opp_runtime_error cRuntimeError

static const char SQL_CREATE_TABLES[] =
        "PRAGMA foreign_keys = OFF; "
        "BEGIN IMMEDIATE TRANSACTION; "
        "CREATE TABLE if not exists run "
        "( "
            "runId       INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runName     TEXT NOT NULL, "
            "simtimeExp  INTEGER NOT NULL "
        "); "
        ""
        "CREATE TABLE if not exists runattr "
        "( "
            "runId       INTEGER  NOT NULL REFERENCES run(runId) on DELETE CASCADE, "
            "attrName    TEXT NOT NULL, "
            "attrValue   TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE if not exists runparam "
        "( "
            "runId       INTEGER  NOT NULL REFERENCES run(runId) on DELETE CASCADE, "
            "parName     TEXT NOT NULL, "
            "parValue    TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE if not exists scalar "
        "( "
            "scalarId      INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runId         INTEGER  NOT NULL REFERENCES run(runId) on DELETE CASCADE, "
            "moduleName    TEXT NOT NULL, "
            "scalarName    TEXT NOT NULL, "
            "scalarValue   REAL "       // NOT NULL removed, because sqlite converts NaN double value to NULL
        "); "
        ""
        "CREATE TABLE if not exists scalarattr "
        "( "
            "scalarId      INTEGER  NOT NULL REFERENCES scalar(scalarId) on DELETE CASCADE, "
            "attrName      TEXT NOT NULL, "
            "attrValue     TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE if not exists histogram "
        "( "
            "histId        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runId         INTEGER  NOT NULL REFERENCES run(runId) on DELETE CASCADE, "
            "moduleName    TEXT NOT NULL, "
            "histName      TEXT NOT NULL, "
            "histCount     INTEGER NOT NULL, "
            "histMean      REAL NOT NULL, "
            "histStddev    REAL NOT NULL, "
            "histSum       REAL NOT NULL, "
            "histSqrsum    REAL NOT NULL, "
            "histMin       REAL NOT NULL, "
            "histMax       REAL NOT NULL, "
            "histWeights          REAL, "
            "histWeightedSum      REAL, "
            "histSqrSumWeights    REAL, "
            "histWeightedSqrSum   REAL "
        "); "
        ""
        "CREATE TABLE if not exists histattr "
        "( "
            "histId        INTEGER  NOT NULL REFERENCES histogram(histId) on DELETE CASCADE, "
            "attrName      TEXT NOT NULL, "
            "attrValue     TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE if not exists histbin "
        "( "
            "histId        INTEGER  NOT NULL REFERENCES histogram(histId) on DELETE CASCADE, "
            "baseValue     NUMERIC NOT NULL, "
            "cellValue     INTEGER NOT NULL "
        "); "
        ""
        "CREATE TABLE if not exists vector "
        "( "
            "vectorId      INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runId         INTEGER  NOT NULL REFERENCES run(runId) on DELETE CASCADE, "
            "moduleName    TEXT NOT NULL, "
            "vectorName    TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE if not exists vectorattr "
        "( "
            "vectorId      INTEGER  NOT NULL REFERENCES vector(vectorId) on DELETE CASCADE, "
            "attrName      TEXT NOT NULL, "
            "attrValue     TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE if not exists vectordata "
        "( "
            "vectorId      INTEGER  NOT NULL REFERENCES vector(vectorId) on DELETE CASCADE, "
            "eventNumber   INTEGER NOT NULL, "
            "simtimeRaw    INTEGER NOT NULL, "
            "value         NUMERIC NOT NULL "
        "); "
        "COMMIT TRANSACTION; "
        ""
        "PRAGMA synchronous = OFF; "
        "PRAGMA journal_mode = TRUNCATE; "
        "PRAGMA cache_size = 100000; "
        "PRAGMA page_size = 16384; "
        ""
        "BEGIN IMMEDIATE TRANSACTION; "         //TODO use it later: at the first insert
        ""
;

cSqliteOutputVectorManager::cSqliteOutputVectorManager()
{
    runId = -1;
    db = nullptr;
    add_vector_stmt = nullptr;
    add_vector_attr_stmt = nullptr;
    add_vector_data_stmt = nullptr;
    commitFreq = 0;
    insertCount = 0;
}

cSqliteOutputVectorManager::~cSqliteOutputVectorManager()
{
    closeDb();
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void cSqliteOutputVectorManager::openDb()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    if (sqlite3_open(fname.c_str(), &db) != SQLITE_OK) {
        throw opp_runtime_error("Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        db = nullptr;
        return;
    }

    sqlite3_busy_timeout(db, 10000);    // max time [ms] for waiting to unlock database

    char *zErrMsg = nullptr;
    if (sqlite3_exec(db, SQL_CREATE_TABLES, callback, 0, &zErrMsg) != SQLITE_OK) {
        throw opp_runtime_error("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        db = nullptr;
    }
    commitFreq = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_VECTOR_DB_COMMIT_FREQ);
    insertCount = 0;
}

void cSqliteOutputVectorManager::closeDb()
{
    if (db) {
        sqlite3_finalize(add_vector_stmt);
        sqlite3_finalize(add_vector_attr_stmt);
        sqlite3_finalize(add_vector_data_stmt);
        char *zErrMsg = nullptr;
        if (sqlite3_exec(db, "COMMIT TRANSACTION;", callback, nullptr, &zErrMsg) != SQLITE_OK)
            throw opp_runtime_error("SQL error: %s\n", zErrMsg);
        if (sqlite3_exec(db, "PRAGMA journal_mode = DELETE; ", callback, nullptr, &zErrMsg) != SQLITE_OK) {
            throw opp_runtime_error("SQL error at PRAGMA journal_mode = DELETE: %s\n", zErrMsg);
        }
        sqlite3_close(db);
        runId = -1;
        db = nullptr;
        add_vector_stmt = nullptr;
        add_vector_attr_stmt = nullptr;
        add_vector_data_stmt = nullptr;
    }
}

void cSqliteOutputVectorManager::commitDb()
{
    if (db) {
        char *zErrMsg = nullptr;
        if (sqlite3_exec(db, "COMMIT TRANSACTION;", callback, nullptr, &zErrMsg) != SQLITE_OK)
            throw opp_runtime_error("SQL error: %s\n", zErrMsg);
        if (sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION;", callback, nullptr, &zErrMsg) != SQLITE_OK)
            throw opp_runtime_error("SQL error: %s\n", zErrMsg);
    }
}


void cSqliteOutputVectorManager::writeRunData()
{
    run.initRun();
    sqlite3_stmt *stmt = nullptr;
    //TODO check and use existing run based on runName
    // save run
    if (sqlite3_prepare_v2(db, "insert into run (runname, simtimeExp) values (?, ?);", -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("database error: %s\n", sqlite3_errmsg(db));
    if (sqlite3_bind_text(stmt, 1, run.runId.c_str(), run.runId.size(), SQLITE_STATIC) != SQLITE_OK)
        throw opp_runtime_error("database error: %s\n", sqlite3_errmsg(db));
    if (sqlite3_bind_int(stmt, 2, SimTime::getScaleExp()) != SQLITE_OK)
        throw opp_runtime_error("database error: %s\n", sqlite3_errmsg(db));
    if (sqlite3_step(stmt) != SQLITE_DONE)
        throw opp_runtime_error("database error: %s\n", sqlite3_errmsg(db));
    sqlite3_clear_bindings(stmt);
    runId = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);

    // save run attributes
    if (sqlite3_prepare_v2(db, "insert into runattr (runid, attrname, attrvalue) values (?, ?, ?);", -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("database error: %s\n", sqlite3_errmsg(db));
    for (opp_string_map::iterator it = run.attributes.begin(); it != run.attributes.end(); ++it) {
        sqlite3_reset(stmt);
        sqlite3_bind_int64(stmt, 1, runId);
        sqlite3_bind_text(stmt, 2, it->first.c_str(), it->first.size(), SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, it->second.c_str(), it->second.size(), SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE)
            throw opp_runtime_error("database error: %s\n", sqlite3_errmsg(db));
        sqlite3_clear_bindings(stmt);
    }
    sqlite3_finalize(stmt);

    // save run params
    if (sqlite3_prepare_v2(db, "insert into runparam (runid, parname, parvalue) values (?, ?, ?);", -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("database error: %s\n", sqlite3_errmsg(db));
    for (opp_string_map::iterator it = run.moduleParams.begin(); it != run.moduleParams.end(); ++it) {
        sqlite3_reset(stmt);
        sqlite3_bind_int64(stmt, 1, runId);
        sqlite3_bind_text(stmt, 2, it->first.c_str(), it->first.size(), SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, it->second.c_str(), it->second.size(), SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE)
            throw opp_runtime_error("database error: %s\n", sqlite3_errmsg(db));
        sqlite3_clear_bindings(stmt);
    }
    sqlite3_finalize(stmt);

    // save iteration variables
    std::vector<const char *> v = getEnvir()->getConfigEx()->getIterationVariableNames();
    for (int i = 0; i < (int)v.size(); i++) {
        const char *name = v[i];
        const char *value = getEnvir()->getConfigEx()->getVariable(v[i]);
//        recordNumericIterationVariable(name, value);  //FIXME implemented in sqliteoutscalarmgr.cc
    }

    // prepare sql statements
    if (sqlite3_prepare_v2(db, "insert into vector (runid, moduleName, vectorName) values (?, ?, ?);", -1, &add_vector_stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("database error: %s\n", sqlite3_errmsg(db));
    if (sqlite3_prepare_v2(db, "insert into vectorattr (vectorId, attrname, attrvalue) values (?, ?, ?);", -1, &add_vector_attr_stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("database error: %s\n", sqlite3_errmsg(db));
    if (sqlite3_prepare_v2(db, "insert into vectordata (vectorId, eventNumber, simtimeRaw, value) values (?, ?, ?, ?);", -1, &add_vector_data_stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("database error: %s\n", sqlite3_errmsg(db));
}

void cSqliteOutputVectorManager::initVector(VectorData *vp)
{
    if (!db) {
        openDb();
        if (!db)
            return;
    }

    if (!run.initialized) {
        // this is the first vector written in this run, write out run attributes
        writeRunData();
    }

    sqlite3_reset(add_vector_stmt);
    sqlite3_bind_int64(add_vector_stmt, 1, runId);
    sqlite3_bind_text(add_vector_stmt, 2, vp->moduleName.c_str(), vp->moduleName.size(), SQLITE_STATIC);
    sqlite3_bind_text(add_vector_stmt, 3, vp->vectorName.c_str(), vp->vectorName.size(), SQLITE_STATIC);
    sqlite3_step(add_vector_stmt);
    sqlite3_clear_bindings(add_vector_stmt);
    vp->id = sqlite3_last_insert_rowid(db);

    for (opp_string_map::iterator it = vp->attributes.begin(); it != vp->attributes.end(); ++it) {
        sqlite3_reset(add_vector_attr_stmt);
        sqlite3_bind_int64(add_vector_attr_stmt, 1, vp->id);
        sqlite3_bind_text(add_vector_attr_stmt, 2, it->first.c_str(), it->first.size(), SQLITE_STATIC);
        sqlite3_bind_text(add_vector_attr_stmt, 3, it->second.c_str(), it->second.size(), SQLITE_STATIC);
        sqlite3_step(add_vector_attr_stmt);
        sqlite3_clear_bindings(add_vector_attr_stmt);
    }
    vp->initialized = true;
}

void cSqliteOutputVectorManager::startRun()
{
    // clean up file from previous runs
    closeDb();
    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_VECTOR_DB).c_str();
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    if (getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_VECTOR_DB_APPEND) == false) {
        removeFile(fname.c_str(), "old sqlite output vector file");
    }

    // clear run data
    run.reset();
}

void cSqliteOutputVectorManager::endRun()
{
    closeDb();
}

void cSqliteOutputVectorManager::getOutVectorConfig(const char *modname, const char *vecname,
        bool& outEnabled, Intervals& outIntervals)
{
    std::string vectorfullpath = std::string(modname) + "." + vecname;
    outEnabled = getEnvir()->getConfig()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING);

    // get interval string
    const char *text = getEnvir()->getConfig()->getAsCustom(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING_INTERVALS);
    if (text)
        outIntervals.parse(text);
}

void *cSqliteOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    VectorData *vp = createVectorData();
    vp->id = -1;
    vp->initialized = false;
    vp->moduleName = modulename;
    vp->vectorName = vectorname;
    getOutVectorConfig(modulename, vectorname, vp->enabled, vp->intervals);
    return vp;
}

cSqliteOutputVectorManager::VectorData *cSqliteOutputVectorManager::createVectorData()
{
    return new VectorData;
}

void cSqliteOutputVectorManager::deregisterVector(void *vectorhandle)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    delete vp;
}

void cSqliteOutputVectorManager::setVectorAttribute(void *vectorhandle, const char *name, const char *value)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    vp->attributes[name] = value;
}

bool cSqliteOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (vp->intervals.contains(t)) {
        if (!vp->initialized)
            initVector(vp);

        assert(db != nullptr);
        sqlite3_reset(add_vector_data_stmt);
        sqlite3_bind_int64(add_vector_data_stmt, 1, vp->id);
        sqlite3_bind_int64(add_vector_data_stmt, 2, getSimulation()->getEventNumber());
        sqlite3_bind_int64(add_vector_data_stmt, 3, t.raw());
        sqlite3_bind_double(add_vector_data_stmt, 4, value);
        sqlite3_step(add_vector_data_stmt);
//        sqlite3_clear_bindings(add_vector_data_stmt);

        // commit every once in a while
        if (++insertCount >= commitFreq) {
            insertCount = 0;
            commitDb();
        }

        return true;
    }
    return false;
}

const char *cSqliteOutputVectorManager::getFileName() const
{
    return fname.c_str();
}

void cSqliteOutputVectorManager::flush()
{
    commitDb();
}

}  // namespace envir
}  // namespace omnetpp

