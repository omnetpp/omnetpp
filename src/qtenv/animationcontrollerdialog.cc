//==========================================================================
//  animationcontrollerdialog.cc - part of
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

#include "animationcontrollerdialog.h"
#include "ui_animationcontrollerdialog.h"

#include "qtenv.h"
#include "mainwindow.h"
#include "displayupdatecontroller.h"
#include <QPoint>

namespace omnetpp {
namespace qtenv {

AnimationControllerDialog::AnimationControllerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnimationControllerDialog),
    duc(getQtenv()->getDisplayUpdateController())
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::Tool);

    displayMetrics();

    adjustSize();

    auto geom = geometry();
    geom.setTopLeft(getQtenv()->getDefaultStopDialogCorner() - QPoint(width(), 0));
    setGeometry(geom);

    // well, this is disgusting.
    connect(ui->minFpsSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value) {
        if (ui->maxFpsSpinBox->value() < value)
            ui->maxFpsSpinBox->setValue(value);
        duc->setMinFps(ui->minFpsSpinBox->value());
    });

    connect(ui->maxFpsSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value) {
        if (ui->minFpsSpinBox->value() > value)
            ui->minFpsSpinBox->setValue(value);
        duc->setMaxFps(ui->maxFpsSpinBox->value());
    });

    connect(ui->playbackSpeedSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            duc, &DisplayUpdateController::setPlaybackSpeed);
}

AnimationControllerDialog::~AnimationControllerDialog()
{
    delete ui;
}

void AnimationControllerDialog::displayMetrics()
{
    auto env = getQtenv();

    ui->minFpsSpinBox->setValue(duc->getMinFps());
    ui->maxFpsSpinBox->setValue(duc->getMaxFps());

    ui->targetFpsValueLabel->setText(QString::number(duc->getTargetFps(), 'f', 0));
    ui->currentFpsValueLabel->setText(QString::number(duc->getCurrentFps(), 'f', 0));

    ui->animationTimeValueLabel->setText(QString::number(duc->getAnimationTime(), 'f', 2));
    ui->holdTimeValueLabel->setText(QString::number(env->getRemainingAnimationHoldTime(), 'f', 2));

    ui->animationSpeedValueLabel->setText(QString::number(duc->getAnimationSpeed(), 'g', 2));

    // don't want to influence the speed here programatically through the valueChanged signal
    bool blocked = ui->playbackSpeedSpinBox->blockSignals(true);

    // making it kindof exponential
    int magnitude = std::ceil(std::log10(duc->getPlaybackSpeed()*1.0001));
    ui->playbackSpeedSpinBox->setSingleStep(duc->getPlaybackSpeed() / 10);
    // the ifs are here to keep it spinning (except when magnitude changes...)
    if (ui->playbackSpeedSpinBox->decimals() != (4 -magnitude))
        ui->playbackSpeedSpinBox->setDecimals(4 - magnitude);
    if (ui->playbackSpeedSpinBox->minimum() != duc->getMinPlaybackSpeed())
        ui->playbackSpeedSpinBox->setMinimum(duc->getMinPlaybackSpeed());
    if (ui->playbackSpeedSpinBox->maximum() != duc->getMaxPlaybackSpeed())
        ui->playbackSpeedSpinBox->setMaximum(duc->getMaxPlaybackSpeed());
    ui->playbackSpeedSpinBox->setValue(duc->getPlaybackSpeed());

    ui->playbackSpeedSpinBox->blockSignals(blocked);

    ui->refreshDisplayCountValueLabel->setText(QString::number(env->getRefreshDisplayCount()));
}

} // namespace qtenv
} // namespace omnetpp
