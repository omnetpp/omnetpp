//==========================================================================
//  HISTOGRAMINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_HISTOGRAMINSPECTOR_H
#define __OMNETPP_QTENV_HISTOGRAMINSPECTOR_H

#include "inspector.h"
#include "histogramview.h"

class QGraphicsView;
class QStatusBar;

namespace omnetpp {
namespace qtenv {

class HistogramInspectorConfigDialog;

class QTENV_API HistogramInspector : public Inspector
{
    Q_OBJECT

    // If Y autoscaling is enabled, then whenever the highest bar fills the visible range
    // (vertically) more than HYST_HIGH, or less than HYST_LOW, the range is adjusted
    // so that the new bar height will be at RATIO of the visible Y range.
    static constexpr double AUTOSCALE_Y_HYST_LOW = 0.75;
    static constexpr double AUTOSCALE_Y_HYST_HIGH = 0.95;
    static constexpr double AUTOSCALE_Y_RATIO = 0.85;

  protected:
    HistogramView *view;
    QStatusBar *statusBar;
    QAction *setUpBinsAction;
    HistogramInspectorConfigDialog *configDialog;
    HistogramView::ChartType chartType = HistogramView::SHOW_PDF;
    HistogramView::DrawingStyle drawingStyle = HistogramView::DRAW_FILLED;
    bool isCountsMinYAutoscaled = true, isCountsMaxYAutoscaled = true, isCountsMinXAutoscaled = true, isCountsMaxXAutoscaled = true;
    bool isPDFMinYAutoscaled = true, isPDFMaxYAutoscaled = true, isPDFMinXAutoscaled = true, isPDFMaxXAutoscaled = true;
    double countsMinY = 0, countsMaxY = 0, countsMinX = 0, countsMaxX = 0;
    double pdfMinY = 0, pdfMaxY = 0, pdfMinX = 0, pdfMaxX = 0;

    double lastMaxY = -1;

    void resizeEvent(QResizeEvent *event) override;
    void xRangeCorrection(double& minX, double& maxX, bool isMinXAutoscaled, bool isMaxXAutoscaled);
    void yRangeCorrection(double& minY, double& maxY, bool isMinYAutoscaled, bool isMaxYAutoscaled);
    double maxY();
    void setConfig();

  public:
    HistogramInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
    virtual void refresh() override;

    // return textual information in general or about a value/value pair
    QString generalInfo();

  private slots:
    void onShowCellInfo(int bin);
    void onCustomContextMenuRequested(QPoint pos);
    void onShowCounts();
    void onShowPDF();
    void onOptionsTriggered();
    void onApplyButtonClicked();
    void onSetUpBinsClicked();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

