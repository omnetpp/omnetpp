//==========================================================================
//  MAINWINDOW.CC - part of
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QStyledItemDelegate>
#include <QTextLayout>
#include <QTimer>
#include <QLineEdit>
#include "qtenv.h"
#include "runselectiondialog.h"
#include "treeitemmodel.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cfutureeventset.h"
#include "inspector.h"
#include "common/stringutil.h"
#include "stopdialog.h"
#include "inspectorutil.h"
#include "moduleinspector.h"
#include "loginspector.h"
#include "timelinegraphicsview.h"

#include <QDebug>

using namespace OPP::common;

namespace omnetpp {
namespace qtenv {

MainWindow::MainWindow(Qtenv *env, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    env(env)
{
    ui->setupUi(this);

    stopDialog = new StopDialog(this);

    // initialize timeline
    QVariant variant = getQtenv()->getPref("display-timeline");
    bool isSunken = variant.isValid() ? variant.value<bool>() : true;
    ui->timeLine->setVisible(isSunken);
    ui->actionTimeline->setChecked(isSunken);

    //TODO
    slider = new QSlider();
    slider->setMinimum(50);
    slider->setMaximum(300);
    slider->setOrientation(Qt::Orientation::Horizontal);
    slider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    slider->setValue(getQtenv()->opt->animationSpeed * 100);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    ui->toolBar->addWidget(slider);

    //TODO
    //if($config(display-statusdetails)
    //{
//        ui->labelDisplay1->hide();
//        ui->labelDisplay2->hide();
//        ui->labelDisplay3->hide();
    // }

    // if we trigger the action here directly, it will block the initialization
    // this way the main window will be shown before the setup dialog
    // because the timer event is processed in the event loop
    QTimer::singleShot(0, this, SLOT(initialSetUpConfiguration()));
}

void MainWindow::displayText(const char *t)
{
    // ui->textBrowser->append(QString(t));
}


bool MainWindow::isRunning()
{
    Qtenv::eState state = (Qtenv::eState)env->getSimulationState();
    return state == Qtenv::SIM_RUNNING || state == Qtenv::SIM_BUSY;
}

void MainWindow::setGuiForRunmode(eMode mode, Inspector *insp, bool untilMode)
{
    ui->actionOneStep->setChecked(false);
    ui->actionRun->setChecked(false);
    ui->actionFastRun->setChecked(false);
    ui->actionExpressRun->setChecked(false);
    // TODO
    // catch {toolbutton:setsunken $opp(sunken-run-button) 0}

    if (insp == nullptr)
        switch (mode) {
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
                // TODO disabled mainwindow
                stopDialog->show();
                break;

            case NOT_RUNNING:
                ui->actionRunUntil->setChecked(false);
                break;
        }
    else
        switch (mode) {
            case NORMAL:
                // TODO
                // toolbutton:setsunken $insp.toolbar.mrun 1
                // set opp(sunken-run-button) $insp.toolbar.mrun
                break;

            case FAST:
                // TODO
                // toolbutton:setsunken $insp.toolbar.mfast 1
                // set opp(sunken-run-button) $insp.toolbar.mfast
                break;

            case EXPRESS:
                // TODO disabled mainwindow
                stopDialog->show();
                break;

            case NOT_RUNNING:
                // TODO toolbutton:setsunken .toolbar.until 0
                break;

            case STEP:
                break;
        }

    ui->actionRunUntil->setChecked(untilMode);
}

bool MainWindow::checkRunning()
{
    if (env->getSimulationState() == Qtenv::SIM_RUNNING) {
        QMessageBox::warning(this, tr("Warning"), tr("Sorry, you cannot do this while the simulation is running. Please stop it first."),
                QMessageBox::Ok);
        return true;
    }
    if (env->getSimulationState() == Qtenv::SIM_BUSY) {
        QMessageBox::warning(this, tr("Warning"), tr("The simulation is waiting for external synchronization -- press STOP to interrupt it."),
                QMessageBox::Ok);
        return true;
    }
    return false;
}

// oneStep
void MainWindow::on_actionOneStep_triggered()
{
    // implements Simulate|One step

    if (isRunning()) {
        setGuiForRunmode(STEP);
        env->setStopSimulationFlag();
    }
    else {
        if (!networkReady())
            return;
        setGuiForRunmode(STEP);
        env->doOneStep();
        setGuiForRunmode(NOT_RUNNING);
    }

    getQtenv()->getAnimator()->hurry();
}

// exitOmnetpp
bool MainWindow::on_actionQuit_triggered()
{
    // TODO
//    global config
//    if {$config(confirm-exit)} {
    if (getSimulation()->getSystemModule() != nullptr) {
        if (isRunning()) {
            int ans = QMessageBox::warning(this, tr("Warning"), tr("The simulation is currently running. Do you really want to quit?"),
                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (ans == QMessageBox::No)
                return false;
        }
        else if (env->getSimulationState() == Qtenv::SIM_READY) {
            int ans = QMessageBox::warning(this, tr("Warning"), tr("Do you want to conclude the simulation by invoking finish() before exiting?"),
                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
            if (ans == QMessageBox::Yes)
                env->finishSimulation();
            else if (ans == QMessageBox::Cancel)
                return false;
        }
        else {
            // #set ans [messagebox {Warning} {Do you really want to quit?} warning yesno]
        }
    }
//    }

    if (isRunning())
        env->setStopSimulationFlag();

//    # save settings (fonts etc) globally, and inspector list locally
//    saveTkenvrc "~/.tkenvrc" 1 1 "# Global OMNeT++/Tkenv config file"
//    saveTkenvrc ".tkenvrc"   0 1 "# Partial OMNeT++/Tkenv config file -- see \$HOME/.tkenvrc as well"

    close();
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!on_actionQuit_triggered())
        event->ignore();
}

void MainWindow::initialSetUpConfiguration()
{
    firstInit = true;
    ui->actionSetUpConfiguration->trigger();
}

void MainWindow::runSimulation(eMode mode)
{
    Qtenv::eRunMode runMode = (Qtenv::eRunMode)modeToRunMode(mode);

    if (isRunning()) {
        setGuiForRunmode(mode);
        env->setSimulationRunMode(runMode);
        setRunUntilModule();
    }
    else {
        if (!networkReady())
            return;
        setGuiForRunmode(mode);
        env->runSimulation(mode);
        setGuiForRunmode(NOT_RUNNING);
    }
}

// runNormal
void MainWindow::on_actionRun_triggered()
{
    // implements Simulate|Run
    runSimulation(NORMAL);
}

// newRun
void MainWindow::on_actionSetUpConfiguration_triggered()
{
    // implements File|Set Up a Configuration...
    if (checkRunning())
        return;

    RunSelectionDialog *dialog = new RunSelectionDialog(this, firstInit);
    firstInit = false;
    if (dialog->exec()) {
        // TODO debug "selected $configname $runnumber"
        busy("Setting up network...");
        // TODO inspectorList:addAll 1
        env->newRun(dialog->getConfigName().c_str(), dialog->getRunNumber());
        // TODO reflectRecordEventlog
        busy();

        if (getSimulation()->getSystemModule() != nullptr) {
            // tell plugins about it
            busy("Notifying Tcl plugins...");
            // TODO notifyPlugins newNetwork
            busy();
        }
    }
    delete dialog;
}

// stopSimulation
void MainWindow::on_actionStop_triggered()
{
    // implements Simulate|Stop
    if (env->getSimulationState() == Qtenv::SIM_RUNNING || env->getSimulationState() == Qtenv::SIM_BUSY) {
        // "opp_stopsimulation" just *asks* the simulation to stop, causing it to return
        // from the "opp_run" command.
        // "setGuiForRunmode notrunning" will be called after "opp_run" has returned.
        env->setStopSimulationFlag();
    }

    getQtenv()->getAnimator()->hurry();

    // this proc doubles as "stop layouting", when in graphical module inspectors
    // TODO
    // global stoplayouting
    // set stoplayouting 1
}

// runFast
void MainWindow::on_actionFastRun_triggered()
{
    // implements Simulate|Fast Run
    runSimulation(FAST);
}

// runExpress
void MainWindow::on_actionExpressRun_triggered()
{
    // implements Simulate|Express Run
    runSimulation(EXPRESS);
}

// runUntil
void MainWindow::on_actionRunUntil_triggered()
{
    // implements Simulate|Run until...
// TODO
    if (!networkReady())
        return;

//    set time ""
//    set event ""
//    set msg ""
//    set mode ""    ;# will be set to Normal, Fast or Express

//    set ok [runUntilDialog time event msg mode]
//    if {$ok==0} return

//    if {$mode=="Normal"} {
//        set mode "normal"
//    } elseif {$mode=="Fast"} {
//        set mode "fast"
//    } elseif {$mode=="Express"} {
//        set mode "express"
//    } else {
//        set mode "normal"
//    }

//    set untilmode "until_on"
//    if {$time=="" && $event=="" && $msg==""} {set until_on "until_off"}

//    if [isRunning] {
//        if [catch {
//            setGuiForRunmode $mode "" $untilmode
//            opp_set_run_mode $mode
//            opp_set_run_until $time $event $msg
//        } err] {
//            messagebox {Error} "Error: $err" error ok
//        }
//    } else {
//        if {![networkReady]} {return}
//        if [catch {
//            setGuiForRunmode $mode "" $untilmode
//            opp_run $mode $time $event $msg
//        } err] {
//            messagebox {Error} "Error: $err" error ok
//        }
//        setGuiForRunmode notrunning
    //    }
}

void MainWindow::onSliderValueChanged(int value)
{
    getQtenv()->opt->animationSpeed = value / 100.0;
}

void MainWindow::updateStatusDisplay()
{
    updateSimtimeDisplay();

    if (true) {  // TODO $config(display-statusdetails)
        int runMode = env->getSimulationRunMode();
        if (env->getSimulationState() == Qtenv::SIM_RUNNING &&
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
    ui->labelMessageStats->setText("Msg stats: " + QString::number(getSimulation()->getFES()->getLength())
            +" scheduled / " + QString::number(cMessage::getLiveMessageCount())
            +" existing / " + QString::number(cMessage::getTotalMessageCount()) + " created");
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
    if (state == Qtenv::SIM_NEW || state == Qtenv::SIM_READY || state == Qtenv::SIM_RUNNING) {
        modptr = getSimulation()->guessNextModule();
        msgptr = getSimulation()->guessNextEvent();
    }

    if (msgptr) {
        int objId = getObjectId(msgptr);
        ui->labelDisplay1->setText(QString("Next: ") + msgptr->getName() + " (" + msgptr->getClassName()
                +", id=" + (objId == -1 ? "" : QString::number(objId)) + ")");
        ui->labelDisplay3->setText(QString("At: last event + ")
                +(getSimulation()->guessNextEvent()->getArrivalTime() - getSimulation()->getSimTime()).str().c_str()
                +"s");
    }
    else {
        ui->labelDisplay1->setText("Next: n/a");
        ui->labelDisplay3->setText("At: n/a");
    }

    if (modptr)
        ui->labelDisplay2->setText(QString("In: ") + modptr->getFullPath().c_str()
                +" (" + getObjectShortTypeName(modptr) + ", id="
                +getObjectShortTypeName(modptr) + ")");
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
    if (dynamic_cast<cComponent *>(object)) {
        try {
            return static_cast<cComponent *>(object)->getComponentType()->getName();
        }
        catch (std::exception& e) {
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
            if (className[0] == 'o' && opp_stringbeginswith(className, "omnetpp::"))
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

    // TODO
//    if {$configname==""} {set configName "n/a"}
//    if {$network==""} {set network "(no network)"}
    ui->labelConfigName->setText(QString(configname) + " #" + QString::number(env->getConfigEx()->getActiveRunNumber()) + ": " + network);
}

QWidget *MainWindow::getMainInspectorArea()
{
    return ui->mainArea;
}

QWidget *MainWindow::getObjectTreeArea()
{
    return ui->treeView;
}

QWidget *MainWindow::getObjectInspectorArea()
{
    return ui->objectInspector;
}

QWidget *MainWindow::getLogInspectorArea()
{
    return ui->logInspector;
}

QWidget *MainWindow::getTimeLineArea()
{
    return ui->timeLine;
}

void MainWindow::excludeMessageFromAnimation(cObject *msg)
{
    const char *cl = getObjectShortTypeName(msg);

    // TODO must be reviewed
    QString namePattern = msg->getFullName();
    namePattern.replace(QRegExp("[0-9]+"), "*");
    namePattern.replace(QRegExp("(?!\\c)"), "?");  // sanitize: replace nonprintable chars with '?'
    namePattern.replace(QRegExp("[\"\\]"), "?");  // sanitize: replace quotes (") and backslashes with '?'
    if (namePattern.contains(' '))  // must be quoted if contains spaces
        namePattern = "\"" + namePattern + "\"";

    QString filters = env->getSilentEventFilters();
    filters = filters.trimmed();
    if (!filters.isEmpty())
        filters += "\n";
    filters += namePattern +" and className(" + cl +")\n";
    env->setSilentEventFilters(filters.toStdString().c_str());

    env->refreshInspectors();
}

void MainWindow::runUntilMsg(cMessage *msg, int runMode)
{
    if (!networkReady())
        return;

    // mode must be "normal", "fast" or "express"
    if (isRunning()) {
        setGuiForRunmode(runModeToMode(runMode), nullptr, true);
        env->setSimulationRunMode(runMode);
        env->setSimulationRunUntil(SIMTIME_ZERO, 0, msg);
    }
    else {
        setGuiForRunmode(runModeToMode(runMode), nullptr, true);
        env->runSimulation(runMode, SIMTIME_ZERO, 0, msg);
        setGuiForRunmode(NOT_RUNNING);
    }
}

// opp_set_run_until_module
void MainWindow::setRunUntilModule(Inspector *insp)
{
    if (insp == nullptr) {
        env->setSimulationRunUntilModule(nullptr);
        return;
    }

    cObject *object = insp->getObject();
    if (!object) {
        // TODO log "inspector has no object"
        return;
    }

    cModule *mod = dynamic_cast<cModule *>(object);
    if (!mod) {
        // TODO log "object is not a module"
        return;
    }

    env->setSimulationRunUntilModule(mod);
}

bool MainWindow::networkReady()
{
    if (!networkPresent())
        return false;

    if (isSimulationOk())
        return true;
    else {
        int ans = QMessageBox::warning(this, tr("Warning"), tr("Cannot continue this simulation. Rebuild network?"),
                    QMessageBox::Yes | QMessageBox::No);
        if (ans == QMessageBox::Yes) {
            on_actionRebuildNetwork_triggered();
            return isSimulationOk();
        }
        else
            return false;
    }

    return true;
}

bool MainWindow::isSimulationOk()
{
    int state = env->getSimulationState();
    return state == Qtenv::SIM_NEW || state == Qtenv::SIM_RUNNING || state == Qtenv::SIM_READY;
}

bool MainWindow::networkPresent()
{
    if (!getSimulation()->getSystemModule()) {
        QMessageBox::warning(this, tr("Error"), tr("No network has been set up yet."), QMessageBox::Ok);
        return false;
    }
    return true;
}

void MainWindow::saveSplitter(QString prefName, QSplitter *splitter)
{
    QList<QVariant> sizes;
    sizes.clear();
    sizes.append(splitter->sizes()[0]);
    sizes.append(splitter->sizes()[1]);
    env->setPref(prefName, sizes);
}

void MainWindow::storeGeometry()
{
    env->setPref("mainwindow-geom", geometry());

    if(ui->actionTimeline->isChecked())
        saveSplitter("mainwin-main-splittersizes", ui->mainSplitter);
    else
    {
        QList<QVariant> sizes;
        sizes.clear();
        sizes.append(timeLineSize[0]);
        sizes.append(timeLineSize[1]);
        env->setPref("mainwin-main-splittersizes", sizes);
    }

    saveSplitter("mainwin-bottom-splittersizes", ui->splitter_3);
    saveSplitter("mainwin-left-splittersizes", ui->splitter);

    if (ui->splitter_2->orientation() == Qt::Horizontal) {
        saveSplitter("mainwin-right-splittersizes-horiz", ui->splitter_2);
        env->setPref("mainwin-right-splitter-orientation", "horiz");
    } else {
        saveSplitter("mainwin-right-splittersizes-vert", ui->splitter_2);
        env->setPref("mainwin-right-splitter-orientation", "vert");
    }
}

void MainWindow::restoreSplitter(QString prefName, QSplitter *splitter)
{
    QList<QVariant> sizes;
    sizes = env->getPref(prefName).value<QList<QVariant >>();
    if (sizes.size() >= 2) {
        QList<int> intSizes;
        intSizes.append(sizes[0].toInt());
        intSizes.append(sizes[1].toInt());
        splitter->setSizes(intSizes);
    }
}

void MainWindow::restoreGeometry()
{
    QRect geom = env->getPref("mainwindow-geom").toRect();
    if (geom.isValid()) setGeometry(geom);

    // set timeline initial size
    ui->mainSplitter->setSizes(QList<int>({100, ui->mainSplitter->height() - 100}));

    restoreSplitter("mainwin-main-splittersizes", ui->mainSplitter);
    restoreSplitter("mainwin-bottom-splittersizes", ui->splitter_3);
    restoreSplitter("mainwin-left-splittersizes", ui->splitter);

    QVariant orient = env->getPref("mainwin-right-splitter-orientation");

    if (orient == "horiz") {
        ui->splitter_2->setOrientation(Qt::Horizontal);
        restoreSplitter("mainwin-right-splittersizes-horiz", ui->splitter_2);
    } else if (orient == "vert") {
        ui->splitter_2->setOrientation(Qt::Vertical);
        restoreSplitter("mainwin-right-splittersizes-vert", ui->splitter_2);
    }

    // TODO save right panel orientation and sizes
}

int MainWindow::modeToRunMode(eMode mode)
{
    switch (mode) {
        case NOT_RUNNING:
        case STEP:
            return -1;

        case NORMAL:
            return Qtenv::RUNMODE_NORMAL;

        case FAST:
            return Qtenv::RUNMODE_FAST;

        case EXPRESS:
            return Qtenv::RUNMODE_EXPRESS;
    }

    return -1;
}

MainWindow::eMode MainWindow::runModeToMode(int runMode)
{
    switch (runMode) {
        case Qtenv::RUNMODE_NORMAL:
            return NORMAL;

        case Qtenv::RUNMODE_FAST:
            return FAST;

        case Qtenv::RUNMODE_EXPRESS:
            return EXPRESS;
    }

    return NORMAL;
}

// rebuild
void MainWindow::on_actionRebuildNetwork_triggered()
{
    // implements Simulate|Rebuild

    if (checkRunning())
        return;

    if (!networkPresent())
        return;

    busy("Rebuilding network...");
    // TODO inspectorList:addAll 1
    env->rebuildSim();
    // TODO reflectRecordEventlog
    busy();
}

void MainWindow::busy(QString msg)
{
    if (!msg.isEmpty()) {
        ui->statusBar->showMessage(msg);
        this->setCursor(QCursor(Qt::WaitCursor));
    }
    else {
        ui->statusBar->showMessage("Ready");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    InspectorUtil::preferencesDialog();
    getQtenv()->refreshInspectors();
}

void MainWindow::on_actionTimeline_toggled(bool isSunken)
{
    if(!isSunken)
        timeLineSize = ui->mainSplitter->sizes();

    ui->timeLine->setVisible(isSunken);
    getQtenv()->setPref("display-timeline", isSunken);
}

} // namespace qtenv
} // namespace omnetpp
