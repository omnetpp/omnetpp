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

NAMESPACE_BEGIN

/**
 * A cOutputScalarManager that uses a line-oriented text file as output.
 *
 * @ingroup Envir
 */
class ENVIR_API cFileOutputScalarManager : public cOutputScalarManager
{
  protected:
    opp_string fname;  // output file name
    FILE *f;           // file ptr of output file
    int prec;          // number of significant digits when writing doubles

  protected:
    bool initialized;

    void openFile();
    void closeFile();
    void init();

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

NAMESPACE_END

#endif

