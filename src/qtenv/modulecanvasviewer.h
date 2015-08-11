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

#ifndef __OMNETPP_QTENV_MODULECANVASVIEWER_H
#define __OMNETPP_QTENV_MODULECANVASVIEWER_H

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
class CompoundModuleItem;
class SubmoduleItem;
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

    CompoundModuleItem *compoundModuleItem = nullptr;
    std::map<int, SubmoduleItem*> submoduleGraphicsItems;

    GraphicsLayer *backgroundLayer;
    GraphicsLayer *rangeLayer;
    GraphicsLayer *submoduleLayer;
    GraphicsLayer *figureLayer;
    GraphicsLayer *animationLayer;
    QGraphicsRectItem *nextEventMarker = nullptr;

    double zoomFactor = 1;
    double imageSizeFactor = 1;

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

    void drawSubmodule(cModule *submod);
    void drawEnclosingModule(cModule *parentModule);
    void drawConnection(cGate *gate);

    void fillFigureRenderingHints(FigureRenderingHints *hints);

    void updateBackgroundColor();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent * event) override;
    // only used to draw the zoom factor label in the bottom right corner
    void drawForeground(QPainter *painter, const QRectF &rect) override;

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
    std::vector<cObject *> getObjectsAt(qreal x, qreal y);

    GraphicsLayer *getAnimationLayer() { return animationLayer; }
    CanvasRenderer *getCanvasRenderer() { return canvasRenderer; }

    void redraw();
    void refresh();

    void setZoomFactor(double zoomFactor);
    void setImageSizeFactor(double imageSizeFactor);

    void bubble(cComponent *subcomponent, const char *text);

    QPointF getSubmodCoords(cModule *mod);
    // also accepts the inspected module, returns the whole rectangle for it
    QRectF getSubmodRect(cModule *mod);
    QLineF getConnectionLine(cGate *gate);

    void clear();
    bool getNeedsRedraw() { return needs_redraw; }
    void setNeedsRedraw(bool isNeed = true) { needs_redraw = isNeed; }
    void setLayoutSeed(int32_t layoutSeed) { this->layoutSeed = layoutSeed; }
    void incLayoutSeed() { ++layoutSeed; }
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_MODULECANVASVIEWER_H
