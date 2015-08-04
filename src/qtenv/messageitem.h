//==========================================================================
//  MESSAGEITEM.H - part of
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

#ifndef __OMNETPP_QTENV_MESSAGEITEM_H
#define __OMNETPP_QTENV_MESSAGEITEM_H

#include "qtutil.h"

#include <QGraphicsObject>

namespace omnetpp {
namespace qtenv {

class MessageItem;
class MessageItemUtil {
public:
    static void setupFromDisplayString(MessageItem *mi, cMessage *msg);
};

class MessageItem : public QGraphicsObject
{
    Q_OBJECT

protected:
    QImage *image = nullptr;
    QColor color;
    QString text;

    OutlinedTextItem *textItem;
    QGraphicsEllipseItem *ellipseItem = nullptr;
    QGraphicsPixmapItem *imageItem = nullptr;

    QRectF ellipseImageBoundingBox() const;

    void updateTextItem();
    void updateEllipseItem();
    void updateImageItem();

public:
    explicit MessageItem(QGraphicsItem *parent = nullptr);
    virtual ~MessageItem();

    void setText(const QString &text);
    void setColor(const QColor &color); // will hide the image and show the ellipse only
    void setImage(QImage *image);       // will hide the ellipse and show the image only

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_MESSAGEITEM_H
