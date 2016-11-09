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

    ui->minFpsSpinBox->setValue(duc->getMinFps());
    ui->maxFpsSpinBox->setValue(duc->getMaxFps());

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
    ui->playbackSpeedValueLabel->setText(QString::number(duc->getPlaybackSpeed(), 'f', 2));

    ui->refreshDisplayCountValueLabel->setText(QString::number(env->getRefreshDisplayCount()));
}

} // namespace qtenv
} // namespace omnetpp
