//==========================================================================
//  CENVIR.CC - part of
//
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Implementation of
//    cEnvir     : user interface class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "opp_ctype.h"
#include "args.h"
#include "distrib.h"
#include "cconfigkey.h"
#include "inifilereader.h"
#include "sectionbasedconfig.h"
#include "cenvir.h"
#include "omnetapp.h"
#include "appreg.h"
#include "cmodule.h"
#include "fsutils.h"
#include "fnamelisttokenizer.h"
#include "stringutil.h"

#include "speedometer.h"     // env_dummy_function()
#include "filemgrs.h"        // env_dummy_function()
#include "akaroarng.h"       // env_dummy_function()
#include "akoutvectormgr.h"  // env_dummy_function()
#include "matchableobject.h" // env_dummy_function()

#include "inttypes.h"

using std::ostream;


NAMESPACE_BEGIN

// Global objects:
cEnvir ev;

// the global list for the registration objects
cRegistrationList omnetapps("omnetapps");

// output buffer
#define ENVIR_TEXTBUF_LEN 1024
static char buffer[ENVIR_TEXTBUF_LEN];

NAMESPACE_END

USING_NAMESPACE;

Register_GlobalConfigEntry(CFGID_LOAD_LIBS, "load-libs", CFG_FILENAMES, "", "A space-separated list of dynamic libraries to be loaded on startup. The libraries should be given without the `.dll' or `.so' suffix -- that will be automatically appended.");
Register_GlobalConfigEntry(CFGID_CONFIGURATION_CLASS, "configuration-class", CFG_STRING, "", "Part of the Envir plugin mechanism: selects the class from which all configuration information will be obtained. This option lets you replace omnetpp.ini with some other implementation, e.g. database input. The simulation program still has to bootstrap from an omnetpp.ini (which contains the configuration-class setting). The class should implement the cConfiguration interface.");
Register_GlobalConfigEntry(CFGID_USER_INTERFACE, "user-interface", CFG_STRING, "", "Selects the user interface to be started. Possible values are Cmdenv and Tkenv, provided the simulation executable contains the respective libraries or loads them dynamically.");

// helper macro
#define CREATE_BY_CLASSNAME(var,classname,baseclass,description) \
     baseclass *var ## _tmp = (baseclass *) createOne(classname); \
     var = dynamic_cast<baseclass *>(var ## _tmp); \
     if (!var) \
         throw cRuntimeError("Class \"%s\" is not subclassed from " #baseclass, (const char *)classname);


//========================================================================

// User interface factory functions.
static cOmnetAppRegistration *chooseBestOmnetApp()
{
    cOmnetAppRegistration *best_appreg = NULL;

    // choose the one with highest score.
    cSymTable *a = omnetapps.instance();
    for (int i=0; i<a->size(); i++)
    {
        cOmnetAppRegistration *appreg = static_cast<cOmnetAppRegistration *>(a->get(i));
        if (!best_appreg || appreg->score()>best_appreg->score())
            best_appreg = appreg;
    }
    return best_appreg;
}

//========================================================================

#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif

cEnvir::cEnvir() : ostream(&ev_buf), ev_buf(this)
{
    disable_tracing = false;
    debug_on_errors = false;
    suppress_notifications = false; //FIXME set to true when not needed!
    app = NULL;
}

cEnvir::~cEnvir()
{
}

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
    sprintf(buf, "%"LL"d %"LL"d", a, b);
    if (strcmp(buf, "1 2")!=0) {printf("INTERNAL ERROR: INT64_PRINTF_FORMAT incorrectly defined in include/inttypes.h, please report this bug!\n\n"); abort();}
#undef LL
}

void cEnvir::setup(int argc, char *argv[])
{
    ArgList *args = NULL;
    SectionBasedConfiguration *bootconfig = NULL;
    try
    {
        simulation.init();

        // construct global lists
        ExecuteOnStartup::executeAll();

        // verify definitions of int64, int32, etc.
        verifyIntTypes();

        // args
        args = new ArgList(argc, argv, "h?f:u:l:c:r:p:n:x:gG");
        args->checkArgs();

        //
        // First, load the ini file. It might contain the name of the user interface
        // to instantiate.
        //
        const char *fname = args->optionValue('f',0);  // 1st '-f filename' option
        if (!fname) fname = args->argument(0);   // first argument
        if (!fname) fname = "omnetpp.ini";   // or default filename

        InifileReader *inifile = new InifileReader();
        inifile->readFile(fname);

        // process additional '-f filename' options or arguments if there are any
        for (int k=1; (fname=args->optionValue('f',k))!=NULL; k++)
            inifile->readFile(fname);
        for (int k=(args->optionValue('f',0) ? 0 : 1); (fname=args->argument(k))!=NULL; k++)
            inifile->readFile(fname);

        // activate [General] section so that we can read global settings from it
        bootconfig = new SectionBasedConfiguration();
        bootconfig->setConfigurationReader(inifile);
        bootconfig->activateConfig("General", 0);

        //
        // Load all libraries specified on the command line ('-l' options),
        // and in the configuration [General]/load-libs=.
        // (The user interface library also might be among them.)
        //
        const char *libname;
        for (int k=0; (libname=args->optionValue('l',k))!=NULL; k++)
            loadExtensionLibrary(libname);
        std::vector<std::string> libs = bootconfig->getAsFilenames(CFGID_LOAD_LIBS);
        for (int k=0; k<(int)libs.size(); k++)
            loadExtensionLibrary(libs[k].c_str());

        //
        // Create custom configuration object, if needed.
        //
        std::string configclass = bootconfig->getAsString(CFGID_CONFIGURATION_CLASS);
        cConfiguration *configobject = NULL;
        if (configclass.empty())
        {
            configobject = bootconfig;
        }
        else
        {
            // create custom configuration object
            CREATE_BY_CLASSNAME(configobject, configclass.c_str(), cConfiguration, "configuration");
            configobject->initializeFrom(bootconfig);
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
        if (omnetapps.instance()->lookup("Cmdenv")==NULL)
            ignorablekeys += " cmdenv-*";
        if (omnetapps.instance()->lookup("Tkenv")==NULL)
            ignorablekeys += " tkenv-*";
        configobject->validate(ignorablekeys.c_str());

        //
        // Choose and set up user interface (TOmnetApp subclass). Everything else
        // will be done by the user interface class.
        //

        // was it specified explicitly which one to use?
        std::string appname = opp_nulltoempty(args->optionValue('u',0));  // 1st '-u name' option
        if (appname.empty())
            appname = configobject->getAsString(CFGID_USER_INTERFACE);

        cOmnetAppRegistration *appreg = NULL;
        if (!appname.empty())
        {
            // look up specified user interface
            appreg = static_cast<cOmnetAppRegistration *>(omnetapps.instance()->lookup(appname.c_str()));
            if (!appreg)
            {
                ::printf("\n"
                         "User interface '%s' not found (not linked in or loaded dynamically).\n"
                         "Available ones are:\n", appname.c_str());
                cSymTable *a = omnetapps.instance();
                for (int i=0; i<a->size(); i++)
                    ::printf("  %s : %s\n", a->get(i)->name(), a->get(i)->info().c_str());

                throw cRuntimeError("Could not start user interface '%s'", appname.c_str());
            }
        }
        else
        {
            // user interface not explicitly selected: pick one from what we have
            appreg = chooseBestOmnetApp();
            if (!appreg)
                throw cRuntimeError("No user interface (Cmdenv, Tkenv, etc.) found");
        }

        //
        // Finally, set up user interface object. All the rest will be done there.
        //
        ::printf("Setting up %s...\n", appreg->name());
        app = appreg->createOne(args, configobject);
        app->setup();
        isgui = app->isGUI();
    }
    catch (std::exception& e)
    {
        printfmsg("Error during startup: %s", e.what());
        if (app)
        {
           delete app;
           app = NULL;
        }
        else
        {
            // normally, these two get deleted by app
            delete args;
            delete bootconfig;
        }
    }
}

int cEnvir::run()
{
    try
    {
        if (app)
            return app->run();
        return 1;
    }
    catch (std::exception& e)
    {
        printfmsg("%s", e.what());
        return 1;
    }
}

void cEnvir::shutdown()
{
    if (app)
    {
        app->shutdown();
        delete app;
        app = NULL;
    }

    simulation.shutdown();

    componentTypes.clear();
    nedFunctions.clear();
    classes.clear();
    enums.clear();
    classDescriptors.clear();
    configKeys.clear();
    omnetapps.clear();
}

//-----------------------------------------------------------------

void cEnvir::readParameter(cPar *parameter)
{
    app->readParameter(parameter);
}

bool cEnvir::isModuleLocal(cModule *parentmod, const char *modname, int index)
{
    return app->isModuleLocal(parentmod, modname, index);
}

cXMLElement *cEnvir::getXMLDocument(const char *filename, const char *path)
{
    return app->getXMLDocument(filename, path);
}

unsigned cEnvir::extraStackForEnvir()
{
    return app->extraStackForEnvir();
}

cConfiguration *cEnvir::config()
{
    return app->getConfig();
}

//-----------------------------------------------------------------

void cEnvir::objectDeleted(cObject *obj)
{
    if (app) app->objectDeleted(obj);
}

void cEnvir::simulationEvent(cMessage *msg)
{
    app->simulationEvent(msg);
}

void cEnvir::messageSent_OBSOLETE(cMessage *msg, cGate *directToGate)
{
    app->messageSent_OBSOLETE(msg, directToGate);
}

void cEnvir::messageScheduled(cMessage *msg)
{
    app->messageScheduled(msg);
}

void cEnvir::messageCancelled(cMessage *msg)
{
    app->messageCancelled(msg);
}

void cEnvir::beginSend(cMessage *msg)
{
    app->beginSend(msg);
}

void cEnvir::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    app->messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);
}

void cEnvir::messageSendHop(cMessage *msg, cGate *srcGate)
{
    app->messageSendHop(msg, srcGate);
}

void cEnvir::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    app->messageSendHop(msg, srcGate, propagationDelay, transmissionDelay);
}

void cEnvir::endSend(cMessage *msg)
{
    app->endSend(msg);
}

void cEnvir::messageDeleted(cMessage *msg)
{
    app->messageDeleted(msg);
}

void cEnvir::componentMethodBegin(cComponent *from, cComponent *to, const char *method)
{
    app->componentMethodBegin(from, to, method);
}

void cEnvir::componentMethodEnd()
{
    app->componentMethodEnd();
}

//FIXME we should probably NOT use EVCB for these! (or we should redraw the network diagram every time!!!)
void cEnvir::moduleCreated(cModule *newmodule)
{
    app->moduleCreated(newmodule);
}

void cEnvir::moduleDeleted(cModule *module)
{
    app->moduleDeleted(module);
}

void cEnvir::moduleReparented(cModule *module, cModule *oldparent)
{
    app->moduleReparented(module, oldparent);
}

void cEnvir::connectionCreated(cGate *srcgate)
{
    app->connectionCreated(srcgate);
}

void cEnvir::connectionRemoved(cGate *srcgate)
{
    app->connectionRemoved(srcgate);
}

void cEnvir::displayStringChanged(cGate *gate)
{
    app->displayStringChanged(gate);
}

void cEnvir::displayStringChanged(cModule *module)
{
    app->displayStringChanged(module);
}

void cEnvir::undisposedObject(cObject *obj)
{
    if (!app)
    {
        // we must have been called after cEnvir has already shut down
        ::printf("<!> WARNING: global object variable (DISCOURAGED) detected: (%s)`%s' at %p\n",
                 obj->className(), obj->fullPath().c_str(), obj);
        return;
    }
    app->undisposedObject(obj);
}

//-----------------------------------------------------------------

void cEnvir::bubble(cModule *mod, const char *text)
{
    if (!isgui || disable_tracing) return;
    app->bubble(mod, text);
}

void cEnvir::printfmsg(const char *fmt,...)
{
    va_list va;
    va_start(va, fmt);
    vsprintf(buffer, fmt, va);
    va_end(va);

    if (app)
       app->putmsg(buffer);
    else
       ::printf("\n<!> %s\n\n", buffer);
}

void cEnvir::printf(const char *fmt,...)
{
    if (disable_tracing) return;

    va_list va;
    va_start(va, fmt);
    int len = vsprintf(buffer, fmt, va); // len<0 means error
    va_end(va);

    // has to go through evbuf to preserve ordering
    if (len>0)
        ev_buf.sputn(buffer,len);
}

void cEnvir::puts(const char *s)
{
    if (disable_tracing) return;

    // has to go through evbuf to preserve ordering
    int len = strlen(s);
    ev_buf.sputn(s,len);
}

void cEnvir::sputn(const char *s, int n)
{
    // invoked from evbuf::sync() to flush stream buffer
    if (disable_tracing) return;

    if (app)
        app->sputn(s,n);
    else
        ::fwrite(s,1,n,stdout);
}

cEnvir& cEnvir::flush()
{
    if (disable_tracing) return *this;

    ev_buf.pubsync();

    if (app)
        app->flush();
    else
        ::fflush(stdout);

    return *this;
}

bool cEnvir::gets(const char *prompt, char *buf, int len)
{
    bool esc = app->gets(prompt, buf, len);
    if (esc)
        throw cRuntimeError(eCANCEL);
    return true;
}

std::string cEnvir::gets(const char *prompt, const char *defaultreply)
{
    buffer[0] = '\0';
    if (defaultreply) strncpy(buffer,defaultreply,ENVIR_TEXTBUF_LEN-1);
    buffer[ENVIR_TEXTBUF_LEN-1]='\0';
    bool esc = app->gets(prompt, buffer, ENVIR_TEXTBUF_LEN-1);
    if (esc)
        throw cRuntimeError(eCANCEL);
    return std::string(buffer);
}

bool cEnvir::askYesNo(const char *msgfmt,...)
{
    va_list va;
    va_start(va, msgfmt);
    vsprintf(buffer, msgfmt, va);
    va_end(va);

    int ret = app->askYesNo(buffer);
    if (ret<0)
        throw cRuntimeError(eCANCEL);
    return ret!=0;
}


//---------------------------------------------------------

int cEnvir::numRNGs()
{
    return app->numRNGs();
}

cRNG *cEnvir::rng(int k)
{
    return app->rng(k);
}

void cEnvir::getRNGMappingFor(cComponent *component)
{
    app->getRNGMappingFor(component);
}

//---------------------------------------------------------

void *cEnvir::registerOutputVector(const char *modulename, const char *vectorname)
{
    return app->registerOutputVector(modulename, vectorname);
}

void cEnvir::deregisterOutputVector(void *vechandle)
{
    app->deregisterOutputVector(vechandle);
}

void cEnvir::setVectorAttribute(void *vechandle, const char *name, const char *value)
{
    app->setVectorAttribute(vechandle, name, value);
}

bool cEnvir::recordInOutputVector(void *vechandle, simtime_t t, double value)
{
    return app->recordInOutputVector(vechandle, t, value);
}

//---------------------------------------------------------

void cEnvir::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    app->recordScalar(component, name, value, attributes);
}

void cEnvir::recordScalar(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    app->recordScalar(component, name, statistic, attributes);
}

//---------------------------------------------------------

ostream *cEnvir::getStreamForSnapshot()
{
    return app->getStreamForSnapshot();
}

void cEnvir::releaseStreamForSnapshot(ostream *os)
{
    app->releaseStreamForSnapshot(os);
}

//---------------------------------------------------------

int cEnvir::argCount()
{
    return app->argList()->argCount();
}

char **cEnvir::argVector()
{
    return app->argList()->argVector();
}

int cEnvir::getParsimProcId()
{
    return app->getParsimProcId();
}

int cEnvir::getParsimNumPartitions()
{
    return app->getParsimNumPartitions();
}

unsigned long cEnvir::getUniqueNumber()
{
    return app->getUniqueNumber();
}

bool cEnvir::idle()
{
    return app->idle();
}

// A dummy function to force UNIX linkers collect Speedometer
// and cFileOutputVectorManager as linker symbols. Otherwise we'd get
// "undefined symbol" messages...
void env_dummy_function() {
    exponential(1.0);
    Speedometer a;
    cFileOutputVectorManager o;
    MatchableObjectAdapter moa;
    (void)a; (void)o; (void)moa; // eliminate 'unused var' warning
#ifdef WITH_AKAROA
    cAkOutputVectorManager ao;
    cAkaroaRNG ar;
    (void)ao; (void)ar; // eliminate 'unused var' warning
#endif
}

