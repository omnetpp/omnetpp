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
#include "ccompoundmodule.h"
#include "cstatistic.h"
#include "cdensityestbase.h"
#include "cwatch.h"
#include "cdisplaystring.h"


USING_NAMESPACE

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

Register_PerObjectConfigOption(CFGID_CMDENV_EV_OUTPUT, "cmdenv-ev-output", KIND_MODULE, CFG_BOOL, "true", "When cmdenv-express-mode=false: whether Cmdenv should print debug messages (ev<<) from the selected modules.")


/*TODO:
notifyListeners(LF_ON_SIMULATION_START);
notifyListeners(LF_ON_SIMULATION_SUCCESS);
notifyListeners(LF_ON_SIMULATION_ERROR);
*/

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

static cEnum stateEnum("State",
        "nonetwork", Cmdenv::SIM_NONETWORK,
        "ready", Cmdenv::SIM_READY,
        "running", Cmdenv::SIM_RUNNING,
        "terminated", Cmdenv::SIM_TERMINATED,
        "error", Cmdenv::SIM_ERROR,
        "finishcalled", Cmdenv::SIM_FINISHCALLED,
        NULL);

static cEnum runModeEnum("RunMode",
        "normal", Cmdenv::RUNMODE_NORMAL,
        "fast", Cmdenv::RUNMODE_FAST,
        "express", Cmdenv::RUNMODE_EXPRESS,
        NULL);


Cmdenv::Cmdenv()
{
    // Note: ctor should only contain trivial initializations, because
    // the class may be instantiated only for the purpose of calling
    // printUISpecificHelp() on it

    // initialize fout to stdout, then we'll replace it if redirection is
    // requested in the ini file
    fout = stdout;

    // init config variables that are used even before readOptions()
    opt_autoflush = true;

    opt_updatefreq_fast = 500;
    opt_updatefreq_express = 1000;

    isConfigRun = false; //TODO check all other new vars too

    lastId = 0;

    errorInfo.isValid = false;
    askParamInfo.isValid = false;

    collectJsonLog = true; //FIXME
    jsonLog = new JsonArray();
}

Cmdenv::~Cmdenv()
{
    delete jsonLog;
}

void Cmdenv::readOptions()
{
    EnvirBase::readOptions();

    cConfiguration *cfg = getConfig();

    // note: configname and runstoexec will possibly be overwritten
    // with the -c, -r command-line options in our setup() method
    opt_configname = cfg->getAsString(CFGID_CONFIG_NAME);
    opt_runstoexec = cfg->getAsString(CFGID_RUNS_TO_EXECUTE);

    opt_extrastack = (size_t) cfg->getAsDouble(CFGID_CMDENV_EXTRA_STACK);
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

void Cmdenv::readPerRunOptions()
{
    EnvirBase::readPerRunOptions();

    cConfiguration *cfg = getConfig();
    opt_expressmode = cfg->getAsBool(CFGID_EXPRESS_MODE);
    opt_interactive = cfg->getAsBool(CFGID_CMDENV_INTERACTIVE);
    opt_autoflush = cfg->getAsBool(CFGID_AUTOFLUSH);
    opt_modulemsgs = cfg->getAsBool(CFGID_MODULE_MESSAGES);
    opt_eventbanners = cfg->getAsBool(CFGID_EVENT_BANNERS);
    opt_eventbanner_details = cfg->getAsBool(CFGID_EVENT_BANNER_DETAILS);
    opt_messagetrace = cfg->getAsBool(CFGID_MESSAGE_TRACE);
    opt_status_frequency_ms = 1000*cfg->getAsDouble(CFGID_STATUS_FREQUENCY);
    opt_perfdisplay = cfg->getAsBool(CFGID_PERFORMANCE_DISPLAY);
}

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

void Cmdenv::run()
{
    state = SIM_NONETWORK; //TODO into ctor
    command = CMD_NONE;

    httpServer->addHttpRequestHandler(this); //FIXME should go into setup()?

    if (args->optionGiven('w'))
    {
        // interactive mode: wait for commands
        processHttpRequests(true);
    }
    else
    {
        // '-c' and '-r' option: configuration to activate, and run numbers to run.
        // Both command-line options take precedence over inifile settings.
        // (NOTE: inifile settings *already* got read at this point! as EnvirBase::setup()
        // invokes readOptions()).

        const char *configname = args->optionValue('c');
        if (configname)
            opt_configname = configname;
        if (opt_configname.empty())
            opt_configname = "General";

        const char *runstoexec = args->optionValue('r');
        if (runstoexec)
            opt_runstoexec = runstoexec;

        // if the list of runs is not given explicitly, must execute all runs
        if (opt_runstoexec.empty())
        {
            int n = cfg->getNumRunsInConfig(opt_configname.c_str());  //note: may throw exception
            if (n==0) {
                ev.printfmsg("Error: Configuration `%s' generates 0 runs", opt_configname.c_str());
                exitcode = 1;
                return;
            }

            char buf[32];
            sprintf(buf, (n==0 ? "" : n==1 ? "%d" : "0..%d"), n-1);
            opt_runstoexec = buf;
        }

        EnumStringIterator runiterator(opt_runstoexec.c_str());
        if (runiterator.hasError())
        {
            ev.printfmsg("Error parsing list of runs to execute: `%s'", opt_runstoexec.c_str());
            exitcode = 1;
            return;
        }

        // we'll return nonzero exitcode if any run was terminated with error
        bool hadError = false;

        for (; runiterator()!=-1; runiterator++)
        {
            int runnumber = runiterator();
            bool networkSetupDone = false;
            bool startrun_done = false;
            try
            {
                ::fprintf(fout, "\nPreparing for running configuration %s, run #%d...\n", opt_configname.c_str(), runnumber);
                ::fflush(fout);

                cfg->activateConfig(opt_configname.c_str(), runnumber);

                const char *itervars = cfg->getVariable(CFGVAR_ITERATIONVARS2);
                if (itervars && strlen(itervars)>0)
                    ::fprintf(fout, "Scenario: %s\n", itervars);
                ::fprintf(fout, "Assigned runID=%s\n", cfg->getVariable(CFGVAR_RUNID));

                //cfg->dump();

                readPerRunOptions();

                // find network
                cModuleType *network = resolveNetwork(opt_network_name.c_str());
                ASSERT(network);

                // set up network
                ::fprintf(fout, "Setting up network `%s'...\n", opt_network_name.c_str());
                ::fflush(fout);

                setupNetwork(network);
                networkSetupDone = true;

                // prepare for simulation run
                ::fprintf(fout, "Initializing...\n");
                ::fflush(fout);

                disable_tracing = opt_expressmode;
                startRun();
                startrun_done = true;

                // run the simulation
                ::fprintf(fout, "\nRunning simulation...\n");
                ::fflush(fout);

                // simulate() should only throw exception if error occurred and
                // finish() should not be called.
                notifyListeners(LF_ON_SIMULATION_START);
                simulate();
                disable_tracing = false;

                ::fprintf(fout, "\nCalling finish() at end of Run #%d...\n", runnumber);
                ::fflush(fout);
                simulation.callFinish();
                flushLastLine();

                checkFingerprint();

                notifyListeners(LF_ON_SIMULATION_SUCCESS);
            }
            catch (std::exception& e)
            {
                hadError = true;
                disable_tracing = false;
                stoppedWithException(e);
                notifyListeners(LF_ON_SIMULATION_ERROR);
                displayException(e);
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
                    hadError = true;
                    notifyListeners(LF_ON_SIMULATION_ERROR);
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
                    notifyListeners(LF_ON_SIMULATION_ERROR);
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

void Cmdenv::processHttpRequests(bool blocking)
{
    //FIXME observe "blocking" flag!
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

        ::printf("[cmdenv] command=%d\n", currentCommand);
        if (currentCommand == CMD_QUIT) {
            return; //XXX set some "should_exit" flag like Tkenv?
        }
        else if (currentCommand == CMD_SETUPNETWORK) {
            const char *networkName = commandArgs["network"].c_str();
            newNetwork(networkName);
        }
        else if (currentCommand == CMD_SETUPRUN) {
            const char *configName = commandArgs["config"].c_str();
            int runNumber = atoi(commandArgs["run"].c_str());
            newRun(configName, runNumber);
        }
        else if (currentCommand == CMD_REBUILD) {
            rebuildSim();
        }
        else if (currentCommand == CMD_STEP) {
            doOneStep();
        }
        else if (currentCommand == CMD_RUN) {
            //TODO exception handling!
            RunMode mode = (RunMode)runModeEnum.lookup(commandArgs["mode"].c_str(), RUNMODE_NONE);
            simtime_t untilTime = commandArgs["utime"]=="" ? 0 : STR_SIMTIME(commandArgs["utime"].c_str());
            eventnumber_t untilEventnum = commandArgs["uevent"]=="" ? 0 : opp_atol(commandArgs["uevent"].c_str());
            long realTimeMillis = commandArgs["rtlimit"]=="" ? 0 : opp_atof(commandArgs["rtlimit"].c_str());
            cMessage *untilMsg = commandArgs["umsg"]=="" ? NULL : check_and_cast<cMessage*>(getObjectById(opp_atol(commandArgs["umsg"].c_str())));
            cModule *untilModule = commandArgs["umod"]=="" ? NULL : check_and_cast<cModule*>(getObjectById(opp_atol(commandArgs["umod"].c_str())));
            runSimulation(mode, untilTime, untilEventnum, realTimeMillis, untilMsg, untilModule);
        }
        else if (currentCommand == CMD_STOP) {
            setStopSimulationFlag();
        }
        else if (currentCommand == CMD_FINISH) {
            finishSimulation();
        }
        else {
            ASSERT(false);
        }
    }
}

bool Cmdenv::handle(cHttpRequest *request)
{
    //TODO: rewrite HttpServer so it directly uses mongoose's url-based callback registration...
    if (strcmp(request->getRequestMethod(), "GET") != 0 || strncmp(request->getUri(), "/sim", 4) !=0)
        return false;

    //TODO: parse request args into std::map, etc
    const char *uri = request->getUri();
    const char *query = request->getQueryString();
    ASSERT2(command == CMD_NONE, "previous command not marked as done yet");

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

    const char *OK_STATUS = "HTTP/1.0 200 OK\n";

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
    else if (strcmp(uri, "/sim/status") == 0) {
        request->print(OK_STATUS);
        request->print("\n");

        JsonObject *result = new JsonObject();
        result->put("hostname", jsonWrap(opp_gethostname()));
        result->put("processid", jsonWrap(getpid()));
        result->put("state", jsonWrap(stateEnum.getStringFor(state)));
        result->put("eventlogfile", jsonWrap(toAbsolutePath(eventlogmgr->getFileName())));
        if (state != SIM_NONETWORK) {
            ASSERT(simulation.getSystemModule());
            cConfigurationEx *cfg = getConfigEx();
            result->put("config", jsonWrap(cfg->getActiveConfigName()));
            result->put("run", jsonWrap(cfg->getActiveRunNumber()));
            result->put("network", jsonWrap(simulation.getNetworkType()->getName()));
            // TODO: eventnumber is immediately incremented after handling a message
            result->put("eventNumber", jsonWrap(simulation.getEventNumber() - 1));
            result->put("simTime", jsonWrap(simTime()));
            result->put("nextEventNumber", jsonWrap(simulation.getEventNumber()));
            result->put("nextEventSimTime", jsonWrap(simulation.guessNextSimtime()));
            if (simulation.guessNextModule())
                result["nextEventModuleId"] = jsonWrap(simulation.guessNextModule()->getId());
            cEvent *guessNextEvent = simulation.guessNextEvent();
            if (guessNextEvent && guessNextEvent->isMessage())
                result->put("nextEventMessageId", jsonWrap(static_cast<cMessage*>(guessNextEvent)->getId()));
        }

        if (collectJsonLog) {
            result->put("log", jsonLog);
            jsonLog = new JsonArray();  // previous one is now owned by result
        }

        if (errorInfo.isValid) {
            JsonObject *info = new JsonObject();
            info->put("message", jsonWrap(errorInfo.message));
            result->put("errorInfo", info);
            errorInfo.isValid = false;
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
            for (int j = 0; (int)j < baseConfigs.size(); j++)
                jbaseConfigs->push_back(jsonWrap(baseConfigs[j]));
            jconfig->put("extends", jbaseConfigs);
            result->push_back(jconfig);
        }

        std::stringstream ss;
        result->printOn(ss);
        delete result;
        request->print(ss.str().c_str());
    }
    else if (strcmp(uri, "/sim/getRootObjectIds") == 0) {
        request->print(OK_STATUS);
        request->print("\n");

        JsonObject *result = new JsonObject();
        result->put("simulation", jsonWrap(getIdStringForObject(&simulation)));
        result->put("fes", jsonWrap(getIdStringForObject(&simulation.msgQueue)));
        result->put("systemModule", jsonWrap(getIdStringForObject(simulation.getSystemModule())));
        result->put("defaultList", jsonWrap(getIdStringForObject(&defaultList)));
        result->put("componentTypes", jsonWrap(getIdStringForObject(componentTypes.getInstance())));
        result->put("nedFunctions", jsonWrap(getIdStringForObject(nedFunctions.getInstance())));
        result->put("classes", jsonWrap(getIdStringForObject(classes.getInstance())));
        result->put("enums", jsonWrap(getIdStringForObject(enums.getInstance())));
        result->put("classDescriptors", jsonWrap(getIdStringForObject(classDescriptors.getInstance())));
        result->put("configOptions", jsonWrap(getIdStringForObject(configOptions.getInstance())));
        result->put("resultFilters", jsonWrap(getIdStringForObject(resultFilters.getInstance())));
        result->put("resultRecorders", jsonWrap(getIdStringForObject(resultRecorders.getInstance())));

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
        JsonObject2 *result = new JsonObject2();
        StringTokenizer tokenizer(ids.c_str(), ",");
        while (tokenizer.hasMoreTokens())
        {
             std::string objectId = tokenizer.nextToken();
             cObject *obj = getObjectById(atol(objectId.c_str()));
             if (obj)
             {
                 JsonObject *jfields = new JsonObject();
                 if (what=="" || what.find('i') != std::string::npos) {  // "info"
                     jfields->put("name", jsonWrap(obj->getName()));
                     jfields->put("fullName", jsonWrap(obj->getFullName()));
                     jfields->put("fullPath", jsonWrap(obj->getFullPath()));
                     jfields->put("className", jsonWrap(obj->getClassName()));
                     jfields->put("knownBaseClass", jsonWrap(getKnownBaseClass(obj)));
                     jfields->put("info", jsonWrap(obj->info()));
                     jfields->put("owner", jsonWrap(getIdStringForObject(obj->getOwner())));
                     cClassDescriptor *desc = obj->getDescriptor();
                     const char *icon = desc ? desc->getProperty("icon") : NULL;
                     jfields->put("icon", jsonWrap(icon ? icon : ""));

                     // cComponent
                     if (dynamic_cast<cComponent *>(obj)) {
                         cComponent *component = (cComponent *)obj;

                         jfields->put("parentModule", jsonWrap(getIdStringForObject(component->getParentModule())));
                         jfields->put("nedTypeName", jsonWrap(component->getNedTypeName()));
                         jfields->put("displayString", jsonWrap(component->getDisplayString().str()));

                         JsonArray *jparameters = new JsonArray();
                         for (int i = 0; i < component->getNumParams(); i++) {
                             cPar *par = &component->par(i);
                             jparameters->push_back(jsonWrap(getIdStringForObject(par)));
                         }
                         jfields->put("parameters", jparameters);
                     }

                     // cModule
                     if (dynamic_cast<cModule *>(obj)) {
                         cModule *mod = (cModule *)obj;
                         jfields->put("moduleId", jsonWrap(mod->getId()));
                         jfields->put("index", jsonWrap(mod->getIndex()));
                         jfields->put("vectorSize", jsonWrap(mod->isVector() ? mod->getVectorSize() : -1));

                         JsonArray *jgates = new JsonArray();
                         for (cModule::GateIterator i(mod); !i.end(); i++) {
                             cGate *gate = i();
                             jgates->push_back(jsonWrap(getIdStringForObject(gate)));
                         }
                         jfields->put("gates", jgates);

                         JsonArray *jsubmodules = new JsonArray();
                         for (cModule::SubmoduleIterator i(mod); !i.end(); i++) {
                             cModule *submod = i();
                             jsubmodules->push_back(jsonWrap(getIdStringForObject(submod)));
                         }
                         jfields->put("submodules", jsubmodules);
                     }

                     // cGate
                     if (dynamic_cast<cGate *>(obj)) {
                         cGate *gate = (cGate *)obj;
                         jfields->put("type", jsonWrap(cGate::getTypeName(gate->getType())));
                         jfields->put("gateId", jsonWrap(gate->getId()));
                         jfields->put("index", jsonWrap(gate->getIndex()));
                         jfields->put("vectorSize", jsonWrap(gate->isVector() ? gate->getVectorSize() : -1));
                         jfields->put("ownerModule", jsonWrap(getIdStringForObject(gate->getOwnerModule())));
                         jfields->put("nextGate", jsonWrap(getIdStringForObject(gate->getNextGate())));
                         jfields->put("previousGate", jsonWrap(getIdStringForObject(gate->getPreviousGate())));
                         jfields->put("channel", jsonWrap(getIdStringForObject(gate->getChannel())));
                     }

                     // cChannel
                     if (dynamic_cast<cChannel *>(obj)) {
                         cChannel *channel = (cChannel *)obj;
                         jfields->put("isTransmissionChannel", jsonWrap(channel->isTransmissionChannel()));
                         jfields->put("sourceGate", jsonWrap(getIdStringForObject(channel->getSourceGate())));
                     }

                     // cMessage
                     if (dynamic_cast<cMessage *>(obj)) {
                         cMessage *msg = (cMessage *)obj;
                         jfields->put("kind", jsonWrap(msg->getKind()));
                         jfields->put("priority", jsonWrap(msg->getSchedulingPriority()));
                         jfields->put("id", jsonWrap(msg->getId()));
                         jfields->put("treeId", jsonWrap(msg->getTreeId()));
                         jfields->put("senderModule", jsonWrap(getIdStringForObject(msg->getSenderModule())));
                         jfields->put("senderGate", jsonWrap(getIdStringForObject(msg->getSenderGate())));
                         jfields->put("arrivalModule", jsonWrap(getIdStringForObject(msg->getArrivalModule())));
                         jfields->put("arrivalGate", jsonWrap(getIdStringForObject(msg->getArrivalGate())));
                         jfields->put("sendingTime", jsonWrap(msg->getSendingTime()));
                         jfields->put("arrivalTime", jsonWrap(msg->getArrivalTime()));
                     }

                     // cPacket
                     if (dynamic_cast<cPacket *>(obj)) {
                         cPacket *msg = (cPacket *)obj;
                         jfields->put("bitLength", jsonWrap(msg->getBitLength()));
                         jfields->put("bitError", jsonWrap(msg->hasBitError()));
                         jfields->put("duration", jsonWrap(msg->getDuration()));
                         jfields->put("isReceptionStart", jsonWrap(msg->isReceptionStart()));
                         jfields->put("encapsulatedPacket", jsonWrap(getIdStringForObject(msg->getEncapsulatedPacket())));
                         jfields->put("encapsulationId", jsonWrap(msg->getEncapsulationId()));
                         jfields->put("encapsulationTreeId", jsonWrap(msg->getEncapsulationTreeId()));
                     }

                 }
                 if (what=="" || what.find('c') != std::string::npos) {  // "children"
                     cCollectChildrenVisitor visitor(obj);
                     visitor.process(obj);

                     JsonArray *jchildren = new JsonArray();
                     for (int i = 0; i < visitor.getArraySize(); i++)
                         jchildren->push_back(jsonWrap(getIdStringForObject(visitor.getArray()[i])));
                     jfields->put("children", jchildren);
                 }
                 if (what=="" || what.find('d') != std::string::npos) {  // "details"
                     cClassDescriptor *desc = obj->getDescriptor();
                     if (desc)
                     {
                         JsonArray *jdetailFields = new JsonArray();
                         //TODO serialize baseclass, properties, etc too
                         for (int fld = 0; fld < desc->getFieldCount(obj); fld++)
                         {
                             JsonObject *jdetailField = new JsonObject();
                             jdetailField->put("name", jsonWrap(desc->getFieldName(obj, fld)));
                             jdetailField->put("type", jsonWrap(desc->getFieldTypeString(obj, fld)));
                             jdetailField->put("declaredOn", jsonWrap(desc->getFieldDeclaredOn(obj, fld)));
                             const char *property = desc->getFieldProperty(obj, fld, "group");
                             if (property)
                                 jdetailField->put("@group", jsonWrap(property));
                             property = desc->getFieldProperty(obj, fld, "label");
                             if (property)
                                 jdetailField->put("@label", jsonWrap(property));
                             property = desc->getFieldProperty(obj, fld, "hint");
                             if (property)
                                 jdetailField->put("@hint", jsonWrap(property));
                             property = desc->getFieldProperty(obj, fld, "enum");
                             if (property)
                                 jdetailField->put("@enum", jsonWrap(property));
                             //TODO: merge this into a single "flags" field!
                             if (desc->getFieldIsArray(obj, fld))
                                 jdetailField->put("isArray", jsonWrap(true));
                             if (desc->getFieldIsCompound(obj, fld))
                                 jdetailField->put("isCompound", jsonWrap(true));
                             if (desc->getFieldIsPointer(obj, fld))
                                 jdetailField->put("isPointer", jsonWrap(true));
                             if (desc->getFieldIsCObject(obj, fld))
                                 jdetailField->put("isCObject", jsonWrap(true));
                             if (desc->getFieldIsCOwnedObject(obj, fld))
                                 jdetailField->put("isCOwnedObject", jsonWrap(true));
                             if (desc->getFieldIsEditable(obj, fld))
                                 jdetailField->put("isEditable", jsonWrap(true));
                             if (desc->getFieldStructName(obj, fld))
                                 jdetailField->put("structName", jsonWrap(desc->getFieldStructName(obj, fld)));
                             //TODO send symbolic names if @enum() exists!!!
                             //TODO virtual void *getFieldStructPointer(void *object, int field, int i) const = 0;
                             if (!desc->getFieldIsArray(obj, fld)) {
                                 if (!desc->getFieldIsCompound(obj, fld))
                                     jdetailField->put("value", jsonWrap(desc->getFieldAsString(obj, fld, 0)));
                                 else {
                                     void *ptr = desc->getFieldStructPointer(obj, fld, 0);
                                     if (desc->getFieldIsCObject(obj, fld)) {
                                         cObject *o = (cObject *)ptr;
                                         jdetailField->put("value", jsonWrap(getIdStringForObject(o)));
                                     }
                                     else {
                                         jdetailField->put("value", jsonWrap("...some struct...")); //XXX look up using getFieldStructName() -- recursive!
                                     }
                                 }
                             } else {
                                 int n = desc->getFieldArraySize(obj, fld);
                                 JsonArray *jValues = new JsonArray();
                                 for (int i = 0; i < n; i++) {
                                     if (!desc->getFieldIsCompound(obj, fld))
                                         jValues->push_back(jsonWrap(desc->getFieldAsString(obj, fld, i)));
                                     else {
                                         void *ptr = desc->getFieldStructPointer(obj, fld, i);
                                         if (desc->getFieldIsCObject(obj, fld)) {
                                             cObject *o = (cObject *)ptr;
                                             jValues->push_back(jsonWrap(getIdStringForObject(o)));
                                         }
                                         else {
                                             jValues->push_back(jsonWrap("...some struct...")); //XXX look up using getFieldStructName() -- recursive!
                                         }
                                     }
                                 }
                                 jdetailField->put("value", jValues);
                             }
                             jdetailFields->push_back(jdetailField);
                         }
                         jfields->put("fields", jdetailFields);
                     }

                 }
                 result->put(objectId, jfields);
             }
        }

        double consumedCPU = (clock() - startTime) / (double)CLOCKS_PER_SEC;
        ::printf("[http] json tree assembly took %lgs\n", consumedCPU);

        startTime = clock();

        std::stringstream ss;
        result->printOn(ss);
        delete result;

        consumedCPU = (clock() - startTime) / (double)CLOCKS_PER_SEC;
        ::printf("[http] json tree serialization took %lgs\n", consumedCPU);

        request->print(ss.str().c_str());
    }
    else {
        return false;
    }

    return true; // handled
}

const char *Cmdenv::getKnownBaseClass(cObject *object)
{
    if (dynamic_cast<cModule *>(object) && ((cModule *)object)->isPlaceholder())
        return"cPlaceholderModule";
    else if (dynamic_cast<cSimpleModule *>(object))
        return "cSimpleModule";
    else if (dynamic_cast<cCompoundModule *>(object))
        return "cCompoundModule";
    else if (dynamic_cast<cPacket *>(object))
        return "cPacket";
    else if (dynamic_cast<cMessage *>(object))
        return "cMessage";
    else if (dynamic_cast<cArray *>(object))
        return "cArray";
    else if (dynamic_cast<cQueue *>(object))
        return "cQueue";
    else if (dynamic_cast<cGate *>(object))
        return "cGate";
    else if (dynamic_cast<cPar *>(object))
        return "cPar";
    else if (dynamic_cast<cChannel *>(object))
        return "cChannel";
    else if (dynamic_cast<cOutVector *>(object))
        return "cOutVector";
    else if (dynamic_cast<cDensityEstBase *>(object))
        return "cDensityEstBase";
    else if (dynamic_cast<cStatistic *>(object))
        return "cStatistic";
    else if (dynamic_cast<cSimulation*>(object))
        return "cSimulation";
    else if (dynamic_cast<cMessageHeap *>(object))
        return "cMessageHeap";
    else if (dynamic_cast<cWatchBase *>(object))
        return "cWatchBase";
    else if (dynamic_cast<cRegistrationList*>(object))
        return "cRegistrationList";
//    else if (dynamic_cast<cOwnedObject*>(object))   -- not interesting -- no new data members etc
//        return "cOwnedObject";
//    else if (dynamic_cast<cNamedObject*>(object))
//        return "cNamedObject";
    else
        return "cObject";
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
        opt_network_name = network->getName();  // override config setting
        setupNetwork(network);
        startRun();

        state = SIM_READY;
        isConfigRun = false;
    }
    catch (std::exception& e)
    {
        notifyListeners(LF_ON_SIMULATION_ERROR);
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

        if (opt_network_name.empty())
        {
            //TODO confirm("No network specified in the configuration.");
            return;
        }

        cModuleType *network = resolveNetwork(opt_network_name.c_str());
        ASSERT(network);

        setupNetwork(network);
        startRun();

        state = SIM_READY;
        isConfigRun = true;
    }
    catch (std::exception& e)
    {
        notifyListeners(LF_ON_SIMULATION_ERROR);
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
    startClock();
    notifyListeners(LF_ON_SIMULATION_RESUME);
    try
    {
        cEvent *event = simulation.takeNextEvent();
        if (event) {
            printEventBanner(event);
            simulation.executeEvent(event);
        }

        state = SIM_READY;
        notifyListeners(LF_ON_SIMULATION_PAUSE);
    }
    catch (cTerminationException& e)
    {
        state = SIM_TERMINATED;
        stoppedWithTerminationException(e);
        notifyListeners(LF_ON_SIMULATION_SUCCESS);
        displayException(e);
    }
    catch (std::exception& e)
    {
        state = SIM_ERROR;
        stoppedWithException(e);
        notifyListeners(LF_ON_SIMULATION_ERROR);
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
void Cmdenv::runSimulation(RunMode mode, simtime_t until_time, eventnumber_t until_eventnum, long realtimemillis, cMessage *until_msg, cModule *until_module)
{
    ASSERT(state==SIM_READY);

    runMode = mode;
    runUntil.simTime = until_time;
    runUntil.eventNumber = until_eventnum;
    runUntil.msg = until_msg;
    runUntil.module = until_module;  // Note: this is NOT supported with RUNMODE_EXPRESS

    stopSimulationFlag = false;

    state = SIM_RUNNING;
    startClock();
    notifyListeners(LF_ON_SIMULATION_RESUME);
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
        notifyListeners(LF_ON_SIMULATION_PAUSE);
    }
    catch (cTerminationException& e)
    {
        state = SIM_TERMINATED;
        stoppedWithTerminationException(e);
        notifyListeners(LF_ON_SIMULATION_SUCCESS);
        displayException(e);
    }
    catch (std::exception& e)
    {
        state = SIM_ERROR;
        stoppedWithException(e);
        notifyListeners(LF_ON_SIMULATION_ERROR);
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

void Cmdenv::setSimulationRunMode(RunMode mode)
{
    // This function (and the next one too) is called while runSimulation() is
    // underway, from Tcl code that gets a chance to run via the
    // Tcl_Eval(interp, "update") commands
    runMode = mode;
}

void Cmdenv::setSimulationRunUntil(simtime_t until_time, eventnumber_t until_eventnum, cMessage *until_msg)
{
    runUntil.simTime = until_time;
    runUntil.eventNumber = until_eventnum;
    runUntil.msg = until_msg;
}

void Cmdenv::setSimulationRunUntilModule(cModule *until_module)
{
    runUntil.module = until_module;
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
    bool firstevent = true;

    struct timeval last_update;
    gettimeofday(&last_update, NULL);

    while(1)
    {
        if (runMode == RUNMODE_EXPRESS)
            return true;  // should continue, but in a different mode

        cEvent *event = simulation.takeNextEvent();
        if (!event) break; // scheduler interrupted (parsim)

        // "run until message": stop if desired event was reached
        if (runUntil.msg && simulation.msgQueue.peekFirst()==runUntil.msg) break;

        // if stepping locally in module, we stop both immediately
        // *before* and *after* executing the event in that module,
        // but we always execute at least one event
        cModule *mod = event->isMessage() ? static_cast<cMessage*>(event)->getArrivalModule() : NULL;
        bool untilmodule_reached = runUntil.module && moduleContains(runUntil.module,mod);
        if (untilmodule_reached && !firstevent)
            break;
        firstevent = false;

        bool frequent_updates = (runMode==RUNMODE_NORMAL);

        speedometer.addEvent(simulation.getSimTime());

        // do a simulation step
        printEventBanner(event);

        // flush *between* printing event banner and event processing, so that
        // if event processing crashes, it can be seen which event it was
        if (opt_autoflush)
            ::fflush(fout);

        simulation.executeEvent(event);

        // flush so that output from different modules don't get mixed
        flushLastLine();

        // display update
        if (frequent_updates || ((simulation.getEventNumber()&0x0f)==0 && elapsed(opt_updatefreq_fast, last_update)))
        {
            if (speedometer.getMillisSinceIntervalStart() > SPEEDOMETER_UPDATEMILLISECS)
            {
                speedometer.beginNewInterval();
//                updatePerformanceDisplay(speedometer);
            }
//            Tcl_Eval(interp, "update");
            processHttpRequests(false);
            resetElapsedTime(last_update); // exclude UI update time [bug #52]
        }

        // exit conditions
        if (untilmodule_reached) break;
        if (stopSimulationFlag) break;
        if (runUntil.simTime>0 && simulation.guessNextSimtime()>=runUntil.simTime) break;
        if (runUntil.eventNumber>0 && simulation.getEventNumber()>=runUntil.eventNumber) break;

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
    //  - opt_expressmode_autoupdate
    //
    // EXPRESS does not support runUntil.module!
    //

    // update, just to get the above notice displayed
//    Tcl_Eval(interp, "update");

    // OK, let's begin
    Speedometer speedometer;
    speedometer.start(simulation.getSimTime());
    disable_tracing = true;

    struct timeval lastUpdateTime;
    gettimeofday(&lastUpdateTime, NULL);

    do
    {
        cEvent *event = simulation.takeNextEvent();
        if (!event) break; // scheduler interrupted (parsim)

        // "run until message": stop if desired event was reached
        if (runUntil.msg && simulation.msgQueue.peekFirst()==runUntil.msg) break;

        speedometer.addEvent(simulation.getSimTime());

        simulation.executeEvent(event);

        if ((simulation.getEventNumber()&0xff)==0 && elapsed(opt_updatefreq_express, lastUpdateTime))
        {
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
        checkTimeLimits();
    }
    while( !stopSimulationFlag &&
           (runUntil.simTime<=0 || simulation.guessNextSimtime() < runUntil.simTime) &&
           (runUntil.eventNumber<=0 || simulation.getEventNumber() < runUntil.eventNumber)
         );
    return false;
}

void Cmdenv::finishSimulation()
{
    // strictly speaking, we shouldn't allow callFinish() after SIM_ERROR, but it comes handy in practice...
    ASSERT(state==SIM_READY || state==SIM_TERMINATED || state==SIM_ERROR);

    if (state == SIM_READY)
    {
        cTerminationException e("The user has finished the simulation");
        stoppedWithTerminationException(e);
    }

//    logBuffer.addInfo("{** Calling finish() methods of modules\n}");
//    printLastLogLine();

    // now really call finish()
    try
    {
        simulation.callFinish();
        flushLastLine();

        checkFingerprint();
    }
    catch (std::exception& e)
    {
        stoppedWithException(e);
        notifyListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
    }

    // then endrun
    try
    {
        endRun();
    }
    catch (std::exception& e)
    {
        notifyListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
    }
    state = SIM_FINISHCALLED;
}


cObject *Cmdenv::getObjectById(long id)
{
    if (id == 0)
        return NULL;
    std::map<long,cObject*>::iterator it = idToObjectMap.find(id);
    return it == idToObjectMap.end() ? NULL : it->second;
}

long Cmdenv::getIdForObject(cObject *obj)
{
    if (obj == NULL)
        return 0;

    std::map<cObject*,long>::iterator it = objectToIdMap.find(obj);
    if (it == objectToIdMap.end())
    {
        int id = ++lastId;
        ::printf("%ld --> (%s)%s\n", id, obj->getClassName(), obj->getFullPath().c_str());
        objectToIdMap[obj] = id;
        idToObjectMap[id] = obj;
        return id;
    }
    else
        return it->second;
}

std::string Cmdenv::getIdStringForObject(cObject *obj)
{
    if (!obj)
        return "0";
    std::stringstream out;
    out << getIdForObject(obj) << ":" << getKnownBaseClass(obj);
    return out.str();
}

void Cmdenv::objectDeleted(cObject *obj)
{
    EnvirBase::objectDeleted(obj);

    if (!objectToIdMap.empty()) // this method has to have minimal overhead if there are no object queries (maps are empty)
    {
        std::map<cObject*,long>::iterator it = objectToIdMap.find(obj);
        if (it != objectToIdMap.end())
        {
            long id = it->second;
            objectToIdMap.erase(it);
            idToObjectMap.erase(idToObjectMap.find(id)); //XXX this lookup could be eliminated if objectToIdMap contained idToObjectMap::iterator as value!
        }
    }
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
        if (!opt_expressmode)
        {
            disable_tracing = false;
            while (true)
            {
                cEvent *event = simulation.takeNextEvent();
                if (!event)
                    throw cTerminationException("scheduler interrupted while waiting");

                // print event banner if necessary
                if (opt_eventbanners)
                    if (!event->isMessage() || static_cast<cMessage*>(event)->getArrivalModule()->isEvEnabled())
                        printEventBanner(event);

                // flush *between* printing event banner and event processing, so that
                // if event processing crashes, it can be seen which event it was
                if (opt_autoflush)
                    ::fflush(fout);

                // execute event
                simulation.executeEvent(event);

                // flush so that output from different modules don't get mixed
                flushLastLine();

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
                if ((simulation.getEventNumber()&0xff)==0 && elapsed(opt_status_frequency_ms, last_update))
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
        if (opt_expressmode)
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
        if (opt_expressmode)
            doStatusUpdate(speedometer);
        disable_tracing = false;
        stopClock();
        deinstallSignalHandler();
        throw;
    }

    // note: C++ lacks "finally": lines below need to be manually kept in sync with catch{...} blocks above!
    if (opt_expressmode)
        doStatusUpdate(speedometer);
    disable_tracing = false;
    stopClock();
    deinstallSignalHandler();
}

void Cmdenv::printEventBanner(cEvent *event)
{
    ::fprintf(fout, "** Event #%"LL"d  T=%s%s   ",
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
    if (opt_eventbanner_details)
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

    if (opt_perfdisplay)
    {
        ::fprintf(fout, "** Event #%"LL"d   T=%s   Elapsed: %s%s\n",
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
        ::fprintf(fout, "** Event #%"LL"d   T=%s   Elapsed: %s%s   ev/sec=%g\n",
                simulation.getEventNumber(),
                SIMTIME_STR(simulation.getSimTime()),
                timeToStr(totalElapsed()),
                progressPercentage(), // note: IDE launcher uses this to track progress
                speedometer.getEventsPerSec());
    }

    // status update is always autoflushed (not only if opt_autoflush is on)
    ::fflush(fout);
}

const char *Cmdenv::progressPercentage()
{
    double simtimeRatio = -1;
    if (opt_simtimelimit!=0)
         simtimeRatio = simulation.getSimTime() / opt_simtimelimit;

    double cputimeRatio = -1;
    if (opt_cputimelimit!=0) {
        timeval now;
        gettimeofday(&now, NULL);
        long elapsedsecs = now.tv_sec - laststarted.tv_sec + elapsedtime.tv_sec;
        cputimeRatio = elapsedsecs / (double)opt_cputimelimit;
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
    EnvirBase::displayException(ex);

    //TODO display the exception in the UI
    errorInfo.message = ex.what();
    errorInfo.isValid = true;
    while (errorInfo.isValid)
        processHttpRequests(true);  // status() will reset isValid
}

void Cmdenv::componentInitBegin(cComponent *component, int stage)
{
    if (!opt_expressmode && opt_eventbanners && component->isEvEnabled())
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

//-----------------------------------------------------

void Cmdenv::putsmsg(const char *s)
{
    ::fprintf(fout, "\n<!> %s\n\n", s);
    ::fflush(fout);
}

void Cmdenv::sputn(const char *s, int n)
{
    EnvirBase::sputn(s, n);

    if (disable_tracing)
        return;

    cComponent *ctx = simulation.getContext();
    if (!ctx || (opt_modulemsgs && ctx->isEvEnabled()) || simulation.getContextType()==CTX_FINISH)
    {
        ::fwrite(s,1,n,fout);
        if (opt_autoflush)
            ::fflush(fout);
    }

    if (collectJsonLog)
    {
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("L"));
        entry->put("txt", jsonWrap(std::string(s, n)));
        jsonLog->push_back(entry);
    }
}

cEnvir& Cmdenv::flush()
{
    ::fflush(fout);
    return *this;
}

std::string Cmdenv::gets(const char *prompt, const char *defaultReply)
{
    if (!opt_interactive)
    {
        throw cRuntimeError("The simulation wanted to ask a question, set cmdenv-interactive=true to allow it: \"%s\"", prompt);
    }
    else
    {
        ::fprintf(fout, "%s", prompt);
        if (!opp_isempty(defaultReply))
            ::fprintf(fout, "(default: %s) ", defaultReply);
        ::fflush(fout);

        ::fgets(buffer, 512, stdin);
        buffer[strlen(buffer)-1] = '\0'; // chop LF

        if (buffer[0]=='\x1b') // ESC?
           throw cRuntimeError(eCANCEL);

        return std::string(buffer);
    }
}

bool Cmdenv::askyesno(const char *question)
{
    if (!opt_interactive)
    {
        throw cRuntimeError("Simulation needs user input in non-interactive mode (prompt text: \"%s (y/n)\")", question);
    }
    else
    {
        // should also return -1 (==CANCEL)
        for(;;)
        {
            ::fprintf(fout, "%s (y/n) ", question);
            ::fflush(fout);
            ::fgets(buffer, 512, stdin);
            buffer[strlen(buffer)-1] = '\0'; // chop LF
            if (opp_toupper(buffer[0])=='Y' && !buffer[1])
                return true;
            else if (opp_toupper(buffer[0])=='N' && !buffer[1])
                return false;
            else
                putsmsg("Please type 'y' or 'n'!\n");
        }
    }
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
    if (!opt_expressmode && opt_messagetrace)
    {
        ::fprintf(fout, "SENT:   %s\n", msg->info().c_str());
        if (opt_autoflush)
            ::fflush(fout);
    }
}

void Cmdenv::simulationEvent(cEvent *event)
{
    EnvirBase::simulationEvent(event);

    if (!disable_tracing)
    {
        if (opt_messagetrace)
        {
            ::fprintf(fout, "DELIVD: %s\n", event->info().c_str());  //TODO can go out!
            if (opt_autoflush)
                ::fflush(fout);
        }

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
    }
}

void Cmdenv::bubble(cComponent *component, const char *text)
{
    EnvirBase::bubble(component, text);
}

void Cmdenv::beginSend(cMessage *msg)
{
    EnvirBase::beginSend(msg);

    if (collectJsonLog)
    {
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("BS"));
        entry->put("msg", jsonWrap(getIdStringForObject(msg)));
        jsonLog->push_back(entry);
    }
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

    if (collectJsonLog)
    {
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("SD"));
        entry->put("msg", jsonWrap(getIdStringForObject(msg)));
        entry->put("destGate", jsonWrap(getIdStringForObject(toGate)));
        entry->put("propagationDelay", jsonWrap(propagationDelay));
        entry->put("transmissionDelay", jsonWrap(transmissionDelay));
        jsonLog->push_back(entry);
    }
}

void Cmdenv::messageSendHop(cMessage *msg, cGate *srcGate)
{
    EnvirBase::messageSendHop(msg, srcGate);

    if (collectJsonLog)
    {
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("SH"));
        entry->put("msg", jsonWrap(getIdStringForObject(msg)));
        entry->put("srcGate", jsonWrap(getIdStringForObject(srcGate)));
        jsonLog->push_back(entry);
    }
}

void Cmdenv::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    EnvirBase::messageSendHop(msg, srcGate, propagationDelay, transmissionDelay);

    if (collectJsonLog) //TODO we won't animate in fast mode, so BS/SH/ES entries won't be needed then!
    {
        // Note: the link or even the gate may be deleted by the time the client receives
        // this JSON info (so there won't be enough info for animation), but we ignore
        // that rare case to keep the implementation simple
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("SH"));
        entry->put("msg", jsonWrap(getIdStringForObject(msg)));
        entry->put("srcGate", jsonWrap(getIdStringForObject(srcGate)));
        entry->put("propagationDelay", jsonWrap(propagationDelay));
        entry->put("transmissionDelay", jsonWrap(transmissionDelay));
        jsonLog->push_back(entry);
    }
}

void Cmdenv::endSend(cMessage *msg)
{
    EnvirBase::endSend(msg);

    if (collectJsonLog)
    {
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("ES"));
        entry->put("msg", jsonWrap(getIdStringForObject(msg)));
        jsonLog->push_back(entry);
    }
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
    EnvirBase::componentMethodBegin(from, to, methodFmt, va, silent);

    if (collectJsonLog)
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
        entry->put("sm", jsonWrap(((cModule *)from)->getId()));  //FIXME may be a channel too!!!
        entry->put("tm", jsonWrap(((cModule *)to)->getId()));
        entry->put("m", jsonWrap(methodText));
        jsonLog->push_back(entry);
    }
}

void Cmdenv::componentMethodEnd()
{
    EnvirBase::componentMethodEnd();

    if (collectJsonLog)
    {
        JsonObject *entry = new JsonObject();
        entry->put("@", jsonWrap("ME"));
        jsonLog->push_back(entry);
    }
}

void Cmdenv::moduleDeleted(cModule *module)
{
    EnvirBase::moduleDeleted(module);
}

void Cmdenv::moduleReparented(cModule *module, cModule *oldparent)
{
    EnvirBase::moduleReparented(module, oldparent);
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
    ev << "Cmdenv-specific options:\n";
    ev << "  -w            Wait for commands over HTTP instead of starting a simulation\n";
    ev << "                interactively. Overrides -c, -r, -a, -x, -g, -G, -X options.\n";
    ev << "  -c <configname>\n";
    ev << "                Select a given configuration for execution. With inifile-based\n";
    ev << "                configuration database, this selects the [Config <configname>]\n";
    ev << "                section; the default is the [General] section.\n";
    ev << "                See also: -r.\n";
    ev << "  -r <runs>     Execute the specified runs in the configuration selected with the\n";
    ev << "                -c option. <runs> is a comma-separated list of run numbers or\n";
    ev << "                run number ranges, for example 1,2,5-10. When not present, all\n" ;
    ev << "                runs of that configuration will be executed.\n" ;
    ev << "  -a            Print all config names and number of runs it them, and exit.\n";
    ev << "  -x <configname>\n";
    ev << "                Print the number of runs in the given configuration, and exit.\n";
    ev << "  -g, -G        Make -x verbose: print the unrolled configuration, iteration\n";
    ev << "                variables, etc. -G provides more details than -g.\n";
    ev << "  -X <configname>\n";
    ev << "                Print the fallback chain of the given configuration, and exit.\n";
}

unsigned Cmdenv::getExtraStackForEnvir() const
{
    return opt_extrastack;
}


