//==========================================================================
//  CONNECTIONITEM.H - part of
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

#ifndef __OMNETPP_QTENV_CONNECTIONITEM_H
#define __OMNETPP_QTENV_CONNECTIONITEM_H

#include <QGraphicsObject>
#include <omnetpp/cdisplaystring.h>
#include "graphicspatharrowitem.h"
#include "qtutil.h"

namespace omnetpp {
namespace qtenv {

class ConnectionItem;
class ConnectionItemUtil {
public:
    // only stylistic tags are handled here (color, width, style, etc), positioning is in the ModuleCanvasViewer
    static void setupFromDisplayString(ConnectionItem *ci, cGate *gate, bool twoWay);
};

class ConnectionItem : public QGraphicsObject
{
    Q_OBJECT

    // adds this many pixels to the collision detection shape around the line
    // to make it easier to select the connection
    const double clickThreshold = 3;

protected:
    QPointF src, dest;
    double lineWidth = 1;
    QColor lineColor = QColor("black");
    Qt::PenStyle lineStyle = Qt::SolidLine; // not directly fed to the lineItem
    double dashOffset = 0;
    QString text;
    Qt::Alignment textAlignment = Qt::AlignCenter; // Left for source, Right for dest, Center for middle
    QColor textColor = QColor("#005030");
    bool lineEnabled = true;
    bool halfLength = false;

    QGraphicsLineItem *lineItem = nullptr;
    OutlinedTextItem *textItem = nullptr; // This is a managed sibling!
    GraphicsPathArrowItem *arrowItem = nullptr;

    void updateLineItem();
    void updateTextItem();
    void updateArrowItem();

public:
    explicit ConnectionItem(QGraphicsItem *parent = 0);
    virtual ~ConnectionItem();

    void setSource(const QPointF &source);
    void setDestination(const QPointF &destination);
    void setLine(const QLineF &line);
    void setWidth(double width);
    void setColor(const QColor &color);
    void setLineStyle(Qt::PenStyle style);
    void setDashOffset(double offset);
    void setText(const QString &text);
    void setTextPosition(Qt::Alignment alignment);
    void setTextBackgroundColor(const QColor &color);
    void setTextOutlineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setLineEnabled(bool enabled);
    bool isLineEnabled() const { return lineEnabled; }
    void setArrowEnabled(bool enabled);
    bool isArrowEnabled() const { return arrowItem->isVisible(); }
    // This is used in two-way connections to show the styles
    // in both cGate's cDisplayStrings next to each other.
    void setHalfLength(bool enabled);
    bool isHalfLength() const { return halfLength; }

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath shape() const override;
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_CONNECTIONITEM_H
