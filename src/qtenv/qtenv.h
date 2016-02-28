//==========================================================================
//  QTENV.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_QTENV_H
#define __OMNETPP_QTENV_QTENV_H

#include <vector>
#include <list>
#include "envir/envirbase.h"
#include "envir/speedometer.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cmodule.h"
#include "qtenvdefs.h"
#include "logbuffer.h"
#include "componenthistory.h"
#include "imagecache.h"
#include "inspector.h"
#include "animator.h"
#include <QSettings>
#include <QSet>
#include <QString>

#include <QDebug>

class QWidget;

namespace omnetpp {
namespace qtenv {

class MainWindow;
class InspectorFactory;
class Inspector;
class GenericObjectInspector;
class LogInspector;
class ModuleInspector;
class TimeLineInspector;
class ObjectTreeInspector;

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

struct QtenvOptions : public omnetpp::envir::EnvirOptions
{
    QtenvOptions();
    size_t extraStack;        // per-module extra stack for activity() modules
    std::string defaultConfig; // automatically set up this config at startup
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
 * A Qt-based user interface.
 */
class QTENV_API Qtenv : public QObject, public omnetpp::envir::EnvirBase
{
   Q_OBJECT
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
         cModule *from; // nullptr if descent
         cModule *to;   // nullptr if ascent
         sPathEntry(cModule *f, cModule *t) {from=f; to=t;}
      };
      typedef std::vector<sPathEntry> PathVec;

   public:
      QtenvOptions *&opt;          // alias to EnvirBase::opt

      // state variables
      bool animating;              // while execution, do message animation or not
      ImageCache icons;

   protected:
      QApplication *app = nullptr;
      MainWindow *mainWindow = nullptr;
      std::string windowTitlePrefix;// contains "procId=.." when using parsim

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

      bool stopSimulationFlag;    // indicates that the simulation should be stopped (STOP button pressed in the UI)
      timeval idleLastUICheck;     // gettimeofday() time when idle() last run the Tk "update" command

      typedef std::list<Inspector*> InspectorList;
      InspectorList inspectors;    // list of inspector objects

      LogBuffer logBuffer;         // text window contents
      ComponentHistory componentHistory; // id-to-fullpath mapping for deleted modules  FIXME TODO clear this between runs!!!

      GenericObjectInspector *mainInspector;
      ModuleInspector *mainNetworkView;
      LogInspector *mainLogView;
      TimeLineInspector *mainTimeLine;
      ObjectTreeInspector *mainObjectTree;

      Animator *animator;

      typedef std::map<std::string,std::string> StringMap;
      StringMap answers;           // key: <ModuleType>:<paramName>, value: <interactively-given-paramvalue>

      typedef omnetpp::common::MatchExpression MatchExpression;
      typedef std::vector<MatchExpression*> MatchExpressions;
      std::string silentEventFilterLines; // match expressions, separated by newlines (\n)
      MatchExpressions silentEventFilters; // silent events: objects to hide from animation and the timeline

      QFont boldFont;
      QFont timelineFont;
      QFont canvasFont;
      QFont logFont;

      struct DefaultFonts
      {
          QFont boldFont;
          QFont timelineFont;
          QFont canvasFont;
          QFont logFont;
      } defaultFonts;

      // these only exist while doRun() runs
      QSettings *globalPrefs = nullptr;
      QSettings *localPrefs = nullptr;

      QSet<QString> localPrefKeys;

      void storeOptsInPrefs();
      void restoreOptsFromPrefs();

   public:
      Qtenv();
      virtual ~Qtenv();

      virtual void objectDeleted(cObject *object) override; // notify environment
      virtual void componentInitBegin(cComponent *component, int stage) override;
      virtual void simulationEvent(cEvent *event) override;
      virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate) override;
      virtual void messageScheduled(cMessage *msg) override;
      virtual void messageCancelled(cMessage *msg) override;
      virtual void beginSend(cMessage *msg) override;
      virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay) override;
      virtual void messageSendHop(cMessage *msg, cGate *srcGate) override;
      virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay) override;
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
      virtual void putsmsg(const char *s) override;
      virtual std::string gets(const char *promt, const char *defaultReply) override;
      virtual bool askyesno(const char *question) override;

      virtual bool idle() override;

      // with Qtenv, activity() modules need extra stack
      virtual unsigned getExtraStackForEnvir() const override;

      MainWindow *getMainWindow() { return mainWindow; }
      Animator *getAnimator() { return animator; }
      GenericObjectInspector *getMainObjectInspector() { return mainInspector; }
      ModuleInspector *getMainModuleInspector() { return mainNetworkView; }
      LogInspector *getMainLogInspector() { return mainLogView; }
      TimeLineInspector *getMainTimeLineInspector() { return mainTimeLine; }
      // currently the rop-right one, ignoring the decoration, use accordingly
      QPoint getDefaultStopDialogCorner(const QPoint &offset = QPoint(-20, 80));

      void setPref(const QString &key, const QVariant &value);
      QVariant getPref(const QString &key, const QVariant &defaultValue = QVariant());
  signals:
      void fontChanged();

  signals:
      void animationSpeedChanged(float newSpeed);

  public slots:
      // on single click, only shows the object in the GenericObjectInspector
      void onSelectionChanged(cObject *object);
      // XXX could have a better name...
      void onObjectDoubleClicked(cObject *object);
      // for context menu
      void inspect();
      void runUntilModule();
      void runUntilMessage();
      void excludeMessage();
      void utilitiesSubMenu();

      // will get the cComponent pointer and LogLevel in an
      // ActionDataPair from the data of the sender QAction
      void setComponentLogLevel();
      void setComponentLogLevel(cComponent *component, LogLevel level, bool save = false);

  protected:
      // redefined virtual functions from EnvirBase
      virtual void doRun() override;
      virtual void printUISpecificHelp() override;

      virtual EnvirOptions *createOptions() override {return new QtenvOptions();}
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
      void setSimulationRunMode(int runMode);
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
      Inspector *inspect(cObject *obj, int type=INSP_DEFAULT, bool ignoreEmbedded=false);
      Inspector *addEmbeddedInspector(InspectorFactory *factory, QWidget *parent);
      Inspector *findFirstInspector(cObject *obj, int type, bool ignoreEmbedded=false);
      Inspector *findInspector(const char *tkWidget) {return nullptr;}  //TODO obsolete, remove! find inspector by Tk widget name
      void deleteInspector(Inspector *insp);
      const std::list<Inspector*>& getInspectors() {return inspectors;}

      int getSimulationState() {return simulationState;}
      void setStopSimulationFlag() {stopSimulationFlag = true;}
      bool getStopSimulationFlag() {return stopSimulationFlag;}
      Speedometer& getSpeedometer() {return speedometer;}
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

      void printEventBanner(cEvent *event);
      void setAnimationSpeed(float speed);
      void animateSend(cMessage *msg, cGate *fromgate, cGate *togate);
      void animateSendDirect(cMessage *msg, cModule *frommodule, cGate *togate);
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

      QFont getBoldFont() {return boldFont;}
      QFont getTimelineFont() {return timelineFont;}
      QFont getCanvasFont() {return canvasFont;}
      QFont getLogFont() {return logFont;}

      QFont getDefaultBoldFont() {return defaultFonts.boldFont;}
      QFont getDefaultTimelineFont() {return defaultFonts.timelineFont;}
      QFont getDefaultCanvasFont() {return defaultFonts.canvasFont;}
      QFont getDefaultLogFont() {return defaultFonts.logFont;}

      void setBoldFont(QFont &font) {boldFont = font;}
      void setTimelineFont(QFont &font) {timelineFont = font;}
      void setCanvasFont(QFont &font) {canvasFont = font;}
      void setLogFont(QFont &font) {logFont = font;}

      void updateQtFonts();

      void initFonts();
      void saveFonts();
      QFont getFirstAvailableFontFamily(std::initializer_list<QString> preferenceList, int pointSize, QFont defaultValue = QString());

      void runSimulationLocal(int runMode, cObject *object = nullptr, Inspector *insp = nullptr);
};


/**
 * Utility function
 */
inline Qtenv *getQtenv()
{
    return (Qtenv *)cSimulation::getActiveEnvir();
}

} // namespace qtenv
} // namespace omnetpp

#endif
