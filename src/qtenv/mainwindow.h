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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum eMode { STEP, NORMAL, FAST, EXPRESS, NOT_RUNNING};

    explicit MainWindow(Qtenv *env, QWidget *parent = 0);

    void displayText(const char* t);
    void updateStatusDisplay();
    void updateNetworkRunDisplay();

    QWidget *getMainInspectorArea();
    QTreeView *getObjectTree();
    QWidget *getObjectInspectorArea();
    QWidget *getLogInspectorArea();
    QWidget *getTimeLineArea();

    void storeGeometry();
    void restoreGeometry();

    static int modeToRunMode(eMode mode);
    static eMode runModeToMode(int runMode);

    //menuproc.tcl
    bool isRunning();
    void setGuiForRunmode(eMode mode, Inspector *insp = nullptr, bool untilMode = false);
    void setRunUntilModule(Inspector *insp = nullptr);
    bool networkReady();
    void runUntilMsg(cMessage *msg, int runMode);
    void excludeMessageFromAnimation(cObject *msg);

private slots:
    void on_actionOneStep_triggered();
    bool on_actionQuit_triggered();
    void on_actionRun_triggered();
    void on_actionSetUpConfiguration_triggered();
    void on_actionFastRun_triggered();
    void on_actionExpressRun_triggered();
    void on_actionRunUntil_triggered();
    void onSliderValueChanged(int value);
    void inspectObject(QModelIndex index);
    void onTreeViewContextMenu(QPoint point);
    void onTreeViewPressed(QModelIndex index);
    void on_actionRebuildNetwork_triggered();
    void closeEvent(QCloseEvent *event);
    void initialSetUpConfiguration();

public slots:
    void on_actionStop_triggered();

private:
    Ui::MainWindow *ui;
    Qtenv *env;
    StopDialog *stopDialog;
    QSlider *slider;

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
};

} // namespace qtenv
} // namespace omnetpp

#endif // MAINWINDOW_H
