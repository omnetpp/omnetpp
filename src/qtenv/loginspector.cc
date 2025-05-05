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
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
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
    findAction->setShortcut((int)Qt::ControlModifier | Qt::Key_F);
    connect(findAction, SIGNAL(triggered()), this, SLOT(onFindButton()));

    saveAction = new QAction(QIcon(":/tools/save"), "&Save Window Contents...", this);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveContent()));

    filterAction = new QAction(QIcon(":/tools/filter"), "Filter...", this);
    filterAction->setToolTip("Filter window contents (Ctrl+H)");
    filterAction->setShortcut((int)Qt::ControlModifier | Qt::Key_H);
    connect(filterAction, SIGNAL(triggered()), this, SLOT(onFilterButton()));

    clearFilterAction = new QAction(QIcon(":/tools/filter_off"), "Clear Filter", this);
    clearFilterAction->setToolTip("Clear content filter");
    //filterAction->setShortcut(Qt::ControlModifier + Qt::Key_H);
    connect(clearFilterAction, SIGNAL(triggered()), this, SLOT(onClearFilterButton()));

    QMenu *fMenu = new QMenu(this);
    fMenu->addAction(clearFilterAction);


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
    findAgainAction->setShortcut((int)Qt::Key_F3);
    findAgainAction->setEnabled(false);
    connect(findAgainAction, SIGNAL(triggered()), this, SLOT(findAgain()));
    addAction(findAgainAction);

    findAgainReverseAction = new QAction("Find &Previous", this);
    findAgainReverseAction->setShortcut((int)Qt::ShiftModifier | Qt::Key_F3);
    findAgainReverseAction->setEnabled(false);
    connect(findAgainReverseAction, SIGNAL(triggered()), this, SLOT(findAgainReverse()));
    addAction(findAgainReverseAction);

    // strips the ANSI control sequences, as expected
    copySelectionAction = new QAction(QIcon(":/tools/copy"), "&Copy", this);
    connect(copySelectionAction, SIGNAL(triggered()), textWidget, SLOT(copySelectionUnformatted()));
    copySelectionAction->setToolTip("Copy selected text to clipboard\nUse Ctrl+Shift+C to include formatting");
    copySelectionAction->setShortcuts({(int)Qt::ControlModifier | Qt::Key_C, (int)Qt::ControlModifier | Qt::Key_Insert});
    copySelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(copySelectionAction);

    copySelectionWithFormattingAction = new QAction("Copy with Formatting", this); // not on toolbar
    connect(copySelectionWithFormattingAction, SIGNAL(triggered()), textWidget, SLOT(copySelection()));
    // we do this opposite of the usual: Hold shift to copy _formatted_ text
    // (because it's not a proper rich-text MIME content, only ANSI control sequences inline plain text)
    copySelectionWithFormattingAction->setShortcut((int)Qt::ControlModifier | Qt::ShiftModifier | Qt::Key_C);
    copySelectionWithFormattingAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(copySelectionWithFormattingAction);

    goToSimTimeAction = new QAction("Go to Simulation &Time...");
    connect(goToSimTimeAction, SIGNAL(triggered()), this, SLOT(onGoToSimTimeAction()));
    goToSimTimeAction->setShortcut((int)Qt::CTRL | Qt::Key_T);
    addAction(goToSimTimeAction);

    goToEventAction = new QAction("Go to &Event...");
    connect(goToEventAction, SIGNAL(triggered()), this, SLOT(onGoToEventAction()));
    goToEventAction->setShortcut((int)Qt::CTRL | Qt::Key_E);
    goToEventAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(goToEventAction);

    setBookmarkAction = new QAction(QIcon(":/tools/bookmark_add"), "Set Boo&kmark");
    connect(setBookmarkAction, SIGNAL(triggered()), this, SLOT(onSetBookmarkAction()));
    setBookmarkAction->setShortcut((int)Qt::CTRL | Qt::Key_M);
    addAction(setBookmarkAction);

    goToBookmarkAction = new QAction(QIcon(":/tools/bookmark"), "Go to &Bookmark");
    connect(goToBookmarkAction, SIGNAL(triggered()), this, SLOT(onGoToBookmarkAction()));
    goToBookmarkAction->setShortcut((int)Qt::CTRL | Qt::Key_B);
    addAction(goToBookmarkAction);

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

    updateFilterActionIcon();

    // every inspector (including the embedded one) connect these signals back and forth with the embedded one
    LogInspector *embeddedInspector = isTopLevel ? getQtenv()->getMainLogInspector() : this;
    connect(this, SIGNAL(globalMessageFormatChanged()), embeddedInspector, SLOT(onGlobalMessageFormatChanged()));
    connect(embeddedInspector, SIGNAL(globalMessageFormatChangedBroadcast()), this, SLOT(onGlobalMessageFormatChangedBroadcast()));
}

LogInspector::~LogInspector()
{
    if (mode == MESSAGES)
        saveColumnWidths();
}

void LogInspector::onFontChanged()
{
    textWidget->setFont(getQtenv()->getLogFont());
}

void LogInspector::onGlobalMessageFormatChanged()
{
    // only the embedded inspector should handle this
    ASSERT(!isTopLevel());

    // by notifying every inspector, including itself, of the change
    Q_EMIT globalMessageFormatChangedBroadcast();
}

void LogInspector::onGlobalMessageFormatChangedBroadcast()
{
    // reacting to the change locally if needed
    if (mode == MESSAGES) {
        sourceContentProvider->refresh();
        refresh();
    }
}

const QString LogInspector::PREF_COLUMNWIDTHS = "columnwidths";
const QString LogInspector::PREF_MODE = "mode";
const QString LogInspector::PREF_EXCLUDED_MODULES = "excluded-modules";
const QString LogInspector::PREF_LINE_FILTER_STRING = "line-filter-string";
const QString LogInspector::PREF_LINE_FILTER_IS_REGEX = "line-filter-is-regex";
const QString LogInspector::PREF_LINE_FILTER_IS_CASE_SENSITIVE = "line-filter-is-case-sensitive";
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
        fastRunUntilAction->setShortcut((int)Qt::CTRL | Qt::Key_F4);

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
    if (QToolButton *filtButt = dynamic_cast<QToolButton*>(toolBar->widgetForAction(filterAction))) {
        filtButt->addAction(clearFilterAction);
        filtButt->setPopupMode(QToolButton::DelayedPopup);
    }
    toolBar->addAction(configureMessagePrinterAction);
    toolBar->addSeparator();

    toolBar->addAction(toMessagesModeAction);
    toolBar->addAction(toLogModeAction);
}

void LogInspector::updateFilterActionIcon()
{
    bool isFilterActive = !excludedModuleIds.empty() || !lineFilterString.empty();
    filterAction->setIcon(QIcon(isFilterActive ? ":/tools/filter_active" : ":/tools/filter"));
    clearFilterAction->setEnabled(isFilterActive);
}

/**
 * Returns all tags supported by the message printers associated with
 * this log inspector, as a sorted list of strings. The list is sorted
 * alphabetically.
 */
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
    if (object && textWidget && textWidget->getContentProvider()) {
        auto *content = textWidget->getContentProvider();
        Pos caretPos = textWidget->getCaretPosition();

        atEnd = textWidget->followingOutput() || (caretPos.line == (content->getLineCount() - 1));
        caretAtEvent = content->getEventNumberAtLine(caretPos.line);
    }

    if (this->mode == MESSAGES)
        saveColumnWidths();

    toLogModeAction->setChecked(mode == LOG);
    toMessagesModeAction->setChecked(mode == MESSAGES);

    this->mode = mode;

    recreateProviders();


    if (mode == MESSAGES)
        restoreColumnWidths();

    configureMessagePrinterAction->setEnabled(mode == MESSAGES);

    setPref(PREF_MODE, mode, false);

    if (atEnd)
        textWidget->followOutput();
    else {
        if (caretAtEvent >= 0) {
            int lineNumber = textWidget->getContentProvider()->getLineAtEvent(caretAtEvent);
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
    restoreFilterSettings();
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
        LogFilterDialog dialog(this, module, excludedModuleIds, lineFilterString, lineFilterIsRegex, lineFilterIsCaseSensitive);
        if (dialog.exec() == QDialog::Accepted) {
            excludedModuleIds = dialog.getExcludedModuleIds();
            sourceContentProvider->setExcludedModuleIds(excludedModuleIds);
            lineFilterString = dialog.getLineFilterString();
            lineFilterIsRegex = dialog.isLineFilterRegExp();
            lineFilterIsCaseSensitive = dialog.isLineFilterCaseSensitive();
            recreateProviders();
            updateFilterActionIcon();
            saveFilterSettings();
        }
    }
}

void LogInspector::onClearFilterButton()
{
    excludedModuleIds = {};
    sourceContentProvider->setExcludedModuleIds(excludedModuleIds);
    lineFilterString = "";
    recreateProviders();
    updateFilterActionIcon();
    saveFilterSettings();
}

void LogInspector::onMessagePrinterTagsButton()
{
    MessagePrinterTagsDialog dialog(this, gatherAllMessagePrinterTags(), &messagePrinterOptions);
    if (dialog.exec() == QDialog::Accepted) {
        messagePrinterOptions.enabledTags = dialog.getEnabledTags();
        sourceContentProvider->refresh();
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
    SimTime t = textWidget->getContentProvider()->getSimTimeAtLine(line);

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
            QMessageBox::critical(this, "Error", e.what());
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
    eventnumber_t e = textWidget->getContentProvider()->getEventNumberAtLine(line);

    dialog.setIntMinimum(0);
    dialog.setIntMaximum(getSimulation()->getEventNumber());

    if (e >= 0)
        dialog.setIntValue(e);

    if (dialog.exec() == QDialog::Accepted)
        goToEvent(dialog.intValue());
}

void LogInspector::onSetBookmarkAction()
{
    int lineIndex = textWidget->getCaretPosition().line;
    textWidget->getContentProvider()->bookmarkLine(lineIndex);
}

void LogInspector::onGoToBookmarkAction()
{
    int markedLine = textWidget->getContentProvider()->getBookmarkedLineIndex();

    if (markedLine >= 0) {
        Pos pos = textWidget->getCaretPosition();
        pos.line = markedLine;
        textWidget->setCaretPosition(pos.line, pos.column);
        int horizontalMargin = textWidget->viewport()->width() / 5;
        int verticalMargin = textWidget->viewport()->height() / 5;
        // 20% on the top and left, 40% on the bottom and right
        textWidget->revealCaret(QMargins(horizontalMargin, verticalMargin, horizontalMargin * 2, verticalMargin * 2));
    }
}

void LogInspector::onCaretMoved(int lineIndex, int column)
{
    auto msg = (cMessage *)textWidget->getContentProvider()->getUserData(lineIndex);
    if (msg)
        Q_EMIT selectionChanged(msg);
}

void LogInspector::onRightClicked(QPoint globalPos, int lineIndex, int column)
{
    cMessage *msg = static_cast<cMessage *>(textWidget->getContentProvider()->getUserData(lineIndex));
    QMenu *menu = msg
        ? InspectorUtil::createInspectorContextMenu(msg, this)
        : new QMenu(this);

    if (mode == LogInspector::MESSAGES) {
        menu->addSeparator();

        QAction *reverseHopsAction = menu->addAction("Allow Backward &Arrows for Hops", [this](bool checked) {
            getQtenv()->opt->allowBackwardArrowsForHops = checked;
            Q_EMIT globalMessageFormatChanged();
        });
        reverseHopsAction->setCheckable(true);
        reverseHopsAction->setChecked(getQtenv()->opt->allowBackwardArrowsForHops);

        QAction *groupDigitsAction = menu->addAction("Digit &Grouping for Simulation Time", [this](bool checked) {
            getQtenv()->opt->messageLogDigitGrouping = checked;
            Q_EMIT globalMessageFormatChanged();
        });
        groupDigitsAction->setCheckable(true);
        groupDigitsAction->setChecked(getQtenv()->opt->messageLogDigitGrouping);

        menu->addSeparator();
        menu->addAction(QIcon(":/tools/label"), "Set Sending Time as &Reference", [msg, this]() {
            if (EventEntryMessageLinesProvider::getReferenceTime() != msg->getSendingTime()) {
                EventEntryMessageLinesProvider::setReferenceTime(msg->getSendingTime());
                Q_EMIT globalMessageFormatChanged();
            }
        });

        SimTime refTime = EventEntryMessageLinesProvider::getReferenceTime();
        if (refTime > 0) {
            std::string refTimeStr = refTime.format(SimTime::getScaleExp(), ".", "'");
            refTimeStr = stripSuffixes(refTimeStr, "'000");
            menu->addAction(QIcon(":/tools/label_off"), "Clear Time Reference (=" + QString::fromStdString(refTimeStr) + ")", [this]() {
                if (EventEntryMessageLinesProvider::getReferenceTime() != 0) {
                    EventEntryMessageLinesProvider::setReferenceTime(0);
                    Q_EMIT globalMessageFormatChanged();
                }
            });
        }

        menu->addSeparator();
    }

    menu->addAction(goToSimTimeAction);
    menu->addAction(goToEventAction);

    menu->addSeparator();

    int bookmarkedLine = textWidget->getContentProvider()->getBookmarkedLineIndex();

    if (lineIndex == bookmarkedLine) {
        menu->addAction(QIcon(":/tools/bookmark_remove"), "C&lear Bookmark", [this] {
            textWidget->getContentProvider()->bookmarkLine(-1);
        });
    }
    else {
        if (bookmarkedLine >= 0)
            menu->addAction(goToBookmarkAction);
        menu->addAction(setBookmarkAction);
    }

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
    menu->addAction(clearFilterAction);
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
    pos.line = textWidget->getContentProvider()->getLineAtSimTime(t);
    textWidget->setCaretPosition(pos.line, pos.column);
    int horizontalMargin = textWidget->viewport()->width() / 5;
    int verticalMargin = textWidget->viewport()->height() / 5;
    // 20% on the top and left, 40% on the bottom and right
    textWidget->revealCaret(QMargins(horizontalMargin, verticalMargin, horizontalMargin * 2, verticalMargin * 2));
}

void LogInspector::goToEvent(eventnumber_t e)
{
    Pos pos = textWidget->getCaretPosition();
    pos.line = textWidget->getContentProvider()->getLineAtEvent(e);
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

void LogInspector::saveFilterSettings()
{
    QStringList excludedModules;
    for (auto id : excludedModuleIds)
        excludedModules.append(getQtenv()->getComponentHistory()->getComponentFullPath(id).c_str());
    setPref(PREF_EXCLUDED_MODULES, excludedModules);
    setPref(PREF_LINE_FILTER_STRING, QString::fromStdString(lineFilterString));
    setPref(PREF_LINE_FILTER_IS_REGEX, lineFilterIsRegex);
    setPref(PREF_LINE_FILTER_IS_CASE_SENSITIVE, lineFilterIsCaseSensitive);
}

void LogInspector::restoreFilterSettings()
{
    QStringList excludedModules = getPref(PREF_EXCLUDED_MODULES, QStringList()).toStringList();
    for (auto path : excludedModules)
        if (auto mod = getSimulation()->findModuleByPath(path.toUtf8()))
            excludedModuleIds.insert(mod->getId());
    sourceContentProvider->setExcludedModuleIds(excludedModuleIds);
    lineFilterString = getPref(PREF_LINE_FILTER_STRING, "").toString().toStdString();
    lineFilterIsRegex = getPref(PREF_LINE_FILTER_IS_REGEX, false).toBool();
    lineFilterIsCaseSensitive = getPref(PREF_LINE_FILTER_IS_CASE_SENSITIVE, false).toBool();

    recreateProviders();

    updateFilterActionIcon();
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

void LogInspector::recreateProviders()
{
    ModuleOutputContentProvider::Bookmark bookmark;
    if (sourceContentProvider)
        bookmark = sourceContentProvider->getBookmark();
    sourceContentProvider = new ModuleOutputContentProvider(getQtenv(), dynamic_cast<cComponent *>(object), mode, &messagePrinterOptions);
    sourceContentProvider->setExcludedModuleIds(excludedModuleIds);
    updateFilterActionIcon();
    if (bookmark.isValid())
        sourceContentProvider->setBookmark(bookmark);

    auto columnWidths = textWidget->getColumnWidths();

    if (lineFilterString.empty()) {
        filteringContentProvider = nullptr;
        textWidget->setContentProvider(sourceContentProvider);
    }
    else {
        filteringContentProvider = new LineFilteringContentProvider(sourceContentProvider, lineFilterString, lineFilterIsRegex, lineFilterIsCaseSensitive);
        textWidget->setContentProvider(filteringContentProvider);
    }

    textWidget->setColumnWidths(columnWidths);
}

void LogInspector::saveContent()
{
    QString fileName = getPref(PREF_SAVE_FILENAME, "omnetpp.out").toString();
    fileName = QFileDialog::getSaveFileName(this, "Save Log Window Contents", fileName, "Log files (*.out);;All files (*)");

    std::ofstream fs(fileName.toStdString());

    if (!fs)
        return;

    int lineNumber = textWidget->getContentProvider()->getLineCount();
    for (int i = 0; i < lineNumber; ++i)
        fs << stripFormattingAndRemoveTrailingNewLine(textWidget->getContentProvider()->getLineText(i)) << '\n';

    setPref(PREF_SAVE_FILENAME, fileName.split(QDir::separator()).last());
}

}  // namespace qtenv
}  // namespace omnetpp

