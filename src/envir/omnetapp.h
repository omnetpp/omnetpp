//==========================================================================
//   OMNETAPP.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    TOmnetApp:           abstract base class for simulation application
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETAPP_H
#define __OMNETAPP_H

#include "carray.h"
#include "cenvir.h"


class cIniFile;

#define NUM_STARTINGSEEDS  256
extern long starting_seeds[NUM_STARTINGSEEDS];  // they are in seeds.cc

//==========================================================================
// TOmnetApp: abstract base class for the user interface.
//      Concrete user interface implementations (Cmdenv, Tvenv,
//      Tkenv's app classes) should be derived from this class, and
//      the create() static function must be written to create an object
//      of the desired type (ie.TOmnetCmdApp, TOmnetWindowsApp etc.)
//      The createSlave() function is used when doing distributed simulation:
//      it should create a "slave" application that can work with the "master"
//      app that runs on the console.
//
class TOmnetApp
{
   protected:
     cIniFile *ini_file;

     // options common for all simulation applications
     opp_string opt_inifile_name;

     int opt_total_stack_kb;

     bool opt_ini_warnings;
     bool opt_distributed; // NET
     opp_string opt_load_libs;
     opp_string opt_network_name;
     opp_string opt_snapshotfile_name;
     opp_string opt_outvectfile_name;
     opp_string opt_scalarfile_name;

     bool opt_logparchanges;
     opp_string opt_parchangefile_name;

     bool opt_pause_in_sendmsg;
     bool opt_warnings;

     long *opt_genk_randomseed;

     simtime_t opt_simtimelimit;
     long opt_cputimelimit;

     int opt_netifcheckfreq;

     int next_startingseed;  // index of next seed to use

   public:
     // defined together with concrete application types, they create
     // an application object of the appropriate type
     static TOmnetApp *create(int argc, char *argv[]);
     static TOmnetApp *createSlave(int argc, char *argv[]);

     TOmnetApp(int argc, char *argv[]);
     virtual ~TOmnetApp();

     // functions called from cEnvir's similar functions
     virtual void setup(int argc, char *argv[]);
     virtual void run() = 0;
     virtual void shutdown();

     // called by cPar::read() to get param value from the ini file
     virtual char *getParameter(int run_nr, char *parname);
     // called from JAR-generated network setup function to get
     //  logical machine --> physical machine mapping
     virtual char *getPhysicalMachineFor(char *logical_mach);
     virtual void getOutVectorConfig(char *modname,char *vecname, /*input*/
                                     bool& enabled, /*output*/
                                     double& starttime, double& stoptime);
     virtual char *getDisplayString(int run_no,char *name);

     // used internally to read opt_xxxxx setting from ini file
     virtual void readOptions();
     virtual void readPerRunOptions(int run_nr);

     // used internally to make options effective in cSimulation and other places
     virtual void makeOptionsEffective();

     // dynamically load a library (uses dlopen())
     virtual void loadLibrary(char *libname);

     // functions called from the objects of the simulation kernel
     // to notify application of certain events
     virtual void objectDeleted(cObject *object) {}
     virtual void messageSent(cMessage *msg) {}
     virtual void messageDelivered(cMessage *msg) {}
     virtual void breakpointHit(char *lbl, cSimpleModule *mod) {}

     // functions called by cEnvir's similar functions
     // provide I/O for simple module activity functions and the sim. kernel
     // default versions use standard I/O
     virtual void putmsg(char *s);
     virtual void puts(char *s);
     virtual bool gets(char *promptstr, char *buf, int len=255);  // 0==OK 1==CANCEL
     virtual int  askYesNo(char *question); //0==NO 1==YES -1==CANCEL
     virtual void foreignPuts(char *hostname, char *mod, char *str);

     // extraStackForEnvir() is called from cSimpleModule; returns how much extra
     // stack space the user interface recommends for the simple modules
     virtual unsigned extraStackForEnvir() {return 0;}
};

#endif
