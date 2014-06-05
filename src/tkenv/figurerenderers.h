//==========================================================================
//  FIGURERENDERERS.H - part of
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

#ifndef __FIGURERENDERERS_H
#define __FIGURERENDERERS_H

#include "tkdefs.h"
#include "ccanvas.h"

NAMESPACE_BEGIN

class TKENV_API FigureRenderer
{
    protected:
        char *point(const cFigure::Point& point, char *buf);
        std::string points(const std::vector<cFigure::Point>& points);
        char *color(const cFigure::Color& color, char *buf);
        char *itoa(int i, char *buf);
        char *arrows(cFigure::ArrowHead start, cFigure::ArrowHead end, char *buf);
        const char *lineStyle(cFigure::LineStyle style);
        const char *anchor(cFigure::Anchor anchor);
        const char *alignment(cFigure::Alignment alignment);

    public:
        FigureRenderer() {}
        virtual ~FigureRenderer() {}
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas) = 0;
};

class TKENV_API LineFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API PolylineFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API RectangleFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API OvalFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API PolygonFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API TextFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API ImageFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};


NAMESPACE_END

#endif
