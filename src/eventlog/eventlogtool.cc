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

#include "filereader.h"
#include "eventlogindex.h"
#include "eventlog.h"
#include "eventlogfilter.h"

void printOffsets(int argc, char **argv)
{
    try {
        fprintf(stderr, "Printing event offsets from event log file %s\n", argv[2]);
    
        FileReader fileReader(argv[2]);
        EventLogIndex eventLogIndex(&fileReader);
        
        for (int i = 3; i < argc; i++) {
            long eventNumber = atol(argv[i]);
            long offset = eventLogIndex.getOffsetForEventNumber(eventNumber);
            printf("Event #%ld --> file offset %ld (0x%lx)\n", eventNumber, offset, offset);
            if (offset!=-1) { //XXX comment out
                fileReader.seekTo(offset);
                printf("  - line at that offset: %s\n", fileReader.readLine());
            }
            //eventLogIndex.dumpTable();
        }
    } catch (Exception *e) {
        fprintf(stderr, "Error: %s\n", e->message());
    }
}

void echo(int argc, char **argv)
{
    try {
        long from = atol(argv[3]);
        long to = atol(argv[4]);
        fprintf(stderr, "Echoing log file %s from event number %d to %d\n", argv[2], from, to);
    
        FileReader fileReader(argv[2]);
        EventLog eventLog(&fileReader);
        eventLog.parse(from, to);
        eventLog.print(stdout);
    } catch (Exception *e) {
        fprintf(stderr, "Error: %s\n", e->message());
    }
}
        
void filter(int argc, char **argv)
{
    try {
        long eventNumber = atol(argv[3]);
        fprintf(stderr, "Filtering log file %s for event number %d\n", argv[2], eventNumber);
    
        FileReader fileReader(argv[2]);
        EventLog eventLog(&fileReader);
        EventLogFilter eventLogFilter(&eventLog, NULL, eventNumber, true, true);
        eventLogFilter.print(stdout);
    } catch (Exception *e) {
        fprintf(stderr, "Error: %s\n", e->message());
    }
}
        
void usage()
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, " eventlogtool offsets <logfile> [<eventnumber>*]\n");
    fprintf(stderr, " eventlogtool echo <logfile> <starteventnumber> <endeventnumber>\n");
    fprintf(stderr, " eventlogtool filter <logfile> <eventnumber>\n");
}

int main(int argc, char **argv)
{
    if (argc<2)
        usage();
    else if (!strcmp(argv[1], "offsets"))
        printOffsets(argc, argv);
    else if (!strcmp(argv[1], "echo"))
        echo(argc, argv);
    else if (!strcmp(argv[1], "filter"))
        filter(argc, argv);
    else
        usage();
    return 0;
}
