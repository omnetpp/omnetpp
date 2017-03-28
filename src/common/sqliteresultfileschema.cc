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
            "statStddev    REAL, "  //TODO why store computed fields? why the "stat" prefix?
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
        "CREATE TABLE IF NOT EXISTS statisticattr "
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

}  // namespace common
}  // namespace omnetpp

