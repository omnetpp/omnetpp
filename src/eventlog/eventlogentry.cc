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

#include "linetokenizer.h"
#include "eventlog.h"
#include "eventlogentry.h"
#include "eventlogentryfactory.h"

EventLogEntry *EventLogEntry::parseEntry(char *line)
{
    if (*line == '-')
    {
        EventLogMessage *eventLogMessage = new EventLogMessage();
        eventLogMessage->parse(line);
        return eventLogMessage;
    }
    else
    {
        LineTokenizer tokenizer;
        EventLogEntryFactory factory;
        tokenizer.tokenize(line);
        return factory.parseEntry(tokenizer.tokens(), tokenizer.numTokens());
    }
}

char *EventLogTokenBasedEntry::getToken(char **tokens, int numTokens, const char *sign)
{
    for (int i = 0; i < numTokens; i++)
        if (!strcmp(tokens[i], sign))
            return tokens[i + 1];

    return NULL;
}

int EventLogTokenBasedEntry::getIntToken(char **tokens, int numTokens, const char *sign)
{
    char *token = getToken(tokens, numTokens, sign);
    return token == NULL ? -1 : atoi(token);
}

long EventLogTokenBasedEntry::getLongToken(char **tokens, int numTokens, const char *sign)
{
    char *token = getToken(tokens, numTokens, sign);
    return token == NULL ? -1 : atol(token);
}

simtime_t EventLogTokenBasedEntry::getSimtimeToken(char **tokens, int numTokens, const char *sign)
{
    char *token = getToken(tokens, numTokens, sign);
    return token == NULL ? -1 : atof(token);
}

const char *EventLogTokenBasedEntry::getStringToken(char **tokens, int numTokens, const char *sign)
{
    return eventLogStringPool.get(getToken(tokens, numTokens, sign));
}

void EventLogTokenBasedEntry::parse(char *line)
{
    LineTokenizer tokenizer;
    tokenizer.tokenize(line);
    parse(tokenizer.tokens(), tokenizer.numTokens());
}

/***********************************************/

EventLogMessage::EventLogMessage()
{
    text = NULL;
}

void EventLogMessage::parse(char *line)
{
    text = eventLogStringPool.get(line + 2);
}

void EventLogMessage::print(FILE *fout)
{
    ::fprintf(fout, "- %s\n", text);
}
