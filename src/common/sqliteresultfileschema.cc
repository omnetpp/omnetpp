//==========================================================================
//  SQLITERESULTFILESCHEMA.CC - part of
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

#include "sqliteresultfileschema.h"

namespace omnetpp {
namespace common {

const char SQL_CREATE_TABLES[] =
        "PRAGMA foreign_keys = ON; "
        "BEGIN IMMEDIATE TRANSACTION; "
        "CREATE TABLE IF NOT EXISTS run "
        "( "
            "runId       INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runName     TEXT NOT NULL, "
            "simtimeExp  INTEGER NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS runAttr "
        "( "
            "runId       INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE, "
            "attrName    TEXT NOT NULL, "
            "attrValue   TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS runItervar "
        "( "
            "runId       INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE, "
            "itervarName TEXT NOT NULL, "
            "itervarValue TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS runParam "
        "( "
            "runId       INTEGER NOT NULL REFERENCES run(runId) ON DELETE CASCADE, "
            "paramKey    TEXT NOT NULL, "
            "paramValue  TEXT NOT NULL, "
            "paramOrder  INTEGER NOT NULL "
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
        "CREATE TABLE IF NOT EXISTS scalarAttr "
        "( "
            "scalarId      INTEGER  NOT NULL REFERENCES scalar(scalarId) ON DELETE CASCADE, "
            "attrName      TEXT NOT NULL, "
            "attrValue     TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS statistic "
        "( "
            "statId        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runId         INTEGER NOT NULL REFERENCES run(runId) ON DELETE CASCADE, "
            "moduleName    TEXT NOT NULL, "
            "statName      TEXT NOT NULL, "
            "isHistogram   INTEGER NOT NULL, "  //actually, BOOL
            "isWeighted    INTEGER NOT NULL, "  //actually, BOOL
            "statCount     INTEGER NOT NULL, "
            "statMean      REAL, "  // note: computed; stored for convenience
            "statStddev    REAL, "  // note: computed; stored for convenience
            "statSum       REAL, "
            "statSqrsum    REAL, "
            "statMin       REAL, "
            "statMax       REAL, "
            "statWeights          REAL, " // note: name of this and subsequent fields is consistent with textual sca file field names
            "statWeightedSum      REAL, "
            "statSqrSumWeights    REAL, "
            "statWeightedSqrSum   REAL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS statisticAttr "
        "( "
            "statId        INTEGER NOT NULL REFERENCES statistic(statId) ON DELETE CASCADE, "
            "attrName      TEXT NOT NULL, "
            "attrValue     TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS histogramBin "
        "( "
            "statId        INTEGER NOT NULL REFERENCES statistic(statId) ON DELETE CASCADE, "
            "lowerEdge     NUMERIC NOT NULL, "
            "binValue      NUMERIC NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS vector "
        "( "
            "vectorId        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "runId           INTEGER NOT NULL REFERENCES run(runId) ON DELETE CASCADE, "
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
        "CREATE TABLE IF NOT EXISTS vectorAttr "
        "( "
            "vectorId      INTEGER  NOT NULL REFERENCES vector(vectorId) ON DELETE CASCADE, "
            "attrName      TEXT NOT NULL, "
            "attrValue     TEXT NOT NULL "
        "); "
        ""
        "CREATE TABLE IF NOT EXISTS vectorData "
        "( "
            "vectorId      INTEGER NOT NULL REFERENCES vector(vectorId) ON DELETE CASCADE, "
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

}  // namespace common
}  // namespace omnetpp

