//==========================================================================
//  SUBMODULEITEM.CC - part of
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

#include "submoduleitem.h"

#include "qtenv.h"
#include "qtutil.h"
#include "graphicsitems.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtCore/QEvent>
#include <QtWidgets/QGraphicsColorizeEffect>
#include <omnetpp/cdisplaystring.h>
#include <omnetpp/cqueue.h>
#include "common/stringutil.h"

namespace omnetpp {
namespace qtenv {

using namespace omnetpp::common;

const char *const SubmoduleItem::DEFAULT_ICON = "block/process";

//TODO graphics not refreshed when user edits displayName in the inspector

void SubmoduleItemUtil::setupFromDisplayString(SubmoduleItem *si, cModule *mod)
{
    // making a copy, so we can modify freely
    cDisplayString ds = mod->getDisplayString();

    // replacing $param args with the actual parameter values
    std::string buffer;
    ds = substituteDisplayStringParamRefs(ds, buffer, mod, true);

    bool widthOk, heightOk;
    double shapeWidth = QString(ds.getTagArg("b", 0)).toDouble(&widthOk);
    double shapeHeight = QString(ds.getTagArg("b", 1)).toDouble(&heightOk);

    if (!widthOk)
        shapeWidth = shapeHeight;
    if (!heightOk)
        shapeHeight = shapeWidth;
    if (!widthOk && !heightOk) {
        shapeWidth = 40;
        shapeHeight = 24;
    }

    si->setWidth(shapeWidth);
    si->setHeight(shapeHeight);

    QString shapeName = QString(ds.getTagArg("b", 2));

    auto shape = (ds.getNumArgs("b") <= 0)
                   ? SubmoduleItem::SHAPE_NONE
                   : shapeName == "oval"
                       ? SubmoduleItem::SHAPE_OVAL
                       : SubmoduleItem::SHAPE_RECT; // if unknown, this is the default
    si->setShape(shape);

    si->setFillColor(parseColor(ds.getTagArg("b", 3)));
    si->setOutlineColor(parseColor(ds.getTagArg("b", 4)));
    bool ok;
    int outlineWidth = QString(ds.getTagArg("b", 5)).toInt(&ok);
    si->setOutlineWidth(ok ? outlineWidth : 2);

    const char *imageName = ds.getTagArg("i", 0);

    if (!imageName[0] && shape == SubmoduleItem::SHAPE_NONE)
        imageName = SubmoduleItem::DEFAULT_ICON;

    if (imageName[0]) {
        const char *imageColor = ds.getTagArg("i", 1);
        double tintAmount = ((ds.getNumArgs("i") == 2 && strlen(ds.getTagArg("i", 1)) > 0)
                ? 30  // color given, but no percentage
                : QString(ds.getTagArg("i", 2)).toDouble()) / 100.0;

        si->setIcon(getQtenv()->icons.getTintedPixmap(
                        imageName, ds.getTagArg("is", 0), parseColor(imageColor), tintAmount));
    }
    else {
        si->setIcon(QPixmap());
    }

    const char *decoratorImageName = ds.getTagArg("i2", 0);
    if (decoratorImageName[0]) {
        const char *decoratorImageColor = ds.getTagArg("i2", 1);
        double tintAmount = ((ds.getNumArgs("i2") == 2 && strlen(ds.getTagArg("i2", 1)) > 0)
                ? 30  // color given, but no percentage
                : QString(ds.getTagArg("i2", 2)).toDouble()) / 100.0;

        si->setDecoratorIcon(getQtenv()->icons.getTintedPixmap(
                                 decoratorImageName, parseColor(decoratorImageColor), tintAmount));
    }
    else {
        si->setDecoratorIcon(QPixmap());
    }

    const char *text = ds.getTagArg("t", 0);
    const char *textPosition = ds.getTagArg("t", 1);
    const char *textColor = ds.getTagArg("t", 2);

    SubmoduleItem::TextPos textPos;
    switch (textPosition[0]) {
        case 'l': textPos = SubmoduleItem::TEXTPOS_LEFT;  break;
        case 'r': textPos = SubmoduleItem::TEXTPOS_RIGHT; break;
        default:  textPos = SubmoduleItem::TEXTPOS_TOP;   break;
    }
    auto color = parseColor(textColor);
    if (!color.isValid())
        color = QColor("blue");
    si->setInfoText(text, textPos, color);

    si->clearRangeItems();
    for (int i = 0; i < ds.getNumTags(); ++i) {
        bool ok = true;

        auto tagName = ds.getTagName(i);
        if (tagName[0] != 'r')
            ok = false;

        for (int i = 1; tagName[i] && ok; ++i)
            if (tagName[i] < '0' || tagName[i] > '9')
                ok = false;

        if (!ok)
            continue;

        double r = QString(ds.getTagArg(i, 0)).toDouble();
        QColor rangeFillColor = parseColor(ds.getTagArg(i, 1));
        QColor rangeOutlineColor = parseColor(ds.getTagArg(i, 2));

        int rangeOutlineWidth = QString(ds.getTagArg(i, 3)).toInt(&ok);
        if (!ok)
            rangeOutlineWidth = 1;
        if (!rangeOutlineColor.isValid() && rangeOutlineWidth > 0)
            rangeOutlineColor = QColor("black");
        si->addRangeItem(r, rangeOutlineWidth, rangeFillColor, rangeOutlineColor);
    }

    si->setOpacity(mod->isPlaceholder() ? 0.5 : 1.0);

    updateQueueSizeLabel(si, mod);
}

void SubmoduleItemUtil::updateQueueSizeLabel(SubmoduleItem *si, cModule *mod)
{
    cDisplayString ds = mod->getDisplayString();
    std::string buffer;
    ds = substituteDisplayStringParamRefs(ds, buffer, mod, true);

    QString queueText;
    if (ds.containsTag("q"))
        if (cQueue *q = dynamic_cast<cQueue *>(mod->findObject(ds.getTagArg("q", 0))))
            queueText = QString("q: %1").arg(q->getLength());
    si->setQueueText(queueText);
}


void SubmoduleItem::updateNameItem()
{
    QString nameLabel;
    const char *fullName = module->getFullName();
    const char *displayName = module->getDisplayName();
    switch (nameFormat) {
        case FMT_FULLNAME: nameLabel = fullName; break;
        case FMT_QDISPLAYNAME: nameLabel = displayName == nullptr ? QString(fullName) : QString("\"%1\"").arg(displayName); break;
        case FMT_FULLNAME_AND_QDISPLAYNAME: nameLabel = displayName == nullptr ? QString(fullName) : QString("%1 \"%2\"").arg(fullName).arg(displayName); break;
    };

    nameItem->setText(nameLabel);
    nameItem->setPos(-nameItem->textRect().width() / 2, shapeImageBoundingRect().height() / 2 + 2);
}

void SubmoduleItem::realignAnchoredItems()
{
    auto mainBounds = shapeImageBoundingRect().adjusted(-2, -2, 2, 2);
    auto textBounds = textItem->boundingRect();

    switch (textPos) {
        case TEXTPOS_LEFT:
            textItem->setPos(mainBounds.left() - textBounds.width(), mainBounds.top());
            textItem->setAlignment(Qt::AlignRight);
            break;
        case TEXTPOS_RIGHT:
            textItem->setPos(mainBounds.topRight());
            textItem->setAlignment(Qt::AlignLeft);
            break;
        case TEXTPOS_TOP:
            textItem->setPos(mainBounds.center().x() - textBounds.width() / 2, mainBounds.top() - textBounds.height());
            textItem->setAlignment(Qt::AlignCenter);
            break;
    }

    // the queue length
    queueItem->setPos(mainBounds.width() / 2, -mainBounds.height() / 2);

    // the icon in the corner
    if (decoratorImageItem)
        decoratorImageItem->setPos(mainBounds.width() / 2, -mainBounds.height() / 2);

    updateNameItem();  // the name is anchored too in some sense
}

void SubmoduleItem::updateShapeItem()
{
    if (shapeItem) {
        shapeItem->setBrush(shapeFillColor.isValid() ? shapeFillColor : QColor("#8080ff"));
        auto pen = shapeOutlineWidth == 0
                     ? Qt::NoPen
                     : QPen(shapeOutlineColor.isValid()
                              ? shapeOutlineColor
                              : QColor("black"),
                            shapeOutlineWidth);
        pen.setJoinStyle(Qt::MiterJoin);
        shapeItem->setPen(pen);

        auto w = shapeWidth * zoomFactor - shapeOutlineWidth;  // so the outline grows inwards
        auto h = shapeHeight * zoomFactor - shapeOutlineWidth;
        if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem *>(shapeItem))
            ellipseItem->setRect(-w / 2.0, -h / 2.0, w, h);
        if (auto rectItem = dynamic_cast<QGraphicsRectItem *>(shapeItem))
            rectItem->setRect(-w / 2.0, -h / 2.0, w, h);
        updateNameItem();
        realignAnchoredItems();
    }
}

void SubmoduleItem::adjustRangeItem(int i)
{
    RangeData& data = ranges[i];
    QGraphicsEllipseItem *range = rangeItems[i];
    // the actual item radius, correcting for zoom and to make the outline grow inwards only
    double corrR = data.radius * zoomFactor - data.outlineWidth / 2.0;
    range->setRect(-corrR, -corrR, 2 * corrR, 2 * corrR);
    range->setZValue(-data.radius);  // bigger ranges go under smaller ones
    QColor fillColorTransp = data.fillColor;
    fillColorTransp.setAlphaF(data.fillColor.alphaF() * 0.5);
    range->setBrush(data.fillColor.isValid() ? QBrush(fillColorTransp) : Qt::NoBrush);
    range->setPen(data.outlineColor.isValid() ? QPen(data.outlineColor, data.outlineWidth) : Qt::NoPen);
    range->setPos(pos());
}

QRectF SubmoduleItem::shapeImageBoundingRect() const
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
        // Shape size is modulated by the zoom via adjusting its defining rectangle,
        // (otherwise the outline would change width too), so its boundingRect is good as it is.
        rect = rect.united(shapeRect);
    }

    if (rect.width() < minimumRectSize) {
        rect.setLeft(-minimumRectSize / 2);
        rect.setRight(minimumRectSize / 2);
    }

    if (rect.height() < minimumRectSize) {
        rect.setTop(-minimumRectSize / 2);
        rect.setBottom(minimumRectSize / 2);
    }

    return rect;
}

SubmoduleItem::SubmoduleItem(cModule *mod, GraphicsLayer *rangeLayer)
    : module(mod), rangeLayer(rangeLayer)
{
    nameItem = new OutlinedTextItem(this);
    queueItem = new OutlinedTextItem(this);
    textItem = new MultiLineOutlinedTextItem(this);

    connect(this, SIGNAL(xChanged()), this, SLOT(onPositionChanged()));
    connect(this, SIGNAL(yChanged()), this, SLOT(onPositionChanged()));
    connect(this, SIGNAL(zChanged()), this, SLOT(onPositionChanged()));
}

SubmoduleItem::~SubmoduleItem()
{
    clearRangeItems();
}

void SubmoduleItem::setZoomFactor(double zoom)
{
    if (zoomFactor != zoom) {
        zoomFactor = zoom;
        updateShapeItem();

        for (int i = 0; i < ranges.length(); ++i) {
            adjustRangeItem(i);
        }
    }
}

void SubmoduleItem::setImageSizeFactor(double imageSize)
{
    if (imageSizeFactor != imageSize) {
        imageSizeFactor = imageSize;
        if (imageItem)
            imageItem->setScale(imageSizeFactor);
        if (decoratorImageItem)
            decoratorImageItem->setScale(imageSizeFactor);
        realignAnchoredItems();
    }
}

void SubmoduleItem::setShape(Shape shape)
{
    if (shape != this->shape) {
        this->shape = shape;

        delete shapeItem;
        shapeItem = nullptr;

        switch (shape) {
            case SHAPE_NONE: /* nothing to do here */ break;
            case SHAPE_OVAL: shapeItem = new QGraphicsEllipseItem(this); break;
            case SHAPE_RECT: shapeItem = new QGraphicsRectItem(this); break;
        }

        updateShapeItem();
    }
}

void SubmoduleItem::setWidth(double width)
{
    if (this->shapeWidth != width) {
        this->shapeWidth = width;
        updateShapeItem();
    }
}

void SubmoduleItem::setHeight(double height)
{
    if (this->shapeHeight != height) {
        this->shapeHeight = height;
        updateShapeItem();
    }
}

void SubmoduleItem::setFillColor(const QColor& color)
{
    if (shapeFillColor != color) {
        shapeFillColor = color;
        updateShapeItem();
    }
}

void SubmoduleItem::setOutlineColor(const QColor& color)
{
    if (shapeOutlineColor != color) {
        shapeOutlineColor = color;
        updateShapeItem();
    }
}

void SubmoduleItem::setOutlineWidth(double width)
{
    if (shapeOutlineWidth != width) {
        shapeOutlineWidth = width;
        updateShapeItem();
    }
}

void SubmoduleItem::setIcon(QPixmap icon)
{
    if (icon.isNull()) {
        delete imageItem;
        imageItem = nullptr;
        return;
    }

    if (!imageItem)
        imageItem = new QGraphicsPixmapItem(this);

    imageItem->setPixmap(icon);
    imageItem->setTransformationMode(Qt::SmoothTransformation);
    imageItem->setOffset(-icon.width() / 2.0f, -icon.height() / 2.0f);
    imageItem->setZValue(1); // icon always on top of shape
    imageItem->setScale(imageSizeFactor);

    realignAnchoredItems();
}

void SubmoduleItem::setDecoratorIcon(QPixmap icon)
{
    if (icon.isNull()) {
        delete decoratorImageItem;
        decoratorImageItem = nullptr;
        return;
    }

    if (!decoratorImageItem)
        decoratorImageItem = new QGraphicsPixmapItem(this);

    decoratorImageItem->setPixmap(icon);
    decoratorImageItem->setTransformationMode(Qt::SmoothTransformation);
    decoratorImageItem->setOffset(-icon.width() + 2, -2);
    decoratorImageItem->setZValue(2); // decorator icon always on top of icon

    realignAnchoredItems();
}

void SubmoduleItem::setNameFormat(NameFormat format)
{
    if (nameFormat != format) {
        nameFormat = format;
        updateNameItem();
    }
}

void SubmoduleItem::setNameVisible(bool visible)
{
    nameItem->setVisible(visible);
}

void SubmoduleItem::setQueueText(const QString& queueText)
{
    if (this->queueText != queueText) {
        this->queueText = queueText;
        queueItem->setText(queueText);
        realignAnchoredItems();
    }
}

void SubmoduleItem::setInfoText(const QString& text, TextPos pos, const QColor& color)
{
    if (this->text != text || textColor != color || pos != textPos) {
        this->text = text;
        textPos = pos;
        textColor = color;

        textItem->setText(text);
        textItem->setBrush(color);
        realignAnchoredItems();
    }
}

void SubmoduleItem::setRangeLayer(GraphicsLayer *layer)
{
    if (layer != rangeLayer) {
        rangeLayer = layer;
        for (auto& i : rangeItems) {
            rangeLayer->addItem(i);
        }
    }
}

int SubmoduleItem::addRangeItem(double radius, double outlineWidth, const QColor& fillColor, const QColor& outlineColor)
{
    return addRangeItem(RangeData { radius, outlineWidth, fillColor, outlineColor });
}

int SubmoduleItem::addRangeItem(const RangeData& data)
{
    auto range = new QGraphicsEllipseItem();

    ranges.append(data);
    rangeItems.append(range);
    rangeLayer->addItem(range);

    adjustRangeItem(rangeItems.length() - 1);

    return rangeItems.length() - 1;
}

void SubmoduleItem::clearRangeItems()
{
    for (auto r : rangeItems)
        delete r;

    rangeItems.clear();
    ranges.clear();
}

QRectF SubmoduleItem::boundingRect() const
{
    return shapeImageBoundingRect();
}

void SubmoduleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // nothing to do here, the child items are painted anyways

    /*
    // DEBUG
    painter->save();
    painter->setBrush(QColor(255, 0, 0, 100));
    painter->setPen(QColor(0, 255, 0, 100));
    painter->drawPath(QGraphicsItem::shape());
    painter->restore();
    */
}

void SubmoduleItem::onPositionChanged()
{
    for (auto i : rangeItems) {
        i->setPos(pos());
    }
}

}  // namespace qtenv
}  // namespace omnetpp

