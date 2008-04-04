//==========================================================================
//  TKENV.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __TKENV_H
#define __TKENV_H

#define WIN32_LEAN_AND_MEAN
#include <tk.h>
#include <vector>
#include <list>

#include "tkdefs.h"
#include "envirbase.h"
#include "bootenv.h"
#include "cchannel.h"
#include "cmodule.h"

NAMESPACE_BEGIN

class Speedometer;
class TInspector;


/**
 * A Tcl/Tk-based user interface.
 */
class TKENV_API Tkenv : public EnvirBase
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
          RUNMODE_EXPRESS = 3
      };

      struct sPathEntry {
         cModule *from; // NULL if descent
         cModule *to;   // NULL if ascent
         sPathEntry(cModule *f, cModule *t) {from=f; to=t;}
      };
      typedef std::vector<sPathEntry> PathVec;

   public:
      // options
      opp_string opt_default_config; // automatically set up this config at startup
      int  opt_default_run;        // automatically set up this run (of the default config) at startup
      bool opt_print_banners;      // print event banners
      bool opt_use_mainwindow;     // dump modules' ev << ... stuff into main window
      bool opt_animation_enabled;  // msg animation
      bool opt_nexteventmarkers;   // display next event marker (red frame around modules)
      bool opt_senddirect_arrows;  // flash arrows when doing sendDirect() animation
      bool opt_anim_methodcalls;   // animate method calls
      int  opt_methodcalls_delay;  // hold animation of method calls (millisec)
      bool opt_animation_msgnames; // msg animation: display message name or not
      bool opt_animation_msgclassnames; // msg animation: display message class name or not
      bool opt_animation_msgcolors;// msg animation: display msg kind as color code or not
      bool opt_penguin_mode;       // msg animation: message appearance
      bool opt_showlayouting;      // show layouting process in graphical module inspectors
      bool opt_usenewlayouter;     // use new layouting algorithms
      bool opt_bubbles;            // show result of bubble() calls
      double opt_animation_speed;  // msg animation speed: 0=slow 1=norm 2=fast
      long opt_stepdelay;          // Delay between steps in ms
      int  opt_updatefreq_fast;    // FastRun updates display every N events
      int  opt_updatefreq_express; // RunExpress updates display every N events
      unsigned opt_extrastack_kb;  // per-module extra stack for activity() modules
      bool opt_expressmode_autoupdate;// update inspectors at every display refresh in EXPRESS mode or not
      opp_string opt_image_path;   // directory of module icon files
      opp_string opt_plugin_path;  // path for loading Tcl and binary plugins

      // state variables
      bool animating;              // while execution, do message animation or not
      bool hasmessagewindow;       // whether the message window is currently open

   protected:
      Tcl_Interp *interp;          // Tcl interpreter
      opp_string windowtitleprefix;// contains "procId=.." when using parsim

      opp_string tkenv_dir;        // directory of Tkenv's *.tcl files

      bool isconfigrun;            // true after newRun(), and false after newConfig()
      eState simstate;             // state of the simulation run
      int runmode;                 // the current mode the simulation is executing under
      simtime_t rununtil_time;     // time limit in current "Run Until" execution, or zero
      long rununtil_event;         // event number in current "Run Until" execution, or zero
      cModule *rununtil_module;    // stop before and after events in this module; ignored with EXPRESS mode

      bool stopsimulation_flag;    // flag to signal that simulation must be stopped (STOP button pressed in the UI)

      typedef std::list<TInspector*> TInspectorList;
      TInspectorList inspectors;   // list of inspector objects

   public:
      Tkenv();
      virtual ~Tkenv();

      virtual void objectDeleted(cObject *object); // notify environment
      virtual void simulationEvent(cMessage *msg);
      virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate);
      virtual void messageScheduled(cMessage *msg);
      virtual void messageCancelled(cMessage *msg);
      virtual void beginSend(cMessage *msg);
      virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay);
      virtual void messageSendHop(cMessage *msg, cGate *srcGate);
      virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay);
      virtual void endSend(cMessage *msg);
      virtual void messageDeleted(cMessage *msg);
      virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *method);
      virtual void componentMethodEnd();
      virtual void moduleCreated(cModule *newmodule);
      virtual void moduleDeleted(cModule *module);
      virtual void moduleReparented(cModule *module, cModule *oldparent);
      virtual void connectionCreated(cGate *srcgate);
      virtual void connectionRemoved(cGate *srcgate);
      virtual void displayStringChanged(cComponent *component);

      virtual bool isGUI() {return true;}
      virtual void bubble(cComponent *component, const char *text);

      virtual void putsmsg(const char *s);
      virtual void sputn(const char *s, int n);
      virtual cEnvir& flush();
      virtual std::string gets(const char *promptstr, const char *defaultreply);
      virtual bool askyesno(const char *question);

      virtual bool idle();

      // with Tkenv, activity() modules need extra stack
      virtual unsigned extraStackForEnvir();

  protected:
      // redefined virtual functions from EnvirBase
      virtual void setup();
      virtual int run();
      virtual void shutdown();
      virtual void printUISpecificHelp();

      virtual void readOptions();
      virtual void readPerRunOptions();

  public:
      // New functions:
      void newNetwork(const char *networkname);
      void newRun(const char *configname, int runnumber);
      void createSnapshot(const char *label);

      void rebuildSim();
      void doOneStep();
      void runSimulation(int mode, simtime_t until_time=0, long until_event=0, cModule *until_module=NULL);
      void setSimulationRunMode(int runmode);
      void setSimulationRunUntil(simtime_t until_time, long until_event);
      void setSimulationRunUntilModule(cModule *until_module);
      bool doRunSimulation();
      bool doRunSimulationExpress();

      void startAll();
      void finishSimulation(); // wrapper around simulation.callFinish() and simulation.endRun()

      void loadNedFile(const char *fname, const char *expectedPackage=NULL, bool isXML=false);

      void updateInspectors();
      TInspector *inspect(cObject *obj, int type, const char *geometry, void *dat);
      TInspector *findInspector(cObject *obj, int type);
      void deleteInspector(TInspector *insp);

      int getSimulationState() {return simstate;}
      void setStopSimulationFlag() {stopsimulation_flag = true;}
      bool getStopSimulationFlag() {return stopsimulation_flag;}
      Tcl_Interp *getInterp() {return interp;}

      void updateGraphicalInspectorsBeforeAnimation();

      void channelDisplayStringChanged(cChannel *channel);
      void moduleDisplayStringChanged(cModule *module);

      // small functions:
      void updateNetworkRunDisplay();
      void updateSimtimeDisplay();
      void updateNextModuleDisplay();
      void clearNextModuleDisplay();
      void updatePerformanceDisplay(Speedometer& speedometer);
      void clearPerformanceDisplay();

      void confirm(const char *msg); // messagebox with OK button

      void printEventBanner(cSimpleModule *mod);
      void animateSend(cMessage *msg, cGate *fromgate, cGate *togate);
      void animateSendDirect(cMessage *msg, cModule *frommodule, cGate *togate);
      void animateDelivery(cMessage *msg);
      void animateDeliveryDirect(cMessage *msg);
      void performAnimations();

      void findDirectPath(cModule *frommodule, cModule *tomodule, PathVec& pathvec);

      const char *getIniFileName()       {return config()->getFileName();}
      const char *getOutVectorFileName() {return outvectormgr->fileName();}
      const char *getOutScalarFileName() {return outscalarmgr->fileName();}
      const char *getSnapshotFileName()  {return snapshotmgr->fileName();}
      const char *getWindowTitlePrefix() {return windowtitleprefix.c_str();}

};


/**
 * Utility function
 */
inline Tkenv *getTkenv()
{
    return (Tkenv *)(&ev);
}

NAMESPACE_END

#endif
