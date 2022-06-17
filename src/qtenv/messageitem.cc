//==========================================================================
//  MESSAGEITEM.CC - part of
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

#include "messageitem.h"
#include <QtCore/QDebug>
#include <QtGui/QPen>
#include "qtenv.h"
#include "graphicsitems.h"
#include "displaystringaccess.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cpacket.h"
#include "common/stringutil.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {


// -------- MessageItemUtil --------

// these colors were chosen to be consistent with the tool and object icons
const QVector<QColor> MessageItemUtil::msgKindColors = {"#C00000", "#379143", "#6060ff", "#f0f0f0", "#c0c000", "#00c0c0", "#c000c0", "#404040"};

void MessageItemUtil::setupMessageCommon(MessageItem *mi, cMessage *msg)
{
    QtenvOptions *opt = getQtenv()->opt;

    QString label;
    if (opt->animationMsgClassNames)
        label += QString("(") + getObjectShortTypeName(msg) + ")";
    if (opt->animationMsgNames)
        label += msg->getFullName();
    mi->setText(label);
}

void MessageItemUtil::setupLineFromDisplayString(LineMessageItem *mi, cMessage *msg)
{
    cDisplayString ds = msg->getDisplayString();

    std::string buffer; // stores getTagArg return values after substitution
    DisplayStringAccess dsa(&ds, nullptr);

    mi->setData(ITEMDATA_COBJECT, QVariant::fromValue((cObject *)msg));

    QtenvOptions *opt = getQtenv()->opt;

    setupMessageCommon(mi, msg);

    QColor kindColor = msgKindColors[msg->getKind() % msgKindColors.size()];

    if (!ds.str()[0]) {  // default red line
        QColor color = opt->animationMsgColors ? kindColor : "red";
        mi->setColor(color);
    }
    else {  // as defined in the dispstr
        const char *fillColorName = dsa.getTagArg("b", 3, buffer);
        QColor fillColor = opp_streq(fillColorName, "kind")
                            ? kindColor
                            : parseColor(fillColorName, "red");
        mi->setColor(fillColor);
    }
}

void MessageItemUtil::setupSymbolFromDisplayString(SymbolMessageItem *mi, cMessage *msg, double imageSizeFactor)
{
    cDisplayString ds = msg->getDisplayString();

    std::string buffer; // stores getTagArg return values after substitution
    DisplayStringAccess dsa(&ds, nullptr);

    mi->setData(ITEMDATA_COBJECT, QVariant::fromValue((cObject *)msg));

    QtenvOptions *opt = getQtenv()->opt;

    setupMessageCommon(mi, msg);

    if (!ds.str()[0] && opt->penguinMode)
        ds = "i=abstract/penguin_s";  // ^^,

    // no zoom factor, it doesn't affect messages
    mi->setImageSizeFactor(imageSizeFactor);

    QColor kindColor = MessageItemUtil::getColorForMessageKind(msg->getKind());

    if (!ds.str()[0]) {  // default little red dot
        QColor color = opt->animationMsgColors ? kindColor : "red";
        mi->setShape(SymbolMessageItem::SHAPE_OVAL);
        mi->setWidth(10);
        mi->setHeight(10);
        mi->setFillColor(color);
        mi->setOutlineColor(color);
        mi->setImage(nullptr);
    }
    else {  // as defined in the dispstr
        bool widthOk, heightOk;
        double shapeWidth = dsa.getTagArgAsDouble("b", 0, 0.0, &widthOk);
        double shapeHeight = dsa.getTagArgAsDouble("b", 1, 0.0, &heightOk);

        if (!widthOk)
            shapeWidth = shapeHeight;
        if (!heightOk)
            shapeHeight = shapeWidth;
        if (!widthOk && !heightOk) {
            shapeWidth = 10;
            shapeHeight = 10;
        }

        mi->setWidth(shapeWidth);
        mi->setHeight(shapeHeight);

        QString shapeName = QString(dsa.getTagArg("b", 2, buffer)).toLower();

        auto shape = (ds.getNumArgs("b") <= 0)
                       ? SymbolMessageItem::SHAPE_NONE
                       : shapeName[0] == 'r'
                           ? SymbolMessageItem::SHAPE_RECT
                           : SymbolMessageItem::SHAPE_OVAL; // if unknown, this is the default
        mi->setShape(shape);

        const char *fillColorName = dsa.getTagArg("b", 3, buffer);
        QColor fillColor = opp_streq(fillColorName, "kind")
                            ? kindColor
                            : parseColor(fillColorName, "red");
        mi->setFillColor(fillColor);
        const char *outlineColorName = dsa.getTagArg("b", 4, buffer);
        mi->setOutlineColor(opp_streq(outlineColorName, "kind")
                             ? kindColor
                             : parseColor(outlineColorName, fillColor));

        mi->setOutlineWidth(dsa.getTagArgAsLong("b", 5, 2));

        const char *imageName = dsa.getTagArg("i", 0, buffer);
        std::string buffer2; // for imageSize while imageName is still needed
        mi->setImage(imageName[0] ? getQtenv()->icons.getImage(imageName, dsa.getTagArg("is", 0, buffer2)) : nullptr);

        const char *imageColorName = dsa.getTagArg("i", 1, buffer);
        mi->setImageColor(opp_streq(imageColorName, "kind") ? kindColor : parseColor(imageColorName));
        mi->setImageColorPercentage((ds.getNumArgs("i") == 2) // color given, but no percentage
                                      ? 30
                                      : dsa.getTagArgAsDouble("i", 2));
    }
}

// -------- MessageItemUtil --------

const QColor& MessageItemUtil::getColorForMessageKind(int messageKind)
{
    int colorIndex = messageKind % msgKindColors.size();
    if (colorIndex < 0)
        colorIndex += msgKindColors.size();
    return msgKindColors[colorIndex];
}

// -------- MessageItem --------

MessageItem::MessageItem(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    textItem = new OutlinedTextItem(this);
    textItem->setZValue(1);
}

MessageItem::~MessageItem()
{
    delete textItem;
}

void MessageItem::setText(const QString& text)
{
    if (this->text != text) {
        this->text = text;
        updateTextItem();
    }
}

void MessageItem::updateTextItem()
{
    textItem->setText(text);
    QRectF textRect = textItem->textRect();
    QPointF textPos = getTextPosition();
    textItem->setPos(textPos.x() - textRect.width() / 2, textPos.y());
}


// -------- LineMessageItem --------


LineMessageItem::LineMessageItem(QGraphicsItem *parent) : MessageItem(parent)
{
    lineItem = new QGraphicsLineItem(this);
    txUpdateMarkerItem = new QGraphicsLineItem(this);
    arrowheadItem = new ArrowheadItem(this);

    arrowheadItem->setFillRatio(1);
    arrowheadItem->setPen(Qt::NoPen);

    txUpdateMarkerItem->setZValue(0.5);
}

LineMessageItem::~LineMessageItem()
{
    delete lineItem;
    delete txUpdateMarkerItem;
    delete arrowheadItem;
}

void LineMessageItem::setColor(const QColor& color)
{
    if (this->color != color) {
        this->color = color;
        updateLineItem();
    }
}

void LineMessageItem::setLineEnabled(bool enabled)
{
    if (lineEnabled != enabled) {
        lineEnabled = enabled;
        updateLineItem();
    }
}

void LineMessageItem::setArrowheadEnabled(bool enabled)
{
    if (arrowheadEnabled != enabled) {
        arrowheadEnabled = enabled;
        updateLineItem();
    }
}

void LineMessageItem::setTxUpdateMarkerEnabled(bool enabled)
{
    if (txUpdateMarkerEnabled != enabled) {
        txUpdateMarkerEnabled = enabled;
        updateLineItem();
    }
}

QPointF LineMessageItem::getTextPosition()
{
    QPointF offset = getSideOffsetForWidth((txUpdateMarkerEnabled) ? 12 : 6);
    return offset + QPointF(0, 3);
}

void LineMessageItem::positionOntoLine(const QLineF& line, double t1, double t2, bool asUpdatePacket)
{
    QLineF packetLine(line.pointAt(t1), line.pointAt(t2));

    bool lineEnabled = true;

    if (t1 == t2 && asUpdatePacket) {// we are marker-only
        packetLine = QLineF(line.pointAt(t1), line.pointAt(t1-0.001));
        lineEnabled = false; // the packetLine is fake
    }

    setLine(packetLine);
    setLineEnabled(lineEnabled);
    setArrowheadEnabled(t1 > 0.0 && t1 < 1.0 && !asUpdatePacket);
    setTxUpdateMarkerEnabled(t1 > 0.0 && t1 < 1.0 && asUpdatePacket);
}

QRectF LineMessageItem::boundingRect() const
{
    QRectF result;
    if (lineEnabled)
        result = result.united(lineItem->boundingRect());
    if (arrowheadEnabled)
        result = result.united(arrowheadItem->boundingRect());
    if (txUpdateMarkerEnabled)
        result = result.united(txUpdateMarkerItem->boundingRect());
    return result;
}

QPainterPath LineMessageItem::shape() const
{
    QPainterPath result;
    if (lineEnabled)
        result = result.united(lineItem->shape());
    if (arrowheadEnabled)
        result = result.united(arrowheadItem->shape());
    if (txUpdateMarkerEnabled)
        result = result.united(txUpdateMarkerItem->shape());
    return result;
}

void LineMessageItem::setLine(const QLineF& line)
{
    if (this->line != line) {
        this->line = line;
        setPos(line.pointAt(0.5));
        updateLineItem();
        updateTextItem();
    }
}

QPointF LineMessageItem::getSideOffsetForWidth(float width) const
{
    return (line.isNull() || std::isnan(line.x1() + line.y1() + line.x2() + line.y2()))
        ? QPointF(0, 0)
        : ((width / 2 + 2) * line.normalVector().unitVector().translated(-line.p1()).p2());
}

void LineMessageItem::updateLineItem()
{
    setPos(line.pointAt(0.5));

    double width = 6;

    // to make it go on the right side of the connection, with a bit of spacing
    QPointF sideOffset = getSideOffsetForWidth(width);

    QPen pen(color, width, Qt::SolidLine, Qt::FlatCap);

    // relative to pos, center is always in origin
    auto localLine = line.translated(-pos());
    localLine = QLineF(localLine.p2(), localLine.p1());

    double arrowheadLength = arrowheadEnabled ? width : 0;

    // don't let it get too short (1 px for the line, 1 for the arrowhead, why not...)
    if (localLine.length() < 2) {
        localLine.setLength(2);
        localLine.translate(-localLine.pointAt(0.5));
    }

    if (std::isnan(localLine.x1() + localLine.y1() + localLine.x2() + localLine.y2()))
        localLine = QLineF(0, 0, 0, 0);

    arrowheadLength = std::min(arrowheadLength, localLine.length()/2);

    localLine = localLine.translated(sideOffset);
    arrowheadItem->setEndPoints(localLine.p1(), localLine.p2());
    localLine.setLength(localLine.length()-arrowheadLength);

    lineItem->setLine(localLine);
    lineItem->setVisible(lineEnabled);
    lineItem->setPen(pen);

    arrowheadItem->setBrush(color);
    arrowheadItem->setArrowWidth(width);
    arrowheadItem->setArrowLength(arrowheadLength + 0.5); // +0.5 is just to make it "watertight" (AA and imprecision and stuff)

    arrowheadItem->setVisible(arrowheadEnabled);

    txUpdateMarkerItem->setLine(QLineF(localLine.p2() + sideOffset * 1.5, localLine.p2() - sideOffset * 1.5));
    txUpdateMarkerItem->setVisible(txUpdateMarkerEnabled);
    pen.setWidth(width / 2);
    txUpdateMarkerItem->setPen(pen);
}


// -------- SymbolMessageItem --------


SymbolMessageItem::SymbolMessageItem(QGraphicsItem *parent) : MessageItem(parent)
{
    shapeItem = new QGraphicsEllipseItem(this);
    imageItem = new QGraphicsPixmapItem(this);

    updateShapeItem();
}

SymbolMessageItem::~SymbolMessageItem()
{
    delete shapeItem;
    delete imageItem;
}

void SymbolMessageItem::setImageSizeFactor(double imageSize)
{
    if (imageSizeFactor != imageSize) {
        imageSizeFactor = imageSize;
        updateImageItem();
    }
}

void SymbolMessageItem::setShape(Shape shape)
{
    if (this->shapeType != shape) {
        this->shapeType = shape;
        updateShapeItem();
    }
}

void SymbolMessageItem::setWidth(double width)
{
    if (shapeWidth != width) {
        shapeWidth = width;
        updateShapeItem();
    }
}

void SymbolMessageItem::setHeight(double height)
{
    if (shapeHeight != height) {
        shapeHeight = height;
        updateShapeItem();
    }
}

void SymbolMessageItem::setFillColor(const QColor& color)
{
    if (this->shapeFillColor != color) {
        this->shapeFillColor = color;
        updateShapeItem();
    }
}

void SymbolMessageItem::setOutlineColor(const QColor& color)
{
    if (this->shapeOutlineColor != color) {
        this->shapeOutlineColor = color;
        updateShapeItem();
    }
}

void SymbolMessageItem::setOutlineWidth(double width)
{
    if (shapeOutlineWidth != width) {
        shapeOutlineWidth = width;
        updateShapeItem();
    }
}

void SymbolMessageItem::setImage(QImage *image)
{
    if (this->image != image) {
        this->image = image;
        updateImageItem();
        updateTextItem();
    }
}

void SymbolMessageItem::setImageColor(const QColor& color)
{
    if (colorizeEffect) {
        colorizeEffect->setColor(color);
    }
}

void SymbolMessageItem::setImageColorPercentage(int percent)
{
    if (colorizeEffect) {
        colorizeEffect->setStrength(percent / 100.0f);
    }
}

QPointF SymbolMessageItem::getTextPosition()
{
    return QPointF(0, shapeImageBoundingRect().bottom());
}

void SymbolMessageItem::positionOntoLine(const QLineF& line, double t1, double t2, bool asUpdatePacket)
{
    ASSERT(t1 == t2);
    ASSERT(!asUpdatePacket);

    setPos(line.pointAt(t1));
}

QRectF SymbolMessageItem::boundingRect() const
{
    return shapeImageBoundingRect();
}

QPainterPath SymbolMessageItem::shape() const
{
    QPainterPath path;
    path.addRect(shapeImageBoundingRect());
    return path;
}

QRectF SymbolMessageItem::shapeImageBoundingRect() const
{
    QRectF rect;
    if (imageItem) {
        QRectF imageRect = imageItem->boundingRect();
        // Image scaling is done with a transformation, and boundingRect does
        // not factor that in, so we have to account the factor in here.
        imageRect.setTopLeft(imageRect.topLeft() * imageSizeFactor);
        imageRect.setBottomRight(imageRect.bottomRight() * imageSizeFactor);
        rect = rect.united(imageRect);
    }
    if (shapeItem) {
        QRectF shapeRect = shapeItem->boundingRect();
        // Shape size is not zoomed in messages;
        rect = rect.united(shapeRect);
    }
    return rect;
}

void SymbolMessageItem::updateShapeItem()
{
    delete shapeItem;
    shapeItem = nullptr;

    QRectF rect(-shapeWidth / 2.0, -shapeHeight / 2.0, shapeWidth, shapeHeight);
    rect.setTopLeft(rect.topLeft());
    rect.setBottomRight(rect.bottomRight());
    rect = rect.adjusted(shapeOutlineWidth / 2, shapeOutlineWidth / 2, -shapeOutlineWidth / 2, -shapeOutlineWidth / 2);

    switch (shapeType) {
        case SHAPE_OVAL: shapeItem = new QGraphicsEllipseItem(rect, this); break;
        case SHAPE_RECT: shapeItem = new QGraphicsRectItem(rect, this); break;
        default: break; // nothing
    }

    if (shapeItem) {
        shapeItem->setBrush(shapeFillColor.isValid() ? shapeFillColor : Qt::NoBrush);
        shapeItem->setPen(shapeOutlineColor.isValid()
                           ? QPen(shapeOutlineColor, shapeOutlineWidth,
                                  Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin)
                           : Qt::NoPen);
    }
    updateTextItem();
}

void SymbolMessageItem::updateImageItem()
{
    delete imageItem;
    imageItem = nullptr;

    if (image) {
        imageItem = new QGraphicsPixmapItem(this);
        imageItem->setPixmap(QPixmap::fromImage(*image));
        imageItem->setOffset(-image->width() / 2.0, -image->height() / 2.0);
        imageItem->setScale(imageSizeFactor);
        imageItem->setTransformationMode(Qt::SmoothTransformation);
        colorizeEffect = new QGraphicsColorizeEffect(this);
        colorizeEffect->setStrength(0);
        imageItem->setGraphicsEffect(colorizeEffect);
    }
    updateTextItem();
}


}  // namespace qtenv
}  // namespace omnetpp

