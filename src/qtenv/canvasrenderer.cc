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

#include <QtCore/QDebug>

#include "canvasrenderer.h"
#include "figurerenderers.h"
#include "graphicsitems.h"
#include "common/stlutil.h"

namespace omnetpp {
namespace qtenv {

void CanvasRenderer::assertCanvas()
{
    if (!canvas)
        throw cRuntimeError("CanvasRenderer: No canvas object");
}

FigureRenderer *CanvasRenderer::getRendererFor(cFigure *figure)
{
    return FigureRenderer::getRendererFor(figure);
}

FigureRenderingArgs CanvasRenderer::makeRootArgs(const FigureRenderingHints *hints)
{
    assertCanvas();

    FigureRenderingArgs args;
    args.figure = canvas->getRootFigure();
    args.item = common::containsKey(items, args.figure)
            ? items[args.figure] : nullptr; // not inserting a nullptr with []
    args.zoom = hints->defaultZoom;
    args.zoomChanged = hints->defaultZoom != lastZoom;
    args.transform.scale(hints->defaultZoom);
    args.figure->updateParentTransform(args.transform);
    args.hints = hints;
    return args;
}

FigureRenderingArgs CanvasRenderer::makeChildArgs(const FigureRenderingArgs& args, int i)
{
    FigureRenderingArgs childArgs;
    childArgs.figure = args.figure->getFigure(i);
    childArgs.item = common::containsKey(items, childArgs.figure)
            ? items[childArgs.figure] : nullptr; // not inserting a nullptr with []
    childArgs.zoom = dynamic_cast<cPanelFigure *>(childArgs.figure) ? 1.0 : args.zoom;
    childArgs.zoomChanged = args.zoomChanged;
    childArgs.transform = args.transform;
    childArgs.figure->updateParentTransform(childArgs.transform);
    childArgs.hints = args.hints;
    return childArgs;
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

void CanvasRenderer::refresh(const FigureRenderingHints &hints)
{
    if (canvas) {
        // if there is a structural change, we rebuild everything;
        // otherwise we only adjust subtree of that particular figure
        cFigure *rootFigure = canvas->getRootFigure();
        uint8_t changes = rootFigure->getLocalChangeFlags() | rootFigure->getSubtreeChangeFlags();
        if (changes & (cFigure::CHANGE_STRUCTURAL | cFigure::CHANGE_TAGS))
            redraw(hints);
        else
            if (changes || hints.defaultZoom != lastZoom)
                refreshFigureRec(makeRootArgs(&hints), 0);

        lastZoom = hints.defaultZoom;
    }
}

void CanvasRenderer::redraw(const FigureRenderingHints &hints)
{
    if (networkLayer)
        layer->removeItem(networkLayer);

    layer->clear();
    items.clear();
    lastZoom = std::nan("");

    // draw
    if (canvas)
        drawFigureRec(makeRootArgs(&hints));
    else
        if (networkLayer)
            layer->addItem(networkLayer);

    lastZoom = hints.defaultZoom;
}

void CanvasRenderer::drawFigureRec(const FigureRenderingArgs& args)
{
    // figures that are not visible because of either their own flag, or the tag filter, are not rendered
    if (args.figure->isVisible() && fulfillsTagFilter(args.figure)) {
        QGraphicsItem *item = getRendererFor(args.figure)->render(args);
        if (item) { // some renderers (like group) do not actually render anything
            items[args.figure] = item;
            layer->addItem(item);
        }

        for (int i = 0; i < args.figure->getNumFigures(); i++)
            drawFigureRec(makeChildArgs(args, i));

        if ((canvas->getSubmodulesLayer() == args.figure) && networkLayer) {
            layer->addItem(networkLayer);
            networkLayer->setZValue(args.figure->getEffectiveZIndex());
        }
    }
}

void CanvasRenderer::refreshFigureRec(const FigureRenderingArgs& args, uint8_t ancestorChanges)
{
    // not skipping the root figure
    if ((args.figure->isVisible() && fulfillsTagFilter(args.figure)) || args.figure->getParentFigure() == nullptr) {
        uint8_t localChanges = args.figure->getLocalChangeFlags();
        uint8_t subtreeChanges = args.figure->getSubtreeChangeFlags();
        uint8_t inheritedChanges = ancestorChanges & (cFigure::CHANGE_TRANSFORM | cFigure::CHANGE_ZINDEX);

        if (localChanges || subtreeChanges || inheritedChanges || args.zoomChanged) {
            uint8_t what = localChanges | inheritedChanges;

            if (what || args.zoomChanged)
                 getRendererFor(args.figure)->refresh(args, what);

            if (subtreeChanges || what || args.zoomChanged)
                for (int i = 0; i < args.figure->getNumFigures(); i++)
                    refreshFigureRec(makeChildArgs(args, i), what);
        }
    }
}

QRectF CanvasRenderer::itemsBoundingRect() const
{
    QRectF bounds;

    for (auto &p : items) {
        QTransform sceneTransform = p.second->sceneTransform();
        QRectF itemBounds = p.second->boundingRect();
        QRectF mapped = sceneTransform.mapRect(itemBounds);

        if (!std::isnan(mapped.left()) && !std::isnan(mapped.right())
                && !std::isnan(mapped.top()) && !std::isnan(mapped.bottom()))
            bounds = bounds.united(mapped);
    }

    return bounds;
}

std::vector<std::string> CanvasRenderer::getAllTagsAsVector()
{
    assertCanvas();
    return canvas->getAllTagsAsVector();
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

}  // namespace qtenv
}  // namespace omnetpp

