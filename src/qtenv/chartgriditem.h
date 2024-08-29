//==========================================================================
//  CHARTGRIDITEM.H - part of
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

#ifndef __OMNETPP_QTENV_CHARTGRIDITEM_H
#define __OMNETPP_QTENV_CHARTGRIDITEM_H

#include <QtWidgets/QGraphicsItem>
#include <QtGui/QFont>
#include "omnetpp/simtime_t.h"
#include "qtenvdefs.h"
#include "qtutil.h"

namespace omnetpp {
namespace qtenv {

class QTENV_API ChartGridItem : public QGraphicsItem
{
  private:
    QGraphicsRectItem *frame;
    double minY = 0, maxY = 10, minX = 0, maxX = 10;
    int yLabelWidth = 0;
    bool isYAxisChanged = true, isXAxisChanged = true;
    QRectF boundingRectangle;
    std::vector<std::pair<ChartTickDecimal, bool> > xTicks, yTicks;

    void updateRect();
    void paintXAxis(QPainter *painter);
    void paintYAxis(QPainter *painter);

    QPoint mapFromData(double t, double y);
    int mapFromSimtime(double t);
    int mapFromValue(double y);
    void calculateYTicks();

    double getYLabelWidth();
    int getTextWidth(const QString& text) const;
    int getTextHeight() const;

    void correctBoundingRectangle();

  public:
    ChartGridItem(QGraphicsItem *parent = nullptr);
    ~ChartGridItem();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    QGraphicsRectItem *getDiagramFrame();

    void setMinX(double x);
    void setMaxX(double x);
    void setMinY(double y);
    void setMaxY(double y);
    void setRect(const QRectF& rect);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

