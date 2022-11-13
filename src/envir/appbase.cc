//==========================================================================
//  APPBASE.CC - part of
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

#include <cstdio>
#include <csignal>
#include <fstream>
#include <sstream>
#include <set>
#include <algorithm>
#include "common/stringutil.h"
#include "common/enumstr.h"
#include "common/ver.h"
#include "common/fileutil.h"
#include "common/stlutil.h"
#include "omnetpp/simtime.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/ccoroutine.h"
#include "omnetpp/cinedloader.h"
#include "omnetpp/platdep/platmisc.h"
#include "appbase.h"
#include "args.h"
#include "envirbase.h"
#include "envirutils.h"
#include "appreg.h"
#include "debuggersupport.h"
#include "simulationholder.h"
#include "inifilecontents.h"
#include "debuggersupport.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace envir {

//TODO CFGID_DEBUGGER_ATTACH_ON_ERROR?
Register_GlobalConfigOption(CFGID_DEBUGGER_ATTACH_ON_STARTUP, "debugger-attach-on-startup", CFG_BOOL, "false", "When set to true, the simulation program will launch an external debugger attached to it (if not already present), allowing you to set breakpoints before proceeding. The debugger command is configurable. Note that debugging (i.e. attaching to) a non-child process needs to be explicitly enabled on some systems, e.g. Ubuntu.");

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

AppBase *AppBase::activeApp = nullptr;


AppBase::AppBase(std::ostream& out) : out(out), debuggerSupport(new DebuggerSupport())
{
    if (activeApp == nullptr)
        activeApp = this;
}

AppBase::~AppBase()
{
    if (activeApp == this)
        activeApp = nullptr;
    delete debuggerSupport;
}

int AppBase::runApp(const std::vector<std::string>& args, InifileContents *ini)
{
    char **argv = new char *[args.size()];
    for (int i = 0; i < args.size(); i++)
        argv[i] = const_cast<char*>(args[i].c_str());
    return runApp(args.size(), argv, ini);
}

int AppBase::runApp(int argc, char *argv[], InifileContents *ini)
{
    if (activeApp && activeApp != this)
        throw opp_runtime_error("AppBase::runApp(): Another app is already active");
    activeApp = this;

    this->ini = ini;
    args = new ArgList();
    args->parse(argc, argv, ARGSPEC);
    useStderr = !args->optionGiven('m');
    verbose = !args->optionGiven('s');

    // ensure correct numeric format in output files
    setPosixLocale();

//TODO move this out of here?
    cConfiguration *globalCfg = ini->extractGlobalConfig();
    debuggerSupport->configure(globalCfg);
    bool attachOnStartup = globalCfg->getAsBool(CFGID_DEBUGGER_ATTACH_ON_STARTUP);
    bool attachOnError = globalCfg->getAsBool(CFGID_DEBUGGER_ATTACH_ON_ERROR);
    delete globalCfg;

     if (attachOnError)
        installCrashHandler();

    try {
        if (attachOnStartup && debuggerSupport->detectDebugger() != DebuggerPresence::PRESENT)
            debuggerSupport->attachDebugger();
    }
    catch(opp_runtime_error& ex) {
        alert(ex.what());
    }

    int exitCode = 0;
    try {
        if (simulationRequired())  // handle help, config queries, etc.
            exitCode = doRunApp();
    }
    catch (std::exception& e) {
        displayException(e);
        exitCode = 1;
    }
    activeApp = nullptr;
    return exitCode;
}

bool AppBase::simulationRequired()
{
    // handle -h and -v command-line options
    if (args->optionGiven('h')) {
        const char *category = args->optionValue('h', 0);
        if (!category)
            printHelp();
        else
            EnvirUtils::dumpComponentList(out, category, verbose);
        return false;
    }

    if (args->optionGiven('v')) {
        out << "Build: " OMNETPP_RELEASE " " OMNETPP_BUILDID << "" << endl;
        out << "Compiler: " << compilerInfo << "" << endl;
        out << "Options: " << opp_stringf(buildInfoFormat, 8*sizeof(void *));
        out << buildOptions << endl;
        return false;
    }

    // -a option: print all config names, and number of runs in them
    if (args->optionGiven('a')) {
        std::vector<std::string> configNames = ini->getConfigNames();
        for (auto & configName : configNames)
            out << "Config " << configName << ": " << ini->getNumRunsInConfig(configName.c_str()) << "" << endl;
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

void AppBase::printRunInfo(const char *configName, const char *runFilter, const char *query)
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

        if (verbose) {
            out <<"Config: " << configName << endl;
            out <<"Number of runs: " << ini->getNumRunsInConfig(configName) << endl;
            if (!opp_isblank(runFilter))
                out << "Number of runs selected: " << runNumbers.size() << endl;
        }

        std::vector<InifileContents::RunInfo> runInfos = ini->unrollConfig(configName);
        if (q == "numruns") {
            if (!verbose) // otherwise it was already printed
                out << runNumbers.size() << endl;
        }
        else if (q == "runnumbers") {
            if (verbose) {
                out << endl;
                out << "Run numbers:";
            }
            for (int runNumber : runNumbers)
                out << " " << runNumber;
            out << endl;
        }
        else if (q == "runs") {
            if (verbose)
                out << endl;
            for (int runNumber : runNumbers) {
                const InifileContents::RunInfo& runInfo = runInfos[runNumber];
                out << "Run " << runNumber << ": " << runInfo.info << endl;
            }
        }
        else if (q == "rundetails") {
            if (verbose)
                out << endl;
            for (int runNumber : runNumbers) {
                const InifileContents::RunInfo& runInfo = runInfos[runNumber];
                out << "Run " << runNumber << ": " << runInfo.info << endl;
                out << opp_indentlines(runInfo.configBrief, "\t");
                if (runNumber != runNumbers.back())
                    out << endl;
            }
        }
        else if (q == "runconfig") {
            if (verbose)
                out << endl;
            for (int runNumber : runNumbers) {
                const InifileContents::RunInfo& runInfo = runInfos[runNumber];
                out << "Run " << runNumber << ": " << runInfo.info << endl;
                cConfiguration *cfg = ini->extractConfig(configName, runNumber);
                std::vector<const char *> keysValues = cfg->getKeyValuePairs(cConfiguration::FILT_ALL);
                for (int i = 0; i < (int)keysValues.size(); i += 2) {
                    const char *key = keysValues[i];
                    const char *value = keysValues[i+1];
                    out << "\t" << key << " = " << value << endl;
                }
                if (runNumber != runNumbers.back())
                    out << endl;
                delete cfg;
            }
        }
        else {
            throw opp_runtime_error("Unrecognized -q argument '%s'", q.c_str());
        }
    }
    else if (q == "sectioninheritance") {
        if (verbose)
            out << endl;
        std::vector<std::string> configNames = ini->getConfigChain(configName);
        for (auto configName : configNames) {
            if (configName != "General")
                configName = "Config " + configName;
            out << configName << endl;
        }
    }
    else {
        throw opp_runtime_error("Unrecognized -q argument '%s'", q.c_str());
    }
}

void AppBase::printConfigValue(const char *configName, const char *runFilter, const char *optionName)
{
    // activate the selected configuration
    std::vector<int> runNumbers = resolveRunFilter(configName, runFilter);
    if (runNumbers.size() == 0)
        throw cRuntimeError("Run filter does not match any run");
    if (runNumbers.size() > 1)
        throw cRuntimeError("Run filter matches more than one run (%d)", (int)runNumbers.size());
    int runNumber = runNumbers[0];
    cConfiguration *cfg = ini->extractConfig(configName, runNumber);

    // query option
    cConfigOption *option = cConfigOption::get(optionName);
    const char *value = cfg->getConfigValue(option, "");
    out << value << endl;
    delete cfg;
}

void AppBase::installCrashHandler()
{
    signal(SIGSEGV, crashHandler);
    signal(SIGILL, crashHandler);
#ifndef _WIN32
    signal(SIGBUS, crashHandler);
    signal(SIGUSR1, crashHandler);
#endif
}

void AppBase::printHelp()
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
        AppBase *app = appreg->createOne();
        app->printUISpecificHelp();
        delete app;
    }
}

std::vector<int> AppBase::resolveRunFilter(const char *configName, const char *runFilter)
{
    std::vector<int> runNumbers;

    if (opp_isblank(runFilter)) {
        int numRuns = ini->getNumRunsInConfig(configName);
        for (int i = 0; i < numRuns; i++)
            runNumbers.push_back(i);
        return runNumbers;
    }

    // if filter contains a list of run numbers (e.g. "0..4,9,12"), parse it accordingly
    if (strspn (runFilter, "0123456789,.- ") == strlen(runFilter)) {
        int numRuns = ini->getNumRunsInConfig(configName);
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
        std::vector<InifileContents::RunInfo> runDescriptions = ini->unrollConfig(configName);
        for (int runNumber = 0; runNumber < (int) runDescriptions.size(); runNumber++) {
            try {
                InifileContents::RunInfo runInfo = runDescriptions[runNumber];
                std::string expandedRunFilter = opp_substitutevariables(runFilter, unionOf(runInfo.runAttrs, runInfo.iterVars));
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

void AppBase::alertf(const char *fmt, ...)
{
    char buffer[1000];
    va_list va;
    va_start(va, fmt);
    vsnprintf(buffer, 1000, fmt, va);
    buffer[999] = '\0';
    va_end(va);
    alert(buffer);
}

bool AppBase::ensureDebugger(cRuntimeError *error)
{
    EnvirBase *envirBase = dynamic_cast<EnvirBase*>(cSimulation::getActiveEnvir());
    if (error == nullptr || (envirBase && envirBase->getAttachDebuggerOnErrors())) { //TODO why ask Envir? move check out of this function
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

void AppBase::crashHandler(int)
{
    AppBase *app = AppBase::getActiveApp();
    if (!app)
        return; // not running an AppBase

    try {
        app->getDebuggerSupport()->attachDebugger();
    }
    catch (opp_runtime_error& ex) {
        app->alert(ex.what());
    }
}

void AppBase::displayException(std::exception& ex)
{
    std::string msg = cException::getFormattedMessage(ex);
    alert(msg.c_str());
}

}  // namespace envir
}  // namespace omnetpp
