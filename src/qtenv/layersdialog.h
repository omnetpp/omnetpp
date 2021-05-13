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

#include <QtWidgets/QDialog>
#include "qtenvdefs.h"

namespace Ui {
class LayersDialog;
}

namespace omnetpp {
namespace qtenv {

class ModuleCanvasViewer;
class CanvasRenderer;

class QTENV_API LayersDialog : public QDialog
{
    Q_OBJECT

    ModuleCanvasViewer *canvasViewer;
    CanvasRenderer *canvasRenderer;

    QString originalEnabledTags, originalExceptTags;
    QStringList enabledTags, exceptTags;

private slots:
    void onEnabledCheckBoxClicked(bool checked);
    void onExceptCheckBoxClicked(bool checked);

    void applyTagFilters();

public slots:
    virtual void accept();
    virtual void reject();

public:
    explicit LayersDialog(ModuleCanvasViewer *canvasViewer, QWidget *parent = 0);
    ~LayersDialog();

    QStringList getEnabledTags() const { return enabledTags; }
    QStringList getExceptTags() const { return exceptTags; }

private:
    Ui::LayersDialog *ui;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_LAYERSDIALOG_H
