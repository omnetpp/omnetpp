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

#include "opp_ctype.h"
#include "commonutil.h"
#include "fileutil.h"
#include "cmddefs.h"
#include "cmdenv.h"
#include "enumstr.h"
#include "appreg.h"
#include "csimplemodule.h"
#include "ccomponenttype.h"
#include "cmessage.h"
#include "args.h"
#include "speedometer.h"
#include "timeutil.h"
#include "stringutil.h"
#include "cconfigoption.h"
#include "checkandcast.h"
#include "cproperties.h"
#include "cproperty.h"
#include "cenum.h"
#include "cscheduler.h"
#include "stringtokenizer.h"
#include "cresultfilter.h"
#include "cresultrecorder.h"
#include "visitor.h"
#include "cclassdescriptor.h"
#include "cqueue.h"
#include "cchannel.h"
#include "coutvector.h"
#include "cstatistic.h"
#include "cdensityestbase.h"
#include "cwatch.h"
#include "cdisplaystring.h"


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


#ifdef SIMFRONTEND_SUPPORT
/*TODO:
notifyLifecycleListeners(LF_ON_SIMULATION_START);
notifyLifecycleListeners(LF_ON_SIMULATION_SUCCESS);
notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
*/
#endif

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

#ifdef SIMFRONTEND_SUPPORT
Register_Enum2(stateEnum, "State",
        ("nonetwork", Cmdenv::SIM_NONETWORK,
        "ready", Cmdenv::SIM_READY,
        "running", Cmdenv::SIM_RUNNING,
        "terminated", Cmdenv::SIM_TERMINATED,
        "error", Cmdenv::SIM_ERROR,
        "finishcalled", Cmdenv::SIM_FINISHCALLED,
        NULL));

Register_Enum2(runModeEnum, "RunMode",
        ("normal", Cmdenv::RUNMODE_NORMAL,
        "fast", Cmdenv::RUNMODE_FAST,
        "express", Cmdenv::RUNMODE_EXPRESS,
        NULL));

Register_Enum2(stoppingReasonEnum, "StoppingReason",
        ("none", Cmdenv::STOP_NONE,
        "untilsimtime", Cmdenv::STOP_UNTILSIMTIME,
        "untilevent", Cmdenv::STOP_UNTILEVENT,
        "untilmodule", Cmdenv::STOP_UNTILMODULE,
        "untilmessage", Cmdenv::STOP_UNTILMESSAGE,
        "realtimechunk", Cmdenv::STOP_REALTIMECHUNK,
        "stopcommand", Cmdenv::STOP_STOPCOMMAND,
        "termination", Cmdenv::STOP_TERMINATION,
        NULL));

Register_Enum2(commandEnum, "Command",
        ("none", Cmdenv::CMD_NONE,
        "setupNetwork", Cmdenv::CMD_SETUPNETWORK,
        "setupRun", Cmdenv::CMD_SETUPRUN,
        "rebuild", Cmdenv::CMD_REBUILD,
        "step", Cmdenv::CMD_STEP,
        "run", Cmdenv::CMD_RUN,
        "stop", Cmdenv::CMD_STOP,
        "finish", Cmdenv::CMD_FINISH,
        "quit", Cmdenv::CMD_QUIT,
        NULL));

Register_Enum2(userInputState, "UserInputState",
        ("none", Cmdenv::INPSTATE_NONE,
        "initiated", Cmdenv::INPSTATE_INITIATED,
        "waitingForReply", Cmdenv::INPSTATE_WAITINGFORREPLY,
        "replyArrived", Cmdenv::INPSTATE_REPLYARRIVED,
        NULL));

Register_Enum2(userInputType, "UserInputType",
        ("none", Cmdenv::INP_NONE,
        "error", Cmdenv::INP_ERROR,
        "askParameter", Cmdenv::INP_ASKPARAMETER,
        "gets", Cmdenv::INP_GETS,
        "askYesNo", Cmdenv::INP_ASKYESNO,
        "msgDialog", Cmdenv::INP_MSGDIALOG,
        NULL));
#endif

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
#ifdef SIMFRONTEND_SUPPORT
    httpControlled = false;
    updatefreqFast = 500; // ms
    updatefreqExpress = 1000; // ms
#endif
}


Cmdenv::Cmdenv() : opt((CmdenvOptions *&)EnvirBase::opt)
{
    // Note: ctor should only contain trivial initializations, because
    // the class may be instantiated only for the purpose of calling
    // printUISpecificHelp() on it

    // initialize fout to stdout, then we'll replace it if redirection is
    // requested in the ini file
    fout = stdout;

#ifdef SIMFRONTEND_SUPPORT
    state = SIM_NONETWORK;
    command = CMD_NONE;
    stoppingReason = STOP_NONE;

    isConfigRun = false; //TODO check all other new vars too

    serializer = NULL;

    collectJsonLog = false;
    jsonLog = new JsonArray();

    userInput.state = INPSTATE_NONE;
    userInput.type = INP_NONE;
#endif

    //XXX log settings should come from some configuration or commmand-line arg
    logging = true;
    logStream = fopen(".cmdenv-log", "w");
    if (!logStream) logStream = stdout;
    //logStream = stdout;
}

Cmdenv::~Cmdenv()
{
#ifdef SIMFRONTEND_SUPPORT
    delete serializer;
    delete jsonLog;
#endif
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
#ifdef SIMFRONTEND_SUPPORT
    httpServer->addHttpRequestHandler(this); //FIXME should go into setup()?

    if (args->optionGiven('w'))
    {
        // interactive mode: wait for commands
        opt->httpControlled = true;

        collectJsonLog = true;
        serializer = new Serializer(); // because collectJsonLog==true requires it

        processHttpRequests(true);
    }
    else
#endif
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

#ifdef SIMFRONTEND_SUPPORT
void Cmdenv::processHttpRequests(bool blocking)
{
    //FIXME handle exceptions! (store them, and return them when status is requested)
    for (;;)
    {
        while (command == CMD_NONE) {
            bool didSomething = httpServer->handleOneRequest(blocking);  // ends up in handle(cHttpRequest*)
            if (!blocking && !didSomething)
                return;
        }

        int currentCommand = command;
        command = CMD_NONE;
        processCommand(currentCommand);
    }
}

bool Cmdenv::handleHttpRequest(cHttpRequest *request)
{
    ASSERT2(command == CMD_NONE, "previous command not marked as done yet");

    //TODO: rewrite HttpServer so it directly uses mongoose's url-based callback registration...
    const char *method = request->getRequestMethod();
    const char *uri = request->getUri();
    const char *query = request->getQueryString();
    debug(!query ? "[http] %s %s\n" : "[http] %s %s?%s\n", method, uri, query);

    const char *OK_STATUS = "HTTP/1.0 200 OK\n";
    const char *ERROR_STATUS = "HTTP/1.0 409 Conflict\n\nRequest not allowed in this state\n";

    // check method and base URL
    if (strcmp(method, "GET") != 0 || strncmp(uri, "/sim", 4) !=0) {
        debug("[http] wrong method or uri\n");
        return false;
    }

    // ping we accept any time, it has no side effect
    if (strcmp(uri, "/sim/ping") == 0) {
        request->print(OK_STATUS);
        return true;
    }

    // if user input is in progress, we only allow requests that post the reply or are side effect free
    if (userInput.state != INPSTATE_NONE) {
        if (strcmp(uri, "/sim/reply") != 0 && strcmp(uri, "/sim/status") != 0 && strcmp(uri, "/sim/getObjectInfo") != 0) {
            debug("[http] request %s not allowed in userInput.state=%s\n", uri, userInputState->getStringFor(userInput.state));
            request->print(ERROR_STATUS);
            return true;
        }
    }

    clock_t startTime = clock();

    if (!serializer)
        serializer = new Serializer();  //TODO only in requests that need it!!!!

    // parse query string into commandArgs[]
    commandArgs.clear();
    StringTokenizer tokenizer(request->getQueryString(), "&");
    while (tokenizer.hasMoreTokens())
    {
        const char *token = tokenizer.nextToken();
        const char *eqPtr = strchr(token, '=');
        if (!eqPtr)
            commandArgs[token] = "";
        else
            commandArgs[std::string(token, eqPtr-token)] = opp_urldecode(eqPtr + 1);
    }

    if (strcmp(uri, "/sim/setupNetwork") == 0) {
        //TODO check state!
        command = CMD_SETUPNETWORK;
        request->print(OK_STATUS);
    }
    else if (strcmp(uri, "/sim/setupRun") == 0) {
        //TODO check state!
        command = CMD_SETUPRUN;
        request->print(OK_STATUS);
    }
    else if (strcmp(uri, "/sim/rebuild") == 0) {
        //TODO check state!
        command = CMD_REBUILD;
        request->print(OK_STATUS);
    }
    else if (strcmp(uri, "/sim/step") == 0) {
        //TODO check state!
        command = CMD_STEP;
        request->print(OK_STATUS);
    }
    else if (strcmp(uri, "/sim/run") == 0) {
        //TODO check state!
        command = CMD_RUN;
        request->print(OK_STATUS);
    }
    else if (strcmp(uri, "/sim/stop") == 0) {
        //TODO check state!
        command = CMD_STOP;
        request->print(OK_STATUS);
    }
    else if (strcmp(uri, "/sim/callFinish") == 0) {
        //TODO check state!
        command = CMD_FINISH;
        request->print(OK_STATUS);
    }
    else if (strcmp(uri, "/sim/quit") == 0) {
        //TODO check state!
        command = CMD_QUIT;
        request->print(OK_STATUS);
    }
    else if (strcmp(uri, "/sim/reply") == 0) {
        if (userInput.state != INPSTATE_WAITINGFORREPLY) {
            request->print(ERROR_STATUS);
        }
        else {
            bool hasValue = commandArgs.find("value") != commandArgs.end();
            userInput.reply = hasValue ? commandArgs["value"] : "";
            userInput.cancel = !hasValue;
            userInput.state = INPSTATE_REPLYARRIVED;
            request->print(OK_STATUS);
        }
    }
    else if (strcmp(uri, "/sim/status") == 0) {
        request->print(OK_STATUS);
        request->print("\n");

        JsonObject *result = new JsonObject();
        result->put("hostname", jsonWrap(opp_gethostname()));
        result->put("processid", jsonWrap(getpid()));
        JsonArray *jArgv = new JsonArray();
        for (int i = 0; i < args->getArgCount(); i++)
            jArgv->push_back(jsonWrap(args->getArgVector()[i]));
        result->put("argv", jArgv);
        char wdbuf[1024] = "n/a";
        result->put("wd", jsonWrap(getcwd(wdbuf,sizeof(wdbuf))));
        result->put("state", jsonWrap(stateEnum->getStringFor(state)));
        result->put("changeCounter", jsonWrap(cObject::getChangeCounter()));
        result->put("eventlogfile", jsonWrap(toAbsolutePath(eventlogmgr->getFileName())));
        if (state != SIM_NONETWORK) {
            ASSERT(simulation.getSystemModule());
            cConfigurationEx *cfg = getConfigEx();
            result->put("config", jsonWrap(cfg->getActiveConfigName()));
            result->put("run", jsonWrap(cfg->getActiveRunNumber()));
            result->put("network", jsonWrap(simulation.getNetworkType()->getName()));
            result->put("lastEventNumber", jsonWrap(simulation.getEventNumber() - 1)); // FIXME: eventnumber is immediately incremented after handling a message -- fix this
            result->put("lastEventSimtime", jsonWrap(simTime()));  //FIXME simTime() is also updated at the wrong place (in the scheduler, not it doOneEvent())
            result->put("nextEventNumber", jsonWrap(simulation.getEventNumber()));
            result->put("nextEventSimtimeGuess", jsonWrap(simulation.guessNextSimtime()));
            if (simulation.guessNextModule())
                result->put("nextEventModuleIdGuess", jsonWrap(simulation.guessNextModule()->getId()));
            cEvent *guessNextEvent = simulation.guessNextEvent();
            if (guessNextEvent && guessNextEvent->isMessage())
                result->put("nextEventMessageIdGuess", jsonWrap(static_cast<cMessage*>(guessNextEvent)->getId()));
            result->put("stoppingReason", jsonWrap(stoppingReasonEnum->getStringFor(stoppingReason)));
        }

        JsonObject *jRootObjects = new JsonObject();
        jRootObjects->put("simulation", jsonWrapObjectId(&simulation));
        jRootObjects->put("fes", jsonWrapObjectId(&simulation.msgQueue));
        jRootObjects->put("systemModule", jsonWrapObjectId(simulation.getSystemModule()));
        jRootObjects->put("defaultList", jsonWrapObjectId(&defaultList));
        jRootObjects->put("componentTypes", jsonWrapObjectId(componentTypes.getInstance()));
        jRootObjects->put("nedFunctions", jsonWrapObjectId(nedFunctions.getInstance()));
        jRootObjects->put("classes", jsonWrapObjectId(classes.getInstance()));
        jRootObjects->put("enums", jsonWrapObjectId(enums.getInstance()));
        jRootObjects->put("classDescriptors", jsonWrapObjectId(classDescriptors.getInstance()));
        jRootObjects->put("configOptions", jsonWrapObjectId(configOptions.getInstance()));
        jRootObjects->put("resultFilters", jsonWrapObjectId(resultFilters.getInstance()));
        jRootObjects->put("resultRecorders", jsonWrapObjectId(resultRecorders.getInstance()));
        result->put("rootObjects", jRootObjects);

        if (collectJsonLog) {
            result->put("log", jsonLog);
            jsonLog = new JsonArray();  // previous one is now owned by result
        }

        if (userInput.state == INPSTATE_INITIATED) {
            ASSERT(userInput.request != NULL);
            userInput.request->put("type", jsonWrap(userInputType->getStringFor(userInput.type)));
            result->put("userInput", userInput.request);
            userInput.request = NULL;
            userInput.state = INPSTATE_WAITINGFORREPLY;
        }

        std::stringstream ss;
        result->printOn(ss);
        delete result;
        request->print(ss.str().c_str());
    }
    else if (strcmp(uri, "/sim/enumerateConfigs") == 0) {
        request->print(OK_STATUS);
        request->print("\n");

        JsonArray *result = new JsonArray();
        cConfigurationEx *cfg = getConfigEx();
        std::vector<std::string> configNames = cfg->getConfigNames();
        for (int i = 0; i < (int)configNames.size(); i++) {
            JsonObject *jconfig = new JsonObject();
            std::string configName = configNames[i];
            jconfig->put("name", jsonWrap(configName));
            jconfig->put("description", jsonWrap(cfg->getConfigDescription(configName.c_str())));
            jconfig->put("numRuns", jsonWrap(cfg->getNumRunsInConfig(configName.c_str())));
            JsonArray *jbaseConfigs = new JsonArray();
            std::vector<std::string> baseConfigs = cfg->getBaseConfigs(configName.c_str());
            for (unsigned int j = 0; j < baseConfigs.size(); j++)
                jbaseConfigs->push_back(jsonWrap(baseConfigs[j]));
            jconfig->put("extends", jbaseConfigs);
            result->push_back(jconfig);
        }

        std::stringstream ss;
        result->printOn(ss);
        delete result;
        request->print(ss.str().c_str());
    }
    else if (strcmp(uri, "/sim/getObjectInfo") == 0) {
        request->print(OK_STATUS);
        request->print("\n");

        clock_t startTime = clock();

        std::string ids = commandArgs["ids"];
        std::string what = commandArgs["what"];
        bool wantInfo = (what=="" || what.find('i') != std::string::npos);
        bool wantChildren = (what=="" || what.find('c') != std::string::npos);
        bool wantDetails = (what=="" || what.find('d') != std::string::npos);
        std::string sinceStr = commandArgs["since"];
        int64 lastRefreshSerial = sinceStr=="" ? -1 : strtoll(sinceStr.c_str(), NULL, 10);
        JsonObject2 *result = new JsonObject2();
        StringTokenizer tokenizer(ids.c_str(), ",");
        while (tokenizer.hasMoreTokens())
        {
             std::string objectId = tokenizer.nextToken();
             cObject *obj = serializer->getObjectById(atol(objectId.c_str()));
             if (obj)
             {
                 JsonObject *jObject = new JsonObject();
                 if (wantInfo)
                     serializer->serializeObject(obj, jObject, lastRefreshSerial);
                 if (wantChildren)
                 serializer->serializeObjectChildren(obj, jObject, lastRefreshSerial);
                 if (wantDetails)
                     serializer->serializeObjectFields(obj, jObject, lastRefreshSerial);
                 result->put(objectId, jObject);
             }
        }

        double consumedCPU = (clock() - startTime) / (double)CLOCKS_PER_SEC;
        debug("[http] json tree assembly took %lgs\n", consumedCPU);

        startTime = clock();

        std::stringstream ss;
        result->printOn(ss);
        delete result;

        consumedCPU = (clock() - startTime) / (double)CLOCKS_PER_SEC;
        debug("[http] json tree serialization took %lgs\n", consumedCPU);

        request->print(ss.str().c_str());
    }
    else if (strcmp(uri, "/sim/search") == 0) {
        request->print(OK_STATUS);
        request->print("\n");

        clock_t startTime = clock();

        // take request params
        cObject *root = serializer->getObjectById(atol(commandArgs["root"].c_str()));   //TODO exception handling
        std::string categoriesParam = commandArgs["cat"];
        std::string classNamePattern = commandArgs["class"];
        std::string objectFullPathPattern = commandArgs["fullpath"];
        int maxCount = atol(commandArgs["max"].c_str());  // 0 for infinite

        // convert categories string to bitfield
        unsigned int categories = (categoriesParam == "") ? CATEGORY_ALL : 0;  // empty means ALL
        for (const char *s = categoriesParam.c_str(); *s; s++) {
            switch (*s) {
                case 'a': categories = CATEGORY_ALL; break;
                case 'm': categories |= CATEGORY_MODULES; break;
                case 'q': categories |= CATEGORY_QUEUES; break;
                case 's': categories |= CATEGORY_STATISTICS; break;
                case 'g': categories |= CATEGORY_MESSAGES; break;
                case 'v': categories |= CATEGORY_VARIABLES; break;
                case 'p': categories |= CATEGORY_MODPARAMS; break;
                case 'c': categories |= CATEGORY_CHANSGATES; break;
                case 'o': categories |= CATEGORY_OTHERS; break;
            }
        }

        // execute query
        cFilteredCollectObjectsVisitor visitor;
        visitor.setSizeLimit(maxCount);
        visitor.setFilterPars(categories, classNamePattern.c_str(), objectFullPathPattern.c_str());  //TODO try/catch!!!
        visitor.process(root);

        // convert result to json
        JsonObject *result = new JsonObject();
        JsonArray *jchildren = new JsonArray();
        for (int i = 0; i < visitor.getArraySize(); i++)
            jchildren->push_back(jsonWrapObjectId(visitor.getArray()[i]));
        result->put("objects", jchildren);

        std::stringstream ss;
        result->printOn(ss);
        delete result;

        double consumedCPU = (clock() - startTime) / (double)CLOCKS_PER_SEC;
        debug("[http] processing took %lgs\n", consumedCPU);

        request->print(ss.str().c_str());
    }
    else {
        debug("[http] unrecognized request\n");
        return false; // not handled
    }

    double consumedCPU = (clock() - startTime) / (double)CLOCKS_PER_SEC;
    debug("[http] done, processing took %lgs\n", consumedCPU);

    return true; // handled
}

void Cmdenv::processCommand(int command)
{
    debug("[cmdenv] processing \"%s\" command\n", commandEnum->getStringFor(command));
    if (command == CMD_SETUPNETWORK) {
        const char *networkName = commandArgs["network"].c_str();
        newNetwork(networkName);
    }
    else if (command == CMD_SETUPRUN) {
        const char *configName = commandArgs["config"].c_str();
        int runNumber = atoi(commandArgs["run"].c_str());
        newRun(configName, runNumber);
    }
    else if (command == CMD_REBUILD) {
        rebuildSim();
    }
    else if (command == CMD_STEP) {
        doOneStep();
    }
    else if (command == CMD_RUN) {
        //TODO exception handling!
        RunMode mode = (RunMode)runModeEnum->lookup(commandArgs["mode"].c_str(), RUNMODE_NONE);
        long realTimeMillis = commandArgs["rtlimit"]=="" ? 0 : opp_atof(commandArgs["rtlimit"].c_str());
        simtime_t untilSimTime = commandArgs["utime"]=="" ? 0 : STR_SIMTIME(commandArgs["utime"].c_str());
        eventnumber_t untilEventNumber = commandArgs["uevent"]=="" ? 0 : opp_atol(commandArgs["uevent"].c_str());
        cMessage *untilMessage = commandArgs["umsg"]=="" ? NULL : check_and_cast<cMessage*>(serializer->getObjectById(opp_atol(commandArgs["umsg"].c_str())));
        cModule *untilModule = commandArgs["umod"]=="" ? NULL : check_and_cast<cModule*>(serializer->getObjectById(opp_atol(commandArgs["umod"].c_str())));
        runSimulation(mode, realTimeMillis, untilSimTime, untilEventNumber, untilMessage, untilModule);
    }
    else if (command == CMD_STOP) {
        setStopSimulationFlag();
    }
    else if (command == CMD_FINISH) {
        finishSimulation();
    }
    else if (command == CMD_QUIT) {
        //TODO kezelni! set some "should_exit" flag like Tkenv?
    }
    else {
        ASSERT(false);
    }
    debug("[cmdenv] command \"%s\" done\n", commandEnum->getStringFor(command));
}

void Cmdenv::newNetwork(const char *networkname)
{
    try
    {
        // finish & cleanup previous run if we haven't done so yet
        if (state != SIM_NONETWORK)
        {
            if (state != SIM_FINISHCALLED)
                endRun();
            simulation.deleteNetwork();
            state = SIM_NONETWORK;
        }

        cModuleType *network = resolveNetwork(networkname);
        ASSERT(network);

        // set up new network with config General.
        getConfigEx()->activateConfig("General", 0);
        readPerRunOptions();
        opt->networkName = network->getName();  // override config setting
        setupNetwork(network);
        startRun();

        state = SIM_READY;
        isConfigRun = false;
    }
    catch (std::exception& e)
    {
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
        state = SIM_ERROR;
    }
}

void Cmdenv::newRun(const char *configname, int runnumber)
{
    try
    {
        // finish & cleanup previous run if we haven't done so yet
        if (state != SIM_NONETWORK)
        {
            if (state != SIM_FINISHCALLED)
                endRun();
            simulation.deleteNetwork();
            state = SIM_NONETWORK;
        }

        // set up new network
        getConfigEx()->activateConfig(configname, runnumber);
        readPerRunOptions();

        if (opt->networkName.empty())
        {
            //TODO confirm("No network specified in the configuration.");
            return;
        }

        cModuleType *network = resolveNetwork(opt->networkName.c_str());
        ASSERT(network);

        setupNetwork(network);
        startRun();

        state = SIM_READY;
        isConfigRun = true;
    }
    catch (std::exception& e)
    {
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
        state = SIM_ERROR;
    }
}

void Cmdenv::rebuildSim()
{
    if (isConfigRun)
         newRun(std::string(getConfigEx()->getActiveConfigName()).c_str(), getConfigEx()->getActiveRunNumber());
    else if (simulation.getNetworkType()!=NULL)
         newNetwork(simulation.getNetworkType()->getName());
    else
         /*nothing*/;
}

void Cmdenv::doOneStep()
{
    ASSERT(state==SIM_READY);

    runUntil.msg = NULL; // deactivate corresponding checks in eventCancelled()/objectDeleted()

    state = SIM_RUNNING;
    stoppingReason = STOP_NONE;

    startClock();
    notifyLifecycleListeners(LF_ON_SIMULATION_RESUME);
    try
    {
        cEvent *event = simulation.takeNextEvent();
        if (event) {
            printEventBanner(event);
            simulation.executeEvent(event);
        }

        state = SIM_READY;
        notifyLifecycleListeners(LF_ON_SIMULATION_PAUSE);
    }
    catch (cTerminationException& e)
    {
        state = SIM_TERMINATED;
        stoppedWithTerminationException(e);
        notifyLifecycleListeners(LF_ON_SIMULATION_SUCCESS);
        displayException(e);
    }
    catch (std::exception& e)
    {
        state = SIM_ERROR;
        stoppedWithException(e);
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
    }
    stopClock();
    stopSimulationFlag = false;

    if (state == SIM_TERMINATED)
    {
        // call wrapper around simulation.callFinish() and simulation.endRun()
        //
        // NOTE: if the simulation is in SIM_ERROR, we don't want endRun() to be
        // called yet, because we want to allow the user to force finish() from
        // the GUI -- and finish() has to precede endRun(). endRun() will be called
        // just before a new network gets set up, or on Tkenv shutdown.
        //
        finishSimulation();
    }
}

/*
 * TODO:
 * futtatasi modok:
 *   EXPRESS: mint most (EV logging off, idokozonkent processHttpRequests)
 *   FAST:    EV logging on, x eventenkent / masodpercenkent processHttpRequests()
 *   NORMAL:  EV logging on, minden event utan processHttpRequests()
 * nem fontos hogy kozben lehessen valtogatni! stop + run szinten muxik!
 * UI update idejere megallitjuk (eleve until darabkakkal kell futtatni!)
 * until-t mindegyik tamogatja (simtime, eventnumber, msg, module, PLUS: elapsed!!! [i.e. run for 2s])
 */
void Cmdenv::runSimulation(RunMode mode, long realTimeMillis, simtime_t untilSimTime, eventnumber_t untilEventNumber, cMessage *untilMessage, cModule *untilModule)
{
    ASSERT(state==SIM_READY);

    runMode = mode;
    runUntil.simTime = untilSimTime;
    runUntil.eventNumber = untilEventNumber;
    runUntil.msg = untilMessage;
    runUntil.module = untilModule;  // Note: this is NOT supported with RUNMODE_EXPRESS
    runUntil.hasRealTimeLimit = (realTimeMillis != 0);
    if (runUntil.hasRealTimeLimit) {
        struct timeval t;
        gettimeofday(&t, NULL);
        runUntil.realTime = timeval_add(t, realTimeMillis * 0.001);
    }

    stopSimulationFlag = false;

    state = SIM_RUNNING;
    stoppingReason = STOP_NONE;

    //TODO: uzemmod valtogatast kiszedni belole!!!! nem kell!!!

    startClock();
    notifyLifecycleListeners(LF_ON_SIMULATION_RESUME);
    try
    {
        // funky while loop to handle switching to and from EXPRESS mode....
        bool cont = true;
        while (cont)
        {
            if (runMode == RUNMODE_EXPRESS)
                cont = doRunSimulationExpress();
            else
                cont = doRunSimulation();
        }
        state = SIM_READY;
        notifyLifecycleListeners(LF_ON_SIMULATION_PAUSE);
    }
    catch (cTerminationException& e)
    {
        state = SIM_TERMINATED;
        stoppingReason = STOP_TERMINATION;
        stoppedWithTerminationException(e);
        notifyLifecycleListeners(LF_ON_SIMULATION_SUCCESS);
        displayException(e);
    }
    catch (std::exception& e)
    {
        state = SIM_ERROR;
        stoppingReason = STOP_TERMINATION;
        stoppedWithException(e);
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
    }
    stopClock();
    stopSimulationFlag = false;

    disable_tracing = false;
    runUntil.msg = NULL;

    if (state==SIM_TERMINATED)
    {
        // call wrapper around simulation.callFinish() and simulation.endRun()
        //
        // NOTE: if the simulation is in SIM_ERROR, we don't want endRun() to be
        // called yet, because we want to allow the user to force finish() from
        // the GUI -- and finish() has to precede endRun(). endRun() will be called
        // just before a new network gets set up, or on Tkenv shutdown.
        //
        finishSimulation();
    }
}
#endif

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

#ifdef SIMFRONTEND_SUPPORT
inline void resetElapsedTime(struct timeval& t)
{
    gettimeofday(&t, NULL);
}

static bool moduleContains(cModule *potentialparent, cModule *mod)
{
   while (mod)
   {
       if (mod == potentialparent)
           return true;
       mod = mod->getParentModule();
   }
   return false;
}

bool Cmdenv::doRunSimulation()
{
    //
    // IMPORTANT:
    // The following variables may change during execution (as a result of user interaction
    // during Tcl_Eval("update"):
    //  - runmode, runUntil.simTime, runUntil.eventNumber, runUntil.msg, runUntil.module;
    //  - stopsimulation_flag
    //
    Speedometer speedometer;
    speedometer.start(simulation.getSimTime());
    disable_tracing = false;
    bool isFirstEvent = true;

    struct timeval lastUpdateTime;
    gettimeofday(&lastUpdateTime, NULL);

    while (true)
    {
        if (runMode == RUNMODE_EXPRESS)
            return true;  // should continue, but in a different mode

        cEvent *event = simulation.takeNextEvent();
        if (!event)
            break; // scheduler interrupted (parsim)

        // "run until message": stop if desired event was reached
        if (runUntil.msg && event == runUntil.msg) {
            simulation.putBackEvent(event);
            stoppingReason = STOP_UNTILMESSAGE;
            break;
        }

        // if stepping locally in module, we stop both immediately
        // *before* and *after* executing the event in that module,
        // but we always execute at least one event
        cModule *mod = event->isMessage() ? static_cast<cMessage*>(event)->getArrivalModule() : NULL;
        bool untilmodule_reached = runUntil.module && moduleContains(runUntil.module,mod);
        if (untilmodule_reached && !isFirstEvent) {
            simulation.putBackEvent(event);
            stoppingReason = STOP_UNTILMODULE;
            break;
        }
        isFirstEvent = false;

        bool frequentUpdates = (runMode==RUNMODE_NORMAL);

        speedometer.addEvent(simulation.getSimTime());

        // do a simulation step
        printEventBanner(event);

        // flush *between* printing event banner and event processing, so that
        // if event processing crashes, it can be seen which event it was
        if (opt->autoflush)
            ::fflush(fout);

        simulation.executeEvent(event);

        // flush so that output from different modules don't get mixed
        cLogProxy::flushLastLine();

        // display update
        if (frequentUpdates || ((simulation.getEventNumber()&0x0f)==0 && elapsed(opt->updatefreqFast, lastUpdateTime)))
        {
            if (runUntil.hasRealTimeLimit) {
                struct timeval t;
                gettimeofday(&t, NULL);
                if (timeval_greater(t, runUntil.realTime)) {
                    stoppingReason = STOP_REALTIMECHUNK;
                    break;
                }
            }

            if (speedometer.getMillisSinceIntervalStart() > SPEEDOMETER_UPDATEMILLISECS) {
                speedometer.beginNewInterval();
//                updatePerformanceDisplay(speedometer);
            }
//            Tcl_Eval(interp, "update");
            processHttpRequests(false);
            resetElapsedTime(lastUpdateTime); // exclude UI update time [bug #52]
        }

        // exit conditions
        if (untilmodule_reached) {
            stoppingReason = STOP_UNTILMODULE;
            break;
        }
        if (stopSimulationFlag) {
            stoppingReason = STOP_STOPCOMMAND;
            break;
        }
        if (runUntil.simTime > 0 && simulation.guessNextSimtime() >= runUntil.simTime) {
            stoppingReason = STOP_UNTILSIMTIME;
            break;
        }
        if (runUntil.eventNumber > 0 && simulation.getEventNumber() >= runUntil.eventNumber) {
            stoppingReason = STOP_UNTILEVENT;
            break;
        }

        checkTimeLimits();
    }
    return false;
}

bool Cmdenv::doRunSimulationExpress()
{
    //
    // IMPORTANT:
    // The following variables may change during execution (as a result of user interaction
    // during Tcl_Eval("update"):
    //  - runmode, runUntil.simTime, runUntil.eventNumber, runUntil.msg, runUntil.module;
    //  - stopsimulation_flag
    //  - opt->expressmode_autoupdate
    //
    // EXPRESS does not support runUntil.module!
    //

    // update, just to get the above notice displayed
//    Tcl_Eval(interp, "update");

    if (runUntil.module)
        throw cRuntimeError("Express mode does not support run-until-module");

    // OK, let's begin
    Speedometer speedometer;
    speedometer.start(simulation.getSimTime());
    disable_tracing = true;

    struct timeval lastUpdateTime;
    gettimeofday(&lastUpdateTime, NULL);

    while (true)
    {
        cEvent *event = simulation.takeNextEvent();
        if (!event)
            break; // scheduler interrupted (parsim)

        // "run until message": stop if desired event was reached
        if (runUntil.msg && event==runUntil.msg) {
            simulation.putBackEvent(event);
            stoppingReason = STOP_UNTILMESSAGE;
            break;
        }

        speedometer.addEvent(simulation.getSimTime());

        simulation.executeEvent(event);

        if ((simulation.getEventNumber()&0xff)==0 && elapsed(opt->updatefreqExpress, lastUpdateTime))
        {
            if (runUntil.hasRealTimeLimit) {
                struct timeval t;
                gettimeofday(&t, NULL);
                if (timeval_greater(t, runUntil.realTime)) {
                    stoppingReason = STOP_REALTIMECHUNK;
                    break;
                }
            }

            if (speedometer.getMillisSinceIntervalStart() > SPEEDOMETER_UPDATEMILLISECS)
            {
                speedometer.beginNewInterval();
//                updatePerformanceDisplay(speedometer);
            }
//            Tcl_Eval(interp, "update");
            processHttpRequests(false);
            resetElapsedTime(lastUpdateTime); // exclude UI update time [bug #52]
            if (runMode != RUNMODE_EXPRESS)
                return true;  // should continue, but in a different mode
        }

        // exit conditions
        if (stopSimulationFlag) {
            stoppingReason = STOP_STOPCOMMAND;
            break;
        }
        if (runUntil.simTime > 0 && simulation.guessNextSimtime() >= runUntil.simTime) {
            stoppingReason = STOP_UNTILSIMTIME;
            break;
        }
        if (runUntil.eventNumber > 0 && simulation.getEventNumber() >= runUntil.eventNumber) {
            stoppingReason = STOP_UNTILEVENT;
            break;
        }

        checkTimeLimits();
    }
    return false;
}

void Cmdenv::finishSimulation()
{
//    // strictly speaking, we shouldn't allow callFinish() after SIM_ERROR, but it comes handy in practice...
//    ASSERT(state==SIM_READY || state==SIM_TERMINATED || state==SIM_ERROR);
//
//    if (state == SIM_READY)
//    {
//        cTerminationException e("The user has finished the simulation");
//        stoppedWithTerminationException(e);
//    }
//
//    logBuffer.addInfo("{** Calling finish() methods of modules\n}");
//    printLastLogLine();

    // now really call finish()
    try
    {
        simulation.callFinish();
        cLogProxy::flushLastLine();

        checkFingerprint();
    }
    catch (std::exception& e)
    {
        stoppedWithException(e);
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
    }

    // then endrun
    try
    {
        endRun();
    }
    catch (std::exception& e)
    {
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
    }
    state = SIM_FINISHCALLED;
}
#endif

void Cmdenv::objectDeleted(cObject *obj)
{
    EnvirBase::objectDeleted(obj);
#ifdef SIMFRONTEND_SUPPORT
    if (serializer)
        serializer->objectDeleted(obj);
#endif
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

                //TODO probably not after each event!
                while (httpServer && httpServer->handleOneRequest(false))
                    ;
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

                //TODO probably not after each event!
                while (httpServer && httpServer->handleOneRequest(false))
                    ;
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
#ifdef SIMFRONTEND_SUPPORT
    return opt->httpControlled;
#else
    return false;
#endif
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

#ifdef SIMFRONTEND_SUPPORT
    if (opt->httpControlled)
    {
        JsonObject *details = new JsonObject();
        details->put("message", jsonWrap(s));
        getUserInput(INP_MSGDIALOG, details);
    }
#endif
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

#ifdef SIMFRONTEND_SUPPORT
    if (collectJsonLog)
    {
        std::string prefix = logFormatter.formatPrefix(entry);  //TODO reuse previously formatted prefix...
        JsonObject *jentry = new JsonObject();
        jentry->put("@", jsonWrap("L"));
        jentry->put("txt", jsonWrap(prefix + std::string(entry->text, entry->textLength)));
        jsonLog->push_back(jentry);
    }
#endif
}

std::string Cmdenv::gets(const char *prompt, const char *defaultReply)
{
    if (!opt->interactive)
        throw cRuntimeError("The simulation wanted to ask a question, set cmdenv-interactive=true to allow it: \"%s\"", prompt);

    ::fprintf(fout, "%s", prompt);
    if (!opp_isempty(defaultReply))
        ::fprintf(fout, "(default: %s) ", defaultReply);
    ::fflush(fout);

#ifdef SIMFRONTEND_SUPPORT
    if (opt->httpControlled)
    {
        JsonObject *details = new JsonObject();
        details->put("prompt", jsonWrap(prompt));
        details->put("default", jsonWrap(defaultReply));
        std::string reply = getUserInput(INP_GETS, details);
        ::fprintf(fout, "%s\n", reply.c_str());
        ::fflush(fout);
        return reply;
    }
    else
#endif
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

#ifdef SIMFRONTEND_SUPPORT
    if (opt->httpControlled)
    {
        ::fprintf(fout, "%s (y/n) ", question);
        ::fflush(fout);
        JsonObject *details = new JsonObject();
        details->put("message", jsonWrap(question));
        std::string reply = getUserInput(INP_ASKYESNO, details);
        ::fprintf(fout, "%s\n", reply.c_str());
        ::fflush(fout);
        return reply == "y";
    }
    else
#endif
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

#ifdef SIMFRONTEND_SUPPORT
std::string Cmdenv::getUserInput(UserInputType type, JsonObject *details)
{
    debug("[cmdenv] entering getUserInput(type='%s')\n", userInputType->getStringFor(type));
    ASSERT(userInput.state == INPSTATE_NONE);
    userInput.type = type;
    userInput.request = details;
    userInput.state = INPSTATE_INITIATED;

    // wait until HTTP communication takes place
    while (userInput.state != INPSTATE_REPLYARRIVED)
        httpServer->handleOneRequest(true);
    ASSERT(command == CMD_NONE); // commands not allowed while user input is in progress

    // done
    userInput.state = INPSTATE_NONE;
    if (userInput.cancel)
        throw cRuntimeError(E_CANCEL);
    debug("[cmdenv] leaving getUserInput(), reply=\"%s\"\n", userInput.reply.c_str());
    return userInput.reply;
}
#endif

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
             simulation.getEventNumber(),
#ifdef SIMFRONTEND_SUPPORT
             stateEnum->getStringFor(state)
#else
             ""
#endif
             );
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

#ifdef SIMFRONTEND_SUPPORT
        if (collectJsonLog && event->isMessage())  //TODO also record non-message events
        {
            cMessage *msg = static_cast<cMessage*>(event);
            JsonObject *entry = new JsonObject();
            entry->put("@", jsonWrap("E"));
            entry->put("#", jsonWrap(simulation.getEventNumber()));
            entry->put("t", jsonWrap(simulation.getSimTime()));
            cModule *mod = msg->getArrivalModule();
            entry->put("moduleId", jsonWrap(mod->getId()));
            entry->put("moduleFullPath", jsonWrap(mod->getFullPath()));
            entry->put("moduleNedType", jsonWrap(mod->getNedTypeName()));
            entry->put("messageId", jsonWrap(msg->getId()));
            entry->put("messageName", jsonWrap((std::string)msg->getName()));
            entry->put("messageClassName", jsonWrap(msg->getClassName()));
            jsonLog->push_back(entry);
        }
#endif
    }
}

void Cmdenv::bubble(cComponent *component, const char *text)
{
    EnvirBase::bubble(component, text);
}

void Cmdenv::beginSend(cMessage *msg)
{
    EnvirBase::beginSend(msg);

#ifdef SIMFRONTEND_SUPPORT
    if (!disable_tracing && collectJsonLog)
    {
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("BS"));
        entry->put("msg", jsonWrapObjectId(msg));
        jsonLog->push_back(entry);
    }
#endif
}

void Cmdenv::messageScheduled(cMessage *msg)
{
    EnvirBase::messageScheduled(msg);
}

void Cmdenv::messageCancelled(cMessage *msg)
{
    EnvirBase::messageCancelled(msg);
}

void Cmdenv::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    EnvirBase::messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);

#ifdef SIMFRONTEND_SUPPORT
    if (!disable_tracing && collectJsonLog)
    {
        cModule *srcModule = msg->getSenderModule();

        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("SD"));  // note: msg is already in BeginSend
        entry->put("srcModule", jsonWrapObjectId(srcModule));
        entry->put("destGate", jsonWrapObjectId(toGate));
        entry->put("propagationDelay", jsonWrap(propagationDelay));
        entry->put("transmissionDelay", jsonWrap(transmissionDelay));
        jsonLog->push_back(entry);
    }
#endif
}

void Cmdenv::messageSendHop(cMessage *msg, cGate *srcGate)
{
    EnvirBase::messageSendHop(msg, srcGate);

#ifdef SIMFRONTEND_SUPPORT
    if (!disable_tracing && collectJsonLog)
    {
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("SH"));  // note: msg is already in BeginSend
        entry->put("srcGate", jsonWrapObjectId(srcGate));
        jsonLog->push_back(entry);
    }
#endif
}

void Cmdenv::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    EnvirBase::messageSendHop(msg, srcGate, propagationDelay, transmissionDelay);

#ifdef SIMFRONTEND_SUPPORT
    if (!disable_tracing && collectJsonLog) //TODO we won't animate in fast mode, so BS/SH/ES entries won't be needed then!
    {
        // Note: the link or even the gate may be deleted by the time the client receives
        // this JSON info (so there won't be enough info for animation), but we ignore
        // that rare case to keep the implementation simple
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("SH"));  // note: msg is already in BeginSend
        entry->put("srcGate", jsonWrapObjectId(srcGate));
        entry->put("propagationDelay", jsonWrap(propagationDelay));
        entry->put("transmissionDelay", jsonWrap(transmissionDelay));
        jsonLog->push_back(entry);
    }
#endif
}

void Cmdenv::endSend(cMessage *msg)
{
    EnvirBase::endSend(msg);

#ifdef SIMFRONTEND_SUPPORT
    if (!disable_tracing && collectJsonLog)
    {
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("ES"));  // note: msg is already in BeginSend
        jsonLog->push_back(entry);
    }
#endif
}

void Cmdenv::messageCreated(cMessage *msg)
{
    EnvirBase::messageCreated(msg);
}

void Cmdenv::messageCloned(cMessage *msg, cMessage *clone)
{
    EnvirBase::messageCloned(msg, clone);
}

void Cmdenv::messageDeleted(cMessage *msg)
{
    EnvirBase::messageDeleted(msg);
}

void Cmdenv::componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent)
{
    //FIXME "from" is NULL if the cross-module call is from a module destructor! maybe add context switching into cModule destructor
    va_list va2;
    va_copy(va2, va);
    EnvirBase::componentMethodBegin(from, to, methodFmt, va2, silent);

#ifdef SIMFRONTEND_SUPPORT
    if (!disable_tracing && collectJsonLog)
    {
        const char *methodText = "";  // for the Enter_Method_Silent case
        if (methodFmt) {
            static char methodTextBuf[MAX_METHODCALL];
            vsnprintf(methodTextBuf, MAX_METHODCALL, methodFmt, va);
            methodTextBuf[MAX_METHODCALL-1] = '\0';
            methodText = methodTextBuf;
        }
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("MB"));
        cModule *fromModule = dynamic_cast<cModule *>(from);
        entry->put("sm", jsonWrap(fromModule ? fromModule->getId() : 0));  //FIXME may be a channel too!!!
        cModule *toModule = dynamic_cast<cModule *>(to);
        entry->put("tm", jsonWrap(toModule ? toModule->getId() : 0));
        entry->put("m", jsonWrap(methodText));
        jsonLog->push_back(entry);
    }
#endif
}

void Cmdenv::componentMethodEnd()
{
    EnvirBase::componentMethodEnd();

#ifdef SIMFRONTEND_SUPPORT
    if (!disable_tracing && collectJsonLog)
    {
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("ME"));
        jsonLog->push_back(entry);
    }
#endif
}

void Cmdenv::moduleDeleted(cModule *module)
{
    EnvirBase::moduleDeleted(module);
}

void Cmdenv::moduleReparented(cModule *module, cModule *oldparent, int oldId)
{
    EnvirBase::moduleReparented(module, oldparent, oldId);
}

void Cmdenv::gateCreated(cGate *newgate)
{
    EnvirBase::gateCreated(newgate);
}

void Cmdenv::gateDeleted(cGate *gate)
{
    EnvirBase::gateDeleted(gate);
}

void Cmdenv::connectionCreated(cGate *srcgate)
{
    EnvirBase::connectionCreated(srcgate);
}

void Cmdenv::connectionDeleted(cGate *srcgate)
{
    EnvirBase::connectionDeleted(srcgate);
}

void Cmdenv::displayStringChanged(cComponent *component)
{
    EnvirBase::displayStringChanged(component);
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
#ifdef SIMFRONTEND_SUPPORT
    std::cout << "  -w            Wait for commands over HTTP instead of starting a simulation\n";
    std::cout << "                interactively. Overrides -c, -r, -a, -x, -g, -G, -X options.\n";
#endif
}

unsigned Cmdenv::getExtraStackForEnvir() const
{
    return opt->extraStack;
}

NAMESPACE_END

