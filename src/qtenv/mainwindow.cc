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

//void MainWindow::setGuiForRunmode(Mode mode, bool untilmode)
//{  #FIXME needs to be revised
//    global opp
//    set insp $modinspwin
//    if {$insp!="" && ![winfo exists $insp]} {set insp ""}

//    toolbutton:setsunken .toolbar.step 0
//    toolbutton:setsunken .toolbar.run 0
//    toolbutton:setsunken .toolbar.fastrun 0
//    toolbutton:setsunken .toolbar.exprrun 0
//    catch {toolbutton:setsunken $opp(sunken-run-button) 0}
//    removeStopDialog

//    if {$insp==""} {
//        if {$mode=="step"} {
//            toolbutton:setsunken .toolbar.step 1
//        } elseif {$mode=="normal"} {
//            toolbutton:setsunken .toolbar.run 1         //Ha rákattintok a gombra lenyomva marad kivéve a one step-nél
//        } elseif {$mode=="fast"} {
//            toolbutton:setsunken .toolbar.fastrun 1
//        } elseif {$mode=="express"} {
//            toolbutton:setsunken .toolbar.exprrun 1
//            displayStopDialog                       //Express gombra egy stop ablak előugrik
//        } elseif {$mode=="notrunning"} {
//            toolbutton:setsunken .toolbar.until 0
//        } else {
//            error "wrong mode parameter $mode"
//        }
//    } else {
//        if {$mode=="normal"} {
//            toolbutton:setsunken $insp.toolbar.mrun 1
//            set opp(sunken-run-button) $insp.toolbar.mrun
//        } elseif {$mode=="fast"} {
//            toolbutton:setsunken $insp.toolbar.mfast 1
//            set opp(sunken-run-button) $insp.toolbar.mfast
//        } elseif {$mode=="express"} {
//            displayStopDialog
//        } elseif {$mode=="notrunning"} {
//            toolbutton:setsunken .toolbar.until 0
//        } else {
//            error "wrong mode parameter $mode with module inspector"
//        }
//    }

//    if {$untilmode=="until_on"} {
//        toolbutton:setsunken .toolbar.until 1
//    } elseif {$untilmode=="until_off"} {
//        toolbutton:setsunken .toolbar.until 0
//    } elseif {$untilmode!=""} {
//        error "wrong untilmode parameter $mode"
//    }
//}

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

    env->runSimulation(Qtenv::RUNMODE_NORMAL);
}

void MainWindow::on_actionSetUpConfiguration_triggered()
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

    if(checkRunning())
        return;

    RunSelectionDialog *dialog = new RunSelectionDialog(env);
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
