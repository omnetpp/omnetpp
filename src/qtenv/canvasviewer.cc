//==========================================================================
//  CANVASVIEWER.CC - part of
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

#include "canvasviewer.h"
#include "canvasrenderer.h"
#include "qtenv.h"
#include "figurerenderers.h"
#include <QMouseEvent>
#include <QDebug>

#define emit

namespace omnetpp {
namespace qtenv {

CanvasViewer::CanvasViewer() :
    object(nullptr)
{
    figureLayer = new GraphicsLayer();

    canvasRenderer = new CanvasRenderer();
    canvasRenderer->setLayer(figureLayer, nullptr);

    setScene(new QGraphicsScene());
    scene()->addItem(figureLayer);

    // that beautiful green shade behind everything
    setBackgroundBrush(QColor("#a0e0a0"));
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

CanvasViewer::~CanvasViewer()
{
    delete canvasRenderer;
}

void CanvasViewer::setObject(cCanvas *obj)
{
    clear();
    object = obj;

    canvasRenderer->setCanvas(object);
    redraw();
}

void CanvasViewer::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        emit click(event);
}

void CanvasViewer::contextMenuEvent(QContextMenuEvent * event)
{
    emit contextMenuRequested(event);
}

void CanvasViewer::drawForeground(QPainter *painter, const QRectF &rect) {
    painter->save();

    auto font = painter->font();
    font.setBold(true);
    painter->setFont(font);

    QString text = "Zoom: " + QString::number(zoomFactor, 'f', 2) + "x";
    QFontMetrics fontMetrics(painter->font());

    QSize textSize = fontMetrics.boundingRect(text).size();
    QSize viewportSize = viewport()->size();

    // moving the whole thing 2 pixels to the left and up as spacing
    // and also adding 2 pixels to the left and right inside the grey area as margin
    // then the painter is in scene coords, so we have to map, and convert back to Rect
    textRect = mapToScene(viewportSize.width() - textSize.width() - 6,
                               viewportSize.height() - textSize.height() - 2,
                               textSize.width() + 4, textSize.height()).boundingRect();

    painter->fillRect(textRect, QColor("lightgrey"));
    // moving 2 pixels to the right and accounting for font descent, since the y coord is the baseline
    painter->drawText(textRect.bottomLeft() + QPoint(2, - fontMetrics.descent()), text);

    painter->restore();
}

void CanvasViewer::scrollContentsBy(int dx, int dy)
{
    //TODO fix zoom label refresh
    scene()->invalidate(textRect);
    QGraphicsView::scrollContentsBy(dx, dy);
}

void CanvasViewer::fillFigureRenderingHints(FigureRenderingHints *hints)
{
    QString prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":zoomfactor";
    QVariant variant = getQtenv()->getPref(prefName);
    hints->zoom = variant.isValid() ? variant.value<double>() : 1;

    prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":imagesizefactor";
    variant = getQtenv()->getPref(prefName);
    hints->iconMagnification = variant.isValid() ? variant.value<double>() : 1;

    prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":showlabels";
    variant = getQtenv()->getPref(prefName);
    hints->showSubmoduleLabels = variant.isValid() ? variant.value<bool>() : true;

    prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":showarrowheads";
    variant = getQtenv()->getPref(prefName);
    hints->showArrowHeads = variant.isValid() ? variant.value<bool>() : false;

    hints->defaultFont = scene()->font().family().toStdString();

    //TODO pixelSize() or pointSize()
    hints->defaultFontSize = scene()->font().pointSize() * 16 / 10;  // FIXME figure out conversion factor (point to pixel?)...
}

std::vector<cObject*> CanvasViewer::getObjectsAt(const QPoint &pos)
{
    QList<QGraphicsItem*> items = scene()->items(mapToScene(pos));
    std::vector<cObject*> objects;

    for (auto item : items) {
        QVariant variant = item->data(1);
        if (variant.isValid())
            objects.push_back(variant.value<cObject*>());
    }

    return objects;
}

void CanvasViewer::redraw()
{
    clear();
    if (object == nullptr)
        return;

    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    getCanvasRenderer()->redraw(&hints);
}

void CanvasViewer::refresh()
{
    if (!object) {
        clear();
        return;
    }

    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    getCanvasRenderer()->refresh(&hints);
}

void CanvasViewer::setZoomFactor(double zoomFactor) {
    if (this->zoomFactor != zoomFactor) {
        this->zoomFactor = zoomFactor;
        redraw();
        viewport()->update();
    }
}

void CanvasViewer::clear()
{
    figureLayer->clear();
}

} // namespace qtenv
} // namespace omnetpp
