//==========================================================================
//  displayupdatecontroller.h - part of
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

#ifndef __OMNETPP_QTENV_DISPLAYUPDATECONTROLLER_H
#define __OMNETPP_QTENV_DISPLAYUPDATECONTROLLER_H

#include <deque>

#include <QtCore/QElapsedTimer>
#include <QtWidgets/QToolButton>

#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

class AnimationControllerDialog;

struct QTENV_API RunModeProfile {
    void save(const QString& prefix);
    void load(const QString& prefix);

    // these bound the value coming from the user/msganim, or NaN if not
    double minAnimationSpeed, maxAnimationSpeed;

    // the range of the slider on the toolbar
    double minPlaybackSpeed, maxPlaybackSpeed;

    // the current position of the slider
    double playbackSpeed;

    void setPlaybackSpeed(double speed) {
        playbackSpeed = clip(minPlaybackSpeed, speed, maxPlaybackSpeed);
    }
};

class StopWatch {
    int64_t lostNSecs = 0;
    int64_t pausedNSecs = 0;
    int64_t pausedAt = 0;

    bool running = true;
    QElapsedTimer timer;
public:
    StopWatch() { timer.start(); }

    void reset() { timer.restart(); pausedNSecs = 0; pausedAt = 0; lostNSecs = 0; running = true; }

    void rewind(double secs) { lostNSecs += secs * 1.0e9; }

    void pause() { ASSERT(running); pausedAt = timer.nsecsElapsed(); running = false; }
    void resume() { ASSERT(!running); pausedNSecs += timer.nsecsElapsed() - pausedAt; running = true; }

    bool isRunning() { return running; }
    double getElapsedSeconds() { return ((running ? timer.nsecsElapsed() : pausedAt) - pausedNSecs - lostNSecs) / 1.0e9; }
    double getElapsedSecondsNoLoss() { return ((running ? timer.nsecsElapsed() : pausedAt) - pausedNSecs) / 1.0e9; }
};

class QTENV_API DisplayUpdateController : public QObject
{
    Q_OBJECT

    struct TimeTriplet {
        double realTime = 0; // can still have "pauses" for a few reasons
        double animationTime = 0;
        SimTime simTime = SimTime::ZERO;

        // animationSpeed * playbackSpeed recorder at the time of the frame,
        // only used for FPS/simSpeed measurement
        double effectiveAnimationSpeed = NAN;

        TimeTriplet() {}
        TimeTriplet(double rt, double at, SimTime st, double eas):
            realTime(rt), animationTime(at), simTime(st), effectiveAnimationSpeed(eas) { }

        QString str() {
            return QString("real: %1 anim: %2 sim: %3")
                    .arg(realTime).arg(animationTime).arg(simTime.str().c_str());
        }
    };

    TimeTriplet currentTimes;

    // simple shortcuts
    cSimulation *sim = getSimulation();
    Qtenv *qtenv = getQtenv();
    MessageAnimator *msgAnim = qtenv->getMessageAnimator();

    // factory defaults
    RunModeProfile runProfile  = {std::nan(""), std::nan(""), 1e-2, 1e2, 1};
    RunModeProfile fastProfile = {std::nan(""), std::nan(""), 1e0,  1e6, 1000};
    RunModeProfile *currentProfile = &runProfile;

    RunMode runMode = RUNMODE_NOT_RUNNING;

    AnimationControllerDialog *dialog = nullptr;

    double videoFps = 30; // the framerate of the recorded video

    // if positive, just stop the progression of animationTime, but when zero, go on with whatever was
    // going on before - useful for when context menus, modal dialogs, data editors are shown, etc...
    // not a simple bool because "pausing" can happen because the simulation is not running, or when
    // an editor is open in the g.o.i. for example. Must not be negative.
    int pausedCount = 0;

    StopWatch stopwatch;

    const double minFrameRate = 2; // frames per second
    // to keep the GUI responsive even when we are not doing "real" display
    // updates (with refreshDisplay()) because we are lagging behind
    const double minGuiUpdateRate = 10; // event processing iterations per second

    double lastIntervalAt = 0;
    double lastFrameAt = 0;
    double lastGuiUpdateAt = 0;
    std::deque<TimeTriplet> frameTimes; // in lossless stopwatch time, to calculate fps

    bool recordingVideo = false; // a simple state variable

    int frameCount = 0; // this will be the sequence number of the next recorded frame
    simtime_t lastRecordedFrame = -SimTime::getMaxTime(); // used in rendering mode, this stores the last SimTime we recorded, incremented by constant amounts

    std::string filenameBase = "frames/"; // the prefix of the frame files' path

    void renderFrame(bool record); // will update many parts of the gui to reflect the current state
    void recordFrame(); // captures the main window and saves the frame into a PNG file with the next sequence number

    // this is called by animateUntilNextEvent if recordingVideo is true
    bool renderUntilNextEvent(bool onlyHold); // returns true if really reached the time for the next event, false if interrupted/stopped before
    // will delegate to renderUntilNextEvent if recordingVideo is true
    bool animateUntilNextEvent(bool onlyHold); // returns true if really reached the time for the next event, false if interrupted/stopped before

    void advanceToSimTime(TimeTriplet &triplet, SimTime simTarget); // not touching realtime
    double advanceToRealTimeLimited(TimeTriplet &triplet, double realTarget, SimTime simLimit);

signals:
    void playbackSpeedChanged(double speed);
    void runModeChanged(RunMode mode);

public slots:
    void setPlaybackSpeed(double speed);
    void setPlaybackSpeed(double speed, RunModeProfile *profile);

    void pause() { if (pausedCount == 0) stopwatch.pause(); ++pausedCount; }
    void resume() { ASSERT(pausedCount > 0); --pausedCount; if (pausedCount == 0) stopwatch.resume(); }

public:
    DisplayUpdateController();

    double getCurrentFps() const;
    double getCurrentSimulationSpeed() const;
    bool effectiveAnimationSpeedRecentlyChanged() const;

    double getAnimationTime() const { return currentTimes.animationTime; }
    double getAnimationSpeed() const;
    double getAnimationHoldEndTime() const;

    void setRunMode(RunMode value);

    // returns true if really reached the time for the next event, false if interrupted/stopped before
    bool animateUntilNextEvent() { return animateUntilNextEvent(false); }
    bool animateUntilHoldEnds() { return animateUntilNextEvent(true); }

    void idle();
    void simulationEvent();

    void skipHold(); // sets animationTime to the end of the current hold, effectively ending it
    void skipToNextEvent(); // the above, plus sets simTime to that of the next event

    bool isPaused() { return pausedCount > 0; }

    // true if nothing (builtin anim, model hold, simtime anim) is "waiting to happen" before the next event
    bool rightBeforeEvent();

    void startVideoRecording() { recordingVideo = true; }
    void stopVideoRecording() { recordingVideo = false; }

    std::string getFilenameBase() const { return filenameBase; }
    void setFilenameBase(const char *base) { filenameBase = base; }

    double getVideoFps() const { return videoFps; }
    void setVideoFps(double value) { videoFps = value; }

    void showDialog(QAction *action);
    void hideDialog();

    RunModeProfile *getCurrentProfile() { return currentProfile; }
    RunModeProfile *getStepRunProfile() { return &runProfile; }
    RunModeProfile *getFastProfile() { return &fastProfile; }

    double getMinPlaybackSpeed() const { return currentProfile->minPlaybackSpeed; }
    double getMaxPlaybackSpeed() const { return currentProfile->maxPlaybackSpeed; }
    double getPlaybackSpeed() const { return currentProfile->playbackSpeed; }

    void setMinAnimSpeed(double value) { currentProfile->minAnimationSpeed = value; }
    void setMaxAnimSpeed(double value) { currentProfile->maxAnimationSpeed = value; }

    int getFrameCount() const { return frameCount; }

    void reset();

    ~DisplayUpdateController();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_DISPLAYUPDATECONTROLLER_H
