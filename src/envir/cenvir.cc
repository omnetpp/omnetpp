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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "args.h"
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

void cEnvir::setup(int ac, char *av[])
{
     argc = ac;
     argv = av;

     argInit(argc,argv);

     running_mode = is_started_as_master();
     // ==> MASTER_MODE, SLAVE_MODE, NONPARALLEL_MODE or STARTUPERROR_MODE

     // choose and set up user interface (TOmnetApp subclass)
     bool slave = (running_mode==SLAVE_MODE); // slave or normal app?
     cOmnetAppRegistration *appreg = chooseBestOmnetApp(slave);
     if (!appreg)
         {opp_error("No appropriate user interface (Cmdenv,Tkenv,etc.) found");return;}

     ::printf("Setting up %s...\n", appreg->description());

     app = appreg->createOne(ac,av);
     app->setup(ac,av);
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
