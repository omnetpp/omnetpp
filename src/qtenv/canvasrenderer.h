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

#include <QtWidgets/QGraphicsItem>
#include "omnetpp/ccanvas.h"
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

class GraphicsLayer;
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

protected:
    void assertCanvas();
    FigureRenderer *getRendererFor(cFigure *figure);
    FigureRenderingArgs makeRootArgs(const FigureRenderingHints *hints);
    FigureRenderingArgs makeChildArgs(const FigureRenderingArgs& args, int i);
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
    void setEnabledTags(const char* tags);
    void setExceptTags(const char* tags);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif
