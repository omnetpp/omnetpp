//=========================================================================
//  EVENTLOGFILTER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGFILTER_H_
#define __EVENTLOGFILTER_H_

#include <vector>
#include <deque>
#include "eventlogdefs.h"

class FilteredEvent
{
   protected:
      typedef std::vector<long> EventNumberList;
      long eventNumber;
      long cause; // the event number from which the message was sent that is being processed in this event
      EventNumberList causes; // the arrival event of messages which we send in this event
      EventNumberList consequences; // a set of events which process messages sent in this event

      // the following fields are for the convenience of the GUI
      double timelineCoordinate;
      int64 cachedX;
      int64 cachedY;
      bool isExpandedInTree;
      int tableRowIndex;
};

class EventLogFilter
{
   protected:
      typedef std::deque<FilteredEvent> FilteredEventList;
      FilteredEventList filteredEventList;

      long firstEventNumber; // event number of the first considered event
      long lastEventNumber; // event number of the last considered event
};

#endif