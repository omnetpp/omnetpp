//==========================================================================
//  STOPDIALOG.CC - part of
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

#include "stopdialog.h"
#include "ui_stopdialog.h"
#include "qtenv.h"
#include <QKeyEvent>

StopDialog::StopDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StopDialog)
{
    ui->setupUi(this);

    stopDialogAutoupdate();
}

StopDialog::~StopDialog()
{
    delete ui;
}

void StopDialog::onClickStop()
{
    qtenv::getTkenv()->setStopSimulationFlag();
    close();
}

void StopDialog::stopDialogAutoupdate()
{
    ui->updateButton->setDisabled(ui->checkBox->isChecked());
    if (ui->checkBox->isChecked())
        qtenv::getTkenv()->opt->autoupdateInExpress = true;
    else
        qtenv::getTkenv()->opt->autoupdateInExpress = false;
}

void StopDialog::onClickUpdate()
{
    qtenv::getTkenv()->refreshInspectors();
}

void StopDialog::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Escape:
        case Qt::Key_F8:
            qtenv::getTkenv()->setStopSimulationFlag();
            break;

        default:
            break;
    }
}

