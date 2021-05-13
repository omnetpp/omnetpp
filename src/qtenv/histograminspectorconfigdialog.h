//==========================================================================
//  HISTOGRAMINSPECTORCONFIGDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_HISTOGRAMINSPECTORCONFIGDIALOG_H
#define __OMNETPP_QTENV_HISTOGRAMINSPECTORCONFIGDIALOG_H

#include <QtWidgets/QDialog>
#include "histogramview.h"

namespace Ui {

class HistogramInspectorConfigDialog;
}  // namespace Ui

namespace omnetpp {
namespace qtenv {

class QTENV_API HistogramInspectorConfigDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit HistogramInspectorConfigDialog(HistogramView::DrawingStyle style = HistogramView::DRAW_FILLED, QWidget *parent = 0);
    ~HistogramInspectorConfigDialog();

    HistogramView::DrawingStyle getDrawingStyle() const;

    bool hasMinY() const;
    double getMinY() const;
    void setMinY(const double minY);

    bool hasMaxY() const;
    double getMaxY() const;
    void setMaxY(const double maxY);

    bool hasMinX() const;
    double getMinX() const;
    void setMinX(const double minX);

    bool hasMaxX() const;
    double getMaxX() const;
    void setMaxX(const double maxX);

  signals:
    void applyButtonClicked();

  public slots:
    void accept() override;

  private slots:
    void onApplyButtonClicked();

  private:
    Ui::HistogramInspectorConfigDialog *ui;

    bool checkInput();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

