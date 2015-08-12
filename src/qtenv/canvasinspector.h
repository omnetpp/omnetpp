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

#include "omnetpp/ccanvas.h"
#include "inspector.h"
#include "canvasrenderer.h"

namespace omnetpp {

class cFigure;

namespace qtenv {

class FigureRenderer;
struct FigureRenderingHints;
class CanvasRenderer;

class QTENV_API CanvasInspector : public Inspector
{
   protected:
      char canvas[128];
      CanvasRenderer *canvasRenderer;

   protected:
      virtual cCanvas *getCanvas();
      virtual void fillFigureRenderingHints(FigureRenderingHints *hints);
      virtual void updateBackgroundColor();

      QGraphicsView *graphicsView;

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
