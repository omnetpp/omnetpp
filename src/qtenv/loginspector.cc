//==========================================================================
//  LOGINSPECTOR.CC - part of
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

#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <fstream>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtCore/QDebug>
#include <QtWidgets/QMenu>

#include "common/stringtokenizer.h"
#include "common/stlutil.h"
#include "loginspector.h"
#include "qtenv.h"
#include "qtutil.h"
#include "inspectorutil.h"
#include "mainwindow.h"
#include "inspectorfactory.h"
#include "logfinddialog.h"
#include "logfilterdialog.h"
#include "messageprintertagsdialog.h"
#include "textviewerproviders.h"
#include "omnetpp/simtime.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace qtenv {

void _dummy_for_loginspector() {}

class LogInspectorFactory : public InspectorFactory
{
  public:
    LogInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cComponent *>(obj) != nullptr; }
    InspectorType getInspectorType() override { return INSP_LOG; }
    double getQualityAsDefault(cObject *object) override { return 0.5; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new LogInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(LogInspectorFactory);

LogInspector::LogInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f)
    : Inspector(parent, isTopLevel, f)
{
    logBuffer = getQtenv()->getLogBuffer();
    componentHistory = getQtenv()->getComponentHistory();

    auto layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    textWidget = new TextViewerWidget(this);
    textWidget->setFont(getQtenv()->getLogFont());

    toMessagesModeAction = new QAction(this);
    toMessagesModeAction->setCheckable(true);
    toMessagesModeAction->setIcon(QIcon(":/tools/pkheader"));
    toMessagesModeAction->setText("Message/Packet Traffic");
    connect(toMessagesModeAction, SIGNAL(triggered()), this, SLOT(toMessagesMode()));
    // no hotkey, no need to "add" it

    toLogModeAction = new QAction(this);
    toLogModeAction->setCheckable(true);
    toLogModeAction->setIcon(QIcon(":/tools/log"));
    toLogModeAction->setText("Log");
    connect(toLogModeAction, SIGNAL(triggered()), this, SLOT(toLogMode()));
    // no hotkey, no need to "add" it

    findAction = new QAction(QIcon(":/tools/find"), "&Find...", this);
    findAction->setToolTip("Find text in window (Ctrl+F)\nUse F3 to find next, Shift+F3 to find previous");
    findAction->setShortcut(Qt::ControlModifier + Qt::Key_F);
    connect(findAction, SIGNAL(triggered()), this, SLOT(onFindButton()));

    saveAction = new QAction(QIcon(":/tools/save"), "&Save Window Contents...", this);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveContent()));

    filterAction = new QAction(QIcon(":/tools/filter"), "Filter by &Module...", this);
    filterAction->setToolTip("Filter window contents by module (Ctrl+H)");
    filterAction->setShortcut(Qt::ControlModifier + Qt::Key_H);
    connect(filterAction, SIGNAL(triggered()), this, SLOT(onFilterButton()));

    configureMessagePrinterAction = new QAction(QIcon(":/tools/winconfig"), "C&onfigure Columns and More...", this);
    connect(configureMessagePrinterAction, SIGNAL(triggered()), this, SLOT(onMessagePrinterTagsButton()));

    /*
    stringContent = new StringTextViewerContentProvider(
                       "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
                       "Quisque eget dolor pharetra, fermentum lectus ac, luctus enim.\n"
                       "Vestibulum maximus nulla molestie bibendum vehicula.\n"
                       "Sed a augue vel ipsum pretium laoreet sit amet quis turpis.\n"
                       "Praesent ut felis ornare, convallis mauris ac, viverra justo.\n"
                       "Fusce id lacus sodales, vestibulum sem tempus, placerat diam.\n"
                       "Nullam suscipit justo vitae tristique blandit.\n"
                       "Duis non purus congue, pulvinar erat quis, tristique nibh.\n"
                       "Integer a lorem vitae tortor elementum facilisis quis ut orci.\n");
    */

    findAgainAction = new QAction("Find &Next", this);
    findAgainAction->setShortcut(Qt::Key_F3);
    findAgainAction->setEnabled(false);
    connect(findAgainAction, SIGNAL(triggered()), this, SLOT(findAgain()));
    addAction(findAgainAction);

    findAgainReverseAction = new QAction("Find &Previous", this);
    findAgainReverseAction->setShortcut(Qt::ShiftModifier + Qt::Key_F3);
    findAgainReverseAction->setEnabled(false);
    connect(findAgainReverseAction, SIGNAL(triggered()), this, SLOT(findAgainReverse()));
    addAction(findAgainReverseAction);

    // strips the ANSI control sequences, as expected
    copySelectionAction = new QAction(QIcon(":/tools/copy"), "&Copy", this);
    connect(copySelectionAction, SIGNAL(triggered()), textWidget, SLOT(copySelectionUnformatted()));
    copySelectionAction->setToolTip("Copy selected text to clipboard\nUse Ctrl+Shift+C to include formatting");
    copySelectionAction->setShortcut(Qt::ControlModifier + Qt::Key_C);
    copySelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(copySelectionAction);

    copySelectionWithFormattingAction = new QAction("Copy with Formatting", this); // not on toolbar
    connect(copySelectionWithFormattingAction, SIGNAL(triggered()), textWidget, SLOT(copySelection()));
    // we do this opposite of the usual: Hold shift to copy _formatted_ text
    // (because it's not a proper rich-text MIME content, only ANSI control sequences inline plain text)
    copySelectionWithFormattingAction->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_C);
    copySelectionWithFormattingAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(copySelectionWithFormattingAction);

    QToolBar *toolBar = createToolbar(isTopLevel);
    if (isTopLevel)
        layout->addWidget(toolBar);

    layout->addWidget(textWidget);
    connect(textWidget, SIGNAL(caretMoved(int, int)), this, SLOT(onCaretMoved(int, int)));
    connect(textWidget, SIGNAL(rightClicked(QPoint, int, int)), this, SLOT(onRightClicked(QPoint, int, int)));
    parent->setMinimumSize(100, 50);  // caution: too small widget height with heading displayed may cause notification loop!

    connect(getQtenv(), SIGNAL(fontChanged()), this, SLOT(onFontChanged()));

    mode = LOG;  // as setMode() *reads* it too
    setMode((Mode)getPref(PREF_MODE, QVariant::fromValue(0), false).toInt());
}

LogInspector::~LogInspector()
{
    if (mode == MESSAGES) {
        saveColumnWidths();
    }
}

void LogInspector::onFontChanged()
{
    textWidget->setFont(getQtenv()->getLogFont());
}

const QString LogInspector::PREF_COLUMNWIDTHS = "columnwidths";
const QString LogInspector::PREF_MODE = "mode";
const QString LogInspector::PREF_EXCLUDED_MODULES = "excluded-modules";
const QString LogInspector::PREF_SAVE_FILENAME = "savefilename";
const QString LogInspector::PREF_MESSAGEPRINTER_TAGS = "messageprinter-tags";

QToolBar *LogInspector::createToolbar(bool isTopLevel)
{
    QToolBar *toolBar = new QToolBar();
    toolBar->setAutoFillBackground(true);

    if (isTopLevel) {
        addTopLevelToolBarActions(toolBar);

        toolBar->addSeparator();
        addOwnActions(toolBar);

        toolBar->addSeparator();
        runUntilAction = toolBar->addAction(QIcon(":/tools/mrun"), "Run until next event in this module", this,
                                            SLOT(runUntil()));
        runUntilAction->setCheckable(true);

        fastRunUntilAction = toolBar->addAction(QIcon(":/tools/mfast"), "Fast run until next event in this module (Ctrl+F4)",
                                                this, SLOT(fastRunUntil()));
        fastRunUntilAction->setCheckable(true);
        fastRunUntilAction->setShortcut(Qt::CTRL + Qt::Key_F4);

        toolBar->addAction(getQtenv()->getMainWindow()->getStopAction());

        // this is to fill the remaining space on the toolbar, replacing the ugly default gradient on Mac
        QWidget *stretch = new QWidget(toolBar);
        stretch->setAutoFillBackground(true);
        stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        toolBar->addWidget(stretch);
    }
    else {
        addOwnActions(toolBar);

        textWidget->setToolBar(toolBar);
    }

    return toolBar;
}

void LogInspector::addOwnActions(QToolBar *toolBar)
{
    toolBar->addAction(copySelectionAction);
    toolBar->addAction(findAction);
    if (isTopLevel()) // looks like we don't need this in embedded mode, for whatever reason...
        toolBar->addAction(saveAction);
    toolBar->addAction(filterAction);
    toolBar->addAction(configureMessagePrinterAction);
    toolBar->addSeparator();

    toolBar->addAction(toMessagesModeAction);
    toolBar->addAction(toLogModeAction);
}

QStringList LogInspector::gatherAllMessagePrinterTags()
{
    std::set<std::string> allTags;

    for (auto mp : messagePrinters) {
        cMessagePrinter *printer = static_cast<cMessagePrinter *>(mp);

        auto curTags = printer->getSupportedTags();
        for (auto t : curTags)
            allTags.insert(t);
    }

    QStringList result;
    for (auto c : allTags)
        result << c.c_str();
    return result;
}

void LogInspector::setMode(Mode mode)
{
    bool atEnd = true;
    eventnumber_t caretAtEvent = -1;
    if (textWidget && textWidget->getContentProvider()) {
        auto *content = textWidget->getContentProvider();
        Pos caretPos = textWidget->getCaretPosition();

        atEnd = textWidget->followingOutput() || (caretPos.line == (content->getLineCount() - 1));
        caretAtEvent = content->getEventNumberAtLine(caretPos.line);
    }

    if (this->mode == MESSAGES)
        saveColumnWidths();

    toLogModeAction->setChecked(mode == LOG);
    toMessagesModeAction->setChecked(mode == MESSAGES);

    contentProvider = new ModuleOutputContentProvider(getQtenv(), dynamic_cast<cComponent *>(object), mode, &messagePrinterOptions);
    contentProvider->setExcludedModuleIds(excludedModuleIds);
    textWidget->setContentProvider(contentProvider);

    this->mode = mode;

    if (mode == MESSAGES)
        restoreColumnWidths();

    configureMessagePrinterAction->setEnabled(mode == MESSAGES);

    setPref(PREF_MODE, mode, false);

    if (atEnd)
        textWidget->followOutput();
    else {
        if (caretAtEvent >= 0) {
            int lineNumber = contentProvider->getLineAtEvent(caretAtEvent);
            if (lineNumber >= 0)
                textWidget->setCaretPosition(lineNumber, 0);
        }
    }
    textWidget->revealCaret();
}

void LogInspector::doSetObject(cObject *obj)
{
    Inspector::doSetObject(obj);
    excludedModuleIds.clear();
    setMode(mode); // this is to rebuild the model
    restoreExcludedModules();
    restoreMessagePrinterOptions();
}

void LogInspector::runUntil()
{
    runUntilAction->setEnabled(false);
    getQtenv()->runSimulationLocal(RUNMODE_NORMAL, nullptr, this);
    runUntilAction->setChecked(false);
    runUntilAction->setEnabled(true);
}

void LogInspector::fastRunUntil()
{
    fastRunUntilAction->setEnabled(false);
    getQtenv()->runSimulationLocal(RUNMODE_FAST, nullptr, this);
    fastRunUntilAction->setChecked(false);
    fastRunUntilAction->setEnabled(true);
}

void LogInspector::refresh()
{
    Inspector::refresh();
    contentProvider->refresh();
    textWidget->update();
    textWidget->viewport()->update();
}

void LogInspector::onFindButton()
{
    auto dialog = new LogFindDialog(this, lastFindText, lastFindOptions);
    if (dialog->exec() == QDialog::Accepted) {
        lastFindText = dialog->getText();
        lastFindOptions = dialog->getOptions();
        if (!lastFindText.isEmpty())
            textWidget->find(lastFindText.toStdString(), lastFindOptions);
        findAgainAction->setEnabled(!lastFindText.isEmpty());
        findAgainReverseAction->setEnabled(!lastFindText.isEmpty());
    }
    delete dialog;
}

void LogInspector::onFilterButton()
{
    if (cModule *module = dynamic_cast<cModule *>(object)) {
        LogFilterDialog dialog(this, module, excludedModuleIds);
        if (dialog.exec() == QDialog::Accepted) {
            excludedModuleIds = dialog.getExcludedModuleIds();
            contentProvider->setExcludedModuleIds(excludedModuleIds);
            saveExcludedModules();
        }
    }
}

void LogInspector::onMessagePrinterTagsButton()
{
    MessagePrinterTagsDialog dialog(this, gatherAllMessagePrinterTags(), &messagePrinterOptions);
    if (dialog.exec() == QDialog::Accepted) {
        messagePrinterOptions.enabledTags = dialog.getEnabledTags();
        contentProvider->refresh();
        saveMessagePrinterOptions();
    }
}

void LogInspector::onGoToSimTimeAction()
{
    QInputDialog dialog(this);
    dialog.setWindowTitle("Go to simulation time...");
    dialog.setLabelText("Enter simulation time (e.g. '12', '1.5us', '3s 125ms'):\n"
                        "You can also start with + or - for relative navigation.");
    dialog.setInputMode(QInputDialog::TextInput);

    int line = textWidget->getCaretPosition().line;
    SimTime t = contentProvider->getSimTimeAtLine(line);

    if (t >= 0)
        dialog.setTextValue(t.ustr().c_str());

    if (dialog.exec() == QDialog::Accepted) {
        QString text = dialog.textValue();
        try {
            if (text.startsWith('+') || text.startsWith('-')) {
                SimTime delta = SimTime::parse(text.mid(1).toUtf8());
                if (text.startsWith('+'))
                    t += delta;
                else
                    t -= delta;
            } else {
                t = SimTime::parse(text.toUtf8());
            }
            goToSimTime(t);
        }
        catch (cRuntimeError& e) {
            QMessageBox::warning(this, "Error", e.what());
        }
    }
}

void LogInspector::onGoToEventAction()
{
    QInputDialog dialog(this);
    dialog.setWindowTitle("Go to event...");
    dialog.setLabelText("Enter event number:");
    dialog.setInputMode(QInputDialog::IntInput);

    int line = textWidget->getCaretPosition().line;
    eventnumber_t e = contentProvider->getEventNumberAtLine(line);

    dialog.setIntMinimum(0);
    dialog.setIntMaximum(getSimulation()->getEventNumber());

    if (e >= 0)
        dialog.setIntValue(e);

    if (dialog.exec() == QDialog::Accepted)
        goToEvent(dialog.intValue());
}

void LogInspector::onCaretMoved(int lineIndex, int column)
{
    auto msg = (cMessage *)textWidget->getContentProvider()->getUserData(lineIndex);
    if (msg)
        Q_EMIT selectionChanged(msg);
}

void LogInspector::onRightClicked(QPoint globalPos, int lineIndex, int column)
{
    auto msg = (cMessage *)textWidget->getContentProvider()->getUserData(lineIndex);
    QMenu *menu = msg
        ? InspectorUtil::createInspectorContextMenu(msg, this)
        : new QMenu(this);

    if (mode == LogInspector::MESSAGES) {
        menu->addSeparator();

        QAction *reverseHopsAction = menu->addAction("Allow Backward &Arrows for Hops", [=](bool checked) {
            getQtenv()->opt->allowBackwardArrowsForHops = checked;
            getQtenv()->refreshInspectors();
        });
        reverseHopsAction->setCheckable(true);
        reverseHopsAction->setChecked(getQtenv()->opt->allowBackwardArrowsForHops);

        QAction *groupDigitsAction = menu->addAction("Digit &Grouping for Simulation Time", [=](bool checked) {
            getQtenv()->opt->messageLogDigitGrouping = checked;
            getQtenv()->refreshInspectors();
        });
        groupDigitsAction->setCheckable(true);
        groupDigitsAction->setChecked(getQtenv()->opt->messageLogDigitGrouping);

        menu->addSeparator();
        menu->addAction("Set Sending Time as &Reference", [=]() {
            EventEntryMessageLinesProvider::setReferenceTime(msg->getSendingTime());
        });

        SimTime refTime = EventEntryMessageLinesProvider::getReferenceTime();
        if (refTime > 0) {
            std::string refTimeStr = refTime.format(SimTime::getScaleExp(), ".", "'");
            refTimeStr = stripSuffixes(refTimeStr, "'000");
            menu->addAction("Clear Time Reference (=" + QString::fromStdString(refTimeStr) + ")", [=]() {
                EventEntryMessageLinesProvider::setReferenceTime(0);
            });
        }

        menu->addSeparator();
    }

    menu->addAction("Go to Simulation &Time...", this, SLOT(onGoToSimTimeAction()));
    menu->addAction("Go to &Event...", this, SLOT(onGoToEventAction()));

    menu->addSeparator();
    menu->addAction(copySelectionAction);

    menu->addSeparator();
    menu->addAction(findAction);
    menu->addAction(findAgainAction);
    menu->addAction(findAgainReverseAction);

    menu->addSeparator();
    if (mode == MESSAGES)
        menu->addAction(configureMessagePrinterAction);
    menu->addAction(filterAction);
    menu->addAction(saveAction);

    menu->exec(globalPos);
}

void LogInspector::findAgain()
{
    if (!lastFindText.isEmpty())
        textWidget->find(lastFindText.toStdString(), lastFindOptions);
}

void LogInspector::findAgainReverse()
{
    if (!lastFindText.isEmpty())
        textWidget->find(lastFindText.toStdString(), lastFindOptions ^ FIND_BACKWARDS);
}

void LogInspector::goToSimTime(SimTime t)
{
    Pos pos = textWidget->getCaretPosition();
    pos.line = contentProvider->getLineAtSimTime(t);
    textWidget->setCaretPosition(pos.line, pos.column);
    int horizontalMargin = textWidget->viewport()->width() / 5;
    int verticalMargin = textWidget->viewport()->height() / 5;
    // 20% on the top and left, 40% on the bottom and right
    textWidget->revealCaret(QMargins(horizontalMargin, verticalMargin, horizontalMargin * 2, verticalMargin * 2));
}

void LogInspector::goToEvent(eventnumber_t e)
{
    Pos pos = textWidget->getCaretPosition();
    pos.line = contentProvider->getLineAtEvent(e);
    textWidget->setCaretPosition(pos.line, pos.column);
    int horizontalMargin = textWidget->viewport()->width() / 5;
    int verticalMargin = textWidget->viewport()->height() / 5;
    // 20% on the top and left, 40% on the bottom and right
    textWidget->revealCaret(QMargins(horizontalMargin, verticalMargin, horizontalMargin * 2, verticalMargin * 2));
}

void LogInspector::toMessagesMode()
{
    setMode(MESSAGES);
}

void LogInspector::toLogMode()
{
    setMode(LOG);
}

void LogInspector::saveColumnWidths()
{
    auto widths = textWidget->getColumnWidths();
    if (!widths.isEmpty() && std::all_of(widths.begin(), widths.end(),
                [](const QVariant& v) { return v.isValid(); })) {
        setPref(PREF_COLUMNWIDTHS, widths);
    }
}

void LogInspector::restoreColumnWidths()
{
    textWidget->setColumnWidths(getPref(PREF_COLUMNWIDTHS, QList<QVariant>()).toList());
}

void LogInspector::saveExcludedModules()
{
    QStringList excludedModules;
    for (auto id : excludedModuleIds)
        excludedModules.append(getQtenv()->getComponentHistory()->getComponentFullPath(id).c_str());
    setPref(PREF_EXCLUDED_MODULES, excludedModules);
}

void LogInspector::restoreExcludedModules()
{
    QStringList excludedModules = getPref(PREF_EXCLUDED_MODULES, QStringList()).toStringList();
    for (auto path : excludedModules)
        if (auto mod = getSimulation()->findModuleByPath(path.toUtf8()))
            excludedModuleIds.insert(mod->getId());
    contentProvider->setExcludedModuleIds(excludedModuleIds);
}

void LogInspector::saveMessagePrinterOptions()
{
    QStringList messagePrinterTags;
    for (auto tag : messagePrinterOptions.enabledTags)
        messagePrinterTags.append(tag.c_str());
    setPref(PREF_MESSAGEPRINTER_TAGS, messagePrinterTags);
}

void LogInspector::restoreMessagePrinterOptions()
{
    messagePrinterOptions.enabledTags.clear();
    QVariant pref = getPref(PREF_MESSAGEPRINTER_TAGS, QVariant());

    if (pref.isValid()) {
        QStringList messagePrinterTags = pref.toStringList();

        for (auto tag : messagePrinterTags)
            messagePrinterOptions.enabledTags.insert(tag.toStdString());
    }
    else {
        // there was no preference saved, so we construct a "factory default"
        for (auto mp : messagePrinters) {
            auto printer = static_cast<cMessagePrinter *>(mp);
            auto defaultTags = printer->getDefaultEnabledTags();

            for (auto t : defaultTags)
                messagePrinterOptions.enabledTags.insert(t);
        }
    }
}

void LogInspector::saveContent()
{
    QString fileName = getPref(PREF_SAVE_FILENAME, "omnetpp.out").toString();
    fileName = QFileDialog::getSaveFileName(this, "Save Log Window Contents", fileName, "Log files (*.out);;All files (*)");

    std::ofstream fs(fileName.toStdString());

    if (!fs)
        return;

    int lineNumber = contentProvider->getLineCount();
    for (int i = 0; i < lineNumber; ++i)
        fs << stripFormattingAndRemoveTrailingNewLine(contentProvider->getLineText(i)) << '\n';

    setPref(PREF_SAVE_FILENAME, fileName.split(QDir::separator()).last());
}

}  // namespace qtenv
}  // namespace omnetpp

