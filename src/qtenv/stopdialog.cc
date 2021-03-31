//==========================================================================
//  STOPDIALOG.CC - part of
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

#include "stopdialog.h"
#include "ui_stopdialog.h"
#include "qtenv.h"
#include <QtGui/QKeyEvent>

namespace omnetpp {
namespace qtenv {

StopDialog::StopDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StopDialog)
{
    ui->setupUi(this);
    ui->checkBox->setChecked(getQtenv()->opt->autoupdateInExpress);

    applyAutoupdate();
}

StopDialog::~StopDialog()
{
    delete ui;
}

void StopDialog::onClickStop()
{
    getQtenv()->setStopSimulationFlag();
    close();
}

void StopDialog::applyAutoupdate()
{
    bool autoUpdate = ui->checkBox->isChecked();
    getQtenv()->opt->autoupdateInExpress = autoUpdate;
    ui->updateButton->setDisabled(autoUpdate);
}

void StopDialog::onClickUpdate()
{
    getQtenv()->callRefreshDisplaySafe();
    getQtenv()->callRefreshInspectors();
}

void StopDialog::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Escape:
        case Qt::Key_F8:
            getQtenv()->setStopSimulationFlag();
            break;

        default:
            break;
    }
}

void StopDialog::closeEvent(QCloseEvent *event)
{
    if (getQtenv()->isExpressMode())
        getQtenv()->setStopSimulationFlag();

    // this is not used if the restoring line below is commented out
    getQtenv()->setPref("stopdialog-geom", geometry());
    QDialog::closeEvent(event);
}

void StopDialog::show()
{
    setFont(getQtenv()->getBoldFont());
    QRect geom;

    /* Uncomment the next line to make the dialog geometry persistent.
       In my opinion it was weird and unnatural. This way it will
       always pop up under the moduleinspector toolbar. I hope. */
    // geom = getQtenv()->getPref("stopdialog-geom").toRect();

    if (!geom.isValid()) {
        geom.setTopLeft(getQtenv()->getDefaultStopDialogCorner() - QPoint(sizeHint().width(), 0));
        geom.setSize(sizeHint());
    }

    setGeometry(geom);

    QDialog::show();
}

}  // namespace qtenv
}  // namespace omnetpp

