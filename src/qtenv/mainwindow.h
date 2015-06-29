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

#include <QDebug>

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
    enum eCopy { COPY_PTR, COPY_PTRWITHCAST, COPY_FULLPATH, COPY_FULLNAME, COPY_CLASSNAME };

    explicit MainWindow(Qtenv *env, QWidget *parent = 0);

    void displayText(const char* t);
    QTreeView *getObjectTree();
    QTreeView *getObjectInspectorTree();
    void updateStatusDisplay();
    void updateNetworkRunDisplay();
    void redrawTimeline();

    QWidget *getMainArea();
    void runSimulationLocal(Inspector *insp, int runMode, cObject *object = nullptr);

    ~MainWindow();

private slots:
    void on_actionOneStep_triggered();
    bool on_actionQuit_triggered();
    void on_actionRun_triggered();
    void on_actionSetUpConfiguration_triggered();
    void on_actionFastRun_triggered();
    void on_actionExpressRun_triggered();
    void on_actionRunUntil_triggered();
    void inspectObject(QModelIndex index);
    void onTreeViewContextMenu(QPoint point);
    void onTreeViewPressed(QModelIndex index);
    void on_actionRebuildNetwork_triggered();
    void closeEvent(QCloseEvent *event);
    void initialSetUpConfiguration();

public slots:
    void on_actionStop_triggered();
    void onClickOpenInspector();
    void onClickRun();
    void onClickRunMessage();
    void onClickExcludeMessage();
    void onClickUtilitiesSubMenu();

private:
    enum Mode { STEP, NORMAL, FAST, EXPRESS, NOT_RUNNING};
    Ui::MainWindow *ui;
    Qtenv *env;
    StopDialog *stopDialog;
    QSlider *slider;

    bool isRunning();
    bool checkRunning();
    void setGuiForRunmode(Mode mode, Inspector *insp = nullptr, bool untilMode = false);
    void runSimulation(Mode mode);

    void updateSimtimeDisplay();
    void updatePerformanceDisplay();
    void updateNextModuleDisplay();
    int getObjectId(cEvent *object);
    const char *getObjectShortTypeName(cObject *object);
    const char *stripNamespace(const char *className);

    void inspectObject(cObject *object, int type = 0, const char *geometry = "");
    void setRunUntilModule(Inspector *insp = nullptr);
    int modeToRunMode(Mode mode);
    Mode runModeToMode(int runMode);

    void runUntilMsg(cMessage *msg, int runMode);
    void excludeMessageFromAnimation(cObject *msg);

    bool networkReady();
    bool networkPresent();
    bool isSimulationOk();

    void busy(QString msg = "");
    void copyToClipboard(cObject *object, int what);
    void setClipboard(QString str);
};

} // namespace qtenv
} // namespace omnetpp

#endif // MAINWINDOW_H
