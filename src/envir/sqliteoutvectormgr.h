//==========================================================================
//  SQLITEOUTPUTVECTORMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga, Zoltan Bojthe
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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
class SqliteOutputVectorManager : public cIOutputVectorManager
{
  protected:
    struct Sample {
        simtime_t simtime;
        eventnumber_t eventNumber;
        double value;

        Sample(simtime_t t, eventnumber_t eventNumber, double val) : simtime(t), eventNumber(eventNumber), value(val) {}
    };

    typedef std::vector<Sample> Samples;
    struct VectorData {
        sqlite_int64 id;           // vector ID
        opp_string moduleName;     // module of cOutVector object
        opp_string vectorName;     // cOutVector object name
        opp_string_map attributes; // vector attributes
        bool initialized;          // true if the vector declaration has been written out
        bool finalized;            // true last data have been flushed; TODO join w/ previous into state variable
        bool enabled;              // write to the output file can be enabled/disabled
        Intervals intervals;       // recording intervals
        Samples buffer;            // buffer holding recorded data not yet written to the database
        long bufferedSamples;      // number of samples in the buffer
        long maxBufferedSamples;   // maximum number of samples gathered in the buffer before writing out

        // statistics
        eventnumber_t startEventNum = 0; // event number of the first sample in the block
        eventnumber_t endEventNum = 0; // event number of the last sample in the block
        simtime_t startTime = 0;   // simulation time of the first sample
        simtime_t endTime = 0;     // simulation time of the last sample
        long count = 0;            // count of samples
        double min = 0;            // minimum value of the samples
        double max = 0;            // maximum value of the samples
        double sum = 0;            // sum of values of the samples
        double sumSqr = 0;         // sum of squares of values

        void allocateBuffer(long count) { buffer.reserve(count); }
        virtual ~VectorData() {}
    };

    typedef std::vector<VectorData*> Vectors;

    enum IndexingMode { INDEX_AHEAD, INDEX_AFTER, INDEX_NONE } indexingMode;

    bool initialized;    // true after first call to initialize(), even if it failed
    RunData run;         // holds data of the current run
    std::string fname;   // output file name
    sqlite_int64 runId;  // runId in sqlite database
    sqlite3 *db;         // sqlite database, nullptr before initialization and after error
    sqlite3_stmt *stmt;
    sqlite3_stmt *add_vector_stmt;
    sqlite3_stmt *add_vector_attr_stmt;
    sqlite3_stmt *add_vector_data_stmt;
    sqlite3_stmt *update_vector_stmt;

    int maxBufferedSamples;  // limit of total buffered samples
    int bufferedSamples;     // currently total buffered samples
    Vectors vectors;         // registered output vectors

  protected:
    virtual void initialize();
    virtual void openDb();
    virtual void closeDb();
    virtual void cleanupDb(); // MUST NOT THROW
    virtual void initVector(VectorData *vp);
    virtual VectorData *createVectorData();
    virtual void writeRunData();
    virtual void writeRecords();
    virtual void writeOneBlock(VectorData *vp);
    virtual void writeBlock(VectorData *vp);
    virtual void finalizeVector(VectorData *vp);
    virtual void createIndex();
    void executeSql(const char *sql);
    void prepareStatement(sqlite3_stmt *&stmt, const char *sql);
    void finalizeStatement(sqlite3_stmt *&stmt);
    bool isBad() {return initialized && db == nullptr;}
    void checkOK(int sqlite3_result);
    void checkDone(int sqlite3_result);
    void error(const char *errmsg);

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit SqliteOutputVectorManager();

    /**
     * Destructor. Closes the output file if it is still open.
     */
    virtual ~SqliteOutputVectorManager();
    //@}

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
