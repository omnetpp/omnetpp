//==========================================================================
//  SQLITEVECTORFILEWRITER.CC - part of
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
#include "commonutil.h"
#include "sqlitevectorfilewriter.h"
#include "sqliteresultfileschema.h"

namespace omnetpp {
namespace common {

/*
 * Performance notes:
 *  - file size: without index, about the same as with the traditional text-based format
 *  - index adds about 30-70% to the file size
 *  - raw recording performance: about half of text based recorder
 *  - with adding the index up front, total time is worse than with adding index after
 */

SqliteVectorFileWriter::~SqliteVectorFileWriter()
{
    cleanup(); // not close() because it throws; also, close() must have been called already if there was no error
}

inline void SqliteVectorFileWriter::checkOK(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_OK)
        error(sqlite3_errmsg(db));
}

inline void SqliteVectorFileWriter::checkDone(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_DONE)
        error(sqlite3_errmsg(db));
}

void SqliteVectorFileWriter::error(const char *errmsg)
{
    std::string msg = errmsg ? errmsg : "unknown error";
    if (db == nullptr) msg = "Database not open (db=nullptr), or " + msg; // sqlite's own error message is usually 'out of memory' (?)
    std::string fname = this->fname; // cleanup may clear it
    cleanup();
    throw opp_runtime_error("SQLite error '%s' on file '%s'", msg.c_str(), fname.c_str());
}

void SqliteVectorFileWriter::open(const char *filename)
{
    fname = filename;
    checkOK(sqlite3_open(filename, &db));

    checkOK(sqlite3_busy_timeout(db, 10000));    // max time [ms] for waiting to unlock database

    checkOK(sqlite3_exec(db, SQL_CREATE_TABLES, nullptr, 0, nullptr));
    prepareStatements();
    //NOTE: this line is only present in the scalar writer:
    //checkOK(sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION;", nullptr, 0, nullptr));
}

void SqliteVectorFileWriter::close()
{
    if (db) {
        finalizeStatement(stmt);
        finalizeStatement(add_vector_stmt);
        finalizeStatement(add_vector_attr_stmt);
        finalizeStatement(add_vector_data_stmt);
        finalizeStatement(update_vector_stmt);

        executeSql("PRAGMA journal_mode = DELETE;");
        checkOK(sqlite3_close(db));

        clearVectors();

        db = nullptr;
        runId = -1;
        fname = "";
    }
}

void SqliteVectorFileWriter::cleanup()  // MUST NOT THROW
{
    if (db) {
        finalizeStatement(stmt);
        finalizeStatement(add_vector_stmt);
        finalizeStatement(add_vector_attr_stmt);
        finalizeStatement(add_vector_data_stmt);
        finalizeStatement(update_vector_stmt);

        // note: no checkOK() because it would throw
        sqlite3_exec(db, "COMMIT TRANSACTION;", nullptr, nullptr, nullptr);
        sqlite3_exec(db, "PRAGMA journal_mode = DELETE;", nullptr, nullptr, nullptr);
        sqlite3_close(db);

        clearVectors();

        db = nullptr;
        runId = -1;
        fname = "";
    }
}

void SqliteVectorFileWriter::createVectorIndex()
{
    executeSql("CREATE INDEX IF NOT EXISTS vectorData_idx ON vectorData (vectorId);");
}

void SqliteVectorFileWriter::executeSql(const char *sql)
{
    checkOK(sqlite3_exec(db, sql, nullptr, nullptr, nullptr));
}

void SqliteVectorFileWriter::prepareStatement(sqlite3_stmt *&stmt, const char *sql)
{
    checkOK(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr));
}

void SqliteVectorFileWriter::finalizeStatement(sqlite3_stmt *&stmt)
{
    sqlite3_finalize(stmt); // note: no checkOK() needed, see sqlite3_finalize() docu
    stmt = nullptr; // prevent use-after-finalize
}

void SqliteVectorFileWriter::prepareStatements()
{
    prepareStatement(add_vector_stmt, "INSERT INTO vector (runId, moduleName, vectorName) VALUES (?, ?, ?);");
    prepareStatement(add_vector_attr_stmt, "INSERT INTO vectorAttr (vectorId, attrName, attrValue) VALUES (?, ?, ?);");
    prepareStatement(add_vector_data_stmt, "INSERT INTO vectorData (vectorId, eventNumber, simtimeRaw, value) VALUES (?, ?, ?, ?);");
}

void SqliteVectorFileWriter::beginRecordingForRun(const std::string& runName, int simtimeScaleExp, const StringMap& attributes, const StringMap& itervars, const OrderedKeyValueList& configEntries)
{
    Assert(vectors.size() == 0);
    bufferedSamples = 0;

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

void SqliteVectorFileWriter::finalizeVector(VectorData *vp)
{
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
    checkOK(sqlite3_bind_int64(update_vector_stmt, 3, vp->startTime));
    checkOK(sqlite3_bind_int64(update_vector_stmt, 4, vp->endTime));
    checkOK(sqlite3_bind_int64(update_vector_stmt, 5, vp->statistics.getCount()));
    checkOK(sqlite3_bind_double(update_vector_stmt, 6, vp->statistics.getMin()));
    checkOK(sqlite3_bind_double(update_vector_stmt, 7, vp->statistics.getMax()));
    checkOK(sqlite3_bind_double(update_vector_stmt, 8, vp->statistics.getSum()));
    checkOK(sqlite3_bind_double(update_vector_stmt, 9, vp->statistics.getSumSqr()));
    checkOK(sqlite3_bind_int64(update_vector_stmt, 10, vp->id));
    checkDone(sqlite3_step(update_vector_stmt));
    checkOK(sqlite3_clear_bindings(update_vector_stmt));
    executeSql("COMMIT TRANSACTION;");
}

void SqliteVectorFileWriter::endRecordingForRun()
{
    Assert(db != nullptr);

    try {
        for (VectorData *vp : vectors)
            finalizeVector(vp); //TODO currently these all go in separate transactions
        clearVectors();
        runId = -1;
    }
    catch (std::exception&) {
        clearVectors();
        runId = -1;
        throw;
    }
}

void SqliteVectorFileWriter::clearVectors()
{
    for (VectorData *vp : vectors)
        delete vp;
    vectors.clear();
    bufferedSamples = 0;
}

void *SqliteVectorFileWriter::registerVector(const std::string& componentFullPath, const std::string& name, const StringMap& attributes, size_t bufferSize)
{
    Assert(db != nullptr);

    VectorData *vp = new VectorData();
    vp->bufferedSamplesLimit = bufferSize / sizeof(Sample);
    if (vp->bufferedSamplesLimit > 0)
        vp->buffer.reserve(vp->bufferedSamplesLimit);
    vectors.push_back(vp);

    checkOK(sqlite3_reset(add_vector_stmt));
    checkOK(sqlite3_bind_int64(add_vector_stmt, 1, runId));
    checkOK(sqlite3_bind_text(add_vector_stmt, 2, componentFullPath.c_str(), componentFullPath.size(), SQLITE_STATIC));
    checkOK(sqlite3_bind_text(add_vector_stmt, 3, name.c_str(), name.size(), SQLITE_STATIC));
    checkDone(sqlite3_step(add_vector_stmt));
    checkOK(sqlite3_clear_bindings(add_vector_stmt));
    vp->id = sqlite3_last_insert_rowid(db);

    for (auto pair : attributes) {
        checkOK(sqlite3_reset(add_vector_attr_stmt));
        checkOK(sqlite3_bind_int64(add_vector_attr_stmt, 1, vp->id));
        checkOK(sqlite3_bind_text(add_vector_attr_stmt, 2, pair.first.c_str(), pair.first.size(), SQLITE_STATIC));
        checkOK(sqlite3_bind_text(add_vector_attr_stmt, 3, pair.second.c_str(), pair.second.size(), SQLITE_STATIC));
        checkDone(sqlite3_step(add_vector_attr_stmt));
        checkOK(sqlite3_clear_bindings(add_vector_attr_stmt));
    }

    return vp;
}

void SqliteVectorFileWriter::deregisterVector(void *vectorhandle)
{
    Assert(db != nullptr && vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    Vectors::iterator newEnd = std::remove(vectors.begin(), vectors.end(), vp);
    vectors.erase(newEnd, vectors.end());
    finalizeVector(vp);
    delete vp;
}

void SqliteVectorFileWriter::recordInVector(void *vectorhandle, eventnumber_t eventNumber, rawsimtime_t t, double value)
{
    Assert(db != nullptr && vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;

    // store value
    vp->buffer.push_back(Sample(t, eventNumber, value));
    bufferedSamples++;

    // write out block if necessary
    if (vp->bufferedSamplesLimit > 0 && (int)vp->buffer.size() >= vp->bufferedSamplesLimit)
        writeOneBlock(vp);
    else if (bufferedSamplesLimit > 0 && bufferedSamples >= bufferedSamplesLimit)
        writeRecords();

    // update vector statistics
    if (vp->statistics.getCount() == 0) {
        vp->startEventNum = eventNumber;
        vp->startTime = t;
    }
    vp->endEventNum = eventNumber;
    vp->endTime = t;

    vp->statistics.collect(value);
}

void SqliteVectorFileWriter::writeRecords()
{
    executeSql("BEGIN IMMEDIATE TRANSACTION;");
    for (auto vp : vectors)
        if (!vp->buffer.empty())
            writeBlock(vp);
    executeSql("COMMIT TRANSACTION;");
}

void SqliteVectorFileWriter::writeOneBlock(VectorData *vp)
{
    executeSql("BEGIN IMMEDIATE TRANSACTION;");
    writeBlock(vp);
    executeSql("COMMIT TRANSACTION;");
}

void SqliteVectorFileWriter::writeBlock(VectorData *vp)
{
    Assert(vp != nullptr);
    Assert(!vp->buffer.empty());

    Assert(db != nullptr);

    for (const Sample& sample : vp->buffer) {
        checkOK(sqlite3_reset(add_vector_data_stmt));
        checkOK(sqlite3_bind_int64(add_vector_data_stmt, 1, vp->id));
        checkOK(sqlite3_bind_int64(add_vector_data_stmt, 2, sample.eventNumber));
        checkOK(sqlite3_bind_int64(add_vector_data_stmt, 3, sample.simtime));
        checkOK(sqlite3_bind_double(add_vector_data_stmt, 4, sample.value));
        checkDone(sqlite3_step(add_vector_data_stmt));
    }
    bufferedSamples -= vp->buffer.size();
    vp->buffer.clear();
}

void SqliteVectorFileWriter::flush()
{
    if (db)
        writeRecords();
}


}  // namespace common
}  // namespace omnetpp

