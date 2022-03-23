//==========================================================================
//  RUNNABLEENVIR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//TODO clean up includes list
#include <cstdio>
#include <csignal>
#include <fstream>
#include <sstream>
#include <set>
#include <algorithm>
#include "common/stringtokenizer.h"
#include "common/fnamelisttokenizer.h"
#include "common/stringutil.h"
#include "common/enumstr.h"
#include "common/opp_ctype.h"
#include "common/stringtokenizer.h"
#include "common/stringutil.h"
#include "common/fileglobber.h"
#include "common/unitconversion.h"
#include "common/commonutil.h"
#include "common/ver.h"
#include "common/fileutil.h"  // splitFileName
#include "omnetpp/ccoroutine.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cscheduler.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/crng.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cobjectfactory.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cfingerprint.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/simtime.h"
#include "omnetpp/platdep/platmisc.h"
#include "omnetpp/cstatisticbuilder.h"
#include "omnetpp/fileline.h"
#include "args.h"
#include "envirbase.h"
#include "envirutils.h"
#include "appreg.h"
#include "valueiterator.h"
#include "xmldoccache.h"
#include "debuggersupport.h"
#include "runnableenvir.h"

#ifdef WITH_PARSIM
#include "omnetpp/cparsimcomm.h"
#include "sim/parsim/cparsimpartition.h"
#include "sim/parsim/cparsimsynchr.h"
#include "sim/parsim/creceivedexception.h"
#endif

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace envir {

extern cConfigOption *CFGID_PARSIM_NUM_PARTITIONS;
extern cConfigOption *CFGID_PARSIM_COMMUNICATIONS_CLASS;
extern cConfigOption *CFGID_PARSIM_SYNCHRONIZATION_CLASS;
extern cConfigOption *CFGID_DEBUGGER_ATTACH_ON_STARTUP;
extern cConfigOption *CFGID_FINGERPRINT;

#define STRINGIZE0(x)    #x
#define STRINGIZE(x)     STRINGIZE0(x)

static const char *compilerInfo =
    #if defined __clang__
    "CLANG " __clang_version__
    #elif defined __GNUC__
    "GCC " __VERSION__
    #elif defined _MSC_VER
    "Microsoft Visual C++ version " STRINGIZE(_MSC_VER)
    #elif defined __INTEL_COMPILER
    "Intel Compiler version " STRINGIZE(__INTEL_COMPILER)
    #else
    "unknown-compiler"
    #endif
    ;

static const char *buildInfoFormat =
    "%d-bit"

    #ifdef __x86_64__
    " ARCH_X86_64"
    #elif __i386__
    " ARCH_I386"
    #elif __aarch64__
    " ARCH_AARCH64"
    #else
    " ARCH_UNKNOWN"
    #endif

    #ifdef NDEBUG
    " RELEASE"
    #else
    " DEBUG"
    #endif
    ;

static const char *buildOptions = ""
    #ifdef WITH_PARSIM
    " WITH_PARSIM"
    #endif

    #ifdef WITH_LIBXML
    " WITH_LIBXML"
    #endif

    #ifdef WITH_MPI
    " WITH_MPI"
    #endif

    #ifdef WITH_NETBUILDER
    " WITH_NETBUILDER"
    #endif

    #ifdef WITH_PYTHONSIM
    " WITH_PYTHONSIM"
    #endif

    #ifdef WITH_QTENV
    " WITH_QTENV"
    #endif

    #ifdef WITH_OSG
    " WITH_OSG"
    #endif

    #ifdef WITH_OSGEARTH
    " WITH_OSGEARTH"
    #endif

    #ifdef WITH_AKAROA
    " WITH_AKAROA"
    #endif
    ;


int RunnableEnvir::run(const std::vector<std::string>& args, cConfiguration *cfg)
{
    char **argv = new char *[args.size()];
    for (int i = 0; i < args.size(); i++)
        argv[i] = const_cast<char*>(args[i].c_str());
    return run(args.size(), argv, cfg);
}

int RunnableEnvir::run(int argc, char *argv[], cConfiguration *configobject)
{
    opt = createOptions();
    args = new ArgList();
    args->parse(argc, argv, ARGSPEC);
    opt->useStderr = !args->optionGiven('m');
    opt->verbose = !args->optionGiven('s');
    cfg = dynamic_cast<cConfigurationEx *>(configobject);
    if (!cfg)
        throw cRuntimeError("Cannot cast configuration object %s to cConfigurationEx", configobject->getClassName());

    try {
        if (cfg->getAsBool(CFGID_DEBUGGER_ATTACH_ON_STARTUP) && debuggerSupport->detectDebugger() != DebuggerPresence::PRESENT)
            debuggerSupport->attachDebugger();
    }
    catch(opp_runtime_error& ex) {
        alert(ex.what());
    }

    if (simulationRequired()) {
        if (setup())
            run();  // must not throw, because we want shutdown() always to be called
        shutdown();
    }
    return exitCode;
}

bool RunnableEnvir::simulationRequired()
{
    // handle -h and -v command-line options
    if (args->optionGiven('h')) {
        const char *category = args->optionValue('h', 0);
        if (!category)
            printHelp();
        else
            EnvirUtils::dumpComponentList(out, category, opt->verbose);
        return false;
    }

    if (args->optionGiven('v')) {
        out << "Build: " OMNETPP_RELEASE " " OMNETPP_BUILDID << "" << endl;
        out << "Compiler: " << compilerInfo << "" << endl;
        out << "Options: " << opp_stringf(buildInfoFormat, 8*sizeof(void *));
        out << buildOptions << endl;
        return false;
    }

    cConfigurationEx *cfg = getConfigEx();

    // -a option: print all config names, and number of runs in them
    if (args->optionGiven('a')) {
        std::vector<std::string> configNames = cfg->getConfigNames();
        for (auto & configName : configNames)
            out << "Config " << configName << ": " << cfg->getNumRunsInConfig(configName.c_str()) << "" << endl;
        return false;
    }

    // try processing -q option
    const char *configName = args->optionValue('c');
    const char *runFilter = args->optionValue('r');
    const char *query = args->optionValue('q');
    const char *configOptionToEcho = args->optionValue('e');

    // process -q
    if (query) {
        if (!configName)
            configName = "General";
        printRunInfo(configName, runFilter, query);
        return false;
    }

    // process -e
    if (configOptionToEcho) {
        if (!configName)
            configName = "General";
        printConfigValue(configName, runFilter, configOptionToEcho);
        return false;
    }

    return true;
}

void RunnableEnvir::printRunInfo(const char *configName, const char *runFilter, const char *query)
{
    //
    // IMPORTANT: the simulation launcher will parse the output of this
    // option, so it should be modified with care and the two kept in sync
    // (see OmnetppLaunchUtils.getSimulationRunInfo()).
    //
    // Rules:
    // (1) the number of runs should appear on the rest of the line
    //     after the "Number of runs:" text
    // (2) per-run information lines should span from the "Number of runs:"
    //     line until the next blank line ("\n\n").
    //

    std::string q = opp_strlower(query); // make match case-insensitive

    if (q.find("run") != q.npos) {
        std::vector<int> runNumbers = resolveRunFilter(configName, runFilter);

        if (opt->verbose) {
            out <<"Config: " << configName << endl;
            out <<"Number of runs: " << cfg->getNumRunsInConfig(configName) << endl;
            if (!opp_isblank(runFilter))
                out << "Number of runs selected: " << runNumbers.size() << endl;
        }

        std::vector<cConfiguration::RunInfo> runInfos = cfg->unrollConfig(configName);
        if (q == "numruns") {
            if (!opt->verbose) // otherwise it was already printed
                out << runNumbers.size() << endl;
        }
        else if (q == "runnumbers") {
            if (opt->verbose) {
                out << endl;
                out << "Run numbers:";
            }
            for (int runNumber : runNumbers)
                out << " " << runNumber;
            out << endl;
        }
        else if (q == "runs") {
            if (opt->verbose)
                out << endl;
            for (int runNumber : runNumbers) {
                const cConfiguration::RunInfo& runInfo = runInfos[runNumber];
                out << "Run " << runNumber << ": " << runInfo.info << endl;
            }
        }
        else if (q == "rundetails") {
            if (opt->verbose)
                out << endl;
            for (int runNumber : runNumbers) {
                const cConfiguration::RunInfo& runInfo = runInfos[runNumber];
                out << "Run " << runNumber << ": " << runInfo.info << endl;
                out << opp_indentlines(runInfo.configBrief, "\t");
                if (runNumber != runNumbers.back())
                    out << endl;
            }
        }
        else if (q == "runconfig") {
            if (opt->verbose)
                out << endl;
            for (int runNumber : runNumbers) {
                const cConfiguration::RunInfo& runInfo = runInfos[runNumber];
                out << "Run " << runNumber << ": " << runInfo.info << endl;
                cfg->activateConfig(configName, runNumber);
                std::vector<const char *> keysValues = cfg->getKeyValuePairs(cConfigurationEx::FILT_ALL);
                for (int i = 0; i < (int)keysValues.size(); i += 2) {
                    const char *key = keysValues[i];
                    const char *value = keysValues[i+1];
                    out << "\t" << key << " = " << value << endl;
                }
                if (runNumber != runNumbers.back())
                    out << endl;
            }
        }
        else {
            err() << "Unrecognized -q argument '" << q << "'" << endl;
            exitCode = 1;
        }
    }
    else if (q == "sectioninheritance") {
        if (opt->verbose)
            out << endl;
        std::vector<std::string> configNames = cfg->getConfigChain(configName);
        for (auto configName : configNames) {
            if (configName != "General")
                configName = "Config " + configName;
            out << configName << endl;
        }
    }
    else {
        err() << "Unrecognized -q argument '" << q << "'" << endl;
        exitCode = 1;
    }
}

void RunnableEnvir::printConfigValue(const char *configName, const char *runFilter, const char *optionName)
{
    // activate the selected configuration
    std::vector<int> runNumbers = resolveRunFilter(configName, runFilter);
    if (runNumbers.size() == 0)
        throw cRuntimeError("Run filter does not match any run");
    if (runNumbers.size() > 1)
        throw cRuntimeError("Run filter matches more than one run (%d)", (int)runNumbers.size());
    int runNumber = runNumbers[0];
    cfg->activateConfig(configName, runNumber);

    // query option
    cConfigOption *option = cConfigOption::get(optionName);
    const char *value = cfg->getConfigValue(option, "");
    out << value << endl;
}

bool RunnableEnvir::setup()
{
    try {
        // ensure correct numeric format in output files
        setPosixLocale();

        // set opt->* variables from ini file(s)
        readOptions();

        if (attachDebuggerOnErrors) {
            signal(SIGSEGV, crashHandler);
            signal(SIGILL, crashHandler);
#ifndef _WIN32
            signal(SIGBUS, crashHandler);
            signal(SIGUSR1, crashHandler);
#endif
        }

        // initialize coroutine library
        if (TOTAL_STACK_SIZE != 0 && opt->totalStack <= MAIN_STACK_SIZE+4096) {
            if (opt->verbose)
                out << "Total stack size " << opt->totalStack << " increased to " << MAIN_STACK_SIZE << endl;
            opt->totalStack = MAIN_STACK_SIZE+4096;
        }
        cCoroutine::init(opt->totalStack, MAIN_STACK_SIZE);

        // install XML document cache
        xmlCache = new XMLDocCache();

        // parallel simulation
        if (opt->parsim) {
#ifdef WITH_PARSIM
            // parsim: create components
            int parsimNumPartitions = cfg->getAsInt(CFGID_PARSIM_NUM_PARTITIONS);
            std::string parsimcommClass = cfg->getAsString(CFGID_PARSIM_COMMUNICATIONS_CLASS);
            std::string parsimsynchClass = cfg->getAsString(CFGID_PARSIM_SYNCHRONIZATION_CLASS);

            parsimComm = createByClassName<cParsimCommunications>(parsimcommClass.c_str(), "parallel simulation communications layer");
            parsimPartition = new cParsimPartition();
            cParsimSynchronizer *parsimSynchronizer = createByClassName<cParsimSynchronizer>(parsimsynchClass.c_str(), "parallel simulation synchronization layer");
            addLifecycleListener(parsimPartition);

            // wire them together (note: 'parsimSynchronizer' is also the scheduler for 'simulation')
            parsimPartition->setContext(getSimulation(), parsimComm, parsimSynchronizer);
            parsimSynchronizer->setContext(getSimulation(), parsimPartition, parsimComm);
            getSimulation()->setScheduler(parsimSynchronizer);

            // initialize them
            parsimComm->init(parsimNumPartitions);
#else
            throw cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif
        }

        // load NED files embedded into the simulation program as string literals
        if (!embeddedNedFiles.empty()) {
            if (opt->verbose)
                out << "Loading embedded NED files: " << embeddedNedFiles.size() << endl;
            for (const auto& file : embeddedNedFiles) {
                std::string nedText = file.nedText;
                if (!file.garblephrase.empty())
                    nedText = opp_ungarble(file.nedText, file.garblephrase);
                getSimulation()->loadNedText(file.fileName.c_str(), nedText.c_str());
            }
        }

        loadNEDFiles();

        // notify listeners when global setup is complete
        notifyLifecycleListeners(LF_ON_STARTUP);
    }
    catch (std::exception& e) {
        displayException(e);
        exitCode = 1;
        return false;  // don't run the app
    }
    return true;
}

void RunnableEnvir::loadNEDFiles()
{
    // load NED files from folders on the NED path
    std::set<std::string> foldersLoaded;
    for (std::string folder : opp_splitpath(opt->nedPath)) {
        if (foldersLoaded.find(folder) == foldersLoaded.end()) {
            if (opt->verbose)
                out << "Loading NED files from " << folder << ": ";
            int count = getSimulation()->loadNedSourceFolder(folder.c_str(), opt->nedExcludedPackages.c_str());
            if (opt->verbose)
                out << " " << count << endl;
            foldersLoaded.insert(folder);
        }
    }
    getSimulation()->doneLoadingNedFiles();
}

void RunnableEnvir::printHelp()
{
    out << "Command line options:\n";
    out << "  <inifile> or -f <inifile>\n";
    out << "                Use the given ini file instead of omnetpp.ini. More than one\n";
    out << "                ini file can be specified.\n";
    out << "  --<configuration-option>=<value>\n";
    out << "                Configuration options and parameter values can be specified\n";
    out << "                on the command line, and they take precedence over options\n";
    out << "                specified in the ini file(s). Examples:\n";
    out << "                      --debug-on-errors=true\n";
    out << "                      --record-eventlog=true\n";
    out << "                      --sim-time-limit=1000s\n";
    out << "                      --**.serviceTime=100ms\n";
    out << "  -u <ui>       Selects the user interface. Standard choices are Cmdenv\n";
    out << "                and Qtenv. Specify -h userinterfaces to see the list\n";
    out << "                of the user interfaces available in your simulation program.\n";
    out << "  -c <configname>\n";
    out << "                Select a configuration for execution. With inifile-based\n";
    out << "                configuration database, this selects the [Config <configname>]\n";
    out << "                section; the default is the [General] section.\n";
    out << "                See also: -r.\n";
    out << "  -r <runfilter>\n";
    out << "                With -c: select runs from the specified configuration. A\n";
    out << "                missing -r option selects all runs in the given configuration.\n";
    out << "                <runfilter> is either a comma-separated list of run numbers or\n";
    out << "                run number ranges (for example 1,2,5-10), or a match expression.\n";
    out << "                The match expression may contain a wildcard pattern that is\n";
    out << "                matched against the iteration variables string (see -q runs),\n";
    out << "                matchers for individual iteration variables in the\n";
    out << "                name(valuepattern) syntax, or their combination using AND and\n";
    out << "                OR. Parentheses may be used to change evaluation order. Values\n";
    out << "                containing spaces etc need to be enclosed in quotes. Patterns\n";
    out << "                may contain elements matching numeric ranges, in the {a..b}\n";
    out << "                syntax. See also: -q.\n";
    out << "  -n <nedpath>  List of folders to load NED files from. Folders are separated\n";
    out << "                with a semicolon (on non-Windows systems, colon may also be used).\n";
    out << "                Multiple -n options may be present. The effective NED path is\n";
    out << "                produced by concatenating the values of the -n options, the\n";
    out << "                ned-path configuration option, and the OMNETPP_NED_PATH environment\n";
    out << "                variable. If the result is empty, the NED path defaults to '.',\n";
    out << "                that is, NED files will be loaded from the current directory and\n";
    out << "                its subfolders.\n";
    out << "  -x <ned-packages-to-exclude>\n";
    out << "                Semicolon-separated list of NED packages to exclude when loading\n";
    out << "                NED files. Sub-packages of excluded ones are also excluded.\n";
    out << "                Multiple -x options may be present, and additional packages may be\n";
    out << "                specified in the ned-package-exclusions configuration option and the\n";
    out << "                OMNETPP_NED_PACKAGE_EXCLUSIONS environment variable.\n";
    out << "  -i <imgpath>  List of folders to load images from. Folders are separated\n";
    out << "                with a semicolon (on non-Windows systems, colon may also be used).\n";
    out << "                Multiple -i options may be present. The effective image path is\n";
    out << "                produced by concatenating the values of the -i options, the\n";
    out << "                image-path configuration option, and the OMNETPP_IMAGE_PATH\n";
    out << "                environment variable. When the environment variable is not\n";
    out << "                present, a hardcoded folder (determined at compile time) pointing\n";
    out << "                to <omnetpp>/images is used instead.\n";
    out << "  -l <library>  Load the specified shared library (.so or .dll) on startup.\n";
    out << "                The file name should be given without the .so or .dll suffix\n";
    out << "                (it will be appended automatically.) The loaded module may\n";
    out << "                contain simple modules, plugins, etc. Multiple -l options\n";
    out << "                can be present.\n";
#ifdef SIMFRONTEND_SUPPORT
    out << "  -p <port>     Port number for the built-in web server.\n";
    out << "                If the port is not available, the program will exit with an\n";
    out << "                error message unless the number is suffixed with the plus\n";
    out << "                sign. The plus sign causes the program to search for the\n";
    out << "                first available port number above the given one, and not stop\n";
    out << "                with an error even if no available port was found. A plain\n";
    out << "                minus sign will turn off the built-in web server.\n";
    out << "                The default value is \"8000+\".\n";
#endif
    out << "  -s            Silent mode; makes the output less verbose.\n";
    out << "  -v            Print version and build info, and exit.\n";
    out << "  -m            Merge standard error into standard output, i.e. report errors on\n";
    out << "                the standard output instead of the default standard error.\n";
    out << "  -a            Print all config names and number of runs in them, and exit.\n";
    out << "  -q <what>     To be used together with -c and -r. Prints information about\n";
    out << "                the specified configuration and runs, and exits.\n";
    out << "    -q numruns  Prints the number of runs in the given configuration and the\n";
    out << "                number of runs selected by the run filter (-r option).\n";
    out << "    -q runnumbers\n";
    out << "                Prints the run numbers of the runs selected by the run filter\n";
    out << "                (-r option).\n";
    out << "    -q runs     Like -q numruns, but also prints one line of information with the\n";
    out << "                iteration variables about each run that the run filter matches.\n";
    out << "    -q rundetails\n";
    out << "                Like -q numruns, but also prints the values of the iteration\n";
    out << "                variables and a summary of the configuration (the expanded\n";
    out << "                values of configuration entries that contain iteration variables)\n";
    out << "                for each matching run.\n";
    out << "    -q runconfig\n";
    out << "                Like -q numruns, but also prints the values of the iteration\n";
    out << "                variables and the full configuration for each matching run.\n";
    out << "    -q sectioninheritance\n";
    out << "                Print the section fallback chain of the specified configuration.\n";
    out << "  -e <option>   Prints the value of the given configuration option in the\n";
    out << "                simulation run denoted by the -c and -r options. (The run filter\n";
    out << "                should match exactly one simulation run.)\n";
    out << "  -h            Print this help and exit.\n";
    out << "  -h <category> Lists registered components:\n";
    out << "    -h config         Prints the list of available configuration options\n";
    out << "    -h configdetails  Prints the list of available configuration options, with\n";
    out << "                      their documentation\n";
    out << "    -h configvars     Prints the list of variables that can be used in configuration values\n";
    out << "    -h userinterfaces Lists available user interfaces (see -u option)\n";
    out << "    -h classes        Lists registered C++ classes (including module classes)\n";
    out << "    -h classdesc      Lists C++ classes that have associated reflection\n";
    out << "                      information (needed for Qtenv inspectors)\n";
    out << "    -h nedfunctions   Lists registered NED functions\n";
    out << "    -h neddecls       Lists built-in NED component declarations\n";
    out << "    -h units          Lists recognized physical units\n";
    out << "    -h enums          Lists registered enums\n";
    out << "    -h resultfilters  Lists result filters\n";
    out << "    -h resultrecorders Lists result recorders\n";
    out << "    -h figures        Lists available figure types\n";
    out << "    -h all            Union of all the above\n";
    out << endl;

    // print specific help for each user interface
    cRegistrationList *table = omnetapps.getInstance();
    table->sort();
    for (int i = 0; i < table->size(); i++) {
        // Instantiate the user interface, call printUISpecificHelp(), then dispose.
        // Note: their constructors are not supposed to do anything but trivial member initialization.
        cOmnetAppRegistration *appreg = dynamic_cast<cOmnetAppRegistration *>(table->get(i));
        ASSERT(appreg != nullptr);
        cEnvir *app = appreg->createOne();
        if (RunnableEnvir *envir = dynamic_cast<RunnableEnvir*>(app))
            envir->printUISpecificHelp();
        delete app;
    }
}

void RunnableEnvir::run()
{
    try {
        doRun();
    }
    catch (std::exception& e) {
        displayException(e);
    }
}

void RunnableEnvir::shutdown()
{
    try {
        getSimulation()->deleteNetwork();
        notifyLifecycleListeners(LF_ON_SHUTDOWN);
    }
    catch (std::exception& e) {
        displayException(e);
    }
}

void RunnableEnvir::setupNetwork(cModuleType *network)
{
    currentEventName = "";
    currentEventClassName = nullptr;
    currentModuleId = -1;

    getSimulation()->setupNetwork(network);
    eventlogManager->flush();

    if (opt->debugStatisticsRecording)
        EnvirUtils::dumpResultRecorders(out, getSimulation()->getSystemModule());
}

void RunnableEnvir::prepareForRun()
{
    resetClock();
    if (opt->simtimeLimit >= SIMTIME_ZERO)
        getSimulation()->setSimulationTimeLimit(opt->simtimeLimit);
    getSimulation()->callInitialize();
    cLogProxy::flushLastLine();
}

//-------------------------------------------------------------

std::vector<int> RunnableEnvir::resolveRunFilter(const char *configName, const char *runFilter)
{
    std::vector<int> runNumbers;

    if (opp_isblank(runFilter)) {
        int numRuns = cfg->getNumRunsInConfig(configName);
        for (int i = 0; i < numRuns; i++)
            runNumbers.push_back(i);
        return runNumbers;
    }

    // if filter contains a list of run numbers (e.g. "0..4,9,12"), parse it accordingly
    if (strspn (runFilter, "0123456789,.- ") == strlen(runFilter)) {
        int numRuns = cfg->getNumRunsInConfig(configName);
        EnumStringIterator it(runFilter);
        while (true) {
            int runNumber = it();
            if (runNumber == -1)
                break;
            if (runNumber >= numRuns)
                throw cRuntimeError("Run number %d in run list '%s' is out of range 0..%d", runNumber, runFilter, numRuns-1);
            runNumbers.push_back(runNumber);
            it++;
        }
        if (it.hasError())
            throw cRuntimeError("Syntax error in run list '%s'", runFilter);
    }
    else {
        // evaluate filter as constraint expression
        std::vector<cConfiguration::RunInfo> runDescriptions = cfg->unrollConfig(configName);
        for (int runNumber = 0; runNumber < (int) runDescriptions.size(); runNumber++) {
            try {
                cConfiguration::RunInfo runInfo = runDescriptions[runNumber];
                std::string expandedRunFilter = opp_substitutevariables(runFilter, runInfo.runAttrs);
                ValueIterator::Expr expr(expandedRunFilter.c_str());
                expr.substituteVariables(ValueIterator::VariableMap());
                expr.evaluate();
                if (expr.boolValue())
                    runNumbers.push_back(runNumber);
            }
            catch (std::exception& e) {
                throw cRuntimeError("Cannot evaluate run filter: %s", e.what());
            }
        }
    }
    return runNumbers;
}

//-------------------------------------------------------------

void RunnableEnvir::startOutputRedirection(const char *fileName)
{
    ASSERT(!isOutputRedirected());

    mkPath(directoryOf(fileName).c_str());

    std::filebuf *fbuf = new std::filebuf;
    fbuf->open(fileName, std::ios_base::out);
    if (!fbuf->is_open())
       throw cRuntimeError("Cannot open output redirection file '%s'", fileName);
    out.rdbuf(fbuf);
    redirectionFilename = fileName;
}

void RunnableEnvir::stopOutputRedirection()
{
    if (isOutputRedirected()) {
        std::streambuf *fbuf = out.rdbuf();
        fbuf->pubsync();
        out.rdbuf(std::cout.rdbuf());
        delete fbuf;
        redirectionFilename = "";
    }
}

bool RunnableEnvir::isOutputRedirected()
{
    return out.rdbuf() != std::cout.rdbuf();
}

std::ostream& RunnableEnvir::err()
{
    std::ostream& err = opt->useStderr && !isOutputRedirected() ? std::cerr : out;
    if (isOutputRedirected())
        (opt->useStderr ? std::cerr : std::cout) << "<!> Error -- see " << redirectionFilename << " for details" << endl;
    err << endl << "<!> Error: ";
    return err;
}

std::ostream& RunnableEnvir::errWithoutPrefix()
{
    std::ostream& err = opt->useStderr && !isOutputRedirected() ? std::cerr : out;
    if (isOutputRedirected())
        (opt->useStderr ? std::cerr : std::cout) << "<!> Error -- see " << redirectionFilename << " for details" << endl;
    err << endl << "<!> ";
    return err;
}

std::ostream& RunnableEnvir::warn()
{
    std::ostream& err = opt->useStderr && !isOutputRedirected() ? std::cerr : out;
    if (isOutputRedirected())
        (opt->useStderr ? std::cerr : std::cout) << "<!> Warning -- see " << redirectionFilename << " for details" << endl;
    err << endl << "<!> Warning: ";
    return err;
}


bool RunnableEnvir::ensureDebugger(cRuntimeError *error)
{
    if (error == nullptr || attachDebuggerOnErrors) {
        try {
            if (debuggerSupport->detectDebugger() == DebuggerPresence::NOT_PRESENT)
                debuggerSupport->attachDebugger();
            return debuggerSupport->detectDebugger() != DebuggerPresence::NOT_PRESENT;
        }
        catch(opp_runtime_error& ex) {
            alert(ex.what());
        }
    }
    else {

#ifdef NDEBUG
       printf("\n[Warning: Program was compiled without debug info, you might not be able to debug.]\n");
#endif

   printf("\n"
          "RUNTIME ERROR. A cRuntimeError exception is about to be thrown, and you\n"
          "requested (by setting debug-on-errors=true in the ini file) that errors\n"
          "abort execution and break into the debugger.\n\n"
#ifdef _MSC_VER
           "If you see a [Debug] button on the Windows crash dialog and you have\n"
           "just-in-time debugging enabled, select it to get into the Visual Studio\n"
           "debugger. Otherwise, you should already have attached to this process from\n"
           "Visual Studio. Once in the debugger, see you can browse to the context of\n"
           "the error in the \"Call stack\" debug view.\n\n"
#else
           "You should now probably be running the simulation under gdb or another\n"
           "debugger. The simulation kernel will now raise a SIGINT signal which will\n"
           "get you into the debugger. If you are not running under a debugger, you can\n"
           "still use the core dump for post-mortem debugging. Once in the debugger,\n"
           "view the call stack (in gdb: \"bt\" command) to see the context of the\n"
           "runtime error.\n\n"
#endif
       );

       printf("<!> %s\n", error->getFormattedMessage().c_str());
       printf("\nTRAPPING on the exception above, due to a debug-on-errors=true configuration option. Is your debugger ready?\n");
       fflush(stdout);

       return true;
    }

    return false;
}

void RunnableEnvir::crashHandler(int)
{
    RunnableEnvir *envir = dynamic_cast<RunnableEnvir*>(cSimulation::getActiveEnvir());
    if (!envir)
        return; // not a suitable envir

    try {
        envir->debuggerSupport->attachDebugger();
    }
    catch (opp_runtime_error& ex) {
        envir->alert(ex.what());
    }
}

std::string RunnableEnvir::getFormattedMessage(std::exception& ex)
{
    if (cException *e = dynamic_cast<cException *>(&ex))
        return e->getFormattedMessage();
    else
        return ex.what();
}

void RunnableEnvir::displayException(std::exception& ex)
{
    std::string msg = getFormattedMessage(ex);
    if (dynamic_cast<cTerminationException*>(&ex) != nullptr)
        out << endl << "<!> " << msg << endl;
    else if (msg.substr(0,5) == "Error")
        errWithoutPrefix() << msg << endl;
    else
        err() << msg << endl;
}

//-------------------------------------------------------------

void RunnableEnvir::resetClock()
{
    stopwatch.resetClock();
}

void RunnableEnvir::startClock()
{
    stopwatch.startClock();
}

void RunnableEnvir::stopClock()
{
    stopwatch.stopClock();
    simulatedTime = getSimulation()->getSimTime();
}

double RunnableEnvir::getElapsedSecs()
{
    return stopwatch.getElapsedSecs();
}

void RunnableEnvir::checkTimeLimits()
{
    if (!stopwatch.hasTimeLimits())
        return;
    if (isExpressMode() && (getSimulation()->getEventNumber() & 1023) != 0)  // optimize: in Express mode, don't read the clock on every event
        return;
    stopwatch.checkTimeLimits();
}

void RunnableEnvir::stoppedWithTerminationException(cTerminationException& e)
{
    // if we're running in parallel and this exception is NOT one we received
    // from other partitions, then notify other partitions
#ifdef WITH_PARSIM
    if (opt->parsim && !dynamic_cast<cReceivedTerminationException *>(&e))
        parsimPartition->broadcastTerminationException(e);
#endif
    if (recordEventlog) {
        //FIXME should not be in this function (Andras)
        eventlogManager->endRun(e.isError(), e.getErrorCode(), e.getFormattedMessage().c_str());
    }
}

void RunnableEnvir::stoppedWithException(std::exception& e)
{
    // if we're running in parallel and this exception is NOT one we received
    // from other partitions, then notify other partitions
#ifdef WITH_PARSIM
    if (opt->parsim && !dynamic_cast<cReceivedException *>(&e))
        parsimPartition->broadcastException(e);
#endif
    if (recordEventlog) {
        // TODO: get error code from the exception?
        eventlogManager->endRun(true, E_CUSTOM, e.what());  //FIXME this should be rather in endRun(), or? (Andras)
    }
}

void RunnableEnvir::checkFingerprint()
{
    cFingerprintCalculator *fingerprint = getSimulation()->getFingerprintCalculator();
    if (!getSimulation()->getFingerprintCalculator())
        return;

    auto flags = opp_substringafterlast(fingerprint->str(), "/");
    if (fingerprint->checkFingerprint())
        printfmsg("Fingerprint successfully verified: %s", fingerprint->str().c_str());
    else
        printfmsg("Fingerprint mismatch! calculated: %s, expected: %s",
                fingerprint->str().c_str(), cfg->getAsString(CFGID_FINGERPRINT).c_str());
}

cModuleType *RunnableEnvir::resolveNetwork(const char *networkname)
{
    cModuleType *network = nullptr;
    std::string inifilePackage = getSimulation()->getNedPackageForFolder(opt->inifileNetworkDir.c_str());

    bool hasInifilePackage = !inifilePackage.empty() && strcmp(inifilePackage.c_str(), "-") != 0;
    if (hasInifilePackage)
        network = cModuleType::find((inifilePackage+"."+networkname).c_str());
    if (!network)
        network = cModuleType::find(networkname);
    if (!network) {
        if (hasInifilePackage)
            throw cRuntimeError("Network '%s' or '%s' not found, check .ini and .ned files",
                    networkname, (inifilePackage+"."+networkname).c_str());
        else
            throw cRuntimeError("Network '%s' not found, check .ini and .ned files", networkname);
    }
    if (!network->isNetwork())
        throw cRuntimeError("Module type '%s' is not a network", network->getFullName());
    return network;
}


}  // namespace envir
}  // namespace omnetpp

