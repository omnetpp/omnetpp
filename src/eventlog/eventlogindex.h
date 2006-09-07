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

        typedef std::map<long,long> EventNumberToOffsetMap;
        EventNumberToOffsetMap eventNumberToOffsetMap;

        typedef std::map<simtime_t,long> SimulationTimeToOffsetMap;
        SimulationTimeToOffsetMap simulationTimeToOffsetMap;

    protected:
        // true if OK, false if no "E" line found till end of file
        bool readToFirstEventLine(long startOffset, long& eventNumber, long& offset);
        
        // return true if the given offset should be stored in the map (not already there, etc)
        bool needsToBeStored(long eventNumber);

    public:
        EventLogIndex(FileReader *reader);
        ~EventLogIndex();
        void addPositionForEventNumber(long eventNumber, long offset);
        void addPositionForSimulationTime(simtime_t simulationTime, bool first, long offset);
        long getOffsetForEventNumber(long eventNumber);
        bool positionToEventNumber(long eventNumber);
        long getOffsetForSimulationTime(simtime_t simulationTime, bool first);
        bool positionToSimulationTime(simtime_t simulationTime, bool first);
        void dumpTable();
        
};

#endif
