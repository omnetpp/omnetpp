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

#define PTR(ptr) if (ptr == 0) throw new Exception("NULL ptr exception");

/**
 * A class that makes it possible to extract info about events, without
 * returning objects. (Wherever a C++ method returns an object pointer,
 * SWIG-generated wrapper creates a corresponding Java object with the
 * pointer value inside. This has disastrous effect on performance
 * when dealing with huge amounts of data).
 */
class EventLogFacade
{
    protected:
        IEventLog *eventLog;

        // SequenceChart state
        long timelineCoordinateSystemVersion;
        TimelineMode timelineMode;

    public:
        EventLogFacade(IEventLog *eventLog);
        virtual ~EventLogFacade() {}

        IEvent* Event_getEvent(int64 ptr);
        int64 Event_getPreviousEvent(int64 ptr);
        int64 Event_getNextEvent(int64 ptr);
        long Event_getEventNumber(int64 ptr);
        int Event_getModuleId(int64 ptr);
        int Event_getNumCauses(int64 ptr);
        int Event_getNumConsequences(int64 ptr);
        int64 Event_getCause(int64 ptr, int index);
        int64 Event_getConsequence(int64 ptr, int index);

        MessageDependency* MessageDependency_getMessageDependency(int64 ptr);
        const char *MessageDependency_getCauseMessageName(int64 ptr);
        const char *MessageDependency_getConsequenceMessageName(int64 ptr);
        MessageDependencyKind MessageDependency_getKind(int64 ptr);
        bool MessageDependency_isMessageSend(int64 ptr);
        bool MessageDependency_isMessageReuse(int64 ptr);
        bool MessageDependency_isFilteredMessageDependency(int64 ptr);
        int64 MessageDependency_getCauseEvent(int64 ptr);
        int64 MessageDependency_getConsequenceEvent(int64 ptr);

        const char *FilteredMessageDependency_getMiddleMessageName(int64 ptr);
};

#endif