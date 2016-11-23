//==========================================================================
//  SQLITEOUTPUTVECTORMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga, Zoltan Bojthe
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_SQLITEOUTPUTVECTORMGR_H
#define __OMNETPP_ENVIR_SQLITEOUTPUTVECTORMGR_H

#include <cstdio>
#include "omnetpp/envirext.h"
#include "omnetpp/simutil.h"
#include "envir/envirdefs.h"
#include "envir/runattributes.h"
#include "envir/intervals.h"

#include "common/sqlite3.h"

namespace omnetpp {
namespace envir {


/**
 * A cIOutputVectorManager that uses a line-oriented text file as output.
 * The file consists of vector declarations lines and data lines.
 * Each data line contains a vector id, time, value triplet.
 *
 * @ingroup Envir
 */
class cSqliteOutputVectorManager : public cIOutputVectorManager
{
  protected:

    struct VectorData { //XXX use stringPool for vectorname etc?
       sqlite_int64 id;              // vector ID
       opp_string moduleName; // module of cOutVector object
       opp_string vectorName; // cOutVector object name
       opp_string_map attributes; // vector attributes
       bool initialized;    // true if the vector declaration has been written out
       bool enabled;        // write to the output file can be enabled/disabled
       Intervals intervals;

       virtual ~VectorData() {}
    };

    RunData run;       // holds data of the current run
    std::string fname; // output file name
    sqlite_int64 runId;  // runId in sqlite database
    sqlite3 *db;       // sqlite database
    sqlite3_stmt *add_vector_stmt;
    sqlite3_stmt *add_vector_attr_stmt;
    sqlite3_stmt *add_vector_data_stmt;

    // we COMMIT after every commitFreq INSERT statements
    int commitFreq;
    int insertCount;

  protected:
    void openDb();
    void closeDb();
    void commitDb();
    virtual void initVector(VectorData *vp);
    virtual VectorData *createVectorData();
    virtual void writeRunData();

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cSqliteOutputVectorManager();

    /**
     * Destructor. Closes the output file if it is still open.
     */
    virtual ~cSqliteOutputVectorManager();
    //@}

    /**
     * Utility function for parsing the configuration of an output vector.
     */
    static void getOutVectorConfig(const char *modname, const char *vecname, bool& outEnabled, Intervals &outIntervals);

    /** @name Redefined cIOutputVectorManager member functions. */
    //@{

    /**
     * Deletes output vector file if exists (left over from previous runs).
     * The file is not yet opened, it is done inside registerVector() on demand.
     */
    virtual void startRun() override;

    /**
     * Closes the output file.
     */
    virtual void endRun() override;

    /**
     * Registers a vector and returns a handle.
     */
    virtual void *registerVector(const char *modulename, const char *vectorname) override;

    /**
     * Deregisters the output vector.
     */
    virtual void deregisterVector(void *vechandle) override;

    /**
     * Sets an attribute of an output vector.
     */
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value) override;

    /**
     * Writes the (time, value) pair into the output file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value) override;

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
