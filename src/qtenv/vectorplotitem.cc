//==========================================================================
//  VECTIRPLOTITEM.CC - part of
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

#include "vectorplotitem.h"
#include "omnetpp/simkerneldefs.h"
#include <QtGui/QPainter>

static const int DOTS_SIZE = 4;

namespace omnetpp {
namespace qtenv {

VectorPlotItem::VectorPlotItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
}

void VectorPlotItem::setMinX(double minX)
{
    if (this->minX != minX) {
        this->minX = minX;
        update();
    }
}

void VectorPlotItem::setMaxX(double maxX)
{
    if (this->maxX != maxX) {
        this->maxX = maxX;
        update();
    }
}

void VectorPlotItem::setMinY(double minY)
{
    if (this->minY != minY) {
        this->minY = minY;
        update();
    }
}

void VectorPlotItem::setMaxY(double maxY)
{
    if (this->maxY != maxY) {
        this->maxY = maxY;
        update();
    }
}

void VectorPlotItem::setData(const CircBuffer *circBuf)
{
    this->circBuf = circBuf;
    update();
}

VectorPlotItem::PlottingMode VectorPlotItem::getPlottingMode() const
{
    return plottingMode;
}

void VectorPlotItem::setPlottingMode(const PlottingMode plottingMode)
{
    if (this->plottingMode != plottingMode) {
        this->plottingMode = plottingMode;
        update();
    }
}

void VectorPlotItem::setColor(const QColor& color)
{
    if (this->color != color) {
        this->color = color;
        update();
    }
}

void VectorPlotItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (circBuf == nullptr || circBuf->items() == 0 || parentItem() == nullptr)
        return;

    painter->setPen(color);
    painter->setBrush(color);

    if (plottingMode == DRAW_LINES)
        painter->setRenderHint(QPainter::Antialiasing);

    // if use next X/Y then next for loop repeat until size - 1
    int upperLimit = circBuf->items();
    if (plottingMode == DRAW_BACKWARD_SAMPLEHOLD || plottingMode == DRAW_BARS || plottingMode == DRAW_LINES || plottingMode == DRAW_SAMPLEHOLD)
        --upperLimit;

    for (int i = 0, pos = circBuf->tailPos(); i < upperLimit; ++i, pos = (pos + 1) % circBuf->size()) {
        CircBuffer::CBEntry& entry = circBuf->entry(pos);

        double x = mapFromSimtime(entry.t);
        double y = mapFromValue(entry.value);
        double nextX = x, nextY = y;
        if (upperLimit == circBuf->items() - 1) {
            CircBuffer::CBEntry& entry = circBuf->entry((pos + 1) % circBuf->size());
            nextX = mapFromSimtime(entry.t);
            nextY = mapFromValue(entry.value);
        }

        double width = DOTS_SIZE, height = DOTS_SIZE;
        switch (plottingMode) {
            case DRAW_DOTS:
                painter->drawRect(x-width/2, y-height/2, width, height);
                break;

            case DRAW_POINTS:
                painter->drawPoint(x, y);
                break;

            case DRAW_PINS:
                painter->drawLine(x, mapFromValue(0), x, y);
                painter->drawRect(x-width/2, y-height/2, width, height);
                break;

            case DRAW_LINES:
                painter->drawLine(x, y, nextX, nextY);
                break;

            case DRAW_SAMPLEHOLD: {
                // Add horizontal line
                painter->drawLine(x, y, nextX, y);

                // Add dot
                painter->drawRect(x-width/2, y-height/2, width, height);

                // Add vertical dashed line
                QPen pen = painter->pen();
                Qt::PenStyle penStyle = pen.style();
                pen.setStyle(Qt::DashLine);
                painter->setPen(pen);
                painter->drawLine(nextX, y, nextX, nextY);

                pen.setStyle(penStyle);
                painter->setPen(pen);
                break;
            }

            case DRAW_BACKWARD_SAMPLEHOLD: {
                // Add horizontal line
                painter->drawLine(x, nextY, nextX, nextY);

                // Add dot
                painter->drawRect(nextX-width/2, nextY-height/2, width, height);

                // Add first dot
                if (i == 0)
                    painter->drawRect(x-width/2, y-height/2, width, height);

                // Add vertical dashed line
                QPen pen = painter->pen();
                Qt::PenStyle penStyle = pen.style();
                pen.setStyle(Qt::DashLine);
                painter->setPen(pen);
                painter->drawLine(x, y, x, nextY);

                pen.setStyle(penStyle);
                painter->setPen(pen);
                break;
            }

            case DRAW_BARS:
                painter->drawRect(x, y, nextX - x, mapFromValue(0) - y);
                break;
        }
    }
}

QRectF VectorPlotItem::boundingRect() const
{
    if (parentItem())
        return parentItem()->boundingRect();
    else
        return QRectF();
}

inline int VectorPlotItem::mapFromSimtime(const simtime_t& t)
{
    return (t.dbl() - minX) * boundingRect().width() / (maxX - minX);
}

inline int VectorPlotItem::mapFromValue(double y)
{
    return (maxY - y) * boundingRect().height() / (maxY-minY);
}

}  // namespace qtenv
}  // namespace omnetpp

