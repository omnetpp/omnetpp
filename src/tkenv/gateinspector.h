//==========================================================================
//  GATEINSPECTOR.H - part of
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

#ifndef __OMNETPP_TKENV_GATEINSPECTOR_H
#define __OMNETPP_TKENV_GATEINSPECTOR_H

#include "inspector.h"

namespace omnetpp {
namespace tkenv {


class TKENV_API GateInspector : public Inspector
{
   protected:
      char canvas[128];

   protected:
      virtual void doSetObject(cObject *obj) override;

   public:
      GateInspector(InspectorFactory *f);
      virtual void createWindow(const char *window, const char *geometry) override;
      virtual void useWindow(const char *window) override;
      virtual void refresh() override;
      virtual void redraw() override;
      virtual int inspectorCommand(int argc, const char **argv) override;

      // notifications from envir:
      virtual void displayStringChanged(cGate *gate);
};

} // namespace tkenv
}  // namespace omnetpp

#endif
