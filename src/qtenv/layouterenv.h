//==========================================================================
//  LAYOUTERENV.H - part of
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

#ifndef __OMNETPP_QTENV_LAYOUTERENV_H
#define __OMNETPP_QTENV_LAYOUTERENV_H

#include "layout/graphlayouter.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtCore/QObject>
#include <climits>
#include "qtenvdefs.h"

namespace omnetpp {

using namespace layout;

class cModule;
class cDisplayString;

namespace qtenv {

class QTENV_API QtenvGraphLayouterEnvironment : public QObject, public GraphLayouterEnvironment
{
    Q_OBJECT

protected:
    // configuration
    cModule *parentModule;
    const cDisplayString& displayString;
    QGraphicsScene *scene;

    // state
    bool stopFlag = false;

    // the smallest size of all the views in both dimensions
    QSize viewSize = QSize(1000, 1000);
    // bbox is used to scale the item coords to fit the viewport
    // and nextbbox is the union of all the unscaled coords drawn
    // in the current "frame", and will be used to scale the next one
    QRectF bbox, nextbbox;

    // the regular scene transformation methods distort text, so we have
    // to scale the coords ourselves.
    void scaleCoords(double &x, double &y);

public slots:
    void stop() { stopFlag = true; }

public:
    QtenvGraphLayouterEnvironment(cModule *parentModule, const cDisplayString& displayString, QGraphicsScene *scene);

    virtual bool inspected() override { return scene; }
    virtual bool okToProceed() override;

    virtual bool getBoolParameter(const char *tagName, int index, bool defaultValue) override;
    virtual long getLongParameter(const char *tagName, int index, long defaultValue) override;
    virtual double getDoubleParameter(const char *tagName, int index, double defaultValue) override;

    virtual void clearGraphics() override { scene->clear(); }
    virtual void showGraphics(const char *text) override;
    virtual void drawText(double x, double y, const char *text, const char *tags, const char *color) override;
    virtual void drawLine(double x1, double y1, double x2, double y2, const char *tags, const char *color) override;
    virtual void drawRectangle(double x1, double y1, double x2, double y2, const char *tags, const char *color) override;
};

}  // namespace qtenv
}  // namespace omnetpp


#endif
