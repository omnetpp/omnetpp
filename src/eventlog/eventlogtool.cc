//=========================================================================
//  EVENTLOGTOOL.CC - part of
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
#include "filereader.h"
#include "eventlogindex.h"

void filter(int argc, char **argv)
{
    fprintf(stderr, "Loading event log file %s\n", argv[2]);
    EventLog eventLog(argv[2]);

    long eventNumber = atol(argv[3]);
    fprintf(stderr, "Filtering for event number %ld\n", eventNumber);
    EventEntry *event = eventLog.getEventByNumber(eventNumber);
    if (event==NULL)
    {
        fprintf(stderr, "%s: Event #%ld not in the given event log file\n", argv[0], eventNumber);
        exit(1);
    }

    std::set<int> *moduleIds = argc < 5 ? NULL : new std::set<int>;

    for (int i = 0; i < argc - 4; i++) {
        int id = atoi(argv[4 + i]);
        fprintf(stderr, "Filtering for module id %d\n", id);
        moduleIds->insert(id);
    }

    EventLog *traceLog = eventLog.traceEvent(event, moduleIds, true, true, false);
    traceLog->writeTrace(stdout);
    delete traceLog;
}

void printOffsets(int argc, char **argv)
{
    fprintf(stderr, "Printing event offsets from event log file %s\n", argv[2]);
    
    try {
        FileReader reader(argv[2]);
        EventLogIndex index(&reader);
        
        for (int i = 3; i < argc; i++) {
            long eventNumber = atol(argv[i]);
            long offset = index.getOffsetFor(eventNumber);
            printf("Event #%ld --> file offset %ld (0x%lx)\n", eventNumber, offset, offset);
reader.seekTo(offset-1); //XXX
printf("   line there: %s\n", reader.readLine());
            index.dumpTable();
        }
    } catch (Exception *e) {
        fprintf(stderr, e->message());
    }
}

void usage()
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, " eventlogtool filter <logfile> <eventnumber> [<moduleid>*]\n");
    fprintf(stderr, " eventlogtool offsets <logfile> [<eventnumber>*]\n");
}

int main(int argc, char **argv)
{
    if (argc<2)
        usage();
    else if (!strcmp(argv[1], "filter"))
        filter(argc, argv);
    else if (!strcmp(argv[1], "offsets"))
        printOffsets(argc, argv);
    else
        usage();
    return 0;
}
