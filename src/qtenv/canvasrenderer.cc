//==========================================================================
//  CANVASRENDERER.CC - part of
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

#include <QDebug>

#include "canvasrenderer.h"
#include "figurerenderers.h"
#include "graphicsitems.h"

namespace omnetpp {
namespace qtenv {

FigureRenderer *CanvasRenderer::getRendererFor(cFigure *figure)
{
    return FigureRenderer::getRendererFor(figure);
}

bool CanvasRenderer::fulfillsTagFilter(cFigure *figure)
{
    uint64_t figureTagBits = figure->getTagBits();
    return figureTagBits == 0 || ((figureTagBits & enabledTagBits) != 0 && (figureTagBits & exceptTagBits) == 0);
}

// TODO: delete comment when ASSERT is available
void CanvasRenderer::setLayer(GraphicsLayer *layer, cCanvas *canvas, GraphicsLayer *networkLayer)
{
    this->layer = layer;
    this->canvas = canvas;
    this->networkLayer = networkLayer;

    if (networkLayer)
        this->layer->addItem(networkLayer);

    // ASSERT(canvas);
}

void CanvasRenderer::setCanvas(cCanvas *canvas)
{
    this->canvas = canvas;
    enabledTagBits = ~(uint64_t)0;  // all enabled
    exceptTagBits = 0;
}

void CanvasRenderer::redraw(FigureRenderingHints *hints)
{
    if (networkLayer)
        layer->removeItem(networkLayer);

    layer->clear();
    layer->setScale(hints->zoom);
    items.clear();

    // draw
    if (canvas) {
        // Shouldn't draw the rootFigure itself, because then the networkLayer
        // can't be shoved between individual "real" top-level figures.
        auto root = canvas->getRootFigure();
        for (int i = 0; i < root->getNumFigures(); ++i)
            drawFigureRec(root->getFigure(i), hints);

        // But we add the layer to or little map as conceptual item for it just to be sure...
        items[root] = layer;
    }
    else
        if (networkLayer)
            layer->addItem(networkLayer);
}

QRectF CanvasRenderer::itemsBoundingRect() const
{
    QRectF bounds;

    for (auto &p : items) {
        // deviceTransform is needed to respect ItemIgnoresTransformations flag.
        // Using identity viewport transform because we want scene coords.
        QTransform tf = p.second->deviceTransform(QTransform());
        bounds = bounds.united(tf.mapRect(p.second->boundingRect()));
    }

    return bounds;
}

// TODO: delete comment when cRuntimeError class is available
void CanvasRenderer::assertCanvas()
{
    if (!canvas) {
    }  // throw cRuntimeError("CanvasRenderer: No canvas object");
}

std::string CanvasRenderer::getAllTags()
{
    assertCanvas();
    return canvas->getAllTags();
}

std::string CanvasRenderer::getEnabledTags()
{
    assertCanvas();
    return canvas->getTags(enabledTagBits);
}

std::string CanvasRenderer::getExceptTags()
{
    assertCanvas();
    return canvas->getTags(exceptTagBits);
}

void CanvasRenderer::setEnabledTags(const char *tags)
{
    assertCanvas();
    enabledTagBits = canvas->parseTags(tags);
    uint64_t allTagBits = canvas->parseTags(canvas->getAllTags().c_str());
    enabledTagBits |= ~allTagBits;  // set extra bits to 1, so potential new tags will be enabled by default
}

void CanvasRenderer::setExceptTags(const char *tags)
{
    assertCanvas();
    exceptTagBits = canvas->parseTags(tags);
}

void CanvasRenderer::drawFigureRec(cFigure *figure, FigureRenderingHints *hints)
{
    if (figure->isVisible() && fulfillsTagFilter(figure)) {
        FigureRenderer *renderer = getRendererFor(figure);
        QGraphicsItem *item = renderer->render(figure, layer, hints);

        if (item) {
            items[figure] = item;

            auto parentFigure = figure->getParentFigure();
            auto parentItem = items.count(parentFigure) > 0 ? items[parentFigure] : layer;
            item->setParentItem(parentItem);

            item->setZValue(figure->getZIndex());
        }

        for (int i = 0; i < figure->getNumFigures(); i++)
            drawFigureRec(figure->getFigure(i), hints);

        if (canvas->getSubmodulesLayer() == figure && networkLayer) {
            layer->addItem(networkLayer);
            networkLayer->setZValue(figure->getZIndex());
        }
    }
}

void CanvasRenderer::refresh(FigureRenderingHints *hints)
{
    if (canvas) {
        // if there is a structural change, we rebuild everything;
        // otherwise we only adjust subtree of that particular figure
        cFigure *rootFigure = canvas->getRootFigure();
        uint8_t changes = rootFigure->getLocalChangeFlags() | rootFigure->getSubtreeChangeFlags();
        if (changes & cFigure::CHANGE_STRUCTURAL) {
            redraw(hints);
            // note: no rootFigure->clearChangeFlags() here, as there might be others inspecting the same canvas object
        }
        else if (changes != 0) {
            refreshFigureRec(rootFigure, hints);
            // note: no rootFigure->clearChangeFlags() here, as there might be others inspecting the same canvas object
        }
    }
}

void CanvasRenderer::refreshFigureRec(cFigure *figure, FigureRenderingHints *hints)
{
    uint8_t localChanges = figure->getLocalChangeFlags();
    uint8_t subtreeChanges = figure->getSubtreeChangeFlags();

    if (localChanges && items.count(figure))
        getRendererFor(figure)->refresh(figure, items[figure], localChanges, hints);

    if (subtreeChanges)
        for (int i = 0; i < figure->getNumFigures(); i++)
            refreshFigureRec(figure->getFigure(i), hints);
}

}  // namespace qtenv
}  // namespace omnetpp

