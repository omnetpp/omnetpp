//==========================================================================
//  TKENV.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_TKENV_TKENV_H
#define __OMNETPP_TKENV_TKENV_H

#include <vector>
#include <list>
#include "envir/envirbase.h"
#include "envir/speedometer.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cmodule.h"
#include "tkdefs.h"
#include "logbuffer.h"
#include "componenthistory.h"

namespace omnetpp {
namespace tkenv {

class Inspector;
class GenericObjectInspector;
class LogInspector;
class ModuleInspector;

using namespace envir;

namespace common { class MatchExpression; };

#define MAX_CLASSNAME  100

enum LayouterChoice
{
    LAYOUTER_FAST,
    LAYOUTER_ADVANCED,
    LAYOUTER_AUTO
};

enum StripNamespace
{
    STRIPNAMESPACE_NONE,
    STRIPNAMESPACE_OMNETPP,
    STRIPNAMESPACE_ALL
};

struct TkenvOptions : public omnetpp::envir::EnvirOptions
{
    TkenvOptions();
    size_t extraStack;        // per-module extra stack for activity() modules
    std::string pluginPath;   // path for loading Tcl and binary plugins
    std::string defaultConfig;// automatically set up this config at startup
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
    StripNamespace stripNamespace; // whether to display type names with full C++ namespace prefix or not
    std::string logFormat;    // format of the log prefix, see the LogFormatter class
    LogLevel logLevel;        // global log level
    int scrollbackLimit;      // global setting for all LogInspectors
};

/**
 * A Tcl/Tk-based user interface.
 */
class TKENV_API Tkenv : public omnetpp::envir::EnvirBase
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

      enum DialogKind {
          INFO,
          WARNING,
          ERROR
      };

      struct sPathEntry {
         cModule *from; // nullptr if descent
         cModule *to;   // nullptr if ascent
         sPathEntry(cModule *f, cModule *t) {from=f; to=t;}
      };
      typedef std::vector<sPathEntry> PathVec;

   public:
      TkenvOptions *&opt;          // alias to EnvirBase::opt

      // state variables
      bool animating;              // while execution, do message animation or not

   protected:
      Tcl_Interp *interp;          // Tcl interpreter
      std::string windowTitlePrefix;// contains "procId=.." when using parsim

      std::string tkenvDir;        // directory of Tkenv's *.tcl files

      bool isConfigRun;            // true after newRun(), and false after newConfig()
      eState simulationState;      // state of the simulation run
      int runMode;                 // the current mode the simulation is executing under
      struct RunUntil {
          simtime_t time;          // time limit in current "Run Until" execution, or zero
          eventnumber_t eventNumber;  // event number in current "Run Until" execution, or zero
          cMessage *msg;           // stop before this event; also when this message is cancelled
          cModule *module;         // stop before and after events in this module; ignored with EXPRESS mode
      } runUntil;
      Speedometer speedometer;

      bool stopSimulationFlag;     // indicates that the simulation should be stopped (STOP button pressed in the UI)
      int64_t idleLastUICheck;     // opp_get_monotonic_clock_usecs() time when idle() last ran the Tk "update" command

      typedef std::list<Inspector*> InspectorList;
      InspectorList inspectors;    // list of inspector objects

      LogBuffer logBuffer;         // text window contents
      ComponentHistory componentHistory; // id-to-fullpath mapping for deleted modules

      GenericObjectInspector *mainInspector;
      ModuleInspector *mainNetworkView;
      LogInspector *mainLogView;

      typedef std::map<std::string,std::string> StringMap;
      StringMap answers;           // key: <ModuleType>:<paramName>, value: <interactively-given-paramvalue>

      typedef omnetpp::common::MatchExpression MatchExpression;
      typedef std::vector<MatchExpression*> MatchExpressions;
      std::string silentEventFilterLines; // match expressions, separated by newlines (\n)
      MatchExpressions silentEventFilters; // silent events: objects to hide from animation and the timeline

      FILE *ferrorlog;             // .tkenvlog file; nullptr if not yet open

   public:
      Tkenv();
      virtual ~Tkenv();

      virtual void objectDeleted(cObject *object) override; // notify environment
      virtual void componentInitBegin(cComponent *component, int stage) override;
      virtual void simulationEvent(cEvent *event) override;
      virtual void messageScheduled(cMessage *msg) override;
      virtual void messageCancelled(cMessage *msg) override;
      virtual void beginSend(cMessage *msg) override;
      virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay) override;
      virtual void messageSendHop(cMessage *msg, cGate *srcGate) override;
      virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay, bool discard) override;
      virtual void endSend(cMessage *msg) override;
      virtual void messageDeleted(cMessage *msg) override;
      virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent) override;
      virtual void componentMethodEnd() override;
      virtual void moduleCreated(cModule *newmodule) override;
      virtual void moduleDeleted(cModule *module) override;
      virtual void moduleReparented(cModule *module, cModule *oldParent, int oldId) override;
      virtual void connectionCreated(cGate *srcgate) override;
      virtual void connectionDeleted(cGate *srcgate) override;
      virtual void displayStringChanged(cComponent *component) override;

      virtual bool isGUI() const override {return true;}
      virtual bool isExpressMode() const override {return runMode == RUNMODE_EXPRESS;}
      virtual void bubble(cComponent *component, const char *text) override;

      virtual void log(cLogEntry *entry) override;
      virtual void alert(const char *msg) override;
      virtual std::string gets(const char *prompt, const char *defaultReply) override;
      virtual bool askYesNo(const char *question) override;

      virtual bool idle() override;

      virtual void getImageSize(const char *imageName, double& outWidth, double& outHeight) override;
      virtual void getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent) override;
      virtual void appendToImagePath(const char *directory) override;
      virtual void loadImage(const char *fileName, const char *imageName=nullptr) override;
      virtual cFigure::Rectangle getSubmoduleBounds(const cModule *submodule) override;
      virtual double getZoomLevel(const cModule *module) override;
      virtual double getAnimationTime() const override {return 0;}  // custom animations not supported
      virtual double getAnimationSpeed() const override {return 0;}
      virtual double getRemainingAnimationHoldTime() const override {return 0;}

      // with Tkenv, activity() modules need extra stack
      virtual unsigned getExtraStackForEnvir() const override;

      virtual void logTclError(const char *file, int line, Tcl_Interp *interp);
      virtual void logTclError(const char *file, int line, const char *text);
      virtual void openTkenvlogIfNeeded();

  protected:
      // redefined virtual functions from EnvirBase
      virtual void doRun() override;
      virtual void printUISpecificHelp() override;

      virtual EnvirOptions *createOptions() override {return new TkenvOptions();}
      virtual void readOptions() override;
      virtual void readPerRunOptions() override;
      virtual void setupNetwork(cModuleType *network) override;
      virtual void askParameter(cPar *par, bool unassigned) override;
      virtual void displayException(std::exception& e) override;
      virtual std::string getWindowTitle();

  public:
      // New functions:
      void newNetwork(const char *networkname);
      void newRun(const char *configname, int runnumber);
      void createSnapshot(const char *label);

      void rebuildSim();
      void doOneStep();
      void runSimulation(int mode, simtime_t until_time=0, eventnumber_t until_eventnum=0, cMessage *until_msg=nullptr, cModule *until_module=nullptr);
      void setSimulationRunMode(int runmode);
      int getSimulationRunMode() const {return runMode;}
      void setSimulationRunUntil(simtime_t until_time, eventnumber_t until_eventnum, cMessage *until_msg);
      void setSimulationRunUntilModule(cModule *until_module);
      bool doRunSimulation();
      bool doRunSimulationExpress();

      void startAll();
      void finishSimulation(); // wrapper around simulation.callFinish() and simulation.endRun()

      void loadNedFile(const char *fname, const char *expectedPackage=nullptr, bool isXML=false);

      void callRefreshDisplay();
      void refreshInspectors();
      void redrawInspectors();
      Inspector *inspect(cObject *obj, int type, bool ignoreEmbedded, const char *geometry);
      void addEmbeddedInspector(const char *widget, Inspector *insp);
      Inspector *findFirstInspector(const cObject *obj, int type, bool ignoreEmbedded=false);
      Inspector *findInspector(const char *widget);
      void deleteInspector(Inspector *insp);
      const std::list<Inspector*>& getInspectors() {return inspectors;}

      int getSimulationState() {return simulationState;}
      void setStopSimulationFlag() {stopSimulationFlag = true;}
      bool getStopSimulationFlag() {return stopSimulationFlag;}
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
      void updateSimtimeDisplay();
      void updateStatusDisplay();

      void confirm(DialogKind kind, const char *msg);
      bool inputDialog(const char *title, const char *prompt,
                       const char *checkboxLabel, const char *defaultValue,
                       std::string& outResult, bool& inoutCheckState);

      void printEventBanner(cEvent *event);
      void animateSendHop(cMessage *msg, cGate *srcGate, bool isLastHop);
      void animateSendDirect(cMessage *msg, cModule *srcModule, cGate *destGate);
      void animateDelivery(cMessage *msg);
      void animateDeliveryDirect(cMessage *msg);
      void performAnimations();

      void findDirectPath(cModule *frommodule, cModule *tomodule, PathVec& pathvec);

      std::string getLocalPackage()      {return getSimulation()->getNedPackageForFolder(opt->inifileNetworkDir.c_str());}
      const char *getIniFileName()       {return getConfigEx()->getFileName();}
      const char *getOutVectorFileName() {return outvectorManager->getFileName();}
      const char *getOutScalarFileName() {return outScalarManager->getFileName();}
      const char *getSnapshotFileName()  {return snapshotManager->getFileName();}
      const char *getWindowTitlePrefix() {return windowTitlePrefix.c_str();}
};


/**
 * Utility function
 */
inline Tkenv *getTkenv()
{
    return (Tkenv *)cSimulation::getActiveEnvir();
}

} // namespace tkenv
}  // namespace omnetpp

#endif
