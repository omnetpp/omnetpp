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
#include <QPainter>
#include <QDebug>

GraphicsPathArrowItem::GraphicsPathArrowItem(QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent), arrowStyle(BARBED)
{
    setVisible(false);
    setFlag(GraphicsItemFlag::ItemIgnoresTransformations);
    arrowDescrInit();
}

void GraphicsPathArrowItem::arrowDescrInit()
{
    arrowDescr.arrowLength = (float)8.0;
    arrowDescr.arrowWidth = (float)4.0;
    arrowDescr.arrowFillRatio = (float)1.0;
    arrowDescr.arrowPointsPtr = QVector<QPointF>(PTS_IN_ARROW);
}

void GraphicsPathArrowItem::setEndPoints(const QPointF &pf, const QPointF &pl)
{
    first = pf;
    last = pl;
    arrowDescr.arrowPointsPtr = configureArrow(pf, pl);
}

QPolygonF GraphicsPathArrowItem::configureArrow(const QPointF &pf, const QPointF &pl) const
{
    QPointF p0;
    double lineWidth = pen().widthF();
    double shapeLength = arrowDescr.arrowLength;
    double shapeWidth = arrowDescr.arrowWidth;
    double shapeFill = arrowDescr.arrowFillRatio;
    double dx, dy, length, sinTheta, cosTheta;
    double backup;          /* Distance to backup end points so the line
                             * ends in the middle of the arrowhead. */
    double minsShapeFill;
    QVector<QPointF> poly = arrowDescr.arrowPointsPtr;
    Qt::PenCapStyle capStyle = pen().capStyle();    /*  Qt::SquareCap, Qt::FlatCap, Qt::RoundCap. */

    if (shapeWidth < lineWidth)
        shapeWidth = lineWidth;

    minsShapeFill = lineWidth*shapeLength/shapeWidth;
    if (shapeFill > 0.0 && fabs(shapeLength*shapeFill) < fabs(minsShapeFill))
        shapeFill = 1.1*minsShapeFill / shapeLength;

    backup = 0.0;
    if (lineWidth > 1.0) {
        backup = (capStyle == Qt::SquareCap) ? 0.5 * lineWidth : 0.0;
        if (shapeFill > 0.0 && shapeLength != 0.0)
            backup += 0.5 * lineWidth * shapeLength / shapeWidth;
    }

    dx = pf.x() - pl.x();
    dy = pf.y() - pl.y();
    length = hypot(dx, dy);
    if (length == 0)
        sinTheta = cosTheta = 0.0;
    else {
        sinTheta = dy/length;
        cosTheta = dx/length;
    }

    p0.setX(pf.x() - shapeLength * cosTheta);
    p0.setY(pf.y() - shapeLength * sinTheta);
    if (shapeFill > 0.0 && shapeLength != 0.0) {
        poly[0].setX(pf.x() - shapeLength * shapeFill * cosTheta);
        poly[0].setY(pf.y() - shapeLength * shapeFill * sinTheta);
    } else {
        QPointF point(nan(""), nan(""));
        poly[0] = point;
    }

    poly[1].setX(p0.x() - shapeWidth * sinTheta);
    poly[1].setY(p0.y() + shapeWidth * cosTheta);
    poly[2] = pf;
    poly[3].setX(p0.x() + shapeWidth * sinTheta);
    poly[3].setY(p0.y() - shapeWidth * cosTheta);

    return poly;
}

QPolygonF GraphicsPathArrowItem::getTranslatePoints() const
{
    QTransform trans = parentItem()->transform();
    QPointF pf(trans.m11()*first.x() + trans.m21()*first.y() + trans.dx(),
              trans.m22()*first.y() + trans.m12()*first.x() + trans.dy());
    QPointF pl(trans.m11()*last.x() + trans.m21()*last.y() + trans.dx(),
              trans.m22()*last.y() + trans.m12()*last.x() + trans.dy());
    if (!trans.isAffine())
    {
        qreal w = trans.m13()*first.x() + trans.m23()*first.y() + trans.m33();
        pf.setX(pf.x() / w);
        pf.setY(pf.y() / w);
        w = trans.m13()*last.x() + trans.m23()*last.y() + trans.m33();
        pl.setX(pl.x() / w);
        pl.setY(pl.y() / w);
    }
    return configureArrow(pf, pl);
}

QRectF GraphicsPathArrowItem::boundingRect() const
{
    if(!isVisible())
        return QRectF();

    QPolygonF points(arrowDescr.arrowPointsPtr);
    if(QTransform() != parentItem()->transform())
        points = getTranslatePoints();

    const qreal max = std::numeric_limits<qreal>::max();
    QRectF rect(max, max, -max, -max);
    for (int i = 0; i < PTS_IN_ARROW; i++)
        if (!std::isnan(points[i].x()) && !std::isnan(points[i].y()))
        {
            qreal x = std::min(points[i].x(), rect.x());
            qreal y = std::min(points[i].y(), rect.y());
            qreal right = std::max(points[i].x(), rect.right());
            qreal bottom = std::max(points[i].y(), rect.bottom());
            rect.setCoords(x, y, right, bottom);
        }

    return rect;
}

void GraphicsPathArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isVisible())
    {
        QBrush brush(pen().color(), Qt::SolidPattern);
        QPen pen = this->pen();
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
        painter->setBrush(brush);

        QPolygonF points(arrowDescr.arrowPointsPtr);
        if(QTransform() != parentItem()->transform())
            points = getTranslatePoints();

        switch(arrowStyle)
        {
            case BARBED:
            {
                points[0] = points.boundingRect().center();
                painter->drawPolygon(points);
                break;
            }
            // if pen.width() == 0 the arrow disappear while zooming
            case SIMPLE:
            {
                points.pop_front();
                painter->drawPolyline(points);
                break;
            }
            case TRIANGLE:
                painter->drawPolygon(points);
                break;
            default:
                break;
        }

    }
    QGraphicsPolygonItem::paint(painter, option, widget);
}
