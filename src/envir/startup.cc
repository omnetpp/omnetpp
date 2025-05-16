//==========================================================================
//  STARTUP.CC - part of
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

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cstdint>

#include "common/opp_ctype.h"
#include "common/fnamelisttokenizer.h"
#include "common/stringutil.h"
#include "common/fileutil.h"
#include "omnetpp/distrib.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/platdep/platmisc.h" // for SetErrorMode(0)
#include "common/ver.h"
#include "envirbase.h" // ARGSPEC
#include "args.h"
#include "inifilereader.h"
#include "sectionbasedconfig.h"
#include "appreg.h"
#include "fsutils.h"
#include "startup.h"
#include "speedometer.h"
#include "filesnapshotmgr.h"
#include "eventlogfilemgr.h"
#include "akaroarng.h"
#include "akoutvectormgr.h"
#include "matchableobject.h"
#include "omnetppoutscalarmgr.h"
#include "omnetppoutvectormgr.h"
#include "sqliteoutscalarmgr.h"
#include "sqliteoutvectormgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

Register_GlobalConfigOption(CFGID_LOAD_LIBS, "load-libs", CFG_FILENAMES, "", "A space-separated list of dynamic libraries to be loaded on startup. The libraries should be given without the `.dll` or `.so` suffix -- that will be automatically appended.");
Register_GlobalConfigOption(CFGID_CONFIGURATION_CLASS, "configuration-class", CFG_STRING, "", "Part of the Envir plugin mechanism: selects the class from which all configuration information will be obtained. This option lets you replace omnetpp.ini with some other implementation, e.g. database input. The simulation program still has to bootstrap from an omnetpp.ini (which contains the configuration-class setting). The class should implement the `cConfigurationEx` interface.");
Register_GlobalConfigOption(CFGID_USER_INTERFACE, "user-interface", CFG_STRING, "", "Selects the user interface to be started. Known good values are Cmdenv and Qtenv. This option is normally left empty, as it is more convenient to specify the user interface via a command-line option or the IDE's Run and Debug dialogs. New user interfaces can be defined by subclassing `cRunnableEnvir`.");

// helper macro
#define CREATE_BY_CLASSNAME(var, classname, baseclass, description) \
    baseclass *var ## _tmp = (baseclass *)createOne(classname); \
    var = dynamic_cast<baseclass *>(var ## _tmp); \
    if (!var) \
        throw cRuntimeError("Class \"%s\" is not subclassed from " #baseclass, (const char *)classname);

static void verifyIntTypes()
{
#define VERIFY(t,size) if (sizeof(t)!=size) {printf("INTERNAL ERROR: sizeof(%s)!=%d, please check typedefs in include/inttypes.h, and report this bug!\n\n", #t, size); abort();}
    VERIFY(int8_t,  1);
    VERIFY(int16_t, 2);
    VERIFY(int32_t, 4);
    VERIFY(int64_t, 8);

    VERIFY(uint8_t, 1);
    VERIFY(uint16_t, 2);
    VERIFY(uint32_t, 4);
    VERIFY(uint64_t, 8);
#undef VERIFY

    char buf[32];
    int64_t a = 1, b = 2;
    snprintf(buf, sizeof(buf), "%" PRId64 " %" PRId64, a, b);
    if (strcmp(buf, "1 2") != 0) {
        printf("INTERNAL ERROR: PRId64 is incorrectly defined, please report this bug!\n\n");
        abort();
    }
}

static bool useStderr = false;

static std::ostream& err()
{
    std::ostream& err = useStderr ? std::cerr : std::cout;
    err << "\n<!> Error: ";
    return err;
}

int setupUserInterface(int argc, char *argv[])
{
    //
    // SETUP
    //
    cSimulation *simulation = nullptr;
    cRunnableEnvir *app = nullptr;
    SectionBasedConfiguration *bootConfig = nullptr;
    cConfigurationEx *config = nullptr;
    bool verbose = false;
    bool silent = false;
    int exitCode = 0;
    try {
        // construct global lists
        CodeFragments::executeAll(CodeFragments::STARTUP);

        // verify definitions of int64_t, int32_t, etc.
        verifyIntTypes();

        // args
        ArgList args;
        args.parse(argc, argv, ARGSPEC);

        useStderr = !args.optionGiven('m');

        silent = args.optionGiven('S');
        verbose = !silent && !args.optionGiven('s');
        if (verbose) {
            std::cout << OMNETPP_PRODUCT " Discrete Event Simulation  (C) 1992-2025 Andras Varga, OpenSim Ltd." << endl;
            std::cout << "Version: " OMNETPP_VERSION_STR ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION << endl;
            std::cout << "See the license for distribution terms and warranty disclaimer" << endl;
            std::cout << endl;
        }

        #ifdef _WIN32
        // All of this is here because we want to get a usable error message if there is any trouble
        // loading the DLL-s (usually model code built as a shared library), or their linked dependencies.
        // Unfortunately the only way to get that is a graphical dialog, and we don't want to get that
        // when running in a terminal (especially when launching large simulation campaign batches).
        // See: https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-seterrormode
        {
            // This will be wrong if the app (envir) to start is selected in the configuration
            // (which is by default SectionBasedConfiguration, reading omnetpp.ini) and is not
            // overridden by a command-line argument. Additionally, sometimes an envir might
            // be loaded from an extension library, and we don't have those yet.
            const char *preliminaryAppName = args.optionValue('u'); // last -u argument
            // Look up the specified user interface or pick one from those that we have.
            // We won't complain if the selected one is not found. There is a second,
            // "real" lookup further down, after library loading, which does it if needed.
            // Also not falling back to choosing the "best" because the real lookup doesn't either.
            cOmnetAppRegistration *appReg = preliminaryAppName != nullptr
                ? static_cast<cOmnetAppRegistration *>(omnetapps.getInstance()->lookup(preliminaryAppName))
                : cOmnetAppRegistration::chooseBest(); // might not find one either in an extreme case

            // This is not a very sophisticated check, but we don't have a better one yet.
            if (appReg != nullptr && strcmp(appReg->getName(), "Qtenv") == 0)
                SetErrorMode(0);
        }
        #endif

        //
        // First, load the ini file(s). It might contain the name of the user interface
        // to instantiate.
        //
        InifileReader *iniReader = new InifileReader();
        const char *fname;
        int inifilesRead = 0;
        for (int k = 0; (fname = args.optionValue('f', k)) != nullptr; k++, inifilesRead++)
            iniReader->readFile(fname);
        for (int k = 0; (fname = args.argument(k)) != nullptr; k++, inifilesRead++)
            iniReader->readFile(fname);
        if (inifilesRead == 0) {
            fname = "omnetpp.ini";
            if (fileExists(fname))
                iniReader->readFile(fname);
            else if (!args.optionGiven('v') && !args.optionGiven('h') && !args.longOptionGiven("network"))
                throw cRuntimeError("Missing configuration: No ini files specified and no 'omnetpp.ini' in the current working directory (specify at least --network=<name> to suppress this message)");
        }

        // activate [General] section so that we can read global settings from it
        bootConfig = new SectionBasedConfiguration();
        bootConfig->setConfigurationReader(iniReader);
        bootConfig->setCommandLineConfigOptions(args.getLongOptions(), getWorkingDir().c_str());

        //
        // Load all libraries specified on the command line ('-l' options),
        // and in the configuration [General]/load-libs=.
        // (The user interface library also might be among them.)
        //
        const char *libName;
        for (int k = 0; (libName = args.optionValue('l', k)) != nullptr; k++)
            loadExtensionLibrary(libName);
        std::vector<std::string> libs = bootConfig->getAsFilenames(CFGID_LOAD_LIBS);
        for (auto & lib : libs)
            loadExtensionLibrary(lib.c_str());

        //
        // Create custom configuration object, if needed.
        //
        std::string configClass = bootConfig->getAsString(CFGID_CONFIGURATION_CLASS);
        if (configClass.empty()) {
            config = bootConfig;
        }
        else {
            // create custom configuration object
            CREATE_BY_CLASSNAME(config, configClass.c_str(), cConfigurationEx, "configuration");
            config->initializeFrom(bootConfig);
            delete bootConfig;
            bootConfig = nullptr;

            // load libs from this config as well
            std::vector<std::string> libs = config->getAsFilenames(CFGID_LOAD_LIBS);
            for (auto & lib : libs)
                loadExtensionLibrary(lib.c_str());
        }

        // validate the configuration, but make sure we don't report cmdenv-* keys
        // as errors if Cmdenv is absent; same for other optional features.
        std::string ignorableKeys;
        if (omnetapps.getInstance()->lookup("Cmdenv") == nullptr)
            ignorableKeys += " cmdenv-*";
        if (omnetapps.getInstance()->lookup("Qtenv") == nullptr)
            ignorableKeys += " qtenv-*";
#ifndef WITH_PARSIM
        ignorableKeys += " parsim-*";
#endif
        config->validate(ignorableKeys.c_str());

        //
        // Choose and set up user interface (EnvirBase subclass). Everything else
        // will be done by the user interface class.
        //

        std::string appName = opp_nulltoempty(args.optionValue('u')); // last -u argument
        if (appName.empty())
            appName = config->getAsString(CFGID_USER_INTERFACE);

        cOmnetAppRegistration *appReg = nullptr;
        if (!appName.empty()) {
            // look up specified user interface
            appReg = static_cast<cOmnetAppRegistration *>(omnetapps.getInstance()->lookup(appName.c_str()));
            if (!appReg) {
                if (verbose) {
                    std::cout << "User interface '" << appName << "' not found (not linked in or loaded dynamically)." << endl << endl;
                    std::cout << "Available ones are:" << endl;
                    cRegistrationList *a = omnetapps.getInstance();
                    for (int i = 0; i < a->size(); i++)
                        std::cout << "  " << a->get(i)->getName() << " : " << a->get(i)->str() << endl;
                }
                throw cRuntimeError("Could not start user interface '%s'", appName.c_str());
            }
        }
        else {
            // user interface not explicitly selected: pick one from what we have
            appReg = cOmnetAppRegistration::chooseBest();
            if (!appReg)
                throw cRuntimeError("No user interface (Cmdenv, Qtenv, etc.) found");
        }

        //
        // Create interface object.
        //
        if (verbose)
            std::cout << "Setting up " << appReg->getName() << "..." << endl << endl;
        app = appReg->createOne();
    }
    catch (std::exception& e) {
        err() << e.what() << endl;
        if (app) {
            delete app;
            app = nullptr;
        }
        else {
            // normally, this is deleted by app
            delete bootConfig;
        }
    }

    //
    // RUN
    //
    try {
        if (app) {
            // as we'll install app as the active envir, take over the listeners added to the boot-time envir
            for (auto l : getEnvir()->getLifecycleListeners())
                app->addLifecycleListener(l);

            // install and run app
            simulation = new cSimulation("simulation", app);
            cSimulation::setActiveSimulation(simulation);
            exitCode = app->run(argc, argv, config);
        }
        else {
            exitCode = 1;
        }
    }
    catch (std::exception& e) {
        err() << e.what() << endl;
        exitCode = 1;
    }

    //
    // SHUTDOWN
    //
    if (verbose)
        std::cout << "\nEnd." << endl;
    cSimulation::setActiveSimulation(nullptr);
    delete simulation;  // will delete app as well

    CodeFragments::executeAll(CodeFragments::SHUTDOWN);

    return exitCode;
}

//---------------------------------------------------------
// A dummy function to force UNIX linkers collect all symbols we need
void env_dummy_function()
{
    cRNG *rng = nullptr;
    exponential(rng, 1.0);
    Speedometer a;
    OmnetppOutputScalarManager oosm;
    OmnetppOutputVectorManager oovm;
    SqliteOutputScalarManager sosm;
    SqliteOutputVectorManager sovm;
    FileSnapshotManager sm;
    MatchableObjectAdapter moa;
    EventlogFileManager elfm;
    (void)a;
    (void)oosm;
    (void)oovm;
    (void)sosm;
    (void)sovm;
    (void)sm;
    (void)moa;  // eliminate 'unused var' warnings
    (void)elfm;
#ifdef WITH_AKAROA
    cAkOutputVectorManager ao;
    cAkaroaRNG ar;
    (void)ao;
    (void)ar;  // eliminate 'unused var' warning
#endif
}

}  // namespace envir
}  // namespace omnetpp

