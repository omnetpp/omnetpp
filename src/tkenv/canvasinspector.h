//==========================================================================
//  CANVASINSPECTOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CANVASINSPECTOR_H
#define __CANVASINSPECTOR_H

#include "platmisc.h"   // must precede <tk.h> otherwise Visual Studio 2013 fails to compile
#include <tk.h>
#include "inspector.h"
#include "ccanvas.h"
#include "canvasrenderer.h"

NAMESPACE_BEGIN

class cFigure;
class FigureRenderer;
class FigureRenderingHints;
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
