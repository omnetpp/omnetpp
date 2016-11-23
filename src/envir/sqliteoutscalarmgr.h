//==========================================================================
//  SQLITEOUTPUTSCALARMGR.H - part of
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

#ifndef __OMNETPP_ENVIR_SQLITEOUTPUTSCALARMGR_H
#define __OMNETPP_ENVIR_SQLITEOUTPUTSCALARMGR_H

#include <cstdio>
#include "omnetpp/envirext.h"
#include "omnetpp/simutil.h"
#include "envir/envirdefs.h"
#include "envir/runattributes.h"

#include "common/sqlite3.h"

namespace omnetpp {
namespace envir {

/**
 * A cIOutputScalarManager that uses a line-oriented text file as output.
 *
 * @ingroup Envir
 */
class cSqliteOutputScalarManager : public cIOutputScalarManager
{
  protected:
    RunData run;       // holds data of the current run
    std::string fname; // output file name
    sqlite_int64 runId;  // runId in sqlite database
    sqlite3 *db;       // sqlite database
    sqlite3_stmt *add_scalar_stmt;
    sqlite3_stmt *add_scalar_attr_stmt;
    sqlite3_stmt *add_statistic_stmt;
    sqlite3_stmt *add_statistic_attr_stmt;
    sqlite3_stmt *add_statistic_bin_stmt;
    // we COMMIT after every commitFreq INSERT statements
    int commitFreq;
    int insertCount;

  protected:
    void openDb();
    void closeDb();
    void commitDb();
    void init();
    void prepareStmt(sqlite3_stmt **stmt, const char *sql);
    sqlite_int64 writeScalar(const std::string &componentFullPath, const char *name, double value);
    void writeScalarAttr(sqlite_int64 scalarId, const char *name, size_t nameLength, const char *value, size_t valueLength);
    void recordNumericIterationVariable(const char *name, const char *value); // i.e. write *if* numeric
    void writeStatisticAttr(sqlite_int64 statisticId, const char *name, const char *value);
    void writeStatisticBin(sqlite_int64 statisticId, double basePoint, unsigned long cellValue);

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cSqliteOutputScalarManager();

    /**
     * Destructor.
     */
    virtual ~cSqliteOutputScalarManager();
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * Opens collecting. Called at the beginning of a simulation run.
     */
    virtual void startRun() override;

    /**
     * Closes collecting. Called at the end of a simulation run.
     */
    virtual void endRun() override;

    /** @name Scalar statistics */
    //@{

    /**
     * Records a double scalar result into the scalar result file.
     */
    void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=nullptr) override;

    /**
     * Records a histogram or statistic object into the scalar result file.
     * This operation may invoke the transform() method on the histogram object.
     */
    void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=nullptr) override;

    /**
     * Returns the file name.
     */
    const char *getFileName() const override;

    /**
     * Calls fflush().
     */
    virtual void flush() override;
    //@}
};

} // namespace envir
}  // namespace omnetpp

#endif
