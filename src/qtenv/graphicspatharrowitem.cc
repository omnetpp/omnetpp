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
#include <QPainter>

#include <QDebug>

GraphicsPathArrowItem::GraphicsPathArrowItem(QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    //setPen(Qt::NoPen);
    setVisible(false);
    arrowDescrInit();
}

void GraphicsPathArrowItem::arrowDescrInit()
{
    arrowDescr.arrowLength = (float)8.0;
    arrowDescr.arrowWidth = (float)4.0;
    arrowDescr.arrowFillRatio = (float)1.0;
    arrowDescr.arrowPointsPtr = QVector<QPointF>(PTS_IN_ARROW);
}

void GraphicsPathArrowItem::configureArrow(const QPointF &pf, const QPointF &pl)
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

    arrowDescr.arrowPointsPtr = poly;
}

QRectF GraphicsPathArrowItem::boundingRect() const
{
    if(!isVisible())
        return QRectF();

    double maxDouble = std::numeric_limits<double>::max();
    QRectF rect(maxDouble, maxDouble, -maxDouble, -maxDouble);
    for (int i = 0; i < PTS_IN_ARROW; i++)
        if (!isnan(arrowDescr.arrowPointsPtr[i].x()) && ! isnan(arrowDescr.arrowPointsPtr[i].y()))
        {
            double x = arrowDescr.arrowPointsPtr[i].x();
            double y = arrowDescr.arrowPointsPtr[i].y();
            double rectX = rect.x() + rect.width();
            double rectY = rect.y() + rect.height();

            rect.setX(std::min(rect.x(), x));
            rect.setY(std::min(rect.y(), y));
            rectX = std::max(rectX, y);
            rectY = std::max(rectY, y);
            rect.setWidth(rect.x() + rectX);
            rect.setHeight(rect.y() + rectY);
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
        qDebug() << "ARROW POINTS";
        for(int i = 0; i < PTS_IN_ARROW; ++i)
            qDebug() << arrowDescr.arrowPointsPtr[i];

        switch(arrowStyle)
        {
            case BARBED:
            {
                QPolygonF points(arrowDescr.arrowPointsPtr);
                points[0] = points.boundingRect().center();
                painter->drawPolygon(points);
                break;
            }
            case SIMPLE:
            {
                QPolygonF points(arrowDescr.arrowPointsPtr);
                points.pop_front();
                painter->drawPolyline(points);
                break;
            }
            case TRIANGLE:
                painter->drawPolygon(QPolygonF(arrowDescr.arrowPointsPtr));
                break;
            default:
                break;
        }

    }
    QGraphicsPolygonItem::paint(painter, option, widget);
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
