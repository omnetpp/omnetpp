//==========================================================================
//  TKAPP.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __TKAPP_H
#define __TKAPP_H

#include <tk.h>
#include <vector>
#include <list>

#include "omnetapp.h"

#ifdef SUNSPARC
#define TKENV_EXTRASTACK   24576
#else
#define TKENV_EXTRASTACK   16384
#endif

class Speedometer;
class TInspector;

// heuristic upper limits for various strings: obj->className(), obj->fullPath(), obj->info()
#define MAX_CLASSNAME       100
#define MAX_OBJECTFULLPATH  500
#define MAX_OBJECTINFO      500

//=========================================================================
// TOmnetTkApp: Tcl/Tk-based user interface.

class TOmnetTkApp : public TOmnetApp
{
   public:
      //
      // state transitions:
      //    SIM_NONET -> SIM_NEW -> (SIM_RUNNING <-> SIM_READY) -> SIM_TERMINATED -> SIM_FINISHCALLED -> SIM_NONET
      //                                               `-> SIM_ERROR
      // plus, at any time it may be temporarily BUSY inside an in idle() call
      //
      enum eState {
          SIM_NONET = 0,
          SIM_NEW = 1,
          SIM_RUNNING = 2,
          SIM_READY = 3,
          SIM_TERMINATED = 4,
          SIM_ERROR = 5,
          SIM_FINISHCALLED = 6,
          SIM_BUSY = 7  // busy doing active wait
      };

      enum eRunMode {
          RUNMODE_SLOW = 0,
          RUNMODE_NORMAL = 1,
          RUNMODE_FAST = 2,
          RUNMODE_EXPRESS = 3,
      };

      struct sPathEntry {
         cModule *from; // NULL if descent
         cModule *to;   // NULL if ascent
         sPathEntry(cModule *f, cModule *t) {from=f; to=t;}
      };
      typedef std::vector<sPathEntry> PathVec;

   public:
      // options
      int  opt_default_run;        // automatically set up this run at startup
      bool opt_bkpts_enabled;      // stop at breakpoints (can be improved...)
      bool opt_print_banners;      // print event banners
      bool opt_use_mainwindow;     // dump modules' ev << ... stuff into main window
      bool opt_animation_enabled;  // msg animation
      bool opt_nexteventmarkers;   // display next event marker (red frame around modules)
      bool opt_senddirect_arrows;  // flash arrows when doing sendDirect() animation
      bool opt_anim_methodcalls;   // animate method calls
      int  opt_methodcalls_delay;  // hold animation of method calls for this many ms
      bool opt_animation_msgnames; // msg animation: display message name or not
      bool opt_animation_msgcolors;// msg animation: display msg kind as color code or not
      bool opt_penguin_mode;       // msg animation: message appearance
      bool opt_showlayouting;      // show layouting process in graphical module inspectors
      bool opt_bubbles;            // show result of bubble() calls
      double opt_animation_speed;  // msg animation speed: 0=slow 1=norm 2=fast
      long opt_stepdelay;          // Delay between steps in ms
      int  opt_updatefreq_fast;    // FastRun updates display every N events
      int  opt_updatefreq_express; // RunExpress updates display every N events
      unsigned opt_extrastack;     // per-module extra stack

      // state variables
      bool animating;              // while execution, do message animation or not

   protected:
      Tcl_Interp *interp;          // Tcl interpreter

      opp_string tkenv_dir;        // directory of Tkenv's *.tcl files
      opp_string bitmap_dir;       // directory of module icon files

      eState simstate;             // state of the simulation run
      int runmode;                 // the current mode the simulation is executing under
      int run_nr;                  // number of current simulation run
      bool  breakpointhit_flag;    // flag to signal that a breakpoint was hit and sim. must be stopped
      bool  stopsimulation_flag;   // flag to signal that simulation must be stopped (STOP button pressed in the UI)

      typedef std::list<TInspector*> TInspectorList;
      TInspectorList inspectors;   // list of inspector objects

      opp_string windowtitleprefix;// contains "procId=.." when using parsim

   public:
      TOmnetTkApp(ArgList *args, cIniFile *inifile);
      ~TOmnetTkApp();

      // redefined virtual functions from TOmnetApp
      virtual void setup();
      virtual int run();
      virtual void shutdown();

      virtual void objectDeleted(cObject *object); // notify environment
      virtual void messageSent(cMessage *msg, cGate *directToGate);
      virtual void messageDelivered(cMessage *msg);
      virtual void breakpointHit(const char *lbl, cSimpleModule *mod);
      virtual void moduleMethodCalled(cModule *from, cModule *to, const char *method);
      virtual void moduleCreated(cModule *newmodule);
      virtual void moduleDeleted(cModule *module);
      virtual void connectionCreated(cGate *srcgate);
      virtual void connectionRemoved(cGate *srcgate);
      virtual void displayStringChanged(cGate *gate);
      virtual void displayStringChanged(cModule *submodule);
      virtual void displayStringAsParentChanged(cModule *parentmodule);

      virtual bool isGUI() {return true;}
      virtual void bubble(cModule *mod, const char *text);

      virtual void putmsg(const char *s);
      virtual void puts(const char *s);
      virtual void flush();
      virtual bool gets(const char *promptstr, char *buf, int len=255);
      virtual int  askYesNo(const char *question);

      virtual void readOptions();
      virtual void readPerRunOptions(int run_nr);

      virtual bool idle();

      // if using Tkenv, modules should have ~16K extra stack
      virtual unsigned extraStackForEnvir();

      // New functions:
      void newNetwork(const char *networkname);
      void newRun(int run_no);
      void createSnapshot(const char *label);

      void rebuildSim();
      void doOneStep();
      void runSimulation(simtime_t until_time, long until_event, int runmode, cSimpleModule *stepwithinmodule=NULL);
      void setSimulationRunMode(int runmode);
      bool doRunSimulation(simtime_t until_time, long until_event, cSimpleModule *stepwithinmodule=NULL);
      bool doRunSimulationExpress(simtime_t until_time, long until_event);

      void startAll();
      void finishSimulation(); // wrapper around simulation.callFinish() and simulation.endRun()

      void loadNedFile(const char *fname);

      bool isBreakpointActive(const char *label, cSimpleModule *mod);
      void stopAtBreakpoint(const char *label, cSimpleModule *mod);

      void updateInspectors();
      TInspector *inspect(cObject *obj, int type, const char *geometry, void *dat);
      TInspector *findInspector(cObject *obj, int type);
      void deleteInspector(TInspector *insp);

      int getSimulationState() {return simstate;}
      void setStopSimulationFlag() {stopsimulation_flag = true;}
      Tcl_Interp *getInterp() {return interp;}

      void updateGraphicalInspectorsBeforeAnimation();

      // small functions:
      void updateNetworkRunDisplay();
      void updateSimtimeDisplay();
      void updateNextModuleDisplay();
      void clearNextModuleDisplay();
      void updatePerformanceDisplay(Speedometer& speedometer);
      void clearPerformanceDisplay();

      void printEventBanner(cSimpleModule *mod);
      void animateSend(cMessage *msg, cGate *fromgate, cGate *togate);
      void animateSendDirect(cMessage *msg, cModule *frommodule, cGate *togate);
      void animateDelivery(cMessage *msg);
      void animateDeliveryDirect(cMessage *msg);

      void findDirectPath(cModule *frommodule, cModule *tomodule, PathVec& pathvec);

      const char *getIniFileName()       {return opt_inifile_name;}
      const char *getOutVectorFileName() {return outvectmgr->fileName();}
      const char *getOutScalarFileName() {return outscalarmgr->fileName();}
      const char *getSnapshotFileName()  {return snapshotmgr->fileName();}
      const char *getWindowTitlePrefix() {return correct(windowtitleprefix);}

};


/**
 * Utility function
 */
inline TOmnetTkApp *getTkApplication()
{
    return (TOmnetTkApp *)(ev.app);
}

#endif
