//==========================================================================
//  FILEOUTPUTSCALARMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILEOUTPUTSCALARMGR_H
#define __FILEOUTPUTSCALARMGR_H

#include <stdio.h>
#include "envirdefs.h"
#include "envirext.h"
#include "util.h"
#include "stringutil.h"
#include "runattributes.h"

NAMESPACE_BEGIN

/**
 * A cOutputScalarManager that uses a line-oriented text file as output.
 *
 * @ingroup Envir
 */
class ENVIR_API cFileOutputScalarManager : public cOutputScalarManager
{
  protected:
    sRunData run;      // holds data of the current run
    opp_string fname;  // output file name
    FILE *f;           // file ptr of output file
    int prec;          // number of significant digits when writing doubles

  protected:
    void openFile();
    void closeFile();
    void init();
    void writeStatisticField(const char *name, long value);
    void writeStatisticField(const char *name, double value);

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
    virtual void startRun();

    /**
     * Closes collecting. Called at the end of a simulation run.
     */
    virtual void endRun();

    /** @name Scalar statistics */
    //@{

    /**
     * Records a double scalar result into the scalar result file.
     */
    void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL);

    /**
     * Records a histogram or statistic object into the scalar result file.
     */
    void recordScalar(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL);

    /**
     * Returns the file name.
     */
    const char *fileName() const;

    /**
     * Calls fflush().
     */
    virtual void flush();
    //@}
};

inline void cFileOutputScalarManager::writeStatisticField(const char *name, long value)
{
	if (fprintf(f, "field %s %ld\n", QUOTE(name), value) < 0)
		throw cRuntimeError("Cannot write output scalar file `%s'", fname.c_str());
}

inline void cFileOutputScalarManager::writeStatisticField(const char *name, double value)
{
	if (fprintf(f, "field %s %.*g\n", QUOTE(name), prec, value) < 0)
		throw cRuntimeError("Cannot write output scalar file `%s'", fname.c_str());
}

NAMESPACE_END

#endif

