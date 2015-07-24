//==========================================================================
//  CANVASRENDERER.H - part of
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

#ifndef __OMNETPP_QTENV_CANVASRENDERER_H
#define __OMNETPP_QTENV_CANVASRENDERER_H

#include "omnetpp/ccanvas.h"
#include "moduleinspector.h"
#include <QGraphicsScene>

namespace omnetpp {
namespace qtenv {

class FigureRenderer;
struct FigureRenderingHints;

class CanvasRenderer
{
protected:
    GraphicsLayer *layer;
    cCanvas *canvas;    // nullptr is allowed
    uint64_t enabledTagBits, exceptTagBits;

protected:
    void assertCanvas();
    virtual FigureRenderer *getRendererFor(cFigure *figure);
    virtual void drawFigureRec(cFigure *figure, const cFigure::Transform& parentTransform, FigureRenderingHints *hints);
    virtual void refreshFigureRec(cFigure *figure, const cFigure::Transform& parentTransform, FigureRenderingHints *hints, bool ancestorTransformChanged = false);
    virtual bool fulfillsTagFilter(cFigure *figure);

public:
    CanvasRenderer() :
        canvas(nullptr), enabledTagBits(0), exceptTagBits(0) {}
    virtual ~CanvasRenderer() {}

    virtual void setLayer(GraphicsLayer *layer, cCanvas *canvas);
    virtual void setCanvas(cCanvas *canvas);
    virtual bool hasCanvas() {return canvas != nullptr;}
    virtual void refresh(FigureRenderingHints *hints);
    virtual void redraw(FigureRenderingHints *hints);

    // tag-based filtering
    virtual std::string getAllTags();
    virtual std::string getEnabledTags();
    virtual std::string getExceptTags();
    virtual void setEnabledTags(const char* tags);
    virtual void setExceptTags(const char* tags);
};

} // namespace qtenv
} // namespace omnetpp

#endif
