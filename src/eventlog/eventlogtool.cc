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

#include <time.h>
#include "filereader.h"
#include "eventlogindex.h"
#include "eventlog.h"
#include "filteredeventlog.h"

void readLines(int argc, char **argv)
{
    try {
        fprintf(stderr, "Reading lines from log%s\n", argv[2]);
        FileReader fileReader(argv[2]);
        long begin = clock();
        long lineCount = 0;

        while (fileReader.readLine()) lineCount++;

        long end = clock();
        fprintf(stderr, "Reading %ld lines completed in %g seconds\n", lineCount, (double)(end - begin) / CLOCKS_PER_SEC);
    } catch (Exception *e) {
        fprintf(stderr, "Error: %s\n", e->message());
    }
}

void loadEvents(int argc, char **argv)
{
    try {
        fprintf(stderr, "Loading events from log file into memory %s\n", argv[2]);
        FileReader *fileReader = new FileReader(argv[2]);
        EventLog eventLog(fileReader);
        long begin = clock();
        long eventCount = 0;

        IEvent *event = eventLog.getFirstEvent();
        while (event) {
            eventCount++;
            event = event->getNextEvent();
        }

        long end = clock();
        fprintf(stderr, "Loading %ld events completed in %g seconds\n", eventCount, (double)(end - begin) / CLOCKS_PER_SEC);
    } catch (Exception *e) {
        fprintf(stderr, "Error: %s\n", e->message());
    }
}

void printOffsets(int argc, char **argv)
{
    try {
        fprintf(stderr, "Printing event offsets from event log file %s\n", argv[2]);
    
        FileReader *fileReader = new FileReader(argv[2]);
        EventLogIndex eventLogIndex(fileReader);
        
        for (int i = 3; i < argc; i++) {
            long eventNumber = atol(argv[i]);
            long offset = eventLogIndex.getOffsetForEventNumber(eventNumber);
            printf("Event #%ld --> file offset %ld (0x%lx)\n", eventNumber, offset, offset);
            if (offset!=-1) { //XXX comment out
                fileReader->seekTo(offset);
                printf("  - line at that offset: %s\n", fileReader->readLine());
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
        long from = argc > 3 ? atol(argv[3]) : -1;
        long to = argc > 4 ? atol(argv[4]) : -1;
        fprintf(stderr, "Echoing log file %s from event number %d to %d\n", argv[2], from, to);
    
        FileReader *fileReader = new FileReader(argv[2]);
        EventLog eventLog(fileReader);
        eventLog.print(stdout, from, to);
    } catch (Exception *e) {
        fprintf(stderr, "Error: %s\n", e->message());
    }
}
        
void filter(int argc, char **argv)
{
    try {
        long tracedEventNumber = atol(argv[3]);
        long fromEventNumber = atol(argv[4]);
        long toEventNumber = atol(argv[5]);
        fprintf(stderr, "Filtering log file: %s for event number: %ld from event number: %ld to event number: %ld\n",
            argv[2], tracedEventNumber, fromEventNumber, toEventNumber);
    
        FileReader *fileReader = new FileReader(argv[2]);
        EventLog *eventLog = new EventLog(fileReader);
        FilteredEventLog filteredEventLog(eventLog, NULL, tracedEventNumber, true, true, fromEventNumber, toEventNumber);
        filteredEventLog.print(stdout);

        fprintf(stderr, "Number of events parsed: %d and number of lines read: %ld\n", Event::getNumParsedEvent(), FileReader::getNumReadLines());
    } catch (Exception *e) {
        fprintf(stderr, "Error: %s\n", e->message());
    }
}
        
void consequences(int argc, char **argv)
{
    try {
        long tracedEventNumber = atol(argv[3]);
        std::set<int> *moduleIds = new std::set<int>();

        for (int i = 4; i < argc; i++)
            moduleIds->insert(atoi(argv[i]));

        fprintf(stderr, "Filtering log file: %s for event number: %ld\n", argv[2], tracedEventNumber);
    
        FileReader *fileReader = new FileReader(argv[2]);
        EventLog *eventLog = new EventLog(fileReader);
        FilteredEventLog filteredEventLog(eventLog, moduleIds, tracedEventNumber, true, true);
        FilteredEvent *filteredEvent = filteredEventLog.getEventForEventNumber(tracedEventNumber);
        MessageDependencyList *messageDependencies = filteredEvent->getConsequences();

        for (MessageDependencyList::iterator it = messageDependencies->begin(); it != messageDependencies->end(); it++)
            (*it)->print(stdout);

        fprintf(stderr, "Number of events parsed: %d and number of lines read: %ld\n", Event::getNumParsedEvent(), FileReader::getNumReadLines());
    } catch (Exception *e) {
        fprintf(stderr, "Error: %s\n", e->message());
    }
}
        
void usage()
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, " eventlogtool readlines <logfile>\n");
    fprintf(stderr, " eventlogtool loadevents <logfile>\n");
    fprintf(stderr, " eventlogtool offsets <logfile> [<eventnumber>*]\n");
    fprintf(stderr, " eventlogtool echo <logfile> <starteventnumber> <endeventnumber>\n");
    fprintf(stderr, " eventlogtool filter <logfile> <tracedEventNumber> <fromeventnumber> <toeventnumber>\n");
    fprintf(stderr, " eventlogtool consequences <logfile> <tracedEventNumber> <moduleid>*\n");
}

int main(int argc, char **argv)
{
    if (argc<2)
        usage();
    else if (!strcmp(argv[1], "readlines"))
        readLines(argc, argv);
    else if (!strcmp(argv[1], "loadevents"))
        loadEvents(argc, argv);
    else if (!strcmp(argv[1], "offsets"))
        printOffsets(argc, argv);
    else if (!strcmp(argv[1], "echo"))
        echo(argc, argv);
    else if (!strcmp(argv[1], "filter"))
        filter(argc, argv);
    else if (!strcmp(argv[1], "consequences"))
        consequences(argc, argv);
    else
        usage();
    return 0;
}
