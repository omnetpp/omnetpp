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
   protected:
      int getIntToken(const char **tokens, int numTokens, const char *sign);
      long getLongToken(const char **tokens, int numTokens, const char *sign);
      simtime_t getSimtimeToken(const char **tokens, int numTokens, const char *sign);
      const char *getStringToken(const char **tokens, int numTokens, const char *sign);

   public:
      EventLogEntry();
      virtual void parse(const char **tokens, int numTokens) = 0;
      virtual void print(FILE *fout) = 0;
};

#endif