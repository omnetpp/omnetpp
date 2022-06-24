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

#include <QtWidgets/QGraphicsColorizeEffect>
#include <QtWidgets/QGraphicsObject>
#include "qtenvdefs.h"
#include "qtutil.h"

namespace omnetpp {

class cMessage;

namespace qtenv {

class OutlinedTextItem;
class ArrowheadItem;

class MessageItem;
class LineMessageItem;
class SymbolMessageItem;


class QTENV_API MessageItemUtil {
    static const QVector<QColor> msgKindColors;
    static void setupMessageCommon(MessageItem *mi, cMessage *msg);

public:
    static void setupLineFromDisplayString(LineMessageItem *mi, cMessage *msg);
    static void setupSymbolFromDisplayString(SymbolMessageItem *mi, cMessage *msg, double imageSizeFactor);
    static const QColor& getColorForMessageKind(int messageKind);
};

/**
 * A common base class for graphical items that can represent a message in
 * module inspectors while animating.
 */
class QTENV_API MessageItem : public QGraphicsObject
{
    Q_OBJECT

protected:
    QString text;
    OutlinedTextItem *textItem;

    void updateTextItem();

public:
    explicit MessageItem(QGraphicsItem *parent = nullptr);
    virtual ~MessageItem();

    void setText(const QString &text);

    // Where the top-middle point of the text should be, in local coordinates.
    virtual QPointF getTextPosition() = 0;

    // Positions (and configures) this item as being sent/transmitted on the given
    // (connection) line. The line parameter is in parent coordinates. t1 and t2 are
    // the start and end positions of the message on line, in range [0..1].
    virtual void positionOntoLine(const QLineF& line, double t1, double t2, bool asUpdatePacket) = 0;

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override { /* empty */ }
};

/**
 * A graphical item that represents a message as a line.
 * Useful for animating packet transmissions.
 */
class QTENV_API LineMessageItem : public MessageItem
{
    Q_OBJECT

protected:
    QLineF line;
    QColor color;

    bool lineEnabled = true;
    bool arrowheadEnabled = true;
    bool txUpdateMarkerEnabled = true;

    QGraphicsLineItem *lineItem = nullptr;
    ArrowheadItem *arrowheadItem = nullptr;
    QGraphicsLineItem *txUpdateMarkerItem = nullptr;

    QPointF getSideOffsetForWidth(float width) const;
    void updateLineItem();

public:
    LineMessageItem(QGraphicsItem *parent);
    virtual ~LineMessageItem();

    void setColor(const QColor& color);
    void setLine(const QLineF& line);

    void setLineEnabled(bool enabled);
    void setArrowheadEnabled(bool enabled);
    void setTxUpdateMarkerEnabled(bool enabled);

    virtual QPointF getTextPosition() override;
    virtual void positionOntoLine(const QLineF& line, double t1, double t2, bool asUpdatePacket) override;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
};

/**
 * A graphical items that represents a message as a shape (oval or rectangle)
 * and/or an icon. Useful for animating zero length messages.
 */
class QTENV_API SymbolMessageItem : public MessageItem
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
    QColor shapeFillColor = colors::RED;
    QColor shapeOutlineColor = colors::RED;
    double shapeOutlineWidth = 0;
    double shapeWidth = 10;
    double shapeHeight = 10;
    Shape shapeType = SHAPE_OVAL;

    QAbstractGraphicsShapeItem *shapeItem = nullptr;
    QGraphicsPixmapItem *imageItem = nullptr;
    // TODO FIXME - this effect does not look the same as the colorization of SubmoduleItem
    QGraphicsColorizeEffect *colorizeEffect = nullptr; // owned by the image item

    QRectF shapeImageBoundingRect() const;

    void updateShapeItem();
    void updateImageItem();

public:
    SymbolMessageItem(QGraphicsItem *parent);
    virtual ~SymbolMessageItem();

    void setImageSizeFactor(double imageSize);

    void setShape(Shape shape);
    void setWidth(double width);
    void setHeight(double height);
    void setFillColor(const QColor &color);
    void setOutlineColor(const QColor &color);
    void setOutlineWidth(double width);

    void setImage(QImage *image);
    void setImageColor(const QColor &color);
    void setImageColorPercentage(int percent);

    virtual QPointF getTextPosition() override;
    /// For this subclass, t1 and t2 must be equal and asUpdatePacket must be false.
    virtual void positionOntoLine(const QLineF& line, double t1, double t2, bool asUpdatePacket);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
};


}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_MESSAGEITEM_H
