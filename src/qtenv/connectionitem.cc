//==========================================================================
//  CONNECTIONITEM.CC - part of
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

#include "connectionitem.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <omnetpp/cgate.h>
#include <omnetpp/cchannel.h>
#include "graphicsitems.h"
#include "qtutil.h"

namespace omnetpp {
namespace qtenv {

void ConnectionItemUtil::setupFromDisplayString(ConnectionItem *ci, cGate *gate, bool showArrowhead)
{
    cChannel *chan = gate->getChannel();

    cDisplayString ds = chan && chan->hasDisplayString() && chan->parametersFinalized()
            ? chan->getDisplayString()
            : cDisplayString();

    // replacing $param args with the actual parameter values
    std::string buffer;
    ds = substituteDisplayStringParamRefs(ds, buffer, chan, true);

    ci->setData(ITEMDATA_COBJECT, QVariant::fromValue((cObject *)gate));
    ci->setData(ITEMDATA_TOOLTIP, ds.getTagArg("tt", 0));

    ci->setColor(parseColor(ds.getTagArg("ls", 0), QColor("black")));

    bool ok;
    double width = QString(ds.getTagArg("ls", 1)).toDouble(&ok);
    ci->setWidth(width);  // will display even with 0 width, as hairline

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

    bool twoWay = isTwoWayConnection(gate);

    // no need for arrowheads on a bidirectional connection
    ci->setArrowEnabled(!twoWay && showArrowhead);
    // only drawing the line from one side if the connection is bidirectional
    ci->setHalfLength(twoWay);
}

void ConnectionItem::updateLineItem()
{
    if (!lineEnabled || (dest == src)) {  // not drawing the line if the conn would be 0 long
        lineItem->setPen(Qt::NoPen);
        return;
    }

    // shadowing with a local, so we can adjust for half length
    QPointF dest = this->dest;

    QPointF dir = dest - src;
    double length = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());

    if (halfLength)
        dest = src + dir / 2;
    else if (arrowItem->isVisible()) {
        // making the end not stick out of the arrowhead
        // the line itself has to be shorter
        dest -= dir / length * lineWidth;
    }

    lineItem->setLine(QLineF(src, dest));
    QPen pen(lineColor, lineWidth);
    pen.setCapStyle(Qt::FlatCap);

    switch (lineStyle) {
        case Qt::DashLine: pen.setDashPattern(QVector<double>() << 2 << 2); break;
        case Qt::DotLine: pen.setDashPattern(QVector<double>() << 1 << 1); break;
        default: pen.setStyle(Qt::SolidLine);
    }

    pen.setDashOffset(dashOffset);

    lineItem->setPen(pen);
}

void ConnectionItem::updateTextItem()
{
    textItem->setText(text);
    textItem->setBrush(textColor);
    textItem->setVisible(isVisible());

    QRectF textRect = textItem->boundingRect();
    QPointF textSize(textRect.width() + 4, textRect.height() + 4);

    switch (textAlignment) {
        case Qt::AlignLeft:
            textItem->setPos(0.75 * src + 0.25 * dest - textSize * 0.5 + QPoint(2, 2));
            textItem->setAlignment(Qt::AlignLeft);
            break;
        case Qt::AlignRight:
            textItem->setPos(0.25 * src + 0.75 * dest - textSize * 0.5 + QPoint(2, 2));
            textItem->setAlignment(Qt::AlignRight);
            break;
        default: // Center
            textItem->setPos(0.5 * src + 0.5 * dest
                              - QPoint(textSize.x() * 0.5,
                                       ((src.x()==dest.x()) ? (src.y()<dest.y()) : (src.x()<dest.x()))
                                        ? 0
                                        : textSize.y()) + QPoint(2, 2));
            textItem->setAlignment(Qt::AlignCenter);
    }
}

void ConnectionItem::updateArrowItem()
{
    if (!arrowItem->isVisible() || (dest == src)) {  // no arrow for a 0 long connection
        arrowItem->setVisible(false);
        return;
    }

    arrowItem->setVisible(true);
    arrowItem->setColor(lineColor);
    arrowItem->setEndPoints(src, dest);
    arrowItem->setSizeForPenWidth(lineWidth);
    arrowItem->setLineWidth(lineWidth);
}

ConnectionItem::ConnectionItem(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    lineItem = new QGraphicsLineItem(this);
    // The text has to be a sibling, otherwise the pair line
    // of a twoway connection would obscure it.
    textItem = new MultiLineOutlinedTextItem(parentItem());
    textItem->setZValue(1); // connect is to update visibility
    connect(this, SIGNAL(visibleChanged()), this, SLOT(updateTextItem()));
    // TODO arrowItem disappear when a part of lineItem is out of view.
    arrowItem = new ArrowheadItem(lineItem);
}

ConnectionItem::~ConnectionItem()
{
    delete lineItem;
    delete textItem;
}

void ConnectionItem::setSource(const QPointF& source)
{
    if (src != source) {
        src = source;
        updateTextItem();
        updateArrowItem();
        updateLineItem();
    }
}

void ConnectionItem::setDestination(const QPointF& destination)
{
    if (dest != destination) {
        dest = destination;
        updateTextItem();
        updateArrowItem();
        updateLineItem();
    }
}

void ConnectionItem::setLine(const QLineF& line)
{
    setSource(line.p1());
    setDestination(line.p2());
}

void ConnectionItem::setWidth(double width)
{
    if (width != lineWidth) {
        lineWidth = width;
        updateArrowItem();
        updateLineItem();
    }
}

void ConnectionItem::setColor(const QColor& color)
{
    if (color != lineColor) {
        lineColor = color;
        updateArrowItem();
        updateLineItem();
    }
}

void ConnectionItem::setLineStyle(Qt::PenStyle style)
{
    ASSERT2(style == Qt::SolidLine
             || style == Qt::DashLine
             || style == Qt::DotLine,
           "Unsupported line style");

    if (style != lineStyle) {
        lineStyle = style;
        updateLineItem();
    }
}

void ConnectionItem::setDashOffset(double offset)
{
    if (dashOffset != offset) {
        dashOffset = offset;
        updateLineItem();
    }
}

void ConnectionItem::setText(const QString& text)
{
    if (this->text != text) {
        this->text = text;
        updateTextItem();
    }
}

void ConnectionItem::setTextPosition(Qt::Alignment alignment)
{
    ASSERT2(alignment == Qt::AlignLeft
             || alignment == Qt::AlignRight
             || alignment == Qt::AlignCenter,
            "Unsupported text alignment");

    if (textAlignment != alignment) {
        textAlignment = alignment;
        updateTextItem();
    }
}

void ConnectionItem::setTextBackgroundColor(const QColor& color)
{
    textItem->setBackgroundBrush(color);
}

void ConnectionItem::setTextOutlineColor(const QColor& color)
{
    textItem->setPen(color);
}

void ConnectionItem::setTextColor(const QColor& color)
{
    if (textColor != color) {
        textColor = color;
        updateTextItem();
    }
}

void ConnectionItem::setLineEnabled(bool enabled)
{
    if (enabled != lineEnabled) {
        lineEnabled = enabled;
        updateLineItem();
    }
}

void ConnectionItem::setArrowEnabled(bool enabled)
{
    if (enabled != arrowItem->isVisible()) {
        arrowItem->setVisible(enabled);
        updateArrowItem();
        updateLineItem();
    }
}

bool ConnectionItem::isArrowEnabled() const
{
    return arrowItem->isVisible();
}

void ConnectionItem::setHalfLength(bool enabled)
{
    if (enabled != halfLength) {
        halfLength = enabled;
        updateLineItem();
    }
}

}  // namespace qtenv
}  // namespace omnetpp

