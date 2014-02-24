//==========================================================================
//  LAYOUTERENV.H - part of
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

#ifndef __LAYOUTERENV_H
#define __LAYOUTERENV_H

#include "timeutil.h"   // must precede <tk.h>, due to collision with <windows.h>
#include "platmisc.h"   // must precede <tk.h> otherwise Visual Studio 2013 fails to compile
#include <tk.h>
#include "graphlayouter.h"

NAMESPACE_BEGIN

class cModule;
class cDisplayString;


class TkenvGraphLayouterEnvironment : public GraphLayouterEnvironment
{
   protected:
      // configuration
      const char *widgetToGrab;
      const char *canvas;
      Tcl_Interp *interp;
      cModule *parentModule;
      const cDisplayString& displayString;

      // state
      struct timeval beginTime;
      struct timeval lastCheck;
      bool grabActive;

   public:
      TkenvGraphLayouterEnvironment(Tcl_Interp *interp, cModule *parentModule, const cDisplayString& displayString);

      void setWidgetToGrab(const char *w) { this->widgetToGrab = w; }
      void setCanvas(const char *canvas) { this->canvas = canvas; }

      void cleanup();

      virtual bool inspected() { return canvas && interp; }
      virtual bool okToProceed();

      virtual bool getBoolParameter(const char *tagName, int index, bool defaultValue);
      virtual long getLongParameter(const char *tagName, int index, long defaultValue);
      virtual double getDoubleParameter(const char *tagName, int index, double defaultValue);

      virtual void clearGraphics();
      virtual void showGraphics(const char *text);
      virtual void drawText(double x, double y, const char *text, const char *tags, const char *color);
      virtual void drawLine(double x1, double y1, double x2, double y2, const char *tags, const char *color);
      virtual void drawRectangle(double x1, double y1, double x2, double y2, const char *tags, const char *color);
};

NAMESPACE_END


#endif
