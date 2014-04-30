//==========================================================================
//  TKENV.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

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
#include "logbuffer.h"
#include "envirbase.h"
#include "cchannel.h"
#include "cmodule.h"
#include "speedometer.h"
#include "componenthistory.h"

NAMESPACE_BEGIN

class Inspector;
class GenericObjectInspector;
class LogInspector;
class ModuleInspector;

#define MAX_CLASSNAME  100

enum LayouterChoice
{
    LAYOUTER_FAST,
    LAYOUTER_ADVANCED,
    LAYOUTER_AUTO
};

struct TkenvOptions : public EnvirOptions
{
    TkenvOptions();
    size_t extraStack;        // per-module extra stack for activity() modules
    opp_string imagePath;     // directory of module icon files
    opp_string pluginPath;    // path for loading Tcl and binary plugins
    opp_string defaultConfig; // automatically set up this config at startup
    int  defaultRun;          // automatically set up this run (of the default config) at startup
    bool printInitBanners;    // print "initializing..." banners ----------- FIXME DOES NOT WORK PROPERLY!!!
    bool printEventBanners;   // print event banners ----------- FIXME DOES NOT WORK PROPERLY!!!
    bool shortBanners;        // controls detail of event banners ----------- FIXME DOES NOT WORK PROPERLY!!!
    bool animationEnabled;    // msg animation
    bool showNextEventMarkers;// display next event marker (red frame around modules)
    bool showSendDirectArrows;// flash arrows when doing sendDirect() animation
    bool animateMethodCalls;  // animate method calls
    int  methodCallAnimDelay; // hold animation of method calls (millisec)
    bool animationMsgNames;   // msg animation: display message name or not
    bool animationMsgClassNames; // msg animation: display message class name or not
    bool animationMsgColors;  // msg animation: display msg kind as color code or not
    bool penguinMode;         // msg animation: message appearance
    bool showLayouting;       // show layouting process in graphical module inspectors
    LayouterChoice layouterChoice; // which new layouting algorithm to use
    bool arrangeVectorConnections; // arrange connections on vector gates parallel to each other
    int iconMinimumSize;      // minimum size of icons when zooming out
    bool showBubbles;         // show result of bubble() calls
    double animationSpeed;    // msg animation speed: 0=slow 1=norm 2=fast
    long updateFreqFast;      // Fast Run updates display every N milliseconds
    long updateFreqExpress;   // Express Run updates display every N milliseconds
    bool autoupdateInExpress; // update inspectors at every display refresh in EXPRESS mode or not
    bool stopOnMsgCancel;     // with rununtil_msg: whether to stop when the message gets cancelled
    int scrollbackLimit;      // global setting for all LogInspectors
};

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
      TkenvOptions *&opt;          // alias to EnvirBase::opt

      // state variables
      bool animating;              // while execution, do message animation or not

   protected:
      Tcl_Interp *interp;          // Tcl interpreter
      opp_string windowtitleprefix;// contains "procId=.." when using parsim

      opp_string tkenv_dir;        // directory of Tkenv's *.tcl files

      bool isconfigrun;            // true after newRun(), and false after newConfig()
      eState simstate;             // state of the simulation run
      int runmode;                 // the current mode the simulation is executing under
      simtime_t rununtil_time;     // time limit in current "Run Until" execution, or zero
      eventnumber_t rununtil_eventnum;// event number in current "Run Until" execution, or zero
      cMessage *rununtil_msg;      // stop before this event; also when this message gets cancelled
      cModule *rununtil_module;    // stop before and after events in this module; ignored with EXPRESS mode
      Speedometer speedometer;

      bool stopsimulation_flag;    // indicates that the simulation should be stopped (STOP button pressed in the UI)
      timeval idleLastUICheck;     // gettimeofday() time when idle() last run the Tk "update" command

      typedef std::list<Inspector*> InspectorList;
      InspectorList inspectors;    // list of inspector objects

      LogBuffer logBuffer;         // text window contents
      ComponentHistory componentHistory; // id-to-fullpath mapping for deleted modules  FIXME TODO clear this between runs!!!

      GenericObjectInspector *mainInspector;
      ModuleInspector *mainNetworkView;
      LogInspector *mainLogView;

      typedef std::map<std::string,std::string> StringMap;
      StringMap answers;           // key: <ModuleType>:<paramName>, value: <interactively-given-paramvalue>

      typedef std::vector<MatchExpression*> MatchExpressions;
      std::string silentEventFilterLines; // match expressions, separated by newlines (\n)
      MatchExpressions silentEventFilters; // silent events: objects to hide from animation and the timeline

      FILE *ferrorlog;             // .tkenvlog file; NULL if not yet open

   public:
      Tkenv();
      virtual ~Tkenv();

      virtual void objectDeleted(cObject *object); // notify environment
      virtual void componentInitBegin(cComponent *component, int stage);
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
      virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent);
      virtual void componentMethodEnd();
      virtual void moduleCreated(cModule *newmodule);
      virtual void moduleDeleted(cModule *module);
      virtual void moduleReparented(cModule *module, cModule *oldparent);
      virtual void connectionCreated(cGate *srcgate);
      virtual void connectionDeleted(cGate *srcgate);
      virtual void displayStringChanged(cComponent *component);

      virtual bool isGUI() const {return true;}
      virtual void bubble(cComponent *component, const char *text);

      virtual void putsmsg(const char *s);
      virtual void sputn(const char *s, int n);
      virtual cEnvir& flush();
      virtual std::string gets(const char *promt, const char *defaultReply);
      virtual bool askyesno(const char *question);

      virtual bool idle();

      // with Tkenv, activity() modules need extra stack
      virtual unsigned getExtraStackForEnvir() const;

      virtual void logTclError(const char *file, int line, Tcl_Interp *interp);
      virtual void logTclError(const char *file, int line, const char *text);
      virtual void openTkenvlogIfNeeded();

  protected:
      // redefined virtual functions from EnvirBase
      virtual void run();
      virtual void printUISpecificHelp();

      virtual EnvirOptions *createOptions() {return new TkenvOptions();}
      virtual void readOptions();
      virtual void readPerRunOptions();
      virtual void setupNetwork(cModuleType *network);
      virtual void askParameter(cPar *par, bool unassigned);
      virtual void displayException(std::exception& e);
      virtual std::string getWindowTitle();

  public:
      // New functions:
      void newNetwork(const char *networkname);
      void newRun(const char *configname, int runnumber);
      void createSnapshot(const char *label);

      void rebuildSim();
      void doOneStep();
      void runSimulation(int mode, simtime_t until_time=0, eventnumber_t until_eventnum=0, cMessage *until_msg=NULL, cModule *until_module=NULL);
      void setSimulationRunMode(int runmode);
      int getSimulationRunMode() const {return runmode;}
      void setSimulationRunUntil(simtime_t until_time, eventnumber_t until_eventnum, cMessage *until_msg);
      void setSimulationRunUntilModule(cModule *until_module);
      bool doRunSimulation();
      bool doRunSimulationExpress();

      void startAll();
      void finishSimulation(); // wrapper around simulation.callFinish() and simulation.endRun()

      void loadNedFile(const char *fname, const char *expectedPackage=NULL, bool isXML=false);

      void updateInspectors();
      void redrawInspectors();
      Inspector *inspect(cObject *obj, int type, bool ignoreEmbedded, const char *geometry);
      void addEmbeddedInspector(const char *widget, Inspector *insp);
      Inspector *findFirstInspector(cObject *obj, int type, bool ignoreEmbedded=false);
      Inspector *findInspector(const char *widget);
      void deleteInspector(Inspector *insp);
      const std::list<Inspector*>& getInspectors() {return inspectors;}

      int getSimulationState() {return simstate;}
      void setStopSimulationFlag() {stopsimulation_flag = true;}
      bool getStopSimulationFlag() {return stopsimulation_flag;}
      Speedometer& getSpeedometer() {return speedometer;}
      Tcl_Interp *getInterp() {return interp;}
      LogBuffer *getLogBuffer() {return &logBuffer;}
      ComponentHistory *getComponentHistory() {return &componentHistory;}
      const char *getSilentEventFilters() const {return silentEventFilterLines.c_str();}
      void setSilentEventFilters(const char *filterLines);
      bool isSilentEvent(cMessage *msg);

      void updateGraphicalInspectorsBeforeAnimation();

      void channelDisplayStringChanged(cChannel *channel);
      void moduleDisplayStringChanged(cModule *module);

      // small functions:
      void updateNetworkRunDisplay();
      void updateStatusDisplay();

      void confirm(const char *msg); // messagebox with OK button
      bool inputDialog(const char *title, const char *prompt,
                       const char *checkboxLabel, const char *defaultValue,
                       std::string& outResult, bool& inoutCheckState);

      void printEventBanner(cMessage *msg, cSimpleModule *mod);
      void animateSend(cMessage *msg, cGate *fromgate, cGate *togate);
      void animateSendDirect(cMessage *msg, cModule *frommodule, cGate *togate);
      void animateDelivery(cMessage *msg);
      void animateDeliveryDirect(cMessage *msg);
      void performAnimations();

      void findDirectPath(cModule *frommodule, cModule *tomodule, PathVec& pathvec);

      std::string getLocalPackage()      {return simulation.getNedPackageForFolder(opt->inifileNetworkDir.c_str());}
      const char *getIniFileName()       {return getConfigEx()->getFileName();}
      const char *getOutVectorFileName() {return outvectormgr->getFileName();}
      const char *getOutScalarFileName() {return outscalarmgr->getFileName();}
      const char *getSnapshotFileName()  {return snapshotmgr->getFileName();}
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
