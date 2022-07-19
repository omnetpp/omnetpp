//==========================================================================
//  RUNNER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <thread>
#include "omnetpp/cevent.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cfutureeventset.h"
#include "fakegui.h"
#include "runner.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace cmdenv {

double Runner::getElapsedSecs()
{
    return stopwatch.getElapsedSecs();
}

bool Runner::elapsed(long millis, int64_t& since)
{
    int64_t now = opp_get_monotonic_clock_usecs();
    bool ret = (now - since) > millis * 1000;
    if (ret)
        since = now;
    return ret;
}

void Runner::doRunNormal()
{
    while (true) {
        cEvent *event = simulation->takeNextEvent();
        if (!event)
            throw cTerminationException("Scheduler interrupted while waiting");

        // flush *between* printing event banner and event processing, so that
        // if event processing crashes, it can be seen which event it was
        if (autoflush)
            out.flush();

        if (printEventBanners)
            if (!event->isMessage() || static_cast<cMessage *>(event)->getArrivalModule()->getLogLevel() != LOGLEVEL_OFF)
                printEventBanner(simulation->getEventNumber()+1, event);  // note: event number is only incremented at the top of cSimulation::executeEvent()

        if (fakeGUI)
            fakeGUI->beforeEvent(event);

        // execute event
        simulation->executeEvent(event);

        if (fakeGUI)
            fakeGUI->afterEvent();

        // flush so that output from different modules don't get mixed
        cLogProxy::flushLastLine();

        if (stopwatch.hasTimeLimits())
            stopwatch.checkTimeLimits();

        if (sigintReceived)
            throw cTerminationException("SIGINT or SIGTERM received, exiting");
    }
}

void Runner::doRunExpressWithFakeGUI()
{
    ASSERT(fakeGUI != nullptr);

    speedometer.start(simulation->getSimTime());

    int64_t lastUpdateTime = opp_get_monotonic_clock_usecs();

    printStatusUpdate();

    while (true) {
        cEvent *event = simulation->takeNextEvent();
        if (!event)
            throw cTerminationException("Scheduler interrupted while waiting");

        speedometer.addEvent(simulation->getSimTime());

        fakeGUI->beforeEvent(event);

        simulation->executeEvent(event);

        fakeGUI->afterEvent();

        if ((simulation->getEventNumber() & 1023) == 0) {
            if (elapsed(statusFrequencyMs, lastUpdateTime))
                printStatusUpdate();
            if (stopwatch.hasTimeLimits())
                stopwatch.checkTimeLimits();
        }

        if (sigintReceived)
            throw cTerminationException("SIGINT or SIGTERM received, exiting");
    }
}

void Runner::doRunExpressNoFakeGui()
{
    ASSERT(fakeGUI == nullptr);

    speedometer.start(simulation->getSimTime());

    int64_t lastUpdateTime = opp_get_monotonic_clock_usecs();

    printStatusUpdate();

    while (true) {
        cEvent *event = simulation->takeNextEvent();
        if (!event)
            throw cTerminationException("Scheduler interrupted while waiting");

        speedometer.addEvent(simulation->getSimTime());

        simulation->executeEvent(event);

        if ((simulation->getEventNumber() & 1023) == 0) {
            if (elapsed(statusFrequencyMs, lastUpdateTime))
                printStatusUpdate();
            if (stopwatch.hasTimeLimits())
                stopwatch.checkTimeLimits();
        }

        if (sigintReceived)
            throw cTerminationException("SIGINT or SIGTERM received, exiting");
    }
}

void Runner::doRunExpressNoFakeGuiNoTimelimit()
{
    ASSERT(fakeGUI == nullptr);
    ASSERT(!stopwatch.hasTimeLimits());

    speedometer.start(simulation->getSimTime());

    int64_t lastUpdateTime = opp_get_monotonic_clock_usecs();

    printStatusUpdate();

    while (true) {
        cEvent *event = simulation->takeNextEvent();
        if (!event)
            throw cTerminationException("Scheduler interrupted while waiting");

        speedometer.addEvent(simulation->getSimTime());

        simulation->executeEvent(event);

        if ((simulation->getEventNumber() & 1023) == 0 && elapsed(statusFrequencyMs, lastUpdateTime))
            printStatusUpdate();

        if (sigintReceived)
            throw cTerminationException("SIGINT or SIGTERM received, exiting");
    }
}

void Runner::run()
{
    ASSERT(simulation == cSimulation::getActiveSimulation());
    stopwatch.resetClock();
    stopwatch.startClock();

#define FINALLY { \
        if (expressMode) \
            printStatusUpdate(); \
        stopwatch.stopClock(); \
        simulatedTime = simulation->getSimTime(); \
    }

    try {
        if (!expressMode)
            doRunNormal();
        else if (fakeGUI)
            doRunExpressWithFakeGUI();
        else if (stopwatch.hasTimeLimits())
            doRunExpressNoFakeGui();
        else
            doRunExpressNoFakeGuiNoTimelimit();
        FINALLY;
    }
    catch (std::exception& e) {
        FINALLY;
        throw;
    }
}

static char *timeToStr(double t, char *buf)
{
    int sec = (int) floor(t);
    if (t < 3600)
        sprintf(buf, "%lgs (%dm %02ds)", t, int(sec/60L), int(sec%60L));
    else if (t < 86400)
        sprintf(buf, "%lgs (%dh %02dm)", t, int(sec/3600L), int((sec%3600L)/60L));
    else
        sprintf(buf, "%lgs (%dd %02dh)", t, int(sec/86400L), int((sec%86400L)/3600L));
    return buf;
}

void Runner::printStatusUpdate()
{
    speedometer.beginNewInterval();

    if (printPerformanceData) {
        char buf[64];
        out << "** Event #" << simulation->getEventNumber()
            << "   t=" << simulation->getSimTime()
            << "   Elapsed: " << timeToStr(getElapsedSecs(), buf);
        if (printThreadId)
            out << "  Thread " << std::this_thread::get_id();
        out << "  " << getProgressPercentage() << std::endl;  // note: IDE launcher uses this to track progress

        out << "     Speed:     ev/sec=" << speedometer.getEventsPerSec()
            << "   simsec/sec=" << speedometer.getSimSecPerSec()
            << "   ev/simsec=" << speedometer.getEventsPerSimSec() << std::endl;

        out << "     Messages:  created: " << cMessage::getTotalMessageCount()
            << "   present: " << cMessage::getLiveMessageCount()
            << "   in FES: " << simulation->getFES()->getLength() << std::endl;
    }
    else {
        char buf[64];
        out << "** Event #" << simulation->getEventNumber() << "   t=" << simulation->getSimTime()
            << "   Elapsed: " << timeToStr(getElapsedSecs(), buf)
            << getProgressPercentage() // note: IDE launcher uses this to track progress
            << "   ev/sec=" << speedometer.getEventsPerSec() << std::endl;
    }

    // status update is always autoflushed (not only if opt->autoflush is on)
    out.flush();
}

std::string Runner::getProgressPercentage()
{
    double simtimeRatio = -1;
    simtime_t simtimeLimit = simulation->getSimulationTimeLimit();
    if (!simtimeLimit.isZero())
        simtimeRatio = simulation->getSimTime() / simtimeLimit;

    double elapsedTimeRatio = -1;
    double realTimeLimit = stopwatch.getRealTimeLimit();
    if (realTimeLimit > 0)
        elapsedTimeRatio = stopwatch.getElapsedSecs() / realTimeLimit;

    double cpuTimeRatio = -1;
    double cpuTimeLimit = stopwatch.getCPUTimeLimit();
    if (cpuTimeLimit > 0)
        cpuTimeRatio = stopwatch.getCPUUsageSecs() / cpuTimeLimit;

    double ratio = std::max(simtimeRatio, std::max(elapsedTimeRatio, cpuTimeRatio));
    ratio = std::min(ratio, 1.0);  // eliminate occasional "101% completed" message
    if (ratio == -1)
        return "";
    else {
        double totalRatio = (ratio + runsTried - 1) / numRuns;
        // DO NOT change the "% completed" string. The IDE launcher plugin matches
        // against this string for detecting user input
        return opp_stringf("  %d%% completed  (%d%% total)", (int)(100*ratio), (int)(100*totalRatio));
    }
    return "";
}

void Runner::printEventBanner(eventnumber_t eventNumber, cEvent *event)
{
    out << "** Event #" << eventNumber << "  t=" << event->getArrivalTime() << getProgressPercentage() << "   ";  // note: IDE launcher uses this to track progress

    if (event->isMessage()) {
        cModule *mod = static_cast<cMessage *>(event)->getArrivalModule();
        out << mod->getFullPath() << " (" << mod->getComponentType()->getName() << ", id=" << mod->getId() << ")";
    }
    else if (event->getTargetObject()) {
        cObject *target = event->getTargetObject();
        out << target->getFullPath() << " (" << target->getClassName() << ")";
    }
    if (printThreadId)
        out << "  Thread " << std::this_thread::get_id();
    out << "\n"; // note: "\n" not endl, because we don't want auto-flush on each event

    if (detailedEventBanners) {
        char buf[64];
        out << "   Elapsed: " << timeToStr(stopwatch.getElapsedSecs(), buf)
            << "   Messages: created: " << cMessage::getTotalMessageCount()
            << "  present: " << cMessage::getLiveMessageCount()
            << "  in FES: " << simulation->getFES()->getLength() << "\n"; // note: "\n" not endl, because we don't want auto-flush on each event
    }
}

}  // namespace cmdenv
}  // namespace omnetpp

