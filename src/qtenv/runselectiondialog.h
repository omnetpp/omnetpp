//==========================================================================
//  RUNSELECTIONDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_RUNSELECTIONDIALOG_H
#define __OMNETPP_QTENV_RUNSELECTIONDIALOG_H

#include <QtWidgets/QDialog>
#include <map>
#include <omnetpp/cconfiguration.h>
#include "qtenvdefs.h"

namespace Ui {
class RunSelectionDialog;
}

namespace omnetpp {
namespace qtenv {

class Qtenv;

class QTENV_API RunSelectionDialog : public QDialog
{
    Q_OBJECT

    Ui::RunSelectionDialog *ui;

    cConfigurationEx *configuration;

    std::vector<std::string> configNames;

    // these two are filled/updated in updateRuns()
    std::vector<cConfigurationEx::RunInfo> runDescriptions;
    std::vector<int> matchingRunNumbers;

    std::string configNameArg; // the name of the config the user specified with the -c command line argument, or empty if none
    std::string runFilter; // the run filter the user specified with the -r command line argument, or empty if none

    std::vector<std::string> groupAndSortConfigNames();
    void updateRuns(const char *configName);

    bool configNameDefinite(); // if there is only one config, or a valid one is given as argument
    bool configAndRunDefinite(); // the above, plus if there is only one run in the one config, or the filter only matches one

protected:
    void accept() override;

private Q_SLOTS:
    void configSelectionChanged(int index);

public:
    // Beware: This throws exceptions if configNameArg isn't an existing config, or the run filter is invalid, etc...
    explicit RunSelectionDialog(cConfigurationEx *configuration, const std::string& configNameArg, const std::string &runFilter, QWidget *parent = nullptr);

    // Returns true if no choice is required from the user to determine which config and run to set up, because
    // it is already determined by the combination of the existing configs/runs and the used command line arguments.
    bool needsShowing() { return !configAndRunDefinite(); }

    // the results of the selection
    std::string getConfigName();
    int getRunNumber();

    ~RunSelectionDialog();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif
