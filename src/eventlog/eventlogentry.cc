//=========================================================================
//  EVENTLOGENTRY.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "eventlog.h"
#include "eventlogentry.h"
#include "eventlogentryfactory.h"

char EventLogEntry::buffer[128];
LineTokenizer EventLogEntry::tokenizer(32768);
static const char *lastLine;
static int lastLineLength;

EventLogEntry::EventLogEntry()
{
    contextModuleId = -1;
    level = -1;
}

EventLogEntry *EventLogEntry::parseEntry(Event *event, char *line, int length)
{
    lastLine = line;
    lastLineLength = length;

    if (*line == '-')
    {
        EventLogMessageEntry *eventLogMessage = new EventLogMessageEntry(event);
        eventLogMessage->parse(line, length);
        return eventLogMessage;
    }
    else
    {
        EventLogEntryFactory factory;
        tokenizer.tokenize(line, length);
        return factory.parseEntry(event, tokenizer.tokens(), tokenizer.numTokens());
    }
}

int EventLogEntry::getIndex()
{
    for (int i = 0; i < event->getNumEventLogEntries(); i++)
        if (this == event->getEventLogEntry(i))
            return i;

    Assert(false);
}

bool EventLogEntry::isMessageSend()
{
    return dynamic_cast<BeginSendEntry *>(this) != NULL;
}

char *EventLogTokenBasedEntry::getToken(char **tokens, int numTokens, const char *sign, bool mandatory)
{
    for (int i = 0; i < numTokens; i++)
        if (!strcmp(tokens[i], sign))
            return tokens[i + 1];

    if (mandatory)
        throw opp_runtime_error("Missing mandatory token %s in line %.*s", sign, lastLineLength, lastLine);

    return NULL;
}

int EventLogTokenBasedEntry::getIntToken(char **tokens, int numTokens, const char *sign, bool mandatory, int defaultValue)
{
    char *token = getToken(tokens, numTokens, sign, mandatory);
    return token ? atoi(token) : defaultValue;
}

long EventLogTokenBasedEntry::getLongToken(char **tokens, int numTokens, const char *sign, bool mandatory, long defaultValue)
{
    char *token = getToken(tokens, numTokens, sign, mandatory);
    return token ? atol(token) : defaultValue;
}

simtime_t EventLogTokenBasedEntry::getSimtimeToken(char **tokens, int numTokens, const char *sign, bool mandatory, simtime_t defaultValue)
{
    char *token = getToken(tokens, numTokens, sign, mandatory);
    return token ? BigDecimal::parse(token) : defaultValue;
}

const char *EventLogTokenBasedEntry::getStringToken(char **tokens, int numTokens, const char *sign, bool mandatory, const char *defaultValue)
{
    char *token = getToken(tokens, numTokens, sign, mandatory);
    return token ? eventLogStringPool.get(token) : defaultValue;
}

void EventLogTokenBasedEntry::parse(char *line, int length)
{
    tokenizer.tokenize(line, length);
    parse(tokenizer.tokens(), tokenizer.numTokens());
}

/***********************************************/

EventLogMessageEntry::EventLogMessageEntry(Event *event)
{
    this->event = event;
    text = NULL;
}

void EventLogMessageEntry::parse(char *line, int length)
{
    char *s = line + length - 1;
    char ch = '\0';

    if (length > 0) {
        ch = *s;
        *s = '\0';
    }

    if (length > 1 && *(s - 1) == '\r')
        *(s - 1) = '\0';

    text = eventLogStringPool.get(line + 2);

    if (length > 0)
        *s = ch;

    if (length > 1)
        *(s - 1) = '\r';
}

void EventLogMessageEntry::print(FILE *fout)
{
    ::fprintf(fout, "- %s\n", text);
}

const std::vector<const char *> EventLogMessageEntry::getAttributeNames() const
{
    std::vector<const char *> names;
    names.push_back("-");

    return names;
}

const char *EventLogMessageEntry::getAttribute(const char *name) const
{
    if (!strcmp(name, "type"))
        return "-";
    else if (!strcmp(name, "-"))
        return text;
    else
        return NULL;
}
