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

#include <cstring>
#include <cstdio>
#include <climits>
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
#define DEBUG_TRAP_IF_REQUESTED   {if (trapOnNextEvent) {trapOnNextEvent=false; DEBUG_TRAP;}}
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
        virtual cEvent *dup() const {copyNotSupported(); return nullptr;}
        virtual cObject *getTargetObject() const {return nullptr;}
        virtual void execute() {throw cTerminationException(E_SIMTIME);}
};

cSimulation::cSimulation(const char *name, cEnvir *env) : cNamedObject(name, false)
{
    ASSERT(cStaticFlag::isSet()); // cannot be instantiated as global variable

    envir = env;

    currentActivityModule = nullptr;
    contextComponent = nullptr;

    simulationStage = CTX_NONE;
    contextType = CTX_NONE;

    systemModule = nullptr;
    scheduler = nullptr;

    delta = 32;
    size = 0;
    lastComponentId = 0;  // vect[0] is not used for historical reasons
#ifdef USE_OMNETPP4x_FINGERPRINTS
    lastVersion4ModuleId = 0;
#endif
    componentv = nullptr;

    networkType = nullptr;
    hasher = nullptr;

    currentSimtime = SIMTIME_ZERO;
    currentEventNumber = 0;
    trapOnNextEvent = false;

    msgQueue.setName("scheduled-events");
    take(&msgQueue);

    // install a default scheduler
    setScheduler(new cSequentialScheduler());
}

cSimulation::~cSimulation()
{
    if (this==activeSimulation)
        // NOTE: subclass destructors will not be called, but the simulation will stop anyway
        throw cRuntimeError(this, "cannot delete the active simulation manager object");

    deleteNetwork();

    delete envir;
    delete hasher;
    delete scheduler;
    drop(&msgQueue);
}

void cSimulation::setActiveSimulation(cSimulation *sim)
{
    activeSimulation = sim;
    activeEnvir = sim==nullptr ? staticEnvir : sim->envir;
}

void cSimulation::setStaticEnvir(cEnvir *env)
{
    if (!env)
         throw cRuntimeError("cSimulation::setStaticEnvir(): argument cannot be NULL");
    staticEnvir = env;
}

void cSimulation::forEachChild(cVisitor *v)
{
    if (systemModule!=nullptr)
        v->visit(systemModule);
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

    ostream *osptr = getEnvir()->getStreamForSnapshot();
    if (!osptr)
        throw cRuntimeError("Could not create stream for snapshot");

    ostream& os = *osptr;

    os << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    os << "<snapshot\n";
    os << "    object=\"" << xmlquote(object->getFullPath()) << "\"\n";
    os << "    label=\"" << xmlquote(label?label:"") << "\"\n";
    os << "    simtime=\"" << xmlquote(SIMTIME_STR(simTime())) << "\"\n";
    os << "    network=\"" << xmlquote(networkType?networkType->getName():"") << "\"\n";
    os << "    >\n";

    cSnapshotWriterVisitor v(os);
    v.process(object);

    os << "</snapshot>\n";

    bool success = !os.fail();
    getEnvir()->releaseStreamForSnapshot(&os);

    if (!success)
        throw cRuntimeError("Could not write snapshot");
    return success;
}

void cSimulation::setScheduler(cScheduler *sch)
{
    if (systemModule)
        throw cRuntimeError(this, "setScheduler(): cannot switch schedulers when a network is already set up");
    if (!sch)
        throw cRuntimeError(this, "setScheduler(): scheduler pointer is NULL");

    if (scheduler) {
        getEnvir()->removeLifecycleListener(scheduler);
        delete scheduler;
    }

    scheduler = sch;
    scheduler->setSimulation(this);
    getEnvir()->addLifecycleListener(scheduler);
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

void cSimulation::loadNedFile(const char *nedFilename, const char *expectedPackage, bool isXML)
{
#ifdef WITH_NETBUILDER
    cNEDLoader::getInstance()->loadNedFile(nedFilename, expectedPackage, isXML);
#else
    throw cRuntimeError("cannot load `%s': simulation kernel was compiled without "
                        "support for dynamic loading of NED files (WITH_NETBUILDER=no)", nedFilename);
#endif
}

void cSimulation::loadNedText(const char *name, const char *nedText, const char *expectedPackage, bool isXML)
{
#ifdef WITH_NETBUILDER
    cNEDLoader::getInstance()->loadNedText(name, nedText, expectedPackage, isXML);
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
    lastComponentId++;

    if (lastComponentId>=size)
    {
        // vector full, grow by delta
        cComponent **v = new cComponent *[size+delta];
        memcpy(v, componentv, sizeof(cComponent*)*size );
        for (int i=size; i<size+delta; i++) v[i]=nullptr;
        delete [] componentv;
        componentv = v;
        size += delta;
    }

    int id = lastComponentId;
    componentv[id] = component;
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
    componentv[id] = nullptr;

    if (component==systemModule)
    {
        drop(systemModule);
        systemModule = nullptr;
    }
}

void cSimulation::setSystemModule(cModule *module)
{
    systemModule = module;
    take(module);
}

cModule *cSimulation::getModuleByPath(const char *path) const
{
    cModule *modp = getSystemModule();
    if (!modp || !path || !path[0] || path[0] == '.' || path[0] == '^')
        return nullptr;
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
    networkType = network;

    // just to be sure
    msgQueue.clear();
    cComponent::clearSignalState();

    simulationStage = CTX_BUILD;

    try
    {
        // set up the network by instantiating the toplevel module
        cContextTypeSwitcher tmp(CTX_BUILD);
        getEnvir()->notifyLifecycleListeners(LF_PRE_NETWORK_SETUP);
        cModule *mod = networkType->create(networkType->getName(), nullptr);
        mod->finalizeParameters();
        mod->buildInside();
        getEnvir()->notifyLifecycleListeners(LF_POST_NETWORK_SETUP);
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
    currentSimtime = 0;
    currentEventNumber = 0; // initialize() has event number 0
    trapOnNextEvent = false;
    cMessage::resetMessageCounters();

    simulationStage = CTX_INITIALIZE;

    // prepare simple modules for simulation run:
    //    1. create starter message for all modules,
    //    2. then call initialize() for them (recursively)
    //  This order is important because initialize() functions might contain
    //  send() calls which could otherwise insert msgs BEFORE starter messages
    //  for the destination module and cause trouble in cSimpleMod's activate().
    if (systemModule)
    {
        cContextSwitcher tmp(systemModule);
        systemModule->scheduleStart(SIMTIME_ZERO);
        getEnvir()->notifyLifecycleListeners(LF_PRE_NETWORK_INITIALIZE);
        systemModule->callInitialize();
        getEnvir()->notifyLifecycleListeners(LF_POST_NETWORK_INITIALIZE);
    }

    currentEventNumber = 1; // events are numbered from 1

    simulationStage = CTX_EVENT;
}

void cSimulation::callFinish()
{
    checkActive();

    simulationStage = CTX_FINISH;

    // call user-defined finish() functions for all modules recursively
    if (systemModule)
    {
        getEnvir()->notifyLifecycleListeners(LF_PRE_NETWORK_FINISH);
        systemModule->callFinish();
        getEnvir()->notifyLifecycleListeners(LF_POST_NETWORK_FINISH);
    }
}

void cSimulation::deleteNetwork()
{
    if (!systemModule)
        return;  // network already deleted

    if (cSimulation::getActiveSimulation() != this)
        throw cRuntimeError("cSimulation: cannot invoke deleteNetwork() on an instance that is not the active one (see cSimulation::getActiveSimulation())"); // because cModule::deleteModule() would crash

    if (getContextModule()!=nullptr)
        throw cRuntimeError("Attempt to delete network during simulation");

    simulationStage = CTX_CLEANUP;

    getEnvir()->notifyLifecycleListeners(LF_PRE_NETWORK_DELETE);

    // delete all modules recursively
    systemModule->deleteModule();

    // make sure it was successful
    for (int i=1; i<size; i++)
        ASSERT(componentv[i]==nullptr);

    // and clean up
    delete [] componentv;
    componentv = nullptr;
    size = 0;
    lastComponentId = 0;
#ifdef USE_OMNETPP4x_FINGERPRINTS
    lastVersion4ModuleId = 0;
#endif

    networkType = nullptr;

    //FIXME todo delete cParImpl caches too (cParImplCache, cParImplCache2)
    cModule::clearNamePools();

    getEnvir()->notifyLifecycleListeners(LF_POST_NETWORK_DELETE);

    // clear remaining messages (module dtors may have cancelled & deleted some of them)
    msgQueue.clear();

    simulationStage = CTX_NONE;

#ifdef DEVELOPER_DEBUG
    printf("DEBUG: after deleteNetwork: %d objects\n", cOwnedObject::getLiveObjectCount());
    printAllObjects();
#endif

}

cEvent *cSimulation::takeNextEvent()
{
    // determine next event. Normally (with sequential simulation),
    // the scheduler just returns msgQueue->peekFirst().
    cEvent *event = scheduler->takeNextEvent();
    if (!event)
        return nullptr;

    ASSERT(!event->isStale()); // it's the scheduler's task to discard stale events

    // advance simulation time
    currentSimtime = event->getArrivalTime();

    return event;
}

void cSimulation::putBackEvent(cEvent *event)
{
    scheduler->putBackEvent(event);
}

cEvent *cSimulation::guessNextEvent()
{
    return scheduler->guessNextEvent();
}

simtime_t cSimulation::guessNextSimtime()
{
    cEvent *event = guessNextEvent();
    return event==nullptr ? -1 : event->getArrivalTime();
}

cSimpleModule *cSimulation::guessNextModule()
{
    cEvent *event = guessNextEvent();
    cMessage *msg = dynamic_cast<cMessage *>(event);
    if (!msg)
        return nullptr;

    // check that destination module still exists and hasn't terminated yet
    if (msg->getArrivalModuleId()==-1)
        return nullptr;
    cComponent *component = componentv[msg->getArrivalModuleId()];
    cSimpleModule *module = dynamic_cast<cSimpleModule *>(component);
    if (!module || module->isTerminated())
        return nullptr;
    return module;
}

void cSimulation::transferTo(cSimpleModule *module)
{
    if (!module)
        throw cRuntimeError("transferTo(): attempt to transfer to NULL");

    // switch to activity() of the simple module
    exception = nullptr;
    currentActivityModule = module;
    cCoroutine::switchTo(module->coroutine);

    if (module->hasStackOverflow())
        throw cRuntimeError("Stack violation in module (%s)%s: module stack too small? "
                            "Try increasing it in the class' Module_Class_Members() or constructor",
                            module->getClassName(), module->getFullPath().c_str());

    // if exception occurred in activity(), re-throw it. This allows us to handle
    // handleMessage() and activity() in an uniform way in the upper layer.
    if (exception)
    {
        cException *e = exception;
        exception = nullptr;

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
    event->setPreviousEventNumber(currentEventNumber);

    cSimpleModule *module = nullptr;
    try
    {
        if (event->isMessage())
        {
            cMessage *msg = static_cast<cMessage*>(event);
            module = static_cast<cSimpleModule *>(msg->getArrivalModule());  // existence and simpleness is asserted in cMessage::isStale()
            doMessageEvent(msg, module);
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
        module->deleteModule();
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
    currentEventNumber++;

    // Note: simulation time (as read via simTime() from modules) will be updated
    // in takeNextEvent(), called right before the next executeEvent().
    // Simtime must NOT be updated here, because it would interfere with parallel
    // simulation (cIdealSimulationProtocol, etc) that relies on simTime()
    // returning the time of the last executed event. If Tkenv wants to display
    // the time of the next event, it should call guessNextSimtime().
}

void cSimulation::doMessageEvent(cMessage *msg, cSimpleModule *module)
{
    // switch to the module's context
    setContext(module);

    // give msg to mod (set ownership)
    module->take(msg);

    if (getHasher())
    {
        // note: there's no value in adding getEventNumber()
        cHasher *hasher = getHasher();
        hasher->add(SIMTIME_RAW(simTime()));
#ifdef USE_OMNETPP4x_FINGERPRINTS
        hasher->add(module->getVersion4ModuleId());
#else
        hasher->add(module->getFullPath().c_str());
        hasher->add(msg->getClassName());
        hasher->add(msg->getKind());
        if (msg->getControlInfo())
            hasher->add(msg->getControlInfo()->getClassName());
        if (msg->isPacket())
            hasher->add(((cPacket *)msg)->getBitLength());
#endif
    }

    if (!module->initialized())
        throw cRuntimeError(module, "Module not initialized (did you forget to invoke "
                "callInitialize() for a dynamically created module?)");

    if (module->usesActivity())
    {
        // switch to the coroutine of the module's activity(). We'll get back control
        // when the module executes a receive() or wait() call.
        // If there was an error during simulation, the call will throw an exception
        // (which originally occurred inside activity()).
        msgForActivity = msg;
        transferTo(module);
    }
    else
    {
        DEBUG_TRAP_IF_REQUESTED; // YOU ARE ABOUT TO ENTER THE handleMessage() CALL YOU REQUESTED -- SELECT "STEP INTO" IN YOUR DEBUGGER
        module->handleMessage(msg);
    }
}

void cSimulation::transferToMain()
{
    if (currentActivityModule!=nullptr)
    {
        currentActivityModule = nullptr;
        cCoroutine::switchToMain();     // stack switch
    }
}

void cSimulation::setContext(cComponent *p)
{
    contextComponent = p;
    cOwnedObject::setDefaultOwner(p);
}

cModule *cSimulation::getContextModule() const
{
    // cannot go inline (upward cast would require including cmodule.h in csimulation.h)
    if (!contextComponent || !contextComponent->isModule())
        return nullptr;
    return (cModule *)contextComponent;
}

cSimpleModule *cSimulation::getContextSimpleModule() const
{
    // cannot go inline (upward cast would require including cmodule.h in csimulation.h)
    if (!contextComponent || !contextComponent->isModule() || !((cModule *)contextComponent)->isSimple())
        return nullptr;
    return (cSimpleModule *)contextComponent;
}

unsigned long cSimulation::getUniqueNumber()
{
    return getEnvir()->getUniqueNumber();
}

void cSimulation::setHasher(cHasher *h)
{
    if (hasher)
        delete hasher;
    hasher = h;
}

void cSimulation::insertEvent(cEvent *event)
{
    event->setPreviousEventNumber(currentEventNumber);
    msgQueue.insert(event);
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
 * cObject's destructor which contains an <tt>getEnvir()->objectDeleted()</tt>.
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
    virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=nullptr)  {}
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
    virtual cXMLElement *getXMLDocument(const char *filename, const char *xpath=nullptr)  {unsupported(); return NULL;}
    virtual cXMLElement *getParsedXMLString(const char *content, const char *xpath=nullptr)  {unsupported(); return NULL;}
    virtual void forgetXMLDocument(const char *filename) {}
    virtual void forgetParsedXMLString(const char *content) {}
    virtual void flushXMLDocumentCache() {}
    virtual void flushXMLParsedContentCache() {}
    virtual unsigned getExtraStackForEnvir() const  {return 0;}
    virtual cConfiguration *getConfig()  {unsupported(); return nullptr;}
    virtual bool isGUI() const  {return false;}

    // UI functions (see also protected ones)
    virtual void bubble(cComponent *component, const char *text)  {}
    virtual std::string gets(const char *prompt, const char *defaultreply=nullptr)  {unsupported(); return "";}
    virtual cEnvir& flush()  {::fflush(stdout); return *this;}

    // RNGs
    virtual int getNumRNGs() const {return 0;}
    virtual cRNG *getRNG(int k)  {unsupported(); return nullptr;}
    virtual void getRNGMappingFor(cComponent *component)  {component->setRNGMap(0,nullptr);}

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname)  {return nullptr;}
    virtual void deregisterOutputVector(void *vechandle)  {}
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value)  {}
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value)  {return false;}

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=nullptr)  {}
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=nullptr)  {}

    virtual void addResultRecorders(cComponent *component, simsignal_t signal, const char *statisticName, cProperty *statisticTemplateProperty) {}

    // snapshot file
    virtual std::ostream *getStreamForSnapshot()  {unsupported(); return nullptr;}
    virtual void releaseStreamForSnapshot(std::ostream *os)  {unsupported();}

    // misc
    virtual int getArgCount() const  {unsupported(); return 0;}
    virtual char **getArgVector() const  {unsupported(); return nullptr;}
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
cEnvir *cSimulation::activeEnvir = &staticEnv;
cEnvir *cSimulation::staticEnvir = &staticEnv;

cSimulation *cSimulation::activeSimulation = nullptr;

NAMESPACE_END
