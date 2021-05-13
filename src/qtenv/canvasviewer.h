//==========================================================================
//  CANVASVIEWER.H - part of
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

#ifndef __OMNETPP_QTENV_CANVASVIEWER_H
#define __OMNETPP_QTENV_CANVASVIEWER_H

#include <QtWidgets/QGraphicsView>
#include "qtenvdefs.h"

class QGraphicsPixmapItem;

namespace omnetpp {

class cCanvas;
class cObject;

namespace qtenv {

class CanvasRenderer;
struct FigureRenderingHints;
class GraphicsLayer;
class ZoomLabel;

class QTENV_API CanvasViewer : public QGraphicsView
{
    Q_OBJECT

private:
    cCanvas *object = nullptr;
    CanvasRenderer *canvasRenderer;
    QRectF textRect;

    GraphicsLayer *figureLayer;
    GraphicsLayer *zoomLabelLayer;

    ZoomLabel *zoomLabel;

    FigureRenderingHints makeFigureRenderingHints();
    void clear();
    void updateZoomLabelPos();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent * event) override;
    void resizeEvent(QResizeEvent *event) override;
    void scrollContentsBy(int dx, int dy) override;

signals:
    void click(QMouseEvent*);
    void contextMenuRequested(QContextMenuEvent*);

public:
    CanvasViewer();
    ~CanvasViewer();

    void setObject(cCanvas *obj);
    std::vector<cObject *> getObjectsAt(const QPoint &pos);

    CanvasRenderer *getCanvasRenderer() { return canvasRenderer; }
    void setZoomFactor(double zoomFactor);

    void redraw();
    void refresh();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_CANVASVIEWER_H
