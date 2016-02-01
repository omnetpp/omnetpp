//==========================================================================
//  STOPDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_STOPDIALOG_H
#define __OMNETPP_QTENV_STOPDIALOG_H

#include <QDialog>

namespace Ui {
class StopDialog;
}

namespace omnetpp {
namespace qtenv {

class StopDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StopDialog(QWidget *parent = 0);
    ~StopDialog();

public slots:
    void onClickStop();
    void onClickUpdate();
    void stopDialogAutoupdate();
    void show();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void closeEvent(QCloseEvent *event);

private:
    Ui::StopDialog *ui;
};

} // namespace qtenv
} // namespace omnetpp

#endif
