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
class QRubberBand;

namespace omnetpp {

class cModule;
class cGate;
class cObject;
class cComponent;

namespace qtenv {

class GraphicsLayer;
class CompoundModuleItem;
class SubmoduleItem;
class ConnectionItem;
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
    QRubberBand *rubberBand;
    QPoint rubberBandStartPos;

    std::map<cModule*,QPointF> submodPosMap;  // recalculateLayout() fills this map

    CompoundModuleItem *compoundModuleItem = nullptr;
    std::map<cModule*, SubmoduleItem*> submoduleGraphicsItems;
    std::map<cGate*, ConnectionItem*> connectionGraphicsItems;

    GraphicsLayer *backgroundLayer;
    GraphicsLayer *rangeLayer;
    GraphicsLayer *submoduleLayer;
    GraphicsLayer *figureLayer;
    GraphicsLayer *animationLayer;
    GraphicsLayer *bubbleLayer;
    GraphicsLayer *zoomLabelLayer;
    QGraphicsRectItem *nextEventMarker = nullptr;

    double zoomFactor = 1;
    double imageSizeFactor = 1;
    bool showModuleNames;
    bool showArrowHeads;

    ZoomLabel *zoomLabel;

    // does full layouting, stores results in submodPosMap
    void recalculateLayout();

    // drawing methods:
    void redrawFigures();
    void refreshFigures();
    void redrawModules();
    void redrawNextEventMarker();

    // updates submodPosMap (new modules, changed display strings, etc.)
    void refreshLayout();

    // helper for layouting code
    void getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeysLayout,
            double& x, double& y, double& sx, double& sy);

    void drawSubmodule(cModule *submod);
    void drawEnclosingModule(cModule *parentModule);
    void drawConnection(cGate *gate);

    void fillFigureRenderingHints(FigureRenderingHints *hints);

    void updateZoomLabelPos();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent *event) override; // for the alignTopLeft call in the PolishEvent
    void contextMenuEvent(QContextMenuEvent *event) override;
    void scrollContentsBy(int dx, int dy) override;

    QRectF getSubmodulesRect();

signals:
    void click(QMouseEvent*);
    void back();
    void forward();
    void doubleClick(QMouseEvent*);
    void contextMenuRequested(const std::vector<cObject*> &objects, const QPoint &globalPos);
    // the parameter will be the center of the view rectangle
    // (in scene coords, not in compound module coords, so scales with zoom)
    void dragged(QPointF);
    void marqueeZoom(QRectF);

public slots:
    void relayoutAndRedrawAll();

public:
    ModuleCanvasViewer();
    ~ModuleCanvasViewer();

    void setObject(cModule *obj);
    std::vector<cObject *> getObjectsAt(const QPoint &pos, int threshold = 4);
    std::vector<cObject *> getObjectsAt(const QRect &rect);

    GraphicsLayer *getAnimationLayer() { return animationLayer; }
    CanvasRenderer *getCanvasRenderer() { return canvasRenderer; }

    void redraw();
    void refresh(bool updateNextEventMarker = true);
    void recalcSceneRect(bool alignTopLeft = false);

    void setZoomFactor(double zoomFactor);
    void setImageSizeFactor(double imageSizeFactor);
    void setShowModuleNames(bool show);
    void setShowArrowHeads(bool show);

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

    void refreshSubmodules();
    void refreshConnections();

    void setZoomLabelVisible(bool visible);
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_MODULECANVASVIEWER_H
