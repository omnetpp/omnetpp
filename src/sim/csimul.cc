//=========================================================================
//
//  CSIMUL.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Text tables:
//    modstate:  module states
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
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>    // strcpy
#include <stdio.h>     // vsprintf()
#include "cmodule.h"
#include "cmessage.h"
#include "csimul.h"
#include "cenvir.h"
#include "ctypes.h"
#include "cpar.h"
#include "cnetmod.h"
#include "cstat.h"
#include "cexception.h"


//==========================================================================
//=== Global object:

cSimulation simulation("simulation");

//==========================================================================

// auxiliary flag, used only locally (a dirty solution...)
static bool currentmod_was_deleted = false;

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
    setName(r.name());
    vect=NULL;
    operator=(r);
}

cSimulation::cSimulation(const char *name) :
 cObject(name),
 locals("simulation-locals"),
 runningmod_deleter(),
 msgQueue( "message-queue" )
{
    take( &locals );
    take( &msgQueue );

    runningmodp = NULL;
    contextmodp = NULL;
    locallistp = &locals;

    netmodp = NULL;

    backtomod = NULL;

    delta = 32;
    size = 0;
    last_id = 0;  // vect[0] is not used for historical reasons
    vect = NULL;

    // err = eOK; -- commented out to enable errors prior to starting main()
    networktype = NULL;
    run_number = 0;

    netif_check_freq = 1000;    // frequency of processing msgs from other segments
    netif_check_cntr = 0;
}

cSimulation::~cSimulation()
{
    deleteNetwork();
    delete netmodp;
}

static void runningmod_deleter_func(void *)
{
    // function to help dynamically created modules delete themselves
    for(;;)
    {
        simulation.deleteModule( simulation.runningModule()->id() );
        currentmod_was_deleted = true;  // checked & reset at cSimulation::doOneEvent()
        simulation.transferToMain();
    }
}

void cSimulation::init()
{
    runningmod_deleter.setup( runningmod_deleter_func, NULL, 16384 );
}

void cSimulation::forEach( ForeachFunc do_fn )
{
    if (do_fn(this,true))
    {
        if (systemmodp!=NULL)
            systemmodp->forEach( do_fn );
        msgQueue.forEach( do_fn );
    }
    do_fn(this,false);
}

const char *cSimulation::fullPath() const
{
    return fullPath(fullpathbuf,FULLPATHBUF_SIZE);
}

const char *cSimulation::fullPath(char *buffer, int bufsize) const
{
    // check we got a decent buffer
    if (!buffer || bufsize<4)
    {
        if (buffer) buffer[0]='\0';
        return "(fullPath(): no or too small buffer)";
    }

    // add our own name
    opp_strprettytrunc(buffer, fullName(), bufsize-1);
    return buffer;
}

static bool _do_writesnapshot(cObject *obj, bool beg, ostream& s)
{
    static ostream *os;

    if (!obj) {os = &s;return false;} //setup call

    if (os->fail()) return false;   // there was an error, quit
    if (beg) obj->writeTo( *os );
    return !os->fail();      // check stream status
}

bool cSimulation::snapshot(cObject *object, const char *label)
{
    if (!object)
        throw new cException("snapshot(): object pointer is NULL");

    ostream *osptr = ev.getStreamForSnapshot();
    if (!osptr)
        throw new cException("Could not create stream for snapshot");

    ostream& os = *osptr;

    os << "================================================" << "\n";
    os << "||               SNAPSHOT                     ||" << "\n";
    os << "================================================" << "\n";
    os << "| Of object:    `" << object->fullPath() << "'" << "\n";
    os << "| Label:        `" << (label?label:"") << "'" << "\n";
    os << "| Sim. time:     " << simtimeToStr(simTime()) << "\n";
    os << "| Network:      `" << networktype->name() << "'\n";
    os << "| Run no.        " << run_number << '\n';
    // os << "| Started at:    " << *localtime(&simbegtime) << '\n';
    // os << "| Time:          " << *localtime(&simendtime) << '\n';
    // os << "| Elapsed:       " << elapsedtime << " sec\n";
    //if (err)
    //   os << "| Simulation stopped with error message.\n";
    if (contextModule())
        os << "| Initiated from: `" << contextModule()->fullPath() << "'\n";
    else
        os << "| Initiated by:  user\n";
    os << "================================================" << "\n\n";

    _do_writesnapshot( NULL,false, os );         // setup
    object->forEach( (ForeachFunc)_do_writesnapshot );   // do

    bool success = !os.fail();
    ev.releaseStreamForSnapshot(&os);

    if (!success)
        throw new cException("Could not write snapshot");
    return success;
}

static void writemodule( ostream& os, cModule& m, int indent )
{
    static char sp[] = "                                                 ";
    os << (sp+sizeof(sp)-indent) << "`" << m.fullName() << "'";
    os << " #" << m.id() << " ";
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

void cSimulation::setNetInterface(cNetMod *netif)
{
    netmodp = netif;
    take( netif );
}

int cSimulation::addModule(cModule *mod)
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
    mod->setId(last_id);
    return last_id;
}

void cSimulation::deleteModule(int id)
{
    if (id<0 || id>last_id)
        throw new cException("cSimulation::deleteModule(): module id %d out of range",id);

    delete vect[id];
    vect[id] = NULL;
}

cModule *cSimulation::moduleByPath(const char *path) const
{
    // start tokenizing the path (path format: "SysModule.DemandGen[2].Source")
    opp_string pathbuf(path);
    char *s = strtok(pathbuf.buffer(),".");

    // search starts from system module
    cModule *modp = systemModule();

    // 1st component in the path may be the system module, skip it then
    if (modp->isName(s))
        s = strtok(pathbuf.buffer(),".");

    // match components of the path
    do {
        char *b;
        if ((b=strchr(s,'['))==NULL)
            modp = modp->submodule(s);  // no index given
        else
        {
            if (s[strlen(s)-1]!=']')
                throw new cException("moduleByPath(): syntax error in path `%s'", path);
            *b='\0';
            modp = modp->submodule(s,atoi(b+1));
        }
    } while ((s=strtok(NULL,"."))!=NULL && modp!=NULL);

    return modp;  // NULL if not found
}

// FIXME change according to doc comment...
void cSimulation::setupNetwork(cNetworkType *network, int run_num)
{
    // FIXME handle exceptions during the setup process!

    if (!network)
        throw new cException(eNONET);

    // set run number
    run_number = run_num;

    // set cNetworkType pointer
    networktype = network;

    try
    {
        // call NEDC-generated network setup function
        networktype->setupNetwork();

        // handle distributed execution
        if (netInterface()!=NULL)
        {
           // master: starts OMNeT++ on other hosts
           if (ev.runningMode()==MASTER_MODE)
           {
              // the hosts we need are the system module's machines
              cArray& machines = systemModule()->machinev;
              netInterface()->start_segments( machines, ev.argCount(), ev.argVector());

              // signal the slaves which run to set up
              //   this causes slaves to start building up the network
              netInterface()->send_runnumber( run_number );

              // process messages from other segments:
              //   display info/error messages, answer questions about param. values etc.
              netInterface()->process_netmsgs();
           }

           // all segments: match gate pairs
           netInterface()->setup_connections();

           // again: display possible info/error messages
           if (ev.runningMode()==MASTER_MODE)
           {
              netInterface()->process_netmsgs();
           }
        }
    }
    catch (cException *)
    {
        // if failed, clean up the whole stuff before passing exception back
        deleteNetwork();
        throw;
    }
}

// FIXME change according to doc comment...
void cSimulation::startRun()
{
    msgQueue.clear();
    sim_time = 0;
    event_num = 0;
    backtomod = NULL;
    netif_check_cntr = 0;

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

    // distributed execution:
    //  create a msg that will notify the network interface that all
    //  module activity()s have been started
    if (netInterface()!=NULL)
    {
        cMessage *msg = new cMessage;
        msg->setArrivalTime(0.0);
        simulation.msgQueue.insert( msg );
        netInterface()->after_modinit_msg = msg;
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

// FIXME change according to doc comment...
void cSimulation::endRun()
{
    if (netInterface()) // FIXME and we're slave
        netInterface()->stop_all_segments(); //????
}

// FIXME change according to doc comment...
void cSimulation::deleteNetwork()
{
    if (networktype==NULL)
        return;  // network already deleted

    if (runningmodp!=NULL)
        throw new cException("Attempt to delete network during simulation");

    if (netInterface()!=NULL)
    {
        if (ev.runningMode()==MASTER_MODE)
        {
            // process final messages (error displays etc) from slaves
            netInterface()->process_netmsgs();
        }

        // reset the network interface itself
        netInterface()->restart();
    }

    // clear remaining messages
    msgQueue.clear();

    // for (int i=1; i<size; i++) delete vect[i]; -- incorrect!!!
    //   (we own only the system module)

    if (systemmodp)  // delete whole network through ownership hierarchy
    {
        delete systemmodp;
        systemmodp = NULL;
    }

    delete [] vect;
    vect = NULL;
    size = 0;
    last_id = 0;

    setGlobalContext();

    networktype = NULL;
}

cSimpleModule *cSimulation::selectNextModule()
{
    // is a send() or a pause() pending?
    if (backtomod!=NULL)
    {
        cSimpleModule *p = backtomod;
        return p;
    }

    // no more events?
    if (msgQueue.empty())
    {
        // message queue empty.
        // However, if we run distributed, still there might be something
        // coming from the network, from other processes
        if (netInterface()!=NULL)
        {
             while (msgQueue.empty())
                 netInterface()->process_netmsg_blocking();
             return selectNextModule();
        }

        // seems like really end of the simulation run
        return NULL;
    }

    // we're about to return this message's destination module:
    cMessage *msg = msgQueue.peekFirst();

    // distributed execution stuff:
    if (netInterface()!=NULL)
    {
        // from time to time, process possible messages from other segments
        if (++netif_check_cntr>=netif_check_freq)
        {
            netif_check_cntr = 0;
            netInterface()->process_netmsgs();
            msg = msgQueue.peekFirst();
        }

        // sync_after_modinits() must be called after all local
        // simple module activity()s have been started
        if (msg==netInterface()->after_modinit_msg)
        {
            netInterface()->sync_after_modinits();

            netInterface()->after_modinit_msg = NULL;
            delete msgQueue.getFirst();
            return selectNextModule();
        }

        // possibly block on next syncpoint.
        // if blocked, new msgs may have arrived so re-do selectNextModule()
        if (netInterface()->block_on_syncpoint(msg->arrivalTime()))
            return selectNextModule();
    }

    // check if dest module exists and still running
    cSimpleModule *modp = (cSimpleModule *)vect[msg->arrivalModuleId()];
    if (!modp)
        throw new cException("Message's destination module no longer exists");

    if (modp->moduleState()==sENDED)
    {
        if (!msg->isSelfMessage())
            throw new cException("Message's destination module `%s' already terminated", modp->fullPath());

        // self-messages are OK, ignore them
        delete msgQueue.getFirst();
        return selectNextModule();
    }

    // advance simulation time
    sim_time = msg->arrivalTime();

    return modp;
}

void cSimulation::transferTo(cSimpleModule *modp)
{
    if (modp==contextmodp)
        return;
    if (modp==NULL)
        throw new cException("transferTo(): attempt to transfer to NULL");

    // set context variables
    runningmodp = modp;
    setContextModule( modp );
    simulation.exception = NULL;

    // switch to activity() of the simple module
    cCoroutine::switchTo(modp->coroutine);

    // if exception occurred in activity(), take it from cSimpleModule::activate() and pass it up
    if (simulation.exception)
    {
        // alas, type info was lost, so we have to recover manually...
        if (simulation.exception_type==0)
            throw (cException *)simulation.exception;
        else
            throw (cTerminationException *)simulation.exception;
    }

    // check stack overflow, but only if this module still exists
    //   (note: currentmod_was_deleted is set by runningmod_deleter)
    if (currentmod_was_deleted)
        currentmod_was_deleted = false;
    else if (modp->stackOverflow())
        throw new cException("Stack violation in module `%s' (%s stack too small?)", modp->fullPath(), modp->className());

}

void cSimulation::doOneEvent(cSimpleModule *mod)
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
        // call handleMessage() in the module's context
        setContextModule( mod );
        cMessage *msg = msgQueue.getFirst();

        // notify the environment about the message
        ev.messageDelivered( msg );

        try
        {
            // if there was an error during simulation, handleMessage() will come back
            // with an exception
            mod->handleMessage( msg );
        }
        catch (cEndModuleException *e)
        {
            // ignore this exception
            delete e;
        }
        catch (cException *)
        {
            // temporarily catch the exception to restore global context
            setGlobalContext();
            throw;
        }
        setGlobalContext();
    }

    // increment event count
    event_num++;
}

void cSimulation::transferToMain()
{
    setGlobalContext();
    if (runningmodp!=NULL)
    {
        runningmodp = NULL;
        cCoroutine::switchToMain();     // stack switch
    }
}

void cSimulation::setContextModule(cModule *p)
{
    contextmodp = p;
    locallistp = p->isSimple() ? &(((cSimpleModule *)p)->locals) :  &locals;
}

cSimpleModule *cSimulation::contextSimpleModule() const
{
    // cannot go inline (upward cast would require including cmodule.h in csimul.h)
    if (!contextmodp || !contextmodp->isSimple())
        return NULL;
    return (cSimpleModule *)contextmodp;
}


