//==========================================================================
//  COMBOSELECTIONDIALOG.CC - part of
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

#include "comboselectiondialog.h"
#include "ui_comboselectiondialog.h"
#include "qtenv.h"
#include <QtCore/QDebug>

namespace omnetpp {
namespace qtenv {

ComboSelectionDialog::ComboSelectionDialog(QString netName, QStringList networkNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComboSelectionDialog)
{
    ui->setupUi(this);
    setFont(getQtenv()->getBoldFont());

    if (!netName.isEmpty() && !networkNames.contains(netName))
        ui->comboBox->addItem(netName);
    ui->comboBox->addItems(networkNames);
    if (!netName.isEmpty())
        ui->comboBox->setCurrentIndex(ui->comboBox->findText(netName));
}

ComboSelectionDialog::~ComboSelectionDialog()
{
    delete ui;
}

QString ComboSelectionDialog::getSelectedNetName()
{
    return ui->comboBox->currentText();
}

}  // namespace qtenv
}  // namespace omnetpp

