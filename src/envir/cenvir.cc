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
#include "cmodule.h"
#include "cnetmod.h"

//=== Global objects:
cEnvir ev;

static char buffer[1024];

// A dummy function to force UNIX linkers collect TSlaveApp's constructor
// as a linker symbol. Otherwise we get "undefined symbol" for it.
void tslave_dummy_function() {TSlaveApp x(0,NULL);}

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

     if (running_mode!=SLAVE_MODE)
        app = TOmnetApp::create(ac,av);
     else  // SLAVE_MODE
        app = TOmnetApp::createSlave(ac,av);

     if (app!=NULL)  app->setup(ac,av);
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

char *cEnvir::getParameter(int run_no, char *parname)
{
    return app->getParameter(run_no, parname);
}

char *cEnvir::getPhysicalMachineFor(char *logical_mach)
{
    return app->getPhysicalMachineFor(logical_mach);
}

void cEnvir::getOutVectorConfig(char *modname,char *vecname,
                                bool& enabled,
                                double& starttime, double& stoptime)
{
    app->getOutVectorConfig(modname,vecname,  enabled,starttime,stoptime);
}

char *cEnvir::getDisplayString(int run_no,char *name)
{
    return app->getDisplayString(run_no,name);
}

//-----------------------------------------------------------------

cEnvir& cEnvir::setPrompt(char *s)
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

void cEnvir::breakpointHit( char *label, cSimpleModule *module )
{
    app->breakpointHit( label, module );
}

//-----------------------------------------------------------------

void cEnvir::printfmsg(char *fmt,...)
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

void cEnvir::printf(char *fmt,...)
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

void cEnvir::puts(char *str)
{
    if (disable_tracing) return;

    if (app)
        app->puts( str );
    else
        ::printf("%s", buffer);
}

bool cEnvir::gets(char *promptstr, char *buf, int len)
{
    int esc = app->gets( promptstr, buf, len );
    if (esc) opp_error(eCANCEL);
    return (bool)esc;
}

bool cEnvir::askf(char *buf, int len, char *promptfmt,...)
{
    va_list va;
    va_start(va, promptfmt);
    vsprintf(buffer, promptfmt, va);
    va_end(va);

    int esc = app->gets( buffer, buf, len );
    if (esc) opp_error(eCANCEL);
    return (bool)esc;
}

bool cEnvir::askYesNo(char *msgfmt,...)
{
    va_list va;
    va_start(va, msgfmt);
    vsprintf(buffer, msgfmt, va);
    va_end(va);

    int ret = app->askYesNo( buffer );
    if (ret<0) opp_error(eCANCEL);
    return (bool)ret;
}

void cEnvir::foreignPuts(char *hostname, char *mod, char *str)
{
    app->foreignPuts(hostname,mod,str);
}

unsigned cEnvir::extraStackForEnvir()
{
    return app->extraStackForEnvir();
}
