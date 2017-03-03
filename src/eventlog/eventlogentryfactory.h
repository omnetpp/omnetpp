//=========================================================================
//  EVENTLOGENTRYFACTORY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_EVENTLOGENTRYFACTORY_H
#define __OMNETPP_EVENTLOG_EVENTLOGENTRYFACTORY_H

#include "eventlogentries.h"

namespace omnetpp {
namespace eventlog {

class EVENTLOG_API EventLogEntryFactory
{
   public:
      EventLogTokenBasedEntry * parseEntry(Event *event, int index, char **tokens, int numTokens);
};

} // namespace eventlog
}  // namespace omnetpp


#endif


