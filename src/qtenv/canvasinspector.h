//==========================================================================
//  CANVASINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_CANVASINSPECTOR_H
#define __OMNETPP_QTENV_CANVASINSPECTOR_H

#include "inspector.h"

namespace omnetpp {

class cCanvas;

namespace qtenv {

class CanvasViewer;
class ZoomLabel;

class QTENV_API CanvasInspector : public Inspector
{
   Q_OBJECT
   private:
      void zoomBy(double mult);

   protected:
      virtual cCanvas *getCanvas();
      QToolBar *createToolbar();

      CanvasViewer *canvasViewer = nullptr;

   private Q_SLOTS:
      void zoomIn();
      void zoomOut();
      void onClick(QMouseEvent *event);
      void onContextMenuRequested(QContextMenuEvent *event);

   public Q_SLOTS:
      virtual void redraw();

   public:
      CanvasInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
      ~CanvasInspector() {}
      void doSetObject(cObject *obj) override;
      void refresh() override;
      void postRefresh() override;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif
