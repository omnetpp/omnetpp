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

#include <string.h>    // strcpy
#include <stdio.h>     // vsprintf()
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

cSimulation simulation("simulation");


// writing date and time on a stream - used in cSimulation::writeresult(..)
ostream& operator<<(ostream& os, struct tm d)
{
    static const char *month[] =
       {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    os << month[d.tm_mon] << ' ' << (int)d.tm_mday << ", " <<
          (1900+d.tm_year) << ", ";
    os << (int)d.tm_hour << ':' <<
          (int)d.tm_min << ':' <<
          (int)d.tm_sec;
    return os;
}


cSimulation::cSimulation(const char *name) : cNoncopyableOwnedObject(name, false)
{
    ASSERT(this==&simulation);

    msgQueue.setName("scheduled-events");

    activitymodp = NULL;
    contextmodp = NULL;
    contexttype = CTX_EVENT;

    systemmodp = NULL;
    schedulerp = NULL;

    delta = 32;
    size = 0;
    last_id = 0;  // vect[0] is not used for historical reasons
    vect = NULL;

    // err = eOK; -- commented out to enable errors prior to starting main()
    networktype = NULL;

    hasherp = NULL;

    // see init()
}

cSimulation::~cSimulation()
{
    // see shutdown()
    msgQueue.ownerp = NULL; //XXX in case shutdown() wasn't called, like with exit()
}

void cSimulation::init()
{
    // remove ourselves from ownership tree because global variables
    // shouldn't be destroyed via operator delete. This cannot be done
    // from the ctor, because this method calls virtual functions of
    // defaultList, and defaultList might not be initialized yet at that point.
    // take() cannot be done from the ctor for the same reason.
    removeFromOwnershipTree();
    take(&msgQueue);
}

void cSimulation::shutdown()
{
    // deleteNetwork() is better called before the dtor (which runs after main())
    deleteNetwork();

    // let go of msgQueue (removeFromOwnershipTree() cannot be called here)
    msgQueue.ownerp = NULL;

    delete hasherp;

#ifdef WITH_NETBUILDER
    cNEDLoader::clear();
#endif
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

static void writemodule( ostream& os, cModule& m, int indent )
{
    static char sp[] = "                                                 ";
    os << (sp+sizeof(sp)-indent) << "`" << m.getFullName() << "'";
    os << " id=" << m.getId() << " ";
    //os <<  (m.isSimple() ? "simple" : "compound");
    os <<  "(" << m.getModuleType()->getName() << ")\n";
}

static void writesubmodules(ostream& os, cModule *p, int indent )
{
    writemodule( os, *p, indent );
    for (int i=0; i<=simulation.getLastModuleId(); i++)
        if (simulation.getModule(i) && p==simulation.getModule(i)->getParentModule())
            writesubmodules(os, simulation.getModule(i), indent+4 );
}

void cSimulation::setScheduler(cScheduler *sched)
{
    if (schedulerp)
        throw cRuntimeError(this, "setScheduler() can only be called once");
    schedulerp = sched;
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

int cSimulation::loadNedSourceFolder(const char *folder)
{
#ifdef WITH_NETBUILDER
    return cNEDLoader::getInstance()->loadNedSourceFolder(folder);
#else
    throw cRuntimeError("cannot load NED files from `%s': simulation kernel was compiled without "
                        "support for dynamic loading of NED files (WITH_NETBUILDER=no)", folder);
#endif
}

std::string cSimulation::getNedPackageForFolder(const char *folder) const
{
#ifdef WITH_NETBUILDER
    return cNEDLoader::getInstance()->getNedPackageForFolder(folder);
#else
    return "-";
#endif
}

void cSimulation::doneLoadingNedFiles()
{
#ifdef WITH_NETBUILDER
    cNEDLoader::getInstance()->doneLoadingNedFiles();
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
    // start tokenizing the path (path format: "SysModule.DemandGen[2].Source")
    opp_string pathbuf(path);
    char *s = strtok(pathbuf.buffer(),".");

    // search starts from system module
    cModule *modp = getSystemModule();
    if (!modp) return NULL;

    // 1st component in the path may be the system module, skip it then
    if (modp->isName(s))
        s = strtok(NULL,".");

    // match components of the path
    while (s && modp)
    {
        char *b;
        if ((b=strchr(s,'['))==NULL)
        {
            modp = modp->getSubmodule(s);  // no index given
        }
        else
        {
            if (s[strlen(s)-1]!=']')
                throw cRuntimeError("getModuleByPath(): syntax error in path `%s'", path);
            *b='\0';
            modp = modp->getSubmodule(s,atoi(b+1));
        }
        s = strtok(NULL,".");
    }

    return modp;  // NULL if not found
}

void cSimulation::setupNetwork(cModuleType *network)
{
#ifdef DEVELOPER_DEBUG
    printf("DEBUG: before setupNetwork: %d objects\n", cOwnedObject::getLiveObjectCount());
    objectlist.clear();
#endif

    if (!network)
        throw cRuntimeError(eNONET);
    if (!network->isNetwork())
        throw cRuntimeError("setupNetwork: `%s' is not a network", network->fullName());

    // set cNetworkType pointer
    networktype = network;

    // just to be sure
    msgQueue.clear();

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

    printf("setupNetwork finished, cParImpl objects in use: %ld\n", cParImpl::getLiveParImplObjectCount()); //XXX
}

void cSimulation::startRun()
{
    sim_time = 0;
    event_num = 0; // initialize() has event number 0

    // NOTE: should NOT call msgQueue.clear() here because the parallel
    // simulation library (cNullMessageProtocol::startRun()) has already
    // put messages there! It has been called from setupNetwork() anyway.

    // reset message counters
    cMessage::resetMessageCounters();

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
}

void cSimulation::callFinish()
{
    // call user-defined finish() functions for all modules recursively
    if (systemmodp)
    {
        systemmodp->callFinish();
    }
}

void cSimulation::endRun()
{
}

void cSimulation::deleteNetwork()
{
    if (!systemmodp)
        return;  // network already deleted

    if (getContextModule()!=NULL)
        throw cRuntimeError("Attempt to delete network during simulation");

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
    simulation.exception = NULL;
    activitymodp = modp;
    cCoroutine::switchTo(modp->coroutine);

    if (modp->hasStackOverflow())
        throw cRuntimeError("Stack violation in module (%s)%s: module stack too small? "
                            "Try increasing it in the class' Module_Class_Members() or constructor",
                            modp->getClassName(), modp->getFullPath().c_str());

    // if exception occurred in activity(), re-throw it. This allows us to handle
    // handleMessage() and activity() in an uniform way in the upper layer.
    if (simulation.exception)
    {
        cException *e = simulation.exception;
        simulation.exception = NULL;

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

void cSimulation::doOneEvent(cSimpleModule *mod)  //FIXME why do we need the cSimpleModule argument??? remove!!!
{
    // switch to the module's context
    setContext(mod);
    setContextType(CTX_EVENT);

    if (getHasher())
    {
        // note: there's probably no value in adding getEventNumber()
        getHasher()->add(SIMTIME_RAW(simTime()));
        getHasher()->add(mod->getId());
        //XXX msg id too?
    }

    // get event to be handled
    cMessage *msg = msgQueue.removeFirst();

    // notify the environment about the message
    EVCB.simulationEvent(msg);

    // store arrival event number of this message; it is useful input for the
    // sequence chart tool if the message doesn't get immediately deleted or
    // sent out again
    msg->setPreviousEventNumber(event_num);

    try
    {
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
            // if there was an error during simulation, handleMessage() will come back
            // with an exception
            mod->handleMessage(msg);
        }
    }
    catch (cDeleteModuleException& e)
    {
        setGlobalContext();
        delete mod;
    }
    catch (std::exception&)
    {
        // restore global context before throwing exception further
        setGlobalContext();
        throw;
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

NAMESPACE_END


