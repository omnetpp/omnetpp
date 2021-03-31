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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>
#include "qtenv.h"
#include "modulecanvasviewer.h"
#include "canvasrenderer.h"

namespace omnetpp {
namespace qtenv {

static const char *geomPrefKey = "canvas-layers-dialog-geom";

LayersDialog::LayersDialog(ModuleCanvasViewer *canvasViewer, QWidget *parent) :
    QDialog(parent),
    canvasViewer(canvasViewer),
    canvasRenderer(canvasViewer->getCanvasRenderer()),
    ui(new Ui::LayersDialog)
{
    ui->setupUi(this);
    setFont(getQtenv()->getBoldFont());

    originalEnabledTags = canvasRenderer->getEnabledTags().c_str();
    originalExceptTags = canvasRenderer->getExceptTags().c_str();

    enabledTags = originalEnabledTags.split(' ', QString::SkipEmptyParts);
    exceptTags = originalExceptTags.split(' ', QString::SkipEmptyParts);

    auto allTags = canvasRenderer->getAllTagsAsVector();

    QVBoxLayout *showFigLayout = static_cast<QVBoxLayout *>(ui->showFigWidget->layout());
    QVBoxLayout *hideFigLayout = static_cast<QVBoxLayout *>(ui->hideFigWidget->layout());

    for (std::string t : allTags) {
        QString tag(t.c_str());

        QCheckBox *enabledChkBox = new QCheckBox(tag);
        QCheckBox *exceptChkBox = new QCheckBox(tag);

        bool tagEnabled = enabledTags.contains(tag);
        enabledChkBox->setChecked(tagEnabled);

        exceptChkBox->setVisible(!tagEnabled);
        exceptChkBox->setChecked(exceptTags.contains(tag));

        showFigLayout->addWidget(enabledChkBox);
        hideFigLayout->addWidget(exceptChkBox);

        connect(enabledChkBox, SIGNAL(clicked(bool)), this, SLOT(onEnabledCheckBoxClicked(bool)));
        connect(exceptChkBox, SIGNAL(clicked(bool)), this, SLOT(onExceptCheckBoxClicked(bool)));
    }

    showFigLayout->addStretch();
    hideFigLayout->addStretch();

    QRect geom = getQtenv()->getPref(geomPrefKey, QRect()).toRect();
    if (geom.isValid())
        setGeometry(geom);
}

void LayersDialog::onEnabledCheckBoxClicked(bool checked)
{
    QCheckBox *chkBox = static_cast<QCheckBox *>(sender());
    int index = ui->showFigWidget->layout()->indexOf(chkBox);
    QCheckBox *exceptChkBox = static_cast<QCheckBox *>(ui->hideFigWidget->layout()->itemAt(index)->widget());

    exceptChkBox->setVisible(!checked);
    exceptChkBox->setChecked(false);

    applyTagFilters();
}

void LayersDialog::onExceptCheckBoxClicked(bool checked)
{
    applyTagFilters();
}

void LayersDialog::applyTagFilters()
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

    canvasRenderer->setEnabledTags(enabledTags.join(" ").toStdString().c_str());
    canvasRenderer->setExceptTags(exceptTags.join(" ").toStdString().c_str());

    canvasViewer->redraw();
}

void LayersDialog::accept()
{
    applyTagFilters();
    QDialog::accept();
}

void LayersDialog::reject()
{
    canvasRenderer->setEnabledTags(originalEnabledTags.toStdString().c_str());
    canvasRenderer->setExceptTags(originalExceptTags.toStdString().c_str());

    canvasViewer->redraw();

    QDialog::reject();
}

LayersDialog::~LayersDialog()
{
    getQtenv()->setPref(geomPrefKey, geometry());
    delete ui;
}

}  // namespace qtenv
}  // namespace omnetpp

