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
  Copyright (C) 2006-2016 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

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

#include "sqliteoutvectormgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

Register_Class(SqliteOutputVectorManager);

// global options
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_FILE_APPEND;
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_FILE;
extern omnetpp::cConfigOption *CFGID_OUTPUTVECTOR_MEMORY_LIMIT;

// per-vector options
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING;
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING_INTERVALS;
extern omnetpp::cConfigOption *CFGID_VECTOR_BUFFER;

Register_GlobalConfigOption(CFGID_OUTPUT_VECTOR_DB_INDEXING, "output-vector-db-indexing", CFG_CUSTOM, "skip", "Whether and when to add an index to the 'vectordata' table in SQLite output vector files. Possible values: skip, ahead, after");

//TODO move out table creation code into a common file!
/*
 * Performance notes:
 *  - file size: without index, about the same as with the traditional text-based format
 *  - index adds about 30-70% to the file size
 *  - raw recording performance: about half of text based recorder
 *  - with adding the index up front, total time is worse than with adding index after
 */

static const char SQL_CREATE_TABLES[] =
        "PRAGMA foreign_keys = OFF; "
        "BEGIN IMMEDIATE TRANSACTION; "
        "CREATE TABLE IF NOT EXISTS run "
        "( "
            "runId       INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runName     TEXT NOT NULL, "
            "simtimeExp  INTEGER NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS runattr "
        "( "
            "runId       INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE, "
            "attrName    TEXT NOT NULL, "
            "attrValue   TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS runparam "
        "( "
            "runId       INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE, "
            "parName     TEXT NOT NULL, "
            "parValue    TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS scalar "
        "( "
            "scalarId      INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runId         INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE, "
            "moduleName    TEXT NOT NULL, "
            "scalarName    TEXT NOT NULL, "
            "scalarValue   REAL "       // NOT NULL removed, because sqlite converts NaN double value to NULL
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS scalarattr "
        "( "
            "scalarId      INTEGER  NOT NULL REFERENCES scalar(scalarId) ON DELETE CASCADE, "
            "attrName      TEXT NOT NULL, "
            "attrValue     TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS histogram "
        "( "
            "histId        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runId         INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE, "
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
        "CREATE TABLE IF NOT EXISTS histattr "
        "( "
            "histId        INTEGER  NOT NULL REFERENCES histogram(histId) ON DELETE CASCADE, "
            "attrName      TEXT NOT NULL, "
            "attrValue     TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS histbin "
        "( "
            "histId        INTEGER  NOT NULL REFERENCES histogram(histId) ON DELETE CASCADE, "
            "baseValue     REAL NOT NULL, "
            "cellValue     INTEGER NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS vector "
        "( "
            "vectorId        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runId           INTEGER  NOT NULL REFERENCES run(runId) on DELETE CASCADE, "
            "moduleName      TEXT NOT NULL, "
            "vectorName      TEXT NOT NULL, "
            "vectorCount     INTEGER, "   // cannot be NOT NULL because we fill it in later
            "vectorMin       REAL, "
            "vectorMax       REAL, "
            "vectorSum       REAL, "
            "vectorSumSqr    REAL, "
            "startEventNum   INTEGER, "
            "endEventNum     INTEGER, "
            "startSimtimeRaw INTEGER, "
            "endSimtimeRaw   INTEGER "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS vectorattr "
        "( "
            "vectorId      INTEGER  NOT NULL REFERENCES vector(vectorId) ON DELETE CASCADE, "
            "attrName      TEXT NOT NULL, "
            "attrValue     TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS vectordata "
        "( "
            "vectorId      INTEGER  NOT NULL REFERENCES vector(vectorId) ON DELETE CASCADE, "
            "eventNumber   INTEGER NOT NULL, "
            "simtimeRaw    INTEGER NOT NULL, "
            "value         REAL NOT NULL "
        "); "
        "COMMIT TRANSACTION; "
        ""
        "PRAGMA synchronous = OFF; "
        "PRAGMA journal_mode = TRUNCATE; "
        "PRAGMA cache_size = 100000; "
        "PRAGMA page_size = 16384; "
;

SqliteOutputVectorManager::SqliteOutputVectorManager()
{
    indexingMode = INDEX_NONE; // an index can be added later
    initialized = false;
    runId = -1;
    db = nullptr;
    stmt = nullptr;
    add_vector_stmt = nullptr;
    add_vector_attr_stmt = nullptr;
    add_vector_data_stmt = nullptr;
    update_vector_stmt = nullptr;
    maxBufferedSamples = 0;
    bufferedSamples = 0;
    long totalBufferSize = (long) getEnvir()->getConfig()->getAsDouble(CFGID_OUTPUTVECTOR_MEMORY_LIMIT);
    maxBufferedSamples = totalBufferSize;
    std::string indexModeStr = getEnvir()->getConfig()->getAsCustom(CFGID_OUTPUT_VECTOR_DB_INDEXING);
    if (indexModeStr == "skip")
        indexingMode = INDEX_NONE;
    else if (indexModeStr == "ahead")
        indexingMode = INDEX_AHEAD;
    else if (indexModeStr == "after")
        indexingMode = INDEX_AFTER;
    else
        throw cRuntimeError("Invalid value '%s' for '%s', expecting 'skip', 'ahead' or 'after'",
                indexModeStr.c_str(), CFGID_OUTPUT_VECTOR_DB_INDEXING->getName());
}

SqliteOutputVectorManager::~SqliteOutputVectorManager()
{
    cleanupDb(); // not closeDb() because it throws; also, closeDb() must have been called already if there was no error
}

inline void SqliteOutputVectorManager::checkOK(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_OK)
        error(sqlite3_errmsg(db));
}

inline void SqliteOutputVectorManager::checkDone(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_DONE)
        error(sqlite3_errmsg(db));
}

void SqliteOutputVectorManager::error(const char *errmsg)
{
    std::string msg = errmsg ? errmsg : "unknown SQLite error";
    cleanupDb();
    throw cRuntimeError("Cannot record results into '%s': %s", fname.c_str(), msg.c_str());
}

void SqliteOutputVectorManager::openDb()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    checkOK(sqlite3_open(fname.c_str(), &db));
    sqlite3_busy_timeout(db, 10000);    // max time [ms] for waiting to unlock database
    executeSql(SQL_CREATE_TABLES);
    if (indexingMode == INDEX_AHEAD)
        createIndex();
}

void SqliteOutputVectorManager::closeDb()
{
    if (db) {
        finalizeStatement(stmt);
        finalizeStatement(add_vector_stmt);
        finalizeStatement(add_vector_attr_stmt);
        finalizeStatement(add_vector_data_stmt);
        finalizeStatement(update_vector_stmt);

        executeSql("PRAGMA journal_mode = DELETE;");
        checkOK(sqlite3_close(db));

        runId = -1;
        db = nullptr;
    }
}

void SqliteOutputVectorManager::cleanupDb() // MUST NOT THROW
{
    if (db) {
        finalizeStatement(stmt);
        finalizeStatement(add_vector_stmt);
        finalizeStatement(add_vector_attr_stmt);
        finalizeStatement(add_vector_data_stmt);
        finalizeStatement(update_vector_stmt);

        sqlite3_exec(db, "PRAGMA journal_mode = DELETE; ", nullptr, nullptr, nullptr); // note: NOT executeSql()! it would throw
        sqlite3_close(db); // note: no checkOK()! it would throw

        runId = -1;
        db = nullptr;
    }
}

void SqliteOutputVectorManager::createIndex()
{
    executeSql("CREATE INDEX IF NOT EXISTS vectordata_idx ON vectordata (vectorId);");
}

void SqliteOutputVectorManager::executeSql(const char *sql)
{
    checkOK(sqlite3_exec(db, sql, nullptr, nullptr, nullptr));
}

void SqliteOutputVectorManager::prepareStatement(sqlite3_stmt *&stmt, const char *sql)
{
    checkOK(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr));
}

void SqliteOutputVectorManager::finalizeStatement(sqlite3_stmt *&stmt)
{
    sqlite3_finalize(stmt); // note: no checkOK() needed, see sqlite3_finalize() docu
    stmt = nullptr; // prevent use-after-finalize
}

void SqliteOutputVectorManager::writeRunData()
{
    run.initRun();

    // prepare sql statements
    prepareStatement(add_vector_stmt, "INSERT INTO vector (runid, moduleName, vectorName) VALUES (?, ?, ?);");
    prepareStatement(add_vector_attr_stmt, "INSERT INTO vectorattr (vectorId, attrname, attrvalue) VALUES (?, ?, ?);");
    prepareStatement(add_vector_data_stmt, "INSERT INTO vectordata (vectorId, eventNumber, simtimeRaw, value) VALUES (?, ?, ?, ?);");

    // save run
    prepareStatement(stmt, "INSERT INTO run (runname, simtimeExp) VALUES (?, ?);");
    checkOK(sqlite3_bind_text(stmt, 1, run.runId.c_str(), run.runId.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_int(stmt, 2, SimTime::getScaleExp()));
    checkDone(sqlite3_step(stmt));
    checkOK(sqlite3_clear_bindings(stmt));
    runId = sqlite3_last_insert_rowid(db);
    finalizeStatement(stmt);

    // save run attributes
    prepareStatement(stmt, "INSERT INTO runattr (runid, attrname, attrvalue) VALUES (?, ?, ?);");
    for (opp_string_map::iterator it = run.attributes.begin(); it != run.attributes.end(); ++it) {
        checkOK(sqlite3_reset(stmt));
        checkOK(sqlite3_bind_int64(stmt, 1, runId));
        checkOK(sqlite3_bind_text(stmt, 2, it->first.c_str(), it->first.size(), SQLITE_STATIC));
        checkOK(sqlite3_bind_text(stmt, 3, it->second.c_str(), it->second.size(), SQLITE_STATIC));
        checkDone(sqlite3_step(stmt));
        checkOK(sqlite3_clear_bindings(stmt));
    }
    finalizeStatement(stmt);

    // save run params
    prepareStatement(stmt, "INSERT INTO runparam (runid, parname, parvalue) VALUES (?, ?, ?);");
    for (opp_string_map::iterator it = run.moduleParams.begin(); it != run.moduleParams.end(); ++it) {
        checkOK(sqlite3_reset(stmt));
        checkOK(sqlite3_bind_int64(stmt, 1, runId));
        checkOK(sqlite3_bind_text(stmt, 2, it->first.c_str(), it->first.size(), SQLITE_STATIC));
        checkOK(sqlite3_bind_text(stmt, 3, it->second.c_str(), it->second.size(), SQLITE_STATIC));
        checkDone(sqlite3_step(stmt));
        checkOK(sqlite3_clear_bindings(stmt));
    }
    finalizeStatement(stmt);
}

void SqliteOutputVectorManager::initVector(VectorData *vp)
{
    checkOK(sqlite3_reset(add_vector_stmt));
    checkOK(sqlite3_bind_int64(add_vector_stmt, 1, runId));
    checkOK(sqlite3_bind_text(add_vector_stmt, 2, vp->moduleName.c_str(), vp->moduleName.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_vector_stmt, 3, vp->vectorName.c_str(), vp->vectorName.size(), SQLITE_STATIC));
    checkDone(sqlite3_step(add_vector_stmt));
    checkOK(sqlite3_clear_bindings(add_vector_stmt));
    vp->id = sqlite3_last_insert_rowid(db);

    for (opp_string_map::iterator it = vp->attributes.begin(); it != vp->attributes.end(); ++it) {
        checkOK(sqlite3_reset(add_vector_attr_stmt));
        checkOK(sqlite3_bind_int64(add_vector_attr_stmt, 1, vp->id));
        checkOK(sqlite3_bind_text(add_vector_attr_stmt, 2, it->first.c_str(), it->first.size(), SQLITE_STATIC));
        checkOK(sqlite3_bind_text(add_vector_attr_stmt, 3, it->second.c_str(), it->second.size(), SQLITE_STATIC));
        checkDone(sqlite3_step(add_vector_attr_stmt));
        checkOK(sqlite3_clear_bindings(add_vector_attr_stmt));
    }
    vp->initialized = true;
}

void SqliteOutputVectorManager::finalizeVector(VectorData *vp)
{
    if (!vp->initialized && vp->buffer.empty()) {  // no data in this vector
        vp->finalized = true;
        return;
    }

    Assert(!vp->finalized);  // note: db may be nullptr here (not yet opened)

    if (!vp->buffer.empty())
        writeOneBlock(vp);

    Assert(db != nullptr);

    // record vector statistics
    if (update_vector_stmt == nullptr) {
        prepareStatement(update_vector_stmt, "UPDATE vector "
                "SET startEventNum=?, endEventNum=?, startSimtimeRaw=?, endSimtimeRaw=?, "
                "vectorCount=?, vectorMin=?, vectorMax=?, vectorSum=?, vectorSumSqr=? "
                "WHERE vectorId=?;");
    }
    executeSql("BEGIN IMMEDIATE TRANSACTION;");
    checkOK(sqlite3_reset(update_vector_stmt));
    checkOK(sqlite3_bind_int64(update_vector_stmt, 1, vp->startEventNum));
    checkOK(sqlite3_bind_int64(update_vector_stmt, 2, vp->endEventNum));
    checkOK(sqlite3_bind_int64(update_vector_stmt, 3, vp->startTime.raw()));
    checkOK(sqlite3_bind_int64(update_vector_stmt, 4, vp->endTime.raw()));
    checkOK(sqlite3_bind_int64(update_vector_stmt, 5, vp->count));
    checkOK(sqlite3_bind_double(update_vector_stmt, 6, vp->min));
    checkOK(sqlite3_bind_double(update_vector_stmt, 7, vp->max));
    checkOK(sqlite3_bind_double(update_vector_stmt, 8, vp->sum));
    checkOK(sqlite3_bind_double(update_vector_stmt, 9, vp->sumSqr));
    checkOK(sqlite3_bind_int64(update_vector_stmt, 10, vp->id));
    checkDone(sqlite3_step(update_vector_stmt));
    checkOK(sqlite3_clear_bindings(update_vector_stmt));
    executeSql("COMMIT TRANSACTION;");

    vp->finalized = true;
}

void SqliteOutputVectorManager::startRun()
{
    // clean up file from previous runs
    //initialized = false;
    //bufferedSamples = 0;
    //vectors.clear(); TODO clearing the remaining vector SHOULD be done after deleteNetwork()! because endRun may not be called at all
    //closeDb();

    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_VECTOR_FILE).c_str();
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    if (getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_VECTOR_FILE_APPEND) == false)
        removeFile(fname.c_str(), "old SQLite output vector file");

    // clear run data
    run.reset();
}

void SqliteOutputVectorManager::endRun()
{
    for (Vectors::iterator it = vectors.begin(); it != vectors.end(); ++it) {
        VectorData *vp = *it;
        if (!vp->finalized)
            finalizeVector(vp); //TODO currently these all go in separate transactions
    }

    if (indexingMode == INDEX_AFTER) {
        time_t startTime = time(nullptr);
        createIndex();
        double elapsedSecs = time(nullptr) - startTime + 1; // +1 is for rounding up
        std::cout << "Indexing SQLite output vector file took about " << elapsedSecs << "s" << endl;
    }

    initialized = false;
    bufferedSamples = 0;
    vectors.clear();
    closeDb();
}

void *SqliteOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    //TODO assert: endRun() not yet called

    VectorData *vp = createVectorData();
    vp->id = -1;
    vp->initialized = false;
    vp->finalized = false;
    vp->moduleName = modulename;
    vp->vectorName = vectorname;
    std::string vectorfullpath = std::string(modulename) + "." + vectorname;
    vp->enabled = getEnvir()->getConfig()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING);

    // get interval string
    const char *text = getEnvir()->getConfig()->getAsCustom(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING_INTERVALS);
    if (text)
        vp->intervals.parse(text);

    long bufferSize = (long) getEnvir()->getConfig()->getAsDouble(modulename, CFGID_VECTOR_BUFFER);
    vp->maxBufferedSamples = bufferSize / sizeof(Sample);
    if (vp->maxBufferedSamples > 0)
        vp->allocateBuffer(vp->maxBufferedSamples);
    vectors.push_back(vp);
    return vp;
}

SqliteOutputVectorManager::VectorData *SqliteOutputVectorManager::createVectorData()
{
    return new VectorData;
}

void SqliteOutputVectorManager::deregisterVector(void *vectorhandle)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    Vectors::iterator newEnd = std::remove(vectors.begin(), vectors.end(), vp);
    vectors.erase(newEnd, vectors.end());
    if (vp->initialized && !vp->finalized)
        finalizeVector(vp);
    delete vp;
}

void SqliteOutputVectorManager::setVectorAttribute(void *vectorhandle, const char *name, const char *value)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    vp->attributes[name] = value;
}

bool SqliteOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;

    //TODO assert: startRun() called, but endRun() yet!
    Assert(!vp->finalized);

    if (!vp->enabled)
        return false;

    if (vp->intervals.contains(t)) {
        // store value
        eventnumber_t eventNumber = getSimulation()->getEventNumber();
        vp->buffer.push_back(Sample(t, eventNumber, value));
        this->bufferedSamples++;

        // write out block if necessary
        if (vp->maxBufferedSamples > 0 && (int)vp->buffer.size() >= vp->maxBufferedSamples)
            writeOneBlock(vp);
        else if (maxBufferedSamples > 0 && bufferedSamples >= maxBufferedSamples)
            writeRecords();

        // update vector statistics
        if (vp->count == 0) {
            vp->startEventNum = eventNumber;
            vp->startTime = t;
            vp->min = value;
            vp->max = value;
        }
        vp->endEventNum = eventNumber;
        vp->endTime = t;
        vp->count++;
        if (vp->min > value)
            vp->min = value;
        if (vp->max < value)
            vp->max = value;
        vp->sum += value;
        vp->sumSqr += value*value;

        return true;
    }
    return false;
}

const char *SqliteOutputVectorManager::getFileName() const
{
    return fname.c_str();
}

void SqliteOutputVectorManager::flush()
{
    writeRecords();
}

void SqliteOutputVectorManager::initialize()
{
    openDb();
    writeRunData();
}

void SqliteOutputVectorManager::writeRecords()
{
    if (!initialized) {
        initialized = true;
        initialize();
    }

    if (isBad())
        return;

    executeSql("BEGIN IMMEDIATE TRANSACTION;");
    for (Vectors::iterator it = vectors.begin(); it != vectors.end(); ++it)
        if (!(*it)->buffer.empty())
            writeBlock(*it);
    executeSql("COMMIT TRANSACTION;");
}

void SqliteOutputVectorManager::writeOneBlock(VectorData *vp)
{
    if (!initialized) {
        initialized = true;
        initialize();
    }

    if (isBad())
        return;

    executeSql("BEGIN IMMEDIATE TRANSACTION;");
    writeBlock(vp);
    executeSql("COMMIT TRANSACTION;");
}

void SqliteOutputVectorManager::writeBlock(VectorData *vp)
{
    ASSERT(vp != nullptr);
    ASSERT(!vp->buffer.empty());

    if (!vp->initialized)
        initVector(vp);

    ASSERT(db != nullptr);

    for (Samples::iterator it = vp->buffer.begin(); it != vp->buffer.end(); ++it) {
        checkOK(sqlite3_reset(add_vector_data_stmt));
        checkOK(sqlite3_bind_int64(add_vector_data_stmt, 1, vp->id));
        checkOK(sqlite3_bind_int64(add_vector_data_stmt, 2, it->eventNumber));
        checkOK(sqlite3_bind_int64(add_vector_data_stmt, 3, it->simtime.raw()));
        checkOK(sqlite3_bind_double(add_vector_data_stmt, 4, it->value));
        checkDone(sqlite3_step(add_vector_data_stmt));
    }
    bufferedSamples -= vp->buffer.size();
    vp->buffer.clear();
}

}  // namespace envir
}  // namespace omnetpp

