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

#include <QMessageBox>
#include <QCloseEvent>
#include <QStyledItemDelegate>
#include <QTextLayout>
#include <QTimer>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qtenv.h"
#include "runselectiondialog.h"
#include "treeitemmodel.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cfutureeventset.h"
#include "inspector.h"
#include "common/stringutil.h"
#include "common/ver.h"
#include "stopdialog.h"
#include "inspectorutil.h"
#include "genericobjectinspector.h"
#include "loginspector.h"
#include "timelineinspector.h"
#include "moduleinspector.h"
#include "timelinegraphicsview.h"
#include "rununtildialog.h"
#include "filteredobjectlistdialog.h"
#include "comboselectiondialog.h"
#include "fileeditor.h"

#include <QDebug>

using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {

QString MainWindow::aboutText = "OMNeT++/OMNEST\nDiscrete Event Simulation Framework\n\n(C) 2015-2016 OpenSim Ltd.\n\
        Release: " + QString(OMNETPP_RELEASE) + ", build: " + OMNETPP_BUILDID + "\n" + OMNETPP_EDITION + "\n\n"
        "Qtenv was compiled with Qt " + QT_VERSION_STR + ", is running with Qt " + qVersion() + "\n\n"
        "NO WARRANTY -- see license for details.";

MainWindow::MainWindow(Qtenv *env, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    env(env),
    filteredObjectListDialog(nullptr)
{
    ui->setupUi(this);

    stopDialog = new StopDialog(this);
    fileEditor = new FileEditor(this);

    slider = new QSlider();
    slider->setMinimum(50);
    slider->setMaximum(500);
    slider->setOrientation(Qt::Orientation::Horizontal);
    slider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    slider->setValue(getQtenv()->opt->animationSpeed * 100);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    ui->mainToolBar->addWidget(slider);

    // add current event status
    simTimeLabel = new QLabel();
    simTimeLabel->setFrameStyle(ui->nextModuleLabel->frameStyle());
    simTimeLabel->setStyleSheet("background-color: palette(base); border: 1px solid palette(mid); font-size: 16px;");
    eventNumLabel = new QLabel();
    eventNumLabel->setFrameStyle(ui->nextModuleLabel->frameStyle());
    eventNumLabel->setStyleSheet("background-color: palette(base); border: 1px solid palette(mid); font-size: 16px;");
    eventNumLabel->setAlignment(Qt::AlignRight);

    QHBoxLayout *l = new QHBoxLayout();
    l->addStretch(0);
    l->addWidget(eventNumLabel, 1);
    eventNumLabel->setMaximumWidth(100);
    eventNumLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
    l->addWidget(simTimeLabel, 1);
    simTimeLabel->setMaximumWidth(200);
    simTimeLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

    QWidget *w = new QWidget();
    w->setLayout(l);
    ui->mainToolBar->addWidget(w);

    connect(getQtenv(), SIGNAL(animationSpeedChanged(float)), this, SLOT(onAnimationSpeedChanged(float)));

    // if we trigger the action here directly, it will block the initialization
    // this way the main window will be shown before the setup dialog
    // because the timer event is processed in the event loop
    QTimer::singleShot(0, this, SLOT(initialSetUpConfiguration()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fileEditor;
    delete stopDialog;
    delete simTimeLabel;
    delete eventNumLabel;
    if(filteredObjectListDialog)
        delete filteredObjectListDialog;
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

void MainWindow::setGuiForRunmode(eMode mode, bool untilMode)
{
    ui->actionOneStep->setChecked(false);
    ui->actionRun->setChecked(false);
    ui->actionFastRun->setChecked(false);
    ui->actionExpressRun->setChecked(false);

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
            break;

        case NOT_RUNNING:
            ui->actionRunUntil->setChecked(false);
            break;
    }

    if (mode == EXPRESS)
        showStopDialog();
    else
        closeStopDialog();

    ui->actionRunUntil->setChecked(untilMode);
}

void MainWindow::showStopDialog()
{
    stopDialog->show();
    setEnabled(false);
    stopDialog->setEnabled(true);
    QApplication::processEvents(); // so the dialog will show up immediately
}

void MainWindow::closeStopDialog()
{
    stopDialog->close();
    setEnabled(true);
}

void MainWindow::enterLayoutingMode()
{
    ASSERT(disabledForLayouting.empty());

    for (auto c : findChildren<QObject*>()) {
        auto action = dynamic_cast<QAction*>(c);
        if (action && action->isEnabled() && action != ui->actionStop) {
            disabledForLayouting.insert(action);
            action->setEnabled(false);
        }

        auto inspector = dynamic_cast<Inspector*>(c);
        // ModuleInspectors themselves aren't disabled, so the STOP button
        // will still work them and any other actions are disabled anyways
        if (inspector && inspector->isEnabled() && !dynamic_cast<ModuleInspector*>(inspector)) {
            disabledForLayouting.insert(inspector);
            inspector->setEnabled(false);
        }
    }
}

void MainWindow::exitLayoutingMode()
{
    for (auto c : disabledForLayouting) {
        auto action = dynamic_cast<QAction*>(c);
        if (action)
            action->setEnabled(true);

        auto inspector = dynamic_cast<Inspector*>(c);
        if (inspector)
            inspector->setEnabled(true);
    }

    disabledForLayouting.clear();
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

    if(filteredObjectListDialog)
        filteredObjectListDialog->close();

    close();
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // will only pop up the dialog in on_actionQuit_triggered
    // if the sim is running and has not been asked yet to stop
    if (isRunning() && !env->getStopSimulationFlag()
            && !on_actionQuit_triggered())
        event->ignore();
}

void MainWindow::initialSetUpConfiguration()
{
    setupConfiguration(false);
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
        closeStopDialog();
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
    setupConfiguration(true);
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
    closeStopDialog();

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
    ui->actionRunUntil->setChecked(false);
    if (!networkReady())
        return;

    RunUntilDialog runUntilDialog;
    if(!runUntilDialog.exec())
        return;

    Qtenv::eRunMode mode = runUntilDialog.getMode();
    simtime_t time = runUntilDialog.getTime();
    eventnumber_t event = runUntilDialog.getEventNumber();
    cMessage *msg = static_cast<cMessage*>(runUntilDialog.getMessage());

    bool untilMode = time.dbl() != 0 || event != 0 || strlen(msg->getDisplayString());
    if (isRunning())
    {
        setGuiForRunmode(runModeToMode(mode), untilMode);
        getQtenv()->setSimulationRunMode(mode);
        getQtenv()->setSimulationRunUntil(time, event, msg);
    } else
    {
        if(!networkReady())
            return;

        setGuiForRunmode(runModeToMode(mode), untilMode);
        getQtenv()->runSimulation(mode, time, event, msg);
        setGuiForRunmode(NOT_RUNNING);
        closeStopDialog();
    }
}

void MainWindow::onSliderValueChanged(int value) {
    getQtenv()->setAnimationSpeed(value / 100.0);
}

void MainWindow::updateStatusDisplay()
{
    updateSimtimeDisplay();

    if (showStatusDetails) {
        if (env->getSimulationState() == Qtenv::SIM_RUNNING
                && (env->getSimulationRunMode() == Qtenv::RUNMODE_FAST
                    || env->getSimulationRunMode() == Qtenv::RUNMODE_EXPRESS))
            updatePerformanceDisplay();
        else
            updateNextEventDisplay();
    }
}

void MainWindow::updateSimtimeDisplay()
{
    eventNumLabel->setText("#" + QString::number(getSimulation()->getEventNumber()));
    simTimeLabel->setText(QString(getSimulation()->getSimTime().str().c_str()) + "s");
    ui->labelMessageStats->setText("Msg stats: " + QString::number(getSimulation()->getFES()->getLength())
            +" scheduled / " + QString::number(cMessage::getLiveMessageCount())
            +" existing / " + QString::number(cMessage::getTotalMessageCount()) + " created");
}

void MainWindow::updatePerformanceDisplay()
{
    ui->nextModuleLabel->setText("Simsec/sec: " + QString::number(env->getSpeedometer().getSimSecPerSec()));
    ui->nextEventLabel->setText("Ev/sec: " + QString::number(env->getSpeedometer().getEventsPerSec()));
    ui->nextTimeLabel->setText("Ev/simsec: " + QString::number(env->getSpeedometer().getEventsPerSimSec()));
}

void MainWindow::updateNextEventDisplay()
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
        simtime_t nextTime = getSimulation()->guessNextEvent()->getArrivalTime();
        simtime_t diff = nextTime - getSimulation()->getSimTime();
        ui->nextEventLabel->setText(QString("Next: ") + msgptr->getName() + " (" + msgptr->getClassName()
                +", id=" + (objId == -1 ? "" : QString::number(objId)) + ")");
        ui->nextTimeLabel->setText(QString("At: ") + nextTime.str().c_str() + "s (+" + diff.str().c_str() + "s)");
    }
    else {
        ui->nextEventLabel->setText("Next: n/a");
        ui->nextTimeLabel->setText("At: n/a");
    }

    if (modptr)
        ui->nextModuleLabel->setText(QString("In: ") + modptr->getFullPath().c_str()
                +" (" + getObjectShortTypeName(modptr) + ", id="
                + QString::number(modptr->getId()) + ")");
    else
        ui->nextModuleLabel->setText("In: n/a");
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

QAction *MainWindow::getStopAction()
{
    return ui->actionStop;
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

void MainWindow::setupConfiguration(bool forceDialog)
{
    // implements File|Set Up a Configuration...
    if (checkRunning())
        return;

    std::string config;
    int run;

    // these are what were specified in either the omnetpp.ini file or as a command line argument
    std::string defaultConfig = getQtenv()->opt->defaultConfig;
    int defaultRun = getQtenv()->opt->defaultRun;

    auto qtenvConf = getQtenv()->getConfigEx();

    // if a default isn't given but there is only one config, use that
    if (defaultConfig.empty() && qtenvConf->getConfigNames().size() == 1)
        defaultConfig = qtenvConf->getConfigNames().front();

    // if a config is known, and a default run isn't given but there is
    // only one run in the config, use that
    if (!defaultConfig.empty() && defaultRun < 0
            && qtenvConf->getNumRunsInConfig(defaultConfig.c_str()) == 1)
        defaultRun = 0;

    // if there's still some doubt, lets ask the user.
    if (forceDialog || defaultConfig.empty() || defaultRun < 0) {
        RunSelectionDialog dialog(defaultConfig, defaultRun, this);
        if (dialog.exec()) {
            config = dialog.getConfigName();
            run = dialog.getRunNumber();
        } else { // the dialog was cancelled
            return;
        }
    } else {
        // a dialog was not forced, and either there is a default config or there is only one
        config = defaultConfig.empty() ? qtenvConf->getConfigNames().front() : defaultConfig;
        // and either there is a default run too, or there is only one run in the config
        run = defaultRun < 0 ? 0 : defaultRun;
    }
    env->newRun(config.c_str(), run);
    reflectRecordEventlog();
}

void MainWindow::runUntilMsg(cMessage *msg, int runMode)
{
    if (!networkReady())
        return;

    // mode must be "normal", "fast" or "express"
    if (isRunning()) {
        setGuiForRunmode(runModeToMode(runMode), true);
        env->setSimulationRunMode(runMode);
        env->setSimulationRunUntil(SIMTIME_ZERO, 0, msg);
    } else {
        setGuiForRunmode(runModeToMode(runMode), true);
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

    getQtenv()->setPref("display-timeline", ui->actionTimeline->isChecked());
    getQtenv()->setPref("display-statusdetails", showStatusDetails);
}

void MainWindow::restoreSplitter(QString prefName, QSplitter *splitter, const QList<int> &defaultSizes)
{
    QList<QVariant> sizes = env->getPref(prefName).value<QList<QVariant >>();
    QList<int> intSizes;

    if (sizes.size() >= 2) {
        intSizes.append(sizes[0].toInt());
        intSizes.append(sizes[1].toInt());
    } else {
        intSizes = defaultSizes;
    }

    if (intSizes.size() >= 2) {
        splitter->setSizes(intSizes);
    }
}

void MainWindow::restoreGeometry()
{
    QRect geom = env->getPref("mainwindow-geom").toRect();
    if (geom.isValid()) setGeometry(geom);

    // set timeline initial size if there is no qtenv.ini
    QList<int> sizes;
    TimeLineInspector *timeLineInsp = static_cast<TimeLineInspector*>(ui->timeLine->children()[0]);
    connect(ui->mainSplitter, SIGNAL(splitterMoved(int,int)), this, SLOT(onSplitterMoved(int, int)));
    double timeLineHeight = timeLineInsp->getInitHeight();
    sizes.append(timeLineHeight);
    sizes.append(ui->mainSplitter->height() - timeLineHeight);
    defaultTimeLineSize = sizes;

    QList<int> defaultSizes;
    defaultSizes.append(ui->splitter_3->width() / 3);
    defaultSizes.append(ui->splitter_3->width() / 3 * 2);

    restoreSplitter("mainwin-main-splittersizes", ui->mainSplitter, sizes);
    restoreSplitter("mainwin-bottom-splittersizes", ui->splitter_3, defaultSizes);
    restoreSplitter("mainwin-left-splittersizes", ui->splitter);

    QVariant orient = env->getPref("mainwin-right-splitter-orientation");

    if (orient == "horiz") {
        ui->splitter_2->setOrientation(Qt::Horizontal);
        ui->actionHorizontalLayout->setChecked(true);
        restoreSplitter("mainwin-right-splittersizes-horiz", ui->splitter_2, defaultSizes);
    } else if (orient == "vert") {
        ui->splitter_2->setOrientation(Qt::Vertical);
        ui->actionVerticalLayout->setChecked(true);
        restoreSplitter("mainwin-right-splittersizes-vert", ui->splitter_2);
    }

    // initialize timeline
    bool isSunken = getQtenv()->getPref("display-timeline", true).value<bool>();
    timeLineSize = ui->mainSplitter->sizes();
    on_actionTimeline_toggled(isSunken);
    ui->actionTimeline->setChecked(isSunken);

    // initialize status bar
    showStatusDetails = getQtenv()->getPref("display-statusdetails", true).value<bool>();
    ui->nextEventLabel->setVisible(showStatusDetails);
    ui->nextModuleLabel->setVisible(showStatusDetails);
    ui->nextTimeLabel->setVisible(showStatusDetails);
}

QSize MainWindow::sizeHint() const
{
    return QSize(1200, 600);
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Polish || event->type() == QEvent::PolishRequest) {
        restoreGeometry();
    }
    return QMainWindow::event(event);
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
    reflectRecordEventlog();
    busy();
}

// XXX The message is ignored now that the status bar got rid of
void MainWindow::busy(QString msg)
{
    setCursor(QCursor(msg.isEmpty() ? Qt::ArrowCursor : Qt::WaitCursor));
}

void MainWindow::on_actionPreferences_triggered()
{
    InspectorUtil::preferencesDialog();
    getQtenv()->refreshInspectors();
}

void MainWindow::on_actionTimeline_toggled(bool isSunken)
{
    if(!isSunken)
    {
        timeLineSize = ui->mainSplitter->sizes().at(0) == 0 ? defaultTimeLineSize : ui->mainSplitter->sizes();
        QList<int> sizes = timeLineSize;
        sizes[0] = 0;
        ui->mainSplitter->setSizes(sizes);
    }
    else
        ui->mainSplitter->setSizes(timeLineSize);


}

void MainWindow::onSplitterMoved(int, int)
{
    // It is needed in case when hide timeline with toolbar button and after
    // is displayed by splitterMoved signal, thus it can be avoided glint.
    if(ui->mainSplitter->sizes().at(0) != 0)
        timeLineSize = defaultTimeLineSize;
    ui->actionTimeline->setChecked(ui->mainSplitter->sizes().at(0) != 0);

}

void MainWindow::onAnimationSpeedChanged(float speed) {
    slider->setValue(speed * 100);
}

void MainWindow::on_actionStatusDetails_triggered()
{
    showStatusDetails = !showStatusDetails;
    ui->nextEventLabel->setVisible(showStatusDetails);
    ui->nextModuleLabel->setVisible(showStatusDetails);
    ui->nextTimeLabel->setVisible(showStatusDetails);
    updateStatusDisplay();
}

void MainWindow::on_actionFindInspectObjects_triggered()
{
    // implements Find/inspect objects...
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    cObject *obj = variant.isValid() ? variant.value<cObject*>() : getQtenv()->getMainObjectInspector()->getObject();
    if(!filteredObjectListDialog)
        filteredObjectListDialog = new FilteredObjectListDialog(obj);
    else
        filteredObjectListDialog->setSearchEdit(obj);
    filteredObjectListDialog->show();
}

// debugNextEvent
void MainWindow::on_actionDebugNextEvent_triggered()
{
    //implements Simulate|Debug next event

    if(isRunning())
        QMessageBox::warning(this, tr("Error"), tr("Simulation is currently running -- please stop it first."),
                QMessageBox::Ok);
    else {
        if (!networkReady())
            return;
        int ans = QMessageBox::warning(this, tr("Confirm"),
                             tr("Trigger debugger breakpoint for the next simulation event?\nNote: If you are not running under a debugger, this will likely result in a crash."),
                QMessageBox::Ok | QMessageBox::Cancel);
        if(ans == QMessageBox::Ok)
        {
           setGuiForRunmode(STEP);
           getSimulation()->requestTrapOnNextEvent();
           on_actionOneStep_triggered();
           setGuiForRunmode(NOT_RUNNING);
        }
    }
}

void MainWindow::on_actionEventlogRecording_triggered()
{
    getQtenv()->setEventlogRecording(!getQtenv()->getEventlogRecording());
}

void MainWindow::reflectRecordEventlog()
{
    ui->actionEventlogRecording->setChecked(getQtenv()->getEventlogRecording());
}

// newNetwork
void MainWindow::on_actionSetUpUnconfiguredNetwork_triggered()
{
    // implements File|New network...

    if(checkRunning())
        return;

    // get list of network names
    QVector<cModuleType*> networks;
    cRegistrationList *types = componentTypes.getInstance();
    for (int i = 0; i < types->size(); i++)
    {
        cModuleType *t = dynamic_cast<cModuleType *>(types->get(i));
        if (t && t->isNetwork())
            networks.push_back(t);
    }
    const char *localPackage = getQtenv()->getLocalPackage().c_str();
    QStringList networkNames;
    QStringList localNetworkNames;
    for(cModuleType *net : networks)
    {
        const char *networkName = net->getName();
        const char *networkQName = net->getFullName();
        char result[100];
        strcpy(result, localPackage);
        strcat(result, ".");
        strcat(result, networkName);
        if(strcmp(result, networkQName) == 0)
            localNetworkNames.push_back(networkName);
        else
            networkNames.push_back(networkQName);
    }

    auto lessThan = [](QString arg1, QString arg2) -> bool
                    {
                        return arg1.toLower() < arg2.toLower();
                    };

    qSort(localNetworkNames.begin(), localNetworkNames.end(), lessThan);
    qSort(networkNames.begin(), networkNames.end(), lessThan);
    networkNames = localNetworkNames << networkNames;

    // pop up dialog, with current network as default
    cModuleType *networkType = getSimulation()->getNetworkType();
    const char *netName = networkType ? networkType->getName() : nullptr;
    ComboSelectionDialog comboDialog(netName, localNetworkNames);
    if(comboDialog.exec() == QDialog::Accepted)
    {
        busy("Setting up network...");
        //TODO
        //inspectorList:addAll 1
        getQtenv()->newNetwork(comboDialog.getSelectedNetName().toStdString().c_str());
        reflectRecordEventlog();
        busy();
    }
}

void MainWindow::on_actionVerticalLayout_triggered(bool checked)
{
    if (ui->splitter_2->orientation() == Qt::Horizontal)
        saveSplitter("mainwin-right-splittersizes-horiz", ui->splitter_2);

    if (checked) { // it just got pressed
        ui->actionHorizontalLayout->setChecked(false);
        ui->splitter_2->setOrientation(Qt::Vertical);
        restoreSplitter("mainwin-right-splittersizes-vert", ui->splitter_2);
    } else { // not letting the user uncheck the action
        ui->actionVerticalLayout->setChecked(true);
    }
}

void MainWindow::on_actionHorizontalLayout_triggered(bool checked)
{
    if (ui->splitter_2->orientation() == Qt::Vertical)
        saveSplitter("mainwin-right-splittersizes-vert", ui->splitter_2);

    if (checked) { // it just got pressed
        ui->actionVerticalLayout->setChecked(false);
        ui->splitter_2->setOrientation(Qt::Horizontal);
        restoreSplitter("mainwin-right-splittersizes-horiz", ui->splitter_2);
    } else { // not letting the user uncheck the action
        ui->actionHorizontalLayout->setChecked(true);
    }
}

void MainWindow::on_actionFlipWindowLayout_triggered()
{
    switch (ui->splitter_2->orientation()) {
    case Qt::Horizontal: ui->actionVerticalLayout->trigger(); break;
    case Qt::Vertical: ui->actionHorizontalLayout->trigger(); break;
    }
}

void MainWindow::on_actionAbout_OMNeT_Qtenv_triggered()
{
    QDialog *about = new QDialog();
    about->setWindowTitle("About OMNeT++/OMNEST");
    QVBoxLayout *layout = new QVBoxLayout();
    about->setLayout(layout);

    QFrame *frame = new QFrame();
    frame->setFrameShape(QFrame::StyledPanel);
    QVBoxLayout *frameLayout = new QVBoxLayout();
    frame->setLayout(frameLayout);

    QLabel *label = new QLabel(aboutText);
    label->setFont(getQtenv()->getBoldFont());
    label->setAlignment(Qt::AlignHCenter);
    frameLayout->addWidget(label);

    layout->addWidget(frame);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), about, SLOT(accept()));
    layout->addWidget(buttonBox);
    about->exec();

    delete layout;
    delete frameLayout;
    delete buttonBox;
    delete about;
}

void MainWindow::on_actionLoadNEDFile_triggered()
{
    QString lastNedFile = getQtenv()->getPref("last-nedfile", ".").value<QString>();

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open NED file"), lastNedFile, tr("NED Files (*.ned);;All files (*)"));

    if(!fileName.isNull())
    {
        getQtenv()->setPref("last-nedfile", fileName);
        getQtenv()->loadNedFile(fileName.toStdString().c_str());

        getQtenv()->refreshInspectors();
    }
}

void MainWindow::on_actionOpenPrimaryIniFile_triggered()
{
    QString fileName = getQtenv()->getIniFileName();
    if(fileName.isEmpty())
    {
        QMessageBox::information(this, tr("Info"), tr("The current configuration manager doesn't use file input."),
                QMessageBox::Ok);
       return;
    }

    fileEditor->setFile(fileName);
    fileEditor->show();
}

int MainWindow::inputBox(const QString &title, const QString &prompt, QString &variable)
{
    QDialog *dialog = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout();
    dialog->setLayout(layout);
    dialog->setWindowTitle(title);

    // Add widget
    layout->addWidget(new QLabel(prompt));
    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setText(variable);
    layout->addWidget(lineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
    dialog->layout()->addWidget(buttonBox);

    int code = dialog->exec();
    variable = lineEdit->text();

    delete dialog;

    return code;
}

// implements File|Create snapshot
void MainWindow::on_actionCreate_Snapshot_triggered()
{
    if(!networkPresent())
        return;

    QString variable = "";

    if(inputBox("Snapshot", "Enter label for current simulation snapshot:", variable) == QDialog::Accepted)
    {
        getQtenv()->createSnapshot(variable.toStdString().c_str());

        QString msg = QString(getQtenv()->getSnapshotFileName()).isEmpty()
                ? "Current state of simulation has been saved."
                : QString("Current state of simulation has been saved into \"") + getQtenv()->getSnapshotFileName() + "\".";

        QMessageBox::information(this, tr("Snapshot created"), msg, QMessageBox::Ok);
    }
}

void MainWindow::on_actionConcludeSimulation_triggered()
{
    // check state is not SIM_RUNNING
    if(checkRunning())
        return;

    // check state is not SIM_NONET
    if(!networkPresent())
        return;

    // check state is not SIM_FINISHCALLED
    if(getQtenv()->getSimulationState() == Qtenv::SIM_FINISHCALLED)
    {
       QMessageBox::information(this, tr("Error"), tr("finish() has been invoked already."), QMessageBox::Ok);
       return;
    }

    // check state is not SIM_ERROR
    if(getQtenv()->getSimulationState() == Qtenv::SIM_ERROR)
    {
       QMessageBox::StandardButton ans =
               QMessageBox::question(this, tr("Warning"),
                                    "Simulation was stopped with error, calling finish() might produce unexpected results. Proceed anyway?",
                                    QMessageBox::Yes | QMessageBox::No);

       if(ans == QMessageBox::No)
           return;
    }
    else
    {
       QMessageBox::StandardButton ans =
               QMessageBox::question(this, tr("Question"),
                                     "Do you want to conclude this simulation run and invoke finish() on all modules?",
                                     QMessageBox::Yes | QMessageBox::No);

       if(ans == QMessageBox::No)
           return;
    }

    busy("Invoking finish() on all modules...");
    getQtenv()->finishSimulation();
    busy();
}

// inspectSystemModule
void MainWindow::on_actionNetwork_triggered()
{
    // implements Inspect|Toplevel modules...
    if(!networkPresent())
        return;

    getQtenv()->inspect(getSimulation()->getSystemModule(), INSP_DEFAULT, true);
}

// inspectMessageQueue
void MainWindow::on_actionScheduledEvents_triggered()
{
    // implements Inspect|Message queue...
    getQtenv()->inspect(getSimulation()->getFES(), INSP_DEFAULT, true);
}

// inspectSimulation
void MainWindow::on_actionSimulation_triggered()
{
    // implements Inspect|Simulation...
    getQtenv()->inspect(getSimulation(), INSP_DEFAULT, true);
}

// inspectComponentTypes
void MainWindow::on_actionNEDComponentTypes_triggered()
{
    getQtenv()->inspect(componentTypes.getInstance(), INSP_DEFAULT, true);
}

// inspectClasses
void MainWindow::on_actionRegisteredClasses_triggered()
{
    getQtenv()->inspect(classes.getInstance(), INSP_DEFAULT, true);
}

// inspectFunctions
void MainWindow::on_actionNED_Functions_triggered()
{
    getQtenv()->inspect(nedFunctions.getInstance(), INSP_DEFAULT, true);
}

// inspectEnums
void MainWindow::on_actionRegistered_Enums_triggered()
{
    getQtenv()->inspect(enums.getInstance(), INSP_DEFAULT, true);
}

// inspectConfigEntries
void MainWindow::on_actionSupportedConfigurationOption_triggered()
{
    getQtenv()->inspect(configOptions.getInstance(), INSP_DEFAULT, true);
}

// inspectBypointer
void MainWindow::on_actionInspectByPointer_triggered()
{
    // implements Inspect|By pointer...
    QString pointer = "ptr";
    int ok = inputBox("Inspect by pointer...", "Enter pointer (invalid pointer may cause segmentation fault!):", pointer);
    if(ok == QDialog::Accepted)
        getQtenv()->inspect(strToPtr(pointer.toStdString().c_str()), INSP_DEFAULT, true);
}

} // namespace qtenv
} // namespace omnetpp
