//==========================================================================
//  PREFERENCESDIALOG.CC - part of
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

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "qtenv.h"
#include "inspectorutil.h"

#include <QDebug>

namespace omnetpp {
namespace qtenv {

PreferencesDialog::PreferencesDialog(int defaultPage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    setFont(getQtenv()->getBoldFont());

    if(defaultPage == InspectorUtil::TAB_NOTDEFINED)
    {
        QVariant variant = getQtenv()->getPref("preferences-dialog-page");
        ui->tabWidget->setCurrentIndex(variant.isValid() ? variant.value<int>() : 0);
    }
    else
        ui->tabWidget->setCurrentIndex(defaultPage);

    init();
}

void PreferencesDialog::init()
{
    // General tab
    QVariant variant = getQtenv()->getPref("keep-inspectors-on-top");
    ui->keepOnTop->setChecked(variant.isValid() ? variant.value<bool>() : false);
    variant = getQtenv()->getPref("reuse-inspectors");
    ui->reuseInsp->setChecked(variant.isValid() ? variant.value<bool>() : false);
    variant = getQtenv()->getPref("confirm-exit");
    ui->confirmExit->setChecked(variant.isValid() ? variant.value<bool>() : false);
    ui->initBanners->setChecked(getQtenv()->opt->printInitBanners);
    ui->eventBanners->setChecked(getQtenv()->opt->printEventBanners);
    ui->shortBanners->setChecked(getQtenv()->opt->shortBanners);
    ui->logPrefix->setText(getQtenv()->opt->logFormat.c_str());
    ui->fast->setText(QString::number(getQtenv()->opt->updateFreqFast));
    ui->express->setText(QString::number(getQtenv()->opt->updateFreqExpress));
    ui->scrollback->setText(QString::number(getQtenv()->opt->scrollbackLimit));
    ui->overall->setText(QString::number(getQtenv()->getLogBuffer()->getMaxNumEntries()));
    ui->logLevel->setCurrentIndex(getQtenv()->opt->logLevel);
    ui->hideNameSpace->setCurrentIndex(getQtenv()->opt->stripNamespace);

    // Layouting tab
    switch(getQtenv()->opt->layouterChoice) {
        case LAYOUTER_FAST:     ui->fastRadio->setChecked(true);     break;
        case LAYOUTER_ADVANCED: ui->advancedRadio->setChecked(true); break;
        case LAYOUTER_AUTO:     ui->adaptiveRadio->setChecked(true); break;
    }
    ui->showLayouting->setChecked(getQtenv()->opt->showLayouting);
    ui->arrange->setChecked(getQtenv()->opt->arrangeVectorConnections);
    variant = getQtenv()->getPref("layout-may-resize-window");
    ui->allowResize->setChecked(variant.isValid() ? variant.value<bool>() : false);
    variant = getQtenv()->getPref("layout-may-change-zoom");
    ui->allowZoom->setChecked(variant.isValid() ? variant.value<bool>() : false);
    ui->minIconSizeEdit->setText(QString::number(getQtenv()->opt->iconMinimumSize));

    // Animation tab
    ui->animMsg->setChecked(getQtenv()->opt->animationEnabled);
    ui->animSpeedSlider->setValue(getQtenv()->opt->animationSpeed*100);
    connect(ui->animSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(onAnimationSliderMoved(int)));
    variant = getQtenv()->getPref("concurrent-anim");
    ui->animBroadcast->setChecked(variant.isValid() ? variant.value<bool>() : false);
    ui->showMarker->setChecked(getQtenv()->opt->showNextEventMarkers);
    ui->showArrows->setChecked(getQtenv()->opt->showSendDirectArrows);
    ui->showBubbles->setChecked(getQtenv()->opt->showBubbles);
    ui->animCalls->setChecked(getQtenv()->opt->animateMethodCalls);
    ui->dispName->setChecked(getQtenv()->opt->animationMsgNames);
    ui->dispClass->setChecked(getQtenv()->opt->animationMsgClassNames);
    ui->colorMsg->setChecked(getQtenv()->opt->animationMsgColors);
    ui->penguinMode->setChecked(getQtenv()->opt->penguinMode);
    ui->delayEdit->setText(QString::number(getQtenv()->opt->methodCallAnimDelay));

// to follow speed changes from other sources (possibly the mainwindow)
    connect(getQtenv(), SIGNAL(animationSpeedChanged(float)), this, SLOT(onAnimationSpeedChanged(float)));

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
    getQtenv()->opt->updateFreqFast = ui->fast->text().toLong();
    getQtenv()->opt->updateFreqExpress = ui->express->text().toLong();
    getQtenv()->setSilentEventFilters(ui->excludMsgEdit->toPlainText().toStdString().c_str());
    QString n = ui->overall->text();
    if(n.isEmpty() || n.toInt() != 0)
    {
        int historySize = n.toInt();
        if(!n.isEmpty() && historySize < 100)
            historySize = 100;
        getQtenv()->getLogBuffer()->setMaxNumEntries(historySize);
    }

    n = ui->scrollback->text();
    if(n.isEmpty() || n.toInt() != 0)
    {
        int scrollBack = n.toInt();
        if(!n.isEmpty() && scrollBack < 500)
             scrollBack = 500;
        getQtenv()->opt->scrollbackLimit = scrollBack;
    }

    getQtenv()->opt->printEventBanners = ui->eventBanners->isChecked();
    getQtenv()->opt->printInitBanners = ui->initBanners->isChecked();
    getQtenv()->opt->shortBanners = ui->shortBanners->isChecked();

    std::string logFormat = ui->logPrefix->text().toStdString();
    getQtenv()->setLogFormat(logFormat.c_str());
    getQtenv()->opt->logFormat = logFormat.c_str();

    // TODO: this conversion is fragile, it depends on the order of the enum which might change
    LogLevel logLevel = LogLevel(ui->logLevel->currentIndex());
    getQtenv()->opt->logLevel = logLevel;
    getQtenv()->setLogLevel(logLevel);

    getQtenv()->opt->animationEnabled = ui->animMsg->isChecked();
    getQtenv()->setPref("concurrent-anim", ui->animBroadcast->isChecked());
    getQtenv()->opt->showNextEventMarkers = ui->showMarker->isChecked();
    getQtenv()->opt->showSendDirectArrows = ui->showArrows->isChecked();
    getQtenv()->opt->animateMethodCalls = ui->animCalls->isChecked();
    getQtenv()->opt->methodCallAnimDelay = ui->delayEdit->text().toInt();
    getQtenv()->opt->animationMsgNames = ui->dispName->isChecked();
    getQtenv()->opt->animationMsgClassNames = ui->dispClass->isChecked();
    getQtenv()->opt->animationMsgColors = ui->colorMsg->isChecked();

    getQtenv()->opt->iconMinimumSize = std::min(40, std::max(1, ui->minIconSizeEdit->text().toInt()));
    getQtenv()->opt->penguinMode = ui->penguinMode->isChecked();
    getQtenv()->opt->showLayouting = ui->showLayouting->isChecked();
    getQtenv()->opt->layouterChoice =
        ui->fastRadio->isChecked() ? LAYOUTER_FAST :
        ui->advancedRadio->isChecked() ? LAYOUTER_ADVANCED :
        LAYOUTER_AUTO;

    getQtenv()->opt->arrangeVectorConnections = ui->arrange->isChecked();
    getQtenv()->opt->showBubbles = ui->showBubbles->isChecked();
    getQtenv()->setAnimationSpeed(ui->animSpeedSlider->value() / 100.0);
    getQtenv()->setPref("keep-inspectors-on-top", ui->keepOnTop->isChecked());
    getQtenv()->setPref("reuse-inspectors", ui->reuseInsp->isChecked());
    getQtenv()->setPref("confirm-exit", ui->confirmExit->isChecked());
    getQtenv()->opt->stripNamespace = StripNamespace(ui->hideNameSpace->currentIndex());
    getQtenv()->setPref("layout-may-resize-window", ui->allowResize->isChecked());
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

    getQtenv()->redrawInspectors();

    getQtenv()->setPref("preferences-dialog-page", ui->tabWidget->currentIndex());

    QDialog::accept();
}

void PreferencesDialog::onAnimationSliderMoved(int value) {
    if (value > 95 && value < 105) value = 100;
    getQtenv()->setAnimationSpeed(value / 100.0);
}

void PreferencesDialog::onAnimationSpeedChanged(float speed) {
    ui->animSpeedSlider->setValue(speed * 100);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

} // namespace qtenv
} // namespace omnetpp
