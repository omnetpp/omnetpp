//==========================================================================
//  QTENV.H - part of
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

#ifndef __OMNETPP_QTENV_QTENV_H
#define __OMNETPP_QTENV_QTENV_H

#include <vector>
#include <list>
#include "envir/appbase.h"
#include "envir/speedometer.h"
#include "envir/inifilecontents.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/ccanvas.h"
#include "common/matchexpression.h"
#include "qtenvdefs.h"
#include "logbuffer.h"
#include "componenthistory.h"
#include "imagecache.h"
#include "modulelayouter.h"
#include "inspector.h"
#include "charttickdecimal.h"
#include <QtCore/QSettings>
#include <QtCore/QSet>
#include <QtCore/QString>

#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <QtCore/QEventLoop>

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
class DisplayUpdateController;
class MessageAnimator;

using common::MatchExpression;

using namespace envir;

#define MAX_CLASSNAME  100

enum LayouterChoice
{
    LAYOUTER_FAST,
    LAYOUTER_ADVANCED,
    LAYOUTER_AUTO
};

struct QtenvOptions : public AppBaseOptions
{
    // note: these values will be overwritten in setup()/readOptions() before taking effect
    size_t extraStack;                     // per-module extra stack for activity() modules
    std::string defaultConfig;             // automatically set up this config at startup
    std::string runFilter;                 // groups the matching runs to the beginning of the list, or if only one matches, will set up that one automatically
    bool printInitBanners = true;          // print "initializing..." banners
    bool printEventBanners = true;         // print event banners
    bool shortBanners = false;             // controls detail of event banners
    bool allowBackwardArrowsForHops = false; // whether to allow right-to-left printing (with "<--" arrows) of relevant hops
    bool messageLogDigitGrouping = true;   // whether to add digit separators (apostrophes) to the sending times printed in the message log
    bool noLoggingRefreshDisplay = false;  // suppresses logging in refreshDisplay() calls
    bool animationEnabled = true;          // msg animation
    bool showNextEventMarkers = true;      // display next event marker (red frame around modules)
    bool showSendDirectArrows = true;      // flash arrows when doing sendDirect() animation
    bool animateMethodCalls = false;       // animate method calls
    int  methodCallAnimDuration = 1500;    // hold animation of method calls (millisec)
    bool animationMsgNames = true;         // msg animation: display message name or not
    bool animationMsgClassNames = true;    // msg animation: display message class name or not
    bool animationMsgColors = true;        // msg animation: display msg kind as color code or not
    bool penguinMode = false;              // msg animation: message appearance
    bool showLayouting = false;            // show layouting process in graphical module inspectors
    LayouterChoice layouterChoice = LAYOUTER_AUTO; // which new layouting algorithm to use
    bool arrangeVectorConnections = false; // arrange connections on vector gates parallel to each other
    bool showBubbles = true;               // show result of bubble() calls
    long updateFreqExpress = 1000;         // Express Run updates display every N milliseconds
    bool autoupdateInExpress = true;       // update inspectors at every display refresh in EXPRESS mode or not
    StripNamespace stripNamespace = STRIPNAMESPACE_ALL; // whether to display type names with full C++ namespace prefix or not
    std::string logFormat = "%l %K%<: ";     // format of the log prefix, see the LogFormatter class
    LogLevel logLevel = LOGLEVEL_TRACE;    // global log level
};

/**
 * A Qt-based user interface.
 */
class QTENV_API Qtenv : public QObject, public AppBase
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

      enum DialogKind
      {
          INFO,
          WARNING,
          ERROR
      };

      QtenvOptions *&opt;          // alias to EnvirBase::opt

      ImageCache icons;

   protected:
      QApplication *app = nullptr;
      MainWindow *mainWindow = nullptr;
      std::string windowTitlePrefix;// contains "procId=.." when using parsim

      QEventLoop *pauseEventLoop = nullptr; // has to be a pointer so it's not constructed unnecessarily. always exists, only started/stopped as needed
      int pausePointNumber = 0;    // which "stop" are we within an event, incremented in pause(), reset after an event is done

      cConfiguration *activeCfg = nullptr;

      bool isConfigRun = false;              // true after newRun(), and false after newConfig()
      eState simulationState = SIM_NONET;    // state of the simulation run
      RunMode runMode = RUNMODE_NOT_RUNNING; // the current mode the simulation is executing under
      struct RunUntil {
          simtime_t time;                // time limit in current "Run Until" execution, or zero
          eventnumber_t eventNumber = 0; // event number in current "Run Until" execution, or zero
          cMessage *msg = nullptr;       // stop before this event; also when this message is cancelled
          cModule *module = nullptr;     // stop before and after events in this module; ignored with EXPRESS mode
          bool stopOnMsgCancel = true;   // with rununtil_msg: whether to stop when the message gets cancelled
      } runUntil;

      bool doNextEventInStep = false;// true if the next event should be executed in STEP mode, or we should stop before
      Speedometer speedometer;

      Stopwatch stopwatch;      // CPU and real time limit checking
      simtime_t simulatedTime;  // sim. time after finishing simulation

      bool stopSimulationFlag;      // indicates that the simulation should be stopped (STOP button pressed in the UI)
      bool callFinishOnExitFlag = false;

      typedef std::list<Inspector*> InspectorList;
      InspectorList inspectors;     // list of inspector objects

      // The Inspectors should only access the simulation outside of refresh() if this is true.
      // (They can access it freely inside refresh(), but then they should not rely on any
      // previously saved/cached pointers/references/state, as they should be considered stale.)
      // Set to true after the inspectors are refreshed, and set to false
      // after any model code is executed (executeEvent or refreshDisplay).
      bool inspectorsFresh = false;

      LogBuffer logBuffer;          // text window contents
      ComponentHistory componentHistory; // id-to-fullpath mapping for deleted modules
      ModuleLayouter moduleLayouter;

      GenericObjectInspector *mainInspector;
      ModuleInspector *mainNetworkView;
      LogInspector *mainLogView;
      TimeLineInspector *mainTimeLine;
      ObjectTreeInspector *mainObjectTree;

      MessageAnimator *messageAnimator = nullptr;
      DisplayUpdateController *displayUpdateController = nullptr;

      int refreshDisplayCount = 0;

      typedef std::map<std::string,std::string> StringMap;
      StringMap answers;           // key: <ModuleType>:<paramName>, value: <interactively-given-paramvalue>

      typedef std::vector<MatchExpression*> MatchExpressions;
      std::string silentEventFilterLines; // match expressions, separated by newlines (\n)
      MatchExpressions silentEventFilters; // silent events: objects to hide from animation and the timeline

      QFont boldFont;
      QFont timelineFont;
      QFont canvasFont;
      QFont logFont;
      QFont timeFont;

      struct DefaultFonts
      {
          QFont boldFont;
          QFont timelineFont;
          QFont canvasFont;
          QFont logFont;
          QFont timeFont;
      } defaultFonts;

      // these only exist while doRun() runs
      QSettings *globalPrefs = nullptr;
      QSettings *localPrefs = nullptr;

      // These will be saved into the .qtenvrc in the working directory, all others into the one in the home of the user.
      bool isLocalPrefKey(const QString& key);

   public:
      Qtenv();
      virtual ~Qtenv();

      virtual void objectDeleted(cObject *object); // notify environment
      virtual void componentInitBegin(cComponent *component, int stage);
      virtual void simulationEvent(cEvent *event);
      virtual void messageScheduled(cMessage *msg);
      virtual void messageCancelled(cMessage *msg);
      virtual void beginSend(cMessage *msg, const SendOptions& options);
      virtual void messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result);
      virtual void messageSendHop(cMessage *msg, cGate *srcGate);
      virtual void messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result);
      virtual void endSend(cMessage *msg);
      virtual void messageDeleted(cMessage *msg);
      virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent);
      virtual void componentMethodEnd();
      virtual void moduleCreated(cModule *newmodule);
      virtual void moduleDeleted(cModule *module);
      virtual void moduleReparented(cModule *module, cModule *oldParent, int oldId);
      virtual void connectionCreated(cGate *srcgate);
      virtual void connectionDeleted(cGate *srcgate);
      virtual void displayStringChanged(cComponent *component);

      virtual bool isExpressMode() const {return runMode == RUNMODE_EXPRESS;}

      virtual void getImageSize(const char *imageName, double& outWidth, double& outHeight);
      virtual void getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent);
      virtual void appendToImagePath(const char *directory);
      virtual void loadImage(const char *fileName, const char *imageName=nullptr);
      virtual cFigure::Rectangle getSubmoduleBounds(const cModule *submodule);
      virtual std::vector<cFigure::Point> getConnectionLine(const cGate *sourceGate);
      virtual double getZoomLevel(const cModule *module);

      // smooth animation API
      virtual double getAnimationTime() const;
      virtual double getAnimationSpeed() const;
      virtual double getRemainingAnimationHoldTime() const;

      virtual void bubble(cComponent *component, const char *text);

      virtual void log(cLogEntry *entry);
      virtual void alert(const char *msg);
      virtual std::string gets(const char *prompt, const char *defaultReply = nullptr);
      virtual bool askYesNo(const char *question);
      virtual void askParameter(cPar *par, bool unassigned);

      virtual void refOsgNode(osg::Node *scene);
      virtual void unrefOsgNode(osg::Node *scene);
      virtual bool idle();

      virtual void pausePoint();
      virtual void requestQuitFromPausePointEventLoop(RunMode continueIn); // internal
      virtual bool isPaused() { return pauseEventLoop->isRunning(); }
      virtual int getPausePointNumber() { return pausePointNumber; }

      virtual bool ensureDebugger(cRuntimeError *error = nullptr);

      MainWindow *getMainWindow() { return mainWindow; }
      MessageAnimator *getMessageAnimator() { return messageAnimator; }
      DisplayUpdateController *getDisplayUpdateController() { return displayUpdateController; }
      ModuleLayouter *getModuleLayouter() { return &moduleLayouter; }

      GenericObjectInspector *getMainObjectInspector() { return mainInspector; }
      ModuleInspector *getMainModuleInspector() { return mainNetworkView; }
      LogInspector *getMainLogInspector() { return mainLogView; }
      TimeLineInspector *getMainTimeLineInspector() { return mainTimeLine; }
      // currently the rop-right one, ignoring the decoration, use accordingly
      QPoint getDefaultStopDialogCorner(const QPoint &offset = QPoint(-20, 80));

      void setPref(const QString &key, const QVariant &value);
      QVariant getPref(const QString &key, const QVariant &defaultValue = QVariant());
      QStringList getKeysInPrefGroup(const QString &prefGroup);

      void storeOptsInPrefs();
      void restoreOptsFromPrefs();

  Q_SIGNALS:
      void fontChanged();
      void objectDeletedSignal(cObject *object); // eh, name collisions

  public Q_SLOTS:
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

      void updateStoredInspector(cObject *newObject, cObject *oldObject); // Inspector* is the sender

      // will get the cComponent pointer and LogLevel in an
      // ActionDataPair from the data of the sender QAction
      void setComponentLogLevel();
      void setComponentLogLevel(cComponent *component, LogLevel level, bool save = false);

      void initialSetUpConfiguration();

  protected:
      // redefined virtual functions from EnvirBase
      virtual void doRun() override;
      virtual void printUISpecificHelp() override;

      virtual QtenvOptions *createOptions() override {return new QtenvOptions();}
      virtual void readOptions(cConfiguration *cfg) override;
      virtual void readPerRunOptions(cConfiguration *cfg) override;

      virtual void setupNetwork(cModuleType *network) override;

      virtual void displayException(std::exception& e) override;
      virtual std::string getWindowTitle();

      // Measuring elapsed time
      void checkTimeLimits();
      void resetClock();
      void startClock();
      void stopClock();
      double getElapsedSecs(); //FIXME into cEnvir, so it can be put into exception texts

  public:
      // New functions:
      void newNetwork(const char *networkname);
      void newRun(const char *configname, int runnumber);
      void createSnapshot(const char *label);

      void rebuildSim();
      void runSimulation(RunMode mode, simtime_t until_time=0, eventnumber_t until_eventnum=0, cMessage *until_msg=nullptr, cModule *until_module=nullptr, bool stopOnMsgCancel=true);
      void setSimulationRunMode(RunMode runMode);
      RunMode getSimulationRunMode() const {return runMode;}
      void setSimulationRunUntil(simtime_t until_time, eventnumber_t until_eventnum, cMessage *until_msg, bool stopOnMsgCancel=true);
      void setSimulationRunUntilModule(cModule *until_module);
      void prepareForRun();
      bool doRunSimulation();
      bool doRunSimulationExpress();

      void startAll();
      void finishSimulation(); // wrapper around simulation.callFinish() and simulation.endRun()
      bool checkRunning();

      // declared here just to make it public, simply delegates back to the base class
      virtual std::vector<int> resolveRunFilter(const char *configName, const char *runFilter) override;

      void loadNedFile(const char *fname, const char *expectedPackage=nullptr, bool isXML=false);

      void callRefreshDisplay();
      void callRefreshDisplaySafe(); // with exception handling

      void refreshInspectors();
      void callRefreshInspectors(); // with exception handling

      bool inspectorsAreFresh() { return inspectorsFresh; }

      void storeInspectors(bool closeThem);
      void restoreInspectors();

      int getRefreshDisplayCount() const { return refreshDisplayCount; }

      double computeModelAnimationSpeedRequest();
      double computeModelHoldEndTime();

      Inspector *inspect(cObject *obj, InspectorType type=INSP_DEFAULT, bool ignoreEmbedded=false);
      Inspector *addEmbeddedInspector(InspectorFactory *factory, QWidget *parent);
      Inspector *findFirstInspector(const cObject *obj, InspectorType type, bool ignoreEmbedded=false);
      void deleteInspector(Inspector *insp);
      void inspectorDeleted(Inspector *insp);
      const std::list<Inspector*>& getInspectors() {return inspectors;}

      EnvirBase *getEnvir() const {return dynamic_cast<EnvirBase*>(cSimulation::getActiveEnvir());}
      cConfiguration *getConfig() {return getEnvir()->getConfig();}

      bool isLoggingEnabled() const {return getEnvir()->isLoggingEnabled();}
      void setLoggingEnabled(bool enabled) {getEnvir()->setLoggingEnabled(enabled);}
      bool getDebugOnErrors() const {return getEnvir()->getDebugOnErrors();}
      void setDebugOnErrors(bool enable) {getEnvir()->setDebugOnErrors(enable);}
      bool getAttachDebuggerOnErrors() const {return getEnvir()->getAttachDebuggerOnErrors();}
      void setAttachDebuggerOnErrors(bool enable) {getEnvir()->setAttachDebuggerOnErrors(enable);}

      void setLogLevel(LogLevel logLevel) {getEnvir()->setLogLevel(logLevel);};
      void setLogFormat(const char *logFormat) {getEnvir()->setLogFormat(logFormat);}
      LogFormatter& getLogFormatter() {return getEnvir()->getLogFormatter();}

      bool getEventlogRecording() const {return getEnvir()->getEventlogRecording();}
      void setEventlogRecording(bool enabled) {getEnvir()->setEventlogRecording(enabled);}

      eState getSimulationState() {return simulationState;}
      void setStopSimulationFlag() {stopSimulationFlag = true;}
      bool getStopSimulationFlag() {return stopSimulationFlag;}
      bool getCallFinishOnExitFlag() { return callFinishOnExitFlag; }
      void setCallFinishOnExitFlag(bool flag) { callFinishOnExitFlag = flag; }
      Speedometer& getSpeedometer() {return speedometer;}
      LogBuffer *getLogBuffer() {return &logBuffer;}
      ComponentHistory *getComponentHistory() {return &componentHistory;}
      const char *getSilentEventFilters() const {return silentEventFilterLines.c_str();}
      void setSilentEventFilters(const char *filterLines);
      bool isSilentEvent(cMessage *msg);

      void performHoldAnimations(); // after an event, will run all holding anims, not moving simTime
      void skipHoldAnimations(); // terminates the above. if not running, no-op.

      void channelDisplayStringChanged(cChannel *channel);
      void moduleDisplayStringChanged(cModule *module);

      // small functions. Not inline to avoid including mainwindow.h in this file
      void updateNetworkRunDisplay();
      void updateSimtimeDisplay();
      void updateStatusDisplay();

      void confirm(DialogKind kind, const char *msg); // messagebox with OK button
      bool inputDialog(const char *title, const char *prompt,
                       const char *checkboxLabel, const char *defaultValue,
                       std::string& outResult, bool& inoutCheckState);

      void showException(std::exception& e);

      void addEventToLog(cEvent *event);

      std::string getLocalPackage()      {return getSimulation()->getNedPackageForFolder(opt->inifileNetworkDir.c_str());}
      const char *getIniFileName()       {return getInifileContents()->getFileName();}
      const char *getOutVectorFileName() {return getEnvir()->getOutVectorManager()->getFileName();}
      const char *getOutScalarFileName() {return getEnvir()->getOutScalarManager()->getFileName();}
      const char *getSnapshotFileName()  {return getEnvir()->getSnapshotManager()->getFileName();}
      const char *getWindowTitlePrefix() {return windowTitlePrefix.c_str();}

      QFont getBoldFont() const {return boldFont;}
      QFont getTimelineFont() const {return timelineFont;}
      QFont getCanvasFont() const {return canvasFont;}
      QFont getLogFont() const {return logFont;}
      QFont getTimeFont() const {return timeFont;}

      QFont getDefaultBoldFont() const {return defaultFonts.boldFont;}
      QFont getDefaultTimelineFont() const {return defaultFonts.timelineFont;}
      QFont getDefaultCanvasFont() const {return defaultFonts.canvasFont;}
      QFont getDefaultLogFont() const {return defaultFonts.logFont;}
      QFont getDefaultTimeFont() const {return defaultFonts.timeFont;}

      void setBoldFont(QFont &font) {boldFont = font;}
      void setTimelineFont(QFont &font) {timelineFont = font;}
      void setCanvasFont(QFont &font) {canvasFont = font;}
      void setLogFont(QFont &font) {logFont = font;}
      void setTimeFont(QFont &font) {timeFont = font;}

      void updateQtFonts();

      void initFonts();
      void saveFonts();
      QFont getFirstAvailableFontFamily(std::initializer_list<QString> preferenceList, int pointSize, QFont defaultValue = QString());

      void runSimulationLocal(RunMode runMode, cObject *object = nullptr, Inspector *insp = nullptr);
};


/**
 * Utility function
 */
inline Qtenv *getQtenv()
{
    return dynamic_cast<Qtenv *>(AppBase::getActiveApp());
}

}  // namespace qtenv
}  // namespace omnetpp

#endif
