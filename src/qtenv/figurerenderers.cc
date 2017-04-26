//==========================================================================
//  FIGURERENDERERS.CC - part of
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

#include "figurerenderers.h"

#include "graphicsitems.h"
#include "qtenv.h"
#include <sstream>
#include <cfloat>
#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsItem>
#include <QFontMetrics>
#include <QPainter>
#include "common/stringutil.h"
#include "omnetpp/platdep/platmisc.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {

Register_Class(NullRenderer);
Register_Class(GroupFigureRenderer);
Register_Class(LineFigureRenderer);
Register_Class(ArcFigureRenderer);
Register_Class(PolylineFigureRenderer);
Register_Class(RectangleFigureRenderer);
Register_Class(OvalFigureRenderer);
Register_Class(RingFigureRenderer);
Register_Class(PieSliceFigureRenderer);
Register_Class(PolygonFigureRenderer);
Register_Class(PathFigureRenderer);
Register_Class(TextFigureRenderer);
Register_Class(LabelFigureRenderer);
Register_Class(ImageFigureRenderer);
Register_Class(IconFigureRenderer);
Register_Class(PixmapFigureRenderer);

QPainterPath FigureRenderer::PathItem::shape() const
{
    QPainterPath result;

    const QBrush& b = brush();
    const QPen& p = pen();

    if (b != Qt::NoBrush && b.color().alpha() > 0)
        result += path();  // if we are filled visibly, including the insides

    if (p != Qt::NoPen && p.color().alpha() > 0) {
        // and if the outline is drawn, including that too.
        // Not using the constructor that takes a QPen because we want to avoid
        // passing the dashOffset and dashPattern to the stroker, as that
        // makes the path too complicated, is too inaccurate, and isn't necessary.
        QPainterPathStroker s;
        // setting up the stroker properly
        s.setMiterLimit(p.miterLimit());
        s.setJoinStyle(p.joinStyle());
        s.setWidth(p.widthF());  // note the F at the end...
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
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QColor(0, 0, 255, 100));
    painter->drawRect(boundingRect());
}
*/

std::map<std::string, FigureRenderer *> FigureRenderer::rendererCache;

FigureRenderer *FigureRenderer::getRendererFor(cFigure *figure)
{
    FigureRenderer *renderer;
    std::string rendererClassName = figure->getRendererClassName();
    std::map<std::string, FigureRenderer *>::iterator it = rendererCache.find(rendererClassName);
    if (it != rendererCache.end())
        renderer = it->second;
    else {
        // create renderer and add to the cache
        if (rendererClassName == "")
            renderer = new NullRenderer();
        else {
            if (rendererClassName.find(':') == std::string::npos)
                rendererClassName = "omnetpp::qtenv::" + rendererClassName;
            cObjectFactory *factory = cObjectFactory::find(rendererClassName.c_str());
            if (!factory)
                throw cRuntimeError("No renderer class '%s' for figure class '%s'", rendererClassName.c_str(), figure->getClassName());
            cObject *obj = factory->createOne();
            renderer = dynamic_cast<FigureRenderer *>(obj);
            if (!renderer)
                throw cRuntimeError("Wrong figure renderer class: Cannot cast %s to FigureRenderer", obj->getClassName());
        }
        rendererCache[rendererClassName] = renderer;
    }
    return renderer;
}

// from mozilla
double FigureRenderer::calcVectorAngle(double ux, double uy, double vx, double vy)
{
    double ta = atan2(uy, ux);
    double tb = atan2(vy, vx);

    if (tb >= ta)
        return tb-ta;
    else
        return 2.0*M_PI - (ta-tb);
}

bool FigureRenderer::doubleEquals(double x, double y)
{
    return fabs(x - y) < DBL_EPSILON;
}

// http://www.w3.org/TR/SVG/implnote.html#ArcConversionEndpointToCenter
FigureRenderer::ArcShape FigureRenderer::endPointToCentralFromArcParameters(double startX, double startY, double endX, double endY, bool largeArcFlag, bool sweepFlag,
        double& rx, double& ry, double angle, double& centerX, double& centerY, double& startAngle, double& sweepLength)
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
            int segments = (int)std::ceil(std::fabs(sweepLength/(M_PI/2.0)));
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
            cpx = currentX + std::fabs(currentX - cpx);
        else
            cpx = currentX - std::fabs(currentX - cpx);

        if (cpy < currentY)
            cpy = currentY + std::fabs(currentY - cpy);
        else
            cpy = currentY - std::fabs(currentY - cpy);
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
            prevCpx = currentX + std::fabs(currentX - prevCpx);
        else
            prevCpx = currentX - std::fabs(currentX - prevCpx);

        if (prevCpy < currentY)
            prevCpy = currentY + std::fabs(currentY - prevCpy);
        else
            prevCpy = currentY - std::fabs(currentY - prevCpy);
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
        case cFigure::LINE_SOLID: style = Qt::SolidLine; break;
        case cFigure::LINE_DOTTED: style = Qt::DotLine; break;
        case cFigure::LINE_DASHED: style = Qt::DashLine; break;
    }
    pen.setStyle(style);

    Qt::PenCapStyle cap;
    switch (figure->getCapStyle()) {
        case cFigure::CAP_BUTT: cap = Qt::FlatCap; break;
        case cFigure::CAP_SQUARE: cap = Qt::SquareCap; break;
        case cFigure::CAP_ROUND: cap = Qt::RoundCap; break;
    }
    pen.setCapStyle(cap);

    pen.setJoinStyle(Qt::MiterJoin);

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
            case cFigure::LINE_SOLID: style = Qt::SolidLine; break;
            case cFigure::LINE_DOTTED: style = Qt::DotLine; break;
            case cFigure::LINE_DASHED: style = Qt::DashLine; break;
        }
        pen.setStyle(style);

        pen.setJoinStyle(Qt::MiterJoin);
        pen.setCapStyle(Qt::FlatCap);
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

QPointF FigureRenderer::getAnchorOffset(cFigure::Anchor anchor, double width, double height, double ascent)
{
    QPointF offset;

    switch (anchor) {
        case cFigure::ANCHOR_CENTER:
        case cFigure::ANCHOR_N:
        case cFigure::ANCHOR_S:
        case cFigure::ANCHOR_BASELINE_MIDDLE:
            offset.setX(width/2);
            break;
        case cFigure::ANCHOR_E:
        case cFigure::ANCHOR_NE:
        case cFigure::ANCHOR_SE:
        case cFigure::ANCHOR_BASELINE_END:
            offset.setX(width);
            break;
        default:
            break;
    }

    switch (anchor) {
        case cFigure::ANCHOR_CENTER:
        case cFigure::ANCHOR_E:
        case cFigure::ANCHOR_W:
            offset.setY(height/2);
            break;
        case cFigure::ANCHOR_S:
        case cFigure::ANCHOR_SE:
        case cFigure::ANCHOR_SW:
            offset.setY(height);
            break;
        case cFigure::ANCHOR_BASELINE_START:
        case cFigure::ANCHOR_BASELINE_MIDDLE:
        case cFigure::ANCHOR_BASELINE_END:
            offset.setY(ascent);
            break;
        default:
            break;
    }

    return -offset;
}

void FigureRenderer::setTransform(const cFigure::Transform& transform, QGraphicsItem *item) const
{
    item->setTransform(QTransform(
                           transform.a,  transform.b,  0,
                           transform.c,  transform.d,  0,
                           transform.t1, transform.t2, 1));
}

QGraphicsItem *FigureRenderer::render(cFigure *figure, GraphicsLayer *layer, FigureRenderingHints *hints)
{
    QGraphicsItem *item = createGeometry(figure, hints);
    createVisual(figure, item, hints);
    refreshTransform(figure, item, hints);

    if (item) {
        item->setParentItem(layer);
        item->setData(ITEMDATA_TOOLTIP, QString(figure->getTooltip()));
        item->setZValue(figure->getZIndex());
    }

    return item;
}

void FigureRenderer::refresh(cFigure *figure, QGraphicsItem *item, int8_t what, FigureRenderingHints *hints)
{
    if (what & cFigure::CHANGE_VISUAL)
        refreshVisual(figure, item, hints);
    if (what & (cFigure::CHANGE_GEOMETRY | cFigure::CHANGE_INPUTDATA))
        refreshGeometry(figure, item, hints);
    if (what & cFigure::CHANGE_TRANSFORM)
        refreshTransform(figure, item, hints);
}

void FigureRenderer::refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    setTransform(figure->getTransform(), item);
}

QGraphicsItem *FigureRenderer::createGeometry(cFigure *figure, FigureRenderingHints *hints)
{
    QGraphicsItem *item = newItem();
    if (item) {
        setItemGeometryProperties(figure, item, hints);
        item->setData(ITEMDATA_COBJECT, QVariant::fromValue((cObject *)figure));
        for (auto c : item->childItems())
            if (c)
                c->setData(ITEMDATA_COBJECT, QVariant::fromValue((cObject *)figure));

    }
    return item;
}

void FigureRenderer::refreshGeometry(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    setItemGeometryProperties(figure, item, hints);
}

void FigureRenderer::refreshVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    createVisual(figure, item, hints);
    item->setData(ITEMDATA_TOOLTIP, QString(figure->getTooltip()));
    item->setZValue(figure->getZIndex());
}

void AbstractShapeFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cAbstractShapeFigure *shapeFigure = static_cast<cAbstractShapeFigure *>(figure);
    QAbstractGraphicsShapeItem *shapeItem = static_cast<QAbstractGraphicsShapeItem *>(item);
    shapeItem->setPen(createPen(shapeFigure, hints));
    shapeItem->setBrush(createBrush(shapeFigure));
}

QGraphicsItem *AbstractShapeFigureRenderer::newItem()
{
    return new PathItem();
}

void AbstractImageFigureRenderer::setImageTransform(cAbstractImageFigure *figure, QGraphicsItem *item, const cFigure::Transform &transform, double naturalWidth, double naturalHeight, bool translateOnly, double zoom)
{
    double width = figure->getWidth();
    if (width <= 0)
        width = naturalWidth;

    double height = figure->getHeight();
    if (height <= 0)
        height = naturalHeight;

    cFigure::Point pos = figure->getPosition();

    QTransform trans;
    QTransform selfTrans;

    if (translateOnly) {
        // figure is an icon, so the zooming transformation will not position it right
        // because of the ItemIgnoresTransformations flag, this is why we have to
        // multiply the coordinates with the zoom level, and not apply the
        // transformation
        pos = figure->getTransform().applyTo(pos);
        pos = pos * zoom;

        // XXX: is this needed?
        // makes the child coordinate system follow zoom, but cancels it out for this item
        trans.scale(zoom, zoom);
        selfTrans.scale(1.0/zoom, 1.0/zoom);
    } else {
        // item doesn't ignore transformations, zoom will be done by that, and
        // we have the use the transformation
        trans = QTransform(transform.a, transform.b, 0,
                            transform.c, transform.d, 0,
                            transform.t1, transform.t2, 1);
    }

    QPointF anchorOffset = getAnchorOffset(figure->getAnchor(), width, height);
    QPointF scale(width / naturalWidth, height / naturalHeight);

    SelfTransformingPixmapItem *pixmapItem = dynamic_cast<SelfTransformingPixmapItem *>(item);
    ASSERT(pixmapItem);

    // anchoring, positioning and width / height must not affect the coordinate system of the children
    selfTrans.translate(anchorOffset.x() + pos.x, anchorOffset.y() + pos.y);
    selfTrans.scale(scale.x(), scale.y());

    pixmapItem->setTransform(trans);
    pixmapItem->setSelfTransform(selfTrans);
}

void AbstractImageFigureRenderer::refresh(cFigure *figure, QGraphicsItem *item, int8_t what, FigureRenderingHints *hints)
{
    if (what & cFigure::CHANGE_VISUAL)
        refreshVisual(figure, item, hints);
    if (what & (cFigure::CHANGE_GEOMETRY | cFigure::CHANGE_INPUTDATA | cFigure::CHANGE_TRANSFORM)) {
        refreshGeometry(figure, item, hints);
        refreshTransform(figure, item, hints);
    }
}

void LineFigureRenderer::setArrows(cLineFigure *lineFigure, QGraphicsLineItem *lineItem, double zoom)
{
    ArrowheadItem *startArrow = dynamic_cast<ArrowheadItem *>(lineItem->childItems()[0]);
    ArrowheadItem *endArrow = dynamic_cast<ArrowheadItem *>(lineItem->childItems()[1]);

    const auto& start = lineFigure->getStart();
    const auto& end = lineFigure->getEnd();

    startArrow->setEndPoints(QPointF(end.x, end.y), QPointF(start.x, start.y));
    endArrow->setEndPoints(QPointF(start.x, start.y), QPointF(end.x, end.y));

    setArrowStyle(lineFigure, startArrow, endArrow, zoom);
}

void AbstractLineFigureRenderer::setArrowStyle(cAbstractLineFigure *figure, ArrowheadItem *startItem, ArrowheadItem *endItem, double zoom)
{
    auto lc = figure->getLineColor();
    QColor color(lc.red, lc.green, lc.blue, figure->getLineOpacity()*255);
    double scale = figure->getZoomLineWidth() ? 1.0 : (1.0 / zoom);


    auto startStyle = figure->getStartArrowhead();

    startItem->setVisible(startStyle != cFigure::ARROW_NONE);
    startItem->setFillRatio(startStyle == cFigure::ARROW_BARBED ? 0.75
                            : startStyle == cFigure::ARROW_SIMPLE ? 0
                            : 1);

    startItem->setColor(color);
    startItem->setLineWidth(scale * figure->getLineWidth());
    startItem->setSizeForPenWidth(figure->getLineWidth(), scale);


    auto endStyle = figure->getEndArrowhead();

    endItem->setVisible(endStyle != cFigure::ARROW_NONE);
    endItem->setFillRatio(endStyle == cFigure::ARROW_BARBED ? 0.75
                            : endStyle == cFigure::ARROW_SIMPLE ? 0
                            : 1);

    endItem->setColor(color);

    endItem->setLineWidth(scale * figure->getLineWidth());
    endItem->setSizeForPenWidth(figure->getLineWidth(), scale);
}

void LineFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cLineFigure *lineFigure = static_cast<cLineFigure *>(figure);
    QGraphicsLineItem *lineItem = static_cast<QGraphicsLineItem *>(item);
    QPointF start(lineFigure->getStart().x, lineFigure->getStart().y);
    QPointF end(lineFigure->getEnd().x, lineFigure->getEnd().y);

    lineItem->setLine(QLineF(start, end));
    setArrows(lineFigure, lineItem, hints->zoom);
}

void LineFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cLineFigure *lineFigure = static_cast<cLineFigure *>(figure);
    QGraphicsLineItem *lineItem = static_cast<QGraphicsLineItem *>(item);
    lineItem->setPen(createPen(lineFigure, hints));
    setArrows(lineFigure, lineItem, hints->zoom);
}

QGraphicsItem *LineFigureRenderer::newItem()
{
    QGraphicsLineItem *lineItem = new QGraphicsLineItem();
    new ArrowheadItem(lineItem);
    new ArrowheadItem(lineItem);
    return lineItem;
}

void ArcFigureRenderer::setArrows(cArcFigure *arcFigure, QGraphicsPathItem *arcItem, double zoom)
{
    ArrowheadItem *startArrow = static_cast<ArrowheadItem *>(arcItem->childItems()[0]);

    { // should set the arrow up even if it isn't visible, so the bounding box is in the right place (even if empty)
        auto b = arcFigure->getBounds();
        QPointF center(b.getCenter().x, b.getCenter().y);
        QPointF radii(b.width/2, b.height/2);

        double angle = arcFigure->getStartAngle();

        // this is from the center towards the end of the arc
        QPointF delta(cos(angle) * radii.x(), -sin(angle) * radii.y());

        // this is the direction in which the arc ends
        QPointF tangent(-sin(angle) * radii.x(), -cos(angle) * radii.y());

        startArrow->setEndPoints(center + delta + tangent, center + delta);
        // use these two instead of the one above to make the arrowheads keep their
        // shapes even if the line they are on is transformed heavily (incomplete)
        // (also see the similar comment in graphicspatharrowitem.cc)
        //cFigure::Point transfDir = arcFigure->getTransform().applyTo(end) - arcFigure->getTransform().applyTo(start);
        //startArrow->setEndPoints(QPointF(end.x - transfDir.x, end.y - transfDir.y), QPointF(end.x, end.y));
    }

    startArrow->setVisible(arcFigure->getStartArrowhead() != cFigure::ARROW_NONE);


    ArrowheadItem *endArrow = static_cast<ArrowheadItem *>(arcItem->childItems()[1]);

    {
        auto b = arcFigure->getBounds();
        QPointF center(b.getCenter().x, b.getCenter().y);
        QPointF radii(b.width/2, b.height/2);

        double angle = arcFigure->getEndAngle();

        // this is from the center towards the start of the arc
        QPointF delta(cos(angle) * radii.x(), -sin(angle) * radii.y());

        // this is the direction in which the arc begins
        QPointF tangent(-sin(angle) * radii.x(), -cos(angle) * radii.y());

        endArrow->setEndPoints(center + delta - tangent, center + delta);
    }

    endArrow->setVisible(arcFigure->getEndArrowhead() != cFigure::ARROW_NONE);


    setArrowStyle(arcFigure, startArrow, endArrow, zoom);
}

void ArcFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cArcFigure *arcFigure = static_cast<cArcFigure *>(figure);
    QGraphicsPathItem *arcItem = static_cast<QGraphicsPathItem *>(item);

    cFigure::Rectangle bounds = arcFigure->getBounds();
    double start = arcFigure->getStartAngle()*180/M_PI;
    double end = arcFigure->getEndAngle()*180/M_PI;
    QPainterPath path;
    path.arcMoveTo(bounds.x, bounds.y, bounds.width, bounds.height, start);
    path.arcTo(bounds.x, bounds.y, bounds.width, bounds.height, start, end - start);
    arcItem->setPath(path);
    setArrows(arcFigure, arcItem, hints->zoom);
}

void ArcFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cArcFigure *arcFigure = static_cast<cArcFigure *>(figure);
    QGraphicsPathItem *arcItem = static_cast<QGraphicsPathItem *>(item);
    QPen pen = createPen(arcFigure, hints);
    arcItem->setPen(pen);

    setArrows(arcFigure, arcItem, hints->zoom);
}

QGraphicsItem *ArcFigureRenderer::newItem()
{
    QGraphicsPathItem *arcItem = new PathItem();
    new ArrowheadItem(arcItem);
    new ArrowheadItem(arcItem);
    return arcItem;
}

void PolylineFigureRenderer::setArrows(cPolylineFigure *polyFigure, FigureRenderer::PathItem *polyItem, double zoom)
{
    ArrowheadItem *startArrow = static_cast<ArrowheadItem *>(polyItem->childItems()[0]);
    ArrowheadItem *endArrow = static_cast<ArrowheadItem *>(polyItem->childItems()[1]);

    const auto& points = polyFigure->getPoints();

    if (points.size() < 2) {
        // can't display arrow on a polyline without at least two points
        startArrow->setVisible(false);
        endArrow->setVisible(false);
        return;
    }

    if (polyFigure->getStartArrowhead() != cFigure::ARROW_NONE) {
        startArrow->setVisible(true);
        const auto& from = points[1];
        const auto& to = points[0];
        startArrow->setEndPoints(QPointF(from.x, from.y), QPointF(to.x, to.y));
    }
    else
        startArrow->setVisible(false);

    if (polyFigure->getEndArrowhead() != cFigure::ARROW_NONE) {
        endArrow->setVisible(true);
        const auto& from = points[points.size() - 2];
        const auto& to = points[points.size() - 1];
        endArrow->setEndPoints(QPointF(from.x, from.y), QPointF(to.x, to.y));
    }
    else
        endArrow->setVisible(false);

    setArrowStyle(polyFigure, startArrow, endArrow, zoom);
}

void PolylineFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPolylineFigure *polyFigure = static_cast<cPolylineFigure *>(figure);
    PathItem *polyItem = static_cast<PathItem *>(item);

    QPainterPath painter;

    std::vector<cFigure::Point> points = polyFigure->getPoints();
    if (points.size() < 2) {
        polyItem->setPath(painter);
        return;  // throw cRuntimeError("PolylineFigureRenderer: points.size() < 2");
    }

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

    setArrows(polyFigure, polyItem, hints->zoom);
}

void PolylineFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPolylineFigure *polyFigure = static_cast<cPolylineFigure *>(figure);
    PathItem *polyItem = static_cast<PathItem *>(item);

    if (!(figure->getParentFigure()->getLocalChangeFlags() & cFigure::CHANGE_STRUCTURAL || figure->getLocalChangeFlags() & cFigure::CHANGE_GEOMETRY))
        setItemGeometryProperties(polyFigure, polyItem, hints);

    QPen pen = createPen(polyFigure, hints);
    auto joinStyle = polyFigure->getJoinStyle();
    pen.setJoinStyle(joinStyle == cFigure::JOIN_BEVEL ? Qt::BevelJoin
                      : joinStyle == cFigure::JOIN_MITER ? Qt::MiterJoin
                      : Qt::RoundJoin);

    polyItem->setPen(pen);

    setArrows(polyFigure, polyItem, hints->zoom);
}

QGraphicsItem *PolylineFigureRenderer::newItem()
{
    PathItem *polylineItem = new PathItem();
    new ArrowheadItem(polylineItem);
    new ArrowheadItem(polylineItem);
    return polylineItem;
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

void OvalFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cOvalFigure *ovalFigure = static_cast<cOvalFigure *>(figure);
    QGraphicsPathItem *pathItem = static_cast<QGraphicsPathItem *>(item);

    cFigure::Rectangle bounds = ovalFigure->getBounds();

    QPainterPath path;
    path.addEllipse(bounds.x, bounds.y, bounds.width, bounds.height);
    pathItem->setPath(path);
}

void RingFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cRingFigure *ringFigure = static_cast<cRingFigure *>(figure);
    QGraphicsPathItem *ringItem = static_cast<QGraphicsPathItem *>(item);
    QPainterPath path;
    cFigure::Rectangle bounds = ringFigure->getBounds();
    cFigure::Point center = bounds.getCenter();
    double innerRx = ringFigure->getInnerRx();
    double innerRy = ringFigure->getInnerRy();

    path.addEllipse(bounds.x, bounds.y, bounds.width, bounds.height);
    path.addEllipse(center.x-innerRx, center.y-innerRy, 2*innerRx, 2*innerRy);

    ringItem->setPath(path);
}

void PieSliceFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPieSliceFigure *pieSliceFigure = static_cast<cPieSliceFigure *>(figure);
    QGraphicsPathItem *pathItem = static_cast<QGraphicsPathItem *>(item);
    cFigure::Rectangle bounds = pieSliceFigure->getBounds();
    cFigure::Point center = bounds.getCenter();

    QPainterPath path;

    double start = pieSliceFigure->getStartAngle()*180/M_PI;
    double end = pieSliceFigure->getEndAngle()*180/M_PI;

    // This is here to move the spoke to the end angle if there is more than 1.0 pie, c:
    // but keep it at the start if there is less than -1.0 pie. :c
    start = std::max(start, end - 360);

    path.moveTo(center.x, center.y);
    path.arcTo(bounds.x, bounds.y, bounds.width, bounds.height, start, end - start);
    path.closeSubpath();

    pathItem->setPath(path);
}

void PolygonFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPolygonFigure *polyFigure = static_cast<cPolygonFigure *>(figure);
    QGraphicsPathItem *polyItem = static_cast<QGraphicsPathItem *>(item);

    QPainterPath painter;

    std::vector<cFigure::Point> points = polyFigure->getPoints();
    if (points.size() < 2) {
        polyItem->setPath(painter);
        return;
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

    painter.setFillRule(polyFigure->getFillRule() == cFigure::FILL_EVENODD
                         ? Qt::OddEvenFill
                         : Qt::WindingFill);

    polyItem->setPath(painter);

    QPen pen = createPen(polyFigure, hints);

    auto joinStyle = polyFigure->getJoinStyle();
    pen.setJoinStyle(joinStyle == cFigure::JOIN_BEVEL ? Qt::BevelJoin
                      : joinStyle == cFigure::JOIN_MITER ? Qt::MiterJoin
                      : Qt::RoundJoin);

    polyItem->setPen(pen);
    polyItem->setBrush(createBrush(polyFigure));
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
                const cPathFigure::MoveTo *moveTo = static_cast<const cPathFigure::MoveTo *>(command);
                painter.moveTo(moveTo->x, moveTo->y);
                break;
            }

            case 'm': {
                const cPathFigure::MoveRel *moveRel = static_cast<const cPathFigure::MoveRel *>(command);
                painter.moveTo(pos.x() + moveRel->dx, pos.y() + moveRel->dy);
                break;
            }

            case 'L': {
                const cPathFigure::LineTo *lineTo = static_cast<const cPathFigure::LineTo *>(command);
                painter.lineTo(lineTo->x, lineTo->y);
                break;
            }

            case 'l': {
                const cPathFigure::LineRel *lineRel = static_cast<const cPathFigure::LineRel *>(command);
                painter.lineTo(pos.x() + lineRel->dx, pos.y() + lineRel->dy);
                break;
            }

            case 'H': {
                const cPathFigure::HorizontalLineTo *horizLineTo = static_cast<const cPathFigure::HorizontalLineTo *>(command);
                painter.lineTo(horizLineTo->x, pos.y());
                break;
            }

            case 'h': {
                const cPathFigure::HorizontalLineRel *horizLineRel = static_cast<const cPathFigure::HorizontalLineRel *>(command);
                painter.lineTo(pos.x() + horizLineRel->dx, pos.y());
                break;
            }

            case 'V': {
                const cPathFigure::VerticalLineTo *vertLineTo = static_cast<const cPathFigure::VerticalLineTo *>(command);
                painter.lineTo(pos.x(), vertLineTo->y);
                break;
            }

            case 'v': {
                const cPathFigure::VerticalLineRel *vertLineRel = static_cast<const cPathFigure::VerticalLineRel *>(command);
                painter.lineTo(pos.x(), pos.y() + vertLineRel->dy);
                break;
            }

            case 'A': {
                const cPathFigure::ArcTo *arcTo = static_cast<const cPathFigure::ArcTo *>(command);
                arcToUsingBezier(painter, pos.x(), pos.y(), arcTo->x, arcTo->y, arcTo->rx, arcTo->ry,
                        arcTo->phi, arcTo->largeArc, arcTo->sweep, false);
                break;
            }

            case 'a': {
                const cPathFigure::ArcRel *arcRel = static_cast<const cPathFigure::ArcRel *>(command);
                arcToUsingBezier(painter, pos.x(), pos.y(), arcRel->dx, arcRel->dy, arcRel->rx, arcRel->ry,
                        arcRel->phi, arcRel->largeArc, arcRel->sweep, true);
                break;
            }

            case 'Q': {
                const cPathFigure::CurveTo *curveTo = static_cast<const cPathFigure::CurveTo *>(command);
                controlPoint = cFigure::Point(curveTo->x1, curveTo->y1);
                painter.quadTo(controlPoint.x, controlPoint.y, curveTo->x, curveTo->y);
                break;
            }

            case 'q': {
                const cPathFigure::CurveRel *curveRel = static_cast<const cPathFigure::CurveRel *>(command);
                QPointF pos = painter.currentPosition();
                controlPoint = cFigure::Point(pos.x() + curveRel->dx1, pos.y() + curveRel->dy1);
                painter.quadTo(controlPoint.x, controlPoint.y, pos.x() + curveRel->dx, pos.y() + curveRel->dy);
                break;
            }

            case 'T': {
                const cPathFigure::SmoothCurveTo *smoothCurveTo = static_cast<const cPathFigure::SmoothCurveTo *>(command);
                calcSmoothBezierCP(painter, prevCommand, pos.x(), pos.y(), controlPoint.x, controlPoint.y,
                        smoothCurveTo->x, smoothCurveTo->y);
                break;
            }

            case 't': {
                const cPathFigure::SmoothCurveRel *smoothCurveRel = static_cast<const cPathFigure::SmoothCurveRel *>(command);
                calcSmoothBezierCP(painter, prevCommand, pos.x(), pos.y(), controlPoint.x, controlPoint.y,
                        smoothCurveRel->dx, smoothCurveRel->dy, true);
                break;
            }

            case 'C': {
                const cPathFigure::CubicBezierCurveTo *bezierTo = static_cast<const cPathFigure::CubicBezierCurveTo *>(command);
                controlPoint = cFigure::Point(bezierTo->x2, bezierTo->y2);
                painter.cubicTo(bezierTo->x1, bezierTo->y1, bezierTo->x2, bezierTo->y2, bezierTo->x,
                        bezierTo->y);
                break;
            }

            case 'c': {
                const cPathFigure::CubicBezierCurveRel *bezierRel = static_cast<const cPathFigure::CubicBezierCurveRel *>(command);
                QPointF controlPoint1(pos.x() + bezierRel->dx1, pos.y() + bezierRel->dy1);
                QPointF controlPoint2(pos.x() + bezierRel->dx2, pos.y() + bezierRel->dy2);
                QPointF endPoint(pos.x() + bezierRel->dx, pos.y() + bezierRel->dy);
                controlPoint = cFigure::Point(controlPoint2.x(), controlPoint2.y());
                painter.cubicTo(controlPoint1, controlPoint2, endPoint);
                break;
            }

            case 'S': {
                const cPathFigure::SmoothCubicBezierCurveTo *sBezierTo = static_cast<const cPathFigure::SmoothCubicBezierCurveTo *>(command);
                calcSmoothQuadBezierCP(painter, prevCommand, pos.x(), pos.y(), controlPoint.x, controlPoint.y,
                        sBezierTo->x2, sBezierTo->y2, sBezierTo->x, sBezierTo->y);
                break;
            }

            case 's': {
                const cPathFigure::SmoothCubicBezierCurveRel *sBezierRel = static_cast<const cPathFigure::SmoothCubicBezierCurveRel *>(command);
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

    auto o = pathFigure->getOffset();
    painter.translate(o.x, o.y);

    pathItem->setPath(painter);
}

void PathFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPathFigure *pathFigure = static_cast<cPathFigure *>(figure);
    QGraphicsPathItem *shapeItem = static_cast<QGraphicsPathItem *>(item);

    shapeItem->setCacheMode(QGraphicsItem::NoCache);
    QPainterPath painter = shapeItem->path();
    if (pathFigure->getFillRule() == cFigure::FILL_EVENODD)
        painter.setFillRule(Qt::WindingFill);
    else
        painter.setFillRule(Qt::OddEvenFill);
    shapeItem->setPath(painter);

    QPen pen = createPen(pathFigure, hints);

    auto joinStyle = pathFigure->getJoinStyle();
    pen.setJoinStyle(joinStyle == cFigure::JOIN_BEVEL ? Qt::BevelJoin
                      : joinStyle == cFigure::JOIN_MITER ? Qt::MiterJoin
                      : Qt::RoundJoin);

    auto capStyle = pathFigure->getCapStyle();
    pen.setCapStyle(capStyle == cFigure::CAP_ROUND ? Qt::RoundCap
                      : capStyle == cFigure::CAP_SQUARE ? Qt::SquareCap
                      : Qt::FlatCap);

    shapeItem->setPen(pen);
    shapeItem->setBrush(createBrush(pathFigure));
}

void TextFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cAbstractTextFigure *textFigure = static_cast<cAbstractTextFigure *>(figure);
    OutlinedTextItem *textItem = static_cast<OutlinedTextItem *>(item);

    ASSERT(textFigure);
    ASSERT(textItem);

    textItem->setText(QString::fromUtf8(textFigure->getText()));
}

void TextFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cAbstractTextFigure *textFigure = dynamic_cast<cAbstractTextFigure *>(figure);
    OutlinedTextItem *textItem = dynamic_cast<OutlinedTextItem *>(item);

    ASSERT(textFigure);
    ASSERT(textItem);

    cFigure::Font font = textFigure->getFont();

    std::string typeface = font.typeface;
    if (typeface.empty())
        typeface = hints->defaultFont;

    int pointSize = font.pointSize;
    if (pointSize <= 0)
        pointSize = hints->defaultFontSize;

    QFont qFont(typeface.c_str(), pointSize);

    qFont.setBold(font.style & cFigure::FONT_BOLD);
    qFont.setItalic(font.style & cFigure::FONT_ITALIC);
    qFont.setUnderline(font.style & cFigure::FONT_UNDERLINE);

    textItem->setFont(qFont);

    cFigure::Color color = textFigure->getColor();
    textItem->setBrush(QColor(color.red, color.green, color.blue));
    textItem->setOpacity(textFigure->getOpacity());
    textItem->setHaloEnabled(textFigure->getHalo());
}

// XXX: necessary? sufficient? could we have a cleaner solution?
void TextFigureRenderer::refresh(cFigure *figure, QGraphicsItem *item, int8_t what, FigureRenderingHints *hints)
{
    if (what & cFigure::CHANGE_VISUAL)
        refreshVisual(figure, item, hints);
    if (what & (cFigure::CHANGE_GEOMETRY | cFigure::CHANGE_INPUTDATA | cFigure::CHANGE_TRANSFORM)) {
        refreshGeometry(figure, item, hints);
        refreshTransform(figure, item, hints);
    }
}

void TextFigureRenderer::refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    OutlinedTextItem *textItem = dynamic_cast<OutlinedTextItem *>(item);
    cAbstractTextFigure *textFigure = dynamic_cast<cAbstractTextFigure *>(figure);

    ASSERT(textFigure);
    ASSERT(textItem);

    cFigure::Point p = textFigure->getPosition();

    QPointF offset(p.x, p.y);

    QFontMetricsF fontMetric(textItem->font());
    QRectF textRect = textItem->textRect();
    offset += getAnchorOffset(textFigure->getAnchor(), textRect.width(), textRect.height(), fontMetric.ascent())
            + (textRect.bottomRight() - textItem->boundingRect().bottomRight()) / 2; // outline offset

    textItem->setOffset(offset);

    setTransform(figure->getTransform(), item);
}

QGraphicsItem *TextFigureRenderer::newItem()
{
    return new OutlinedTextItem();
}

void LabelFigureRenderer::refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    OutlinedTextItem *textItem = dynamic_cast<OutlinedTextItem *>(item);
    cAbstractTextFigure *textFigure = dynamic_cast<cAbstractTextFigure *>(figure);

    ASSERT(textFigure);
    ASSERT(textItem);

    item->setFlag(QGraphicsItem::ItemIgnoresTransformations);

    cFigure::Point p = figure->getTransform().applyTo(textFigure->getPosition()) * hints->zoom;

    QPointF offset(p.x, p.y);

    QFontMetricsF fontMetric(textItem->font());
    QRectF textRect = textItem->textRect();
    offset += getAnchorOffset(textFigure->getAnchor(), textRect.width(), textRect.height(), fontMetric.ascent())
            + (textRect.bottomRight() - textItem->boundingRect().bottomRight()) / 2; // outline offset

    textItem->setOffset(offset);
}

void ImageFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cImageFigure *imageFigure = static_cast<cImageFigure *>(figure);
    SelfTransformingPixmapItem *imageItem = static_cast<SelfTransformingPixmapItem *>(item);

    cFigure::Color color = imageFigure->getTintColor();
    QColor tintColor(color.red, color.green, color.blue);
    imageItem->setPixmap(getQtenv()->icons.getTintedPixmap(imageFigure->getImageName(), "", tintColor, imageFigure->getTintAmount()));
}

void ImageFigureRenderer::createVisual(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cImageFigure *imageFigure = static_cast<cImageFigure *>(figure);
    SelfTransformingPixmapItem *imageItem = static_cast<SelfTransformingPixmapItem *>(item);

    imageItem->setTransformationMode(
        imageFigure->getInterpolation() == cFigure::INTERPOLATION_NONE
        ? Qt::FastTransformation : Qt::SmoothTransformation);

    imageItem->setOpacity(imageFigure->getOpacity());

    // to set the tinting and pixmap together
    setItemGeometryProperties(figure, item, hints);
}

void ImageFigureRenderer::refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cImageFigure *imageFigure = dynamic_cast<cImageFigure *>(figure);
    ASSERT(imageFigure);
    setImageTransform(imageFigure, item, figure->getTransform(),
                      imageFigure->getImageNaturalWidth(), imageFigure->getImageNaturalHeight(),
                      false, hints->zoom);
}

QGraphicsItem *ImageFigureRenderer::newItem()
{
    return new SelfTransformingPixmapItem();
}

void PixmapFigureRenderer::setItemGeometryProperties(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPixmapFigure *pixmapFigure = dynamic_cast<cPixmapFigure *>(figure);
    SelfTransformingPixmapItem *pixmapItem = dynamic_cast<SelfTransformingPixmapItem *>(item);
    ASSERT(pixmapFigure);
    ASSERT(pixmapItem);

    // Qt 5.2.0 is required for this, but 5.4.0 is our minimum anyways
    const cFigure::Pixmap& pixmap = pixmapFigure->getPixmap();
    QImage image(pixmap.buffer(), pixmap.getWidth(), pixmap.getHeight(), QImage::Format_RGBA8888);

    cFigure::Color color = pixmapFigure->getTintColor();
    QColor tintColor(color.red, color.green, color.blue);
    pixmapItem->setPixmap(getQtenv()->icons.makeTintedPixmap(&image, tintColor, pixmapFigure->getTintAmount()));
}

void PixmapFigureRenderer::refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cPixmapFigure *pixmapFigure = dynamic_cast<cPixmapFigure *>(figure);
    ASSERT(pixmapFigure);
    ASSERT(item);
    setImageTransform(pixmapFigure, item, figure->getTransform(),
                      pixmapFigure->getPixmapWidth(), pixmapFigure->getPixmapHeight(),
                      false, hints->zoom);
}

void IconFigureRenderer::refreshTransform(cFigure *figure, QGraphicsItem *item, FigureRenderingHints *hints)
{
    cIconFigure *iconFigure = dynamic_cast<cIconFigure *>(figure);
    ASSERT(iconFigure);
    ASSERT(item);

    item->setFlag(QGraphicsItem::ItemIgnoresTransformations);

    setImageTransform(iconFigure, item, figure->getTransform(),
                      iconFigure->getImageNaturalWidth(), iconFigure->getImageNaturalHeight(),
                      true, hints->zoom);
}

QGraphicsItem *GroupFigureRenderer::newItem() {
    return new GraphicsLayer();
}

}  // namespace qtenv
}  // namespace omnetpp

