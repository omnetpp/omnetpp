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
  Copyright (C) 1992,99 Andras Varga
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

//==========================================================================
// TSlaveApp: "slave mode" simulation application
//
// Leave the choice to the user whether he/she really wants to use
// this very slave application or rather derive another one.
//
// We leave TOmnetApp *TOmnetApp::createSlave() undefined here.
//
//--------------

TSlaveApp::TSlaveApp(int argc, char *argv[]) : TOmnetApp(argc, argv)
{
     // these are default values and will be overwritten by ini file settings
     opt_write_slavelog = TRUE;
     opt_slavelogfile_name = "slave.log";
}

TSlaveApp::~TSlaveApp()
{
}

void TSlaveApp::run()
{
     if (!simulation.ok()) return;

     /* execute a simulation run, then exit */
     int run_nr = simulation.netInterface()->receive_runnumber();

     readPerRunOptions( run_nr );

     cNetworkType *network = findNetwork( opt_network_name );
     if (!network)
     {
         opp_error("Network `%s' not found", (char *)opt_network_name);
         return;
     }

     ev.printf("Setting up network `%s'...\n", (char *)opt_network_name);
     simulation.setupNetwork( network, run_nr );
     if (simulation.ok())
     {
        makeOptionsEffective();

        if (simulation.ok())
        {
            simulation.startRun();

            if (simulation.ok())
            {
               simulate();
            }
            simulation.endRun();
        }
     }
     simulation.deleteNetwork();
}

void TSlaveApp::simulate()
{
     simulation.startClock();
     while(1)
     {
            cSimpleModule *mod = simulation.selectNextModule();
            if (mod==NULL || !simulation.ok()) break;
            simulation.transferTo( mod );
            if (!simulation.ok()) break;
     }
     simulation.stopClock();
}

void TSlaveApp::shutdown()
{
     TOmnetApp::shutdown();
}

void TSlaveApp::readOptions()
{
     TOmnetApp::readOptions();

     opt_write_slavelog = ini_file->getAsBool("Slaves", "write-slavelog", TRUE );
     opt_slavelogfile_name = ini_file->getAsString("Slaves", "slavelog-file", "slave.log" );
     opt_module_msgs = ini_file->getAsBool("Slaves", "module-messages", TRUE );
     opt_errmsgs2cons = ini_file->getAsBool("Slaves", "errmsgs-to-console", TRUE );
     opt_infomsgs2cons = ini_file->getAsBool("Slaves", "infomsgs-to-console", FALSE );
     opt_modmsgs2cons = ini_file->getAsBool("Slaves", "modmsgs-to-console", FALSE );
}


void TSlaveApp::putmsg(char *str)
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

void TSlaveApp::puts(char *str)
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

bool TSlaveApp::gets(char *promptstr, char *buf, int len)
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
        return FALSE;
}

int TSlaveApp::askYesNo(char *question )
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
