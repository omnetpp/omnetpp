//==========================================================================
//  LOGINSPECTOR.CC - part of
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

#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <QToolBar>
#include <QAction>
#include <QToolButton>
#include <QMessageBox>

#include "common/stringtokenizer.h"
#include "loginspector.h"
#include "qtenv.h"
#include "tklib.h"
#include "qtutil.h"
#include "inspectorfactory.h"
#include "logfinddialog.h"
#include "logfilterdialog.h"
#include "genericobjectinspector.h"
#include <QGridLayout>
#include <QDebug>
#include "textviewerproviders.h"

using namespace OPP::common;

namespace omnetpp {
namespace qtenv {

#define LL    INT64_PRINTF_FORMAT

void _dummy_for_loginspector() {}

class LogInspectorFactory : public InspectorFactory
{
  public:
    LogInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cComponent*>(obj) != nullptr; }
    int getInspectorType() override { return INSP_MODULEOUTPUT; }
    double getQualityAsDefault(cObject *object) override { return 0.5; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new LogInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(LogInspectorFactory);

LogInspector::LogInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f)
    : Inspector(parent, isTopLevel, f) {
    logBuffer = getQtenv()->getLogBuffer();

    componentHistory = getQtenv()->getComponentHistory();

    auto layout = new QGridLayout(this);
    layout->setMargin(0);

    textWidget = new TextViewerWidget(this);
    layout->addWidget(textWidget, 0, 0, 1, 1);
    connect(textWidget, SIGNAL(caretMoved(int,int)), this, SLOT(onCaretMoved(int, int)));
    parent->setMinimumSize(20, 20);

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

    QToolBar *toolBar = new QToolBar();

    toolBar->addAction(QIcon(":/tools/icons/tools/copy.png"), "Copy selected text to clipboard (Ctrl+C)",
                       textWidget, SLOT(copySelection()))->setShortcut(Qt::ControlModifier + Qt::Key_C);
    toolBar->addAction(QIcon(":/tools/icons/tools/find.png"), "Find string in window (Ctrl+F)",
                       this, SLOT(onFindButton()))->setShortcut(Qt::ControlModifier + Qt::Key_F);
    toolBar->addAction(QIcon(":/tools/icons/tools/filter.png"), "Filter window contents (Ctrl+H)",
                       this, SLOT(onFilterButton()))->setShortcut(Qt::ControlModifier + Qt::Key_H);

    toMessagesModeAction = new QAction(toolBar);
    toMessagesModeAction->setCheckable(true);
    toMessagesModeAction->setIcon(QIcon(":/tools/icons/tools/pkheader.png"));
    toMessagesModeAction->setText("Show message/packet traffic");
    connect(toMessagesModeAction, SIGNAL(triggered()), this, SLOT(toMessagesMode()));
    toolBar->addAction(toMessagesModeAction);

    toLogModeAction = new QAction(toolBar);
    toLogModeAction->setCheckable(true);
    toLogModeAction->setIcon(QIcon(":/tools/icons/tools/log.png"));
    toLogModeAction->setText("Show log");
    connect(toLogModeAction, SIGNAL(triggered()), this, SLOT(toLogMode()));
    toolBar->addAction(toLogModeAction);

    textWidget->setToolBar(toolBar);

    QAction *findAgainAction = new QAction(this);
    findAgainAction->setShortcut(Qt::Key_F3);
    connect(findAgainAction, SIGNAL(triggered()), this, SLOT(findAgain()));
    addAction(findAgainAction);

    QAction *findAgainReverseAction = new QAction(this);
    findAgainReverseAction->setShortcut(Qt::ShiftModifier + Qt::Key_F3);
    connect(findAgainReverseAction, SIGNAL(triggered()), this, SLOT(findAgainReverse()));
    addAction(findAgainReverseAction);

    setMode(LOG);
}

LogInspector::~LogInspector() {
    if (mode == MESSAGES) {
        saveColumnWidths();
    }
}

void LogInspector::setMode(Mode m) {
    if (mode == MESSAGES) {
        saveColumnWidths();
    }

    switch (m) {
    case LOG:
        toLogModeAction->setChecked(true);
        toMessagesModeAction->setChecked(false);
        textWidget->setContentProvider(new ModuleOutputContentProvider(getQtenv(), getInspectedObject(), LOG));
        break;
    case MESSAGES:
        toMessagesModeAction->setChecked(true);
        toLogModeAction->setChecked(false);
        textWidget->setContentProvider(new ModuleOutputContentProvider(getQtenv(), getInspectedObject(), MESSAGES));
        break;
    }

    mode = m;

    if (mode == MESSAGES) {
        restoreColumnWidths();
    }
}

void LogInspector::doSetObject(cObject *obj)
{
    Inspector::doSetObject(obj);
    setMode(getMode());
}

cComponent *LogInspector::getInspectedObject()
{
    return static_cast<cComponent*>(object);
}

void LogInspector::refresh() {
    Inspector::refresh();
    textWidget->contentChanged();
    textWidget->update();
    textWidget->viewport()->update();
}


void LogInspector::onFindButton() {
    auto dialog = new LogFindDialog(this, lastFindText, lastFindOptions);
    if (dialog->exec() == QDialog::Accepted) {
        lastFindText = dialog->getText();
        lastFindOptions = dialog->getOptions();
        if (!lastFindText.isEmpty())
            textWidget->find(lastFindText, lastFindOptions);
    }
    delete dialog;
}

void LogInspector::onFilterButton() {
    auto dialog = new LogFilterDialog(this, dynamic_cast<cModule *>(getInspectedObject()), excludedModuleIds);
    if (dialog->exec() == QDialog::Accepted) {
        excludedModuleIds = dialog->getExcludedModuleIds();
        auto provider = dynamic_cast<ModuleOutputContentProvider *>(textWidget->getContentProvider());
        if (provider) {
            provider->setExcludedModuleIds(excludedModuleIds);
        }
        textWidget->contentChanged();
    }
    delete dialog;
}

void LogInspector::onCaretMoved(int lineIndex, int column)
{
    auto msg = (cMessage*)textWidget->getContentProvider()->getUserData(lineIndex);
    if (msg)
        getQtenv()->getMainObjectInspector()->setObject(msg);
}

void LogInspector::findAgain() {
    if (!lastFindText.isEmpty())
        textWidget->find(lastFindText, lastFindOptions);
}

void LogInspector::findAgainReverse() {
    if (!lastFindText.isEmpty())
        textWidget->find(lastFindText, lastFindOptions ^ TextViewerWidget::FIND_BACKWARDS);
}

void LogInspector::toMessagesMode() {
    setMode(MESSAGES);
}

void LogInspector::toLogMode() {
    setMode(LOG);
}

void LogInspector::saveColumnWidths() {
    auto widths = textWidget->getColumnWidths();
    if (!widths.isEmpty() && std::all_of(widths.begin(), widths.end(),
                                         [](const QVariant &v){ return v.isValid(); } )) {
        getQtenv()->setPref("loginspector_columnwidths", widths);
    }
}

void LogInspector::restoreColumnWidths() {
    auto widths = getQtenv()->getPref("loginspector_columnwidths");
    if (widths.isValid())
        textWidget->setColumnWidths(widths.toList());
}


} // namespace qtenv
} // namespace omnetpp

