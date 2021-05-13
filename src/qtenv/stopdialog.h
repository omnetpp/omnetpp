//==========================================================================
//  STOPDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_STOPDIALOG_H
#define __OMNETPP_QTENV_STOPDIALOG_H

#include <QtWidgets/QDialog>
#include "qtenvdefs.h"

namespace Ui {
class StopDialog;
}

namespace omnetpp {
namespace qtenv {

class QTENV_API StopDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StopDialog(QWidget *parent = 0);
    ~StopDialog();

public slots:
    void onClickStop();
    void onClickUpdate();
    void applyAutoupdate();
    void show();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::StopDialog *ui;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif
