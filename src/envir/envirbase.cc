//==========================================================================
//  ENVIRBASE.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
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
#include <assert.h>
#include <fstream>

#include "args.h"
#include "envirbase.h"
#include "appreg.h"
#include "ccoroutine.h"
#include "csimulation.h"
#include "cscheduler.h"
#include "cpar.h"
#include "cproperties.h"
#include "cproperty.h"
#include "random.h"
#include "crng.h"
#include "cmodule.h"
#include "ccomponenttype.h"
#include "cxmlelement.h"
#include "cxmldoccache.h"
#include "fnamelisttokenizer.h"
#include "chasher.h"
#include "cconfigkey.h"
#include "regmacros.h"
#include "stringutil.h"
#include "enumstr.h"
#include "simtime.h"

#ifdef WITH_PARSIM
#include "cparsimcomm.h"
#include "parsim/cparsimpartition.h"
#include "parsim/cparsimsynchr.h"
#include "parsim/creceivedexception.h"
#endif

#include "opp_ctype.h"
#include "stringtokenizer.h"
#include "fileglobber.h"
#include "unitconversion.h"
#include "commonutil.h"
#include "../utils/ver.h"


#include "timeutil.h"
#include "platmisc.h"
#include "fileutil.h"  // splitFileName


#ifdef USE_PORTABLE_COROUTINES /* coroutine stacks reside in main stack area */

# define TOTAL_STACK_SIZE   (2*1024*1024)
# define MAIN_STACK_SIZE       (128*1024)  // Tkenv needs more than 64K

#else /* nonportable coroutines, stacks are allocated on heap */

# define TOTAL_STACK_SIZE        0  // dummy value
# define MAIN_STACK_SIZE         0  // dummy value

#endif

#ifdef _WIN32
#define PATH_SEPARATOR   ";"
#else
#define PATH_SEPARATOR   ":;"
#endif

USING_NAMESPACE


using std::ostream;


#define CREATE_BY_CLASSNAME(var,classname,baseclass,description) \
     baseclass *var ## _tmp = (baseclass *) createOne(classname); \
     var = dynamic_cast<baseclass *>(var ## _tmp); \
     if (!var) \
         throw cRuntimeError("Class \"%s\" is not subclassed from " #baseclass, (const char *)classname);

Register_GlobalConfigEntryU(CFGID_TOTAL_STACK, "total-stack", "B", NULL, "Specifies the maximum memory for activity() simple module stacks. You need to increase this value if you get a ``Cannot allocate coroutine stack'' error.");
Register_GlobalConfigEntry(CFGID_PARALLEL_SIMULATION, "parallel-simulation", CFG_BOOL, "false", "Enables parallel distributed simulation.");
Register_GlobalConfigEntry(CFGID_SCHEDULER_CLASS, "scheduler-class", CFG_STRING, "cSequentialScheduler", "Part of the Envir plugin mechanism: selects the scheduler class. This plugin interface allows for implementing real-time, hardware-in-the-loop, distributed and distributed parallel simulation. The class has to implement the cScheduler interface.");
Register_GlobalConfigEntry(CFGID_PARSIM_COMMUNICATIONS_CLASS, "parsim-communications-class", CFG_STRING, "cFileCommunications", "If parallel-simulation=true, it selects the class that implements communication between partitions. The class must implement the cParsimCommunications interface.");
Register_GlobalConfigEntry(CFGID_PARSIM_SYNCHRONIZATION_CLASS, "parsim-synchronization-class", CFG_STRING, "cNullMessageProtocol", "If parallel-simulation=true, it selects the parallel simulation algorithm. The class must implement the cParsimSynchronizer interface.");
Register_GlobalConfigEntry(CFGID_OUTPUTVECTORMANAGER_CLASS, "outputvectormanager-class", CFG_STRING, "cIndexedFileOutputVectorManager", "Part of the Envir plugin mechanism: selects the output vector manager class to be used to record data from output vectors. The class has to implement the cOutputVectorManager interface.");
Register_GlobalConfigEntry(CFGID_OUTPUTSCALARMANAGER_CLASS, "outputscalarmanager-class", CFG_STRING, "cFileOutputScalarManager", "Part of the Envir plugin mechanism: selects the output scalar manager class to be used to record data passed to recordScalar(). The class has to implement the cOutputScalarManager interface.");
Register_GlobalConfigEntry(CFGID_SNAPSHOTMANAGER_CLASS, "snapshotmanager-class", CFG_STRING, "cFileSnapshotManager", "Part of the Envir plugin mechanism: selects the class to handle streams to which snapshot() writes its output. The class has to implement the cSnapshotManager interface.");
Register_GlobalConfigEntry(CFGID_FNAME_APPEND_HOST, "fname-append-host", CFG_BOOL, "false", "Turning it on will cause the host name and process Id to be appended to the names of output files (e.g. omnetpp.vec, omnetpp.sca). This is especially useful with distributed simulation.");
Register_GlobalConfigEntry(CFGID_DEBUG_ON_ERRORS, "debug-on-errors", CFG_BOOL, "false", "When set to true, runtime errors will cause the simulation program to break into the C++ debugger (if the simulation is running under one, or just-in-time debugging is activated). Once in the debugger, you can view the stack trace or examine variables.");
Register_GlobalConfigEntry(CFGID_PERFORM_GC, "perform-gc", CFG_BOOL, "false", "Whether the simulation kernel should delete on network cleanup the simulation objects not deleted by simple module destructors. Not recommended.");
Register_GlobalConfigEntry(CFGID_PRINT_UNDISPOSED, "print-undisposed", CFG_BOOL, "true", "Whether to report objects left (that is, not deallocated by simple module destructors) after network cleanup.");
Register_GlobalConfigEntry(CFGID_SIMTIME_SCALE, "simtime-scale", CFG_INT, "-12", "Sets the scale exponent, and thus the resolution of time for the 64-bit fixed-point simulation time representation. Accepted values are -18..0; for example, -6 selects microsecond resolution. -12 means picosecond resolution, with a maximum simtime of ~110 days.");
Register_GlobalConfigEntry(CFGID_NED_PATH, "ned-path", CFG_STRING, "", "A semicolon-separated list of directories. The directories will be regarded as roots of the NED package hierarchy, and all NED files will be loaded from their subdirectory trees. This option is normally left empty, as the OMNeT++ IDE sets the NED path automatically, and for simulations started outside the IDE it is more convenient to specify it via a command-line option or the NEDPATH environment variable.");

Register_PerRunConfigEntry(CFGID_NETWORK, "network", CFG_STRING, NULL, "The name of the network to be simulated.  The package name can be omitted if the ini file is in the same directory as the NED file that contains the network.");
Register_PerRunConfigEntry(CFGID_WARNINGS, "warnings", CFG_BOOL, "true", "Enables warnings.");
Register_PerRunConfigEntryU(CFGID_SIM_TIME_LIMIT, "sim-time-limit", "s", NULL, "Stops the simulation when simulation time reaches the given limit. The default is no limit.");
Register_PerRunConfigEntryU(CFGID_CPU_TIME_LIMIT, "cpu-time-limit", "s", NULL, "Stops the simulation when CPU usage has reached the given limit. The default is no limit.");
Register_PerRunConfigEntry(CFGID_FINGERPRINT, "fingerprint", CFG_STRING, NULL, "The expected fingerprint of the simulation. When provided, a fingerprint will be calculated from the simulation event times and other quantities during simulation, and checked against the given one. Fingerprints are suitable for crude regression tests. As fingerprints occasionally differ across platforms, more than one fingerprint values can be specified here, separated by spaces, and a match with any of them will be accepted. To calculate the initial fingerprint, enter any dummy string (such as \"none\"), and run the simulation.");
Register_PerRunConfigEntry(CFGID_NUM_RNGS, "num-rngs", CFG_INT, "1", "The number of random number generators.");
Register_PerRunConfigEntry(CFGID_RNG_CLASS, "rng-class", CFG_STRING, "cMersenneTwister", "The random number generator class to be used. It can be `cMersenneTwister', `cLCG32', `cAkaroaRNG', or you can use your own RNG class (it must be subclassed from cRNG).");
Register_PerRunConfigEntry(CFGID_SEED_SET, "seed-set", CFG_INT, "${runnumber}", "Selects the kth set of automatic random number seeds for the simulation. Meaningful values include ${repetition} which is the repeat loop counter (see repeat= key), and ${runnumber}.");
Register_PerRunConfigEntry(CFGID_RESULT_DIR, "result-dir", CFG_STRING, "results", "Value for the ${resultdir} variable, which is used as the default directory for result files (output vector file, output scalar file, eventlog file, etc.)");
Register_PerRunConfigEntry(CFGID_RECORD_EVENTLOG, "record-eventlog", CFG_BOOL, "false", "Enables recording an eventlog file, which can be later visualized on a sequence chart. See eventlog-file= option too.");
Register_PerObjectConfigEntry(CFGID_PARTITION_ID, "partition-id", CFG_STRING, NULL, "With parallel simulation: in which partition the module should be instantiated. Specify numeric partition ID, or a comma-separated list of partition IDs for compound modules that span across multiple partitions. Ranges (\"5..9\") and \"*\" (=all) are accepted too.");
Register_PerObjectConfigEntry(CFGID_RNG_K, "rng-%", CFG_INT, "", "Maps a module-local RNG to one of the global RNGs. Example: **.gen.rng-1=3 maps the local RNG 1 of modules matching `**.gen' to the global RNG 3. The default is one-to-one mapping.");

#define STRINGIZE0(x) #x
#define STRINGIZE(x) STRINGIZE0(x)

static const char *compilerInfo =
    #if defined __GNUC__
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

    #ifdef NDEBUG
    " RELEASE"
    #else
    " DEBUG"
    #endif

    " simtime_t=%s"
    " large-file-support=%s"
    ;

static const char *buildOptions = ""
    #ifdef USE_DOUBLE_SIMTIME
    " USE_DOUBLE_SIMTIME"
    #endif

    #ifdef WITHOUT_CPACKET
    " WITHOUT_CPACKET"
    #endif

    #ifdef WITH_PARSIM
    " WITH_PARSIM"
    #endif

    #ifdef WITH_MPI
    " WITH_MPI"
    #endif

    #ifdef WITH_NETBUILDER
    " WITH_NETBUILDER"
    #endif

    #ifdef WITH_AKAROA
    " WITH_AKAROA"
    #endif

    #ifdef XMLPARSER
    " XMLPARSER=" STRINGIZE(XMLPARSER)
    #endif
    ;


EnvirBase::EnvirBase()
{
    args = NULL;
    cfg = NULL;
    xmlcache = NULL;

    eventlogmgr = NULL;
    outvectormgr = NULL;
    outscalarmgr = NULL;
    snapshotmgr = NULL;

    num_rngs = 0;
    rngs = NULL;

    scheduler = NULL;
#ifdef WITH_PARSIM
    parsimcomm = NULL;
    parsimpartition = NULL;
#endif

    initialized = false;
}

EnvirBase::~EnvirBase()
{
    delete args;
    delete cfg;
    delete xmlcache;

    delete outvectormgr;
    delete outscalarmgr;
    delete snapshotmgr;

    for (int i = 0; i < num_rngs; i++)
         delete rngs[i];
    delete [] rngs;

    delete scheduler;
#ifdef WITH_PARSIM
    delete parsimcomm;
    delete parsimpartition;
#endif
}

int EnvirBase::run(int argc, char *argv[], cConfiguration *configobject)
{
    args = new ArgList();
    args->parse(argc, argv, "h?f:u:l:c:r:p:n:x:gGv");  //TODO share spec with BootEnv!
    cfg = configobject;

    setup();
    int exitcode = run();
    shutdown();
    return exitcode;
}

void EnvirBase::setup()
{
    // handle -h command-line option
    if (args->optionGiven('h'))
    {
        const char *category = args->optionValue('h',0);
        if (!category)
            printHelp();
        else
            dumpComponentList(category);
        return;  // don't set initialized==true
    }

    if (args->optionGiven('v'))
    {
        struct opp_stat_t statbuf;
        ev << "\n";
        ev << "Build: " OMNETPP_RELEASE " " OMNETPP_BUILDID << "\n";
        ev << "Compiler: " << compilerInfo << "\n";
        ev << "Options: " << opp_stringf(buildInfoFormat,
                                         8*sizeof(void*),
                                         opp_typename(typeid(simtime_t)),
                                         sizeof(statbuf.st_size)>=8 ? "yes" : "no");
        ev << buildOptions << "\n" << "\n";
        return;  // don't set initialized==true
    }

    try
    {
        // ensure correct numeric format in output files
        setPosixLocale();

        // set opt_* variables from ini file(s)
        readOptions();

        // initialize coroutine library
        if (TOTAL_STACK_SIZE!=0 && opt_total_stack<=MAIN_STACK_SIZE+4096)
        {
            ev.printf("Total stack size %d increased to %d\n", opt_total_stack, MAIN_STACK_SIZE);
            opt_total_stack = MAIN_STACK_SIZE+4096;
        }
        cCoroutine::init(opt_total_stack, MAIN_STACK_SIZE);

        // install XML document cache
        xmlcache = new cXMLDocCache();

        // install output vector manager
        CREATE_BY_CLASSNAME(outvectormgr, opt_outputvectormanager_class.c_str(), cOutputVectorManager, "output vector manager");

        // install output scalar manager
        CREATE_BY_CLASSNAME(outscalarmgr, opt_outputscalarmanager_class.c_str(), cOutputScalarManager, "output scalar manager");

        // install snapshot manager
        CREATE_BY_CLASSNAME(snapshotmgr, opt_snapshotmanager_class.c_str(), cSnapshotManager, "snapshot manager");

        // set up for sequential or distributed execution
        if (!opt_parsim)
        {
            // sequential
            CREATE_BY_CLASSNAME(scheduler, opt_scheduler_class.c_str(), cScheduler, "event scheduler");
            scheduler->setSimulation(&simulation);
            simulation.setScheduler(scheduler);
        }
        else
        {
#ifdef WITH_PARSIM
            // parsim: create components
            CREATE_BY_CLASSNAME(parsimcomm, opt_parsimcomm_class.c_str(), cParsimCommunications, "parallel simulation communications layer");
            parsimpartition = new cParsimPartition();
            cParsimSynchronizer *parsimsynchronizer;
            CREATE_BY_CLASSNAME(parsimsynchronizer, opt_parsimsynch_class.c_str(), cParsimSynchronizer, "parallel simulation synchronization layer");

            // wire them together (note: 'parsimsynchronizer' is also the scheduler for 'simulation')
            parsimpartition->setContext(&simulation, parsimcomm, parsimsynchronizer);
            parsimsynchronizer->setContext(&simulation, parsimpartition, parsimcomm);
            simulation.setScheduler(parsimsynchronizer);
            scheduler = parsimsynchronizer;

            // initialize them
            parsimcomm->init();
#else
            throw cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif
        }

        // load NED files from folders on the NED path. Note: NED path is taken
        // from the "-n" command-line option or the NEDPATH variable ("-n" takes
        // precedence), and the "ned-path=" config entry gets appended to it.
        // If the result is still empty, we fall back to "." -- this is needed
        // for single-directory models to work
        const char *nedpath1 = args->optionValue('n',0);
        if (!nedpath1)
            nedpath1 = getenv("NEDPATH");
        std::string nedpath2 = getConfig()->getAsString(CFGID_NED_PATH, "");
        std::string nedpath = opp_join(";", nedpath1, nedpath2.c_str());
        if (nedpath.empty())
            nedpath = ".";

        StringTokenizer tokenizer(nedpath.c_str(), PATH_SEPARATOR);
        std::set<std::string> foldersloaded;
        while (tokenizer.hasMoreTokens())
        {
            const char *folder = tokenizer.nextToken();
            if (foldersloaded.find(folder)==foldersloaded.end())
            {
                ev.printf("Loading NED files from %s:", folder); ev.flush();
                int count = simulation.loadNedSourceFolder(folder);
                ev.printf(" %d\n", count);
                foldersloaded.insert(folder);
            }
        }
        simulation.doneLoadingNedFiles();

        // load NED files from the "preload-ned-files=" config entry.
        // XXX This code is now obsolete, as we load NED files from NEDPATH trees
        //std::vector<std::string> nedfiles = getConfig()->getAsFilenames(CFGID_PRELOAD_NED_FILES);
        //if (!nedfiles.empty())
        //{
        //    // iterate through file names
        //    ev.printf("\n");
        //    for (int i=0; i<(int)nedfiles.size(); i++)
        //    {
        //        const char *fname = nedfiles[i].c_str();
        //        if (fname[0]=='@' && fname[1]=='@')
        //            globAndLoadListFile(fname+2, true);
        //        else if (fname[0]=='@')
        //            globAndLoadListFile(fname+1, false);
        //        else if (fname[0])
        //            globAndLoadNedFile(fname);
        //    }
        //    simulation.doneLoadingNedFiles();
        //}
    }
    catch (std::exception& e)
    {
        displayError(e);
        return;  // don't set initialized to true
    }
    initialized = true;
}

void EnvirBase::printHelp()
{
    ev << "\n";
    ev << "Command line options:\n";
    ev << "  <inifile> or -f <inifile>\n";
    ev << "                Use the given ini file instead of omnetpp.ini. More than one\n";
    ev << "                ini files can be loaded this way.\n";
    ev << "  -u <ui>       Selects the user interface. Standard choices are Cmdenv\n";
    ev << "                and Tkenv. To make a user interface available, you need\n";
    ev << "                to link the simulation executable with the Cmdenv/Tkenv\n";
    ev << "                library, or load it as shared library via the -l option.\n";
    ev << "  -n <nedpath>  When present, overrides the NEDPATH environment variable.\n";
    ev << "  -l <library>  Load the specified shared library (.so or .dll) on startup.\n";
    ev << "                The file name should be given without the .so or .dll suffix\n";
    ev << "                (it will be appended automatically.) The loaded module may\n";
    ev << "                contain simple modules, plugins, etc. Multiple -l options\n";
    ev << "                can be present.\n";
    ev << "  --<configuration-key> <value>\n";
    ev << "                Any configuration option can be specified on the command\n";
    ev << "                line, and they take precedence over setting specified in the\n";
    ev << "                ini file(s). Examples:\n";
    ev << "                      --debug-on-errors true\n";
    ev << "                      --record-eventlog true\n";
    ev << "                      --sim-time-limit 1000s\n";
    ev << "  -v            Print version and build info.\n";
    ev << "  -h            Print this help and exit.\n";
    ev << "  -h <category> Lists registered components:\n";
    ev << "    -h config         Prints the list of available config options\n";
    ev << "    -h configdetails  Prints the list of available config options, with\n";
    ev << "                      their documentation\n";
    ev << "    -h userinterfaces Lists available user interfaces (see -u option)\n";
    ev << "    -h classes        Lists registered C++ classes (including module classes)\n";
    ev << "    -h classdesc      Lists C++ classes that have associated reflection\n";
    ev << "                      information (needed for Tkenv inspectors)\n";
    ev << "    -h nedfunctions   Lists registered NED functions\n";
    ev << "    -h units          Lists recognized physical units\n";
    ev << "    -h enums          Lists registered enums\n";
    ev << "    -h all            Union of all the above\n";
    ev << "\n";

    // print specific help for each user interface
    cRegistrationList *table = omnetapps.getInstance();
    table->sort();
    for (int i=0; i<table->size(); i++)
    {
        // instantiate the ui, call printUISpecificHelp(), then dispose.
        // note: their ctors are not supposed to do anything but trivial member initializations
        cOmnetAppRegistration *appreg = check_and_cast<cOmnetAppRegistration *>(table->get(i));
        cEnvir *app = appreg->createOne();
        if (dynamic_cast<EnvirBase *>(app))
            ((EnvirBase *)app)->printUISpecificHelp();
        delete app;
    }
}

void EnvirBase::dumpComponentList(const char *category)
{
    bool wantAll = !strcmp(category, "all");
    bool processed = false;
    if (wantAll || !strcmp(category, "config") || !strcmp(category, "configdetails"))
    {
        processed = true;
        ev << "Supported configuration keys (omnetpp.ini):\n";
        bool printDescriptions = strcmp(category, "configdetails")==0;

        cRegistrationList *table = configKeys.getInstance();
        table->sort();
        for (int i=0; i<table->size(); i++)
        {
            cConfigKey *obj = dynamic_cast<cConfigKey *>(table->get(i));
            ASSERT(obj);
            if (!printDescriptions) ev << "  ";
            if (obj->isPerObject()) ev << "<object-full-path>.";
            ev << obj->getName() << "=";
            ev << "<" << cConfigKey::getTypeName(obj->getType()) << ">";
            if (obj->getUnit())
                ev << ", unit=\"" << obj->getUnit() << "\"";
            if (obj->getDefaultValue())
                ev << ", default:" << obj->getDefaultValue() << "";
            ev << "; " << (obj->isGlobal() ? "global" : obj->isPerObject() ? "per-object" : "per-run") << " setting";
            ev << "\n";
            if (printDescriptions && !opp_isempty(obj->getDescription()))
                ev << opp_indentlines(opp_breaklines(obj->getDescription(),75).c_str(), "    ") << "\n";
            if (printDescriptions) ev << "\n";
        }
        ev << "\n";

        ev << "Predefined variables that can be used in config values:\n";
        std::vector<const char *> v = getConfig()->getPredefinedVariableNames();
        for (int i=0; i<(int)v.size(); i++)
        {
            if (!printDescriptions) ev << "  ";
            ev << "${" << v[i] << "}\n";
            const char *desc = getConfig()->getVariableDescription(v[i]);
            if (printDescriptions && !opp_isempty(desc))
                ev << opp_indentlines(opp_breaklines(desc,75).c_str(), "    ") << "\n";
        }
        ev << "\n";
    }
    if (!strcmp(category, "jconfig")) // internal undocumented option, for maintenance purposes
    {
        // generate Java code for ConfigurationRegistry.java in the IDE
        processed = true;
        ev << "Supported configuration keys (as Java code):\n";
        cRegistrationList *table = configKeys.getInstance();
        table->sort();
        for (int i=0; i<table->size(); i++)
        {
            cConfigKey *key = dynamic_cast<cConfigKey *>(table->get(i));
            ASSERT(key);

            std::string id = "CFGID_";
            for (const char *s = key->getName(); *s; s++)
                id.append(1, opp_isalpha(*s) ? opp_toupper(*s) : *s=='-' ? '_' : *s=='%' ? 'n' : *s);
            const char *method = key->isGlobal() ? "addGlobalEntry" :
                                 !key->isPerObject() ? "addPerRunEntry" :
                                 "addPerObjectEntry";
            #define CASE(X)  case cConfigKey::X: typestring = #X; break;
            const char *typestring;
            switch (key->getType()) {
                CASE(CFG_BOOL)
                CASE(CFG_INT)
                CASE(CFG_DOUBLE)
                CASE(CFG_STRING)
                CASE(CFG_FILENAME)
                CASE(CFG_FILENAMES)
                CASE(CFG_CUSTOM)
            }
            #undef CASE

            ev << "    public static final ConfigKey " << id << " = ";
            ev << method << (key->getUnit() ? "U" : "") << "(\n";
            ev << "        \"" << key->getName() << "\", ";
            if (!key->getUnit())
                ev << typestring << ", ";
            else
                ev << "\"" << key->getUnit() << "\", ";
            if (!key->getDefaultValue())
                ev << "null";
            else
                ev << "\"" << opp_replacesubstring(key->getDefaultValue(), "\"", "\\\"", true) << "\"";
            ev << ",\n";

            std::string desc = key->getDescription();
            desc = opp_replacesubstring(desc.c_str(), "\n", "\\n\n", true); // keep explicit line breaks
            desc = opp_breaklines(desc.c_str(), 75);  // break long lines
            desc = opp_replacesubstring(desc.c_str(), "\"", "\\\"", true);
            desc = opp_replacesubstring(desc.c_str(), "\n", " \" +\n\"", true);
            desc = opp_replacesubstring(desc.c_str(), "\\n \"", "\\n\"", true); // remove bogus space after explicit line breaks
            desc = "\"" + desc + "\"";

            ev << opp_indentlines(desc.c_str(), "        ") << ");\n";
        }
        ev << "\n";

        std::vector<const char *> vars = getConfig()->getPredefinedVariableNames();
        for (int i=0; i<(int)vars.size(); i++)
        {
            opp_string id = vars[i];
            opp_strupr(id.buffer());
            const char *desc = getConfig()->getVariableDescription(vars[i]);
            ev << "    public static final String CFGVAR_" << id << " = addConfigVariable(";
            ev << "\"" << vars[i] << "\", \"" << opp_replacesubstring(desc, "\"", "\\\"", true) << "\");\n";
        }
        ev << "\n";
    }
    if (wantAll || !strcmp(category, "classes"))
    {
        processed = true;
        ev << "Registered C++ classes, including modules, channels and messages:\n";
        cRegistrationList *table = classes.getInstance();
        table->sort();
        for (int i=0; i<table->size(); i++)
        {
            cObject *obj = table->get(i);
            ev << "  class " << obj->getFullName() << "\n";
        }
        ev << "Note: if your class is not listed, it needs to be registered in the\n";
        ev << "C++ code using Define_Module(), Define_Channel() or Register_Class().\n";
        ev << "\n";
    }
    if (wantAll || !strcmp(category, "classdesc"))
    {
        processed = true;
        ev << "Classes that have associated reflection information (needed for Tkenv inspectors):\n";
        cRegistrationList *table = classDescriptors.getInstance();
        table->sort();
        for (int i=0; i<table->size(); i++)
        {
            cObject *obj = table->get(i);
            ev << "  class " << obj->getFullName() << "\n";
        }
        ev << "\n";
    }
    if (wantAll || !strcmp(category, "nedfunctions"))
    {
        processed = true;
        ev << "Functions that can be used in NED expressions and in omnetpp.ini:\n";
        cRegistrationList *table = nedFunctions.getInstance();
        table->sort();
        for (int i=0; i<table->size(); i++)
        {
            cObject *obj = table->get(i);
            ev << "  " << obj->getFullName() << " : " << obj->info() << "\n";
        }
        ev << "\n";
    }
    if (wantAll || !strcmp(category, "units"))
    {
        processed = true;
        ev << "Recognized physical units (note: other units can be used as well, only\n";
        ev << "no automatic conversion will be available for them):\n";
        std::vector<const char *> units = UnitConversion::getAllUnits();
        for (int i=0; i<(int)units.size(); i++)
        {
            const char *u = units[i];
            const char *bu = UnitConversion::getBaseUnit(u);
            ev << "  " << u << "\t" << UnitConversion::getLongName(u);
            if (opp_strcmp(u,bu)!=0)
                ev << "\t" << UnitConversion::convertUnit(1,u,bu) << bu;
            ev << "\n";
        }
        ev << "\n";
    }
    if (wantAll || !strcmp(category, "enums"))
    {
        processed = true;
        ev << "Enums defined in .msg files\n";
        cRegistrationList *table = enums.getInstance();
        table->sort();
        for (int i=0; i<table->size(); i++)
        {
            cObject *obj = table->get(i);
            ev << "  " << obj->getFullName() << " : " << obj->info() << "\n";
        }
        ev << "\n";
    }
    if (wantAll || !strcmp(category, "userinterfaces"))
    {
        processed = true;
        ev << "User interfaces loaded:\n";
        cRegistrationList *table = omnetapps.getInstance();
        table->sort();
        for (int i=0; i<table->size(); i++)
        {
            cObject *obj = table->get(i);
            ev << "  " << obj->getFullName() << " : " << obj->info() << "\n";
        }
    }

    if (!processed)
        throw cRuntimeError("Unrecognized category for '-h' option: %s", category);
}

int EnvirBase::getParsimProcId() const
{
#ifdef WITH_PARSIM
    return parsimcomm ? parsimcomm->getProcId() : 0;
#else
    return 0;
#endif
}

int EnvirBase::getParsimNumPartitions() const
{
#ifdef WITH_PARSIM
    return parsimcomm ? parsimcomm->getNumPartitions() : 0;
#else
    return 0;
#endif
}


void EnvirBase::shutdown()
{
    if (!initialized)
        return;

    try
    {
        simulation.deleteNetwork();
#ifdef WITH_PARSIM
        if (opt_parsim && parsimpartition)
            parsimpartition->shutdown();
#endif
    }
    catch (std::exception& e)
    {
        displayError(e);
    }
}

void EnvirBase::startRun()
{
    runid = getConfig()->getVariable(CFGVAR_RUNID);

    resetClock();
    outvectormgr->startRun();
    outscalarmgr->startRun();
    snapshotmgr->startRun();
    if (eventlogmgr)
        eventlogmgr->startRun(runid.c_str());
    if (opt_parsim)
    {
#ifdef WITH_PARSIM
        parsimpartition->startRun();
#endif
    }
    simulation.getScheduler()->startRun();
    simulation.startRun();
    flushLastLine();
}

void EnvirBase::endRun()
{
    // reverse order as startRun()
    simulation.endRun();
    simulation.getScheduler()->endRun();
    if (opt_parsim)
    {
#ifdef WITH_PARSIM
        parsimpartition->endRun();
#endif
    }
    if (eventlogmgr) {
        eventlogmgr->endRun();
        delete eventlogmgr;
        eventlogmgr = NULL;
    }
    snapshotmgr->endRun();
    outscalarmgr->endRun();
    outvectormgr->endRun();
}

//-------------------------------------------------------------

void EnvirBase::readParameter(cPar *par)
{
    ASSERT(!par->isSet());  // must be unset at this point

    // get it from the ini file
    std::string moduleFullPath = par->getOwner()->getFullPath();
    const char *str = getConfig()->getParameterValue(moduleFullPath.c_str(), par->getName(), par->containsValue());

/* XXX hack to use base directory for resolving xml files location has been commented out
 * FIXME a solution needs to be worked out!
    if (str[0]=='x' && !strncmp(str,"xmldoc",6) && !opp_isalnum(str[6]))
    {
        // Make XML file location relative to the ini file in which it occurs.
        // Find substring between first two quotes (that is, the XML filename),
        // and prefix it with the directory.
        const char *begQuote = strchr(str+6,'"');
        if (!begQuote)
            return std::string(str);
        const char *endQuote = strchr(begQuote+1,'"');
        while (endQuote && *(endQuote-1)=='\\' && *(endQuote-2)!='\\')
            endQuote = strchr(endQuote+1,'"');
        if (!endQuote)
            return std::string(str);
        std::string fname(begQuote+1, endQuote-begQuote-1);
        const char *baseDir = getConfig()->getBaseDirectoryFor(NULL, "Parameters", parname);
        fname = tidyFilename(concatDirAndFile(baseDir, fname.c_str()).c_str(),true);
        std::string ret = std::string(str, begQuote-str+1) + fname + endQuote;
        //XXX use "ret" further!!!
    }
*/

    if (opp_strcmp(str, "default")==0)
    {
        ASSERT(par->containsValue());  // cConfiguration should not return "=default" lines for params that have no default value
        par->acceptDefault();
    }
    else if (opp_strcmp(str, "ask")==0)
    {
        askParameter(par);
    }
    else if (!opp_isempty(str))
    {
        par->parse(str);
    }
    else
    {
        // str empty: no value in the ini file
        if (par->containsValue())
            par->acceptDefault();
        else
            askParameter(par);
    }
}

bool EnvirBase::isModuleLocal(cModule *parentmod, const char *modname, int index)
{
#ifdef WITH_PARSIM
    if (!opt_parsim)
       return true;

    // toplevel module is local everywhere
    if (!parentmod)
       return true;

    // find out if this module is (or has any submodules that are) on this partition
    char parname[MAX_OBJECTFULLPATH];
    if (index<0)
        sprintf(parname,"%s.%s", parentmod->getFullPath().c_str(), modname);
    else
        sprintf(parname,"%s.%s[%d]", parentmod->getFullPath().c_str(), modname, index); //FIXME this is incorrectly chosen for non-vector modules too!
    std::string procIds = getConfig()->getAsString(parname, CFGID_PARTITION_ID, "");
    if (procIds.empty())
    {
        // modules inherit the setting from their parents, except when the parent is the system getModule(the network) itself
        if (!parentmod->getParentModule())
            throw cRuntimeError("incomplete partitioning: missing value for '%s'",parname);
        // "true" means "inherit", because an ancestor which answered "false" doesn't get recursed into
        return true;
    }
    else if (strcmp(procIds.c_str(), "*") == 0)
    {
        // present on all partitions (provided that ancestors have "*" set as well)
        return true;
    }
    else
    {
        // we expect a partition Id (or partition Ids, separated by commas) where this
        // module needs to be instantiated. So we return true if any of the numbers
        // is the Id of the local partition, otherwise false.
        EnumStringIterator procIdIter(procIds.c_str());
        if (procIdIter.hasError())
            throw cRuntimeError("wrong partitioning: syntax error in value '%s' for '%s' "
                                "(allowed syntax: '', '*', '1', '0,3,5-7')",
                                procIds.c_str(), parname);
        int numPartitions = parsimcomm->getNumPartitions();
        int myProcId = parsimcomm->getProcId();
        for (; procIdIter()!=-1; procIdIter++)
        {
            if (procIdIter() >= numPartitions)
                throw cRuntimeError("wrong partitioning: value %d too large for '%s' (total partitions=%d)",
                                    procIdIter(), parname, numPartitions);
            if (procIdIter() == myProcId)
                return true;
        }
        return false;
    }
#else
    return true;
#endif
}

cXMLElement *EnvirBase::getXMLDocument(const char *filename, const char *path)
{
    cXMLElement *documentnode = xmlcache->getDocument(filename);
    assert(documentnode);
    if (path)
    {
        ModNameParamResolver resolver(simulation.getContextModule()); // resolves $MODULE_NAME etc in XPath expr.
        return cXMLElement::getDocumentElementByPath(documentnode, path, &resolver);
    }
    else
    {
        // returns the root element (child of the document node)
        return documentnode->getFirstChild();
    }
}

cConfiguration *EnvirBase::getConfig()
{
    return cfg;
}

//-------------------------------------------------------------

void EnvirBase::bubble(cComponent *component, const char *text)
{
    if (eventlogmgr)
        eventlogmgr->bubble(component, text);
}

void EnvirBase::objectDeleted(cObject *object)
{
}

void EnvirBase::simulationEvent(cMessage *msg)
{
    if (eventlogmgr)
        eventlogmgr->simulationEvent(msg);
}

void EnvirBase::beginSend(cMessage *msg)
{
    if (eventlogmgr)
        eventlogmgr->beginSend(msg);
}

void EnvirBase::messageScheduled(cMessage *msg)
{
    if (eventlogmgr)
        eventlogmgr->messageScheduled(msg);
}

void EnvirBase::messageCancelled(cMessage *msg)
{
    if (eventlogmgr)
        eventlogmgr->messageCancelled(msg);
}

void EnvirBase::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    if (eventlogmgr)
        eventlogmgr->messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);
}

void EnvirBase::messageSendHop(cMessage *msg, cGate *srcGate)
{
    if (eventlogmgr)
        eventlogmgr->messageSendHop(msg, srcGate);
}

void EnvirBase::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    if (eventlogmgr)
        eventlogmgr->messageSendHop(msg, srcGate, propagationDelay, transmissionDelay);
}

void EnvirBase::endSend(cMessage *msg)
{
    if (eventlogmgr)
        eventlogmgr->endSend(msg);
}

void EnvirBase::messageDeleted(cMessage *msg)
{
    if (eventlogmgr)
        eventlogmgr->messageDeleted(msg);
}

void EnvirBase::componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va)
{
    if (eventlogmgr)
        eventlogmgr->componentMethodBegin(from, to, methodFmt, va);
}

void EnvirBase::componentMethodEnd()
{
    if (eventlogmgr)
        eventlogmgr->componentMethodEnd();
}

void EnvirBase::moduleCreated(cModule *newmodule)
{
    if (eventlogmgr)
        eventlogmgr->moduleCreated(newmodule);
}

void EnvirBase::moduleDeleted(cModule *module)
{
    if (eventlogmgr)
        eventlogmgr->moduleDeleted(module);
}

void EnvirBase::moduleReparented(cModule *module, cModule *oldparent)
{
    if (eventlogmgr)
        eventlogmgr->moduleReparented(module, oldparent);
}

void EnvirBase::gateCreated(cGate *newgate)
{
    if (eventlogmgr)
        eventlogmgr->gateCreated(newgate);
}

void EnvirBase::gateDeleted(cGate *gate)
{
    if (eventlogmgr)
        eventlogmgr->gateDeleted(gate);
}

void EnvirBase::connectionCreated(cGate *srcgate)
{
    if (eventlogmgr)
        eventlogmgr->connectionCreated(srcgate);
}

void EnvirBase::connectionDeleted(cGate *srcgate)
{
    if (eventlogmgr)
        eventlogmgr->connectionDeleted(srcgate);
}

void EnvirBase::displayStringChanged(cComponent *component)
{
    if (eventlogmgr)
        eventlogmgr->displayStringChanged(component);
}

void EnvirBase::sputn(const char *s, int n)
{
    if (eventlogmgr)
        eventlogmgr->sputn(s, n);
}

void EnvirBase::undisposedObject(cObject *obj)
{
    if (opt_print_undisposed)
        ::printf("undisposed object: (%s) %s -- check module destructor\n", obj->getClassName(), obj->getFullPath().c_str());
}

//-------------------------------------------------------------

void EnvirBase::processFileName(opp_string& fname)
{
    std::string text = fname.c_str();

    // append ".<hostname>.<pid>" if requested
    // (note: parsimProcId cannot be appended because of initialization order)
    if (opt_fname_append_host)
    {
        const char *hostname=getenv("HOST");
        if (!hostname)
            hostname=getenv("HOSTNAME");
        if (!hostname)
            hostname=getenv("COMPUTERNAME");
        if (!hostname)
            throw cRuntimeError("Cannot append hostname to file name `%s': no HOST, HOSTNAME "
                                "or COMPUTERNAME (Windows) environment variable",
                                fname.c_str());
        int pid = getpid();

        // append
        text += opp_stringf(".%s.%d", hostname, pid);
    }
    fname = text.c_str();
}

void EnvirBase::readOptions()
{
    cConfiguration *cfg = getConfig();

    opt_total_stack = (size_t) cfg->getAsDouble(CFGID_TOTAL_STACK, TOTAL_STACK_SIZE);
    opt_parsim = cfg->getAsBool(CFGID_PARALLEL_SIMULATION);
    if (!opt_parsim)
    {
        opt_scheduler_class = cfg->getAsString(CFGID_SCHEDULER_CLASS);
    }
    else
    {
#ifdef WITH_PARSIM
        opt_parsimcomm_class = cfg->getAsString(CFGID_PARSIM_COMMUNICATIONS_CLASS);
        opt_parsimsynch_class = cfg->getAsString(CFGID_PARSIM_SYNCHRONIZATION_CLASS);
#else
        throw cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif
    }

    opt_outputvectormanager_class = cfg->getAsString(CFGID_OUTPUTVECTORMANAGER_CLASS);
    opt_outputscalarmanager_class = cfg->getAsString(CFGID_OUTPUTSCALARMANAGER_CLASS);
    opt_snapshotmanager_class = cfg->getAsString(CFGID_SNAPSHOTMANAGER_CLASS);

    opt_fname_append_host = cfg->getAsBool(CFGID_FNAME_APPEND_HOST);

    ev.debug_on_errors = cfg->getAsBool(CFGID_DEBUG_ON_ERRORS);
    opt_print_undisposed = cfg->getAsBool(CFGID_PRINT_UNDISPOSED);

    int scaleexp = (int) cfg->getAsInt(CFGID_SIMTIME_SCALE);
    SimTime::setScaleExp(scaleexp);

    // note: this is read per run as well, but Tkenv needs its value on startup too
    opt_inifile_network_dir = cfg->getConfigEntry(CFGID_NETWORK->getName()).getBaseDirectory();

    // other options are read on per-run basis
}

void EnvirBase::readPerRunOptions()
{
    cConfiguration *cfg = getConfig();

    // get options from ini file
    opt_network_name = cfg->getAsString(CFGID_NETWORK);
    opt_inifile_network_dir = cfg->getConfigEntry(CFGID_NETWORK->getName()).getBaseDirectory();
    opt_warnings = cfg->getAsBool(CFGID_WARNINGS);
    opt_simtimelimit = cfg->getAsDouble(CFGID_SIM_TIME_LIMIT);
    opt_cputimelimit = (long) cfg->getAsDouble(CFGID_CPU_TIME_LIMIT);
    opt_fingerprint = cfg->getAsString(CFGID_FINGERPRINT);
    opt_num_rngs = cfg->getAsInt(CFGID_NUM_RNGS);
    opt_rng_class = cfg->getAsString(CFGID_RNG_CLASS);
    opt_seedset = cfg->getAsInt(CFGID_SEED_SET);
    opt_record_eventlog = cfg->getAsBool(CFGID_RECORD_EVENTLOG);

    // install hasher object
    if (!opt_fingerprint.empty())
        simulation.setHasher(new cHasher());
    else
        simulation.setHasher(NULL);

    // run RNG self-test on RNG class selected for this run
    cRNG *testrng;
    CREATE_BY_CLASSNAME(testrng, opt_rng_class.c_str(), cRNG, "random number generator");
    testrng->selfTest();
    delete testrng;

    // set up RNGs
    int i;
    for (i=0; i<num_rngs; i++)
         delete rngs[i];
    delete [] rngs;

    num_rngs = opt_num_rngs;
    rngs = new cRNG *[num_rngs];
    for (i=0; i<num_rngs; i++)
    {
        cRNG *rng;
        CREATE_BY_CLASSNAME(rng, opt_rng_class.c_str(), cRNG, "random number generator");
        rngs[i] = rng;
        rngs[i]->initialize(opt_seedset, i, num_rngs, getParsimProcId(), getParsimNumPartitions(), getConfig());
    }

    // init nextuniquenumber -- startRun() is too late because simple module ctors have run by then
    nextuniquenumber = 0;
#ifdef WITH_PARSIM
    if (opt_parsim)
        nextuniquenumber = (unsigned)parsimcomm->getProcId() * ((~0UL) / (unsigned)parsimcomm->getNumPartitions());
#endif

    if (opt_record_eventlog)
    {
        // open message log file. Note: in startRun() it would be too late,
        // because modules have already been created by then
        eventlogmgr = new EventlogFileManager();
        eventlogmgr->setup();
    }

}

//void EnvirBase::globAndLoadNedFile(const char *fnamepattern)
//{
//    try {
//        FileGlobber glob(fnamepattern);
//        const char *fname;
//        while ((fname=glob.getNext())!=NULL)
//        {
//            ev.printf("Loading NED file: %s\n", fname);
//            simulation.loadNedFile(fname);
//        }
//    }
//    catch (std::runtime_error e) {
//        throw cRuntimeError(e.what());
//    }
//}
//
//void EnvirBase::globAndLoadListFile(const char *fnamepattern, bool istemplistfile)
//{
//    try {
//        FileGlobber glob(fnamepattern);
//        const char *fname;
//        while ((fname=glob.getNext())!=NULL)
//        {
//            processListFile(fname, istemplistfile);
//        }
//    }
//     catch (std::runtime_error e) {
//        throw cRuntimeError(e.what());
//    }
//}
//
//void EnvirBase::processListFile(const char *listfilename, bool istemplistfile)
//{
//    std::ifstream in(listfilename, std::ios::in);
//    if (in.fail())
//        throw cRuntimeError("Cannot open list file '%s'",listfilename);
//
//    ev.printf("Processing listfile: %s\n", listfilename);
//
//    // @listfile: files should be relative to list file, so try cd into list file's directory
//    // @@listfile (temp=true): don't cd.
//    PushDir d(istemplistfile ? NULL : directoryOf(listfilename).c_str());
//
//    const int maxline=1024;
//    char line[maxline];
//    while (in.getline(line, maxline))
//    {
//        int len = in.gcount();
//        if (line[len-1]=='\n')
//            line[len-1] = '\0';
//        const char *fname = line;
//
//        if (fname[0]=='@' && fname[1]=='@')
//            globAndLoadListFile(fname+2, true);
//        else if (fname[0]=='@')
//            globAndLoadListFile(fname+1, false);
//        else if (fname[0] && fname[0]!='#')
//            globAndLoadNedFile(fname);
//    }
//
//    if (in.bad())
//        throw cRuntimeError("Error reading list file '%s'",listfilename);
//    in.close();
//}

//-------------------------------------------------------------

int EnvirBase::getNumRNGs() const
{
    return num_rngs;
}

cRNG *EnvirBase::getRNG(int k)
{
    if (k<0 || k>=num_rngs)
        throw cRuntimeError("RNG index %d is out of range (num-rngs=%d, check the configuration)", k, num_rngs);
    return rngs[k];
}

void EnvirBase::getRNGMappingFor(cComponent *component)
{
    cConfiguration *cfg = getConfig();
    std::string componentFullPath = component->getFullPath();
    std::vector<const char *> suffixes = cfg->getMatchingPerObjectConfigKeySuffixes(componentFullPath.c_str(), "rng-*"); // CFGID_RNG_K
    if (suffixes.size()==0)
        return;

    // extract into tmpmap[]
    int mapsize=0;
    int tmpmap[100];
    for (int i=0; i<(int)suffixes.size(); i++)
    {
        const char *suffix = suffixes[i];  // contains "rng-1", "rng-4" or whichever has been found in the config for this module/channel
        const char *value = cfg->getPerObjectConfigValue(componentFullPath.c_str(), suffix);
        ASSERT(value!=NULL);
        char *s1, *s2;
        int modRng = strtol(suffix+strlen("rng-"), &s1, 10);
        int physRng = strtol(value, &s2, 10);
        if (*s1!='\0' || *s2!='\0')
            throw cRuntimeError("Configuration error: %s=%s for module/channel %s: "
                                "numeric RNG indices expected",
                                suffix, value, component->getFullPath().c_str());

        if (physRng>getNumRNGs())
            throw cRuntimeError("Configuration error: rng-%d=%d of module/channel %s: "
                                "RNG index out of range (num-rngs=%d)",
                                modRng, physRng, component->getFullPath().c_str(), getNumRNGs());
        if (modRng>=mapsize)
        {
            if (modRng>=100)
                throw cRuntimeError("Configuration error: rng-%d=... of module/channel %s: "
                                    "local RNG index out of supported range 0..99",
                                    modRng, component->getFullPath().c_str());
            while (mapsize<=modRng)
            {
                tmpmap[mapsize] = mapsize;
                mapsize++;
            }
        }
        tmpmap[modRng] = physRng;
    }

    // install map into the module
    if (mapsize>0)
    {
        int *map = new int[mapsize];
        memcpy(map, tmpmap, mapsize*sizeof(int));
        component->setRNGMap(mapsize, map);
    }
}

//-------------------------------------------------------------

void *EnvirBase::registerOutputVector(const char *modulename, const char *vectorname)
{
    assert(outvectormgr);
    return outvectormgr->registerVector(modulename, vectorname);
}

void EnvirBase::deregisterOutputVector(void *vechandle)
{
    assert(outvectormgr);
    outvectormgr->deregisterVector(vechandle);
}

void EnvirBase::setVectorAttribute(void *vechandle, const char *name, const char *value)
{
    assert(outvectormgr);
    outvectormgr->setVectorAttribute(vechandle, name, value);
}

bool EnvirBase::recordInOutputVector(void *vechandle, simtime_t t, double value)
{
    assert(outvectormgr);
    return outvectormgr->record(vechandle, t, value);
}

//-------------------------------------------------------------

void EnvirBase::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    assert(outscalarmgr);
    outscalarmgr->recordScalar(component, name, value, attributes);
}

void EnvirBase::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    assert(outscalarmgr);
    outscalarmgr->recordStatistic(component, name, statistic, attributes);
}

//-------------------------------------------------------------

std::ostream *EnvirBase::getStreamForSnapshot()
{
    return snapshotmgr->getStreamForSnapshot();
}

void EnvirBase::releaseStreamForSnapshot(std::ostream *os)
{
    snapshotmgr->releaseStreamForSnapshot(os);
}

//-------------------------------------------------------------

unsigned long EnvirBase::getUniqueNumber()
{
    // TBD check for overflow
    return nextuniquenumber++;
}

void EnvirBase::displayError(std::exception& ex)
{
    cException *e = dynamic_cast<cException *>(&ex);
    if (!e || !e->hasContext())
        ev.printfmsg("Error: %s.", ex.what());
    else if (e->getModuleID()==-1)
        ev.printfmsg("Error in component (%s) %s: %s.", e->getContextClassName(), e->getContextFullPath(), e->what());
    else
        ev.printfmsg("Error in module (%s) %s (id=%d): %s.", e->getContextClassName(), e->getContextFullPath(), e->getModuleID(), e->what());
}

void EnvirBase::displayMessage(std::exception& ex)
{
    cException *e = dynamic_cast<cException *>(&ex);
    if (!e || !e->hasContext())
        ev.printfmsg("%s.", ex.what());
    else if (e->getModuleID()==-1)
        ev.printfmsg("Component (%s) %s: %s.", e->getContextClassName(), e->getContextFullPath(), e->what());
    else
        ev.printfmsg("Module (%s) %s (id=%d): %s.", e->getContextClassName(), e->getContextFullPath(), e->getModuleID(), e->what());
}

bool EnvirBase::idle()
{
    return false;
}

int EnvirBase::getArgCount() const
{
    return args->getArgCount();
}

char **EnvirBase::getArgVector() const
{
    return args->getArgVector();
}

//-------------------------------------------------------------

void EnvirBase::resetClock()
{
    timeval now;
    gettimeofday(&now, NULL);
    laststarted = simendtime = simbegtime = now;
    elapsedtime.tv_sec = elapsedtime.tv_usec = 0;
}

void EnvirBase::startClock()
{
    gettimeofday(&laststarted, NULL);
}

void EnvirBase::stopClock()
{
    gettimeofday(&simendtime, NULL);
    elapsedtime = elapsedtime + simendtime - laststarted;
    simulatedtime = simulation.getSimTime();
}

timeval EnvirBase::totalElapsed()
{
    timeval now;
    gettimeofday(&now, NULL);
    return now - laststarted + elapsedtime;
}

void EnvirBase::checkTimeLimits()
{
    if (opt_simtimelimit!=0 && simulation.getSimTime()>=opt_simtimelimit)
         throw cTerminationException(eSIMTIME);
    if (opt_cputimelimit==0) // no limit
         return;
    if (disable_tracing && (simulation.getEventNumber()&0xFF)!=0) // optimize: in Express mode, don't call gettimeofday() on every event
         return;
    timeval now;
    gettimeofday(&now, NULL);
    long elapsedsecs = now.tv_sec - laststarted.tv_sec + elapsedtime.tv_sec;
    if (elapsedsecs>=opt_cputimelimit)
         throw cTerminationException(eREALTIME);
}

void EnvirBase::stoppedWithTerminationException(cTerminationException& e)
{
    // if we're running in parallel and this exception is NOT one we received
    // from other partitions, then notify other partitions
#ifdef WITH_PARSIM
    if (opt_parsim && !dynamic_cast<cReceivedTerminationException *>(&e))
        parsimpartition->broadcastTerminationException(e);
#endif
}

void EnvirBase::stoppedWithException(std::exception& e)
{
    // if we're running in parallel and this exception is NOT one we received
    // from other partitions, then notify other partitions
#ifdef WITH_PARSIM
    if (opt_parsim && !dynamic_cast<cReceivedException *>(&e))
        parsimpartition->broadcastException(e);
#endif
}

void EnvirBase::checkFingerprint()
{
    if (opt_fingerprint.empty() || !simulation.getHasher())
        return;

    int k = 0;
    StringTokenizer tokenizer(opt_fingerprint.c_str());
    while (tokenizer.hasMoreTokens())
    {
        const char *fingerprint = tokenizer.nextToken();
        if (simulation.getHasher()->equals(fingerprint))
        {
            printfmsg("Fingerprint successfully verified: %s", fingerprint);
            return;
        }
        k++;
    }

    printfmsg("Fingerprint mismatch! calculated: %s, expected: %s%s",
              simulation.getHasher()->str().c_str(),
              (k>=2 ? "one of: " : ""), opt_fingerprint.c_str());
}

cModuleType *EnvirBase::resolveNetwork(const char *networkname)
{
    cModuleType *network = NULL;
    std::string inifilePackage = simulation.getNedPackageForFolder(opt_inifile_network_dir.c_str());

    bool hasInifilePackage = !inifilePackage.empty() && strcmp(inifilePackage.c_str(),"-")!=0;
    if (hasInifilePackage)
        network = cModuleType::find((inifilePackage+"."+networkname).c_str());
    if (!network)
        network = cModuleType::find(networkname);
    if (!network) {
        if (hasInifilePackage)
            throw cRuntimeError("Network `%s' or `%s' not found, check .ini and .ned files",
                                networkname, (inifilePackage+"."+networkname).c_str());
        else
            throw cRuntimeError("Network `%s' not found, check .ini and .ned files", networkname);
    }
    if (!network->isNetwork())
        throw cRuntimeError("Module type `%s' is not a network", network->getFullName());
    return network;
}


