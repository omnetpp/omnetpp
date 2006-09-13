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

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "ctypes.h"
#include "args.h"
#include "distrib.h"
#include "cinifile.h"
#include "cenvir.h"
#include "omnetapp.h"
#include "appreg.h"
#include "cmodule.h"
#include "fsutils.h"
#include "cstrtokenizer2.h"

#include "speedmtr.h"       // env_dummy_function()
#include "filemgrs.h"       // env_dummy_function()
#include "akaroarng.h"      // env_dummy_function()
#include "akoutvectormgr.h" // env_dummy_function()

using std::ostream;

//=== Global objects:
cEnvir ev;

// the global list for the registration objects
cSingleton<cArray> omnetapps("omnetapps");

// output buffer
#define ENVIR_TEXTBUF_LEN 1024
static char buffer[ENVIR_TEXTBUF_LEN];

// helper macro
#define CREATE_BY_CLASSNAME(var,classname,baseclass,description) \
     baseclass *var ## _tmp = (baseclass *) createOne(classname); \
     var = dynamic_cast<baseclass *>(var ## _tmp); \
     if (!var) \
         throw new cRuntimeError("Class \"%s\" is not subclassed from " #baseclass, (const char *)classname);


//========================================================================

// User interface factory functions.
cOmnetAppRegistration *chooseBestOmnetApp()
{
    cOmnetAppRegistration *best_appreg = NULL;

    // choose the one with highest score.
    cArray *a = omnetapps.instance();
    for (int i=0; i<a->items(); i++)
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
    app = NULL;
}

cEnvir::~cEnvir()
{
}

static void loadLibs(const char *libs)
{
    // "libs" may contain quoted filenames, so use cStringTokenizer2 to parse it
    if (!libs) libs = "";
    cStringTokenizer2 tokenizer(libs);
    const char *lib;
    while ((lib = tokenizer.nextToken())!=NULL)
        loadExtensionLibrary(lib);
}

void cEnvir::setup(int argc, char *argv[])
{
    ArgList *args = NULL;
    cIniFile *inifile = NULL;
    try
    {
        simulation.init();

        // construct global lists
        ExecuteOnStartup::executeAll();

        // args
        args = new ArgList(argc,argv);

        //
        // First, load the ini file. It might contain the name of the user interface
        // to instantiate.
        //
        const char *fname = args->argValue('f',0);  // 1st '-f filename' option
        if (!fname) fname="omnetpp.ini";   // or default filename

        inifile = new cIniFile();
        inifile->readFile(fname);

        // process additional '-f filename' options if there are any
        int k;
        for (k=1; (fname=args->argValue('f',k))!=NULL; k++)
            inifile->readFile( fname );

        //
        // Load all libraries specified on the command line or in the ini file.
        // (The user interface library also might be among them.)
        //

        // load shared libraries given with '-l' option(s)
        const char *libname;
        for (k=0; (libname=args->argValue('l',k))!=NULL; k++)
            loadExtensionLibrary(libname);

        // load shared libs given in [General]/load-libs=
        std::string libs = inifile->getAsFilenames("General", "load-libs", NULL);
        loadLibs(libs.c_str());

        //
        // Create custom configuration object, if needed.
        //
        const char *configclass = inifile->getAsString("General", "configuration-class", NULL);
        cConfiguration *configobject = NULL;
        if (!configclass)
        {
            configobject = inifile;
        }
        else
        {
            // create custom configuration object
            CREATE_BY_CLASSNAME(configobject, configclass, cConfiguration, "configuration");
            configobject->initializeFrom(inifile);
            delete inifile;
            inifile = NULL;

            // load libs from this config as well
            std::string libs = configobject->getAsFilenames("General", "load-libs", NULL);
            loadLibs(libs.c_str());
        }


        //
        // Choose and set up user interface (TOmnetApp subclass). Everything else
        // will be done by the user interface class.
        //

        // was it specified explicitly which one to use?
        const char *appname = args->argValue('u',0);  // 1st '-u name' option
        if (!appname)
            appname = configobject->getAsString("General", "user-interface", NULL);

        cOmnetAppRegistration *appreg = NULL;
        if (appname && appname[0])
        {
            // try to look up specified user interface; if we don't have it already,
            // try to load dynamically...
            appreg = static_cast<cOmnetAppRegistration *>(omnetapps.instance()->get(appname));
            if (!appreg)
            {
                ::printf("\n"
                         "User interface '%s' not found (not linked in or loaded dynamically).\n"
                         "Available ones are:\n", appname);
                for (cArray::Iterator iter(*omnetapps.instance()); iter(); iter++)
                    ::printf("  %s : %s\n", iter()->name(), iter()->info().c_str());

                throw new cRuntimeError("Could not start user interface '%s'",appname);
            }
        }
        else
        {
            // user interface not explicitly selected: pick one from what we have
            appreg = chooseBestOmnetApp();
            if (!appreg)
                throw new cRuntimeError("No user interface (Cmdenv, Tkenv, etc.) found");
        }

        //
        // Finally, set up user interface object. All the rest will be done there.
        //
        ::printf("Setting up %s...\n", appreg->name());
        app = appreg->createOne(args, configobject);
        app->setup();
        isgui = app->isGUI();
    }
    catch (cException *e)
    {
        printfmsg("Error during startup: %s", e->message());
        if (app)
        {
           delete app;
           app = NULL;
        }
        delete args;
        delete inifile;
        delete e;
    }
}

int cEnvir::run()
{
    if (app)
        return app->run();
    return 1;
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
}

//-----------------------------------------------------------------

std::string cEnvir::getParameter(int run_no, const char *parname)
{
    return app->getParameter(run_no, parname);
}

bool cEnvir::getParameterUseDefault(int run_no, const char *parname)
{
    return app->getParameterUseDefault(run_no, parname);
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

void cEnvir::messageSent( cMessage *msg, cGate *directToGate)
{
    if (disable_tracing) return;
    app->messageSent( msg, directToGate );
}

void cEnvir::messageDelivered( cMessage *msg )
{
    if (disable_tracing) return;
    app->messageDelivered( msg );
}

void cEnvir::moduleMethodCalled(cModule *from, cModule *to, const char *method)
{
    if (disable_tracing) return;
    app->moduleMethodCalled(from, to, method);
}

void cEnvir::objectDeleted( cObject *obj )
{
    if (app) app->objectDeleted( obj );
}

void cEnvir::breakpointHit( const char *label, cSimpleModule *module )
{
    app->breakpointHit( label, module );
}

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

void cEnvir::displayStringChanged(cModule *submodule)
{
    app->displayStringChanged(submodule);
}

void cEnvir::backgroundDisplayStringChanged(cModule *parentmodule)
{
    app->backgroundDisplayStringChanged(parentmodule);
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
        throw new cRuntimeError(eCANCEL);
    return true;
}

std::string cEnvir::gets(const char *prompt, const char *defaultreply)
{
    buffer[0] = '\0';
    if (defaultreply) strncpy(buffer,defaultreply,ENVIR_TEXTBUF_LEN-1);
    buffer[ENVIR_TEXTBUF_LEN-1]='\0';
    bool esc = app->gets(prompt, buffer, ENVIR_TEXTBUF_LEN-1);
    if (esc)
        throw new cRuntimeError(eCANCEL);
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
        throw new cRuntimeError(eCANCEL);
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

void cEnvir::getRNGMappingFor(cModule *mod)
{
    app->getRNGMappingFor(mod);
}

//---------------------------------------------------------

void *cEnvir::registerOutputVector(const char *modulename, const char *vectorname, int tuple)
{
    return app->registerOutputVector(modulename, vectorname, tuple);
}

void cEnvir::deregisterOutputVector(void *vechandle)
{
    app->deregisterOutputVector(vechandle);
}

bool cEnvir::recordInOutputVector(void *vechandle, simtime_t t, double value)
{
    return app->recordInOutputVector(vechandle, t, value);
}

bool cEnvir::recordInOutputVector(void *vechandle, simtime_t t, double value1, double value2)
{
    return app->recordInOutputVector(vechandle, t, value1, value2);
}

//---------------------------------------------------------

void cEnvir::recordScalar(cModule *module, const char *name, double value)
{
    app->recordScalar(module, name, value);
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

bool memoryIsLow()
{
    return ev.app->memoryIsLow();
}


//=== The DUMMY SECTION -- a tribute to "smart" linkers
// force the linker to include the user interface library into the executable.
// dummyDummy() can't be static or very smart linkers will leave it out...
#ifndef WIN32_DLL
void envirDummy();
void dummyDummy() {envirDummy();}
#endif

// another dummy variant in case you want to have both Cmdenv and Tkenv in
/*
#ifndef WIN32_DLL
OPP_DLLIMPORT void cmdenvDummy();
OPP_DLLIMPORT void tkenvDummy();
void dummyDummy() {cmdenvDummy();tkenvDummy();}
#endif
*/

// A dummy function to force UNIX linkers collect Speedometer
// and cFileOutputVectorManager as linker symbols. Otherwise we'd get
// "undefined symbol" messages...
void env_dummy_function() {
    exponential(1.0);
    Speedometer a;
    cFileOutputVectorManager o;
    (void)a; (void)o; // eliminate 'unused var' warning
#ifdef WITH_AKAROA
    cAkOutputVectorManager ao;
    cAkaroaRNG ar;
    (void)ao; (void)ar; // eliminate 'unused var' warning
#endif
}

