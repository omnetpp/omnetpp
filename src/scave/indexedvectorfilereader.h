//=========================================================================
//  INDEXEDVECTORFILEREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_INDEXEDVECTORFILEREADER_H
#define __OMNETPP_SCAVE_INDEXEDVECTORFILEREADER_H

#include <cassert>
#include <cfloat>
#include <vector>
#include <set>
#include <map>
#include <functional>
#include <cstdarg>
#include "common/filereader.h"
#include "scavedefs.h"
#include "node.h"
#include "nodetype.h"
#include "resultfilemanager.h"
#include "vectorfileindex.h"

namespace omnetpp {
namespace scave {

struct VectorDatum {
    long serial;
    eventnumber_t eventNumber;
    simultime_t simtime;
    double value;

    VectorDatum()
        : eventNumber(-1) {}
    VectorDatum(long serial, eventnumber_t eventNumber, simultime_t simtime, double value)
        : serial(serial), eventNumber(eventNumber), simtime(simtime), value(value) {}
};

typedef std::vector<VectorDatum> Entries;

/**
 * Vector file reader with random access.
 * Each instance reads one vector from a vector file.
 */
class SCAVE_API IndexedVectorFileReader
{
    using VectorInfo = VectorFileIndex::VectorInfo;
    using Block = VectorFileIndex::Block;

    public:
        class SCAVE_API Adapter {
            public:
                virtual void process(int vectorId, const std::vector<VectorDatum>& data) = 0;
                virtual ~Adapter() {};
        };

        using AdapterLambdaType = std::function<void(int /* vectorId */, const std::vector<VectorDatum>& /* data */)>;

    private:

        AdapterLambdaType adapterLambda;

        std::string fname;  // file name of the vector file
        VectorFileIndex *index; // index of the vector file, loaded fully into the memory

    protected:
        /** reads a block from the vector file */
        Entries loadBlock(const Block& block, std::function<bool(const VectorDatum&)> filter = nullptr);

    public:
        explicit IndexedVectorFileReader(const char* filename, Adapter *adapter) :
            IndexedVectorFileReader(filename, [adapter](int vectorId, const std::vector<VectorDatum>& data) { adapter->process(vectorId, data); })
        { }

        explicit IndexedVectorFileReader(const char* filename, AdapterLambdaType adapter);
        ~IndexedVectorFileReader();

        /**
         * Returns the number of entries in the vector.
         */
        int getNumberOfEntries(int vectorId) const { return index->getVectorById(vectorId)->getCount(); };
        /**
         * Returns the entry with the specified serial,
         * or nullptr if the serial is out of range.
         * The pointer will be valid until the next call to getEntryBySerial().
         */
        VectorDatum *getEntryBySerial(int vectorId, long serial);

        /**
         * Returns the first entry whose simtime is >= than the given simtime (when after is true),
         * or the last entry whose simtime is <= than the given simtime (when after is false).
         * Returns nullptr when no entry after/before.
         */
        VectorDatum *getEntryBySimtime(int vectorId, simultime_t simtime, bool after);

        /**
         * Returns the first entry whose simtime is >= than the given simtime (when after is true),
         * or the last entry whose simtime is <= than the given simtime (when after is false).
         * Returns nullptrs when no entry after/before.
         */
        VectorDatum *getEntryByEventnum(int vectorId, eventnumber_t eventNum, bool after);

        void collectEntries(const std::set<int>& vectorIds);
        void collectEntriesInSimtimeInterval(const std::set<int>& vectorIds, simultime_t startTime, simultime_t endTime);
        void collectEntriesInEventnumInterval(const std::set<int>& vectorIds, eventnumber_t startEventNum, eventnumber_t endEventNum);
};


} // namespace scave
}  // namespace omnetpp

#endif


