//==========================================================================
//  FILEOUTPUTSCALARMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_FILEOUTPUTSCALARMGR_H
#define __OMNETPP_ENVIR_FILEOUTPUTSCALARMGR_H

#include <cstdio>
#include <cstdint>
#include "omnetpp/envirext.h"
#include "omnetpp/simutil.h"
#include "envirdefs.h"
#include "resultfileutils.h"

namespace omnetpp {
namespace envir {

/**
 * A cIOutputScalarManager that uses a line-oriented text file as output.
 *
 * @ingroup Envir
 */
class ENVIR_API cFileOutputScalarManager : public cIOutputScalarManager
{
  protected:
    bool initialized;  // true after first call to initialize(), even if it failed
    std::string fname; // output file name
    FILE *f;           // file ptr of output file; nullptr if closed (not yet opened, or after error)
    int prec;          // number of significant digits when writing doubles

  protected:
    void openFile();
    void closeFile();
    void initialize();
    void check(int fprintfResult);
    void writeStatisticField(const char *name, int64_t value);
    void writeStatisticField(const char *name, double value);
    void recordNumericIterationVariableAsScalar(const char *name, const char *value); // i.e. write *if* numeric

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cFileOutputScalarManager();

    /**
     * Destructor.
     */
    virtual ~cFileOutputScalarManager();
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

