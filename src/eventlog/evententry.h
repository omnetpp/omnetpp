//=========================================================================
//  EVENTENTRY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTENTRY_H_
#define __EVENTENTRY_H_

#include <sstream>
#include <list>
#include <vector>
#include <map>
#include "stringpool.h"
#include "filetokenizer.h"
#include "exception.h"
#include "eventlogdefs.h"

/**
 * A compound or simple module extracted from the log file
 */
class ModuleEntry
{
    public:
        const char *moduleClassName; // stringpooled
        std::string moduleFullPath;  // unique, no need for stringpool
        int moduleId;

    public:
        ModuleEntry();
        ~ModuleEntry();
};

typedef std::vector<ModuleEntry*> ModuleEntryList;

class EventEntry;

/**
 * A message sent from one module to another.
 */
class MessageEntry
{
    public:
        /** Tells if this entry represents a message delivery or a message send */
        bool isDelivery; // if true, this message is the target event's cause (this==source->cause)
        long lineNumber; // in the event log file
        const char *messageClassName; // stringpooled
        const char *messageName;  // stringpooled
        ModuleEntry *module; // this is the target event's module (unless there's Enter_Method() involved)

        /** These log messages actually belong to the target event, but this way we can preserve ordering of message entries within the event */
        std::vector<const char *> logMessages; // stringpooled

        EventEntry *source;
        EventEntry *target;

    public:
        MessageEntry();
        ~MessageEntry();
};

typedef std::vector<MessageEntry*> MessageEntryList;

/**
 * An event log entry as seen in the log file corresponding to a handleMessage call.
 */
class EventEntry
{
    public:
        long eventNumber;
        double simulationTime;
        MessageEntry *cause;  // the message handled in this event (i.e. passed to handleMessage())
        MessageEntryList causes;  // also includes "cause"
        MessageEntryList consequences;
        int numLogMessages; // total number of log messages for this event (sum of its causes[]' log messages)

        // temporary state for tracing events, coloring (graph)
        bool isInCollectedEvents;

        // the following fields are for the convenience of the GUI
        double timelineCoordinate;
        int64 cachedX;
        int64 cachedY;
        bool isExpandedInTree;
        int tableRowIndex;

    public:
        EventEntry();
        ~EventEntry();
};

#endif


