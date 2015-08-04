//==========================================================================
//  MODULECANVASVIEWER.H - part of
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

#ifndef MODULECANVASVIEWER_H
#define MODULECANVASVIEWER_H

#include "submoduleitem.h"

#include <map>
#include <QPointF>
#include <QGraphicsView>

class QGraphicsPixmapItem;

namespace omnetpp {

class cModule;
class cGate;
class cObject;
class cComponent;

namespace qtenv {

class GraphicsLayer;
struct FigureRenderingHints;
class CanvasRenderer;

class ModuleCanvasViewer : public QGraphicsView
{
    Q_OBJECT

private:
    cModule *object;
    int32_t layoutSeed;
    bool notDrawn;
    bool needs_redraw;
    CanvasRenderer *canvasRenderer;

    typedef std::map<cModule*,QPointF> PositionMap;
    PositionMap submodPosMap;  // recalculateLayout() fills this map

    std::map<int, SubmoduleItem*> submoduleGraphicsItems;

    GraphicsLayer *backgroundLayer;
    GraphicsLayer *rangeLayer;
    GraphicsLayer *submoduleLayer;
    GraphicsLayer *figureLayer;
    GraphicsLayer *animationLayer;
    QGraphicsRectItem *rectangleItem = nullptr;
    QGraphicsRectItem *nextEventMarker = nullptr;

private:
    // does full layouting, stores results in submodPosMap
    void recalculateLayout();

    // drawing methods:
    void redrawFigures();
    void refreshFigures();
    void redrawModules();
    void redrawNextEventMarker();
    void refreshSubmodules();
    void adjustSubmodulesZOrder();

    // updates submodPosMap (new modules, changed display strings, etc.)
    void refreshLayout();

    // helper for layouting code
    void getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeysLayout,
            double& x, double& y, double& sx, double& sy);

    void drawSubmodule(cModule *submod, double x, double y);
    void drawEnclosingModule(cModule *parentModule);
    void drawConnection(cGate *gate);

    void fillFigureRenderingHints(FigureRenderingHints *hints);

    void updateBackgroundColor();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent * event);

signals:
    void click(QMouseEvent*);
    void back();
    void forward();
    void doubleClick(QMouseEvent*);
    void contextMenuRequested(QContextMenuEvent*);

public slots:
    void relayoutAndRedrawAll();

public:
    ModuleCanvasViewer();
    ~ModuleCanvasViewer();

    void setObject(cModule *obj);
    cObject *getObjectAt(qreal x, qreal y);
    QList<cObject*> getObjectsAt(qreal x, qreal y);

    GraphicsLayer *getAnimationLayer() { return animationLayer; }
    CanvasRenderer *getCanvasRenderer() { return canvasRenderer; }

    void redraw();
    void refresh();

    void bubble(cComponent *subcomponent, const char *text);

    QPointF getSubmodCoords(cModule *mod);
    QPointF getMessageEndPos(const QPointF &src, const QPointF &dest);

    void clear();
    bool getNeedsRedraw() { return needs_redraw; }
    void setNeedsRedraw(bool isNeed = true) { needs_redraw = isNeed; }
    void setLayoutSeed(int32_t layoutSeed) { this->layoutSeed = layoutSeed; }
    void incLayoutSeed() { ++layoutSeed; }
};

} // namespace qtenv
} // namespace omnetpp

#endif // MODULEGRAPHICSVIEW_H
