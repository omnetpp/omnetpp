//==========================================================================
//  FAKEGUI.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <algorithm>

#include "fakegui.h"
#include "common/lcgrandom.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace cmdenv {

Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_BEFORE_EVENT_PROBABILITY, "cmdenv-fake-gui-before-event-probability", CFG_DOUBLE, "1", "When `cmdenv-fake-gui=true`: TODO.");
Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_AFTER_EVENT_PROBABILITY, "cmdenv-fake-gui-after-event-probability", CFG_DOUBLE, "1", "When `cmdenv-fake-gui=true`: TODO.");
Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_HOLD_NUMSTEPS_MIN, "cmdenv-fake-gui-hold-numsteps-min", CFG_INT, "0", "When `cmdenv-fake-gui=true`: TODO.");
Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_HOLD_NUMSTEPS_MAX, "cmdenv-fake-gui-hold-numsteps-max", CFG_INT, "3", "When `cmdenv-fake-gui=true`: TODO.");
Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_SIMTIME_NUMSTEPS_MIN, "cmdenv-fake-gui-simtime-numsteps-min", CFG_INT, "0", "When `cmdenv-fake-gui=true`: TODO.");
Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_SIMTIME_NUMSTEPS_MAX, "cmdenv-fake-gui-simtime-numsteps-max", CFG_INT, "3", "When `cmdenv-fake-gui=true`: TODO.");

void FakeGUI::readConfigOptions(cConfiguration *cfg)
{
    beforeEventProbability = cfg->getAsDouble(CFGID_CMDENV_FAKEGUI_BEFORE_EVENT_PROBABILITY);
    afterEventProbability = cfg->getAsDouble(CFGID_CMDENV_FAKEGUI_AFTER_EVENT_PROBABILITY);
    holdNumStepsMin = cfg->getAsInt(CFGID_CMDENV_FAKEGUI_HOLD_NUMSTEPS_MIN);
    holdNumStepsMax = cfg->getAsInt(CFGID_CMDENV_FAKEGUI_HOLD_NUMSTEPS_MAX);
    simtimeNumStepsMin = cfg->getAsInt(CFGID_CMDENV_FAKEGUI_SIMTIME_NUMSTEPS_MIN);
    simtimeNumStepsMax = cfg->getAsInt(CFGID_CMDENV_FAKEGUI_SIMTIME_NUMSTEPS_MAX);
}

void FakeGUI::beforeEvent(cEvent *event)
{
    animateUntil(event->getArrivalTime());

    if (rng.next01() < beforeEventProbability)
        callRefreshDisplay("before event");

    if (debug)
        std::cout << "*** processing event at\tt=" << getSimulation()->getSimTime() << "\tat=" << getAnimationTime() << "\t'" << event->getFullName() << "'" << endl;
}

void FakeGUI::afterEvent()
{
    if (rng.next01() < afterEventProbability)
        callRefreshDisplay("after event");

    animateHold();
}

void FakeGUI::callRefreshDisplay(const char *reason)
{
    if (debug)
        std::cout << "--- refreshDisplay() at\tt=" << getSimulation()->getSimTime() << "\tat=" << getAnimationTime() << "\t" << reason << endl;
    getSimulation()->callRefreshDisplay();
}

void FakeGUI::animateUntil(simtime_t targetSimTime)
{
    simtime_t simtimeInterval = targetSimTime - getSimulation()->getSimTime();
    if (getAnimationSpeed() != 0 && simtimeInterval > 0) {
        int numSteps = getSimulationNumFrames(simtimeInterval);
        if (numSteps > 0) {
            simtime_t simtimeStep = simtimeInterval / numSteps;
            simtime_t t = getSimulation()->getSimTime();
            while (true) {
                t += simtimeStep;
                if (t >= targetSimTime)
                    break;
                getSimulation()->setSimTime(t);
                lastUpdateSimTime = t;
                double animSpeed = getAnimationSpeed();
                if (animSpeed == 0)
                    break;
                animationTime += simtimeStep.dbl() / animSpeed;
                callRefreshDisplay("during simtime progression");
                animateHold();
            }
        }
    }

    simtime_t remainingSimtime = targetSimTime - lastUpdateSimTime;
    double animSpeed = getAnimationSpeed();
    if (animSpeed != 0)
        animationTime += remainingSimtime.dbl() / animSpeed;
    lastUpdateSimTime = targetSimTime;
}

void FakeGUI::animateHold()
{
    double animationHoldEndTime = getAnimationHoldEndTime();
    if (animationHoldEndTime <= animationTime)
        return;

    double animationTimeDelta = animationHoldEndTime - getAnimationTime();
    int numSteps = getHoldNumFrames(animationTimeDelta);
    double animationTimeStep = animationTimeDelta / numSteps;
    for (int i = 0; i < numSteps; i++) {
        animationTime += animationTimeStep;
        callRefreshDisplay("during hold");
    }

    animationTime = std::max(animationTime, getAnimationHoldEndTime());
    lastUpdateSimTime = getSimulation()->getSimTime();
}

int FakeGUI::getHoldNumFrames(double animationTimeInterval)
{
    return holdNumStepsMin + rng.draw(holdNumStepsMax-holdNumStepsMin);
}

int FakeGUI::getSimulationNumFrames(simtime_t simtimeInterval)
{
    return simtimeNumStepsMin + rng.draw(simtimeNumStepsMax-simtimeNumStepsMin);
}

double FakeGUI::getAnimationTime() const
{
    return animationTime;
}

double FakeGUI::getAnimationHoldEndTime() const
{
    cCanvas *mainCanvas = getSimulation()->getSystemModule()->getCanvasIfExists();
    return mainCanvas ? mainCanvas->getAnimationHoldEndTime() : 0;
}

double FakeGUI::getAnimationSpeed() const
{
    cCanvas *mainCanvas = getSimulation()->getSystemModule()->getCanvasIfExists();
    if (!mainCanvas)
        return 0;
    const auto& animationSpeeds = mainCanvas->getAnimationSpeedMap();
    if (animationSpeeds.empty())
        return 0;
    double min = INFINITY;
    for (auto pair : mainCanvas->getAnimationSpeedMap())
        if (min > pair.second)
            min = pair.second;
    return min;
}

double FakeGUI::getRemainingAnimationHoldTime() const
{
    double animationHoldEndTime = getAnimationHoldEndTime();
    double delta = animationHoldEndTime - getAnimationTime(); // negative if endTime already passed
    return delta < 0 ? 0 : delta;
}

void FakeGUI::getImageSize(const char *imageName, double& outWidth, double& outHeight)
{
    outWidth = outHeight = 32;
}

void FakeGUI::getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent)
{
    outWidth = 10 * strlen(text);
    outHeight = 12;
    outAscent = 8;
}

void FakeGUI::appendToImagePath(const char *directory)
{
}

void FakeGUI::loadImage(const char *fileName, const char *imageName)
{
}

cFigure::Rectangle FakeGUI::getSubmoduleBounds(const cModule *submodule)
{
    double x = 1000 * rng.next01();
    double y = 1000 * rng.next01();
    double width = 32;
    double height = 32;
    return cFigure::Rectangle(x-width/2, y-height/2, x+width/2, y+height/2); // note: result could be cached
}

std::vector<cFigure::Point> FakeGUI::getConnectionLine(const cGate *sourceGate)
{
    double x1 = 1000 * rng.next01();
    double y1 = 1000 * rng.next01();
    double x2 = 1000 * rng.next01();
    double y2 = 1000 * rng.next01();
    return { cFigure::Point(x1,y1), cFigure::Point(x2,y2) }; // note: result could be cached
}

double FakeGUI::getZoomLevel(const cModule *module)
{
     return 1;
}

}  // namespace cmdenv
}  // namespace omnetpp

