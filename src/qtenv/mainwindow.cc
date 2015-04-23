#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsItem>
#include "qtenv.h"

using namespace qtenv;

MainWindow::MainWindow(Qtenv *env, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    env(env)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(exitOmnetpp()));
    connect(ui->actionOne_Step, SIGNAL(triggered()), this, SLOT(oneStep()));
    connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(runSimulation()));
    connect(ui->actionSet_Up_a_Configuration, SIGNAL(triggered()), this, SLOT(setUpConfiguration()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(stopSimulation()));
}

void MainWindow::exitOmnetpp()
{
    close();
}

void MainWindow::setUpConfiguration()//tkenv: newRun
{
//    proc newRun {} {
//        # implements File|New run...

//        if [checkRunning] return

//        # pop up selection dialog
//        set configandrun [runSelectionDialog]
//        if {[llength $configandrun] != 0} {
//           setvars {configname runnumber} $configandrun
//           debug "selected $configname $runnumber"
//           busy "Setting up network..."
//           inspectorList:addAll 1
//           opp_newrun $configname $runnumber
//           reflectRecordEventlog
//           busy

//           if [opp_isnotnull [opp_object_systemmodule]] {
//               # tell plugins about it
//               busy "Notifying Tcl plugins..."
//               notifyPlugins newNetwork
//               busy
//           }
//        }
//    }

    env->newRun("CarDemo", 0);
}

void MainWindow::oneStep()
{
//# implements Simulate|One step

//    if [isRunning] {
//        setGuiForRunmode step
//        opp_stopsimulation
//    } else {
//        if {![networkReady]} {return}
//        setGuiForRunmode step
//        opp_onestep
//        setGuiForRunmode notrunning
//    }

    env->doOneStep();
}

void MainWindow::runSimulation()
{
//    if [isRunning] {
//            setGuiForRunmode $mode
//            opp_set_run_mode $mode
//            opp_set_run_until_module
//        } else {
//            if {![networkReady]} {return}
//            setGuiForRunmode $mode
//            opp_run $mode
//            setGuiForRunmode notrunning
//        }

    env->runSimulation(Qtenv::RUNMODE_NORMAL);
}

void MainWindow::stopSimulation()
{
//    proc stopSimulation {} {
//        # implements Simulate|Stop
//        if {[opp_getsimulationstate] == "SIM_RUNNING" || [opp_getsimulationstate] == "SIM_BUSY"} {
//           # "opp_stopsimulation" just *asks* the simulation to stop, causing it to return
//           # from the "opp_run" command.
//           # "setGuiForRunmode notrunning" will be called after "opp_run" has returned.
//           opp_stopsimulation
//        }
//
//        # this proc doubles as "stop layouting", when in graphical module inspectors
//        global stoplayouting
//        set stoplayouting 1
//    }

    env->setStopSimulationFlag();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayText(const char *t) {
    ui->textBrowser->append(QString(t));
}
