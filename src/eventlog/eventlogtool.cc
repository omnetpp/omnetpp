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
#include "linetokenizer.h"
#include "eventlogindex.h"
#include "eventlog.h"
#include "filteredeventlog.h"

struct Options {
    char *inputFileName;
    char *outputFileName;
    FILE *outputFile;
    long fromEventNumber;
    long toEventNumber;
    bool outputInitialization;
    bool traceBackward;
    bool traceForward;
    std::vector<long> *eventNumbers;
    std::vector<long> *fileOffsets;
    bool verbose;

    Options() {
        inputFileName = NULL;
        outputFileName = NULL;
        outputFile = NULL;
        fromEventNumber = -1;
        toEventNumber = -1;
        outputInitialization = true;
        traceBackward = true;
        traceForward = true;
        eventNumbers = NULL;
        fileOffsets = NULL;
        verbose = false;
    }
};

void readLines(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Reading lines from log file %s\n", options.inputFileName);

    char *line;
    LineTokenizer tokenizer;
    FileReader fileReader(options.inputFileName);

    long begin = clock();

    while (line = fileReader.readNextLine())
        tokenizer.tokenize(line, fileReader.getLastLineLength());

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Reading of %ld lines and %ld bytes from log file %s completed in %g seconds\n", fileReader.getNumReadLines(), fileReader.getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void loadEvents(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Loading events from log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLog eventLog(fileReader);
    long begin = clock();

    IEvent *event = eventLog.getFirstEvent();
    while (event)
        event = event->getNextEvent();

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Loading of %ld events, %ld lines and %ld bytes form log file %s completed in %g seconds\n", eventLog.getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void printOffsets(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Printing event offsets from log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLogIndex eventLogIndex(fileReader);

    long begin = clock();

    if (options.eventNumbers) {
        for (std::vector<long>::iterator it = options.eventNumbers->begin(); it != options.eventNumbers->end(); it++) {
            long offset = eventLogIndex.getOffsetForEventNumber(*it);

            if (options.verbose)
                fprintf(stdout, "# Event #%ld --> file offset %ld (0x%lx)\n", *it, offset, offset);

            fprintf(options.outputFile, "%ld\n", offset);

            if (offset != -1 && options.verbose) {
                fileReader->seekTo(offset);
                fprintf(stdout, "#  - line at that offset: %.*s\n", fileReader->getLastLineLength(), fileReader->readNextLine());
            }
        }
    }

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Printing offsets for %ld events while reading %ld lines and %ld bytes form log file %s completed in %g seconds\n", options.eventNumbers->size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void printEvents(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Printing events from log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLog eventLog(fileReader);

    long begin = clock();

    if (options.fileOffsets) {
        for (std::vector<long>::iterator it = options.fileOffsets->begin(); it != options.fileOffsets->end(); it++) {
            IEvent *event = eventLog.getEventForBeginOffset(*it);

            if (options.verbose)
                fprintf(stdout, "# Event #%ld found at file offset %ld (0x%lx)\n", event->getEventNumber(), *it, *it);

            event->print(options.outputFile);
        }
    }

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Printing events for %ld offsets while reading %ld lines and %ld bytes form log file %s completed in %g seconds\n", options.fileOffsets->size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void echo(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Echoing events from log file %s from event number %ld to %ld\n", options.inputFileName, options.fromEventNumber, options.toEventNumber);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLog eventLog(fileReader);

    long begin = clock();

    if (options.outputInitialization)
        eventLog.print(options.outputFile, options.fromEventNumber, options.toEventNumber);
    else
        eventLog.printEvents(options.outputFile, options.fromEventNumber, options.toEventNumber);

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Parsing of %ld events, %ld lines and %ld bytes form log file %s completed in %g seconds\n", eventLog.getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}
        
void consistency(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Checking consistency of log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLog eventLog(fileReader);

    // TODO: check for filter parameters and use filered event log if applicable

    long begin = clock();

    IEvent *event = options.fromEventNumber == -1 ? eventLog.getFirstEvent() : eventLog.getEventForEventNumber(options.fromEventNumber);

    while (event) {
        // check causes
        MessageDependencyList *causes = event->getCauses();

        for (MessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++) {
            MessageDependency *messageDependency = *it;
            IEvent *causeEvent = messageDependency->getCauseEvent();

            if (causeEvent) {
                bool foundCauseConsequence = false;
                MessageDependencyList *causeConseqences = causeEvent->getConsequences();

                for (MessageDependencyList::iterator ot = causeConseqences->begin(); ot != causeConseqences->end(); ot++) {
                    if ((*ot)->getConsequenceEvent() == event) {
                        foundCauseConsequence = true;
                        break;
                    }
                }

                if (!foundCauseConsequence)
                    fprintf(options.outputFile, "Consistency check failed, could not find event %ld in the consequences of event %ld which is included in the causes of event %ld\n",
                        event->getEventNumber(), causeEvent->getEventNumber(), event->getEventNumber());
            }
        }

        // check consequences
        MessageDependencyList *consequences = event->getConsequences();

        for (MessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++) {
            MessageDependency *messageDependency = *it;
            IEvent *consequenceEvent = messageDependency->getConsequenceEvent();

            if (consequenceEvent) {
                bool foundConsequenceCause = false;
                MessageDependencyList *consequenceCauses = consequenceEvent->getCauses();

                for (MessageDependencyList::iterator ot = consequenceCauses->begin(); ot != consequenceCauses->end(); ot++) {
                    if ((*ot)->getCauseEvent() == event) {
                        foundConsequenceCause = true;
                        break;
                    }
                }

                if (!foundConsequenceCause)
                    fprintf(options.outputFile, "Consistency check failed, could not find event %ld in the causes of event %ld which is included in the consequences of event %ld\n",
                        event->getEventNumber(), consequenceEvent->getEventNumber(), event->getEventNumber());
            }
        }

        if (event->getEventNumber() == options.toEventNumber)
            break;

        event = event->getNextEvent();
    }

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Checking consistency while loading of %ld events, %ld lines and %ld bytes form log file %s completed in %g seconds\n", eventLog.getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void filter(Options options)
{
    long tracedEventNumber = options.eventNumbers ? options.eventNumbers->at(0) : -1;

    if (options.verbose)
        fprintf(stdout, "# Filtering events from log file %s for traced event number %ld from event number %ld to event number %ld\n",
            options.inputFileName, tracedEventNumber, options.fromEventNumber, options.toEventNumber);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLog *eventLog = new EventLog(fileReader);
    FilteredEventLog filteredEventLog(eventLog, NULL, tracedEventNumber, options.traceBackward, options.traceForward, options.fromEventNumber, options.toEventNumber);

    long begin = clock();
    
    if (options.outputInitialization)
        filteredEventLog.print(options.outputFile);
    else
        filteredEventLog.printEvents(options.outputFile);
    
    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Parsing of %ld events, %ld lines and %ld bytes form log file %s completed in %g seconds\n", eventLog->getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}
        
void usage(char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);

    fprintf(stderr, ""
"Usage:\n"
"   eventlogtool <command> [options]* <input-file-name>\n"
"\n"
"   Commands:\n"
"      readlines   - reads the input lines one at a time and outputs nothing, options are ignored.\n"
"      loadevents  - loads the whole input file at once into memory and outputs nothing, options are ignored.\n"
"      offsets     - prints the file offsets for the given even numbers (-e) one per line, all other options are ignored.\n"
"      events      - prints the events for the given offsets (-f), all other options are ignored.\n"
"      echo        - echos the input to the output, range options are supported.\n"
"      consistency - checks the consistency of the requested region in the input file.\n"
"      filter      - filters the input according to the varios options, only one event number is traced, but\n"
"                    it may be outside of the specified event number or simulation time region.\n"
"\n"
"   Options: Not all options may be used for all commands. Some options optionally accept a list of\n"
"            space separated tokens as a single parameter. Name and type filters may include patterns.\n"
"      input-file-name                   <file-name>\n"
"      -o      --output                  <file-name>\n"
"         defaults to standard output\n"
"      -fe     --from-event-number       <integer>\n"
"         inclusive\n"
"      -te     --to-event-number         <integer>\n"
"         inclusive\n"
"      -ft     --from-simulation-time    <number>\n"
"         inclusive\n"
"      -tt     --to-simulation-time      <number>\n"
"         inclusive\n"
"      -e      --event-numbers           <integer>+\n"
"      -f      --file-offsets            <integer>+\n"
"      -ob     --omit-trace-backward\n"
"      -of     --omit-trace-forward\n"
"      -oi     --omit-initialization\n"
"      -mi     --module-ids              <integer>+\n"
"         compound module ids are allowed\n"
"      -mn     --module-names            <pattern>+\n"
"      -mt     --module-types            <pattern>+\n"
"      -sid    --message-ids             <integer>+\n"
"      -stid   --message-tids            <integer>+\n"
"      -seid   --message-eids            <integer>+\n"
"      -setid  --message-etids           <integer>+\n"
"      -sn     --message-names           <pattern>+\n"
"      -st     --message-types           <pattern>+\n"
"      -ol     --omit-log-lines\n"
"      -v      --verbose\n"
"         prints performance information\n");
}

int main(int argc, char **argv)
{
    if (argc < 3)
        usage("Not enough arguments specified");
    else {
        char *command = argv[1];
        Options options;
        LineTokenizer tokenizer;

        for (int i = 2; i < argc; i++) {
            if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output"))
                options.outputFileName = argv[++i];
            else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose"))
                options.verbose = true;
            else if (!strcmp(argv[i], "-fe") || !strcmp(argv[i], "--from-event-number"))
                options.fromEventNumber = atol(argv[++i]);
            else if (!strcmp(argv[i], "-te") || !strcmp(argv[i], "--to-event-number"))
                options.toEventNumber = atol(argv[++i]);
            else if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--event-numbers")) {
                i++;
                tokenizer.tokenize(argv[i], strlen(argv[i]));
                options.eventNumbers = new std::vector<long>();
                char **tokens = tokenizer.tokens();

                for (int j = 0; j < tokenizer.numTokens(); j++)
                    options.eventNumbers->push_back(atol(tokens[j]));
            }
            else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file-offsets")) {
                i++;
                tokenizer.tokenize(argv[i], strlen(argv[i]));
                options.fileOffsets = new std::vector<long>();
                char **tokens = tokenizer.tokens();

                for (int j = 0; j < tokenizer.numTokens(); j++)
                    options.fileOffsets->push_back(atol(tokens[j]));
            }
            else if (!strcmp(argv[i], "-ob") || !strcmp(argv[i], "--trace-backward"))
                options.traceBackward = false;
            else if (!strcmp(argv[i], "-of") || !strcmp(argv[i], "--trace-forward"))
                options.traceForward = false;
            else if (!strcmp(argv[i], "-oi") || !strcmp(argv[i], "--omit-initialization"))
                options.outputInitialization = false;
            else if (i == argc - 1)
                options.inputFileName = argv[i];

            // TODO: some options still not handled
        }

        if (!options.inputFileName)
            usage("No input file specified");
        else {
            if (options.outputFileName)
                options.outputFile = fopen(options.outputFileName, "w");
            else
                options.outputFile = stdout;

            try {
                if (!strcmp(command, "readlines"))
                    readLines(options);
                else if (!strcmp(command, "loadevents"))
                    loadEvents(options);
                else if (!strcmp(command, "offsets"))
                    printOffsets(options);
                else if (!strcmp(command, "events"))
                    printEvents(options);
                else if (!strcmp(command, "echo"))
                    echo(options);
                else if (!strcmp(command, "consistency"))
                    consistency(options);
                else if (!strcmp(command, "filter"))
                    filter(options);
                else
                    usage("Unknown or invalid command");
            }
            catch (Exception *e) {
                fprintf(stderr, "Error: %s\n", e->message());
            }

            if (options.outputFileName)
                fclose(options.outputFile);
        }
    }

    return 0;
}
