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

#include "histogramview.h"
#include <QMouseEvent>
#include "omnetpp/cdensityestbase.h"
#include "chartgriditem.h"
#include <QDebug>

#define emit

// unused? static const int MINOR_TICK_LENGTH = 4;
// unused? static const int MAJOR_TICK_LENGTH = 6;
// unused? static const int VERT_TEXT_DISTANCE = 5;
static const int MARGIN = 10;
static const QColor HISTOGRAM_COLOR("#1D3B9C");
static const QColor HIGHLIGHT_FILL_COLOR("#7792EB");
static const QColor HIGHLIGHT_LINE_COLOR("#F4D286");

namespace omnetpp {
namespace qtenv {

using namespace common;

HistogramView::HistogramView(QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent),
    actual(nullptr)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMouseTracking(true);

    gridItem = new ChartGridItem();
    gridItem->setPos(MARGIN, MARGIN);
    scene->addItem(gridItem);
}

QPointF HistogramView::mapToView(const double x, const double y) const
{
    return QPointF(mapXToView(x), mapYToView(y));
}

int HistogramView::mapXToView(const double x) const
{
    int histogramWidth = gridItem->getDiagramFrame()->boundingRect().width();
    return histogramWidth - (maxX - x) * histogramWidth / (maxX - minX);
}

int HistogramView::mapYToView(const double y) const
{
    int histogramHeight = gridItem->getDiagramFrame()->boundingRect().height();
    return (maxY - y) * histogramHeight / (maxY - minY);
}

void HistogramView::mouseMoveEvent(QMouseEvent *event)
{
    showInfo(event->pos());
}

// invalidate rect highlighting because the mouse sometime leaves view fast
// then no chance to call mouseMoveEvent
void HistogramView::leaveEvent(QEvent *)
{
    showInfo(QPoint(0, 0));
}

void HistogramView::showInfo(QPoint mousePos)
{
    if (actual) {
        if (drawingStyle == DRAW_FILLED)
            actual->setBrush(HISTOGRAM_COLOR);
        else {
            actual->setPen(HISTOGRAM_COLOR);
            actual->setZValue(0);
        }
        actual = nullptr;
    }

    int cell = -1;
    for (QGraphicsItem *item : items(mousePos))
        if (QGraphicsRectItem *rect = dynamic_cast<QGraphicsRectItem *>(item)) {
            if (rect == gridItem->getDiagramFrame())
                continue;

            if (drawingStyle == DRAW_FILLED)
                rect->setBrush(HIGHLIGHT_FILL_COLOR);
            else {
                rect->setPen(HIGHLIGHT_LINE_COLOR);
                rect->setZValue(1);
            }
            cell = rect->data(0).toInt();

            actual = rect;
            break;
        }

    emit showCellInfo(cell);
}

void HistogramView::setMinX(const double minX)
{
    this->minX = minX;
    gridItem->setMinX(minX);
}

void HistogramView::setMaxX(const double maxX)
{
    this->maxX = maxX;
    gridItem->setMaxX(maxX);
}

void HistogramView::setMinY(const double minY)
{
    this->minY = minY;
    gridItem->setMinY(minY);
}

void HistogramView::setMaxY(const double maxY)
{
    this->maxY = maxY;
    gridItem->setMaxY(maxY);
}

void HistogramView::draw(ChartType type, DrawingStyle drawingStyle, cDensityEstBase *distr)
{
    this->drawingStyle = drawingStyle;
    actual = nullptr;

    // delete previous drawing
    for (QGraphicsItem *item : gridItem->getDiagramFrame()->childItems())
        delete item;

    // set canvas size
    setSceneRect(rect());
    gridItem->setRect(QRectF(0, 0, width() - 2 * MARGIN, height() - 2 * MARGIN));

    // no need to draw histogram
    if (this->maxY < 0)
        return;

    // draw the histogram
    int prevLeftX = mapXToView(distr->getBasepoint(0));
    for (int cell = 0; cell < distr->getNumCells(); cell++) {
        // calculate height
        double y = type == SHOW_PDF ? distr->getCellPDF(cell) : distr->getCellValue(cell);

        // draw rectangle
        int width = std::max(1, mapXToView(distr->getBasepoint(cell+1)) - prevLeftX);
        if (y < minY) {
            prevLeftX += width;
            continue;
        }

        int rectMinY = std::max(0, mapYToView(y));
        // if minY == y then diagram frame hide the 1 pixel high rectangle
        // that's why it's made 2 pixel high rectangle thus 1 pixel is seen above the axis
        if (y == minY)
            --rectMinY;
        int height = mapYToView(0) + 1 - rectMinY;

        QBrush brush;
        QPen pen;
        if (drawingStyle == DRAW_FILLED) {
            brush = HISTOGRAM_COLOR;
            pen = Qt::NoPen;
        }
        else
            pen.setColor(HISTOGRAM_COLOR);

        QGraphicsRectItem *item = new QGraphicsRectItem(QRectF(prevLeftX, rectMinY, width, height), gridItem->getDiagramFrame());
        item->setData(0, cell);
        item->setPen(pen);
        item->setBrush(brush);
        prevLeftX += width;
    }

    showInfo(mapFromGlobal(QCursor::pos()));
}

}  // namespace qtenv
}  // namespace omnetpp

