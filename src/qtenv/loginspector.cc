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
#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QMessageBox>
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
    QToolBar *toolBar = createToolbar(isTopLevel);
    if (isTopLevel)
        layout->addWidget(toolBar);

    layout->addWidget(textWidget);
    connect(textWidget, SIGNAL(caretMoved(int, int)), this, SLOT(onCaretMoved(int, int)));
    connect(textWidget, SIGNAL(rightClicked(QPoint, int, int)), this, SLOT(onRightClicked(QPoint, int, int)));
    parent->setMinimumSize(100, 50);  // caution: too small widget height with heading displayed may cause notification loop!

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

    QAction *findAgainAction = new QAction(this);
    findAgainAction->setShortcut(Qt::Key_F3);
    connect(findAgainAction, SIGNAL(triggered()), this, SLOT(findAgain()));
    addAction(findAgainAction);

    QAction *findAgainReverseAction = new QAction(this);
    findAgainReverseAction->setShortcut(Qt::ShiftModifier + Qt::Key_F3);
    connect(findAgainReverseAction, SIGNAL(triggered()), this, SLOT(findAgainReverse()));
    addAction(findAgainReverseAction);

    QAction *copySelectionWithFormattingAction = new QAction(this);
    // we do this opposite of the usual: Hold shift to copy _formatted_ text
    // (because it's not a proper rich-text MIME content, only ANSI control sequences inline plain text)
    copySelectionWithFormattingAction->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_C);
    copySelectionWithFormattingAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(copySelectionWithFormattingAction, SIGNAL(triggered()), textWidget, SLOT(copySelection()));
    addAction(copySelectionWithFormattingAction);

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

//void LogInspector::setSunkenRunUntil(bool isSunken)
//{
//    runUntilAction->setChecked(isSunken);
//}

void LogInspector::addOwnActions(QToolBar *toolBar)
{
    // strips the ANSI control sequences, as expected
    QAction *copySelectionAction = toolBar->addAction(QIcon(":/tools/copy"), "Copy selected text to clipboard (Ctrl+C)\nUse Ctrl+Shift+C to include formatting",
                       textWidget, SLOT(copySelectionUnformatted()));
    copySelectionAction->setShortcut(Qt::ControlModifier + Qt::Key_C);
    copySelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(copySelectionAction);

    toolBar->addAction(QIcon(":/tools/find"), "Find string in window (Ctrl+F)",
                       this, SLOT(onFindButton()))->setShortcut(Qt::ControlModifier + Qt::Key_F);
    if (isTopLevel()) // looks like we don't need this in embedded mode, for whatever reason...
        toolBar->addAction(QIcon(":/tools/save"), "Save window contents to file",
                           this, SLOT(saveContent()));
    toolBar->addAction(QIcon(":/tools/filter"), "Filter window contents (Ctrl+H)",
                       this, SLOT(onFilterButton()))->setShortcut(Qt::ControlModifier + Qt::Key_H);
    configureMessagePrinterAction = toolBar->addAction(QIcon(":/tools/winconfig"), "Configure message display",
                                 this, SLOT(onMessagePrinterTagsButton()));
    toolBar->addSeparator();

    toMessagesModeAction = new QAction(toolBar);
    toMessagesModeAction->setCheckable(true);
    toMessagesModeAction->setIcon(QIcon(":/tools/pkheader"));
    toMessagesModeAction->setText("Message/packet traffic");
    connect(toMessagesModeAction, SIGNAL(triggered()), this, SLOT(toMessagesMode()));
    toolBar->addAction(toMessagesModeAction);

    toLogModeAction = new QAction(toolBar);
    toLogModeAction->setCheckable(true);
    toLogModeAction->setIcon(QIcon(":/tools/log"));
    toLogModeAction->setText("Log");
    connect(toLogModeAction, SIGNAL(triggered()), this, SLOT(toLogMode()));
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
            textWidget->find(lastFindText, lastFindOptions);
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

void LogInspector::onCaretMoved(int lineIndex, int column)
{
    auto msg = (cMessage *)textWidget->getContentProvider()->getUserData(lineIndex);
    if (msg)
        emit selectionChanged(msg);
}

void LogInspector::onRightClicked(QPoint globalPos, int lineIndex, int column)
{
    auto msg = (cMessage *)textWidget->getContentProvider()->getUserData(lineIndex);
    if (msg) {
        QMenu *menu = InspectorUtil::createInspectorContextMenu(msg, this);

        if (mode == LogInspector::MESSAGES) {
            menu->addSeparator();
            menu->addAction("Set Sending Time as Reference", [=]() {
                EventEntryMessageLinesProvider::setReferenceTime(msg->getSendingTime());
            });

            SimTime refTime = EventEntryMessageLinesProvider::getReferenceTime();
            if (refTime > 0) {
                QString refTimeStr = refTime.format(SimTime::getScaleExp(), ".", "'").c_str();
                refTimeStr = stripSuffixes(refTimeStr, "'000");
                menu->addAction("Clear Time Reference (=" + refTimeStr + ")", [=]() {
                    EventEntryMessageLinesProvider::setReferenceTime(0);
                });
            }

            menu->addSeparator();

            QAction *reverseHopsAction = menu->addAction("Allow Backward Arrows for Hops", [=](bool checked) {
                getQtenv()->opt->allowBackwardArrowsForHops = checked;
                getQtenv()->refreshInspectors();
            });
            reverseHopsAction->setCheckable(true);
            reverseHopsAction->setChecked(getQtenv()->opt->allowBackwardArrowsForHops);

            QAction *groupDigitsAction = menu->addAction("Digit Grouping for Simulation Time", [=](bool checked) {
                getQtenv()->opt->messageLogDigitGrouping = checked;
                getQtenv()->refreshInspectors();
            });
            groupDigitsAction->setCheckable(true);
            groupDigitsAction->setChecked(getQtenv()->opt->messageLogDigitGrouping);
        }

        menu->exec(globalPos);
    }
}

void LogInspector::findAgain()
{
    if (!lastFindText.isEmpty())
        textWidget->find(lastFindText, lastFindOptions);
}

void LogInspector::findAgainReverse()
{
    if (!lastFindText.isEmpty())
        textWidget->find(lastFindText, lastFindOptions ^ TextViewerWidget::FIND_BACKWARDS);
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

    int lineNumber = contentProvider->getLineCount();

    QFile file(fileName);
    if (fileName.isEmpty() || !file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    for (int i = 0; i < lineNumber; ++i)
        out << stripFormatting(contentProvider->getLineText(i)) << '\n';

    file.close();
    setPref(PREF_SAVE_FILENAME, fileName.split(QDir::separator()).last());
}

}  // namespace qtenv
}  // namespace omnetpp

