//==========================================================================
//  COMBOSELECTIONDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_COMBOSELECTIONDIALOG_H
#define __OMNETPP_QTENV_COMBOSELECTIONDIALOG_H

#include <QtWidgets/QDialog>
#include "qtenvdefs.h"

namespace Ui {
class ComboSelectionDialog;
}

namespace omnetpp {
namespace qtenv {

class QTENV_API ComboSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComboSelectionDialog(QString netName, QStringList networkNames, QWidget *parent = nullptr);
    ~ComboSelectionDialog();

    QString getSelectedNetName();

private:
    Ui::ComboSelectionDialog *ui;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_COMBOSELECTIONDIALOG_H
