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

class QBoxLayout;
class QMouseEvent;
class QContextMenuEvent;

namespace omnetpp {
class cObject;
class cModule;
class cGate;
class cCanvas;

namespace qtenv {

class CanvasRenderer;
class ModuleGraphicsView;


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

      void click(QMouseEvent *event);
      void doubleClick(QMouseEvent *event);
      void createContextMenu(QContextMenuEvent *event);

      void layers();
      void toggleLabels();
      void toggleArrowheads();
      void zoomIconsBy();

   protected:
      // These layers should be the only top level items!
      // After adding anything else to the scene, make
      // sure to make it a descendant of one of these!
      GraphicsLayer *backgroundLayer;
      GraphicsLayer *rangeLayer;
      GraphicsLayer *networkLayer;
      GraphicsLayer *figureLayer;
      GraphicsLayer *animationLayer;

      CanvasRenderer *canvasRenderer;
      ModuleGraphicsView *view; // holds the scene

   protected:
      //TODO Where is getCanvas() right place? Here or in ModuleGraphicsView.
      cCanvas *getCanvas();
      static const char *animModeToStr(SendAnimMode mode);

      void addToolBar(QBoxLayout *layout);
      void createView(QWidget *parent);

      void zoomBy(double mult, bool snaptoone = false, int x = 0, int y = 0);

   public:
      ModuleInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
      ~ModuleInspector();
      virtual void doSetObject(cObject *obj) override;
      virtual void refresh() override;
      virtual void clearObjectChangeFlags() override;
      virtual int inspectorCommand(int argc, const char **argv) override;

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

      GraphicsLayer *getAnimationLayer() { return animationLayer; }
      QPointF getSubmodCoords(cModule *mod);
      QPointF getMessageEndPos(const QPointF &src, const QPointF &dest);
};

} // namespace qtenv
} // namespace omnetpp

#endif
