//==========================================================================
//  RUNUNTILDIALOG.CC - part of
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

#include "rununtildialog.h"
#include "ui_rununtildialog.h"
#include "omnetpp/cfutureeventset.h"
#include "qtutil.h"
#include "qtenvdefs.h"

#include <QtCore/QDebug>

namespace omnetpp {
namespace qtenv {

RunUntilDialog::RunUntilDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunUntilDialog)
{
    ui->setupUi(this);
    setFont(getQtenv()->getBoldFont());

    // collect FES messages for combo
    msgid_t lastMsgId = getQtenv()->getPref("rununtil-msg", -1).value<long long>();
    QList<cObject *> msgPtrs = fesEvents(1000, 0, 1, 1, 1);  // exclude non-message events
    ui->msgCombo->addItem("");
    for (cObject *ptr : msgPtrs) {
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%p", (void *)ptr);
        QString msgLabel = ptr->getFullName() + QString(" (") + getObjectShortTypeName(ptr) + "), " +
                ptr->str().c_str() + " -- " + buffer;
        ui->msgCombo->addItem(msgLabel, reinterpret_cast<quintptr>(ptr));

        // restore last value
        if (lastMsgId == static_cast<cMessage *>(ptr)->getId())
            ui->msgCombo->setCurrentIndex(ui->msgCombo->count() - 1);
    }

    ui->stopCheckbox->setChecked(getQtenv()->getPref("stoponmsgcancel", true).value<bool>());
    ui->timeEdit->setText(getQtenv()->getPref("rununtil-time", "").value<QString>());
    ui->eventEdit->setText(getQtenv()->getPref("rununtil-event", "").value<QString>());
    ui->modeComboBox->setCurrentIndex(getQtenv()->getPref("rununtil-mode", 0).value<int>());
}

QList<cObject *> RunUntilDialog::fesEvents(int maxNum, bool wantEvents, bool wantSelfMsgs, bool wantNonSelfMsgs, bool wantSilentMsgs)
{
    QList<cObject *> objects;
    cFutureEventSet *fes = getSimulation()->getFES();
    int fesLen = fes->getLength();
    for (int i = 0; maxNum > 0 && i < fesLen; i++, maxNum--) {
        cEvent *event = fes->get(i);
        if (!event->isMessage()) {
            if (!wantEvents)
                continue;
        }
        else {
            cMessage *msg = (cMessage *)event;
            if (msg->isSelfMessage() ? !wantSelfMsgs : !wantNonSelfMsgs)
                continue;
            if (!wantSilentMsgs && getQtenv()->isSilentEvent(msg))
                continue;
        }

        objects.push_back(event);
    }

    return objects;
}

simtime_t RunUntilDialog::getTime()
{
    QString time = ui->timeEdit->text();
    return time.isEmpty() ? SimTime() : SimTime::parse(ui->timeEdit->text().toStdString().c_str());
}

eventnumber_t RunUntilDialog::getEventNumber()
{
    return ui->eventEdit->text().toInt();
}

cObject *RunUntilDialog::getMessage()
{
    return reinterpret_cast<cObject *>(ui->msgCombo->itemData(ui->msgCombo->currentIndex()).value<quintptr>());
}

RunMode RunUntilDialog::getMode()
{
    return RunMode(ui->modeComboBox->currentIndex() + 1);
}

bool RunUntilDialog::stopOnMsgCancel()
{
    return ui->stopCheckbox->isChecked();
}

void RunUntilDialog::accept()
{
    getQtenv()->setPref("rununtil-time", ui->timeEdit->text());
    getQtenv()->setPref("rununtil-event", ui->eventEdit->text());

    cMessage *msg = reinterpret_cast<cMessage *>(ui->msgCombo->itemData(ui->msgCombo->currentIndex()).value<quintptr>());
    getQtenv()->setPref("rununtil-msg", msg ? (long long)msg->getId() : -1);
    getQtenv()->setPref("rununtil-mode", ui->modeComboBox->currentIndex());
    getQtenv()->setPref("stoponmsgcancel", ui->stopCheckbox->isChecked());
    QDialog::accept();
}

RunUntilDialog::~RunUntilDialog()
{
    delete ui;
}

}  // namespace qtenv
}  // namespace omnetpp

