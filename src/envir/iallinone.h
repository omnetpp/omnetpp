//==========================================================================
//  IALLINONE.H - part of
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

#ifndef __OMNETPP_ENVIR_IALLINONE_H
#define __OMNETPP_ENVIR_IALLINONE_H

#include <sstream>
#include <iostream>
#include "envirdefs.h"
#include "omnetpp/simtime_t.h"
#include "omnetpp/opp_string.h"
#include "omnetpp/clistener.h"  // for simsignal_t
#include "omnetpp/ccanvas.h"

namespace osg { class Node; }

namespace omnetpp {

class cObject;
class cOwnedObject;
class cLogEntry;
class cEvent;
class cMessage;
class cPar;
class cGate;
class cComponent;
class cComponentType;
class cModule;
class cSimpleModule;
class cStatistic;
class cProperty;
class cRNG;
class cXMLElement;
class cEnvir;
class cConfiguration;
class cConfigurationEx;
struct SendOptions;
struct ChannelResult;

using std::endl;

namespace envir {

class ENVIR_API IAllInOne
{
  public:
    virtual ~IAllInOne();

    virtual void objectDeleted(cObject *object) {}
    virtual void componentInitBegin(cComponent *component, int stage) {}
    virtual void simulationEvent(cEvent *event) {}
    virtual void messageScheduled(cMessage *msg) {}
    virtual void messageCancelled(cMessage *msg) {}
    virtual void beginSend(cMessage *msg, const SendOptions& options) {}
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result) {}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) {}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, const ChannelResult& result) {}
    virtual void endSend(cMessage *msg) {}
    virtual void messageCreated(cMessage *msg) {}
    virtual void messageCloned(cMessage *msg, cMessage* clone) {}
    virtual void messageDeleted(cMessage *msg) {}
    virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId) {}
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent) {}
    virtual void componentMethodEnd() {}
    virtual void moduleCreated(cModule *newmodule) {}
    virtual void moduleDeleted(cModule *module) {}
    virtual void gateCreated(cGate *newgate) {}
    virtual void gateDeleted(cGate *gate) {}
    virtual void connectionCreated(cGate *srcgate) {}
    virtual void connectionDeleted(cGate *srcgate) {}
    virtual void displayStringChanged(cComponent *component) {}
    virtual void undisposedObject(cObject *obj) {}

    virtual void preconfigureComponent(cComponent *component) {}
    virtual void configureComponent(cComponent *component) {}

    virtual unsigned getExtraStackForEnvir() const = 0;

    virtual bool isGUI() const = 0;

    virtual bool isExpressMode() const = 0;

    virtual void bubble(cComponent *component, const char *text) {}
    virtual void log(cLogEntry *entry) = 0;
    virtual void alert(const char *msg) = 0;
    virtual std::string gets(const char *prompt, const char *defaultReply=nullptr) = 0;
    virtual bool askYesNo(const char *prompt) = 0;
    virtual void askParameter(cPar *par, bool unassigned) = 0;

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

    virtual void refOsgNode(osg::Node *scene) {}
    virtual void unrefOsgNode(osg::Node *scene) {}
    virtual bool idle() = 0;
    virtual void pausePoint() = 0;
    virtual bool ensureDebugger(cRuntimeError *error = nullptr) = 0;
};

}  // namespace envir
}  // namespace omnetpp

#endif


