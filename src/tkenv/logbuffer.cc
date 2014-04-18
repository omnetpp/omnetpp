//==========================================================================
//  LOGBUFFER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include "tklib.h"
#include "logbuffer.h"
#include "cmodule.h"
#include "cmessage.h"

NAMESPACE_BEGIN

LogBuffer::Entry::~Entry()
{
    delete[] moduleIds;
    delete[] banner;
    for (int i=0; i<(int)lines.size(); i++) {
        delete[] lines[i].prefix;
        delete[] lines[i].line;
    }
    for (int i=0; i<(int)msgs.size(); i++) {
        delete msgs[i].msg;
    }
}

//----

LogBuffer::LogBuffer()
{
    maxNumEntries = 100000;
    entriesDiscarded = 0;
}

LogBuffer::~LogBuffer()
{
    clear();
}

void LogBuffer::addListener(ILogBufferListener *l)
{
    listeners.push_back(l);
}

void LogBuffer::removeListener(ILogBufferListener *l)
{
    for (unsigned int i = 0; i < listeners.size(); i++)
        if (listeners[i] == l)
            {listeners.erase(listeners.begin()+i); break;}
}

void LogBuffer::fillEntry(Entry *entry, eventnumber_t e, simtime_t t, cModule *mod, const char *banner)
{
    entry->eventNumber = e;
    entry->simtime = t;
    entry->banner = opp_strdup(banner);

    // store all moduleIds up to the root
    if (mod)
    {
        int depth = 0;
        for (cModule *p=mod; p; p=p->getParentModule())
            depth++;
        entry->moduleIds = new int[depth+1];
        int i = 0;
        for (cModule *p=mod; p; p=p->getParentModule(), i++)
            entry->moduleIds[i] = p->getId();
         entry->moduleIds[depth] = 0;
    }
}

void LogBuffer::addEvent(eventnumber_t e, simtime_t t, cModule *mod, const char *banner)
{
    Entry *entry = new Entry();
    entries.push_back(entry);
    fillEntry(entry, e, t, mod, banner);
    discardEventsIfLimitExceeded();

    for (unsigned int i = 0; i < listeners.size(); i++)
        listeners[i]->logEntryAdded();
}

void LogBuffer::addLogLine(const char *prefix, const char *text)
{
    if (entries.empty())
    {
        // this is likely the initialize() phase -- hence no banner
        addEvent(0, SIMTIME_ZERO, NULL, "{}");
        Entry *entry = entries.back();
        entry->moduleIds = new int[1]; // add empty array, to distinguish entry from an info entry
        entry->moduleIds[0] = 0;
    }

    //FIXME if last line is "info" then we cannot append to it! create new entry with empty banner?

    Entry *entry = entries.back();
    entry->lines.push_back(Line(opp_strdup(prefix), opp_strdup(text)));

    for (unsigned int i = 0; i < listeners.size(); i++)
        listeners[i]->logLineAdded();
}

void LogBuffer::addInfo(const char *text)
{
    Entry *entry = new Entry();
    entries.push_back(entry);
    fillEntry(entry, 0, SIMTIME_ZERO, NULL, text);
    discardEventsIfLimitExceeded();

    for (unsigned int i = 0; i < listeners.size(); i++)
        listeners[i]->logEntryAdded();
}

void LogBuffer::beginSend(cMessage *msg)
{
    if (entries.empty())
    {
        // this is likely the initialize() phase -- hence no banner
        addEvent(0, SIMTIME_ZERO, NULL, "{}");
        Entry *entry = entries.back();
        entry->moduleIds = new int[1]; // add empty array, to distinguish entry from an info entry
        entry->moduleIds[0] = 0;
    }

    //FIXME if last line is "info" then we cannot append to it! create new entry with empty banner?

    Entry *entry = entries.back();
    entry->msgs.push_back(MessageSend());
    MessageSend& msgsend = entry->msgs.back();
    msgsend.msg = msg->dup();  //FIXME this assigns a new ID!!!
    msgsend.msg->removeFromOwnershipTree();
    msgsend.hopModuleIds.push_back(msg->getSenderModuleId());
}

void LogBuffer::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    ASSERT(!entries.empty());
    Entry *entry = entries.back();
    MessageSend& msgsend = entry->msgs.back();
    ASSERT(msgsend.msg->getTreeId() == msg->getTreeId());  //XXX IDs differ because dup() assigns a new ID!!!

    std::vector<cModule*> hops;
    resolveSendDirectHops(msg->getSenderModule(), toGate->getOwnerModule(), hops);

    for (unsigned i=1; i<hops.size(); i++)  // iterate from 1 because src module is already in hopModuleIds
        msgsend.hopModuleIds.push_back(hops[i]->getId());
}

void LogBuffer::messageSendHop(cMessage *msg, cGate *srcGate)
{
    ASSERT(!entries.empty());
    Entry *entry = entries.back();
    MessageSend& msgsend = entry->msgs.back();
    ASSERT(msgsend.msg->getTreeId() == msg->getTreeId());  //XXX IDs differ because dup() assigns a new ID!!!
    msgsend.hopModuleIds.push_back(srcGate->getNextGate()->getOwnerModule()->getId());
}

void LogBuffer::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    messageSendHop(msg, srcGate);  //TODO store propagationDelay and transmissionDelay as well
}

void LogBuffer::endSend(cMessage *msg)
{
    for (unsigned int i = 0; i < listeners.size(); i++)
        listeners[i]->messageSendAdded();
}

void LogBuffer::setMaxNumEntries(int limit)
{
    maxNumEntries = limit;
    discardEventsIfLimitExceeded();
}

void LogBuffer::discardEventsIfLimitExceeded()
{
    // discard first entry
    while (maxNumEntries > 0 && entries.size() > maxNumEntries)
    {
        delete entries.front();
        entries.pop_front();
        entriesDiscarded++;
    }
}

void LogBuffer::clear()
{
    for (int i = 0; i < entries.size(); i++)
        delete entries[i];
    entries.clear();
    entriesDiscarded = 0;
}

int LogBuffer::findEntryByEventNumber(eventnumber_t eventNumber)
{
    int first = 0, last = entries.size()-1;
    int middle = (first + last)/2;
    while (first <= last)
    {
        if (entries[middle]->eventNumber < eventNumber)
            first = middle + 1;
        else if (entries[middle]->eventNumber == eventNumber)
            return middle;
        else
            last = middle - 1;

        middle = (first + last)/2;
    }
    return -1;
}

LogBuffer::Entry *LogBuffer::getEntryByEventNumber(eventnumber_t eventNumber)
{
    int i = findEntryByEventNumber(eventNumber);
    return i == -1 ? NULL : entries[i];
}

#define LL  INT64_PRINTF_FORMAT

void LogBuffer::dump() const
{
    printf("LogBuffer: %d entries\n", (int)entries.size());

    for (int i = 0; i < entries.size(); i++)
    {
        const Entry *entry = entries[i];
        printf("[%d] #%" LL "d t=%s moduleId=%d: %s", i, entry->eventNumber, SIMTIME_STR(entry->simtime), entry->moduleIds?entry->moduleIds[0]:-1, entry->banner);
        for (int j=0; j<(int)entry->lines.size(); j++)
            printf("\t[l%d]:%s%s", i, entry->lines[j].prefix, entry->lines[j].line);
    }
}

NAMESPACE_END

