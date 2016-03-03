//==========================================================================
//  GRAPHICSPATHARROWITEM.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_GRAPHICSPATHARROWITEM_H
#define __OMNETPP_QTENV_GRAPHICSPATHARROWITEM_H

#include <QGraphicsPolygonItem>
#include "qtenvdefs.h"

class GraphicsPathArrowItem : public QGraphicsPolygonItem
{
    double arrowWidth = 6;
    double arrowLength = 4;
    double fillRatio = 0.75;
    double lineWidth = 1;
QPointF start, end;
    void updatePolygon();
public:
    GraphicsPathArrowItem(QGraphicsItem *parent);

    QPainterPath shape() const override;

    void setEndPoints(const QPointF &start, const QPointF &end);

    void setArrowWidth(double width);
    void setArrowLength(double length);
    void setFillRatio(double ratio);
    void setColor(const QColor &color);
    void setLineWidth(double width);
};

#endif // __OMNETPP_QTENV_GRAPHICSPATHARROWITEM_H
