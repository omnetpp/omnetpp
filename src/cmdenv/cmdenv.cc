//==========================================================================
//   CMDENV.CC - part of
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Implementation of the following classes:
//    TCmdenvApp
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmddefs.h"
#include "cmdenv.h"
#include "enumstr.h"
#include "appreg.h"
#include "cinifile.h"
#include "cmodule.h"
#include "cmessage.h"
#include "cnetmod.h"
#include "args.h"
#include "speedmtr.h"


static char buffer[1024];

//
// Register the Cmdenv user interface
//
Register_OmnetApp("Cmdenv",TCmdenvApp,false,10,"Cmdenv (command-line user interface)");

// some functions that can/should be called from Envir in order to force the
// linker to include the Tkenv library into the executable:
CMDENV_API void cmdenvDummy() {}
CMDENV_API void envirDummy() {}

// a function defined in heap.cc
bool cmdenvMemoryIsLow();

bool TCmdenvApp::sigint_received;


// utility function for printing elapsed time
char *timeToStr(time_t t, char *buf=NULL)
{
   static char buf2[48];
   char *b = buf ? buf : buf2;

   if (t<3600)
       sprintf(b,"%dm %2ds", int(t/60L), int(t%60L));
   else if (t<86400)
       sprintf(b,"%dh %2dm", int(t/3600L), int((t%3600L)/60L));
   else
       sprintf(b,"%dd %2dh)", int(t/86400L), int((t%86400L)/3600L));

   return b;
}

//==========================================================================
// TCmdenvApp: command line user interface.

TCmdenvApp::TCmdenvApp(ArgList *args, cIniFile *inifile) : TOmnetApp(args, inifile)
{
}

TCmdenvApp::~TCmdenvApp()
{
}

void TCmdenvApp::readOptions()
{
    TOmnetApp::readOptions();

    opt_runstoexec = ini_file->getAsString("Cmdenv", "runs-to-execute", "");
    opt_extrastack = ini_file->getAsInt("Cmdenv", "extra-stack", CMDENV_EXTRASTACK);
}

void TCmdenvApp::readPerRunOptions( int run_nr )
{
    TOmnetApp::readPerRunOptions( run_nr);

    char section[16];
    sprintf(section,"Run %d",run_nr);
    ini_file->error(); // clear error flag

    opt_expressmode = ini_file->getAsBool2( section,"Cmdenv", "express-mode", false);
    opt_autoflush = ini_file->getAsBool2( section,"Cmdenv", "autoflush", false);
    opt_modulemsgs = ini_file->getAsBool2( section,"Cmdenv", "module-messages", true );
    opt_eventbanners = ini_file->getAsBool2( section,"Cmdenv", "event-banners", true );
    opt_messagetrace = ini_file->getAsBool2( section,"Cmdenv", "message-trace", false );
    opt_status_frequency_ev = ini_file->getAsInt2( section,"Cmdenv", "status-frequency", 100000 );
    //opt_status_frequency_sec = ini_file->getAsTime2( section,"Cmdenv", "status-frequency-interval", 5.0 );
    opt_perfdisplay = ini_file->getAsBool2( section,"Cmdenv", "performance-display", false );

    // warnings on old entries
    if (ini_file->exists2( section,"Cmdenv", "display-update"))
         ev.printfmsg("Warning: ini file entry display-update= is obsolete, "
                      "use status-frequency= instead!");
    if (ini_file->exists2( section,"Cmdenv", "verbose-simulation"))
         ev.printfmsg("Warning: ini file entry verbose-simulation= is obsolete, use event-banners= instead!");
}

void TCmdenvApp::setup()
{
    // initialize base class
    TOmnetApp::setup();    // implies readOptions()
    if (!initialized)
        return;

    // '-h' (help) flag
    opt_helponly = args->argGiven( 'h' );
    if (opt_helponly) return;  // only give a command line help

    // '-r' option: specifies runs to execute; overrides ini file setting
    char *r = args->argValue( 'r' );
    if (r)
       opt_runstoexec = r;

    // If the list of runs is not given explicitly, must execute all runs
    // that have an ini file section; if no runs have ini file sections,
    // must do one run.
    if ((const char *)opt_runstoexec==NULL)
    {
       buffer[0]='\0';
       for (cIniFileSectionIterator sect(ini_file); !sect.end(); sect++)
       {
          if (strncmp(sect.section(),"Run ",4)==0)
          {
              strcat(buffer,sect.section()+4);
              strcat(buffer,",");
          }
       }
       if (buffer[0]=='\0')
          strcpy(buffer,"1");
       opt_runstoexec = buffer;
    }
}

void TCmdenvApp::signalHandler(int signum)
{
    // Only for Unix. WIN32 doesn't support either SIGINT or SIGTERM :(
    if (signum == SIGINT || signum == SIGTERM)
        sigint_received = true;
}

void TCmdenvApp::setupSignals()
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

int TCmdenvApp::run()
{
    if (!initialized)
        return 1;

    setupSignals();

    if (opt_helponly)
    {
        help();
        return 0;
    }

    EnumStringIterator run_nr( (const char *)opt_runstoexec );
    if (run_nr.error())
    {
        ev.printfmsg("Error parsing list of runs to execute: `%s'",
                     (const char *)opt_runstoexec );
        return 1;
    }

    // we'll return nonzero exitcode if any run was terminated with error
    bool had_error = false;

    for (; run_nr()!=-1; run_nr++)
    {
        bool setupnetwork_done = false;
        bool startrun_done = false;
        try
        {
            printf("\nPreparing for Run #%d...\n", run_nr());
            fflush(stdout);

            readPerRunOptions(run_nr());

            // find network
            cNetworkType *network = findNetwork(opt_network_name);
            if (!network)
                throw new cException("Network `%s' not found, check .ini and .ned files", (const char *)opt_network_name);

            // set up network
            printf("Setting up network `%s'...\n", (const char *)opt_network_name);
            fflush(stdout);

            simulation.setupNetwork(network, run_nr());
            setupnetwork_done = true;

            makeOptionsEffective();

            // run the simulation
            printf("Running simulation...\n");
            fflush(stdout);

            // start execution on other segments too
            startRun();
            startrun_done = true;

            // simulate() should only throw exception if error occurred and
            // finish() should not be called.
            simulate();

            printf("\nCalling finish() at end of Run #%d...\n", run_nr());
            fflush(stdout);
            simulation.callFinish();
        }
        catch (cException *e)
        {
            had_error = true;
            displayError(e);
            delete e;
        }

        // stop run on other segments
        if (startrun_done)
        {
            try
            {
                endRun();
            }
            catch (cException *e)
            {
                had_error = true;
                displayError(e);
                delete e;
            }
        }

        // delete network (this also forces slaves to exit)
        if (setupnetwork_done)
        {
            try
            {
                simulation.deleteNetwork();
            }
            catch (cException *e)
            {
                had_error = true;
                displayError(e);
                delete e;
            }
        }

        // skip further runs if signal was caught
        if (sigint_received)
            break;
    }

    if (had_error)
        return 1;
    else if (sigint_received)
        return 2;
    else
        return 0;
}

void TCmdenvApp::makeOptionsEffective()
{
    TOmnetApp::makeOptionsEffective();
    // further stuff may come here
}

void TCmdenvApp::simulate()
{
    startClock();
    sigint_received = false;
    try
    {
        if (!opt_expressmode)
        {
           ev.disable_tracing = false;
           while (true)
           {
               cSimpleModule *mod = simulation.selectNextModule();
               ASSERT(mod!=NULL);

               // print event banner if neccessary
               if (opt_eventbanners)
               {
                   printf( "** Event #%ld  T=%s.  (%s) %s (id=%d)\n",
                           simulation.eventNumber(),
                           simtimeToStr( simulation.simTime() ),
                           mod->className(),
                           mod->fullPath(),
                           mod->id()
                         );
               }

               // flush *between* printing event banner and event processing, so that
               // if event processing crashes, it can be seen which event it was
               if (opt_autoflush)
                   fflush(stdout);

               // execute event
               simulation.doOneEvent( mod );

               checkTimeLimits();
               if (sigint_received)
                   throw new cTerminationException("SIGINT or SIGTERM received, exiting");
           }
        }
        else
        {
           ev.disable_tracing = true;
           Speedometer speedometer;
           while (true)
           {
               cSimpleModule *mod = simulation.selectNextModule();
               ASSERT(mod!=NULL);

               // print event banner from time to time
               // ... if (simulation.eventNumber() >= last_update_ev + opt_status_frequency_ev && ...
               if (simulation.eventNumber()%opt_status_frequency_ev==0)
               {
                   if (opt_perfdisplay)
                   {
                       printf( "** Event #%ld   T=%s    Elapsed: %s\n",
                               simulation.eventNumber(),
                               simtimeToStr(simulation.simTime()),
                               timeToStr(totalElapsed()));
                       printf( "     Speed:     ev/sec=%g   simsec/sec=%g   ev/simsec=%g\n",
                               speedometer.eventsPerSec(),
                               speedometer.simSecPerSec(),
                               speedometer.eventsPerSimSec());

                       printf( "     Messages:  created: %ld   present: %ld   in FES: %d\n",
                               cMessage::totalMessageCount(),
                               cMessage::liveMessageCount(),
                               simulation.msgQueue.length());
                   }
                   else
                   {
                       printf( "** Event #%ld   T=%s   Elapsed: %s   ev/sec=%g\n",
                               simulation.eventNumber(),
                               simtimeToStr(simulation.simTime()),
                               timeToStr(totalElapsed()),
                               speedometer.eventsPerSec());
                   }
                   speedometer.beginNewInterval();

                   if (opt_autoflush)
                       fflush(stdout);
               }

               // execute event
               simulation.doOneEvent( mod );

               speedometer.addEvent(simulation.simTime());
               checkTimeLimits();
               if (sigint_received)
                   throw new cTerminationException("SIGINT or SIGTERM received, exiting");
           }
        }
    }
    catch (cTerminationException *e)
    {
        ev.disable_tracing = false;
        stopClock();

        displayMessage(e);
        delete e;
        return;
    }
    catch (cException *e)
    {
        ev.disable_tracing = false;
        stopClock();
        throw;
    }
    ev.disable_tracing = false;
    stopClock();
}

void TCmdenvApp::messageSent(cMessage *msg)
{
    if (!opt_expressmode && opt_messagetrace)
    {
        static char buf[1000];
        msg->info( buf );
        printf("SENT:   %s\n", buf);

        if (opt_autoflush)
            fflush(stdout);
    }
}

void TCmdenvApp::messageDelivered(cMessage *msg)
{
    if (!opt_expressmode && opt_messagetrace)
    {
        static char buf[1000];
        msg->info( buf );
        printf("DELIVD: %s\n", buf);

        if (opt_autoflush)
            fflush(stdout);
    }
}

void TCmdenvApp::help()
{
    ev << "\n";
    ev << "Command line switches:\n";
    ev << "  -h               this help\n";
    ev << "  -fsomething.ini  specifies ini file to use (instead of omnetpp.ini)\n";
    ev << "  -r1,2,5-10       specifies runs to be executed\n";
    ev << "\n";

    ev << "Available networks:\n";
    cIterator iter(networks);
    for (; iter(); iter++)
        ev << "  " << ((cNetworkType *)iter())->name() << '\n';
    ev << "\n";

    ev << "Available modules:\n";
    cIterator iter2(modtypes);
    for (; iter2(); iter2++)
        ev << "  " << ((cModuleType *)iter2())->name() << '\n';
    ev << "\n";

    ev << "Available channels:\n";
    cIterator iter3(linktypes);
    for (; iter3(); iter3++)
        ev << "  " << ((cLinkType *)iter3())->name() << '\n';
    ev << "\n";
}

void TCmdenvApp::puts(const char *s)
{
    if (opt_modulemsgs || simulation.contextModule()==NULL)
        TOmnetApp::puts( s );
}

unsigned TCmdenvApp::extraStackForEnvir()
{
    return opt_extrastack;
}

bool TCmdenvApp::memoryIsLow()
{
    return cmdenvMemoryIsLow();
}


