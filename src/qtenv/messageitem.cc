//==========================================================================
//  MESSAGEITEM.CC - part of
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

#include "messageitem.h"

#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

void MessageItemUtil::setupFromDisplayString(MessageItem *mi, cMessage *msg) {
    mi->setColor(QColor("red"));
    QString label;
    if (getQtenv()->opt->animationMsgClassNames)
        label += QString("(") + getObjectShortTypeName(msg) + ")";
    if (getQtenv()->opt->animationMsgNames)
        label += msg->getFullName();

    mi->setText(label);
    mi->setData(1, QVariant::fromValue((cObject*)msg));
}

MessageItem::MessageItem(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    textItem = new OutlinedTextItem(this);
    ellipseItem = new QGraphicsEllipseItem(this);
    imageItem = new QGraphicsPixmapItem(this);

    ellipseItem->setVisible(false);
    imageItem->setVisible(false);

    if (getQtenv()->opt->penguinMode) {
        setImage(getQtenv()->icons.getImage("abstract/penguin_s"));
    }
}

MessageItem::~MessageItem() {
    delete ellipseItem;
    delete imageItem;
    delete textItem;
}

void MessageItem::setText(const QString &text) {
    if (this->text != text) {
        this->text = text;
        updateTextItem();
    }
}

void MessageItem::setColor(const QColor &color) {
    if (this->color != color) {
        this->color = color;
        if (!getQtenv()->opt->penguinMode) {
            ellipseItem->setVisible(true);
            imageItem->setVisible(false);
        }
        updateEllipseItem();
        updateTextItem();
    }
}

void MessageItem::setImage(QImage *image) {
    if (this->image != image) {
        this->image = image;
        ellipseItem->setVisible(false);
        imageItem->setVisible(true);
        updateImageItem();
        updateTextItem();
    }
}

QRectF MessageItem::boundingRect() const {
    return ellipseImageBoundingBox();
}

void MessageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

}

QRectF MessageItem::ellipseImageBoundingBox() const {
    QRectF box;
    if (ellipseItem->isVisible()) box = box.united(ellipseItem->boundingRect());
    if (imageItem->isVisible()) box = box.united(imageItem->boundingRect());
    return box;
}

void MessageItem::updateTextItem() {
    textItem->setText(text);
    QSizeF textRectSize = textItem->boundingRect().size();
    QRectF mainRect = ellipseImageBoundingBox();
    textItem->setPos(-textRectSize.width() / 2, mainRect.height() / 2);
}

void MessageItem::updateEllipseItem() {
    ellipseItem->setRect(-5, -5, 10, 10);
    ellipseItem->setBrush(color);
}

void MessageItem::updateImageItem() {
    imageItem->setPixmap(QPixmap::fromImage(*image));
    imageItem->setOffset(-image->width() / 2.0, -image->height() / 2.0);
}

} // namespace qtenv
} // namespace omnetpp
