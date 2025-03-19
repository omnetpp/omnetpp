//==========================================================================
//  MAINWINDOW.H - part of
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

#ifndef __OMNETPP_QTENV_MAINWINDOW_H
#define __OMNETPP_QTENV_MAINWINDOW_H

#include <set>
#include "ui_mainwindow.h"
#include <QtWidgets/QMainWindow>
#include <QtCore/QModelIndex>
#include "qtenv.h"

class QGraphicsScene;
class QStandardItem;
class QTreeView;
class QGraphicsView;
class QWidget;
class QSlider;
class QSplitter;
class QLabel;
class QToolButton;

namespace omnetpp {

class cObject;
class cEvent;
class cCanvas;
class cMessage;

namespace qtenv {

class Qtenv;
class Inspector;
class StopDialog;
class AnimationControllerDialog;
class FileEditor;

class QTENV_API MainWindow : public QMainWindow
{
    Q_OBJECT

    // stores actions and inspectors that have been disabled
    // only for layouting (so we can restore the state later)
    std::set<QObject*> disabledForLayouting;

public:
    explicit MainWindow(Qtenv *env, QWidget *parent = nullptr);
    ~MainWindow();

    void updateSimtimeDisplay();
    void updateStatusDisplay();
    void updateNetworkRunDisplay();
    void updateSimulationIdenticon(const QString &tooltip, const QString &seed);

    QWidget *getMainInspectorArea() { return ui->mainArea; }
    QWidget *getObjectTreeArea() { return ui->treeView; }
    QWidget *getObjectInspectorArea() { return ui->objectInspector; }
    QWidget *getLogInspectorArea() { return ui->logInspector; }
    QWidget *getTimeLineArea() { return ui->timeLine; }
    QAction *getStopAction() { return ui->actionStop; }
    QAction *getFindObjectsAction() { return ui->actionFindInspectObjects; }
    QAction *getShowAnimationParamsDialogAction() { return ui->actionShowAnimationParams; }

    void storeGeometry();
    void restoreGeometry();

    QSize sizeHint() const override { return QSize(1100, 700); }

    void busy(QString msg = "");
    bool isRunning();
    void setGuiForRunmode(RunMode runMode, bool untilMode = false);
    void setRunUntilModule(Inspector *insp = nullptr);
    bool networkReady();
    void runUntilMsg(cMessage *msg, RunMode runMode);
    void excludeMessageFromAnimation(cObject *msg);

    void reflectConfigOnUi();
    void configureNetwork();

    // the slider value is an integer, we divide it by 100 to get a double value, then raise it to 10's exponent
    static int playbackSpeedToSliderValue(double speed) { return std::round(std::log10(speed) * 100); }
    static double sliderValueToPlaybackSpeed(int value) { return std::pow(10, value / 100.0); }

public Q_SLOTS:

    void on_actionOneStep_triggered() { runSimulation(RUNMODE_STEP); } // has special skipping behavior of its own
    void on_actionRun_triggered() { stopOrRunSimulation(RUNMODE_NORMAL); }
    void on_actionFastRun_triggered() { stopOrRunSimulation(RUNMODE_FAST); }
    void on_actionExpressRun_triggered() { runSimulation(RUNMODE_EXPRESS); } // the whole window is disabled in express mode anyway
    void on_actionStop_triggered() { stopSimulation(); }

    void on_actionRunUntil_triggered();
    void on_actionDebugNextEvent_triggered();
    void on_actionDebugOnErrors_triggered(bool checked);
    void on_actionDebugNow_triggered();

    void on_actionQuit_triggered() { close(); }
    void onSliderValueChanged(int value);

    void on_actionSetUpUnconfiguredNetwork_triggered();
    void on_actionSetUpConfiguration_triggered();
    void on_actionRebuildNetwork_triggered();

    void on_actionPreferences_triggered();
    void on_actionStatusDetails_triggered();
    void on_actionFindInspectObjects_triggered();
    void on_actionEventlogRecording_triggered();

    void on_actionAbout_OMNeT_Qtenv_triggered();

    void closeEvent(QCloseEvent *event) override;

    void showFindObjectsDialog(cObject *obj);

    // without emitting the change signal!
    void updateSpeedSlider();

    // Disables all actions, widgets and inspectors, except the stopAction and widgets that contain it,
    // and saves the disabled objects into disabledForLayouting.
    // The exit counterpart must be called at least once between calls.
    void enterLayoutingMode();
    // Restores the changes made by the function above.
    // Will do no harm if called multiple times, or without any enterLayoutingMode call at all.
    void exitLayoutingMode();

protected Q_SLOTS:
    void on_actionVerticalLayout_triggered(bool checked);
    void on_actionHorizontalLayout_triggered(bool checked);
    void on_actionFlipWindowLayout_triggered();
    void on_actionTimeline_toggled(bool isSunken);

private Q_SLOTS:
    void onSplitterMoved(int, int);
    void onSimTimeLabelContextMenuRequested(QPoint pos);
    void onSimTimeLabelGroupingTriggered();
    void onSimTimeLabelUnitsTriggered();
    void onEventNumLabelContextMenuRequested(QPoint pos);
    void onEventNumLabelGroupingTriggered();
    void on_actionLoadNedFile_triggered();
    void on_actionOpenPrimaryIniFile_triggered();
    void on_actionCreate_Snapshot_triggered();
    void on_actionConcludeSimulation_triggered();
    void on_actionNetwork_triggered();
    void on_actionScheduledEvents_triggered();
    void on_actionSimulation_triggered();
    void on_actionNedComponentTypes_triggered();
    void on_actionRegisteredClasses_triggered();
    void on_actionClassDescriptors_triggered();
    void on_actionNED_Functions_triggered();
    void on_actionRegistered_Enums_triggered();
    void on_actionSupportedConfigurationOption_triggered();
    void on_actionResultFilters_triggered();
    void on_actionResultRecorders_triggered();
    void on_actionMessagePrinters_triggered();
    void on_actionInspectByPointer_triggered();
    void on_actionRecordVideo_toggled(bool checked);
    void on_actionShowAnimationParams_toggled(bool checked);
    void on_actionSimulationInfo_triggered() { showSimulationInfo(); }
    void showSimulationInfo();

Q_SIGNALS:
    void setNewNetwork();
    void closed();

private:
    Ui::MainWindow *ui;
    Qtenv *env;
    StopDialog *stopDialog = nullptr;
    QSlider *slider;
    QList<int> timeLineSize;
    bool showStatusDetails;
    static QString aboutText;
    QList<int> defaultTimeLineSize;
    FileEditor *fileEditor;
    QLabel *simTimeLabel = nullptr;
    QLabel *eventNumLabel = nullptr;
    QToolButton *identiconButton = nullptr;
    bool simTimeUnits;

    enum DigitGrouping
    {
        SPACE,
        COMMA,
        APOSTROPHE,
        NONE
    } simTimeDigitGrouping, eventNumDigitGrouping;

    void runSimulation(RunMode runMode);
    void stopSimulation();
    // If runMode is currently active, stops; otherwise starts in runMode.
    // adds radiobutton-like functionality to the "run"-like actions.
    void stopOrRunSimulation(RunMode runMode);

    void updatePerformanceDisplay();
    void updateNextEventDisplay();

    bool networkPresent();
    bool isSimulationOk();

    void copyToClipboard(cObject *object, int what);

    void saveSplitter(QString prefName, QSplitter *splitter);
    void restoreSplitter(QString prefName, QSplitter *splitter, const QList<int> &defaultSizes = QList<int>());

    void showStopDialog();
    void closeStopDialog();

    int inputBox(const QString &title, const QString &prompt, QString &variable);

    void updateSimTimeLabel();
    void updateEventNumLabel();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_MAINWINDOW_H
