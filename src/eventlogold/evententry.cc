//=========================================================================
//  EVENTENTRY.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include <assert.h>
#include "eventlog.h"

/*--------------------------------------------------------------*/
ModuleEntry::ModuleEntry()
{
    moduleId = -1;
    moduleClassName = ""; // must not be NULL because SWIG-generated code will crash!
}

ModuleEntry::~ModuleEntry()
{
}

/*--------------------------------------------------------------*/
EventEntry::EventEntry()
{
    eventNumber = -1;
    simulationTime = -1;
    cause = NULL;
    numLogMessages = 0;

    cachedX = cachedY = 0;
    isExpandedInTree = false;
    tableRowIndex = 0;
}

EventEntry::~EventEntry()
{
}

/*--------------------------------------------------------------*/
MessageEntry::MessageEntry()
{
    isDelivery = false;
    lineNumber = -1;
    messageClassName = "";  // must not be NULL because SWIG-generated code will crash!
    messageName = ""; // must not be NULL because SWIG-generated code will crash!
    source = target = NULL;
    module = NULL;
}

MessageEntry::~MessageEntry()
{
}


