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
#include <QGraphicsItem>
#include <QMessageBox>
#include <QClipboard>
#include <QCloseEvent>
#include "qtenv.h"
#include "runselectiondialog.h"
#include "treeitemmodel.h"
#include "genericobjecttreemodel.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cfutureeventset.h"
#include "inspector.h"
#include "common/stringutil.h"
#include "stopdialog.h"
#include <QStyledItemDelegate>
#include <QTextLayout>
#include <QTimer>
#include <QLineEdit>
#include <QDebug>

using namespace OPP::common;

namespace omnetpp {
namespace qtenv {

// uses a QTextLayout to highlight a part of the displayed text
// which is given by a HighlightRegion, returned by the tree model
class HighlighterItemDelegate : public QStyledItemDelegate {
public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
};

void HighlighterItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // drawing the selection background and focus rectangle, but no text
    QStyledItemDelegate::paint(painter, option, QModelIndex());

    // selecting the palette to use, depending on the item state
    QPalette::ColorGroup group = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (group == QPalette::Normal && !(option.state & QStyle::State_Active))
        group = QPalette::Inactive;

    painter->save();

    //Text from item
    QString text = index.data(Qt::DisplayRole).toString();

    QTextLayout layout;
    layout.setText(text);

    // this is the standard layout procedure in a single line case
    layout.beginLayout();
    QTextLine line = layout.createLine();
    line.setLineWidth(option.rect.width());
    layout.endLayout();

    // the formatted regions
    QList<QTextLayout::FormatRange> formats;

    QTextLayout::FormatRange f;

    // this sets the color of the whole text depending on whether the item is selected or not
    f.format.setForeground(option.palette.brush(group,
        (option.state & QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::Text));
    f.start = 0;
    f.length = text.length();
    formats.append(f);

    // and then adding another format region to highlight the range specified by the model
    HighlightRange range = index.data(Qt::UserRole).value<HighlightRange>();
    f.start = range.start;
    f.length = range.length;
    f.format.setForeground(QBrush(QColor(0, 0, 255)));
    f.format.setFontWeight(QFont::Bold);
    formats.append(f);

    // applying the format ranges
    layout.setAdditionalFormats(formats);

    // getting the icon for the object, and if found, offsetting the text and drawing the icon
    int textOffset = 0;
    auto iconData = index.data(Qt::DecorationRole);
    if (iconData.isValid()) {
        textOffset += option.decorationSize.width();
        QIcon icon = iconData.value<QIcon>();
        painter->drawImage(option.rect.topLeft(), icon.pixmap(option.decorationSize).toImage());
    }

    // the layout is complete, now we just draw it on the appropriate position
    layout.draw(painter, option.rect.translated(3 + textOffset, 1).topLeft());
    painter->restore();

}

void HighlighterItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // setting the initial geometry which covers the entire line
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);

    // extracting the value from the index
    QString text = index.data().toString();
    auto range = index.data(Qt::UserRole).value<HighlightRange>();

    // the value is bold, so it is wider, we need to get that font too
    auto boldFont = option.font;
    boldFont.setWeight(QFont::Bold);
    auto boldFontMetrics = QFontMetrics(boldFont);

    // this is where the editor should start
    auto beforeText = text.left(range.start);
    int editorLeft = option.fontMetrics.width(beforeText);

    // and this is how wide it should be
    auto inText = text.left(range.start + range.length).right(range.length);
    int editorWidth = boldFontMetrics.width(inText);

    // moving the editor horizontally and setting its width as computed
    auto geom = editor->geometry();
    geom.translate(editorLeft + 3, 0);
    geom.setWidth(qMax(20, editorWidth)); // so empty values can be edited too
    editor->setGeometry(geom);
}

// XXX Currently the editor will get the initial string from the HighlightRange.
// This is not really a good way to hand it over, but since at the moment
// the highlighting matches the value perfectly (and includes single quotes...),
// and the UserRole in the index is already used for the HighlightRange itself
// (and adding more [e.g. UserRole + 1] would work, but would hurt clarity),
// at the moment this should suffice
void HighlighterItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    // getting the right type of editor
    auto lineEdit = dynamic_cast<QLineEdit *>(editor);
    if (lineEdit) {
        // extracting the value to be edited
        QString text = index.data().toString();
        auto range = index.data(Qt::UserRole).value<HighlightRange>();

        // set it up in the editor
        lineEdit->setText(text.left(range.start + range.length - 1).right(range.length - 2));

        // match the cosmetics
        auto font = lineEdit->font();
        font.setWeight(QFont::Bold);
        lineEdit->setFont(font);
    }
}

MainWindow::MainWindow(Qtenv *env, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    env(env)
{
    ui->setupUi(this);

    // this should be deleted later
    TreeItemModel *model = new TreeItemModel();
    model->setRootObject(getSimulation());
    ui->treeView->setModel(model);
    ui->treeView->setHeaderHidden(true);

    // the model should be deleted later
    ui->treeView_2->setModel(new GenericObjectTreeModel(nullptr));
    ui->treeView_2->setHeaderHidden(true);
    // TODO, to highlight the values in blue, etc
    ui->treeView_2->setItemDelegate(new HighlighterItemDelegate());

    stopDialog = new StopDialog();

    //TODO
    slider = new QSlider();
    slider->setOrientation(Qt::Orientation::Horizontal);
    slider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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

MainWindow::~MainWindow()
{
    delete ui;
    delete stopDialog;
}

void MainWindow::displayText(const char *t)
{
    ui->textBrowser->append(QString(t));
}

QTreeView *MainWindow::getObjectTree()
{
    return ui->treeView;
}

QTreeView *MainWindow::getObjectInspectorTree()
{
    return ui->treeView_2;
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
                        QMessageBox::Yes | QMessageBox::No);
            if (ans == QMessageBox::No)
                return false;
        }
        else if (env->getSimulationState() == Qtenv::SIM_READY) {
            int ans = QMessageBox::warning(this, tr("Warning"), tr("Do you want to conclude the simulation by invoking finish() before exiting?"),
                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
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
    ui->actionSetUpConfiguration->trigger();
}

void MainWindow::runSimulation(Mode mode)
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

    RunSelectionDialog *dialog = new RunSelectionDialog(env, this);
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

void MainWindow::inspectObject(QModelIndex index)
{
    qDebug() << "Inspecting object.";
    if (!index.isValid())
        return;

    cObject *parent = static_cast<cObject *>(index.internalPointer());
    cCollectChildrenVisitor visitor(parent);
    visitor.process(parent);
    cObject **objs = visitor.getArray();
    if (visitor.getArraySize() > index.row())
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

void MainWindow::redrawTimeline()
{
    // TODO
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
    if (index.isValid()) {
        QMenu *menu = static_cast<TreeItemModel *>(ui->treeView->model())->getContextMenu(index, this);
        menu->exec(ui->treeView->mapToGlobal(point));
        delete menu;
    }
}

// Handle object tree's context menu QAction's triggerd event.
void MainWindow::onClickOpenInspector()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        QPair<cObject *, int> objTypePair = variant.value<QPair<cObject *, int> >();
        inspectObject(objTypePair.first, objTypePair.second);
    }
}

// Handle object tree's context menu QAction's triggerd event.
void MainWindow::onClickRun()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        QPair<cObject *, int> objTypePair = variant.value<QPair<cObject *, int> >();
        runSimulationLocal(nullptr, objTypePair.second, objTypePair.first);
    }
}

// Handle object tree's context menu QAction's triggerd event.
void MainWindow::onClickRunMessage()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        QPair<cObject *, int> objTypePair = variant.value<QPair<cObject *, int> >();
        runUntilMsg(static_cast<cMessage *>(objTypePair.first), objTypePair.second);
    }
}

// Handle object tree's context menu QAction's triggerd event.
void MainWindow::onClickExcludeMessage()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid())
        excludeMessageFromAnimation(variant.value<cObject *>());
}

// Handle object tree's context menu QAction's triggerd event.
void MainWindow::onClickUtilitiesSubMenu()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        QPair<cObject *, int> objTypePair = variant.value<QPair<cObject *, int> >();
        copyToClipboard(static_cast<cMessage *>(objTypePair.first), objTypePair.second);
    }
}

void MainWindow::copyToClipboard(cObject *object, int what)
{
    switch (what) {
        case COPY_PTR: {
            void *address = static_cast<void *>(object);
            std::stringstream ss;
            ss << address;
            setClipboard(QString(ss.str().c_str()));
            break;
        }

        case COPY_PTRWITHCAST: {
            void *address = static_cast<void *>(object);
            std::stringstream ss;
            ss << address;
            setClipboard(QString("((") + object->getClassName() + " *)" + ss.str().c_str() + ")");
            break;
        }

        case COPY_FULLPATH:
            setClipboard(object->getFullPath().c_str());
            break;

        case COPY_FULLNAME:
            setClipboard(object->getFullName());
            break;

        case COPY_CLASSNAME:
            setClipboard(object->getClassName());
            break;
    }
}

void MainWindow::setClipboard(QString str)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear();
    clipboard->setText(str);
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
    filters.trimmed();
    if (!filters.isEmpty())
        filters += "\n";
    filters += namePattern +" and className(" + cl +")\n";
    env->setSilentEventFilters(filters.toStdString().c_str());

    redrawTimeline();
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

void MainWindow::runSimulationLocal(Inspector *insp, int runMode, cObject *object)
{
    Mode mode = runModeToMode(runMode);
    if (isRunning()) {
        setGuiForRunmode(mode);
        env->setSimulationRunMode(runMode);
        setRunUntilModule(insp);
    }
    else {
        if (!networkReady())
            return;
        setGuiForRunmode(mode, insp);
        if (object == nullptr)
            object = insp->getObject();

        cModule *mod = dynamic_cast<cModule *>(object);
        if (!mod) {
            // TODO log "object is not a module"
            return;
        }
        env->runSimulation(runMode, 0, 0, nullptr, mod);
        setGuiForRunmode(NOT_RUNNING);
    }
}

int MainWindow::modeToRunMode(Mode mode)
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
}

MainWindow::Mode MainWindow::runModeToMode(int runMode)
{
    switch (runMode) {
        case Qtenv::RUNMODE_NORMAL:
            return NORMAL;

        case Qtenv::RUNMODE_FAST:
            return FAST;

        case Qtenv::RUNMODE_EXPRESS:
            return EXPRESS;
    }
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

QWidget *MainWindow::getMainArea()
{
    return ui->mainArea;
}

} // namespace qtenv
} // namespace omnetpp
