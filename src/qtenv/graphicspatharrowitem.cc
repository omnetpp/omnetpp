//==========================================================================
//  GRAPHICSPATHARROWITEM.CC - part of
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

#include "graphicspatharrowitem.h"
#include <QPen>

GraphicsPathArrowItem::GraphicsPathArrowItem(QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    setPen(Qt::NoPen);
}

void GraphicsPathArrowItem::setLineWidth(double width) {
    width = std::max(minimumWidth, width);
    QPolygonF polygon;

    polygon.append(QPointF(0, 0));
    polygon.append(QPointF(-width * 3,   -width * 2));
    polygon.append(QPointF(-width * 1.5,  0));
    polygon.append(QPointF(-width * 3,    width * 2));

    setPolygon(polygon);
}
