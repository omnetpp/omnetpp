//==========================================================================
//  RUNSELECTIONDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_RUNSELECTIONDIALOG_H
#define __OMNETPP_QTENV_RUNSELECTIONDIALOG_H

#include <QDialog>
#include <map>
#include "qtenvdefs.h"

namespace Ui {
class RunSelectionDialog;
}

namespace omnetpp {
namespace qtenv {

class Qtenv;

class RunSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunSelectionDialog(Qtenv *env, QWidget *parent = 0);
    ~RunSelectionDialog();

    std::string getConfigName();
    int getRunNumber();

private:
    Ui::RunSelectionDialog *ui;
    Qtenv *env;

    std::vector<std::string> groupAndSortConfigNames();
};

} // namespace qtenv
} // namespace omnetpp

#endif
