#ifndef FIGURERENDERERS_H
#define FIGURERENDERERS_H

#include <string>
#include <map>

#include "omnetpp/ccanvas.h"

class QPen;
class QGraphicsScene;
class QGraphicsItem;
class QGraphicsLineItem;
class QGraphicsPathItem;
class QBrush;
class QPainterPath;
class QPointF;

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

protected:
    static std::map<int, QGraphicsItem*> items;

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

    QPen createPen(const cAbstractLineFigure *figure) const;
    QPen createPen(const cAbstractShapeFigure * figure) const;
    QBrush createBrush(const cAbstractShapeFigure *figure) const;

    virtual void setTransform(const cFigure::Transform &transform, QGraphicsItem *item, const QPointF *offset = 0) const;

    virtual void refreshGeometry(cFigure* figure);
    virtual void refreshVisual(cFigure* figure);
    virtual void refreshTransform(cFigure *figure, const cFigure::Transform &transform);

    virtual QGraphicsItem *newItem() = 0;
    QGraphicsItem *createGeometry(cFigure *figure);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item) = 0;
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item) = 0;

public:
    FigureRenderer() {}
    virtual ~FigureRenderer() {}

    static FigureRenderer *getRendererFor(cFigure *figure);
    static void deleteItems() { items.clear(); }
    void render(cFigure *figure, QGraphicsScene *scene, const cFigure::Transform &transform, FigureRenderingHints *hints);
    void refresh(cFigure *figure, int8_t what, const cFigure::Transform &transform, FigureRenderingHints *hints);
};

class AbstractShapeFigureRenderer : public FigureRenderer
{
    virtual void createVisual(cFigure *figure, QGraphicsItem *item);
};

//TODO remove?
class AbstractTextFigureRenderer : public FigureRenderer
{
protected:
    virtual void refreshTransform(cFigure *figure, const cFigure::Transform &transform);
};

class AbstractImageFigureRenderer : public FigureRenderer
{
    virtual void refreshTransform(cFigure *figure, const cFigure::Transform &transform);
};

class NullRenderer : public FigureRenderer
{
protected:
    virtual void setTransform(const cFigure::Transform&, QGraphicsItem*, const QPointF* = 0) const {}
    virtual QGraphicsItem *newItem() { return nullptr; }
    virtual void createVisual(cFigure*, QGraphicsItem*) {}
    virtual void setItemGeometryProperties(cFigure*, QGraphicsItem*) {}
};

class LineFigureRenderer : public FigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item);
    virtual QGraphicsItem *newItem();
};

class ArcFigureRenderer : public FigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item);
    virtual QGraphicsItem *newItem();
};

class PolylineFigureRenderer : public FigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item);
    virtual QGraphicsItem *newItem();
};

class RectangleFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item);
    virtual QGraphicsItem *newItem();
};

class OvalFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item);
    virtual QGraphicsItem *newItem();
};

class RingFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item);
    virtual QGraphicsItem *newItem();
};

class PieSliceFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item);
    virtual QGraphicsItem *newItem();
};

class PolygonFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item);
    virtual QGraphicsItem *newItem();
};

class PathFigureRenderer : public AbstractShapeFigureRenderer
{
protected:
    virtual void refreshTransform(cFigure *figure, const cFigure::Transform &transform);
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item);
    virtual QGraphicsItem *newItem();
};

class TextFigureRenderer : public AbstractTextFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item);
    virtual QGraphicsItem *newItem();
};

class LabelFigureRenderer : public TextFigureRenderer
{
protected:
    virtual void setTransform(const cFigure::Transform&, QGraphicsItem*, const QPointF* = 0) const {}
    virtual void refreshTransform(cFigure *figure, const cFigure::Transform &transform);
};

class ImageFigureRenderer : public AbstractImageFigureRenderer
{
protected:
    virtual void setItemGeometryProperties(cFigure *figure, QGraphicsItem *item);
    virtual void createVisual(cFigure *figure, QGraphicsItem *item);
    virtual QGraphicsItem *newItem();
};

}

#endif // FIGURERENDERERS_H
