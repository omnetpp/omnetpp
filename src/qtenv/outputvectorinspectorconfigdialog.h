//==========================================================================
//  OUTPUTVECTORINSPECTORCONFIGDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_OUTPUTVECTORINSPECTORCONFIGDIALOG_H
#define __OMNETPP_QTENV_OUTPUTVECTORINSPECTORCONFIGDIALOG_H

#include <QtWidgets/QDialog>
#include "vectorplotitem.h"

namespace Ui {

class OutputVectorInspectorConfigDialog;
}  // namespace Ui

namespace omnetpp {
namespace qtenv {

class QTENV_API OutputVectorInspectorConfigDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit OutputVectorInspectorConfigDialog(VectorPlotItem::PlottingMode plottingMode, QWidget *parent = nullptr);
    ~OutputVectorInspectorConfigDialog();

    // DrawingStyle
    VectorPlotItem::PlottingMode getPlottingMode() const;

    bool hasMinY() const;
    double getMinY() const;
    void setMinY(double minY);

    bool hasMaxY() const;
    double getMaxY() const;
    void setMaxY(double maxY);

    bool hasTimeScale() const;
    double getTimeScale() const;
    void setTimeScale(double timeScale);

  private Q_SLOTS:
    void onApplyButtonClicked();
    void onTextChanged();

  public Q_SLOTS:
    void accept() override;

  Q_SIGNALS:
    void applyButtonClicked();

  private:
    Ui::OutputVectorInspectorConfigDialog *ui;

    bool checkInput();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

