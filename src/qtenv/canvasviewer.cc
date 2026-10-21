//==========================================================================
//  CANVASVIEWER.CC - part of
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

#include "canvasviewer.h"
#include "canvasrenderer.h"
#include "graphicsitems.h"
#include "qtenv.h"
#include "figurerenderers.h"
#include <QtGui/QMouseEvent>
#include <QtCore/QDebug>

namespace omnetpp {
namespace qtenv {

CanvasViewer::CanvasViewer()

{
    figureLayer = new GraphicsLayer();

    canvasRenderer = new CanvasRenderer();
    canvasRenderer->setLayer(figureLayer, nullptr);

    setScene(new QGraphicsScene());
    scene()->addItem(figureLayer);

    // that beautiful green shade behind everything
    setBackgroundBrush(colors::LIGHTGREEN);
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

void CanvasViewer::setZoomFactor(double zoomFactor)
{
}

void CanvasViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        Q_EMIT click(event);
}

void CanvasViewer::contextMenuEvent(QContextMenuEvent *event)
{
    Q_EMIT contextMenuRequested(event);
}

FigureRenderingHints CanvasViewer::makeFigureRenderingHints()
{
    FigureRenderingHints hints;
    QString prefName = object->getFullName() + QString(":") + QString::number((int)INSP_DEFAULT) + ":zoomfactor";
    QVariant variant = getQtenv()->getPref(prefName);
    hints.defaultZoom = variant.isValid() ? variant.value<double>() : 1;
/*
    prefName = object->getFullName() + QString(":") + QString::number((int)INSP_DEFAULT) + ":imagesizefactor";
    variant = getQtenv()->getPref(prefName);
    hints->iconMagnification = variant.isValid() ? variant.value<double>() : 1;

    prefName = object->getFullName() + QString(":") + QString::number((int)INSP_DEFAULT) + ":showlabels";
    variant = getQtenv()->getPref(prefName);
    hints->showSubmoduleLabels = variant.isValid() ? variant.value<bool>() : true;

    prefName = object->getFullName() + QString(":") + QString::number((int)INSP_DEFAULT) + ":showarrowheads";
    variant = getQtenv()->getPref(prefName);
    hints->showArrowheads = variant.isValid() ? variant.value<bool>() : false;
*/
    hints.defaultFont = scene()->font().family().toStdString();

    //TODO use getQtenv()->getCanvasFont()
    hints.defaultFontSize = scene()->font().pointSize();

    return hints;
}

std::vector<cObject *> CanvasViewer::getObjectsAt(const QPoint& pos)
{
    QPointF scenePos = mapToScene(pos);
    QList<QGraphicsItem *> items = scene()->items(scenePos);
    QPainterPath area;
    area.addRect(QRectF(scenePos, QSizeF(1, 1))); // this is also what QGraphicsScene::items() tests with
    removeClippedItemsOutsideShape(items, area);
    std::vector<cObject *> objects;

    for (auto item : items) {
        QVariant variant = item->data(ITEMDATA_COBJECT);
        if (variant.isValid())
            objects.push_back(variant.value<cObject *>());
    }

    return objects;
}

void CanvasViewer::redraw()
{
    clear();
    if (object == nullptr)
        return;

    object->getRootFigure()->callRefreshDisplay();

    getCanvasRenderer()->redraw(makeFigureRenderingHints());
}

void CanvasViewer::refresh()
{
    if (!object) {
        clear();
        return;
    }

    object->getRootFigure()->callRefreshDisplay();

    getCanvasRenderer()->refresh(makeFigureRenderingHints());
}

void CanvasViewer::clear()
{
    figureLayer->clear();
}

}  // namespace qtenv
}  // namespace omnetpp

