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
    explicit RunSelectionDialog(QWidget *parent = 0, bool firstRun = false);
    ~RunSelectionDialog();

    std::string getConfigName();
    int getRunNumber();

private slots:
    void configNameChanged(int index);

public slots:
    int exec();

private:
    Ui::RunSelectionDialog *ui;
    bool firstRun;

    std::vector<std::string> groupAndSortConfigNames();
    void fillRunNumberCombo(const char *configName);
};

} // namespace qtenv
} // namespace omnetpp

#endif
