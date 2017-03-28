//==========================================================================
//  SQLITESCALARFILEWRITER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Zoltan Bojthe, Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cstring"
#include "commonutil.h"
#include "sqlitescalarfilewriter.h"
#include "sqliteresultfileschema.h"

namespace omnetpp {
namespace common {

SqliteScalarFileWriter::SqliteScalarFileWriter()
{
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

SqliteScalarFileWriter::~SqliteScalarFileWriter()
{
    cleanup(); // not close() because it throws; also, close() must have been called already if there was no error
}

inline void SqliteScalarFileWriter::checkOK(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_OK)
        error(sqlite3_errmsg(db));
}

inline void SqliteScalarFileWriter::checkDone(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_DONE)
        error(sqlite3_errmsg(db));
}

void SqliteScalarFileWriter::error(const char *errmsg)
{
    std::string msg = errmsg ? errmsg : "unknown error";
    if (db == nullptr) msg += " (db == nullptr)"; // sqlite's own error message is usually 'out of memory' (?)
    std::string fname = this->fname; // cleanup may clear it
    cleanup();
    throw opp_runtime_error("SQLite error '%s' on file '%s'", msg.c_str(), fname.c_str());
}

void SqliteScalarFileWriter::open(const char *filename)
{
    fname = filename;
    checkOK(sqlite3_open(filename, &db));

    checkOK(sqlite3_busy_timeout(db, 10000));    // max time [ms] for waiting to unlock database

    checkOK(sqlite3_exec(db, SQL_CREATE_TABLES, nullptr, 0, nullptr));
    prepareStatements();
    checkOK(sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION;", nullptr, 0, nullptr));

    insertCount = 0;
}

void SqliteScalarFileWriter::close()
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

        db = nullptr;
        runId = -1;
        fname = "";
    }
}

void SqliteScalarFileWriter::cleanup()  // MUST NOT THROW
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

        db = nullptr;
        runId = -1;
        fname = "";
    }
}

void SqliteScalarFileWriter::prepareStatement(sqlite3_stmt *&stmt, const char *sql)
{
    checkOK(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr));
}

void SqliteScalarFileWriter::finalizeStatement(sqlite3_stmt *&stmt)
{
    sqlite3_finalize(stmt); // note: no checkOK() needed, see sqlite3_finalize() docu
    stmt = nullptr; // prevent use-after-finalize
}

void SqliteScalarFileWriter::prepareStatements()
{
    prepareStatement(add_scalar_stmt, "INSERT INTO scalar (runId, moduleName, scalarName, scalarValue) VALUES (?, ?, ?, ?);");
    prepareStatement(add_scalar_attr_stmt, "INSERT INTO scalarattr (scalarId, attrName, attrValue) VALUES (?, ?, ?);");
    prepareStatement(add_statistic_stmt,
            "INSERT INTO statistic (runId, moduleName, statName, "
            "statCount, "
            "statMean, statStddev, statSum, statSqrsum, statMin, statMax, "
            "statWeights, statWeightedSum, statSqrSumWeights, statWeightedSqrSum"
            ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
    prepareStatement(add_statistic_attr_stmt, "INSERT INTO statisticattr (statId, attrName, attrValue) VALUES (?, ?, ?);");
    prepareStatement(add_statistic_bin_stmt, "INSERT INTO histbin (statId, baseValue, cellValue) VALUES (?, ?, ?);");
}

void SqliteScalarFileWriter::beginRecordingForRun(const std::string& runName, int simtimeScaleExp, const StringMap& attributes, const StringMap& moduleParams)
{
    // save run
    prepareStatement(stmt, "INSERT INTO run (runname, simTimeExp) VALUES (?, ?);");
    checkOK(sqlite3_bind_text(stmt, 1, runName.c_str(), runName.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_int(stmt, 2, simtimeScaleExp));
    checkDone(sqlite3_step(stmt));
    checkOK(sqlite3_clear_bindings(stmt));
    runId = sqlite3_last_insert_rowid(db);
    finalizeStatement(stmt);

    // save run attributes
    prepareStatement(stmt, "INSERT INTO runattr (runid, attrname, attrvalue) VALUES (?, ?, ?);");
    for (auto it = attributes.begin(); it != attributes.end(); ++it) {
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
    for (auto it = moduleParams.begin(); it != moduleParams.end(); ++it) {
        checkOK(sqlite3_reset(stmt));
        checkOK(sqlite3_bind_int64(stmt, 1, runId));
        checkOK(sqlite3_bind_text(stmt, 2, it->first.c_str(), it->first.size(), SQLITE_STATIC));
        checkOK(sqlite3_bind_text(stmt, 3, it->second.c_str(), it->second.size(), SQLITE_STATIC));
        checkDone(sqlite3_step(stmt));
        checkOK(sqlite3_clear_bindings(stmt));
    }
    finalizeStatement(stmt);
}

void SqliteScalarFileWriter::endRecordingForRun()
{
    runId = -1;
}

sqlite_int64 SqliteScalarFileWriter::writeScalar(const std::string& componentFullPath, const std::string& name, double value)
{
    checkOK(sqlite3_reset(add_scalar_stmt));
    checkOK(sqlite3_bind_int64(add_scalar_stmt, 1, runId));
    checkOK(sqlite3_bind_text(add_scalar_stmt, 2, componentFullPath.c_str(), componentFullPath.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_scalar_stmt, 3, name.c_str(), name.length(), SQLITE_STATIC));
    checkOK(sqlite3_bind_double(add_scalar_stmt, 4, value));
    checkDone(sqlite3_step(add_scalar_stmt));
    checkOK(sqlite3_clear_bindings(add_scalar_stmt));
    sqlite3_int64 scalarId = sqlite3_last_insert_rowid(db);
    return scalarId;
}

void SqliteScalarFileWriter::writeScalarAttr(sqlite_int64 scalarId, const char *name, size_t nameLength, const char *value, size_t valueLength)
{
    checkOK(sqlite3_reset(add_scalar_attr_stmt));
    checkOK(sqlite3_bind_int64(add_scalar_attr_stmt, 1, scalarId));
    checkOK(sqlite3_bind_text(add_scalar_attr_stmt, 2, name, nameLength, SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_scalar_attr_stmt, 3, value, valueLength, SQLITE_STATIC));
    checkDone(sqlite3_step(add_scalar_attr_stmt));
    checkOK(sqlite3_clear_bindings(add_scalar_attr_stmt));
}

void SqliteScalarFileWriter::recordScalar(const std::string& componentFullPath, const std::string& name, double value, const StringMap& attributes)
{
    Assert(runId != -1); // ensure run data has been written out
    sqlite3_int64 scalarId = writeScalar(componentFullPath, name, value);
    for (auto it = attributes.begin(); it != attributes.end(); ++it)
        writeScalarAttr(scalarId, it->first.c_str(), it->first.size(), it->second.c_str(), it->second.size());

    // commit every once in a while
    if (++insertCount >= commitFreq) {
        insertCount = 0;
        commitAndBeginNew();
    }
}

sqlite_int64 SqliteScalarFileWriter::writeStatistic(const std::string& componentFullPath, const std::string& name, const Statistics& statistic)
{
    checkOK(sqlite3_reset(add_statistic_stmt));
    checkOK(sqlite3_bind_int64(add_statistic_stmt, 1, runId));
    checkOK(sqlite3_bind_text(add_statistic_stmt, 2, componentFullPath.c_str(), componentFullPath.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_statistic_stmt, 3, name.c_str(), name.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_int64(add_statistic_stmt, 4, statistic.getCount()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 5, statistic.getMean()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 6, statistic.getStddev()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 7, statistic.getSum()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 8, statistic.getSumSqr()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 9, statistic.getMin()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 10, statistic.getMax()));
//TODO
//    if (statistic->isWeighted()) {
//        checkOK(sqlite3_bind_double(add_statistic_stmt, 11, statistic->getWeights()));
//        checkOK(sqlite3_bind_double(add_statistic_stmt, 12, statistic->getWeightedSum()));
//        checkOK(sqlite3_bind_double(add_statistic_stmt, 13, statistic->getSqrSumWeights()));
//        checkOK(sqlite3_bind_double(add_statistic_stmt, 14, statistic->getWeightedSqrSum()));
//    }
    checkDone(sqlite3_step(add_statistic_stmt));
    sqlite3_int64 statisticId = sqlite3_last_insert_rowid(db);
    checkOK(sqlite3_clear_bindings(add_statistic_stmt));
    return statisticId;
}

void SqliteScalarFileWriter::recordStatistic(const std::string& componentFullPath, const std::string& name, const Statistics& statistic, const StringMap& attributes)
{
    Assert(runId != -1); // ensure run data has been written out
    sqlite3_int64 statisticId = writeStatistic(componentFullPath, name, statistic);
    for (auto it = attributes.begin(); it != attributes.end(); ++it)
        writeStatisticAttr(statisticId, it->first.c_str(), it->second.c_str());
}

void SqliteScalarFileWriter::recordHistogram(const std::string& componentFullPath, const std::string& name, const Statistics& statistic, const Histogram& bins, const StringMap& attributes)
{
    Assert(runId != -1); // ensure run data has been written out
    sqlite3_int64 statisticId = writeStatistic(componentFullPath, name, statistic);
    for (auto it = attributes.begin(); it != attributes.end(); ++it)
        writeStatisticAttr(statisticId, it->first.c_str(), it->second.c_str());
    for (auto bin : bins.getBins())
        writeStatisticBin(statisticId, bin.lowerBound, bin.count);
}

void SqliteScalarFileWriter::writeStatisticAttr(sqlite_int64 statisticId, const char *name, const char *value)
{
    checkOK(sqlite3_reset(add_statistic_attr_stmt));
    checkOK(sqlite3_bind_int64(add_statistic_attr_stmt, 1, statisticId));
    checkOK(sqlite3_bind_text(add_statistic_attr_stmt, 2, name, strlen(name), SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_statistic_attr_stmt, 3, value, strlen(value), SQLITE_STATIC));
    checkDone(sqlite3_step(add_statistic_attr_stmt));
    checkOK(sqlite3_clear_bindings(add_statistic_attr_stmt));
}

void SqliteScalarFileWriter::writeStatisticBin(sqlite_int64 statisticId, double basePoint, unsigned long cellValue)
{
    checkOK(sqlite3_reset(add_statistic_bin_stmt));
    checkOK(sqlite3_bind_int64(add_statistic_bin_stmt, 1, statisticId));
    checkOK(sqlite3_bind_double(add_statistic_bin_stmt, 2, basePoint));
    checkOK(sqlite3_bind_int64(add_statistic_bin_stmt, 3, cellValue));
    checkDone(sqlite3_step(add_statistic_bin_stmt));
    checkOK(sqlite3_clear_bindings(add_statistic_bin_stmt));
}

void SqliteScalarFileWriter::commitAndBeginNew()
{
    checkOK(sqlite3_exec(db, "COMMIT TRANSACTION;", nullptr, nullptr, nullptr));
    checkOK(sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION;", nullptr, nullptr, nullptr));
}

void SqliteScalarFileWriter::flush()
{
    if (db)
        commitAndBeginNew();
}

}  // namespace common
}  // namespace omnetpp

