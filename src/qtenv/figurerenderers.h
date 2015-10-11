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
{
        double zoom;
        double iconMagnification;
        std::string defaultFont;
        int defaultFontSize;
        bool showSubmoduleLabels;
        bool showArrowHeads;
        bool showMessageLabels;

        FigureRenderingHints() :
            zoom(1), iconMagnification(1), defaultFont("Arial"), defaultFontSize(12),
            showSubmoduleLabels(true), showArrowHeads(true), showMessageLabels(true) {}
};

class FigureRenderer
{
protected:
    // This class should be used in place of every QGraphicsPathItem
    // (for the polyline, polygon, ring and path figures - these
    // are the ones that can have holes in them) because this
    // this does not include the filling area in its shape() if
    // the figure is not in fact filled. The Qt one does, and it
    // messes up mouse selection (picking, collision detection, etc...)
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

    inline bool doubleEquals(double x, double y) const;
    double calcVectorAngle(double ux, double uy, double vx, double vy) const;
    ArcShape endPointToCentralFromArcParameters(double startX, double startY,
                                            double endX, double endY,
                                            bool largeArcFlag, bool sweepFlag,
                                            double &rx, double &ry, double angle,
                                            double &centerX, double &centerY,
                                            double &startAngle, double &sweepLength) const;

    template<class T>
    static FigureRenderer *searchInCache(std::string className);

protected:

    inline cFigure::Point polarToCertasian(cFigure::Point center, double rx, double ry, double rad) const;
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
    QPen createPen(const cAbstractShapeFigure * figure, FigureRenderingHints *hints) const;
    QBrush createBrush(const cAbstractShapeFigure *figure) const;

    virtual void setTransform(const cFigure::Transform &transform, QGraphicsItem *item, const QPointF *offset = 0) const;

    virtual void refreshGeometry(cFigure* figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void refreshVisual(cFigure* figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void refreshTransform(cFigure *figure, QGraphicsItem *item, const cFigure::Transform &transform);

    virtual QGraphicsItem *newItem() = 0;
    QGraphicsItem *createGeometry(cFigure *figure, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) = 0;
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) = 0;

public:
    FigureRenderer() {}
    virtual ~FigureRenderer() {}

    static FigureRenderer *getRendererFor(cFigure *figure);
    QGraphicsItem *render(cFigure *figure, GraphicsLayer *layer, const cFigure::Transform &transform, FigureRenderingHints *hints);
    virtual void refresh(cFigure *figure, QGraphicsItem *item, int8_t what, const cFigure::Transform &transform, FigureRenderingHints *hints);
};

class AbstractShapeFigureRenderer : public FigureRenderer
{
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
};

//TODO remove?
class AbstractTextFigureRenderer : public FigureRenderer
{
protected:
    virtual void refresh(cFigure *figure, QGraphicsItem *item, int8_t what, const cFigure::Transform &transform, FigureRenderingHints *hints);
    virtual void refreshTransform(cFigure *figure, QGraphicsItem *item, const cFigure::Transform &transform);
};

class AbstractImageFigureRenderer : public FigureRenderer
{
protected:
    virtual void refresh(cFigure *figure, QGraphicsItem *item, int8_t what, const cFigure::Transform &transform, FigureRenderingHints *hints);
    virtual void refreshTransform(cFigure *figure, QGraphicsItem *item, const cFigure::Transform &transform);
};

class NullRenderer : public FigureRenderer
{
protected:
    virtual void setTransform(const cFigure::Transform&, QGraphicsItem*, const QPointF* = 0) const {}
    virtual QGraphicsItem *newItem() { return nullptr; }
    virtual void createVisual(cFigure*, QGraphicsItem*, FigureRenderingHints *hints) {}
    virtual void setItemGeometryProperties(cFigure*, QGraphicsItem*, FigureRenderingHints *hints) {}
};

class AbstractLineFigureRenderer : public FigureRenderer
{
protected:
    void setArrowStyle(cFigure::ArrowHead style, GraphicsPathArrowItem *arrowItem);
};

class LineFigureRenderer : public AbstractLineFigureRenderer
{
private:
    void setArrows(cLineFigure *lineFigure, QGraphicsLineItem *lineItem, QPen *pen = nullptr);

protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class ArcFigureRenderer : public AbstractLineFigureRenderer
{
private:
    void setArrows(cArcFigure *arcFigure, QGraphicsPathItem *arcItem, QPen *pen = nullptr);

protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class PolylineFigureRenderer : public AbstractLineFigureRenderer
{
private:
    void setArrows(cPolylineFigure *polyFigure, PathItem *polyItem, QPen *pen = nullptr);

protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class RectangleFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class OvalFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class RingFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class PieSliceFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class PolygonFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class PathFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void refreshTransform(cFigure *figure, QGraphicsItem *item, const cFigure::Transform &transform);
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class TextFigureRenderer : public AbstractTextFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class LabelFigureRenderer : public TextFigureRenderer
{
protected:
    virtual void setTransform(const cFigure::Transform&, QGraphicsItem *item, const QPointF* = 0) const {}
    virtual void refreshTransform(cFigure *figure, QGraphicsItem *item, const cFigure::Transform &transform);
};

class ImageFigureRenderer : public AbstractImageFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
    virtual QGraphicsItem *newItem();
};

class PixmapFigureRenderer : public ImageFigureRenderer
{
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints);
};

} // namespace qtenv
} // namespace omnetpp

#endif
