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
#include "eventlog.h"
#include "eventlogindex.h"

void echo(int argc, char **argv)
{
    try {
        long from = atol(argv[3]);
        long to = atol(argv[4]);
        fprintf(stderr, "Echoing log file %s from event number %d to %d\n", argv[2], from, to);
    
        FileReader reader(argv[2]);
        EventLog log(&reader);
        log.parse(from, to);
        log.print(stdout);
    } catch (Exception *e) {
        fprintf(stderr, "Error: %s\n", e->message());
    }
}
        
void printOffsets(int argc, char **argv)
{
    try {
        fprintf(stderr, "Printing event offsets from event log file %s\n", argv[2]);
    
        FileReader reader(argv[2]);
        EventLogIndex index(&reader);
        
        for (int i = 3; i < argc; i++) {
            long eventNumber = atol(argv[i]);
            long offset = index.getOffsetForEventNumber(eventNumber);
            printf("Event #%ld --> file offset %ld (0x%lx)\n", eventNumber, offset, offset);
            if (offset!=-1) { //XXX comment out
                reader.seekTo(offset);
                printf("  - line at that offset: %s\n", reader.readLine());
            }
            //index.dumpTable();
        }
    } catch (Exception *e) {
        fprintf(stderr, "Error: %s\n", e->message());
    }
}

void usage()
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, " eventlogtool echo <logfile> <starteventnumber> <endeventnumber>\n");
    fprintf(stderr, " eventlogtool offsets <logfile> [<eventnumber>*]\n");
}

int main(int argc, char **argv)
{
    if (argc<2)
        usage();
    else if (!strcmp(argv[1], "echo"))
        echo(argc, argv);
    else if (!strcmp(argv[1], "offsets"))
        printOffsets(argc, argv);
    else
        usage();
    return 0;
}
