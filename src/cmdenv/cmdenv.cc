//==========================================================================
//   CMDENV.CC - part of
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Implementation of the following classes:
//    TCmdenvApp
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmddefs.h"
#include "cmdenv.h"
#include "enumstr.h"
#include "appreg.h"
#include "cinifile.h"
#include "cmodule.h"
#include "cnetmod.h"
#include "args.h"

static char buffer[1024];

//
// Register the Cmdenv user interface
//
Register_OmnetApp("Cmdenv",TCmdenvApp,false,10,"Cmdenv (command-line user interface)");

// some functions that can/should be called from Envir in order to force the
// linker to include the Tkenv library into the executable:
CMDENV_API void cmdenvDummy() {}
CMDENV_API void envirDummy() {}

// a function defined in heap.cc
bool cmdenvMemoryIsLow();

//==========================================================================
// TCmdenvApp: command line user interface.

TCmdenvApp::TCmdenvApp(ArgList *args, cIniFile *inifile) : TOmnetApp(args, inifile)
{
     opt_modulemsgs = true;
     opt_verbose = true;
}

TCmdenvApp::~TCmdenvApp()
{
}

void TCmdenvApp::readOptions()
{
     TOmnetApp::readOptions();

     opt_runstoexec = ini_file->getAsString("Cmdenv", "runs-to-execute", "");
     opt_extrastack = ini_file->getAsInt("Cmdenv", "extra-stack", CMDENV_EXTRASTACK);
}

void TCmdenvApp::readPerRunOptions( int run_nr )
{
     TOmnetApp::readPerRunOptions( run_nr);

     char section[16];
     sprintf(section,"Run %d",run_nr);
     ini_file->error(); // clear error flag

     opt_modulemsgs = ini_file->getAsBool2( section,"Cmdenv", "module-messages", true );
     opt_verbose = ini_file->getAsBool2( section,"Cmdenv", "verbose-simulation", true );
     opt_displayinterval = ini_file->getAsTime2( section,"Cmdenv", "display-update", 10.0 );
}

void TCmdenvApp::setup()
{
     // initialize base class
     TOmnetApp::setup();    // implies readOptions()
     if (!simulation.ok())
         return;

     // '-h' (help) flag
     opt_helponly = args->argGiven( 'h' );
     if (opt_helponly) return;  // only give a command line help

     // '-r' option: specifies runs to execute; overrides ini file setting
     char *r = args->argValue( 'r' );
     if (r)
        opt_runstoexec = r;

     // If the list of runs is not given explicitly, must execute all runs
     // that have an ini file section; if no runs have ini file sections,
     // must do one run.
     if ((const char *)opt_runstoexec==NULL)
     {
        buffer[0]='\0';
        for (cIniFileSectionIterator sect(ini_file); !sect.end(); sect++)
           if (strncmp(sect.section(),"Run ",4)==0)
           {
               strcat(buffer,sect.section()+4);
               strcat(buffer,",");
           }
        if (buffer[0]=='\0')
           strcpy(buffer,"1");
        opt_runstoexec = buffer;
     }
}

void TCmdenvApp::run()
{
     if (!simulation.ok()) return;

     if (opt_helponly)
     {
         help();
         return;
     }

     EnumStringIterator run_nr( (const char *)opt_runstoexec );
     if (run_nr.error())
     {
         ev.printfmsg("Error parsing list of runs to execute: `%s'",
                      (const char *)opt_runstoexec );
         return;
     }

     for (; run_nr()!=-1; run_nr++)
     {
         ev.printf("\n"
                   "Preparing for Run #%d...\n", run_nr() );

         readPerRunOptions( run_nr() );

         cNetworkType *network = findNetwork( opt_network_name );
         if (network==NULL)
         {
             opp_error("Network `%s' not found, check .ini and .ned files", (const char *)opt_network_name);
             continue;
         }

         ev.printf("Setting up network `%s'...\n", (const char *)opt_network_name);

         simulation.setupNetwork( network, run_nr() );
         if (simulation.ok())
         {
            makeOptionsEffective();
            if (simulation.ok())
            {
               simulation.startRun();

               if (simulation.ok())
               {
                   ev.printf("Running simulation...\n");
                   simulate();
               }
               if (simulation.normalTermination()) simulation.callFinish();
               simulation.endRun();
            }
         }

         // end of run: delete network (this also forces slaves to exit)
         simulation.deleteNetwork();
     }
}

void TCmdenvApp::makeOptionsEffective()
{
     TOmnetApp::makeOptionsEffective();
     // further stuff may come here
}

void TCmdenvApp::simulate()
{
     simulation.startClock();
     if (opt_verbose || opt_modulemsgs)
     {
        while(1)
        {
            cSimpleModule *mod = simulation.selectNextModule();
            if (mod==NULL || !simulation.ok()) break;

            // print event banner if neccessary
            if (opt_verbose)
            {
               if (mod->phase()[0]==0)
               {
                   printf( "** Event #%ld  T=%s.  Module #%d `%s'.\n",
                           simulation.eventNumber(),
                           simtimeToStr( simulation.simTime() ),
                           mod->id(),
                           mod->fullPath()
                         );
               }
               else
               {
                   printf( "** Event #%ld  T=%s.  Module #%d `%s' in phase `%s'.\n",
                           simulation.eventNumber(),
                           simtimeToStr( simulation.simTime() ),
                           mod->id(),
                           mod->fullPath(),
                           mod->phase()
                         );
               }
            }

            // execute event
            simulation.doOneEvent( mod );
            simulation.incEventNumber();
            if (!simulation.ok()) break;
        }
     }
     else
     {
        ev.disable_tracing = true;
        simtime_t next_update = 0.0 + opt_displayinterval;
        while(1)
        {
            cSimpleModule *mod = simulation.selectNextModule();
            if (mod==NULL || !simulation.ok()) break;

            // print event banner from time to time
            if (next_update <= simulation.simTime())
            {
                next_update += opt_displayinterval;
                printf( "** Event #%ld \tT=%s.\n",
                        simulation.eventNumber(),
                        simtimeToStr(simulation.simTime()) );
            }

            // execute event
            simulation.doOneEvent( mod );
            simulation.incEventNumber();
            if (!simulation.ok()) break;
        }
        ev.disable_tracing = false;
     }
     simulation.stopClock();
}

void TCmdenvApp::help()
{
     ev << "\n";
     ev << "Command line switches:\n";
     ev << "  -h               this help\n";
     ev << "  -fsomething.ini  specifies ini file to use (instead of omnetpp.ini)\n";
     ev << "  -r1,2,5-10       specifies runs to be executed\n";
     ev << "\n";
     ev << "Networks linked into this executable:\n";
     cIterator iter(networks);
     for (int i=1; iter(); i++,iter++)
        ev << "  " << i << ". " << ((cNetworkType *)iter())->name() << '\n';
}

void TCmdenvApp::puts(const char *s)
{
     if (opt_modulemsgs || simulation.contextModule()==NULL)
         TOmnetApp::puts( s );
}

unsigned TCmdenvApp::extraStackForEnvir()
{
     return opt_extrastack;
}

bool TCmdenvApp::memoryIsLow()
{
     return cmdenvMemoryIsLow();
}
