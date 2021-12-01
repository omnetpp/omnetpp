//==========================================================================
//  SQLITERESULTFILESCHEMA.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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

const char SQL_CREATE_TABLES[] = R"EOT(
    PRAGMA foreign_keys = ON;
    BEGIN IMMEDIATE TRANSACTION;
    CREATE TABLE IF NOT EXISTS run
    (
        runId       INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        runName     TEXT NOT NULL,
        simtimeExp  INTEGER NOT NULL
    );

    CREATE TABLE IF NOT EXISTS runAttr
    (
        runId       INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE,
        attrName    TEXT NOT NULL,
        attrValue   TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS runItervar
    (
        runId       INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE,
        itervarName TEXT NOT NULL,
        itervarValue TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS runConfig
    (
        runId       INTEGER NOT NULL REFERENCES run(runId) ON DELETE CASCADE,
        configKey   TEXT NOT NULL,
        configValue TEXT NOT NULL,
        configOrder INTEGER NOT NULL
    );

    CREATE TABLE IF NOT EXISTS scalar
    (
        scalarId      INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        runId         INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE,
        moduleName    TEXT NOT NULL,
        scalarName    TEXT NOT NULL,
        scalarValue   REAL        -- cannot be NOT NULL, because sqlite converts NaN double value to NULL
    );

    CREATE TABLE IF NOT EXISTS scalarAttr
    (
        scalarId      INTEGER  NOT NULL REFERENCES scalar(scalarId) ON DELETE CASCADE,
        attrName      TEXT NOT NULL,
        attrValue     TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS parameter
    (
        paramId       INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        runId         INTEGER  NOT NULL REFERENCES run(runId) ON DELETE CASCADE,
        moduleName    TEXT NOT NULL,
        paramName     TEXT NOT NULL,
        paramValue    TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS paramAttr
    (
        paramId       INTEGER  NOT NULL REFERENCES parameter(paramId) ON DELETE CASCADE,
        attrName      TEXT NOT NULL,
        attrValue     TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS statistic
    (
        statId        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        runId         INTEGER NOT NULL REFERENCES run(runId) ON DELETE CASCADE,
        moduleName    TEXT NOT NULL,
        statName      TEXT NOT NULL,
        isHistogram   INTEGER NOT NULL,   -- actually, BOOL
        isWeighted    INTEGER NOT NULL,   -- actually, BOOL
        statCount     INTEGER NOT NULL,
        statMean      REAL,   -- note: computed; stored for convenience
        statStddev    REAL,   -- note: computed; stored for convenience
        statSum       REAL,
        statSqrsum    REAL,
        statMin       REAL,
        statMax       REAL,
        statWeights          REAL,  -- note: names of this and subsequent fields are consistent with textual sca file field names
        statWeightedSum      REAL,
        statSqrSumWeights    REAL,
        statWeightedSqrSum   REAL
    );

    CREATE TABLE IF NOT EXISTS statisticAttr
    (
        statId        INTEGER NOT NULL REFERENCES statistic(statId) ON DELETE CASCADE,
        attrName      TEXT NOT NULL,
        attrValue     TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS histogramBin
    (
        statId        INTEGER NOT NULL REFERENCES statistic(statId) ON DELETE CASCADE,
        lowerEdge     REAL NOT NULL,
        binValue      REAL NOT NULL
    );

    CREATE TABLE IF NOT EXISTS vector
    (
        vectorId        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        runId           INTEGER NOT NULL REFERENCES run(runId) ON DELETE CASCADE,
        moduleName      TEXT NOT NULL,
        vectorName      TEXT NOT NULL,
        vectorCount     INTEGER,    -- cannot be NOT NULL because we fill it in later
        vectorMin       REAL,
        vectorMax       REAL,
        vectorSum       REAL,
        vectorSumSqr    REAL,
        startEventNum   INTEGER,
        endEventNum     INTEGER,
        startSimtimeRaw INTEGER,
        endSimtimeRaw   INTEGER

    );

    CREATE TABLE IF NOT EXISTS vectorAttr
    (
        vectorId      INTEGER  NOT NULL REFERENCES vector(vectorId) ON DELETE CASCADE,
        attrName      TEXT NOT NULL,
        attrValue     TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS vectorData
    (
        vectorId      INTEGER NOT NULL REFERENCES vector(vectorId) ON DELETE CASCADE,
        eventNumber   INTEGER NOT NULL,
        simtimeRaw    INTEGER NOT NULL,
        value         REAL  -- cannot be NOT NULL because of NaN values
    );

    COMMIT TRANSACTION;

    PRAGMA synchronous = OFF;
    PRAGMA journal_mode = TRUNCATE;
    PRAGMA cache_size = 100000;
    PRAGMA page_size = 16384;
)EOT";


}  // namespace common
}  // namespace omnetpp

