//=========================================================================
//  EVENTLOGFACADE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGFACADE_H_
#define __EVENTLOGFACADE_H_

#include "ievent.h"
#include "ieventlog.h"

/**
 * A class that makes it possible to extract info about events, without
 * returning objects. (Wherever a C++ method returns an object pointer,
 * SWIG-generated wrapper creates a corresponding Java object with the
 * pointer value inside. This has disastrous effect on performance
 * when dealing with huge amounts of data).
 *
 * IMPORTANT! Java code MUST NOT ask members of objects that don't exist!
 * Otherwise there will be a crash. For example, if getEvent_i_hasCause(i)
 * returns false, then getEvent_i_cause_messageClassName(i) will CRASH!
 */
class EventLogFacade
{
    protected:
        IEventLog *eventLog;

    public:
        EventLogFacade(IEventLog *eventLog);
};

#endif