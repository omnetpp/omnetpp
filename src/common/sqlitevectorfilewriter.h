//==========================================================================
//  SQLITEVECTORFILEWRITER.H - part of
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

#ifndef __OMNETPP_COMMON_SQLITEVECTORFILEWRITER_H
#define __OMNETPP_COMMON_SQLITEVECTORFILEWRITER_H

#include <string>
#include <map>
#include <vector>
#include "sqlite3.h"
#include "commondefs.h"
#include "statistics.h"

namespace omnetpp {
namespace common {


/**
 * Class for writing SQLite-based output vector files.
 */
class COMMON_API SqliteVectorFileWriter
{
  public:
    typedef std::map<std::string, std::string> StringMap;
    typedef std::vector<std::pair<std::string, std::string>> OrderedKeyValueList;
    typedef int64_t eventnumber_t;
    typedef int64_t rawsimtime_t;

  protected:
    struct Sample {
        rawsimtime_t simtime;
        eventnumber_t eventNumber;
        double value;

        Sample(rawsimtime_t t, eventnumber_t eventNumber, double val) : simtime(t), eventNumber(eventNumber), value(val) {}
    };

    typedef std::vector<Sample> Samples;  //TODO these typedefs are no longer needed

    struct VectorData {
        sqlite_int64 id;           // vector ID
        Samples buffer;            // buffer holding recorded data not yet written to the database
        long bufferedSamples;      // number of samples in the buffer  //---FIXME isn't this buffer.size() ????
        long bufferedSamplesLimit; // maximum number of samples gathered in the buffer before writing out; 0=no limit

        // statistics
        eventnumber_t startEventNum = 0; // event number of the first sample in the block    //TODO these four are completely useless, could be removed
        eventnumber_t endEventNum = 0; // event number of the last sample in the block
        rawsimtime_t startTime = 0;   // simulation time of the first sample
        rawsimtime_t endTime = 0;     // simulation time of the last sample
        Statistics statistics;        // value statistics
    };

    typedef std::vector<VectorData*> Vectors;

    std::string fname;   // output file name
    sqlite_int64 runId;  // runId in sqlite database
    sqlite3 *db;         // sqlite database, nullptr before initialization and after error
    sqlite3_stmt *stmt;
    sqlite3_stmt *add_vector_stmt;
    sqlite3_stmt *add_vector_attr_stmt;
    sqlite3_stmt *add_vector_data_stmt;
    sqlite3_stmt *update_vector_stmt;

    int bufferedSamplesLimit;  // limit of total buffered samples; 0=no limit

    Vectors vectors;           // registered output vectors
    int bufferedSamples;       // currently total buffered samples

  protected:
    void prepareStatements();
    void cleanup();  // MUST NOT THROW
    virtual void writeRecords();
    virtual void writeOneBlock(VectorData *vp);
    virtual void writeBlock(VectorData *vp);
    virtual void finalizeVector(VectorData *vp);
    void executeSql(const char *sql);

    void prepareStatement(sqlite3_stmt *&stmt, const char *sql);
    void finalizeStatement(sqlite3_stmt *&stmt);
    void checkOK(int sqlite3_result);
    void checkDone(int sqlite3_result);
    void error(const char *errmsg);

  public:
    SqliteVectorFileWriter();
    virtual ~SqliteVectorFileWriter();

    void open(const char *filename); // append if file exists
    void close();
    bool isOpen() const {return db != nullptr;} // IMPORTANT: db will be closed when an error occurs

    void setOverallMemoryLimit(size_t limit) {bufferedSamplesLimit = limit / sizeof(Sample);}
    size_t getOverallMemoryLimit() const {return bufferedSamplesLimit * sizeof(Sample);}

    void beginRecordingForRun(const std::string& runName, int simtimeScaleExp, const StringMap& attributes, const StringMap& itervars, const OrderedKeyValueList& paramAssignments);
    void endRecordingForRun();
    void *registerVector(const std::string& componentFullPath, const std::string& name, const StringMap& attributes, size_t bufferSize);
    void deregisterVector(void *vechandle);
    void recordInVector(void *vectorhandle, eventnumber_t eventNumber, rawsimtime_t t, double value);
    void createVectorIndex();

    void flush();
};


} // namespace envir
}  // namespace omnetpp

#endif
