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
#include <QtWidgets/QGraphicsScene>
#include <QtCore/QDebug>
#include <QtWidgets/QGraphicsItem>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "omnetpp/platdep/platmisc.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {

Register_Class(NullRenderer);
Register_Class(GroupFigureRenderer);
Register_Class(PanelFigureRenderer);
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
    if (!rendererClassName.empty() && (rendererClassName.find(':') == std::string::npos))
        rendererClassName = "omnetpp::qtenv::" + rendererClassName;
    auto it = rendererCache.find(rendererClassName);
    if (it != rendererCache.end())
        renderer = it->second;
    else {
        // create renderer and add to the cache
        if (rendererClassName == "")
            renderer = new NullRenderer();
        else {
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

void FigureRenderer::clearRendererCache()
{
    for (auto p : rendererCache)
        delete p.second;
    rendererCache.clear();
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
    return fabs(x - y) < DBL_EPSILON; // XXX ?
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

void FigureRenderer::arcToUsingBezier(QPainterPath& path, double currentX, double currentY, double x, double y, double rx, double ry, double angle,
        bool largeArcFlag, bool sweepFlag, bool isRel)
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
            path.moveTo(endX, endY);
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
                path.cubicTo(x1+dx1, y1+dy1, xe+dxe, ye+dye, xe, ye);

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

// for the T path command
void FigureRenderer::calcSmoothQuadraticBezierCP(QPainterPath& path, char prevCommand, double currentX, double currentY,
                                        double& cpx, double& cpy, double x, double y, bool isRel)
{
    if (tolower(prevCommand) == 'q' || tolower(prevCommand) == 't') {
        // mirror cp to current
        cpx = currentX + (currentX - cpx);
        cpy = currentY + (currentY - cpy);
    }
    else {
        // if we can't continue, just draw a line
        cpx = currentX;
        cpy = currentY;
    }

    if (isRel)
        path.quadTo(cpx, cpy, x + currentX, y + currentY);
    else
        path.quadTo(cpx, cpy, x, y);
}

// for the S path command
void FigureRenderer::calcSmoothCubicBezierCP(QPainterPath& path, char prevCommand, double currentX, double currentY,
                                            double& prevCpx, double& prevCpy, double cpx, double cpy,
                                            double x, double y, bool isRel)
{
    if (tolower(prevCommand) == 's' || tolower(prevCommand) == 'c') {
        // mirror prevCp to current
        prevCpx = currentX + currentX - prevCpx;
        prevCpy = currentY + currentY - prevCpy;
    }
    else {
        // if we can't continue, use the current position as the first control point
        prevCpx = currentX;
        prevCpy = currentY;
    }

    if (isRel) {
        path.cubicTo(prevCpx, prevCpy, cpx + currentX, cpy + currentY, x + currentX, y + currentY);

        // storing the second control point for potential further continuations (s or S commands)
        prevCpx = cpx + currentX;
        prevCpy = cpy + currentY;
    }
    else {
        path.cubicTo(prevCpx, prevCpy, cpx, cpy, x, y);

        // storing the second control point for potential further continuations (s or S commands)
        prevCpx = cpx;
        prevCpy = cpy;
    }
}

QPen FigureRenderer::createPen(const FigureRenderingArgs& args)
{
    if (auto lineFigure = dynamic_cast<cAbstractLineFigure*>(args.figure)) {
        cFigure::Color color = lineFigure->getLineColor();
        QPen pen;
        pen.setColor(QColor(color.red, color.green, color.blue, lineFigure->getLineOpacity()*255));

        double width = lineFigure->getLineWidth();
        if (!lineFigure->getZoomLineWidth())
            width /= args.zoom;
        pen.setWidthF(width);

        Qt::PenStyle style;
        switch (lineFigure->getLineStyle()) {
            case cFigure::LINE_SOLID: style = Qt::SolidLine; break;
            case cFigure::LINE_DOTTED: style = Qt::DotLine; break;
            case cFigure::LINE_DASHED: style = Qt::DashLine; break;
        }
        pen.setStyle(style);

        Qt::PenCapStyle cap;
        switch (lineFigure->getCapStyle()) {
            case cFigure::CAP_BUTT: cap = Qt::FlatCap; break;
            case cFigure::CAP_SQUARE: cap = Qt::SquareCap; break;
            case cFigure::CAP_ROUND: cap = Qt::RoundCap; break;
        }
        pen.setCapStyle(cap);

        pen.setJoinStyle(Qt::MiterJoin);

        return pen;
    }


    if (auto shapeFigure = dynamic_cast<cAbstractShapeFigure*>(args.figure)) {
        if (shapeFigure->isOutlined()) {
            QPen pen;

            cFigure::Color color = shapeFigure->getLineColor();
            pen.setColor(QColor(color.red, color.green, color.blue, shapeFigure->getLineOpacity()*255));
            double width = shapeFigure->getLineWidth();
            if (!shapeFigure->getZoomLineWidth())
                width /= args.zoom;
            pen.setWidthF(width);

            Qt::PenStyle style;
            switch (shapeFigure->getLineStyle()) {
                case cFigure::LINE_SOLID: style = Qt::SolidLine; break;
                case cFigure::LINE_DOTTED: style = Qt::DotLine; break;
                case cFigure::LINE_DASHED: style = Qt::DashLine; break;
            }
            pen.setStyle(style);

            pen.setJoinStyle(Qt::MiterJoin);
            pen.setCapStyle(Qt::FlatCap);

            return pen;
        }
    }

    return Qt::NoPen;
}

QBrush FigureRenderer::createBrush(const cAbstractShapeFigure *figure)
{
    if (figure->isFilled()) {
        cFigure::Color color = figure->getFillColor();
        return QBrush(QColor(color.red, color.green, color.blue, figure->getFillOpacity()*255));  // setColor not working
    }
    else
        return Qt::NoBrush;
}

QPointF FigureRenderer::getAnchorOffset(cFigure::Anchor anchor, double width, double height, double ascent)
{
    QPointF offset;

    switch (anchor) {
        case cFigure::ANCHOR_CENTER:
        case cFigure::ANCHOR_N:
        case cFigure::ANCHOR_S:
        case cFigure::ANCHOR_BASELINE_MIDDLE:
            offset.setX(width / 2);
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
            offset.setY(height / 2);
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

QGraphicsItem *FigureRenderer::render(const FigureRenderingArgs& args)
{
    QGraphicsItem *item = newItem();

    if (!item)
        return nullptr;

    item->setData(ITEMDATA_COBJECT, QVariant::fromValue((cObject *)args.figure));
    for (auto c : item->childItems())
        if (c)
            c->setData(ITEMDATA_COBJECT, QVariant::fromValue((cObject *)args.figure));

    FigureRenderingArgs refreshArgs = args;
    refreshArgs.item = item;
    refresh(refreshArgs, 0xFF);

    return item;
}

void FigureRenderer::refresh(const FigureRenderingArgs& args, uint8_t what)
{
    if (!args.item)
        return;

    // We must not set the data with ITEMDATA_TOOLTIP role on figure items,
    // it will be gathered by makeObjectTooltip with special inheritance rules.

    // XXX: is the order important?
    if (what & cFigure::CHANGE_INPUTDATA)
        refreshInputData(args);
    if (what & cFigure::CHANGE_GEOMETRY)
        refreshGeometry(args);
    if (what & cFigure::CHANGE_VISUAL)
        refreshVisual(args);
    if (what & cFigure::CHANGE_TRANSFORM)
        refreshTransform(args);

    if (what & cFigure::CHANGE_ZINDEX)
        args.item->setZValue(args.figure->getEffectiveZIndex());
    if (args.zoomChanged)
        refreshZoom(args);

    // we don't care about CHANGE_OTHER, the tooltip and the associated object is
    // queried from the figure on demand, and is not stored in the items
}

void FigureRenderer::refreshTransform(const FigureRenderingArgs& args)
{
    args.item->setTransform(QTransform(
                           args.transform.a,  args.transform.b,  0,
                           args.transform.c,  args.transform.d,  0,
                           args.transform.t1, args.transform.t2, 1));
}

void FigureRenderer::refreshZoom(const FigureRenderingArgs& args)
{
    refreshTransform(args);
}

void AbstractShapeFigureRenderer::refreshVisual(const FigureRenderingArgs& args)
{
    cAbstractShapeFigure *shapeFigure = static_cast<cAbstractShapeFigure *>(args.figure);
    QAbstractGraphicsShapeItem *shapeItem = static_cast<QAbstractGraphicsShapeItem *>(args.item);
    shapeItem->setPen(createPen(args));
    shapeItem->setBrush(createBrush(shapeFigure));
}

void AbstractShapeFigureRenderer::refreshZoom(const FigureRenderingArgs& args)
{
    FigureRenderer::refreshZoom(args);
    // need to update the pen if linewidth is not zooming
    refreshVisual(args);
}

QGraphicsItem *AbstractShapeFigureRenderer::newItem()
{
    return new PathItem();
}

void ImageFigureRenderer::setImageTransform(const FigureRenderingArgs& args, double naturalWidth, double naturalHeight, bool translateOnly)
{
    cAbstractImageFigure *imageFigure = static_cast<cAbstractImageFigure *>(args.figure);

    double width = imageFigure->getWidth();
    if (width <= 0)
        width = naturalWidth;

    double height = imageFigure->getHeight();
    if (height <= 0)
        height = naturalHeight;

    cFigure::Point pos = imageFigure->getPosition();
    QPointF anchorOffset = getAnchorOffset(imageFigure->getAnchor(), width, height);

    QTransform trans;

    if (translateOnly) {
        pos = args.transform.applyTo(pos);
        pos.translate(anchorOffset.x(), anchorOffset.y()); // offset after transform
    } else {
        pos.translate(anchorOffset.x(), anchorOffset.y()); // offset before transform
        trans = QTransform(args.transform.a, args.transform.b, 0,
                           args.transform.c, args.transform.d, 0,
                           args.transform.t1, args.transform.t2, 1);
    }

    trans.translate(pos.x, pos.y);
    trans.scale(width / naturalWidth, height / naturalHeight);

    args.item->setTransform(trans);
}

void LineFigureRenderer::setArrows(const FigureRenderingArgs& args)
{
    cLineFigure *lineFigure = static_cast<cLineFigure *>(args.figure);
    QGraphicsPathItem *lineItem = static_cast<QGraphicsPathItem *>(args.item);

    ArrowheadItem *startArrow = static_cast<ArrowheadItem *>(lineItem->childItems()[0]);
    ArrowheadItem *endArrow = static_cast<ArrowheadItem *>(lineItem->childItems()[1]);

    const auto& start = lineFigure->getStart();
    const auto& end = lineFigure->getEnd();

    startArrow->setEndPoints(QPointF(end.x, end.y), QPointF(start.x, start.y));
    endArrow->setEndPoints(QPointF(start.x, start.y), QPointF(end.x, end.y));
}

void AbstractLineFigureRenderer::refreshVisual(const FigureRenderingArgs& args)
{
    cAbstractLineFigure *lineFigure = static_cast<cAbstractLineFigure *>(args.figure);
    QGraphicsPathItem *lineItem = static_cast<QGraphicsPathItem *>(args.item);

    lineItem->setPen(createPen(args));

    // setting up the arrowheads
    setArrows(args); // subclass-specific position and orientation

    // common stuff
    ArrowheadItem *startItem = static_cast<ArrowheadItem *>(lineItem->childItems()[0]);
    ArrowheadItem *endItem = static_cast<ArrowheadItem *>(lineItem->childItems()[1]);

    auto lineColor = lineFigure->getLineColor();
    QColor color(lineColor.red, lineColor.green, lineColor.blue, lineFigure->getLineOpacity() * 255);
    double penWidth = lineFigure->getLineWidth() / (lineFigure->getZoomLineWidth() ? 1.0 : args.zoom);

    // start arrowhead
    auto startStyle = lineFigure->getStartArrowhead();
    startItem->setVisible(startStyle != cFigure::ARROW_NONE);
    startItem->setFillRatio(startStyle == cFigure::ARROW_BARBED ? 0.6
                            : startStyle == cFigure::ARROW_SIMPLE ? 0
                            : 1);
    startItem->setColor(color);
    startItem->setLineWidth(penWidth);
    startItem->setSizeForPenWidth(penWidth, args.zoom);

    // end arrowhead
    auto endStyle = lineFigure->getEndArrowhead();
    endItem->setVisible(endStyle != cFigure::ARROW_NONE);
    endItem->setFillRatio(endStyle == cFigure::ARROW_BARBED ? 0.6
                            : endStyle == cFigure::ARROW_SIMPLE ? 0
                            : 1);
    endItem->setColor(color);
    endItem->setLineWidth(penWidth);
    endItem->setSizeForPenWidth(penWidth, args.zoom);
}

void AbstractLineFigureRenderer::refreshZoom(const FigureRenderingArgs& args)
{
    FigureRenderer::refreshZoom(args);
    // need to update the pen if linewidth is not zooming
    refreshVisual(args);
}

void LineFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    cLineFigure *lineFigure = static_cast<cLineFigure *>(args.figure);
    QGraphicsPathItem *lineItem = static_cast<QGraphicsPathItem *>(args.item);

    auto start = lineFigure->getStart();
    auto end = lineFigure->getEnd();

    QPainterPath path;
    path.moveTo(start.x, start.y);
    path.lineTo(end.x, end.y);
    lineItem->setPath(path);

    setArrows(args);
}

QGraphicsItem *LineFigureRenderer::newItem()
{
    // not using QGraphicsLineItem for the sake of consistency,
    // so refreshVisual can be common in AbstractLineFigureRenderer
    QGraphicsPathItem *lineItem = new QGraphicsPathItem();
    new ArrowheadItem(lineItem);
    new ArrowheadItem(lineItem);
    return lineItem;
}

void ArcFigureRenderer::setArrows(const FigureRenderingArgs& args)
{
    cArcFigure *arcFigure = static_cast<cArcFigure *>(args.figure);

    ArrowheadItem *startArrow = static_cast<ArrowheadItem *>(args.item->childItems()[0]);
    ArrowheadItem *endArrow = static_cast<ArrowheadItem *>(args.item->childItems()[1]);

    auto b = arcFigure->getBounds();
    QPointF center(b.getCenter().x, b.getCenter().y);
    QPointF radii(b.width / 2, b.height / 2);

    // in radians
    double startAngle = arcFigure->getStartAngle();
    double endAngle = arcFigure->getEndAngle();

    // this is from the center towards the start/end of the arc
    QPointF startDelta(cos(startAngle) * radii.x(), -sin(startAngle) * radii.y());
    QPointF endDelta(cos(endAngle) * radii.x(), -sin(endAngle) * radii.y());

    // this is the direction in which the arc begins/ends
    QPointF startTangent(-sin(startAngle) * radii.x(), -cos(startAngle) * radii.y());
    QPointF endTangent(-sin(endAngle) * radii.x(), -cos(endAngle) * radii.y());

    startArrow->setEndPoints(center + startDelta + startTangent, center + startDelta);
    endArrow->setEndPoints(center + endDelta - endTangent, center + endDelta);

    // use these two instead of the one above to make the arrowheads keep their
    // shapes even if the line they are on is transformed heavily (incomplete)
    // (also see the similar comment in graphicspatharrowitem.cc)
    //cFigure::Point transfDir = arcFigure->getTransform().applyTo(end) - arcFigure->getTransform().applyTo(start);
    //startArrow->setEndPoints(QPointF(end.x - transfDir.x, end.y - transfDir.y), QPointF(end.x, end.y));
}

void ArcFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    cArcFigure *arcFigure = static_cast<cArcFigure *>(args.figure);
    QGraphicsPathItem *arcItem = static_cast<QGraphicsPathItem *>(args.item);

    cFigure::Rectangle bounds = arcFigure->getBounds();
    // angles, converted to degrees
    double start = arcFigure->getStartAngle() * 180 / M_PI;
    double end = arcFigure->getEndAngle() * 180 / M_PI;

    QPainterPath path;
    path.arcMoveTo(bounds.x, bounds.y, bounds.width, bounds.height, start);
    path.arcTo(bounds.x, bounds.y, bounds.width, bounds.height, start, end - start);
    arcItem->setPath(path);
    setArrows(args);
}

QGraphicsItem *ArcFigureRenderer::newItem()
{
    QGraphicsPathItem *arcItem = new PathItem();
    new ArrowheadItem(arcItem);
    new ArrowheadItem(arcItem);
    return arcItem;
}

void PolylineFigureRenderer::setArrows(const FigureRenderingArgs& args)
{
    cPolylineFigure *polyFigure = static_cast<cPolylineFigure *>(args.figure);
    PathItem *polyItem = static_cast<PathItem *>(args.item);

    ArrowheadItem *startArrow = static_cast<ArrowheadItem *>(polyItem->childItems()[0]);
    ArrowheadItem *endArrow = static_cast<ArrowheadItem *>(polyItem->childItems()[1]);

    const auto& points = polyFigure->getPoints();

    if (points.size() < 2) {
        // can't display arrow on a polyline without at least two points
        startArrow->setVisible(false);
        endArrow->setVisible(false);
        return;
    }

    const auto& startFrom = points[1];
    const auto& startTo = points[0];
    startArrow->setEndPoints(QPointF(startFrom.x, startFrom.y), QPointF(startTo.x, startTo.y));

    const auto& endFrom = points[points.size() - 2];
    const auto& endTo = points[points.size() - 1];
    endArrow->setEndPoints(QPointF(endFrom.x, endFrom.y), QPointF(endTo.x, endTo.y));
}

void PolylineFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    cPolylineFigure *polyFigure = static_cast<cPolylineFigure *>(args.figure);
    PathItem *polyItem = static_cast<PathItem *>(args.item);

    QPainterPath path;

    std::vector<cFigure::Point> points = polyFigure->getPoints();
    if (points.size() < 2) {
        polyItem->setPath(path);
        return;  // throw cRuntimeError("PolylineFigureRenderer: points.size() < 2");
    }

    path.moveTo(points[0].x, points[0].y);

    if (points.size() == 2)
        path.lineTo(points[1].x, points[1].y);
    else if (polyFigure->getSmooth())
        for (size_t i = 2; i < points.size(); i++) {
            const cFigure::Point& control = points[i - 1];
            bool isLast = (i == points.size() - 1);
            cFigure::Point to = isLast ? points[i] : (points[i - 1] + points[i]) * 0.5;
            path.quadTo(control.x, control.y, to.x, to.y);
        }
    else
        for (size_t i = 1; i < points.size(); i++)
            path.lineTo(points[i].x, points[i].y);

    polyItem->setPath(path);

    setArrows(args);
}

void PolylineFigureRenderer::refreshVisual(const FigureRenderingArgs& args)
{
    AbstractLineFigureRenderer::refreshVisual(args);

    // overriding just the pen from the superclass method
    cPolylineFigure *polyFigure = static_cast<cPolylineFigure *>(args.figure);
    PathItem *polyItem = static_cast<PathItem *>(args.item);

    QPen pen = createPen(args);
    auto joinStyle = polyFigure->getJoinStyle();
    pen.setJoinStyle(joinStyle == cFigure::JOIN_BEVEL ? Qt::BevelJoin
                      : joinStyle == cFigure::JOIN_MITER ? Qt::MiterJoin
                      : Qt::RoundJoin);

    polyItem->setPen(pen);
}

QGraphicsItem *PolylineFigureRenderer::newItem()
{
    PathItem *polylineItem = new PathItem();
    new ArrowheadItem(polylineItem);
    new ArrowheadItem(polylineItem);
    return polylineItem;
}

void RectangleFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    cRectangleFigure *rectFigure = static_cast<cRectangleFigure *>(args.figure);
    QGraphicsPathItem *rectItem = static_cast<QGraphicsPathItem *>(args.item);

    cFigure::Rectangle bounds = rectFigure->getBounds();

    QPainterPath path;
    path.addRoundedRect(bounds.x, bounds.y, bounds.width, bounds.height,
                        rectFigure->getCornerRx(), rectFigure->getCornerRy());
    rectItem->setPath(path);
}

void OvalFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    cOvalFigure *ovalFigure = static_cast<cOvalFigure *>(args.figure);
    QGraphicsPathItem *ovalItem = static_cast<QGraphicsPathItem *>(args.item);

    cFigure::Rectangle bounds = ovalFigure->getBounds();

    QPainterPath path;
    path.addEllipse(bounds.x, bounds.y, bounds.width, bounds.height);
    ovalItem->setPath(path);
}

void RingFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    cRingFigure *ringFigure = static_cast<cRingFigure *>(args.figure);
    QGraphicsPathItem *ringItem = static_cast<QGraphicsPathItem *>(args.item);

    cFigure::Rectangle bounds = ringFigure->getBounds();
    cFigure::Point center = bounds.getCenter();
    double innerRx = ringFigure->getInnerRx();
    double innerRy = ringFigure->getInnerRy();

    QPainterPath path;
    path.addEllipse(bounds.x, bounds.y, bounds.width, bounds.height);
    // the default FillRule will make this second ellipse a hole
    path.addEllipse(center.x-innerRx, center.y-innerRy, 2*innerRx, 2*innerRy);
    ringItem->setPath(path);
}

void PieSliceFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    cPieSliceFigure *pieSliceFigure = static_cast<cPieSliceFigure *>(args.figure);
    QGraphicsPathItem *pieSliceItem = static_cast<QGraphicsPathItem *>(args.item);
    cFigure::Rectangle bounds = pieSliceFigure->getBounds();
    cFigure::Point center = bounds.getCenter();

    double start = pieSliceFigure->getStartAngle()*180/M_PI;
    double end = pieSliceFigure->getEndAngle()*180/M_PI;

    // This is here to move the spoke to the end angle if there is more than 1.0 pie, c:
    // but keep it at the start if there is less than -1.0 pie. :c
    start = std::max(start, end - 360);

    QPainterPath path;
    path.moveTo(center.x, center.y);
    path.arcTo(bounds.x, bounds.y, bounds.width, bounds.height, start, end - start);
    path.closeSubpath();
    pieSliceItem->setPath(path);
}

void PolygonFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    cPolygonFigure *polyFigure = static_cast<cPolygonFigure *>(args.figure);
    QGraphicsPathItem *polyItem = static_cast<QGraphicsPathItem *>(args.item);

    std::vector<cFigure::Point> points = polyFigure->getPoints();

    QPainterPath path;

    if (points.size() < 2) {
        polyItem->setPath(path);
        return;
    }  // throw cRuntimeError("PolylineFigureRenderer: points.size() < 2");

    if (points.size() == 2) {
        path.moveTo(points[0].x, points[0].y);
        path.lineTo(points[1].x, points[1].y);
    }
    else if (polyFigure->getSmooth()) {
        cFigure::Point start = (points[0] + points[1]) * 0.5;
        path.moveTo(start.x, start.y);

        for (size_t i = 0; i < points.size(); i++) {
            int i1 = (i + 1) % points.size();
            int i2 = (i + 2) % points.size();

            cFigure::Point control = points[i1];
            cFigure::Point to = (points[i1] + points[i2]) * 0.5;

            path.quadTo(control.x, control.y, to.x, to.y);
        }
    }
    else {
        path.moveTo(points[0].x, points[0].y);

        for (const auto& point : points)
            path.lineTo(point.x, point.y);
    }

    path.closeSubpath();

    polyItem->setPath(path);
}

void PolygonFigureRenderer::refreshVisual(const FigureRenderingArgs& args)
{
    AbstractShapeFigureRenderer::refreshVisual(args);

    cPolygonFigure *polyFigure = static_cast<cPolygonFigure *>(args.figure);
    QGraphicsPathItem *polyItem = static_cast<QGraphicsPathItem *>(args.item);

    QPainterPath path = polyItem->path();

    path.setFillRule(polyFigure->getFillRule() == cFigure::FILL_EVENODD
                         ? Qt::OddEvenFill
                         : Qt::WindingFill);

    polyItem->setPath(path);

    QPen pen = createPen(args);

    auto joinStyle = polyFigure->getJoinStyle();
    pen.setJoinStyle(joinStyle == cFigure::JOIN_BEVEL ? Qt::BevelJoin
                      : joinStyle == cFigure::JOIN_MITER ? Qt::MiterJoin
                      : Qt::RoundJoin);

    polyItem->setPen(pen);
    polyItem->setBrush(createBrush(polyFigure));
}

void PathFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    cPathFigure *pathFigure = static_cast<cPathFigure *>(args.figure);
    QGraphicsPathItem *pathItem = static_cast<QGraphicsPathItem *>(args.item);

    QPainterPath path;

    char prevCommand = ' ';
    cFigure::Point controlPoint;

    for (int i = 0; i < pathFigure->getNumPathItems(); ++i) {
        const cPathFigure::PathItem *command = pathFigure->getPathItem(i);
        QPointF pos = path.currentPosition();
        switch (command->code) {
            case 'M': {
                const cPathFigure::MoveTo *moveTo = static_cast<const cPathFigure::MoveTo *>(command);
                path.moveTo(moveTo->x, moveTo->y);
                break;
            }

            case 'm': {
                const cPathFigure::MoveRel *moveRel = static_cast<const cPathFigure::MoveRel *>(command);
                path.moveTo(pos.x() + moveRel->dx, pos.y() + moveRel->dy);
                break;
            }

            case 'L': {
                const cPathFigure::LineTo *lineTo = static_cast<const cPathFigure::LineTo *>(command);
                path.lineTo(lineTo->x, lineTo->y);
                break;
            }

            case 'l': {
                const cPathFigure::LineRel *lineRel = static_cast<const cPathFigure::LineRel *>(command);
                path.lineTo(pos.x() + lineRel->dx, pos.y() + lineRel->dy);
                break;
            }

            case 'H': {
                const cPathFigure::HorizontalLineTo *horizLineTo = static_cast<const cPathFigure::HorizontalLineTo *>(command);
                path.lineTo(horizLineTo->x, pos.y());
                break;
            }

            case 'h': {
                const cPathFigure::HorizontalLineRel *horizLineRel = static_cast<const cPathFigure::HorizontalLineRel *>(command);
                path.lineTo(pos.x() + horizLineRel->dx, pos.y());
                break;
            }

            case 'V': {
                const cPathFigure::VerticalLineTo *vertLineTo = static_cast<const cPathFigure::VerticalLineTo *>(command);
                path.lineTo(pos.x(), vertLineTo->y);
                break;
            }

            case 'v': {
                const cPathFigure::VerticalLineRel *vertLineRel = static_cast<const cPathFigure::VerticalLineRel *>(command);
                path.lineTo(pos.x(), pos.y() + vertLineRel->dy);
                break;
            }

            case 'A': {
                const cPathFigure::ArcTo *arcTo = static_cast<const cPathFigure::ArcTo *>(command);
                arcToUsingBezier(path, pos.x(), pos.y(), arcTo->x, arcTo->y, arcTo->rx, arcTo->ry,
                        arcTo->phi, arcTo->largeArc, arcTo->sweep, false);
                break;
            }

            case 'a': {
                const cPathFigure::ArcRel *arcRel = static_cast<const cPathFigure::ArcRel *>(command);
                arcToUsingBezier(path, pos.x(), pos.y(), arcRel->dx, arcRel->dy, arcRel->rx, arcRel->ry,
                        arcRel->phi, arcRel->largeArc, arcRel->sweep, true);
                break;
            }

            case 'Q': {
                const cPathFigure::CurveTo *curveTo = static_cast<const cPathFigure::CurveTo *>(command);
                controlPoint = cFigure::Point(curveTo->x1, curveTo->y1);
                path.quadTo(controlPoint.x, controlPoint.y, curveTo->x, curveTo->y);
                break;
            }

            case 'q': {
                const cPathFigure::CurveRel *curveRel = static_cast<const cPathFigure::CurveRel *>(command);
                QPointF pos = path.currentPosition();
                controlPoint = cFigure::Point(pos.x() + curveRel->dx1, pos.y() + curveRel->dy1);
                path.quadTo(controlPoint.x, controlPoint.y, pos.x() + curveRel->dx, pos.y() + curveRel->dy);
                break;
            }

            case 'T': {
                const cPathFigure::SmoothCurveTo *smoothCurveTo = static_cast<const cPathFigure::SmoothCurveTo *>(command);
                calcSmoothQuadraticBezierCP(path, prevCommand, pos.x(), pos.y(), controlPoint.x, controlPoint.y,
                        smoothCurveTo->x, smoothCurveTo->y);
                break;
            }

            case 't': {
                const cPathFigure::SmoothCurveRel *smoothCurveRel = static_cast<const cPathFigure::SmoothCurveRel *>(command);
                calcSmoothQuadraticBezierCP(path, prevCommand, pos.x(), pos.y(), controlPoint.x, controlPoint.y,
                        smoothCurveRel->dx, smoothCurveRel->dy, true);
                break;
            }

            case 'C': {
                const cPathFigure::CubicBezierCurveTo *bezierTo = static_cast<const cPathFigure::CubicBezierCurveTo *>(command);
                controlPoint = cFigure::Point(bezierTo->x2, bezierTo->y2);
                path.cubicTo(bezierTo->x1, bezierTo->y1, bezierTo->x2, bezierTo->y2, bezierTo->x,
                        bezierTo->y);
                break;
            }

            case 'c': {
                const cPathFigure::CubicBezierCurveRel *bezierRel = static_cast<const cPathFigure::CubicBezierCurveRel *>(command);
                QPointF controlPoint1(pos.x() + bezierRel->dx1, pos.y() + bezierRel->dy1);
                QPointF controlPoint2(pos.x() + bezierRel->dx2, pos.y() + bezierRel->dy2);
                QPointF endPoint(pos.x() + bezierRel->dx, pos.y() + bezierRel->dy);
                controlPoint = cFigure::Point(controlPoint2.x(), controlPoint2.y());
                path.cubicTo(controlPoint1, controlPoint2, endPoint);
                break;
            }

            case 'S': {
                const cPathFigure::SmoothCubicBezierCurveTo *sBezierTo = static_cast<const cPathFigure::SmoothCubicBezierCurveTo *>(command);
                calcSmoothCubicBezierCP(path, prevCommand, pos.x(), pos.y(), controlPoint.x, controlPoint.y,
                        sBezierTo->x2, sBezierTo->y2, sBezierTo->x, sBezierTo->y);
                break;
            }

            case 's': {
                const cPathFigure::SmoothCubicBezierCurveRel *sBezierRel = static_cast<const cPathFigure::SmoothCubicBezierCurveRel *>(command);
                calcSmoothCubicBezierCP(path, prevCommand, pos.x(), pos.y(), controlPoint.x, controlPoint.y,
                        sBezierRel->dx2, sBezierRel->dy2, sBezierRel->dx, sBezierRel->dy, true);
                break;
            }

            case 'Z': {
                path.closeSubpath();
                break;
            }
        }
        prevCommand = command->code;
    }

    auto o = pathFigure->getOffset();
    path.translate(o.x, o.y);

    pathItem->setPath(path);
}

void PathFigureRenderer::refreshVisual(const FigureRenderingArgs& args)
{
    AbstractShapeFigureRenderer::refreshVisual(args);

    // overriding the pen and the fillrule from the superclass method
    cPathFigure *pathFigure = static_cast<cPathFigure *>(args.figure);
    QGraphicsPathItem *shapeItem = static_cast<QGraphicsPathItem *>(args.item);

    QPainterPath path = shapeItem->path();
    path.setFillRule(pathFigure->getFillRule() == cFigure::FILL_EVENODD
                     ? Qt::OddEvenFill : Qt::WindingFill);
    shapeItem->setPath(path);

    QPen pen = createPen(args);

    auto joinStyle = pathFigure->getJoinStyle();
    pen.setJoinStyle(joinStyle == cFigure::JOIN_BEVEL ? Qt::BevelJoin
                      : joinStyle == cFigure::JOIN_MITER ? Qt::MiterJoin
                      : Qt::RoundJoin);

    auto capStyle = pathFigure->getCapStyle();
    pen.setCapStyle(capStyle == cFigure::CAP_ROUND ? Qt::RoundCap
                      : capStyle == cFigure::CAP_SQUARE ? Qt::SquareCap
                      : Qt::FlatCap);

    shapeItem->setPen(pen);
}

void TextFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    cAbstractTextFigure *textFigure = static_cast<cAbstractTextFigure *>(args.figure);
    MultiLineOutlinedTextItem *textItem = static_cast<MultiLineOutlinedTextItem *>(args.item);

    QFontMetricsF fontMetric(textItem->font());
    QRectF textRect = textItem->textRect();

    QPointF offset = getAnchorOffset(textFigure->getAnchor(), textRect.width(), textRect.height(), fontMetric.ascent());

    // Since setPos is in the parent coordinate system (parent item, which
    // is the scene itself in this case, and not parent figure), it is not
    // affected by this item's transform. But we need that, so we
    // transform the position by hand to move it to the right place.
    // This is not an issue on any other kind of figure (rect, ring, etc),
    // because those are drawn using a path that is at the right place,
    // and pos is always kept at origin.
    // An other way of doing this (similar to other figures) would be to
    // add an offset to MultiLineOutlinedTextItem, but that might not work
    // well (I mean fast) with DeviceCoordinateCache.
    auto op = args.transform.applyTo(cFigure::Point(0, 0));
    auto pp = args.transform.applyTo(textFigure->getPosition() + cFigure::Point(offset.x(), offset.y()));

    textItem->setPos(pp.x - op.x, pp.y - op.y);

    cFigure::Alignment alignment = textFigure->getAlignment();
    textItem->setAlignment(alignment == cFigure::ALIGN_RIGHT ? Qt::AlignRight
                         : alignment == cFigure::ALIGN_CENTER ? Qt::AlignCenter
                         : Qt::AlignLeft);
}

void TextFigureRenderer::refreshInputData(const FigureRenderingArgs& args)
{
    cAbstractTextFigure *textFigure = static_cast<cAbstractTextFigure *>(args.figure);
    MultiLineOutlinedTextItem *textItem = static_cast<MultiLineOutlinedTextItem *>(args.item);

    textItem->setText(QString::fromUtf8(textFigure->getText()));

    // the bounding box might have changed, so we might have to realign
    refreshGeometry(args);
}

void TextFigureRenderer::refreshVisual(const FigureRenderingArgs& args)
{
    cAbstractTextFigure *textFigure = static_cast<cAbstractTextFigure *>(args.figure);
    MultiLineOutlinedTextItem *textItem = static_cast<MultiLineOutlinedTextItem *>(args.item);

    cFigure::Font font = textFigure->getFont();

    std::string typeface = font.typeface;
    if (typeface.empty())
        typeface = args.hints->defaultFont;

    int pointSize = font.pointSize;
    if (pointSize <= 0)
        pointSize = args.hints->defaultFontSize;

    QFont qFont(typeface.c_str(), pointSize);

    qFont.setBold(font.style & cFigure::FONT_BOLD);
    qFont.setItalic(font.style & cFigure::FONT_ITALIC);
    qFont.setUnderline(font.style & cFigure::FONT_UNDERLINE);

    textItem->setFont(qFont);

    cFigure::Color color = textFigure->getColor();
    textItem->setBrush(QColor(color.red, color.green, color.blue));
    textItem->setOpacity(textFigure->getOpacity());
    textItem->setHaloEnabled(textFigure->getHalo());

    // since the font might have changed, we must update the position because anchoring
    refreshGeometry(args);
}

void TextFigureRenderer::refreshTransform(const FigureRenderingArgs& args)
{
    FigureRenderer::refreshTransform(args);
    // we also need to adjust item pos, see the comment in refreshGeometry()
    refreshGeometry(args);
}

void TextFigureRenderer::refreshZoom(const FigureRenderingArgs& args)
{
    FigureRenderer::refreshZoom(args);

    MultiLineOutlinedTextItem *textItem = static_cast<MultiLineOutlinedTextItem *>(args.item);
    QColor col = parseColor("grey82");
    col.setAlphaF(0.6);  // 4 pixels wide, so 2 pixels will go outwards
    textItem->setPen(QPen(col, 4.0 / args.zoom, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

QGraphicsItem *TextFigureRenderer::newItem()
{
    return new MultiLineOutlinedTextItem();
}

void LabelFigureRenderer::refreshTransform(const FigureRenderingArgs& args)
{
    // Not calling the superclass transform, as the label is not transforming.
    // However, we need to update the geometry, see the comment in
    // TextFigureRenderer::refreshGeometry for the reason.
    refreshGeometry(args);
}

void LabelFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    MultiLineOutlinedTextItem *textItem = static_cast<MultiLineOutlinedTextItem *>(args.item);
    cLabelFigure *labelFigure = static_cast<cLabelFigure *>(args.figure);

    // see the comment in TextFigureRenderer::refreshGeometry
    cFigure::Point pos = args.transform.applyTo(labelFigure->getPosition());

    QFontMetricsF fontMetric(textItem->font());
    QRectF textRect = textItem->textRect();
    QPointF offset = getAnchorOffset(labelFigure->getAnchor(), textRect.width(), textRect.height(), fontMetric.ascent());

    textItem->setPos(QPointF(pos.x, pos.y) + offset);
    textItem->setRotation(-labelFigure->getAngle()/M_PI*180);
    textItem->setTransformOriginPoint(-offset);

    cFigure::Alignment alignment = labelFigure->getAlignment();
    textItem->setAlignment(alignment == cFigure::ALIGN_RIGHT ? Qt::AlignRight
                         : alignment == cFigure::ALIGN_CENTER ? Qt::AlignCenter
                         : Qt::AlignLeft);
}

void LabelFigureRenderer::refreshZoom(const FigureRenderingArgs& args)
{
    // Skipping TextFigureRenderer::refreshZoom(), we don't need to adjust the pen width.
    FigureRenderer::refreshZoom(args);
}

void ImageFigureRenderer::refreshGeometry(const FigureRenderingArgs& args)
{
    // position is incorporated to the transform
    // XXX performance with many moving images? DeviceCoordinateCache maybe?
    refreshTransform(args);
}

void ImageFigureRenderer::refreshInputData(const FigureRenderingArgs& args)
{
    cImageFigure *imageFigure = static_cast<cImageFigure *>(args.figure);
    QGraphicsPixmapItem *imageItem = static_cast<QGraphicsPixmapItem *>(args.item);

    cFigure::Color color = imageFigure->getTintColor();
    QColor tintColor(color.red, color.green, color.blue);
    imageItem->setPixmap(getQtenv()->icons.getTintedPixmap(imageFigure->getImageName(), "", tintColor, imageFigure->getTintAmount()));

    // size might have changed, so we might need to adjust the anchoring
    refreshGeometry(args);
}

void ImageFigureRenderer::refreshVisual(const FigureRenderingArgs& args)
{
    cImageFigure *imageFigure = static_cast<cImageFigure *>(args.figure);
    QGraphicsPixmapItem *imageItem = static_cast<QGraphicsPixmapItem *>(args.item);

    imageItem->setTransformationMode(
        imageFigure->getInterpolation() == cFigure::INTERPOLATION_NONE
        ? Qt::FastTransformation : Qt::SmoothTransformation);

    imageItem->setOpacity(imageFigure->getOpacity());

    // to set the tinting and pixmap together
    refreshInputData(args);
}

void ImageFigureRenderer::refreshTransform(const FigureRenderingArgs& args)
{
    cImageFigure *imageFigure = static_cast<cImageFigure *>(args.figure);
    setImageTransform(args, imageFigure->getImageNaturalWidth(), imageFigure->getImageNaturalHeight(), false);
}

QGraphicsItem *ImageFigureRenderer::newItem()
{
    return new QGraphicsPixmapItem();
}

void PixmapFigureRenderer::refreshInputData(const FigureRenderingArgs& args)
{
    // not calling superclass, different behavior
    cPixmapFigure *pixmapFigure = static_cast<cPixmapFigure *>(args.figure);
    QGraphicsPixmapItem *pixmapItem = static_cast<QGraphicsPixmapItem *>(args.item);

    const cFigure::Pixmap& pixmap = pixmapFigure->getPixmap();
    QImage image(pixmap.buffer(), pixmap.getWidth(), pixmap.getHeight(), QImage::Format_RGBA8888);

    cFigure::Color color = pixmapFigure->getTintColor();
    QColor tintColor(color.red, color.green, color.blue);

    pixmapItem->setPixmap(getQtenv()->icons.makeTintedPixmap(&image, tintColor, pixmapFigure->getTintAmount()));

    // size might have changed, so we might need to adjust the anchoring
    refreshGeometry(args);
}

void PixmapFigureRenderer::refreshTransform(const FigureRenderingArgs& args)
{
    // not calling superclass, different behavior
    cPixmapFigure *pixmapFigure = static_cast<cPixmapFigure *>(args.figure);
    setImageTransform(args, pixmapFigure->getPixmapWidth(), pixmapFigure->getPixmapHeight(), false);
}

void IconFigureRenderer::refreshTransform(const FigureRenderingArgs& args)
{
    // not calling superclass, different behavior
    cIconFigure *iconFigure = static_cast<cIconFigure *>(args.figure);
    setImageTransform(args, iconFigure->getImageNaturalWidth(), iconFigure->getImageNaturalHeight(), true);
}

}  // namespace qtenv
}  // namespace omnetpp

