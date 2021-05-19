//==========================================================================
//  MODULEINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_MODULEINSPECTOR_H
#define __OMNETPP_QTENV_MODULEINSPECTOR_H

#include <vector>
#include <QtWidgets/QGraphicsScene>
#include "inspector.h"
#include "modulecanvasviewer.h"

class QAction;
class QStackedLayout;
class QMouseEvent;
class QContextMenuEvent;
class QToolBar;
class QPrinter;

namespace omnetpp {

class cObject;
class cModule;
class cGate;
class cCanvas;
class cOsgCanvas;

namespace qtenv {

class IOsgViewer;
class GraphicsLayer;

class QTENV_API ModuleInspector : public Inspector
{
    Q_OBJECT

public:
    using SubmoduleNameFormat = ModuleCanvasViewer::SubmoduleNameFormat;
private slots:
    void runUntil();
    void fastRunUntil();
    void relayout();
    void zoomIn(int x = 0, int y = 0, int n = 1);
    void zoomOut(int x = 0, int y = 0, int n = 1);
    void increaseIconSize() { zoomIconsBy(1.25); }
    void decreaseIconSize() { zoomIconsBy(0.8); }
    void addNameFormatItem(SubmoduleNameFormat format, QString label, QActionGroup *actionGroup, QMenu *subMenu);
    void runPreferencesDialog();

    void click(QMouseEvent *event);
    void doubleClick(QMouseEvent *event);
    void onViewerDragged(QPointF center);

    void onObjectsPicked(const std::vector<cObject*>&);
    void onMarqueeZoom(QRectF rect);

    void createContextMenu(const std::vector<cObject*> &objects, const QPoint &globalPos);

    void showCanvasLayersDialog();
    void showMethodCalls(bool show);
    void showLabels(bool show);
    void showArrowheads(bool show);
    void setSubmoduleNameFormat(SubmoduleNameFormat format);
    void zoomIconsBy(double mult);

    void switchToOsgView();
    void switchToCanvasView();

    void onFontChanged();
    void updateToolbarLayout(); // mostly the margins, to prevent occluding the scrollbar

protected:
    static const QString PREF_MODE; // 0 is 2D, 1 is OSG
    static const QString PREF_CENTER; // of the viewport in the 2D scene
    static const QString PREF_ZOOMFACTOR;
    static const QString PREF_ZOOMBYFACTOR;
    static const QString PREF_ICONSCALE;
    static const QString PREF_SHOWMETHODCALLS;
    static const QString PREF_SHOWLABELS;
    static const QString PREF_SHOWARROWHEADS;
    static const QString PREF_SUBMODULENAMEFORMAT;

    const int toolbarSpacing = 4; // from the edges, in pixels, the scrollbar size will be added to this

    QAction *switchToOsgViewAction;
    QAction *switchToCanvasViewAction;

    QAction *canvasRelayoutAction;
    QAction *canvasZoomInAction;
    QAction *canvasZoomOutAction;
    QAction *canvasLayersAction;
    QAction *resetOsgViewAction;
    QAction *showMethodCallsAction;
    QAction *showModuleNamesAction;
    QAction *showArrowheadsAction;
    QAction *submoduleNameFormatAction;
    QAction *increaseIconSizeAction;
    QAction *decreaseIconSizeAction;

    QStackedLayout *stackedLayout;
    QGridLayout *toolbarLayout = nullptr; // not used in topLevel mode
    QToolBar *toolbar;

    ModuleCanvasViewer *canvasViewer;
    IOsgViewer *osgViewer = nullptr;

public slots:
    void onLayoutVisualizationStarts(cModule *module, QGraphicsScene *layoutingScene);
    void onLayoutVisualizationEnds(cModule *module);
    void onModuleLayoutChanged(cModule *module);

protected:
    cCanvas *getCanvas();

    void createViews(bool isTopLevel);
    QToolBar *createToolbar(bool isTopLevel);
    void refreshOsgViewer();

    cOsgCanvas *getOsgCanvas();
    void setOsgCanvas(cOsgCanvas *osgCanvas);

    QSize sizeHint() const override { return QSize(600, 500); }
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void zoomBy(double mult, bool snaptoone = false, int x = 0, int y = 0);

    void renderToPrinter(QPrinter &printer);

    std::map<cComponent *, std::string> bubblesToShow;

    virtual void redraw();

public:
    ModuleInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
    ~ModuleInspector();

    virtual void doSetObject(cObject *obj) override;
    virtual void refresh() override;
    virtual void postRefresh() override;

    QImage getScreenshot();

    // notifications from envir:
    virtual void submoduleCreated(cModule *newmodule);
    virtual void submoduleDeleted(cModule *module);
    virtual void connectionCreated(cGate *srcgate);
    virtual void connectionDeleted(cGate *srcgate);
    virtual void displayStringChanged();
    virtual void displayStringChanged(cModule *submodule);
    virtual void displayStringChanged(cGate *gate);
    virtual void bubble(cComponent *subcomponent, const char *text) { bubblesToShow[subcomponent] = text; }

    double getZoomFactor();
    double getImageSizeFactor();

    GraphicsLayer *getAnimationLayer();
    QPointF getSubmodCoords(cModule *mod);
    QRectF getSubmodRect(cModule *mod);
    QLineF getConnectionLine(cGate *gate);

    bool getShowMethodCalls() const { return getPref(PREF_SHOWMETHODCALLS, true, true).toBool(); }

    QRect getModuleRect(bool includeBorder, int margin);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif
