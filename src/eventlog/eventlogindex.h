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
#include "defs.h"
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
class EventLogIndex
{
    protected:
        LineTokenizer tokenizer;
        FileReader *reader;

        typedef std::map<long, long> EventNumberToOffsetMap;
        EventNumberToOffsetMap eventNumberToOffsetMap;

        typedef std::map<simtime_t, long> SimulationTimeToOffsetMap;
        SimulationTimeToOffsetMap simulationTimeToOffsetMap;

        long firstEventNumber;
        long lastEventNumber;
        simtime_t firstSimulationTime;
        simtime_t lastSimulationTime;

    protected:
        // return true if the given offset should be stored in the map (not already there, etc)
        bool needsToBeStored(long eventNumber);
        // true if OK, false if no "E" line found till the end of file
        bool readToFirstEventLine(long readStartOffset, long& eventNumber, simtime_t& simulationTime, long& lineStartOffset, long& lineEndOffset);
        // reads the first event line in the given direction starting from the given offset
        bool readToEventLine(bool forward, long readStartOffset, long& eventNumber, simtime_t& simulationTime, long& lineStartOffset, long& lineEndOffset);

        void addPosition(long eventNumber, simtime_t simulationTime, long offset);
        template<typename T> long binarySearchForOffset(bool eventNumberBased, std::map<T, long> *keyToOffsetMap, T key, MatchKind matchKind);
        template <typename T> long linearSearchForOffset(bool eventNumberBased, long offset, T key, MatchKind matchKind, bool exactMatchFound);

    public:
        // reader will be deleted
        EventLogIndex(FileReader *reader);
        ~EventLogIndex();

        long getFirstEventNumber();
        long getLastEventNumber();
        long getBeginOffsetForEndOffset(long endOffset);
        long getEndOffsetForBeginOffset(long beginOffset);
        long getOffsetForEventNumber(long eventNumber, MatchKind matchKind = EXACT);
        bool positionToEventNumber(long eventNumber, MatchKind matchKind = EXACT);
        long getOffsetForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT);
        bool positionToSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT);
        void dumpTable();
};

#endif
