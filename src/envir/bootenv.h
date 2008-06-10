//==========================================================================
//  BOOTENV.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Declaration of the following classes:
//    BootEnv    : user interface class
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __BOOTENV_H
#define __BOOTENV_H

#include "envirdefs.h"
#include "exception.h"
#include "cenvir.h"

NAMESPACE_BEGIN

//FIXME refine this macro
#define UNSUPPORTED   {throw opp_runtime_error("unsupported function called");}


/**
 * FIXME todo comment
 */
class ENVIR_API BootEnv : public cEnvir
{
  protected:
    virtual void sputn(const char *s, int n);
    virtual void putsmsg(const char *msg);
    virtual bool askyesno(const char *msg)  {UNSUPPORTED;}

  public:
    // constructor, destructor
    BootEnv();
    virtual ~BootEnv();

    // life cycle
    virtual int run(int argc, char *argv[], cConfiguration *cfg);

    // eventlog callback interface
    virtual void objectDeleted(cObject *object) {}
    virtual void simulationEvent(cMessage *msg)  {UNSUPPORTED;}
    virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=NULL)  {UNSUPPORTED;}
    virtual void messageScheduled(cMessage *msg)  {UNSUPPORTED;}
    virtual void messageCancelled(cMessage *msg)  {UNSUPPORTED;}
    virtual void beginSend(cMessage *msg)  {UNSUPPORTED;}
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay=0, simtime_t transmissionDelay=0)  {UNSUPPORTED;}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate)  {UNSUPPORTED;}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)  {UNSUPPORTED;}
    virtual void endSend(cMessage *msg)  {UNSUPPORTED;}
    virtual void messageDeleted(cMessage *msg)  {UNSUPPORTED;}
    virtual void moduleReparented(cModule *module, cModule *oldparent)  {UNSUPPORTED;}
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *method)  {UNSUPPORTED;}
    virtual void componentMethodEnd()  {UNSUPPORTED;}
    virtual void moduleCreated(cModule *newmodule)  {UNSUPPORTED;}
    virtual void moduleDeleted(cModule *module)  {UNSUPPORTED;}
    virtual void gateCreated(cGate *newgate)  {UNSUPPORTED;}
    virtual void gateDeleted(cGate *gate)  {UNSUPPORTED;}
    virtual void connectionCreated(cGate *srcgate)  {UNSUPPORTED;}
    virtual void connectionDeleted(cGate *srcgate)  {UNSUPPORTED;}
    virtual void displayStringChanged(cComponent *component)  {UNSUPPORTED;}
    virtual void undisposedObject(cObject *obj);

     // configuration, model parameters
    virtual void readParameter(cPar *parameter)  {UNSUPPORTED;}
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index)  {UNSUPPORTED;}
    virtual cXMLElement *getXMLDocument(const char *filename, const char *path=NULL)  {UNSUPPORTED;}
    virtual unsigned extraStackForEnvir() const  {UNSUPPORTED;}
    virtual cConfiguration *config()  {UNSUPPORTED;}
    virtual bool isGUI()  {UNSUPPORTED;}

    // UI functions (see also protected ones)
    virtual void bubble(cComponent *component, const char *text)  {UNSUPPORTED;}
    virtual std::string gets(const char *prompt, const char *defaultreply=NULL)  {UNSUPPORTED;}
    virtual cEnvir& flush();

    // RNGs
    virtual int numRNGs() const {UNSUPPORTED;}
    virtual cRNG *rng(int k)  {UNSUPPORTED;}
    virtual void getRNGMappingFor(cComponent *component)  {UNSUPPORTED;}

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname)  {UNSUPPORTED;}
    virtual void deregisterOutputVector(void *vechandle)  {UNSUPPORTED;}
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value)  {UNSUPPORTED;}
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value)  {UNSUPPORTED;}

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL)  {UNSUPPORTED;}
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL)  {UNSUPPORTED;}

    // snapshot file
    virtual std::ostream *getStreamForSnapshot()  {UNSUPPORTED;}
    virtual void releaseStreamForSnapshot(std::ostream *os)  {UNSUPPORTED;}

    // misc
    virtual int argCount() const  {UNSUPPORTED;}
    virtual char **argVector() const  {UNSUPPORTED;}
    virtual int getParsimProcId()  {UNSUPPORTED;}
    virtual int getParsimNumPartitions()  {UNSUPPORTED;}
    virtual const char *getRunId()  {UNSUPPORTED;}
    virtual unsigned long getUniqueNumber()  {UNSUPPORTED;}
    virtual bool idle()  {UNSUPPORTED;}
};

#undef UNSUPPORTED

NAMESPACE_END

#endif
