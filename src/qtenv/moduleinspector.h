//==========================================================================
//  MODULEINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_MODULEINSPECTOR_H
#define __OMNETPP_QTENV_MODULEINSPECTOR_H

#include "animator.h"
#include "inspector.h"

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

class CanvasRenderer;
class ModuleCanvasViewer;
class OsgViewer;


class QTENV_API ModuleInspector : public Inspector
{
   Q_OBJECT

   private slots:
      void runUntil();
      void fastRunUntil();
      void stopSimulation();
      void relayout();
      void zoomIn(int x = 0, int y = 0);
      void zoomOut(int x = 0, int y = 0);
      void increaseIconSize();
      void decreaseIconSize();
      void runPreferencesDialog();

      void click(QMouseEvent *event);
      void doubleClick(QMouseEvent *event);
      void onViewerDragged(QPointF center);

      void onObjectsPicked(const std::vector<cObject*>&);
      void onMarqueeZoom(QRectF rect);

      void createContextMenu(const std::vector<cObject*> &objects, const QPoint &globalPos);

      void layers();
      void showLabels(bool show);
      void showArrowheads(bool show);
      void zoomIconsBy(double mult);
      void resetOsgView();

      void switchToOsgView();
      void switchToCanvasView();

      void onFontChanged();
      void updateToolbarLayout(); // mostly the margins, to prevent occluding the scrollbar

      void exportToPdf();
      void print();

   protected:
      static const QString PREF_MODE; // 0 is 2D, 1 is OSG
      static const QString PREF_CENTER; // of the viewport in the 2D scene
      static const QString PREF_ZOOMFACTOR;
      static const QString PREF_ZOOMBYFACTOR;
      static const QString PREF_ICONSCALE;
      static const QString PREF_SHOWLABELS;
      static const QString PREF_SHOWARROWHEADS;

      const int toolbarSpacing = 4; // from the edges, in pixels, the scrollbar size will be added to this

      QAction *switchToOsgViewAction;
      QAction *switchToCanvasViewAction;

      QAction *canvasRelayoutAction;
      QAction *canvasZoomInAction;
      QAction *canvasZoomOutAction;
      QAction *resetOsgViewAction;
      QAction *showModuleNamesAction;
      QAction *showArrowheadsAction;
      QAction *increaseIconSizeAction;
      QAction *decreaseIconSizeAction;

      QStackedLayout *stackedLayout;
      QGridLayout *toolbarLayout = nullptr; // not used in topLevel mode

      ModuleCanvasViewer *canvasViewer;

#ifdef WITH_OSG
      OsgViewer *osgViewer;
#endif

   protected:
      cCanvas *getCanvas();
      static const char *animModeToStr(SendAnimMode mode);

      void createViews(bool isTopLevel);
      QToolBar *createToolbar(bool isTopLevel);
      void refreshOsgViewer();
#ifdef WITH_OSG
      cOsgCanvas *getOsgCanvas();
      void setOsgCanvas(cOsgCanvas *osgCanvas);
#endif

      QSize sizeHint() const override;
      void wheelEvent(QWheelEvent *event) override;
      void resizeEvent(QResizeEvent *event) override;
      void zoomBy(double mult, bool snaptoone = false, int x = 0, int y = 0);
      void firstObjectSet(cObject *obj) override;

      void renderToPrinter(QPrinter &printer);

   public:
      ModuleInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
      ~ModuleInspector();
      virtual void doSetObject(cObject *obj) override;
      virtual void refresh() override;
      virtual void clearObjectChangeFlags() override;
      void updateBeforeAnimation();

      bool needsRedraw();

      // implementations of inspector commands:
      virtual int getDefaultLayoutSeed();
      virtual int getLayoutSeed(int argc, const char **argv);
      virtual int setLayoutSeed(int argc, const char **argv);
      virtual int getSubmoduleCount(int argc, const char **argv);
      virtual int getSubmodQ(int argc, const char **argv);
      virtual int getSubmodQLen(int argc, const char **argv);

      // drawing methods:
      virtual void redraw() override;

      // notifications from envir:
      virtual void submoduleCreated(cModule *newmodule);
      virtual void submoduleDeleted(cModule *module);
      virtual void connectionCreated(cGate *srcgate);
      virtual void connectionDeleted(cGate *srcgate);
      virtual void displayStringChanged();
      virtual void displayStringChanged(cModule *submodule);
      virtual void displayStringChanged(cGate *gate);
      virtual void bubble(cComponent *subcomponent, const char *text);

      double getZoomFactor();
      double getImageSizeFactor();

      GraphicsLayer *getAnimationLayer();
      QPointF getSubmodCoords(cModule *mod);
      QLineF getConnectionLine(cGate *gate);
};

} // namespace qtenv
} // namespace omnetpp

#endif
