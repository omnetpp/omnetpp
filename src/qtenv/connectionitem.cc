//==========================================================================
//  CONNECTIONITEM.CC - part of
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

#include "connectionitem.h"

#include <QDebug>
#include <QPainter>
#include <omnetpp/cgate.h>
#include <omnetpp/cchannel.h>
#include "qtutil.h"

namespace omnetpp {
namespace qtenv {


void ConnectionItemUtil::setupFromDisplayString(ConnectionItem *ci, cGate *gate, cGate *destGate, bool twoWay) {
    cChannel *chan = gate->getChannel();

    cDisplayString ds = chan && chan->hasDisplayString() && chan->parametersFinalized()
            ? chan->getDisplayString()
            : cDisplayString();

    // replacing $param args with the actual parameter values
    std::string buffer;
    ds = substituteDisplayStringParamRefs(ds, buffer, chan, true);

    ci->setData(1, QVariant::fromValue((cObject*)gate));

    ci->setToolTip(ds.getTagArg("tt", 0));

    ci->setColor(parseColor(ds.getTagArg("ls", 0), QColor("black")));

    bool ok;
    double width = QString(ds.getTagArg("ls", 1)).toDouble(&ok);
    ci->setWidth(width); // will display even with 0 width, as hairline

    // explicit 0 width, so hiding the line completely
    if (ok && width == 0)
        ci->setColor(QColor("transparent"));

    const char *style = ds.getTagArg("ls", 2);
    ci->setLineStyle(style[0] == 'd'
                      ? style[1] == 'a'
                         ? Qt::DashLine
                         : Qt::DotLine
                      : Qt::SolidLine);

    const char *text = ds.getTagArg("t", 0);
    ci->setText(text);

    const char *textPos = ds.getTagArg("t", 1);

    switch (textPos[0]) {
    case 'l': ci->setTextPosition(Qt::AlignLeft);   break;
    case 'r': ci->setTextPosition(Qt::AlignRight);  break;
    default:  ci->setTextPosition(Qt::AlignCenter); break;
    }

    ci->setTextColor(parseColor(ds.getTagArg("t", 2), QColor("#005030")));

    // no need for arrowheads on a bidirectional connection
    ci->setArrowEnabled(!twoWay);
    // only drawing the line from one side if the connection is bidirectional
    ci->setLineEnabled(!(twoWay && (gate->getOwnerModule() > destGate->getOwnerModule())));
}

void ConnectionItem::updateLineItem() {
    if (!lineEnabled || (dest == src)) { // not drawing the line if the conn would be 0 long
        lineItem->setPen(Qt::NoPen);
        return;
    }

    lineItem->setLine(QLineF(src, dest));
    QPen pen(lineColor, lineWidth);
    pen.setCapStyle(Qt::FlatCap);

    switch (lineStyle) {
    case Qt::DashLine:
        pen.setDashPattern(QVector<double>() << 2 << 2);
        break;
    case Qt::DotLine:
        pen.setDashPattern(QVector<double>() << 1 << 1);
        break;
    default:
        pen.setStyle(Qt::SolidLine);
    }

    pen.setDashOffset(dashOffset);

    lineItem->setPen(pen);
}

void ConnectionItem::updateTextItem() {
    textItem->setText(text);
    textItem->setBrush(textColor);

    QRectF textRect = textItem->boundingRect();
    QPointF textSize(textRect.width(), textRect.height());

    switch (textAlignment) {
    case Qt::AlignLeft:
        textItem->setPos(0.75 * src + 0.25 * dest - textSize * 0.5);
        break;
    case Qt::AlignRight:
        textItem->setPos(0.25 * src + 0.75 * dest - textSize * 0.5);
        break;
    default: // Center
        textItem->setPos(0.5 * src + 0.5 * dest
                          - QPoint(textSize.x() * 0.5,
                                   ((src.x()==dest.x()) ? (src.y()<dest.y()) : (src.x()<dest.x()))
                                    ? 0
                                    : textSize.y()));
    }
}

void ConnectionItem::updateArrowItem() {
    if (!arrowItem->isVisible() || (dest == src)) { // no arrow for a 0 long connection
        arrowItem->setVisible(false);
        return;
    }

    arrowItem->setVisible(true);
    arrowItem->setBrush(lineColor);
    arrowItem->setPen(lineColor);
    arrowItem->setEndPoints(dest, src);
}

ConnectionItem::ConnectionItem(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    lineItem = new QGraphicsLineItem(this);
    textItem = new OutlinedTextItem(this);
    //TODO arrowItem disappear when a part of lineItem is out of view.
    arrowItem = new GraphicsPathArrowItem(lineItem);
}

ConnectionItem::~ConnectionItem() {
    delete lineItem;
    delete textItem;
}

void ConnectionItem::setSource(const QPointF &source){
    if (src != source) {
        src = source;
        updateLineItem();
        updateTextItem();
        updateArrowItem();
    }
}

void ConnectionItem::setDestination(const QPointF &destination) {
    if (dest != destination) {
        dest = destination;
        updateLineItem();
        updateTextItem();
        updateArrowItem();
    }
}

void ConnectionItem::setLine(const QLineF &line) {
    setSource(line.p1());
    setDestination(line.p2());
}

void ConnectionItem::setWidth(double width) {
    if (width != lineWidth) {
        lineWidth = width;
        updateLineItem();
        updateArrowItem();
    }
}

void ConnectionItem::setColor(const QColor &color) {
    if (color != lineColor) {
        lineColor = color;
        updateLineItem();
        updateArrowItem();
    }
}

void ConnectionItem::setLineStyle(Qt::PenStyle style) {
    ASSERT2(style == Qt::SolidLine
             || style == Qt::DashLine
             || style == Qt::DotLine,
           "Unsupported line style");

    if (style != lineStyle) {
        lineStyle = style;
        updateLineItem();
    }
}

void ConnectionItem::setDashOffset(double offset) {
    if (dashOffset != offset) {
        dashOffset = offset;
        updateLineItem();
    }
}

void ConnectionItem::setText(const QString &text) {
    if (this->text != text) {
        this->text = text;
        updateTextItem();
    }
}

void ConnectionItem::setTextPosition(Qt::Alignment alignment) {
    ASSERT2(alignment == Qt::AlignLeft
             || alignment == Qt::AlignRight
             || alignment == Qt::AlignCenter,
            "Unsupported text alignment");

    if (textAlignment != alignment) {
        textAlignment = alignment;
        updateTextItem();
    }
}

void ConnectionItem::setTextColor(const QColor &color) {
    if (textColor != color) {
        textColor = color;
        updateTextItem();
    }
}

void ConnectionItem::setLineEnabled(bool enabled) {
    if (enabled != lineEnabled) {
        lineEnabled = enabled;
        updateLineItem();
    }
}

void ConnectionItem::setArrowEnabled(bool enabled) {
    if (enabled != arrowItem->isVisible()) {
        arrowItem->setVisible(enabled);
        updateLineItem();
        updateArrowItem();
    }
}

QRectF ConnectionItem::boundingRect() const{
    return QRectF(src, dest).normalized() // adjusting to make it easier to pick
            .adjusted(-clickThreshold, -clickThreshold, clickThreshold, clickThreshold);
}

void ConnectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // nothing to do here
}

// so the tooltip will not appear while hovering anywhere in the bounding rect, only on the line
QPainterPath ConnectionItem::shape() const {
    QPainterPath shape = lineItem->shape(); // even if it is not enabled, because it is most likely half of a bidirectional

    // expanding the path to make the connection easier to pick
    // thanks: http://stackoverflow.com/a/5732289
    QPainterPathStroker stroker;
    stroker.setWidth(1 + 2 * clickThreshold);
    stroker.setJoinStyle(Qt::RoundJoin);
    stroker.setCapStyle(Qt::RoundCap);
    return (stroker.createStroke(shape) + shape).simplified();
}

} // namespace qtenv
} // namespace omnetpp
