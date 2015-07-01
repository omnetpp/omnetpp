//==========================================================================
//  MODULEGRAPHICSVIEW.H - part of
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

#ifndef MODULEGRAPHICSVIEW_H
#define MODULEGRAPHICSVIEW_H

#include <map>
#include <QPointF>
#include <QGraphicsView>

class QGraphicsPixmapItem;

namespace omnetpp {

class cModule;
class cGate;
class cObject;

namespace qtenv {

class ModuleGraphicsView : public QGraphicsView
{
    Q_OBJECT

private:
    cModule *object;
    int32_t layoutSeed;
    bool notDrawn;

    struct Point {double x,y;};
    typedef std::map<cModule*,Point> PositionMap;
    PositionMap submodPosMap;  // recalculateLayout() fills this map

    std::map<int, QGraphicsPixmapItem*> submoduleGraphicsItems;

    void recalculateLayout();
    void redrawFigures();
    void redrawModules();
    void redrawNextEventMarker();
    void redrawMessages();
    void refreshSubmodules();
    void adjustSubmodulesZOrder();

    void refreshLayout();
    void getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeysLayout,
            double& x, double& y, double& sx, double& sy);
    void drawSubmodule(cModule *submod, double x, double y);
    void drawEnclosingModule(cModule *parentModule);
    void drawConnection(cGate *gate);

    QPointF getSubmodCoords(cModule *mod);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent * event);

signals:
    void click(QMouseEvent*);
    void doubleClick(QMouseEvent*);
    void contextMenuRequested(QContextMenuEvent*);

public slots:
    void relayoutAndRedrawAll();

public:
    ModuleGraphicsView();

    void setObject(cModule *obj);
    cObject *getObjectAt(qreal x, qreal y);
    QList<cObject*> getObjectsAt(qreal x, qreal y);

    void clear();
};

} // namespace qtenv
} // namespace omnetpp

#endif // MODULEGRAPHICSVIEW_H
