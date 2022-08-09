//==========================================================================
//  IFAKEGUI.H - part of
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

#ifndef __OMNETPP_ENVIR_IFAKEGUI_H
#define __OMNETPP_ENVIR_IFAKEGUI_H

#include <vector>
#include "omnetpp/ccanvas.h"
#include "envirdefs.h"

namespace omnetpp {

class cSimulation;
class cConfiguration;
class cEvent;
class cGate;

namespace envir {

class IFakeGUI
{
  public:
    IFakeGUI() {}
    virtual ~IFakeGUI();
    virtual void configure(cSimulation *simulation, cConfiguration *cfg) = 0;
    virtual void beforeEvent(cEvent *event) = 0;
    virtual void afterEvent() = 0;
    virtual void getImageSize(const char *imageName, double& outWidth, double& outHeight) = 0;
    virtual void getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent) = 0;
    virtual void appendToImagePath(const char *directory) = 0;
    virtual void loadImage(const char *fileName, const char *imageName=nullptr) = 0;
    virtual cFigure::Rectangle getSubmoduleBounds(const cModule *submodule) = 0;
    virtual std::vector<cFigure::Point> getConnectionLine(const cGate *sourceGate) = 0;
    virtual double getZoomLevel(const cModule *module) = 0;
    virtual double getAnimationTime() const = 0;
    virtual double getAnimationSpeed() const = 0;
    virtual double getRemainingAnimationHoldTime() const = 0;
};

}  // namespace envir
}  // namespace omnetpp

#endif

