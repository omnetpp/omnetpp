//==========================================================================
//  SQLITEOUTPUTSCALARMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Zoltan Bojthe, Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_SQLITEOUTPUTSCALARMGR_H
#define __OMNETPP_ENVIR_SQLITEOUTPUTSCALARMGR_H

#include <cstdio>
#include "omnetpp/envirext.h"
#include "omnetpp/simutil.h"
#include "envir/envirdefs.h"
#include "common/sqlitescalarfilewriter.h"
#include "resultfileutils.h"

namespace omnetpp {
namespace envir {

using omnetpp::common::SqliteScalarFileWriter;

/**
 * A cIOutputScalarManager that uses an SQLite database as output.
 *
 * @ingroup Envir
 */
class SqliteOutputScalarManager : public cIOutputScalarManager
{
  protected:
    bool initialized;    // true after first call to initialize(), even if it failed
    std::string fname;
    SqliteScalarFileWriter writer;

  protected:
    void openDb();
    void closeDb();
    void writeRunData();
    virtual void initialize();
    virtual void recordNumericIterationVariableAsScalar(const char *name, const char *value); // i.e. write *if* numeric
    bool isBad() {return initialized && !writer.isOpen();}

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit SqliteOutputScalarManager();

    /**
     * Destructor.
     */
    virtual ~SqliteOutputScalarManager();
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
     * This operation may invoke the setUpBins() method on the histogram object.
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
