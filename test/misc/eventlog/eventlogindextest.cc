//=========================================================================
//  EVENTLOGINDEXTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <common/commonutil.h>
#include <common/exception.h>
#include <common/lcgrandom.h>
#include <common/filereader.h>
#include <eventlog/eventlogindex.h>

using namespace omnetpp::common;
using namespace omnetpp::eventlog;

bool getReadForward(MatchKind matchKind)
{
    switch (matchKind) {
        case FIRST_OR_NEXT:
        case LAST_OR_NEXT:
            return false;

        case EXACT:
        case FIRST_OR_PREVIOUS:
        case LAST_OR_PREVIOUS:
            return true;

        default:
            throw opp_runtime_error("*** Invalid match kind\n");
    }
}

void checkOffset(EventLogIndex& eventLogIndex, file_offset_t offset)
{
    Assert(offset != -1);

    file_offset_t endOffset = eventLogIndex.getEndOffsetForBeginOffset(offset);
    if (endOffset == -1)
        throw opp_runtime_error("*** No end offset for begin offset: %" PRId64 "\n", offset);

    file_offset_t beginOffset = eventLogIndex.getBeginOffsetForEndOffset(endOffset);
    if (beginOffset == -1)
        throw opp_runtime_error("*** No begin offset for end offset: %" PRId64 "\n", endOffset);

    if (offset != beginOffset)
        throw opp_runtime_error("*** Inconsistent begin and end offsets for offset: %" PRId64 "\n", offset);
}

void checkEventNumberOffset(EventLogIndex& eventLogIndex, file_offset_t offset, MatchKind matchKind, eventnumber_t eventNumber)
{
    if (offset != -1) {
        checkOffset(eventLogIndex, offset);

        eventnumber_t checkEventNumber;
        file_offset_t lineStartOffset, lineEndOffset;
        simtime_t simulationTime;

        eventLogIndex.readToEventLine(true, offset, checkEventNumber, simulationTime, lineStartOffset, lineEndOffset);

        if (checkEventNumber != eventNumber) {
            bool forward = getReadForward(matchKind);
            eventLogIndex.readToEventLine(forward, forward ? offset + 1 : offset, checkEventNumber, simulationTime, lineStartOffset, lineEndOffset);

            if ((!forward || eventNumber > checkEventNumber) &&
                (forward || eventNumber < checkEventNumber))
                throw opp_runtime_error("*** Found wrong offset for event number: %" PRId64 "\n", eventNumber);
        }
    }
}

void checkSimulationTimeOffset(EventLogIndex& eventLogIndex, file_offset_t offset, MatchKind matchKind, simtime_t simulationTime)
{
    if (offset != -1) {
        checkOffset(eventLogIndex, offset);

        eventnumber_t eventNumber;
        file_offset_t lineStartOffset, lineEndOffset;
        simtime_t checkSimulationTime;
        bool forward = getReadForward(matchKind);

        eventLogIndex.readToEventLine(true, offset, eventNumber, checkSimulationTime, lineStartOffset, lineEndOffset);

        if (checkSimulationTime != simulationTime) {
            bool forward = getReadForward(matchKind);
            eventLogIndex.readToEventLine(forward, forward ? offset + 1 : offset, eventNumber, checkSimulationTime, lineStartOffset, lineEndOffset);

            if ((!forward || simulationTime > checkSimulationTime) &&
                (forward || simulationTime < checkSimulationTime))
                throw opp_runtime_error("*** Found wrong offset for simulation time: %g\n", simulationTime.dbl());
        }
    }
}

void testEventLogIndex(const char *fileName, int numberOfOffsetLookups)
{
    LCGRandom random;
    EventLogIndex eventLogIndex(new FileReader(fileName));

    eventnumber_t firstEventNumber = eventLogIndex.getFirstEventNumber();
    simtime_t firstSimulationTime = eventLogIndex.getFirstSimulationTime();
    file_offset_t firstEventOffset = eventLogIndex.getFirstEventOffset();

    eventnumber_t lastEventNumber = eventLogIndex.getLastEventNumber();
    simtime_t lastSimulationTime = eventLogIndex.getLastSimulationTime();
    file_offset_t lastEventOffset = eventLogIndex.getLastEventOffset();

    if ((firstEventNumber == -1 || lastEventNumber == -1) && firstEventNumber != lastEventNumber)
        throw opp_runtime_error("*** Inconsistent first and last event numbers: %" PRId64 ", %" PRId64 "\n", firstEventNumber, lastEventNumber);

    if (lastEventNumber != -1) {
        while (numberOfOffsetLookups--) {
            // seek to random event number
            eventnumber_t eventNumber = random.next01() * lastEventNumber;
            printf("Seeking for event number: %" PRId64 "\n", eventNumber);
            file_offset_t offset = eventLogIndex.getOffsetForEventNumber(eventNumber, EXACT);
            file_offset_t firstOrPreviousOffset = eventLogIndex.getOffsetForEventNumber(eventNumber, FIRST_OR_PREVIOUS);
            file_offset_t firstOrNextOffset = eventLogIndex.getOffsetForEventNumber(eventNumber, FIRST_OR_NEXT);
            file_offset_t lastOrPreviousOffset = eventLogIndex.getOffsetForEventNumber(eventNumber, LAST_OR_PREVIOUS);
            file_offset_t lastOrNextOffset = eventLogIndex.getOffsetForEventNumber(eventNumber, LAST_OR_NEXT);

            if (firstOrPreviousOffset > lastOrNextOffset)
                throw opp_runtime_error("*** Inconsistent first or previous and last or next offsets for event number: %" PRId64 "\n", eventNumber);

            if (firstOrNextOffset < firstOrPreviousOffset)
                throw opp_runtime_error("*** Inconsistent first or next and first or previous offsets for event number: %" PRId64 "\n", eventNumber);

            if (lastOrNextOffset < lastOrPreviousOffset)
                throw opp_runtime_error("*** Inconsistent last or next and last or previous offsets for event number: %" PRId64 "\n", eventNumber);

            checkEventNumberOffset(eventLogIndex, offset, EXACT, eventNumber);
            checkEventNumberOffset(eventLogIndex, firstOrPreviousOffset, FIRST_OR_PREVIOUS, eventNumber);
            checkEventNumberOffset(eventLogIndex, firstOrNextOffset, FIRST_OR_NEXT, eventNumber);
            checkEventNumberOffset(eventLogIndex, lastOrPreviousOffset, LAST_OR_PREVIOUS, eventNumber);
            checkEventNumberOffset(eventLogIndex, lastOrNextOffset, LAST_OR_NEXT, eventNumber);

            // seek to random simulation time
            simtime_t simulationTime = random.next01() * lastSimulationTime;
            printf("Seeking for simulation time: %g\n", simulationTime.dbl());
            offset = eventLogIndex.getOffsetForSimulationTime(simulationTime, EXACT);
            firstOrPreviousOffset = eventLogIndex.getOffsetForSimulationTime(simulationTime, FIRST_OR_PREVIOUS);
            firstOrNextOffset = eventLogIndex.getOffsetForSimulationTime(simulationTime, FIRST_OR_NEXT);
            lastOrPreviousOffset = eventLogIndex.getOffsetForSimulationTime(simulationTime, LAST_OR_PREVIOUS);
            lastOrNextOffset = eventLogIndex.getOffsetForSimulationTime(simulationTime, LAST_OR_NEXT);

            if (firstOrPreviousOffset > lastOrNextOffset)
                throw opp_runtime_error("*** Inconsistent first or previous and last or next offsets for simulation time: %g\n", simulationTime.dbl());

            if (firstOrNextOffset < firstOrPreviousOffset)
                throw opp_runtime_error("*** Inconsistent first or next and first or previous offsets for simulation time: %g\n", simulationTime.dbl());

            if (lastOrNextOffset < lastOrPreviousOffset)
                throw opp_runtime_error("*** Inconsistent last or next and last or previous offsets for simulation time: %g\n", simulationTime.dbl());

            checkSimulationTimeOffset(eventLogIndex, offset, EXACT, simulationTime);
            checkSimulationTimeOffset(eventLogIndex, firstOrPreviousOffset, FIRST_OR_PREVIOUS, simulationTime);
            checkSimulationTimeOffset(eventLogIndex, firstOrNextOffset, FIRST_OR_NEXT, simulationTime);
            checkSimulationTimeOffset(eventLogIndex, lastOrPreviousOffset, LAST_OR_PREVIOUS, simulationTime);
            checkSimulationTimeOffset(eventLogIndex, lastOrNextOffset, LAST_OR_NEXT, simulationTime);
        }
    }
}

void usage(const char *message)
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

