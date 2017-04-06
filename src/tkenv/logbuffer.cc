//==========================================================================
//  LOGBUFFER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include "omnetpp/cmodule.h"
#include "omnetpp/cmessage.h"
#include "tklib.h"
#include "logbuffer.h"

namespace omnetpp {
namespace tkenv {

LogBuffer::Entry::~Entry()
{
    delete[] banner;
    for (auto & line : lines) {
        delete[] line.prefix;
        delete[] line.line;
    }
    for (auto & msg : msgs) {
        delete msg.msg;
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
        if (listeners[i] == l) {
            listeners.erase(listeners.begin()+i);
            break;
        }
}

void LogBuffer::fillEntry(Entry *entry, eventnumber_t e, simtime_t t, cModule *mod, const char *banner)
{
    entry->eventNumber = e;
    entry->simtime = t;
    entry->banner = banner;
    entry->componentId = mod ? mod->getId() : 0;
}

void LogBuffer::addEvent(eventnumber_t e, simtime_t t, cModule *mod, const char *banner)
{
    Entry *entry = new Entry();
    entries.push_back(entry);
    fillEntry(entry, e, t, mod, opp_strdup(banner));
    discardEventsIfLimitExceeded();

    for (auto & listener : listeners)
        listener->logEntryAdded();
}

void LogBuffer::addInitialize(cComponent *component, const char *banner)
{
    if (entries.empty()) {
        Entry *entry = new Entry();
        entries.push_back(entry);
        fillEntry(entry, 0, simTime(), getSimulation()->getSystemModule(), opp_strdup("** Initializing network\n"));
    }

    Entry *entry = entries.back();
    cComponent *contextComponent = getSimulation()->getContext();
    int contextComponentId = contextComponent ? contextComponent->getId() : 0;
    entry->lines.push_back(Line(contextComponentId, nullptr, opp_strdup(banner)));

    for (auto & listener : listeners)
        listener->logLineAdded();
}

void LogBuffer::addLogLine(const char *prefix, const char *text)
{
    addLogLine(prefix, text, strlen(text));
}

void LogBuffer::addLogLine(const char *prefix, const char *text, int len)
{
    if (entries.empty()) {
        Entry *entry = new Entry();
        entries.push_back(entry);
        fillEntry(entry, 0, simTime(), nullptr, nullptr);
    }

    // FIXME if last line is "info" then we cannot append to it! create new entry with empty banner?

    Entry *entry = entries.back();
    cComponent *contextComponent = getSimulation()->getContext();
    int contextComponentId = contextComponent ? contextComponent->getId() : 0;
    entry->lines.push_back(Line(contextComponentId, opp_strdup(prefix), opp_strdup(text, len)));

    for (auto & listener : listeners)
        listener->logLineAdded();
}

void LogBuffer::addInfo(const char *text)
{
    addInfo(text, strlen(text));
}

void LogBuffer::addInfo(const char *text, int len)
{
    // TODO ha inline info (contextmodule!=nullptr), sima logline-kent adjuk hozza!!!!
    Entry *entry = new Entry();
    entries.push_back(entry);
    fillEntry(entry, 0, simTime(), nullptr, opp_strdup(text, len));
    discardEventsIfLimitExceeded();

    for (auto & listener : listeners)
        listener->logEntryAdded();
}

void LogBuffer::beginSend(cMessage *msg)
{
    if (entries.empty()) {
        // this is likely the initialize() phase -- hence no banner
        addEvent(0, SIMTIME_ZERO, nullptr, "{}");
        Entry *entry = entries.back();
        entry->componentId = -1;
    }

    // FIXME if last line is "info" then we cannot append to it! create new entry with empty banner?

    Entry *entry = entries.back();
    entry->msgs.push_back(MessageSend());
    MessageSend& msgsend = entry->msgs.back();
    msgsend.msg = msg->privateDup();
    msgsend.hopModuleIds.push_back(msg->getSenderModuleId());
}

void LogBuffer::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    ASSERT(!entries.empty());
    Entry *entry = entries.back();
    MessageSend& msgsend = entry->msgs.back();
    ASSERT(msgsend.msg->getId() == msg->getId());

    std::vector<cModule *> hops;
    resolveSendDirectHops(msg->getSenderModule(), toGate->getOwnerModule(), hops);

    for (unsigned i = 1; i < hops.size(); i++)  // iterate from 1 because src module is already in hopModuleIds
        msgsend.hopModuleIds.push_back(hops[i]->getId());
}

void LogBuffer::messageSendHop(cMessage *msg, cGate *srcGate)
{
    ASSERT(!entries.empty());
    Entry *entry = entries.back();
    MessageSend& msgsend = entry->msgs.back();
    ASSERT(msgsend.msg->getId() == msg->getId());
    msgsend.hopModuleIds.push_back(srcGate->getNextGate()->getOwnerModule()->getId());
}

void LogBuffer::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    messageSendHop(msg, srcGate);  // TODO store propagationDelay and transmissionDelay as well
}

void LogBuffer::endSend(cMessage *msg)
{
    for (auto & listener : listeners)
        listener->messageSendAdded();  // TODO but endSend() is not called when msg is discarded in the channel!
}

void LogBuffer::setMaxNumEntries(int limit)
{
    maxNumEntries = limit;
    discardEventsIfLimitExceeded();
}

void LogBuffer::discardEventsIfLimitExceeded()
{
    // discard first entry
    while (maxNumEntries > 0 && entries.size() > maxNumEntries) {
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
    while (first <= last) {
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
    return i == -1 ? nullptr : entries[i];
}

void LogBuffer::dump() const
{
    printf("LogBuffer: %d entries\n", (int)entries.size());

    for (int i = 0; i < entries.size(); i++) {
        const Entry *entry = entries[i];
        printf("[%d] #%" PRId64 " t=%s componentId=%d: %s", i, entry->eventNumber, SIMTIME_STR(entry->simtime), entry->componentId, entry->banner);
        for (const auto & line : entry->lines)
            printf("\t[l%d]:%s%s", i, line.prefix, line.line);
    }
}

}  // namespace tkenv
}  // namespace omnetpp

