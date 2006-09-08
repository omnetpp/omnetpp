//=========================================================================
//  EVENT.CC - part of
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
#include "filereader.h"
#include "event.h"
#include "eventlogentry.h"

Event::Event()
{
    eventEntry = NULL;
}

Event::~Event()
{
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
    {
        delete *it;
    }
}

long Event::parse(FileReader *reader, long offset)
{
    eventEntry = NULL;
    eventLogEntries.clear();
    reader->seekTo(offset);

    while (true)
    {
        char *line = reader->readLine();

        if (!line)
            return reader->fileSize();

        EventLogEntry *eventLogEntry = EventLogEntry::parseEntry(line);
        EventEntry *eventEntry = dynamic_cast<EventEntry *>(eventLogEntry);

        if (eventEntry)
        {
            if (this->eventEntry)
                break;
            else
                this->eventEntry = eventEntry;
        }

        if (eventLogEntry)
            eventLogEntries.push_back(eventLogEntry);
    }

    return reader->lineStartOffset();
}

void Event::print(FILE *file)
{
    for (EventLogEntryList::iterator it = eventLogEntries.begin(); it != eventLogEntries.end(); it++)
    {
        EventLogEntry *eventLogEntry = *it;
        eventLogEntry->print(file);
    }
}
