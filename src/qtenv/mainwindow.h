//==========================================================================
//  MAINWINDOW.H - part of
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

#ifndef __OMNETPP_QTENV_MAINWINDOW_H
#define __OMNETPP_QTENV_MAINWINDOW_H

#include "qtenvdefs.h"
#include <QMainWindow>
#include <QModelIndex>

class QGraphicsScene;
class QStandardItem;
class QTreeView;
class QGraphicsView;
class QWidget;
class QSlider;
class QSplitter;

namespace Ui {
class MainWindow;
}

namespace omnetpp {

class cObject;
class cEvent;
class cCanvas;
class cMessage;

namespace qtenv {

class Qtenv;
class Inspector;
class StopDialog;
class FilteredObjectListDialog;
class FileEditor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum eMode { STEP, NORMAL, FAST, EXPRESS, NOT_RUNNING};

    explicit MainWindow(Qtenv *env, QWidget *parent = 0);
    ~MainWindow();

    void displayText(const char* t);
    void updateStatusDisplay();
    void updateNetworkRunDisplay();

    QWidget *getMainInspectorArea();
    QWidget *getObjectTreeArea();
    QWidget *getObjectInspectorArea();
    QWidget *getLogInspectorArea();
    QWidget *getTimeLineArea();

    void storeGeometry();
    void restoreGeometry();

    static int modeToRunMode(eMode mode);
    static eMode runModeToMode(int runMode);

    //menuproc.tcl
    bool isRunning();
    void setGuiForRunmode(eMode mode, bool untilMode = false);
    void setRunUntilModule(Inspector *insp = nullptr);
    bool networkReady();
    void runUntilMsg(cMessage *msg, int runMode);
    void excludeMessageFromAnimation(cObject *msg);

protected:
    // if the parameter is true, the dialog will be shown even if there is only a single config
    void setupConfiguration(bool forceDialog);

public slots:
    void on_actionOneStep_triggered();
    bool on_actionQuit_triggered();
    void on_actionRun_triggered();
    void on_actionSetUpConfiguration_triggered();
    void on_actionFastRun_triggered();
    void on_actionExpressRun_triggered();
    void on_actionRunUntil_triggered();
    void onSliderValueChanged(int value);
    void on_actionRebuildNetwork_triggered();
    void closeEvent(QCloseEvent *event);
    void initialSetUpConfiguration();
    void on_actionPreferences_triggered();
    void on_actionStatusDetails_triggered();
    void on_actionFindInspectObjects_triggered();
    void on_actionStop_triggered();
    void on_actionDebugNextEvent_triggered();
    void on_actionEventlogRecording_triggered();
    void on_actionSetUpUnconfiguredNetwork_triggered();
    void on_actionAbout_OMNeT_Qtenv_triggered();

protected slots:
    void on_actionVerticalLayout_triggered(bool checked);
    void on_actionHorizontalLayout_triggered(bool checked);
    void on_actionTimeline_toggled(bool isSunken);
    // most likely the slider in the preferences window was moved
    void onAnimationSpeedChanged(float speed);

private slots:
    void onSplitterMoved(int, int);
    void on_actionLoadNEDFile_triggered();
    void on_actionOpenPrimaryIniFile_triggered();
    void on_actionCreate_Snapshot_triggered();
    void on_actionConcludeSimulation_triggered();
    void on_actionNetwork_triggered();
    void on_actionScheduledEvents_triggered();
    void on_actionSimulation_triggered();

private:
    Ui::MainWindow *ui;
    Qtenv *env;
    StopDialog *stopDialog;
    QSlider *slider;
    QList<int> timeLineSize;
    bool showStatusDetails;
    FilteredObjectListDialog *filteredObjectListDialog;
    static QString aboutText;
    QList<int> defaultTimeLineSize;
    FileEditor *fileEditor;

    bool checkRunning();
    void runSimulation(eMode mode);

    void updateSimtimeDisplay();
    void updatePerformanceDisplay();
    void updateNextModuleDisplay();
    int getObjectId(cEvent *object);
    const char *getObjectShortTypeName(cObject *object);
    const char *stripNamespace(const char *className);

    bool networkPresent();
    bool isSimulationOk();

    void busy(QString msg = "");
    void copyToClipboard(cObject *object, int what);

    void saveSplitter(QString prefName, QSplitter *splitter);
    void restoreSplitter(QString prefName, QSplitter *splitter);

    void reflectRecordEventlog();

    void showStopDialog();
    void closeStopDialog();
};

} // namespace qtenv
} // namespace omnetpp

#endif // MAINWINDOW_H
