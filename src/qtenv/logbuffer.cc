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
#include "common/stringutil.h"
#include "logbuffer.h"
#include "qtutil.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {

LogBuffer::Line::Line(int contextComponentId, LogLevel logLevel, const char *prefix, const char *line)
    : contextComponentId(contextComponentId), logLevel(logLevel), prefix(prefix), line(opp_strdup(line))
{

}


LogBuffer::Line::Line(int contextComponentId, LogLevel logLevel, const char *prefix, const char *line, int lineLength)
    : contextComponentId(contextComponentId), logLevel(logLevel), prefix(prefix), line(opp_strndup(line, lineLength))
{

}


LogBuffer::Entry::Entry(Kind kind, eventnumber_t e, simtime_t t, cComponent *comp, const char *banner)
    : kind(kind), eventNumber(e), simtime(t), componentId(comp ? (comp->getId()) : 0), banner(opp_strdup(banner))
{

}

LogBuffer::Entry::Entry(Kind kind, eventnumber_t e, simtime_t t, cComponent *comp, const char *banner, int bannerLength)
    : kind(kind), eventNumber(e), simtime(t), componentId(comp ? (comp->getId()) : 0), banner(opp_strndup(banner, bannerLength))
{

}

LogBuffer::Entry::~Entry()
{
    delete[] banner;
    for (const auto& line : lines) {
        delete[] line.prefix;
        delete[] line.line;
    }
    for (const auto& msg : msgs) {
        delete msg.msg;
    }
}

//----

LogBuffer::~LogBuffer()
{
    // note: calling clear() is not good, because it allocates a new entry that would be leaked
    for (int i = 0; i < entries.size(); i++)
        delete entries[i];
}

void LogBuffer::addEvent(eventnumber_t e, simtime_t t, cModule *mod, const char *banner)
{
    Entry *entry = new Entry(Entry::Kind::PROCESSED_EVENT, e, t, mod, banner);
    entries.push_back(entry);
    Q_EMIT logEntryAdded(entry);

    discardEventsIfLimitExceeded();
}

void LogBuffer::addInitialize(cComponent *component, const char *banner)
{
    Entry *entry = new Entry(Entry::Kind::COMPONENT_INIT_STAGE, 0, simTime(), component, banner);
    entries.push_back(entry);
    Q_EMIT logEntryAdded(entry);

    discardEventsIfLimitExceeded();
}

void LogBuffer::addLogLine(LogLevel logLevel, const char *prefix, const char *text, int len)
{
    ASSERT(!entries.empty());

    // FIXME if last line is "info" then we cannot append to it! create new entry with empty banner?

    Entry *entry = entries.back();
    cComponent *contextComponent = getSimulation()->getContext();
    int contextComponentId = contextComponent ? contextComponent->getId() : 0;
    entry->lines.push_back(Line(contextComponentId, logLevel, opp_strdup(prefix), text, len));

    Q_EMIT logLineAdded(entry);
}

void LogBuffer::addInfo(const char *text, int len)
{
    // TODO ha inline info (contextmodule!=nullptr), sima logline-kent adjuk hozza!!!!
    Entry *entry = new Entry(Entry::Kind::SYSTEM_MESSAGE, 0, simTime(), nullptr, text, len);
    entries.push_back(entry);
    Q_EMIT logEntryAdded(entry);

    discardEventsIfLimitExceeded();
}

void LogBuffer::addScheduler(const char *text, int len)
{
    // TODO ha inline info (contextmodule!=nullptr), sima logline-kent adjuk hozza!!!!
    Entry *entry = new Entry(Entry::Kind::SCHEDULER, 0, simTime(), nullptr, text, len);
    entries.push_back(entry);
    Q_EMIT logEntryAdded(entry);

    discardEventsIfLimitExceeded();
}

void LogBuffer::beginSend(cMessage *msg, const SendOptions& options)
{
    ASSERT(!entries.empty());

    // FIXME if last line is "info" then we cannot append to it! create new entry with empty banner?

    Entry *entry = entries.back();
    entry->msgs.push_back(MessageSend());
    MessageSend& msgsend = entry->msgs.back();
    msgsend.msg = nullptr; // will be populated in endSend() or when it is discarded
    msgsend.hopModuleIds.push_back(msg->getSenderModuleId());
}

void LogBuffer::messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result)
{
    ASSERT(!entries.empty());
    Entry *entry = entries.back();
    MessageSend& msgsend = entry->msgs.back();
    ASSERT(!msgsend.msg || msgsend.msg->getId() == msg->getId());

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
    ASSERT(!msgsend.msg || msgsend.msg->getId() == msg->getId());
    msgsend.hopModuleIds.push_back(srcGate->getNextGate()->getOwnerModule()->getId());
}

void LogBuffer::messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result)
{
    ASSERT(!entries.empty());
    Entry *entry = entries.back();
    MessageSend& msgsend = entry->msgs.back();
    ASSERT(!msgsend.msg || msgsend.msg->getId() == msg->getId());

    if (result.discard) {
        // the message was discarded, so it will not arrive, endSend() will not be called,
        // but we have to make a copy anyway
        msgsend.msg = msg->privateDup();

        // storing the copy for animation
        messageDups.insert({msg, msgsend.msg});

        // clearing the previous arrival module/gate, since it did not really arrive
        msgsend.msg->setArrival(0, 0);
        msgsend.discarded = true;
    }

    msgsend.hopModuleIds.push_back(srcGate->getNextGate()->getOwnerModule()->getId());
}

void LogBuffer::endSend(cMessage *msg)
{
    ASSERT(!entries.empty());
    Entry *entry = entries.back();
    MessageSend& msgsend = entry->msgs.back();
    // the message has arrived, we have to make a copy
    msgsend.msg = msg->privateDup();

    // storing the copy for animation
    messageDups.insert({msg, msgsend.msg});

    Q_EMIT messageSendAdded(entry);
}

void LogBuffer::delivery(cMessage *msg)
{
    // most likely we just got out of express mode
    if (!getLastMessageDup(msg))
        // storing a copy for animation
        messageDups.insert({msg, msg->privateDup()});
}

void LogBuffer::setMaxNumEntries(int limit)
{
    maxNumEntries = limit;
    discardEventsIfLimitExceeded();
}

void LogBuffer::discardEventsIfLimitExceeded()
{
    // a limit of 0 means no limit
    while (maxNumEntries > 0 && entries.size() > maxNumEntries) {
        // discard first entry
        auto discardedEntry = entries.front();
        entries.pop_front();
        entriesDiscarded++;
        Q_EMIT entryDiscarded(discardedEntry);
        delete discardedEntry;
    }
}

void LogBuffer::clear()
{
    for (int i = 0; i < entries.size(); i++)
        delete entries[i];
    entries.clear();
    entriesDiscarded = 0;
    messageDups.clear();

    // just so log lines can be put somewhere even before initialization
    Entry *genesisEntry = new Entry(Entry::Kind::GENESIS, 0, SimTime::ZERO, nullptr, nullptr);
    entries.push_back(genesisEntry);
    Q_EMIT logEntryAdded(genesisEntry);
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

int LogBuffer::findEntryBySimTime(simtime_t simTime)
{
    if (entries.empty())
        return -1;
    int first = 0, last = entries.size()-1;
    int middle = (first + last)/2;
    while (first <= last) {
        if (entries[middle]->simtime < simTime)
            first = middle + 1;
        else if (entries[middle]->simtime == simTime)
            return middle;
        else
            last = middle - 1;

        middle = (first + last)/2;
    }
    return middle;
}

LogBuffer::Entry *LogBuffer::getEntryBySimTime(simtime_t simTime)
{
    int i = findEntryBySimTime(simTime);
    return i == -1 ? nullptr : entries[i];
}

cMessage *LogBuffer::getLastMessageDup(cMessage *of) const
{
    auto range = messageDups.equal_range(of);
    if (range.first == range.second)
        return nullptr;
    return (--range.second)->second;
}

void LogBuffer::dump() const
{
    printf("LogBuffer: %d entries\n", (int)entries.size());

    for (int i = 0; i < entries.size(); i++) {
        const Entry *entry = entries[i];
        printf("[e %d] #%" PRId64 " t=%s componentId=%d: %s\n", i, entry->eventNumber, SIMTIME_STR(entry->simtime), entry->componentId, entry->banner);
        for (int j = 0; j < entry->lines.size(); j++)
            printf("\t[l %d]: contextComponentId=%d: %s%s\n", j, entry->lines[j].contextComponentId, entry->lines[j].prefix, entry->lines[j].line);
        for (int j = 0; j < entry->msgs.size(); j++) {
            printf("\t[m %d]:%s\n", j, entry->msgs[j].msg->str().c_str());
            for (int k = 0; k < entry->msgs[j].hopModuleIds.size(); k++)
                printf("\t\t[h %d]:%s\n", k, getSimulation()->getComponent(entry->msgs[j].hopModuleIds[k])->getFullPath().c_str());
        }
    }
}

}  // namespace qtenv
}  // namespace omnetpp

