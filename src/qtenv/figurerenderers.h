//==========================================================================
//  FIGURERENDERERS.H - part of
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

#ifndef __OMNETPP_QTENV_FIGURERENDERERS_H
#define __OMNETPP_QTENV_FIGURERENDERERS_H

#include <string>
#include <map>

#include "moduleinspector.h"
#include "omnetpp/ccanvas.h"

class QPen;
class QGraphicsScene;
class QGraphicsItem;
class QGraphicsLineItem;
class QGraphicsPathItem;
class QBrush;
class QPainter;
class QPainterPath;
class QPointF;

namespace omnetpp {
namespace qtenv {

struct FigureRenderingHints
{ // all of these should be overwritten before use
    double zoom = 1;
    std::string defaultFont = "Arial";
    int defaultFontSize = 12;
};

class FigureRenderer : public cObject // because Register_Class() takes cObject*
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
    // ALL shape figures, including simple ovals and rects, piesices, etc,
    class PathItem : public QGraphicsPathItem {
    public:
        using QGraphicsPathItem::QGraphicsPathItem;
        QPainterPath shape() const override;
        QRectF boundingRect() const override;
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

    void arcToUsingBezier(QPainterPath &painter, double currentX,
                          double currentY, double x, double y,
                          double rx, double ry, double angle,
                          bool largeArcFlag, bool sweepFlag, bool isRel) const;

    void calcSmoothBezierCP(QPainterPath &painter, char prevCommand,
                            double currentX, double currentY,
                            double &cpx, double &cpy,
                            double x, double y,
                            bool isRel = false) const;

    void calcSmoothQuadBezierCP(QPainterPath &painter, char prevCommand,
                                double currentX, double currentY,
                                double &prevCpx, double &prevCpy,
                                double cpx, double cpy,
                                double x, double y, bool isRel = false) const;

    QPen createPen(const cAbstractLineFigure *figure, FigureRenderingHints *hints) const;
    QPen createPen(const cAbstractShapeFigure *figure, FigureRenderingHints *hints) const;
    QBrush createBrush(const cAbstractShapeFigure *figure) const;

    virtual void setTransform(const cFigure::Transform& transform, QGraphicsItem *item) const;

    virtual void refreshGeometry(cFigure* figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void refreshVisual(cFigure* figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);

    virtual QGraphicsItem *newItem() = 0;
    QGraphicsItem *createGeometry(cFigure *figure, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) = 0;
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) = 0;

public:
    FigureRenderer() {}
    virtual ~FigureRenderer() {}

    static FigureRenderer *getRendererFor(cFigure *figure);
    QGraphicsItem *render(cFigure *figure, GraphicsLayer *layer, FigureRenderingHints *hints);
    virtual void refresh(cFigure *figure, QGraphicsItem *item, int8_t what, FigureRenderingHints *hints);
};

class AbstractShapeFigureRenderer : public FigureRenderer
{
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    QGraphicsItem *newItem() override;
};

class AbstractImageFigureRenderer : public FigureRenderer
{
protected:
    void setImageTransform(cAbstractImageFigure *figure, QGraphicsItem *item, const cFigure::Transform &transform, double naturalWidth, double naturalHeight, bool translateOnly, double zoom);
    virtual void refresh(cFigure *figure, QGraphicsItem *item, int8_t what, FigureRenderingHints *hints);
};

class NullRenderer : public FigureRenderer
{
protected:
    virtual QGraphicsItem *newItem() { return nullptr; }
    virtual void createVisual(cFigure*, QGraphicsItem*, FigureRenderingHints *hints) {}
    virtual void setItemGeometryProperties(cFigure*, QGraphicsItem*, FigureRenderingHints *hints) {}
};

class GroupFigureRenderer : public FigureRenderer
{
protected:
    virtual QGraphicsItem *newItem() { return new GraphicsLayer(); }
    virtual void createVisual(cFigure*, QGraphicsItem*, FigureRenderingHints *) {}
    virtual void setItemGeometryProperties(cFigure*, QGraphicsItem*, FigureRenderingHints *) {}
};

class AbstractLineFigureRenderer : public FigureRenderer
{
protected:
    void setArrowStyle(cAbstractLineFigure *figure, GraphicsPathArrowItem *startItem, GraphicsPathArrowItem *endItem, double zoom = 1.0);
};

class LineFigureRenderer : public AbstractLineFigureRenderer
{
private:
    void setArrows(cLineFigure *lineFigure, QGraphicsLineItem *lineItem, double zoom = 1.0);

protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class ArcFigureRenderer : public AbstractLineFigureRenderer
{
private:
    void setArrows(cArcFigure *arcFigure, QGraphicsPathItem *arcItem, double zoom = 1.0);

protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class PolylineFigureRenderer : public AbstractLineFigureRenderer
{
private:
    void setArrows(cPolylineFigure *polyFigure, PathItem *polyItem, double zoom = 1.0);

protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class RectangleFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
};

class OvalFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
};

class RingFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
};

class PieSliceFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
};

class PolygonFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
};

class PathFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
};

class TextFigureRenderer : public FigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void refresh(cFigure *figure, QGraphicsItem *item, int8_t what, FigureRenderingHints *hints);
    virtual void refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class LabelFigureRenderer : public TextFigureRenderer
{
protected:
    virtual void refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
};

class ImageFigureRenderer : public AbstractImageFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) override;
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) override;
    virtual void refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) override;
    virtual QGraphicsItem *newItem() override;
};

class PixmapFigureRenderer : public ImageFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) override;
    virtual void refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) override;
};

class IconFigureRenderer : public ImageFigureRenderer
{
protected:
    virtual void refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) override;
};

} // namespace qtenv
} // namespace omnetpp

#endif
