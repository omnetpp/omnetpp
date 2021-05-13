//==========================================================================
//  LOGFILTERDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_LOGFILTERDIALOG_H
#define __OMNETPP_QTENV_LOGFILTERDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QTreeWidgetItem>
#include <set>
#include "omnetpp/cmodule.h"
#include "qtenvdefs.h"

namespace Ui {
class logfilterdialog;
}

namespace omnetpp {
class cComponent;

namespace qtenv {

class QTENV_API LogFilterDialog : public QDialog
{
    Q_OBJECT

    Ui::logfilterdialog *ui;

    void addModuleChildren(cModule *module, QTreeWidgetItem *item, const std::set<int> &excludedModuleIds);
    static QString getTextForModule(cModule *module);

private slots:
    void onItemChanged(QTreeWidgetItem *item, int column);

public:
    explicit LogFilterDialog(QWidget *parent, cModule *rootModule, const std::set<int> &excludedModuleIds);
    ~LogFilterDialog();

    std::set<int> getExcludedModuleIds();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_LOGFILTERDIALOG_H
