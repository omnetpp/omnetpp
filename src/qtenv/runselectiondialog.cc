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

#include <QDebug>

namespace omnetpp {
namespace qtenv {

RunSelectionDialog::RunSelectionDialog(QWidget *parent, bool firstRun) :
    QDialog(parent),
    ui(new Ui::RunSelectionDialog),
    firstRun(firstRun)
{
    ui->setupUi(this);
    adjustSize();

    bool isBase = false;
    for (auto name : groupAndSortConfigNames()) {
        if (name == "") {
            isBase = true;
            continue;
        }

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
    }

    std::string configName = getQtenv()->opt->defaultConfig.c_str();
    int runNumber = getQtenv()->opt->defaultRun;

    if (configName == "" && ui->configName->size().rheight() != 0) {
        configName = groupAndSortConfigNames()[0];
        runNumber = 0;
    }

    int index = ui->configName->findData(configName.c_str());
    ui->configName->setCurrentIndex(std::max(0, index));

    fillRunNumberCombo(configName.c_str());

    ui->runNumber->setCurrentIndex(runNumber);

    connect(ui->configName, SIGNAL(currentIndexChanged(int)), this, SLOT(configNameChanged(int)));
}

RunSelectionDialog::~RunSelectionDialog()
{
    getQtenv()->opt->defaultConfig = getConfigName();
    getQtenv()->opt->defaultRun = getRunNumber();

    delete ui;
}

int RunSelectionDialog::exec()
{
    if(!firstRun || ui->configName->count() > 2)
        return QDialog::exec();

    return QDialog::Accepted;
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

void RunSelectionDialog::configNameChanged(int index)
{
    fillRunNumberCombo(ui->configName->itemData(index).toString().toStdString().c_str());
}

void RunSelectionDialog::fillRunNumberCombo(const char *configName)
{
    ui->runNumber->clear();
    int runs = getQtenv()->getConfigEx()->getNumRunsInConfig(configName);
    std::vector<std::string> configVariables = getQtenv()->getConfigEx()->unrollConfig(configName, false);

    for (int i = 0; i < runs; ++i)
        ui->runNumber->addItem(QString::number(i) + " (" + configVariables[i].c_str() + ")", QVariant(i));

    ui->runNumber->setDisabled(ui->runNumber->count() < 2);
}

} // namespace qtenv
} // namespace omnetpp
