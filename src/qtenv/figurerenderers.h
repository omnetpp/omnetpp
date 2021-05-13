//==========================================================================
//  FIGURERENDERERS.H - part of
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

#ifndef __OMNETPP_QTENV_FIGURERENDERERS_H
#define __OMNETPP_QTENV_FIGURERENDERERS_H

#include <string>
#include <map>
#include <QtWidgets/QGraphicsPathItem>
#include "omnetpp/ccanvas.h"
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

class ArrowheadItem;

// this comes from way outside, from the inspector, through the CanvasRenderer,
// and is constant during a single refresh
struct QTENV_API FigureRenderingHints
{ // all of these should be overwritten (in the inspector) before use
    double defaultZoom = 1; // XXX name: topLevelZoom? not really a default...
    std::string defaultFont = "Arial";
    int defaultFontSize = 12;
};

// this is a bit more private, only used by CanvasRenderer and FigureRenderer,
// and changes on every figure during tree traversal
struct QTENV_API FigureRenderingArgs {
    cFigure *figure;
    QGraphicsItem *item; // nullptr if rendering (not refreshing)
    cFigure::Transform transform; // cumulative, includes the actual figure's transform
    double zoom; // same as hints->defaultZoom most of the time, 1.0 inside panel figures
    bool zoomChanged; // well, this one is constant, but not putting it into "hints"
    const FigureRenderingHints *hints;
};

class QTENV_API FigureRenderer : public cObject // because Register_Class() takes cObject*
{
protected:
    // This class should be used in place of every QGraphicsPathItem
    // (for the polyline, polygon, ring and path figures - these
    // are the ones that can have holes in them) because this
    // does not include the filling area in its shape() if
    // the figure is not in fact filled. The Qt one does, and it
    // messes up mouse selection (picking, collision detection, etc...)
    // UPDATE: Turns out, this should be used in all cases where there
    // is an optional outline with optional fill. So basically, with
    // ALL shape figures, including simple ovals and rects, pieslices, etc,
    // XXX move this to graphicsitems.[h|cc]?
    class PathItem : public QGraphicsPathItem {
    public:
        using QGraphicsPathItem::QGraphicsPathItem;
        QPainterPath shape() const override;
        //For debugging purposes only:
        //void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
    };

private:
    enum ArcShape { ARC_NOTHING, ARC_LINE, ARC_ARC };
    static std::map<std::string, FigureRenderer*> rendererCache;

    static inline bool doubleEquals(double x, double y);
    static double calcVectorAngle(double ux, double uy, double vx, double vy);
    static ArcShape endPointToCentralFromArcParameters(
            double startX, double startY, double endX, double endY,
            bool largeArcFlag, bool sweepFlag, double &rx, double &ry, double angle,
            double &centerX, double &centerY, double &startAngle, double &sweepLength);

protected:

    static QPointF getAnchorOffset(cFigure::Anchor anchor, double width, double height, double ascent = 0);

    static void arcToUsingBezier(QPainterPath &path, double currentX,
                          double currentY, double x, double y,
                          double rx, double ry, double angle,
                          bool largeArcFlag, bool sweepFlag, bool isRel);

    static void calcSmoothQuadraticBezierCP(QPainterPath &path, char prevCommand,
                            double currentX, double currentY,
                            double &cpx, double &cpy,
                            double x, double y,
                            bool isRel = false);

    static void calcSmoothCubicBezierCP(QPainterPath &path, char prevCommand,
                                double currentX, double currentY,
                                double &prevCpx, double &prevCpy,
                                double cpx, double cpy,
                                double x, double y, bool isRel = false);

    static QPen createPen(const FigureRenderingArgs& args);
    static QBrush createBrush(const cAbstractShapeFigure *figure);

    virtual QGraphicsItem *newItem() = 0;

    virtual void refreshGeometry(const FigureRenderingArgs& args) { }
    virtual void refreshInputData(const FigureRenderingArgs& args) { }
    virtual void refreshVisual(const FigureRenderingArgs& args) { }
    virtual void refreshTransform(const FigureRenderingArgs& args);
    virtual void refreshZoom(const FigureRenderingArgs& args);

public:
    static FigureRenderer *getRendererFor(cFigure *figure);
    static void clearRendererCache();
    virtual QGraphicsItem *render(const FigureRenderingArgs& args);
    virtual void refresh(const FigureRenderingArgs& args, uint8_t what);
};

class QTENV_API AbstractShapeFigureRenderer : public FigureRenderer
{
protected:
    QGraphicsItem *newItem() override;
    virtual void refreshVisual(const FigureRenderingArgs& args) override;
    virtual void refreshZoom(const FigureRenderingArgs& args) override;
};

class QTENV_API NullRenderer : public FigureRenderer
{
protected:
    virtual QGraphicsItem *newItem() override { return nullptr; }
};

class QTENV_API GroupFigureRenderer : public NullRenderer
{
    // nothing
};

class QTENV_API PanelFigureRenderer : public NullRenderer
{
    // nothing
};

class QTENV_API AbstractLineFigureRenderer : public FigureRenderer
{
    // every item returned by newItem() of subclasses must have
    // the start and end arrowheads in it as child items 0 and 1
protected:
    // only set position and rotation in setArrows overrides.
    // style is taken care of in this refreshVisual
    virtual void setArrows(const FigureRenderingArgs& args) = 0;
    virtual void refreshVisual(const FigureRenderingArgs& args) override;
    virtual void refreshZoom(const FigureRenderingArgs& args) override;
};

class QTENV_API LineFigureRenderer : public AbstractLineFigureRenderer
{
protected:
    virtual void setArrows(const FigureRenderingArgs& args) override;
    virtual QGraphicsItem *newItem() override;
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
};

class QTENV_API ArcFigureRenderer : public AbstractLineFigureRenderer
{
protected:
    virtual void setArrows(const FigureRenderingArgs& args) override;
    virtual QGraphicsItem *newItem() override;
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
};

class QTENV_API PolylineFigureRenderer : public AbstractLineFigureRenderer
{
protected:
    virtual void setArrows(const FigureRenderingArgs& args) override;
    virtual QGraphicsItem *newItem();
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
    virtual void refreshVisual(const FigureRenderingArgs& args) override;
};

class QTENV_API RectangleFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
};

class QTENV_API OvalFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
};

class QTENV_API RingFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
};

class QTENV_API PieSliceFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
};

class QTENV_API PolygonFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
    virtual void refreshVisual(const FigureRenderingArgs& args) override;
};

class QTENV_API PathFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
    virtual void refreshVisual(const FigureRenderingArgs& args) override;
};

class QTENV_API TextFigureRenderer : public FigureRenderer
{
protected:
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
    virtual void refreshInputData(const FigureRenderingArgs& args) override;
    virtual void refreshVisual(const FigureRenderingArgs& args) override;
    virtual void refreshTransform(const FigureRenderingArgs& args) override;
    virtual void refreshZoom(const FigureRenderingArgs& args) override;
    virtual QGraphicsItem *newItem();
};

class QTENV_API LabelFigureRenderer : public TextFigureRenderer
{
protected:
    virtual void refreshTransform(const FigureRenderingArgs& args) override;
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
    virtual void refreshZoom(const FigureRenderingArgs& args) override;
};

class QTENV_API ImageFigureRenderer : public FigureRenderer
{
protected:
    static void setImageTransform(const FigureRenderingArgs& args, double naturalWidth, double naturalHeight, bool translateOnly);
    virtual void refreshGeometry(const FigureRenderingArgs& args) override;
    virtual void refreshInputData(const FigureRenderingArgs& args) override;
    virtual void refreshVisual(const FigureRenderingArgs& args) override;
    virtual void refreshTransform(const FigureRenderingArgs& args) override;
    virtual QGraphicsItem *newItem() override;
};

class QTENV_API PixmapFigureRenderer : public ImageFigureRenderer
{
protected:
    virtual void refreshInputData(const FigureRenderingArgs& args) override;
    virtual void refreshTransform(const FigureRenderingArgs& args) override;
};

class QTENV_API IconFigureRenderer : public ImageFigureRenderer
{
protected:
    virtual void refreshTransform(const FigureRenderingArgs& args) override;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif
