//==========================================================================
//  RUNUNTILDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_RUNUNTILDIALOG_H
#define __OMNETPP_QTENV_RUNUNTILDIALOG_H

#include <QtWidgets/QDialog>
#include "qtenv.h"

namespace Ui {
class RunUntilDialog;
}

namespace omnetpp {

class cObject;

namespace qtenv {

class QTENV_API RunUntilDialog : public QDialog
{
    Q_OBJECT
private:
    QList<cObject *> fesEvents(int maxNum, bool wantEvents, bool wantSelfMsgs, bool wantNonSelfMsgs, bool wantSilentMsgs);

public:
    explicit RunUntilDialog(QWidget *parent = nullptr);
    ~RunUntilDialog();

    simtime_t getTime();
    eventnumber_t getEventNumber();
    cObject *getMessage();
    RunMode getMode();
    bool stopOnMsgCancel();

public Q_SLOTS:
    void accept() override;

private:
    Ui::RunUntilDialog *ui;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_RUNUNTILDIALOG_H
