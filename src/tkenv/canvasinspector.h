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

#ifndef __OMNETPP_CANVASINSPECTOR_H
#define __OMNETPP_CANVASINSPECTOR_H

#include "omnetpp/ccanvas.h"
#include "inspector.h"
#include "canvasrenderer.h"

NAMESPACE_BEGIN

class cFigure;
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
      virtual void doSetObject(cObject *obj);
      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *window);
      virtual void refresh();
      virtual void redraw();
      virtual void clearObjectChangeFlags();
      virtual int inspectorCommand(int argc, const char **argv);
};

NAMESPACE_END

#endif
