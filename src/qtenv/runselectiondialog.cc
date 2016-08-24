//==========================================================================
//  RUNSELECTIONDIALOG.CC - part of
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

#include <iterator>
#include <algorithm>
#include <vector>
#include <set>
#include <QDebug>
#include "runselectiondialog.h"
#include "ui_runselectiondialog.h"
#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

RunSelectionDialog::RunSelectionDialog(const std::string& defaultConfig, int defaultRun, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunSelectionDialog)
{
    ui->setupUi(this);
    adjustSize();
    setFont(getQtenv()->getBoldFont());

    bool isBase = false;
    auto configNames = groupAndSortConfigNames();
    configNumber = 0;
    std::string firstConfigName = "";
    for (auto name : configNames) {
        if (name == "") {
            isBase = true;
            continue;
        }
        else if (firstConfigName.empty())
            firstConfigName = name;

        std::string desc = getQtenv()->getConfigEx()->getConfigDescription(name.c_str());
        int runs = getQtenv()->getConfigEx()->getNumRunsInConfig(name.c_str());

        std::string displayName = name;
        if (isBase)
            displayName = "(" + name + ")";
        if (desc != "")
            displayName += " -- " + desc;
        if (runs == 0)
            displayName += " (invalid config, generates 0 runs)";
        if (runs > 1)
            displayName += " (config with " + std::to_string(runs) + " runs)";

        ui->configName->addItem(displayName.c_str(), QVariant(name.c_str()));
        ++configNumber;
    }

    std::string configName = defaultConfig.empty() ? getQtenv()->getPref("default-configname", "").toString().toUtf8().constData() : defaultConfig;
    int runNumber = defaultRun < 0 ? getQtenv()->getPref("default-runnumber", "").toInt() : defaultRun;

    auto it = std::find(configNames.begin(), configNames.end(), configName);

    if ((configName.size() == 0 && ui->configName->count() != 0) || it == configNames.end()) {
        configName = firstConfigName;
        runNumber = 0;
    }

    int index = ui->configName->findData(configName.c_str());
    ui->configName->setCurrentIndex(std::max(0, index));

    fillRunNumberCombo(configName.c_str());

    ui->runNumber->setCurrentIndex(runNumber);

    connect(ui->configName, SIGNAL(currentIndexChanged(int)), this, SLOT(configNameChanged(int)));

    // needs to be set here too, the setting in the Designer wasn't enough on Mac
    QApplication::setWindowIcon(QIcon(":/logo/icons/logo/logo128w.png"));
}

RunSelectionDialog::~RunSelectionDialog()
{
    delete ui;
}

std::vector<std::string> RunSelectionDialog::groupAndSortConfigNames()
{
    std::set<std::string> hasderivedconfig;

    for (auto c : getQtenv()->getConfigEx()->getConfigNames())
        for (auto base : getQtenv()->getConfigEx()->getBaseConfigs(c.c_str()))
            hasderivedconfig.insert(base);


    std::vector<std::string> leaves;
    for (auto c : getQtenv()->getConfigEx()->getConfigNames())
        if (hasderivedconfig.end() == hasderivedconfig.find(c))
            leaves.push_back(c);


    leaves.push_back("");
    leaves.insert(leaves.end(), hasderivedconfig.begin(),
            hasderivedconfig.end());

    return leaves;  // it will be implicitly moved
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

void RunSelectionDialog::configNameChanged(int index)
{
    fillRunNumberCombo(ui->configName->itemData(index).toString().toStdString().c_str());
}

void RunSelectionDialog::fillRunNumberCombo(const char *configName)
{
    // There isn't any config, do not have to fill Combobox
    if (strlen(configName) == 0 || strcmp(configName, "General") == 0)
        return;

    ui->runNumber->clear();
    int runs = getQtenv()->getConfigEx()->getNumRunsInConfig(configName);
    std::vector<std::string> configVariables = getQtenv()->getConfigEx()->unrollConfig(configName, false);

    for (int i = 0; i < runs; ++i)
        ui->runNumber->addItem(QString::number(i) + " (" + configVariables[i].c_str() + ")", QVariant(i));

    ui->runNumber->setDisabled(ui->runNumber->count() < 2);
}

int RunSelectionDialog::getConfigNumber()
{
    return configNumber;
}

void RunSelectionDialog::accept()
{
    getQtenv()->setPref("default-configname", getConfigName().c_str());
    getQtenv()->setPref("default-runnumber", getRunNumber());
    QDialog::accept();
}

}  // namespace qtenv
}  // namespace omnetpp

