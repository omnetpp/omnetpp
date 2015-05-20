#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsItem>
#include <QMessageBox>
#include "qtenv.h"
#include "runselectiondialog.h"
#include "treeitemmodel.h"
#include "omnetpp/csimplemodule.h"
#include "common/stringutil.h"

#include "qdebug.h"

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

    TreeItemModel *model = new TreeItemModel();
    model->setRootObject(getSimulation());
    ui->treeView->setModel(model);
    ui->treeView->setHeaderHidden(true);

    //TODO
    //if($config(display-statusdetails)
    //{
//        ui->labelDisplay1->hide();
//        ui->labelDisplay2->hide();
//        ui->labelDisplay3->hide();
    //}
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayText(const char *t) {
    ui->textBrowser->append(QString(t));
}

QTreeView *MainWindow::getObjectTree()
{
    return ui->treeView;
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
    if(env->getSimulationState() == Qtenv::SIM_RUNNING)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Sorry, you cannot do this while the simulation is running. Please stop it first."),
                             QMessageBox::Ok);
        return true;
    }
    if(env->getSimulationState() == Qtenv::SIM_BUSY)
    {
        QMessageBox::warning(this, tr("Warning"), tr("The simulation is waiting for external synchronization -- press STOP to interrupt it."),
                             QMessageBox::Ok);
        return true;
    }
    return false;
}

void MainWindow::on_actionOneStep_triggered()
{
    if(isRunning())
    {
        setGuiForRunmode(STEP);
        env->setStopSimulationFlag();
    }
    else
    {
        //TODO Network Ready
        setGuiForRunmode(STEP);
        env->doOneStep();
        setGuiForRunmode(NOT_RUNNING);
    }
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::runSimulation(Mode mode)
{
    Qtenv::eRunMode runMode;

    switch(mode)
    {
        case NOT_RUNNING:
        case STEP:
            break;
        case NORMAL:
            runMode = Qtenv::RUNMODE_NORMAL;
            break;
        case FAST:
            runMode = Qtenv::RUNMODE_FAST;
            break;
        case EXPRESS:
            runMode = Qtenv::RUNMODE_EXPRESS;
            break;
    }

    if(isRunning())
    {
        setGuiForRunmode(mode);
        env->setSimulationRunMode(runMode);
        //TODO Set Run Until Module
    }
    else
    {
        //TODO Network Ready
        setGuiForRunmode(mode);
        //TODO opp_run
        env->runSimulation(mode);
        setGuiForRunmode(NOT_RUNNING);
    }
}

void MainWindow::on_actionRun_triggered()
{
    runSimulation(NORMAL);
}

void MainWindow::on_actionSetUpConfiguration_triggered()
{
    if(checkRunning())
        return;

    RunSelectionDialog *dialog = new RunSelectionDialog(env, this);
    dialog->exec();
//    debug "selected $configname $runnumber"
//    busy "Setting up network..."
//    inspectorList:addAll 1
    env->newRun(dialog->getConfigName().c_str(), dialog->getRunNumber());
//    reflectRecordEventlog
//    busy

    if(getSimulation()->getSystemModule() != nullptr)
    {
//        # tell plugins about it
//        busy "Notifying Tcl plugins..."
//        notifyPlugins newNetwork
//        busy
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

void MainWindow::on_actionFastRun_triggered()
{
    runSimulation(FAST);
}

void MainWindow::on_actionExpressRun_triggered()
{
    runSimulation(EXPRESS);
}

void MainWindow::on_actionRunUntil_triggered()
{

}

void MainWindow::inspectObject(QModelIndex index)
{
    qDebug() << "Inspecting object.";
    if(!index.isValid())
        return;

    cObject *parent = static_cast<cObject*>(index.internalPointer());
    cCollectChildrenVisitor visitor(parent);
    visitor.process(parent);
    cObject **objs = visitor.getArray();
    if(visitor.getArraySize() > index.row())
        env->inspect(objs[index.row()], 0, true, "");
}

void MainWindow::updateStatusDisplay()
{
    updateSimtimeDisplay();

    if(true)    //TODO $config(display-statusdetails)
    {
        int runMode = env->getSimulationRunMode();
        if(env->getSimulationState() == Qtenv::SIM_RUNNING &&
                (runMode == Qtenv::RUNMODE_FAST || runMode == Qtenv::RUNMODE_EXPRESS))
            updatePerformanceDisplay();
        else
            updateNextModuleDisplay();
    }
}

void MainWindow::updateSimtimeDisplay()
{
    ui->labelEvent->setText("Event #" + QString::number(getSimulation()->getEventNumber()));
    ui->labelTime->setText("t=" + QString(getSimulation()->guessNextSimtime().str().c_str()) + "s");
    ui->labelMessageStats->setText("Msg stats: " + QString::number(getSimulation()->msgQueue.getLength()) +
                                " scheduled / " + QString::number(cMessage::getLiveMessageCount()) +
                                " existing / " + QString::number(cMessage::getTotalMessageCount()) + " created");
}

void MainWindow::updatePerformanceDisplay()
{
    ui->labelDisplay2->setText("Simsec/sec: " + QString::number(env->getSpeedometer().getSimSecPerSec()));
    ui->labelDisplay1->setText("Ev/sec: " + QString::number(env->getSpeedometer().getEventsPerSec()));
    ui->labelDisplay3->setText("Ev/simsec: " + QString::number(env->getSpeedometer().getEventsPerSimSec()));
}

void MainWindow::updateNextModuleDisplay()
{
    cSimpleModule *modptr = nullptr;
    cEvent *msgptr = nullptr;

    int state = env->getSimulationState();
    if(state == Qtenv::SIM_NEW || state == Qtenv::SIM_READY || state == Qtenv::SIM_RUNNING)
    {
        modptr = getSimulation()->guessNextModule();
        msgptr = getSimulation()->guessNextEvent();
    }

    if(msgptr)
    {
        int objId = getObjectId(msgptr);
        ui->labelDisplay1->setText(QString("Next: ") + msgptr->getName() + " (" + msgptr->getClassName() +
                                   ", id=" + (objId == -1 ? "" : QString::number(objId)) + ")");
        ui->labelDisplay3->setText(QString("At: last event + ") +
                                   (getSimulation()->guessNextEvent()->getArrivalTime() - getSimulation()->getSimTime()).str().c_str() +
                                   "s");
    }
    else {
        ui->labelDisplay1->setText("Next: n/a");
        ui->labelDisplay3->setText("At: n/a");
    }

    if(modptr)
        ui->labelDisplay2->setText(QString("In: ") + modptr->getFullPath().c_str() +
                                   " (" + getObjectShortTypeName(modptr) + ", id=" +
                                   getObjectShortTypeName(modptr) + ")");
    else
        ui->labelDisplay2->setText("In: n/a");
}

int MainWindow::getObjectId(cEvent *object)
{
    if (dynamic_cast<cModule *>(object))
        return dynamic_cast<cModule *>(object)->getId();
    if (dynamic_cast<cMessage *>(object))
        return dynamic_cast<cMessage *>(object)->getId();

     return -1;
}

const char *MainWindow::getObjectShortTypeName(cObject *object)
{
    if (dynamic_cast<cComponent*>(object))
    {
        try
        {
            return static_cast<cComponent*>(object)->getComponentType()->getName();
        }
        catch(std::exception& e)
        {
            printf("<!> Warning: %s\n", e.what());
        }
    }
    return stripNamespace(object->getClassName());
}

const char *MainWindow::stripNamespace(const char *className)
{
    switch (env->opt->stripNamespace) {
        case STRIPNAMESPACE_ALL: {
            const char *lastColon = strrchr(className, ':');
            if (lastColon)
                className = lastColon+1;
            break;
        }
        case STRIPNAMESPACE_OMNETPP: {
            if (className[0]=='o' && opp_stringbeginswith(className, "omnetpp::"))
                className += sizeof("omnetpp::")-1;
            break;
        }
        case STRIPNAMESPACE_NONE:
            break;
    }
    return className;
}

void MainWindow::updateNetworkRunDisplay()
{
    const char *configname = opp_nulltoempty(env->getConfigEx()->getActiveConfigName());
    const char *network = !getSimulation()->getNetworkType() ? "" : getSimulation()->getNetworkType()->getName();

    //TODO
//    if {$configname==""} {set configName "n/a"}
//    if {$network==""} {set network "(no network)"}
    ui->labelConfigName->setText(QString(configname) + " #" + QString::number(env->getConfigEx()->getActiveRunNumber()) + ": " + network);
}
