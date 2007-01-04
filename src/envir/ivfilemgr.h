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
#include "envirext.h"
#include "util.h"
#include "filemgrs.h"

#define MIN_BUFFER_SIZE 512
#define DEFAULT_BUFFER_SIZE 65536

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
      long num_samples;
      sBlock(long offset, long num_samples) : offset(offset), num_samples(num_samples) {}
    };

    struct sVectorDataExt : sVectorData {
      char* buffer;            // buffer holding recorded data
      int buffer_size;         // size of the allocated buffer
      char* buffer_ptr;        // pointer to the current position in the buffer
      int buffer_num_samples;  // number of samples written into the buffer
      int num_samples;         // number of samples written into the vector
      double min;              // minimum of samples written into the vector
      double max;              // maximum of samples written into the vector
      double sum;              // sum of samples written into the vector
      double sumsqr;           // sum of squares of samples written into the vector
      std::vector<sBlock> blocks; // attributes of the chunks written into the file

      sVectorDataExt() { buffer=buffer_ptr=NULL; buffer_size=0; num_samples=0; min=DBL_MAX; max=DBL_MIN; sum=sumsqr=0.0; }
      ~sVectorDataExt() { if (buffer) delete[] buffer; }
      void allocate_buffer(int size) { buffer=new char[size]; buffer_size=size; clear_buffer(); }
      void clear_buffer() { buffer_ptr = buffer; buffer_num_samples=0; if (buffer) buffer[0] = '\0';}
      bool dirty() { return buffer_ptr!=buffer; }
    };
    opp_string ifname;  // index file name
    FILE *fi;           // file ptr of index file
    std::vector<sVectorDataExt*> vectors; // registered output vectors

  protected:
    virtual sVectorData *createVectorData();
    void openIndexFile();
    void closeIndexFile();
    virtual void finalizeVector(sVectorDataExt *vector);
    virtual void writeRecords(sVectorDataExt *vector);
    virtual void writeIndex(sVectorDataExt *vector);
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
    virtual void *registerVector(const char *modulename, const char *vectorname, int tuple);

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

