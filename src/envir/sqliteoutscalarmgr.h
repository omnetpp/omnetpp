//==========================================================================
//  SQLITEOUTPUTSCALARMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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
    enum State {NEW, STARTED, OPENED, ENDED} state = NEW;
    std::string fname;
    SqliteScalarFileWriter writer;

  protected:
    virtual void openFileForRun();
    virtual void closeFile();
    bool isBad() {return state==OPENED && !writer.isOpen();}

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    SqliteOutputScalarManager() {}

    /**
     * Destructor.
     */
    virtual ~SqliteOutputScalarManager() {closeFile();}
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
    virtual bool recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=nullptr) override;

    /**
     * Records a histogram or statistic object into the scalar result file.
     * This operation may invoke the setUpBins() method on the histogram object.
     */
    virtual bool recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=nullptr) override;

    /**
     * Records a module or channel parameter into the scalar result file.
     */
    virtual bool recordParameter(cPar *par) override;

    /**
     * Records the runtime NED type of module or channel into the scalar result file.
     */
    virtual bool recordComponentType(cComponent *component) override;

    /**
     * Returns the file name.
     */
    const virtual char *getFileName() const override {return fname.c_str();}

    /**
     * Calls fflush().
     */
    virtual void flush() override;
    //@}
};

}  // namespace envir
}  // namespace omnetpp

#endif
