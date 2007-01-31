//=========================================================================
//  EVENTLOGINDEXTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "commonutil.h"
#include "exception.h"
#include "lcgrandom.h"
#include "filereader.h"
#include "eventlogindex.h"

void checkOffset(EventLogIndex& eventLogIndex, file_offset_t offset)
{
    Assert(offset != -1);

    file_offset_t endOffset = eventLogIndex.getEndOffsetForBeginOffset(offset);
    if (endOffset == -1)
        throw opp_runtime_error("*** No end offset for begin offset: %lld\n", offset);

    file_offset_t beginOffset = eventLogIndex.getBeginOffsetForEndOffset(endOffset);
    if (beginOffset == -1)
        throw opp_runtime_error("*** No begin offset for end offset: %lld\n", endOffset);

    if (offset != beginOffset)
        throw opp_runtime_error("*** Inconsistent begin and end offsets for offset: %lld\n", offset);
}

void checkValidOffset(EventLogIndex& eventLogIndex, file_offset_t offset)
{
    if (offset != -1)
        checkOffset(eventLogIndex, offset);
}

void testEventLogIndex(const char *fileName, int numberOfOffsetLookups)
{
    LCGRandom random;
    EventLogIndex eventLogIndex(new FileReader(fileName));

    long firstEventNumber = eventLogIndex.getFirstEventNumber();
    simtime_t firstSimulationTime = eventLogIndex.getFirstSimulationTime();
    file_offset_t firstEventOffset = eventLogIndex.getFirstEventOffset();

    long lastEventNumber = eventLogIndex.getLastEventNumber();
    simtime_t lastSimulationTime = eventLogIndex.getLastSimulationTime();
    file_offset_t lastEventOffset = eventLogIndex.getLastEventOffset();

    if ((firstEventNumber == -1 || lastEventNumber == -1) && firstEventNumber != lastEventNumber)
        throw opp_runtime_error("*** Inconsistent first and last event numbers: %ld, %ld\n", firstEventNumber, lastEventNumber);

    if (lastEventNumber != -1) {
        while (numberOfOffsetLookups--) {
            // seek to random event number
            long eventNumber = random.next01() * lastEventNumber;
            printf("Seeking for event number: %ld\n", eventNumber);
            file_offset_t offset = eventLogIndex.getOffsetForEventNumber(eventNumber, (MatchKind)(int)(random.next01() * 5));
            if (offset != -1)
                checkOffset(eventLogIndex, offset);
            else {
                file_offset_t firstOrPreviousOffset = eventLogIndex.getOffsetForEventNumber(eventNumber, FIRST_OR_PREVIOUS);
                file_offset_t firstOrNextOffset = eventLogIndex.getOffsetForEventNumber(eventNumber, FIRST_OR_NEXT);
                file_offset_t lastOrPreviousOffset = eventLogIndex.getOffsetForEventNumber(eventNumber, LAST_OR_PREVIOUS);
                file_offset_t lastOrNextOffset = eventLogIndex.getOffsetForEventNumber(eventNumber, LAST_OR_NEXT);

                if (firstOrPreviousOffset >= lastOrNextOffset)
                    throw opp_runtime_error("*** Inconsistent first or previous and last or next offsets for event number: %ld\n", eventNumber);

                if (firstOrNextOffset < firstOrPreviousOffset)
                    throw opp_runtime_error("*** Inconsistent first or next and first or previous offsets for event number: %ld\n", eventNumber);

                if (lastOrNextOffset < lastOrPreviousOffset)
                    throw opp_runtime_error("*** Inconsistent last or next and last or previous offsets for event number: %ld\n", eventNumber);

                checkValidOffset(eventLogIndex, firstOrPreviousOffset);
                checkValidOffset(eventLogIndex, firstOrNextOffset);
                checkValidOffset(eventLogIndex, firstOrPreviousOffset);
                checkValidOffset(eventLogIndex, firstOrNextOffset);
            }

            // seek to random simulation time
            simtime_t simulationTime = random.next01() * lastSimulationTime;
            printf("Seeking for simulation time: %g\n", simulationTime);
            offset = eventLogIndex.getOffsetForSimulationTime(simulationTime, (MatchKind)(int)(random.next01() * 5));
            if (offset != -1)
                checkOffset(eventLogIndex, offset);
            else {
                file_offset_t firstOrPreviousOffset = eventLogIndex.getOffsetForSimulationTime(simulationTime, FIRST_OR_PREVIOUS);
                file_offset_t firstOrNextOffset = eventLogIndex.getOffsetForSimulationTime(simulationTime, FIRST_OR_NEXT);
                file_offset_t lastOrPreviousOffset = eventLogIndex.getOffsetForSimulationTime(simulationTime, LAST_OR_PREVIOUS);
                file_offset_t lastOrNextOffset = eventLogIndex.getOffsetForSimulationTime(simulationTime, LAST_OR_NEXT);

                if (firstOrPreviousOffset >= lastOrNextOffset)
                    throw opp_runtime_error("*** Inconsistent first or previous and last or next offsets for simulation time: %g\n", simulationTime);

                if (firstOrNextOffset < firstOrPreviousOffset)
                    throw opp_runtime_error("*** Inconsistent first or next and first or previous offsets for simulation time: %ld\n", simulationTime);

                if (lastOrNextOffset < lastOrPreviousOffset)
                    throw opp_runtime_error("*** Inconsistent last or next and last or previous offsets for simulation time: %ld\n", simulationTime);

                checkValidOffset(eventLogIndex, firstOrPreviousOffset);
                checkValidOffset(eventLogIndex, firstOrNextOffset);
                checkValidOffset(eventLogIndex, firstOrPreviousOffset);
                checkValidOffset(eventLogIndex, firstOrNextOffset);
            }
        }
    }
}

void usage(char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);

    fprintf(stderr, ""
"Usage:\n"
"   eventlogindextest <input-file-name> <number-of-offset-lookups>\n"
);
}

int main(int argc, char **argv)
{
    try {
        if (argc < 3) {
            usage("Not enough arguments specified");

            return -1;
        }
        else {
            testEventLogIndex(argv[1], atoi(argv[2]));
            printf("PASS\n");

            return 0;
        }
    }
    catch (std::exception& e) {
        printf("FAIL: %s", e.what());

        return -2;
    }
}
