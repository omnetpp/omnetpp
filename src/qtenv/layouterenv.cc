//==========================================================================
//  LAYOUTERENV.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Implementation of
//    inspectors
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include "omnetpp/cmodule.h"
#include "omnetpp/cdisplaystring.h"
#include "layouterenv.h"
#include "qtutil.h"
#include "qtenv.h"
#include <QtCore/QDebug>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsLineItem>

namespace omnetpp {
namespace qtenv {

QtenvGraphLayouterEnvironment::QtenvGraphLayouterEnvironment(cModule *parentModule, const cDisplayString& displayString, QGraphicsScene *scene)
    : parentModule(parentModule), displayString(displayString), scene(scene)
{
}

bool QtenvGraphLayouterEnvironment::getBoolParameter(const char *tagName, int index, bool defaultValue)
{
    return resolveBoolDispStrArg(displayString.getTagArg(tagName, index), parentModule, defaultValue);
}

long QtenvGraphLayouterEnvironment::getLongParameter(const char *tagName, int index, long defaultValue)
{
    return resolveLongDispStrArg(displayString.getTagArg(tagName, index), parentModule, defaultValue);
}

double QtenvGraphLayouterEnvironment::getDoubleParameter(const char *tagName, int index, double defaultValue)
{
    return resolveDoubleDispStrArg(displayString.getTagArg(tagName, index), parentModule, defaultValue);
}

void QtenvGraphLayouterEnvironment::showGraphics(const char *text)
{
    bbox = nextbbox;
    nextbbox = QRectF();
    auto item = new QGraphicsTextItem(text);
    item->setTextWidth(viewSize.width());
    item->setFont(getQtenv()->getCanvasFont());
    scene->addItem(item);
    scene->setSceneRect(scene->itemsBoundingRect());
    QApplication::processEvents();

    viewSize = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    for (auto v : scene->views())
        viewSize = viewSize.boundedTo(v->geometry().size());
}

void QtenvGraphLayouterEnvironment::drawText(double x, double y, const char *text, const char *tags, const char *color)
{
    scaleCoords(x, y);
    auto item = new QGraphicsSimpleTextItem(text);
    item->setPos(x, y);
    item->setBrush(parseColor(color));
    item->setFont(getQtenv()->getCanvasFont());
    scene->addItem(item);
}

void QtenvGraphLayouterEnvironment::drawLine(double x1, double y1, double x2, double y2, const char *tags, const char *color)
{
    scaleCoords(x1, y1);
    scaleCoords(x2, y2);
    auto item = new QGraphicsLineItem(x1, y1, x2, y2);
    item->setPen(parseColor(color));
    scene->addItem(item);
}

void QtenvGraphLayouterEnvironment::drawRectangle(double x1, double y1, double x2, double y2, const char *tags, const char *color)
{
    scaleCoords(x1, y1);
    scaleCoords(x2, y2);
    auto item = new QGraphicsRectItem(x1, y1, x2-x1, y2-y1);
    item->setPen(parseColor(color));
    scene->addItem(item);
}

bool QtenvGraphLayouterEnvironment::okToProceed()
{
    QApplication::processEvents();
    return !stopFlag;
}

void QtenvGraphLayouterEnvironment::scaleCoords(double& x, double& y)
{
    int vMargin = QFontMetrics(getQtenv()->getCanvasFont()).height()*2;
    int hMargin = 20;

    nextbbox = nextbbox.united(QRectF(x, y, 1, 1));
    x -= bbox.left();
    y -= bbox.top();
    float scale =std::min(1.0,
                          std::min((viewSize.width()-hMargin*2) / bbox.width(),
                                   (viewSize.height()-vMargin*2) / bbox.height()));
    x *= scale;
    y *= scale;
    x += hMargin;
    y += vMargin;
}

}  // namespace qtenv
}  // namespace omnetpp

