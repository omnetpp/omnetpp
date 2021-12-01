//=========================================================================
//  IVECTORDATAREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_IVECTORDATAREADER_H
#define __OMNETPP_SCAVE_IVECTORDATAREADER_H

#include <cassert>
#include <cfloat>
#include <vector>
#include <set>
#include <map>
#include <functional>
#include <cstdarg>
#include "common/filereader.h"
#include "scavedefs.h"
#include "resultfilemanager.h"
#include "vectorfileindex.h"

namespace omnetpp {
namespace scave {


struct VectorDatum {
    long serial;
    eventnumber_t eventNumber = -1;
    simultime_t simtime;
    double value;

    VectorDatum() {}
    VectorDatum(long serial, eventnumber_t eventNumber, simultime_t simtime, double value)
        : serial(serial), eventNumber(eventNumber), simtime(simtime), value(value) {}
};

typedef std::vector<VectorDatum> Entries;

/**
 * Vector file reader with random access.
 * Each instance reads one vector from a vector file.
 */
class SCAVE_API IVectorDataReader
{
    public:

        class SCAVE_API Adapter {
            public:
                virtual void process(int vectorId, const std::vector<VectorDatum>& data) = 0;
                virtual ~Adapter() {};
        };

        using AdapterLambdaType = std::function<void(int /* vectorId */, const std::vector<VectorDatum>& /* data */)>;

        /**
         * Returns the number of entries in the vector.
         */
        virtual int getNumberOfEntries(int vectorId) = 0;

        /**
         * Returns the entry with the specified serial,
         * or nullptr if the serial is out of range.
         * The pointer will be valid until the next call to getEntryBySerial().
         */
        virtual VectorDatum *getEntryBySerial(int vectorId, int64_t serial) = 0;

        /**
         * Returns the first entry whose simtime is >= than the given simtime (when after is true),
         * or the last entry whose simtime is <= than the given simtime (when after is false).
         * Returns nullptr when no entry after/before.
         */
        virtual VectorDatum *getEntryBySimtime(int vectorId, simultime_t simtime, bool after) = 0;

        /**
         * Returns the first entry whose simtime is >= than the given simtime (when after is true),
         * or the last entry whose simtime is <= than the given simtime (when after is false).
         * Returns nullptrs when no entry after/before.
         */
        virtual VectorDatum *getEntryByEventnum(int vectorId, eventnumber_t eventNum, bool after) = 0;

        // These functions don't always (with the SQLite reader) fill the serial field of the VectorDatum entries given to the adapter!
        virtual void collectEntries(const std::set<int>& vectorIds) = 0;
        virtual void collectEntriesInSimtimeInterval(const std::set<int>& vectorIds, simultime_t startTime, simultime_t endTime) = 0;
        virtual void collectEntriesInEventnumInterval(const std::set<int>& vectorIds, eventnumber_t startEventNum, eventnumber_t endEventNum) = 0;

        virtual ~IVectorDataReader() {}
};


}  // namespace scave
}  // namespace omnetpp

#endif
