//==========================================================================
//   FILEMGRS.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//     cFileOutputVectorManager
//     cFileOutputScalarManager
//     cFileSnapshotManager
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILEMGRS_H
#define __FILEMGRS_H

#include <stdio.h>
#include "envirext.h"
#include "util.h"


/**
 * A cOutputVectorManager that uses a line-oriented text file as output.
 * The file consists of vector declarations lines and data lines.
 * Each data line contains a vector id, time, value triplet.
 *
 * @ingroup Envir
 */
class cFileOutputVectorManager : public cOutputVectorManager
{
    struct sVectorData {
       int tuple;           // 1 or 2
       long id;             // vector ID
       opp_string modulename; // module of cOutVector object
       opp_string vectorname; // cOutVector object name
       bool initialised;    // true if the "label" line is already written out
       bool enabled;        // write to the output file can be enabled/disabled
       simtime_t starttime; // write begins at starttime
       simtime_t stoptime;  // write stops at stoptime
    };

  protected:
    long nextid;       // holds next free ID for output vectors
    opp_string fname;  // output file name
    FILE *f;           // file ptr of output file

  protected:
    void openFile();
    void closeFile();
    void initVector(sVectorData *vp);
    virtual sVectorData *createVectorData();

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
    virtual void *registerVector(const char *modulename, const char *vectorname, int tuple);

    /**
     * Deregisters the output vector.
     */
    virtual void deregisterVector(void *vechandle);

    /**
     * Writes the (time, value) pair into the output file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value);

    /**
     * Writes the (time, value1, value2) triplet into the output file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value1, double value2);

    /**
     * Returns the file name.
     */
    const char *fileName() const;
    //@}
};


/**
 * A cOutputScalarManager that uses a line-oriented text file as output.
 *
 * @ingroup Envir
 */
class cFileOutputScalarManager : public cOutputScalarManager
{
  protected:
    opp_string fname;  // output file name
    FILE *f;           // file ptr of output file

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
    void recordScalar(cModule *module, const char *name, double value);

    /**
     * Records a string result into the scalar result file.
     */
    void recordScalar(cModule *module, const char *name, const char *text);

    /**
     * Records a statistics object into the scalar result file.
     */
    void recordScalar(cModule *module, const char *name, cStatistic *stats);

    /**
     * Returns the file name.
     */
    const char *fileName() const;
    //@}
};

/**
 * The default cSnapshotManager.
 *
 * @ingroup Envir
 */
class cFileSnapshotManager : public cSnapshotManager
{
  protected:
    opp_string fname;  // output file name

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cFileSnapshotManager();

    /**
     * Destructor.
     */
    virtual ~cFileSnapshotManager();
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun();

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun();
    //@}

    /** @name Snapshot management */
    //@{
    /**
     * Returns a stream where a snapshot can be written.
     */
    virtual ostream *getStreamForSnapshot();

    /**
     * Releases a stream after a snapshot was written.
     */
    virtual void releaseStreamForSnapshot(ostream *os);

    /**
     * Returns the file name.
     */
    const char *fileName() const;
    //@}
};

#endif

