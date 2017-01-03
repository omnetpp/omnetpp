//==========================================================================
//  LAYERSDIALOG.CC - part of
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

#include "layersdialog.h"
#include "ui_layersdialog.h"
#include <QCheckBox>
#include <QVBoxLayout>
#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

LayersDialog::LayersDialog(QString allTags, QString enabledTags, QString exceptTags, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LayersDialog)
{
    ui->setupUi(this);
    setFont(getQtenv()->getBoldFont());

    this->allTags = QString(allTags).split(' ', QString::SkipEmptyParts);
    this->enabledTags = QString(enabledTags).split(' ', QString::SkipEmptyParts);
    this->exceptTags = QString(exceptTags).split(' ', QString::SkipEmptyParts);

    QVBoxLayout *showFigLayout = static_cast<QVBoxLayout *>(ui->showFigWidget->layout());
    QVBoxLayout *hideFigLayout = static_cast<QVBoxLayout *>(ui->hideFigWidget->layout());

    for (QString tag : this->allTags) {
        QCheckBox *chkBox = new QCheckBox(tag);
        QCheckBox *exceptChkBox = new QCheckBox(tag);

        bool isEnabledTagsContains = this->enabledTags.contains(tag);
        chkBox->setChecked(isEnabledTagsContains);

        exceptChkBox->setVisible(!isEnabledTagsContains);
        exceptChkBox->setChecked(this->exceptTags.contains(tag));

        showFigLayout->addWidget(chkBox);
        hideFigLayout->addWidget(exceptChkBox);

        connect(chkBox, SIGNAL(clicked(bool)), this, SLOT(clickOnShowFig(bool)));
    }

    showFigLayout->addStretch();
    hideFigLayout->addStretch();
}

void LayersDialog::clickOnShowFig(bool checked)
{
    QCheckBox *chkBox = static_cast<QCheckBox *>(sender());
    int index = ui->showFigWidget->layout()->indexOf(chkBox);
    QCheckBox *exceptChkBox = static_cast<QCheckBox *>(ui->hideFigWidget->layout()->itemAt(index)->widget());

    exceptChkBox->setVisible(!checked);
    exceptChkBox->setChecked(false);
}

void LayersDialog::accept()
{
    enabledTags.clear();
    for (QObject *child : ui->showFigWidget->children()) {
        QCheckBox *chkBox = dynamic_cast<QCheckBox *>(child);
        if (chkBox && chkBox->isChecked())
            enabledTags.push_back(chkBox->text());
    }

    exceptTags.clear();
    for (QObject *child : ui->hideFigWidget->children()) {
        QCheckBox *chkBox = dynamic_cast<QCheckBox *>(child);
        if (chkBox && chkBox->isVisible() && chkBox->isChecked())
            exceptTags.push_back(chkBox->text());
    }
    QDialog::accept();
}

LayersDialog::~LayersDialog()
{
    delete ui;
}

}  // namespace qtenv
}  // namespace omnetpp

