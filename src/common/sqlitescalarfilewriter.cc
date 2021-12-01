//==========================================================================
//  SQLITESCALARFILEWRITER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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
    if (db == nullptr) msg = "Database not open (db=nullptr), or " + msg; // sqlite's own error message is usually 'out of memory' (?)
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
        finalizeStatement(add_parameter_stmt);
        finalizeStatement(add_parameter_attr_stmt);

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
        finalizeStatement(add_parameter_stmt);
        finalizeStatement(add_parameter_attr_stmt);

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
    prepareStatement(add_scalar_attr_stmt, "INSERT INTO scalarAttr (scalarId, attrName, attrValue) VALUES (?, ?, ?);");
    prepareStatement(add_statistic_stmt,
            "INSERT INTO statistic (runId, moduleName, statName, isHistogram, isWeighted, "
            "statCount, statMean, statStddev, statMin, statMax, statSum, statSqrsum, "
            "statWeights, statWeightedSum, statSqrSumWeights, statWeightedSqrSum"
            ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
    prepareStatement(add_statistic_attr_stmt, "INSERT INTO statisticAttr (statId, attrName, attrValue) VALUES (?, ?, ?);");
    prepareStatement(add_statistic_bin_stmt, "INSERT INTO histogramBin (statId, lowerEdge, binValue) VALUES (?, ?, ?);");
    prepareStatement(add_parameter_stmt, "INSERT INTO parameter (runId, moduleName, paramName, paramValue) VALUES (?, ?, ?, ?);");
    prepareStatement(add_parameter_attr_stmt, "INSERT INTO paramAttr (paramId, attrName, attrValue) VALUES (?, ?, ?);");
}

void SqliteScalarFileWriter::beginRecordingForRun(const std::string& runName, int simtimeScaleExp, const StringMap& attributes, const StringMap& itervars, const OrderedKeyValueList& configEntries)
{
    // save run
    prepareStatement(stmt, "INSERT INTO run (runName, simTimeExp) VALUES (?, ?);");
    checkOK(sqlite3_bind_text(stmt, 1, runName.c_str(), runName.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_int(stmt, 2, simtimeScaleExp));
    checkDone(sqlite3_step(stmt));
    checkOK(sqlite3_clear_bindings(stmt));
    runId = sqlite3_last_insert_rowid(db);
    finalizeStatement(stmt);

    // save run attributes
    prepareStatement(stmt, "INSERT INTO runAttr (runId, attrName, attrValue) VALUES (?, ?, ?);");
    for (auto& p : attributes) {
        checkOK(sqlite3_reset(stmt));
        checkOK(sqlite3_bind_int64(stmt, 1, runId));
        checkOK(sqlite3_bind_text(stmt, 2, p.first.c_str(), p.first.size(), SQLITE_STATIC));
        checkOK(sqlite3_bind_text(stmt, 3, p.second.c_str(), p.second.size(), SQLITE_STATIC));
        checkDone(sqlite3_step(stmt));
        checkOK(sqlite3_clear_bindings(stmt));
    }
    finalizeStatement(stmt);

    // save itervars
    prepareStatement(stmt, "INSERT INTO runItervar (runId, itervarName, itervarValue) VALUES (?, ?, ?);");
    for (auto& p : itervars) {
        checkOK(sqlite3_reset(stmt));
        checkOK(sqlite3_bind_int64(stmt, 1, runId));
        checkOK(sqlite3_bind_text(stmt, 2, p.first.c_str(), p.first.size(), SQLITE_STATIC));
        checkOK(sqlite3_bind_text(stmt, 3, p.second.c_str(), p.second.size(), SQLITE_STATIC));
        checkDone(sqlite3_step(stmt));
        checkOK(sqlite3_clear_bindings(stmt));
    }
    finalizeStatement(stmt);

    // save config entries
    prepareStatement(stmt, "INSERT INTO runConfig (runId, configKey, configValue, configOrder) VALUES (?, ?, ?, ?);");
    int i = 0;
    for (auto& p : configEntries) {
        checkOK(sqlite3_reset(stmt));
        checkOK(sqlite3_bind_int64(stmt, 1, runId));
        checkOK(sqlite3_bind_text(stmt, 2, p.first.c_str(), p.first.size(), SQLITE_STATIC));
        checkOK(sqlite3_bind_text(stmt, 3, p.second.c_str(), p.second.size(), SQLITE_STATIC));
        checkOK(sqlite3_bind_int(stmt, 4, i++));
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
    for (const auto & attribute : attributes)
        writeScalarAttr(scalarId, attribute.first.c_str(), attribute.first.size(), attribute.second.c_str(), attribute.second.size());

    // commit every once in a while
    if (++insertCount >= commitFreq) {
        insertCount = 0;
        commitAndBeginNew();
    }
}

sqlite_int64 SqliteScalarFileWriter::writeStatistic(const std::string& componentFullPath, const std::string& name, const Statistics& statistic, bool isHistogram)
{
    checkOK(sqlite3_reset(add_statistic_stmt));
    checkOK(sqlite3_bind_int64(add_statistic_stmt, 1, runId));
    checkOK(sqlite3_bind_text(add_statistic_stmt, 2, componentFullPath.c_str(), componentFullPath.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_statistic_stmt, 3, name.c_str(), name.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_int(add_statistic_stmt, 4, (int)isHistogram));
    checkOK(sqlite3_bind_int(add_statistic_stmt, 5, (int)statistic.isWeighted()));
    checkOK(sqlite3_bind_int64(add_statistic_stmt, 6, statistic.getCount()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 7, statistic.getMean()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 8, statistic.getStddev()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 9, statistic.getMin()));
    checkOK(sqlite3_bind_double(add_statistic_stmt, 10, statistic.getMax()));

    if (!statistic.isWeighted()) {
        checkOK(sqlite3_bind_double(add_statistic_stmt, 11, statistic.getSum()));
        checkOK(sqlite3_bind_double(add_statistic_stmt, 12, statistic.getSumSqr()));
    } else {
        checkOK(sqlite3_bind_double(add_statistic_stmt, 13, statistic.getSumWeights()));
        checkOK(sqlite3_bind_double(add_statistic_stmt, 14, statistic.getWeightedSum()));
        checkOK(sqlite3_bind_double(add_statistic_stmt, 15, statistic.getSumSquaredWeights()));
        checkOK(sqlite3_bind_double(add_statistic_stmt, 16, statistic.getSumWeightedSquaredValues()));
    }
    checkDone(sqlite3_step(add_statistic_stmt));
    sqlite3_int64 statisticId = sqlite3_last_insert_rowid(db);
    checkOK(sqlite3_clear_bindings(add_statistic_stmt));
    return statisticId;
}

void SqliteScalarFileWriter::recordStatistic(const std::string& componentFullPath, const std::string& name, const Statistics& statistic, const StringMap& attributes)
{
    Assert(runId != -1); // ensure run data has been written out
    sqlite3_int64 statisticId = writeStatistic(componentFullPath, name, statistic, false);
    for (const auto & attribute : attributes)
        writeStatisticAttr(statisticId, attribute.first.c_str(), attribute.second.c_str());
}

void SqliteScalarFileWriter::recordHistogram(const std::string& componentFullPath, const std::string& name, const Statistics& statistic, const Histogram& bins, const StringMap& attributes)
{
    Assert(runId != -1); // ensure run data has been written out
    sqlite3_int64 statisticId = writeStatistic(componentFullPath, name, statistic, true);
    for (const auto & attribute : attributes)
        writeStatisticAttr(statisticId, attribute.first.c_str(), attribute.second.c_str());

    int n = bins.getNumBins();
    writeBin(statisticId, -INFINITY, bins.getUnderflows());
    for (int i = 0; i < n; i++)
        writeBin(statisticId, bins.getBinEdge(i), bins.getBinValue(i));
    writeBin(statisticId, bins.getBinEdge(n), bins.getOverflows());
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

void SqliteScalarFileWriter::writeBin(sqlite_int64 statisticId, double lowerEdge, double binValue)
{
    checkOK(sqlite3_reset(add_statistic_bin_stmt));
    checkOK(sqlite3_bind_int64(add_statistic_bin_stmt, 1, statisticId));
    checkOK(sqlite3_bind_double(add_statistic_bin_stmt, 2, lowerEdge));
    checkOK(sqlite3_bind_double(add_statistic_bin_stmt, 3, binValue));
    checkDone(sqlite3_step(add_statistic_bin_stmt));
    checkOK(sqlite3_clear_bindings(add_statistic_bin_stmt));
}

void SqliteScalarFileWriter::recordParameter(const std::string& componentFullPath, const std::string& name, const std::string& value, const StringMap& attributes)
{
    Assert(runId != -1); // ensure run data has been written out
    sqlite3_int64 parameterId = writeParameter(componentFullPath, name, value);
    for (const auto & attribute : attributes)
        writeParameterAttr(parameterId, attribute.first.c_str(), attribute.first.size(), attribute.second.c_str(), attribute.second.size());

    // commit every once in a while
    if (++insertCount >= commitFreq) {
        insertCount = 0;
        commitAndBeginNew();
    }
}

sqlite_int64 SqliteScalarFileWriter::writeParameter(const std::string& componentFullPath, const std::string& name, const std::string& value)
{
    checkOK(sqlite3_reset(add_parameter_stmt));
    checkOK(sqlite3_bind_int64(add_parameter_stmt, 1, runId));
    checkOK(sqlite3_bind_text(add_parameter_stmt, 2, componentFullPath.c_str(), componentFullPath.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_parameter_stmt, 3, name.c_str(), name.length(), SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_parameter_stmt, 4, value.c_str(), value.length(), SQLITE_STATIC));
    checkDone(sqlite3_step(add_parameter_stmt));
    checkOK(sqlite3_clear_bindings(add_parameter_stmt));
    sqlite3_int64 parameterId = sqlite3_last_insert_rowid(db);
    return parameterId;
}

void SqliteScalarFileWriter::writeParameterAttr(sqlite_int64 parameterId, const char *name, size_t nameLength, const char *value, size_t valueLength)
{
    checkOK(sqlite3_reset(add_parameter_attr_stmt));
    checkOK(sqlite3_bind_int64(add_parameter_attr_stmt, 1, parameterId));
    checkOK(sqlite3_bind_text(add_parameter_attr_stmt, 2, name, nameLength, SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_parameter_attr_stmt, 3, value, valueLength, SQLITE_STATIC));
    checkDone(sqlite3_step(add_parameter_attr_stmt));
    checkOK(sqlite3_clear_bindings(add_parameter_attr_stmt));
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

