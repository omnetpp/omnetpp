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
#include "common/stringutil.h"
#include "omnetpp/cevent.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "ifakegui.h"
#include "runner.h"
#include "genericenvir.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace envir {

void Runner::configure(cConfiguration *cfg)
{
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

        if (simulation->hasRealTimeLimit())
            simulation->checkRealTimeLimits();

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
            if (simulation->hasRealTimeLimit())
                simulation->checkRealTimeLimits();
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
            if (simulation->hasRealTimeLimit())
                simulation->checkRealTimeLimits();
        }

        if (sigintReceived)
            throw cTerminationException("SIGINT or SIGTERM received, exiting");
    }
}

void Runner::doRunExpressNoFakeGuiNoTimelimit()
{
    ASSERT(fakeGUI == nullptr);
    ASSERT(!simulation->hasRealTimeLimit());

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

void Runner::doRunExpressNoStatusUpdates()
{
    ASSERT(fakeGUI == nullptr);
    ASSERT(!simulation->hasRealTimeLimit());
    ASSERT(statusFrequencyMs <= 0);

    while (true) {
        cEvent *event = simulation->takeNextEvent();
        if (!event)
            throw cTerminationException("Scheduler interrupted while waiting");

        simulation->executeEvent(event);

        if (sigintReceived)
            throw cTerminationException("SIGINT or SIGTERM received, exiting");
    }
}

void Runner::run()
{
    ASSERT(simulation == cSimulation::getActiveSimulation());

    if (GenericEnvir *envir = dynamic_cast<GenericEnvir*>(simulation->getEnvir()))
        setFakeGUI(envir->getFakeGui());

#define FINALLY { \
        if (expressMode) \
            printStatusUpdate(); \
        simulatedTime = simulation->getSimTime(); \
    }

    try {
        if (!expressMode)
            doRunNormal();
        else if (fakeGUI)
            doRunExpressWithFakeGUI();
        else if (simulation->hasRealTimeLimit())
            doRunExpressNoFakeGui();
        else if (statusFrequencyMs > 0)
            doRunExpressNoFakeGuiNoTimelimit();
        else
            doRunExpressNoStatusUpdates();
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
            << "   Elapsed: " << timeToStr(simulation->getElapsedTime(), buf);
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
            << "   Elapsed: " << timeToStr(simulation->getElapsedTime(), buf)
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
    if (simtimeLimit != SIMTIME_ZERO)
        simtimeRatio = simulation->getSimTime() / simtimeLimit;

    double elapsedTimeRatio = -1;
    double realTimeLimit = simulation->getElapsedTimeLimit();
    if (realTimeLimit > 0)
        elapsedTimeRatio = simulation->getElapsedTime() / realTimeLimit;

    double cpuTimeRatio = -1;
    double cpuTimeLimit = simulation->getCpuTimeLimit();
    if (cpuTimeLimit > 0)
        cpuTimeRatio = simulation->getCpuUsageTime() / cpuTimeLimit;

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
        out << "   Elapsed: " << timeToStr(simulation->getElapsedTime(), buf)
            << "   Messages: created: " << cMessage::getTotalMessageCount()
            << "  present: " << cMessage::getLiveMessageCount()
            << "  in FES: " << simulation->getFES()->getLength() << "\n"; // note: "\n" not endl, because we don't want auto-flush on each event
    }
}

}  // namespace envir
}  // namespace omnetpp

