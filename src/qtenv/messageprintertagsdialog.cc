//==========================================================================
//  MESSAGEPRINTERTAGSDIALOG.CC - part of
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

#include "messageprintertagsdialog.h"
#include "ui_messageprintertagsdialog.h"
#include "qtenv.h"
#include "qtutil.h"
#include "common/stlutil.h"
#include <QtCore/QList>

namespace omnetpp {
using namespace common;
namespace qtenv {

void MessagePrinterTagsDialog::onItemChanged(QListWidgetItem *item)
{

}

MessagePrinterTagsDialog::MessagePrinterTagsDialog(QWidget *parent, const QStringList& allTags, cMessagePrinter::Options *options) :
    QDialog(parent),
    ui(new Ui::messageprintertagsdialog)
{
    ui->setupUi(this);
    setFont(getQtenv()->getBoldFont());

    //Q_ASSERT(rootModule);

    for (const auto& t : allTags) {
        QListWidgetItem *item = new QListWidgetItem(t);
        ui->listWidget->addItem(item);
        //item->setData(0, Qt::UserRole, QVariant::fromValue((void *)rootModule));
        item->setCheckState(contains(options->enabledTags, t.toStdString()) ? Qt::Checked : Qt::Unchecked);
    }

    connect(ui->listWidget, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(onItemChanged(QListWidgetItem *)));
}

std::set<std::string> MessagePrinterTagsDialog::getEnabledTags()
{
    std::set<std::string> tags;

    for (int i = 0; i < ui->listWidget->count(); ++i) {
        auto item = ui->listWidget->item(i);

        if (item->checkState() == Qt::Checked)
            tags.insert(item->text().toStdString());
    }

    return tags;
}

MessagePrinterTagsDialog::~MessagePrinterTagsDialog()
{
    delete ui;
}
/*
std::set<int> MessagePrinterTagsDialog::getExcludedModuleIds()
{
    std::set<int> result;

    QTreeWidgetItemIterator it(ui->treeWidget);
    while (*it) {
        if ((*it)->checkState(0) != Qt::Checked)
            result.insert(((cModule *)(((*it)->data(0, Qt::UserRole)).value<void *>()))->getId());
        ++it;
    }

    return result;
}
*/

}  // namespace qtenv
}  // namespace omnetpp

