//==========================================================================
//  RUNSELECTIONDIALOG.CC - part of
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

#include <iterator>
#include <algorithm>
#include <vector>
#include <set>
#include <QtCore/QDebug>
#include <QtWidgets/QPushButton>
#include "common/stlutil.h"
#include "runselectiondialog.h"
#include "ui_runselectiondialog.h"
#include "qtenv.h"

namespace omnetpp {
using namespace common;
namespace qtenv {

RunSelectionDialog::RunSelectionDialog(cConfigurationEx *configuration, const std::string& configNameArg, const std::string &runFilter, QWidget *parent)
    : QDialog(parent), ui(new Ui::RunSelectionDialog), configuration(configuration), configNameArg(configNameArg), runFilter(runFilter)
{
    ui->setupUi(this);
    adjustSize();
    setFont(getQtenv()->getBoldFont());

    configNames = configuration->getConfigNames();

    if (!configNameArg.empty() && !contains(configNames, configNameArg))
        throw opp_runtime_error("No such config: %s", configNameArg.c_str());

    const char *fileName = configuration->getFileName();

    ui->label->setText(fileName
                       ? QString("Set up one of the configurations defined in '%1'.").arg(fileName)
                       : QString("No '*.ini' file is being used."));

    // filling the configname combobox
    for (auto name : configNames) {
        std::string desc = configuration->getConfigDescription(name.c_str());
        int runs = configuration->getNumRunsInConfig(name.c_str());

        std::string displayName = name;
        if (desc != "")
            displayName += " -- " + desc;
        if (runs == 0)
            displayName += " (invalid config, generates 0 runs)";
        if (runs > 1)
            displayName += " (config with " + std::to_string(runs) + " runs)";

        ui->configName->addItem(displayName.c_str(), QVariant(name.c_str()));
    }

    // only used to set the initially selected item in the combobox. if no specified coming in, using the last chosen
    std::string initialConfigName = configNameArg.empty()
            ? getQtenv()->getPref("last-configname", "").toString().toStdString()
            : configNameArg;

    // if no config is selected/saved, or the selected/saved config is invalid,
    // choosing the first config, while trying to skip the General one
    if (initialConfigName.empty() || !contains(configNames, initialConfigName)) {
        initialConfigName = configNames.front();
        if (initialConfigName == "General" && configNames.size() > 1)
            initialConfigName = configNames[1];
    }

    int index = ui->configName->findData(initialConfigName.c_str());
    ui->configName->setCurrentIndex(std::max(0, index));

    // this will set up the runnumber combobox
    updateRuns(initialConfigName.c_str());

    // only used to set the initially selected item in the combobox
    int initialRunNumber = getQtenv()->getPref("last-runnumber", -1).toInt();

    // if there isn't a saved default runNumber, or there is but it doesn't match
    if (initialRunNumber < 0 || (!matchingRunNumbers.empty() && !contains(matchingRunNumbers, initialRunNumber)))
        // then the default runNumber will be either the first matching (if there is any), or the first non-matching run
        initialRunNumber = matchingRunNumbers.empty() ? -1 : matchingRunNumbers[0];


    index = ui->runNumber->findData(initialRunNumber);
    ui->runNumber->setCurrentIndex(std::max(0, index));

    connect(ui->configName, SIGNAL(currentIndexChanged(int)), this, SLOT(configSelectionChanged(int)));

    // the contents of the Config selection ComboBox can affect the size of the dialog
    adjustSize();

    if (parent) {
        auto geom = geometry();
        geom.moveCenter(parent->geometry().center());
        setGeometry(geom);
    }

    // so far the horizontal policy was "MinimumExpanding" to make sure it will enlarge the dialog
    // if needed in adjustSize(), but now we want to allow the user to resize it to a smaller width
    ui->configName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setFocus(Qt::PopupFocusReason);
}

RunSelectionDialog::~RunSelectionDialog()
{
    delete ui;
}

std::string RunSelectionDialog::getConfigName()
{
    int index = ui->configName->currentIndex();
    return ui->configName->itemData(index).toString().toStdString();
}

int RunSelectionDialog::getRunNumber()
{
    int index = ui->runNumber->currentIndex();
    return ui->runNumber->itemData(index).toInt();
}

void RunSelectionDialog::configSelectionChanged(int index)
{
    updateRuns(ui->configName->itemData(index).toString().toStdString().c_str());
}

void RunSelectionDialog::updateRuns(const char *configName)
{
    runDescriptions.clear();
    matchingRunNumbers.clear();
    ui->runNumber->clear();

    // if there isn't any config, we don't have to fill the combobox
    if (!configName || !configName[0])
        return;

    runDescriptions = configuration->unrollConfig(configName);

    try {
        // we try to apply the filter to all configurations
        matchingRunNumbers = getQtenv()->resolveRunFilter(configName, runFilter.c_str());
    }
    catch (std::exception&) {
        // but only report an error if it is applied to the config the user specifically selected
        if (configName == configNameArg)
            throw;
    }

    // grouping the matching runs to the top
    for (auto i : matchingRunNumbers)
        ui->runNumber->addItem(QString("%1 (%2)").arg(i).arg(runDescriptions[i].info.c_str()), QVariant(i));

    if (!matchingRunNumbers.empty() && matchingRunNumbers.size() < runDescriptions.size())
        ui->runNumber->insertSeparator(matchingRunNumbers.size());

    // and then the non-matching ones after a separator
    for (int i = 0; i < (int)runDescriptions.size(); ++i)
        if (!contains(matchingRunNumbers, i))
            ui->runNumber->addItem(QString("%1 (%2)").arg(i).arg(runDescriptions[i].info.c_str()), QVariant(i));


    ui->runNumber->setDisabled(ui->runNumber->count() < 2);
}

bool RunSelectionDialog::configNameDefinite()
{
    // if there is only one configuration, and either no argument is given, or it is matching
    if (configNames.size() == 1 && (configNameArg.empty() || configNameArg == configNames[0]))
        return true;

    return !configNameArg.empty() && contains(configNames, configNameArg);
}

bool RunSelectionDialog::configAndRunDefinite()
{
    return configNameDefinite() && (runDescriptions.size() == 1 || matchingRunNumbers.size() == 1);
}

void RunSelectionDialog::accept()
{
    getQtenv()->setPref("last-configname", getConfigName().c_str());
    getQtenv()->setPref("last-runnumber", getRunNumber());
    QDialog::accept();
}

}  // namespace qtenv
}  // namespace omnetpp
