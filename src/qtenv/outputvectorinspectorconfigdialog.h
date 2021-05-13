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
    explicit OutputVectorInspectorConfigDialog(VectorPlotItem::PlottingMode plottingMode, QWidget *parent = 0);
    ~OutputVectorInspectorConfigDialog();

    // DrawingStyle
    VectorPlotItem::PlottingMode getPlottingMode() const;

    bool hasMinY() const;
    double getMinY() const;
    void setMinY(const double minY);

    bool hasMaxY() const;
    double getMaxY() const;
    void setMaxY(const double maxY);

    bool hasTimeScale() const;
    double getTimeScale() const;
    void setTimeScale(const double timeScale);

  private slots:
    void onApplyButtonClicked();
    void onTextChanged();

  public slots:
    void accept() override;

  signals:
    void applyButtonClicked();

  private:
    Ui::OutputVectorInspectorConfigDialog *ui;

    bool checkInput();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

