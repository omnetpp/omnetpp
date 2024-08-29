//==========================================================================
//  HISTOGRAMVIEW.H - part of
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

#ifndef __OMNETPP_QTENV_HISTOGRAMVIEW_H
#define __OMNETPP_QTENV_HISTOGRAMVIEW_H

#include <QtWidgets/QGraphicsView>
#include "qtenvdefs.h"

namespace omnetpp {

class cAbstractHistogram;

namespace qtenv {

class ChartGridItem;

class QTENV_API HistogramView : public QGraphicsView
{
    Q_OBJECT

  public:
    enum ChartType { SHOW_COUNTS, SHOW_PDF };
    enum DrawingStyle { DRAW_FILLED = 0, DRAW_EMPTY = 1 };

  private:
    ChartGridItem *gridItem;
    QGraphicsRectItem *actual;
    double minY, maxY, minX, maxX;
    DrawingStyle drawingStyle;

    QPointF mapToView(double x, double y) const;
    int mapXToView(double x) const;
    int mapYToView(double y) const;

    void showInfo(QPoint mousePos);

    void drawBin(ChartType type, DrawingStyle drawingStyle, int binIndex, bool isOutliers, double leftEdge, double rightEdge, double value, double pdfValue);

  Q_SIGNALS:
    void showCellInfo(int bin);

  protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event);
    // Disable mouse scrolling
    void scrollContentsBy(int, int) override {}

  public:
    HistogramView(QGraphicsScene *scene, QWidget *parent = nullptr);
    void draw(ChartType type, DrawingStyle drawingStyle, cAbstractHistogram *distr);

    void setMinX(double minX);
    void setMaxX(double maxX);
    void setMinY(double minY);
    void setMaxY(double maxY);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

