#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsItem>
#include <QMessageBox>
#include "qtenv.h"
#include "runselectiondialog.h"

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayText(const char *t) {
    ui->textBrowser->append(QString(t));
}

bool MainWindow::isRunning()
{
    Qtenv::eState state = (Qtenv::eState)env->getSimulationState();
    return state == Qtenv::SIM_RUNNING || state == Qtenv::SIM_BUSY;
}

void MainWindow::setGuiForRunmode(Mode mode, bool untilMode)
{
    ui->actionOneStep->setChecked(false);
    ui->actionRun->setChecked(false);
    ui->actionFastRun->setChecked(false);
    ui->actionExpressRun->setChecked(false);
    //TODO Remove Stop Dialog

    switch(mode)
    {
        case STEP:
            ui->actionOneStep->setChecked(true);
            break;
        case NORMAL:
            ui->actionRun->setChecked(true);
            break;
        case FAST:
            ui->actionFastRun->setChecked(true);
            break;
        case EXPRESS:
            ui->actionExpressRun->setChecked(true);
            //TODO display Stop Dialog
            break;
        case NOT_RUNNING:
            ui->actionRunUntil->setChecked(false);
            break;
    }

    ui->actionRunUntil->setChecked(untilMode);
}

bool MainWindow::checkRunning()
{
    if(env->getSimulationState() == Qtenv::eState::SIM_RUNNING)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Sorry, you cannot do this while the simulation is running. Please stop it first."),
                             QMessageBox::Ok);
        return true;
    }
    if(env->getSimulationState() == Qtenv::eState::SIM_BUSY)
    {
        QMessageBox::warning(this, tr("Warning"), tr("The simulation is waiting for external synchronization -- press STOP to interrupt it."),
                             QMessageBox::Ok);
        return true;
    }
    return false;
}

void MainWindow::on_actionOneStep_triggered()
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

    if(isRunning())
    {

    }
    else
    {

    }
    env->doOneStep();
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionRun_triggered()
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

    if(isRunning())
    {

    }
    env->runSimulation(Qtenv::RUNMODE_NORMAL);
}

void MainWindow::on_actionSetUpConfiguration_triggered()
{
    if(checkRunning())
        return;

    RunSelectionDialog *dialog = new RunSelectionDialog(env, this);
    dialog->exec();
    //TODO Add all open inspectors to the inspector list.
    //TODO log
    env->newRun(dialog->getConfigName().c_str(), dialog->getRunNumber());

    if(getSimulation()->getSystemModule() != nullptr)
    {
        //TODO log
        //TODO notify plugins newNetwork
    }

    delete dialog;
}

void MainWindow::on_actionStop_triggered()
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
