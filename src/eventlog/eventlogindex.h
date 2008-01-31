//=========================================================================
//  EVENTLOGINDEX.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGINDEX_H_
#define __EVENTLOGINDEX_H_

#include <sstream>
#include <list>
#include <vector>
#include <map>
#include "eventlogdefs.h"
#include "exception.h"
#include "eventlogdefs.h"
#include "filereader.h"
#include "linetokenizer.h"

NAMESPACE_BEGIN

/**
 * Allows random access of an event log file, i.e. positioning on arbitrary event numbers and simulation times.
 * TODO: throw out entries from cache to free memory. This is not that urgent because the cache will be quite
 * small unless the file is linearly read through which is not supposed to happen.
 */
class EVENTLOG_API EventLogIndex
{
    protected:
        FileReader *reader;
        LineTokenizer tokenizer;

        file_offset_t firstEventOffset;
        file_offset_t lastEventOffset;
        long firstEventNumber;
        long lastEventNumber;
        simtime_t firstSimulationTime;
        simtime_t lastSimulationTime;

        /**
         * An entry stores information for a simulation time and the corresponding event number and file offset ranges.
         * It actually describes a range of lines in the event log file.
         * The range might be temporarily smaller than what is actually present in the file for the given
         * simulation time or event number. The range gets closer and finally reaches the exact values by subsequent search operations.
         */
        class CacheEntry
        {
            public:
                simtime_t simulationTime;
                long beginEventNumber; // begin event with simulation time
                long endEventNumber; // end event with simulation time
                file_offset_t beginOffset; // begin offset of begin event
                file_offset_t endEventBeginOffset; // begin offset of end event
                file_offset_t endOffset; // end offset of end event

            public:
                CacheEntry();
                CacheEntry(long eventNumber, simtime_t simulationTime, file_offset_t beginOffset, file_offset_t endOffset);
                void include(long eventNumber, simtime_t simulationTime, file_offset_t beginOffset, file_offset_t endOffset);
                void getBeginKey(long &key);
                void getBeginKey(simtime_t &key);
                void getEndKey(long &key);
                void getEndKey(simtime_t &key);
        };

        /**
         * Subsequent events in an event log file may not have subsequent event numbers,
         * therefore it is insufficient to store the file offset only in the cache.
         */
        typedef std::map<long, CacheEntry> EventNumberToCacheEntryMap;
        EventNumberToCacheEntryMap eventNumberToCacheEntryMap;

        /**
         * Subsequent events in an event log file may have the same simulation time,
         * therefore it is insufficient to store the file offset only in the cache.
         */
        typedef std::map<simtime_t, CacheEntry> SimulationTimeToCacheEntryMap;
        SimulationTimeToCacheEntryMap simulationTimeToCacheEntryMap;

    protected:
        void cacheEntry(long eventNumber, simtime_t simulationTime, file_offset_t beginOffset, file_offset_t endOffset);

        /**
         * Search for the file offset based on the key with the given match kind.
         * The key is either an event number or a simulation time.
         */
        template <typename T> file_offset_t searchForOffset(std::map<T, CacheEntry> &map, T key, MatchKind matchKind);
        /**
         * Search the internal cache finding the file offset(s and keys) for the given key with the given match kind.
         * Sets lower and upper keys and offsets to the closest appropriate values found in the cache.
         * Lower is less than or equal to key, while upper is greater than or equal that could theoretically be found in the event log file.
         * Sets found offset or returns false if the offset cannot be exactly determined.
         */
        template <typename T> bool cacheSearchForOffset(std::map<T, CacheEntry> &map, T key, MatchKind matchKind, T& lowerKey, T& upperKey, file_offset_t& foundOffset, file_offset_t& lowerOffset, file_offset_t& upperOffset);
        /**
         * Binary search through the event log file finding the file offset for the given key with the given match kind.
         * Sets the closest lower and upper keys and offsets around the key found in the event log file. 
         * Returns -1 if the offset cannot be determined.
         */
        template <typename T> file_offset_t  binarySearchForOffset(T key, MatchKind matchKind, T& lowerKey, T& upperKey, file_offset_t& lowerOffset, file_offset_t& upperOffset);
        /**
         * Linear search through the event log file finding the file offset for the given key.
         * The search starts from begin offset and continues in the direction specified by the forward flag.
         * Returns -1 if no such event found otherwise the last exact match or the first non exact match depending 
         * on the exact match required flag.
         */
        template <typename T> file_offset_t linearSearchForOffset(T key, file_offset_t beginOffset, bool forward, bool exactMatchRequired);

        void clearInternalState(FileReader::FileChangedState change = FileReader::OVERWRITTEN);

        bool isEventBeginOffset(file_offset_t offset);

    public:
        /**
         * The reader will be deleted with this object.
         */
        EventLogIndex(FileReader *reader);
        virtual ~EventLogIndex();

        virtual void synchronize();
        long getFirstEventNumber();
        long getLastEventNumber();
        simtime_t& getFirstSimulationTime();
        simtime_t& getLastSimulationTime();
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
        file_offset_t getOffsetForEventNumber(long eventNumber, MatchKind matchKind = EXACT);
        /**
         * Returns the begin file offset of the requested simulation time. See MatchKind for details.
         */
        file_offset_t getOffsetForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT);
        /**
         * Returns true if OK, false if no "E" line found till the end of file in the given direction.
         * Reads the first event line in the given direction starting from the given offset.
         */
        bool readToEventLine(bool forward, file_offset_t readBeginOffset, long& eventNumber, simtime_t& simulationTime, file_offset_t& lineBeginOffset, file_offset_t& lineEndOffset);
        void dump();
};

NAMESPACE_END

#endif
