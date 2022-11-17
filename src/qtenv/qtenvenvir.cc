//==========================================================================
//  QTENVENVIR.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "qtenvenvir.h"

#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

void QtenvEnvir::askParameter(cPar *par, bool unassigned)
{
    app->askParameter(par, unassigned);
}

void QtenvEnvir::bubble(cComponent *component, const char *text)
{
    GenericEnvir::bubble(component, text);
    app->bubble(component, text);
}

void QtenvEnvir::objectDeleted(cObject *object)
{
    GenericEnvir::objectDeleted(object);
    app->objectDeleted(object);
}

void QtenvEnvir::simulationEvent(cEvent *event)
{
    GenericEnvir::simulationEvent(event);
    app->simulationEvent(event);
}

void QtenvEnvir::componentInitBegin(cComponent *component, int stage)
{
    GenericEnvir::componentInitBegin(component, stage);
    app->componentInitBegin(component, stage);
}

void QtenvEnvir::beginSend(cMessage *msg, const SendOptions& options)
{
    GenericEnvir::beginSend(msg, options);
    app->beginSend(msg, options);
}

void QtenvEnvir::messageScheduled(cMessage *msg)
{
    GenericEnvir::messageScheduled(msg);
    app->messageScheduled(msg);
}

void QtenvEnvir::messageCancelled(cMessage *msg)
{
    GenericEnvir::messageCancelled(msg);
    app->messageCancelled(msg);
}

void QtenvEnvir::messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result)
{
    GenericEnvir::messageSendDirect(msg, toGate, result);
    app->messageSendDirect(msg, toGate, result);
}

void QtenvEnvir::messageSendHop(cMessage *msg, cGate *srcGate)
{
    GenericEnvir::messageSendHop(msg, srcGate);
    app->messageSendHop(msg, srcGate);
}

void QtenvEnvir::messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result)
{
    GenericEnvir::messageSendHop(msg, srcGate, result);
    app->messageSendHop(msg, srcGate, result);
}

void QtenvEnvir::endSend(cMessage *msg)
{
    GenericEnvir::endSend(msg);
    app->endSend(msg);
}

void QtenvEnvir::messageDeleted(cMessage *msg)
{
    GenericEnvir::messageDeleted(msg);
    app->messageDeleted(msg);
}

void QtenvEnvir::componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent)
{
    GenericEnvir::componentMethodBegin(from, to, methodFmt, va, silent);
    app->componentMethodBegin(from, to, methodFmt, va, silent);
}

void QtenvEnvir::componentMethodEnd()
{
    GenericEnvir::componentMethodEnd();
    app->componentMethodEnd();
}

void QtenvEnvir::moduleCreated(cModule *newmodule)
{
    GenericEnvir::moduleCreated(newmodule);
    app->moduleCreated(newmodule);
}

void QtenvEnvir::moduleDeleted(cModule *module)
{
    GenericEnvir::moduleDeleted(module);
    app->moduleDeleted(module);
}

void QtenvEnvir::moduleReparented(cModule *module, cModule *oldparent, int oldId)
{
    GenericEnvir::moduleReparented(module, oldparent, oldId);
    app->moduleReparented(module, oldparent, oldId);
}

void QtenvEnvir::connectionCreated(cGate *srcgate)
{
    GenericEnvir::connectionCreated(srcgate);
    app->connectionCreated(srcgate);
}

void QtenvEnvir::connectionDeleted(cGate *srcgate)
{
    GenericEnvir::connectionDeleted(srcgate);
    app->connectionDeleted(srcgate);
}

void QtenvEnvir::displayStringChanged(cComponent *component)
{
    GenericEnvir::displayStringChanged(component);
    app->displayStringChanged(component);
}

void QtenvEnvir::log(cLogEntry *entry)
{
    GenericEnvir::log(entry);
    app->log(entry);
}

bool QtenvEnvir::idle()
{
    return app->idle();
}

bool QtenvEnvir::ensureDebugger(cRuntimeError *error)
{
    return app->ensureDebugger(error);
}

void QtenvEnvir::alert(const char *msg)
{
    app->alert(msg);
}

std::string QtenvEnvir::input(const char *prompt, const char *defaultReply)
{
    return app->gets(prompt, defaultReply);
}

bool QtenvEnvir::askYesNo(const char *prompt)
{
    return app->askYesNo(prompt);
}

void QtenvEnvir::getImageSize(const char *imageName, double &outWidth, double &outHeight)
{
    app->getImageSize(imageName, outWidth, outHeight);
}

void QtenvEnvir::getTextExtent(const cFigure::Font &font, const char *text,
        double &outWidth, double &outHeight, double &outAscent)
{
    app->getTextExtent(font, text, outWidth, outHeight, outAscent);
}

void QtenvEnvir::appendToImagePath(const char *directory)
{
    app->appendToImagePath(directory);
}

void QtenvEnvir::loadImage(const char *fileName, const char *imageName)
{
    app->loadImage(fileName, imageName);
}

cFigure::Rectangle QtenvEnvir::getSubmoduleBounds(const cModule *submodule)
{
    return app->getSubmoduleBounds(submodule);
}

std::vector<cFigure::Point> QtenvEnvir::getConnectionLine(const cGate *sourceGate)
{
    return app->getConnectionLine(sourceGate);
}

double QtenvEnvir::getZoomLevel(const cModule *module)
{
    return app->getZoomLevel(module);
}

double QtenvEnvir::getAnimationTime() const
{
    return app->getAnimationTime();
}

double QtenvEnvir::getAnimationSpeed() const
{
    return app->getAnimationSpeed();
}

double QtenvEnvir::getRemainingAnimationHoldTime() const
{
    return app->getRemainingAnimationHoldTime();
}

void QtenvEnvir::pausePoint()
{
    GenericEnvir::pausePoint();
    app->pausePoint();
}

}  // namespace qtenv
}  // namespace omnetpp

