//==========================================================================
//  FIGURERENDERERS.CC - part of
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

#include "figurerenderers.h"

#include "graphicspatharrowitem.h"
#include "qtenv.h"
#include <sstream>
#include <cfloat>
#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsItem>
#include <QFontMetrics>
#include <QGraphicsColorizeEffect>
#include <QPainter>

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

namespace omnetpp {
namespace qtenv {

QPainterPath FigureRenderer::PathItem::shape() const {
    QPainterPath result;

    const QBrush &b = brush();
    const QPen &p = pen();

    if (b != Qt::NoBrush && b.color().alpha() > 0)
        result += path(); // if we are filled visibly, including the insides

    if (p != Qt::NoPen && p.color().alpha() > 0) {
        // and if the outline is drawn, including that too.
        QPainterPathStroker s;
        // setting up the stroker properly
        s.setDashPattern(p.dashPattern());
        s.setDashOffset(p.dashOffset());
        s.setMiterLimit(p.miterLimit());
        s.setJoinStyle(p.joinStyle());
        s.setWidth(p.widthF()); // note the F at the end...
        s.setCapStyle(p.capStyle());
        // and adding the outline to the path
        result += s.createStroke(path());
    }

    return result;
}

/* // only for debugging
void FigureRenderer::PathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QGraphicsPathItem::paint(painter, option, widget);
    painter->setBrush(QColor(255, 0, 0, 100));
    painter->setPen(QColor(0, 255, 0, 100));
    painter->drawPath(shape());
}
*/

std::map<std::string, FigureRenderer *> FigureRenderer::rendererCache;

template<class T>
FigureRenderer *FigureRenderer::searchInCache(std::string className)
{
    FigureRenderer *renderer;
    auto it = rendererCache.find(className);
    if (it != rendererCache.end())
        renderer = it->second;
    else {
        renderer = new T;
        rendererCache[className] = renderer;
    }

    return renderer;
}

FigureRenderer *FigureRenderer::getRendererFor(cFigure *figure)
{
    FigureRenderer *renderer = nullptr;
    std::string className;

    if (dynamic_cast<cArcFigure *>(figure))
        renderer = searchInCache<ArcFigureRenderer>("ArcFigure");
    else if (dynamic_cast<cLineFigure *>(figure))
        renderer = searchInCache<LineFigureRenderer>("LineFigure");
    else if (dynamic_cast<cPolylineFigure *>(figure))
        renderer = searchInCache<PolylineFigureRenderer>("PolylineFigure");
    else if (dynamic_cast<cRectangleFigure *>(figure))
        renderer = searchInCache<RectangleFigureRenderer>("RectangleFigure");
    else if (dynamic_cast<cOvalFigure *>(figure))
        renderer = searchInCache<OvalFigureRenderer>("OvalFigure");
    else if (dynamic_cast<cRingFigure *>(figure))
        renderer = searchInCache<RingFigureRenderer>("RingFigure");
    else if (dynamic_cast<cPieSliceFigure *>(figure))
        renderer = searchInCache<PieSliceFigureRenderer>("PieSliceFigure");
    else if (dynamic_cast<cPolygonFigure *>(figure))
        renderer = searchInCache<PolygonFigureRenderer>("PolygonFigure");
    else if (dynamic_cast<cPathFigure *>(figure))
        renderer = searchInCache<PathFigureRenderer>("PathFigure");
    else if (dynamic_cast<cTextFigure *>(figure))
        renderer = searchInCache<TextFigureRenderer>("TextFigure");
    else if (dynamic_cast<cLabelFigure *>(figure))
        renderer = searchInCache<LabelFigureRenderer>("LabelFigure");
    else if (dynamic_cast<cImageFigure *>(figure))
        renderer = searchInCache<ImageFigureRenderer>("ImageFigure");
    else if (dynamic_cast<cPixmapFigure *>(figure))
        renderer = searchInCache<PixmapFigureRenderer>("PixmapFigure");
    else
        renderer = searchInCache<NullRenderer>("Null");

    return renderer;
}

cFigure::Point FigureRenderer::polarToCertasian(cFigure::Point center, double rx, double ry, double rad) const
{
    return cFigure::Point(
            center.x + rx * cos(rad),
            center.y - ry * sin(rad)
            );
}

// from mozilla
double FigureRenderer::calcVectorAngle(double ux, double uy, double vx, double vy) const
{
    double ta = atan2(uy, ux);
    double tb = atan2(vy, vx);

    if (tb >= ta)
        return tb-ta;
    else
        return 2.0*M_PI - (ta-tb);
}

bool FigureRenderer::doubleEquals(double x, double y) const
{
    return fabs(x - y) < DBL_EPSILON;
}

// http://www.w3.org/TR/SVG/implnote.html#ArcConversionEndpointToCenter
FigureRenderer::ArcShape FigureRenderer::endPointToCentralFromArcParameters(double startX, double startY, double endX, double endY, bool largeArcFlag, bool sweepFlag,
        double& rx, double& ry, double angle, double& centerX, double& centerY, double& startAngle, double& sweepLength) const
{
    /* 1. Treat out-of-range parameters as described in
     * http://www.w3.org/TR/SVG/implnote.html#ArcImplementationNotes
     *
     * If the endpoints (x1, y1) and (x2, y2) are identical, then this
     * is equivalent to omitting the elliptical arc segment entirely
     */
    if (doubleEquals(startX, endX) && doubleEquals(startY, endY))
        return ARC_NOTHING;

    /* If rx = 0 or ry = 0 then this arc is treated as a straight line
     * segment (a "lineto") joining the endpoints.
     */
    if (rx == 0 || ry == 0)
        return ARC_LINE;

    /* If rx or ry have negative signs, these are dropped; the absolute
     * value is used instead.
     */
    if (rx < 0.0)
        rx = -rx;
    if (ry < 0.0)
        ry = -ry;

    // Step 1
    double dx = (startX-endX)/2;
    double dy = (startY-endY)/2;
    double x1Dash = cos(angle)*dx + sin(angle)*dy;
    double y1Dash = -sin(angle)*dx + cos(angle)*dy;

    // Step 2
    int sign = largeArcFlag == sweepFlag ? -1 : 1;
    double numerator = rx*rx*ry*ry - rx*rx*y1Dash*y1Dash - ry*ry*x1Dash*x1Dash;
    double rootFactor;

    /* If rx , ry and are such that there is no solution (basically,
     * the ellipse is not big enough to reach from (x1, y1) to (x2,
     * y2)) then the ellipse is scaled up uniformly until there is
     * exactly one solution (until the ellipse is just big enough).
     *  -> find factor s, such that numerator' with rx'=s*rx and
     *    ry'=s*ry becomes 0 :
     */
    if (numerator < 0) {
        double s = sqrt(1.0 - numerator/(rx*rx*ry*ry));
        rx *= s;
        ry *= s;
        rootFactor = 0;
    }
    else
        rootFactor = sqrt(numerator/(rx*rx*y1Dash*y1Dash + ry*ry*x1Dash*x1Dash));

    double cxDash = sign*rootFactor*rx*y1Dash/ry;
    double cyDash = -sign*rootFactor*ry*x1Dash/rx;

    // Step 3
    centerX = cos(angle)*cxDash - sin(angle)*cyDash + (startX+endX)/2;
    centerY = sin(angle)*cxDash + cos(angle)*cyDash + (startY+endY)/2;

    // Step 4?
    startAngle = calcVectorAngle(1, 0, (x1Dash-cxDash)/rx, (y1Dash-cyDash)/ry);
    sweepLength = calcVectorAngle((x1Dash-cxDash)/rx, (y1Dash-cyDash)/ry, (-x1Dash-cxDash)/rx, (-y1Dash-cyDash)/ry);

    if (sweepFlag && sweepLength < 0)
        sweepLength += 2*M_PI;
    else if (!sweepFlag && sweepLength > 0)
        sweepLength -= 2*M_PI;

    return ARC_ARC;
}

void FigureRenderer::arcToUsingBezier(QPainterPath& painter, double currentX, double currentY, double x, double y, double rx, double ry, double angle,
        bool largeArcFlag, bool sweepFlag, bool isRel) const
{
    double centerX, centerY;
    double startAngle, sweepLength;
    double phi = angle * M_PI/180;

    double endX, endY;
    if (isRel) {
        endX = x + currentX;
        endY = y + currentY;
    }
    else {
        endX = x;
        endY = y;
    }
    ArcShape shape = endPointToCentralFromArcParameters(currentX, currentY, endX, endY, largeArcFlag, sweepFlag, rx, ry, phi, centerX, centerY, startAngle, sweepLength);

    switch (shape) {
        case ARC_LINE: {
            painter.moveTo(endX, endY);
            break;
        }

        case ARC_ARC: {
            int segments = (int)ceil(fabs(sweepLength/(M_PI/2.0)));
            double delta = sweepLength/segments;
            double t = 8.0/3.0 * sin(delta/4.0) * sin(delta/4.0) / sin(delta/2.0);

            double x1 = currentX;
            double y1 = currentY;
            for (int j = 0; j < segments; ++j) {
                double theta2 = startAngle + delta;
                double cosTheta2 = cos(theta2);
                double sinTheta2 = sin(theta2);

                // a) calculate endpoint of the segment:
                double xe = cos(phi) * rx*cosTheta2 - sin(phi) * ry*sinTheta2 + centerX;
                double ye = sin(phi) * rx*cosTheta2 + cos(phi) * ry*sinTheta2 + centerY;

                // b) calculate gradients at start/end points of segment:
                double dx1 = t * (-cos(phi) * rx*sin(startAngle) - sin(phi) * ry*cos(startAngle));
                double dy1 = t * (-sin(phi) * rx*sin(startAngle) + cos(phi) * ry*cos(startAngle));
                double dxe = t * (cos(phi) * rx*sinTheta2 + sin(phi) * ry*cosTheta2);
                double dye = t * (sin(phi) * rx*sinTheta2 - cos(phi) * ry*cosTheta2);

                // c) draw the cubic bezier:
                painter.cubicTo(x1+dx1, y1+dy1, xe+dxe, ye+dye, xe, ye);

                // do next segment
                startAngle = theta2;
                x1 = xe;
                y1 = ye;
            }
            break;
        }

        case ARC_NOTHING:
            break;
    }
}

// for Svg's T statement
void FigureRenderer::calcSmoothBezierCP(QPainterPath& painter, char prevCommand, double currentX, double currentY,
        double& cpx, double& cpy, double x, double y, bool isRel) const
{
    if (tolower(prevCommand) == 'q' || tolower(prevCommand) == 't') {
        if (cpx < currentX)
            cpx = currentX + fabs(currentX - cpx);
        else
            cpx = currentX - fabs(currentX - cpx);

        if (cpy < currentY)
            cpy = currentY + fabs(currentY - cpy);
        else
            cpy = currentY - fabs(currentY - cpy);
    }
    else {
        cpx = currentX;
        cpy = currentY;
    }

    if (isRel)
        painter.quadTo(cpx, cpy, x + currentX, y + currentY);
    else
        painter.quadTo(cpx, cpy, x, y);
}

// for Svg's S statement
void FigureRenderer::calcSmoothQuadBezierCP(QPainterPath& painter, char prevCommand, double currentX, double currentY, double& prevCpx, double& prevCpy, double cpx, double cpy,
        double x, double y, bool isRel) const
{
    if (tolower(prevCommand) == 's' || tolower(prevCommand) == 'c') {
        if (prevCpx < currentX)
            prevCpx = currentX + fabs(currentX - prevCpx);
        else
            prevCpx = currentX - fabs(currentX - prevCpx);

        if (prevCpy < currentY)
            prevCpy = currentY + fabs(currentY - prevCpy);
        else
            prevCpy = currentY - fabs(currentY - prevCpy);
    }
    else {
        prevCpx = currentX;
        prevCpy = currentY;
    }
    if (isRel)
        painter.cubicTo(prevCpx, prevCpy, cpx, cpy, x + currentX, y + currentY);
    else
        painter.cubicTo(prevCpx, prevCpy, cpx, cpy, x, y);
}

QPen FigureRenderer::createPen(const cAbstractLineFigure *figure, FigureRenderingHints *hints) const
{
    cFigure::Color color = figure->getLineColor();
    QPen pen;
    pen.setColor(QColor(color.red, color.green, color.blue, figure->getLineOpacity()*255));

    double width = figure->getLineWidth();
    if (!figure->getZoomLineWidth())
        width /= hints->zoom;
    pen.setWidthF(width);

    Qt::PenStyle style;
    switch (figure->getLineStyle()) {
        case cFigure::LINE_SOLID:
            style = Qt::SolidLine;
            break;

        case cFigure::LINE_DOTTED:
            style = Qt::DotLine;
            break;

        case cFigure::LINE_DASHED:
            style = Qt::DashLine;
            break;
    }
    pen.setStyle(style);

    Qt::PenCapStyle cap;
    switch (figure->getCapStyle()) {
        case cFigure::CAP_BUTT:
            cap = Qt::FlatCap;
            break;

        case cFigure::CAP_SQUARE:
            cap = Qt::SquareCap;
            break;

        case cFigure::CAP_ROUND:
            cap = Qt::RoundCap;
            break;
    }
    pen.setCapStyle(cap);

    return pen;
}

QPen FigureRenderer::createPen(const cAbstractShapeFigure *figure, FigureRenderingHints *hints) const
{
    QPen pen;
    if (figure->isOutlined()) {
        cFigure::Color color = figure->getLineColor();
        pen.setColor(QColor(color.red, color.green, color.blue, figure->getLineOpacity()*255));
        double width = figure->getLineWidth();
        if (!figure->getZoomLineWidth())
            width /= hints->zoom;
        pen.setWidthF(width);

        Qt::PenStyle style;
        switch (figure->getLineStyle()) {
            case cFigure::LINE_SOLID:
                style = Qt::SolidLine;
                break;

            case cFigure::LINE_DOTTED:
                style = Qt::DotLine;
                break;

            case cFigure::LINE_DASHED:
                style = Qt::DashLine;
                break;
        }
        pen.setStyle(style);
    }
    else
        pen.setStyle(Qt::NoPen);

    return pen;
}

QBrush FigureRenderer::createBrush(const cAbstractShapeFigure *figure) const
{
    if (figure->isFilled()) {
        cFigure::Color color = figure->getFillColor();
        return QBrush(QColor(color.red, color.green, color.blue, figure->getFillOpacity()*255));  // setColor not working
    }
    else
        return QBrush(Qt::NoBrush);
}

// TODO setScaleLineWidth when is in omnet++
void FigureRenderer::setTransform(const cFigure::Transform& transform, QGraphicsItem *item, const QPointF *offset) const
{
    QTransform qTrans(transform.a, transform.b, 0, transform.c, transform.d, 0, transform.t1, transform.t2, 1);
    if (offset)
        qTrans.translate(offset->x(), offset->y());
    item->setTransform(qTrans);
}

QGraphicsItem *FigureRenderer::render(cFigure *figure, GraphicsLayer *layer, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    QGraphicsItem *item = createGeometry(figure, hints);
    createVisual(figure, item, hints);

    refreshTransform(figure, item, transform);

    if (item)
        item->setParentItem(layer);

    return item;
}

void FigureRenderer::refresh(cFigure *figure, QGraphicsItem *item, int8_t what, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    if (what & cFigure::CHANGE_VISUAL)
        refreshVisual(figure, item, hints);
    if (what & (cFigure::CHANGE_GEOMETRY | cFigure::CHANGE_INPUTDATA))
        refreshGeometry(figure, item, hints);
    if (what & cFigure::CHANGE_TRANSFORM)
        refreshTransform(figure, item, transform);
}

void FigureRenderer::refreshTransform(cFigure *figure, QGraphicsItem *item, const cFigure::Transform& transform)
{
    setTransform(transform, item);
}

QGraphicsItem *FigureRenderer::createGeometry(cFigure *figure, FigureRenderingHints *hints)
{
    QGraphicsItem *item = newItem();
    setItemGeometryProperties(figure, item, hints);
    item->setData(1, QVariant::fromValue((cObject*)figure));
    return item;
}

void FigureRenderer::refreshGeometry(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) {
    setItemGeometryProperties(figure, item, hints);
}

void FigureRenderer::refreshVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints) {
    createVisual(figure, item, hints);
}

void AbstractShapeFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cAbstractShapeFigure *shapeFigure = static_cast<cAbstractShapeFigure *>(figure);
    QAbstractGraphicsShapeItem *shapeItem = static_cast<QAbstractGraphicsShapeItem *>(item);
    shapeItem->setPen(createPen(shapeFigure, hints));
    shapeItem->setBrush(createBrush(shapeFigure));
}

void AbstractTextFigureRenderer::refresh(cFigure *figure, QGraphicsItem *item, int8_t what, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    if (what & cFigure::CHANGE_VISUAL)
        refreshVisual(figure, item, hints);
    if (what & (cFigure::CHANGE_GEOMETRY | cFigure::CHANGE_INPUTDATA | cFigure::CHANGE_TRANSFORM)) {
        refreshGeometry(figure, item, hints);
        refreshTransform(figure, item, transform);
    }
}

void AbstractTextFigureRenderer::refreshTransform(cFigure *figure, QGraphicsItem *item, const cFigure::Transform& transform)
{
    QGraphicsSimpleTextItem *textItem = static_cast<QGraphicsSimpleTextItem *>(item);
    cAbstractTextFigure *textFigure = static_cast<cAbstractTextFigure *>(figure);

    cFigure::Point pos = textFigure->getPosition();

    QFontMetrics fontMetric(textItem->font());
    QRectF bounds = item->boundingRect();
    QPointF anchor;
    switch (textFigure->getAnchor()) {
        case cFigure::ANCHOR_CENTER:
            anchor = QPointF(pos.x - bounds.width()/2, pos.y - bounds.height()/2);
            break;

        case cFigure::ANCHOR_N:
            anchor = QPointF(pos.x - bounds.width()/2, pos.y);
            break;

        case cFigure::ANCHOR_E:
            anchor = QPointF(pos.x - bounds.width(), pos.y - bounds.height()/2);
            break;

        case cFigure::ANCHOR_S:
            anchor = QPointF(pos.x - bounds.width()/2, pos.y - bounds.height());
            break;

        case cFigure::ANCHOR_W:
            anchor = QPointF(pos.x, pos.y - bounds.height()/2);
            break;

        case cFigure::ANCHOR_NE:
            anchor = QPointF(pos.x - bounds.width(), pos.y);
            break;

        case cFigure::ANCHOR_SE:
            anchor = QPointF(pos.x - bounds.width(), pos.y - bounds.height());
            break;

        case cFigure::ANCHOR_SW:
            anchor = QPointF(pos.x, pos.y - bounds.height());
            break;

        case cFigure::ANCHOR_NW:
            anchor = QPointF(pos.x, pos.y);
            break;

        case cFigure::ANCHOR_BASELINE_START:
            anchor = QPointF(pos.x, pos.y - fontMetric.ascent());
            break;

        case cFigure::ANCHOR_BASELINE_MIDDLE:
            anchor = QPointF(pos.x - bounds.width()/2, pos.y - fontMetric.ascent());
            break;

        case cFigure::ANCHOR_BASELINE_END:
            anchor = QPointF(pos.x - bounds.width(), pos.y - fontMetric.ascent());
            break;
    }

    setTransform(transform, item, &anchor);
}

void AbstractImageFigureRenderer::refresh(cFigure *figure, QGraphicsItem *item, int8_t what, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    if (what & cFigure::CHANGE_VISUAL)
        refreshVisual(figure, item, hints);
    if (what & (cFigure::CHANGE_GEOMETRY | cFigure::CHANGE_INPUTDATA | cFigure::CHANGE_TRANSFORM)) {
        refreshGeometry(figure, item, hints);
        refreshTransform(figure, item, transform);
    }
}

void AbstractImageFigureRenderer::refreshTransform(cFigure *figure, QGraphicsItem *item, const cFigure::Transform& transform)
{
    cAbstractImageFigure *imageFigure = static_cast<cAbstractImageFigure *>(figure);

    QRectF bounds = item->boundingRect();
    cFigure::Point pos = imageFigure->getPosition();
    QPointF anchor;

    switch (imageFigure->getAnchor()) {
        case cFigure::ANCHOR_CENTER:
            anchor = QPointF(pos.x - bounds.width()/2, pos.y - bounds.height()/2);
            break;

        case cFigure::ANCHOR_N:
            anchor = QPointF(pos.x - bounds.width()/2, pos.y);
            break;

        case cFigure::ANCHOR_E:
            anchor = QPointF(pos.x - bounds.width(), pos.y - bounds.height()/2);
            break;

        case cFigure::ANCHOR_S:
            anchor = QPointF(pos.x - bounds.width()/2, pos.y - bounds.height());
            break;

        case cFigure::ANCHOR_W:
            anchor = QPointF(pos.x, pos.y - bounds.height()/2);
            break;

        case cFigure::ANCHOR_NE:
            anchor = QPointF(pos.x - bounds.width(), pos.y);
            break;

        case cFigure::ANCHOR_SE:
            anchor = QPointF(pos.x - bounds.width(), pos.y - bounds.height());
            break;

        case cFigure::ANCHOR_SW:
            anchor = QPointF(pos.x, pos.y - bounds.height());
            break;

        case cFigure::ANCHOR_NW:
            anchor = QPointF(pos.x, pos.y);
            break;

        case cFigure::ANCHOR_BASELINE_START:
        case cFigure::ANCHOR_BASELINE_MIDDLE:
        case cFigure::ANCHOR_BASELINE_END:
            break;
    }

    setTransform(transform, item, &anchor);
}

// TODO: Start/End Arrow
void LineFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cLineFigure *lineFigure = static_cast<cLineFigure *>(figure);
    QGraphicsLineItem *lineItem = static_cast<QGraphicsLineItem *>(item);
    QPointF start(lineFigure->getStart().x, lineFigure->getStart().y);
    QPointF end(lineFigure->getEnd().x, lineFigure->getEnd().y);
    lineItem->setLine(QLineF(start, end));
}

void LineFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cLineFigure *lineFigure = static_cast<cLineFigure *>(figure);
    QGraphicsLineItem *lineItem = static_cast<QGraphicsLineItem *>(item);
    lineItem->setPen(createPen(lineFigure, hints));
}

QGraphicsItem *LineFigureRenderer::newItem()
{
    return new QGraphicsLineItem();
}

void ArcFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cArcFigure *arcFigure = static_cast<cArcFigure *>(figure);
    QGraphicsPathItem *arcItem = static_cast<QGraphicsPathItem *>(item);

    cFigure::Rectangle bounds = arcFigure->getBounds();
    cFigure::Point startPoint = polarToCertasian(bounds.getCenter(), bounds.width/2, bounds.height/2, arcFigure->getStartAngle());
    QPainterPath painter(QPointF(startPoint.x, startPoint.y));
    painter.arcTo(bounds.x, bounds.y, bounds.width, bounds.height, arcFigure->getStartAngle()*180/M_PI, qAbs(arcFigure->getStartAngle()-arcFigure->getEndAngle())*180/M_PI);
    arcItem->setPath(painter);
}

void ArcFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    QGraphicsPathItem *arcItem = static_cast<QGraphicsPathItem *>(item);
    arcItem->setPen(createPen(static_cast<cAbstractLineFigure *>(figure), hints));
}

QGraphicsItem *ArcFigureRenderer::newItem()
{
    return new QGraphicsPathItem();
}

void PolylineFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPolylineFigure *polyFigure = static_cast<cPolylineFigure *>(figure);
    QGraphicsPathItem *polyItem = static_cast<QGraphicsPathItem *>(item);

    std::vector<cFigure::Point> points = polyFigure->getPoints();
    if (points.size() < 2)
        return;  // throw cRuntimeError("PolylineFigureRenderer: points.size() < 2");

    QPainterPath painter;

    painter.moveTo(points[0].x, points[0].y);

    if (points.size() == 2)
        painter.lineTo(points[1].x, points[1].y);
    else if (polyFigure->getSmooth()) {
        for (size_t i = 2; i < points.size(); i++) {
            const cFigure::Point& control = points[i-1];
            bool isLast = (i == points.size()-1);
            cFigure::Point to = isLast ? points[i] : (points[i-1] + points[i]) * 0.5;
            painter.quadTo(control.x, control.y, to.x, to.y);
        }
    }
    else
        for (size_t i = 1; i < points.size(); i++)
            painter.lineTo(points[i].x, points[i].y);


    polyItem->setPath(painter);
}

void PolylineFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPolylineFigure *polyFigure = static_cast<cPolylineFigure *>(figure);
    QGraphicsPathItem *polyItem = static_cast<QGraphicsPathItem *>(item);

    if (!(figure->getParentFigure()->getLocalChangeFlags() & cFigure::CHANGE_STRUCTURAL || figure->getLocalChangeFlags() & cFigure::CHANGE_GEOMETRY))
        setItemGeometryProperties(polyFigure, polyItem, hints);

    QPen pen = createPen(polyFigure, hints);
    Qt::PenJoinStyle joinStyle;

    switch (polyFigure->getJoinStyle()) {
        case cFigure::JOIN_BEVEL:
            joinStyle = Qt::BevelJoin;
            break;

        case cFigure::JOIN_MITER:
            joinStyle = Qt::MiterJoin;
            break;

        case cFigure::JOIN_ROUND:
            joinStyle = Qt::RoundJoin;
            break;
    }
    pen.setJoinStyle(joinStyle);

    polyItem->setPen(pen);
}

QGraphicsItem *PolylineFigureRenderer::newItem()
{
    return new PathItem();
}

void RectangleFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cRectangleFigure *rectFigure = static_cast<cRectangleFigure *>(figure);
    QGraphicsPathItem *rectItem = static_cast<QGraphicsPathItem *>(item);
    cFigure::Rectangle bounds = rectFigure->getBounds();
    QPainterPath painter;

    painter.addRoundedRect(bounds.x, bounds.y, bounds.width, bounds.height, rectFigure->getCornerRx(), rectFigure->getCornerRy());
    rectItem->setPath(painter);
}

QGraphicsItem *RectangleFigureRenderer::newItem()
{
    return new QGraphicsPathItem();
}

void OvalFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cOvalFigure *ovalFigure = static_cast<cOvalFigure *>(figure);
    QGraphicsEllipseItem *ellipseItem = static_cast<QGraphicsEllipseItem *>(item);

    cFigure::Rectangle bounds = ovalFigure->getBounds();
    ellipseItem->setRect(bounds.x, bounds.y, bounds.width, bounds.height);
}

QGraphicsItem *OvalFigureRenderer::newItem()
{
    return new QGraphicsEllipseItem();
}

void RingFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cRingFigure *ringFigure = static_cast<cRingFigure *>(figure);
    QGraphicsPathItem *ringItem = static_cast<QGraphicsPathItem *>(item);
    QPainterPath painter;
    cFigure::Rectangle bounds = ringFigure->getBounds();
    cFigure::Point center = bounds.getCenter();

    painter.addEllipse(bounds.x, bounds.y, bounds.width, bounds.height);

    double innerRx = ringFigure->getInnerRx();
    double innerRy = ringFigure->getInnerRy();
    painter.addEllipse(center.x-innerRx, center.y-innerRy, 2*innerRx, 2*innerRy);

    ringItem->setPath(painter);
}

QGraphicsItem *RingFigureRenderer::newItem()
{
    return new PathItem();
}

void PieSliceFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPieSliceFigure *pieSliceFigure = static_cast<cPieSliceFigure *>(figure);
    QGraphicsEllipseItem *pieSliceItem = static_cast<QGraphicsEllipseItem *>(item);
    cFigure::Rectangle bounds = pieSliceFigure->getBounds();
    pieSliceItem->setRect(bounds.x, bounds.y, bounds.width, bounds.height);

    pieSliceItem->setStartAngle(16*pieSliceFigure->getStartAngle()*180/M_PI);
    double endAngle = std::abs(std::abs(pieSliceFigure->getEndAngle())-std::abs(pieSliceFigure->getStartAngle())) * 180/M_PI;
    pieSliceItem->setSpanAngle(16*endAngle);
}

QGraphicsItem *PieSliceFigureRenderer::newItem()
{
    return new QGraphicsEllipseItem();
}

void PolygonFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPolygonFigure *polyFigure = static_cast<cPolygonFigure *>(figure);
    QGraphicsPathItem *polyItem = static_cast<QGraphicsPathItem *>(item);

    QPainterPath painter;

    std::vector<cFigure::Point> points = polyFigure->getPoints();
    if (points.size() < 2) {
    }  // throw cRuntimeError("PolylineFigureRenderer: points.size() < 2");

    if (points.size() == 2) {
        painter.moveTo(points[0].x, points[0].y);
        painter.lineTo(points[1].x, points[1].y);
    }
    else if (polyFigure->getSmooth()) {
        cFigure::Point start = (points[0] + points[1]) * 0.5;
        painter.moveTo(start.x, start.y);

        for (size_t i = 0; i < points.size(); i++) {
            int i1 = (i + 1) % points.size();
            int i2 = (i + 2) % points.size();

            cFigure::Point control = points[i1];
            cFigure::Point to = (points[i1] + points[i2]) * 0.5;

            painter.quadTo(control.x, control.y, to.x, to.y);
        }
    }
    else {
        painter.moveTo(points[0].x, points[0].y);

        for (size_t i = 0; i < points.size(); i++)
            painter.lineTo(points[i].x, points[i].y);
    }

    painter.closeSubpath();

    polyItem->setPath(painter);
}

void PolygonFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPolygonFigure *polyFigure = static_cast<cPolygonFigure *>(figure);
    QGraphicsPathItem *polyItem = static_cast<QGraphicsPathItem *>(item);

    if (!(figure->getParentFigure()->getLocalChangeFlags() & cFigure::CHANGE_STRUCTURAL || figure->getLocalChangeFlags() & cFigure::CHANGE_GEOMETRY))
        setItemGeometryProperties(polyFigure, polyItem, hints);

    QPainterPath painter = polyItem->path();

    if (polyFigure->getFillRule() == cFigure::FILL_EVENODD)
        painter.setFillRule(Qt::OddEvenFill);
    else
        painter.setFillRule(Qt::WindingFill);

    polyItem->setPath(painter);

    QPen pen = createPen(polyFigure, hints);
    Qt::PenJoinStyle joinStyle;

    switch (polyFigure->getJoinStyle()) {
        case cFigure::JOIN_BEVEL:
            joinStyle = Qt::BevelJoin;
            break;

        case cFigure::JOIN_MITER:
            joinStyle = Qt::MiterJoin;
            break;

        case cFigure::JOIN_ROUND:
            joinStyle = Qt::RoundJoin;
            break;
    }

    pen.setJoinStyle(joinStyle);
    polyItem->setPen(pen);
    polyItem->setBrush(createBrush(polyFigure));
}

QGraphicsItem *PolygonFigureRenderer::newItem()
{
    return new PathItem();
}

void PathFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPathFigure *pathFigure = static_cast<cPathFigure *>(figure);
    QGraphicsPathItem *pathItem = static_cast<QGraphicsPathItem *>(item);
    QPainterPath painter;

    char prevCommand;
    cFigure::Point controlPoint;

    for (int i = 0; i < pathFigure->getNumPathItems(); ++i) {
        const cPathFigure::PathItem *command = pathFigure->getPathItem(i);
        QPointF pos = painter.currentPosition();
        switch (command->code) {
            case 'M': {
                const cPathFigure::MoveTo *moveTo = static_cast<const cPathFigure::MoveTo*>(command);
                painter.moveTo(moveTo->x, moveTo->y);
                break;
            }

            case 'm': {
                const cPathFigure::MoveRel *moveRel = static_cast<const cPathFigure::MoveRel*>(command);
                painter.moveTo(pos.x() + moveRel->dx, pos.y() + moveRel->dy);
                break;
            }

            case 'L': {
                const cPathFigure::LineTo *lineTo = static_cast<const cPathFigure::LineTo*>(command);
                painter.lineTo(lineTo->x, lineTo->y);
                break;
            }

            case 'l': {
                const cPathFigure::LineRel *lineRel = static_cast<const cPathFigure::LineRel*>(command);
                painter.lineTo(pos.x() + lineRel->dx, pos.y() + lineRel->dy);
                break;
            }

            case 'H': {
                const cPathFigure::HorizLineTo *horizLineTo = static_cast<const cPathFigure::HorizLineTo*>(command);
                painter.lineTo(horizLineTo->x, pos.y());
                break;
            }

            case 'h': {
                const cPathFigure::HorizLineRel *horizLineRel = static_cast<const cPathFigure::HorizLineRel*>(command);
                painter.lineTo(pos.x() + horizLineRel->dx, pos.y());
                break;
            }

            case 'V': {
                const cPathFigure::VertLineTo *vertLineTo = static_cast<const cPathFigure::VertLineTo*>(command);
                painter.lineTo(pos.x(), vertLineTo->y);
                break;
            }

            case 'v': {
                const cPathFigure::VertLineRel *vertLineRel = static_cast<const cPathFigure::VertLineRel*>(command);
                painter.lineTo(pos.x(), pos.y() + vertLineRel->dy);
                break;
            }

            case 'A': {
                const cPathFigure::ArcTo *arcTo = static_cast<const cPathFigure::ArcTo*>(command);
                arcToUsingBezier(painter, pos.x(), pos.y(), arcTo->x, arcTo->y, arcTo->rx, arcTo->ry,
                        arcTo->phi, arcTo->largeArc, arcTo->sweep, false);
                break;
            }

            case 'a': {
                const cPathFigure::ArcRel *arcRel = static_cast<const cPathFigure::ArcRel*>(command);
                arcToUsingBezier(painter, pos.x(), pos.y(), arcRel->dx, arcRel->dy, arcRel->rx, arcRel->ry,
                        arcRel->phi, arcRel->largeArc, arcRel->sweep, true);
                break;
            }

            case 'Q': {
                const cPathFigure::CurveTo *curveTo = static_cast<const cPathFigure::CurveTo*>(command);
                controlPoint = cFigure::Point(curveTo->x1, curveTo->y1);
                painter.quadTo(controlPoint.x, controlPoint.y, curveTo->x, curveTo->y);
                break;
            }

            case 'q': {
                const cPathFigure::CurveRel *curveRel = static_cast<const cPathFigure::CurveRel*>(command);
                QPointF pos = painter.currentPosition();
                controlPoint = cFigure::Point(pos.x() + curveRel->dx1, pos.y() + curveRel->dy1);
                painter.quadTo(controlPoint.x, controlPoint.y, pos.x() + curveRel->dx, pos.y() + curveRel->dy);
                break;
            }

            case 'T': {
                const cPathFigure::SmoothCurveTo *smoothCurveTo = static_cast<const cPathFigure::SmoothCurveTo*>(command);
                calcSmoothBezierCP(painter, prevCommand, pos.x(), pos.y(), controlPoint.x, controlPoint.y,
                                   smoothCurveTo->x, smoothCurveTo->y);
                break;
            }

            case 't': {
                const cPathFigure::SmoothCurveRel *smoothCurveRel = static_cast<const cPathFigure::SmoothCurveRel*>(command);
                calcSmoothBezierCP(painter, prevCommand, pos.x(), pos.y(), controlPoint.x, controlPoint.y,
                                   smoothCurveRel->dx, smoothCurveRel->dy, true);
                break;
            }

            case 'C': {
                const cPathFigure::CubicBezierCurveTo *bezierTo = static_cast<const cPathFigure::CubicBezierCurveTo*>(command);
                controlPoint = cFigure::Point(bezierTo->x2, bezierTo->y2);
                painter.cubicTo(bezierTo->x1, bezierTo->y1, bezierTo->x2, bezierTo->y2, bezierTo->x,
                        bezierTo->y);
                break;
            }

            case 'c': {
                const cPathFigure::CubicBezierCurveRel *bezierRel = static_cast<const cPathFigure::CubicBezierCurveRel*>(command);
                controlPoint = cFigure::Point(bezierRel->dx2, bezierRel->dy2);
                painter.cubicTo(bezierRel->dx1, bezierRel->dy1, bezierRel->dx2, bezierRel->dy2, pos.x() + bezierRel->dx,
                        pos.y() + bezierRel->dy);
                break;
            }

            case 'S': {
                const cPathFigure::SmoothCubicBezierCurveTo *sBezierTo = static_cast<const cPathFigure::SmoothCubicBezierCurveTo*>(command);
                calcSmoothQuadBezierCP(painter, prevCommand, pos.x(), pos.y(), controlPoint.x, controlPoint.y,
                                       sBezierTo->x2, sBezierTo->y2, sBezierTo->x, sBezierTo->y);
                break;
            }

            case 's': {
                const cPathFigure::SmoothCubicBezierCurveRel *sBezierRel = static_cast<const cPathFigure::SmoothCubicBezierCurveRel*>(command);
                calcSmoothQuadBezierCP(painter, prevCommand, pos.x(), pos.y(), controlPoint.x, controlPoint.y,
                                       sBezierRel->dx2, sBezierRel->dy2, sBezierRel->dx, sBezierRel->dy, true);
                i += 4;
                break;
            }

            case 'Z': {
                painter.closeSubpath();
                break;
            }
        }
        prevCommand = command->code;
    }

    pathItem->setPath(painter);
    cFigure::Point offset = pathFigure->getOffset();
    pathItem->setTransform(QTransform().scale(offset.x, offset.y)); // XXX FIXME scale?! why?!
}

void PathFigureRenderer::refreshTransform(cFigure *figure, QGraphicsItem *item, const cFigure::Transform& transform)
{
    cFigure::Point offset = static_cast<cPathFigure *>(figure)->getOffset();
    QPointF qOffset = QPointF(offset.x, offset.y);
    setTransform(transform, item, &qOffset);
}

void PathFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPathFigure *pathFigure = static_cast<cPathFigure *>(figure);
    QGraphicsPathItem *shapeItem = static_cast<QGraphicsPathItem *>(item);

    QPainterPath painter = shapeItem->path();
    if (pathFigure->getFillRule() == cFigure::FILL_EVENODD)
        painter.setFillRule(Qt::WindingFill);
    else
        painter.setFillRule(Qt::OddEvenFill);
    shapeItem->setPath(painter);

    QPen pen = createPen(pathFigure, hints);
    Qt::PenJoinStyle joinStyle;

    switch (pathFigure->getJoinStyle()) {
        case cFigure::JOIN_BEVEL:
            joinStyle = Qt::BevelJoin;
            break;

        case cFigure::JOIN_MITER:
            joinStyle = Qt::MiterJoin;
            break;

        case cFigure::JOIN_ROUND:
            joinStyle = Qt::RoundJoin;
            break;
    }
    pen.setJoinStyle(joinStyle);

    Qt::PenCapStyle cap;
    switch (pathFigure->getCapStyle()) {
        case cFigure::CAP_BUTT:
            cap = Qt::FlatCap;
            break;

        case cFigure::CAP_SQUARE:
            cap = Qt::SquareCap;
            break;

        case cFigure::CAP_ROUND:
            cap = Qt::RoundCap;
            break;
    }
    pen.setCapStyle(cap);

    shapeItem->setPen(pen);
    shapeItem->setBrush(createBrush(pathFigure));
}

QGraphicsItem *PathFigureRenderer::newItem()
{
    return new PathItem();
}

void TextFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cAbstractTextFigure *textFigure = static_cast<cAbstractTextFigure *>(figure);
    QGraphicsSimpleTextItem *textItem = static_cast<QGraphicsSimpleTextItem *>(item);
    textItem->setText(QObject::trUtf8(textFigure->getText()));

    cFigure::Font font = textFigure->getFont();
    QFont qFont(font.typeface.c_str());

    qFont.setBold(font.style & cFigure::FONT_BOLD);
    qFont.setItalic(font.style & cFigure::FONT_ITALIC);
    qFont.setUnderline(font.style & cFigure::FONT_UNDERLINE);
    qFont.setPixelSize(font.pointSize <= 0 ? 16 : font.pointSize);

    textItem->setFont(qFont);
}

void TextFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cAbstractTextFigure *textFigure = static_cast<cAbstractTextFigure *>(figure);
    QGraphicsSimpleTextItem *textItem = static_cast<QGraphicsSimpleTextItem *>(item);

    cFigure::Color color = textFigure->getColor();
    QColor qColor(color.red, color.green, color.blue, textFigure->getOpacity()*255);
    textItem->setPen(QPen(Qt::NoPen));
    textItem->setBrush(QBrush(qColor));
}

QGraphicsItem *TextFigureRenderer::newItem()
{
    return new QGraphicsSimpleTextItem();
}

void LabelFigureRenderer::refreshTransform(cFigure *figure, QGraphicsItem *item, const cFigure::Transform& transform)
{
    QGraphicsSimpleTextItem *textItem = static_cast<QGraphicsSimpleTextItem *>(item);
    cAbstractTextFigure *textFigure = static_cast<cAbstractTextFigure *>(figure);

    QFontMetrics fontMetric(textItem->font());
    QRectF bounds = item->boundingRect();
    QPointF anchor;
    switch (textFigure->getAnchor()) {
        case cFigure::ANCHOR_CENTER:
            anchor = QPointF(-bounds.width()/2, -bounds.height()/2);
            break;

        case cFigure::ANCHOR_N:
            anchor = QPointF(-bounds.width()/2, 0);
            break;

        case cFigure::ANCHOR_E:
            anchor = QPointF(-bounds.width(), -bounds.height()/2);
            break;

        case cFigure::ANCHOR_S:
            anchor = QPointF(-bounds.width()/2, -bounds.height());
            break;

        case cFigure::ANCHOR_W:
            anchor = QPointF(0, -bounds.height()/2);
            break;

        case cFigure::ANCHOR_NE:
            anchor = QPointF(-bounds.width(), 0);
            break;

        case cFigure::ANCHOR_SE:
            anchor = QPointF(-bounds.width(), -bounds.height());
            break;

        case cFigure::ANCHOR_SW:
            anchor = QPointF(0, -bounds.height());
            break;

        case cFigure::ANCHOR_NW:
            anchor = QPointF(0, 0);
            break;

        case cFigure::ANCHOR_BASELINE_START:
            anchor = QPointF(0, -fontMetric.ascent());
            break;

        case cFigure::ANCHOR_BASELINE_MIDDLE:
            anchor = QPointF(-bounds.width()/2, -fontMetric.ascent());
            break;

        case cFigure::ANCHOR_BASELINE_END:
            anchor = QPointF(-bounds.width(), -fontMetric.ascent());
            break;
    }

    QTransform qTrans;
    cFigure::Point p = transform.applyTo(textFigure->getPosition());
    qTrans.translate(p.x, p.y);
    qTrans.translate(anchor.x(), anchor.y());
    item->setTransform(qTrans);
}

void ImageFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cImageFigure *imageFigure = static_cast<cImageFigure *>(figure);
    QImage *image = getQtenv()->icons.getImage(imageFigure->getImageName(), "");
    if(image->isNull())
        qDebug() << "ImageFigureRenderer::setItemGeometryProperties: Image file not found.";
    QGraphicsPixmapItem *imageItem = static_cast<QGraphicsPixmapItem *>(item);

    if (imageFigure->getWidth() != 0 && imageFigure->getHeight() != 0)
        imageItem->setPixmap(QPixmap::fromImage(image->scaled(imageFigure->getWidth(), imageFigure->getHeight())));
    else
        imageItem->setPixmap(QPixmap::fromImage(*image));
}

void ImageFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cImageFigure *imageFigure = static_cast<cImageFigure *>(figure);
    QGraphicsPixmapItem *imageItem = static_cast<QGraphicsPixmapItem *>(item);

    //TODO not working with QTransform
    if (imageFigure->getInterpolation() == cFigure::INTERPOLATION_BEST)
        imageItem->setTransformationMode(Qt::SmoothTransformation);
    else
        imageItem->setTransformationMode(Qt::FastTransformation);

    imageItem->setOpacity(imageFigure->getOpacity());

    QGraphicsColorizeEffect *colorizeEffect = new QGraphicsColorizeEffect();
    cFigure::Color color = imageFigure->getTintColor();
    colorizeEffect->setColor(QColor(color.red, color.green, color.blue));
    colorizeEffect->setStrength(imageFigure->getTintAmount());
    imageItem->setGraphicsEffect(colorizeEffect);
}

QGraphicsItem *ImageFigureRenderer::newItem()
{
    return new QGraphicsPixmapItem();
}

void PixmapFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPixmapFigure *pixmapFigure = static_cast<cPixmapFigure *>(figure);
    QGraphicsPixmapItem *pixmapItem = static_cast<QGraphicsPixmapItem *>(item);

    QImage image(pixmapFigure->getPixmapWidth(), pixmapFigure->getPixmapHeight(), QImage::Format_ARGB32);
    for(int x = 0; x < image.width(); ++x)
        for(int y = 0; y < image.height(); ++y)
        {
            cFigure::RGBA rgba = pixmapFigure->getPixel(x, y);
            image.setPixel(x, y, qRgba(rgba.red, rgba.green, rgba.blue, rgba.alpha));
        }

    Qt::TransformationMode transMode = pixmapFigure->getInterpolation() == cFigure::INTERPOLATION_BEST ?
                Qt::FastTransformation : Qt::SmoothTransformation;
    image = image.scaled(pixmapFigure->getWidth(), pixmapFigure->getHeight(), Qt::IgnoreAspectRatio, transMode);
    pixmapItem->setPixmap(QPixmap::fromImage(image));
}

} // namespace qtenv
} // namespace omnetpp

