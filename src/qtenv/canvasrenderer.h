//==========================================================================
//  CANVASRENDERER.H - part of
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

#ifndef __OMNETPP_QTENV_CANVASRENDERER_H
#define __OMNETPP_QTENV_CANVASRENDERER_H

#include <tuple>
#include <QtGui/QPainterPath>
#include <QtWidgets/QGraphicsItem>
#include "omnetpp/ccanvas.h"
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

class GraphicsLayer;
class ClipContainerItem;
class FigureRenderer;
struct FigureRenderingHints;
struct FigureRenderingArgs;

class QTENV_API CanvasRenderer
{
protected:
    GraphicsLayer *layer = nullptr;
    GraphicsLayer *networkLayer = nullptr;

    cCanvas *canvas = nullptr;    // nullptr is allowed
    uint64_t enabledTagBits = 0, exceptTagBits = 0;
    double lastZoom = std::nan(""); // the canvas was last refreshed with this zoom

    std::map<cFigure *, QGraphicsItem*> items;

    // Clipping (see cAbstractShapeFigure::setClipChildren()): the item of a figure under
    // clipping figures is put into a chain of ClipContainerItems, one for each clipping
    // ancestor, outermost first. The outermost container is in the layer and has the
    // effective Z-index of the figure as Z value, so clipping does not affect the stacking
    // order. Figures with the same clipping ancestors and effective Z-index share containers:
    // the stacking order is by effective Z-index first, then by preorder, so these figures
    // are contiguous in it.
    struct Clipper {
        QPainterPath clipPath; // in layer coordinates
        std::vector<ClipContainerItem *> containerItems; // the containers that clip to this figure
    };
    struct ClipContainer {
        ClipContainerItem *item;
        std::vector<cFigure *> figures; // whose items are in the container, directly or in nested containers
    };
    using ClipContainerKey = std::tuple<QGraphicsItem *, cFigure *, double>; // parent item (nullptr: the layer), clipping figure, effective Z-index
    std::map<cFigure *, Clipper> clippers;
    std::map<ClipContainerKey, ClipContainer> clipContainers;
    std::vector<cFigure *> activeClippers; // the clipping ancestors of the figure being drawn, outermost first

protected:
    void assertCanvas();
    FigureRenderer *getRendererFor(cFigure *figure);
    FigureRenderingArgs makeRootArgs(const FigureRenderingHints *hints);
    FigureRenderingArgs makeChildArgs(const FigureRenderingArgs& args, int i);
    void addItem(cFigure *figure, QGraphicsItem *item);
    void refreshClipPath(const FigureRenderingArgs& args, Clipper& clipper);
    bool refreshClipContainerZIndices();
    void drawFigureRec(const FigureRenderingArgs& args);
    void refreshFigureRec(const FigureRenderingArgs& args, uint8_t ancestorChanges);
    bool fulfillsTagFilter(cFigure *figure);

public:
    void setLayer(GraphicsLayer *layer, cCanvas *canvas, GraphicsLayer *networkLayer = nullptr);
    void setCanvas(cCanvas *canvas);
    bool hasCanvas() {return canvas != nullptr;}
    void refresh(const FigureRenderingHints& hints);
    void redraw(const FigureRenderingHints& hints);

    QRectF itemsBoundingRect() const;

    // tag-based filtering
    std::vector<std::string> getAllTagsAsVector();
    std::string getEnabledTags();
    std::string getExceptTags();
    void setEnabledTags(const char *tags);
    void setExceptTags(const char *tags);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif
