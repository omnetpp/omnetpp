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

        std::vector<const char *> moduleNames;
        std::vector<const char *> moduleTypes;
        std::vector<int> moduleIds;

        std::vector<const char *> messageNames;
        std::vector<const char *> messageTypes;
        std::vector<long> messageIds;
        std::vector<long> messageTids;
        std::vector<long> messageEids;
        std::vector<long> messageEtids;

        bool verbose;

    public:
        Options();

        IEventLog *getEventLog(FileReader *fileReader);
        long getFirstEventNumber();
        long getLastEventNumber();
};

Options::Options()
{
    inputFileName = NULL;
    outputFileName = NULL;
    outputFile = NULL;

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

IEventLog *Options::getEventLog(FileReader *fileReader)
{
    if (eventNumbers.empty() &&
        moduleNames.empty() && moduleTypes.empty() && moduleIds.empty() &&
        messageNames.empty() && messageTypes.empty() &&
        messageIds.empty() && messageTids.empty() && messageEids.empty() && messageEtids.empty())
    {
        return new EventLog(fileReader);
    }
    else
    {
        FilteredEventLog *filteredEventLog = new FilteredEventLog(new EventLog(fileReader));

        if (!eventNumbers.empty())
            filteredEventLog->setTracedEventNumber(eventNumbers.at(0));

        filteredEventLog->setModuleNames(moduleNames);
        filteredEventLog->setModuleTypes(moduleTypes);
        filteredEventLog->setModuleIds(moduleIds);

        filteredEventLog->setMessageNames(messageNames);
        filteredEventLog->setMessageTypes(messageTypes);
        filteredEventLog->setMessageIds(messageIds);
        filteredEventLog->setMessageTids(messageTids);
        filteredEventLog->setMessageEids(messageEids);
        filteredEventLog->setMessageEtids(messageEtids);

        filteredEventLog->setTraceCauses(traceCauses);
        filteredEventLog->setTraceConsequences(traceConsequences);
        filteredEventLog->setFirstEventNumber(getFirstEventNumber());
        filteredEventLog->setLastEventNumber(getLastEventNumber());

        return filteredEventLog ;
    }
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

    if (!options.eventNumbers.empty()) {
        for (std::vector<long>::iterator it = options.eventNumbers.begin(); it != options.eventNumbers.end(); it++) {
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
        fprintf(stdout, "# Printing offsets for %ld events while reading %lld lines and %lld bytes form log file %s completed in %g seconds\n", options.eventNumbers.size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
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
        fprintf(stdout, "# Printing events for %ld offsets while reading %lld lines and %lld bytes form log file %s completed in %g seconds\n", options.fileOffsets.size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
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
        fprintf(stdout, "# Printing ranges while reading %lld lines and %lld bytes form log file %s completed in %g seconds\n", options.fileOffsets.size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void echo(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Echoing events from log file %s from event number %ld to event number %ld\n", options.inputFileName, options.getFirstEventNumber(), options.getLastEventNumber());

    FileReader *fileReader = new FileReader(options.inputFileName);
    IEventLog *eventLog = options.getEventLog(fileReader);

    long begin = clock();
    eventLog->print(options.outputFile, options.getFirstEventNumber(), options.getLastEventNumber(), options.outputInitialization, options.outputLogLines);
    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Parsing of %ld events, %lld lines and %lld bytes form log file %s completed in %g seconds\n", eventLog->getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);

    delete eventLog;
}

void consistency(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Checking consistency of log file %s from event number %ld to event number %ld\n", options.inputFileName, options.getFirstEventNumber(), options.getLastEventNumber());

    FileReader *fileReader = new FileReader(options.inputFileName);
    IEventLog *eventLog = options.getEventLog(fileReader);

    long begin = clock();

    IEvent *event = options.getFirstEventNumber() == -1 ? eventLog->getFirstEvent() : eventLog->getEventForEventNumber(options.getFirstEventNumber());

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

        if (event->getEventNumber() == options.getLastEventNumber())
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
    long tracedEventNumber = options.eventNumbers.empty() ? -1 : options.eventNumbers.at(0);

    if (options.verbose)
        fprintf(stdout, "# Filtering events from log file %s for traced event number %ld from event number %ld to event number %ld\n",
            options.inputFileName, tracedEventNumber, options.getFirstEventNumber(), options.getLastEventNumber());

    FileReader *fileReader = new FileReader(options.inputFileName);
    IEventLog *eventLog = options.getEventLog(fileReader);

    long begin = clock();
    eventLog->print(options.outputFile, -1, -1, options.outputInitialization, options.outputLogLines);
    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Parsing of %ld events, %lld lines and %lld bytes form log file %s completed in %g seconds\n", eventLog->getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);

    delete eventLog;
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
"         events must be present in the input file"
"      -f      --file-offsets            <integer>+\n"
"      -mn     --module-names            <pattern>+\n"
"      -mt     --module-types            <pattern>+\n"
"      -mi     --module-ids              <integer>+\n"
"         compound module ids are allowed\n"
"      -sn     --message-names           <pattern>+\n"
"      -st     --message-types           <pattern>+\n"
"      -si     --message-ids             <integer>+\n"
"      -sti    --message-tids            <integer>+\n"
"      -sei    --message-eids            <integer>+\n"
"      -seti   --message-etids           <integer>+\n"
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

void parseStringTokens(std::vector<const char *> &parameter, char *str)
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
                options.fromSimulationTime = atof(argv[++i]);
            else if (!strcmp(argv[i], "-tt") || !strcmp(argv[i], "--to-simulation-time"))
                options.toSimulationTime = atof(argv[++i]);
            else if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--event-numbers"))
                parseLongTokens(options.eventNumbers, argv[++i]);
            else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file-offsets"))
                parseFileOffsetTokens(options.fileOffsets, argv[++i]);
            else if (!strcmp(argv[i], "-mn") || !strcmp(argv[i], "--module-names"))
                parseStringTokens(options.moduleNames, argv[++i]);
            else if (!strcmp(argv[i], "-mt") || !strcmp(argv[i], "--module-types"))
                parseStringTokens(options.moduleTypes, argv[++i]);
            else if (!strcmp(argv[i], "-mi") || !strcmp(argv[i], "--module-ids"))
                parseIntTokens(options.moduleIds, argv[++i]);
            else if (!strcmp(argv[i], "-sn") || !strcmp(argv[i], "--message-names"))
                parseStringTokens(options.messageNames, argv[++i]);
            else if (!strcmp(argv[i], "-st") || !strcmp(argv[i], "--message-types"))
                parseStringTokens(options.messageTypes, argv[++i]);
            else if (!strcmp(argv[i], "-si") || !strcmp(argv[i], "--message-ids"))
                parseLongTokens(options.messageIds, argv[++i]);
            else if (!strcmp(argv[i], "-sti") || !strcmp(argv[i], "--message-tids"))
                parseLongTokens(options.messageTids, argv[++i]);
            else if (!strcmp(argv[i], "-sei") || !strcmp(argv[i], "--message-eids"))
                parseLongTokens(options.messageEids, argv[++i]);
            else if (!strcmp(argv[i], "-seti") || !strcmp(argv[i], "--message-etids"))
                parseLongTokens(options.messageEtids, argv[++i]);
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
            catch (Exception& e) {
                fprintf(stderr, "Error: %s\n", e.what());
            }

            if (options.outputFileName)
                fclose(options.outputFile);
        }
    }

    return 0;
}
