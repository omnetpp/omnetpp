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

class TKENV_API ICoordMapping
{
    public:
        typedef cFigure::Point Point;
    public:
        virtual ~ICoordMapping() {}
        virtual Point apply(const Point& p) const = 0;
        virtual bool isLinear() const = 0;
};

class TKENV_API LinearCoordMapping : public ICoordMapping
{
    public:
        double scaleX, offsetX, scaleY, offsetY;
    public:
        LinearCoordMapping() : scaleX(1), offsetX(0), scaleY(1), offsetY(0) {}
        LinearCoordMapping(double scaleX, double offsetX, double scaleY, double offsetY) :
            scaleX(scaleX), offsetX(offsetX), scaleY(scaleY), offsetY(offsetY) {}
        virtual Point apply(const Point& p) const {return Point(scaleX * p.x + offsetX, scaleY * p.y + offsetY);}
        virtual bool isLinear() const {return true;}
};

class TKENV_API FigureRenderer : public cObject // for because Register_Class() takes cObject*
{
    private:
        static std::map<std::string, FigureRenderer*> rendererCache;
    protected:
        char *point(const cFigure::Point& point, ICoordMapping *mapping, char *buf);
        std::string points(const std::vector<cFigure::Point>& points, ICoordMapping *mapping);
        char *bounds(const cFigure::Rectangle& bounds, ICoordMapping *mapping, char *buf);
        char *color(const cFigure::Color& color, char *buf);
        char *itoa(int i, char *buf);
        static int round(double d) { return (int)floor(d+0.5);}
        char *arrows(cFigure::ArrowHead start, cFigure::ArrowHead end, char *buf);
        const char *smooth(bool b);
        const char *lineStyle(cFigure::LineStyle style);
        const char *capStyle(cFigure::CapStyle style);
        const char *joinStyle(cFigure::JoinStyle style);
        const char *anchor(cFigure::Anchor anchor);
        const char *alignment(cFigure::Alignment alignment);
        std::string font(const cFigure::Font& font);

    public:
        FigureRenderer() {}
        virtual ~FigureRenderer() {}
        static FigureRenderer *getRendererFor(cFigure *figure);
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping) = 0;
        virtual void refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping) = 0;
        virtual void refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas) = 0;
        virtual void remove(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API NullRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API LineFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API ArcFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API PolylineFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API RectangleFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API OvalFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API PieSliceFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API PolygonFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API TextFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};

class TKENV_API ImageFigureRenderer : public FigureRenderer
{
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping);
        virtual void refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas);
};


NAMESPACE_END

#endif
