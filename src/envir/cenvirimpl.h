//==========================================================================
//  CENVIRIMPL.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cEnvirImpl    : user interface class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CENVIRIMPL_H
#define __CENVIRIMPL_H

#include "envirdefs.h"
#include "cenvir.h"

NAMESPACE_BEGIN

/**
 * FIXME todo comment
 */
class ENVIR_API cEnvirImpl : public cEnvir
{
  public:
    TOmnetApp *app;  // the application instance

  public:
    virtual void sputn(const char *s, int n);

  public:
    // constructor, destructor
    cEnvirImpl();
    virtual ~cEnvirImpl();

    // life cycle
    virtual void setup(int ac, char *av[]);
    virtual int run();
    virtual void shutdown();

    // eventlog callback interface
    virtual void objectDeleted(cObject *object);
    virtual void simulationEvent(cMessage *msg);
    virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=NULL);
    virtual void messageScheduled(cMessage *msg);
    virtual void messageCancelled(cMessage *msg);
    virtual void beginSend(cMessage *msg);
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay=0, simtime_t transmissionDelay=0);
    virtual void messageSendHop(cMessage *msg, cGate *srcGate);
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay);
    virtual void endSend(cMessage *msg);
    virtual void messageDeleted(cMessage *msg);  //XXX document this and all above funcs
    virtual void moduleReparented(cModule *module, cModule *oldparent);
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *method);
    virtual void componentMethodEnd();
    virtual void moduleCreated(cModule *newmodule);
    virtual void moduleDeleted(cModule *module);
    virtual void connectionCreated(cGate *srcgate);
    virtual void connectionRemoved(cGate *srcgate);
    virtual void displayStringChanged(cComponent *component);
    virtual void undisposedObject(cObject *obj);

     // configuration, model parameters
    virtual void readParameter(cPar *parameter);
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index);
    virtual cXMLElement *getXMLDocument(const char *filename, const char *path=NULL);
    virtual unsigned extraStackForEnvir();
    virtual cConfiguration *config();

    // UI functions
    virtual void bubble(cComponent *component, const char *text);
    virtual void printfmsg(const char *fmt,...);
    virtual void printf(const char *fmt="\n",...);
    virtual _OPPDEPRECATED void puts(const char *s);
    virtual cEnvirImpl& flush();
    virtual std::string gets(const char *prompt, const char *defaultreply=NULL);
    virtual _OPPDEPRECATED bool gets(const char *prompt, char *buf, int len=255);
    virtual bool askYesNo(const char *msgfmt,...);

    // RNGs
    virtual int numRNGs();
    virtual cRNG *rng(int k);
    virtual void getRNGMappingFor(cComponent *component);

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname);
    virtual void deregisterOutputVector(void *vechandle);
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value);
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value);

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL);
    virtual void recordScalar(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL);

    // snapshot file
    virtual std::ostream *getStreamForSnapshot();
    virtual void releaseStreamForSnapshot(std::ostream *os);

    // misc
    virtual int argCount();
    virtual char **argVector();
    virtual int getParsimProcId();
    virtual int getParsimNumPartitions();
    virtual unsigned long getUniqueNumber();
    virtual bool idle();
};

NAMESPACE_END

#endif
