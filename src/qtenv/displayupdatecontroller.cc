//==========================================================================
//  displayupdatecontroller.cc - part of
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

#include "displayupdatecontroller.h"

#include <iomanip>

#include <QtWidgets/QApplication>
#include <QtGui/QPainter>
#include <QtCore/QThread>

#include "qtutil.h"
#include "mainwindow.h"
#include "moduleinspector.h"
#include "messageanimator.h"
#include "animationcontrollerdialog.h"
#include "common/unitconversion.h"
#include "common/fileutil.h"
#include "envir/speedometer.h"

namespace omnetpp {
using namespace common;
namespace qtenv {

bool DisplayUpdateController::animateUntilNextEvent(bool onlyHold)
{
    // if no more events, we lie that we reached it, so we notice that it's all over when we
    // try to execute the non-existent next event.
    if (!sim->guessNextEvent())
        return true;

    ASSERT(currentTimes.simTime <= simTime());
    advanceToSimTime(currentTimes, simTime());

    while (!qtenv->getStopSimulationFlag()) {
        if (runMode == RUNMODE_EXPRESS)
            return true;

        if (isPaused()) {
            QApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents, 100);
            lastGuiUpdateAt = stopwatch.getElapsedSecondsNoLoss();
            continue;
        }
        ASSERT(stopwatch.isRunning());

        if (recordingVideo) {
            bool reached = renderUntilNextEvent(onlyHold);
            if (reached)
                return true;
            else {
                if (qtenv->getStopSimulationFlag() || onlyHold)
                    return false;
            }
        }

        // so the fresh animations can begin, and we get an updated animSpeed / holdTime
        if (runMode != RUNMODE_FAST)
            msgAnim->updateAnimations();

        double animationSpeed = getAnimationSpeed();
        double holdTime = qtenv->getRemainingAnimationHoldTime();

        bool inHold = (holdTime > 0 || msgAnim->isHoldActive()) && runMode != RUNMODE_FAST;

        if (animationSpeed == 0 && !inHold) {
            // this is the old "event - frame - event - frame - event - frame" behavior
            if (runMode == RUNMODE_NORMAL)
                renderFrame(false);

            // except in FAST mode we don't have to do a frame after every event
            if (runMode == RUNMODE_FAST) {
                double timeSinceFrame = stopwatch.getElapsedSecondsNoLoss() - lastFrameAt;
                double timeSinceUpdate = stopwatch.getElapsedSecondsNoLoss() - lastGuiUpdateAt;
                if (timeSinceFrame > (1.0/minFrameRate))
                    renderFrame(false);
                else if (timeSinceUpdate > (1.0/minGuiUpdateRate)) {
                    QApplication::processEvents();
                    lastGuiUpdateAt = stopwatch.getElapsedSecondsNoLoss();
                }
            }

            return true;
        }

        // is it time to do the next event?

        const double maxDeficit = 1.0 / minFrameRate; // in real seconds

        TimeTriplet nextTimes(currentTimes);
        double deficit = 0;

        if (inHold) { // we are on a hold, simTime is paused
            nextTimes.realTime = stopwatch.getElapsedSeconds();
            double realDt = nextTimes.realTime - currentTimes.realTime;
            nextTimes.animationTime = currentTimes.animationTime + realDt * currentProfile->playbackSpeed;
        }
        else {
            if (onlyHold)
                return true;
            deficit = advanceToRealTimeLimited(nextTimes, stopwatch.getElapsedSeconds(), sim->guessNextSimtime());
        }

        currentTimes = nextTimes;
        sim->setSimTime(currentTimes.simTime);

        if (deficit > 0) {
            if (deficit > maxDeficit) {
                // we can't keep up, let's give up on some time...
                stopwatch.rewind(maxDeficit);
                renderFrame(false);
            }
            else {
                double timeSinceFrame = stopwatch.getElapsedSecondsNoLoss() - lastFrameAt;
                double timeSinceUpdate = stopwatch.getElapsedSecondsNoLoss() - lastGuiUpdateAt;
                // if the deficit hovers between 0 and maxDeficit for a long time
                // (this happens when the playback speed is just about where the
                // computer can still barely handle it without skipping)
                // we must still do some frames (or at least process Qt events)
                if (timeSinceFrame > maxDeficit)
                    renderFrame(false);
                else if (timeSinceUpdate > (1.0/minGuiUpdateRate)) {
                    QApplication::processEvents();
                    lastGuiUpdateAt = stopwatch.getElapsedSecondsNoLoss();
                }
            }
            return true;
        }

        renderFrame(false);
    }

    return false;
}

DisplayUpdateController::DisplayUpdateController()
{
    runProfile.load("run");
    fastProfile.load("fast");
}

double DisplayUpdateController::getAnimationSpeed() const
{
    // animation speed requests come from the model and the internal message animator
    double msgAnimSpeed = msgAnim->getAnimationSpeed();
    double modelAnimSpeed = qtenv->computeModelAnimationSpeedRequest();

    // if any of them is 0 (which means not in effect)
    if (modelAnimSpeed == 0.0)
        modelAnimSpeed = msgAnimSpeed;

    if (msgAnimSpeed == 0.0)
        msgAnimSpeed = modelAnimSpeed;

    double computedSpeed = std::min(msgAnimSpeed, modelAnimSpeed);

    // enforcing the two limits, given they are set (not NaNs)
    double minSpeed = currentProfile->minAnimationSpeed;
    double maxSpeed = currentProfile->maxAnimationSpeed;

    if (!std::isnan(minSpeed))
        computedSpeed = std::max(minSpeed, computedSpeed);

    if (!std::isnan(maxSpeed))
        computedSpeed = std::min(maxSpeed, computedSpeed);

    if (computedSpeed != 0.0)
        computedSpeed = std::max(computedSpeed, 1e-9);

    return computedSpeed;
}

double DisplayUpdateController::getAnimationHoldEndTime() const
{
    return std::max(msgAnim->getAnimationHoldEndTime(), qtenv->computeModelHoldEndTime());
}

void DisplayUpdateController::setRunMode(RunMode value)
{
    if (runMode == value)
        return;

    auto oldMode = runMode;
    runMode = value;
    currentProfile = &runProfile;

    ASSERT(isPaused() == !stopwatch.isRunning());

    if (isPaused())
        resume();

    switch (runMode) {
        case RUNMODE_STEP:
            //currentFps = 0;
            break;
        case RUNMODE_FAST:
            currentProfile = &fastProfile;
            break;
        case RUNMODE_NOT_RUNNING:
            msgAnim->clearDeliveries();
            pause();
            // fallthrough
        case RUNMODE_EXPRESS:
            // express clears all animations anyway
            break;
        case RUNMODE_NORMAL:
            // nothing to do
            break;
    }

    if (oldMode != runMode)
        Q_EMIT runModeChanged(runMode);

    Q_EMIT playbackSpeedChanged(getPlaybackSpeed());
}

// this is mostly (maybe even only...) called when the cSocketRTScheduler is used
void DisplayUpdateController::idle()
{
    advanceToRealTimeLimited(currentTimes, stopwatch.getElapsedSeconds(), simTime());

    renderFrame(false); // seems a bit out of place
}

void DisplayUpdateController::simulationEvent()
{
    if (!getQtenv()->isExpressMode())
        advanceToRealTimeLimited(currentTimes, stopwatch.getElapsedSeconds(), simTime());
}

void DisplayUpdateController::skipHold()
{
    currentTimes.animationTime = std::max(currentTimes.animationTime, getAnimationHoldEndTime());
}

void DisplayUpdateController::skipToNextEvent()
{
    skipHold();

    simtime_t nextGuess = sim->guessNextSimtime();
    if (nextGuess > simTime()) { // is -1 if no more events...
        advanceToSimTime(currentTimes, nextGuess);
        sim->setSimTime(nextGuess);
    }

    qtenv->callRefreshDisplaySafe();
    qtenv->callRefreshInspectors();
}

bool DisplayUpdateController::rightBeforeEvent()
{
    return !msgAnim->isHoldActive()
            && qtenv->computeModelHoldEndTime() <= currentTimes.animationTime
            /*&& simTime() == sim->guessNextSimtime()*/;
}

bool DisplayUpdateController::renderUntilNextEvent(bool onlyHold)
{
    // if no more events, we lie that we reached it, so we notice that it's all over when we
    // try to execute the non-existent next event.
    if (!sim->guessNextEvent())
        return true;

    // in animationSpeed
    double frameDelta = 1.0 / videoFps;

    while (!qtenv->getStopSimulationFlag()) {
        if (runMode == RUNMODE_EXPRESS)
            return true;

        if (!recordingVideo)
            return false;

        if (isPaused()) {
            QApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents, 100);
            lastGuiUpdateAt = stopwatch.getElapsedSecondsNoLoss();
            continue;
        }

        double holdTime = qtenv->getRemainingAnimationHoldTime();

        if (holdTime > 0 || msgAnim->isHoldActive()) { // we are on a hold, simTime is paused
            renderFrame(true);
            lastRecordedFrame = simTime();

            // TODO: make this a bit more correct (consider when the hold ends before the next frame, etc...)
            currentTimes.animationTime += frameDelta;
            currentTimes.realTime = stopwatch.getElapsedSeconds();
        }
        else {
            if (onlyHold)
                return true;

            double animationSpeed = getAnimationSpeed();

            if (animationSpeed == 0) {
                sim->setSimTime(sim->guessNextSimtime());
                if (runMode == RUNMODE_FAST) {
                    double timeSinceFrame = stopwatch.getElapsedSecondsNoLoss() - lastFrameAt;
                    double timeSinceUpdate = stopwatch.getElapsedSecondsNoLoss() - lastGuiUpdateAt;
                    if (timeSinceFrame > (1.0/minFrameRate)) {
                        lastRecordedFrame = simTime();
                        currentTimes.realTime = stopwatch.getElapsedSeconds();
                        renderFrame(true);
                    }
                    else if (timeSinceUpdate > (1.0/minGuiUpdateRate)) {
                        QApplication::processEvents();
                        lastGuiUpdateAt = stopwatch.getElapsedSecondsNoLoss();
                    }
                } else {
                    lastRecordedFrame = simTime();
                    currentTimes.realTime = stopwatch.getElapsedSeconds();
                    renderFrame(true);
                }
                return true; // to get the old behaviour back
            }

            SimTime nextEvent = sim->guessNextSimtime();
            SimTime frameDeltaSimTime = frameDelta * animationSpeed * currentProfile->playbackSpeed;

            if (lastRecordedFrame < simTime() - frameDeltaSimTime) {
                // there was a skip. most likely the recording was disabled for a while,
                // or has just been enabled for the first time
                lastRecordedFrame = simTime();
                currentTimes.realTime = stopwatch.getElapsedSeconds();
                renderFrame(true);
            }

            SimTime nextFrame = lastRecordedFrame + frameDeltaSimTime;

            // simTime can be moved forward even without event (skipping, etc...)
            if (simTime() > nextFrame)
                nextFrame = simTime();

            if (nextEvent <= nextFrame) {
                // the time has come to execute the next event

                // TODO: compute the real animation delta time for the actual currentEvent->arrivalTime() - simTime delta
                //animationTime += animDeltaTime;
                return true;
            }
            else {
                ASSERT(nextFrame >= simTime()); // this became a bit redundant, but can't hurt
                sim->setSimTime(nextFrame);
                lastRecordedFrame = nextFrame;
                currentTimes.realTime = stopwatch.getElapsedSeconds();
                renderFrame(true);
            }
        }
    }

    return false;
}

void DisplayUpdateController::advanceToSimTime(TimeTriplet &triplet, SimTime simTarget)
{
    ASSERT(simTarget >= triplet.simTime);

    SimTime simDt = simTarget - triplet.simTime;

    double animSpeed = getAnimationSpeed();

    double animDt = (animSpeed == 0) ? 0 : simDt.dbl() / animSpeed;

    triplet.simTime += simDt;
    triplet.animationTime += animDt;

    ASSERT(triplet.simTime == simTarget);
}

double DisplayUpdateController::advanceToRealTimeLimited(TimeTriplet &triplet, double realTarget, SimTime simLimit)
{
    ASSERT(triplet.realTime <= realTarget);
    ASSERT(triplet.simTime <= simLimit);

    SimTime simResidual = SimTime::ZERO;

    double animSpeed = getAnimationSpeed();

    double realDt = realTarget - triplet.realTime;
    double animDt = realDt * currentProfile->playbackSpeed;

    SimTime simDt;
    SimTime newSim;

    try {
        simDt = animDt * animSpeed;
        newSim = triplet.simTime + simDt;
    }
    catch (cRuntimeError&) {
        simDt = SimTime::getMaxTime();
        newSim = SimTime::getMaxTime();
    }

    if (newSim > simLimit) {
        simResidual = newSim - simLimit;
        newSim = simLimit;

        simDt = newSim - triplet.simTime;

        if (animSpeed > 0) {
            animDt = simDt.dbl() / animSpeed;
            realDt = animDt / currentProfile->playbackSpeed;
        }
    }

    triplet.simTime += simDt;
    triplet.animationTime += animDt;
    triplet.realTime += realDt;

    ASSERT(triplet.simTime <= simLimit);
    ASSERT(simResidual >= 0);
    double factor = animSpeed * currentProfile->playbackSpeed;
    return simResidual.dbl() / (factor == 0 ? 1.0 : factor);
}

void DisplayUpdateController::recordFrame()
{
    char temp[200];

    auto mainWin = qtenv->getMainWindow();
    auto mainInsp = qtenv->getMainModuleInspector();
    auto frame = mainWin->grab().toImage();

    const char *targetUnit = "s";
    simtime_t now = simTime();
    QPainter painter(&frame);

    // disabling alpha blending
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    auto inspPos = mainInsp->mapTo(mainWin, (mainInsp->geometry() - mainInsp->contentsMargins()).topLeft());
    painter.drawImage(inspPos, mainInsp->getScreenshot());
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    painter.setFont(QFont("Arial", 28));
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setPen(colors::BLACK);
    painter.setBackground(colors::LIGHTGREY);
    snprintf(temp, sizeof(temp), "time: %s s", now.str().c_str());
    //painter.drawText(size.width() - 380, 200, temp);
    simtime_t frameTime = now - lastRecordedFrame;
    if (frameTime.inUnit(SimTimeUnit::SIMTIME_MS) > 0)
        targetUnit = "ms";
    else if (frameTime.inUnit(SimTimeUnit::SIMTIME_US) > 0)
        targetUnit = "us";
    else if (frameTime.inUnit(SimTimeUnit::SIMTIME_NS) > 0)
        targetUnit = "ns";
    else if (frameTime.inUnit(SimTimeUnit::SIMTIME_PS) > 0)
        targetUnit = "ps";
    double v = UnitConversion::convertUnit(frameTime.dbl(), "s", targetUnit);

    snprintf(temp, sizeof(temp), "delta: %.3g %s", v, targetUnit);
    //painter.drawText(frame.size().width() - 380, 250, temp);
    std::stringstream ss;
    ss << filenameBase << std::setw(4) << std::setfill('0') << frameCount << ".png";
    mkPath(directoryOf(ss.str().c_str()).c_str());
    frame.save(ss.str().c_str());
    frameCount++;
}

void DisplayUpdateController::showDialog(QAction *action)
{
    if (!dialog) {
        dialog = new AnimationControllerDialog(qtenv->getMainWindow());
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->show();
        dialog->activateWindow();
        connect(dialog, &QDialog::destroyed, [this, action]() {
            dialog = nullptr;
            action->setChecked(false);
        });
        connect(this, &DisplayUpdateController::runModeChanged,
                dialog, &AnimationControllerDialog::switchToRunMode);
    }
}

void DisplayUpdateController::hideDialog()
{
    if (dialog) {
        delete dialog;
        dialog = nullptr;
    }
}

void DisplayUpdateController::setPlaybackSpeed(double speed)
{
    currentProfile->setPlaybackSpeed(speed);
    if (dialog) {
        dialog->displayControlValues();
        dialog->displayMetrics();
    }

    Q_EMIT playbackSpeedChanged(speed);
}

void DisplayUpdateController::setPlaybackSpeed(double speed, RunModeProfile *profile)
{
    ASSERT(profile == &runProfile || profile == &fastProfile);

    profile->setPlaybackSpeed(speed);

    if (profile == currentProfile) {
        if (dialog) {
            dialog->displayControlValues();
            dialog->displayMetrics();
        }

        Q_EMIT playbackSpeedChanged(speed);
    }
}

void DisplayUpdateController::reset()
{
    currentProfile = &runProfile;

    runMode = RUNMODE_NOT_RUNNING;

    videoFps = 30; // the framerate of the recorded video

    currentTimes = TimeTriplet();

    stopwatch.reset();
    pausedCount = 0;
    pause();

    recordingVideo = false; // a simple state variable

    lastIntervalAt = 0;
    lastFrameAt = 0;
    lastGuiUpdateAt = 0;
    frameTimes.clear();
    frameCount = 0; // this will be the sequence number of the next recorded frame
    lastRecordedFrame = -SimTime::getMaxTime(); // used in rendering mode, this stores the last SimTime we recorded, incremented by constant amounts

    filenameBase = "frames/"; // the prefix of the frame files' path

    Q_EMIT playbackSpeedChanged(getPlaybackSpeed());

    if (dialog)
        dialog->displayMetrics();
}

DisplayUpdateController::~DisplayUpdateController()
{
    hideDialog();
    runProfile.save("run");
    fastProfile.save("fast");
}

void DisplayUpdateController::renderFrame(bool record)
{
    const double now = stopwatch.getElapsedSecondsNoLoss();

    if (runMode != RUNMODE_STEP && lastIntervalAt < now - 0.1) {
        qtenv->getSpeedometer().beginNewInterval();
        lastIntervalAt = now;
    }

    if (dialog)
        dialog->displayMetrics();

    msgAnim->updateAnimations();

    qtenv->updateSimtimeDisplay();
    qtenv->updateStatusDisplay();

    // We have to call the "unsafe" versions (without exception handling) here,
    // because we are in the simulation event loop, we have to let that handle it.
    qtenv->callRefreshDisplay();
    qtenv->refreshInspectors();

    QApplication::processEvents();

    lastGuiUpdateAt = now;
    lastFrameAt = now;

    double holdTime = qtenv->getRemainingAnimationHoldTime();
    bool inHold = (holdTime > 0 || msgAnim->isHoldActive()) && runMode != RUNMODE_FAST;

    frameTimes.push_back(TimeTriplet(now, getAnimationTime(), simTime(),
                                      inHold ? 0 : (getAnimationSpeed() * getPlaybackSpeed())));
    const int maxNumFrameTimes = 10;
    const int minNumFrameTimes = 5;
    const double frameTimeRetirement = 0.5; // seconds
    while (frameTimes.size() > maxNumFrameTimes ||
           ((frameTimes.size() > minNumFrameTimes) && (now - frameTimes.front().realTime) > frameTimeRetirement))
        frameTimes.pop_front();

    if (record)
        recordFrame();
}

double DisplayUpdateController::getCurrentFps() const
{
    if (frameTimes.size() < 2)
        return 0;

    return (frameTimes.size()-1.0) / (frameTimes.back().realTime - frameTimes.front().realTime);
}

double DisplayUpdateController::getCurrentSimulationSpeed() const
{
    if (frameTimes.size() < 2)
        return 0;

    SimTime simSecs = (frameTimes.back().simTime - frameTimes.front().simTime);
    double secs = (frameTimes.back().realTime - frameTimes.front().realTime);

    return simSecs.dbl() / secs;
}

bool DisplayUpdateController::effectiveAnimationSpeedRecentlyChanged() const
{
    if (frameTimes.size() < 2)
        return true;

    double currEffAnimSpeed = getAnimationSpeed() * getPlaybackSpeed();

    for (size_t i = 0; i < frameTimes.size(); ++i)
        if (frameTimes[i].effectiveAnimationSpeed != currEffAnimSpeed)
            return true;

    return false;
}

void RunModeProfile::save(const QString &prefix)
{
    auto qtenv = getQtenv();

    qtenv->setPref("RunModeProfiles/" + prefix + "-playback-speed", playbackSpeed);
    qtenv->setPref("RunModeProfiles/" + prefix + "-min-playback-speed", minPlaybackSpeed);
    qtenv->setPref("RunModeProfiles/" + prefix + "-max-playback-speed", maxPlaybackSpeed);
    qtenv->setPref("RunModeProfiles/" + prefix + "-min-animation-speed", minAnimationSpeed);
    qtenv->setPref("RunModeProfiles/" + prefix + "-max-animation-speed", maxAnimationSpeed);
}

void RunModeProfile::load(const QString &prefix)
{
    auto qtenv = getQtenv();

    playbackSpeed = qtenv->getPref("RunModeProfiles/" + prefix + "-playback-speed", playbackSpeed).toDouble();
    minPlaybackSpeed = qtenv->getPref("RunModeProfiles/" + prefix + "-min-playback-speed", minPlaybackSpeed).toDouble();
    maxPlaybackSpeed = qtenv->getPref("RunModeProfiles/" + prefix + "-max-playback-speed", maxPlaybackSpeed).toDouble();
    minAnimationSpeed = qtenv->getPref("RunModeProfiles/" + prefix + "-min-animation-speed", minAnimationSpeed).toDouble();
    maxAnimationSpeed = qtenv->getPref("RunModeProfiles/" + prefix + "-max-animation-speed", maxAnimationSpeed).toDouble();
}

}  // namespace qtenv
}  // namespace omnetpp
