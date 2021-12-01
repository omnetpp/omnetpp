//==========================================================================
//  FAKEGUI.CC - part of
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
#include "sim/nedsupport.h"

using namespace omnetpp::common;
using namespace omnetpp::nedsupport;

namespace omnetpp {
namespace cmdenv {

Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_SEED, "cmdenv-fake-gui-seed", CFG_INT, "1", "When `cmdenv-fake-gui=true`: The seed for the RNG governing the operation of the fake GUI component. This is entirely independent of the RNGs used by the model.");
Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_BEFORE_EVENT_PROBABILITY, "cmdenv-fake-gui-before-event-probability", CFG_DOUBLE, "1", "When `cmdenv-fake-gui=true`: The probability with which `refreshDisplay()` is called before each event.");
Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_AFTER_EVENT_PROBABILITY, "cmdenv-fake-gui-after-event-probability", CFG_DOUBLE, "1", "When `cmdenv-fake-gui=true`: The probability with which `refreshDisplay()` is called after each event.");
Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_ON_HOLD_PROBABILITY, "cmdenv-fake-gui-on-hold-probability", CFG_DOUBLE, "0.5", "When `cmdenv-fake-gui=true`: The probability with which `refreshDisplay()` is called (possibly multiple times, see `cmdenv-fake-gui-on-hold-numsteps`) during a \"hold\" period (animation during which simulation time does not advance).");
Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_ON_HOLD_NUMSTEPS, "cmdenv-fake-gui-on-hold-numsteps", CFG_CUSTOM, "3", "When `cmdenv-fake-gui=true`: The number of times `refreshDisplay()` is called during a \"hold\" period (animation during which simulation time does not advance), provided a trial with `cmdenv-fake-gui-on-hold-probability` yielded success. This an expression which will be evaluated each time, so it can be random. Zero is also a valid value.");
Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_ON_SIMTIME_PROBABILITY, "cmdenv-fake-gui-on-simtime-probability", CFG_DOUBLE, "0.1", "When `cmdenv-fake-gui=true`: The probability with which `refreshDisplay()` is called (possibly multiple times, see `cmdenv-fake-gui-on-simtime-numsteps`) when simulation time advances from one simulation event to the next.");
Register_PerRunConfigOption(CFGID_CMDENV_FAKEGUI_ON_SIMTIME_NUMSTEPS, "cmdenv-fake-gui-on-simtime-numsteps", CFG_CUSTOM, "3", "When `cmdenv-fake-gui=true`: The number of times `refreshDisplay()` is called when simulation time advances from one simulation event to the next, provided a trial with `cmdenv-fake-gui-on-simtime-probability` yielded success. This an expression which will be evaluated each time, so it can be random. Zero is also a valid value.");

void FakeGUI::readConfigOptions(cConfiguration *cfg)
{
    rng.setSeed(cfg->getAsInt(CFGID_CMDENV_FAKEGUI_SEED));

    beforeEventProbability = cfg->getAsDouble(CFGID_CMDENV_FAKEGUI_BEFORE_EVENT_PROBABILITY);
    afterEventProbability = cfg->getAsDouble(CFGID_CMDENV_FAKEGUI_AFTER_EVENT_PROBABILITY);
    onHoldProbability = cfg->getAsDouble(CFGID_CMDENV_FAKEGUI_ON_HOLD_PROBABILITY);

    parseExpression(cfg, CFGID_CMDENV_FAKEGUI_ON_HOLD_NUMSTEPS, onHoldNumSteps);
    onSimtimeProbability = cfg->getAsDouble(CFGID_CMDENV_FAKEGUI_ON_SIMTIME_PROBABILITY);
    parseExpression(cfg, CFGID_CMDENV_FAKEGUI_ON_SIMTIME_NUMSTEPS, onSimtimeNumSteps);
}

Expression::ExprNode *FakeGUI::Translator::createIdentNode(const char *varName, bool withIndex)
{
    if (std::string(varName) == "dt") {
        auto fakeGui = this->fakeGui; // note: [=] will only capture local variables and "this", but not members
        return new LambdaVariableNode("dt", [fakeGui](Context*) {return fakeGui->getCurrentTimeDelta();});
    }
    return nullptr;
}

Expression::ExprNode *FakeGUI::Translator::createFunctionNode(const char *functionName, int argCount)
{
    return nullptr; //TODO int(), uniform(), normal(), exponential(), etc.
}

void FakeGUI::parseExpression(cConfiguration *cfg, cConfigOption *configOption, common::Expression& expression)
{
    try {
        const char *text = cfg->getAsCustom(configOption, "");
        Translator extraTranslator(this);
        Expression::MultiAstTranslator translator({ &extraTranslator, Expression::getDefaultAstTranslator() });
        expression.parse(text, &translator);
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error in config option '%s': %s", configOption->getName(), e.what());
    }
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

int FakeGUI::getHoldNumFrames(double dt)
{
    if (rng.next01() >= onHoldProbability)
        return 0;
    this->isWithinHold = true;
    this->animationTimeDelta = dt;
    return std::min((intval_t)1000, onHoldNumSteps.intValue());
}

int FakeGUI::getSimulationNumFrames(simtime_t dt)
{
    if (rng.next01() >= onSimtimeProbability)
        return 0;
    this->isWithinHold = false;
    this->simulationTimeDelta = dt;
    return std::min((intval_t)1000, onSimtimeNumSteps.intValue());
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
    return mainCanvas ? mainCanvas->getMinAnimationSpeed() : 0;
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

