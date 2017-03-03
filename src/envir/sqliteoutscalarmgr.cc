//==========================================================================
//  SQLITEOUTPUTSCALARMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Zoltan Bojthe, Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <fstream>
#include <clocale>
#include "common/opp_ctype.h"
#include "common/fileutil.h"
#include "common/stringutil.h"
#include "common/unitconversion.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/cdensityestbase.h"
#include "omnetpp/ccomponenttype.h"
#include "envir/envirbase.h"
#include "sqliteoutscalarmgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

#define DEFAULT_COMMIT_FREQ      "100000"

// global options
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_FILE;
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_FILE_APPEND;

Register_GlobalConfigOption(CFGID_OUTPUT_SCALAR_DB_COMMIT_FREQ, "output-scalar-db-commit-freq", CFG_INT, DEFAULT_COMMIT_FREQ, "Used with SqliteOutputScalarManager: COMMIT every n INSERTs.");

// per-scalar options
extern omnetpp::cConfigOption *CFGID_SCALAR_RECORDING;
extern omnetpp::cConfigOption *CFGID_BIN_RECORDING;

Register_Class(SqliteOutputScalarManager);

static const char SQL_CREATE_TABLES[] =
        "PRAGMA foreign_keys = ON; "
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
        "CREATE TABLE IF NOT EXISTS statistic "
        "( "
            "statId        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runId         INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE, "
            "moduleName    TEXT NOT NULL, "
            "statName      TEXT NOT NULL, "
            "statCount     INTEGER NOT NULL, "
            "statMean      REAL, "
            "statStddev    REAL, "
            "statSum       REAL, "
            "statSqrsum    REAL, "
            "statMin       REAL, "
            "statMax       REAL, "
            "statWeights          REAL, "
            "statWeightedSum      REAL, "
            "statSqrSumWeights    REAL, "
            "statWeightedSqrSum   REAL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS histattr "
        "( "
            "statId        INTEGER  NOT NULL REFERENCES statistic(statId) ON DELETE CASCADE, "
            "attrName      TEXT NOT NULL, "
            "attrValue     TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS histbin "
        "( "
            "statId        INTEGER  NOT NULL REFERENCES statistic(statId) ON DELETE CASCADE, "
            "baseValue     NUMERIC NOT NULL, "
            "cellValue     INTEGER NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS vector "
        "( "
            "vectorId        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runId           INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE, "
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
            "value         NUMERIC NOT NULL "
        "); "
        ""
        "COMMIT TRANSACTION; "
        ""
        "PRAGMA synchronous = OFF; "
        "PRAGMA journal_mode = TRUNCATE; "
        "PRAGMA cache_size = 100000; "
        "PRAGMA page_size = 16384; "
;

SqliteOutputScalarManager::SqliteOutputScalarManager()
{
    initialized = false;
    runId = -1;
    db = nullptr;
    stmt = nullptr;
    add_scalar_stmt = nullptr;
    add_scalar_attr_stmt = nullptr;
    add_statistic_stmt = nullptr;
    add_statistic_attr_stmt = nullptr;
    add_statistic_bin_stmt = nullptr;

    commitFreq = 0;
    insertCount = 0;
}

SqliteOutputScalarManager::~SqliteOutputScalarManager()
{
    cleanupDb(); // not closeDb() because it throws; also, closeDb() must have been called already if there was no error
}

inline void SqliteOutputScalarManager::checkOK(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_OK)
        error(sqlite3_errmsg(db));
}

inline void SqliteOutputScalarManager::checkDone(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_DONE)
        error(sqlite3_errmsg(db));
}

void SqliteOutputScalarManager::error(const char *errmsg)
{
    std::string msg = errmsg ? errmsg : "unknown SQLite error";
    cleanupDb();
    throw cRuntimeError("Cannot record results into '%s': %s", fname.c_str(), msg.c_str());
}

void SqliteOutputScalarManager::openDb()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    checkOK(sqlite3_open(fname.c_str(), &db));

    checkOK(sqlite3_busy_timeout(db, 10000));    // max time [ms] for waiting to unlock database

    checkOK(sqlite3_exec(db, SQL_CREATE_TABLES, nullptr, 0, nullptr));
    checkOK(sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION;", nullptr, 0, nullptr));

    commitFreq = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_SCALAR_DB_COMMIT_FREQ);
    insertCount = 0;
}

void SqliteOutputScalarManager::closeDb()
{
    if (db) {
        finalizeStatement(stmt);
        finalizeStatement(add_scalar_stmt);
        finalizeStatement(add_scalar_attr_stmt);
        finalizeStatement(add_statistic_stmt);
        finalizeStatement(add_statistic_attr_stmt);
        finalizeStatement(add_statistic_bin_stmt);

        checkOK(sqlite3_exec(db, "COMMIT TRANSACTION;", nullptr, nullptr, nullptr));
        checkOK(sqlite3_exec(db, "PRAGMA journal_mode = DELETE;", nullptr, nullptr, nullptr));
        checkOK(sqlite3_close(db));

        runId = -1;
        db = nullptr;
    }
}

void SqliteOutputScalarManager::cleanupDb()  // MUST NOT THROW
{
    if (db) {
        finalizeStatement(stmt);
        finalizeStatement(add_scalar_stmt);
        finalizeStatement(add_scalar_attr_stmt);
        finalizeStatement(add_statistic_stmt);
        finalizeStatement(add_statistic_attr_stmt);
        finalizeStatement(add_statistic_bin_stmt);

        // note: no checkOK() because it would throw
        sqlite3_exec(db, "COMMIT TRANSACTION;", nullptr, nullptr, nullptr);
        sqlite3_exec(db, "PRAGMA journal_mode = DELETE;", nullptr, nullptr, nullptr);
        sqlite3_close(db);

        runId = -1;
        db = nullptr;
    }
}

void SqliteOutputScalarManager::prepareStatement(sqlite3_stmt *&stmt, const char *sql)
{
    checkOK(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr));
}

void SqliteOutputScalarManager::finalizeStatement(sqlite3_stmt *&stmt)
{
    sqlite3_finalize(stmt); // note: no checkOK() needed, see sqlite3_finalize() docu
    stmt = nullptr; // prevent use-after-finalize
}

void SqliteOutputScalarManager::startRun()
{
    // clean up file from previous runs
    initialized = false;
    closeDb();
    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_SCALAR_FILE);
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    if (getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_SCALAR_FILE_APPEND) == false)
        removeFile(fname.c_str(), "old SQLite output scalar file");
    run.reset();
}

void SqliteOutputScalarManager::endRun()
{
    closeDb();
    initialized = false;
}

void SqliteOutputScalarManager::initialize()
{
    openDb();
    writeRunData();
}

void SqliteOutputScalarManager::writeRunData()
{
    run.initRun();

    // prepare sql statements
    prepareStatement(add_scalar_stmt, "INSERT INTO scalar (runId, moduleName, scalarName, scalarValue) VALUES (?, ?, ?, ?);");
    prepareStatement(add_scalar_attr_stmt, "INSERT INTO scalarattr (scalarId, attrName, attrValue) VALUES (?, ?, ?);");
    prepareStatement(add_statistic_stmt,
            "INSERT INTO statistic (runId, moduleName, statName, "
            "statCount, "
            "statMean, statStddev, statSum, statSqrsum, statMin, statMax, "
            "statWeights, statWeightedSum, statSqrSumWeights, statWeightedSqrSum"
            ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
    prepareStatement(add_statistic_attr_stmt, "INSERT INTO histattr (statId, attrName, attrValue) VALUES (?, ?, ?);");
    prepareStatement(add_statistic_bin_stmt, "INSERT INTO histbin (statId, baseValue, cellValue) VALUES (?, ?, ?);");

    // save run
    prepareStatement(stmt, "INSERT INTO run (runname, simTimeExp) VALUES (?, ?);");
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

    // save numeric iteration variables as scalars as well, after saving them as run attributes (TODO this should not be necessary)
    std::vector<const char *> v = getEnvir()->getConfigEx()->getIterationVariableNames();
    for (int i = 0; i < (int)v.size(); i++) {
        const char *name = v[i];
        const char *value = getEnvir()->getConfigEx()->getVariable(v[i]);
        recordNumericIterationVariableAsScalar(name, value);
    }
}

void SqliteOutputScalarManager::recordNumericIterationVariableAsScalar(const char *name, const char *valueStr)
{
    static const std::string MODULE_FOR_RUNATTR_SCALAR("_runattrs_");
    char *e;
    setlocale(LC_NUMERIC, "C");
    double value = strtod(valueStr, &e);
    if (*e == '\0') {
        // plain number - just record as it is
        writeScalar(MODULE_FOR_RUNATTR_SCALAR, name, value);
    }
    else if (e != valueStr) {
        // starts with a number, so it might be something like "100s"; if so, record it as scalar with "unit" attribute
        std::string unit;
        bool parsedOK = false;
        try {
            value = UnitConversion::parseQuantity(valueStr, unit);
            parsedOK = true;
        }
        catch (std::exception& e) {
        }
        if (parsedOK) {
            sqlite3_int64 scalarId = writeScalar(MODULE_FOR_RUNATTR_SCALAR, name, value);
            if (!unit.empty())
                writeScalarAttr(scalarId, "unit", 4, unit.c_str(), unit.size());
        }
    }
}

sqlite_int64 SqliteOutputScalarManager::writeScalar(const std::string &componentFullPath, const char *name, double value)
{
    checkOK(sqlite3_reset(add_scalar_stmt));
    checkOK(sqlite3_bind_int64(add_scalar_stmt, 1, runId));
    checkOK(sqlite3_bind_text(add_scalar_stmt, 2, componentFullPath.c_str(), componentFullPath.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_scalar_stmt, 3, name, strlen(name), SQLITE_STATIC));
    checkOK(sqlite3_bind_double(add_scalar_stmt, 4, value));
    checkDone(sqlite3_step(add_scalar_stmt));
    checkOK(sqlite3_clear_bindings(add_scalar_stmt));
    sqlite3_int64 scalarId = sqlite3_last_insert_rowid(db);
    return scalarId;
}

void SqliteOutputScalarManager::writeScalarAttr(sqlite_int64 scalarId, const char *name, size_t nameLength, const char *value, size_t valueLength)
{
    checkOK(sqlite3_reset(add_scalar_attr_stmt));
    checkOK(sqlite3_bind_int64(add_scalar_attr_stmt, 1, scalarId));
    checkOK(sqlite3_bind_text(add_scalar_attr_stmt, 2, name, nameLength, SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_scalar_attr_stmt, 3, value, valueLength, SQLITE_STATIC));
    checkDone(sqlite3_step(add_scalar_attr_stmt));
    checkOK(sqlite3_clear_bindings(add_scalar_attr_stmt));
}

void SqliteOutputScalarManager::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    if (!initialized) {
        initialized = true;
        initialize();
    }

    if (isBad())
        return;

    if (!name || !name[0])
        name = "(unnamed)";

    std::string componentFullPath = component->getFullPath();
    bool enabled = getEnvir()->getConfig()->getAsBool((componentFullPath+"."+name).c_str(), CFGID_SCALAR_RECORDING);
    if (enabled) {
        sqlite3_int64 scalarId = writeScalar(componentFullPath, name, value);
        if (attributes) {
            for (opp_string_map::iterator it = attributes->begin(); it != attributes->end(); ++it)
                writeScalarAttr(scalarId, it->first.c_str(), it->first.size(), it->second.c_str(), it->second.size());
        }
    }

    // commit every once in a while
    if (++insertCount >= commitFreq) {
        insertCount = 0;
        commitAndBeginNew();
    }
}

void SqliteOutputScalarManager::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    static const double MinusInfinity = -1.0/0.0;

    if (!initialized) {
        initialized = true;
        initialize();
    }

    if (isBad())
        return;

    if (!name)
        name = statistic->getFullName();
    if (!name || !name[0])
        name = "(unnamed)";

    std::string fullPath = component->getFullPath();
    // check that recording this statistic is not disabled as a whole
    std::string objectFullPath = fullPath + "." + name;
    bool enabled = getEnvir()->getConfig()->getAsBool(objectFullPath.c_str(), CFGID_SCALAR_RECORDING);
    if (!enabled)
        return;

    checkOK(sqlite3_reset(add_statistic_stmt));
    checkOK(sqlite3_bind_int64(add_statistic_stmt, 1, runId));
    checkOK(sqlite3_bind_text(add_statistic_stmt, 2, fullPath.c_str(), fullPath.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_statistic_stmt, 3, name, strlen(name), SQLITE_STATIC));
    checkOK(sqlite3_bind_int64(add_statistic_stmt, 4, statistic->getCount()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 5, statistic->getMean()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 6, statistic->getStddev()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 7, statistic->getSum()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 8, statistic->getSqrSum()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 9, statistic->getMin()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 10, statistic->getMax()));
    if (statistic->isWeighted()) {
        checkOK(sqlite3_bind_double(add_statistic_stmt, 11, statistic->getWeights()));
        checkOK(sqlite3_bind_double(add_statistic_stmt, 12, statistic->getWeightedSum()));
        checkOK(sqlite3_bind_double(add_statistic_stmt, 13, statistic->getSqrSumWeights()));
        checkOK(sqlite3_bind_double(add_statistic_stmt, 14, statistic->getWeightedSqrSum()));
    }
    checkDone(sqlite3_step(add_statistic_stmt));
    sqlite3_int64 statisticId = sqlite3_last_insert_rowid(db);
    checkOK(sqlite3_clear_bindings(add_statistic_stmt));

    if (attributes)
        for (opp_string_map::iterator it = attributes->begin(); it != attributes->end(); ++it)
            writeStatisticAttr(statisticId, it->first.c_str(), it->second.c_str());

    if (cDensityEstBase *histogram = dynamic_cast<cDensityEstBase *>(statistic)) {
        // check that recording the histogram is enabled
        bool binsEnabled = getEnvir()->getConfig()->getAsBool(objectFullPath.c_str(), CFGID_BIN_RECORDING);
        if (binsEnabled) {
            if (!histogram->isTransformed())
                histogram->transform();

            int n = histogram->getNumCells();
            if (n > 0) {
                writeStatisticBin(statisticId, MinusInfinity, histogram->getUnderflowCell());
                for (int i = 0; i < n; i++)
                    writeStatisticBin(statisticId, histogram->getBasepoint(i), histogram->getCellValue(i));
                writeStatisticBin(statisticId, histogram->getBasepoint(n), histogram->getOverflowCell());
            }
        }
    }
}

void SqliteOutputScalarManager::writeStatisticAttr(sqlite_int64 statisticId, const char *name, const char *value)
{
    checkOK(sqlite3_reset(add_statistic_attr_stmt));
    checkOK(sqlite3_bind_int64(add_statistic_attr_stmt, 1, statisticId));
    checkOK(sqlite3_bind_text(add_statistic_attr_stmt, 2, name, strlen(name), SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_statistic_attr_stmt, 3, value, strlen(value), SQLITE_STATIC));
    checkDone(sqlite3_step(add_statistic_attr_stmt));
    checkOK(sqlite3_clear_bindings(add_statistic_attr_stmt));
}

void SqliteOutputScalarManager::writeStatisticBin(sqlite_int64 statisticId, double basePoint, unsigned long cellValue)
{
    checkOK(sqlite3_reset(add_statistic_bin_stmt));
    checkOK(sqlite3_bind_int64(add_statistic_bin_stmt, 1, statisticId));
    checkOK(sqlite3_bind_double(add_statistic_bin_stmt, 2, basePoint));
    checkOK(sqlite3_bind_int64(add_statistic_bin_stmt, 3, cellValue));
    checkDone(sqlite3_step(add_statistic_bin_stmt));
    checkOK(sqlite3_clear_bindings(add_statistic_bin_stmt));
}

const char *SqliteOutputScalarManager::getFileName() const
{
    return fname.c_str();
}

void SqliteOutputScalarManager::commitAndBeginNew()
{
    checkOK(sqlite3_exec(db, "COMMIT TRANSACTION;", nullptr, nullptr, nullptr));
    checkOK(sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION;", nullptr, nullptr, nullptr));
}

void SqliteOutputScalarManager::flush()
{
    if (db)
        commitAndBeginNew();
}

}  // namespace envir
}  // namespace omnetpp

