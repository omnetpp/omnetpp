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

#include <assert.h>
#include "eventlogentry.h"

EventLogEntry::EventLogEntry()
{
}

int getIntToken(const char **tokens, int numTokens, const char *sign)
{
   return 0;
}

long getLongToken(const char **tokens, int numTokens, const char *sign)
{
   return 0;
}

simtime_t getSimtimeToken(const char **tokens, int numTokens, const char *sign)
{
   return 0;
}

const char *getStringToken(const char **tokens, int numTokens, const char *sign)
{
   return NULL;
}
