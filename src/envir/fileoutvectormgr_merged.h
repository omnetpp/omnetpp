//==========================================================================
//  FILEOUTVECTORMGR_MERGED.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_FILEOUTVECTORMGR_MERGED_H
#define __OMNETPP_ENVIR_FILEOUTVECTORMGR_MERGED_H

#include <cstdio>
#include <cfloat>
#include "omnetpp/envirext.h"
#include "omnetpp/simutil.h"
#include "envirdefs.h"
#include "intervals.h"
#include "resultfileutils.h"

namespace omnetpp {
namespace envir {

/**
 * cIOutputVectorManager that writes vector data in chunks and
 * generate an index file for the output.
 *
 * This version has been produced from cIndexedFileOutputVectorManager
 * by merging its base class cFileOutputVectorManager into it.
 *
 * @ingroup Envir
 */
class ENVIR_API cFileOutputVectorManager_merged : public cIOutputVectorManager
{
  protected:
    struct Block {
      file_offset_t offset; // file offset of the block
      file_offset_t size;   // size of the block
      eventnumber_t startEventNum; // event number of the first sample in the block
      eventnumber_t endEventNum;   // event number of the last sample in the block
      simtime_t startTime;  // simulation time of the first sample in the block
      simtime_t endTime;    // simulation time of the last sample in the block
      long count;           // count of samples in the block
      double min;           // minimum value of the samples
      double max;           // maximum value of the samples
      double sum;           // sum of values of the samples
      double sumSqr;        // sum of squares of values

      Block() { reset(); }
      void reset() { offset=-1; size=0; count=0; min=DBL_MAX; max=-DBL_MAX; sum=0.0; sumSqr=0.0; }
    };

    struct Sample {
        simtime_t simtime;
        eventnumber_t eventNumber;
        double value;

        Sample(simtime_t t, eventnumber_t eventNumber, double val) : simtime(t), eventNumber(eventNumber), value(val) {}
    };

    typedef std::vector<Sample> Samples;

    struct Vector { //XXX use stringPool for vectorname etc?
       int id;              // vector ID
       opp_string moduleName; // module of cOutVector object
       opp_string vectorName; // cOutVector object name
       opp_string_map attributes; // vector attributes
       bool initialized;    // true if the vector declaration has been written out
       bool enabled;        // write to the output file can be enabled/disabled
       bool recordEventNumbers; // record the current event number for each sample
       Intervals intervals;

       std::vector<Sample> buffer; // buffer holding recorded data not yet written to the file
       long maxBufferedSamples=0;    // maximum number of samples gathered in the buffer
       Block currentBlock;

       void allocateBuffer(long count) { buffer.reserve(count); }
       const char *getColumns() { return recordEventNumbers ? "ETV" : "TV"; }
    };

    typedef std::vector<Vector*> Vectors;

    bool initialized;  // true after first call to initialize(), even if it failed
    int nextid;        // holds next free ID for output vectors
    std::string fname; // output file name
    FILE *f;           // file ptr of output file
    int prec;          // number of significant digits when writing doubles (time, value)

    opp_string ifname;  // index file name
    FILE *fi;           // file ptr of index file
    size_t totalMemoryLimit;
    size_t memoryUsed;
    Vectors vectors; // registered output vectors

  protected:
    virtual Vector *createVectorData();
    void openFile();
    void closeFile();
    void check(int fprintfResult);
    void checki(int fprintfResult);
    void openIndexFile();
    void closeIndexFile();
    void getOutVectorConfig(const char *modname, const char *vecname, bool& outEnabled, bool& outRecordEventNumbers, Intervals& outIntervals);
    virtual void initVector(Vector *vp);
    virtual void finalizeVector(Vector *vector);
    virtual void writeRunData();
    virtual void writeRecords();
    virtual void writeBlock(Vector *vector);
    virtual void writeBlockToIndexFile(Vector *vector);
  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cFileOutputVectorManager_merged();
    //@}

    /** @name Redefined cIOutputVectorManager member functions. */
    //@{

    /**
     * Deletes output vector  and index files if exists (left over from previous runs).
     * The file is not yet opened, it is done inside registerVector() on demand.
     */
    virtual void startRun() override;

    /**
     * Writes out last chunk of registered output vectors and generates index entries for them.
     * Closes the output and index files.
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
     * Writes the (time, value) pair into the buffer.
     * When the buffer is full, writes it out into the file.
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

