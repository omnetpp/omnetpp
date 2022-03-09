//=========================================================================
//  SQLITEVECTORDATAREADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/opp_ctype.h"
#include "omnetpp/platdep/platmisc.h"
#include "scaveutils.h"
#include "resultfilemanager.h"
#include "sqlitevectordatareader.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

#define DEBUG(arglist)
//#define DEBUG(arglist)  {printf arglist; fflush(stdout);}


/* // old fragment, kept only for future reference
void SqliteVectorReader::process()
{
    if (!db) {
        checkOK(sqlite3_open_v2(filename.c_str(), &db, SQLITE_OPEN_READWRITE, 0));
        if (allowIndexing) {
            DEBUG(("indexing started\n"));
            checkOK(sqlite3_exec(db, "CREATE INDEX IF NOT EXISTS vectordata_idx ON vectordata (vectorId);", nullptr, nullptr, nullptr));
            DEBUG(("indexing finished\n"));
        }
    }
}
*/

static std::string makePlaceholders(int n)
{
    std::string questionmarks;

    for (int i = 0; i < n; ++i)
        questionmarks += (i == 0) ? "?" : ", ?";

    return questionmarks;
}

SqliteVectorDataReader::SqliteVectorDataReader(const char *filename, bool includeEventNumbers, AdapterLambdaType adapterLambda, const FileFingerprint& fingerprint) :
    db(nullptr),
    stmt(nullptr),
    filename(filename),
    includeEventNumbers(includeEventNumbers),
    adapterLambda(adapterLambda),
    expectedFingerprint(fingerprint)
{
    // empty
}

SqliteVectorDataReader::~SqliteVectorDataReader()
{
    if (db) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
}

double SqliteVectorDataReader::sqlite3ColumnDouble(sqlite3_stmt *stmt, int fieldIdx)
{
    return (sqlite3_column_type(stmt, fieldIdx) != SQLITE_NULL) ? sqlite3_column_double(stmt, fieldIdx) : NAN;
}

inline void SqliteVectorDataReader::checkOK(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_OK)
        error(sqlite3_errmsg(db));
}

inline void SqliteVectorDataReader::checkRow(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_ROW)
        error(sqlite3_errmsg(db));
}

void SqliteVectorDataReader::error(const char *errmsg)
{
    throw opp_runtime_error("Cannot read SQLite result file '%s': %s", filename.c_str(), errmsg);
}

void SqliteVectorDataReader::prepareStatement(const char *sql)
{
    assert(stmt == nullptr);
    checkOK(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr));
}

void SqliteVectorDataReader::finalizeStatement()
{
    assert(stmt != nullptr);
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    stmt = nullptr;
}

VectorDatum *SqliteVectorDataReader::getSingleEntry(int simtimeExp)
{
    assert(stmt != nullptr);
    int resultCode = sqlite3_step(stmt);

    if (resultCode != SQLITE_ROW) {
        finalizeStatement();
        return nullptr;
    }

    VectorDatum *datum = new VectorDatum();
    int64_t rowid = sqlite3_column_int64(stmt, 0);
    datum->eventNumber = sqlite3_column_int64(stmt, 1);
    sqlite_int64 simtimeRaw = sqlite3_column_int64(stmt, 2);
    datum->simtime = BigDecimal(simtimeRaw, simtimeExp);
    datum->value = sqlite3_column_double(stmt, 3);

    finalizeStatement();

    datum->serial = getSerialForRowId(rowid);

    return datum;
}

void SqliteVectorDataReader::processStatementRows()
{
    assert(stmt != nullptr);

    int currentVectorId = -1;
    std::vector<VectorDatum> entryBuffer;
    entryBuffer.reserve(batchSize);

    while (true) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);

        sqlite3_int64 vectorId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 eventNumber = sqlite3_column_int64(stmt, 1);
        sqlite3_int64 simtimeRaw = sqlite3_column_int64(stmt, 2);
        double value = sqlite3ColumnDouble(stmt, 3);

        if (vectorId != currentVectorId || entryBuffer.size() >= batchSize) {
            if (!entryBuffer.empty())
                adapterLambda(currentVectorId, entryBuffer);
            currentVectorId = vectorId;
            entryBuffer.clear();
        }

        int simtimeExp = getSimtimeExp(vectorId);

        // TODO serial is missing
        entryBuffer.push_back(VectorDatum(-1, eventNumber, BigDecimal(simtimeRaw, simtimeExp), value));
    }

    if (!entryBuffer.empty())
        adapterLambda(currentVectorId, entryBuffer);

    finalizeStatement();
}

void SqliteVectorDataReader::ensureDbOpen()
{
    if (!expectedFingerprint.isEmpty() && readFileFingerprint(filename.c_str()) != expectedFingerprint)
        throw opp_runtime_error("Vector file \"%s\" changed on disk", filename.c_str());

    if (db == nullptr) {
        checkOK(sqlite3_open_v2(filename.c_str(), &db, SQLITE_OPEN_READONLY, 0));

        prepareStatement(
            "SELECT vectorId, simtimeExp FROM vector "
            "INNER JOIN run USING (runId);");

        while (true) {
            int resultCode = sqlite3_step(stmt);
            if (resultCode == SQLITE_DONE)
                break;

            checkRow(resultCode);

            int vectorId = sqlite3_column_int(stmt, 0);
            int simtimeExp = sqlite3_column_int(stmt, 1);

            simtimeExpForVectorId[vectorId] = simtimeExp;
        }

        finalizeStatement();
    }
}

int SqliteVectorDataReader::getSimtimeExp(int vectorId)
{
    return simtimeExpForVectorId[vectorId];
}

std::map<int, std::set<int>> SqliteVectorDataReader::groupVectorIdsBySimtimeExp(const std::set<int>& vectorIds)
{
    std::map<int, std::set<int>> result;
    for (int id : vectorIds)
        result[getSimtimeExp(id)].insert(id);
    return result;
}

int SqliteVectorDataReader::getSerialForRowId(int64_t rowId)
{
    prepareStatement(
        "SELECT COUNT(*) "
        "FROM vectorData WHERE rowid < ? "
        " AND vectorId IN (SELECT vectorId FROM vectorData WHERE rowid == ?);");

    checkOK(sqlite3_bind_int64(stmt, 1, rowId));
    checkOK(sqlite3_bind_int64(stmt, 2, rowId));

    int resultCode = sqlite3_step(stmt);

    if (resultCode == SQLITE_DONE) {
        finalizeStatement();
        error("vector entry not found");
    }
    checkRow(resultCode);

    sqlite3_int64 count = sqlite3_column_int64(stmt, 0);

    finalizeStatement();

    return count;
}

int SqliteVectorDataReader::getNumberOfEntries(int vectorId)
{
    ensureDbOpen();

    prepareStatement("SELECT vectorCount FROM vector WHERE vectorId == ?;");

    checkOK(sqlite3_bind_int64(stmt, 1, vectorId));

    int resultCode = sqlite3_step(stmt);

    if (resultCode == SQLITE_DONE) {
        finalizeStatement();
        error("vector not found");
    }
    checkRow(resultCode);

    sqlite3_int64 count = sqlite3_column_int64(stmt, 0);

    finalizeStatement();

    return count;
}

VectorDatum *SqliteVectorDataReader::getEntryBySerial(int vectorId, int64_t serial)
{
    ensureDbOpen();

    prepareStatement(
            "SELECT rowid, eventNumber, simtimeRaw, value "
            "FROM vectorData WHERE vectorId = ? ORDER BY rowid LIMIT 1 OFFSET ?;");

    checkOK(sqlite3_bind_int64(stmt, 1, vectorId));
    checkOK(sqlite3_bind_int64(stmt, 2, serial));

    return getSingleEntry(getSimtimeExp(vectorId));
}

VectorDatum *SqliteVectorDataReader::getEntryBySimtime(int vectorId, simultime_t simtime, bool after)
{
    ensureDbOpen();

    if (after) {
        prepareStatement(
            "SELECT rowid, eventNumber, simtimeRaw, value "
            "FROM vectorData WHERE vectorId = ? AND simtimeRaw >= ? ORDER BY rowid ASC LIMIT 1;");
    }
    else {
        prepareStatement(
            "SELECT rowid, eventNumber, simtimeRaw, value "
            "FROM vectorData WHERE vectorId = ? AND simtimeRaw <= ? ORDER BY rowid DESC LIMIT 1;");
    }

    int simtimeExp = getSimtimeExp(vectorId);

    checkOK(sqlite3_bind_int64(stmt, 1, vectorId));
    checkOK(sqlite3_bind_int64(stmt, 2, simtime.getMantissaForScale(simtimeExp)));

    return getSingleEntry(simtimeExp);
}

VectorDatum *SqliteVectorDataReader::getEntryByEventnum(int vectorId, eventnumber_t eventNum, bool after)
{
    ensureDbOpen();

    if (after) {
        prepareStatement(
            "SELECT rowid, eventNumber, simtimeRaw, value "
            "FROM vectorData WHERE vectorId = ? AND eventNumber >= ? ORDER BY rowid ASC LIMIT 1;");
    }
    else {
        prepareStatement(
            "SELECT rowid, eventNumber, simtimeRaw, value "
            "FROM vectorData WHERE vectorId = ? AND eventNumber <= ? ORDER BY rowid DESC LIMIT 1;");
    }

    checkOK(sqlite3_bind_int64(stmt, 1, vectorId));
    checkOK(sqlite3_bind_int64(stmt, 2, eventNum));

    return getSingleEntry(getSimtimeExp(vectorId));
}

void SqliteVectorDataReader::collectEntries(const std::set<int>& vectorIds)
{
    ensureDbOpen();

    prepareStatement((
        "SELECT vectorId, eventNumber, simtimeRaw, value "
        "FROM vectorData WHERE vectorId IN (" + makePlaceholders(vectorIds.size()) + ") ORDER BY rowid;").c_str());

    int i = 1;
    for (int v : vectorIds)
        checkOK(sqlite3_bind_int64(stmt, i++, v));

    processStatementRows();
}

void SqliteVectorDataReader::collectEntriesInSimtimeInterval(const std::set<int>& vectorIds, simultime_t startTime, simultime_t endTime)
{
    ensureDbOpen();

    std::map<int, std::set<int>> vectorIdGroups = groupVectorIdsBySimtimeExp(vectorIds);

    for (auto vectorIdGroup : vectorIdGroups) {
        int simtimeExp = vectorIdGroup.first;
        std::set<int>& idsInGroup = vectorIdGroup.second;

        if (idsInGroup.empty())
            continue; // safeguard

        int64_t startTimeRaw = startTime.getMantissaForScale(simtimeExp);
        int64_t endTimeRaw = endTime.getMantissaForScale(simtimeExp);

        prepareStatement((
            "SELECT vectorId, eventNumber, simtimeRaw, value "
            "FROM vectorData WHERE vectorId IN (" + makePlaceholders(idsInGroup.size()) + ") "
            " AND simtimeRaw >= ? AND simtimeRaw < ? ORDER BY rowid;").c_str());

        int i = 1;
        for (int v : idsInGroup)
            checkOK(sqlite3_bind_int64(stmt, i++, v));

        checkOK(sqlite3_bind_int64(stmt, i++, startTimeRaw));
        checkOK(sqlite3_bind_int64(stmt, i++, endTimeRaw));

        processStatementRows();
    }
}

void SqliteVectorDataReader::collectEntriesInEventnumInterval(const std::set<int>& vectorIds, eventnumber_t startEventNum, eventnumber_t endEventNum)
{
    ensureDbOpen();

    prepareStatement((
        "SELECT vectorId, eventNumber, simtimeRaw, value "
        "FROM vectorData WHERE vectorId IN (" + makePlaceholders(vectorIds.size()) + ") "
        " AND eventNumber >= ? AND eventNumber < ? ORDER BY rowid;").c_str());

    int i = 1;
    for (int v : vectorIds)
        checkOK(sqlite3_bind_int64(stmt, i++, v));

    checkOK(sqlite3_bind_int64(stmt, i++, startEventNum));
    checkOK(sqlite3_bind_int64(stmt, i++, endEventNum));

    processStatementRows();
}


}  // namespace scave
}  // namespace omnetpp
