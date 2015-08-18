//==========================================================================
//  CANVASINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_CANVASINSPECTOR_H
#define __OMNETPP_QTENV_CANVASINSPECTOR_H

#include "inspector.h"

namespace omnetpp {

class cCanvas;

namespace qtenv {

class CanvasViewer;

class QTENV_API CanvasInspector : public Inspector
{
   Q_OBJECT
   private:
      void zoomBy(double mult);

   protected:
      virtual cCanvas *getCanvas();
      QToolBar *createToolbar();

      CanvasViewer *canvasViewer;

   private slots:
      void relayout();
      void zoomIn();
      void zoomOut();

   public:
      CanvasInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
      ~CanvasInspector();
      virtual void doSetObject(cObject *obj) override;
      virtual void refresh() override;
      virtual void redraw() override;
      virtual void clearObjectChangeFlags() override;
      //TCLKILL virtual int inspectorCommand(int argc, const char **argv) override;
};

} // namespace qtenv
} // namespace omnetpp

#endif
