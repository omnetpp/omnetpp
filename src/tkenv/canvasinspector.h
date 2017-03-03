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

#ifndef __OMNETPP_TKENV_CANVASINSPECTOR_H
#define __OMNETPP_TKENV_CANVASINSPECTOR_H

#include "omnetpp/ccanvas.h"
#include "inspector.h"
#include "canvasrenderer.h"

namespace omnetpp {

class cFigure;

namespace tkenv {

class FigureRenderer;
struct FigureRenderingHints;
class CanvasRenderer;

class TKENV_API CanvasInspector : public Inspector
{
   protected:
      char canvas[128];
      CanvasRenderer *canvasRenderer;

   protected:
      virtual cCanvas *getCanvas();
      virtual void fillFigureRenderingHints(FigureRenderingHints *hints);
      virtual void updateBackgroundColor();

   public:
      CanvasInspector(InspectorFactory *f);
      ~CanvasInspector();
      virtual void doSetObject(cObject *obj) override;
      virtual void createWindow(const char *window, const char *geometry) override;
      virtual void useWindow(const char *window) override;
      virtual void refresh() override;
      virtual void redraw() override;
      virtual void clearObjectChangeFlags() override;
      virtual int inspectorCommand(int argc, const char **argv) override;
};

} // namespace tkenv
}  // namespace omnetpp

#endif
