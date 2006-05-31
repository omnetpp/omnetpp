//=========================================================================
//  TRACETOOL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "eventlog.h"

int main(int argc, char **argv)
{
    if (argc<3)
    {
        fprintf(stderr, "%s: Usage: tracetool <logfile> <eventnumber> [<moduleid>*]\n", argv[0]);
        exit(0);
    }

    fprintf(stderr, "Loading log file %s\n", argv[1]);
    EventLog eventLog(argv[1]);

    long eventNumber = atol(argv[2]);
    fprintf(stderr, "Filtering for event number %ld\n", eventNumber);
    EventEntry *event = eventLog.getEventByNumber(eventNumber);
    if (event==NULL)
    {
        fprintf(stderr, "%s: Event #%ld not in the given trace file\n", argv[0], eventNumber);
        exit(1);
    }

    std::set<int> *moduleIds = argc < 4 ? NULL : new std::set<int>;

    for (int i = 0; i < argc - 3; i++) {
        int id = atoi(argv[3] + i);
        fprintf(stderr, "Filtering for module id %d\n", id);
        moduleIds->insert(id);
    }

    EventLog *traceLog = eventLog.traceEvent(event, moduleIds, true, true);
    traceLog->writeTrace(stdout);
    delete traceLog;
    return 0;
}
