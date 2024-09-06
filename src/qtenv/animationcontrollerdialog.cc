//==========================================================================
//  animationcontrollerdialog.cc - part of
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

#include "animationcontrollerdialog.h"
#include "ui_animationcontrollerdialog.h"

#include "qtenv.h"
#include "mainwindow.h"
#include "displayupdatecontroller.h"
#include <QtCore/QPoint>

namespace omnetpp {
namespace qtenv {

AnimationControllerDialog::AnimationControllerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnimationControllerDialog),
    duc(getQtenv()->getDisplayUpdateController())
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::Tool);

    ui->profileComboBox->addItems({"Step/Run", "Fast"});
    ui->profileComboBox->setItemData(0, RUNMODE_NORMAL);
    ui->profileComboBox->setItemData(1, RUNMODE_FAST);

    switchToRunMode(getQtenv()->getSimulationRunMode());
    displayMetrics();

    adjustSize();

    auto geom = geometry();
    geom.setTopLeft(getQtenv()->getDefaultStopDialogCorner() - QPoint(width(), 0));
    setGeometry(geom);

    // well, this is disgusting.
    connect(ui->minSpeedSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        auto selectedProfile = getSelectedProfile();

        double speed = getMinAnimSpeed();
        selectedProfile->minAnimationSpeed = speed;

        if (getMaxAnimSpeed() < speed)
            ui->maxSpeedSpinBox->adjust(value, true);

        displayMetrics();
    });

    connect(ui->maxSpeedSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        auto selectedProfile = getSelectedProfile();

        double speed = getMaxAnimSpeed();
        selectedProfile->maxAnimationSpeed = speed;

        if (getMinAnimSpeed() > speed)
            ui->minSpeedSpinBox->adjust(value, true);

        displayMetrics();
    });

    connect(ui->playbackSpeedSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [this](double value) {
        duc->setPlaybackSpeed(value, getSelectedProfile());
    });

    connect(ui->profileComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(displayControlValues()));
}

AnimationControllerDialog::~AnimationControllerDialog()
{
    delete ui;
}

RunModeProfile *AnimationControllerDialog::getSelectedProfile() {
    switch (ui->profileComboBox->currentData().toInt()) {
        case RUNMODE_NORMAL: return duc->getStepRunProfile();
        case RUNMODE_FAST:   return duc->getFastProfile();
        default: ASSERT(false);
    }
    return nullptr; // just to silence a warning
}

double AnimationControllerDialog::getMinAnimSpeed()
{
    double value = ui->minSpeedSpinBox->value();
    return value == ui->minSpeedSpinBox->minimum() ? std::nan("") : value;
}

double AnimationControllerDialog::getMaxAnimSpeed()
{
    double value = ui->maxSpeedSpinBox->value();
    return value == ui->maxSpeedSpinBox->minimum() ? std::nan("") : value;
}

void AnimationControllerDialog::switchToRunMode(RunMode mode)
{
    auto cb = ui->profileComboBox;

    int runIndex = cb->findData(RUNMODE_NORMAL);
    int fastIndex = cb->findData(RUNMODE_FAST);

    cb->setItemText(runIndex, QString("Step/Run") +
                    ((mode == RUNMODE_NORMAL || mode == RUNMODE_STEP) ? " [active]" : ""));
    cb->setItemText(fastIndex, QString("Fast") +
                    ((mode == RUNMODE_FAST) ? " [active]" : ""));

    if (mode == RUNMODE_FAST)
        cb->setCurrentIndex(cb->findData(RUNMODE_FAST));

    if (mode == RUNMODE_NORMAL || mode == RUNMODE_STEP)
        cb->setCurrentIndex(cb->findData(RUNMODE_NORMAL));

    displayControlValues();
}

void AnimationControllerDialog::displayMetrics()
{
    auto env = getQtenv();

    double animSpeed = duc->getAnimationSpeed();
    double simSpeed = duc->getCurrentSimulationSpeed();

    ui->currentFpsValueLabel->setText(QString::number(duc->getCurrentFps(), 'f', 0));
    ui->animationTimeValueLabel->setText(QString::number(duc->getAnimationTime(), 'f', 2));
    ui->simulationSpeedValueLabel->setText(QString::number(simSpeed, 'g', 4));
    ui->holdTimeValueLabel->setText(QString::number(env->getRemainingAnimationHoldTime(), 'f', 2));
    ui->animationSpeedValueLabel->setText(animSpeed == 0.0 ? "not set" : QString::number(animSpeed, 'g', 4));
    ui->refreshDisplayCountValueLabel->setText(QString::number(env->getRefreshDisplayCount()));

    ui->animationSpeedLabel->setStyleSheet(animSpeed == 0.0 ? "color: red" : "");
    ui->animationSpeedValueLabel->setStyleSheet(animSpeed == 0.0 ? "color: red" : "");

    double expectedSimSpeed = animSpeed * duc->getPlaybackSpeed();
    bool simSpeedMismatch = animSpeed != 0.0 && !duc->effectiveAnimationSpeedRecentlyChanged()
            && (simSpeed < expectedSimSpeed * 0.9 || simSpeed > expectedSimSpeed * 1.1);

    ui->simulationSpeedLabel->setStyleSheet(simSpeedMismatch ? "color: red" : "");
    ui->simulationSpeedValueLabel->setStyleSheet(simSpeedMismatch ? "color: red" : "");
}

void AnimationControllerDialog::displayControlValues()
{
    auto cb = ui->profileComboBox;
    cb->setCurrentText(cb->itemText(cb->currentIndex()));

    auto selectedProfile = getSelectedProfile();

    ui->minSpeedSpinBox->adjust(selectedProfile->minAnimationSpeed);
    ui->maxSpeedSpinBox->adjust(selectedProfile->maxAnimationSpeed);
    ui->playbackSpeedSpinBox->adjust(selectedProfile->playbackSpeed);
}

}  // namespace qtenv
}  // namespace omnetpp
