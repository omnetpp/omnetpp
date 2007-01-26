//==========================================================================
//  IVFILEMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//     cIndexedFileOutputVectorManager
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __IVFILEMGR_H
#define __IVFILEMGR_H

#include <stdio.h>
#include <float.h>
#include <deque>
#include "envirext.h"
#include "util.h"
#include "filemgrs.h"

#define MIN_BUFFER_MEMORY 1024*1024
#define DEFAULT_MEMORY_LIMIT "16MB"

/**
 * cFileOutputManager that writes vector data in chunks and
 * generate an index file for the output.
 *
 * @ingroup Envir
 */
class cIndexedFileOutputVectorManager : public cFileOutputVectorManager
{
  protected:
    struct sBlock {
      long offset;
      long count;

      sBlock(long offset, long count) : offset(offset), count(count) {}
    };

    typedef std::vector<sBlock> Blocks;

    struct sSample {
        simtime_t simtime;
        double value;

        sSample(simtime_t t, double val) : simtime(t), value(val) {}
    };

    typedef std::vector<sSample> Samples;

    struct sVector : sVectorData {
      std::vector<sSample> buffer; // buffer holding recorded data not yet written to the file
      long maxBufferedSamples; // maximum number of samples gathered in the buffer
      long count;              // number of samples written into the vector
      double min;              // minimum of samples written into the vector
      double max;              // maximum of samples written into the vector
      double sum;              // sum of samples written into the vector
      double sumsqr;           // sum of squares of samples written into the vector
      long maxBlockSize;       // maximum size of the blocks in bytes
      std::vector<sBlock> blocks; // attributes of the chunks written into the file

      sVector() : buffer(), maxBufferedSamples(0), count(0), min(DBL_MAX), max(DBL_MIN), sum(0.0), sumsqr(0.0), maxBlockSize(0), blocks() {}
      void allocateBuffer(long count) { buffer.reserve(count); }
    };

    typedef std::vector<sVector*> Vectors;

    opp_string ifname;  // index file name
    FILE *fi;           // file ptr of index file
    size_t maxMemoryUsed;
    size_t memoryUsed;
    Vectors vectors; // registered output vectors

  protected:
    virtual sVectorData *createVectorData();
    void openIndexFile();
    void closeIndexFile();
    virtual void finalizeVector(sVector *vector);
    virtual void writeRecords();
    virtual void writeRecords(sVector *vector);
    virtual void writeIndex(sVector *vector);
  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cIndexedFileOutputVectorManager();
    //@}

    /** @name Redefined cOutputVectorManager member functions. */
    //@{

    /**
     * Deletes output vector  and index files if exists (left over from previous runs).
     * The file is not yet opened, it is done inside registerVector() on demand.
     */
    virtual void startRun();

    /**
     * Writes out last chunk of registered output vectors and generates index entries for them.
     * Closes the output and index files.
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
     * Writes the (time, value) pair into the buffer.
     * When the buffer is full, writes it out into the file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value);

    /**
     * Writes the (time, value1, value2) triplet into the buffer.
     * When the buffer is full, writes it out into the file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value1, double value2);

    //@}
};

#endif

