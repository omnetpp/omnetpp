//==========================================================================
//  STARTUP.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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


#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "opp_ctype.h"
#include "args.h"
#include "distrib.h"
#include "cconfigoption.h"
#include "inifilereader.h"
#include "sectionbasedconfig.h"
#include "appreg.h"
#include "cmodule.h"
#include "fsutils.h"
#include "fnamelisttokenizer.h"
#include "stringutil.h"
#include "fileutil.h"
#include "intxtypes.h"
#include "startup.h"

#include "speedometer.h"
#include "fileoutvectormgr.h"
#include "fileoutscalarmgr.h"
#include "filesnapshotmgr.h"
#include "indexedfileoutvectormgr.h"
#include "akaroarng.h"
#include "akoutvectormgr.h"
#include "matchableobject.h"

NAMESPACE_BEGIN

Register_GlobalConfigOption(CFGID_LOAD_LIBS, "load-libs", CFG_FILENAMES, "", "A space-separated list of dynamic libraries to be loaded on startup. The libraries should be given without the `.dll' or `.so' suffix -- that will be automatically appended.");
Register_GlobalConfigOption(CFGID_CONFIGURATION_CLASS, "configuration-class", CFG_STRING, "", "Part of the Envir plugin mechanism: selects the class from which all configuration information will be obtained. This option lets you replace omnetpp.ini with some other implementation, e.g. database input. The simulation program still has to bootstrap from an omnetpp.ini (which contains the configuration-class setting). The class should implement the cConfigurationEx interface.");
Register_GlobalConfigOption(CFGID_USER_INTERFACE, "user-interface", CFG_STRING, "", "Selects the user interface to be started. Possible values are Cmdenv and Tkenv. This option is normally left empty, as it is more convenient to specify the user interface via a command-line option or the IDE's Run and Debug dialogs. New user interfaces can be defined by subclassing cRunnableEnvir.");

// helper macro
#define CREATE_BY_CLASSNAME(var,classname,baseclass,description) \
     baseclass *var ## _tmp = (baseclass *) createOne(classname); \
     var = dynamic_cast<baseclass *>(var ## _tmp); \
     if (!var) \
         throw cRuntimeError("Class \"%s\" is not subclassed from " #baseclass, (const char *)classname);


static void verifyIntTypes()
{
#define VERIFY(t,size) if (sizeof(t)!=size) {printf("INTERNAL ERROR: sizeof(%s)!=%d, please check typedefs in include/inttypes.h, and report this bug!\n\n", #t, size); abort();}
    VERIFY(int8,  1);
    VERIFY(int16, 2);
    VERIFY(int32, 4);
    VERIFY(int64, 8);

    VERIFY(uint8, 1);
    VERIFY(uint16,2);
    VERIFY(uint32,4);
    VERIFY(uint64,8);
#undef VERIFY

#define LL  INT64_PRINTF_FORMAT
    char buf[32];
    int64 a=1, b=2;
    sprintf(buf, "%" LL "d %" LL "d", a, b);
    if (strcmp(buf, "1 2")!=0) {printf("INTERNAL ERROR: INT64_PRINTF_FORMAT incorrectly defined in include/inttypes.h, please report this bug!\n\n"); abort();}
#undef LL
}

int setupUserInterface(int argc, char *argv[])
{
    //
    // SETUP
    //
    cSimulation *simulationobject = NULL;
    cRunnableEnvir *app = NULL;
    SectionBasedConfiguration *bootconfig = NULL;
    cConfigurationEx *configobject = NULL;
    int exitcode = 0;
    try
    {
        // construct global lists
        CodeFragments::executeAll(CodeFragments::STARTUP);

        // verify definitions of int64, int32, etc.
        verifyIntTypes();

        // args
        ArgList args;
        args.parse(argc, argv, "h?f:u:l:c:r:p:n:x:X:agGv");

        //
        // First, load the ini file. It might contain the name of the user interface
        // to instantiate.
        //
        const char *fname = args.optionValue('f',0);  // 1st '-f filename' option
        if (!fname) fname = args.argument(0);   // first argument
        if (!fname) fname = "omnetpp.ini";   // or default filename

        // when -h or -v is specified, be forgiving about nonexistent omnetpp.ini
        InifileReader *inifile = new InifileReader();
        if ((!args.optionGiven('v') && !args.optionGiven('h')) || fileExists(fname))
            inifile->readFile(fname);

        // process additional '-f filename' options or arguments if there are any
        for (int k=1; (fname=args.optionValue('f',k))!=NULL; k++)
            inifile->readFile(fname);
        for (int k=(args.optionValue('f',0) ? 0 : 1); (fname=args.argument(k))!=NULL; k++)
            inifile->readFile(fname);

        // activate [General] section so that we can read global settings from it
        bootconfig = new SectionBasedConfiguration();
        bootconfig->setConfigurationReader(inifile);
        bootconfig->setCommandLineConfigOptions(args.getLongOptions(), getWorkingDir().c_str());
        bootconfig->activateConfig("General", 0);

        //
        // Load all libraries specified on the command line ('-l' options),
        // and in the configuration [General]/load-libs=.
        // (The user interface library also might be among them.)
        //
        const char *libname;
        for (int k=0; (libname=args.optionValue('l',k))!=NULL; k++)
            loadExtensionLibrary(libname);
        std::vector<std::string> libs = bootconfig->getAsFilenames(CFGID_LOAD_LIBS);
        for (int k=0; k<(int)libs.size(); k++)
            loadExtensionLibrary(libs[k].c_str());

        //
        // Create custom configuration object, if needed.
        //
        std::string configclass = bootconfig->getAsString(CFGID_CONFIGURATION_CLASS);
        if (configclass.empty())
        {
            configobject = bootconfig;
        }
        else
        {
            // create custom configuration object
            CREATE_BY_CLASSNAME(configobject, configclass.c_str(), cConfigurationEx, "configuration");
            configobject->initializeFrom(bootconfig);
            configobject->activateConfig("General", 0);
            delete bootconfig;
            bootconfig = NULL;

            // load libs from this config as well
            std::vector<std::string> libs = configobject->getAsFilenames(CFGID_LOAD_LIBS);
            for (int k=0; k<(int)libs.size(); k++)
                loadExtensionLibrary(libs[k].c_str());
        }

        // validate the configuration, but make sure we don't report cmdenv-* keys
        // as errors if Cmdenv is absent; same for Tkenv.
        std::string ignorablekeys;
        if (omnetapps.getInstance()->lookup("Cmdenv")==NULL)
            ignorablekeys += " cmdenv-*";
        if (omnetapps.getInstance()->lookup("Tkenv")==NULL)
            ignorablekeys += " tkenv-*";
        configobject->validate(ignorablekeys.c_str());

        //
        // Choose and set up user interface (EnvirBase subclass). Everything else
        // will be done by the user interface class.
        //

        // was it specified explicitly which one to use?
        std::string appname = opp_nulltoempty(args.optionValue('u',0));  // 1st '-u name' option
        if (appname.empty())
            appname = configobject->getAsString(CFGID_USER_INTERFACE);

        cOmnetAppRegistration *appreg = NULL;
        if (!appname.empty())
        {
            // look up specified user interface
            appreg = static_cast<cOmnetAppRegistration *>(omnetapps.getInstance()->lookup(appname.c_str()));
            if (!appreg)
            {
                ::printf("\n"
                         "User interface '%s' not found (not linked in or loaded dynamically).\n"
                         "Available ones are:\n", appname.c_str());
                cRegistrationList *a = omnetapps.getInstance();
                for (int i=0; i<a->size(); i++)
                    ::printf("  %s : %s\n", a->get(i)->getName(), a->get(i)->info().c_str());

                throw cRuntimeError("Could not start user interface '%s'", appname.c_str());
            }
        }
        else
        {
            // user interface not explicitly selected: pick one from what we have
            appreg = cOmnetAppRegistration::chooseBest();
            if (!appreg)
                throw cRuntimeError("No user interface (Cmdenv, Tkenv, etc.) found");
        }

        //
        // Create interface object.
        //
        ::printf("Setting up %s...\n", appreg->getName());
        app = appreg->createOne();
    }
    catch (std::exception& e)
    {
        ::fprintf(stderr, "\n<!> Error during startup: %s.\n", e.what());
        if (app)
        {
            delete app;
            app = NULL;
        }
        else
        {
            // normally, this is deleted by app
            delete bootconfig;
        }
    }

    //
    // RUN
    //
    try
    {
        if (app)
        {
            simulationobject = new cSimulation("simulation", app);
            cSimulation::setActiveSimulation(simulationobject);
            exitcode = app->run(argc, argv, configobject);
        }
        else
        {
            exitcode = 1;
        }
    }
    catch (std::exception& e)
    {
        ::fprintf(stderr, "\n<!> %s.\n", e.what());
        exitcode = 1;
    }

    //
    // SHUTDOWN
    //
    cSimulation::setActiveSimulation(NULL);
    delete simulationobject;  // will delete app as well

    CodeFragments::executeAll(CodeFragments::SHUTDOWN);

    return exitcode;
}

//---------------------------------------------------------
// A dummy function to force UNIX linkers collect all symbols we need
void env_dummy_function() {
    exponential(1.0);
    Speedometer a;
    cFileOutputVectorManager ovm;
    cFileOutputScalarManager osm;
    cFileSnapshotManager sm;
    cIndexedFileOutputVectorManager iovm;
    MatchableObjectAdapter moa;
    (void)a; (void)ovm; (void)osm; (void)sm; (void)iovm; (void)moa; // eliminate 'unused var' warnings
#ifdef WITH_AKAROA
    cAkOutputVectorManager ao;
    cAkaroaRNG ar;
    (void)ao; (void)ar; // eliminate 'unused var' warning
#endif
}

NAMESPACE_END

