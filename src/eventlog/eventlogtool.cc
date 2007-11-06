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
#include "../utils/ver.h"
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

        long firstEventNumber;
        long lastEventNumber;

        long fromEventNumber;
        long toEventNumber;

        simtime_t fromSimulationTime;
        simtime_t toSimulationTime;

        bool outputInitialization;
        bool outputLogLines;
        bool traceCauses;
        bool traceConsequences;

        std::vector<file_offset_t> fileOffsets;
        std::vector<long> eventNumbers;

        const char *moduleExpression;
        std::vector<std::string> moduleNames;
        std::vector<std::string> moduleClassNames;
        std::vector<int> moduleIds;

        const char *messageExpression;
        std::vector<std::string> messageNames;
        std::vector<std::string> messageClassNames;
        std::vector<long> messageIds;
        std::vector<long> messageTreeIds;
        std::vector<long> messageEncapsulationIds;
        std::vector<long> messageEncapsulationTreeIds;

        bool verbose;

    public:
        Options();

        IEventLog *createEventLog(FileReader *fileReader);
        void deleteEventLog(IEventLog *eventLog);
        long getFirstEventNumber();
        long getLastEventNumber();
};

Options::Options()
{
    inputFileName = NULL;
    outputFileName = NULL;
    outputFile = NULL;

    moduleExpression = NULL;
    messageExpression = NULL;

    firstEventNumber = -2;
    lastEventNumber = -2;

    fromEventNumber = -1;
    toEventNumber = -1;

    fromSimulationTime = -1;
    toSimulationTime = -1;

    outputInitialization = true;
    outputLogLines = true;
    traceCauses = true;
    traceConsequences = true;

    verbose = false;
}

IEventLog *Options::createEventLog(FileReader *fileReader)
{
    if (eventNumbers.empty() &&
        moduleNames.empty() && moduleClassNames.empty() && moduleIds.empty() &&
        messageNames.empty() && messageClassNames.empty() &&
        messageIds.empty() && messageTreeIds.empty() && messageEncapsulationIds.empty() && messageEncapsulationTreeIds.empty())
    {
        return new EventLog(fileReader);
    }
    else
    {
        FilteredEventLog *filteredEventLog = new FilteredEventLog(new EventLog(fileReader));

        if (!eventNumbers.empty())
            filteredEventLog->setTracedEventNumber(eventNumbers.at(0));

        filteredEventLog->setEnableModuleFilter(moduleExpression || !moduleNames.empty() || !moduleClassNames.empty() || !moduleIds.empty());
        filteredEventLog->setModuleExpression(moduleExpression);
        filteredEventLog->setModuleNames(moduleNames);
        filteredEventLog->setModuleClassNames(moduleClassNames);
        filteredEventLog->setModuleIds(moduleIds);

        filteredEventLog->setEnableMessageFilter(messageExpression || !messageNames.empty() || !messageClassNames.empty() || !messageIds.empty() || !messageTreeIds.empty() || !messageEncapsulationIds.empty() || !messageEncapsulationTreeIds.empty());
        filteredEventLog->setMessageExpression(messageExpression);
        filteredEventLog->setMessageNames(messageNames);
        filteredEventLog->setMessageClassNames(messageClassNames);
        filteredEventLog->setMessageIds(messageIds);
        filteredEventLog->setMessageTreeIds(messageTreeIds);
        filteredEventLog->setMessageEncapsulationIds(messageEncapsulationIds);
        filteredEventLog->setMessageEncapsulationTreeIds(messageEncapsulationTreeIds);

        filteredEventLog->setTraceCauses(traceCauses);
        filteredEventLog->setTraceConsequences(traceConsequences);
        filteredEventLog->setFirstEventNumber(getFirstEventNumber());
        filteredEventLog->setLastEventNumber(getLastEventNumber());

        return filteredEventLog;
    }
}

void Options::deleteEventLog(IEventLog *eventLog)
{
    FilteredEventLog *filteredEventLog = dynamic_cast<FilteredEventLog *>(eventLog);

    if (filteredEventLog)
        delete filteredEventLog->getEventLog();

    delete eventLog;
}

long Options::getFirstEventNumber()
{
    if (firstEventNumber == -2) {
        FileReader *fileReader = new FileReader(inputFileName);
        EventLog eventLog(fileReader);

        firstEventNumber = -1;

        if (fromEventNumber != -1)
            firstEventNumber = fromEventNumber;
        else if (fromSimulationTime != -1) {
            IEvent *event = eventLog.getEventForSimulationTime(fromSimulationTime, FIRST_OR_NEXT);

            if (event)
                firstEventNumber = event->getEventNumber();
            else
                firstEventNumber = LONG_MAX;
        }
    }

    return firstEventNumber;
}

long Options::getLastEventNumber()
{
    if (lastEventNumber == -2) {
        FileReader *fileReader = new FileReader(inputFileName);
        EventLog eventLog(fileReader);

        lastEventNumber = -1;

        if (toEventNumber != -1)
            lastEventNumber = toEventNumber;
        else if (toSimulationTime != -1) {
            IEvent *event = eventLog.getEventForSimulationTime(toSimulationTime, LAST_OR_PREVIOUS);

            if (event)
                lastEventNumber = event->getEventNumber();
            else
                lastEventNumber = -LONG_MAX;
        }
    }

    return lastEventNumber;
}

void offsets(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Printing event offsets from log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLogIndex eventLogIndex(fileReader);

    long begin = clock();

    if (!options.eventNumbers.empty()) {
        for (std::vector<long>::iterator it = options.eventNumbers.begin(); it != options.eventNumbers.end(); it++) {
            file_offset_t offset = eventLogIndex.getOffsetForEventNumber(*it);

            if (options.verbose)
                fprintf(stdout, "# Event #%ld --> file offset %lld (0x%llx)\n", *it, offset, offset);

            if (offset != -1 && options.verbose) {
                fileReader->seekTo(offset);
                fprintf(stdout, "#  - line at that offset: %.*s", fileReader->getLastLineLength(), fileReader->getNextLineBufferPointer());
            }

            fprintf(options.outputFile, "%lld\n", offset);
        }
    }

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Printing offsets for %d events while reading %lld lines and %lld bytes form log file %s completed in %g seconds\n", (int)options.eventNumbers.size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void events(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Printing events from log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLog eventLog(fileReader);

    long begin = clock();

    if (!options.fileOffsets.empty()) {
        for (std::vector<file_offset_t>::iterator it = options.fileOffsets.begin(); it != options.fileOffsets.end(); it++) {
            IEvent *event = eventLog.getEventForBeginOffset(*it);

            if (options.verbose)
                fprintf(stdout, "# Event #%ld found at file offset %lld (0x%llx)\n", event->getEventNumber(), *it, *it);

            event->print(options.outputFile);
        }
    }

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Printing events for %d offsets while reading %lld lines and %lld bytes form log file %s completed in %g seconds\n", (int)options.fileOffsets.size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void ranges(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Printing coherent ranges from log file %s\n", options.inputFileName);

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
        fprintf(stdout, "# Printing coherent ranges while reading %lld lines and %lld bytes form log file %s completed in %g seconds\n", fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void echo(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Echoing events from log file %s from event number %ld to event number %ld\n", options.inputFileName, options.getFirstEventNumber(), options.getLastEventNumber());

    FileReader *fileReader = new FileReader(options.inputFileName);
    IEventLog *eventLog = options.createEventLog(fileReader);

    long begin = clock();
    eventLog->print(options.outputFile, options.getFirstEventNumber(), options.getLastEventNumber(), options.outputInitialization, options.outputLogLines);
    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Echoing of %ld events, %lld lines and %lld bytes form log file %s completed in %g seconds\n", eventLog->getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);

    options.deleteEventLog(eventLog);
}

void filter(Options options)
{
    long tracedEventNumber = options.eventNumbers.empty() ? -1 : options.eventNumbers.at(0);

    if (options.verbose)
        fprintf(stdout, "# Filtering events from log file %s for traced event number %ld from event number %ld to event number %ld\n",
            options.inputFileName, tracedEventNumber, options.getFirstEventNumber(), options.getLastEventNumber());

    FileReader *fileReader = new FileReader(options.inputFileName);
    IEventLog *eventLog = options.createEventLog(fileReader);

    long begin = clock();
    eventLog->print(options.outputFile, -1, -1, options.outputInitialization, options.outputLogLines);
    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Filtering of %ld events, %lld lines and %lld bytes form log file %s completed in %g seconds\n", eventLog->getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);

    options.deleteEventLog(eventLog);
}

void usage(char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);

    fprintf(stderr, ""
"eventlogtool -- part of OMNeT++/OMNEST, (C) 2006 Andras Varga\n"
"Release: " OMNETPP_RELEASE ", edition: " OMNETPP_EDITION ".\n"
"\n"
"Usage:\n"
"   eventlogtool <command> [options]* <input-file-name>\n"
"\n"
"   Commands:\n"
"      offsets     - prints the file offsets for the given even numbers (-e) one per line, all other options are ignored.\n"
"      events      - prints the events for the given offsets (-f), all other options are ignored.\n"
"      ranges      - prints the coherent event number ranges found in the input as event number pairs, all other options are ignored.\n"
"      echo        - echos the input to the output, range options are supported.\n"
"      filter      - filters the input according to the varios options and outputs the result, only one event number is traced,\n"
"                    but it may be outside of the specified event number or simulation time region.\n"
"\n"
"   Options: Not all options may be used for all commands. Some options optionally accept a list of\n"
"            space separated tokens as a single parameter. Name and class name filters may include patterns.\n"
"      input-file-name                            <file-name>\n"
"      -o      --output                           <file-name>\n"
"         defaults to standard output\n"
"      -fe     --from-event-number                <integer>\n"
"         inclusive\n"
"      -te     --to-event-number                  <integer>\n"
"         inclusive\n"
"      -ft     --from-simulation-time             <number>\n"
"         inclusive\n"
"      -tt     --to-simulation-time               <number>\n"
"         inclusive\n"
"      -e      --event-numbers                    <integer>+\n"
"         events must be present in the input file\n"
"      -f      --file-offsets                     <integer>+\n"
"      -mn     --module-names                     <pattern>+\n"
"      -mt     --module-class-names               <pattern>+\n"
"      -mi     --module-ids                       <integer>+\n"
"         compound module ids are allowed\n"
"      -sn     --message-names                    <pattern>+\n"
"      -st     --message-class-names              <pattern>+\n"
"      -si     --message-ids                      <integer>+\n"
"      -sti    --message-tree-ids                 <integer>+\n"
"      -sei    --message-encapsulation-ids        <integer>+\n"
"      -seti   --message-encapsulation-tree-ids   <integer>+\n"
"      -ob     --omit-causes-trace\n"
"      -of     --omit-consequences-trace\n"
"      -oi     --omit-initialization\n"
"      -ol     --omit-log-lines\n"
"      -v      --verbose\n"
"         prints performance information\n");
}

void parseIntTokens(std::vector<int> &parameter, char *str)
{
    LineTokenizer tokenizer;
    tokenizer.tokenize(str, strlen(str));
    char **tokens = tokenizer.tokens();

    for (int j = 0; j < tokenizer.numTokens(); j++)
        parameter.push_back(atoi(tokens[j]));
}

void parseLongTokens(std::vector<long> &parameter, char *str)
{
    LineTokenizer tokenizer;
    tokenizer.tokenize(str, strlen(str));
    char **tokens = tokenizer.tokens();

    for (int j = 0; j < tokenizer.numTokens(); j++)
        parameter.push_back(atol(tokens[j]));
}

void parseFileOffsetTokens(std::vector<file_offset_t> &parameter, char *str)
{
    LineTokenizer tokenizer;
    tokenizer.tokenize(str, strlen(str));
    char **tokens = tokenizer.tokens();

    for (int j = 0; j < tokenizer.numTokens(); j++)
        parameter.push_back(atol(tokens[j]));
}

void parseStringTokens(std::vector<std::string> &parameter, char *str)
{
    LineTokenizer tokenizer;
    tokenizer.tokenize(str, strlen(str));
    char **tokens = tokenizer.tokens();

    for (int j = 0; j < tokenizer.numTokens(); j++)
        parameter.push_back((char *)eventLogStringPool.get(tokens[j]));
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
            else if (!strcmp(argv[i], "-ft") || !strcmp(argv[i], "--from-simulation-time"))
                options.fromSimulationTime = BigDecimal::parse(argv[++i]);
            else if (!strcmp(argv[i], "-tt") || !strcmp(argv[i], "--to-simulation-time"))
                options.toSimulationTime = BigDecimal::parse(argv[++i]);
            else if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--event-numbers"))
                parseLongTokens(options.eventNumbers, argv[++i]);
            else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file-offsets"))
                parseFileOffsetTokens(options.fileOffsets, argv[++i]);
            else if (!strcmp(argv[i], "-me") || !strcmp(argv[i], "--module-expression"))
                options.moduleExpression = argv[++i];
            else if (!strcmp(argv[i], "-mn") || !strcmp(argv[i], "--module-names"))
                parseStringTokens(options.moduleNames, argv[++i]);
            else if (!strcmp(argv[i], "-mt") || !strcmp(argv[i], "--module-class-names"))
                parseStringTokens(options.moduleClassNames, argv[++i]);
            else if (!strcmp(argv[i], "-mi") || !strcmp(argv[i], "--module-ids"))
                parseIntTokens(options.moduleIds, argv[++i]);
            else if (!strcmp(argv[i], "-sn") || !strcmp(argv[i], "--message-expression"))
                options.messageExpression = argv[++i];
            else if (!strcmp(argv[i], "-sn") || !strcmp(argv[i], "--message-names"))
                parseStringTokens(options.messageNames, argv[++i]);
            else if (!strcmp(argv[i], "-st") || !strcmp(argv[i], "--message-class-names"))
                parseStringTokens(options.messageClassNames, argv[++i]);
            else if (!strcmp(argv[i], "-si") || !strcmp(argv[i], "--message-ids"))
                parseLongTokens(options.messageIds, argv[++i]);
            else if (!strcmp(argv[i], "-sti") || !strcmp(argv[i], "--message-tree-ids"))
                parseLongTokens(options.messageTreeIds, argv[++i]);
            else if (!strcmp(argv[i], "-sei") || !strcmp(argv[i], "--message-encapsulation-ids"))
                parseLongTokens(options.messageEncapsulationIds, argv[++i]);
            else if (!strcmp(argv[i], "-seti") || !strcmp(argv[i], "--message-encapsulation-tree-ids"))
                parseLongTokens(options.messageEncapsulationTreeIds, argv[++i]);
            else if (!strcmp(argv[i], "-ob") || !strcmp(argv[i], "--omit-causes-trace"))
                options.traceCauses = false;
            else if (!strcmp(argv[i], "-of") || !strcmp(argv[i], "--omit-consequences-trace"))
                options.traceConsequences = false;
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
                if (!strcmp(command, "offsets"))
                    offsets(options);
                else if (!strcmp(command, "events"))
                    events(options);
                else if (!strcmp(command, "ranges"))
                    ranges(options);
                else if (!strcmp(command, "echo"))
                    echo(options);
                else if (!strcmp(command, "filter"))
                    filter(options);
                else
                    usage("Unknown or invalid command");
            }
            catch (std::exception& e) {
                fprintf(stderr, "Error: %s\n", e.what());
            }

            if (options.outputFileName)
                fclose(options.outputFile);
        }
    }

    return 0;
}
