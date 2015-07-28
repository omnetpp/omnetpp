//==========================================================================
//  RUNUNTILDIALOG.CC - part of
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

#include "rununtildialog.h"
#include "ui_rununtildialog.h"
#include "omnetpp/cfutureeventset.h"
#include "qtutil.h"
#include "qtenvdefs.h"

#include <QDebug>

namespace omnetpp {
namespace qtenv {

RunUntilDialog::RunUntilDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunUntilDialog)
{
    ui->setupUi(this);

    // collect FES messages for combo
    QList<cObject*> msgPtrs = fesEvents(1000, 0, 1, 1, 1); // exclude non-message events
    ui->msgCombo->addItem("");
    for(cObject *ptr : msgPtrs)
    {
        char buffer[50];
        sprintf(buffer, "%p", (void *)ptr);
        QString msgLabel = ptr->getFullName() + QString(" (") + getObjectShortTypeName(ptr) + "), " +
                ptr->info().c_str() + " -- " + buffer;
        ui->msgCombo->addItem(msgLabel, reinterpret_cast<quintptr>(ptr));
    }

    ui->stopCheckbox->setChecked(getQtenv()->opt->stopOnMsgCancel);

    // restore last values
    QVariant variant = getQtenv()->getPref("rununtil-msg");
    cObject *lastMsg = variant.isValid() ? variant.value<cObject*>() : nullptr;
    if(lastMsg && msgPtrs.indexOf(lastMsg)!=-1)
    {
        char buffer[50];
        sprintf(buffer, "%p", (void *)lastMsg);
        QString msgLabel = lastMsg->getFullName() + QString(" (") + getObjectShortTypeName(lastMsg) + "), " +
                lastMsg->info().c_str() + " -- " + buffer;
        ui->msgCombo->addItem(msgLabel, reinterpret_cast<quintptr>(lastMsg));
    }

    variant = getQtenv()->getPref("rununtil-time");
    ui->timeEdit->setText(variant.isValid() ? variant.value<QString>() : "");
    variant = getQtenv()->getPref("rununtil-event");
    ui->eventEdit->setText(variant.isValid() ? variant.value<QString>() : "");
    variant = getQtenv()->getPref("rununtil-mode");
    ui->modeComboBox->setCurrentIndex(variant.isValid() ? variant.value<int>() : 0);
}

QList<cObject*> RunUntilDialog::fesEvents(int maxNum, bool wantEvents, bool wantSelfMsgs, bool wantNonSelfMsgs, bool wantSilentMsgs)
{
    QList<cObject*> objects;
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

cObject* RunUntilDialog::getMessage()
{
    return reinterpret_cast<cObject*>(ui->msgCombo->itemData(ui->msgCombo->currentIndex()).value<quintptr>());
}

Qtenv::eRunMode RunUntilDialog::getMode()
{
    return Qtenv::eRunMode(ui->modeComboBox->currentIndex() + 1);
}

void RunUntilDialog::accept()
{
    getQtenv()->setPref("rununtil-time", ui->timeEdit->text());
    getQtenv()->setPref("rununtil-event", ui->eventEdit->text());
    getQtenv()->setPref("rununtil-msg", ui->msgCombo->itemData(ui->msgCombo->currentIndex()));
    getQtenv()->setPref("rununtil-mode", ui->modeComboBox->currentIndex());
    getQtenv()->opt->stopOnMsgCancel = ui->stopCheckbox->isChecked();
    QDialog::accept();
}

RunUntilDialog::~RunUntilDialog()
{
    delete ui;
}

} // namespace qtenv
} // namespace omnetpp
