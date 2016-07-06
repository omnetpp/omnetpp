//==========================================================================
//  SUBMODULEITEM.CC - part of
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

#include "submoduleitem.h"
#include "qtenv.h"
#include <QDebug>
#include <QPainter>
#include <QGraphicsColorizeEffect>
#include <omnetpp/cdisplaystring.h>
#include <omnetpp/cqueue.h>
#include "common/stringutil.h"

namespace omnetpp {
namespace qtenv {

using namespace omnetpp::common;

void SubmoduleItemUtil::setupFromDisplayString(SubmoduleItem *si, cModule *mod)
{
    si->setName(mod->getName());
    si->setVectorIndex(mod->isVector() ? mod->getIndex() : -1);

    // making a copy, so we can modify freely
    cDisplayString ds = mod->getDisplayString();

    // replacing $param args with the actual parameter values
    std::string buffer;
    ds = substituteDisplayStringParamRefs(ds, buffer, mod, true);

    bool widthOk, heightOk;
    double shapeWidth = QString(ds.getTagArg("b", 0)).toDouble(&widthOk);
    double shapeHeight = QString(ds.getTagArg("b", 1)).toDouble(&heightOk);

    if (!widthOk) shapeWidth = shapeHeight;
    if (!heightOk) shapeHeight = shapeWidth;
    if (!widthOk && !heightOk) {
        shapeWidth = 40;
        shapeHeight = 24;
    }

    si->setWidth(shapeWidth);
    si->setHeight(shapeHeight);

    QString shapeName = QString(ds.getTagArg("b", 2)).toLower();

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
        imageName = "block/process";

    if (imageName[0]) {
        auto image = getQtenv()->icons.getImage(imageName, ds.getTagArg("is", 0));
        si->setImage(image);

        const char *imageColor = ds.getTagArg("i", 1);
        si->setImageColor(parseColor(imageColor));
        si->setImageColorPercentage((ds.getNumArgs("i") == 2 && strlen(ds.getTagArg("i", 1)) > 0)
                                      ? 30 // color given, but no percentage
                                      : QString(ds.getTagArg("i", 2)).toDouble());
    } else {
        si->setImage(nullptr);
    }


    const char *decoratorImageName = ds.getTagArg("i2", 0);
    if (decoratorImageName[0]) {
        auto decoratorImage = getQtenv()->icons.getImage(decoratorImageName);
        si->setDecoratorImage(decoratorImage);

        const char *decoratorImageColor = ds.getTagArg("i2", 1);
        si->setDecoratorImageColor(parseColor(decoratorImageColor));
        si->setDecoratorImageColorPercentage((ds.getNumArgs("i2") == 2 && strlen(ds.getTagArg("i2", 1)) > 0)
                                             ? 30 // color given, but no percentage
                                               : QString(ds.getTagArg("i2", 2)).toDouble());
    } else {
        si->setDecoratorImage(nullptr);
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
    if (!color.isValid()) {
        color = QColor("blue");
    }
    si->setInfoText(text, textPos, color);

    int rangeIndex = 0;
    while (true) {
        // the first indexed is r1
        std::string tagName = (QString("r") + ((rangeIndex > 0) ? QString::number(rangeIndex) : "")).toStdString();
        if (!ds.containsTag(tagName.c_str()))
            break;

        double r = QString(ds.getTagArg(tagName.c_str(), 0)).toDouble();
        QColor rangeFillColor = parseColor(ds.getTagArg(tagName.c_str(), 1));
        QColor rangeOutlineColor = parseColor(ds.getTagArg(tagName.c_str(), 2));
        bool ok;
        int rangeOutlineWidth = QString(ds.getTagArg(tagName.c_str(), 3)).toInt(&ok);
        if (!ok) {
            rangeOutlineWidth = 1;
        }
        if (!rangeOutlineColor.isValid() && rangeOutlineWidth > 0) {
            rangeOutlineColor = QColor("black");
        }
        si->addRangeItem(r, rangeOutlineWidth, rangeFillColor, rangeOutlineColor);

        ++rangeIndex;
    }

    QString queueText;
    if (ds.containsTag("q")) {
        auto queueName = ds.getTagArg("q", 0);
        cQueue *q = dynamic_cast<cQueue *>(mod->findObject(queueName));
        if (q)
            queueText = QString("q: %1").arg(q->getLength());
    }
    si->setQueueText(queueText);
}

void SubmoduleItem::updateNameItem() {
    if (nameItem) {
        QString label = name;
        if (vectorIndex >= 0) {
            label += "[" + QString::number(vectorIndex) + "]";
        }
        nameItem->setText(label);
        nameItem->setPos(-nameItem->boundingRect().width() / 2, shapeImageBoundingRect().height() / 2);
    }
}

void SubmoduleItem::realignAnchoredItems() {
    auto mainBounds = shapeImageBoundingRect();

    // the info text label
    if (textItem) {
        auto textBounds = textItem->boundingRect();
        QPointF pos;

        switch (textPos) {
        case TEXTPOS_LEFT:
            pos.setX(mainBounds.left() - textBounds.width());
            pos.setY(mainBounds.top());
            break;
        case TEXTPOS_RIGHT:
            pos.setX(mainBounds.right());
            pos.setY(mainBounds.top());
            break;
        case TEXTPOS_TOP:
            pos.setX(mainBounds.center().x() - textBounds.width() / 2.0f);
            pos.setY(mainBounds.top() - textBounds.height());
            break;
        }

        textItem->setPos(pos);
    }

    // the queue length
    if (queueItem)
        queueItem->setPos(mainBounds.width() / 2, -mainBounds.height() / 2);

    // the icon in the corner
    if (decoratorImageItem)
        decoratorImageItem->setPos(mainBounds.width() / 2, -mainBounds.height() / 2);

    updateNameItem(); // the name is anchored too in some sense
}

void SubmoduleItem::updateShapeItem() {
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

        auto w = shapeWidth * zoomFactor - shapeOutlineWidth; // so the outline grows inwards
        auto h = shapeHeight * zoomFactor - shapeOutlineWidth;
        if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem *>(shapeItem))
            ellipseItem->setRect(-w / 2.0, -h / 2.0, w, h);
        if (auto rectItem = dynamic_cast<QGraphicsRectItem *>(shapeItem))
            rectItem->setRect(-w / 2.0, -h / 2.0, w, h);
        updateNameItem();
        realignAnchoredItems();
    }
}

void SubmoduleItem::adjustRangeItem(int i) {
    RangeData &data = ranges[i];
    QGraphicsEllipseItem *range = rangeItems[i];
    // the actual item radius, correcting for zoom and to make the outline grow inwards only
    double corrR = data.radius * zoomFactor - data.outlineWidth / 2.0;
    range->setRect(-corrR, -corrR, 2 * corrR, 2 * corrR);
    range->setZValue(-data.radius); // bigger ranges go under smaller ones
    QColor fillColorTransp = data.fillColor;
    fillColorTransp.setAlphaF(data.fillColor.alphaF() * 0.5);
    range->setBrush(data.fillColor.isValid() ? QBrush(fillColorTransp) : Qt::NoBrush);
    range->setPen(data.outlineColor.isValid() ? QPen(data.outlineColor, data.outlineWidth) : Qt::NoPen);
    range->setPos(pos());
}

QRectF SubmoduleItem::shapeImageBoundingRect() const {
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
    nameItem = new OutlinedTextItem(this, scene());
    queueItem = new OutlinedTextItem(this, scene());
    textItem = new OutlinedTextItem(this, scene());
    setAcceptsHoverEvents(true);

    connect(this, SIGNAL(xChanged()), this, SLOT(onPositionChanged()));
    connect(this, SIGNAL(yChanged()), this, SLOT(onPositionChanged()));
    connect(this, SIGNAL(zChanged()), this, SLOT(onPositionChanged()));
}

SubmoduleItem::~SubmoduleItem()
{
    for (auto i : rangeItems) {
        delete i;
    }
}

void SubmoduleItem::setZoomFactor(double zoom) {
    if (zoomFactor != zoom) {
        zoomFactor = zoom;
        updateShapeItem();

        for (int i = 0; i < ranges.length(); ++i) {
            adjustRangeItem(i);
        }
    }
}

void SubmoduleItem::setImageSizeFactor(double imageSize) {
    if (imageSizeFactor != imageSize) {
        imageSizeFactor = imageSize;
        if (imageItem)
            imageItem->setScale(imageSizeFactor);
        if (decoratorImageItem)
            decoratorImageItem->setScale(imageSizeFactor);
        realignAnchoredItems();
    }
}

void SubmoduleItem::setShape(Shape shape) {
    if (shape != this->shape) {
        this->shape = shape;

        delete shapeItem;
        shapeItem = nullptr;

        switch (shape) {
        case SHAPE_NONE: /* nothing to do here */                    break;
        case SHAPE_OVAL: shapeItem = new QGraphicsEllipseItem(this); break;
        case SHAPE_RECT: shapeItem = new QGraphicsRectItem(this);    break;
        }

        updateShapeItem();
    }
}

void SubmoduleItem::setWidth(double width) {
    if (this->shapeWidth != width) {
        this->shapeWidth = width;
        updateShapeItem();
    }
}

void SubmoduleItem::setHeight(double height) {
    if (this->shapeHeight != height) {
        this->shapeHeight = height;
        updateShapeItem();
    }
}

void SubmoduleItem::setFillColor(const QColor &color) {
    if (shapeFillColor != color) {
        shapeFillColor = color;
        updateShapeItem();
    }
}

void SubmoduleItem::setOutlineColor(const QColor &color) {
    if (shapeOutlineColor != color) {
        shapeOutlineColor = color;
        updateShapeItem();
    }
}

void SubmoduleItem::setOutlineWidth(double width) {
    if (shapeOutlineWidth != width) {
        shapeOutlineWidth = width;
        updateShapeItem();
    }
}

void SubmoduleItem::setImage(QImage *image) {
    if (this->image != image) {
        this->image = image;
        delete imageItem;
        imageItem = nullptr;
        colorizeEffect = nullptr;
        if (image) {
            imageItem = new QGraphicsPixmapItem(QPixmap::fromImage(*image), this);
            imageItem->setOffset(-image->width() / 2.0f, -image->height() / 2.0f);
            colorizeEffect = new ColorizeEffect();
            imageItem->setGraphicsEffect(colorizeEffect);
            colorizeEffect->setSmooth(true);
            imageItem->setScale(imageSizeFactor);
        }
        updateNameItem();
        realignAnchoredItems();
    }
}

void SubmoduleItem::setImageColor(const QColor &color) {
    if (colorizeEffect) {
        colorizeEffect->setColor(color);
    }
}

void SubmoduleItem::setImageColorPercentage(int percent) {
    if (colorizeEffect) {
        colorizeEffect->setWeight(percent / 100.0);
    }
}

void SubmoduleItem::setDecoratorImage(QImage *decoratorImage) {
    if (this->decoratorImage != decoratorImage) {
        this->decoratorImage = decoratorImage;
        delete decoratorImageItem;
        decoratorImageItem = nullptr;
        decoratorColorizeEffect = nullptr;
        if (decoratorImage) {
            decoratorImageItem = new QGraphicsPixmapItem(QPixmap::fromImage(*decoratorImage), this);
            // It is easier to position using its (almost) upper right corner.
            // The 2 pixel offset moves it a bit to the right and up.
            decoratorImageItem->setOffset(-decoratorImage->width() + 2, -2);
            decoratorColorizeEffect = new ColorizeEffect();
            decoratorColorizeEffect->setSmooth(true);
            decoratorImageItem->setGraphicsEffect(decoratorColorizeEffect);
        }
        realignAnchoredItems();
    }
}

void SubmoduleItem::setDecoratorImageColor(const QColor &color) {
    if (decoratorColorizeEffect) {
        decoratorColorizeEffect->setColor(color);
    }
}

void SubmoduleItem::setDecoratorImageColorPercentage(int percent) {
    if (decoratorColorizeEffect) {
        decoratorColorizeEffect->setWeight(percent / 100.0);
    }
}

void SubmoduleItem::setName(const QString &text) {
    if (name != text) {
        name = text;
        updateNameItem();
    }
}

void SubmoduleItem::setVectorIndex(int index) {
    if (vectorIndex != index) {
        vectorIndex = index;
        updateNameItem();
    }
}

void SubmoduleItem::setQueueText(const QString &queueText) {
    if (this->queueText != queueText) {
        this->queueText = queueText;
        queueItem->setText(queueText);
        realignAnchoredItems();
    }
}

void SubmoduleItem::setInfoText(const QString &text, TextPos pos, const QColor &color) {
    if (this->text != text || textColor != color || pos != textPos) {
        this->text = text;
        textPos = pos;
        textColor = color;
        textItem->setText(text);
        textItem->setBrush(color);
        realignAnchoredItems();
    }
}

void SubmoduleItem::setRangeLayer(GraphicsLayer *layer) {
    if (layer != rangeLayer) {
        rangeLayer = layer;
        for (auto &i : rangeItems) {
            rangeLayer->addItem(i);
        }
    }
}

int SubmoduleItem::addRangeItem(double radius, double outlineWidth, const QColor &fillColor, const QColor &outlineColor) {
    return addRangeItem(RangeData{radius, outlineWidth, fillColor, outlineColor});
}

int SubmoduleItem::addRangeItem(const RangeData &data) {
    auto range = new QGraphicsEllipseItem();

    ranges.append(data);
    rangeItems.append(range);
    rangeLayer->addItem(range);

    adjustRangeItem(rangeItems.length() - 1);

    return rangeItems.length() - 1;
}

QRectF SubmoduleItem::boundingRect() const {
    return shapeImageBoundingRect();
}

void SubmoduleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
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

void SubmoduleItem::onPositionChanged() {
    for (auto i : rangeItems) {
        i->setPos(pos());
    }
}

void SubmoduleItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    QString toolTip = QString("(") + getObjectShortTypeName(module) + ") " + module->getFullName() + ", " + module->info().c_str();
    const char *userTooltip = module->getDisplayString().getTagArg("tt", 0);
    if (!opp_isempty(userTooltip))
        toolTip += QString("\n") + userTooltip;
    setToolTip(toolTip);

    QGraphicsObject::hoverEnterEvent(event);
}

} // namespace qtenv
} // namespace omnetpp
