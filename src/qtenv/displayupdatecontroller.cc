//==========================================================================
//  displayupdatecontroller.cc - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

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
    if (!animationTimer.isValid())
        animationTimer.start();

    while (!qtenv->getStopSimulationFlag()) {
        if (runMode == RUNMODE_EXPRESS)
            return true;

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

        if (!isExplicitAnimationSpeed())
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
        if ((holdTime > 0 || qtenv->getMessageAnimator()->isHoldActive())
                && runMode != RUNMODE_FAST) { // we are on a hold, simTime is paused
            animationTime += std::min(std::max(0.0, holdTime), (now - lastStepAt) * currentProfile->playbackSpeed);
            adjustFrameRate(renderFrame(false));

            lastEventAt += now - lastFrameAt; // dragging this marker with us, so we can keep a correct pace when the hold ends
            lastEventAt = std::min(now, lastEventAt);
        }
        else {
            if (onlyHold)
                return true;

            if (!isExplicitAnimationSpeed() && runMode != RUNMODE_FAST) {
                renderFrame(false);
                return true; // to get the old behaviour back
            }

            if (nextEventAt <= nextFrameAt || nextEventAt <= now) { // the time has come to execute the next event
                // TODO: compute the real animation delta time for the actual currentEvent->arrivalTime() - simTime delta
                //animationTime += animDeltaTime;
                // but even before that, we still have to churn out a frame
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
    double animSpeed = qtenv->getMessageAnimator()->getAnimationSpeed();
    double modelSpeed = qtenv->computeModelAnimationSpeedRequest();

    if (modelSpeed == 0.0)
        modelSpeed = animSpeed;

    if (animSpeed == 0.0)
        animSpeed = modelSpeed;

    if (animSpeed == 0.0)
        return 0;

    return clip(currentProfile->minAnimationSpeed,
                std::min(animSpeed, modelSpeed),
                currentProfile->maxAnimationSpeed);
}

double DisplayUpdateController::getAnimationHoldEndTime() const
{
    return std::max(qtenv->getMessageAnimator()->getAnimationHoldEndTime(),
            qtenv->computeModelHoldEndTime());
}

bool DisplayUpdateController::isExplicitAnimationSpeed()
{
    return qtenv->computeModelAnimationSpeedRequest() != 0.0
            || qtenv->getMessageAnimator()->getAnimationSpeed() != 0.0;
}

void DisplayUpdateController::setRunMode(RunMode value)
{
    runMode = value;
    currentProfile = &runProfile;

    switch (runMode) {
        case RUNMODE_STEP:
            currentFps = 0;
            break;
        case RUNMODE_FAST:
            currentProfile = &fastProfile;
            break;
        case RUNMODE_NOT_RUNNING:
        case RUNMODE_EXPRESS:
            currentFps = 0;
            animationTimer.invalidate();
            break;
        case RUNMODE_NORMAL:
            // nothing to do
            break;
    }

    if (dialog)
        dialog->displayMetrics();

    emit playbackSpeedChanged(getPlaybackSpeed());
}

void DisplayUpdateController::startVideoRecording()
{
    animationTimer.restart();
    recordingVideo = true;
    lastRecordedFrame = simTime();
    // XXX maybe record a frame right here?
}

void DisplayUpdateController::stopVideoRecording()
{
    animationTimer.restart();
    recordingVideo = false;
}

bool DisplayUpdateController::renderUntilNextEvent(bool onlyHold)
{
    double frameDelta = 1.0 / videoFps;

    while (!qtenv->getStopSimulationFlag()) {
        if (runMode == RUNMODE_EXPRESS)
            return true;

        if (!recordingVideo)
            return false;

        double animationSpeed = getAnimationSpeed();

        double holdTime = qtenv->getRemainingAnimationHoldTime();
        if (holdTime > 0 || qtenv->getMessageAnimator()->isHoldActive()) { // we are on a hold, simTime is paused
            animationTime += frameDelta; // TODO account for the cases where there is less hold left than frameDelta
            renderFrame(true);
        }
        else {
            if (onlyHold)
                return true;

            if (!isExplicitAnimationSpeed() && runMode != RUNMODE_FAST) {
                renderFrame(true);
                return true; // to get the old behaviour back
            }

            SimTime nextEvent = sim->guessNextSimtime();
            SimTime nextFrame = lastRecordedFrame + frameDelta * animationSpeed * currentProfile->playbackSpeed;

            if (nextEvent <= nextFrame) {
                // the time has come to execute the next event

                // TODO: compute the real animation delta time for the actual currentEvent->arrivalTime() - simTime delta
                //animationTime += animDeltaTime;

                // showing a frame for the user, but not recording it.
                // XXX is this needed?
                if (animationTimer.elapsed() >= 1000.0 / currentProfile->minFps) {
                    renderFrame(false);
                    animationTimer.restart();
                }
                return true;
            }
            else {
                // this is the case when the event is far away, and we have time to animate peacefully
                animationTime += frameDelta;
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
    if (dialog)
        dialog->displayMetrics();

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

    runMode = RUNMODE_NORMAL;
    hideDialog();

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
    lastRecordedFrame = 0; // used in rendering mode, this stores the last SimTime we recorded, incremented by constant amounts

    filenameBase = "frames/"; // the prefix of the frame files' path

    emit playbackSpeedChanged(getPlaybackSpeed());
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

    qtenv->getMessageAnimator()->update();

    qtenv->callRefreshDisplay();
    qtenv->updateSimtimeDisplay();
    qtenv->updateStatusDisplay();
    qtenv->refreshInspectors();
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

    qtenv->setPref("runmodeprofile-" + prefix + "-target-cpu-usage", targetAnimationCpuUsage);
    qtenv->setPref("runmodeprofile-" + prefix + "-min-fps", minFps);
    qtenv->setPref("runmodeprofile-" + prefix + "-max-fps", maxFps);
    qtenv->setPref("runmodeprofile-" + prefix + "-playback-speed", playbackSpeed);
    qtenv->setPref("runmodeprofile-" + prefix + "-min-playback-speed", minPlaybackSpeed);
    qtenv->setPref("runmodeprofile-" + prefix + "-max-playback-speed", maxPlaybackSpeed);
    qtenv->setPref("runmodeprofile-" + prefix + "-min-animation-speed", minAnimationSpeed);
    qtenv->setPref("runmodeprofile-" + prefix + "-max-animation-speed", maxAnimationSpeed);
}

void RunModeProfile::load(const QString &prefix)
{
    auto qtenv = getQtenv();

    targetAnimationCpuUsage = qtenv->getPref("runmodeprofile-" + prefix + "-target-cpu-usage", targetAnimationCpuUsage).toDouble();
    minFps = qtenv->getPref("runmodeprofile-" + prefix + "-min-fps", minFps).toDouble();
    maxFps = qtenv->getPref("runmodeprofile-" + prefix + "-max-fps", maxFps).toDouble();
    playbackSpeed = qtenv->getPref("runmodeprofile-" + prefix + "-playback-speed", playbackSpeed).toDouble();
    minPlaybackSpeed = qtenv->getPref("runmodeprofile-" + prefix + "-min-playback-speed", minPlaybackSpeed).toDouble();
    maxPlaybackSpeed = qtenv->getPref("runmodeprofile-" + prefix + "-max-playback-speed", maxPlaybackSpeed).toDouble();
    minAnimationSpeed = qtenv->getPref("runmodeprofile-" + prefix + "-min-animation-speed", minAnimationSpeed).toDouble();
    maxAnimationSpeed = qtenv->getPref("runmodeprofile-" + prefix + "-max-animation-speed", maxAnimationSpeed).toDouble();
}

} // namespace qtenv
} // namespace omnetpp
