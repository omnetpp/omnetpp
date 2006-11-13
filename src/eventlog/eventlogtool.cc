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

class Options
{
    public:
        char *inputFileName;
        char *outputFileName;
        FILE *outputFile;

        long fromEventNumber;
        long toEventNumber;

        simtime_t fromSimulationTime;
        simtime_t toSimulationTime;

        bool outputInitialization;
        bool outputLogLines;
        bool traceBackward;
        bool traceForward;

        std::vector<file_offset_t> *fileOffsets;
        std::vector<long> *eventNumbers;

        std::vector<int> *moduleIds;
        std::vector<char *> *moduleNames;
        std::vector<char *> *moduleTypes;

        std::vector<long> *messageIds;
        std::vector<char *> *messageNames;
        std::vector<char *> *messageTypes;
        std::vector<long> *messageTids;
        std::vector<long> *messageEids;
        std::vector<long> *messageEtids;

        bool verbose;

    public:
        Options();
};

Options::Options()
{
    inputFileName = NULL;
    outputFileName = NULL;
    outputFile = NULL;

    fromEventNumber = -1;
    toEventNumber = -1;

    fromSimulationTime = -1;
    toSimulationTime = -1;

    outputInitialization = true;
    outputLogLines = true;
    traceBackward = true;
    traceForward = true;

    eventNumbers = NULL;
    fileOffsets = NULL;

    moduleIds = NULL;
    moduleNames = NULL;
    moduleTypes = NULL;

    messageIds = NULL;
    messageNames = NULL;
    messageTypes = NULL;
    messageTids = NULL;
    messageEids = NULL;
    messageEtids = NULL;

    verbose = false;
}

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
        fprintf(stdout, "# Reading of %lld lines and %lld bytes from log file %s completed in %g seconds\n", fileReader.getNumReadLines(), fileReader.getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
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
        fprintf(stdout, "# Loading of %ld events, %lld lines and %lld bytes form log file %s completed in %g seconds\n", eventLog.getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void offsets(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Printing event offsets from log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLogIndex eventLogIndex(fileReader);

    long begin = clock();

    if (options.eventNumbers) {
        for (std::vector<long>::iterator it = options.eventNumbers->begin(); it != options.eventNumbers->end(); it++) {
            file_offset_t offset = eventLogIndex.getOffsetForEventNumber(*it);

            if (options.verbose)
                fprintf(stdout, "# Event #%ld --> file offset %lld (0x%llx)\n", *it, offset, offset);

            if (offset != -1 && options.verbose) {
                fileReader->seekTo(offset);
                fprintf(stdout, "#  - line at that offset: %.*s", fileReader->getLastLineLength(), fileReader->readNextLine());
            }

            fprintf(options.outputFile, "%lld\n", offset);
        }
    }

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Printing offsets for %ld events while reading %lld lines and %lld bytes form log file %s completed in %g seconds\n", options.eventNumbers->size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void events(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Printing events from log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLog eventLog(fileReader);

    long begin = clock();

    if (options.fileOffsets) {
        for (std::vector<file_offset_t>::iterator it = options.fileOffsets->begin(); it != options.fileOffsets->end(); it++) {
            IEvent *event = eventLog.getEventForBeginOffset(*it);

            if (options.verbose)
                fprintf(stdout, "# Event #%ld found at file offset %lld (0x%llx)\n", event->getEventNumber(), *it, *it);

            event->print(options.outputFile);
        }
    }

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Printing events for %ld offsets while reading %lld lines and %lld bytes form log file %s completed in %g seconds\n", options.fileOffsets->size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void ranges(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Printing ranges from log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLog eventLog(fileReader);

    long begin = clock();

    IEvent *event = eventLog.getFirstEvent();
    IEvent *rangeFirstEvent = event;

    while (event) {
        IEvent *nextEvent = event->getNextEvent();

        if (!nextEvent || event->getEventNumber() != nextEvent->getEventNumber() - 1) {
            fprintf(stdout, "%ld -> %ld\n", rangeFirstEvent->getEventNumber(), event->getEventNumber());
            rangeFirstEvent = nextEvent;
        }

        event = nextEvent;
    }

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Printing ranges while reading %lld lines and %lld bytes form log file %s completed in %g seconds\n", options.fileOffsets->size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void echo(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Echoing events from log file %s from event number %ld to %ld\n", options.inputFileName, options.fromEventNumber, options.toEventNumber);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLog eventLog(fileReader);

    long begin = clock();
    eventLog.print(options.outputFile, options.fromEventNumber, options.toEventNumber, options.outputInitialization, options.outputLogLines);
    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Parsing of %ld events, %lld lines and %lld bytes form log file %s completed in %g seconds\n", eventLog.getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}
        
void consistency(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Checking consistency of log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    IEventLog *eventLog = new EventLog(fileReader);
    
    if (options.eventNumbers != NULL)
        eventLog = new FilteredEventLog(eventLog, NULL, options.eventNumbers->at(0), options.traceBackward, options.traceForward, options.fromEventNumber, options.toEventNumber);

    long begin = clock();

    IEvent *event = options.fromEventNumber == -1 ? eventLog->getFirstEvent() : eventLog->getEventForEventNumber(options.fromEventNumber);

    while (event) {
        if (options.verbose)
            fprintf(stdout, "# Checking consistency for event %ld\n", event->getEventNumber());

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
        fprintf(stdout, "# Checking consistency while loading of %ld events, %lld lines and %lld bytes form log file %s completed in %g seconds\n", eventLog->getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);

    delete eventLog;
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
    filteredEventLog.print(options.outputFile, options.outputInitialization, options.outputLogLines);
    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Parsing of %ld events, %lld lines and %lld bytes form log file %s completed in %g seconds\n", eventLog->getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
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
"      ranges      - prints the event number ranges found in the input, all other options are ignored.\n"
"      echo        - echos the input to the output, range options are supported.\n"
"      consistency - checks the consistency of the requested region in the input file.\n"
"      filter      - filters the input according to the varios options and outputs the result, only one event number is traced,\n"
"                    but it may be outside of the specified event number or simulation time region.\n"
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

std::vector<int> *parseIntTokens(char *parameter)
{
    LineTokenizer tokenizer;
    tokenizer.tokenize(parameter, strlen(parameter));
    char **tokens = tokenizer.tokens();

    std::vector<int> *result = new std::vector<int>();
    for (int j = 0; j < tokenizer.numTokens(); j++)
        result->push_back(atoi(tokens[j]));

    return result;
}

std::vector<long> *parseLongTokens(char *parameter)
{
    LineTokenizer tokenizer;
    tokenizer.tokenize(parameter, strlen(parameter));
    char **tokens = tokenizer.tokens();

    std::vector<long> *result = new std::vector<long>();
    for (int j = 0; j < tokenizer.numTokens(); j++)
        result->push_back(atol(tokens[j]));

    return result;
}

std::vector<file_offset_t> *parseFileOffsetTokens(char *parameter)
{
    LineTokenizer tokenizer;
    tokenizer.tokenize(parameter, strlen(parameter));
    char **tokens = tokenizer.tokens();

    std::vector<file_offset_t> *result = new std::vector<file_offset_t>();
    for (int j = 0; j < tokenizer.numTokens(); j++)
        result->push_back(atol(tokens[j]));

    return result;
}

std::vector<char *> *parseStringTokens(char *parameter)
{
    LineTokenizer tokenizer;
    tokenizer.tokenize(parameter, strlen(parameter));
    char **tokens = tokenizer.tokens();

    std::vector<char *> *result = new std::vector<char *>();
    for (int j = 0; j < tokenizer.numTokens(); j++)
        result->push_back(tokens[j]);

    return result;
}

int main(int argc, char **argv)
{
    if (argc < 3)
        usage("Not enough arguments specified");
    else {
        char *command = argv[1];
        Options options;

        for (int i = 2; i < argc; i++) {
            if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output"))
                options.outputFileName = argv[++i];
            else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose"))
                options.verbose = true;
            else if (!strcmp(argv[i], "-fe") || !strcmp(argv[i], "--from-event-number"))
                options.fromEventNumber = atol(argv[++i]);
            else if (!strcmp(argv[i], "-te") || !strcmp(argv[i], "--to-event-number"))
                options.toEventNumber = atol(argv[++i]);
            else if (!strcmp(argv[i], "-ft") || !strcmp(argv[i], "--from-simulation-number"))
                options.fromSimulationTime = atof(argv[++i]);
            else if (!strcmp(argv[i], "-tt") || !strcmp(argv[i], "--to-simulation-number"))
                options.toSimulationTime = atof(argv[++i]);
            else if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--event-numbers"))
                options.eventNumbers = parseLongTokens(argv[++i]);
            else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file-offsets"))
                options.fileOffsets = parseFileOffsetTokens(argv[++i]);
            else if (!strcmp(argv[i], "-mi") || !strcmp(argv[i], "--module-ids"))
                options.moduleIds = parseIntTokens(argv[++i]);
            else if (!strcmp(argv[i], "-mi") || !strcmp(argv[i], "--module-names"))
                options.moduleNames = parseStringTokens(argv[++i]);
            else if (!strcmp(argv[i], "-mi") || !strcmp(argv[i], "--module-types"))
                options.moduleTypes = parseStringTokens(argv[++i]);
            else if (!strcmp(argv[i], "-mi") || !strcmp(argv[i], "--message-ids"))
                options.messageIds = parseLongTokens(argv[++i]);
            else if (!strcmp(argv[i], "-mi") || !strcmp(argv[i], "--message-tids"))
                options.messageTids = parseLongTokens(argv[++i]);
            else if (!strcmp(argv[i], "-mi") || !strcmp(argv[i], "--message-eids"))
                options.messageEids = parseLongTokens(argv[++i]);
            else if (!strcmp(argv[i], "-mi") || !strcmp(argv[i], "--message-etids"))
                options.messageEtids = parseLongTokens(argv[++i]);
            else if (!strcmp(argv[i], "-ob") || !strcmp(argv[i], "--trace-backward"))
                options.traceBackward = false;
            else if (!strcmp(argv[i], "-of") || !strcmp(argv[i], "--trace-forward"))
                options.traceForward = false;
            else if (!strcmp(argv[i], "-oi") || !strcmp(argv[i], "--omit-initialization"))
                options.outputInitialization = false;
            else if (!strcmp(argv[i], "-ol") || !strcmp(argv[i], "--omit-log-lines"))
                options.outputLogLines = false;
            else if (i == argc - 1)
                options.inputFileName = argv[i];
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
                    offsets(options);
                else if (!strcmp(command, "events"))
                    events(options);
                else if (!strcmp(command, "ranges"))
                    ranges(options);
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
