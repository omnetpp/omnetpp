//=========================================================================
//  OPP_EVENTLOGTOOL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctime>
#include "common/ver.h"
#include "common/filereader.h"
#include "common/linetokenizer.h"
#include "omnetpp/platdep/platmisc.h"
#include "eventlogindex.h"
#include "eventlog.h"
#include "filteredeventlog.h"

#if defined(__MINGW32__)
int _CRT_glob = 0;  // Turn off runtime file globbing support on MinGW. The shell already handles file globbing on the command line.
#endif

using namespace omnetpp::common;

namespace omnetpp {
namespace eventlog {

class Options
{
    public:
        char *inputFileName = nullptr;
        char *outputFileName = nullptr;
        FILE *outputFile = nullptr;

        eventnumber_t firstEventNumber = -2;
        eventnumber_t lastEventNumber = -2;

        eventnumber_t fromEventNumber = -1;
        eventnumber_t toEventNumber = -1;

        simtime_t fromSimulationTime = simtime_nil;
        simtime_t toSimulationTime = simtime_nil;

        bool outputLogLines = true;
        bool traceCauses = true;
        bool traceConsequences = true;

        std::vector<file_offset_t> fileOffsets;
        std::vector<eventnumber_t> eventNumbers;

        const char *moduleExpression = nullptr;
        std::vector<std::string> moduleNames;
        std::vector<std::string> moduleClassNames;
        std::vector<std::string> moduleNedTypeNames;
        std::vector<int> moduleIds;

        const char *messageExpression = nullptr;
        std::vector<std::string> messageNames;
        std::vector<std::string> messageClassNames;
        std::vector<msgid_t> messageIds;
        std::vector<msgid_t> messageTreeIds;
        std::vector<msgid_t> messageEncapsulationIds;
        std::vector<msgid_t> messageEncapsulationTreeIds;

        bool verbose = false;

    public:
        IEventLog *createEventLog(FileReader *fileReader);
        void deleteEventLog(IEventLog *eventLog);
        eventnumber_t getFirstEventNumber();
        eventnumber_t getLastEventNumber();
};

IEventLog *Options::createEventLog(FileReader *fileReader)
{
    if (eventNumbers.empty() &&
        !moduleExpression && moduleNames.empty() && moduleClassNames.empty() && moduleNedTypeNames.empty() && moduleIds.empty() &&
        !messageExpression && messageNames.empty() && messageClassNames.empty() &&
        messageIds.empty() && messageTreeIds.empty() && messageEncapsulationIds.empty() && messageEncapsulationTreeIds.empty())
    {
        return new EventLog(fileReader);
    }
    else {
        FilteredEventLog *filteredEventLog = new FilteredEventLog(new EventLog(fileReader));

        if (!eventNumbers.empty())
            filteredEventLog->setTracedEventNumber(eventNumbers.at(0));

        filteredEventLog->setEnableModuleFilter(moduleExpression || !moduleNames.empty() || !moduleClassNames.empty() || !moduleNedTypeNames.empty() || !moduleIds.empty());
        filteredEventLog->setModuleExpression(moduleExpression);
        filteredEventLog->setModuleNames(moduleNames);
        filteredEventLog->setModuleClassNames(moduleClassNames);
        filteredEventLog->setModuleNedTypeNames(moduleNedTypeNames);
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
        filteredEventLog->setFirstConsideredEventNumber(getFirstEventNumber());
        filteredEventLog->setLastConsideredEventNumber(getLastEventNumber());

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

eventnumber_t Options::getFirstEventNumber()
{
    if (firstEventNumber == -2) {
        if (fromEventNumber != -1)
            firstEventNumber = fromEventNumber;
        else if (fromSimulationTime != simtime_nil) {
            FileReader *fileReader = new FileReader(inputFileName);
            EventLog eventLog(fileReader);
            IEvent *event = eventLog.getEventForSimulationTime(fromSimulationTime, FIRST_OR_NEXT);
            if (event)
                firstEventNumber = event->getEventNumber();
            else
                firstEventNumber = -1;
        }
        else
            firstEventNumber = -1;
    }

    return firstEventNumber;
}

eventnumber_t Options::getLastEventNumber()
{
    if (lastEventNumber == -2) {
        if (toEventNumber != -1)
            lastEventNumber = toEventNumber;
        else if (toSimulationTime != simtime_nil) {
            FileReader *fileReader = new FileReader(inputFileName);
            EventLog eventLog(fileReader);
            IEvent *event = eventLog.getEventForSimulationTime(toSimulationTime, LAST_OR_PREVIOUS);
            if (event)
                lastEventNumber = event->getEventNumber();
            else
                lastEventNumber = -1;
        }
        else
            lastEventNumber = -1;
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
        for (std::vector<eventnumber_t>::iterator it = options.eventNumbers.begin(); it != options.eventNumbers.end(); ++it) {
            file_offset_t offset = eventLogIndex.getOffsetForEventNumber(*it);

            if (options.verbose)
                fprintf(stdout, "# Event #%" EVENTNUMBER_PRINTF_FORMAT " --> file offset %" PRId64 " (0x%" PRId64 "x)\n", *it, offset, offset);

            if (offset != -1 && options.verbose) {
                fileReader->seekTo(offset);
                fprintf(stdout, "#  - line at that offset: %.*s", (int)fileReader->getCurrentLineLength(), fileReader->getNextLineBufferPointer());
            }

            fprintf(options.outputFile, "%" PRId64 "\n", offset);
        }
    }

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Printing offsets for %d events while reading %" PRId64 " lines and %" PRId64 " bytes from log file %s completed in %g seconds\n", (int)options.eventNumbers.size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void events(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Printing events from log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLog eventLog(fileReader);

    long begin = clock();

    if (!options.fileOffsets.empty()) {
        for (std::vector<file_offset_t>::iterator it = options.fileOffsets.begin(); it != options.fileOffsets.end(); ++it) {
            IEvent *event = eventLog.getEventForBeginOffset(*it);

            if (options.verbose)
                fprintf(stdout, "# Event #%" EVENTNUMBER_PRINTF_FORMAT " found at file offset %" PRId64 " (0x%" PRId64 "x)\n", event->getEventNumber(), *it, *it);

            event->print(options.outputFile);
        }
    }

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Printing events for %d offsets while reading %" PRId64 " lines and %" PRId64 " bytes from log file %s completed in %g seconds\n", (int)options.fileOffsets.size(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void ranges(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Printing continuous ranges from log file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);
    EventLog eventLog(fileReader);

    long begin = clock();

    IEvent *event = eventLog.getFirstEvent();
    IEvent *rangeFirstEvent = event;

    while (event) {
        IEvent *nextEvent = event->getNextEvent();

        if (!nextEvent || event->getEventNumber() != nextEvent->getEventNumber() - 1) {
            fprintf(stdout, "#%" EVENTNUMBER_PRINTF_FORMAT " -> #%" EVENTNUMBER_PRINTF_FORMAT "\n", rangeFirstEvent->getEventNumber(), event->getEventNumber());
            rangeFirstEvent = nextEvent;
        }

        event = nextEvent;
    }

    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Printing continuous ranges while reading %" PRId64 " lines and %" PRId64 " bytes from log file %s completed in %g seconds\n", fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void echo(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Echoing events from log file %s from event number #%" EVENTNUMBER_PRINTF_FORMAT " to event number #%" EVENTNUMBER_PRINTF_FORMAT "\n", options.inputFileName, options.getFirstEventNumber(), options.getLastEventNumber());

    FileReader *fileReader = new FileReader(options.inputFileName);
    IEventLog *eventLog = options.createEventLog(fileReader);

    long begin = clock();
    eventLog->print(options.outputFile, options.getFirstEventNumber(), options.getLastEventNumber(), options.outputLogLines);
    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Echoing of %" EVENTNUMBER_PRINTF_FORMAT " events, %" PRId64 " lines and %" PRId64 " bytes from log file %s completed in %g seconds\n", eventLog->getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);

    options.deleteEventLog(eventLog);
}

void cat(Options options)
{
    if (options.verbose)
        fprintf(stdout, "# Cating from file %s\n", options.inputFileName);

    FileReader *fileReader = new FileReader(options.inputFileName);

    long begin = clock();
    char *line;
    while ((line = fileReader->getNextLineBufferPointer()))
        fprintf(stdout, "%.*s", (int)fileReader->getCurrentLineLength(), line);
    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Cating of %" PRId64 " lines and %" PRId64 " bytes from log file %s completed in %g seconds\n", fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);
}

void filter(Options options)
{
    eventnumber_t tracedEventNumber = options.eventNumbers.empty() ? -1 : options.eventNumbers.at(0);

    if (options.verbose)
        fprintf(stdout, "# Filtering events from log file %s for traced event number #%" EVENTNUMBER_PRINTF_FORMAT " from event number #%" EVENTNUMBER_PRINTF_FORMAT " to event number #%" EVENTNUMBER_PRINTF_FORMAT "\n",
                options.inputFileName, tracedEventNumber, options.getFirstEventNumber(), options.getLastEventNumber());

    FileReader *fileReader = new FileReader(options.inputFileName);
    IEventLog *eventLog = options.createEventLog(fileReader);

    long begin = clock();
    eventLog->print(options.outputFile, -1, -1, options.outputLogLines);
    long end = clock();

    if (options.verbose)
        fprintf(stdout, "# Filtering of %" EVENTNUMBER_PRINTF_FORMAT " events, %" PRId64 " lines and %" PRId64 " bytes from log file %s completed in %g seconds\n", eventLog->getNumParsedEvents(), fileReader->getNumReadLines(), fileReader->getNumReadBytes(), options.inputFileName, (double)(end - begin) / CLOCKS_PER_SEC);

    options.deleteEventLog(eventLog);
}

void usage(const char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);

    fprintf(stderr, ""
"opp_eventlogtool -- part of " OMNETPP_PRODUCT ", (C) 2006-2018 OpenSim Ltd.\n"
"Version: " OMNETPP_VERSION_STR ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION "\n"
"\n"
"Usage:\n"
"   opp_eventlogtool <command> [options]* <input-file-name>\n"
"\n"
"   Commands:\n"
"      offsets     - prints the file offsets for the given even numbers (-e) one per line, all other options are ignored.\n"
"      events      - prints the events for the given offsets (-f), all other options are ignored.\n"
"      ranges      - prints the continuous event number ranges found in the input as event number pairs, all other options are ignored.\n"
"      echo        - echos the input to the output, range options are supported.\n"
"      filter      - filters the input according to the various options and outputs the result, only one event number is traced,\n"
"                    but it may be outside of the specified event number or simulation time range.\n"
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
"      -me     --module-expression                <pattern>\n"
"      -mn     --module-names                     <pattern>+\n"
"      -mt     --module-class-names               <pattern>+\n"
"      -mi     --module-ids                       <integer>+\n"
"         compound module ids are allowed\n"
"      -se     --message-expression               <pattern>\n"
"      -sn     --message-names                    <pattern>+\n"
"      -st     --message-class-names              <pattern>+\n"
"      -si     --message-ids                      <integer>+\n"
"      -sti    --message-tree-ids                 <integer>+\n"
"      -sei    --message-encapsulation-ids        <integer>+\n"
"      -seti   --message-encapsulation-tree-ids   <integer>+\n"
"      -ob     --omit-causes-trace\n"
"      -of     --omit-consequences-trace\n"
"      -ol     --omit-log-lines\n"
"      -v      --verbose\n"
"         prints performance information\n");
}

void parseIntTokens(std::vector<int>& parameter, char *str)
{
    LineTokenizer tokenizer;
    tokenizer.tokenize(str, strlen(str));
    char **tokens = tokenizer.tokens();

    for (int j = 0; j < tokenizer.numTokens(); j++)
        parameter.push_back(atoi(tokens[j]));
}

void parseLongTokens(std::vector<msgid_t>& parameter, char *str)
{
    char *e;
    LineTokenizer tokenizer;
    tokenizer.tokenize(str, strlen(str));
    char **tokens = tokenizer.tokens();

    for (int j = 0; j < tokenizer.numTokens(); j++)
        parameter.push_back(strtoll(tokens[j], &e, 10));
}

void parseEventNumberTokens(std::vector<eventnumber_t>& parameter, char *str)
{
    char *e;
    LineTokenizer tokenizer;
    tokenizer.tokenize(str, strlen(str));
    char **tokens = tokenizer.tokens();

    for (int j = 0; j < tokenizer.numTokens(); j++)
        parameter.push_back(strtoll(tokens[j], &e, 10));
}

void parseFileOffsetTokens(std::vector<file_offset_t>& parameter, char *str)
{
    char *e;
    LineTokenizer tokenizer;
    tokenizer.tokenize(str, strlen(str));
    char **tokens = tokenizer.tokens();

    for (int j = 0; j < tokenizer.numTokens(); j++)
        parameter.push_back(strtoll(tokens[j], &e, 10));
}

void parseStringTokens(std::vector<std::string>& parameter, char *str)
{
    LineTokenizer tokenizer;
    tokenizer.tokenize(str, strlen(str));
    char **tokens = tokenizer.tokens();

    for (int j = 0; j < tokenizer.numTokens(); j++)
        parameter.push_back(tokens[j]);
}

}  // namespace eventlog
}  // namespace omnetpp

using namespace omnetpp::eventlog;

int main(int argc, char **argv)
{
    try {
        if (argc < 3)
            usage("Not enough arguments specified");
        else {
            char *e;
            char *command = argv[1];
            Options options;

            for (int i = 2; i < argc; i++) {
                try {
                    if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output"))
                        options.outputFileName = argv[++i];
                    else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose"))
                        options.verbose = true;
                    else if (!strcmp(argv[i], "-fe") || !strcmp(argv[i], "--from-event-number"))
                        options.fromEventNumber = strtoll(argv[++i], &e, 10);
                    else if (!strcmp(argv[i], "-te") || !strcmp(argv[i], "--to-event-number"))
                        options.toEventNumber = strtoll(argv[++i], &e, 10);
                    else if (!strcmp(argv[i], "-ft") || !strcmp(argv[i], "--from-simulation-time"))
                        options.fromSimulationTime = BigDecimal::parse(argv[++i]);
                    else if (!strcmp(argv[i], "-tt") || !strcmp(argv[i], "--to-simulation-time"))
                        options.toSimulationTime = BigDecimal::parse(argv[++i]);
                    else if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--event-numbers"))
                        parseEventNumberTokens(options.eventNumbers, argv[++i]);
                    else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file-offsets"))
                        parseFileOffsetTokens(options.fileOffsets, argv[++i]);
                    else if (!strcmp(argv[i], "-me") || !strcmp(argv[i], "--module-expression"))
                        options.moduleExpression = argv[++i];
                    else if (!strcmp(argv[i], "-mn") || !strcmp(argv[i], "--module-names"))
                        parseStringTokens(options.moduleNames, argv[++i]);
                    else if (!strcmp(argv[i], "-mt") || !strcmp(argv[i], "--module-class-names"))
                        parseStringTokens(options.moduleClassNames, argv[++i]);
                    else if (!strcmp(argv[i], "-md") || !strcmp(argv[i], "--module-ned-type-names"))
                        parseStringTokens(options.moduleNedTypeNames, argv[++i]);
                    else if (!strcmp(argv[i], "-mi") || !strcmp(argv[i], "--module-ids"))
                        parseIntTokens(options.moduleIds, argv[++i]);
                    else if (!strcmp(argv[i], "-se") || !strcmp(argv[i], "--message-expression"))
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
                    else if (!strcmp(argv[i], "-ol") || !strcmp(argv[i], "--omit-log-lines"))
                        options.outputLogLines = false;
                    else if (i == argc - 1)
                        options.inputFileName = argv[i];
                }
                catch (std::exception& e) {
                    fprintf(stderr, "Invalid argument '%s': %s\n", argv[i], e.what());
                    return 1;
                }
            }

            if (!options.inputFileName)
                usage("No input file specified");
            else {
                if (options.outputFileName)
                    options.outputFile = fopen(options.outputFileName, "w");
                else
                    options.outputFile = stdout;

                if (!strcmp(command, "offsets"))
                    offsets(options);
                else if (!strcmp(command, "events"))
                    events(options);
                else if (!strcmp(command, "ranges"))
                    ranges(options);
                else if (!strcmp(command, "filter"))
                    filter(options);
                else if (!strcmp(command, "echo"))
                    echo(options);
                else if (!strcmp(command, "cat"))
                    cat(options);
                else
                    usage("Unknown or invalid command");

                if (options.outputFileName)
                    fclose(options.outputFile);
            }
        }

        return 0;
    }
    catch (std::exception& e) {
        fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }
}

