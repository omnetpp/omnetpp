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
#include "ccomponenttype.h"
#include "cmessage.h"
#include "args.h"
#include "speedmtr.h"
#include "timeutil.h"
#include "cconfigentry.h"

#define CMDENV_EXTRASTACK_KB  "8"

Register_GlobalConfigEntry(CFGID_RUNS_TO_EXECUTE, "cmdenv-runs-to-execute", CFG_STRING, NULL, "Specifies which simulation runs should be executed. It accepts a comma-separated list of run numbers or run number ranges, e.g. 1,3-4,7-9. If the value is missing, Cmdenv executes all runs that have ini file sections; if no runs are specified in the ini file, Cmdenv does one run. The -r command line option overrides this setting.")
Register_GlobalConfigEntry(CFGID_CMDENV_EXTRA_STACK_KB, "cmdenv-extra-stack-kb", CFG_INT,  CMDENV_EXTRASTACK_KB, "Specifies the extra amount of stack (in kilobytes) that is reserved for each activity() simple module when the simulation is run under Cmdenv.")
Register_GlobalConfigEntry(CFGID_OUTPUT_FILE, "cmdenv-output-file", CFG_FILENAME, NULL, "When a filename is specified, Cmdenv redirects standard output into the given file. This is especially useful with parallel simulation. See `fname-append-host' option as well.")
Register_PerRunConfigEntry(CFGID_EXPRESS_MODE, "cmdenv-express-mode", CFG_BOOL, "true", "Selects ``normal'' (debug/trace) or ``express'' mode.")
Register_PerRunConfigEntry(CFGID_AUTOFLUSH, "cmdenv-autoflush", CFG_BOOL, "false", "Call fflush(stdout) after each event banner or status update; affects both express and normal mode. Turning on autoflush can be useful with printf-style debugging for tracking down program crashes.")
Register_PerRunConfigEntry(CFGID_MODULE_MESSAGES, "cmdenv-module-messages", CFG_BOOL, "true", "When express-mode=false: turns printing module ev<< output on/off.")
Register_PerRunConfigEntry(CFGID_EVENT_BANNERS, "cmdenv-event-banners", CFG_BOOL, "true", "When express-mode=false: turns printing event banners on/off.")
Register_PerRunConfigEntry(CFGID_EVENT_BANNER_DETAILS, "cmdenv-event-banner-details", CFG_BOOL, "false", "When express-mode=false: print extra information after event banners.")
Register_PerRunConfigEntry(CFGID_MESSAGE_TRACE, "cmdenv-message-trace", CFG_BOOL, "false", "When express-mode=false: print a line per message sending (by send(),scheduleAt(), etc) and delivery on the standard output.")
Register_PerRunConfigEntry(CFGID_STATUS_FREQUENCY, "cmdenv-status-frequency", CFG_INT, "100000", "When express-mode=true: print status update every n events. Typical values are 100,000...1,000,000.")
Register_PerRunConfigEntry(CFGID_PERFORMANCE_DISPLAY, "cmdenv-performance-display", CFG_BOOL, "true", "When express-mode=true: print detailed performance information. Turning it on results in a 3-line entry printed on each update, containing ev/sec, simsec/sec, ev/simsec, number of messages created/still present/currently scheduled in FES.")

Register_PerObjectConfigEntry(CFGID_CMDENV_EV_OUTPUT, "cmdenv-ev-output", CFG_BOOL, "true", "FIXME XXX write docu")


//
// Register the Cmdenv user interface
//
Register_OmnetApp("Cmdenv", TCmdenvApp, 10, "command-line user interface");

//
// The following function can be used to force linking with Cmdenv; specify
// -u _cmdenv_lib (gcc) or /include:_cmdenv_lib (vc++) in the link command.
//
extern "C" CMDENV_API void cmdenv_lib() {}

static char buffer[1024];

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

    opt_runstoexec = cfg->getAsString(CFGID_RUNS_TO_EXECUTE);
    opt_extrastack_kb = cfg->getAsInt(CFGID_CMDENV_EXTRA_STACK_KB);
    opt_outputfile = cfg->getAsFilename(CFGID_OUTPUT_FILE).c_str();

    if (!opt_outputfile.empty())
    {
        processFileName(opt_outputfile);
        ::printf("Cmdenv: redirecting output to file `%s'...\n",opt_outputfile.c_str());
        FILE *out = fopen(opt_outputfile.c_str(), "w");
        if (!out)
            throw cRuntimeError("Cannot open output redirection file `%s'",opt_outputfile.c_str());
        fout = out;
    }
}

void TCmdenvApp::readPerRunOptions()
{
    TOmnetApp::readPerRunOptions();

    cConfiguration *cfg = getConfig();
    opt_expressmode = cfg->getAsBool(CFGID_EXPRESS_MODE);
    opt_autoflush = cfg->getAsBool(CFGID_AUTOFLUSH);
    opt_modulemsgs = cfg->getAsBool(CFGID_MODULE_MESSAGES);
    opt_eventbanners = cfg->getAsBool(CFGID_EVENT_BANNERS);
    opt_eventbanner_details = cfg->getAsBool(CFGID_EVENT_BANNER_DETAILS);
    opt_messagetrace = cfg->getAsBool(CFGID_MESSAGE_TRACE);
    opt_status_frequency_ev = cfg->getAsInt(CFGID_STATUS_FREQUENCY);
    opt_perfdisplay = cfg->getAsBool(CFGID_PERFORMANCE_DISPLAY);
}

void TCmdenvApp::setup()
{
    // initialize base class
    TOmnetApp::setup();    // implies readOptions()
    if (!initialized)
        return;

    // '-n' option: print number of runs in the given scenario, and exit
    opt_printnumruns = args->optionValue('n');

    // '-c' option: specifies configuration or scenario to activate
    opt_configname = args->optionValue('c');
    if (opt_configname.empty())
        opt_configname = "General";

    // '-r' option: specifies runs to execute; overrides ini file setting
    opt_runstoexec = args->optionValue('r');
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
    //FIXME: check if wrong configname! check if runnumber out of range!
    if (!initialized)
        return 1;

    setupSignals();

    cConfiguration *cfg = getConfig();

    if (!opt_printnumruns.empty())
    {
        ev.printf("Scenario: %s\n", opt_printnumruns.c_str());
        ev.printf("Number of runs: %d\n", cfg->getNumRunsInScenario(opt_printnumruns.c_str()));
        ::printf("\n%s\n", cfg->unrollScenario(opt_printnumruns.c_str()).c_str()); //FIXME only for debugging -- remove it
        return 0;
    }

    // if the list of runs is not given explicitly, must execute all runs
    if (opt_runstoexec.empty())
    {
        int n = cfg->getNumRunsInScenario(opt_configname.c_str());  //XXX may throw exception
        char buf[32];
        sprintf(buf, (n==0 ? "" : n==1 ? "%d" : "0-%d"), n-1);
        opt_runstoexec = buf;
    }

    EnumStringIterator runiterator(opt_runstoexec.c_str());
    if (runiterator.error())
    {
        ev.printfmsg("Error parsing list of runs to execute: `%s'", opt_runstoexec.c_str());
        return 1;
    }

    // we'll return nonzero exitcode if any run was terminated with error
    bool had_error = false;

    for (; runiterator()!=-1; runiterator++)
    {
        int runnumber = runiterator();
        bool setupnetwork_done = false;
        bool startrun_done = false;
        try
        {
            ::fprintf(fout, "\nPreparing for Run #%d...\n", runnumber);
            ::fflush(fout);

            cfg->activateConfig(opt_configname.c_str(), runnumber);
            cfg->dump(); //XXX remove

            readPerRunOptions();

            // find network
            cModuleType *network = cModuleType::find(opt_network_name.c_str());
            if (!network)
                throw cRuntimeError("Network `%s' not found, check .ini and .ned files", opt_network_name.c_str());

            // set up network
            ::fprintf(fout, "Setting up network `%s'...\n", opt_network_name.c_str());
            ::fflush(fout);

            simulation.setupNetwork(network);
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

            ::fprintf(fout, "\nCalling finish() at end of Run #%d...\n", runnumber);
            ::fflush(fout);
            simulation.callFinish();
            ev.flushlastline();

            checkFingerprint();
        }
        catch (std::exception& e)
        {
            had_error = true;
            stoppedWithException(e);
            displayError(e);
        }

        // call endRun()
        if (startrun_done)
        {
            try
            {
                endRun();
            }
            catch (std::exception& e)
            {
                had_error = true;
                displayError(e);
            }
        }

        // delete network
        if (setupnetwork_done)
        {
            try
            {
                simulation.deleteNetwork();
            }
            catch (std::exception& e)
            {
                had_error = true;
                displayError(e);
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
                   throw cTerminationException("scheduler interrupted while waiting");

               // print event banner if neccessary
               if (opt_eventbanners && mod->isEvEnabled())
               {
                   ::fprintf(fout, "** Event #%ld  T=%s.  (%s) %s (id=%d)\n",
                           simulation.eventNumber(),
                           SIMTIME_STR(simulation.simTime()),
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
                   throw cTerminationException("SIGINT or SIGTERM received, exiting");
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
                   throw cTerminationException("scheduler interrupted while waiting");

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
                               SIMTIME_STR(simulation.simTime()),
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
                               SIMTIME_STR(simulation.simTime()),
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
                   throw cTerminationException("SIGINT or SIGTERM received, exiting");
           }
        }
    }
    catch (cTerminationException& e)
    {
        ev.disable_tracing = false;
        stopClock();
        stoppedWithTerminationException(e);
        displayMessage(e);
        return;
    }
    catch (std::exception& e)
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
    TOmnetApp::sputn(s, n);

    cModule *ctxmod = simulation.contextModule();  //FIXME shouldn't this be "component" ?
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
    TOmnetApp::moduleCreated(mod);

    bool ev_enabled = ev.config()->getAsBool(mod->fullPath().c_str(), CFGID_CMDENV_EV_OUTPUT);
    mod->setEvEnabled(ev_enabled);
}

void TCmdenvApp::messageSent_OBSOLETE(cMessage *msg, cGate *)
{
    if (!opt_expressmode && opt_messagetrace)
    {
        ::fprintf(fout, "SENT:   %s\n", msg->info().c_str());
        if (opt_autoflush)
            ::fflush(fout);
    }
}

void TCmdenvApp::simulationEvent(cMessage *msg)
{
    TOmnetApp::simulationEvent(msg);

    if (!opt_expressmode && opt_messagetrace)
    {
        ::fprintf(fout, "DELIVD: %s\n", msg->info().c_str());  //FIXME can go out!!!
        if (opt_autoflush)
            ::fflush(fout);
    }
}

void TCmdenvApp::printUISpecificHelp()
{
    ev << "Cmdenv-specific options:\n";
    ev << "  -n <scenarioname>\n";
    ev << "                Print the number of runs in the given scenario, and exit.\n";
    ev << "  -c <configname-or-scenarioname>\n";
    ev << "                Select a given configuration or scenario for execution. With\n";
    ev << "                inifile-based configuration, this means the [Scenario <name>] or\n";
    ev << "                [Config <name>] sections; the default is the [General] section.\n";
    ev << "                See also: -r.\n";
    ev << "  -r <runs>     Execute the specified runs in the scenario selected with the\n";
    ev << "                -c option. <runs> is a comma-separated list of run numbers or\n";
    ev << "                run number ranges, for example 1,2,5-10. When not present, all\n" ;
    ev << "                runs will be executed.\n" ;
    ev << "\n";
}

unsigned TCmdenvApp::extraStackForEnvir()
{
    return 1024*opt_extrastack_kb;
}


