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

#include <QApplication>
#include <QPainter>
#include <QThread>

#include "mainwindow.h"
#include "moduleinspector.h"
#include "messageanimator.h"
#include "animationcontrollerdialog.h"
#include "common/unitconversion.h"
#include "common/fileutil.h"
#include "envir/speedometer.h"

#define emit

namespace omnetpp {
using namespace common;
namespace qtenv {

bool DisplayUpdateController::animateUntilNextEvent(bool onlyHold)
{
    // if no more events, we lie that we reached it, so we notice that it's all over when we
    // try to execute the non-existent next event.
    if (!sim->guessNextEvent())
        return true;

    if (!animationTimer.isValid())
        animationTimer.start();

    while (!qtenv->getStopSimulationFlag()) {
        if (runMode == RUNMODE_EXPRESS)
            return true;

        if (paused) {
            QApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents, 100);
            continue;
        }

        if (recordingVideo) {
            bool reached = renderUntilNextEvent(onlyHold);
            if (reached)
                return true;
            else {
                if (qtenv->getStopSimulationFlag() || onlyHold)
                    return false;
                animationTimer.restart();
            }
        }

        double animationSpeed = getAnimationSpeed();
        double lastStepAt = std::max(lastFrameAt, lastEventAt);

        double nextFrameAt = lastFrameAt + 1.0 / targetFps;
        double nextEventAt;

        if (animationSpeed == 0)
            nextEventAt = now;
        else {
            if (runMode == RUNMODE_FAST) {
                double eventdelta = (sim->guessNextSimtime() - lastExecutedEvent).dbl() / (animationSpeed * currentProfile->playbackSpeed);
                nextEventAt = lastEventAt + eventdelta;
            }
            else {
                double eventdelta = (sim->guessNextSimtime() - simTime()).dbl() / (animationSpeed * currentProfile->playbackSpeed);
                nextEventAt = now + eventdelta;
            }
        }

        now += animationTimer.nsecsElapsed() / 1.0e9;
        animationTimer.restart();

        now = std::max(now, std::max(lastEventAt, lastFrameAt));

        double nextStepAt = std::min(nextFrameAt, nextEventAt);

        if (nextStepAt > (now + 0.001)) {
            QApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents,
                                        (nextStepAt - now) * 1000);
            continue;
        }

        double holdTime = qtenv->getRemainingAnimationHoldTime();
        if ((holdTime > 0 || msgAnim->isHoldActive())
                && runMode != RUNMODE_FAST) { // we are on a hold, simTime is paused
            animationTime += std::min(std::max(0.0, holdTime), (now - lastStepAt) * currentProfile->playbackSpeed);
            adjustFrameRate(renderFrame(false));

            lastEventAt += now - lastFrameAt; // dragging this marker with us, so we can keep a correct pace when the hold ends
            lastEventAt = std::min(now, lastEventAt);
        }
        else {
            if (onlyHold)
                return true;

            if (animationSpeed == 0 && runMode != RUNMODE_FAST) {
                sim->setSimTime(sim->guessNextSimtime());
                renderFrame(false);
                return true; // to get the old behaviour back
            }

            if (nextEventAt <= nextFrameAt || nextEventAt <= now) { // the time has come to execute the next event
                // TODO: compute the real animation delta time for the actual currentEvent->arrivalTime() - simTime delta
                //animationTime += animDeltaTime;

                // let's set to time there already
                sim->setSimTime(sim->guessNextSimtime());

                // but even before letting the event happen, we still have to churn out a frame
                if (nextEventAt > nextFrameAt)
                    adjustFrameRate(renderFrame(false));
                return true;
            }
            else {
                // this is the case when the event is far away, and we have time to animate peacefully
                animationTime += (now - lastStepAt) * currentProfile->playbackSpeed;

                ASSERT2(now >= lastStepAt, ("now is " + QString::number(now) + ", lastStepAt is " + QString::number(lastStepAt)).toStdString().c_str());

                double simtimeplus = (now - lastStepAt) * animationSpeed * currentProfile->playbackSpeed;

                auto nextSt = simTime() + simtimeplus;
                auto nextGuess = sim->guessNextSimtime();

                if (nextGuess >= SIMTIME_ZERO && nextSt > nextGuess)
                    nextSt = nextGuess;

                sim->setSimTime(nextSt);

                adjustFrameRate(renderFrame(false));
            }
        }
    }

    return false;
}

void DisplayUpdateController::setTargetFps(double fps)
{
    targetFps = clip(currentProfile->minFps, fps, currentProfile->maxFps);
}

DisplayUpdateController::DisplayUpdateController()
{
    animationTimer.restart();
    setTargetFps(maxPossibleFps * currentProfile->targetAnimationCpuUsage);

    runProfile.load("run");
    fastProfile.load("fast");
}

double DisplayUpdateController::getAnimationSpeed() const
{
    double animSpeed = msgAnim->getAnimationSpeed();
    double modelSpeed = qtenv->computeModelAnimationSpeedRequest();

    if (modelSpeed == 0.0)
        modelSpeed = animSpeed;

    if (animSpeed == 0.0)
        animSpeed = modelSpeed;

    double computedSpeed = std::min(animSpeed, modelSpeed);

    double minSpeed = currentProfile->minAnimationSpeed;
    double maxSpeed = currentProfile->maxAnimationSpeed;

    if (!std::isnan(minSpeed))
        computedSpeed = std::max(minSpeed, computedSpeed);

    if (!std::isnan(maxSpeed))
        computedSpeed = std::min(maxSpeed, computedSpeed);

    return computedSpeed;
}

double DisplayUpdateController::getAnimationHoldEndTime() const
{
    return std::max(msgAnim->getAnimationHoldEndTime(),
            qtenv->computeModelHoldEndTime());
}

void DisplayUpdateController::setRunMode(RunMode value)
{
    auto oldMode = runMode;
    runMode = value;
    currentProfile = &runProfile;

    switch (runMode) {
        case RUNMODE_STEP:
            //currentFps = 0;
            break;
        case RUNMODE_FAST:
            currentProfile = &fastProfile;
            msgAnim->clear();
            break;
        case RUNMODE_NOT_RUNNING:
            msgAnim->clearDeliveries();
            // fallthrough
        case RUNMODE_EXPRESS:
            // express clears all animations anyway
            currentFps = 0;
            animationTimer.invalidate();
            break;
        case RUNMODE_NORMAL:
            // nothing to do
            break;
    }

    if (oldMode != runMode)
        emit runModeChanged(runMode);

    emit playbackSpeedChanged(getPlaybackSpeed());
}

void DisplayUpdateController::skipHold()
{
    animationTime = std::max(animationTime, getAnimationHoldEndTime());
}

void DisplayUpdateController::skipToNextEvent()
{
    skipHold();
    simtime_t nextGuess = sim->guessNextSimtime();
    if (nextGuess > simTime()) // is -1 if no more events...
        sim->setSimTime(nextGuess);

    qtenv->callRefreshDisplaySafe();
    qtenv->callRefreshInspectors();
}

bool DisplayUpdateController::rightBeforeEvent()
{
    return !msgAnim->isHoldActive()
            && qtenv->computeModelHoldEndTime() <= animationTime
            && simTime() == sim->guessNextSimtime();
}

void DisplayUpdateController::startVideoRecording()
{
    animationTimer.restart();
    recordingVideo = true;
}

void DisplayUpdateController::stopVideoRecording()
{
    animationTimer.restart();
    recordingVideo = false;
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

        if (paused) {
            QApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents, 100);
            continue;
        }

        double holdTime = qtenv->getRemainingAnimationHoldTime();

        if (holdTime > 0 || msgAnim->isHoldActive()) { // we are on a hold, simTime is paused
            animationTime += frameDelta; // TODO account for the cases where there is less hold left than frameDelta
            renderFrame(true);
        }
        else {
            if (onlyHold)
                return true;

            double animationSpeed = getAnimationSpeed();

            if (animationSpeed == 0) {
                sim->setSimTime(sim->guessNextSimtime());
                if (runMode == RUNMODE_FAST) {
                    if (animationTimer.elapsed() >= 1000.0 / targetFps) {
                        animationTime += frameDelta;
                        lastRecordedFrame = simTime();
                        renderFrame(true);
                        animationTimer.restart();
                    }
                } else {
                    animationTime += frameDelta;
                    lastRecordedFrame = simTime();
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
                // this is the case when the event is far away, and we have time to animate peacefully
                animationTime += frameDelta;
                ASSERT(nextFrame >= simTime()); // this became a bit redundant, but can't hurt
                sim->setSimTime(nextFrame);
                renderFrame(true);
                lastRecordedFrame = nextFrame;
                animationTimer.restart();
            }
        }
    }

    return false;
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
    painter.setPen(QColor("black"));
    painter.setBackground(QBrush(QColor("#eeeeee")));
    sprintf(temp, "time: %s s", now.str().c_str());
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


    sprintf(temp, "delta: %.3g %s", v, targetUnit);
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

    emit playbackSpeedChanged(speed);
}

void DisplayUpdateController::simulationEvent(cEvent *event)
{
    lastExecutedEvent = event->getArrivalTime();
    lastEventAt = now + animationTimer.nsecsElapsed() / 10.0e9;
}

void DisplayUpdateController::reset()
{
    animationTimer.restart();

    currentProfile = &runProfile;
    setTargetFps(maxPossibleFps * currentProfile->targetAnimationCpuUsage);

    runMode = RUNMODE_NOT_RUNNING;

    animationTime = 0; // this is The Animation Time

    currentFps = targetFps;
    maxPossibleFps = currentProfile->maxFps;

    fpsMemory = 0.0;

    videoFps = 30; // the framerate of the recorded video

    now = 0; // this is a "real time" stopwatch, only incremented if the simulation is not stopped

    lastFrameAt = 0; // these are in realTime (calculated from "now")
    lastEventAt = 0;

    lastExecutedEvent = SIMTIME_ZERO; // this is used to compute the adaptive nonlinear mapping

    recordingVideo = false; // a simple state variable

    frameCount = 0; // this will be the sequence number of the next recorded frame
    lastRecordedFrame = -SimTime::getMaxTime(); // used in rendering mode, this stores the last SimTime we recorded, incremented by constant amounts

    filenameBase = "frames/"; // the prefix of the frame files' path

    emit playbackSpeedChanged(getPlaybackSpeed());

    if (dialog)
        dialog->displayMetrics();
}

DisplayUpdateController::~DisplayUpdateController()
{
    hideDialog();
    runProfile.save("run");
    fastProfile.save("fast");
}

double DisplayUpdateController::renderFrame(bool record)
{
    if (runMode != RUNMODE_STEP)
        qtenv->getSpeedometer().beginNewInterval();

    double frameDelta = frameTimer.isValid() ? (frameTimer.nsecsElapsed() / 1.0e9) : 0;
    frameTimer.restart();

    if (dialog)
        dialog->displayMetrics();

    msgAnim->updateAnimations();

    qtenv->updateSimtimeDisplay();
    qtenv->updateStatusDisplay();

    qtenv->callRefreshDisplay();
    qtenv->refreshInspectors();
    // We have to call the "unsafe" versions (without exception handling) here,
    // because we are in the simulation event loop, we have to let that handle it.

    QApplication::processEvents();

    lastFrameAt = now;

    if (record)
        recordFrame();

    double frameTime = frameTimer.nsecsElapsed() / 1.0e9;

    maxPossibleFps = fpsMemory * maxPossibleFps + (1-fpsMemory) * (1.0/frameTime);

    double totalTime = frameDelta + frameTime;
    currentFps = fpsMemory * currentFps + (1-fpsMemory) * (1.0/totalTime);

    frameTimer.restart();
    return frameTime;
}

void DisplayUpdateController::adjustFrameRate(float frameTime)
{
    setTargetFps(targetFps * fpsMemory + ((1.0 / frameTime) * currentProfile->targetAnimationCpuUsage) * (1-fpsMemory));
}

void RunModeProfile::save(const QString &prefix)
{
    auto qtenv = getQtenv();

    qtenv->setPref("RunModeProfiles/" + prefix + "-target-cpu-usage", targetAnimationCpuUsage);
    qtenv->setPref("RunModeProfiles/" + prefix + "-min-fps", minFps);
    qtenv->setPref("RunModeProfiles/" + prefix + "-max-fps", maxFps);
    qtenv->setPref("RunModeProfiles/" + prefix + "-playback-speed", playbackSpeed);
    qtenv->setPref("RunModeProfiles/" + prefix + "-min-playback-speed", minPlaybackSpeed);
    qtenv->setPref("RunModeProfiles/" + prefix + "-max-playback-speed", maxPlaybackSpeed);
    qtenv->setPref("RunModeProfiles/" + prefix + "-min-animation-speed", minAnimationSpeed);
    qtenv->setPref("RunModeProfiles/" + prefix + "-max-animation-speed", maxAnimationSpeed);
}

void RunModeProfile::load(const QString &prefix)
{
    auto qtenv = getQtenv();

    targetAnimationCpuUsage = qtenv->getPref("RunModeProfiles/" + prefix + "-target-cpu-usage", targetAnimationCpuUsage).toDouble();
    minFps = qtenv->getPref("RunModeProfiles/" + prefix + "-min-fps", minFps).toDouble();
    maxFps = qtenv->getPref("RunModeProfiles/" + prefix + "-max-fps", maxFps).toDouble();
    playbackSpeed = qtenv->getPref("RunModeProfiles/" + prefix + "-playback-speed", playbackSpeed).toDouble();
    minPlaybackSpeed = qtenv->getPref("RunModeProfiles/" + prefix + "-min-playback-speed", minPlaybackSpeed).toDouble();
    maxPlaybackSpeed = qtenv->getPref("RunModeProfiles/" + prefix + "-max-playback-speed", maxPlaybackSpeed).toDouble();
    minAnimationSpeed = qtenv->getPref("RunModeProfiles/" + prefix + "-min-animation-speed", minAnimationSpeed).toDouble();
    maxAnimationSpeed = qtenv->getPref("RunModeProfiles/" + prefix + "-max-animation-speed", maxAnimationSpeed).toDouble();
}

} // namespace qtenv
} // namespace omnetpp
