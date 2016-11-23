//=========================================================================
//  RESULTFILEMANAGER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
   `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <utility>
#include <functional>

#include "sqliteresultfileloader.h"

#include "common/opp_ctype.h"
#include "common/matchexpression.h"
#include "common/patternmatcher.h"
#include "common/filereader.h"
#include "common/linetokenizer.h"
#include "common/stringtokenizer.h"
#include "common/filereader.h"
#include "common/fileutil.h"
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "omnetpp/platdep/platmisc.h"
#include "indexfile.h"
#include "scaveutils.h"
#include "scaveexception.h"


#ifdef THREADED
#define READER_MUTEX    Mutex __reader_mutex_(getReadLock());
#define WRITER_MUTEX    Mutex __writer_mutex_(getWriteLock());
#else
#define READER_MUTEX
#define WRITER_MUTEX
#endif

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {


void SqliteResultFileLoader::loadRuns()
{
    // LOAD RUNS
    assert(fileRef != nullptr);
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, "select runid, runname from run;", -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));

    for (int row=1; ; row++) {
        int s = sqlite3_step (stmt);
        if (s == SQLITE_ROW) {
            sqlite3_int64 runId = sqlite3_column_int64(stmt, 0);
            std::string runName  = (const char *)(sqlite3_column_text(stmt, 1));
            Run *runRef = resultFileManager->getRunByName(runName.c_str());
            if (!runRef) {
                // not yet: add it
                runRef = resultFileManager->addRun(false);
                runRef->runName = runName;
            }
            // associate Run with this file
            if (resultFileManager->getFileRun(fileRef, runRef) != nullptr)
                throw opp_runtime_error("non-unique runId '%s' in the sqlite database '%s'", runName.c_str(), fileRef->fileSystemFilePath.c_str());
            FileRun *fileRunRef = resultFileManager->addFileRun(fileRef, runRef);
            fileRunMap[runId] = fileRunRef;
        }
        else if (s == SQLITE_DONE) {
            break;
        }
        else {
            throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
        }
    }
}

void SqliteResultFileLoader::loadRunAttrs()
{
    // LOAD RUNATTRS
    assert(fileRef != nullptr);
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, "select runId, attrName, attrValue from runattr;", -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));

    for (int row=1; ; row++) {
        int s = sqlite3_step (stmt);
        if (s == SQLITE_ROW) {
            sqlite3_int64 runId = sqlite3_column_int64(stmt, 0);
            std::string attrName = (const char *)(sqlite3_column_text(stmt, 1));
            std::string attrValue = (const char *)(sqlite3_column_text(stmt, 2));

            FileRun *fileRunRef = fileRunMap.at(runId);
            StringMap& attributes = fileRunRef->runRef->attributes;
            StringMap::iterator oldPairRef = attributes.find(attrName);
            if (oldPairRef != attributes.end() && oldPairRef->second != attrValue)
                throw opp_runtime_error("Value of run attribute conflicts with previously loaded value");       //FIXME show more info
            attributes[attrName] = attrValue;

            // the "runNumber" attribute is also stored separately
            if (attrName == "runNumber")
                if (!parseInt(attrValue.c_str(), fileRunRef->runRef->runNumber))
                    throw opp_runtime_error("invalid result file: int value expected as runNumber");       //FIXME show more info
        }
        else if (s == SQLITE_DONE) {
            break;
        }
        else {
            throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
        }
    }
}

void SqliteResultFileLoader::loadDbInfo()
{
    assert(fileRef != nullptr);
    // LOAD Scalars
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, "SELECT name FROM sqlite_master WHERE type='table' and name NOT LIKE 'sqlite_%'", -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));

    for (int row=1; ; row++) {
        int s = sqlite3_step(stmt);
        if (s == SQLITE_ROW) {
            std::string tableName = (const char *)sqlite3_column_text(stmt, 0);
            std::cout << "sqlite: tablename: " << tableName << std::endl;
            if (tableName == "scalar")
                hasScalar = true;
            else if (tableName == "vector")
                hasVector = true;
        }
        else if (s == SQLITE_DONE) {
            break;
        }
        else {
            throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
        }
    }
}

double SqliteResultFileLoader::sqlite3ColumnDouble(sqlite3_stmt *stmt, int fieldIdx)
{
    return (sqlite3_column_type(stmt, fieldIdx) != SQLITE_NULL) ? sqlite3_column_double(stmt, fieldIdx) : 0.0/0.0;
}

void SqliteResultFileLoader::loadScalars()
{
    assert(fileRef != nullptr);
    // LOAD Scalars
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, "select scalarId, runId, moduleName, scalarName, scalarValue from scalar;", -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));

    typedef std::map<sqlite3_int64,int> SqliteScalarIdToScalarIdx;
    SqliteScalarIdToScalarIdx sqliteScalarIdToScalarIdx;

    for (int row=1; ; row++) {
        int s = sqlite3_step(stmt);
        if (s == SQLITE_ROW) {
            sqlite3_int64 scalarId = sqlite3_column_int64(stmt, 0);
            sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
            std::string moduleName = (const char *)sqlite3_column_text(stmt, 2);
            std::string scalarName = (const char *)sqlite3_column_text(stmt, 3);
            double scalarValue = sqlite3ColumnDouble(stmt,4);        // converts NULL to NaN
            int i = resultFileManager->addScalar(fileRunMap.at(runId), moduleName.c_str(), scalarName.c_str(), scalarValue, false);
            sqliteScalarIdToScalarIdx[scalarId] = i;
        }
        else if (s == SQLITE_DONE) {
            break;
        }
        else {
            throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
        }
    }
    sqlite3_finalize(stmt);
    if (sqlite3_prepare_v2(db, "select scalarId, runId, attrName, attrValue from scalarattr join scalar using (scalarId) order by runId, scalarId;", -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
    for (int row=1; ; row++) {
        int s = sqlite3_step(stmt);
        if (s == SQLITE_ROW) {
            sqlite3_int64 scalarId = sqlite3_column_int64(stmt, 0);
            sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
            std::string attrName = (const char *)sqlite3_column_text(stmt, 2);
            std::string attrValue = (const char *)sqlite3_column_text(stmt, 3);
            SqliteScalarIdToScalarIdx::iterator it = sqliteScalarIdToScalarIdx.find(scalarId);
            if (it == sqliteScalarIdToScalarIdx.end())
                throw opp_runtime_error("Invalid scalarId %li in scalarattr table", scalarId);
            ScalarResult& sca = fileRunMap.at(runId)->fileRef->scalarResults.at(sqliteScalarIdToScalarIdx.at(scalarId));
            sca.attributes[attrName] = attrValue;
        }
        else if (s == SQLITE_DONE) {
            break;
        }
        else {
            throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
        }
    }

    loadHistograms();
}


void SqliteResultFileLoader::loadHistograms()
{
    Statistics stat;
    StringMap attrs;
    assert(fileRef != nullptr);
    // LOAD Scalars
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, "select histId, runId, moduleName, histName, "
            "histCount, "
            "histMean, histStddev, histSum, histSqrsum, histMin, histMax, "
            "histWeights, histWeightedSum, histSqrSumWeights, histWeightedSqrSum "
            "from histogram;", -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));

    typedef std::map<sqlite3_int64,int> SqliteHistogramIdToHistogramIdx;
    SqliteHistogramIdToHistogramIdx sqliteHistogramIdToHistogramIdx;

    for (int row=1; ; row++) {
        int s = sqlite3_step(stmt);
        if (s == SQLITE_ROW) {
            sqlite3_int64 histId = sqlite3_column_int64(stmt, 0);
            sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
            std::string moduleName = (const char *)sqlite3_column_text(stmt, 2);
            std::string histName = (const char *)sqlite3_column_text(stmt, 3);
            sqlite3_int64 histCount = sqlite3_column_int64(stmt, 4);
            double histMean = sqlite3ColumnDouble(stmt, 5);
            double histStddev = sqlite3ColumnDouble(stmt, 6);
            double histSum = sqlite3ColumnDouble(stmt, 7);
            double histSqrsum = sqlite3ColumnDouble(stmt, 8);
            double histMin = sqlite3ColumnDouble(stmt, 9);
            double histMax = sqlite3ColumnDouble(stmt, 10);
            double histWeights = sqlite3ColumnDouble(stmt, 11);
            double histWeightedsum = sqlite3ColumnDouble(stmt, 12);
            double histSqrSumWeights = sqlite3ColumnDouble(stmt, 13);
            double histWeightedSqrSum = sqlite3ColumnDouble(stmt, 14);
            Statistics stat(histCount, histMin, histMax, histSum, histSqrsum);
            sqliteHistogramIdToHistogramIdx[histId] = resultFileManager->addHistogram(fileRunMap.at(runId), moduleName.c_str(), histName.c_str(), stat, StringMap());
        }
        else if (s == SQLITE_DONE) {
            break;
        }
        else {
            throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
        }
    }
    sqlite3_finalize(stmt);

    if (sqlite3_prepare_v2(db, "select histId, runId, attrName, attrValue from histattr join histogram using (histId) order by runId, histId;", -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
    for (int row=1; ; row++) {
        int s = sqlite3_step(stmt);
        if (s == SQLITE_ROW) {
            sqlite3_int64 histId = sqlite3_column_int64(stmt, 0);
            sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
            std::string attrName = (const char *)sqlite3_column_text(stmt, 2);
            std::string attrValue = (const char *)sqlite3_column_text(stmt, 3);
            SqliteHistogramIdToHistogramIdx::iterator it = sqliteHistogramIdToHistogramIdx.find(histId);
            if (it == sqliteHistogramIdToHistogramIdx.end())
                throw opp_runtime_error("Invalid scalarId %li in histattr table");
            HistogramResult& hist = fileRunMap.at(runId)->fileRef->histogramResults.at(sqliteHistogramIdToHistogramIdx.at(histId));
            hist.attributes[attrName] = attrValue;
        }
        else if (s == SQLITE_DONE) {
            break;
        }
        else {
            throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
        }
    }
    sqlite3_finalize(stmt);

    if (sqlite3_prepare_v2(db, "select histId, runId, baseValue, cellValue from histbin join histogram using (histId) order by runId, histId;", -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
    for (int row=1; ; row++) {
        int s = sqlite3_step(stmt);
        if (s == SQLITE_ROW) {
            sqlite3_int64 histId = sqlite3_column_int64(stmt, 0);
            sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
            double baseValue = sqlite3_column_double(stmt, 2);
            sqlite3_int64 cellValue = sqlite3_column_int64(stmt, 3);
            SqliteHistogramIdToHistogramIdx::iterator it = sqliteHistogramIdToHistogramIdx.find(histId);
            if (it == sqliteHistogramIdToHistogramIdx.end())
                throw opp_runtime_error("Invalid scalarId %li in histbin table");
            HistogramResult& hist = fileRunMap.at(runId)->fileRef->histogramResults.at(sqliteHistogramIdToHistogramIdx.at(histId));
            hist.addBin(baseValue, cellValue);
        }
        else if (s == SQLITE_DONE) {
            break;
        }
        else {
            throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
        }
    }
    sqlite3_finalize(stmt);
}


// select vectorId, runId, moduleName, vectorName, count, vmin, vmax, vsum, vsumsqr, startEventNum, endEventNum  from vector left join (select vectorId, runId, count(value) as count, min(value) as vmin, max(value) as vmax,     sum(value) as vsum, sum(value*value) as vsumsqr     min(eventNumber) as startEventNum, max(eventNumber) as endEventNum    from vectordata group by vectorId ) as vectorstat using (vectorId, runId);

void SqliteResultFileLoader::loadVectors()
{
    assert(fileRef != nullptr);
    // LOAD Vectors
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db,
            "select vectorId, runId, moduleName, vectorName, count, vmin, vmax, vsum, vsumsqr, startEventNum, endEventNum, "
            "    startSimtimeRaw, endSimtimeRaw, simtimeExp "
            "from vector "
            "left join run using (runId) "
            "left join (select vectorId, count(value) as count, min(value) as vmin, max(value) as vmax, "
            "    sum(value) as vsum, sum(value*value) as vsumsqr, "
            "    min(eventNumber) as startEventNum, "
            "    max(eventNumber) as endEventNum, "
            "    min(simtimeRaw) as startSimtimeRaw, "
            "    max(simtimeRaw) as endSimtimeRaw "
            "    from vectordata group by vectorId ) as vectorstat "
            "using (vectorId);",
            -1, &stmt, nullptr) != SQLITE_OK)
        throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));

    for (int row=1; ; row++) {
        int s = sqlite3_step(stmt);
        if (s == SQLITE_ROW) {
            sqlite3_int64 vectorId = sqlite3_column_int64(stmt, 0);
            sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
            std::string moduleName = (const char *)sqlite3_column_text(stmt, 2);
            std::string vectorName = (const char *)sqlite3_column_text(stmt, 3);
            sqlite3_int64 count = sqlite3_column_int64(stmt, 4);
            double vmin = sqlite3ColumnDouble(stmt, 5);
            double vmax = sqlite3ColumnDouble(stmt, 6);
            double vsum = sqlite3ColumnDouble(stmt, 7);
            double vsumsqr = sqlite3ColumnDouble(stmt, 8);
            sqlite3_int64 startEventNum = sqlite3_column_int64(stmt, 9);
            sqlite3_int64 endEventNum = sqlite3_column_int64(stmt, 10);
            sqlite3_int64 startSimtimeRaw = sqlite3_column_int64(stmt, 11);
            sqlite3_int64 endSimtimeRaw = sqlite3_column_int64(stmt, 12);
            int simtimeExp = sqlite3_column_int(stmt, 13);
            int i = resultFileManager->addVector(fileRunMap.at(runId), vectorId, moduleName.c_str(), vectorName.c_str(), "ETV");
            VectorResult& vec = fileRunMap.at(runId)->fileRef->vectorResults.at(i);
            vec.stat = Statistics(count, vmin, vmax, vsum, vsumsqr);
            vec.startEventNum = startEventNum;
            vec.endEventNum = endEventNum;
            vec.startTime = simultime_t(startSimtimeRaw, simtimeExp);
            vec.endTime = simultime_t(endSimtimeRaw, simtimeExp);
        }
        else if (s == SQLITE_DONE) {
            break;
        }
        else {
            throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
        }
    }
    sqlite3_finalize(stmt);
}

ResultFile *SqliteResultFileLoader::loadFile(const char *fileName, const char *fileSystemFileName, bool reload)
{
    // add to fileList
    fileRef = nullptr;

    try {
        fileRef = resultFileManager->addFile(fileName, fileSystemFileName, false);

        if (sqlite3_open_v2(fileSystemFileName, &db, SQLITE_OPEN_READONLY, 0) != SQLITE_OK)
            throw opp_runtime_error("Can't open sqlite database '%s': %s\n", fileSystemFileName, sqlite3_errmsg(db));

        loadDbInfo();
        loadRuns();
        loadRunAttrs();
        if (hasScalar)
            loadScalars();
        if (hasVector)
            loadVectors();
    }
    catch (std::exception&) {
        if (db != nullptr)
            sqlite3_close(db);
        db = nullptr;
        try {
            if (fileRef)
                resultFileManager->unloadFile(fileRef);
        }
        catch (...) {
        }
        throw;
    }
    return fileRef;
}

}  // namespace scave
}  // namespace omnetpp
