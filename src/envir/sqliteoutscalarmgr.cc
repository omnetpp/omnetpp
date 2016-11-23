//==========================================================================
//  SQLITEOUTPUTSCALARMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Zoltan Bojthe
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//TODO redesign for buffered inserts

#include "omnetpp/simkerneldefs.h"

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

Register_PerRunConfigOption(CFGID_OUTPUT_SCALAR_DB, "output-scalar-db", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.sqlite.sca", "Name for the output scalar file.");
Register_PerRunConfigOption(CFGID_OUTPUT_SCALAR_DB_APPEND, "output-scalar-db-append", CFG_BOOL, "false", "What to do when the output scalar file already exists: append to it, or delete it and begin a new file (default).");
Register_GlobalConfigOption(CFGID_OUTPUT_SCALAR_DB_COMMIT_FREQ, "scalar-db-commit-freq", CFG_INT, DEFAULT_COMMIT_FREQ, "COMMIT every n INSERTs, default=" DEFAULT_COMMIT_FREQ);

//TODO registered in fileoutscalarmgr:
extern omnetpp::cConfigOption *CFGID_SCALAR_RECORDING;

Register_Class(cSqliteOutputScalarManager);

#define opp_runtime_error cRuntimeError

static const char SQL_CREATE_TABLES[] =
        "PRAGMA foreign_keys = ON; "
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
        ""
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

cSqliteOutputScalarManager::cSqliteOutputScalarManager()
{
    runId = -1;
    db = nullptr;
    add_scalar_stmt = nullptr;
    add_scalar_attr_stmt = nullptr;
    add_statistic_stmt = nullptr;
    add_statistic_attr_stmt = nullptr;
    add_statistic_bin_stmt = nullptr;

    commitFreq = 0;
    insertCount = 0;
}

cSqliteOutputScalarManager::~cSqliteOutputScalarManager()
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

void cSqliteOutputScalarManager::openDb()
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
    commitFreq = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_SCALAR_DB_COMMIT_FREQ);
    insertCount = 0;
}

void cSqliteOutputScalarManager::closeDb()
{
    if (db) {
        sqlite3_finalize(add_scalar_stmt);
        sqlite3_finalize(add_scalar_attr_stmt);
        char *zErrMsg = nullptr;
        if (sqlite3_exec(db, "COMMIT TRANSACTION;", callback, nullptr, &zErrMsg) != SQLITE_OK)
            throw opp_runtime_error("SQL error at COMMIT TRANSACTION: %s\n", zErrMsg);
        if (sqlite3_exec(db, "PRAGMA journal_mode = DELETE; ", callback, nullptr, &zErrMsg) != SQLITE_OK) {
            throw opp_runtime_error("SQL error at PRAGMA journal_mode = DELETE: %s\n", zErrMsg);
        }
        sqlite3_close(db);
        runId = -1;
        db = nullptr;
        add_scalar_stmt = nullptr;
        add_scalar_attr_stmt = nullptr;
    }
}

void cSqliteOutputScalarManager::startRun()
{
    // clean up file from previous runs
    closeDb();
    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_SCALAR_DB);
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    if (getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_SCALAR_DB_APPEND) == false) {
        removeFile(fname.c_str(), "old sqlite output scalar file");
    }
    run.reset();
}

void cSqliteOutputScalarManager::endRun()
{
    closeDb();
}

void cSqliteOutputScalarManager::init()
{
    if (!db) {
        openDb();
        if (!db)
            return;
    }

    if (!run.initialized) {
        // this is the first scalar written in this run

        run.initRun();

        sqlite3_stmt *stmt = nullptr;
        //TODO check and use existing run based on runName
        // save run
        prepareStmt(&stmt, "insert into run (runname, simTimeExp) values (?, ?);");
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
        prepareStmt(&stmt, "insert into runattr (runid, attrname, attrvalue) values (?, ?, ?);");
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
        prepareStmt(&stmt, "insert into runparam (runid, parname, parvalue) values (?, ?, ?);");
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

        // prepare sql statements
        prepareStmt(&add_scalar_stmt, "insert into scalar (runId, moduleName, scalarName, scalarValue) values (?, ?, ?, ?);");
        prepareStmt(&add_scalar_attr_stmt, "insert into scalarattr (scalarId, attrName, attrValue) values (?, ?, ?);");
        prepareStmt(&add_statistic_stmt,
                "insert into histogram (runId, moduleName, histName, "
                "histCount, "
                "histMean, histStddev, histSum, histSqrsum, histMin, histMax, "
                "histWeights, histWeightedSum, histSqrSumWeights, histWeightedSqrSum"
                ") values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
        prepareStmt(&add_statistic_attr_stmt, "insert into histattr (histId, attrName, attrValue) values (?, ?, ?);");
        prepareStmt(&add_statistic_bin_stmt, "insert into histbin (histId, baseValue, cellValue) values (?, ?, ?);");

        // save iteration variables
        std::vector<const char *> v = getEnvir()->getConfigEx()->getIterationVariableNames();
        std::cout << "Save iteration variables: " << v.size() << std::endl;
        for (int i = 0; i < (int)v.size(); i++) {
            const char *name = v[i];
            const char *value = getEnvir()->getConfigEx()->getVariable(v[i]);
            std::cout << "recordNumericIterationVariable(" << name << ", " << value << ");" << std::endl;
            recordNumericIterationVariable(name, value);
        }
    }
}

void cSqliteOutputScalarManager::prepareStmt(sqlite3_stmt **stmt, const char *sql)
{
    if (sqlite3_prepare_v2(db, sql, -1, stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("database error at prepareStmt(`%s`): %s in statement `%s`\n", sql, sqlite3_errmsg(db));
}

void cSqliteOutputScalarManager::recordNumericIterationVariable(const char *name, const char *valueStr)
{
    static const std::string DOT(".");
    char *e;
    setlocale(LC_NUMERIC, "C");
    double value = strtod(valueStr, &e);
    if (*e == '\0') {
        // plain number - just record as it is
        // XXX write with using an "itervar" keyword not "scalar" (needs to be understood by IDE as well)
        writeScalar(DOT, name, value);
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
            sqlite3_int64 scalarId = writeScalar(DOT, name, value);
            if (!unit.empty()) {
                writeScalarAttr(scalarId, "unit", 4, unit.c_str(), unit.size());
            }
        }
    }
}

sqlite_int64 cSqliteOutputScalarManager::writeScalar(const std::string &componentFullPath, const char *name, double value)
{
    sqlite3_reset(add_scalar_stmt);
    sqlite3_bind_int64(add_scalar_stmt, 1, runId);
    sqlite3_bind_text(add_scalar_stmt, 2, componentFullPath.c_str(), componentFullPath.size(), SQLITE_STATIC);
    sqlite3_bind_text(add_scalar_stmt, 3, name, strlen(name), SQLITE_STATIC);
    sqlite3_bind_double(add_scalar_stmt, 4, value);
    sqlite3_step(add_scalar_stmt);
//    sqlite3_clear_bindings(add_scalar_stmt);
    sqlite3_int64 scalarId = sqlite3_last_insert_rowid(db);
    return scalarId;
}

void cSqliteOutputScalarManager::writeScalarAttr(sqlite_int64 scalarId, const char *name, size_t nameLength, const char *value, size_t valueLength)
{
    sqlite3_reset(add_scalar_attr_stmt);
    sqlite3_bind_int64(add_scalar_attr_stmt, 1, scalarId);
    sqlite3_bind_text(add_scalar_attr_stmt, 2, name, nameLength, SQLITE_STATIC);
    sqlite3_bind_text(add_scalar_attr_stmt, 3, value, valueLength, SQLITE_STATIC);
    sqlite3_step(add_scalar_attr_stmt);
//    sqlite3_clear_bindings(add_scalar_attr_stmt);
}

void cSqliteOutputScalarManager::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    if (!run.initialized)
        init();
    if (!db)
        return;

    if (!name || !name[0])
        name = "(unnamed)";

    std::string componentFullPath = component->getFullPath();
    bool enabled = getEnvir()->getConfig()->getAsBool((componentFullPath+"."+name).c_str(), CFGID_SCALAR_RECORDING);
    if (enabled) {
        sqlite3_int64 scalarId = writeScalar(componentFullPath, name, value);
        if (attributes) {
            for (opp_string_map::iterator it = attributes->begin(); it != attributes->end(); ++it) {
                writeScalarAttr(scalarId, it->first.c_str(), it->first.size(), it->second.c_str(), it->second.size());
            }
        }
    }
    // commit every once in a while
    if (++insertCount >= commitFreq) {
        insertCount = 0;
        commitDb();
    }
}

void cSqliteOutputScalarManager::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    static const double MinusInfinity = -1.0/0.0;

    if (!run.initialized)
        init();
    if (!db)
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

    sqlite3_reset(add_statistic_stmt);
    sqlite3_bind_int64(add_statistic_stmt, 1, runId);
    sqlite3_bind_text(add_statistic_stmt, 2, fullPath.c_str(), fullPath.size(), SQLITE_STATIC);
    sqlite3_bind_text(add_statistic_stmt, 3, name, strlen(name), SQLITE_STATIC);
    sqlite3_bind_int64(add_statistic_stmt, 4, statistic->getCount());
    sqlite3_bind_double(add_statistic_stmt, 5, statistic->getMean());
    sqlite3_bind_double(add_statistic_stmt, 6, statistic->getStddev());
    sqlite3_bind_double(add_statistic_stmt, 7, statistic->getSum());
    sqlite3_bind_double(add_statistic_stmt, 8, statistic->getSqrSum());
    sqlite3_bind_double(add_statistic_stmt, 9, statistic->getMin());
    sqlite3_bind_double(add_statistic_stmt, 10, statistic->getMax());
    if (statistic->isWeighted()) {
        sqlite3_bind_double(add_statistic_stmt, 11, statistic->getWeights());
        sqlite3_bind_double(add_statistic_stmt, 12, statistic->getWeightedSum());
        sqlite3_bind_double(add_statistic_stmt, 13, statistic->getSqrSumWeights());
        sqlite3_bind_double(add_statistic_stmt, 14, statistic->getWeightedSqrSum());
    }
    sqlite3_step(add_statistic_stmt);
    sqlite3_int64 statisticId = sqlite3_last_insert_rowid(db);
    sqlite3_clear_bindings(add_statistic_stmt);

    if (attributes)
        for (opp_string_map::iterator it = attributes->begin(); it != attributes->end(); ++it)
            writeStatisticAttr(statisticId, it->first.c_str(), it->second.c_str());

    if (cDensityEstBase *histogram = dynamic_cast<cDensityEstBase *>(statistic)) {
        // check that recording the histogram is enabled
        bool enabled = getEnvir()->getConfig()->getAsBool((objectFullPath+":histogram").c_str(), CFGID_SCALAR_RECORDING);
        if (enabled) {
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

void cSqliteOutputScalarManager::writeStatisticAttr(sqlite_int64 statisticId, const char *name, const char *value)
{
    sqlite3_reset(add_statistic_attr_stmt);
    sqlite3_bind_int64(add_statistic_attr_stmt, 1, statisticId);
    sqlite3_bind_text(add_statistic_attr_stmt, 2, name, strlen(name), SQLITE_STATIC);
    sqlite3_bind_text(add_statistic_attr_stmt, 3, value, strlen(value), SQLITE_STATIC);
    sqlite3_step(add_statistic_attr_stmt);
//    sqlite3_clear_bindings(add_statistic_attr_stmt);
}

void cSqliteOutputScalarManager::writeStatisticBin(sqlite_int64 statisticId, double basePoint, unsigned long cellValue)
{
    sqlite3_reset(add_statistic_bin_stmt);
    sqlite3_bind_int64(add_statistic_bin_stmt, 1, statisticId);
    sqlite3_bind_double(add_statistic_bin_stmt, 2, basePoint);
    sqlite3_bind_int64(add_statistic_bin_stmt, 3, cellValue);
    sqlite3_step(add_statistic_bin_stmt);
//    sqlite3_clear_bindings(add_statistic_attr_stmt);
}

const char *cSqliteOutputScalarManager::getFileName() const
{
    return fname.c_str();
}

void cSqliteOutputScalarManager::commitDb()
{
    if (db) {
        char *zErrMsg = nullptr;
        if (sqlite3_exec(db, "COMMIT TRANSACTION;", callback, nullptr, &zErrMsg) != SQLITE_OK)
            throw opp_runtime_error("SQL error at COMMIT TRANSACTION: %s\n", zErrMsg);
        if (sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION;", callback, nullptr, &zErrMsg) != SQLITE_OK)
            throw opp_runtime_error("SQL error at BEGIN TRANSACTION: %s\n", zErrMsg);
    }
}

void cSqliteOutputScalarManager::flush()
{
    commitDb();
}

}  // namespace envir
}  // namespace omnetpp

