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

  protected:
    HistogramView *view;
    QStatusBar *statusBar;
    QAction *setUpBinsAction;
    HistogramInspectorConfigDialog *configDialog;
    HistogramView::ChartType chartType;
    HistogramView::DrawingStyle drawingStyle;
    bool isCountsMinYAutoscaled, isCountsMaxYAutoscaled, isCountsMinXAutoscaled, isCountsMaxXAutoscaled;
    bool isPDFMinYAutoscaled, isPDFMaxYAutoscaled, isPDFMinXAutoscaled, isPDFMaxXAutoscaled;
    double countsMinY, countsMaxY, countsMinX, countsMaxX;
    double pdfMinY, pdfMaxY, pdfMinX, pdfMaxX;

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

