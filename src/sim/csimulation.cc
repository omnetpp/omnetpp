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
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "common/stringutil.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cscheduler.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cparimpl.h"
#include "omnetpp/chasher.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/ccoroutine.h"
#include "omnetpp/clifecyclelistener.h"
#include "omnetpp/platdep/platmisc.h"  // for DEBUG_TRAP

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

#ifdef WITH_NETBUILDER
#include "sim/netbuilder/cnedloader.h"
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

/**
 * Stops the simulation at the time it is scheduled for.
 * Used internally by cSimulation.
 */
class SIM_API cEndSimulationEvent : public cEvent, public noncopyable
{
    public:
        cEndSimulationEvent(const char *name, simtime_t simTimeLimit) : cEvent(name) {
            setArrivalTime(simTimeLimit);
            setSchedulingPriority(SHRT_MAX);  // lowest priority
        }
        virtual cEvent *dup() const {copyNotSupported(); return NULL;}
        virtual cObject *getTargetObject() const {return NULL;}
        virtual void execute() {throw cTerminationException(E_SIMTIME);}
};

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
#ifdef USE_OMNETPP4x_FINGERPRINTS
    lastVersion4ModuleId = 0;
#endif
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

    delete ownEvPtr;
    delete hasherp;
    delete schedulerp;
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

    if (schedulerp) {
        ev.removeLifecycleListener(schedulerp);
        delete schedulerp;
    }

    schedulerp = sch;
    schedulerp->setSimulation(this);
    ev.addLifecycleListener(schedulerp);
}

void cSimulation::setSimulationTimeLimit(simtime_t simTimeLimit)
{
#ifndef USE_OMNETPP4x_FINGERPRINTS
    getMessageQueue().insert(new cEndSimulationEvent("endsimulation", simTimeLimit));
#else
    // In 4.x fingerprints mode, we check simTimeLimit manually in EnvirBase::checkTimeLimits()
#endif
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

int cSimulation::registerComponent(cComponent *component)
{
    last_id++;

    if (last_id>=size)
    {
        // vector full, grow by delta
        cComponent **v = new cComponent *[size+delta];
        memcpy(v, vect, sizeof(cComponent*)*size );
        for (int i=size; i<size+delta; i++) v[i]=NULL;
        delete [] vect;
        vect = v;
        size += delta;
    }

    int id = last_id;
    vect[id] = component;
    component->componentId = id;
#ifdef USE_OMNETPP4x_FINGERPRINTS
    if (component->isModule())
        ((cModule *)component)->version4ModuleId = ++lastVersion4ModuleId;
#endif
    return id;
}

void cSimulation::deregisterComponent(cComponent *component)
{
    int id = component->componentId;
    component->componentId = -1;
    vect[id] = NULL;

    if (component==systemmodp)
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
        throw cRuntimeError(E_NONET);
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
        ev.notifyLifecycleListeners(LF_PRE_NETWORK_SETUP);
        cModule *mod = networktype->create(networktype->getName(), NULL);
        mod->finalizeParameters();
        mod->buildInside();
        ev.notifyLifecycleListeners(LF_POST_NETWORK_SETUP);
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

void cSimulation::callInitialize()
{
    checkActive();

    // reset counters. Note msgQueue.clear() was already called from setupNetwork()
    sim_time = 0;
    event_num = 0; // initialize() has event number 0
    trap_on_next_event = false;
    cMessage::resetMessageCounters();

    simulationstage = CTX_INITIALIZE;

    // prepare simple modules for simulation run:
    //    1. create starter message for all modules,
    //    2. then call initialize() for them (recursively)
    //  This order is important because initialize() functions might contain
    //  send() calls which could otherwise insert msgs BEFORE starter messages
    //  for the destination module and cause trouble in cSimpleMod's activate().
    if (systemmodp)
    {
        cContextSwitcher tmp(systemmodp);
        systemmodp->scheduleStart(SIMTIME_ZERO);
        ev.notifyLifecycleListeners(LF_PRE_NETWORK_INITIALIZE);
        systemmodp->callInitialize();
        ev.notifyLifecycleListeners(LF_POST_NETWORK_INITIALIZE);
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
        ev.notifyLifecycleListeners(LF_PRE_NETWORK_FINISH);
        systemmodp->callFinish();
        ev.notifyLifecycleListeners(LF_POST_NETWORK_FINISH);
    }
}

void cSimulation::deleteNetwork()
{
    if (!systemmodp)
        return;  // network already deleted

    if (cSimulation::getActiveSimulation() != this)
        throw cRuntimeError("cSimulation: cannot invoke deleteNetwork() on an instance that is not the active one (see cSimulation::getActiveSimulation())"); // because cModule::deleteModule() would crash

    if (getContextModule()!=NULL)
        throw cRuntimeError("Attempt to delete network during simulation");

    simulationstage = CTX_CLEANUP;

    ev.notifyLifecycleListeners(LF_PRE_NETWORK_DELETE);

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
#ifdef USE_OMNETPP4x_FINGERPRINTS
    lastVersion4ModuleId = 0;
#endif

    networktype = NULL;

    //FIXME todo delete cParImpl caches too (cParImplCache, cParImplCache2)
    cModule::clearNamePools();

    ev.notifyLifecycleListeners(LF_POST_NETWORK_DELETE);

    // clear remaining messages (module dtors may have cancelled & deleted some of them)
    msgQueue.clear();

    simulationstage = CTX_NONE;

#ifdef DEVELOPER_DEBUG
    printf("DEBUG: after deleteNetwork: %d objects\n", cOwnedObject::getLiveObjectCount());
    printAllObjects();
#endif

}

cEvent *cSimulation::takeNextEvent()
{
    // determine next event. Normally (with sequential simulation),
    // the scheduler just returns msgQueue->peekFirst().
    cEvent *event = schedulerp->takeNextEvent();
    if (!event)
        return NULL;

    ASSERT(!event->isStale()); // it's the scheduler's task to discard stale events

    // advance simulation time
    sim_time = event->getArrivalTime();

    return event;
}

void cSimulation::putBackEvent(cEvent *event)
{
    schedulerp->putBackEvent(event);
}

cEvent *cSimulation::guessNextEvent()
{
    return schedulerp->guessNextEvent();
}

simtime_t cSimulation::guessNextSimtime()
{
    cEvent *event = guessNextEvent();
    return event==NULL ? -1 : event->getArrivalTime();
}

cSimpleModule *cSimulation::guessNextModule()
{
    cEvent *event = guessNextEvent();
    cMessage *msg = dynamic_cast<cMessage *>(event);
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
    if (!modp)
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

void cSimulation::executeEvent(cEvent *event)
{
#ifndef NDEBUG
    checkActive();
#endif

    setContextType(CTX_EVENT);

    // notify the environment about the event (writes eventlog, etc.)
    EVCB.simulationEvent(event);

    // store arrival event number of this message; it is useful input for the
    // sequence chart tool if the message doesn't get immediately deleted or
    // sent out again
    event->setPreviousEventNumber(event_num);

    cSimpleModule *mod = NULL;
    try
    {
        if (event->isMessage())
        {
            cMessage *msg = static_cast<cMessage*>(event);
            mod = static_cast<cSimpleModule *>(msg->getArrivalModule());  // existence and simpleness is asserted in cMessage::isStale()
            doMessageEvent(msg, mod);
        }
        else
        {
            DEBUG_TRAP_IF_REQUESTED; // ABOUT TO PROCESS THE EVENT YOU REQUESTED TO DEBUG -- SELECT "STEP INTO" IN YOUR DEBUGGER
            event->execute();
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

    // Note: simulation time (as read via simTime() from modules) will be updated
    // in takeNextEvent(), called right before the next executeEvent().
    // Simtime must NOT be updated here, because it would interfere with parallel
    // simulation (cIdealSimulationProtocol, etc) that relies on simTime()
    // returning the time of the last executed event. If Tkenv wants to display
    // the time of the next event, it should call guessNextSimtime().
}

void cSimulation::doMessageEvent(cMessage *msg, cSimpleModule *mod)
{
    // switch to the module's context
    setContext(mod);

    // give msg to mod (set ownership)
    mod->take(msg);

    if (getHasher())
    {
        // note: there's no value in adding getEventNumber()
        cHasher *hasher = getHasher();
        hasher->add(SIMTIME_RAW(simTime()));
#ifdef USE_OMNETPP4x_FINGERPRINTS
        hasher->add(mod->getVersion4ModuleId());
#else
        hasher->add(mod->getFullPath().c_str());
        hasher->add(msg->getClassName());
        hasher->add(msg->getKind());
        if (msg->getControlInfo())
            hasher->add(msg->getControlInfo()->getClassName());
        if (msg->isPacket())
            hasher->add(((cPacket *)msg)->getBitLength());
#endif
    }

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

void cSimulation::insertEvent(cEvent *event)
{
    event->setPreviousEventNumber(event_num);
    simulation.msgQueue.insert(event);
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
    virtual void putsmsg(const char *msg) {::printf("\n<!> %s\n\n", msg);}
    virtual bool askyesno(const char *msg)  {unsupported(); return false;}

  public:
    // constructor, destructor
    StaticEnv() {}
    virtual ~StaticEnv() {}

    // eventlog callback interface
    virtual void objectDeleted(cObject *object) {}
    virtual void simulationEvent(cEvent *event)  {}
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
    virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId)  {}
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
    virtual void log(cLogEntry *entry) {}

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

    // lifecycle listeners
    virtual void addLifecycleListener(cISimulationLifecycleListener *listener) {}
    virtual void removeLifecycleListener(cISimulationLifecycleListener *listener) {}
    virtual void notifyLifecycleListeners(SimulationLifecycleEventType eventType, cObject *details) {}
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
