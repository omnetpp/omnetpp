//==========================================================================
//  LOGBUFFER.H - part of
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

#ifndef __OMNETPP_QTENV_LOGBUFFER_H
#define __OMNETPP_QTENV_LOGBUFFER_H

#include <vector>
#include <map>
#include "omnetpp/simtime_t.h"
#include "omnetpp/cchannel.h"
#include "circularbuffer.h"
#include "qtenvdefs.h"
#include <QtCore/QObject>

namespace omnetpp {

class cModule;

namespace qtenv {

/**
 * Stores textual debug output from modules.
 */
class QTENV_API LogBuffer : public QObject
{
    Q_OBJECT // for signals

  public:
    struct Line {
        int contextComponentId; // this is also set correctly in initialization stages, and if applies, for "info log" lines too
        LogLevel logLevel;
        const char *prefix;
        const char *line;  // including newline
        Line(int contextComponentId, LogLevel logLevel, const char *prefix, const char *line); // line is null-terminated
        Line(int contextComponentId, LogLevel logLevel, const char *prefix, const char *line, int lineLength);
    };
    struct MessageSend {
        cMessage *msg;
        std::vector<int> hopModuleIds; //TODO also: txStartTime, propagationDelay, duration for each hop
        bool discarded = false;
    };

    // Stores information about either:
    //   - a processed event
    //   - an initialization stage of a given component
    //   - a "system message" ("info log line")
    //
    // Each entry contains, on top of some basic info (like a timestamp):
    // any number of sent messages and printed log lines, independently.
    struct Entry {
        enum class Kind {
            GENESIS,              // for things printed in module constructors, etc.
            PROCESSED_EVENT,
            COMPONENT_INIT_STAGE,
            SCHEDULER,
            SYSTEM_MESSAGE,       // an "info log line"
            INTERNAL_WARNING,     // invalid DisplayString for example
        };

        Kind kind;

        eventnumber_t eventNumber = 0; // 0 for initialization, >0 afterwards
        simtime_t simtime = 0;

        // the component that PROCESSED the EVENT, or of which the INIT_STAGE was (or 0 if not available)
        int componentId = 0;

        //TODO add processed event (msg) name, class, kind, previousEventNumber
        const char *banner = nullptr;
        std::vector<Line> lines;
        std::vector<MessageSend> msgs;

        Entry(Kind kind, eventnumber_t e, simtime_t t, cComponent *comp, const char *banner); // banner is null-terminated
        Entry(Kind kind, eventnumber_t e, simtime_t t, cComponent *comp, const char *banner, int bannerLength);

        bool isGenesis() { return kind == Kind::GENESIS; }
        bool isInitializationStage() { return kind == Kind::COMPONENT_INIT_STAGE; }
        bool isSystemMessage() { return kind == Kind::SYSTEM_MESSAGE; }
        bool isEvent() { return kind == Kind::PROCESSED_EVENT; }
        bool isScheduler() { return kind == Kind::SCHEDULER; }

        ~Entry();
    };

  protected:
    circular_buffer<Entry*> entries; // this should NEVER be completely empty - at least a GENESIS Entry should be present
    int maxNumEntries = 100000;
    int entriesDiscarded = 0;

    void discardEventsIfLimitExceeded();

    // Makes our privateDups of the logged messages easily accessible.
    // Every message is duplicated once each time it is sent.
    std::multimap<cMessage *, cMessage *> messageDups;

  public:
    ~LogBuffer();
    void addInitialize(cComponent *component, const char *banner);
    void addEvent(eventnumber_t e, simtime_t t, cModule *moduleIds, const char *banner);
    void addLogLine(LogLevel logLevel, const char *prefix, const char *text) { addLogLine(logLevel, prefix, text, strlen(text)); }
    void addLogLine(LogLevel logLevel, const char *prefix, const char *text, int len);
    void addInfo(const char *text) { addInfo(text, strlen(text)); }
    void addInfo(const char *text, int len);
    void addScheduler(const char *text) { addScheduler(text, strlen(text)); }
    void addScheduler(const char *text, int len);

    void beginSend(cMessage *msg, const SendOptions& options);
    void messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result);
    void messageSendHop(cMessage *msg, cGate *srcGate);
    void messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result);
    void endSend(cMessage *msg);

    void delivery(cMessage *msg); // doesn't matter if direct or not

    void setMaxNumEntries(int limit); // when exceeded, oldest entries are discarded
    int getMaxNumEntries()  {return maxNumEntries;}

    const circular_buffer<Entry*>& getEntries() const {return entries;}
    int getNumEntries() const {return entries.size();}
    int getNumEntriesDiscarded() const {return entriesDiscarded;}
    // NOTE: Unlike `findEntryBySimTime`, looks for an exact match.
    int findEntryByEventNumber(eventnumber_t eventNumber);
    Entry *getEntryByEventNumber(eventnumber_t eventNumber);
    // NOTE: Unlike `findEntryByEventNumber`, returns an approximate match.
    int findEntryBySimTime(simtime_t simTime);
    Entry *getEntryBySimTime(simtime_t simTime);

    // Returns the last private copy we made of a given message,
    // ot nullptr if no copy is found. The parameter doesn't have
    // to point to an existing message, or be valid at all.
    cMessage *getLastMessageDup(cMessage *of) const;

    void clear();

    void dump() const;

Q_SIGNALS:
    // `entry` is the last entry already in the LogBuffer, that just got added
    void logEntryAdded(LogBuffer::Entry *entry);
    // `entry` is the last, already existing entry, with a newly added line in it
    void logLineAdded(LogBuffer::Entry *entry);
    // `entry` is the last, already existing entry, with a newly added msgsend in it
    void messageSendAdded(LogBuffer::Entry *entry);
    // When this signal is fired, the entry in the parameter is
    // no longer in the buffer, but is not deleted yet.
    void entryDiscarded(LogBuffer::Entry *discardedEntry);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif
