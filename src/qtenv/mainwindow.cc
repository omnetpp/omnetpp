//==========================================================================
//  MAINWINDOW.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <algorithm>

#include <QtWidgets/QMessageBox>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QToolTip>
#include <QtWidgets/QStyledItemDelegate>
#include <QtGui/QTextLayout>
#include <QtCore/QTimer>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QCheckBox>
#include <QtCore/QDebug>
#include <QtWidgets/QToolButton>

#include "omnetpp/csimplemodule.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cscheduler.h"
#include "common/stringutil.h"
#include "common/ver.h"

#include "runselectiondialog.h"
#include "inspector.h"
#include "stopdialog.h"
#include "inspectorutil.h"
#include "genericobjectinspector.h"
#include "loginspector.h"
#include "timelineinspector.h"
#include "moduleinspector.h"
#include "timelinegraphicsview.h"
#include "rununtildialog.h"
#include "findobjectsdialog.h"
#include "comboselectiondialog.h"
#include "fileeditor.h"
#include "animationcontrollerdialog.h"
#include "displayupdatecontroller.h"
#include "videorecordingdialog.h"
#include "qtutil.h"

#define emit

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace qtenv {

QString MainWindow::aboutText = "OMNeT++/OMNEST\nDiscrete Event Simulation Framework\n\n(C) 2015-2022 OpenSim Ltd.\n\
        Release: " + QString(OMNETPP_RELEASE) + ", build: " + OMNETPP_BUILDID + "\n" + OMNETPP_EDITION + "\n\n"
        "Qtenv was compiled with Qt " + QT_VERSION_STR + ", is running with Qt " + qVersion() + "\n\n"
        "NO WARRANTY -- see license for details.";

MainWindow::MainWindow(Qtenv *env, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    env(env)
{
    ui->setupUi(this);

    stopDialog = new StopDialog(this);
    fileEditor = new FileEditor(this);

    // This will hold the toolbar itself, the animation speed
    // slider (the slider can't be allowed to collapse, since it
    // can't be inserted into a menu), then a stretch, and
    // the two event labels at the right.
    auto toolBarLayout = new QHBoxLayout(ui->toolBarArea);
    toolBarLayout->setMargin(0);

    // clearing the hack notification, so we won't be caught
    ui->toolBarArea->setWhatsThis("");

    // This is a HACK, but this will reparent the main toolbar
    // into this widget without any problems.
    // It is necessary to make the event labels always visible and
    // start collapsing the action icons when the window becomes
    // too narrow instead of the more important labels on the toolbar.
    toolBarLayout->addWidget(ui->mainToolBar);

    // This is the animation playback speed slider on the toolbar.
    // It is exponential, see the playbackSpeedToSliderValue and
    // sliderValueToPlaybackSpeed functions for the mapping.
    // A simple snapping mechanism is also implemented in
    // onSliderValueChanged to make selecting 1.0 easier
    slider = new QSlider();
    slider->setOrientation(Qt::Orientation::Horizontal);
    slider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    slider->setTracking(true);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setMinimumWidth(100);
    slider->setMaximumHeight(ui->mainToolBar->height());
    toolBarLayout->addWidget(slider);
    // this will set it up with the current limits
    setGuiForRunmode(env->getSimulationRunMode());

    // add current event status
    simTimeLabel = new QLabel();
    simTimeLabel->setToolTip("Current simulation time");
    simTimeLabel->setFrameStyle(ui->nextModuleLabel->frameStyle());
    simTimeLabel->setObjectName("simTimeLabel");
    simTimeLabel->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(simTimeLabel, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onSimTimeLabelContextMenuRequested(QPoint)));

    eventNumLabel = new QLabel();
    eventNumLabel->setToolTip("Event number");
    eventNumLabel->setFrameStyle(ui->nextModuleLabel->frameStyle());
    eventNumLabel->setAlignment(Qt::Alignment(Qt::AlignVCenter | Qt::AlignRight));
    eventNumLabel->setObjectName("eventNumLabel");
    eventNumLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(eventNumLabel, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onEventNumLabelContextMenuRequested(QPoint)));

    // this little widget will add a small margin above the two labels
    QWidget *labelsContainer = new QWidget();
    QHBoxLayout *labelsLayout = new QHBoxLayout();
    labelsLayout->setMargin(0);
    labelsContainer->setLayout(labelsLayout);

    labelsLayout->addWidget(eventNumLabel);
    eventNumLabel->setMinimumWidth(100);
    eventNumLabel->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelsLayout->addWidget(simTimeLabel);
    simTimeLabel->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    auto margin = centralWidget()->layout()->contentsMargins().right();
    labelsContainer->setContentsMargins(0, margin, 0, 0);

    toolBarLayout->addStretch(1);
    toolBarLayout->addWidget(labelsContainer);

    connect(env->getDisplayUpdateController(), &DisplayUpdateController::playbackSpeedChanged,
            this, &MainWindow::updateSpeedSlider);

    adjustSize();

    ui->actionDebugOnErrors->setChecked(getQtenv()->debugOnErrors);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fileEditor;
    delete stopDialog;
    delete simTimeLabel;
    delete eventNumLabel;
}

void MainWindow::onSimTimeLabelGroupingTriggered()
{
    QVariant variant = static_cast<QAction *>(sender())->data();

    simTimeDigitGrouping = (DigitGrouping)variant.value<int>();
    updateSimTimeLabel();
}

void MainWindow::onSimTimeLabelUnitsTriggered()
{
    simTimeUnits = !simTimeUnits;
    updateSimTimeLabel();
}

void MainWindow::updateSimTimeLabel()
{
    const char *digitSeparator, *decimalSeparator = ".";

    switch (simTimeDigitGrouping) {
        case SPACE:
            digitSeparator = "<small> </small> ";
            decimalSeparator = ".";
            break;
        case COMMA:
            digitSeparator = ",";
            break;
        case APOSTROPHE:
            digitSeparator = "'";
            break;
        case NONE:
            digitSeparator = "";
            break;
    }

    QString simTimeText;
    if (simTimeUnits)
        simTimeText = getSimulation()->getSimTime().format(SimTime::getScaleExp(), decimalSeparator, digitSeparator, true, "<small><font color=grey>", " </font></small>").c_str();
    else
        simTimeText = getSimulation()->getSimTime().format(SimTime::getScaleExp(), decimalSeparator, digitSeparator).c_str();

    simTimeLabel->setText(simTimeText);
}

void MainWindow::onSimTimeLabelContextMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu();

    QActionGroup *actionGroup = new QActionGroup(menu);

    //TODO use this once it compiles everywere:
    //QVector<QPair<QString, int>> elements = {
    //    {"No digit grouping", NONE},
    //    {"Group digits with comma", COMMA}
    //    {"Group digits with apostrophe", APOSTROPHE},
    //    {"Group digits with space", SPACE}
    //};
    QVector<QPair<QString, int>> elements;
    elements.push_back(QPair<QString, int>("&No Digit Grouping", NONE));
    elements.push_back(QPair<QString, int>("Group Digits with &Comma", COMMA));
    elements.push_back(QPair<QString, int>("Group Digits with &Apostrophe", APOSTROPHE));
    elements.push_back(QPair<QString, int>("Group Digits with &Space", SPACE));

    QAction *action;
    for (auto elem : elements) {
        action = menu->addAction(elem.first, this, SLOT(onSimTimeLabelGroupingTriggered()));
        action->setData(QVariant::fromValue(elem.second));
        action->setActionGroup(actionGroup);
        action->setCheckable(true);
        if (simTimeDigitGrouping == elem.second)
            action->setChecked(true);
    }
    menu->addSeparator();

    action = menu->addAction("&Display Units", this, SLOT(onSimTimeLabelUnitsTriggered()));
    action->setCheckable(true);
    action->setChecked(simTimeUnits);

    menu->exec(simTimeLabel->mapToGlobal(pos));

    delete menu;
}

void MainWindow::updateEventNumLabel()
{
    const char *digitSeparator;

    switch (eventNumDigitGrouping) {
        case SPACE:
            digitSeparator = "<small> </small> ";
            break;
        case COMMA:
            digitSeparator = ",";
            break;
        case APOSTROPHE:
            digitSeparator = "'";
            break;
        case NONE:
            digitSeparator = "";
            break;
    }

    int pausePointNumber = env->getPausePointNumber();
    bool paused = pausePointNumber > 0; // XXX: env->isPaused() might not be accurate yet
    bool showNextEvent = env->getDisplayUpdateController()->rightBeforeEvent() && env->getSimulationRunMode() == RUNMODE_NOT_RUNNING;

    eventnumber_t numToShow = getSimulation()->getEventNumber() + (showNextEvent ? 1 : 0);
    const char *prefix = paused ? "in: " : showNextEvent ? "next: " : "last: ";

    QString eventNumText = QString("<font color=grey><small>%1</small>#</font>").arg(prefix)
            + opp_formati64(numToShow, digitSeparator).c_str();

    if (pausePointNumber > 0)
        eventNumText += "/" + QString::number(pausePointNumber);

    eventNumLabel->setText(eventNumText);
    eventNumLabel->setStyleSheet(paused ? "QLabel { background: orange; }" : showNextEvent ? "" : "QLabel { background: palette(alternate-base); }");
}

void MainWindow::onEventNumLabelGroupingTriggered()
{
    QVariant variant = static_cast<QAction *>(sender())->data();

    eventNumDigitGrouping = (DigitGrouping)variant.value<int>();
    updateEventNumLabel();
}

void MainWindow::onEventNumLabelContextMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu();

    QActionGroup *actionGroup = new QActionGroup(menu);

    QVector<QPair<QString, int>> elements = {
        { "&No Digit Grouping", NONE },
        { "Group Digits with &Comma", COMMA },
        { "Group Digits with &Apostrophe", APOSTROPHE },
        { "Group Digits with &Space", SPACE }
    };

    QAction *action;
    for (auto elem : elements) {
        action = menu->addAction(elem.first, this, SLOT(onEventNumLabelGroupingTriggered()));
        action->setData(elem.second);
        action->setActionGroup(actionGroup);
        action->setCheckable(true);
        if (eventNumDigitGrouping == elem.second)
            action->setChecked(true);
    }

    menu->exec(eventNumLabel->mapToGlobal(pos));

    delete menu;
}

bool MainWindow::isRunning()
{
    Qtenv::eState state = (Qtenv::eState)env->getSimulationState();
    return state == Qtenv::SIM_RUNNING || state == Qtenv::SIM_BUSY;
}

void MainWindow::setGuiForRunmode(RunMode runMode, bool untilMode)
{
    ui->actionOneStep->setChecked(runMode == RUNMODE_STEP);
    ui->actionRun->setChecked(runMode == RUNMODE_NORMAL);
    ui->actionFastRun->setChecked(runMode == RUNMODE_FAST);
    ui->actionExpressRun->setChecked(runMode == RUNMODE_EXPRESS);

    if (runMode == RUNMODE_NOT_RUNNING)
        ui->actionRunUntil->setChecked(false);

    if (runMode == RUNMODE_EXPRESS)
        showStopDialog();
    else
        closeStopDialog();

    auto duc = env->getDisplayUpdateController();
    duc->setRunMode(runMode);

    //slider->setEnabled(runMode != RUNMODE_NOT_RUNNING);

    ui->actionRunUntil->setChecked(untilMode);
}

void MainWindow::showStopDialog()
{
    stopDialog->show();
    setEnabled(false);
    stopDialog->setEnabled(true);
    QApplication::processEvents();  // so the dialog will show up immediately
}

void MainWindow::closeStopDialog()
{
    if (stopDialog->isVisible())
        stopDialog->close();

    setEnabled(true);
}

void MainWindow::enterLayoutingMode()
{
    ASSERT(disabledForLayouting.empty());

    for (auto c : findChildren<QObject *>()) {
        auto action = dynamic_cast<QAction *>(c);
        if (action && action->isEnabled() && action != ui->actionStop) {
            disabledForLayouting.insert(action);
            action->setEnabled(false);
        }

        auto inspector = dynamic_cast<Inspector *>(c);
        if (inspector && inspector->isEnabled()) {
            disabledForLayouting.insert(inspector);
            inspector->setEnabled(false);
        }
    }

    for (auto c : ui->toolBarArea->findChildren<QObject *>()) {
        auto widget = dynamic_cast<QWidget*>(c);
        if (widget && widget->isEnabled() && !widget->actions().contains(ui->actionStop)) {
            disabledForLayouting.insert(widget);
            widget->setEnabled(false);
        }
    }
}

void MainWindow::exitLayoutingMode()
{
    for (auto c : disabledForLayouting) {
        if (auto action = dynamic_cast<QAction *>(c))
            action->setEnabled(true);

        if (auto inspector = dynamic_cast<Inspector *>(c))
            inspector->setEnabled(true);

        if (auto widget = dynamic_cast<QWidget *>(c))
            widget->setEnabled(true);
    }

    disabledForLayouting.clear();
}

void MainWindow::updateSpeedSlider()
{
    auto duc = env->getDisplayUpdateController();
    bool blocked = slider->blockSignals(true);
    int min = playbackSpeedToSliderValue(duc->getMinPlaybackSpeed());
    int max = playbackSpeedToSliderValue(duc->getMaxPlaybackSpeed());
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setPageStep((max - min) / 25);
    slider->setSingleStep((max - min) / 50);
    slider->setTickInterval(playbackSpeedToSliderValue(10) - playbackSpeedToSliderValue(1));

    slider->setValue(playbackSpeedToSliderValue(duc->getPlaybackSpeed()));
    slider->setToolTip("Playback speed: " + QString::number(duc->getPlaybackSpeed(), 'f', 2));
    slider->blockSignals(blocked);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Qtenv::eState state = env->getSimulationState();

    enum Action {
        QUIT, // no dialog, no finish, quit
        FINISH_QUIT, // no dialog, finish, quit
        YES_NO_CANCEL, // yes/no/cancel dialog, maybe (maybe finish, quit)
    };

    Action action = QUIT;

    bool confirmExit = env->getPref("confirm-exit", true).toBool();

    // First, deciding what to do
    switch (state) {
        case Qtenv::SIM_NONET: // if there is no network, we simply quit
        case Qtenv::SIM_NEW: // if there's a network, but not started, always exiting
            action = QUIT;
            break;

        case Qtenv::SIM_READY: // during simulation (running or paused), either ask to finish, or just do it
        case Qtenv::SIM_RUNNING:
        case Qtenv::SIM_BUSY:
            action = confirmExit ? YES_NO_CANCEL : FINISH_QUIT;
            break;

        case Qtenv::SIM_TERMINATED: // <- this can't happen by the way, we always finish() after termination right away
        case Qtenv::SIM_FINISHCALLED: // if the simulation ended properly, a simple confirmation or nothing
            action = QUIT;
            break;

        case Qtenv::SIM_ERROR: // after an error, do not ask for confirmation [Andras]
            action = QUIT;
            break;
    }

    // Then acting on our decision:
    switch (action) {
        case QUIT:
            // nothing to do
            break;
        case FINISH_QUIT:
            ASSERT(getQtenv()->getCallFinishOnExitFlag() == false);
            getQtenv()->setCallFinishOnExitFlag(true);
            break;
        case YES_NO_CANCEL: {
            QString question3 = "Do you want to conclude the simulation by invoking finish() before exiting?";
            QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;

            int ans = QMessageBox::question(this, "Question", question3, buttons, QMessageBox::Yes);

            if (ans == QMessageBox::Yes) {
                ASSERT(getQtenv()->getCallFinishOnExitFlag() == false);
                getQtenv()->setCallFinishOnExitFlag(true);
            }

            if (ans == QMessageBox::Cancel) {
                event->ignore();
                return;
            }
            break;
        }
    }

    // finally letting it go, if we got here anyway
    env->setStopSimulationFlag();
    QMainWindow::closeEvent(event);
    emit closed();
}

void MainWindow::runSimulation(RunMode runMode)
{
    if (getQtenv()->isPaused()) {
        getQtenv()->requestQuitFromPausePointEventLoop(runMode);
        return;
    }

    if (isRunning()) {
        setGuiForRunmode(runMode);
        env->setSimulationRunMode(runMode);
        setRunUntilModule();
    }
    else {
        if (!networkReady()) {
            setGuiForRunmode(RUNMODE_NOT_RUNNING);
            return;
        }
        setGuiForRunmode(runMode);
        env->runSimulation(runMode);
        setGuiForRunmode(RUNMODE_NOT_RUNNING);
        closeStopDialog();
    }
}

void MainWindow::stopSimulation()
{
    if (getQtenv()->isPaused())
        getQtenv()->requestQuitFromPausePointEventLoop(RunMode::RUNMODE_NOT_RUNNING);
        // no return here!

    // implements Simulate|Stop
    if (env->getSimulationState() == Qtenv::SIM_RUNNING || env->getSimulationState() == Qtenv::SIM_BUSY) {
        // This just *asks* the simulation to stop, causing it to break from the loop in env->runSimulation().
        // setGuiForRunmode(...NOT_RUNNING) will be called after env->runSimulation() has returned.
        env->setStopSimulationFlag();
    }

    closeStopDialog();
}

void MainWindow::stopOrRunSimulation(RunMode runMode)
{
    if (getQtenv()->isPaused()) {
        getQtenv()->requestQuitFromPausePointEventLoop(runMode);
        return;
    }

    if (env->getSimulationRunMode() == runMode)
        stopSimulation();
    else
        runSimulation(runMode);
}

// newRun
void MainWindow::on_actionSetUpConfiguration_triggered()
{
    if (env->checkRunning())
        return;

    cConfigurationEx *configEx = getQtenv()->getConfigEx();

    // No filter used for subsequent run selections.
    // Note that if invoked this way, we pretty much avoid all possibility of an exception,
    // because the run filter is constant, and the config name is the current one, so it must exist.
    // This, and the fact that Qtenv::displayException is protected, along with Qt not supporting
    // throwing exceptions from slots, justifies the omission of a try-catch block.
    // It would only be an ASSERT(false) or something similar anyway.
    RunSelectionDialog dialog(configEx, configEx->getActiveConfigName(), "", this);
    if (dialog.exec()) {
        busy("Setting up new run...");
        emit setNewNetwork();
        env->newRun(dialog.getConfigName().c_str(), dialog.getRunNumber());
        busy();
        reflectConfigOnUi();
    }
}

// runUntil
void MainWindow::on_actionRunUntil_triggered()
{
    // implements Simulate|Run until...
    if (!networkReady()) {
        setGuiForRunmode(RunMode::RUNMODE_NOT_RUNNING);
        return;
    }

    RunUntilDialog runUntilDialog;
    if (!runUntilDialog.exec()) {
        // popping the button back out
        setGuiForRunmode(getQtenv()->getSimulationRunMode());
        return;
    }

    RunMode runMode = runUntilDialog.getMode();
    simtime_t time = runUntilDialog.getTime();
    eventnumber_t event = runUntilDialog.getEventNumber();
    cMessage *msg = static_cast<cMessage *>(runUntilDialog.getMessage());
    bool stopOnMsgCancel = runUntilDialog.stopOnMsgCancel();

    bool untilMode = time.dbl() != 0 || event != 0 || msg != nullptr;
    if (isRunning()) {
        // XXX: this would cause an assertion failure in DisplayUpdateController
        //setGuiForRunmode(runMode, untilMode);
        getQtenv()->setSimulationRunMode(runMode);
        getQtenv()->setSimulationRunUntil(time, event, msg, stopOnMsgCancel);
        if (getQtenv()->isPaused())
            getQtenv()->requestQuitFromPausePointEventLoop(runMode);
    }
    else {
        if (!networkReady())
            return;

        setGuiForRunmode(runMode, untilMode);
        getQtenv()->runSimulation(runMode, time, event, msg, nullptr, stopOnMsgCancel);
        setGuiForRunmode(RUNMODE_NOT_RUNNING);
        closeStopDialog();
    }
}

void MainWindow::onSliderValueChanged(int value)
{
    double speed = sliderValueToPlaybackSpeed(value);

    // only doing snapping when it is moved with the mouse
    if (slider->isSliderDown()) {
        int pixelThreshold = 2;
        double snapToSpeed = 1.0;

        // finding out the length in pixels the slider handle can travel (approx)
        QStyleOptionSlider opt;
        opt.initFrom(slider);
        int span = slider->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this).width()
                - slider->style()->pixelMetric(QStyle::PM_SliderLength);

        int valueOne = playbackSpeedToSliderValue(snapToSpeed);

        int positionOne = QStyle::sliderPositionFromValue(slider->minimum(), slider->maximum(), valueOne, span);
        int positionCurrent = QStyle::sliderPositionFromValue(slider->minimum(), slider->maximum(), value, span);

        if (std::abs(positionOne - positionCurrent) <= pixelThreshold) {
            slider->setValue(valueOne);
            speed = snapToSpeed;
        }

        if (env->getAnimationSpeed() == 0)
            QToolTip::showText(QCursor::pos(), "Playback speed has no effect: set animation speed!", slider);
    }

    slider->setToolTip(QString::number(speed, 'f', 2));
    env->getDisplayUpdateController()->setPlaybackSpeed(speed);
}

void MainWindow::updateStatusDisplay()
{
    updateSimtimeDisplay();

    if (showStatusDetails) {
        //FIXME: switching between NextEvent Display and Performance Display should be explicit and in the Qtenv class not here! --Andras
        if (env->getSimulationState() == Qtenv::SIM_RUNNING
                && (env->getSimulationRunMode() == RUNMODE_FAST
                    || env->getSimulationRunMode() == RUNMODE_EXPRESS))
            updatePerformanceDisplay();
        else
            updateNextEventDisplay();
    }
}

void MainWindow::updateSimtimeDisplay()
{
    updateEventNumLabel();
    updateSimTimeLabel();
    ui->labelMessageStats->setText("Msg stats: " + QString::number(getSimulation()->getFES()->getLength())
            +" scheduled / " + QString::number(cMessage::getLiveMessageCount())
            +" existing / " + QString::number(cMessage::getTotalMessageCount()) + " created");
}

void MainWindow::updatePerformanceDisplay()
{
    //FIXME FIXME FIXME: should NOT overload the widgets used by updateNextEventDisplay()! Should use SEPARATE widgets not nextModuleLabel/nextEventLabel/nextTimeLabel, and swap the two statusbars when needed! Also: move away tooltip setting, into some initialization code! --Andras

    // Set Status Detail's tooltips
    ui->nextEventLabel->setToolTip("Performance: events processed per second");
    ui->nextModuleLabel->setToolTip("Relative speed: simulated seconds processed per second");
    ui->nextTimeLabel->setToolTip("Event density: events per simulated second");

    // Set Status Detail's texts
    ui->nextModuleLabel->setText("Simsec/sec: " + QString::number(env->getSpeedometer().getSimSecPerSec()));
    ui->nextEventLabel->setText("Ev/sec: " + QString::number(env->getSpeedometer().getEventsPerSec()));
    ui->nextTimeLabel->setText("Ev/simsec: " + QString::number(env->getSpeedometer().getEventsPerSimSec()));
}

void MainWindow::updateNextEventDisplay()
{
    // Set Status Detail's tooltips
    ui->nextEventLabel->setToolTip("Next simulation event");
    ui->nextModuleLabel->setToolTip("Module where next event will occur");
    ui->nextTimeLabel->setToolTip("Simulation time of next event");

    cSimpleModule *modptr = nullptr;
    cEvent *msgptr = nullptr;

    int state = env->getSimulationState();
    if (state == Qtenv::SIM_NEW || state == Qtenv::SIM_READY || state == Qtenv::SIM_RUNNING || state == Qtenv::SIM_BUSY) {
        modptr = getSimulation()->guessNextModule();
        msgptr = getSimulation()->guessNextEvent();
    }

    // Set Status Detail's texts
    if (msgptr) {
        long objId = getObjectId(msgptr);
        simtime_t nextTime = getSimulation()->guessNextEvent()->getArrivalTime();
        simtime_t diff = nextTime - getSimulation()->getSimTime();
        ui->nextEventLabel->setText(QString("Next: ") + msgptr->getFullName() + " (" + msgptr->getClassName()
                +", id=" + (objId == -1 ? "" : QString::number(objId)) + ")");
        ui->nextTimeLabel->setText(QString("At: ") + nextTime.str().c_str() + "s (now+" + diff.str().c_str() + "s)");
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

void MainWindow::updateNetworkRunDisplay()
{
    const char *configName = opp_nulltoempty(env->getConfigEx()->getActiveConfigName());
    const char *network = !getSimulation()->getNetworkType() ? "" : getSimulation()->getNetworkType()->getName();
    std::string scheduler = getSimulation()->getScheduler()->str();
    const char *sep = scheduler.empty() ? "" : " - ";

    // TODO
//    if {$configname==""} {set configName "n/a"}
//    if {$network==""} {set network "(no network)"}
    ui->labelConfigName->setText(QString(configName) + " #" + QString::number(env->getConfigEx()->getActiveRunNumber()) + ": " + network + sep + scheduler.c_str());
}

void MainWindow::excludeMessageFromAnimation(cObject *msg)
{
    QString namePattern = msg->getFullName();
    namePattern.replace(QRegularExpression("[0-9]+"), "*"); // replace indices and such with '*' - hence the "messages like this" meaning
    namePattern.replace(QRegularExpression("[^[:print:]]"), "?");  // sanitize: replace nonprintable chars with '?'
    namePattern.replace(QRegularExpression("[\"\\\\]"), "?");  // sanitize: replace quotes (") and backslashes with '?'
    if (namePattern.contains(' '))  // must be quoted if contains spaces
        namePattern = "\"" + namePattern + "\"";

    QString filters = env->getSilentEventFilters();
    filters = filters.trimmed();
    if (!filters.isEmpty())
        filters += "\n";
    filters += namePattern + " and className =~ " + getObjectShortTypeName(msg, STRIPNAMESPACE_NONE) + "\n";
    env->setSilentEventFilters(filters.toStdString().c_str());

    env->callRefreshInspectors();
}

void MainWindow::runUntilMsg(cMessage *msg, RunMode runMode)
{
    if (!networkReady())
        return;

    // mode must be "normal", "fast" or "express"
    if (isRunning()) {
        setGuiForRunmode(runMode, true);
        env->setSimulationRunMode(runMode);
        env->setSimulationRunUntil(SIMTIME_ZERO, 0, msg);
    }
    else {
        setGuiForRunmode(runMode, true);
        env->runSimulation(runMode, SIMTIME_ZERO, 0, msg);
        setGuiForRunmode(RUNMODE_NOT_RUNNING);
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

    if (ui->actionTimeline->isChecked())
        saveSplitter("mainwin-main-splittersizes", ui->mainSplitter);
    else {
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
    }
    else {
        saveSplitter("mainwin-right-splittersizes-vert", ui->splitter_2);
        env->setPref("mainwin-right-splitter-orientation", "vert");
    }

    getQtenv()->setPref("display-timeline", ui->actionTimeline->isChecked());
    getQtenv()->setPref("display-statusdetails", showStatusDetails);

    getQtenv()->setPref("simtimelabel-digitgrouping", simTimeDigitGrouping);
    getQtenv()->setPref("simtimelabel-units", simTimeUnits);

    getQtenv()->setPref("eventnumlabel-digitgrouping", eventNumDigitGrouping);
}

void MainWindow::restoreSplitter(QString prefName, QSplitter *splitter, const QList<int>& defaultSizes)
{
    QList<QVariant> sizes = env->getPref(prefName).value<QList<QVariant> >();
    QList<int> intSizes;

    if (sizes.size() >= 2) {
        intSizes.append(sizes[0].toInt());
        intSizes.append(sizes[1].toInt());
    }
    else {
        intSizes = defaultSizes;
    }

    if (intSizes.size() >= 2) {
        splitter->setSizes(intSizes);
    }
}

void MainWindow::restoreGeometry()
{
    QRect geom = env->getPref("mainwindow-geom").toRect();
    if (geom.isValid())
        setGeometry(geom);

    // set timeline initial size if there is no qtenv.ini
    QList<int> sizes;
    TimeLineInspector *timeLineInsp = static_cast<TimeLineInspector *>(ui->timeLine->children()[0]);
    connect(ui->mainSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(onSplitterMoved(int, int)));
    double timeLineHeight = timeLineInsp->getInitHeight();
    sizes.append(timeLineHeight);
    sizes.append(ui->mainSplitter->height() - timeLineHeight);
    defaultTimeLineSize = sizes;

    QList<int> defaultSizes;
    defaultSizes.append(ui->splitter_3->width() / 4);
    defaultSizes.append(ui->splitter_3->width() / 4 * 3);

    restoreSplitter("mainwin-main-splittersizes", ui->mainSplitter, sizes);
    restoreSplitter("mainwin-bottom-splittersizes", ui->splitter_3, defaultSizes);
    restoreSplitter("mainwin-left-splittersizes", ui->splitter);

    QVariant orient = env->getPref("mainwin-right-splitter-orientation");

    defaultSizes.clear();

    if (orient == "horiz") {
        ui->splitter_2->setOrientation(Qt::Horizontal);
        ui->actionHorizontalLayout->setChecked(true);
        defaultSizes.append(ui->splitter_2->width() / 4 * 3);
        defaultSizes.append(ui->splitter_2->width() / 4);
        restoreSplitter("mainwin-right-splittersizes-horiz", ui->splitter_2, defaultSizes);
    }
    else if (orient == "vert") {
        ui->splitter_2->setOrientation(Qt::Vertical);
        ui->actionVerticalLayout->setChecked(true);
        defaultSizes.append(ui->splitter_2->height() / 4 * 3);
        defaultSizes.append(ui->splitter_2->height() / 4);
        restoreSplitter("mainwin-right-splittersizes-vert", ui->splitter_2, defaultSizes);
    }

    // initialize timeline
    bool isSunken = getQtenv()->getPref("display-timeline", true).value<bool>();
    timeLineSize = ui->mainSplitter->sizes();
    on_actionTimeline_toggled(isSunken);
    ui->actionTimeline->setChecked(isSunken);

    // initialize status bar
    showStatusDetails = getQtenv()->getPref("display-statusdetails", true).value<bool>();
    ui->actionStatusDetails->setChecked(showStatusDetails);
    ui->nextEventLabel->setVisible(showStatusDetails);
    ui->nextModuleLabel->setVisible(showStatusDetails);
    ui->nextTimeLabel->setVisible(showStatusDetails);

    // initialize simTimeLabel
    simTimeDigitGrouping = (DigitGrouping)getQtenv()->getPref("simtimelabel-digitgrouping", int(SPACE)).value<int>();
    simTimeUnits = getQtenv()->getPref("simtimelabel-units", true).value<bool>();

    // initialize eventNumLabel
    eventNumDigitGrouping = (DigitGrouping)getQtenv()->getPref("eventnumlabel-digitgrouping", int(SPACE)).value<int>();
}

// rebuild
void MainWindow::on_actionRebuildNetwork_triggered()
{
    // implements Simulate|Rebuild

    if (env->checkRunning())
        return;

    if (!networkPresent())
        return;

    busy("Rebuilding network...");
    // TODO inspectorList:addAll 1
    env->rebuildSim();
    reflectConfigOnUi();
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
    getQtenv()->callRefreshInspectors();
}

void MainWindow::on_actionTimeline_toggled(bool isSunken)
{
    if (!isSunken) {
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
    if (ui->mainSplitter->sizes().at(0) != 0)
        timeLineSize = defaultTimeLineSize;
    ui->actionTimeline->setChecked(ui->mainSplitter->sizes().at(0) != 0);
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
    showFindObjectsDialog(variant.value<cObject *>());
}

// debugNextEvent
void MainWindow::on_actionDebugNextEvent_triggered()
{
    // implements Simulate|Debug next event

    if (isRunning()) {
        if (env->isPaused())
            QMessageBox::warning(this, "Error", "Simulation is paused in the middle of an event -- please press 'stop' first.",
                    QMessageBox::Ok);
        else
            QMessageBox::warning(this, "Error", "Simulation is currently running -- please stop it first.",
                    QMessageBox::Ok);
    }
    else {
        if (!networkReady())
            return;

        setGuiForRunmode(RUNMODE_STEP);
        getSimulation()->requestTrapOnNextEvent();
        on_actionOneStep_triggered();
        setGuiForRunmode(RUNMODE_NOT_RUNNING);
    }
}

void MainWindow::on_actionDebugOnErrors_triggered(bool checked)
{
    getQtenv()->debugOnErrors = checked;
}

void MainWindow::on_actionDebugNow_triggered()
{
    if (env->ensureDebugger())
        DEBUG_TRAP; // YOU ASKED FOR A DEBUGGER IN THE MENU -- PLACE YOUR BREAKPOINTS AND CONTINUE EXECUTION
}

void MainWindow::on_actionEventlogRecording_triggered()
{
    getQtenv()->setEventlogRecording(!getQtenv()->getEventlogRecording());
}

void MainWindow::reflectConfigOnUi()
{
    ui->actionEventlogRecording->setChecked(getQtenv()->getEventlogRecording());
    ui->actionDebugOnErrors->setChecked(getQtenv()->debugOnErrors);
}

// XXX why is this in MainWindow, and not in Qtenv?
void MainWindow::configureNetwork()
{
    if (env->checkRunning())
        return;

    // get list of network names
    QVector<cModuleType *> networks;
    cRegistrationList *types = componentTypes.getInstance();
    for (int i = 0; i < types->size(); i++) {
        cModuleType *t = dynamic_cast<cModuleType *>(types->get(i));
        if (t && t->isNetwork())
            networks.push_back(t);
    }
    std::string localPackage = getQtenv()->getLocalPackage();
    QStringList networkNames;
    QStringList localNetworkNames;
    for (cModuleType *net : networks) {
        const char *networkName = net->getName();
        const char *networkQName = net->getFullName();
        char result[100];
        strcpy(result, localPackage.c_str());
        strcat(result, ".");
        strcat(result, networkName);
        if (strcmp(result, networkQName) == 0)
            localNetworkNames.push_back(networkName);
        else
            networkNames.push_back(networkQName);
    }

    auto lessThan = [](QString arg1, QString arg2) -> bool
                    {
                        return arg1.toLower() < arg2.toLower();
                    };

    std::sort(localNetworkNames.begin(), localNetworkNames.end(), lessThan);
    std::sort(networkNames.begin(), networkNames.end(), lessThan);
    networkNames = localNetworkNames << networkNames;

    // pop up dialog, with current network as default
    cModuleType *networkType = getSimulation()->getNetworkType();
    const char *netName = networkType ? networkType->getName() : nullptr;
    ComboSelectionDialog comboDialog(netName, localNetworkNames);
    if (comboDialog.exec() == QDialog::Accepted) {
        busy("Setting up network...");
        //TODO
        //inspectorList:addAll 1
        getQtenv()->newNetwork(comboDialog.getSelectedNetName().toStdString().c_str());
        reflectConfigOnUi();
        busy();
    }
}

// newNetwork
void MainWindow::on_actionSetUpUnconfiguredNetwork_triggered()
{
    // implements File|New network...
    configureNetwork();
}

void MainWindow::on_actionVerticalLayout_triggered(bool checked)
{
    if (ui->splitter_2->orientation() == Qt::Horizontal)
        saveSplitter("mainwin-right-splittersizes-horiz", ui->splitter_2);

    if (checked) {  // it just got pressed
        ui->actionHorizontalLayout->setChecked(false);
        ui->splitter_2->setOrientation(Qt::Vertical);
        restoreSplitter("mainwin-right-splittersizes-vert", ui->splitter_2);
    }
    else {  // not letting the user uncheck the action
        ui->actionVerticalLayout->setChecked(true);
    }
}

void MainWindow::on_actionHorizontalLayout_triggered(bool checked)
{
    if (ui->splitter_2->orientation() == Qt::Vertical)
        saveSplitter("mainwin-right-splittersizes-vert", ui->splitter_2);

    if (checked) {  // it just got pressed
        ui->actionVerticalLayout->setChecked(false);
        ui->splitter_2->setOrientation(Qt::Horizontal);
        restoreSplitter("mainwin-right-splittersizes-horiz", ui->splitter_2);
    }
    else {  // not letting the user uncheck the action
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
    about->setWindowTitle("About OMNeT++");
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

void MainWindow::showFindObjectsDialog(cObject *obj)
{
    // Set mainwindow to dialog's parent, thus it is closed when mainwindow is closed.
    FindObjectsDialog *dialog = new FindObjectsDialog(obj, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
    connect(this, SIGNAL(setNewNetwork()), dialog, SLOT(invalidate()));
    connect(this, SIGNAL(closed()), dialog, SLOT(close()));
}

void MainWindow::on_actionLoadNedFile_triggered()
{
    QString lastNedFile = getQtenv()->getPref("last-nedfile", ".").value<QString>();

    QString fileName = QFileDialog::getOpenFileName(this,
                tr("Open NED file"), lastNedFile, tr("NED Files (*.ned);;All files (*)"));

    if (!fileName.isNull()) {
        getQtenv()->setPref("last-nedfile", fileName);
        getQtenv()->loadNedFile(fileName.toStdString().c_str());

        getQtenv()->callRefreshInspectors();
    }
}

void MainWindow::on_actionOpenPrimaryIniFile_triggered()
{
    QString fileName = getQtenv()->getIniFileName();
    if (fileName.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("The current configuration manager doesn't use file input."),
                QMessageBox::Ok);
        return;
    }

    fileEditor->setFile(fileName);
    fileEditor->show();
}

int MainWindow::inputBox(const QString& title, const QString& prompt, QString& variable)
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
    if (!networkPresent())
        return;

    QString variable = "";

    if (inputBox("Snapshot", "Enter label for current simulation snapshot:", variable) == QDialog::Accepted) {
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
    if (env->checkRunning())
        return;

    // check state is not SIM_NONET
    if (!networkPresent())
        return;

    // check state is not SIM_FINISHCALLED
    if (getQtenv()->getSimulationState() == Qtenv::SIM_FINISHCALLED) {
        QMessageBox::information(this, tr("Error"), tr("finish() has been invoked already."), QMessageBox::Ok);
        return;
    }

    // check state is not SIM_ERROR
    if (getQtenv()->getSimulationState() == Qtenv::SIM_ERROR) {
        QMessageBox::StandardButton ans =
            QMessageBox::question(this, tr("Warning"),
                    "Simulation has stopped with error, calling finish() might produce unexpected results. Proceed anyway?",
                    QMessageBox::Yes | QMessageBox::No);

        if (ans == QMessageBox::No)
            return;
    }
    else {
        QMessageBox::StandardButton ans =
            QMessageBox::question(this, tr("Question"),
                    "Do you want to conclude this simulation run and invoke finish() on all modules?",
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        if (ans == QMessageBox::No)
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
    if (!networkPresent())
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
void MainWindow::on_actionNedComponentTypes_triggered()
{
    getQtenv()->inspect(componentTypes.getInstance(), INSP_DEFAULT, true);
}

// inspectClasses
void MainWindow::on_actionRegisteredClasses_triggered()
{
    getQtenv()->inspect(classes.getInstance(), INSP_DEFAULT, true);
}

void MainWindow::on_actionClassDescriptors_triggered()
{
    getQtenv()->inspect(classDescriptors.getInstance(), INSP_DEFAULT, true);
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

void MainWindow::on_actionResultFilters_triggered()
{
    getQtenv()->inspect(resultFilters.getInstance(), INSP_DEFAULT, true);
}

void MainWindow::on_actionResultRecorders_triggered()
{
    getQtenv()->inspect(resultRecorders.getInstance(), INSP_DEFAULT, true);
}

void MainWindow::on_actionMessagePrinters_triggered()
{
    getQtenv()->inspect(messagePrinters.getInstance(), INSP_DEFAULT, true);
}

// inspectBypointer
void MainWindow::on_actionInspectByPointer_triggered()
{
    // implements Inspect|By pointer...
    QString pointer = "ptr";
    int ok = inputBox("Inspect by pointer...", "Enter pointer (invalid pointer may cause segmentation fault!):", pointer);
    if (ok == QDialog::Accepted)
        getQtenv()->inspect(strToPtr(pointer.toStdString().c_str()), INSP_DEFAULT, true);
}

void MainWindow::on_actionRecordVideo_toggled(bool checked)
{
    auto duc = env->getDisplayUpdateController();
    // have to resize the mainwindow to be a size of a multiple of 4 in both dimensions
    // because many video encoders (like x264) demand it
    if (checked) {
        QString configRun = env->getConfigEx()->getActiveConfigName();
        configRun += "#" + QString::number(env->getConfigEx()->getActiveRunNumber());

        VideoRecordingDialog dialog(this, configRun);

        dialog.exec();

        if (dialog.result() == QDialog::Accepted) {
            setFixedSize(width() / 2 * 2, height() / 2 * 2);
            QString base = "frames/" + configRun + "_";
            duc->setFilenameBase(base.toUtf8());
            duc->startVideoRecording();
        }
        else
            ui->actionRecordVideo->setChecked(false);
    } else {
        setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        duc->stopVideoRecording();
    }
}

void MainWindow::on_actionShowAnimationParams_toggled(bool checked)
{
    auto duc = env->getDisplayUpdateController();
    if (checked)
        duc->showDialog(ui->actionShowAnimationParams);
    else
        duc->hideDialog();
    setFocus();
    QApplication::processEvents();
}

}  // namespace qtenv
}  // namespace omnetpp
