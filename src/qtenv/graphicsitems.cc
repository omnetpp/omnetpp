//==========================================================================
//  graphicsitems.cc - part of
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

#include "graphicsitems.h"

#include <cmath>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

//---- ArrowheadItem implementation ----

void ArrowheadItem::updatePolygon()
{
    QPolygonF polygon;

    auto aside = arrowWidth * arrowSkew / 2;

    polygon.append(QPointF(0, 0));
    polygon.append(QPointF(-arrowLength, -arrowWidth / 2 + aside));
    polygon.append(QPointF(-arrowLength * fillRatio, aside));
    polygon.append(QPointF(-arrowLength, arrowWidth / 2 + aside));

    setPolygon(polygon);
}

ArrowheadItem::ArrowheadItem(QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    // uncomment this if you want the arrowheads to keep their shape
    // even if the line on which they are placed is heavily transformed:
    // (also see similar comments in figurerenderers.cc)
    // setFlags(QGraphicsItem::ItemIgnoresTransformations);
    setFillRule(Qt::WindingFill);
    updatePolygon();
    setPen(QPen(QColor("black"), 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    setData(ITEMDATA_COBJECT, parentItem()->data(ITEMDATA_COBJECT));
    setData(ITEMDATA_TOOLTIP, parentItem()->data(ITEMDATA_TOOLTIP));
}

QPainterPath ArrowheadItem::shape() const
{
    QPainterPath result;
    result.setFillRule(Qt::WindingFill);
    result.addPolygon(polygon());
    result.closeSubpath();

    QPainterPathStroker s;
    s.setCapStyle(pen().capStyle());
    s.setDashOffset(pen().dashOffset());
    s.setDashPattern(pen().dashPattern());
    s.setJoinStyle(pen().joinStyle());
    s.setMiterLimit(pen().miterLimit());
    s.setWidth(pen().widthF());

    result += s.createStroke(result);
    return result;
}

void ArrowheadItem::setEndPoints(const QPointF& start, const QPointF& end, double addAngle)
{
    setPos(end);
    setRotation(atan2(end.y() - start.y(), end.x() - start.x()) * 180.0 / M_PI + addAngle);
}

void ArrowheadItem::setFillRatio(double ratio)
{
    if (ratio != fillRatio) {
        fillRatio = ratio;
        updatePolygon();
    }
}

void ArrowheadItem::setColor(const QColor& color)
{
    auto p = pen();
    p.setColor(color);
    setPen(p);
    setBrush(color);
}

void ArrowheadItem::setLineWidth(double width)
{
    auto p = pen();
    p.setWidthF(width);
    setPen(p);
}

void ArrowheadItem::setArrowWidth(double width)
{
    if (width != arrowWidth) {
        arrowWidth = width;
        updatePolygon();
    }
}

void ArrowheadItem::setArrowLength(double length)
{
    if (length != arrowLength) {
        arrowLength = length;
        updatePolygon();
    }
}

void ArrowheadItem::setArrowSkew(double skew)
{
    if (skew != arrowSkew) {
        arrowSkew = skew;
        updatePolygon();
    }
}

void ArrowheadItem::setSizeForPenWidth(double penWidth, double scale, double addSize)
{
    double size = (1.25 * penWidth + addSize) * scale;
    setArrowWidth(size);
    setArrowLength(size);
}

//---- end of ArrowheadItem ----

//---- GraphicsLayer implementation ----

void GraphicsLayer::clear()
{
    while (!childItems().isEmpty())
        delete childItems()[0];
}

//---- end of GraphicsLayer ----

//---- ZoomLabel implementation ----

void ZoomLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QFont f = font();
    f.setBold(true);
    painter->setFont(f);
    painter->setBrush(Qt::lightGray);
    painter->setPen(Qt::lightGray);
    painter->drawRect(boundingRect());

    QGraphicsSimpleTextItem::paint(painter, option, widget);
}

void ZoomLabel::setZoomFactor(double zoomFactor)
{
    if (this->zoomFactor == zoomFactor)
        return;

    this->zoomFactor = zoomFactor;
    setText(" Zoom:" + QString::number(zoomFactor, 'f', 2) + "x");
    update();
}

//---- end of ZoomLabel ----

//---- ColorizeEffect implementation ----

void ColorizeEffect::setColor(const QColor& color)
{
    if (this->color != color) {
        this->color = color;
        changed = true;
        update();
    }
}

void ColorizeEffect::setWeight(double weight)
{
    if (this->weight != weight) {
        this->weight = weight;
        changed = true;
        update();
    }
}

void ColorizeEffect::setSmooth(double smooth)
{
    if (this->smooth != smooth) {
        this->smooth = smooth;
        smooth = true;
        update();
    }
}

void ColorizeEffect::draw(QPainter *painter)
{
    if (changed) {
        cachedImage = sourcePixmap(Qt::LogicalCoordinates, &offset, NoPad).toImage();

        if (weight > 0) {
            double rdest = color.redF();
            double gdest = color.greenF();
            double bdest = color.blueF();

            for (int y = 0; y < cachedImage.height(); y++) {
                QRgb *scanLine = reinterpret_cast<QRgb *>(cachedImage.scanLine(y));
                for (int x = 0; x < cachedImage.width(); x++) {
                    QRgb& pixel = scanLine[x];

                    int r = qRed(pixel);
                    int g = qGreen(pixel);
                    int b = qBlue(pixel);

                    // transform - code taken from tkenv/tkcmd.cc (colorizeImage_cmd)
                    int lum = (int)(0.2126*r + 0.7152*g + 0.0722*b);
                    r = (int)((1-weight)*r + weight*lum*rdest);
                    g = (int)((1-weight)*g + weight*lum*gdest);
                    b = (int)((1-weight)*b + weight*lum*bdest);

                    pixel = qRgba(r, g, b, qAlpha(pixel));
                }
            }
        }

        changed = false;
    }

    painter->save();
    painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth);
    painter->drawImage(cachedImage.rect().translated(offset), cachedImage);
    painter->restore();
}

void ColorizeEffect::sourceChanged(QGraphicsEffect::ChangeFlags flags)
{
    if (flags.testFlag(SourceAttached)
            || flags.testFlag(SourceDetached)
            || flags.testFlag(SourceBoundingRectChanged))
        changed = true;
    update();
}

//---- end of ColorizeEffect ----

//---- OutlinedTextItem implementation ----

OutlinedTextItem::OutlinedTextItem(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItem(parent)
{
    outlineItem = new QGraphicsSimpleTextItem();
    outlineItem->setFont(getQtenv()->getCanvasFont());
    fillItem = new QGraphicsSimpleTextItem();
    fillItem->setFont(getQtenv()->getCanvasFont());

    outlineItem->setBrush(Qt::NoBrush);
    QColor col = parseColor("grey82");
    col.setAlphaF(0.6);  // 4 pixels wide, so 2 pixels will go outwards
    outlineItem->setPen(QPen(col, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    fillItem->setPen(Qt::NoPen);
    // the default brush is good for fillItem

    // Makes scrolling the module (with many submodules) a lot faster while
    // not degrading performance, but using (hopefully just) a bit more memory.
    // Needed because text outline drawing is very slow in Qt (4.8).
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

OutlinedTextItem::~OutlinedTextItem()
{
    delete fillItem;
    delete outlineItem;
}

void OutlinedTextItem::setText(const QString& text)
{
    if (text != fillItem->text()) {
        fillItem->setText(text);
        outlineItem->setText(text);
        prepareGeometryChange();
        update();
    }
}

QRectF OutlinedTextItem::boundingRect() const
{
    QRectF rect = fillItem->boundingRect();
    double width = outlineItem->pen().widthF();
    rect.adjust(0, 0, width, width);
    return rect.translated(offset);
}

QRectF OutlinedTextItem::textRect() const
{
    return fillItem->boundingRect().translated(offset);
}

void OutlinedTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->setBrush(backgroundBrush);
    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());
    painter->restore();
    painter->save();
    double halfWidth = outlineItem->pen().widthF() / 2;
    painter->translate(halfWidth + offset.x(), halfWidth + offset.y());
    if (haloEnabled)
        outlineItem->paint(painter, option, widget);
    fillItem->paint(painter, option, widget);
    painter->restore();
}

void OutlinedTextItem::setFont(const QFont &font)
{
    outlineItem->setFont(font);
    fillItem->setFont(font);
    prepareGeometryChange();
    update();
}

void OutlinedTextItem::setPen(const QPen& pen)
{
    if (pen != outlineItem->pen()) {
        outlineItem->setPen(pen);
        prepareGeometryChange();
        update();
    }
}

void OutlinedTextItem::setBrush(const QBrush& brush)
{
    if (brush != fillItem->brush()) {
        fillItem->setBrush(brush);
        update();
    }
}

void OutlinedTextItem::setBackgroundBrush(const QBrush& brush)
{
    if (backgroundBrush != brush) {
        backgroundBrush = brush;
        update();
    }
}

void OutlinedTextItem::setOffset(const QPointF& offset)
{
    if (this->offset != offset) {
        this->offset = offset;
        prepareGeometryChange();
        update();
    }
}

void OutlinedTextItem::setHaloEnabled(bool enabled)
{
    if (enabled != haloEnabled) {
        haloEnabled = enabled;
        prepareGeometryChange();
        update();
    }
}

//---- end of OutlinedTextItem ----

//---- BubbleItem implementation ----

BubbleItem::BubbleItem(QPointF position, const QString& text, QGraphicsItem *parent)
    : QGraphicsObject(parent), text(text)
{
    setPos(position);
    startTimer(1000);
}

QRectF BubbleItem::boundingRect() const
{
    QFontMetrics metrics(getQtenv()->getCanvasFont());
    double textWidth = metrics.width(text);
    double textHeight = metrics.height();

    // the -1 and +2 is the line width, and also "just to be safe".
    return QRectF(-textWidth/2 - margin - 1,
                  -vertOffset - textHeight - margin - 1,
                  textWidth + margin*2 + 2,
                  margin + textHeight + vertOffset + 2);
}

void BubbleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QFontMetrics metrics(getQtenv()->getCanvasFont());

    double textWidth = metrics.width(text);
    double textHeight = metrics.height();

    if (!pathBuilt) {
        // will translate into the 4 corners
        QRectF arcRect(-margin, -margin, margin*2, margin*2);

        path.moveTo(0, 0);
        path.lineTo(0, -vertOffset + margin); // moving up (left edge of the triangle)
        path.arcTo(arcRect.translated(-textWidth/2, -vertOffset),               -90, -90); // lower left
        path.arcTo(arcRect.translated(-textWidth/2, -vertOffset - textHeight), -180, -90); // upper left
        path.arcTo(arcRect.translated( textWidth/2, -vertOffset - textHeight),   90, -90); // upper right
        path.arcTo(arcRect.translated( textWidth/2, -vertOffset),                 0, -90); // lower right
        path.lineTo(std::min(textWidth/2, vertOffset/2), -vertOffset + margin); // moving a bit to the left (the right half of the bottom edge)
        path.closeSubpath(); // the right edge of the triangle

        pathBuilt = true;
    }

    painter->save();
    painter->setPen(QPen(QColor("black"), 0));
    painter->setBrush(QColor(248, 248, 216));  // yellowish
    painter->drawPath(path);
    // uses the pen as color, also have to move up by the descent, to align the baseline
    painter->setFont(getQtenv()->getCanvasFont());
    painter->drawText(-textWidth/2, -vertOffset - metrics.descent(), text);
    painter->restore();
}

//---- end of BubbleItem ----

//---- SelfTransformingPixmapItem implementation ----

void SelfTransformingPixmapItem::setSelfTransform(const QTransform& transform)
{
    if (selfTransform != transform) {
        prepareGeometryChange(); // will call update() if necessary
        selfTransform = transform;
    }
}

QPainterPath SelfTransformingPixmapItem::shape() const
{
    return selfTransform.map(QGraphicsPixmapItem::shape());
}

QRectF SelfTransformingPixmapItem::boundingRect() const
{
    return selfTransform.mapRect(QGraphicsPixmapItem::boundingRect());
}

void SelfTransformingPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->setTransform(selfTransform, true);
    QGraphicsPixmapItem::paint(painter, option, widget);
    painter->restore();
}

//---- end of SelfTransformingPixmapItem ----

}  // namespace qtenv
}  // namespace omnetpp
