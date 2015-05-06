#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsItem>
#include <QMessageBox>
#include <QStandardItemModel>
#include "qtenv.h"
#include "runselectiondialog.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cwatch.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/ccanvas.h"

#include "qdebug.h"

using namespace qtenv;

MainWindow::MainWindow(Qtenv *env, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    env(env),
    treeModel(new QStandardItemModel())
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);

    ui->treeView->setModel(treeModel);
    ui->treeView->setHeaderHidden(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete treeModel;
}

void MainWindow::displayText(const char *t) {
    ui->textBrowser->append(QString(t));
}

void MainWindow::initTreeManager()
{
    treeModel->clear();
    //event handler's are connected in ui
    setNode(getSimulation(), treeModel->invisibleRootItem());
}

void MainWindow::setNode(cObject *node, QStandardItem *parent)
{
    long id = -1;
    if (dynamic_cast<cModule *>(node))
        id = dynamic_cast<cModule *>(node)->getId();
    else if (dynamic_cast<cMessage *>(node))
        id = dynamic_cast<cMessage *>(node)->getId();

    QString text = node->getFullName() + QString(" (") + getObjectShortTypeName(node) + ")" + (id>=0 ? "(id=" + QString::number(id) + QString(")") : "");

    //Set node in tree
    QIcon icon(":/objects/icons/objects/" + getObjectIcon(node));
    QStandardItem *item = new QStandardItem(icon, text);
    parent->appendRow(item);

    //Has child objects?
    cHasChildrenVisitor visitor(node);
    visitor.process(node);
    if(visitor.getResult())
    {
        //Call recursively setNode with its child object
        cCollectChildrenVisitor visitor(node);
        visitor.process(node);

        cObject **objs = visitor.getArray();
        for(size_t i = 0; i < visitor.getArraySize(); ++i)
            setNode(objs[i], item);
    }
}

QString MainWindow::getObjectIcon(cObject *object)
{
    QString iconName;
    if (object == NULL)
        iconName = "none_vs.png";
    else if (dynamic_cast<cModule *>(object) && ((cModule *)object)->isPlaceholder())
        iconName = "placeholder_vs.png";
    else if (dynamic_cast<cSimpleModule *>(object))
        iconName = "simple_vs.png";
    else if (dynamic_cast<cModule *>(object))
        iconName = "compound_vs.png";
    else if (dynamic_cast<cWatchBase *>(object))
        iconName = "cogwheel_vs.png";
    else if (dynamic_cast<cMessage *>(object))
        iconName = "message_vs.png";
    else if (dynamic_cast<cArray *>(object))
        iconName = "container_vs.png";
    else if (dynamic_cast<cQueue *>(object))
        iconName = "queue_vs.png";
    else if (dynamic_cast<cGate *>(object))
        iconName = "gate_vs.png";
    else if (dynamic_cast<cPar *>(object))
        iconName = "param_vs.png";
    else if (dynamic_cast<cChannel *>(object))
        iconName = "chan_vs.png";
    else if (dynamic_cast<cOutVector *>(object))
        iconName = "outvect_vs.png";
    else if (dynamic_cast<cStatistic *>(object))
        iconName = "stat_vs.png";
    else if (dynamic_cast<cFigure *>(object))
        iconName = "figure_vs.png";
    else if (dynamic_cast<cCanvas *>(object))
        iconName = "canvas_vs.png";
    else if (dynamic_cast<cSimulation *>(object))
        iconName = "container_vs.png";
    else if (dynamic_cast<cMessageHeap *>(object))
        iconName = "container_vs.png";
    else if (dynamic_cast<cRegistrationList *>(object))
        iconName = "container_vs.png";
    else
        iconName = "cogwheel_vs.png";

    return iconName;
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
    //TODO Add all open inspectors to the inspector list.
    //TODO log
    env->newRun(dialog->getConfigName().c_str(), dialog->getRunNumber());

    if(getSimulation()->getSystemModule() != nullptr)
    {
        //TODO log
        //TODO notify plugins newNetwork
        initTreeManager();
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
