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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "args.h"
#include "cinifile.h"
#include "cenvir.h"
#include "omnetapp.h"
#include "slaveapp.h"   // dummy_func()
#include "appreg.h"
#include "cmodule.h"
#include "cnetmod.h"

//=== Global objects:
cEnvir ev;

// the global list for the registration objects
cHead omnetapps( "user-interfaces", &superhead );

// output buffer
static char buffer[1024];

// A dummy function to force UNIX linkers collect TSlaveApp's constructor
// as a linker symbol. Otherwise we get "undefined symbol" for it.
void tslave_dummy_function() {TSlaveApp x(0,NULL);}

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
    disable_tracing = FALSE;

    app = NULL;
}

cEnvir::~cEnvir()
{
}

void cEnvir::setup(int argc, char *argv[])
{
    ArgList *args = new ArgList(argc,argv);

    //
    // First, load the ini file. It might contain the name of the user interface
    // to instantiate.
    //
    const char *fname = args->argValue('f',0);  // 1st '-f filename' option
    if (!fname) fname="omnetpp.ini";   // or default filename

    cIniFile *ini_file = new cIniFile( fname );
    if (ini_file->error())
    {
        opp_error("Ini file processing failed");
        return;
    }

    // process additional '-f filename' options if there are any
    int k;
    for (k=1; (fname=args->argValue('f',k))!=NULL; k++)
    {
        ini_file->readFile( fname );
        if (ini_file->error())
        {
            opp_error("Processing of additional ini file failed");
            return;
        }
    }

    //
    // Load all libraries specified on the command line or in the ini file.
    // (The user interface library also might be among them.)
    //

    // load shared libraries given with '-l' option(s)
    const char *libname;
    for (k=0; simulation.ok() && (libname=args->argValue('l',k))!=NULL; k++)
       opp_loadlibrary(libname);

    // load shared libs given in [General]/load-libs=
    const char *libs = ini_file->getAsString( "General", "load-libs", "" );
    if (libs && *libs)
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
        appname = ini_file->getAsString( "General", "user-interface", "" );

    cOmnetAppRegistration *appreg = NULL;
    if (appname)
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
        {
            opp_error("Could not start user interface '%s'",appname);
            return;
        }
    }
    else
    {
        // user interface not explicitly selected: pick one from what we have
        bool slave = (running_mode==SLAVE_MODE); // slave or normal app?
        appreg = chooseBestOmnetApp(slave);
        if (!appreg)
        {
            opp_error("No appropriate user interface (Cmdenv,Tkenv,etc.) found");
            return;
        }
    }

    //
    // Finally, set up user interface object. All the rest will be done there.
    //
    ::printf("Setting up %s...\n", appreg->description());
    app = appreg->createOne(args, ini_file);
    app->setup();
}

void cEnvir::run()
{
    if (app) app->run();
}

void cEnvir::shutdown()
{
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

void cEnvir::getOutVectorConfig(const char *modname,const char *vecname,
                                bool& enabled,
                                double& starttime, double& stoptime)
{
    app->getOutVectorConfig(modname,vecname,  enabled,starttime,stoptime);
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

bool cEnvir::gets(const char *promptstr, char *buf, int len)
{
    bool esc = app->gets( promptstr, buf, len );
    if (esc) opp_error(eCANCEL);
    return (bool)esc;
}

bool cEnvir::askf(char *buf, int len, const char *promptfmt,...)
{
    va_list va;
    va_start(va, promptfmt);
    vsprintf(buffer, promptfmt, va);
    va_end(va);

    bool esc = app->gets( buffer, buf, len );
    if (esc) opp_error(eCANCEL);
    return (bool)esc;
 }

bool cEnvir::askYesNo(const char *msgfmt,...)
{
    va_list va;
    va_start(va, msgfmt);
    vsprintf(buffer, msgfmt, va);
    va_end(va);

    int ret = app->askYesNo( buffer );
    if (ret<0) opp_error(eCANCEL);
    return ret!=0;
}

void cEnvir::foreignPuts(const char *hostname, const char *mod, const char *str)
{
    app->foreignPuts(hostname,mod,str);
}

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

//--------------------------------------------------------------
#if HAVE_DLOPEN
#include <dlfcn.h>
#endif

#include "envdefs.h"   // __WIN32__
#ifdef __WIN32__
#include <windows.h>   // LoadLibrary() et al.
#endif

bool opp_loadlibrary(const char *libname)
{
#if HAVE_DLOPEN
     if (!dlopen(libname,RTLD_NOW))
     {
         opp_error("Cannot load library '%s': %s",libname,dlerror());
         return false;
     }
     return true;
#elif defined(__WIN32__)
     if (!LoadLibrary(libname))
     {
         // Some nice microsoftism to produce an error msg :-(
         LPVOID lpMsgBuf;
         FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                        FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL,
                        GetLastError(),
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        (LPTSTR) &lpMsgBuf,
                        0,
                        NULL );// Process any inserts in lpMsgBuf.
         opp_error("Cannot load library '%s': %s",libname,lpMsgBuf);
         LocalFree( lpMsgBuf );
         return false;
     }
     return true;
#else
     opp_error("Cannot load '%s': dlopen() syscall not available",libname);
     return false;
#endif
}

