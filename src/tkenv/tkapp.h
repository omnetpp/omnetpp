//==========================================================================
//  TKAPP.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __TKAPP_H
#define __TKAPP_H

#include <tk.h>

#include "omnetapp.h"

//=========================================================================

class TOmnetTkApp : public TOmnetApp
{
   public:
      int  opt_default_run;        // automatically set up this run at startup
      bool opt_bkpts_enabled;      // stop at breakpoints (can be improved...)
      bool opt_print_banners;      // print event banners
      bool opt_use_mainwindow;     // dump modules' ev << ... stuff into main window
      bool opt_animation_enabled;  // msg animation
      bool opt_animation_msgnames; // msg animation: display message name or not
      bool opt_animation_msgcolors;// msg animation: display msg kind as color code or not
      double opt_animation_speed;  // msg animation speed: 0=slow 1=norm 2=fast
      long opt_stepdelay;          // Delay between steps in 100th seconds
      int  opt_updatefreq_fast;    // FastRun updates display every N events
      int  opt_updatefreq_express; // RunNoTracing updates display every N events

      Tcl_Interp *interp;      // TCL interpreter
      opp_string tkenv_dir;    // directory of Tkenv's *.tcl files
      opp_string bitmap_dir;   // directory of icon files

      int   run_nr;            // number of current simulation run
      bool  animation_ok;      // while execution, do message animation or not
      bool  bkpt_hit;          // TRUE when must stop sim. due to breakpoint
      bool  stop_simulation;   // TRUE when must stop simulation
      bool  is_running;        // TRUE while simulation is running

      cHead inspectors;        // list of inspector objects
   public:

      TOmnetTkApp(int argc, char *argv[]);
      ~TOmnetTkApp();

      // redefined virtual functions from TOmnetApp
      virtual void setup(int argc, char *argv[]);
      virtual void run();

      virtual void objectDeleted(cObject *object); // notify environment
      virtual void messageSent(cMessage *msg);
      virtual void messageDelivered(cMessage *msg);
      virtual void breakpointHit(const char *lbl, cSimpleModule *mod);

      virtual void putmsg(const char *s);
      virtual void puts(const char *s);
      virtual bool gets(const char *promptstr, char *buf, int len=255);
      virtual int  askYesNo(const char *question);

      virtual void readOptions();
      virtual void readPerRunOptions(int run_nr);

      // if using Tkenv, modules should have 16K extra stack
      virtual unsigned extraStackForEnvir() {return 16384;}

      // New functions:
      void newNetwork( const char *networkname );
      void newRun( int run_no );
      void createSnapshot(const char *label);

      void rebuildSim();
      void doOneStep();
      void runSimulation(simtime_t until_time, long until_event,
                         bool slowexec, bool fast,
                         cSimpleModule *stopinmod=NULL);
      void runSimulationNoTracing( simtime_t until_time, long until_event );
      void startAll();
      void callFinish();

      void inspectorByName();
      void newMsgWindow();
      void newFileWindow();

      bool isBreakpointActive(const char *label, cSimpleModule *mod);
      void stopAtBreakpoint(const char *label, cSimpleModule *mod);

      void updateInspectors();
      TInspector *inspect(cObject *obj, int type, void *dat);
      TInspector *findInspector(cObject *obj, int type);

      // small functions:
      void updateNetworkRunDisplay();
      void updateSimtimeDisplay();
      void updateNextModuleDisplay();
      void clearNextModuleDisplay();
      void printEventBanner(cSimpleModule *mod);

      const char *getIniFileName()       {return opt_inifile_name;}
      const char *getSnapshotFileName()  {return opt_snapshotfile_name;}
      const char *getOutVectorFileName() {return opt_outvectfile_name;}
      const char *getParChangeFileName() {return opt_parchangefile_name;}
};

#endif
