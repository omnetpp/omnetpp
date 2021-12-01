//=========================================================================
//  EVENTLOGINDEX.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_EVENTLOGINDEX_H
#define __OMNETPP_EVENTLOG_EVENTLOGINDEX_H

#include <sstream>
#include <list>
#include <vector>
#include <map>
#include "common/exception.h"
#include "common/filereader.h"
#include "common/linetokenizer.h"
#include "eventlogdefs.h"
#include "enums.h"

namespace omnetpp {
namespace eventlog {

/**
 * Allows random access of an eventlog file, i.e. positioning on arbitrary event numbers and simulation times.
 * TODO: throw out entries from cache to free memory. This is not that urgent because the cache will be quite
 * small unless the file is linearly read through which is not supposed to happen.
 */
class EVENTLOG_API EventLogIndex
{
    protected:
        FileReader *reader;
        omnetpp::common::LineTokenizer *tokenizer;

        file_offset_t firstEventOffset;
        file_offset_t lastEventOffset;
        eventnumber_t firstEventNumber;
        eventnumber_t lastEventNumber;
        simtime_t firstSimulationTime;
        simtime_t lastSimulationTime;

        /**
         * An entry stores information for a simulation time and the corresponding event number and file offset ranges.
         * It actually describes a range of lines in the eventlog file.
         * The range might be temporarily smaller than what is actually present in the file for the given
         * simulation time or event number. The range gets closer and finally reaches the exact values by subsequent search operations.
         */
        class CacheEntry
        {
            public:
                simtime_t simulationTime = simtime_t(-1.0);
                eventnumber_t beginEventNumber = -1; // begin event with simulation time
                eventnumber_t endEventNumber = -1; // end event with simulation time
                file_offset_t beginOffset = -1; // begin offset of begin event
                file_offset_t endEventBeginOffset = -1; // begin offset of end event
                file_offset_t endOffset = -1; // end offset of end event

            public:
                CacheEntry() {}
                CacheEntry(eventnumber_t eventNumber, simtime_t simulationTime, file_offset_t beginOffset, file_offset_t endOffset);
                void include(eventnumber_t eventNumber, simtime_t simulationTime, file_offset_t beginOffset, file_offset_t endOffset);
                void getBeginKey(eventnumber_t &key);
                void getBeginKey(simtime_t &key);
                void getEndKey(eventnumber_t &key);
                void getEndKey(simtime_t &key);
        };

        /**
         * Subsequent events in an eventlog file may not have subsequent event numbers,
         * therefore it is insufficient to store the file offset only in the cache.
         */
        typedef std::map<eventnumber_t, CacheEntry> EventNumberToCacheEntryMap;
        EventNumberToCacheEntryMap eventNumberToCacheEntryMap;

        /**
         * Subsequent events in an eventlog file may have the same simulation time,
         * therefore it is insufficient to store the file offset only in the cache.
         */
        typedef std::map<simtime_t, CacheEntry> SimulationTimeToCacheEntryMap;
        SimulationTimeToCacheEntryMap simulationTimeToCacheEntryMap;

    protected:
        void cacheEntry(eventnumber_t eventNumber, simtime_t simulationTime, file_offset_t beginOffset, file_offset_t endOffset);

        /**
         * Search for the file offset based on the key with the given match kind.
         * The key is either an event number or a simulation time.
         */
        template <typename T> file_offset_t searchForOffset(std::map<T, CacheEntry> &map, T key, MatchKind matchKind);
        /**
         * Search the internal cache finding the file offset(s and keys) for the given key with the given match kind.
         * Sets lower and upper keys and offsets to the closest appropriate values found in the cache.
         * Lower is less than or equal to key, while upper is greater than or equal that could theoretically be found in the eventlog file.
         * Sets found offset or returns false if the offset cannot be exactly determined.
         */
        template <typename T> bool cacheSearchForOffset(std::map<T, CacheEntry> &map, T key, MatchKind matchKind, T& lowerKey, T& upperKey, file_offset_t& foundOffset, file_offset_t& lowerOffset, file_offset_t& upperOffset);
        /**
         * Binary search through the eventlog file finding the file offset for the given key with the given match kind.
         * Sets the closest lower and upper keys and offsets around the key found in the eventlog file.
         * Returns -1 if the offset cannot be determined.
         */
        template <typename T> file_offset_t  binarySearchForOffset(T key, MatchKind matchKind, T& lowerKey, T& upperKey, file_offset_t& lowerOffset, file_offset_t& upperOffset);
        /**
         * Linear search through the eventlog file finding the file offset for the given key.
         * The search starts from begin offset and continues in the direction specified by the forward flag.
         * Returns -1 if no such event found otherwise the last exact match or the first non exact match depending
         * on the exact match required flag.
         */
        template <typename T> file_offset_t linearSearchForOffset(T key, file_offset_t beginOffset, bool forward, bool exactMatchRequired);

        void clearInternalState();

        bool isEventBeginOffset(file_offset_t offset);

    public:
        /**
         * The reader will be deleted with this object.
         */
        EventLogIndex(FileReader *reader);
        virtual ~EventLogIndex();

        virtual void synchronize(FileReader::FileChange change);
        eventnumber_t getFirstEventNumber();
        eventnumber_t getLastEventNumber();
        simtime_t getFirstSimulationTime();
        simtime_t getLastSimulationTime();
        file_offset_t getFirstEventOffset();
        file_offset_t getLastEventOffset();
        void ensureFirstEventAndLastEventCached();
        /**
         * Returns the begin file offset of an event reading linearly backward from its end file offset.
         */
        file_offset_t getBeginOffsetForEndOffset(file_offset_t endOffset);
        /**
         * Returns the end file offset of an event reading linearly forward from its begin file offset.
         */
        file_offset_t getEndOffsetForBeginOffset(file_offset_t beginOffset);
        /**
         * Returns the begin file offset of the requested event. See MatchKind for details.
         */
        file_offset_t getOffsetForEventNumber(eventnumber_t eventNumber, MatchKind matchKind = EXACT);
        /**
         * Returns the begin file offset of the requested simulation time. See MatchKind for details.
         */
        file_offset_t getOffsetForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT);
        /**
         * Returns true if OK, false if no "E" line found till the end of file in the given direction.
         * Reads the first event line in the given direction starting from the given offset.
         */
        bool readToEventLine(bool forward, file_offset_t readBeginOffset, eventnumber_t& eventNumber, simtime_t& simulationTime, file_offset_t& lineBeginOffset, file_offset_t& lineEndOffset);
        void dump();
};

}  // namespace eventlog
}  // namespace omnetpp

#endif
