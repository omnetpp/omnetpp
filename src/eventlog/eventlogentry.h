//=========================================================================
//  EVENTLOGENTRY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGENTRY_H_
#define __EVENTLOGENTRY_H_

#include <sstream>
#include "defs.h"

class EventLogEntry
{
    public:
        virtual void parse(char *line) = 0;
        virtual void print(FILE *fout) = 0;
};

class EventLogTokenBasedEntry : public EventLogEntry
{
    protected:
        char *getToken(char **tokens, int numTokens, const char *sign);
        int getIntToken(char **tokens, int numTokens, const char *sign);
        long getLongToken(char **tokens, int numTokens, const char *sign);
        simtime_t getSimtimeToken(char **tokens, int numTokens, const char *sign);
        const char *getStringToken(char **tokens, int numTokens, const char *sign);

    public:
        virtual void parse(char *line);
        virtual void parse(char **tokens, int numTokens) = 0;
};

class EventLogMessage : public EventLogEntry
{
    public:
        const char *text;

    public: 
        EventLogMessage();
        virtual void parse(char *line);
        virtual void print(FILE *fout);
};

#endif