//=========================================================================
//
//  CSIMUL.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Text tables:
//    emsg:      error messages
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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>    // strcpy
#include <stdio.h>     // vsprintf()
#include <fstream.h>   // ostream
#include <iostream.h>  // ostream
#include "cmodule.h"
#include "cmessage.h"
#include "csimul.h"
#include "cenvir.h"
#include "ctypes.h"
#include "cpar.h"
#include "cnetmod.h"
#include "cstat.h"

//=== Global text tables:

char *modstate[] = {
  "Ended",    // sENDED, module has ended
  "Ready" ,   // sREADY, module is ready to run
  ""
};

//==========================================================================
//=== Global object:

cSimulation simulation( "simulation", &superhead );

//==========================================================================

// auxiliary flag, used only locally (a dirty solution...)
static bool currentmod_was_deleted = FALSE;

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

cSimulation::cSimulation(char *namestr, cHead *h) :
 cObject(namestr, h),
 locals("simulation-locals"),
 msgQueue( "message-queue" ),
 runningmod_deleter()
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
     tmlimit = 0;
     simtmlimit = 0;

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
         simulation.del( simulation.runningModule()->id() );
         currentmod_was_deleted = TRUE;  // checked & reset at cSimulation::doOneEvent()
         simulation.transferToMain();
     }
}

void cSimulation::setup()
{
     runningmod_deleter.setup( runningmod_deleter_func, NULL, 16384 );
}

void cSimulation::forEach( ForeachFunc do_fn )
{
     if (do_fn(this,TRUE))
     {
         if (systemmodp!=NULL)
             systemmodp->forEach( do_fn );
         msgQueue.forEach( do_fn );
     }
     do_fn(this,FALSE);
}

static bool _do_writesnapshot(cObject *obj, bool beg, ostream& s)
{
      static ostream *os;

      if (!obj) {os = &s;return FALSE;} //setup call

      if (os->fail()) return FALSE;   // there was an error, quit
      if (beg) obj->writeTo( *os );
      return !os->fail();      // check stream status
}

bool cSimulation::snapshot(cObject *object, char *label)
{
     if (!object)
         {opp_error("snapshot(): object pointer is NULL");return FALSE;}

     filebuf file;
     file.open( snapshotfilemgr.fileName(), ios::out|ios::app);
     ostream os( &file );

     bool w = warnings(); // temporarily disable warnings
     setWarnings( FALSE );

     os << "================================================" << "\n";
     os << "||               SNAPSHOT                     ||" << "\n";
     os << "================================================" << "\n";
     os << "| Of object:    `" << object->fullPath() << "'" << "\n";
     os << "| Label:        `" << label << "'" << "\n";
     os << "| Sim. time:     " << simtimeToStr(simTime()) << "\n";
     os << "| Network:      `" << (networktype->name() ?
                                  networktype->name():"unnamed") << "'\n";
     os << "| Run no.        " << run_number << '\n';
     os << "| Started at:    " << *localtime(&simbegtime) << '\n';
     os << "| Time:          " << *localtime(&simendtime) << '\n';
     os << "| Elapsed:       " << elapsedtime << " sec\n";
     if (err)
        os << "| Simulation stopped with error message.\n";
     if (contextModule())
        os << "| Initiated from: `" << contextModule()->fullPath() << "'\n";
     else
        os << "| Initiated by:  user\n";
     os << "================================================" << "\n\n";

     _do_writesnapshot( NULL,FALSE, os );         // setup
     object->forEach( (ForeachFunc)_do_writesnapshot );   // do

     setWarnings( w );
     file.close();

     if (os.fail()) opp_error("Can't write snapshot file");

     return !os.fail(); // success
}

static void write_cmod( ostream& os, cModule& m, int indent )
{
     static char sp[] = "                                                 ";
     os << (sp+sizeof(sp)-indent) << "`" << m.fullName() << "'";
     os << " #" << m.id() << " ";
     //os <<  (m.isSimple() ? "simple" : "compound");
     os <<  "(" << m.moduleType()->name() << ")\n";
}

static void writesubmodules(ostream& os, cModule *p, int indent )
{
     write_cmod( os, *p, indent );
     for (int i=1; i<=simulation.lastModuleIndex(); i++)
        if (&simulation[i] && p==simulation[i].parentModule())
            writesubmodules(os, &simulation[i], indent+4 );
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

int cSimulation::add(cModule *mod)
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
        delete vect;
        vect = v;
        size += delta;
     }

     vect[last_id] = mod;
     mod->setId(last_id);
     return last_id;
}

void cSimulation::del(int id)
{
     if (id<0 || id>last_id)
     {
         opp_error("cSimulation::del(): module id %d out of range",id);
         return;
     }

     delete vect[id];
     vect[id] = NULL;
}

int cSimulation::find(cModule *obj)
{
     if(obj==NULL) return -1;
     int i;
     for (i=1; i<size; i++) if (vect[i]==obj) break;
     if (i<size)
         return i;
     else
         return -1;
}

bool cSimulation::isUnique(char *s)
{
     for (int i=1, k=-1; i<size; i++)
        if (vect[i] && vect[i]->isName(s) && (!vect[i]->isVector() || vect[i]->index()==0))
           if (k==-1) k=i;
           else return FALSE;
     return TRUE;
}

int cSimulation::find(char *s, int n, int pt)
{
     if (pt==-1 && !isUnique(s))
        opp_warning("cSimulation::find(): Module name `%s' not unique, use moduleByPath()",s);

     for (int i=1; i<size; i++)
        if ( vect[i] && vect[i]->isName(s) &&
             ((n<0 && !vect[i]->isVector()) || vect[i]->index()==n) &&
             (pt==-1 || (vect[i]->parentModule()!=NULL && vect[i]->parentModule()->id()==pt)) )
           return i;
     return -1;
}

// Format of path p: "SysModule.DemandGen[2].Source"
cModule *cSimulation::moduleByPath(char *p)
{
     char buf[201];
     char *s, *b;
     strncpy(buf,p,200); buf[200]=0;
     s = strtok(buf,".");
     int m = systemModule()->id();
     if (!vect[m]->isName(s))
     {
        return NO(cModule);
     }
     while ((s=strtok(NULL,"."))!=NULL && m!=-1)
        if ((b=strchr(s,'['))==NULL)
           m = find(s,0,m);  // no index given
        else
           if (s[strlen(s)-1]!=']')
              m=-1; // missing ']' -- syntax error
           else {
              *b=0;
              m = find(s,atol(b+1),m);
           }
     if (m==-1)
     {
        return NO(cModule);
     }
     return vect[m];
}

cModule *cSimulation::module(char *s,int n,int pt)
{
     int i=find(s,n,pt);
     return i>=0 ? vect[i] : NO(cModule);
}

void cSimulation::resetClock()
{
     laststarted = simendtime = simbegtime = time(0);
     elapsedtime = 0LU;
}

void cSimulation::startClock()
{
     laststarted = time(0);
}

void cSimulation::stopClock()
{
     simendtime = time(0);
     elapsedtime +=  simendtime - laststarted;
     simulatedtime = simTime();
}

void cSimulation::checkTimes()
{
     if (simtmlimit!=0 && simTime()>=simtmlimit)
          opp_terminate(eSIMTIME);
     else if (tmlimit!=0 && elapsedtime+time(0)-laststarted>=tmlimit)
          opp_terminate(eREALTIME);
}

bool cSimulation::setupNetwork(cNetworkType *network, int run_num)
{
      // checks
      err = eOK;

      if (!network || !network->setupfunc)
      {
          // bad network
          opp_error(eSETUP);
          return FALSE;
      }

      // set run number
      run_number = run_num;

      // set cNetworkType pointer
      networktype = network;

      // call JAR-generated network setup function (with warnings turned off)
      bool w=warnings();setWarnings(FALSE);// temporarily turn off warnings
      networktype->setupfunc();
      setWarnings( w );
      if (!ok()) goto error;

      // handle distributed execution
      if (netInterface()!=NULL)
      {
         // master: starts OMNeT++ on other hosts
         if (ev.runningMode()==MASTER_MODE)
         {
            // the hosts we need are the system module's machines
            cArray& machines = systemModule()->machinev;
            netInterface()->start_segments( machines, ev.argc, ev.argv);
            if (!ok()) goto error;

            // signal the slaves which run to set up
            //   this causes slaves to start building up the network
            netInterface()->send_runnumber( run_number );
            if (!ok()) goto error;

            // process messages from other segments:
            //   display info/error messages, answer questions about param. values etc.
            netInterface()->process_netmsgs();
            if (!ok()) goto error;
         }

         // all segments: match gate pairs
         netInterface()->setup_connections();
         if (!ok()) goto error;

         // again: display possible info/error messages
         if (ev.runningMode()==MASTER_MODE)
         {
            netInterface()->process_netmsgs();
            if (!ok()) goto error;
         }
      }

      // network set up correctly
      return TRUE;

      // if failed, clean up the whole stuff
      error:
      deleteNetwork();
      return FALSE;
}

void cSimulation::startRun()
{
     // temporarily disable warnings
     int w = warnings(); setWarnings(FALSE);

     err = eOK;
     msgQueue.clear();
     resetClock();
     sim_time = 0;
     event_num = 0;
     backtomod = NULL;
     netif_check_cntr = 0;

     // initialize modules
     for (int i=1; i<size; i++)
     {
        if( vect[i] && vect[i]->isSimple() )
        {
            cSimpleModule *sm = (cSimpleModule *)vect[i];

            // create starter message for activity()
            //
            // (if the module uses handleMessage() and it needs some starter
            // message, initialize() should schedule it)
            //
            if (sm->usesActivity())
                sm->scheduleStart(0.0);

            // call user-defined init function in the module's own context
            //
            setContextModule( sm );
            sm->initialize();
        }
     }
     // restore global context
     setGlobalContext();

     // distributed execution:
     //  create a msg that will notify the network interface that all
     //  module activity()s have been started
     if (netInterface()!=NULL)
     {
         cMessage *msg = new cMessage;
         msg->sent = msg->delivd = 0;
         simulation.msgQueue.insert( msg );
         netInterface()->after_modinit_msg = msg;
     }

     // delete output files (except scalar output file)
     outvectfilemgr.deleteFile();    // output vector file manager
     parchangefilemgr.deleteFile();  // parameter change log file manager
     snapshotfilemgr.deleteFile();   // snapshot file manager

     scalarfile_header_written=FALSE;

     setWarnings(w);
}

void cSimulation::callFinish()
{
     // call user-defined finish() functions of simple modules
     for (int i=1; i<size; i++)
     {
        if (vect[i] && vect[i]->isSimple())
        {
            cSimpleModule *sm = (cSimpleModule *)vect[i];
            setContextModule( sm );
            sm->finish();         // user-defined function
        }
     }
     setGlobalContext();
}

void cSimulation::endRun()
{
     // close files
     outvectfilemgr.closeFile();    // output vector file manager
     scalarfilemgr.closeFile();     // output scalar file manager
     parchangefilemgr.closeFile();  // parameter change log file manager
     snapshotfilemgr.closeFile();   // snapshot file manager
}

void cSimulation::deleteNetwork()
{
     if (networktype==NULL)
     {
         return;  // network already deleted
     }

     if (runningmodp!=NULL)
     {
         opp_error("Attempt to delete network during simulation");
         return;
     }

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

     delete vect;
     vect = NULL;
     size = 0;
     last_id = 0;

     setGlobalContext();

     networktype = NULL;

     modtypes.deleteChildren();
     linktypes.deleteChildren();
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
             while (ok() && msgQueue.empty())
                 netInterface()->process_netmsg_blocking();
             if (ok())
                 return selectNextModule();
             return NULL;
        }

        // seems like really end of the simulation run
        opp_terminate(eENDEDOK);
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
     cSimpleModule *modp = (cSimpleModule *)vect[msg->tomod];
     if (modp==NULL)
     {
        opp_error("Message's destination module no longer exists");
        return NULL;
     }
     if (modp->moduleState()==sENDED)
     {
         if (msg->togate==-1)  // self-message is OK, ignore it
         {
             delete msgQueue.getFirst();
             return selectNextModule();
         }
         else
         {
             opp_error("Message's destination module `%s' already terminated",
                             modp->fullPath());
             return NULL;
         }
     }

     // advance simulation time
     sim_time = msg->arrivalTime();

     // check time limits
     checkTimes();
     return modp;
}

int cSimulation::transferTo(cSimpleModule *sm)
{
    if (sm==contextmodp)
        return 0;
    if (sm==NULL)
        {opp_error("transferTo(): attempt to transfer to NULL");return -1;}
    runningmodp = sm;
    setContextModule( sm );
    cCoroutine::switchTo( sm );     // stack switch
    return 0;
}

void cSimulation::doOneEvent(cSimpleModule *mod)
{
     if (mod->usesActivity())
     {
        transferTo( mod );

        // check stack overflow, but only if this module still exists
        //   (note: currentmod_was_deleted is set by runningmod_deleter)
        if (currentmod_was_deleted)
           currentmod_was_deleted = FALSE;
        else
           if (mod->stackOverflow())
              opp_error("Stack violation (%s stack too small?) in module `%s'",
                              mod->isA(),mod->fullPath());
     }
     else
     {
         // call handleMessage() in the module's context
         setContextModule( mod );
         cMessage *msg = msgQueue.getFirst();
         mod->handleMessage( msg );
         setGlobalContext();
     }
}

int cSimulation::transferToMain()
{
    setGlobalContext();
    if (runningmodp!=NULL)
    {
        runningmodp = NULL;
        cCoroutine::switchtoMain();     // stack switch
    }
    return 0;
}

void cSimulation::setContextModule(cModule *p)
{
    contextmodp = p;
    locallistp = p->isSimple() ? &(((cSimpleModule *)p)->locals) :  &locals;
}

cSimpleModule *cSimulation::contextSimpleModule()
{
    // cannot go inline (upward cast would require including cmodule.h in csimul.h)
    return (cSimpleModule *)contextmodp;
}

void cSimulation::terminate(int errc, char *message)
{
    // If we run distributed, stop the other segments unless we were stopped
    // by one of them.
    if (netInterface() && errc!=eSTOPSIMRCVD)
        netInterface()->stop_all_segments();

    if (contextModule()==NULL)
        ev.printfmsg("%s.\n", message);
    else
        ev.printfmsg("Module %s: %s.\n", contextModule()->fullPath(), message);
    err = errc;
    transferToMain();
}

void cSimulation::error(int errc, char *message)
{
    // If we run distributed, stop the other segments unless we were stopped
    // by one of them.
    if (netInterface() && errc!=eSTOPSIMRCVD)
        netInterface()->stop_all_segments();

    // Print error msg and set error code.
    if (contextModule()==NULL)
        ev.printfmsg("Error: %s.", message);
    else
        ev.printfmsg("Error in module %s: %s.",
                       contextModule()->fullPath(), message);
    err = errc;
    transferToMain();
}

void cSimulation::warning(int errc, char *message)
{
    if (!warnings() || (contextModule()!=NULL && !contextModule()->warnings()))
        return;      // warnings are disabled

    if (contextModule()==NULL)
        ev.printfmsg( "%s.", message);
    else if (contextModule()!=NULL && runningModule()==NULL)
        ev.printfmsg( "Module %s: %s.", contextModule()->fullPath(), message);
    else
    {
        if(ev.askYesNo( "Module %s: %s. Continue?",
                          contextModule()->fullPath(), message) == FALSE)
        {
            err = errc;

            // If we run distributed, stop the other segments.
            if (netInterface())
                netInterface()->stop_all_segments();

            transferToMain();
        }
    }
}


void cSimulation::recordScalar(char *name, double value)
{
    FILE *f = simulation.scalarfilemgr.filePointer();
    if (f==NULL) return;

    if (!scalarfile_header_written)
    {
        scalarfile_header_written = TRUE;
        fprintf(f,"run %d \"%s\"\n", run_number, networktype->name());
    }

    fprintf(f,"scalar \"%s\" \t\"%s\" \t%lg\n",
              contextSimpleModule()->fullPath(), name, value);
}

void cSimulation::recordScalar(char *name, char *text)
{
    FILE *f = simulation.scalarfilemgr.filePointer();
    if (f==NULL) return;

    if (!scalarfile_header_written)
    {
        scalarfile_header_written = TRUE;
        fprintf(f,"run %d \"%s\"\n", run_number, networktype->name());
    }

    fprintf(f,"scalar \"%s\" \t\"%s\" \t\"%s\"\n",
              contextSimpleModule()->fullPath(), name, text);
}

void cSimulation::recordStats(char *name, cStatistic *stats)
{
    FILE *f = simulation.scalarfilemgr.filePointer();
    if (f==NULL) return;

    if (!scalarfile_header_written)
    {
        scalarfile_header_written = TRUE;
        fprintf(f,"run %d \"%s\"\n", run_number, networktype->name());
    }

    fprintf(f,"statistics \"%s\" \t\"%s\" \t\"%s\"\n",
              contextSimpleModule()->fullPath(), name, stats->isA());
    stats->saveToFile( f );
}

bool cSimulation::normalTermination()
{
    return err==eSTOPSIMRCVD // stopped by another PVM segment
        || err==eENDEDOK     // no more events
        || err==eFINISH      // 'finish simulation' was requested interactively
        || err==eENDSIM      // endSimulation() called
        || err==eREALTIME    // execution time limit
        || err==eSIMTIME;    // sim. time limit
}

