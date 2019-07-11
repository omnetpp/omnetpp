//==========================================================================
//  FAKEGUI.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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

#ifndef __OMNETPP_CMDENV_FAKEGUI_H
#define __OMNETPP_CMDENV_FAKEGUI_H

#include "cmddefs.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cevent.h"
#include "omnetpp/cconfiguration.h"
#include "common/lcgrandom.h"

namespace omnetpp {
namespace cmdenv {

/**
 * Fake GUI for Cmdenv.
 */
class CMDENV_API FakeGUI
{
   private:
    bool debug = false;
    double beforeEventProbability;
    double afterEventProbability;
    int holdNumStepsMin;
    int holdNumStepsMax;
    int simtimeNumStepsMin;
    int simtimeNumStepsMax;

    // state
    common::LCGRandom rng; // for randomizing calls to refreshDisplay()
    double animationTime = 0;
    simtime_t lastUpdateSimTime;

   protected:
     virtual double getAnimationHoldEndTime() const;
     virtual void animateHold();
     virtual void animateUntil(simtime_t targetSimTime);
     virtual int getHoldNumFrames(double animationTimeInterval);
     virtual int getSimulationNumFrames(simtime_t simtimeInterval);
     virtual void callRefreshDisplay(const char *reason);

   public:
     virtual ~FakeGUI() {}
     virtual void readConfigOptions(cConfiguration *cfg);
     virtual void beforeEvent(cEvent *event);
     virtual void afterEvent();
     virtual void getImageSize(const char *imageName, double& outWidth, double& outHeight);
     virtual void getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent);
     virtual void appendToImagePath(const char *directory);
     virtual void loadImage(const char *fileName, const char *imageName=nullptr);
     virtual cFigure::Rectangle getSubmoduleBounds(const cModule *submodule);
     virtual std::vector<cFigure::Point> getConnectionLine(const cGate *sourceGate);
     virtual double getZoomLevel(const cModule *module);
     virtual double getAnimationTime() const;
     virtual double getAnimationSpeed() const;
     virtual double getRemainingAnimationHoldTime() const;
};

}  // namespace cmdenv
}  // namespace omnetpp

#endif

