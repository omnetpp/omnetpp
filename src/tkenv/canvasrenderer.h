//==========================================================================
//  CANVASRENDERER.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
 *--------------------------------------------------------------*/

#ifndef __CANVASRENDERER_H
#define __CANVASRENDERER_H

#include "platmisc.h"   // must precede <tk.h> otherwise Visual Studio 2013 fails to compile
#include <tk.h>
#include "ccanvas.h"
#include "tkdefs.h"

NAMESPACE_BEGIN

class cFigure;
class FigureRenderer;
struct FigureRenderingHints;

class TKENV_API CanvasRenderer
{
    protected:
        Tcl_Interp *interp;
        std::string canvasWidget;
        Tcl_CmdInfo canvasCmdInfo;
        cCanvas *canvas; // NULL is allowed
        uint64_t enabledTagBits, exceptTagBits;

    protected:
        void assertCanvas();
        virtual FigureRenderer *getRendererFor(cFigure *figure);
        virtual void drawFigureRec(cFigure *figure, const cFigure::Transform& parentTransform, FigureRenderingHints *hints);
        virtual void refreshFigureRec(cFigure *figure, const cFigure::Transform& parentTransform, FigureRenderingHints *hints, bool ancestorTransformChanged=false);
        virtual bool fulfillsTagFilter(cFigure *figure);

    public:
        CanvasRenderer();
        virtual ~CanvasRenderer();
        virtual void setTkCanvas(Tcl_Interp *interp, const char *canvas);
        virtual void setCanvas(cCanvas *canvas);
        virtual bool hasCanvas() {return canvas != NULL;}
        virtual void refresh(FigureRenderingHints *hints);
        virtual void redraw(FigureRenderingHints *hints);

        // tag-based filtering
        virtual std::string getAllTags();
        virtual std::string getEnabledTags();
        virtual std::string getExceptTags();
        virtual void setEnabledTags(const char* tags);
        virtual void setExceptTags(const char* tags);
};

NAMESPACE_END

#endif
