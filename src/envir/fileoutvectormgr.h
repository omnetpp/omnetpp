//==========================================================================
//  FILEOUTPUTVECTORMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILEOUTPUTVECTORMGR_H
#define __FILEOUTPUTVECTORMGR_H

#include <stdio.h>
#include "envirdefs.h"
#include "envirext.h"
#include "util.h"
#include "runattributes.h"
#include "intervals.h"

NAMESPACE_BEGIN


/**
 * A cOutputVectorManager that uses a line-oriented text file as output.
 * The file consists of vector declarations lines and data lines.
 * Each data line contains a vector id, time, value triplet.
 *
 * @ingroup Envir
 */
class ENVIR_API cFileOutputVectorManager : public cOutputVectorManager
{
  protected:

    struct sVectorData { //XXX use stringPool for vectorname etc?
       int id;              // vector ID
       opp_string modulename; // module of cOutVector object
       opp_string vectorname; // cOutVector object name
       opp_string_map attributes; // vector attributes
       bool initialized;    // true if the vector declaration has been written out
       bool enabled;        // write to the output file can be enabled/disabled
       bool recordEventNumbers; // record the current event number for each sample
       Intervals intervals;

       const char *getColumns() { return recordEventNumbers ? "ETV" : "TV"; }
    };

    sRunData run;      // holds data of the current run
    int nextid;        // holds next free ID for output vectors
    opp_string fname;  // output file name
    FILE *f;           // file ptr of output file
    int prec;          // number of significant digits when writing doubles (time, value)

  protected:
    void openFile();
    void closeFile();
    virtual void initVector(sVectorData *vp);
    virtual sVectorData *createVectorData();
    virtual void writeRunData();

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cFileOutputVectorManager();

    /**
     * Destructor. Closes the output file if it is still open.
     */
    virtual ~cFileOutputVectorManager();
    //@}

    /**
     * Utility function for parsing the configuration of an output vector.
     */
    static void getOutVectorConfig(const char *modname, const char *vecname,
                                   bool& outEnabled, bool& outRecordEventNumbers, Intervals &outIntervals);

    /** @name Redefined cOutputVectorManager member functions. */
    //@{

    /**
     * Deletes output vector file if exists (left over from previous runs).
     * The file is not yet opened, it is done inside registerVector() on demand.
     */
    virtual void startRun();

    /**
     * Closes the output file.
     */
    virtual void endRun();

    /**
     * Registers a vector and returns a handle.
     */
    virtual void *registerVector(const char *modulename, const char *vectorname);

    /**
     * Deregisters the output vector.
     */
    virtual void deregisterVector(void *vechandle);

    /**
     * Sets an attribute of an output vector.
     */
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value);

    /**
     * Writes the (time, value) pair into the output file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value);

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

