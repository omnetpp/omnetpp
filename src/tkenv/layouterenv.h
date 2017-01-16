//==========================================================================
//  LAYOUTERENV.H - part of
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

#ifndef __OMNETPP_TKENV_LAYOUTERENV_H
#define __OMNETPP_TKENV_LAYOUTERENV_H

#include "layout/graphlayouter.h"
#include "tkdefs.h"

namespace omnetpp {

class cModule;
class cDisplayString;

namespace tkenv {

class TkenvGraphLayouterEnvironment : public omnetpp::layout::GraphLayouterEnvironment
{
   protected:
      // configuration
      const char *widgetToGrab;
      const char *canvas;
      Tcl_Interp *interp;
      cModule *parentModule;
      const cDisplayString& displayString;

      // state
      int64_t beginTime; // both of them in microseconds, as returned by opp_get_monotonic_clock_usecs()
      int64_t lastCheck;
      bool grabActive;

   public:
      TkenvGraphLayouterEnvironment(Tcl_Interp *interp, cModule *parentModule, const cDisplayString& displayString);

      void setWidgetToGrab(const char *w) { this->widgetToGrab = w; }
      void setCanvas(const char *canvas) { this->canvas = canvas; }

      void cleanup();

      virtual bool inspected() override { return canvas && interp; }
      virtual bool okToProceed() override;

      virtual bool getBoolParameter(const char *tagName, int index, bool defaultValue) override;
      virtual long getLongParameter(const char *tagName, int index, long defaultValue) override;
      virtual double getDoubleParameter(const char *tagName, int index, double defaultValue) override;

      virtual void clearGraphics() override;
      virtual void showGraphics(const char *text) override;
      virtual void drawText(double x, double y, const char *text, const char *tags, const char *color) override;
      virtual void drawLine(double x1, double y1, double x2, double y2, const char *tags, const char *color) override;
      virtual void drawRectangle(double x1, double y1, double x2, double y2, const char *tags, const char *color) override;
};

} // namespace tkenv
}  // namespace omnetpp


#endif
