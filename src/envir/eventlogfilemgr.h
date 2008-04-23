//==========================================================================
//  EVENTLOGFILEMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGFILEMGR_H
#define __EVENTLOGFILEMGR_H

#include "simkerneldefs.h"
#include "envirdefs.h"
#include "opp_string.h"
#include "objectprinter.h"
#include "intervals.h"

class cModule;
class cMessage;
class cGate;
class cComponent;

NAMESPACE_BEGIN

/**
 * Responsible for writing the eventlog file.
 */
class ENVIR_API EventlogFileManager
{
  private:
    opp_string filename;
    FILE *feventlog;
    ObjectPrinter *objectPrinter;
    Intervals *recordingIntervals;

  public:
    EventlogFileManager();
    virtual ~EventlogFileManager();

    virtual void setup();
    virtual void startRun(const char *runId);
    virtual void endRun();

    virtual void flush();

    /** @name Functions called from cEnvir's similar functions */
    //@{
    virtual void simulationEvent(cMessage *msg);
    virtual void bubble(cComponent *component, const char *text);
    virtual void messageScheduled(cMessage *msg);
    virtual void messageCancelled(cMessage *msg);
    virtual void beginSend(cMessage *msg);
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay=0, simtime_t transmissionDelay=0);
    virtual void messageSendHop(cMessage *msg, cGate *srcGate);
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay);
    virtual void endSend(cMessage *msg);
    virtual void messageDeleted(cMessage *msg);
    virtual void moduleReparented(cModule *module, cModule *oldparent);
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *method);
    virtual void componentMethodEnd();
    virtual void moduleCreated(cModule *newmodule);
    virtual void moduleDeleted(cModule *module);
    virtual void connectionCreated(cGate *srcgate);
    virtual void connectionRemoved(cGate *srcgate);
    virtual void displayStringChanged(cComponent *component);
    virtual void sputn(const char *s, int n);
    //@}
};

NAMESPACE_END

#endif
