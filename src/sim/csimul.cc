//=========================================================================
//  CSIMUL.CC - part of
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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>    // strcpy
#include <stdio.h>     // vsprintf()
#include <assert.h>
#include "cmodule.h"
#include "csimplemodule.h"
#include "cmessage.h"
#include "csimul.h"
#include "cscheduler.h"
#include "cenvir.h"
#include "ctypes.h"
#include "cpar.h"
#include "cstat.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

#ifdef WITH_NETBUILDER
#include "netbuilder/loadnedfile.h"
#endif

using std::ostream;


#ifdef DEVELOPER_DEBUG
#include <set>
extern std::set<cObject*> objectlist;
void printAllObjects();
#endif


//==========================================================================
//=== Global object:

cSimulation simulation("simulation");

//==========================================================================

// writing date and time on a stream - used in cSimulation::writeresult(..)

ostream& operator<<(ostream& os, struct tm d)
{
    static char *month[] =
       {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    os << month[d.tm_mon] << ' ' << (int)d.tm_mday << ", " <<
          (1900+d.tm_year) << ", ";
    os << (int)d.tm_hour << ':' <<
          (int)d.tm_min << ':' <<
          (int)d.tm_sec;
    return os;
}


//==========================================================================
//=== cSimulation - member functions

cSimulation::cSimulation(const cSimulation& r) :
 cObject(r)
{
    assert(this==&simulation);

    setName(r.name());
    vect=NULL;
    schedulerp=NULL;
    operator=(r);
}

cSimulation::cSimulation(const char *name) :
 cObject(name),
 msgQueue( "scheduled-events" )
{
    assert(this==&simulation);

    runningmodp = NULL;
    contextmodp = NULL;
    contexttype = CTX_EVENT;

    systemmodp = NULL;
    schedulerp = NULL;

    backtomod = NULL;

    delta = 32;
    size = 0;
    last_id = 0;  // vect[0] is not used for historical reasons
    vect = NULL;

    // err = eOK; -- commented out to enable errors prior to starting main()
    networktype = NULL;
    run_number = 0;

    // see init()
}

cSimulation::~cSimulation()
{
    // see shutdown()
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
    // let go of msgQueue (removeFromOwnershipTree() cannot be called)
    msgQueue.ownerp = NULL;
}

void cSimulation::forEachChild(cVisitor *v)
{
    if (systemmodp!=NULL)
        v->visit(systemmodp);
    v->visit(&msgQueue);
}

std::string cSimulation::fullPath() const
{
    return std::string(fullPath(fullpathbuf,MAX_OBJECTFULLPATH));
}

const char *cSimulation::fullPath(char *buffer, int bufsize) const
{
    // check we got a decent buffer
    if (!buffer || bufsize<4)
    {
        if (buffer) buffer[0]='\0';
        return "(fullPath(): no buffer or buffer too small)";
    }

    // add our own name
    opp_strprettytrunc(buffer, fullName(), bufsize-1);
    return buffer;
}

class cSnapshotWriterVisitor : public cVisitor
{
  protected:
    ostream& os;
  public:
    cSnapshotWriterVisitor(ostream& ostr) : os(ostr) {}
    virtual void visit(cObject *obj) {
        obj->writeTo(os);
        if (os.fail()) throw EndTraversalException();
        obj->forEachChild(this);
    }
};

bool cSimulation::snapshot(cObject *object, const char *label)
{
    if (!object)
        throw new cRuntimeError("snapshot(): object pointer is NULL");

    ostream *osptr = ev.getStreamForSnapshot();
    if (!osptr)
        throw new cRuntimeError("Could not create stream for snapshot");

    ostream& os = *osptr;

    os << "================================================" << "\n";
    os << "||               SNAPSHOT                     ||" << "\n";
    os << "================================================" << "\n";
    os << "| Of object:    `" << object->fullPath() << "'" << "\n";
    os << "| Label:        `" << (label?label:"") << "'" << "\n";
    os << "| Sim. time:     " << simtimeToStr(simTime()) << "\n";
    os << "| Network:      `" << networktype->name() << "'\n";
    os << "| Run no.        " << run_number << '\n';
    if (contextModule())
        os << "| Initiated from: `" << contextModule()->fullPath() << "'\n";
    else
        os << "| Initiated by:  user\n";
    os << "================================================" << "\n\n";

    cSnapshotWriterVisitor v(os);
    v.process(object);

    bool success = !os.fail();
    ev.releaseStreamForSnapshot(&os);

    if (!success)
        throw new cRuntimeError("Could not write snapshot");
    return success;
}

static void writemodule( ostream& os, cModule& m, int indent )
{
    static char sp[] = "                                                 ";
    os << (sp+sizeof(sp)-indent) << "`" << m.fullName() << "'";
    os << " id=" << m.id() << " ";
    //os <<  (m.isSimple() ? "simple" : "compound");
    os <<  "(" << m.moduleType()->name() << ")\n";
}

static void writesubmodules(ostream& os, cModule *p, int indent )
{
    writemodule( os, *p, indent );
    for (int i=0; i<=simulation.lastModuleId(); i++)
        if (simulation.module(i) && p==simulation.module(i)->parentModule())
            writesubmodules(os, simulation.module(i), indent+4 );
}

void cSimulation::writeContents( ostream& os )
{
    os << "  Modules in the network:\n";
    writesubmodules(os, systemModule(), 5 );
}

void cSimulation::setScheduler(cScheduler *sched)
{
    if (schedulerp)
        throw new cRuntimeError(this, "setScheduler() can only be called once");
    schedulerp = sched;
}

void cSimulation::loadNedFile(const char *nedfile)
{
#ifdef WITH_NETBUILDER
    ::loadNedFile(nedfile, false);
#else
    throw new cRuntimeError("cannot load `%s': simulation kernel was compiled without "
                            "support for dynamic loading of NED files (WITH_NETBUILDER=no)", nedfile);
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
    int id = mod->id();
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

cModule *cSimulation::moduleByPath(const char *path) const
{
    // start tokenizing the path (path format: "SysModule.DemandGen[2].Source")
    opp_string pathbuf(path);
    char *s = strtok(pathbuf.buffer(),".");

    // search starts from system module
    cModule *modp = systemModule();
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
            modp = modp->submodule(s);  // no index given
        }
        else
        {
            if (s[strlen(s)-1]!=']')
                throw new cRuntimeError("moduleByPath(): syntax error in path `%s'", path);
            *b='\0';
            modp = modp->submodule(s,atoi(b+1));
        }
        s = strtok(NULL,".");
    }

    return modp;  // NULL if not found
}

void cSimulation::setupNetwork(cNetworkType *network, int run_num)
{
#ifdef DEVELOPER_DEBUG
    printf("DEBUG: before setupNetwork: %d objects\n", cObject::liveObjectCount());
    objectlist.clear();
#endif
    if (!network)
        throw new cRuntimeError(eNONET);

    // set run number
    run_number = run_num;

    // set cNetworkType pointer
    networktype = network;

    // just to be sure
    msgQueue.clear();

    try
    {
        // call NEDC-generated network setup function
        cContextTypeSwitcher tmp(CTX_BUILD);
        networktype->setupNetwork();
    }
    catch (cException *)
    {
        // we could clean up the whole stuff with deleteNetwork()
        // before passing the exception back, but it is dangerous.
        // Module destructors may try to delete uninitialized pointers
        // and crash. (Often ptrs incorrectly get initialized in initialize()
        // and not in the constructor.)
        //deleteNetwork();
        throw;
    }
    catch (std::exception e)
    {
        // omit deleteNetwork() -- see note above
        //deleteNetwork();
        throw new cRuntimeError("standard C++ exception %s: %s",
                                opp_typename(typeid(e)), e.what());
    }
    //catch (...) -- this is probably not a good idea because it makes debugging more difficult
    //{
    //    deleteNetwork();
    //    throw new cRuntimeError("unknown exception occurred");
    //}
}

void cSimulation::startRun()
{
    sim_time = 0;
    event_num = 0;
    backtomod = NULL;

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
        systemmodp->scheduleStart(0.0);
        systemmodp->callInitialize();
    }
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

    if (runningmodp!=NULL)
        throw new cRuntimeError("Attempt to delete network during simulation");

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

    // clear remaining messages (module dtors may have cancelled & deleted some of them)
    msgQueue.clear();

#ifdef DEVELOPER_DEBUG
    printf("DEBUG: after deleteNetwork: %d objects\n", cObject::liveObjectCount());
    printAllObjects();
#endif

}

cSimpleModule *cSimulation::selectNextModule()
{
    // is a send() or a pause() pending?
    if (backtomod!=NULL)
    {
        cSimpleModule *p = backtomod;
        return p;
    }

    // determine next event. Normally (with sequential simulation),
    // the scheduler just returns msgQueue->peekFirst().
    cMessage *msg = schedulerp->getNextEvent();
    if (!msg)
        return NULL; // scheduler got interrupted while waiting

    // check if destination module exists and is still running
    cSimpleModule *modp = (cSimpleModule *)vect[msg->arrivalModuleId()];
    if (!modp || modp->moduleState()==sENDED)
    {
        // Deleted/ended modules may have self-messages and sent messages
        // pending for them. Here we choose just to ignore them (delete them without
        // any error or warning). Rationale: if we thew an error here, it would
        // be very difficult to delete compound modules in which simple modules
        // send messages to one another -- each and every simple module
        // would have to be notified in advance that they're "shutting down",
        // and even then, deletion could progress only after waiting the maximum
        // propagation delay to elapse.
        delete msgQueue.getFirst();
        return selectNextModule();
    }

    // advance simulation time
    sim_time = msg->arrivalTime();

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
    return msg==NULL ? -1 : msg->arrivalTime();
}

cSimpleModule *cSimulation::guessNextModule()
{
    cMessage *msg = guessNextEvent();
    if (!msg)
        return NULL;

    // check if dest module exists and still running
    if (msg->arrivalModuleId()==-1)
        return NULL;
    cSimpleModule *modp = (cSimpleModule *)vect[msg->arrivalModuleId()];
    if (!modp)
        return NULL;
    if (modp->moduleState()==sENDED)
        return NULL;
    return modp;
}

void cSimulation::transferTo(cSimpleModule *modp)
{
    if (modp==NULL)
        throw new cRuntimeError("transferTo(): attempt to transfer to NULL");

    // switch to activity() of the simple module
    simulation.exception = NULL;
    runningmodp = modp;
    cCoroutine::switchTo(modp->coroutine);

    // if exception occurred in activity(), take it from cSimpleModule::activate() and pass it up
    if (simulation.exception)
    {
        // alas, type info was lost, so we have to recover manually...
        // TBD change to using dynamic_cast
        if (simulation.exception_type==0)
            throw (cException *)simulation.exception;
        else if (simulation.exception_type==1)
            throw (cTerminationException *)simulation.exception;
        else if (simulation.exception_type==2)
            throw (cEndModuleException *)simulation.exception;
        else
            throw new cRuntimeError("some exception occurred");
    }

    if (modp->stackOverflow())
        throw new cRuntimeError("Stack violation in module (%s)%s: module stack too small? "
                                "Try increasing it in the class' Module_Class_Members() or constructor",
                                modp->className(), modp->fullPath().c_str());
}

void cSimulation::doOneEvent(cSimpleModule *mod)
{
    // switch to the module's context
    setContextModule(mod);
    setContextType(CTX_EVENT);

    try
    {
        if (mod->usesActivity())
        {
            // switch to the coroutine of the module's activity(). We'll get back control
            // when the module executes a receive() or wait() call.
            // If there was an error during simulation, the call will throw an exception
            // (which originally occurred inside activity()).
            transferTo( mod );
        }
        else
        {
            // get event to be handled
            cMessage *msg = msgQueue.getFirst();

            // notify the environment about the message
            ev.messageDelivered( msg );

            // if there was an error during simulation, handleMessage() will come back
            // with an exception
            mod->handleMessage( msg );
        }
    }
    catch (cEndModuleException *e)
    {
        // handle locally
        // TBD make separate cDeleteModuleException
        setGlobalContext();
        if (e->moduleToBeDeleted())
            delete mod;
        delete e;
    }
    catch (cException *)
    {
        // temporarily catch the exception to restore global context
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
    if (runningmodp!=NULL)
    {
        runningmodp = NULL;
        cCoroutine::switchToMain();     // stack switch
    }
}

void cSimulation::setContextModule(cModule *p)
{
    contextmodp = p;
    cObject::setDefaultOwner(p);
}

cSimpleModule *cSimulation::contextSimpleModule() const
{
    // cannot go inline (upward cast would require including cmodule.h in csimul.h)
    if (!contextmodp || !contextmodp->isSimple())
        return NULL;
    return (cSimpleModule *)contextmodp;
}

unsigned long cSimulation::getUniqueNumber()
{
    return ev.getUniqueNumber();
}
