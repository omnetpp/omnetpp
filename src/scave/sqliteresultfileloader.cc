//=========================================================================
//  SQLITERESULTFILELOADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Zoltan Bojthe, Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
   `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <utility>
#include "omnetpp/platdep/platdefs.h"
#include "common/bigdecimal.h"
#include "common/histogram.h"
#include "sqliteresultfileloader.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {


SqliteResultFileLoader::SqliteResultFileLoader(ResultFileManager *resultFileManagerPar) :
    IResultFileLoader(resultFileManagerPar), db(nullptr), stmt(nullptr), fileRef(nullptr)
{
}

SqliteResultFileLoader::~SqliteResultFileLoader()
{
}

inline void SqliteResultFileLoader::checkOK(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_OK)
        error(sqlite3_errmsg(db));
}

inline void SqliteResultFileLoader::checkRow(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_ROW)
        error(sqlite3_errmsg(db));
}

void SqliteResultFileLoader::error(const char *errmsg)
{
    throw opp_runtime_error("Cannot read SQLite result file '%s': %s", fileRef->getFileName().c_str(), errmsg);
}

void SqliteResultFileLoader::prepareStatement(const char *sql)
{
    assert(stmt == nullptr);
    checkOK(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr));
}

void SqliteResultFileLoader::finalizeStatement()
{
    assert(stmt != nullptr);
    sqlite3_finalize(stmt);
    stmt = nullptr;
}

void SqliteResultFileLoader::loadRuns()
{
    prepareStatement("SELECT runId, runName FROM run;");

    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 0);
        std::string runName  = (const char *) sqlite3_column_text(stmt, 1);
        Run *runRef = resultFileManager->getOrAddRun(runName);
        if (resultFileManager->getFileRun(fileRef, runRef) != nullptr)
            error("Non-unique runId in run table");
        FileRun *fileRunRef = resultFileManager->addFileRun(fileRef, runRef);
        fileRunMap[runId] = fileRunRef;
    }
    finalizeStatement();
}

void SqliteResultFileLoader::loadRunAttrs()
{
    prepareStatement("SELECT runId, attrName, attrValue FROM runAttr;");

    for (int row=1; ; row++) {
        int resultCode = sqlite3_step (stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 0);
        std::string attrName = (const char *) sqlite3_column_text(stmt, 1);
        std::string attrValue = (const char *) sqlite3_column_text(stmt, 2);

        FileRun *fileRunRef = fileRunMap.at(runId);
        const StringMap& attributes = fileRunRef->runRef->getAttributes();
        StringMap::const_iterator oldPairRef = attributes.find(attrName);
        if (oldPairRef != attributes.end() && oldPairRef->second != attrValue)
            error("Value of run attribute conflicts with previously loaded value");
        fileRunRef->runRef->setAttribute(attrName, attrValue);
    }
    finalizeStatement();
}

void SqliteResultFileLoader::loadRunItervars()
{
    prepareStatement("SELECT runId, itervarName, itervarValue FROM runItervar;");

    for (int row=1; ; row++) {
        int resultCode = sqlite3_step (stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 0);
        std::string itervarName = (const char *) sqlite3_column_text(stmt, 1);
        std::string itervarValue = (const char *) sqlite3_column_text(stmt, 2);

        FileRun *fileRunRef = fileRunMap.at(runId);
        const StringMap& itervars = fileRunRef->runRef->getIterationVariables();
        StringMap::const_iterator oldPairRef = itervars.find(itervarName);
        if (oldPairRef != itervars.end() && oldPairRef->second != itervarValue)
            error("Value of iteration variable conflicts with previously loaded value");
        fileRunRef->runRef->itervars[itervarName] = itervarValue;
    }
    finalizeStatement();
}

void SqliteResultFileLoader::loadRunParams()
{
    prepareStatement("SELECT runId, paramKey, paramValue FROM runParam ORDER BY paramOrder ASC;");

    for (int row=1; ; row++) {
        int resultCode = sqlite3_step (stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 0);
        std::string paramKey = (const char *) sqlite3_column_text(stmt, 1);
        std::string paramValue = (const char *) sqlite3_column_text(stmt, 2);

        FileRun *fileRunRef = fileRunMap.at(runId);
        fileRunRef->runRef->addParamAssignmentEntry(paramKey, paramValue); //FIXME if not already exists
    }
    finalizeStatement();
}

double SqliteResultFileLoader::sqlite3ColumnDouble(sqlite3_stmt *stmt, int fieldIdx)
{
    return (sqlite3_column_type(stmt, fieldIdx) != SQLITE_NULL) ? sqlite3_column_double(stmt, fieldIdx) : NAN;
}

void SqliteResultFileLoader::loadScalars()
{
    typedef std::map<sqlite3_int64,int> SqliteScalarIdToScalarIdx;
    SqliteScalarIdToScalarIdx sqliteScalarIdToScalarIdx;
    const StringMap emptyAttrs;

    prepareStatement("SELECT scalarId, runId, moduleName, scalarName, scalarValue FROM scalar;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 scalarId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        std::string moduleName = (const char *)sqlite3_column_text(stmt, 2);
        std::string scalarName = (const char *)sqlite3_column_text(stmt, 3);
        double scalarValue = sqlite3ColumnDouble(stmt,4);        // converts NULL to NaN
        int i = resultFileManager->addScalar(fileRunMap.at(runId), moduleName.c_str(), scalarName.c_str(), emptyAttrs, scalarValue, false, false);
        sqliteScalarIdToScalarIdx[scalarId] = i;
    }
    finalizeStatement();

    prepareStatement("SELECT scalarId, runId, attrName, attrValue FROM scalarAttr JOIN scalar USING (scalarId) ORDER BY runId, scalarId;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 scalarId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        std::string attrName = (const char *)sqlite3_column_text(stmt, 2);
        std::string attrValue = (const char *)sqlite3_column_text(stmt, 3);
        SqliteScalarIdToScalarIdx::iterator it = sqliteScalarIdToScalarIdx.find(scalarId);
        if (it == sqliteScalarIdToScalarIdx.end())
            error("Invalid scalarId in scalarAttr table");
        ScalarResult& sca = fileRunMap.at(runId)->fileRef->scalarResults.at(sqliteScalarIdToScalarIdx.at(scalarId));
        sca.setAttribute(attrName, attrValue);
    }
    finalizeStatement();
}

void SqliteResultFileLoader::loadHistograms()
{
    std::map<sqlite3_int64,int> sqliteStatIdToStatisticsIdx;
    std::map<sqlite3_int64,int> sqliteStatIdToHistogramIdx;
    const StringMap emptyAttrs;

    prepareStatement("SELECT statId, runId, moduleName, statName, isHistogram, isWeighted, "
            "statCount, statMean, statStddev, statSum, statSqrsum, statMin, statMax, "
            "statWeights, statWeightedSum, statSqrSumWeights, statWeightedSqrSum "
            "FROM statistic;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 statId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        std::string moduleName = (const char *)sqlite3_column_text(stmt, 2);
        std::string statName = (const char *)sqlite3_column_text(stmt, 3);
        bool isHistogram = sqlite3_column_int(stmt, 4);
        bool isWeighted = sqlite3_column_int(stmt, 5);
        sqlite3_int64 statCount = sqlite3_column_int64(stmt, 6);
        double statMean = sqlite3ColumnDouble(stmt, 7); // can be computed from the others
        double statStddev = sqlite3ColumnDouble(stmt, 8); // can be computed from the others
        double statSum = sqlite3ColumnDouble(stmt, 9);
        double statSqrsum = sqlite3ColumnDouble(stmt, 10);
        double statMin = sqlite3ColumnDouble(stmt, 11);
        double statMax = sqlite3ColumnDouble(stmt, 12);
        double statWeights = sqlite3ColumnDouble(stmt, 13);
        double statWeightedsum = sqlite3ColumnDouble(stmt, 14);
        double statSqrSumWeights = sqlite3ColumnDouble(stmt, 15);
        double statWeightedSqrSum = sqlite3ColumnDouble(stmt, 16);
        (void)statMean; (void)statStddev; // unused vars; we'll recompute mean and stddev from the other fields

        Statistics stat;
        if (isWeighted)
            stat = Statistics::makeWeighted(statCount, statMin, statMax, statWeights, statWeightedsum, statSqrSumWeights, statWeightedSqrSum);
        else
            stat = Statistics::makeUnweighted(statCount, statMin, statMax, statSum, statSqrsum);
        if (isHistogram)
            sqliteStatIdToHistogramIdx[statId] = resultFileManager->addHistogram(fileRunMap.at(runId), moduleName.c_str(), statName.c_str(), stat, Histogram(), emptyAttrs);
        else
            sqliteStatIdToStatisticsIdx[statId] = resultFileManager->addStatistics(fileRunMap.at(runId), moduleName.c_str(), statName.c_str(), stat, emptyAttrs);
    }
    finalizeStatement();

    prepareStatement("SELECT statId, runId, attrName, attrValue FROM statisticAttr JOIN statistic USING (statId) ORDER BY runId, statId;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 statId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        std::string attrName = (const char *)sqlite3_column_text(stmt, 2);
        std::string attrValue = (const char *)sqlite3_column_text(stmt, 3);

        auto it = sqliteStatIdToHistogramIdx.find(statId);
        if (it != sqliteStatIdToHistogramIdx.end()) {
            HistogramResult& hist = fileRunMap.at(runId)->fileRef->histogramResults.at(sqliteStatIdToHistogramIdx.at(statId));
            hist.setAttribute(attrName, attrValue);
            continue;
        }
        it = sqliteStatIdToStatisticsIdx.find(statId);
        if (it != sqliteStatIdToStatisticsIdx.end()) {
            StatisticsResult& stats = fileRunMap.at(runId)->fileRef->statisticsResults.at(sqliteStatIdToStatisticsIdx.at(statId));
            stats.setAttribute(attrName, attrValue);
            continue;
        }
        error("Invalid statId in statisticAttr table");
    }
    finalizeStatement();

    prepareStatement("SELECT statId, runId, lowerEdge, binValue FROM histogramBin JOIN statistic USING (statId) ORDER BY runId, statId;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 statId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        double lowerEdge = sqlite3_column_double(stmt, 2);
        double binValue = sqlite3_column_double(stmt, 3);
        auto it = sqliteStatIdToHistogramIdx.find(statId);
        if (it == sqliteStatIdToHistogramIdx.end())
            error("Invalid statId in histogramBin table, or isHistogram field in the corresponding statistic table record is not set");
        HistogramResult& hist = fileRunMap.at(runId)->fileRef->histogramResults.at(sqliteStatIdToHistogramIdx.at(statId));
        hist.addBin(lowerEdge, binValue);
    }
    finalizeStatement();
}

void SqliteResultFileLoader::loadVectors()
{
    std::map<sqlite3_int64,int> sqliteVectorIdToVectorIdx;

    prepareStatement(
            "SELECT vectorId, runId, moduleName, vectorName, "
            "    vectorCount, vectorMin, vectorMax, vectorSum, vectorSumSqr, "
            "    startEventNum, endEventNum, startSimtimeRaw, endSimtimeRaw, simtimeExp "
            "FROM vector LEFT JOIN run USING (runId);");

    const StringMap emptyAttrs;
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
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
        int i = resultFileManager->addVector(fileRunMap.at(runId), vectorId, moduleName.c_str(), vectorName.c_str(), emptyAttrs, "ETV");
        sqliteVectorIdToVectorIdx[vectorId] = i;
        VectorResult& vec = fileRunMap.at(runId)->fileRef->vectorResults.at(i);
        vec.stat = Statistics::makeUnweighted(count, vmin, vmax, vsum, vsumsqr);
        vec.startEventNum = startEventNum;
        vec.endEventNum = endEventNum;
        vec.startTime = simultime_t(startSimtimeRaw, simtimeExp);
        vec.endTime = simultime_t(endSimtimeRaw, simtimeExp);
    }
    finalizeStatement();

    prepareStatement("SELECT vectorId, runId, attrName, attrValue FROM vectorAttr JOIN vector USING (vectorId) ORDER BY runId, vectorId;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 vectorId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        std::string attrName = (const char *)sqlite3_column_text(stmt, 2);
        std::string attrValue = (const char *)sqlite3_column_text(stmt, 3);
        auto it = sqliteVectorIdToVectorIdx.find(vectorId);
        if (it == sqliteVectorIdToVectorIdx.end())
            error("Invalid vectorId in vectorAttr table");
        VectorResult& vec = fileRunMap.at(runId)->fileRef->vectorResults.at(sqliteVectorIdToVectorIdx.at(vectorId));
        vec.setAttribute(attrName, attrValue);
    }
    finalizeStatement();
}

void SqliteResultFileLoader::cleanupDb()
{
    if (db != nullptr) {
        sqlite3_close(db);
        sqlite3_finalize(stmt);
        db = nullptr;
        stmt = nullptr;
    }
}

ResultFile *SqliteResultFileLoader::loadFile(const char *fileName, const char *fileSystemFileName, bool reload)
{
    try {
        fileRef = resultFileManager->addFile(fileName, fileSystemFileName, ResultFile::FILETYPE_SQLITE, false);
        checkOK(sqlite3_open_v2(fileSystemFileName, &db, SQLITE_OPEN_READONLY, 0));
        loadRuns();
        loadRunAttrs();
        loadRunParams();
        loadRunItervars();
        loadScalars();
        loadVectors();
        loadHistograms();
    }
    catch (std::exception&) {
        cleanupDb();
        try {
            if (fileRef)
                resultFileManager->unloadFile(fileRef);
        } catch (...) {}

        throw;
    }
    cleanupDb();
    return fileRef;
}

}  // namespace scave
}  // namespace omnetpp
