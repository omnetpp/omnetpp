//==========================================================================
//  FILEMGRS.H - part of
//                     OMNeT++/OMNEST
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
  Copyright (C) 1992-2005 Andras Varga

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
class ENVIR_API cFileOutputVectorManager : public cOutputVectorManager
{
  protected:
    struct sRunData {
       bool initialized;        // true if the other fields are valid
       opp_string runId;        // id of the run
       opp_string_map attributes;    // attributes of the run
       opp_string_map moduleParams;  // module parameters in the current run

       sRunData() : initialized(false) {}
    };

    struct sVectorData {
       int id;              // vector ID
       opp_string modulename; // module of cOutVector object
       opp_string vectorname; // cOutVector object name
       opp_string_map attributes; // vector attributes
       bool initialized;    // true if the vector declaration has been written out
       bool enabled;        // write to the output file can be enabled/disabled
       bool recordEventNumbers; // record the current event number for each sample
       Interval *intervals; // array of (starttime, stoptime) pairs terminated with (0,0), or NULL

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
    void initRun();
    virtual void initVector(sVectorData *vp);
    virtual sVectorData *createVectorData();
    virtual void writeRunData();
    static bool containsTime(simtime_t t, Interval *intervals);

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
                                   bool& outEnabled, bool& outRecordEventNumbers, Interval *&outIntervals);

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

/**
 * The default cSnapshotManager.
 *
 * @ingroup Envir
 */
class ENVIR_API cFileSnapshotManager : public cSnapshotManager
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
    virtual std::ostream *getStreamForSnapshot();

    /**
     * Releases a stream after a snapshot was written.
     */
    virtual void releaseStreamForSnapshot(std::ostream *os);

    /**
     * Returns the file name.
     */
    const char *fileName() const;
    //@}
};

#endif

