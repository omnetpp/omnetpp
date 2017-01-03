//==========================================================================
//  LAYERSDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_LAYERSDIALOG_H
#define __OMNETPP_QTENV_LAYERSDIALOG_H

#include <QDialog>
#include "qtenvdefs.h"

namespace Ui {
class LayersDialog;
}

namespace omnetpp {
namespace qtenv {

class QTENV_API LayersDialog : public QDialog
{
    Q_OBJECT

private slots:
    void clickOnShowFig(bool checked);

public slots:
    virtual void accept();

public:
    explicit LayersDialog(QString allTags, QString enabledTags, QString exceptTags, QWidget *parent = 0);
    ~LayersDialog();

    QStringList getEnabledTags() const { return enabledTags; }
    QStringList getExceptTags() const { return exceptTags; }

private:
    Ui::LayersDialog *ui;

    QStringList allTags, enabledTags, exceptTags;
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_LAYERSDIALOG_H
