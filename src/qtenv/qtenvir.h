//==========================================================================
//  QTENVIR.H - part of
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

#ifndef __OMNETPP_QTENV_QTENVIR_H
#define __OMNETPP_QTENV_QTENVIR_H

#include "qtenvdefs.h"
#include "envir/envirbase.h"

using namespace omnetpp::envir;

namespace omnetpp {
namespace qtenv {

class Qtenv;

class QTENV_API QtEnvir : public EnvirBase
{
  protected:
    Qtenv *app = nullptr;

  protected:
    // Called internally from readParameter(), to interactively prompt the
    // user for a parameter value.
    virtual void askParameter(cPar *par, bool unassigned) override;

  public:
    QtEnvir(Qtenv *app) : app(app) {}
    virtual ~QtEnvir() {}

    // getters/setters
    Qtenv *getApp() {return app;}

    // eventlog callback interface
    virtual void objectDeleted(cObject *object) override;
    virtual void simulationEvent(cEvent *event) override;
    virtual void componentInitBegin(cComponent *component, int stage) override;
    virtual void messageScheduled(cMessage *msg) override;
    virtual void messageCancelled(cMessage *msg) override;
    virtual void beginSend(cMessage *msg, const SendOptions& options) override;
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result) override;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) override;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result) override;
    virtual void endSend(cMessage *msg) override;
    virtual void messageDeleted(cMessage *msg) override;
    virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId) override;
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent) override;
    virtual void componentMethodEnd() override;
    virtual void moduleCreated(cModule *newmodule) override;
    virtual void moduleDeleted(cModule *module) override;
    virtual void connectionCreated(cGate *srcgate) override;
    virtual void connectionDeleted(cGate *srcgate) override;
    virtual void displayStringChanged(cComponent *component) override;
    virtual void log(cLogEntry *entry) override;
    virtual void bubble(cComponent *component, const char *text) override;

    // misc
    virtual void refOsgNode(osg::Node *scene) override {}
    virtual void unrefOsgNode(osg::Node *scene) override {}
    virtual bool idle() override;
    virtual bool ensureDebugger(cRuntimeError *error = nullptr) override;

    virtual void alert(const char *msg) override;
    virtual std::string gets(const char *prompt, const char *defaultReply=nullptr) override;
    virtual bool askYesNo(const char *prompt) override;
    virtual void getImageSize(const char *imageName, double& outWidth, double& outHeight) override;
    virtual void getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent) override;
    virtual void appendToImagePath(const char *directory) override;
    virtual void loadImage(const char *fileName, const char *imageName=nullptr) override;
    virtual cFigure::Rectangle getSubmoduleBounds(const cModule *submodule) override;
    virtual std::vector<cFigure::Point> getConnectionLine(const cGate *sourceGate) override;
    virtual double getZoomLevel(const cModule *module) override;
    virtual double getAnimationTime() const override;
    virtual double getAnimationSpeed() const override;
    virtual double getRemainingAnimationHoldTime() const override;
    virtual void pausePoint() override;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

