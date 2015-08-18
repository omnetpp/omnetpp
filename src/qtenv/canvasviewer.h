//==========================================================================
//  CANVASVIEWER.H - part of
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


#ifndef CANVASVIEWER_H
#define CANVASVIEWER_H

#include <QGraphicsView>

class QGraphicsPixmapItem;

namespace omnetpp {

class cCanvas;
class cObject;

namespace qtenv {

class CanvasRenderer;
class FigureRenderingHints;
class GraphicsLayer;

class CanvasViewer : public QGraphicsView
{
    Q_OBJECT

private:
    cCanvas *object;
    CanvasRenderer *canvasRenderer;
    double zoomFactor = 1;

    GraphicsLayer *figureLayer;

    // does full layouting, stores results in submodPosMap
    void recalculateLayout();

    // updates submodPosMap (new modules, changed display strings, etc.)
    void refreshLayout();

    void fillFigureRenderingHints(FigureRenderingHints *hints); ///

    void updateBackgroundColor(); ///
    void clear();

protected:
    void mousePressEvent(QMouseEvent *event) override; ///
    void contextMenuEvent(QContextMenuEvent * event) override; ///

signals:
    void click(QMouseEvent*); ///
    void contextMenuRequested(QContextMenuEvent*); ///

public slots:
    void relayoutAndRedrawAll();

public:
    CanvasViewer(); ///
    ~CanvasViewer(); ///

    void setObject(cCanvas *obj); ///
    std::vector<cObject *> getObjectsAt(const QPoint &pos); ///

    CanvasRenderer *getCanvasRenderer() { return canvasRenderer; } ///

    void redraw(); ///
    void refresh(); ///

    void setZoomFactor(double zoomFactor); ///

//    void setLayoutSeed(int32_t layoutSeed) { this->layoutSeed = layoutSeed; }
//    void incLayoutSeed() { ++layoutSeed; }
};

} // namespace qtenv
} // namespace omnetpp

#endif // CANVASVIEWER_H
