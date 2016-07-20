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

#include <cmath>
#include <QPen>
#include <QDebug>

void GraphicsPathArrowItem::updatePolygon()
{
    QPolygonF polygon;
    polygon.append(QPointF(0, 0));
    polygon.append(QPointF(-arrowLength, -arrowWidth / 2));
    polygon.append(QPointF(-arrowLength * fillRatio, 0));
    polygon.append(QPointF(-arrowLength, arrowWidth / 2));

    setPolygon(polygon);
}

GraphicsPathArrowItem::GraphicsPathArrowItem(QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    // uncomment this if you want the arrowheads to keep their shape
    // even if the line on which they are placed is heavily transformed:
    // (also see similar comments in figurerenderers.cc)
    // setFlags(QGraphicsItem::ItemIgnoresTransformations);
    setFillRule(Qt::WindingFill);
    updatePolygon();
    setPen(QPen(QColor("black"), 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    setData(1, parentItem()->data(1));
}

QPainterPath GraphicsPathArrowItem::shape() const
{
    QPainterPath result;
    result.setFillRule(Qt::WindingFill);
    result.addPolygon(polygon());
    result.closeSubpath();

    QPainterPathStroker s;
    s.setCapStyle(pen().capStyle());
    s.setDashOffset(pen().dashOffset());
    s.setDashPattern(pen().dashPattern());
    s.setJoinStyle(pen().joinStyle());
    s.setMiterLimit(pen().miterLimit());
    s.setWidth(pen().widthF());

    result += s.createStroke(result);
    return result;
}

void GraphicsPathArrowItem::setEndPoints(const QPointF& start, const QPointF& end)
{
    setPos(end);
    setRotation(atan2(end.y() - start.y(), end.x() - start.x()) * 180.0 / M_PI);
}

void GraphicsPathArrowItem::setFillRatio(double ratio)
{
    if (ratio != fillRatio) {
        fillRatio = ratio;
        updatePolygon();
    }
}

void GraphicsPathArrowItem::setColor(const QColor& color)
{
    auto p = pen();
    p.setColor(color);
    setPen(p);
    setBrush(color);
    setEndPoints(start, end);
}

void GraphicsPathArrowItem::setLineWidth(double width)
{
    if (width != lineWidth) {
        auto p = pen();
        p.setWidthF(width);
        setPen(p);
    }
}

void GraphicsPathArrowItem::setArrowWidth(double width)
{
    if (width != arrowWidth) {
        arrowWidth = width;
        updatePolygon();
    }
}

void GraphicsPathArrowItem::setArrowLength(double length)
{
    if (length != arrowLength) {
        arrowLength = length;
        updatePolygon();
    }
}

