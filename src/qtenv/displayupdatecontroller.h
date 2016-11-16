//==========================================================================
//  displayupdatecontroller.h - part of
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

#ifndef __OMNETPP_QTENV_DISPLAYUPDATECONTROLLER_H
#define __OMNETPP_QTENV_DISPLAYUPDATECONTROLLER_H

#include <QElapsedTimer>
#include <QToolButton>

#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

class AnimationControllerDialog;

struct RunModeProfile {
    void save(const QString& prefix);
    void load(const QString& prefix);

    double targetAnimationCpuUsage;
    double minFps, maxFps; // these limit the adaptive mechanism

    double minPlaybackSpeed, maxPlaybackSpeed;
    double playbackSpeed;

    double minAnimationSpeed, maxAnimationSpeed;

    void setPlaybackSpeed(double speed) {
        playbackSpeed = clip(minPlaybackSpeed, speed, maxPlaybackSpeed);
    }
};

class DisplayUpdateController : public QObject
{
    Q_OBJECT

    // simple shortcuts
    cSimulation *sim = getSimulation();
    Qtenv *qtenv = getQtenv();

    // factory defaults
    RunModeProfile runProfile  = {0.8,   2.0, 60.0, 1e-2, 1e2, 1,    1e-15, 1e6};
    RunModeProfile fastProfile = {0.05, 10.0, 30.0, 1e0,  1e6, 1000, 1e-3,  1e12};
    RunModeProfile *currentProfile = &runProfile;

    RunMode runMode = RUNMODE_NORMAL;

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
    simtime_t lastRecordedFrame = 0; // used in rendering mode, this stores the last SimTime we recorded, incremented by constant amounts

    std::string filenameBase = "frames/"; // the prefix of the frame files' path

    void adjustFrameRate(float frameTime); // will adjust the targetFps according to the parameters, based on how much the rendering of a frame took
    double renderFrame(bool record); // will update many parts of the gui to reflect the current state
    void recordFrame(); // captures the main window and saves the frame into a PNG file with the next sequence number

    // this is called by animateUntilNextEvent if recordingVideo is true
    bool renderUntilNextEvent(bool onlyHold); // returns true if really reached the time for the next event, false if interrupted/stopped before
    bool animateUntilNextEvent(bool onlyHold); // returns true if really reached the time for the next event, false if interrupted/stopped before

    void setTargetFps(double fps); // obeys limits

signals:
    void playbackSpeedChanged(double speed);

public slots:
    void setPlaybackSpeed(double speed);

public:
    DisplayUpdateController();

    double getAnimationTime() const { return animationTime; }
    double getAnimationSpeed() const;
    double getAnimationHoldEndTime() const;

    void setRunMode(RunMode value);

    // will delegate to renderUntilNextEvent if recordingVideo is true
    // returns true if really reached the time for the next event, false if interrupted/stopped before
    bool animateUntilNextEvent() { return animateUntilNextEvent(false); }
    bool animateUntilHoldEnds() { return animateUntilNextEvent(true); }

    void startVideoRecording();
    void stopVideoRecording();

    std::string getFilenameBase() const { return filenameBase; }
    void setFilenameBase(const char *base) { filenameBase = base; }

    double getVideoFps() const { return videoFps; }
    void setVideoFps(double value) { videoFps = value; }

    void showDialog(QAction *action);
    void hideDialog();

    double getMinPlaybackSpeed() { return currentProfile->minPlaybackSpeed; }
    double getMaxPlaybackSpeed() { return currentProfile->maxPlaybackSpeed; }
    double getPlaybackSpeed() { return currentProfile->playbackSpeed; }

    double getMinFps() const { return currentProfile->minFps; }
    void setMinFps(double value) { currentProfile->minFps = value; setTargetFps(targetFps); }

    double getMaxFps() const { return currentProfile->maxFps; }
    void setMaxFps(double value) { currentProfile->maxFps = value; setTargetFps(targetFps); }

    int getFrameCount() const { return frameCount; }

    double getTargetFps() const { return targetFps; }
    double getCurrentFps() const { return currentFps; }
    double getMaxPossibleFps() const { return maxPossibleFps; }

    void simulationEvent(cEvent *event);

    void reset();

    ~DisplayUpdateController();
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_DISPLAYUPDATECONTROLLER_H
