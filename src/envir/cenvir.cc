//==========================================================================
//  CENVIR.CC - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//  Implementation of
//    cEnvir     : user interface class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "args.h"
#include "distrib.h"
#include "cinifile.h"
#include "cenvir.h"
#include "omnetapp.h"
#include "appreg.h"
#include "cmodule.h"
#include "cnetmod.h"
#include "slaveapp.h"   // tslave_dummy_function()
#include "speedmtr.h"   // tslave_dummy_function()
#include "filemgrs.h"   // tslave_dummy_function()


//=== Global objects:
cEnvir ev;

// the global list for the registration objects
cHead omnetapps("user-interfaces");

// output buffer
static char buffer[1024];


//=== The DUMMY SECTION -- a tribute to smart linkers
// force the linker to include the user interface library into the executable.
// dummyDummy() can't be static or very smart linkers will leave it out...
#ifndef WIN32_DLL
void envirDummy();
void dummyDummy() {envirDummy();}
#endif

// another dummy variant in case you want to have both Cmdenv and Tkenv in
//#ifndef WIN32_DLL
//OPP_DLLIMPORT void cmdenvDummy();
//OPP_DLLIMPORT void tkenvDummy();
//void dummyDummy() {cmdenvDummy();tkenvDummy();}
//#endif

// A dummy function to force UNIX linkers collect TSlaveApp, Speedometer
// and cFileOutputVectorManager as linker symbols. Otherwise we'd get
// "undefined symbol" messages...
void tslave_dummy_function() {exponential(1.0);TSlaveApp x(0,NULL);Speedometer a;cFileOutputVectorManager o;}

//========================================================================

// User interface factory functions.
cOmnetAppRegistration *chooseBestOmnetApp(bool slave)
{
    cOmnetAppRegistration *best_appreg = NULL;

    // choose the one with appropriate slave flag and highest score.
    for (cIterator i(omnetapps); !i.end(); i++)
    {
        cOmnetAppRegistration *appreg = (cOmnetAppRegistration*) i();
        if (appreg->isSlave()==slave && (!best_appreg || appreg->score()>best_appreg->score()))
            best_appreg = appreg;
    }
    return best_appreg;
}

//========================================================================

cEnvir::cEnvir()
{
    running_mode = NONPARALLEL_MODE;
    prmpt[0] = '\0';
    disable_tracing = false;

    app = NULL;
}

cEnvir::~cEnvir()
{
}

void cEnvir::setup(int argc, char *argv[])
{
    try
    {
        // construct global lists
        ExecuteOnStartup::executeAll();

        // args
        ArgList *args = new ArgList(argc,argv);

        //
        // First, load the ini file. It might contain the name of the user interface
        // to instantiate.
        //
        const char *fname = args->argValue('f',0);  // 1st '-f filename' option
        if (!fname) fname="omnetpp.ini";   // or default filename

        cIniFile *ini_file = new cIniFile( fname );
        if (ini_file->error())
            throw new cException("Processing of ini file '%s' failed", fname);

        // process additional '-f filename' options if there are any
        int k;
        for (k=1; (fname=args->argValue('f',k))!=NULL; k++)
        {
            ini_file->readFile( fname );
            if (ini_file->error())
                throw new cException("Processing of additional ini file '%s' failed", fname);
        }

        //
        // Load all libraries specified on the command line or in the ini file.
        // (The user interface library also might be among them.)
        //

        // load shared libraries given with '-l' option(s)
        const char *libname;
        for (k=0; (libname=args->argValue('l',k))!=NULL; k++)
            opp_loadlibrary(libname);

        // load shared libs given in [General]/load-libs=
        const char *libs = ini_file->getAsString( "General", "load-libs", NULL);
        if (libs && libs[0])
        {
            // 'libs' contains several file names separated by whitespaces
            char *buf = opp_strdup(libs);
            char *lib, *s = buf;
            while (isspace(*s)) s++;
            while (*s)
            {
                lib = s;
                while (*s && !isspace(*s)) s++;
                if (*s) *s++ = 0;
                opp_loadlibrary(lib);
            }
            delete buf;
        }

        //
        // Determine if this is a distributed simulation, and if so, whether we run
        // as master (=console) or slave. This also affects which user interface
        // to set up.
        // ==> MASTER_MODE, SLAVE_MODE, NONPARALLEL_MODE or STARTUPERROR_MODE
        //
        running_mode = is_started_as_master();

        //
        // Choose and set up user interface (TOmnetApp subclass). Everything else
        // will be done by the user interface class.
        //

        // was it specified explicitly which one to use?
        const char *appname = args->argValue('u',0);  // 1st '-u name' option
        if (!appname)
            appname = ini_file->getAsString( "General", "user-interface", NULL);

        cOmnetAppRegistration *appreg = NULL;
        if (appname && appname[0])
        {
            // try to look up specified user interface; if we don't have it already,
            // try to load dynamically...
            appreg = (cOmnetAppRegistration *) omnetapps.find(appname);
            if (!appreg)
            {
                // try to load it dynamically
                // TBD add extension: .so or .dll
                if (opp_loadlibrary(appname))
                {
                    appreg = (cOmnetAppRegistration *) omnetapps.find(appname);
                }
            }
            if (!appreg)
                throw new cException("Could not start user interface '%s'",appname);
        }
        else
        {
            // user interface not explicitly selected: pick one from what we have
            bool slave = (running_mode==SLAVE_MODE); // slave or normal app?
            appreg = chooseBestOmnetApp(slave);
            if (!appreg)
                throw new cException("No appropriate user interface (Cmdenv,Tkenv,etc.) found");
        }

        //
        // Finally, set up user interface object. All the rest will be done there.
        //
        ::printf("Setting up %s...\n", appreg->description());
        app = appreg->createOne(args, ini_file);
        app->setup();
    }
    catch (cException *e)
    {
        printfmsg("Error during startup: %s", e->message());
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
        app->shutdown();
    delete app;
    app = NULL;
}

//-----------------------------------------------------------------

const char *cEnvir::getParameter(int run_no, const char *parname)
{
    return app->getParameter(run_no, parname);
}

const char *cEnvir::getPhysicalMachineFor(const char *logical_mach)
{
    return app->getPhysicalMachineFor(logical_mach);
}

const char *cEnvir::getDisplayString(int run_no,const char *name)
{
    return app->getDisplayString(run_no,name);
}

//-----------------------------------------------------------------

cEnvir& cEnvir::setPrompt(const char *s)
{
    strncpy(prmpt,s,80);
    prmpt[80] = '\0';
    return *this;
}

void cEnvir::messageSent( cMessage *msg )
{
    if (disable_tracing) return;
    app->messageSent( msg );
}

void cEnvir::messageDelivered( cMessage *msg )
{
    if (disable_tracing) return;
    app->messageDelivered( msg );
}

void cEnvir::objectDeleted( cObject *obj )
{
    if (app) app->objectDeleted( obj );
}

void cEnvir::breakpointHit( const char *label, cSimpleModule *module )
{
    app->breakpointHit( label, module );
}

//-----------------------------------------------------------------

void cEnvir::printfmsg(const char *fmt,...)
{
    va_list va;
    va_start(va, fmt);
    vsprintf(buffer, fmt, va);
    va_end(va);

    if (app)
       app->putmsg( buffer );
    else
       ::printf("<!> %s\n", buffer);
}

void cEnvir::printf(const char *fmt,...)
{
    if (disable_tracing) return;

    va_list va;
    va_start(va, fmt);
    vsprintf(buffer, fmt, va);
    va_end(va);

    if (app)
        app->puts( buffer );
    else
        ::printf("%s", buffer);
}

void cEnvir::puts(const char *str)
{
    if (disable_tracing) return;

    if (app)
        app->puts( str );
    else
        ::printf("%s", buffer);
}

void cEnvir::flush()
{
    if (disable_tracing) return;

    if (app)
        app->flush();
    else
        ::fflush(stdout);
}

bool cEnvir::gets(const char *promptstr, char *buf, int len)
{
    bool esc = app->gets( promptstr, buf, len );
    if (esc)
        throw new cException(eCANCEL);
    return (bool)esc;
}

bool cEnvir::askf(char *buf, int len, const char *promptfmt,...)
{
    va_list va;
    va_start(va, promptfmt);
    vsprintf(buffer, promptfmt, va);
    va_end(va);

    bool esc = app->gets( buffer, buf, len );
    if (esc)
        throw new cException(eCANCEL);
    return (bool)esc;
 }

bool cEnvir::askYesNo(const char *msgfmt,...)
{
    va_list va;
    va_start(va, msgfmt);
    vsprintf(buffer, msgfmt, va);
    va_end(va);

    int ret = app->askYesNo( buffer );
    if (ret<0)
        throw new cException(eCANCEL);
    return ret!=0;
}

void cEnvir::foreignPuts(const char *hostname, const char *mod, const char *str)
{
    app->foreignPuts(hostname,mod,str);
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

void cEnvir::recordScalar(cModule *module, const char *name, const char *text)
{
    app->recordScalar(module, name, text);
}

void cEnvir::recordScalar(cModule *module, const char *name, cStatistic *stats)
{
    app->recordScalar(module, name, stats);
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

unsigned cEnvir::extraStackForEnvir()
{
    return app->extraStackForEnvir();
}

int cEnvir::argCount()
{
    return app->argList()->argCount();
}

char **cEnvir::argVector()
{
    return app->argList()->argVector();
}

bool memoryIsLow()
{
    return ev.app->memoryIsLow();
}

