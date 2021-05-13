//==========================================================================
//  MODULECANVASVIEWER.H - part of
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

#ifndef __OMNETPP_QTENV_MODULECANVASVIEWER_H
#define __OMNETPP_QTENV_MODULECANVASVIEWER_H

#include <map>
#include <QtCore/QPointF>
#include <QtWidgets/QGraphicsView>
#include <unordered_set>
#include <QtPrintSupport/QPrinter>
#include "qtenvdefs.h"
#include "submoduleitem.h"

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
class ZoomLabel;

class QTENV_API ModuleCanvasViewer : public QGraphicsView
{
    Q_OBJECT

public:
    using SubmoduleNameFormat = SubmoduleItem::NameFormat;
private:
    cModule *object = nullptr;
    bool notDrawn = false;
    bool needsRedraw = false;
    CanvasRenderer *canvasRenderer;
    QRubberBand *rubberBand;
    QPoint rubberBandStartPos;

    // If this is non-null, there is a submodule captured and moved under the
    // mouse cursor, due to the user previously Shift+clicking on it.
    cModule *draggedSubmod = nullptr;

    // see setLayoutingScene()
    QGraphicsScene *moduleScene; // this is used to show the module usually
    QGraphicsScene *layoutingScene = nullptr; // borrowed from ModuleLayouter during layout visualization, not owned

    CompoundModuleItem *compoundModuleItem = nullptr;
    std::map<cModule*, SubmoduleItem*> submoduleGraphicsItems;
    std::map<cGate*, ConnectionItem*> connectionGraphicsItems;

    // Change flags to remember components with "dirty" DisplayStrings.
    // Used for, and cleared after, selective refresh of network graphics.
    // Beware that the queue sizes should still be updated on every refresh
    // because the queues are indirectly referenced from the DisplayString,
    // and we don't get a notification about changes in their sizes.
    bool compoundModuleChanged = false;
    std::unordered_set<cModule *> changedSubmodules;
    std::unordered_set<cGate *> changedConnections;

    GraphicsLayer *backgroundLayer;
    GraphicsLayer *rangeLayer;
    GraphicsLayer *submoduleLayer;
    GraphicsLayer *networkLayer;
    GraphicsLayer *figureLayer;
    GraphicsLayer *animationLayer;
    GraphicsLayer *bubbleLayer;
    GraphicsLayer *zoomLabelLayer;

    double zoomFactor = 1;
    double imageSizeFactor = 1;
    bool showModuleNames = true;
    bool showArrowHeads = true;
    SubmoduleNameFormat submoduleNameFormat = SubmoduleNameFormat::FMT_FULLNAME_AND_QDISPLAYNAME;

    ZoomLabel *zoomLabel;

    // drawing methods:
    void redrawFigures();
    void refreshFigures();
    void redrawModules();

    void redrawEnclosingModule();
    void drawSubmodule(cModule *submod);
    void drawConnection(cGate *gate);

    FigureRenderingHints makeFigureRenderingHints();

    void updateZoomLabelPos();

    QRectF askExportArea(); // returns a Null rectangle if the dialog was cancelled.
    void renderToPrinter(QPrinter& printer, const QRectF& sceneRect);
    void renderToPaintDevice(QPaintDevice& paintDevice, const QRectF& sceneRect, const QRectF& pageRect);

    // similar logic as in getObjectsAt()
    QString gatherTooltips(const QPoint& pos, int threshold = 4);
    QString gatherTooltips(const QRect& rect);

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
    void exportToImage();
    void exportToPdf();
    void print();

    // This is to support visualizing the layouting process.
    // The scene passed in here will replace the ordinary scene used to display the module.
    // Does not take ownership of the scene. Pass nullptr to return to the regular module graphics.
    // Should only be used when any actions that manipulate the module in any way are disabled (in "layouting mode").
    void setLayoutingScene(QGraphicsScene *layoutingScene);

public:
    ModuleCanvasViewer();
    ~ModuleCanvasViewer();

    void setObject(cModule *obj);
    std::vector<cObject *> getObjectsAt(const QPoint &pos, int threshold = 4);
    std::vector<cObject *> getObjectsAt(const QRect &rect);

    GraphicsLayer *getAnimationLayer() { return animationLayer; }
    CanvasRenderer *getCanvasRenderer() { return canvasRenderer; }

    void refreshLayout();

    void redraw();
    void refresh();
    void recalcSceneRect(bool alignTopLeft = false);

    void setZoomFactor(double zoomFactor);
    void setImageSizeFactor(double imageSizeFactor);
    void setShowModuleNames(bool show);
    void setShowArrowheads(bool show);
    void setSubmoduleNameFormat(SubmoduleNameFormat format);
    SubmoduleNameFormat getSubmoduleNameFormat() const {return submoduleNameFormat;}

    void bubble(cComponent *subcomponent, const char *text);

    QPointF getSubmodCoords(cModule *mod); // gets the coords from the layouter! might return (NAN,NAN) if not yet layouted
    // also accepts the inspected module, returns the whole rectangle for it
    QRectF getSubmodRect(cModule *mod);
    QRectF getModuleRect(bool includeBorder);
    QLineF getConnectionLine(cGate *gate);

    void clear();
    void setNeedsRedraw(bool isNeed = true) { needsRedraw = isNeed; }

    void refreshSubmodule(cModule *submod);
    void refreshSubmodules();
    void refreshQueueSizes(); // the little q: labels on the corner of the submodules

    void refreshConnection(cGate *gate);
    void refreshConnections(cModule *module); // only the ones starting at a gate of module, not the "incoming" ones
    void refreshConnections();

    void setZoomLabelVisible(bool visible);

    void displayStringChanged();
    void displayStringChanged(cModule *submod);
    void displayStringChanged(cGate *gate);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_MODULECANVASVIEWER_H
