//==========================================================================
//  CNULLENVIR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cNullEnvir : a cEnvir implementation with empty member functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CNULLENVIR_H
#define __CNULLENVIR_H

#include "cenvir.h"
#include "cexception.h"
#include "ccomponent.h"
#include "cmersennetwister.h"

NAMESPACE_BEGIN


/**
 * A cEnvir implementation with all pure virtual methods defined with an
 * empty body, with a trivial implementation, or to throw an exception
 * with the "unsupported method" text.
 *
 * This class is not used by \opp, it is provided for convenience of
 * writing standlone programs that embed the simulation kernel as a library.
 *
 * You may want to subclass from cNullEnvir, and redefine some the following
 * methods:
 * <ul>
 *   <li> sputn() to print module log messages instead of discarding
 *        them (<tt>::fwrite(s,1,n,stdout);</tt>);
 *   <li> readParameter() to set module parameters, or at least apply the
 *        NED default value;
 *   <li> the RNG methods (getNumRNGs(), getRNG()) to provide more than one
 *        random number generator
 *   <li> recordScalar() and other statistics recording methods to store
 *        simulation results instead of discarding them
 * </ul>
 *
 * @ingroup Envir
 * @ingroup EnvirExtensions
 */
class SIM_API cNullEnvir : public cEnvir
{
  protected:
    int argc;
    char **argv;
    cConfiguration *cfg;
    cRNG *rng;
    unsigned long lastnum;

  protected:
    void unsupported() const {throw cRuntimeError("cNullEnvir: unsupported method called");}

    virtual void sputn(const char *s, int n) {}
    virtual void putsmsg(const char *msg) {::printf("\n<!> %s\n\n", msg);}
    virtual bool askyesno(const char *msg)  {unsupported(); return false;}

  public:
    // constructor, destructor
    cNullEnvir(int ac, char **av, cConfiguration *c) {argc=ac; argv=av; cfg=c; rng=new cMersenneTwister(); lastnum=0;}
    virtual ~cNullEnvir() {delete cfg; delete rng;}

    // eventlog callback interface
    virtual void objectDeleted(cObject *object) {}
    virtual void simulationEvent(cMessage *msg)  {}
    virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=NULL)  {}
    virtual void messageScheduled(cMessage *msg)  {}
    virtual void messageCancelled(cMessage *msg)  {}
    virtual void beginSend(cMessage *msg)  {}
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)  {}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate)  {}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)  {}
    virtual void endSend(cMessage *msg)  {}
    virtual void messageCreated(cMessage *msg)  {}
    virtual void messageCloned(cMessage *msg, cMessage *clone)  {}
    virtual void messageDeleted(cMessage *msg)  {}
    virtual void moduleReparented(cModule *module, cModule *oldparent)  {}
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent)  {}
    virtual void componentMethodEnd()  {}
    virtual void moduleCreated(cModule *newmodule)  {}
    virtual void moduleDeleted(cModule *module)  {}
    virtual void gateCreated(cGate *newgate)  {}
    virtual void gateDeleted(cGate *gate)  {}
    virtual void connectionCreated(cGate *srcgate)  {}
    virtual void connectionDeleted(cGate *srcgate)  {}
    virtual void displayStringChanged(cComponent *component)  {}
    virtual void undisposedObject(cObject *obj) {}

    // configuration, model parameters
    virtual void configure(cComponent *component) {}
    virtual void readParameter(cPar *par)  {unsupported();}
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index)  {return true;}
    virtual cXMLElement *getXMLDocument(const char *filename, const char *xpath=NULL)  {unsupported(); return NULL;}
    virtual cXMLElement *getParsedXMLString(const char *content, const char *xpath=NULL)  {unsupported(); return NULL;}
    virtual void forgetXMLDocument(const char *filename) {}
    virtual void forgetParsedXMLString(const char *content) {}
    virtual void flushXMLDocumentCache() {}
    virtual void flushXMLParsedContentCache() {}
    virtual unsigned getExtraStackForEnvir() const  {return 0;}
    virtual cConfiguration *getConfig()  {return cfg;}
    virtual bool isGUI() const  {return false;}

    // UI functions (see also protected ones)
    virtual void bubble(cComponent *component, const char *text)  {}
    virtual std::string gets(const char *prompt, const char *defaultreply=NULL)  {unsupported(); return "";}
    virtual cEnvir& flush()  {::fflush(stdout); return *this;}

    // RNGs
    virtual int getNumRNGs() const {return 1;}
    virtual cRNG *getRNG(int k)  {return rng;}
    virtual void getRNGMappingFor(cComponent *component)  {component->setRNGMap(0,NULL);}

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname)  {return (void *)1;}
    virtual void deregisterOutputVector(void *vechandle)  {}
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value)  {}
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value)  {return false;}

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL)  {}
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL)  {}

    // snapshot file
    virtual std::ostream *getStreamForSnapshot()  {unsupported(); return NULL;}
    virtual void releaseStreamForSnapshot(std::ostream *os)  {unsupported();}

    // misc
    virtual int getArgCount() const  {return argc;}
    virtual char **getArgVector() const  {return argv;}
    virtual int getParsimProcId() const {return 0;}
    virtual int getParsimNumPartitions() const {return 1;}
    virtual unsigned long getUniqueNumber()  {return ++lastnum;}
    virtual bool idle()  {return false;}
};


NAMESPACE_END

#endif


