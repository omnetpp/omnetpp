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

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include "common/stringutil.h"
#include "canvasrenderer.h"
#include "figurerenderers.h"
#include "tklib.h"
#include "tkutil.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace tkenv {

CanvasRenderer::CanvasRenderer() : interp(nullptr), canvas(nullptr), enabledTagBits(0), exceptTagBits(0)
{
}

CanvasRenderer::~CanvasRenderer()
{
}

void CanvasRenderer::setTkCanvas(Tcl_Interp *interp, const char *canvas)
{
    this->interp = interp;
    this->canvasWidget = canvas;

    int success = Tcl_GetCommandInfo(interp, TCLCONST(canvasWidget.c_str()), &canvasCmdInfo);
    ASSERT(success);
}

void CanvasRenderer::setCanvas(cCanvas *canvas)
{
    this->canvas = canvas;
    enabledTagBits = ~(uint64_t)0;  // all enabled
    exceptTagBits = 0;
}

void CanvasRenderer::refresh(FigureRenderingHints *hints)
{
    if (canvas) {
        // if there is a structural change, we rebuild everything;
        // otherwise we only adjust subtree of that particular figure
        cFigure *rootFigure = canvas->getRootFigure();
        uint8_t changes = rootFigure->getLocalChangeFlags() | rootFigure->getSubtreeChangeFlags();
        if (changes & (cFigure::CHANGE_STRUCTURAL | cFigure::CHANGE_TAGS)) {
            redraw(hints);
            // note: no rootFigure->clearChangeFlags() here, as there might be others inspecting the same canvas object
        }
        else if (changes != 0) {
            cFigure::Transform transform;
            transform.scale(hints->zoom);
            refreshFigureRec(rootFigure, transform, hints);
            // note: no rootFigure->clearChangeFlags() here, as there might be others inspecting the same canvas object
        }
    }
}

void CanvasRenderer::redraw(FigureRenderingHints *hints)
{
    // remove existing figures
    CHK(Tcl_VarEval(interp, canvasWidget.c_str(), " delete fig", TCL_NULL));

    // draw
    if (canvas) {
        cFigure::Transform transform;
        transform.scale(hints->zoom);
        drawFigureRec(canvas->getRootFigure(), transform, hints);
    }
}

FigureRenderer *CanvasRenderer::getRendererFor(cFigure *figure)
{
    return FigureRenderer::getRendererFor(figure);
}

bool CanvasRenderer::fulfillsTagFilter(cFigure *figure)
{
    uint64_t figureTagBits = figure->getTagBits();
    return figureTagBits == 0 || ((figureTagBits & enabledTagBits) != 0 && (figureTagBits & exceptTagBits) == 0);
}

void CanvasRenderer::drawFigureRec(cFigure *figure, const cFigure::Transform& parentTransform, FigureRenderingHints *hints)
{
    if (figure->isVisible() && fulfillsTagFilter(figure)) {
        cFigure::Transform transform(parentTransform);
        figure->updateParentTransform(transform);

        FigureRenderer *renderer = getRendererFor(figure);
        renderer->render(figure, interp, &canvasCmdInfo, transform, hints);

        for (int i = 0; i < figure->getNumFigures(); i++)
            drawFigureRec(figure->getFigure(i), transform, hints);
    }
}

void CanvasRenderer::refreshFigureRec(cFigure *figure, const cFigure::Transform& parentTransform, FigureRenderingHints *hints, bool ancestorTransformChanged)
{
    uint8_t localChanges = figure->getLocalChangeFlags();
    uint8_t subtreeChanges = figure->getSubtreeChangeFlags();

    if (localChanges & cFigure::CHANGE_TRANSFORM)
        ancestorTransformChanged = true;  // must refresh this figure and its entire subtree

    if (localChanges || subtreeChanges || ancestorTransformChanged) {
        cFigure::Transform transform(parentTransform);
        figure->updateParentTransform(transform);

        uint8_t what = localChanges | (ancestorTransformChanged ? cFigure::CHANGE_TRANSFORM : 0);
        if (what) {
            FigureRenderer *renderer = getRendererFor(figure);
            renderer->refresh(figure, what, interp, &canvasCmdInfo, transform, hints);
        }

        if (subtreeChanges || ancestorTransformChanged) {
            for (int i = 0; i < figure->getNumFigures(); i++)
                refreshFigureRec(figure->getFigure(i), transform, hints, ancestorTransformChanged);
        }
    }
}

void CanvasRenderer::assertCanvas()
{
    if (!canvas)
        throw cRuntimeError("CanvasRenderer: No canvas object");
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

}  // namespace tkenv
}  // namespace omnetpp

