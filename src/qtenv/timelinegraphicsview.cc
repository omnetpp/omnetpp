//==========================================================================
//  TIMELINEGRAPHICSVIEW.CC - part of
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

#include "timelinegraphicsview.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsSimpleTextItem>
#include <QtGui/QMouseEvent>
#include "omnetpp/csimulation.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cdisplaystring.h"
#include "qtenv.h"
#include "inspectorutil.h"
#include "mainwindow.h"
#include "qtutil.h"
#include "graphicsitems.h"
#include "messageitem.h"

#include <QtCore/QDebug>

namespace omnetpp {
namespace qtenv {

TimeLineGraphicsView::TimeLineGraphicsView()
{
    messageLabelFont = tickLabelFont = getQtenv()->getTimelineFont();

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // TODO it can still be scrolled with the mouse however...
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

int TimeLineGraphicsView::getMinExponent()
{
    return minExponent;
}

void TimeLineGraphicsView::setMinExponent(int minExponent)
{
    this->minExponent = minExponent;
    update();
}

int TimeLineGraphicsView::getMaxExponent()
{
    return maxExponent;
}

void TimeLineGraphicsView::setMaxExponent(int maxExponent)
{
    this->maxExponent = maxExponent;
    update();
}

void TimeLineGraphicsView::setAdaptiveExponents(bool value)
{
    setAdaptiveMinExponent(value);
    setAdaptiveMaxExponent(value);
}

bool TimeLineGraphicsView::isAdaptiveMinExponent()
{
    return adaptiveMinExponent;
}

void TimeLineGraphicsView::setAdaptiveMinExponent(bool adaptiveMinExponent)
{
    this->adaptiveMinExponent = adaptiveMinExponent;
    update();
}

bool TimeLineGraphicsView::isAdaptiveMaxExponent()
{
    return adaptiveMaxExponent;
}

void TimeLineGraphicsView::setAdaptiveMaxExponent(bool adaptiveMaxExponent)
{
    this->adaptiveMaxExponent = adaptiveMaxExponent;
    update();
}

bool TimeLineGraphicsView::getDrawMinorTicks()
{
    return drawMinorTicks;
}

void TimeLineGraphicsView::setDrawMinorTicks(bool drawMinorTicks)
{
    this->drawMinorTicks = drawMinorTicks;
    update();
}

bool TimeLineGraphicsView::getShowTickLabels()
{
    return enableTickLabels;
}

void TimeLineGraphicsView::setShowTickLabels(bool enableTickLabels)
{
    this->enableTickLabels = enableTickLabels;
    update();
}

bool TimeLineGraphicsView::getShowMessageLabels()
{
    return enableMessageLabels;
}

void TimeLineGraphicsView::setShowMessageLabels(bool enableMessageLabels)
{
    this->enableMessageLabels = enableMessageLabels;
    update();
}

QFont TimeLineGraphicsView::getTickLabelFont()
{
    return tickLabelFont;
}

void TimeLineGraphicsView::setTickLabelFont(QFont tickLabelFont)
{
    this->tickLabelFont = tickLabelFont;
    update();
}

QFont TimeLineGraphicsView::getMessageLabelFont()
{
    return messageLabelFont;
}

void TimeLineGraphicsView::setMessageLabelFont(QFont messageLabelFont)
{
    this->messageLabelFont = messageLabelFont;
    update();
}

int TimeLineGraphicsView::getDefaultNumMessageLabelRows()
{
    return defaultNumMessageLabelRows;
}

void TimeLineGraphicsView::setDefaultNumMessageLabelRows(int defaultNumMessageLabelRows)
{
    this->defaultNumMessageLabelRows = defaultNumMessageLabelRows;
}

QVector<cModule *> TimeLineGraphicsView::getModulePathsMessageFilter()
{
    return modules;
}

void TimeLineGraphicsView::setModulePathsMessageFilter(QVector<cModule *> modules)
{
    this->modules = modules;
}

QVector<cMessage *> TimeLineGraphicsView::getMessages()
{
    QVariant variant = getQtenv()->getPref("timeline-maxnumevents");
    int maxNum = variant.isValid() ? variant.value<int>() : 10;
    variant = getQtenv()->getPref("timeline-wantselfmsgs");
    bool wantSelfMsgs = variant.isValid() ? variant.value<bool>() : true;
    variant = getQtenv()->getPref("timeline-wantnonselfmsgs");
    bool wantNonSelfMsgs = variant.isValid() ? variant.value<bool>() : true;
    variant = getQtenv()->getPref("timeline-wantsilentmsgs");
    bool wantSilentMsgs = variant.isValid() ? variant.value<bool>() : true;

    cFutureEventSet *fes = getSimulation()->getFES();
    fes->sort();
    int fesLen = fes->getLength();
    QVector<cMessage *> messages;
    for (int i = 0; maxNum > 0 && i < fesLen; i++, maxNum--) {
        cEvent *event = fes->get(i);
        if (!event->isMessage())
            continue;
        else {
            cMessage *msg = (cMessage *)event;
            if (msg->isSelfMessage() ? !wantSelfMsgs : !wantNonSelfMsgs)
                continue;
            if (!wantSilentMsgs && getQtenv()->isSilentEvent(msg))
                continue;
        }
        messages.push_back(static_cast<cMessage *>(event));
    }

    return messages;
}

void TimeLineGraphicsView::resizeEvent(QResizeEvent *event)
{
    rebuildScene();
}

void TimeLineGraphicsView::updateMinExponent(simtime_t now, QVector<cMessage *> messages)
{
    if (messages.size() == 0)
        return;  // nothing to do

    // find the smallest timestamp which is greater than "now" (ignore equal or
    // smaller ones, because they don't yield a finite log10(delta)),
    // and ensure minExponent is small enough to accommodate it.
    // Note that messages[] is ordered by timestamp.
    simtime_t currentMinTime;
    bool isSetCurrentMinTime = false;
    for (cMessage *msg : messages)
        if (msg->getArrivalTime() > now) {
            currentMinTime = msg->getArrivalTime();
            isSetCurrentMinTime = true;
            break;
        }

    if (isSetCurrentMinTime) {
        double currentMinDelta = (currentMinTime - now).dbl();
        int currentMinExponent = (int)std::floor(log10(currentMinDelta));
        if (minExponent > currentMinExponent)
            minExponent = currentMinExponent;
    }
}

void TimeLineGraphicsView::updateMaxExponent(simtime_t now, QVector<cMessage *> messages)
{
    if (messages.size() == 0)
        return;  // nothing to do

    // find the largest timestamp, and ensure maxExponent is large enough to accommodate it
    simtime_t currentMaxTime = messages[messages.size()-1]->getArrivalTime();
    if (currentMaxTime > now) {
        double currentMaxDelta = (currentMaxTime - now).dbl();
        int currentMaxExponent = (int)std::ceil(log10(currentMaxDelta));
        if (maxExponent < currentMaxExponent)
            maxExponent = currentMaxExponent;
    }
}

int TimeLineGraphicsView::getXForExponent(double exp)
{
    exp = std::max((double)minExponent, std::min((double)maxExponent, exp));
    int x = minExponentX + (int)((maxExponentX-minExponentX) * (exp - minExponent) / (maxExponent - minExponent));  // map [min,max] to [margin,width-margin]
    return x;
}

QString TimeLineGraphicsView::getTickLabel(int mantissa, int exp)
{
    QString unit;
    if (exp < -15) {unit = "as"; exp += 18;}
    else if (exp < -12) {unit = "fs"; exp += 15;}
    else if (exp < -9) {unit = "ps"; exp += 12;}
    else if (exp < -6) {unit = "ns"; exp += 9;}
    else if (exp < -3) {unit = "us"; exp += 6;}
    else if (exp < 0) {unit = "ms"; exp += 3;}
    else unit = "s";
    QString label = QString("+")
            + (abs(exp) <= 3
                  ? QString::fromStdString(SimTime(mantissa, (SimTimeUnit)exp).str())
                  : QString("%1e%2").arg(mantissa).arg(exp))
            + unit;
    return label;
}

int TimeLineGraphicsView::getXForTimeDelta(simtime_t delta)
{
    return getXForExponent(log10(delta.dbl()));
}

QVector<int> TimeLineGraphicsView::computeMessageCoordinates(simtime_t now, QVector<cMessage *> messages)
{
    // count how many messages are exactly for "now"
    int i = 0;
    for (i = 0; i < messages.size() && messages[i]->getArrivalTime() == now; i++)
        ;
    int numNow = i;

    // place those messages in the zero-delta (+0s) area
    QVector<int> coords;
    double dx = std::min(5.0, (zeroEndX-zeroStartX) / (double)numNow);  // spacing: depending on available space, but maximum 5 pixels
    double x = (zeroStartX+zeroEndX)/2 - (numNow-1)*dx/2;
    for (i = 0; i < numNow; i++, x += dx)
        coords.push_back((int)x);

    // place other messages on the axis
    double smallestExp = (numNow < messages.size()) ? log10((messages[numNow]->getArrivalTime() - now).dbl()) : nan("")  /*unused*/;
    double largestExp = (messages.size() > 0) ? log10((messages[messages.size()-1]->getArrivalTime() - now).dbl()) : nan("")  /*unused*/;

    for (i = numNow; i < messages.size(); i++) {
        simtime_t delta = messages[i]->getArrivalTime() - now;
        double exp = log10(delta.dbl());
        if (exp < minExponent) {
            // out of range (too small): map to dotted line segment between arrowStartX and minExponentX
            coords.push_back(arrowStartX + (int)((minExponentX - arrowStartX) * (exp - smallestExp) / (minExponent - smallestExp)));
        }
        else if (exp > maxExponent) {
            // out of range (too large): map to dotted line segment between maxExponentX and arrowEndX
            coords.push_back(maxExponentX + (int)((arrowEndX - maxExponentX) * (exp - maxExponent) / (largestExp - maxExponent)));
        }
        else {
            // normal: map to [minExponentX,maxExponentX]
            coords.push_back(getXForExponent(exp));
        }
    }
    return coords;
}

void TimeLineGraphicsView::paintMessages(QVector<cMessage *> messages, bool wantHighlighted, int messageLabelHeight, int numRows)
{
    QVector<int> labelRowLastX;  // right edge of last label written in each row, with [0] being the bottom and [numRows-1] the top line
    for (int i = 0; i < numRows; i++)
        labelRowLastX.push_back(INT_MIN);
    int bottomLabelRowY = axisY-3 - messageLabelHeight;

    QFontMetrics fontMetrics(tickLabelFont);

    // storing this, so we can append a … to it if the label of the next message did not fit
    QGraphicsSimpleTextItem *lastItem = nullptr;
    // after appending, we'll also have to adjust the labelRowLastX entry
    int lastItemRow = -1;

    for (int i = 0; i < messages.size(); i++) {
        cMessage *msg = messages[i];

        // filter: draw either only the highlighted ones or the not highlighted ones
        bool isHighlighted = modules.size() == 0 || matches(msg);
        if (isHighlighted != wantHighlighted)
            continue;

        // draw symbol
        int x = messageXCoords[i];
        drawMessageSymbol(msg, isHighlighted, x, axisY);

        // draw label
        if (enableMessageLabels && numRows > 0) {
            QString label = msg->getFullName();
            int labelWidth = fontMetrics.boundingRect(label).width();
            int labelX = x - labelWidth/2;
            int row;
            for (row = 0; row < numRows; row++)
                if (labelRowLastX[row] < labelX)
                    break;

            if (row < numRows) {
                QBrush brush(Qt::SolidPattern);
                QColor color = palette().color(QPalette::Active, QPalette::WindowText);
                color.setAlpha(isHighlighted ? 255 : 40);
                brush.setColor(color);

                QGraphicsSimpleTextItem *simpleTextItem = new QGraphicsSimpleTextItem(label);
                simpleTextItem->setFont(messageLabelFont);
                simpleTextItem->setBrush(brush);
                simpleTextItem->setPos(labelX, bottomLabelRowY - row*messageLabelHeight);
                simpleTextItem->setData(ITEMDATA_COBJECT, QVariant::fromValue(msg));

                scene()->addItem(simpleTextItem);
                labelRowLastX[row] = labelX + labelWidth;
                lastItem = simpleTextItem;
                lastItemRow = row;
            }
            else {
                // this item did not fit, so appending an ellipsis to the label of the previously drawn one
                if (lastItem) {
                    // we are not repositioning because of this change...
                    // (that would make this problem way less trivial, we'd have to backtrack and whatnot...)
                    labelRowLastX[lastItemRow] -= lastItem->boundingRect().width();
                    lastItem->setText(lastItem->text() + QString("…"));
                    labelRowLastX[lastItemRow] += lastItem->boundingRect().width();
                    lastItem = nullptr; // just so we don't append more ellipses to it
                }
            }
        }
    }
}

bool TimeLineGraphicsView::matches(cMessage *object)
{
    cMessage *msg = object;
    cModule *module = msg->getArrivalModule();
    QString moduleFullPath = module->getFullPath().c_str();

    // check if moduleFullPath is the same (or prefix of) one of the filter module paths
    for (cModule *mod : modules) {
        // try to avoid calling String.startsWith(), so first check last-but-one char, as that is
        // likely to differ (it's the last digit of the index if filter ends in a vector submodule)
        QString filterModulePath = mod->getFullPath().c_str();
        int filterModulePathLength = filterModulePath.length();
        if (moduleFullPath.length() >= filterModulePathLength)
            if (filterModulePathLength < 2 || moduleFullPath[filterModulePathLength-2] == filterModulePath[filterModulePathLength-2])
                if (moduleFullPath.startsWith(filterModulePath))  // this is the most expensive operation
                    if (moduleFullPath.length() == filterModulePathLength || moduleFullPath[filterModulePathLength] == '.')
                        return true;

    }
    return false;
}

void TimeLineGraphicsView::drawMessageSymbol(cMessage *message, bool active, int x, int y)
{
    QColor brushColor = MessageItemUtil::getColorForMessageKind(message->getKind());
    QColor penColor = brushColor;

    brushColor.setAlpha(active ? 255 : 40);
    penColor.setAlpha(active ? 255 : 40);

    // make the outline color stand out a bit if the message color is too close to that of the background
    QColor textColor = QApplication::palette().windowText().color();
    QColor baseColor = QApplication::palette().window().color();

    if (std::abs(baseColor.valueF() - brushColor.valueF()) < 0.1) {
        penColor.setRed(  penColor.red()   + (textColor.red()   - penColor.red())   / 3);
        penColor.setGreen(penColor.green() + (textColor.green() - penColor.green()) / 3);
        penColor.setBlue( penColor.blue()  + (textColor.blue()  - penColor.blue())  / 3);
    }

    QGraphicsItem *ellipse = scene()->addEllipse(x-2, y-4, 5, 9, QPen(penColor, 2), QBrush(brushColor));
    ellipse->setData(ITEMDATA_COBJECT, QVariant::fromValue(message));

    QString toolTip = QString("(") + getObjectShortTypeName(message) + ") " + message->getFullName();
    if (!message->str().empty())
        toolTip += QString(", ") + message->str().c_str();
    ellipse->setToolTip(toolTip);
}

void TimeLineGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        QVector<cObject *> objects = getObjectsUnderCursor(mapToScene(event->pos()));
        Q_EMIT contextMenuRequested(objects, mapToGlobal(event->pos()));
    }
    else if (event->button() == Qt::LeftButton) {
        QVector<cObject *> objects = getObjectsUnderCursor(mapToScene(event->pos()));
        if (objects.size() > 0)
            Q_EMIT click(objects[0]);
    }
}

void TimeLineGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QVector<cObject *> objects = getObjectsUnderCursor(mapToScene(event->pos()));
        if (objects.size() != 0)
            Q_EMIT doubleClick(objects[0]);
    }
}

QVector<cObject *> TimeLineGraphicsView::getObjectsUnderCursor(QPointF pos)
{
    // cursor position plus minus 2 pixel
    QRectF rect(pos.x() - 2, pos.y() - 2, 5, 5);
    QList<QGraphicsItem *> items = scene()->items(rect);

    QVector<cObject *> objects;
    for (auto item : items) {
        cObject *object = item->data(ITEMDATA_COBJECT).value<cMessage *>();
        if (object && !objects.contains(object))
            objects.push_back(object);
    }
    return objects;
}

double TimeLineGraphicsView::getInitHeight()
{
    // similar to the calculation of showTickLabels in rebuildScene method
    QGraphicsSimpleTextItem simpleText("+100ms");
    simpleText.setFont(tickLabelFont);
    // don't add 16 thus tick labels aren't shown
    return simpleText.boundingRect().height();
}

void TimeLineGraphicsView::rebuildScene()
{
    setSceneRect(0, contentsRect().height() * (2/3), contentsRect().width(), contentsRect().height());

    if (scene() == nullptr)
        qDebug() << "TimeLineGraphicsView::rebuildScene: scene must be set";

    scene()->clear();
    simtime_t now = getSimulation()->getSimTime();

    QVector<cMessage *> messages = getMessages();
    setRenderHints(QPainter::Antialiasing);

    QRectF r = rect();
    setFont(tickLabelFont);
    QGraphicsSimpleTextItem simpleText("+100ms");
    simpleText.setFont(tickLabelFont);
    QRectF maxTickLabelSize = simpleText.boundingRect();
    int tickLabelHeight = maxTickLabelSize.height();

    bool showTickLabels = enableTickLabels && (r.height() >= tickLabelHeight+16);
    // the -10 is a margin at the bottom, might want to correct later
    axisY = showTickLabels ? r.height() - tickLabelHeight - 10 : (r.height()-1)/2;
    zeroStartX = maxTickLabelSize.width()/2;
    zeroEndX = zeroStartX + 20;
    arrowStartX = zeroEndX + 20;
    arrowEndX = r.width() - 8;
    minExponentX = arrowStartX + maxTickLabelSize.width();
    maxExponentX = arrowEndX - maxTickLabelSize.width();

    // adaptive modification of displayed exponent range
    if (isAdaptiveMinExponent())
        updateMinExponent(now, messages);
    if (isAdaptiveMaxExponent())
        updateMaxExponent(now, messages);

    // draw axis
    QPen pen;
    pen.setColor(palette().color(QPalette::Active, QPalette::WindowText));
    pen.setStyle(Qt::DotLine);
    scene()->addLine(arrowStartX, axisY, minExponentX, axisY, pen);
    scene()->addLine(maxExponentX, axisY, arrowEndX, axisY, pen);
    pen.setStyle(Qt::SolidLine);
    scene()->addLine(minExponentX, axisY, maxExponentX, axisY, pen);
    scene()->addLine(arrowEndX-7, axisY-2, arrowEndX, axisY, pen);
    scene()->addLine(arrowEndX-7, axisY+2, arrowEndX, axisY, pen);

    // draw zero delta tray
    scene()->setBackgroundBrush(palette().color(QPalette::Active, QPalette::Window));
    int h = 5;
    scene()->addLine(zeroStartX-3, axisY-h, zeroStartX-3, axisY+h, pen);
    scene()->addLine(zeroEndX+3, axisY-h, zeroEndX+3, axisY+h, pen);
    scene()->addLine(zeroStartX-3, axisY-h, zeroStartX-1, axisY-h, pen);
    scene()->addLine(zeroStartX-3, axisY+h, zeroStartX-1, axisY+h, pen);
    scene()->addLine(zeroEndX+3, axisY-h, zeroEndX+1, axisY-h, pen);
    scene()->addLine(zeroEndX+3, axisY+h, zeroEndX+1, axisY+h, pen);

    // draw ticks
    int tickSpacing = (maxExponentX-minExponentX) / (maxExponent-minExponent);
    bool reduceTickLabels = tickSpacing < maxTickLabelSize.width()+10;  // if there's not enough space, we only write out 1ns, 1us, 1ms, 1s,1000s etc.
    for (int exp = std::max(minExponent-1, simtime_t::getScaleExp()); exp <= maxExponent; exp++) {
        if (exp >= minExponent) {
            // tick
            int x = getXForExponent(exp);
            scene()->addLine(x, axisY-4, x, axisY+4, pen);

            // label
            if (showTickLabels && (!reduceTickLabels || exp % 3 == 0)) {
                QString label = getTickLabel(1, exp);
                simpleText.setText(label);
                int labelWidth = simpleText.boundingRect().width();

                QGraphicsSimpleTextItem *simpleTextItem = new QGraphicsSimpleTextItem(label);
                simpleTextItem->setBrush(palette().color(QPalette::Active, QPalette::WindowText));
                simpleTextItem->setFont(tickLabelFont);
                simpleTextItem->setPos(x - labelWidth/2, axisY+5);
                scene()->addItem(simpleTextItem);
            }
        }

        // exp < maxExponent, because there is no need to draw minor ticks past the last major tick
        if (drawMinorTicks && exp < maxExponent)
            for (int i = 2; i <= 9; i++) {
                int x = getXForTimeDelta(SimTime(i, (SimTimeUnit)exp));
                scene()->addLine(x, axisY-2, x, axisY+2, pen);
            }
    }

    if (showTickLabels) {
        QString label = "+0s";
        simpleText.setText(label);
        int labelWidth = simpleText.boundingRect().width();

        QGraphicsSimpleTextItem *simpleTextItem = new QGraphicsSimpleTextItem(label);
        simpleTextItem->setBrush(palette().color(QPalette::Active, QPalette::WindowText));
        simpleTextItem->setFont(tickLabelFont);
        simpleTextItem->setPos((zeroEndX+zeroStartX)/2 - labelWidth/2, axisY+5);
        scene()->addItem(simpleTextItem);
    }

    messageXCoords = computeMessageCoordinates(now, messages);

    // paint messages
    scene()->setFont(messageLabelFont);
    simpleText.setFont(messageLabelFont);
    simpleText.setText("Lj");
    int messageLabelHeight = simpleText.boundingRect().height();
    int numRows = (axisY-3) / messageLabelHeight;  // we can place message labels in multiple rows, depending on available space

    if (modules.size())
        paintMessages(messages, false, messageLabelHeight, numRows);  // draw non-highlighted (alpha) msgs first
    paintMessages(messages, true, messageLabelHeight, numRows);
}

}  // namespace qtenv
}  // namespace omnetpp

