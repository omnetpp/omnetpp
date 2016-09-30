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
#include "common/unitconversion.h"
#include "common/fileutil.h"
#include "envir/speedometer.h"

namespace omnetpp {
using namespace common;
namespace qtenv {

bool DisplayUpdateController::animateUntilNextEvent(bool onlyHold)
{
    while (!qtenv->getStopSimulationFlag()) {
        if (recordingVideo) {
            lastRecordedFrame = simTime();
            bool reached = renderUntilNextEvent(onlyHold);
            if (reached)
                return true;
            else {
                if (qtenv->getStopSimulationFlag() || onlyHold)
                    return false;
                animationTimer.restart();
            }
        }

        targetAnimationCpuUsage = (runMode == RUNMODE_FAST) ? 0.01 : 0.8;

        now += animationTimer.nsecsElapsed() / 1.0e9;
        animationTimer.restart();

        double animationSpeed = qtenv->getAnimationSpeed();
        double animationSpeedFactor = qtenv->opt->playbackSpeed;

        double lastStepAt = std::max(lastFrameAt, lastEventAt);

        double nextFrameAt = lastFrameAt + 1.0 / targetFps;
        double nextEventAt = lastEventAt + (sim->guessNextSimtime() - lastExecutedEvent).dbl() / (animationSpeed * animationSpeedFactor);

        if (runMode == RUNMODE_FAST)
            nextEventAt = now;

        double nextStepAt = std::min(nextFrameAt, nextEventAt);

        if (nextStepAt > now) {
            QApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents,
                                        (nextStepAt - now) * 100); // intentionally not 1000
            continue;
        }

        double holdTime = qtenv->getRemainingAnimationHoldTime();
        if (holdTime > 0 && runMode != RUNMODE_FAST) { // we are on a hold, simTime is paused
            animationTime += std::min(holdTime, (now - lastStepAt) * animationSpeedFactor);
            adjustFrameRate(renderFrame(false));

            lastEventAt += now - lastFrameAt; // dragging this marker with us, so we can keep a correct pace when the hold ends
            lastEventAt = std::min(now, lastEventAt);

            lastFrameAt = now;
        }
        else {
            if (onlyHold)
                return true;

            if (animationSpeed == 0.0 && runMode != RUNMODE_FAST) {
                renderFrame(false);
                return true; // to get the old behaviour back
            }

            if (nextEventAt <= nextFrameAt) { // the time has come to execute the next event

                // TODO: compute the real animation delta time for the actual currentEvent->arrivalTime() - simTime delta
                //animationTime += animDeltaTime;

                lastEventAt = now;

                if (runMode != RUNMODE_FAST) {
                    qtenv->updateStatusDisplay();
                    qtenv->updateSimtimeDisplay();
                    QApplication::processEvents();
                }

                return true;
            }
            else {
                // this is the case when the event is far away, and we have time to animate peacefully
                animationTime += (now - lastStepAt) * animationSpeedFactor;

                ASSERT(now >= lastStepAt);

                double simtimeplus = (now - lastStepAt) * animationSpeedFactor * animationSpeed;

                auto nextSt = simTime() + simtimeplus;
                auto nextGuess = sim->guessNextSimtime();


                if (nextGuess >= SIMTIME_ZERO && nextSt > nextGuess) {
                    lastEventAt = now;
                    lastExecutedEvent = nextGuess;
                    return true;
                }

                sim->setSimTime(nextSt);

                lastFrameAt = now;
                adjustFrameRate(renderFrame(false));
            }
        }
    }

    return false;
}

void DisplayUpdateController::setTargetFps(double fps)
{
    targetFps = std::min(maxFps, std::max(minFps, fps));
}

void DisplayUpdateController::setRunMode(RunMode value)
{
    runMode = value;
    if (runMode == RUNMODE_STEP || runMode == RUNMODE_NOT_RUNNING)
        currentFps = 0;

    if (dialog)
        dialog->displayMetrics();
}

bool DisplayUpdateController::renderUntilNextEvent(bool onlyHold)
{
    if (runMode == RUNMODE_FAST) {
        renderFrame(true);
        return true;
    }

    double frameDelta = 1.0 / videoFps;

    while (!qtenv->getStopSimulationFlag()) {
        if (!recordingVideo)
            return false;

        double animationSpeed = qtenv->getAnimationSpeed();

        double holdTime = qtenv->getRemainingAnimationHoldTime();
        if (holdTime > 0) { // we are on a hold, simTime is paused
            animationTime += frameDelta; // TODO account for the cases where there is less hold left than frameDelta
            renderFrame(true);
        }
        else {
            if (onlyHold)
                return true;

            if (animationSpeed == 0.0) {
                renderFrame(true);
                return true; // to get the old behaviour back
            }

            SimTime nextEvent = sim->guessNextSimtime();
            SimTime nextFrame = lastRecordedFrame + frameDelta * animationSpeed;

            if (nextEvent <= nextFrame) {
                // the time has come to execute the next event

                // TODO: compute the real animation delta time for the actual currentEvent->arrivalTime() - simTime delta
                //animationTime += animDeltaTime;

                lastEventAt = now;
                renderFrame(false);
                return true;
            }
            else {
                // this is the case when the event is far away, and we have time to animate peacefully
                animationTime += frameDelta;
                sim->setSimTime(nextFrame);
                renderFrame(true);
                lastRecordedFrame = nextFrame;
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
    auto frame = mainWin->grab();

    const char *targetUnit = "s";
    simtime_t now = simTime();
    QPainter painter(&frame);
    painter.drawPixmap(mainInsp->mapTo(mainWin, (mainInsp->geometry() - mainInsp->contentsMargins()).topLeft()),
                       mainInsp->getScreenshot());
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

void DisplayUpdateController::holdSimulationFor(double s)
{
    animationHoldEndTime = std::max(animationHoldEndTime, animationTime + s);
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

void DisplayUpdateController::simulationEvent(cEvent *event)
{
    lastExecutedEvent = event->getArrivalTime();
    lastEventAt = now;
}

double DisplayUpdateController::renderFrame(bool record) {
    if (runMode != RUNMODE_STEP)
        qtenv->getSpeedometer().beginNewInterval();

    double frameDelta = frameTimer.isValid() ? (frameTimer.nsecsElapsed() / 1.0e9) : 0;
    frameTimer.restart();

    if (dialog)
        dialog->displayMetrics();

    if (qtenv->animating)
        qtenv->getMessageAnimator()->frame();

    qtenv->callRefreshDisplay();
    qtenv->updateSimtimeDisplay();
    qtenv->updateStatusDisplay();
    qtenv->refreshInspectors();
    QApplication::processEvents();

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
    setTargetFps(targetFps * fpsMemory + ((1.0 / frameTime) * targetAnimationCpuUsage) * (1-fpsMemory));
}

} // namespace qtenv
} // namespace omnetpp
