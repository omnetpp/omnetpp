//==========================================================================
//  CMDENV.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Implementation of the following classes:
//    TCmdenvApp
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

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
#include "csimplemodule.h"
#include "cmessage.h"
#include "args.h"
#include "speedmtr.h"
#include "platdep/time.h"


static char buffer[1024];

//
// Register the Cmdenv user interface
//
Register_OmnetApp("Cmdenv",TCmdenvApp,10,"command-line user interface");

// some functions that can/should be called from Envir in order to force the
// linker to include the Cmdenv library into the executable:
CMDENV_API void cmdenvDummy() {}
CMDENV_API void envirDummy() {}

// a function defined in heap.cc
bool cmdenvMemoryIsLow();

bool TCmdenvApp::sigint_received;


// utility function for printing elapsed time
char *timeToStr(timeval t, char *buf=NULL)
{
   static char buf2[64];
   char *b = buf ? buf : buf2;

   if (t.tv_sec<3600)
       sprintf(b,"%ld.%.3ds (%dm %02ds)", (long)t.tv_sec, (int)(t.tv_usec/1000), int(t.tv_sec/60L), int(t.tv_sec%60L));
   else if (t.tv_sec<86400)
       sprintf(b,"%ld.%.3ds (%dh %02dm)", (long)t.tv_sec, (int)(t.tv_usec/1000), int(t.tv_sec/3600L), int((t.tv_sec%3600L)/60L));
   else
       sprintf(b,"%ld.%.3ds (%dd %02dh)", (long)t.tv_sec, (int)(t.tv_usec/1000), int(t.tv_sec/86400L), int((t.tv_sec%86400L)/3600L));

   return b;
}

//==========================================================================
// TCmdenvApp: command line user interface.

TCmdenvApp::TCmdenvApp(ArgList *args, cConfiguration *config) : TOmnetApp(args, config)
{
    // initialize fout to stdout, then we'll replace it if redirection is
    // requested in the ini file
    fout = stdout;
}

TCmdenvApp::~TCmdenvApp()
{
}

void TCmdenvApp::readOptions()
{
    TOmnetApp::readOptions();

    cConfiguration *cfg = getConfig();

    opt_runstoexec = cfg->getAsString("Cmdenv", "runs-to-execute", "");
    opt_extrastack_kb = cfg->getAsInt("Cmdenv", "extra-stack-kb", CMDENV_EXTRASTACK_KB);
    opt_outputfile = cfg->getAsFilename("Cmdenv", "output-file", "").c_str();

    if (!opt_outputfile.empty())
    {
        processFileName(opt_outputfile);
        ::printf("Cmdenv: redirecting output to file `%s'...\n",opt_outputfile.c_str());
        FILE *out = fopen(opt_outputfile.c_str(), "w");
        if (!out)
            throw new cRuntimeError("Cannot open output file `%s'",opt_outputfile.c_str());
        fout = out;
    }
}

void TCmdenvApp::readPerRunOptions(int run_nr)
{
    TOmnetApp::readPerRunOptions(run_nr);

    cConfiguration *cfg = getConfig();
    const char *section = getRunSectionName(run_nr);

    opt_expressmode = cfg->getAsBool2(section,"Cmdenv", "express-mode", false);
    opt_autoflush = cfg->getAsBool2(section,"Cmdenv", "autoflush", false);
    opt_modulemsgs = cfg->getAsBool2(section,"Cmdenv", "module-messages", true);
    opt_eventbanners = cfg->getAsBool2(section,"Cmdenv", "event-banners", true);
    opt_eventbanner_details = cfg->getAsBool2(section,"Cmdenv", "event-banner-details", false);
    opt_messagetrace = cfg->getAsBool2(section,"Cmdenv", "message-trace", false);
    opt_status_frequency_ev = cfg->getAsInt2(section,"Cmdenv", "status-frequency", 100000);
    //opt_status_frequency_sec = cfg->getAsTime2(section,"Cmdenv", "status-frequency-interval", 5.0);
    opt_perfdisplay = cfg->getAsBool2(section,"Cmdenv", "performance-display", true);

    // warnings on old entries
    if (cfg->exists2(section,"Cmdenv", "display-update"))
         ev.printfmsg("Warning: ini file entry display-update= is obsolete, "
                      "use status-frequency= instead!");
    if (cfg->exists2(section,"Cmdenv", "verbose-simulation"))
         ev.printfmsg("Warning: ini file entry verbose-simulation= is obsolete, use event-banners= instead!");
}

void TCmdenvApp::setup()
{
    // initialize base class
    TOmnetApp::setup();    // implies readOptions()
    if (!initialized)
        return;

    // '-r' option: specifies runs to execute; overrides ini file setting
    char *r = args->argValue( 'r' );
    if (r)
       opt_runstoexec = r;

    // If the list of runs is not given explicitly, must execute all runs
    // that have an ini file section; if no runs have ini file sections,
    // must do one run.
    if (opt_runstoexec.empty())
    {
       buffer[0]='\0';
       cConfiguration *cfg = getConfig();
       for (int i=0; i<cfg->getNumSections(); i++)
       {
          if (strncmp(cfg->getSectionName(i),"Run ",4)==0)
          {
              strcat(buffer,cfg->getSectionName(i)+4); // FIXME danger of buffer overrun here
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
    if (signum == SIGINT || signum == SIGTERM)
        sigint_received = true;
}

void TCmdenvApp::setupSignals()
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

void TCmdenvApp::shutdown()
{
    if (!initialized)
        return;

    TOmnetApp::shutdown();
    ::fflush(fout);
}

int TCmdenvApp::run()
{
    if (!initialized)
        return 1;

    setupSignals();

    EnumStringIterator run_nr( opt_runstoexec.c_str() );
    if (run_nr.error())
    {
        ev.printfmsg("Error parsing list of runs to execute: `%s'",
                     opt_runstoexec.c_str() );
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
            ::fprintf(fout, "\nPreparing for Run #%d...\n", run_nr());
            ::fflush(fout);

            readPerRunOptions(run_nr());

            // find network
            cNetworkType *network = findNetwork(opt_network_name.c_str());
            if (!network)
                throw new cRuntimeError("Network `%s' not found, check .ini and .ned files", opt_network_name.c_str());

            // set up network
            ::fprintf(fout, "Setting up network `%s'...\n", opt_network_name.c_str());
            ::fflush(fout);

            simulation.setupNetwork(network, run_nr());
            setupnetwork_done = true;

            // prepare for simulation run
            ::fprintf(fout, "Initializing...\n");
            ::fflush(fout);

            startRun();
            startrun_done = true;

            // run the simulation
            ::fprintf(fout, "\nRunning simulation...\n");
            ::fflush(fout);

            // simulate() should only throw exception if error occurred and
            // finish() should not be called.
            simulate();

            ::fprintf(fout, "\nCalling finish() at end of Run #%d...\n", run_nr());
            ::fflush(fout);
            simulation.callFinish();
            ev.flushlastline();
        }
        catch (cException *e)
        {
            had_error = true;
            stoppedWithException(e);
            displayError(e);
            delete e;
        }

        // call endRun()
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
               if (!mod)
                   throw new cTerminationException("scheduler interrupted while waiting");

               // print event banner if neccessary
               if (opt_eventbanners && mod->isEvEnabled())
               {
                   ::fprintf(fout, "** Event #%ld  T=%s.  (%s) %s (id=%d)\n",
                           simulation.eventNumber(),
                           simtimeToStr( simulation.simTime() ),
                           mod->className(),
                           mod->fullPath().c_str(),
                           mod->id()
                         );
                   if (opt_eventbanner_details)
                   {
                       ::fprintf(fout, "   Elapsed: %s   Messages: created: %ld  present: %ld  in FES: %d\n",
                               timeToStr(totalElapsed()),
                               cMessage::totalMessageCount(),
                               cMessage::liveMessageCount(),
                               simulation.msgQueue.length());
                   }
               }

               // flush *between* printing event banner and event processing, so that
               // if event processing crashes, it can be seen which event it was
               if (opt_autoflush)
                   ::fflush(fout);

               // execute event
               simulation.doOneEvent( mod );

               // flush so that output from different modules don't get mixed
               ev.flushlastline();

               checkTimeLimits();
               if (sigint_received)
                   throw new cTerminationException("SIGINT or SIGTERM received, exiting");
           }
        }
        else
        {
           ev.disable_tracing = true;
           Speedometer speedometer;
           speedometer.start(simulation.simTime());
           while (true)
           {
               cSimpleModule *mod = simulation.selectNextModule();
               if (!mod)
                   throw new cTerminationException("scheduler interrupted while waiting");

               speedometer.addEvent(simulation.simTime());

               // print event banner from time to time
               // ... if (simulation.eventNumber() >= last_update_ev + opt_status_frequency_ev && ...
               if (simulation.eventNumber()%opt_status_frequency_ev==0)
               {
                   speedometer.beginNewInterval();

                   if (opt_perfdisplay)
                   {
                       ::fprintf(fout, "** Event #%ld   T=%s    Elapsed: %s\n",
                               simulation.eventNumber(),
                               simtimeToStr(simulation.simTime()),
                               timeToStr(totalElapsed()));
                       ::fprintf(fout, "     Speed:     ev/sec=%g   simsec/sec=%g   ev/simsec=%g\n",
                               speedometer.eventsPerSec(),
                               speedometer.simSecPerSec(),
                               speedometer.eventsPerSimSec());

                       ::fprintf(fout, "     Messages:  created: %ld   present: %ld   in FES: %d\n",
                               cMessage::totalMessageCount(),
                               cMessage::liveMessageCount(),
                               simulation.msgQueue.length());
                   }
                   else
                   {
                       ::fprintf(fout, "** Event #%ld   T=%s   Elapsed: %s   ev/sec=%g\n",
                               simulation.eventNumber(),
                               simtimeToStr(simulation.simTime()),
                               timeToStr(totalElapsed()),
                               speedometer.eventsPerSec());
                   }

                   if (opt_autoflush)
                       ::fflush(fout);
               }

               // execute event
               simulation.doOneEvent( mod );

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
        stoppedWithTerminationException(e);
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

//-----------------------------------------------------

void TCmdenvApp::putmsg(const char *s)
{
    ::fprintf(fout, "\n<!> %s\n\n", s);
    ::fflush(fout);
}

void TCmdenvApp::sputn(const char *s, int n)
{
    cModule *ctxmod = simulation.contextModule();
    if (!ctxmod || (opt_modulemsgs && ctxmod->isEvEnabled()) || simulation.contextType()==CTX_FINISH)
    {
        ::fwrite(s,1,n,fout);
        if (opt_autoflush)
            ::fflush(fout);
    }
}

void TCmdenvApp::flush()
{
    ::fflush(fout);
}

bool TCmdenvApp::gets(const char *promptstr, char *buf, int len)
{
    ::fprintf(fout, "%s", promptstr);
    if (buf[0]) ::fprintf(fout, "(default: %s) ",buf);

    ::fgets(buffer,512,stdin);
    buffer[strlen(buffer)-1] = '\0'; // chop LF

    if( buffer[0]=='\x1b' ) // ESC?
       return true;
    else
    {
       if( buffer[0] )
          strncpy(buf, buffer, len);
       return false;
    }
}

int TCmdenvApp::askYesNo(const char *question )
{
    // should also return -1 (==CANCEL)
    for(;;)
    {
        ::fprintf(fout, "%s (y/n) ", question);
        ::fgets(buffer,512,stdin);
        buffer[strlen(buffer)-1] = '\0'; // chop LF
        if (toupper(buffer[0])=='Y' && !buffer[1])
           return 1;
        else if (toupper(buffer[0])=='N' && !buffer[1])
           return 0;
        else
           putmsg("Please type 'y' or 'n'!\n");
    }
}

bool TCmdenvApp::idle()
{
    return sigint_received;
}

void TCmdenvApp::moduleCreated(cModule *mod)
{
    std::string entryname = mod->fullPath() + ".ev-output";
    bool ev_enabled = ev.config()->getAsBool("Cmdenv", entryname.c_str(), true);
    mod->setEvEnabled(ev_enabled);
}

void TCmdenvApp::messageSent(cMessage *msg, cGate *)
{
    if (!opt_expressmode && opt_messagetrace)
    {
        ::fprintf(fout, "SENT:   %s\n", msg->info().c_str());
        if (opt_autoflush)
            ::fflush(fout);
    }
}

void TCmdenvApp::messageDelivered(cMessage *msg)
{
    if (!opt_expressmode && opt_messagetrace)
    {
        ::fprintf(fout, "DELIVD: %s\n", msg->info().c_str());
        if (opt_autoflush)
            ::fflush(fout);
    }
}

void TCmdenvApp::printUISpecificHelp()
{
    ev << "Cmdenv-specific options:\n";
    ev << "  -r <runs>     Execute the specified runs in the ini file.\n";
    ev << "                <runs> is a comma-separated list of run numbers or\n";
    ev << "                run number ranges, for example 1,2,5-10.\n" ;
    ev << "\n";
}

unsigned TCmdenvApp::extraStackForEnvir()
{
    return 1024*opt_extrastack_kb;
}

bool TCmdenvApp::memoryIsLow()
{
    return cmdenvMemoryIsLow();
}


