//==========================================================================
//  OMNETPPVECTORFILEWRITER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Authors: Andras Varga, Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_OMNETPPVECTORFILEWRITER_H
#define __OMNETPP_COMMON_OMNETPPVECTORFILEWRITER_H

#include <string>
#include <map>
#include <vector>
#include "commondefs.h"
#include "statistics.h"
#include "omnetpp/platdep/platmisc.h"  // file_offset_t

namespace omnetpp {
namespace common {


/**
 * Class for writing text-based output vector files.
 */
class COMMON_API OmnetppVectorFileWriter
{
  public:
    typedef std::map<std::string, std::string> StringMap;
    typedef std::vector<std::pair<std::string, std::string>> OrderedKeyValueList;
    typedef int64_t eventnumber_t;
    typedef int64_t rawsimtime_t;

  protected:
    struct SimtimeValue {
        int64_t t;
        int scaleExp;
        const char *ttoa(char *buf) const {char *endp; return opp_ttoa(buf, t, scaleExp, endp);}
    };

    struct Sample {
        SimtimeValue time;
        eventnumber_t eventNumber;
        double value;

        Sample(rawsimtime_t t, int scaleExp, eventnumber_t eventNumber, double value) :
            time(SimtimeValue{t,scaleExp}), eventNumber(eventNumber), value(value) {}
    };

    typedef std::vector<Sample> Samples;

    struct Block {
      file_offset_t offset; // file offset of the block
      file_offset_t size;   // size of the block

      eventnumber_t startEventNum; // event number of the first sample in the block
      eventnumber_t endEventNum;   // event number of the last sample in the block
      SimtimeValue startTime;      // simulation time of the first sample in the block
      SimtimeValue endTime;        // simulation time of the last sample in the block
      Statistics statistics;       // statistics of the samples in the block

      Block() { reset(); }
      void reset() { offset=-1; size=0; statistics.clear(); }
    };

    struct VectorData {
       int id;                    // vector ID
       Samples buffer;            // buffer holding recorded data not yet written to the file
       long bufferedSamplesLimit; // maximum number of samples gathered in the buffer before writing out (0=no limit)
       bool recordEventNumbers;   // record the current event number for each sample
       Block currentBlock;
    };

    typedef std::vector<VectorData*> Vectors;

    std::string fname;     // output file name
    FILE *f = nullptr;     // file ptr of output file
    int prec = 14;         // number of significant digits when writing doubles
    int nextVectorId = 0;  // holds next free ID for output vectors

    std::string ifname;  // index file name
    FILE *fi = nullptr;  // file ptr of index file

    Vectors vectors;               // registered output vectors
    int bufferedSamples = 0;       // currently total buffered samples
    int bufferedSamplesLimit = 0;  // limit of total buffered samples (0=no limit)

  protected:
    void cleanup();  // MUST NOT THROW
    void check(int fprintfResult);
    void checki(int fprintfResult);
    virtual void writeRecords();
    virtual void writeBlock(VectorData *vp);
    virtual void finalizeVector(VectorData *vp);

  public:
    OmnetppVectorFileWriter() {}
    virtual ~OmnetppVectorFileWriter();

    void open(const char *filename); // overwrite if file exists (append not supported)
    void close();
    bool isOpen() const {return f != nullptr;} // IMPORTANT: file will be closed when an error occurs

    void setPrecision(int p) {prec = p;}
    int getPrecision() const {return prec;}
    void setOverallMemoryLimit(size_t limit) {bufferedSamplesLimit = limit / sizeof(Sample);}
    size_t getOverallMemoryLimit() const {return bufferedSamplesLimit * sizeof(Sample);}

    void beginRecordingForRun(const std::string& runName, const StringMap& attributes, const StringMap& itervars, const OrderedKeyValueList& paramAssignments);
    void endRecordingForRun();
    void *registerVector(const std::string& componentFullPath, const std::string& name, const StringMap& attributes, size_t bufferSize, bool recordEventNumbers);
    void deregisterVector(void *vechandle);
    void recordInVector(void *vectorhandle, eventnumber_t eventNumber, rawsimtime_t t, int simtimeScaleExp, double value);

    void flush();
};


}  // namespace envir
}  // namespace omnetpp

#endif
