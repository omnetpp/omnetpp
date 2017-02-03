//==========================================================================
//  MESSAGEITEM.H - part of
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

#ifndef __OMNETPP_QTENV_MESSAGEITEM_H
#define __OMNETPP_QTENV_MESSAGEITEM_H

#include <QGraphicsColorizeEffect>
#include <QGraphicsObject>
#include "qtenvdefs.h"

namespace omnetpp {

class cMessage;

namespace qtenv {

class OutlinedTextItem;
class ArrowheadItem;

class MessageItem;
class QTENV_API MessageItemUtil {
    static QVector<QColor> msgKindColors;
public:
    static void setupFromDisplayString(MessageItem *mi, cMessage *msg, double imageSizeFactor);
};

class QTENV_API MessageItem : public QGraphicsObject
{
    Q_OBJECT

public:
    enum Shape {
        SHAPE_NONE,
        SHAPE_OVAL,
        SHAPE_RECT
    };

protected:
    double imageSizeFactor = 1; // no zoom factor, it doesn't affect messages
    QImage *image = nullptr;
    QColor shapeFillColor = QColor("red");
    QColor shapeOutlineColor = QColor("red");
    double shapeOutlineWidth = 0;
    double shapeWidth = 10;
    double shapeHeight = 10;
    Shape shapeType = SHAPE_OVAL;
    QString text;
    QLineF line;
    bool showAsLine = false;
    bool arrowheadEnabled = true;

    OutlinedTextItem *textItem;
    QAbstractGraphicsShapeItem *shapeItem = nullptr;
    QGraphicsPixmapItem *imageItem = nullptr;
    // TODO FIXME - this effect does not look the same as the one in tkenv
    QGraphicsColorizeEffect *colorizeEffect = nullptr; // owned by the image item
    QGraphicsLineItem *lineItem = nullptr;
    ArrowheadItem *arrowheadItem = nullptr;

    QRectF shapeImageBoundingRect() const;

    void updateTextItem();
    void updateShapeItem();
    void updateImageItem();
    void updateLineItem();

public:
    explicit MessageItem(QGraphicsItem *parent = nullptr);
    virtual ~MessageItem();

    void setImageSizeFactor(double imageSize);

    void setText(const QString &text);

    void setShape(Shape shape);
    void setWidth(double width);
    void setHeight(double height);
    void setFillColor(const QColor &color);
    void setOutlineColor(const QColor &color);
    void setOutlineWidth(double width);
    void setArrowheadEnabled(bool enabled);

    void setImage(QImage *image);
    void setImageColor(const QColor &color);
    void setImageColorPercentage(int percent);

    void setLine(const QLineF& line, bool showAsLine);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override { /* empty */ }
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_MESSAGEITEM_H
