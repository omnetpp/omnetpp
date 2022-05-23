//==========================================================================
//  MESSAGEPRINTERTAGSDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_MESSAGEPRINTERTAGSDIALOG_H
#define __OMNETPP_QTENV_MESSAGEPRINTERTAGSDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidgetItem>
#include "omnetpp/cmodule.h"
#include "omnetpp/cmessageprinter.h"
#include "qtenvdefs.h"

namespace Ui {
class messageprintertagsdialog;
}

namespace omnetpp {

namespace qtenv {

class QTENV_API MessagePrinterTagsDialog : public QDialog
{
    Q_OBJECT

    Ui::messageprintertagsdialog *ui;

private Q_SLOTS:
    void onItemChanged(QListWidgetItem *item);

public:
    explicit MessagePrinterTagsDialog(QWidget *parent, const QStringList& allTags, cMessagePrinter::Options *options);

    std::set<std::string> getEnabledTags();

    ~MessagePrinterTagsDialog();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_MESSAGEPRINTERTAGSDIALOG_H
