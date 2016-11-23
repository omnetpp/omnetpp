//==========================================================================
//  SQLITEBUFFEREDOUTPUTVECTORMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Authors: Andras Varga, Zoltan Bojthe
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2016 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/simkerneldefs.h"

#include <algorithm>
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

#include "sqlitebufferedoutvectormgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

Register_Class(cSqliteBufferedOutputVectorManager);

#define MIN_VECTORITEM_LIMIT 10000
#define DEFAULT_VECTORITEM_LIMIT   "1000000"

#define LL  INT64_PRINTF_FORMAT

//TODO registered in sqliteoutvectormgr:
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_DB;
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_DB_APPEND;

//
Register_PerRunConfigOption(CFGID_OUTPUT_VECTOR_DB_TOTAL_MAX_BUFFERED_VALUES, "output-vector-db-total-max-buffered-values", CFG_INT, DEFAULT_VECTORITEM_LIMIT, "Total memory that can be used for buffering output vectors. Larger values produce less fragmented vector files (i.e. cause vector data to be grouped into larger chunks), and therefore allow more efficient processing later.");
Register_PerObjectConfigOption(CFGID_OUTPUT_VECTOR_DB_MAX_BUFFERED_VALUES_PER_VECTOR, "output-vector-db-max-buffered-values-per-vector", KIND_VECTOR, CFG_INT, nullptr, "For output vectors: the maximum number of values to buffer per vector, before writing out a block into the output vector file. The default is no per-vector limit (i.e. only the total limit is in effect)");

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
;

cSqliteBufferedOutputVectorManager::cSqliteBufferedOutputVectorManager()
{
    runId = -1;
    db = nullptr;
    add_vector_stmt = nullptr;
    add_vector_attr_stmt = nullptr;
    add_vector_data_stmt = nullptr;
    maxBufferedSamples = 0;
    bufferedSamples = 0;
    long d = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_VECTOR_DB_TOTAL_MAX_BUFFERED_VALUES);
    maxBufferedSamples = std::max(d, (long)MIN_VECTORITEM_LIMIT);

}

cSqliteBufferedOutputVectorManager::~cSqliteBufferedOutputVectorManager()
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

void cSqliteBufferedOutputVectorManager::openDb()
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
        throw opp_runtime_error("SQL error at CREATE TABLES: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        db = nullptr;
    }
}

void cSqliteBufferedOutputVectorManager::closeDb()
{
    if (db) {
        char *zErrMsg = nullptr;
        sqlite3_finalize(add_vector_stmt);
        sqlite3_finalize(add_vector_attr_stmt);
        sqlite3_finalize(add_vector_data_stmt);
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

void cSqliteBufferedOutputVectorManager::commitDb()
{
}


void cSqliteBufferedOutputVectorManager::writeRunData()
{
    run.initRun();
    sqlite3_stmt *stmt = nullptr;
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

void cSqliteBufferedOutputVectorManager::initVector(VectorData *vp)
{
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

void cSqliteBufferedOutputVectorManager::finalizeVector(VectorData *vp)
{
    if (db) {
        if (!vp->buffer.empty())
            writeOneBlock(vp);
    }
}


void cSqliteBufferedOutputVectorManager::startRun()
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

void cSqliteBufferedOutputVectorManager::endRun()
{
    writeRecords();
    vectors.clear();
    closeDb();
}

void *cSqliteBufferedOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    VectorData *vp = createVectorData();
    vp->id = -1;
    vp->initialized = false;
    vp->moduleName = modulename;
    vp->vectorName = vectorname;
    std::string vectorfullpath = std::string(modulename) + "." + vectorname;
    vp->enabled = getEnvir()->getConfig()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING);

    // get interval string
    const char *text = getEnvir()->getConfig()->getAsCustom(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING_INTERVALS);
    if (text)
        vp->intervals.parse(text);
    vp->maxBufferedSamples = getEnvir()->getConfig()->getAsInt(modulename, CFGID_OUTPUT_VECTOR_DB_MAX_BUFFERED_VALUES_PER_VECTOR);
    if (vp->maxBufferedSamples > 0)
        vp->allocateBuffer(vp->maxBufferedSamples);
    vectors.push_back(vp);
    return vp;
}

cSqliteBufferedOutputVectorManager::VectorData *cSqliteBufferedOutputVectorManager::createVectorData()
{
    return new VectorData;
}

void cSqliteBufferedOutputVectorManager::deregisterVector(void *vectorhandle)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    Vectors::iterator newEnd = std::remove(vectors.begin(), vectors.end(), vp);
    vectors.erase(newEnd, vectors.end());
    finalizeVector(vp);
    delete vp;
}

void cSqliteBufferedOutputVectorManager::setVectorAttribute(void *vectorhandle, const char *name, const char *value)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    vp->attributes[name] = value;
}

bool cSqliteBufferedOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (vp->intervals.contains(t)) {
        eventnumber_t eventNumber = getSimulation()->getEventNumber();
        vp->buffer.push_back(Sample(t, eventNumber, value));
        this->bufferedSamples++;

        if (vp->maxBufferedSamples > 0 && (int)vp->buffer.size() >= vp->maxBufferedSamples)
            writeOneBlock(vp);
        else if (bufferedSamples >= maxBufferedSamples)
            writeRecords();

        return true;
    }
    return false;
}

const char *cSqliteBufferedOutputVectorManager::getFileName() const
{
    return fname.c_str();
}

void cSqliteBufferedOutputVectorManager::flush()
{
    writeRecords();
}

// empties all buffer
void cSqliteBufferedOutputVectorManager::writeRecords()
{
    if (!db) {
        openDb();
        if (!run.initialized) {
            // this is the first vector written in this run, write out run attributes
            writeRunData();
        }
    }
    char *zErrMsg = nullptr;
    if (sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION;", callback, nullptr, &zErrMsg) != SQLITE_OK)
        throw opp_runtime_error("SQL error at BEGIN TRANSACTION: %s\n", zErrMsg);
    for (Vectors::iterator it = vectors.begin(); it != vectors.end(); ++it) {
        if (!(*it)->buffer.empty())  // TODO: size() > configured limit
            writeBlock(*it);
    }
    if (sqlite3_exec(db, "COMMIT TRANSACTION;", callback, nullptr, &zErrMsg) != SQLITE_OK)
        throw opp_runtime_error("SQL error at COMMIT TRANSACTION: %s\n", zErrMsg);
}

void cSqliteBufferedOutputVectorManager::writeOneBlock(VectorData *vp)
{
    if (!db) {
        openDb();
        if (!run.initialized) {
            // this is the first vector written in this run, write out run attributes
            writeRunData();
        }
    }
    char *zErrMsg = nullptr;
    if (sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION;", callback, nullptr, &zErrMsg) != SQLITE_OK)
        throw opp_runtime_error("SQL error at BEGIN TRANSACTION: %s\n", zErrMsg);

    writeBlock(vp);

    if (sqlite3_exec(db, "COMMIT TRANSACTION;", callback, nullptr, &zErrMsg) != SQLITE_OK)
        throw opp_runtime_error("SQL error at COMMIT TRANSACTION: %s\n", zErrMsg);
}

void cSqliteBufferedOutputVectorManager::writeBlock(VectorData *vp)
{
    assert(vp != nullptr);
    assert(!vp->buffer.empty());

    if (!vp->initialized)
        initVector(vp);

    assert(db != nullptr);

    for (Samples::iterator it = vp->buffer.begin(); it != vp->buffer.end(); ++it) {
        sqlite3_reset(add_vector_data_stmt);
        sqlite3_bind_int64(add_vector_data_stmt, 1, vp->id);
        sqlite3_bind_int64(add_vector_data_stmt, 2, it->eventNumber);
        sqlite3_bind_int64(add_vector_data_stmt, 3, it->simtime.raw());
        sqlite3_bind_double(add_vector_data_stmt, 4, it->value);
        sqlite3_step(add_vector_data_stmt);
    }
    bufferedSamples -= vp->buffer.size();
    vp->buffer.clear();
}

}  // namespace envir
}  // namespace omnetpp

