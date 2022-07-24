//==========================================================================
//  QTENVIR.CC - part of
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

#include "qtenv.h"
#include "qtenvir.h"

namespace omnetpp {
namespace qtenv {

void QtEnvir::askParameter(cPar *par, bool unassigned)
{
    app->askParameter(par, unassigned);
}

void QtEnvir::bubble(cComponent *component, const char *text)
{
    EnvirBase::bubble(component, text);
    app->bubble(component, text);
}

void QtEnvir::objectDeleted(cObject *object)
{
    EnvirBase::objectDeleted(object);
    app->objectDeleted(object);
}

void QtEnvir::simulationEvent(cEvent *event)
{
    EnvirBase::simulationEvent(event);
    app->simulationEvent(event);
}

void QtEnvir::componentInitBegin(cComponent *component, int stage)
{
    EnvirBase::componentInitBegin(component, stage);
    app->componentInitBegin(component, stage);
}

void QtEnvir::beginSend(cMessage *msg, const SendOptions& options)
{
    EnvirBase::beginSend(msg, options);
    app->beginSend(msg, options);
}

void QtEnvir::messageScheduled(cMessage *msg)
{
    EnvirBase::messageScheduled(msg);
    app->messageScheduled(msg);
}

void QtEnvir::messageCancelled(cMessage *msg)
{
    EnvirBase::messageCancelled(msg);
    app->messageCancelled(msg);
}

void QtEnvir::messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result)
{
    EnvirBase::messageSendDirect(msg, toGate, result);
    app->messageSendDirect(msg, toGate, result);
}

void QtEnvir::messageSendHop(cMessage *msg, cGate *srcGate)
{
    EnvirBase::messageSendHop(msg, srcGate);
    app->messageSendHop(msg, srcGate);
}

void QtEnvir::messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result)
{
    EnvirBase::messageSendHop(msg, srcGate, result);
    app->messageSendHop(msg, srcGate, result);
}

void QtEnvir::endSend(cMessage *msg)
{
    EnvirBase::endSend(msg);
    app->endSend(msg);
}

void QtEnvir::messageDeleted(cMessage *msg)
{
    EnvirBase::messageDeleted(msg);
    app->messageDeleted(msg);
}

void QtEnvir::componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent)
{
    EnvirBase::componentMethodBegin(from, to, methodFmt, va, silent);
    app->componentMethodBegin(from, to, methodFmt, va, silent);
}

void QtEnvir::componentMethodEnd()
{
    EnvirBase::componentMethodEnd();
    app->componentMethodEnd();
}

void QtEnvir::moduleCreated(cModule *newmodule)
{
    EnvirBase::moduleCreated(newmodule);
    app->moduleCreated(newmodule);
}

void QtEnvir::moduleDeleted(cModule *module)
{
    EnvirBase::moduleDeleted(module);
    app->moduleDeleted(module);
}

void QtEnvir::moduleReparented(cModule *module, cModule *oldparent, int oldId)
{
    EnvirBase::moduleReparented(module, oldparent, oldId);
    app->moduleReparented(module, oldparent, oldId);
}

void QtEnvir::connectionCreated(cGate *srcgate)
{
    EnvirBase::connectionCreated(srcgate);
    app->connectionCreated(srcgate);
}

void QtEnvir::connectionDeleted(cGate *srcgate)
{
    EnvirBase::connectionDeleted(srcgate);
    app->connectionDeleted(srcgate);
}

void QtEnvir::displayStringChanged(cComponent *component)
{
    EnvirBase::displayStringChanged(component);
    app->displayStringChanged(component);
}

void QtEnvir::log(cLogEntry *entry)
{
    EnvirBase::log(entry);
    app->log(entry);
}

bool QtEnvir::idle()
{
    return app->idle();
}

bool QtEnvir::ensureDebugger(cRuntimeError *error)
{
    return app->ensureDebugger(error);
}

void QtEnvir::alert(const char *msg)
{
    app->alert(msg);
}

std::string QtEnvir::input(const char *prompt, const char *defaultReply)
{
    return app->gets(prompt, defaultReply);
}

bool QtEnvir::askYesNo(const char *prompt)
{
    return app->askYesNo(prompt);
}

void QtEnvir::getImageSize(const char *imageName, double &outWidth, double &outHeight)
{
    app->getImageSize(imageName, outWidth, outHeight);
}

void QtEnvir::getTextExtent(const cFigure::Font &font, const char *text,
        double &outWidth, double &outHeight, double &outAscent)
{
    app->getTextExtent(font, text, outWidth, outHeight, outAscent);
}

void QtEnvir::appendToImagePath(const char *directory)
{
    app->appendToImagePath(directory);
}

void QtEnvir::loadImage(const char *fileName, const char *imageName)
{
    app->loadImage(fileName, imageName);
}

cFigure::Rectangle QtEnvir::getSubmoduleBounds(const cModule *submodule)
{
    return app->getSubmoduleBounds(submodule);
}

std::vector<cFigure::Point> QtEnvir::getConnectionLine(const cGate *sourceGate)
{
    return app->getConnectionLine(sourceGate);
}

double QtEnvir::getZoomLevel(const cModule *module)
{
    return app->getZoomLevel(module);
}

double QtEnvir::getAnimationTime() const
{
    return app->getAnimationTime();
}

double QtEnvir::getAnimationSpeed() const
{
    return app->getAnimationSpeed();
}

double QtEnvir::getRemainingAnimationHoldTime() const
{
    return app->getRemainingAnimationHoldTime();
}

void QtEnvir::pausePoint()
{
    EnvirBase::pausePoint();
    app->pausePoint();
}

}  // namespace qtenv
}  // namespace omnetpp

