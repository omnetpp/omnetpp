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

#include <QElapsedTimer>
#include <QToolButton>

#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

class AnimationControllerDialog;

struct QTENV_API RunModeProfile {
    void save(const QString& prefix);
    void load(const QString& prefix);

    double targetAnimationCpuUsage;
    double minFps, maxFps; // these limit the adaptive mechanism
    double minAnimationSpeed, maxAnimationSpeed; // bound the value coming from the user/msganim, or NaN if not

    double minPlaybackSpeed, maxPlaybackSpeed;
    double playbackSpeed;

    void setPlaybackSpeed(double speed) {
        playbackSpeed = clip(minPlaybackSpeed, speed, maxPlaybackSpeed);
    }
};

class QTENV_API DisplayUpdateController : public QObject
{
    Q_OBJECT

    bool paused = false;

    // simple shortcuts
    cSimulation *sim = getSimulation();
    Qtenv *qtenv = getQtenv();
    MessageAnimator *msgAnim = qtenv->getMessageAnimator();

    // factory defaults
    RunModeProfile runProfile  = {0.8,   2.0, 60.0, std::nan(""), std::nan(""), 1e-2, 1e2, 1};
    RunModeProfile fastProfile = {0.05, 10.0, 30.0, std::nan(""), std::nan(""), 1e0,  1e6, 1000 };
    RunModeProfile *currentProfile = &runProfile;

    RunMode runMode = RUNMODE_NOT_RUNNING;

    AnimationControllerDialog *dialog = nullptr;

    double animationTime = 0; // this is The Animation Time

    double targetFps = 30; // this is what we try to achieve to maintain smoothness / cpu usage
    double currentFps = targetFps; // this is the actual measured framerate
    double maxPossibleFps = currentProfile->maxFps; // this is what the computer could achieve at most

    double fpsMemory = 0.0;

    double videoFps = 30; // the framerate of the recorded video

    QElapsedTimer frameTimer; // used to measure frame time and fps
    QElapsedTimer animationTimer; // used to advance the animationTime
    double now = 0; // this is a "real time" stopwatch, only incremented if the simulation is not stopped

    double lastFrameAt = 0; // these are in realTime (calculated from "now")
    double lastEventAt = 0;

    SimTime lastExecutedEvent = SIMTIME_ZERO; // this is used to compute the adaptive nonlinear mapping

    bool recordingVideo = false; // a simple state variable

    int frameCount = 0; // this will be the sequence number of the next recorded frame
    simtime_t lastRecordedFrame = -SimTime::getMaxTime(); // used in rendering mode, this stores the last SimTime we recorded, incremented by constant amounts

    std::string filenameBase = "frames/"; // the prefix of the frame files' path

    void adjustFrameRate(float frameTime); // will adjust the targetFps according to the parameters, based on how much the rendering of a frame took
    double renderFrame(bool record); // will update many parts of the gui to reflect the current state
    void recordFrame(); // captures the main window and saves the frame into a PNG file with the next sequence number

    // this is called by animateUntilNextEvent if recordingVideo is true
    bool renderUntilNextEvent(bool onlyHold); // returns true if really reached the time for the next event, false if interrupted/stopped before
    // will delegate to renderUntilNextEvent if recordingVideo is true
    bool animateUntilNextEvent(bool onlyHold); // returns true if really reached the time for the next event, false if interrupted/stopped before

    void setTargetFps(double fps); // obeys limits

signals:
    void playbackSpeedChanged(double speed);
    void runModeChanged(RunMode mode);

public slots:
    void setPlaybackSpeed(double speed);

    void pause() { animationTimer.invalidate(); paused = true; }
    void resume() { animationTimer.invalidate(); paused = false; }

public:
    DisplayUpdateController();

    double getAnimationTime() const { return animationTime; }
    double getAnimationSpeed() const;
    double getAnimationHoldEndTime() const;

    void setRunMode(RunMode value);

    // returns true if really reached the time for the next event, false if interrupted/stopped before
    bool animateUntilNextEvent() { return animateUntilNextEvent(false); }
    bool animateUntilHoldEnds() { return animateUntilNextEvent(true); }

    void skipHold(); // sets animationTime to the end of the current hold, effectively ending it
    void skipToNextEvent(); // the above, plus sets simTime to that of the next event

    bool isPaused() { return paused; }

    // true if nothing (builtin anim, model hold, simtime anim) is "waiting to happen" before the next event
    bool rightBeforeEvent();

    void startVideoRecording();
    void stopVideoRecording();

    std::string getFilenameBase() const { return filenameBase; }
    void setFilenameBase(const char *base) { filenameBase = base; }

    double getVideoFps() const { return videoFps; }
    void setVideoFps(double value) { videoFps = value; }

    void showDialog(QAction *action);
    void hideDialog();

    RunModeProfile *getCurrentProfile() { return currentProfile; }
    RunModeProfile *getStepRunProfile() { return &runProfile; }
    RunModeProfile *getFastProfile() { return &fastProfile; }

    double getMinPlaybackSpeed() { return currentProfile->minPlaybackSpeed; }
    double getMaxPlaybackSpeed() { return currentProfile->maxPlaybackSpeed; }
    double getPlaybackSpeed() { return currentProfile->playbackSpeed; }

    void setMinAnimSpeed(double value) { currentProfile->minAnimationSpeed = value; }
    void setMaxAnimSpeed(double value) { currentProfile->maxAnimationSpeed = value; }

    int getFrameCount() const { return frameCount; }
    double getCurrentFps() const { return currentFps; }

    void simulationEvent(cEvent *event);

    void reset();

    ~DisplayUpdateController();
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_DISPLAYUPDATECONTROLLER_H
