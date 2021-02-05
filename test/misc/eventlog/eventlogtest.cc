//=========================================================================
//  EVENTLOGTEST.CC - part of
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
#include <eventlog/eventlog.h>

using namespace omnetpp::common;
using namespace omnetpp::eventlog;

void checkEvent(IEvent *event)
{
    printf("Checking event: %" PRId64 "\n", event->getEventNumber());

    if (event != ((EventLog *)event->getEventLog())->getEventForBeginOffset(event->getBeginOffset()))
        throw opp_runtime_error("*** Event at begin offset does not match", event->getEventNumber());

    if (event != ((EventLog *)event->getEventLog())->getEventForEndOffset(event->getEndOffset()))
        throw opp_runtime_error("*** Event at end offset does not match", event->getEventNumber());

    if (event->getEventNumber() < 0)
        throw opp_runtime_error("*** Event without event number", event->getEventNumber());

    if (event->getSimulationTime() < 0)
        throw opp_runtime_error("*** Event without simulation time", event->getEventNumber());

    // check causes
    IMessageDependencyList *causes = event->getCauses();
    for (IMessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++) {
        IMessageDependency *messageDependency = *it;
        IEvent *causeEvent = messageDependency->getCauseEvent();

        if (causeEvent) {
            bool foundCauseConsequence = false;
            IMessageDependencyList *causeConseqences = causeEvent->getConsequences();

            for (IMessageDependencyList::iterator ot = causeConseqences->begin(); ot != causeConseqences->end(); ot++) {
                if ((*ot)->getConsequenceEvent() == event) {
                    foundCauseConsequence = true;
                    break;
                }
            }

            if (!foundCauseConsequence)
                throw opp_runtime_error("*** Consistency check failed, could not find event %" PRId64 " in the consequences of event %" PRId64 " which is included in the causes of event %" PRId64 "\n",
                        event->getEventNumber(), causeEvent->getEventNumber(), event->getEventNumber());
        }
    }

    // check consequences
    IMessageDependencyList *consequences = event->getConsequences();
    for (IMessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++) {
        IMessageDependency *messageDependency = *it;
        IEvent *consequenceEvent = messageDependency->getConsequenceEvent();

        if (consequenceEvent) {
            bool foundConsequenceCause = false;
            IMessageDependencyList *consequenceCauses = consequenceEvent->getCauses();

            for (IMessageDependencyList::iterator ot = consequenceCauses->begin(); ot != consequenceCauses->end(); ot++) {
                if ((*ot)->getCauseEvent() == event) {
                    foundConsequenceCause = true;
                    break;
                }
            }

            if (!foundConsequenceCause)
                throw opp_runtime_error("*** Consistency check failed, could not find event %" PRId64 " in the causes of event %" PRId64 " which is included in the consequences of event %" PRId64 "\n",
                        event->getEventNumber(), consequenceEvent->getEventNumber(), event->getEventNumber());
        }
    }
}

void testRandomEventLogAccess(const char *fileName, int numberOfRandomReads, int numberOfEventsToScan)
{
    LCGRandom random;
    EventLog *eventLog = new EventLog(new FileReader(fileName));

    IEvent *firstEvent = eventLog->getFirstEvent();
    IEvent *lastEvent = eventLog->getLastEvent();

    long numberOfEvents = eventLog->getApproximateNumberOfEvents();

    if ((!firstEvent || !lastEvent) &&
        firstEvent != lastEvent)
        throw opp_runtime_error("*** Inconsistent first and last events");

    if (lastEvent) {
        checkEvent(firstEvent);
        checkEvent(lastEvent);

        while (numberOfRandomReads--) {
            Event *event;

            if (random.next01() < 0.5) {
                eventnumber_t eventNumber = random.next01() * (lastEvent->getEventNumber() + 1);
                printf("Seeking for event number: %" PRId64 "\n", eventNumber);
                event = eventLog->getEventForEventNumber(eventNumber, (MatchKind)(int)(random.next01() * 5));
            }
            else {
                simtime_t simulationTime = random.next01() * lastEvent->getSimulationTime();
                printf("Seeking for simulation time: %g\n", simulationTime.dbl());
                event = eventLog->getEventForSimulationTime(simulationTime, (MatchKind)(int)(random.next01() * 5));
            }

            if (event) {
                checkEvent(event);
                bool forward = random.next01() < 0.5;

                for (long i = 0; i < numberOfEventsToScan; i++) {
                    Event *neighbour = forward ? event->getNextEvent() : event->getPreviousEvent();

                    if (neighbour) {
                        checkEvent(neighbour);
                        event = neighbour;
                    }
                    else if (event != lastEvent &&
                             event != firstEvent)
                        throw opp_runtime_error("*** No neighbour event but not at the very beginning or end of the file");
                    else
                        break;
                }
            }

            checkEvent(eventLog->getApproximateEventAt(random.next01()));
        }
    }

    delete eventLog;
}

void testSerialEventLogAccess(const char *fileName, bool forward)
{
    IEventLog *eventLog = new EventLog(new FileReader(fileName));
    IEvent *firstEvent = eventLog->getFirstEvent();
    IEvent *lastEvent = eventLog->getLastEvent();
    IEvent *event = forward ? firstEvent : lastEvent;

    while (event) {
        checkEvent(event);

        if (event == (forward ? lastEvent : firstEvent))
            break;
        else
            event = forward ? event->getNextEvent() : event->getPreviousEvent();
    }

    delete eventLog;
}

void usage(const char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);

    fprintf(stderr, ""
                    "Usage:\n"
                    "   eventlogtest <input-file-name> [(forward|backward)] [<number-of-random-reads>] [<number-of-events-to-scan>]\n"
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
            if (!strcmp(argv[2], "forward"))
                testSerialEventLogAccess(argv[1], true);
            else if (!strcmp(argv[2], "backward"))
                testSerialEventLogAccess(argv[1], false);
            else
                testRandomEventLogAccess(argv[1], atoi(argv[2]), atoi(argv[3]));
            printf("PASS\n");

            return 0;
        }
    }
    catch (std::exception& e) {
        printf("FAIL: %s", e.what());

        return -2;
    }
}

