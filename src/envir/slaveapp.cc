//==========================================================================
//   SLAVEAPP.CC - part of
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Implementation of the following classes:
//    TSlaveApp
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
#include "slaveapp.h"
#include "cinifile.h"
#include "csimul.h"
#include "cnetmod.h"
#include "appreg.h"


Register_OmnetApp("SlaveEnv",TSlaveApp,true,10,"default slave interface, for distributed execution");


//==========================================================================
// TSlaveApp: "slave mode" simulation application
//
// Leave the choice to the user whether he/she really wants to use
// this very slave application or rather derive another one.
//
// We leave TOmnetApp *TOmnetApp::createSlave() undefined here.
//
//--------------

TSlaveApp::TSlaveApp(ArgList *args, cIniFile *inifile) : TOmnetApp(args, inifile)
{
     // these are default values and will be overwritten by ini file settings
     opt_write_slavelog = true;
     opt_slavelogfile_name = "slave.log";
}

TSlaveApp::~TSlaveApp()
{
}

void TSlaveApp::run()
{
     if (!initialized)
         return;

     // FIXME setupSignals();

     // execute a simulation run, then exit
     int run_nr = simulation.netInterface()->receive_runnumber();

     bool setupnetwork_done = false;
     bool startrun_done = false;
     try
     {
         //ev.printf("\nPreparing for Run #%d...\n", run_nr());

         readPerRunOptions(run_nr);

         // find network
         cNetworkType *network = findNetwork(opt_network_name);
         if (!network)
             throw new cException("Network `%s' not found, check .ini and .ned files", (const char *)opt_network_name);

         // set up network
         ev.printf("Setting up network `%s'...\n", (const char *)opt_network_name);
         simulation.setupNetwork(network, run_nr);
         setupnetwork_done = true;

         makeOptionsEffective();

         // start execution on other segments too
         startRun();
         startrun_done = true;

         // run the simulation
         //ev.printf("Running simulation...\n");

         // simulate() should only throw exception if error occurred and
         // finish() should not be called. sigint_received is treated differently
         simulate();

         simulation.callFinish();
     }
     catch (cException *e)
     {
         displayError(e);
         delete e;
     }

     // stop run on other segments
     if (startrun_done)
     {
         try
         {
             simulation.endRun();
         }
         catch (cException *e)
         {
             displayError(e);
             delete e;
         }
     }

     // delete network (this also forces slaves to exit)
     if (setupnetwork_done)
     {
         try
         {
             simulation.deleteNetwork();
         }
         catch (cException *e)
         {
             displayError(e);
             delete e;
         }
     }
}

void TSlaveApp::simulate()
{
/* FIXME to be revised!
     simulation.startClock();
     while(1)
     {
         cSimpleModule *mod = simulation.selectNextModule();
         if (mod==NULL || !simulation.ok()) break;
         simulation.transferTo( mod ); //FIXME!!!!
         if (!simulation.ok()) break;
     }
     simulation.stopClock();
*/
}

void TSlaveApp::shutdown()
{
     TOmnetApp::shutdown();
}

void TSlaveApp::readOptions()
{
     TOmnetApp::readOptions();

     opt_write_slavelog = ini_file->getAsBool("Slaves", "write-slavelog", true );
     opt_slavelogfile_name = ini_file->getAsString("Slaves", "slavelog-file", "slave.log" );
     opt_module_msgs = ini_file->getAsBool("Slaves", "module-messages", true );
     opt_errmsgs2cons = ini_file->getAsBool("Slaves", "errmsgs-to-console", true );
     opt_infomsgs2cons = ini_file->getAsBool("Slaves", "infomsgs-to-console", false );
     opt_modmsgs2cons = ini_file->getAsBool("Slaves", "modmsgs-to-console", false );
}


void TSlaveApp::putmsg(const char *str)
{
     if (opt_write_slavelog)
     {
        FILE *f=fopen( opt_slavelogfile_name,"a");
        fprintf(f,"<!> %s\n",str);
        fclose(f);
     }

     if (opt_errmsgs2cons && simulation.netInterface())
         simulation.netInterface()->putmsg_onconsole( str );
}

void TSlaveApp::puts(const char *str)
{
     int is_modmsg = simulation.contextModule()!=NULL;
     if (is_modmsg && !opt_module_msgs)
        return;

     if (opt_write_slavelog)
     {
        FILE *f=fopen( opt_slavelogfile_name,"a");
        fprintf(f,"%s",str);
        fclose(f);
     }

     if (simulation.netInterface() && (is_modmsg?opt_modmsgs2cons:opt_infomsgs2cons))
         simulation.netInterface()->puts_onconsole( str );
}

bool TSlaveApp::gets(const char *promptstr, char *buf, int len)
{
     if (opt_write_slavelog)
     {
        FILE *f=fopen( opt_slavelogfile_name,"a");
        fprintf(f,"%s (%s)",promptstr, buf);
        fclose(f);
     }

     if (simulation.netInterface())
        return simulation.netInterface()->gets_onconsole(promptstr,buf,len);
     else
        return false;
}

int TSlaveApp::askYesNo(const char *question)
{
     if (opt_write_slavelog)
     {
        FILE *f=fopen( opt_slavelogfile_name,"a");
        fprintf(f,"%s (y/n)",question);
        fclose(f);
     }

     if (simulation.netInterface())
        return simulation.netInterface()->askyesno_onconsole( question );
     else
        return 0;
}
