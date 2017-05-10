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
#include <QDebug>
#include <QPen>
#include "qtenv.h"
#include "graphicsitems.h"
#include "omnetpp/cdisplaystring.h"

namespace omnetpp {
namespace qtenv {

QVector<QColor> MessageItemUtil::msgKindColors = {"red", "green", "blue", "white", "yellow", "cyan", "magenta", "black"};

void MessageItemUtil::setupFromDisplayString(MessageItem *mi, cMessage *msg, double imageSizeFactor)
{
    cDisplayString ds = msg->getDisplayString();

    mi->setData(ITEMDATA_COBJECT, QVariant::fromValue((cObject *)msg));

    QtenvOptions *opt = getQtenv()->opt;

    QString label;
    if (opt->animationMsgClassNames)
        label += QString("(") + getObjectShortTypeName(msg) + ")";
    if (opt->animationMsgNames)
        label += msg->getFullName();
    mi->setText(label);

    if (!ds.str()[0] && opt->penguinMode)
        ds = "i=abstract/penguin_s";  // ^^,

    // no zoom factor, it doesn't affect messages
    mi->setImageSizeFactor(imageSizeFactor);

    QColor kindColor = msgKindColors[msg->getKind() % msgKindColors.size()];

    if (!ds.str()[0]) {  // default little red dot
        QColor color = opt->animationMsgColors ? kindColor : "red";
        mi->setShape(MessageItem::SHAPE_OVAL);
        mi->setWidth(10);
        mi->setHeight(10);
        mi->setFillColor(color);
        mi->setOutlineColor(color);
        mi->setImage(nullptr);
    }
    else {  // as defined in the dispstr
        bool widthOk, heightOk;
        double shapeWidth = QString(ds.getTagArg("b", 0)).toDouble(&widthOk);
        double shapeHeight = QString(ds.getTagArg("b", 1)).toDouble(&heightOk);

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

        QString shapeName = QString(ds.getTagArg("b", 2)).toLower();

        auto shape = (ds.getNumArgs("b") <= 0)
                       ? MessageItem::SHAPE_NONE
                       : shapeName[0] == 'r'
                           ? MessageItem::SHAPE_RECT
                           : MessageItem::SHAPE_OVAL; // if unknown, this is the default
        mi->setShape(shape);

        QString fillColorName = ds.getTagArg("b", 3);
        QColor fillColor = fillColorName == "kind"
                            ? kindColor
                            : parseColor(fillColorName, "red");
        mi->setFillColor(fillColor);
        QString outlineColorName = ds.getTagArg("b", 4);
        mi->setOutlineColor(outlineColorName == "kind"
                             ? kindColor
                             : parseColor(outlineColorName, fillColor));

        bool ok;
        int outlineWidth = QString(ds.getTagArg("b", 5)).toInt(&ok);
        mi->setOutlineWidth(ok ? outlineWidth : 2);

        const char *imageName = ds.getTagArg("i", 0);
        mi->setImage(imageName[0] ? getQtenv()->icons.getImage(imageName, ds.getTagArg("is", 0)) : nullptr);

        QString imageColorName = ds.getTagArg("i", 1);
        mi->setImageColor(imageColorName == "kind" ? kindColor : parseColor(imageColorName));
        mi->setImageColorPercentage((ds.getNumArgs("i") == 2) // color given, but no percentage
                                      ? 30
                                      : QString(ds.getTagArg("i", 2)).toDouble());
    }
}

MessageItem::MessageItem(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    textItem = new OutlinedTextItem(this);
    shapeItem = new QGraphicsEllipseItem(this);
    imageItem = new QGraphicsPixmapItem(this);
    lineItem = new QGraphicsLineItem(this);
    arrowheadItem = new ArrowheadItem(this);
    arrowheadItem->setFillRatio(1);
    arrowheadItem->setPen(Qt::NoPen);

    textItem->setZValue(1);

    updateShapeItem();
}

MessageItem::~MessageItem()
{
    delete shapeItem;
    delete imageItem;
    delete textItem;
    delete lineItem;
    delete arrowheadItem;
}

void MessageItem::setImageSizeFactor(double imageSize)
{
    if (imageSizeFactor != imageSize) {
        imageSizeFactor = imageSize;
        updateImageItem();
    }
}

void MessageItem::setText(const QString& text)
{
    if (this->text != text) {
        this->text = text;
        updateTextItem();
    }
}

void MessageItem::setShape(Shape shape)
{
    if (this->shapeType != shape) {
        this->shapeType = shape;
        updateShapeItem();
    }
}

void MessageItem::setWidth(double width)
{
    if (shapeWidth != width) {
        shapeWidth = width;
        updateShapeItem();
    }
}

void MessageItem::setHeight(double height)
{
    if (shapeHeight != height) {
        shapeHeight = height;
        updateShapeItem();
    }
}

void MessageItem::setFillColor(const QColor& color)
{
    if (this->shapeFillColor != color) {
        this->shapeFillColor = color;
        updateShapeItem();
    }
}

void MessageItem::setOutlineColor(const QColor& color)
{
    if (this->shapeOutlineColor != color) {
        this->shapeOutlineColor = color;
        updateShapeItem();
    }
}

void MessageItem::setOutlineWidth(double width)
{
    if (shapeOutlineWidth != width) {
        shapeOutlineWidth = width;
        updateShapeItem();
    }
}

void MessageItem::setArrowheadEnabled(bool enabled)
{
    if (arrowheadEnabled != enabled) {
        arrowheadEnabled = enabled;
        updateLineItem();
    }
}

void MessageItem::setImage(QImage *image)
{
    if (this->image != image) {
        this->image = image;
        updateImageItem();
        updateTextItem();
    }
}

void MessageItem::setImageColor(const QColor& color)
{
    if (colorizeEffect) {
        colorizeEffect->setColor(color);
    }
}

void MessageItem::setImageColorPercentage(int percent)
{
    if (colorizeEffect) {
        colorizeEffect->setStrength(percent / 100.0f);
    }
}

void MessageItem::setLine(const QLineF& line, bool showAsLine)
{
    if (this->line != line || this->showAsLine != showAsLine) {
        this->line = line;
        this->showAsLine = showAsLine;
        setPos(line.pointAt(0.5));
        updateLineItem();
        updateShapeItem();
    }
}

QRectF MessageItem::boundingRect() const
{
    return lineItem->isVisible() ? lineItem->boundingRect() : shapeImageBoundingRect();
}

QPainterPath MessageItem::shape() const
{
    if (lineItem->isVisible())
        return lineItem->shape();

    QPainterPath path;
    path.addRect(shapeImageBoundingRect());
    return path;
}

QRectF MessageItem::shapeImageBoundingRect() const
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

void MessageItem::updateTextItem()
{
    textItem->setText(text);
    QRectF textRect = textItem->textRect();
    QRectF mainRect = shapeImageBoundingRect();
    textItem->setPos(-textRect.width() / 2, mainRect.height() / 2);
}

void MessageItem::updateShapeItem()
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
    updateLineItem();
}

void MessageItem::updateImageItem()
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

void MessageItem::updateLineItem()
{
    setPos(line.pointAt(0.5));

    double width = 6;

    // to make it go on the right side of the connection, with a bit of spacing
    QPointF sideOffset = (width / 2 + 2) * line.normalVector().unitVector().translated(-line.p1()).p2();

    QPen pen(shapeFillColor, width, Qt::SolidLine, Qt::FlatCap);

    // relative to pos, center is always in origin
    auto localLine = line.translated(-pos());
    localLine = QLineF(localLine.p2(), localLine.p1());

    double arrowheadLength = arrowheadEnabled ? width : 0;

    // don't let it get too short (1 px for the line, 1 for the arrowhead, why not...)
    if (localLine.length() < 2) {
        localLine.setLength(2);
        localLine.translate(-localLine.pointAt(0.5));
    }

    arrowheadLength = std::min(arrowheadLength, localLine.length()/2);

    localLine = localLine.translated(sideOffset);
    arrowheadItem->setEndPoints(localLine.p1(), localLine.p2());
    localLine.setLength(localLine.length()-arrowheadLength);

    lineItem->setLine(localLine);
    lineItem->setPen(pen);

    arrowheadItem->setBrush(shapeFillColor);
    arrowheadItem->setArrowWidth(width);
    arrowheadItem->setArrowLength(arrowheadLength + 0.5); // +0.5 is just to make it "watertight" (AA and imprecision and stuff)

    lineItem->setVisible(showAsLine);
    arrowheadItem->setVisible(showAsLine && arrowheadEnabled);

    if (shapeItem)
        shapeItem->setVisible(!showAsLine);
    if (imageItem)
        imageItem->setVisible(!showAsLine);
}

}  // namespace qtenv
}  // namespace omnetpp

