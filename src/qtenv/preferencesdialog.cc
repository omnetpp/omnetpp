//==========================================================================
//  PREFERENCESDIALOG.CC - part of
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

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "qtenv.h"
#include "mainwindow.h"
#include "messageanimator.h"
#include "inspectorutil.h"

#include <QtCore/QDebug>

namespace omnetpp {
namespace qtenv {

PreferencesDialog::PreferencesDialog(eTab defaultPage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    setFont(getQtenv()->getBoldFont());

    if (defaultPage == TAB_NOTDEFINED) {
        QVariant variant = getQtenv()->getPref("preferences-dialog-page");
        ui->tabWidget->setCurrentIndex(variant.isValid() ? variant.value<int>() : 0);
    }
    else
        ui->tabWidget->setCurrentIndex(defaultPage);

    init();
}

// TODO: use default values passed to getPref, and store ref to opt or ptr to env
void PreferencesDialog::init()
{
    // General tab
    QVariant variant = getQtenv()->getPref("confirm-exit");
    ui->confirmExit->setChecked(variant.isValid() ? variant.value<bool>() : true);
    variant = getQtenv()->getPref("ned-doc-tooltips");
    ui->nedDocTooltips->setChecked(variant.isValid() ? variant.value<bool>() : true);
    ui->express->setText(QString::number(getQtenv()->opt->updateFreqExpress));

    // XXX This conversion is fragile, it depends on the order of
    // both the enum values, and the items in the checkbox.
    ui->hideNameSpace->setCurrentIndex(getQtenv()->opt->stripNamespace);

    // Logs tab
    ui->initBanners->setChecked(getQtenv()->opt->printInitBanners);
    ui->eventBanners->setChecked(getQtenv()->opt->printEventBanners);
    ui->shortBanners->setChecked(getQtenv()->opt->shortBanners);
    ui->noLoggingRefreshDisplay->setChecked(getQtenv()->opt->noLoggingRefreshDisplay);
    ui->logPrefix->setText(getQtenv()->opt->logFormat.c_str());
    ui->overall->setText(QString::number(getQtenv()->getLogBuffer()->getMaxNumEntries()));

    for (int i = 0; i < ui->logLevel->count(); ++i)
        ui->logLevel->setItemData(i, cLog::resolveLogLevel(ui->logLevel->itemText(i).toLatin1()));
    ui->logLevel->setCurrentIndex(ui->logLevel->findData(getQtenv()->opt->logLevel));

    // Layouting tab
    switch(getQtenv()->opt->layouterChoice) {
        case LAYOUTER_FAST:     ui->fastRadio->setChecked(true);     break;
        case LAYOUTER_ADVANCED: ui->advancedRadio->setChecked(true); break;
        case LAYOUTER_AUTO:     ui->adaptiveRadio->setChecked(true); break;
    }
    ui->showLayouting->setChecked(getQtenv()->opt->showLayouting);
    ui->arrange->setChecked(getQtenv()->opt->arrangeVectorConnections);
    variant = getQtenv()->getPref("layout-may-change-zoom");
    ui->allowZoom->setChecked(variant.isValid() ? variant.value<bool>() : false);

    // Animation tab
    ui->animMsg->setChecked(getQtenv()->opt->animationEnabled);
    ui->animBroadcast->setChecked(getQtenv()->getMessageAnimator()->getBroadcastAnimation());
    ui->showMarker->setChecked(getQtenv()->opt->showNextEventMarkers);
    ui->showArrows->setChecked(getQtenv()->opt->showSendDirectArrows);
    ui->showBubbles->setChecked(getQtenv()->opt->showBubbles);
    ui->animCalls->setChecked(getQtenv()->opt->animateMethodCalls);
    ui->dispName->setChecked(getQtenv()->opt->animationMsgNames);
    ui->dispClass->setChecked(getQtenv()->opt->animationMsgClassNames);
    ui->colorMsg->setChecked(getQtenv()->opt->animationMsgColors);
    ui->penguinMode->setChecked(getQtenv()->opt->penguinMode);
    ui->delayEdit->setText(QString::number(getQtenv()->opt->methodCallAnimDuration));
    // there is only one link in that label, and we know what it should do
    connect(ui->animationSpeedNoteLabel, &QLabel::linkActivated,
            getQtenv()->getMainWindow()->getShowAnimationParamsDialogAction(), &QAction::trigger);

    // Filtering tab
    variant = getQtenv()->getPref("timeline-wantselfmsgs");
    ui->selfMsg->setChecked(variant.isValid() ? variant.value<bool>() : true);
    variant = getQtenv()->getPref("timeline-wantnonselfmsgs");
    ui->nonSelfMsg->setChecked(variant.isValid() ? variant.value<bool>() : true);
    variant = getQtenv()->getPref("timeline-wantsilentmsgs");
    ui->silentMsg->setChecked(variant.isValid() ? variant.value<bool>() : true);
    ui->excludMsgEdit->setText(getQtenv()->getSilentEventFilters());

    // Fonts tab
    QFont interfaceFont = getQtenv()->getBoldFont();
    QFont timelineFont = getQtenv()->getTimelineFont();
    QFont canvasFont = getQtenv()->getCanvasFont();
    QFont logBoxFont = getQtenv()->getLogFont();
    QFont timeFont = getQtenv()->getTimeFont();

    setFontsTabFonts(interfaceFont, timelineFont, canvasFont, logBoxFont, timeFont);
    connect(ui->restoreButton, SIGNAL(clicked()), this, SLOT(restoreDefaultFonts()));

    adjustSize();
}

void PreferencesDialog::setFontsTabFonts(const QFont &interfaceFont, const QFont &timelineFont,
                                         const QFont &canvasFont, const QFont &logBoxFont, const QFont &timeFont)
{
    ui->userInterfaceBox->setCurrentFont(interfaceFont);
    ui->userInterfaceNumBox->setValue(interfaceFont.pointSize());
    ui->timeLineBox->setCurrentFont(timelineFont);
    ui->timeLineNumBox->setValue(timelineFont.pointSize());
    ui->canvasBox->setCurrentFont(canvasFont);
    ui->canvasNumBox->setValue(canvasFont.pointSize());
    ui->logBox->setCurrentFont(logBoxFont);
    ui->logNumBox->setValue(logBoxFont.pointSize());
    ui->timeBox->setCurrentFont(timeFont);
    ui->timeNumBox->setValue(timeFont.pointSize());
}

void PreferencesDialog::restoreDefaultFonts()
{
    QFont interfaceFont = getQtenv()->getDefaultBoldFont();
    QFont timelineFont = getQtenv()->getDefaultTimelineFont();
    QFont canvasFont = getQtenv()->getDefaultCanvasFont();
    QFont logBoxFont = getQtenv()->getDefaultLogFont();
    QFont timeFont = getQtenv()->getDefaultTimeFont();

    setFontsTabFonts(interfaceFont, timelineFont, canvasFont, logBoxFont, timeFont);
}

void PreferencesDialog::accept()
{
    getQtenv()->opt->updateFreqExpress = ui->express->text().toLong();
    getQtenv()->setSilentEventFilters(ui->excludMsgEdit->toPlainText().toStdString().c_str());
    QString n = ui->overall->text();
    if (n.isEmpty() || n.toInt() != 0) {
        int historySize = n.toInt();
        if (!n.isEmpty() && historySize < 100)
            historySize = 100;
        getQtenv()->getLogBuffer()->setMaxNumEntries(historySize);
    }

    getQtenv()->opt->printEventBanners = ui->eventBanners->isChecked();
    getQtenv()->opt->printInitBanners = ui->initBanners->isChecked();
    getQtenv()->opt->shortBanners = ui->shortBanners->isChecked();
    getQtenv()->opt->noLoggingRefreshDisplay = ui->noLoggingRefreshDisplay->isChecked();

    std::string logFormat = ui->logPrefix->text().toStdString();
    try {
        getQtenv()->setLogFormat(logFormat.c_str());
        getQtenv()->opt->logFormat = logFormat.c_str();
    }
    catch (std::exception& e) {
        getQtenv()->confirm(Qtenv::ERROR, (std::string("Error in log prefix format: ") + e.what()).c_str());
    }

    LogLevel logLevel = LogLevel(ui->logLevel->currentData().toInt());
    getQtenv()->opt->logLevel = logLevel;
    getQtenv()->setLogLevel(logLevel);

    getQtenv()->opt->animationEnabled = ui->animMsg->isChecked();
    getQtenv()->getMessageAnimator()->setBroadcastAnimation(ui->animBroadcast->isChecked());
    getQtenv()->opt->showNextEventMarkers = ui->showMarker->isChecked();
    getQtenv()->opt->showSendDirectArrows = ui->showArrows->isChecked();
    getQtenv()->opt->animateMethodCalls = ui->animCalls->isChecked();
    getQtenv()->opt->methodCallAnimDuration = ui->delayEdit->text().toInt();
    getQtenv()->opt->animationMsgNames = ui->dispName->isChecked();
    getQtenv()->opt->animationMsgClassNames = ui->dispClass->isChecked();
    getQtenv()->opt->animationMsgColors = ui->colorMsg->isChecked();

    getQtenv()->opt->penguinMode = ui->penguinMode->isChecked();
    getQtenv()->opt->showLayouting = ui->showLayouting->isChecked();
    getQtenv()->opt->layouterChoice =
        ui->fastRadio->isChecked() ? LAYOUTER_FAST :
        ui->advancedRadio->isChecked() ? LAYOUTER_ADVANCED :
        LAYOUTER_AUTO;

    getQtenv()->opt->arrangeVectorConnections = ui->arrange->isChecked();
    getQtenv()->opt->showBubbles = ui->showBubbles->isChecked();
    getQtenv()->setPref("confirm-exit", ui->confirmExit->isChecked());
    getQtenv()->setPref("ned-doc-tooltips", ui->nedDocTooltips->isChecked());

    // TODO: this conversion is fragile, it depends on the order of the enum which might change
    getQtenv()->opt->stripNamespace = StripNamespace(ui->hideNameSpace->currentIndex());
    getQtenv()->setPref("layout-may-change-zoom", ui->allowZoom->isChecked());
    getQtenv()->setPref("timeline-wantselfmsgs", ui->selfMsg->isChecked());
    getQtenv()->setPref("timeline-wantnonselfmsgs", ui->nonSelfMsg->isChecked());
    getQtenv()->setPref("timeline-wantsilentmsgs", ui->silentMsg->isChecked());

    // Fonts tab
    QFont font = ui->userInterfaceBox->currentFont();
    font.setPointSize(ui->userInterfaceNumBox->value());
    getQtenv()->setBoldFont(font);

    font = ui->timeLineBox->currentFont();
    font.setPointSize(ui->timeLineNumBox->value());
    getQtenv()->setTimelineFont(font);

    font = ui->canvasBox->currentFont();
    font.setPointSize(ui->canvasNumBox->value());
    getQtenv()->setCanvasFont(font);

    font = ui->logBox->currentFont();
    font.setPointSize(ui->logNumBox->value());
    getQtenv()->setLogFont(font);

    font = ui->timeBox->currentFont();
    font.setPointSize(ui->timeNumBox->value());
    getQtenv()->setTimeFont(font);

    getQtenv()->updateQtFonts();

    getQtenv()->callRefreshInspectors();

    getQtenv()->setPref("preferences-dialog-page", ui->tabWidget->currentIndex());

    getQtenv()->storeOptsInPrefs();

    QDialog::accept();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

}  // namespace qtenv
}  // namespace omnetpp
