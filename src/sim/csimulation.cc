//=========================================================================
//  CSIMULATION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Definition of global object:
//    simulation
//
//   Member functions of
//    cSimulation  : holds modules and manages the simulation
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <algorithm>
#include "stringutil.h"
#include "cmodule.h"
#include "csimplemodule.h"
#include "cmessage.h"
#include "csimulation.h"
#include "cscheduler.h"
#include "cenvir.h"
#include "ccomponenttype.h"
#include "cstatistic.h"
#include "cexception.h"
#include "cparimpl.h"
#include "chasher.h"
#include "cconfiguration.h"
#include "ccoroutine.h"
#include "platdep/platmisc.h"  // for DEBUG_TRAP

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

#ifdef WITH_NETBUILDER
#include "netbuilder/cnedloader.h"
#endif

NAMESPACE_BEGIN

using std::ostream;

#ifdef DEVELOPER_DEBUG
#include <set>
extern std::set<cOwnedObject*> objectlist;
void printAllObjects();
#endif

#ifdef NDEBUG
#define DEBUG_TRAP_IF_REQUESTED   /*no-op*/
#else
#define DEBUG_TRAP_IF_REQUESTED   {if (trap_on_next_event) {trap_on_next_event=false; DEBUG_TRAP;}}
#endif

cSimulation::cSimulation(const char *name, cEnvir *env) : cNamedObject(name, false)
{
    ASSERT(cStaticFlag::isSet()); // cannot be instantiated as global variable

    ownEvPtr = env;

    activitymodp = NULL;
    contextmodp = NULL;

    simulationstage = CTX_NONE;
    contexttype = CTX_NONE;

    systemmodp = NULL;
    schedulerp = NULL;

    delta = 32;
    size = 0;
    last_id = 0;  // vect[0] is not used for historical reasons
    vect = NULL;

    networktype = NULL;
    hasherp = NULL;

    sim_time = SIMTIME_ZERO;
    event_num = 0;
    trap_on_next_event = false;

    msgQueue.setName("scheduled-events");
    take(&msgQueue);

    // install a default scheduler
    setScheduler(new cSequentialScheduler());
}

cSimulation::~cSimulation()
{
    if (this==simPtr)
        // NOTE: subclass destructors will not be called, but the simulation will stop anyway
        throw cRuntimeError(this, "cannot delete the active simulation manager object");

    deleteNetwork();

    delete hasherp;
    delete schedulerp;
    delete ownEvPtr;
    drop(&msgQueue);
}

void cSimulation::setActiveSimulation(cSimulation *sim)
{
    simPtr = sim;
    evPtr = sim==NULL ? staticEvPtr : sim->ownEvPtr;
}

void cSimulation::setStaticEnvir(cEnvir *env)
{
    if (!env)
         throw cRuntimeError("cSimulation::setStaticEnvir(): argument cannot be NULL");
    staticEvPtr = env;
}

void cSimulation::forEachChild(cVisitor *v)
{
    if (systemmodp!=NULL)
        v->visit(systemmodp);
    v->visit(&msgQueue);
}

std::string cSimulation::getFullPath() const
{
    return getFullName();
}

static std::string xmlquote(const std::string& str)
{
    if (!strchr(str.c_str(), '<') && !strchr(str.c_str(), '>'))
        return str;

    std::stringstream out;
    for (const char *s=str.c_str(); *s; s++)
    {
        char c = *s;
        if (c=='<')
           out << "&lt;";
        else if (c=='>')
           out << "&gt;";
        else
           out << c;
    }
    return out.str();
}

class cSnapshotWriterVisitor : public cVisitor
{
  protected:
    ostream& os;
    int indentlevel;
  public:
    cSnapshotWriterVisitor(ostream& ostr) : os(ostr) {
        indentlevel = 0;
    }
    virtual void visit(cObject *obj) {
        std::string indent(2*indentlevel, ' ');
        os << indent << "<object class=\"" << obj->getClassName() << "\" fullpath=\"" << xmlquote(obj->getFullPath()) << "\">\n";
        os << indent << "  <info>" << xmlquote(obj->info()) << "</info>\n";
        std::string details = obj->detailedInfo();
        if (!details.empty())
            os << indent << "  <detailedinfo>" << xmlquote(details) << "</detailedinfo>\n";
        indentlevel++;
        obj->forEachChild(this);
        indentlevel--;
        os << indent << "</object>\n\n";

        if (os.fail()) throw EndTraversalException();
    }
};

bool cSimulation::snapshot(cObject *object, const char *label)
{
    if (!object)
        throw cRuntimeError("snapshot(): object pointer is NULL");

    ostream *osptr = ev.getStreamForSnapshot();
    if (!osptr)
        throw cRuntimeError("Could not create stream for snapshot");

    ostream& os = *osptr;

    os << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    os << "<snapshot\n";
    os << "    object=\"" << xmlquote(object->getFullPath()) << "\"\n";
    os << "    label=\"" << xmlquote(label?label:"") << "\"\n";
    os << "    simtime=\"" << xmlquote(SIMTIME_STR(simTime())) << "\"\n";
    os << "    network=\"" << xmlquote(networktype?networktype->getName():"") << "\"\n";
    os << "    >\n";

    cSnapshotWriterVisitor v(os);
    v.process(object);

    os << "</snapshot>\n";

    bool success = !os.fail();
    ev.releaseStreamForSnapshot(&os);

    if (!success)
        throw cRuntimeError("Could not write snapshot");
    return success;
}

void cSimulation::setScheduler(cScheduler *sch)
{
    if (systemmodp)
        throw cRuntimeError(this, "setScheduler(): cannot switch schedulers when a network is already set up");
    if (!sch)
        throw cRuntimeError(this, "setScheduler(): scheduler pointer is NULL");
    delete schedulerp;
    schedulerp = sch;
    schedulerp->setSimulation(this);
}

int cSimulation::loadNedSourceFolder(const char *folder)
{
#ifdef WITH_NETBUILDER
    return cNEDLoader::getInstance()->loadNedSourceFolder(folder);
#else
    throw cRuntimeError("cannot load NED files from `%s': simulation kernel was compiled without "
                        "support for dynamic loading of NED files (WITH_NETBUILDER=no)", folder);
#endif
}

void cSimulation::loadNedFile(const char *nedfile, const char *expectedPackage, bool isXML)
{
#ifdef WITH_NETBUILDER
    cNEDLoader::getInstance()->loadNedFile(nedfile, expectedPackage, isXML);
#else
    throw cRuntimeError("cannot load `%s': simulation kernel was compiled without "
                        "support for dynamic loading of NED files (WITH_NETBUILDER=no)", nedfile);
#endif
}

void cSimulation::loadNedText(const char *name, const char *nedtext, const char *expectedPackage, bool isXML)
{
#ifdef WITH_NETBUILDER
    cNEDLoader::getInstance()->loadNedText(name, nedtext, expectedPackage, isXML);
#else
    throw cRuntimeError("cannot source NED text: simulation kernel was compiled without "
                        "support for dynamic loading of NED files (WITH_NETBUILDER=no)");
#endif
}

void cSimulation::doneLoadingNedFiles()
{
#ifdef WITH_NETBUILDER
    cNEDLoader::getInstance()->doneLoadingNedFiles();
#endif
}

std::string cSimulation::getNedPackageForFolder(const char *folder)
{
#ifdef WITH_NETBUILDER
    return cNEDLoader::getInstance()->getNedPackageForFolder(folder);
#else
    return "-";
#endif
}

int cSimulation::registerModule(cModule *mod)
{
    // Insert module into the vector.
    // The module will get (last_id+1) as ID. We do not reuse "holes"
    // (empty slots) in the vector because we want the module ids to be
    // unique during the whole simulation.

    last_id++;

    if (last_id>=size)
    {
        // vector full, grow by delta
        cModule **v = new cModule *[size+delta];
        memcpy(v, vect, sizeof(cModule*)*size );
        for (int i=size; i<size+delta; i++) v[i]=NULL;
        delete [] vect;
        vect = v;
        size += delta;
    }
    vect[last_id] = mod;
    return last_id;
}

void cSimulation::deregisterModule(cModule *mod)
{
    int id = mod->getId();
    vect[id] = NULL;

    if (mod==systemmodp)
    {
        drop(systemmodp);
        systemmodp = NULL;
    }
}

void cSimulation::setSystemModule(cModule *p)
{
    systemmodp = p;
    take(p);
}

cModule *cSimulation::getModuleByPath(const char *path) const
{
    cModule *modp = getSystemModule();
    if (!modp || !path || !path[0] || path[0] == '.' || path[0] == '^')
        return NULL;
    return modp->getModuleByPath(path);
}

void cSimulation::setupNetwork(cModuleType *network)
{
#ifdef DEVELOPER_DEBUG
    printf("DEBUG: before setupNetwork: %d objects\n", cOwnedObject::getLiveObjectCount());
    objectlist.clear();
#endif

    checkActive();
    if (!network)
        throw cRuntimeError(eNONET);
    if (!network->isNetwork())
        throw cRuntimeError("setupNetwork: `%s' is not a network", network->getFullName());

    // set cNetworkType pointer
    networktype = network;

    // just to be sure
    msgQueue.clear();
    cComponent::clearSignalState();

    simulationstage = CTX_BUILD;

    try
    {
        // set up the network by instantiating the toplevel module
        cContextTypeSwitcher tmp(CTX_BUILD);
        cModule *mod = networktype->create(networktype->getName(), NULL);
        mod->finalizeParameters();
        mod->buildInside();
    }
    catch (std::exception& e)
    {
        // Note: no deleteNetwork() call here. We could call it here, but it's
        // dangerous: module destructors may try to delete uninitialized pointers
        // and crash. (Often pointers incorrectly get initialized in initialize()
        // and not in the constructor.)
        throw;
    }
    //catch (...) -- this is not a good idea because it would make debugging more difficult
    //{
    //    deleteNetwork();
    //    throw cRuntimeError("unknown exception occurred");
    //}

    //printf("setupNetwork finished, cParImpl objects in use: %ld\n", cParImpl::getLiveParImplObjectCount());
}

void cSimulation::startRun()
{
    checkActive();

    // reset counters. Note msgQueue.clear() was already called from setupNetwork()
    sim_time = 0;
    event_num = 0; // initialize() has event number 0
    trap_on_next_event = false;
    cMessage::resetMessageCounters();

    simulationstage = CTX_INITIALIZE;

    // init the scheduler. Note this may insert events into the FES (see e.g. cNullMessageProtocol)
    getScheduler()->startRun();

    // prepare simple modules for simulation run:
    //    1. create starter message for all modules,
    //    2. then call initialize() for them (recursively)
    //  This order is important because initialize() functions might contain
    //  send() calls which could otherwise insert msgs BEFORE starter messages
    //  for the destination module and cause trouble in cSimpleMod's activate().
    if (systemmodp)
    {
        cContextSwitcher tmp(systemmodp);
        systemmodp->scheduleStart(0);
        systemmodp->callInitialize();
    }

    event_num = 1; // events are numbered from 1

    simulationstage = CTX_EVENT;
}

void cSimulation::callFinish()
{
    checkActive();

    simulationstage = CTX_FINISH;

    // call user-defined finish() functions for all modules recursively
    if (systemmodp)
    {
        systemmodp->callFinish();
    }
}

void cSimulation::endRun()
{
    checkActive();
    getScheduler()->endRun();
}

void cSimulation::deleteNetwork()
{
    if (!systemmodp)
        return;  // network already deleted

    if (getContextModule()!=NULL)
        throw cRuntimeError("Attempt to delete network during simulation");

    simulationstage = CTX_CLEANUP;

    // delete all modules recursively
    systemmodp->deleteModule();

    // make sure it was successful
    for (int i=1; i<size; i++)
        ASSERT(vect[i]==NULL);

    // and clean up
    delete [] vect;
    vect = NULL;
    size = 0;
    last_id = 0;

    networktype = NULL;

    //FIXME todo delete cParImpl caches too (cParImplCache, cParImplCache2)
    cModule::clearNamePools();

    // clear remaining messages (module dtors may have cancelled & deleted some of them)
    msgQueue.clear();

    simulationstage = CTX_NONE;

#ifdef DEVELOPER_DEBUG
    printf("DEBUG: after deleteNetwork: %d objects\n", cOwnedObject::getLiveObjectCount());
    printAllObjects();
#endif

}

cSimpleModule *cSimulation::selectNextModule()
{
    // determine next event. Normally (with sequential simulation),
    // the scheduler just returns msgQueue->peekFirst().
    cMessage *msg = schedulerp->getNextEvent();
    if (!msg)
        return NULL; // scheduler got interrupted while waiting

    // check if destination module exists and is still running
    cSimpleModule *modp = (cSimpleModule *)vect[msg->getArrivalModuleId()];
    if (!modp || modp->isTerminated())
    {
        // Deleted/ended modules may have self-messages and sent messages
        // pending for them. Here we choose just to ignore them (delete them without
        // any error or warning). Rationale: if we thew an error here, it would
        // be very difficult to delete compound modules in which simple modules
        // send messages to one another -- each and every simple module
        // would have to be notified in advance that they're "shutting down",
        // and even then, deletion could progress only after waiting the maximum
        // propagation delay to elapse.
        delete msgQueue.removeFirst();
        return selectNextModule();
    }

    // advance simulation time
    sim_time = msg->getArrivalTime();

    return modp;
}

cMessage *cSimulation::guessNextEvent()
{
    // determine the probable next event. No call to cSheduler!
    // TBD if this event is "not good" (no module or module ended),
    // we might look for another event, but this is not done right now.
    return msgQueue.peekFirst();
}

simtime_t cSimulation::guessNextSimtime()
{
    cMessage *msg = guessNextEvent();
    return msg==NULL ? -1 : msg->getArrivalTime();
}

cSimpleModule *cSimulation::guessNextModule()
{
    cMessage *msg = guessNextEvent();
    if (!msg)
        return NULL;

    // check if dest module exists and still running
    if (msg->getArrivalModuleId()==-1)
        return NULL;
    cSimpleModule *modp = (cSimpleModule *)vect[msg->getArrivalModuleId()];
    if (!modp || modp->isTerminated())
        return NULL;
    return modp;
}

void cSimulation::transferTo(cSimpleModule *modp)
{
    if (modp==NULL)
        throw cRuntimeError("transferTo(): attempt to transfer to NULL");

    // switch to activity() of the simple module
    exception = NULL;
    activitymodp = modp;
    cCoroutine::switchTo(modp->coroutine);

    if (modp->hasStackOverflow())
        throw cRuntimeError("Stack violation in module (%s)%s: module stack too small? "
                            "Try increasing it in the class' Module_Class_Members() or constructor",
                            modp->getClassName(), modp->getFullPath().c_str());

    // if exception occurred in activity(), re-throw it. This allows us to handle
    // handleMessage() and activity() in an uniform way in the upper layer.
    if (exception)
    {
        cException *e = exception;
        exception = NULL;

        // ok, so we have an exception *pointer*, but we have to throw further
        // by *value*, and possibly without leaking it. Hence the following magic...
        if (dynamic_cast<cDeleteModuleException *>(e))
        {
            cDeleteModuleException e2(*(cDeleteModuleException *)e);
            delete e;
            throw e2;
        }
        else if (dynamic_cast<cTerminationException *>(e))
        {
            cTerminationException e2(*(cTerminationException *)e);
            delete e;
            throw e2;
        }
        else if (dynamic_cast<cRuntimeError *>(e))
        {
            cRuntimeError e2(*(cRuntimeError *)e);
            delete e;
            throw e2;
        }
        else
        {
            cException e2(*(cException *)e);
            delete e;
            throw e2;
        }
    }
}

void cSimulation::doOneEvent(cSimpleModule *mod)
{
#ifndef NDEBUG
    checkActive();
#endif
    try
    {
        // switch to the module's context
        setContext(mod);
        setContextType(CTX_EVENT);

        if (getHasher())
        {
            // note: there's no value in adding getEventNumber()
            getHasher()->add(SIMTIME_RAW(simTime()));
            getHasher()->add(mod->getId());
        }

        // get event to be handled (note: it becomes owned by the target module)
        cMessage *msg = msgQueue.removeFirst();

        // notify the environment about the event (writes eventlog, etc.)
        EVCB.simulationEvent(msg);

        // store arrival event number of this message; it is useful input for the
        // sequence chart tool if the message doesn't get immediately deleted or
        // sent out again
        msg->setPreviousEventNumber(event_num);

        if (!mod->initialized())
            throw cRuntimeError(mod, "Module not initialized (did you forget to invoke "
                                     "callInitialize() for a dynamically created module?)");

        if (mod->usesActivity())
        {
            // switch to the coroutine of the module's activity(). We'll get back control
            // when the module executes a receive() or wait() call.
            // If there was an error during simulation, the call will throw an exception
            // (which originally occurred inside activity()).
            msg_for_activity = msg;
            transferTo(mod);
        }
        else
        {
            DEBUG_TRAP_IF_REQUESTED; // YOU ARE ABOUT TO ENTER THE handleMessage() CALL YOU REQUESTED -- SELECT "STEP INTO" IN YOUR DEBUGGER
            mod->handleMessage(msg);
        }
    }
    catch (cDeleteModuleException& e)
    {
        setGlobalContext();
        mod->deleteModule();
    }
    catch (cException&)
    {
        // restore global context before throwing the exception further
        setGlobalContext();
        throw;
    }
    catch (std::exception& e)
    {
        // restore global context before throwing the exception further
        // but wrap into a cRuntimeError which captures the module before that
        cRuntimeError e2("%s: %s", opp_typename(typeid(e)), e.what());
        setGlobalContext();
        throw e2;
    }

    setGlobalContext();

    // increment event count
    event_num++;

    // Note: simulation time (as read via simTime() from modules) is updated
    // in selectNextModule()) called right before the next doOneEvent().
    // It must not be updated here, because it will interfere with parallel
    // simulation (cIdealSimulationProtocol, etc) that relies on simTime()
    // returning the time of the last executed event. If Tkenv wants to display
    // the time of the next event, it should call guessNextSimtime().
}

void cSimulation::transferToMain()
{
    if (activitymodp!=NULL)
    {
        activitymodp = NULL;
        cCoroutine::switchToMain();     // stack switch
    }
}

void cSimulation::setContext(cComponent *p)
{
    contextmodp = p;
    cOwnedObject::setDefaultOwner(p);
}

cModule *cSimulation::getContextModule() const
{
    // cannot go inline (upward cast would require including cmodule.h in csimulation.h)
    if (!contextmodp || !contextmodp->isModule())
        return NULL;
    return (cModule *)contextmodp;
}

cSimpleModule *cSimulation::getContextSimpleModule() const
{
    // cannot go inline (upward cast would require including cmodule.h in csimulation.h)
    if (!contextmodp || !contextmodp->isModule() || !((cModule *)contextmodp)->isSimple())
        return NULL;
    return (cSimpleModule *)contextmodp;
}

unsigned long cSimulation::getUniqueNumber()
{
    return ev.getUniqueNumber();
}

void cSimulation::setHasher(cHasher *hasher)
{
    if (hasherp)
        delete hasherp;
    hasherp = hasher;
}

void cSimulation::insertMsg(cMessage *msg)
{
    msg->setPreviousEventNumber(event_num);
    simulation.msgQueue.insert(msg);
}


//----

/**
 * A dummy implementation of cEnvir, only provided so that one
 * can use simulation library classes outside simulations, that is,
 * in programs that only link with the simulation library (<i>sim_std</i>)
 * and not with the <i>envir</i>, <i>cmdenv</i>, <i>tkenv</i>,
 * etc. libraries.
 *
 * Many simulation library classes make calls to <i>ev</i> methods,
 * which would crash if <tt>evPtr</tt> was NULL; one example is
 * cObject's destructor which contains an <tt>ev.objectDeleted()</tt>.
 * The solution provided here is that <tt>evPtr</tt> is initialized
 * to point to a StaticEnv instance, thus enabling library classes to work.
 *
 * StaticEnv methods either do nothing, or throw an "unsupported method"
 * exception, so StaticEnv is only useful for the most basic usage scenarios.
 * For anything more complicated, <tt>evPtr</tt> must be set in <tt>main()</tt>
 * to point to a proper cEnvir implementation, like the Cmdenv or
 * Tkenv classes. (The <i>envir</i> library provides a <tt>main()</tt>
 * which does exactly that.)
 *
 * @ingroup Envir
 */
class StaticEnv : public cEnvir
{
  protected:
    void unsupported() const {throw opp_runtime_error("StaticEnv: unsupported method called");}

    virtual void sputn(const char *s, int n) {(void) ::fwrite(s,1,n,stdout);}
    virtual void putsmsg(const char *msg) {::printf("\n<!> %s\n\n", msg);}
    virtual bool askyesno(const char *msg)  {unsupported(); return false;}

  public:
    // constructor, destructor
    StaticEnv() {}
    virtual ~StaticEnv() {}

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
    virtual void undisposedObject(cObject *obj);

     // configuration, model parameters
    virtual void configure(cComponent *component) {}
    virtual void readParameter(cPar *parameter)  {unsupported();}
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index)  {return true;}
    virtual cXMLElement *getXMLDocument(const char *filename, const char *xpath=NULL)  {unsupported(); return NULL;}
    virtual cXMLElement *getParsedXMLString(const char *content, const char *xpath=NULL)  {unsupported(); return NULL;}
    virtual void forgetXMLDocument(const char *filename) {}
    virtual void forgetParsedXMLString(const char *content) {}
    virtual void flushXMLDocumentCache() {}
    virtual void flushXMLParsedContentCache() {}
    virtual unsigned getExtraStackForEnvir() const  {return 0;}
    virtual cConfiguration *getConfig()  {unsupported(); return NULL;}
    virtual bool isGUI() const  {return false;}

    // UI functions (see also protected ones)
    virtual void bubble(cComponent *component, const char *text)  {}
    virtual std::string gets(const char *prompt, const char *defaultreply=NULL)  {unsupported(); return "";}
    virtual cEnvir& flush()  {::fflush(stdout); return *this;}

    // RNGs
    virtual int getNumRNGs() const {return 0;}
    virtual cRNG *getRNG(int k)  {unsupported(); return NULL;}
    virtual void getRNGMappingFor(cComponent *component)  {component->setRNGMap(0,NULL);}

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname)  {return NULL;}
    virtual void deregisterOutputVector(void *vechandle)  {}
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value)  {}
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value)  {return false;}

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL)  {}
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL)  {}

    virtual void addResultRecorders(cComponent *component, simsignal_t signal, const char *statisticName, cProperty *statisticTemplateProperty) {}

    // snapshot file
    virtual std::ostream *getStreamForSnapshot()  {unsupported(); return NULL;}
    virtual void releaseStreamForSnapshot(std::ostream *os)  {unsupported();}

    // misc
    virtual int getArgCount() const  {unsupported(); return 0;}
    virtual char **getArgVector() const  {unsupported(); return NULL;}
    virtual int getParsimProcId() const {return 0;}
    virtual int getParsimNumPartitions() const {return 1;}
    virtual unsigned long getUniqueNumber()  {unsupported(); return 0;}
    virtual bool idle()  {return false;}
    virtual void attachDebugger() {}
};

void StaticEnv::undisposedObject(cObject *obj)
{
    if (!cStaticFlag::isSet())
    {
        ::printf("<!> WARNING: global object variable (DISCOURAGED) detected: "
                 "(%s)`%s' at %p\n", obj->getClassName(), obj->getFullPath().c_str(), obj);
    }
}

static StaticEnv staticEnv;

// cSimulation's global variables
cEnvir *cSimulation::evPtr = &staticEnv;
cEnvir *cSimulation::staticEvPtr = &staticEnv;

cSimulation *cSimulation::simPtr = NULL;

NAMESPACE_END
