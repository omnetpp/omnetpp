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

#include "runselectiondialog.h"
#include "ui_runselectiondialog.h"
#include "qtenv.h"
#include <iterator>
#include <algorithm>
#include <vector>
#include <set>

namespace omnetpp {
namespace qtenv {

RunSelectionDialog::RunSelectionDialog(Qtenv *env, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunSelectionDialog),
    env(env)
{
    ui->setupUi(this);
    adjustSize();

    bool isBase = false;
    for (auto name : groupAndSortConfigNames()) {
        if (name == "") {
            isBase = true;
            continue;
        }

        std::string desc = env->getConfigEx()->getConfigDescription(name.c_str());
        int runs = env->getConfigEx()->getNumRunsInConfig(name.c_str());

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
    }

    std::string configName = env->opt->defaultConfig.c_str();
    int runNumber = env->opt->defaultRun;

    if (configName == "" && ui->configName->size().rheight() != 0) {
        configName = ui->configName->itemText(0).toStdString();
        runNumber = 0;
    }

    int index = ui->configName->findData(configName.c_str());
    ui->configName->setCurrentIndex(std::max(0, index));

    for (int i = 0; i <= runNumber; ++i)
        ui->runNumber->addItem(QString::number(i), QVariant(i));
}

RunSelectionDialog::~RunSelectionDialog()
{
    env->opt->defaultConfig = getConfigName();
    env->opt->defaultRun = getRunNumber();

    delete ui;
}

std::vector<std::string> RunSelectionDialog::groupAndSortConfigNames()
{
    std::set<std::string> hasderivedconfig;

    for (auto c : env->getConfigEx()->getConfigNames())
        for (auto base : env->getConfigEx()->getBaseConfigs(c.c_str()))
            hasderivedconfig.insert(base);


    std::vector<std::string> leaves;
    for (auto c : env->getConfigEx()->getConfigNames())
        if (hasderivedconfig.end() == hasderivedconfig.find(c))
            leaves.push_back(c);


    leaves.push_back("");
    leaves.insert(leaves.end(), hasderivedconfig.begin(),
            hasderivedconfig.end());

    return std::move(leaves);
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

} // namespace qtenv
} // namespace omnetpp
