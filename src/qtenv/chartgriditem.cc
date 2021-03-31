//==========================================================================
//  COORDINATESYSTEMITEM.CC - part of
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

#include "chartgriditem.h"
#include "qtutil.h"
#include "inspectorutil.h"
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtCore/QDebug>
#include <QtWidgets/QGraphicsScene>

static const int MINOR_TICK_LENGTH = 4;
static const int MAJOR_TICK_LENGTH = 6;
static const int VERT_TEXT_DISTANCE = 5;
static const int MINIMUM_DIAGRAM_SIZE = 10;
static const double DASHED_LINE_OPACITY = 0.3;

namespace omnetpp {
namespace qtenv {

using namespace common;

ChartGridItem::ChartGridItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    frame = new QGraphicsRectItem(this);
    frame->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
}

ChartGridItem::~ChartGridItem()
{
    delete frame;
}

QRectF ChartGridItem::boundingRect() const
{
    QRectF rect = boundingRectangle;
    rect.setY(-getTextHeight() / 2);
    rect.setHeight(rect.height() + getTextHeight() / 2);
    // This is just an estimate
    rect.setX(-getTextWidth(QString::number(minX)) / 2);
    rect.setWidth(rect.width() + getTextWidth(QString::number(minX)) / 2);

    return rect;
}

QGraphicsRectItem *ChartGridItem::getDiagramFrame()
{
    return frame;
}

void ChartGridItem::setMinX(const double x)
{
    if (minX != x) {
        minX = x;
        isXAxisChanged = true;
        update();
    }
}

void ChartGridItem::setMaxX(const double x)
{
    if (maxX != x) {
        maxX = x;
        isXAxisChanged = true;
        update();
    }
}

void ChartGridItem::setMinY(const double y)
{
    if (minY != y) {
        isYAxisChanged = true;
        minY = y;
        correctBoundingRectangle();
        updateRect();
        update();
    }
}

void ChartGridItem::setMaxY(const double y)
{
    if (maxY != y) {
        isYAxisChanged = true;
        maxY = y;
        correctBoundingRectangle();
        updateRect();
        update();
    }
}

void ChartGridItem::setRect(const QRectF& rect)
{
    ASSERT(rect.width() >= 0 && rect.height() >= 0);

    if (boundingRectangle != rect) {
        isYAxisChanged = true;
        isXAxisChanged = true;

        boundingRectangle = rect;
        correctBoundingRectangle();
        updateRect();
    }
}

void ChartGridItem::correctBoundingRectangle()
{
    int diagramWidth = boundingRectangle.width() - getYLabelWidth() - VERT_TEXT_DISTANCE;
    int diagramHeight = boundingRectangle.height() - getTextHeight();
    if (diagramWidth < MINIMUM_DIAGRAM_SIZE)
        boundingRectangle.setWidth(MINIMUM_DIAGRAM_SIZE + getYLabelWidth() + VERT_TEXT_DISTANCE);
    if (diagramHeight < MINIMUM_DIAGRAM_SIZE)
        boundingRectangle.setHeight(MINIMUM_DIAGRAM_SIZE + getTextHeight());
}

QPoint ChartGridItem::mapFromData(const double t, const double y)
{
    return QPoint(mapFromSimtime(t), mapFromValue(y));
}

int ChartGridItem::mapFromSimtime(const double t)
{
    double diagramWidth = std::max((double)MINIMUM_DIAGRAM_SIZE, boundingRectangle.width() - getYLabelWidth() - VERT_TEXT_DISTANCE);
    return (t - minX) * diagramWidth / (maxX - minX);
}

int ChartGridItem::mapFromValue(const double y)
{
    int diagramHeight = std::max((double)MINIMUM_DIAGRAM_SIZE, boundingRectangle.height() - getTextHeight());
    return (maxY - y) * diagramHeight / (maxY-minY);
}

void ChartGridItem::calculateYTicks()
{
    if (!isYAxisChanged)
        return;

    if (maxY - minY <= 0) {
        yTicks.clear();
        return;
    }

    double rangeY = mapFromValue(minY) - mapFromValue(maxY);

    double tickSize = (maxY - minY) / (rangeY / (2 * getTextHeight()));
    isYAxisChanged = false;

    yTicks = getLinearTicks(minY, maxY, tickSize);
}

double ChartGridItem::getYLabelWidth()
{
    if (!isYAxisChanged)
        return yLabelWidth;

    yLabelWidth = 0;
    calculateYTicks();
    for (std::pair<ChartTickDecimal, bool> tick : yTicks)
        if (tick.first >= minY && tick.first <= maxY && tick.second && getTextWidth(tick.first.str().c_str()) > yLabelWidth)
            yLabelWidth = getTextWidth(tick.first.str().c_str());


    return yLabelWidth;
}

inline int ChartGridItem::getTextWidth(const QString& text) const
{
    QFontMetrics m(scene()->font());
    int w = m.width(text);
    return w;
}

inline int ChartGridItem::getTextHeight() const
{
    QFontMetrics m(scene()->font());
    return m.lineSpacing();
}

void ChartGridItem::updateRect()
{
    QRectF rect(mapFromData(minX, maxY), mapFromData(maxX, minY));
    frame->setRect(rect);
    update();
}

void ChartGridItem::paintXAxis(QPainter *painter)
{
    ASSERT(maxX - minX > 0);

    if (isXAxisChanged) {
        // Draw Tick lines
        // tWidth have to store the max width of major ticks text. But major ticks text get from
        // getLinearTicks which use tickSize that's why add minimum width literal
        double tTextWidth = std::max(40, std::max(getTextWidth(QString::number((int)maxX)), getTextWidth(QString::number((int)minX))));
        double rangeX = mapFromSimtime(maxX) - mapFromSimtime(minX);
        if (rangeX <= 0) {
            xTicks.clear();
            return;
        }

        double tickSize = (maxX - minX) / (rangeX / tTextWidth);

        xTicks = getLinearTicks(minX, maxX, tickSize);
        isXAxisChanged = false;
    }

    int rangeY = mapFromValue(minY) - mapFromValue(maxY);
    for (std::pair<ChartTickDecimal, bool> tick : xTicks)
        if (tick.first >= minX && tick.first <= maxX) {
            int tickLength = std::min(tick.second ? MAJOR_TICK_LENGTH : MINOR_TICK_LENGTH, rangeY);
            if (tick.second) {
                QPointF pos = mapFromData(tick.first.dbl(), minY);
                pos.setX(pos.x() - getTextWidth(tick.first.str().c_str()) / 2);
                // North anchor
                QRectF rect(pos, QSizeF(getTextWidth(tick.first.str().c_str()), getTextHeight()));
                painter->drawText(rect, Qt::AlignCenter, tick.first.str().c_str());

                QPen pen = painter->pen();
                painter->setPen(QPen(Qt::DashLine));
                painter->setOpacity(DASHED_LINE_OPACITY);
                painter->drawLine(mapFromSimtime(tick.first.dbl()), mapFromValue(minY) - tickLength, mapFromSimtime(tick.first.dbl()), mapFromValue(maxY) + tickLength);
                painter->setOpacity(1);
                painter->setPen(pen);
            }
            painter->drawLine(mapFromSimtime(tick.first.dbl()), mapFromValue(minY), mapFromSimtime(tick.first.dbl()), mapFromValue(minY) - tickLength);
            painter->drawLine(mapFromSimtime(tick.first.dbl()), mapFromValue(maxY), mapFromSimtime(tick.first.dbl()), mapFromValue(maxY) + tickLength);
        }
}

void ChartGridItem::paintYAxis(QPainter *painter)
{
    calculateYTicks();

    int rangeX = mapFromSimtime(maxX) - mapFromSimtime(minX);
    for (std::pair<ChartTickDecimal, bool> tick : yTicks) {
        if (tick.first >= minY && tick.first <= maxY) {
            int tickLength = std::max(0, std::min(tick.second ? MAJOR_TICK_LENGTH : MINOR_TICK_LENGTH, rangeX));
            if (tick.second) {
                QPointF pos = mapFromData(maxX, tick.first.dbl());
                // West anchor
                pos.setX(pos.x() + VERT_TEXT_DISTANCE);
                pos.setY(pos.y() - getTextHeight() / 2);
                QRectF rect(pos, QSizeF(getTextWidth(tick.first.str().c_str()), getTextHeight()));
                painter->drawText(rect, Qt::AlignCenter, tick.first.str().c_str());

                QPen pen = painter->pen();
                painter->setPen(QPen(Qt::DashLine));
                painter->setOpacity(DASHED_LINE_OPACITY);
                painter->drawLine(mapFromSimtime(minX) + tickLength, mapFromValue(tick.first.dbl()), mapFromSimtime(maxX) - tickLength, mapFromValue(tick.first.dbl()));
                painter->setOpacity(1);
                painter->setPen(pen);
            }
            painter->drawLine(mapFromSimtime(maxX), mapFromValue(tick.first.dbl()), mapFromSimtime(maxX) - tickLength, mapFromValue(tick.first.dbl()));
            painter->drawLine(mapFromSimtime(minX), mapFromValue(tick.first.dbl()), mapFromSimtime(minX) + tickLength, mapFromValue(tick.first.dbl()));
        }
    }
}

void ChartGridItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // In these cases the drawing is meaningless
    if (boundingRectangle == QRectF() || maxX - minX <= 0 || maxY - minY <= 0)
        return;

    painter->setPen(QPen(Qt::black));
    paintXAxis(painter);
    paintYAxis(painter);
}

}  // namespace qtenv
}  // namespace omnetpp

