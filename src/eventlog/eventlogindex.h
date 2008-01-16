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

// we store the offset of roughly every Xth event
#define EVENTNUM_INDEX_DENSITY 100

/**
 * Allows random access of an event log file, i.e. positioning
 * on arbitrary event numbers.
 */
class EVENTLOG_API EventLogIndex
{
    protected:
        LineTokenizer tokenizer;
        FileReader *reader;

        typedef std::map<long, file_offset_t> EventNumberToOffsetMap;
        EventNumberToOffsetMap eventNumberToOffsetMap;

        typedef std::map<simtime_t, file_offset_t> SimulationTimeToOffsetMap;
        SimulationTimeToOffsetMap simulationTimeToOffsetMap;

        long firstEventNumber;
        long lastEventNumber;
        file_offset_t firstEventOffset;
        file_offset_t lastEventOffset;
        simtime_t firstSimulationTime;
        simtime_t lastSimulationTime;

    protected:
        // return true if the given offset should be stored in the map (not already there, etc)
        bool needsToBeStored(long eventNumber);
        void addPosition(long eventNumber, simtime_t simulationTime, file_offset_t offset);
        template <typename T> file_offset_t binarySearchForOffset(bool eventNumberBased, std::map<T, file_offset_t> *keyToOffsetMap, T key, MatchKind matchKind);
        template <typename T> file_offset_t linearSearchForOffset(bool eventNumberBased, file_offset_t offset, T key, bool forward, bool exactMatchFound);
        void clearInternalState(FileReader::FileChangedState change = FileReader::OVERWRITTEN);

    public:
        // reader will be deleted
        EventLogIndex(FileReader *reader);
        virtual ~EventLogIndex();

        virtual void synchronize();
        long getFirstEventNumber();
        long getLastEventNumber();
        simtime_t& getFirstSimulationTime();
        simtime_t& getLastSimulationTime();
        file_offset_t getFirstEventOffset();
        file_offset_t getLastEventOffset();
        file_offset_t getBeginOffsetForEndOffset(file_offset_t endOffset);
        file_offset_t getEndOffsetForBeginOffset(file_offset_t beginOffset);
        file_offset_t getOffsetForEventNumber(long eventNumber, MatchKind matchKind = EXACT);
        file_offset_t getOffsetForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT);
        bool positionToEventNumber(long eventNumber, MatchKind matchKind = EXACT);
        bool positionToSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT);
        // true if OK, false if no "E" line found till the end of file in the given direction
        // reads the first event line in the given direction starting from the given offset
        bool readToEventLine(bool forward, file_offset_t readStartOffset, long& eventNumber, simtime_t& simulationTime, file_offset_t& lineStartOffset, file_offset_t& lineEndOffset);
        void dumpTable();
};

#endif
