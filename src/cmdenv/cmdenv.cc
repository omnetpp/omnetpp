//==========================================================================
//  CMDENV.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <algorithm>

#include "common/opp_ctype.h"
#include "common/commonutil.h"
#include "common/fileutil.h"
#include "cmddefs.h"
#include "cmdenv.h"
#include "common/enumstr.h"
#include "envir/appreg.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cmessage.h"
#include "envir/args.h"
#include "envir/speedometer.h"
#include "omnetpp/platdep/timeutil.h"
#include "common/stringutil.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/cenum.h"
#include "omnetpp/cscheduler.h"
#include "common/stringtokenizer.h"
#include "omnetpp/cresultfilter.h"
#include "omnetpp/cresultrecorder.h"
#include "envir/visitor.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/cdensityestbase.h"
#include "omnetpp/cwatch.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/platdep/platmisc.h" // INT64_PRINTF_FORMAT


NAMESPACE_BEGIN

Register_GlobalConfigOption(CFGID_CONFIG_NAME, "cmdenv-config-name", CFG_STRING, NULL, "Specifies the name of the configuration to be run (for a value `Foo', section [Config Foo] will be used from the ini file). See also cmdenv-runs-to-execute=. The -c command line option overrides this setting.")
Register_GlobalConfigOption(CFGID_RUNS_TO_EXECUTE, "cmdenv-runs-to-execute", CFG_STRING, NULL, "Specifies which runs to execute from the selected configuration (see cmdenv-config-name=). It accepts a comma-separated list of run numbers or run number ranges, e.g. 1,3..4,7..9. If the value is missing, Cmdenv executes all runs in the selected configuration. The -r command line option overrides this setting.")
Register_GlobalConfigOptionU(CFGID_CMDENV_EXTRA_STACK, "cmdenv-extra-stack", "B",  "8KiB", "Specifies the extra amount of stack that is reserved for each activity() simple module when the simulation is run under Cmdenv.")
Register_GlobalConfigOption(CFGID_CMDENV_INTERACTIVE, "cmdenv-interactive", CFG_BOOL,  "false", "Defines what Cmdenv should do when the model contains unassigned parameters. In interactive mode, it asks the user. In non-interactive mode (which is more suitable for batch execution), Cmdenv stops with an error.")
Register_GlobalConfigOption(CFGID_OUTPUT_FILE, "cmdenv-output-file", CFG_FILENAME, NULL, "When a filename is specified, Cmdenv redirects standard output into the given file. This is especially useful with parallel simulation. See the `fname-append-host' option as well.")
Register_PerRunConfigOption(CFGID_EXPRESS_MODE, "cmdenv-express-mode", CFG_BOOL, "true", "Selects ``normal'' (debug/trace) or ``express'' mode.")
Register_PerRunConfigOption(CFGID_AUTOFLUSH, "cmdenv-autoflush", CFG_BOOL, "false", "Call fflush(stdout) after each event banner or status update; affects both express and normal mode. Turning on autoflush may have a performance penalty, but it can be useful with printf-style debugging for tracking down program crashes.")
Register_PerRunConfigOption(CFGID_MODULE_MESSAGES, "cmdenv-module-messages", CFG_BOOL, "true", "When cmdenv-express-mode=false: turns printing module ev<< output on/off.")
Register_PerRunConfigOption(CFGID_EVENT_BANNERS, "cmdenv-event-banners", CFG_BOOL, "true", "When cmdenv-express-mode=false: turns printing event banners on/off.")
Register_PerRunConfigOption(CFGID_EVENT_BANNER_DETAILS, "cmdenv-event-banner-details", CFG_BOOL, "false", "When cmdenv-express-mode=false: print extra information after event banners.")
Register_PerRunConfigOption(CFGID_MESSAGE_TRACE, "cmdenv-message-trace", CFG_BOOL, "false", "When cmdenv-express-mode=false: print a line per message sending (by send(),scheduleAt(), etc) and delivery on the standard output.")
Register_PerRunConfigOptionU(CFGID_STATUS_FREQUENCY, "cmdenv-status-frequency", "s", "2s", "When cmdenv-express-mode=true: print status update every n seconds.")
Register_PerRunConfigOption(CFGID_PERFORMANCE_DISPLAY, "cmdenv-performance-display", CFG_BOOL, "true", "When cmdenv-express-mode=true: print detailed performance information. Turning it on results in a 3-line entry printed on each update, containing ev/sec, simsec/sec, ev/simsec, number of messages created/still present/currently scheduled in FES.")
Register_PerRunConfigOption(CFGID_LOG_FORMAT, "cmdenv-log-format", CFG_STRING, "[%l]\t", "Specifies the format string that determines the prefix of each log line. Log can be written using macros such as EV_FATAL, EV_ERROR, EV_WARN, EV_INFO, EV_DETAIL, EV_DEBUG and EV_TRACE. The format string may contain constant texts interleaved with format directives. A format directive is a '%' character followed by a single format character. See the manual for the list of available format characters.");
Register_PerRunConfigOption(CFGID_GLOBAL_LOGLEVEL, "cmdenv-log-level", CFG_STRING, "DEBUG", "Specifies the level of detail recorded by log statements, output below the specified level is omitted. This setting is with AND relationship with per-component log level settings. Available values are (case insensitive): fatal, error, warn, info, detail, debug or trace. Note that the level of detail is also controlled by the specified per component log levels and the GLOBAL_COMPILETIME_LOGLEVEL macro that is used to completely remove log statements from the executable.");

Register_PerObjectConfigOption(CFGID_CMDENV_EV_OUTPUT, "cmdenv-ev-output", KIND_MODULE, CFG_BOOL, "true", "When cmdenv-express-mode=false: whether Cmdenv should print log messages (EV<<, EV_INFO, etc.) from the selected modules.")


//
// Register the Cmdenv user interface
//
Register_OmnetApp("Cmdenv", Cmdenv, 10, "command-line user interface");

//
// The following function can be used to force linking with Cmdenv; specify
// -u _cmdenv_lib (gcc) or /include:_cmdenv_lib (vc++) in the link command.
//
extern "C" CMDENV_API void cmdenv_lib() {}
// on some compilers (e.g. linux gcc 4.2) the functions are generated without _
extern "C" CMDENV_API void _cmdenv_lib() {}

#define SPEEDOMETER_UPDATEMILLISECS 1000

#define LL  INT64_PRINTF_FORMAT

static char buffer[1024];

bool Cmdenv::sigintReceived;


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


CmdenvOptions::CmdenvOptions()
{
    // note: these values will be overwritten in setup()/readOptions() before taking effect
    extraStack = 0;
    autoflush = true;
    expressMode = false;
    interactive = false;
    printModuleMsgs = true;
    printEventBanners = true;
    detailedEventBanners = false;
    messageTrace = false;
    statusFrequencyMs = 2000;
    printPerformanceData = false;
}


Cmdenv::Cmdenv() : opt((CmdenvOptions *&)EnvirBase::opt)
{
    // Note: ctor should only contain trivial initializations, because
    // the class may be instantiated only for the purpose of calling
    // printUISpecificHelp() on it

    // initialize fout to stdout, then we'll replace it if redirection is
    // requested in the ini file
    fout = stdout;

    //XXX log settings should come from some configuration or commmand-line arg
    logging = true;
    logStream = fopen(".cmdenv-log", "w");
    if (!logStream) logStream = stdout;
    //logStream = stdout;
}

Cmdenv::~Cmdenv()
{
}

void Cmdenv::readOptions()
{
    EnvirBase::readOptions();

    cConfiguration *cfg = getConfig();

    // note: configname and runstoexec will possibly be overwritten
    // with the -c, -r command-line options in our setup() method
    opt->configName = cfg->getAsString(CFGID_CONFIG_NAME);
    opt->runsToExec = cfg->getAsString(CFGID_RUNS_TO_EXECUTE);

    opt->extraStack = (size_t) cfg->getAsDouble(CFGID_CMDENV_EXTRA_STACK);
    opt->outputFile = cfg->getAsFilename(CFGID_OUTPUT_FILE).c_str();

    if (!opt->outputFile.empty())
    {
        processFileName(opt->outputFile);
        ::printf("Cmdenv: redirecting output to file `%s'...\n",opt->outputFile.c_str());
        FILE *out = fopen(opt->outputFile.c_str(), "w");
        if (!out)
            throw cRuntimeError("Cannot open output redirection file `%s'",opt->outputFile.c_str());
        fout = out;
    }
}

void Cmdenv::readPerRunOptions()
{
    EnvirBase::readPerRunOptions();

    cConfiguration *cfg = getConfig();
    opt->expressMode = cfg->getAsBool(CFGID_EXPRESS_MODE);
    opt->interactive = cfg->getAsBool(CFGID_CMDENV_INTERACTIVE);
    opt->autoflush = cfg->getAsBool(CFGID_AUTOFLUSH);
    opt->printModuleMsgs = cfg->getAsBool(CFGID_MODULE_MESSAGES);
    opt->printEventBanners = cfg->getAsBool(CFGID_EVENT_BANNERS);
    opt->detailedEventBanners = cfg->getAsBool(CFGID_EVENT_BANNER_DETAILS);
    opt->messageTrace = cfg->getAsBool(CFGID_MESSAGE_TRACE);
    opt->statusFrequencyMs = 1000*cfg->getAsDouble(CFGID_STATUS_FREQUENCY);
    opt->printPerformanceData = cfg->getAsBool(CFGID_PERFORMANCE_DISPLAY);
    setLogLevel(cLogLevel::getLevel(getConfig()->getAsString(CFGID_GLOBAL_LOGLEVEL).c_str()));
    setLogFormat(getConfig()->getAsString(CFGID_LOG_FORMAT).c_str());
}

void Cmdenv::doRun()
{
    {
        // '-c' and '-r' option: configuration to activate, and run numbers to run.
        // Both command-line options take precedence over inifile settings.
        // (NOTE: inifile settings *already* got read at this point! as EnvirBase::setup()
        // invokes readOptions()).

        const char *configName = args->optionValue('c');
        if (configName)
            opt->configName = configName;
        if (opt->configName.empty())
            opt->configName = "General";

        const char *runsToExec = args->optionValue('r');
        if (runsToExec)
            opt->runsToExec = runsToExec;

        // if the list of runs is not given explicitly, must execute all runs
        if (opt->runsToExec.empty())
        {
            int n = cfg->getNumRunsInConfig(opt->configName.c_str());  //note: may throw exception
            if (n==0)
            {
                ev.printfmsg("Error: Configuration `%s' generates 0 runs", opt->configName.c_str());
                exitcode = 1;
                return;
            }
            else
            {
                char buf[32];
                sprintf(buf, (n==1 ? "%d" : "0..%d"), n-1);
                opt->runsToExec = buf;
            }
        }

        EnumStringIterator runiterator(opt->runsToExec.c_str());
        if (runiterator.hasError())
        {
            ev.printfmsg("Error parsing list of runs to execute: `%s'", opt->runsToExec.c_str());
            exitcode = 1;
            return;
        }

        // we'll return nonzero exitcode if any run was terminated with error
        bool hadError = false;

        for (; runiterator()!=-1; runiterator++)
        {
            int runNumber = runiterator();
            bool networkSetupDone = false;
            bool startrunDone = false;
            try
            {
                ::fprintf(fout, "\nPreparing for running configuration %s, run #%d...\n", opt->configName.c_str(), runNumber);
                ::fflush(fout);

                cfg->activateConfig(opt->configName.c_str(), runNumber);

                const char *itervars = cfg->getVariable(CFGVAR_ITERATIONVARS2);
                if (itervars && strlen(itervars)>0)
                    ::fprintf(fout, "Scenario: %s\n", itervars);
                ::fprintf(fout, "Assigned runID=%s\n", cfg->getVariable(CFGVAR_RUNID));

                //cfg->dump();

                readPerRunOptions();

                // find network
                cModuleType *network = resolveNetwork(opt->networkName.c_str());
                ASSERT(network);

                // set up network
                ::fprintf(fout, "Setting up network `%s'...\n", opt->networkName.c_str());
                ::fflush(fout);

                setupNetwork(network);
                networkSetupDone = true;

                // prepare for simulation run
                ::fprintf(fout, "Initializing...\n");
                ::fflush(fout);

                disable_tracing = opt->expressMode;
                startRun();
                startrunDone = true;

                // run the simulation
                ::fprintf(fout, "\nRunning simulation...\n");
                ::fflush(fout);

                // simulate() should only throw exception if error occurred and
                // finish() should not be called.
                notifyLifecycleListeners(LF_ON_SIMULATION_START);
                simulate();
                disable_tracing = false;

                ::fprintf(fout, "\nCalling finish() at end of Run #%d...\n", runNumber);
                ::fflush(fout);
                simulation.callFinish();
                cLogProxy::flushLastLine();

                checkFingerprint();

                notifyLifecycleListeners(LF_ON_SIMULATION_SUCCESS);
            }
            catch (std::exception& e)
            {
                hadError = true;
                disable_tracing = false;
                stoppedWithException(e);
                notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
                displayException(e);
            }

            // call endRun()
            if (startrunDone)
            {
                try
                {
                    endRun();
                }
                catch (std::exception& e)
                {
                    hadError = true;
                    notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
                    displayException(e);
                }
            }

            // delete network
            if (networkSetupDone)
            {
                try
                {
                    simulation.deleteNetwork();
                }
                catch (std::exception& e)
                {
                    hadError = true;
                    notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
                    displayException(e);
                }
            }

            // skip further runs if signal was caught
            if (sigintReceived)
                break;
        }

        ::fflush(fout);

        exitcode = hadError ? 1 : sigintReceived ? 2 : 0;
    }
}

// note: also updates "since" (sets it to the current time) if answer is "true"
inline bool elapsed(long millis, struct timeval& since)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    bool ret = timeval_diff_usec(now, since) > 1000*millis;
    if (ret)
        since = now;
    return ret;
}


void Cmdenv::simulate() //XXX probably not needed anymore -- take over interesting bits to other methods!
{
    // implement graceful exit when Ctrl-C is hit during simulation. We want
    // to finish the current event, then normally exit via callFinish() etc
    // so that simulation results are not lost.
    installSignalHandler();

    startClock();
    sigintReceived = false;

    Speedometer speedometer;  // only used by Express mode, but we need it in catch blocks too

    try
    {
        if (!opt->expressMode)
        {
            disable_tracing = false;
            while (true)
            {
                cEvent *event = simulation.takeNextEvent();
                if (!event)
                    throw cTerminationException("scheduler interrupted while waiting");

                // print event banner if necessary
                if (opt->printEventBanners)
                    if (!event->isMessage() || static_cast<cMessage*>(event)->getArrivalModule()->isEvEnabled())
                        printEventBanner(event);

                // flush *between* printing event banner and event processing, so that
                // if event processing crashes, it can be seen which event it was
                if (opt->autoflush)
                    ::fflush(fout);

                // execute event
                simulation.executeEvent(event);

                // flush so that output from different modules don't get mixed
                cLogProxy::flushLastLine();

                checkTimeLimits();
                if (sigintReceived)
                    throw cTerminationException("SIGINT or SIGTERM received, exiting");
            }
        }
        else
        {
            disable_tracing = true;
            speedometer.start(simulation.getSimTime());

            struct timeval last_update;
            gettimeofday(&last_update, NULL);

            doStatusUpdate(speedometer);

            while (true)
            {
                cEvent *event = simulation.takeNextEvent();
                if (!event)
                    throw cTerminationException("scheduler interrupted while waiting");

                speedometer.addEvent(simulation.getSimTime()); //XXX potential performance hog

                // print event banner from time to time
                if ((simulation.getEventNumber()&0xff)==0 && elapsed(opt->statusFrequencyMs, last_update))
                    doStatusUpdate(speedometer);

                // execute event
                simulation.executeEvent(event);

                checkTimeLimits();  //XXX potential performance hog (maybe check every 256 events, unless "cmdenv-strict-limits" is on?)
                if (sigintReceived)
                    throw cTerminationException("SIGINT or SIGTERM received, exiting");
            }
        }
    }
    catch (cTerminationException& e)
    {
        if (opt->expressMode)
            doStatusUpdate(speedometer);
        disable_tracing = false;
        stopClock();
        deinstallSignalHandler();

        stoppedWithTerminationException(e);
        displayException(e);
        return;
    }
    catch (std::exception& e)
    {
        if (opt->expressMode)
            doStatusUpdate(speedometer);
        disable_tracing = false;
        stopClock();
        deinstallSignalHandler();
        throw;
    }

    // note: C++ lacks "finally": lines below need to be manually kept in sync with catch{...} blocks above!
    if (opt->expressMode)
        doStatusUpdate(speedometer);
    disable_tracing = false;
    stopClock();
    deinstallSignalHandler();
}

void Cmdenv::printEventBanner(cEvent *event)
{
    ::fprintf(fout, "** Event #%" LL "d  T=%s%s   ",
            simulation.getEventNumber(),
            SIMTIME_STR(simulation.getSimTime()),
            progressPercentage()); // note: IDE launcher uses this to track progress
    if (event->isMessage()) {
        cModule *mod = static_cast<cMessage*>(event)->getArrivalModule();
        ::fprintf(fout, "%s (%s, id=%d)\n",
                mod->getFullPath().c_str(),
                mod->getComponentType()->getName(),
                mod->getId());
    }
    else if (event->getTargetObject())
    {
        cObject *target = event->getTargetObject();
        ::fprintf(fout, "%s (%s)\n",
                target->getFullPath().c_str(),
                target->getClassName());
    }
//TODO:
//    ::fprintf(fout, "on %s (%s)\n",
//            event->getName(),
//            event->getClassName());
    if (opt->detailedEventBanners)
    {
        ::fprintf(fout, "   Elapsed: %s   Messages: created: %ld  present: %ld  in FES: %d\n",
                timeToStr(totalElapsed()),
                cMessage::getTotalMessageCount(),
                cMessage::getLiveMessageCount(),
                simulation.msgQueue.getLength());
    }
}

void Cmdenv::doStatusUpdate(Speedometer& speedometer)
{
    speedometer.beginNewInterval();

    if (opt->printPerformanceData)
    {
        ::fprintf(fout, "** Event #%" LL "d   T=%s   Elapsed: %s%s\n",
                simulation.getEventNumber(),
                SIMTIME_STR(simulation.getSimTime()),
                timeToStr(totalElapsed()),
                progressPercentage()); // note: IDE launcher uses this to track progress
        ::fprintf(fout, "     Speed:     ev/sec=%g   simsec/sec=%g   ev/simsec=%g\n",
                speedometer.getEventsPerSec(),
                speedometer.getSimSecPerSec(),
                speedometer.getEventsPerSimSec());

        ::fprintf(fout, "     Messages:  created: %ld   present: %ld   in FES: %d\n",
                cMessage::getTotalMessageCount(),
                cMessage::getLiveMessageCount(),
                simulation.msgQueue.getLength());
    }
    else
    {
        ::fprintf(fout, "** Event #%" LL "d   T=%s   Elapsed: %s%s   ev/sec=%g\n",
                simulation.getEventNumber(),
                SIMTIME_STR(simulation.getSimTime()),
                timeToStr(totalElapsed()),
                progressPercentage(), // note: IDE launcher uses this to track progress
                speedometer.getEventsPerSec());
    }

    // status update is always autoflushed (not only if opt->autoflush is on)
    ::fflush(fout);
}

const char *Cmdenv::progressPercentage()
{
    double simtimeRatio = -1;
    if (opt->simtimeLimit!=0)
         simtimeRatio = simulation.getSimTime() / opt->simtimeLimit;

    double cputimeRatio = -1;
    if (opt->cpuTimeLimit!=0) {
        timeval now;
        gettimeofday(&now, NULL);
        long elapsedsecs = now.tv_sec - laststarted.tv_sec + elapsedtime.tv_sec;
        cputimeRatio = elapsedsecs / (double)opt->cpuTimeLimit;
    }

    double ratio = std::max(simtimeRatio, cputimeRatio);
    if (ratio == -1)
        return "";
    else {
        static char buf[32];
        // DO NOT change the "% completed" string. The IDE launcher plugin matches
        // against this string for detecting user input
        sprintf(buf, "  %d%% completed", (int)(100*ratio));
        return buf;
    }
}

void Cmdenv::displayException(std::exception& ex)
{
    EnvirBase::displayException(ex); // will end up in putsmsg()
}

void Cmdenv::componentInitBegin(cComponent *component, int stage)
{
    if (!opt->expressMode && opt->printEventBanners && component->isEvEnabled())
        ::fprintf(fout, "Initializing %s %s, stage %d\n",
                component->isModule() ? "module" : "channel", component->getFullPath().c_str(), stage);
}

void Cmdenv::signalHandler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM)
        sigintReceived = true;
}

void Cmdenv::installSignalHandler()
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

void Cmdenv::deinstallSignalHandler()
{
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
}

bool Cmdenv::isGUI() const
{
    return false;
}

//-----------------------------------------------------

void Cmdenv::askParameter(cPar *par, bool unassigned)
{
    bool success = false;
    while (!success)
    {
        cProperties *props = par->getProperties();
        cProperty *prop = props->get("prompt");
        std::string prompt = prop ? prop->getValue(cProperty::DEFAULTKEY) : "";
        std::string reply;

        // ask the user. note: gets() will signal "cancel" by throwing an exception
        if (!prompt.empty())
            reply = this->gets(prompt.c_str(), par->str().c_str());
        else
            // DO NOT change the "Enter parameter" string. The IDE launcher plugin matches
            // against this string for detecting user input
            reply = this->gets((std::string("Enter parameter `")+par->getFullPath()+"' ("+(unassigned?"unassigned":"ask")+"):").c_str(), par->str().c_str());

        try
        {
            par->parse(reply.c_str());
            success = true;
        }
        catch (std::exception& e)
        {
            ev.printfmsg("%s -- please try again.", e.what());
        }
    }
}

void Cmdenv::putsmsg(const char *s)
{
    ::fprintf(fout, "\n<!> %s\n\n", s);
    ::fflush(fout);
}

void Cmdenv::log(cLogEntry *entry)
{
    EnvirBase::log(entry);

    if (disable_tracing)
        return;

    cComponent *ctx = simulation.getContext();
    if (!ctx || (opt->printModuleMsgs && ctx->isEvEnabled()) || simulation.getContextType()==CTX_FINISH)
    {
        std::string prefix = logFormatter.formatPrefix(entry);
        ::fputs(prefix.c_str(), fout);
        ::fwrite(entry->text, 1, entry->textLength, fout);
        if (opt->autoflush)
            ::fflush(fout);
    }
}

std::string Cmdenv::gets(const char *prompt, const char *defaultReply)
{
    if (!opt->interactive)
        throw cRuntimeError("The simulation wanted to ask a question, set cmdenv-interactive=true to allow it: \"%s\"", prompt);

    ::fprintf(fout, "%s", prompt);
    if (!opp_isempty(defaultReply))
        ::fprintf(fout, "(default: %s) ", defaultReply);
    ::fflush(fout);

    {
        ::fgets(buffer, 512, stdin);
        buffer[strlen(buffer)-1] = '\0'; // chop LF

        if (buffer[0]=='\x1b') // ESC?
           throw cRuntimeError(E_CANCEL);

        return std::string(buffer);
    }
}

bool Cmdenv::askyesno(const char *question)
{
    if (!opt->interactive)
        throw cRuntimeError("Simulation needs user input in non-interactive mode (prompt text: \"%s (y/n)\")", question);

    {
        for(;;)
        {
            ::fprintf(fout, "%s (y/n) ", question);
            ::fflush(fout);
            ::fgets(buffer, 512, stdin);
            buffer[strlen(buffer)-1] = '\0'; // chop LF
            if (buffer[0]=='\x1b') // ESC?
               throw cRuntimeError(E_CANCEL);
            if (opp_toupper(buffer[0])=='Y' && !buffer[1])
                return true;
            else if (opp_toupper(buffer[0])=='N' && !buffer[1])
                return false;
            else
                putsmsg("Please type 'y' or 'n'!\n");
        }
    }
}

void Cmdenv::debug(const char *fmt,...)
{
    if (!logging)
        return;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t t = (time_t) tv.tv_sec;
    struct tm tm = *localtime(&t);

    ::fprintf(logStream, "[%02d:%02d:%02d.%03d event #%" LL "d %s] ",
             tm.tm_hour, tm.tm_min, tm.tm_sec, (int)(tv.tv_usec/1000),
             simulation.getEventNumber(), "");
    va_list va;
    va_start(va, fmt);
    ::vfprintf(logStream, fmt, va);
    va_end(va);
    ::fflush(logStream); // needed for sensible output in the IDE console
}

bool Cmdenv::idle()
{
    return sigintReceived;
}

void Cmdenv::moduleCreated(cModule *mod)
{
    EnvirBase::moduleCreated(mod);

    bool ev_enabled = getConfig()->getAsBool(mod->getFullPath().c_str(), CFGID_CMDENV_EV_OUTPUT);
    mod->setEvEnabled(ev_enabled);
}

void Cmdenv::messageSent_OBSOLETE(cMessage *msg, cGate *)
{
    if (!opt->expressMode && opt->messageTrace)
    {
        ::fprintf(fout, "SENT:   %s\n", msg->info().c_str());
        if (opt->autoflush)
            ::fflush(fout);
    }
}

void Cmdenv::simulationEvent(cEvent *event)
{
    EnvirBase::simulationEvent(event);

    if (!disable_tracing)
    {
        if (opt->messageTrace)
        {
            ::fprintf(fout, "DELIVD: %s\n", event->info().c_str());  //TODO can go out!
            if (opt->autoflush)
                ::fflush(fout);
        }
    }
}

void Cmdenv::printUISpecificHelp()
{
    std::cout << "Cmdenv-specific options:\n";
    std::cout << "  -c <configname>\n";
    std::cout << "                Select a given configuration for execution. With inifile-based\n";
    std::cout << "                configuration database, this selects the [Config <configname>]\n";
    std::cout << "                section; the default is the [General] section.\n";
    std::cout << "                See also: -r.\n";
    std::cout << "  -r <runs>     Execute the specified runs in the configuration selected with the\n";
    std::cout << "                -c option. <runs> is a comma-separated list of run numbers or\n";
    std::cout << "                run number ranges, for example 1,2,5-10. When not present, all\n" ;
    std::cout << "                runs of that configuration will be executed.\n" ;
    std::cout << "  -a            Print all config names and number of runs it them, and exit.\n";
    std::cout << "  -x <configname>\n";
    std::cout << "                Print the number of runs in the given configuration, and exit.\n";
    std::cout << "  -g, -G        Make -x verbose: print the unrolled configuration, iteration\n";
    std::cout << "                variables, etc. -G provides more details than -g.\n";
    std::cout << "  -X <configname>\n";
    std::cout << "                Print the fallback chain of the given configuration, and exit.\n";
}

unsigned Cmdenv::getExtraStackForEnvir() const
{
    return opt->extraStack;
}

NAMESPACE_END

