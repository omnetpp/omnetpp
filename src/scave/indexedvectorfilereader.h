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
#include <map>
#include <cstdarg>
#include "common/filereader.h"
#include "scavedefs.h"
#include "indexfile.h"
#include "node.h"
#include "nodetype.h"
#include "resultfilemanager.h"

namespace omnetpp {
namespace scave {

struct OutputVectorEntry {
    long serial;
    eventnumber_t eventNumber;
    simultime_t simtime;
    double value;

    OutputVectorEntry()
        : eventNumber(-1) {}
    OutputVectorEntry(long serial, eventnumber_t eventNumber, simultime_t simtime, double value)
        : serial(serial), eventNumber(eventNumber), simtime(simtime), value(value) {}
};

typedef std::vector<OutputVectorEntry> Entries;

/**
 * Vector file reader with random access.
 * Each instance reads one vector from a vector file.
 */
class SCAVE_API IndexedVectorFileReader
{
    using VectorInfo = VectorFileIndex::VectorInfo;
    using Block = VectorFileIndex::Block;

    std::string fname;  // file name of the vector file

    VectorFileIndex *index; // index of the vector file, loaded fully into the memory
    const VectorInfo *vector;     // index data of the read vector, points into index
    const Block *currentBlock;    // last loaded block, points into index
    Entries currentEntries; // entries of the loaded block

    public:
        explicit IndexedVectorFileReader(const char* filename, int vectorId);
        ~IndexedVectorFileReader();
    protected:
        /** reads a block from the vector file */
        void loadBlock(const Block& block);
    public:
        /**
         * Returns the number of entries in the vector.
         */
        int getNumberOfEntries() const { return vector->getCount(); };
        /**
         * Returns the entry with the specified serial,
         * or nullptr if the serial is out of range.
         * The pointer will be valid until the next call to getEntryBySerial().
         */
        OutputVectorEntry *getEntryBySerial(long serial);

        /**
         * Returns the first entry whose simtime is >= than the given simtime (when after is true),
         * or the last entry whose simtime is <= than the given simtime (when after is false).
         * Returns nullptr when no entry after/before.
         */
        OutputVectorEntry *getEntryBySimtime(simultime_t simtime, bool after);

        /**
         * Returns the first entry whose simtime is >= than the given simtime (when after is true),
         * or the last entry whose simtime is <= than the given simtime (when after is false).
         * Returns nullptr when no entry after/before.
         */
        OutputVectorEntry *getEntryByEventnum(eventnumber_t eventNum, bool after);

        /**
         * Adds output vector entries in the [startTime,endTime] interval to
         * the specified vector. Returns the number of entries added.
         */
        long collectEntriesInSimtimeInterval(simultime_t startTime, simultime_t endTime, Entries& out);
        /**
         * Adds output vector entries in the [startTime,endTime] interval to
         * the specified vector. Returns the number of entries added.
         */
        long collectEntriesInEventnumInterval(eventnumber_t startEventNum, eventnumber_t endEventNum, Entries& out);
};

} // namespace scave
}  // namespace omnetpp

#endif


