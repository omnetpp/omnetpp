#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsItem>
#include <QMessageBox>
#include "qtenv.h"
#include "runselectiondialog.h"
#include "treeitemmodel.h"
#include "omnetpp/csimplemodule.h"
#include "inspector.h"
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

void MainWindow::setGuiForRunmode(Mode mode, Inspector *insp, bool untilMode)
{
    ui->actionOneStep->setChecked(false);
    ui->actionRun->setChecked(false);
    ui->actionFastRun->setChecked(false);
    ui->actionExpressRun->setChecked(false);
    //TODO
    //catch {toolbutton:setsunken $opp(sunken-run-button) 0}
    //removeStopDialog

    if(insp == nullptr)
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
                //TODO displayStopDialog
                break;
            case NOT_RUNNING:
                ui->actionRunUntil->setChecked(false);
                break;
        }
    else
        switch(mode)
        {
            case NORMAL:
                //TODO
                //toolbutton:setsunken $insp.toolbar.mrun 1
                //set opp(sunken-run-button) $insp.toolbar.mrun
                break;
            case FAST:
                //TODO
                //toolbutton:setsunken $insp.toolbar.mfast 1
                //set opp(sunken-run-button) $insp.toolbar.mfast
                break;
            case EXPRESS:
                //TODO displayStopDialog
                break;
            case NOT_RUNNING:
                //TODO toolbutton:setsunken .toolbar.until 0
                break;
            case STEP:
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
    Qtenv::eRunMode runMode = (Qtenv::eRunMode)modeToRunMode(mode);

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
        inspectObject(objs[index.row()]);
}

void MainWindow::inspectObject(cObject *object, int type, const char *geometry)
{
    if (!object)
        return;

    env->inspect(object, type, true, geometry);
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

void MainWindow::redrawTimeline()
{
    //TODO
//    global tkenv config widgets

//    # spare work if we're not displayed
//    if {$config(display-timeline)==0} {return}

//    set c $widgets(timeline)

//    # figure out vertical layout (see also timeline:fontChanged)
//    set fontheight [font metrics TimelineFont -linespace]
//    set row1y       [expr 2]
//    set row2y       [expr $row1y+$fontheight]
//    set axisy       [expr $row2y+$fontheight+3]
//    set labely      [expr $axisy+3]
//    set minorticky1 [expr $axisy-2]
//    set minorticky2 [expr $axisy+3]
//    set majorticky1 [expr $axisy-3]
//    set majorticky2 [expr $axisy+4]
//    set arrowy1     [expr $row2y-2]
//    set arrowy2     [expr $axisy-4]

//    # sort the FES and adjust display range
//    set minexp $tkenv(timeline-minexp)
//    set maxexp $tkenv(timeline-maxexp)

//    set fesrange [opp_sortfesandgetrange]
//    set fesmin [lindex $fesrange 0]
//    set fesmax [lindex $fesrange 1]
//    if [expr $fesmin!=0 && $fesmax!=0] {
//        set fesminexp [expr int(floor(log10($fesmin)))]
//        set fesmaxexp [expr int(ceil(log10($fesmax)))]
//        if {$fesminexp < $minexp && $fesminexp > -10} {set minexp $fesminexp}
//        if {$fesmaxexp > $maxexp && $fesmaxexp < 10} {set maxexp $fesmaxexp}
//    }
//    set tkenv(timeline-minexp) $minexp
//    set tkenv(timeline-maxexp) $maxexp

//    # start drawing
//    $c delete all

//    # draw axis
//    set w [winfo width $c]
//    incr w -10
//    $c create line 20 $axisy $w $axisy -arrow last -fill black -width 1
//    $c create text [expr $w+4] $labely -anchor ne -text "sec" -font TimelineFont

//    # draw ticks
//    set dx [expr $w/($maxexp-$minexp+1)]
//    set x0 [expr int($dx/2)+15]
//    set x $x0
//    for {set i $minexp} {$i<=$maxexp} {incr i} {
//        $c create line $x $majorticky1 $x $majorticky2 -fill black -width 1
//        if {$i>=4} {
//            set txt "1e$i"
//        } elseif {$i>=0} {
//           set txt "1[string repeat 0 $i]"
//        } elseif {$i>=-3} {
//           set txt "0.[string repeat 0 [expr -$i-1]]1"
//        } else {
//            set txt "1e$i"
//        }
//        $c create text $x $labely -anchor n -text "+$txt" -fill "#808080" -font TimelineFont

//        # minor ticks at 2, 4, 6, 8
//        foreach tick {0.301 0.602 0.778 0.903} {
//            set minorx [expr $x+int($tick*$dx)]
//            $c create line $minorx $minorticky1 $minorx $minorticky2 -fill black -width 1
//        }
//        incr x $dx
//    }

//    # draw events
//    set dtmin [expr 1e$minexp]
//    set minlabelx -1000
//    set minlabelx2 -1000
//    set labelssuppressed 0
//    set events [opp_fesevents $config(timeline-maxnumevents)
//                              $config(timeline-wantevents)
//                              $config(timeline-wantselfmsgs)
//                              $config(timeline-wantnonselfmsgs)
//                              $config(timeline-wantsilentmsgs)]

//    foreach eventptr $events {
//        # calculate position
//        set dt [opp_eventarrtimefromnow $eventptr]
//        if {$dt < $dtmin} {
//            set anchor "nw"
//            set x 10
//        } else {
//            set anchor "n"
//            set x [expr int($x0+(log10($dt)-$minexp)*$dx)]
//        }

//        # display ball
//        if [opp_instanceof $eventptr cMessage] {
//            if [opp_getsimoption animation_msgcolors] {
//               set msgkind [opp_getobjectfield $eventptr kind]
//               set color [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
//            } else {
//                set color red
//            }
//            set ball [$c create oval -2 -3 2 4 -fill $color -outline $color -tags "dx tooltip msg $eventptr"]
//        } else {
//            set ball [$c create oval -2 -3 2 4 -fill "" -outline red -tags "dx tooltip msg $eventptr"]
//        }
//        $c move $ball $x $axisy

//        # print msg name, if it's not too close to previous label
//        # label for only those msgs past this label's right edge will be displayed
//        set eventlabel [opp_getobjectfullname $eventptr]
//        if {$eventlabel!=""} {
//            set estlabelwidth [font measure TimelineFont $eventlabel]
//            set estlabelx [expr $x-$estlabelwidth/2]
//            if {$estlabelx>=$minlabelx} {
//                set labelid [$c create text $x $row2y -text $eventlabel -anchor $anchor -font TimelineFont -tags "dx tooltip msgname $eventptr"]
//                set minlabelx [lindex [$c bbox $labelid] 2]
//                set labelssuppressed 0
//            } elseif {$estlabelx>=$minlabelx2} {
//                set labelid [$c create text $x $row1y -text $eventlabel -anchor $anchor -font TimelineFont -tags "dx tooltip msgname $eventptr"]
//                $c create line $x $arrowy1 $x $arrowy2 -fill "#ccc" -width 1 -tags "h"
//                set minlabelx2 [lindex [$c bbox $labelid] 2]
//                set labelssuppressed 0
//            } else {
//                incr labelssuppressed
//                if {$labelssuppressed==1} {
//                    $c insert $labelid end ",..."
//                }
//            }
//        }
//    }
//    $c lower "h"
}

void MainWindow::onTreeViewContextMenu(QPoint point)
{
    QModelIndex index = ui->treeView->indexAt(point);
    if (index.isValid())
    {
        QMenu *menu = static_cast<TreeItemModel*>(ui->treeView->model())->getContextMenu(index, this);
        menu->exec(ui->treeView->mapToGlobal(point));
        delete menu;
    }
}

//Handle object tree's context menu QAction's triggerd event.
void MainWindow::onClickOpenInspector()
{
    QVariant variant = static_cast<QAction*>(QObject::sender())->data();
    if(variant.isValid())
    {
        QPair<cObject*, int> objTypePair = variant.value<QPair<cObject*, int>>();
        inspectObject(objTypePair.first, objTypePair.second);
    }
}

//Handle object tree's context menu QAction's triggerd event.
void MainWindow::onClickRun()
{
    QVariant variant = static_cast<QAction*>(QObject::sender())->data();
    if(variant.isValid())
    {
        QPair<cObject*, int> objTypePair = variant.value<QPair<cObject*, int>>();
        runSimulationLocal(nullptr, objTypePair.second, objTypePair.first);
    }
}

void MainWindow::runSimulationLocal(Inspector *insp, int runMode, cObject *object)
{
    Mode mode = runModeToMode(runMode);
    if(isRunning())
    {
        setGuiForRunmode(mode);
        env->setSimulationRunMode(runMode);
        //TODO opp_set_run_until_module $insp
    }
    else
    {
        //TODO if {![networkReady]} {return}
        setGuiForRunmode(mode, insp);
        //TODO
        //if {$ptr==""} {set ptr [opp_inspector_getobject $insp]}
        //opp_onestepinmodule $ptr $mode
        setGuiForRunmode(NOT_RUNNING);
    }
}

int MainWindow::modeToRunMode(Mode mode)
{
    switch(mode)
    {
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
}

MainWindow::Mode MainWindow::runModeToMode(int runMode)
{
    switch(runMode)
    {
        case Qtenv::RUNMODE_NORMAL:
            return NORMAL;
        case Qtenv::RUNMODE_FAST:
            return FAST;
        case Qtenv::RUNMODE_EXPRESS:
            return EXPRESS;
    }
}
