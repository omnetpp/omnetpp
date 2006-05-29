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
        fprintf(stderr, "%s: Usage: tracetool <logfile> <eventnumber>\n", argv[0]);
        exit(0);
    }

    EventLog eventLog(argv[1]);

    long eventNumber = atol(argv[2]);
    EventEntry *event = eventLog.getEventByNumber(eventNumber);
    if (event==NULL)
    {
        fprintf(stderr, "%s: Event #%ld not in the given trace file\n", argv[0], eventNumber);
        exit(1);
    }

    EventLog *traceLog = eventLog.traceEvent(event, true, true);
    traceLog->writeTrace(stdout);
    delete traceLog;
    return 0;
}
